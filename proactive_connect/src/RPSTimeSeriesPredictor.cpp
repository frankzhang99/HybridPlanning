//
 // Copyright (c) 2015 Carnegie Mellon University. All Rights Reserved.

 // Redistribution and use in source and binary forms, with or without
 // modification, are permitted provided that the following conditions
 // are met:

 // 1. Redistributions of source code must retain the above copyright
 // notice, this list of conditions and the following acknowledgments
 // and disclaimers.

 // 2. Redistributions in binary form must reproduce the above
 // copyright notice, this list of conditions and the following
 // disclaimer in the documentation and/or other materials provided
 // with the distribution.

 // 3. The names "Carnegie Mellon University," "SEI" and/or "Software
 // Engineering Institute" shall not be used to endorse or promote
 // products derived from this software without prior written
 // permission. For written permission, please contact
 // permission@sei.cmu.edu.

 // 4. Products derived from this software may not be called "SEI" nor
 // may "SEI" appear in their names without prior written permission of
 // permission@sei.cmu.edu.

 // 5. Redistributions of any form whatsoever must retain the following
 // acknowledgment:

 // This material is based upon work funded and supported by the
 // Department of Defense under Contract No. FA8721-05-C-0003 with
 // Carnegie Mellon University for the operation of the Software
 // Engineering Institute, a federally funded research and development
 // center.

 // Any opinions, findings and conclusions or recommendations expressed
 // in this material are those of the author(s) and do not necessarily
 // reflect the views of the United States Department of Defense.

 // NO WARRANTY. THIS CARNEGIE MELLON UNIVERSITY AND SOFTWARE
 // ENGINEERING INSTITUTE MATERIAL IS FURNISHED ON AN "AS-IS"
 // BASIS. CARNEGIE MELLON UNIVERSITY MAKES NO WARRANTIES OF ANY KIND,
 // EITHER EXPRESSED OR IMPLIED, AS TO ANY MATTER INCLUDING, BUT NOT
 // LIMITED TO, WARRANTY OF FITNESS FOR PURPOSE OR MERCHANTABILITY,
 // EXCLUSIVITY, OR RESULTS OBTAINED FROM USE OF THE MATERIAL. CARNEGIE
 // MELLON UNIVERSITY DOES NOT MAKE ANY WARRANTY OF ANY KIND WITH
 // RESPECT TO FREEDOM FROM PATENT, TRADEMARK, OR COPYRIGHT
 // INFRINGEMENT.

 // This material has been approved for public release and unlimited
 // distribution.

 // DM-0002494
 //
/*
 * RPSTimeSeriesPredictor.cpp
 *
 *  Created on: Jun 25, 2014
 *      Author: gmoreno
 */

/***************************************
 * IMPORTANT: this works with an AR model or REFIT AR
 * other predictors may need a new Clone() method that
 * copies the variance cache
 ***************************************/

#include "RPSTimeSeriesPredictor.h"
#include <memory>
#include <math.h>
#include <float.h>
#include "fit.h"
#include <iostream>


using namespace std;


const double ALPHA = 0.2;


RPSTimeSeriesPredictor::RPSTimeSeriesPredictor(const std::vector<std::string>& model, unsigned trainingLength,
        unsigned horizon)
    : TimeSeriesPredictor(trainingLength, horizon),
      modelTemplate(0), model(0), predictor(0), observationCount(0), observations(25),
      exponentialSmoothingPrediction(0), refitInterval(25), countSinceLastRefit(0)
{
	unsigned base = 0;

	if (base < model.size() && model[base] == "REFIT") {
	    base++; // skip REFIT
	    refitInterval = atoi(model[base++].c_str());
	} else {
	    refitInterval = 0;
	}
	cerr << "RPSTimeSeriesPredictor setting model trainLenght=" << trainLength
	        << " refitInterval=" << refitInterval;

	int n = model.size() - base;
	const char* argv[n];
	for (int i = 0; i < n; i++) {
		argv[i] = model[base + i].c_str();
		cerr << ' ' << argv[i];
	}
	cerr << endl;

	modelTemplate = ParseModel(n, (char**) argv);
	observations.set_capacity(trainLength);
}

RPSTimeSeriesPredictor::RPSTimeSeriesPredictor(const RPSTimeSeriesPredictor& p)
    : TimeSeriesPredictor(p),
      modelTemplate(new ModelTemplate(*(p.modelTemplate))),
      model(p.model->Clone()),
      predictor(p.predictor->Clone()),
      observationCount(p.observationCount),
      observations(p.observations),
      exponentialSmoothingPrediction(p.exponentialSmoothingPrediction),
      refitInterval(p.refitInterval),
      countSinceLastRefit(p.countSinceLastRefit)
{
}

TimeSeriesPredictor* RPSTimeSeriesPredictor::clone() const {
    RPSTimeSeriesPredictor* pClone = new RPSTimeSeriesPredictor(*this);

    /*
     * To get the same behavior as the first version uncomment the following line
     * because that version was not refitting when building the prob tree
     * It can be either way, but with refitting it seems to perform slightly better
     */
    //pClone->refitInterval = 0; // disable refitting
    return pClone;
}


double simplePrediction(double lastPrediction, double newObservation) {
	return ALPHA * newObservation + (1 - ALPHA) * lastPrediction;
}

void RPSTimeSeriesPredictor::observe(double v) {
	TimeSeriesPredictor::observe(v);
    observations.push_back(v);
    observationCount++;
    countSinceLastRefit++;
    if (observations.full() && (!model || (refitInterval > 0 && countSinceLastRefit >= refitInterval))) {
        //cerr << "Refitting model" << endl;
        try {
            double observationsArray[observations.size()];
            //cerr << "obs size " << observations.size() << endl;
            for (unsigned i = 0; i < observations.size(); i++) {
                observationsArray[i] = observations[i];
            }
            Model* newModel = FitThis(observationsArray, observations.size(), *modelTemplate);
            if (model) {
                delete model;
            }
            model = newModel;
            if (predictor) {
                delete predictor;
            }
            predictor = model->MakePredictor();

            // prime predictor : NOT NEEDED, MakePredictor does it (at least with REFIT)
            for (unsigned i = 0; i < observations.size(); i++) {
                predictor->Step(observationsArray[i]);
            }
            countSinceLastRefit = 0;
            //cerr << "Refitted at step " << observationCount << endl;
        }
        catch(...) {
        }
    }
    if (!model) {

	    // haven't fit the first model yet

		if (observations.size() == 1) {
			exponentialSmoothingPrediction = v;
		}
		exponentialSmoothingPrediction = simplePrediction(exponentialSmoothingPrediction, v);
	} else {
		predictor->Step(v);
	}
}

void RPSTimeSeriesPredictor::predict(unsigned n, double* predictions, double* variances) const {
	if (!predictor) {

		/*
		 * we haven't fit the model yet, so we use a simple predictor
		 */
		double dummyPrediction = exponentialSmoothingPrediction;
		for (unsigned i = 0; i < n; i++) {
			predictions[i] = dummyPrediction;
			if (variances) {
				variances[i] = 0;
			}
		}
	} else {
		predictor->Predict(n, predictions);
		if (variances) {
			predictor->ComputeVariances(n, variances);
		}
	}
}


RPSTimeSeriesPredictor::~RPSTimeSeriesPredictor() {
	if (predictor) {
		delete predictor;
	}
	if (model) {
		delete model;
	}
	if (modelTemplate) {
		delete modelTemplate;
	}
}

