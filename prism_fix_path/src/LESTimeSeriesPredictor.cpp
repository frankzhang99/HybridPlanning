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
 * LESTimeSeriesPredictor.cpp
 *
 *  Created on: May 19, 2015
 *      Author: gmoreno
 */

#include "LESTimeSeriesPredictor.h"
#include <exception>
#include <sstream>
#include <math.h>
#include <iostream>

using namespace std;

LESTimeSeriesPredictor::LESTimeSeriesPredictor(const std::vector<std::string>& model,
        unsigned trainingLength, unsigned horizon)
    : TimeSeriesPredictor(trainingLength, horizon),
      horizon(horizon), forecastBuffer(horizon), observationCount(0),
      squaredErrorSums(new double[horizon]), squaredErrorCount(0), lt(0), bt(0)
{
    if (model.size() == 3 || model.size() == 4) {
        alpha = atof(model[1].c_str());
        beta = atof(model[2].c_str());
        if (model.size() == 4) {
            phi = atof(model[3].c_str());
        } else {
            phi = 1.0;
        }
    } else {
        stringstream ss;
        ss << "LESTimeSeriesPredictor(): model not valid:";
        for (unsigned i = 0; i < model.size(); i++) {
            ss << ' ';
            ss << model[i];
        }
        throw runtime_error(ss.str());
    }
    cout << "LESTimeSeriesPredictor trainLength=" << trainLength
            << " alpha=" << alpha
            << " beta=" << beta
            << " phi=" << phi << endl;
}

/*LESTimeSeriesPredictor::LESTimeSeriesPredictor(const std::vector<std::string>& model,
        unsigned trainingLength, unsigned horizon)
    : TimeSeriesPredictor(trainingLength, horizon),
      horizon(horizon), forecastBuffer(horizon), observationCount(0),
      squaredErrorSums(new double[horizon]), squaredErrorCount(0)
{
    if (model.size() == 3) {
        alpha = atof(model[1].c_str());
        beta = atof(model[2].c_str());
    } else {
        stringstream ss;
        ss << "LESTimeSeriesPredictor(): model not valid:";
        for (unsigned i = 0; i < model.size(); i++) {
            ss << ' ';
            ss << model[i];
        }
        throw runtime_error(ss.str());
    }
    cout << "LESTimeSeriesPredictor trainLength=" << trainLength
            << " alpha=" << alpha
            << " beta=" << beta << endl;
}*/

LESTimeSeriesPredictor::LESTimeSeriesPredictor(const LESTimeSeriesPredictor& p)
    : TimeSeriesPredictor(p.trainLength, p.horizon),
      alpha(p.alpha),
      beta(p.beta),
      phi(p.phi),
      horizon(p.horizon),
      forecastBuffer(p.forecastBuffer),
      observationCount(p.observationCount),
      squaredErrorSums(new double[horizon]),
      squaredErrorCount(p.squaredErrorCount),
      lt(p.lt),
      bt(p.bt)
{
    for (unsigned i = 0; i < horizon; i++) {
        squaredErrorSums[i] = p.squaredErrorSums[i];
    }
}

/*LESTimeSeriesPredictor::LESTimeSeriesPredictor(const LESTimeSeriesPredictor& p)
    : TimeSeriesPredictor(p.trainLength, p.horizon),
      alpha(p.alpha),
      beta(p.beta),
      horizon(p.horizon),
      forecastBuffer(p.forecastBuffer),
      observationCount(p.observationCount),
      squaredErrorSums(new double[horizon]),
      squaredErrorCount(p.squaredErrorCount),
      lt(p.lt),
      bt(p.bt)
{
    for (unsigned i = 0; i < horizon; i++) {
        squaredErrorSums[i] = p.squaredErrorSums[i];
    }
}*/

TimeSeriesPredictor* LESTimeSeriesPredictor::clone() const {
    return new LESTimeSeriesPredictor(*this);
}

void LESTimeSeriesPredictor::observe(double yt) {
    TimeSeriesPredictor::observe(yt);
    // update
    observationCount++;
    if (observationCount == 1) {
        lt = yt;
        bt = 0;
    } else if (observationCount == 2) {
        lt = (yt + lt) / 2.0;
        bt = yt - lt;
    } else {
        double prevLt = lt;
        double prevBt = bt;
        lt = alpha * yt + (1 - alpha) * (prevLt + phi * prevBt);
        bt = beta * (lt - prevLt) + (1 - beta) * phi * prevBt;
    }

    // compute error
    //        se <- (yt - diag(prevState$forecast)) ^ 2
    //        se[is.na(se)] <- 0
    //        state$sse <- prevState$sse + se
    for (unsigned i = 0; i < horizon; i++) {
        if (i < forecastBuffer.size()) {
            squaredErrorSums[i] += pow(yt - forecastBuffer[i][i], 2);
        }
    }

    // state$nse <- prevState$nse + 1
    if (forecastBuffer.size() > 0) { // inc the count only if we added one in the prev loop
        squaredErrorCount++;
    }

    // forecast
    Forecast forecast;
    double cumPhi = 0;
    for (unsigned i = 1; i <= horizon; i++) {
        cumPhi += pow(phi, i);
        forecast.push_back(lt + bt * cumPhi);
    }

    // shift forecast matrix (remove last row) and add fc as first row
    // state$forecastbuffer <- rbind(fc, prevState$forecastbuffer[-h,])
    forecastBuffer.push_front(forecast);
//    cout << "FC matrix" <<  endl;
//    for (unsigned i = 0; i < forecastBuffer.size(); i++) {
//        for (unsigned j = 0; j < forecastBuffer[i].size(); j++) {
//            cout << ' ' << forecastBuffer[i][j];
//        }
//        cout << endl;
//    }
}

/*void LESTimeSeriesPredictor::observe(double yt) {
    TimeSeriesPredictor::observe(yt);
    // update
    observationCount++;
    if (observationCount == 1) {
        lt = yt;
        bt = 0;
    } else if (observationCount == 2) {
        lt = (yt + lt) / 2.0;
        bt = yt - lt;
    } else {
        double prevLt = lt;
        double prevBt = bt;
        lt = alpha * yt + (1 - alpha) * (prevLt + prevBt);
        bt = beta * (lt - prevLt) + (1 - beta) * prevBt;
    }

    // compute error
    //        se <- (yt - diag(prevState$forecast)) ^ 2
    //        se[is.na(se)] <- 0
    //        state$sse <- prevState$sse + se
    for (unsigned i = 0; i < horizon; i++) {
        if (i < forecastBuffer.size()) {
            squaredErrorSums[i] += pow(yt - forecastBuffer[i][i], 2);
        }
    }

    // state$nse <- prevState$nse + 1
    if (forecastBuffer.size() > 0) { // inc the count only if we added one in the prev loop
        squaredErrorCount++;
    }

    // forecast
    Forecast forecast;
    for (unsigned i = 1; i <= horizon; i++) {
        //fc <- state$lt + state$bt * (1:h)
        forecast.push_back(lt + bt * i);
    }

    // shift forecast matrix (remove last row) and add fc as first row
    // state$forecastbuffer <- rbind(fc, prevState$forecastbuffer[-h,])
    forecastBuffer.push_front(forecast);
//    cout << "FC matrix" <<  endl;
//    for (unsigned i = 0; i < forecastBuffer.size(); i++) {
//        for (unsigned j = 0; j < forecastBuffer[i].size(); j++) {
//            cout << ' ' << forecastBuffer[i][j];
//        }
//        cout << endl;
//    }
}*/

void LESTimeSeriesPredictor::predict(unsigned n, double* predictions,
        double* variances) const {
    if (n > horizon) {
        throw runtime_error("LESTimeSeriesPredictor::predict(): request for more predictions than horizon length");
    }

    for (unsigned i = 0; i < n; i++) {
        predictions[i] = forecastBuffer[0][i];
        if (variances) {
            //        rmse <- sqrt(state$sse / (state$nse - 0:(h-1)))
            // the stddev of the error is the squared root of the mean squared error, so....
            int errCount = squaredErrorCount - i;
            variances[i] = (errCount > 0) ? squaredErrorSums[i] / errCount : 0.0;
        }
    }
}

LESTimeSeriesPredictor::~LESTimeSeriesPredictor() {
    delete[] squaredErrorSums;
}

