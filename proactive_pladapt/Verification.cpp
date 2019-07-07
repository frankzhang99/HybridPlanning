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
 * Verification.cpp
 *
 *  Created on: Feb 16, 2016
 *      Author: ashutosp
 */

using namespace std;

#include "Verification.h"
#include "GenerateSpecification.h"
#include "AdaptationPlanner.h"

#include "assert.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <cstdlib>
#include <fcntl.h>
#include <errno.h>
#include <stdexcept>
#include <string.h>


Verification::Verification(const std::string& initialState,
        const std::string& envModel,
        const std::string& path) :
                mInitialState(initialState),
                mEnvModel(envModel),
                mPath(path) {
    // TODO Auto-generated constructor stub
}

Verification::~Verification() {
    // TODO Auto-generated destructor stub
}

bool Verification::CreateVerificationDirectory() {
    bool result = false;
    struct stat st = { 0 };
    const std::string dir = GetVerificationDirectory();

    if (stat(dir.c_str(), &st) == -1) {
        mkdir(dir.c_str(), 0700);
        result = true;
    }

    return result;
}

inline string Verification::GetPrismPath() {
    return "/home/ashutosp/Dropbox/Prism/mdp/linux/prism/bin/prism";
}

inline string Verification::GetVerificationProperty() {
    return "Rmax=? [ F \"final\" ]";
}

inline string Verification::GetVerificationDirectory() {
    return mPath + "/verify/";
}

inline string Verification::GetSpecificationFile() {
    // First create directory for verification process
    return "verify.prism";
}

inline string Verification::GetAdversaryFile() {
    return "verify.mdp";
}

inline string Verification::GetLabelsFile() {
    return "verify.lab";
}

inline string Verification::GetStatesFile() {
    return "verify.sta";
}


bool Verification::ExecuteActions(const Actions& actions) {
    bool result = false;

    CreateVerificationDirectory();
    GenerateSpecificationFile(actions);

    // Now run verification process in the verification directory.
    int currentDir = open(".", O_RDONLY);

    if (currentDir == -1) {
        throw runtime_error("error Verification::plan chdir");
    }

    if (chdir(GetVerificationDirectory().c_str()) != 0) {
        // TODO improve error handling
        close(currentDir);
        throw runtime_error("error Verification::plan chdir");
    }

    Verify();

    // Now extract the adversary actions from the output
    Actions decision = ExtractActions();

    fchdir(currentDir);
    close(currentDir);

    for (Actions::iterator it = decision.begin(); it != decision.end(); ++it) {
        printf("%s\n", it->c_str());
    }

    //if (action == "op") {
    //    result = true;
    //}

    return result;
}

void Verification::GenerateSpecificationFile(const Actions& actions) {
    vector<string>::const_iterator it = actions.begin();
    GenerateSpecification::ServerTactic* serverTactic = NULL;
    GenerateSpecification::DimmerTactic* dimmerTactic = NULL;
    GenerateSpecification::DivertTactic* divertTactic = NULL;

    for (; it != actions.end(); it++) {
        //int discretizedBrownoutFactor = hpModel->getDimmerLevel();
        printf("%s\n", it->c_str());
        //dump_str(it->c_str());
        if (it->compare("addServerA_start") == 0) {
            serverTactic = new GenerateSpecification::ServerTactic(true, GenerateSpecification::A);
        } else if (it->compare("addServerB_start") == 0) {
            serverTactic = new GenerateSpecification::ServerTactic(true, GenerateSpecification::B);
        } else if (it->compare("addServerC_start") == 0) {
            serverTactic = new GenerateSpecification::ServerTactic(true, GenerateSpecification::C);
        } else if (it->compare("removeServerA_start") == 0) {
            serverTactic = new GenerateSpecification::ServerTactic(false, GenerateSpecification::A);
        } else if (it->compare("removeServerB_start") == 0) {
            serverTactic = new GenerateSpecification::ServerTactic(false, GenerateSpecification::B);
        } else if (it->compare("removeServerC_start") == 0) {
            serverTactic = new GenerateSpecification::ServerTactic(false, GenerateSpecification::C);
        } else if (it->compare(0, 3, "dec") == 0) {
            // TODO Ideally increase and decrease dimmer should be reverted
            dimmerTactic = new GenerateSpecification::DimmerTactic(false);
        } else if (it->compare(0, 3, "inc") == 0) {
            dimmerTactic = new GenerateSpecification::DimmerTactic(true);
        } else if (it->compare("divert_100_0_0") == 0) {
            divertTactic = new GenerateSpecification::DivertTactic(GenerateSpecification::HUNDRED,
                    GenerateSpecification::ZERO, GenerateSpecification::ZERO);
        } else if (it->compare("divert_75_25_0") == 0) {
            divertTactic = new GenerateSpecification::DivertTactic(GenerateSpecification::SEVENTYFIVE,
                    GenerateSpecification::TWENTYFIVE, GenerateSpecification::ZERO);
        } else if (it->compare("divert_75_0_25") == 0) {
            divertTactic = new GenerateSpecification::DivertTactic(GenerateSpecification::SEVENTYFIVE,
                    GenerateSpecification::ZERO, GenerateSpecification::TWENTYFIVE);
        } else if (it->compare("divert_50_50_0") == 0) {
            divertTactic = new GenerateSpecification::DivertTactic(GenerateSpecification::FIFTY,
                    GenerateSpecification::FIFTY, GenerateSpecification::ZERO);
        } else if (it->compare("divert_50_0_50") == 0) {
            divertTactic = new GenerateSpecification::DivertTactic(GenerateSpecification::FIFTY,
                    GenerateSpecification::ZERO, GenerateSpecification::FIFTY);
        } else if (it->compare("divert_50_25_25") == 0) {
            divertTactic = new GenerateSpecification::DivertTactic(GenerateSpecification::FIFTY,
                    GenerateSpecification::TWENTYFIVE, GenerateSpecification::TWENTYFIVE);
        } else if (it->compare("divert_25_75_0") == 0) {
            divertTactic = new GenerateSpecification::DivertTactic(GenerateSpecification::TWENTYFIVE,
                    GenerateSpecification::SEVENTYFIVE, GenerateSpecification::ZERO);
        } else if (it->compare("divert_25_0_75") == 0) {
            divertTactic = new GenerateSpecification::DivertTactic(GenerateSpecification::TWENTYFIVE,
                    GenerateSpecification::ZERO, GenerateSpecification::SEVENTYFIVE);
        } else if (it->compare("divert_25_50_25") == 0) {
            divertTactic = new GenerateSpecification::DivertTactic(GenerateSpecification::TWENTYFIVE,
                    GenerateSpecification::FIFTY, GenerateSpecification::TWENTYFIVE);
        } else if (it->compare("divert_25_25_50") == 0) {
            divertTactic = new GenerateSpecification::DivertTactic(GenerateSpecification::TWENTYFIVE,
                    GenerateSpecification::TWENTYFIVE, GenerateSpecification::FIFTY);
        } else if (it->compare("divert_0_100_0") == 0) {
            divertTactic = new GenerateSpecification::DivertTactic(GenerateSpecification::ZERO,
                    GenerateSpecification::HUNDRED, GenerateSpecification::ZERO);
        } else if (it->compare("divert_0_0_100") == 0) {
            divertTactic = new GenerateSpecification::DivertTactic(GenerateSpecification::ZERO,
                    GenerateSpecification::ZERO, GenerateSpecification::HUNDRED);
        } else if (it->compare("divert_0_75_25") == 0) {
            divertTactic = new GenerateSpecification::DivertTactic(GenerateSpecification::ZERO,
                    GenerateSpecification::SEVENTYFIVE, GenerateSpecification::TWENTYFIVE);
        } else if (it->compare("divert_0_25_75") == 0) {
            divertTactic = new GenerateSpecification::DivertTactic(GenerateSpecification::ZERO,
                    GenerateSpecification::TWENTYFIVE, GenerateSpecification::SEVENTYFIVE);
        } else if (it->compare("divert_0_50_50") == 0) {
            divertTactic = new GenerateSpecification::DivertTactic(GenerateSpecification::ZERO,
                    GenerateSpecification::FIFTY, GenerateSpecification::FIFTY);
        } else {
            assert(false); // unknown tactic
        }
    }

    const string file = GetVerificationDirectory() + GetSpecificationFile();

    GenerateSpecification generateSpec = GenerateSpecification(file, serverTactic, dimmerTactic, divertTactic);
    generateSpec.Generate(mInitialState, mEnvModel);

    if (serverTactic != NULL) {
        delete serverTactic;
    }

    if (dimmerTactic != NULL) {
        delete dimmerTactic;
    }

    if (divertTactic != NULL) {
        delete divertTactic;
    }
}

bool Verification::Verify() {
    // TODO need to throw exceptions to provide better error handling
    //checkPwd();
    //static int i = 0;
    //printf("i=%d\n", ++i);
    //char command[4096];

    //sprintf( command, "%s %s -pctl %s -exportadv %s -exportstates %s -exportlabels %s %s %s",
    //        PRISM, modelPath, PRISM_PROPERTY, adversaryPath, statesPath, labelsPath, CUDD, ENGINE);
    //printf("%s\n", command);
    //int res = system(command);

    //assert(res!=-1);
    //return true;*/
    //char command[40960];
    //sprintf( command, "%s %s -pctl %s -exportadv %s -exportstates %s -exportlabels %s",
     //           PRISM, modelPath, PRISM_PROPERTY, adversaryPath, statesPath, labelsPath);
    //printf("%s\n", command);

    pid_t pid = fork();
    if (pid == 0) {
        // child
        // TODO perhaps we need to pipe the output to get errorr descriptions
        // see this url for example: http://www.cs.uleth.ca/~holzmann/C/system/pipeforkexec.htmls
        int status = 0;

        status = execlp(GetPrismPath().c_str(), GetPrismPath().c_str(), GetSpecificationFile().c_str(),
                "-pctl", GetVerificationProperty().c_str(), "-exportadv", GetAdversaryFile().c_str(),
                "-exportstates", GetStatesFile().c_str(), "-exportlabels", GetLabelsFile().c_str(), (char*) 0);

        if (status == -1) { // the only option really, otherwise execlp doesn't return
            cout << "Verification::runPrism() error: " << strerror(errno) << endl;
            return false; // error
        }
    } else if (pid == -1) {
        // error
        return false;
    } else {
        // parent
        int status;
        int rval = waitpid(pid, &status, 0);
        if (rval > 0 && WIFEXITED(status) && WEXITSTATUS(status) == 0) {
            return true;
        }
    }

    return false;
}

Verification::Actions Verification::ExtractActions() {
    set<int> states = getNowStates(GetStatesFile().c_str());
    //printf("State Size %u\n", states.size());
    Actions actions = getActions(GetAdversaryFile().c_str(), GetLabelsFile().c_str(), states);

    return actions;
}
