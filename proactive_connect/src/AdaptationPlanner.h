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
 * AdaptationPlanner.h
 *
 *  Created on: Jun 25, 2014
 *      Author: gmoreno
 */

#ifndef ADAPTATIONPLANNER_H_
#define ADAPTATIONPLANNER_H_

#include <string>
#include <vector>
#include <set>
using namespace std;

extern std::vector<string> getActions(const char* adversaryPath, const char* labelsPath,
        std::set<int>& states);
extern std::set<int> getNowStates(const char* statesPath);

class AdaptationPlanner {
protected:
    std::string modelTemplatePath;

    bool generateModel(std::string environmentModel, std::string initialState, const char* modelPath, bool returnPlan);

public:
	AdaptationPlanner();

	/**
	 * Returns a vector of tactics that must be started now
	 * TODO the args should be a probability tree and a key-value map for the state
	 *
	 * @param path if not null, the temp directory path (relative) is stored there, and the
	 *   directory is not deleted
	 */
	std::vector<std::string> plan(std::string environmentModel, std::string initialState, std::string* path = 0, bool returnPlan = true);

	void setModelTemplatePath(const std::string& modelTemplatePath);

	virtual ~AdaptationPlanner();
	void test();
};

#endif /* ADAPTATIONPLANNER_H_ */
