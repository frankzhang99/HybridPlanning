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
 * AdaptationPlanner.cpp
 *
 *  Created on: Jun 25, 2014
 *      Author: gmoreno
 */

#include "AdaptationPlanner.h"

#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fstream>
#include <string>
#include <boost/tokenizer.hpp>
#include <cstdlib>
#include <set>
#include <string.h>
#include <map>
#include <sstream>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

using namespace std;

const char* PRISM = "/home/frank/Prism/prism/bin/prism";
//const char* PRISM_PROPERTY = "\"Rmax=? [ F \\\"final\\\" ]\"";
const char* PRISM_PROPERTY = "Rmax=? [ F \"final\" ]";
const char* PRISM_PROPERTY_RT = "Rmax=? [ F end ]";
const char* PRISM_PROPERTY_PROB = "Pmax=?[true U end]";
const char* TACTIC_SUFFIX = "_start";
const char* DIVERT = "divert_";
const char* ENGINE = "-s";
const char* CUDD = "-cuddmaxmem";
const char* CUDDMEM = "4g";

AdaptationPlanner::AdaptationPlanner() {
}

void AdaptationPlanner::setModelTemplatePath(const std::string& modelTemplatePath) {
    // unless it's an absolute path, prefix it with .. because will be in the temp dir when this is used
    if (modelTemplatePath[0] == '/') {
        this->modelTemplatePath = modelTemplatePath;
    } else {
        this->modelTemplatePath = string("../") + modelTemplatePath;
    }
}

void checkPwd() {
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) != NULL)
       fprintf(stdout, "Current working dir: %s\n", cwd);
    else
       perror("getcwd() error");
}


void get_absolute_path(std::string& path) {
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        fprintf(stdout, "Current working dir: %s\n", cwd);
    } else {
        perror("getcwd() error");
    }

    path = string(cwd) + "/" + path;
}

bool runPrism(const char* modelPath, const char* adversaryPath, const char* statesPath,
		const char* labelsPath, bool returnPlan) {
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

		if (returnPlan) {
		    //if (simulation.getSystemModule()->par("usePredictor").boolValue()) {

		    //} else {
		    status = execlp(PRISM, PRISM, modelPath, "-pctl", PRISM_PROPERTY, "-exportadv", adversaryPath,
				"-exportstates", statesPath, "-exportlabels", labelsPath, (char*) 0);
		    //}
		} else {
		    status = execlp(PRISM, PRISM, modelPath, "-pctl", PRISM_PROPERTY, "-exportadv", adversaryPath,
		                    "-exportstates", statesPath, CUDD, CUDDMEM, "-s", (char*) 0);
		}

		if (status == -1) { // the only option really, otherwise execlp doesn't return
		    //cout << "runPrism() error: " << strerror(errno) << endl;
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

/**
 * Finds the states that correspond to the current time.
 *
 * Assumption: time is the first variable in the state
 */
set<int> getNowStates(const char* statesPath) {
	// TODO throw exception on error (but be careful with the method that calls this one)

	/*
	 * Format:
	 * (time,s,env_turn,addServer_state,addServer_go,removeServer_state,removeServer_go,servers)
	 * 0:(0,0,false,0,false,0,true,1)
	 * 1:(0,0,false,0,true,0,true,1)
	 *
	 * From the second line on, the format is:
	 * <state>:(<value1>,<value2>,...)
	 */
	set<int> states;
	ifstream fin(statesPath);
	if (!fin) {
		//cout << "Could not read input file " << statesPath << endl;
		return states;
	}

	string line;
	bool firstLine = true;
	while (getline(fin, line)) {
		if (firstLine) {
			firstLine = false;
			continue;
		}
		typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
		tokenizer tokens(line, boost::char_separator<char>(":(,)"));
		tokenizer::iterator it = tokens.begin();
		if (it != tokens.end()) {
			int state = atoi(it->c_str());
			if (++it != tokens.end()) {
				if (*it == "0") {
					states.insert(state);
					//cout << "State @0: " << state << endl;
					continue;
				}
			}
		}
		break;
	}
	fin.close();

	return states;
}


/**
 * Finds the actions that take place in the given states
 *
 * Assumption: time is the first variable in the state
 */
struct AdversaryRow {
	int next;
	double probability;
	string action;
};

typedef map<int, AdversaryRow> AdversaryTable;

vector<string> getActions(const char* adversaryPath, const char* labelsPath,
		set<int>& states) {
	// TODO throw exception on error

	/*
	 * The format is like this:
	 * 4673 ?
	 * 0 3 1 clk
	 * 1 0 1 addServer_nostart
	 * 2 4 1
	 *
	 * From the second like on, <from state> <to state> <probability> [<action>]]
	 */


	vector<string> actions;
	typedef set<string> StringSet;
	StringSet actionSet;
	ifstream fin(adversaryPath);
	if (!fin) {
		//cout << "Could not read input file " << adversaryPath << endl;
		return actions;
	}


	// first, load the adversary table for the states at time 0
	AdversaryTable table;

	size_t suffixLenght = strlen(TACTIC_SUFFIX);
	size_t divertprefixLen = strlen(DIVERT);
	string line;
	bool firstLine = true;
	while (getline(fin, line)) {
		if (firstLine) {
			firstLine = false;
			continue;
		}
		typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
		tokenizer tokens(line, boost::char_separator<char>(" "));
		tokenizer::iterator it = tokens.begin();
		if (it != tokens.end()) {
			int state = atoi(it->c_str());
			//cout << "state = " << state << endl;
			if (states.find(state) != states.end()) { // this is one of the relevant states
				AdversaryRow row;
				++it; // point to target state;
				row.next = atoi(it->c_str());
				++it; // point to the probability
				row.probability = atof(it->c_str());
				if (++it != tokens.end()) { // has action
					size_t actionLength = it->length();
					//cout << "action### " << *it << endl;
					if (actionLength > suffixLenght || actionLength > divertprefixLen) {
						if (it->compare(actionLength - suffixLenght, suffixLenght, TACTIC_SUFFIX) == 0
						        || strncmp((*it).c_str(), DIVERT, divertprefixLen) == 0) { // it's a tactic start action
							row.action = *it;
						}
					}
				}
				table.insert(pair<int, AdversaryRow>(state, row));
			}
		}
	}
	fin.close();

	// now find the initial state;
	/*
	 * assumptions:
	 * 	0 is "initial", and 0 is always the first label if it appears in a row
	 */
	int state = -1;
	ifstream labels(labelsPath);
	if (!labels) {
		//cout << "Could not read input file " << labelsPath << endl;
		return actions;
	}
	firstLine = false;
	while (getline(labels, line)) {
		if (firstLine) {
			firstLine = false;
			continue;
		}
		typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
		tokenizer tokens(line, boost::char_separator<char>(": "));
		tokenizer::iterator it = tokens.begin();
		if (it != tokens.end()) {
			int rowState = atoi(it->c_str());
			if (++it != tokens.end()) {
				int labelNumber = atoi(it->c_str());
				if (labelNumber == 0) {
					state = rowState;
					break;
				}
			}
		}
	}
	labels.close();

	/*
	 * Now walk the table from the initial state collecting the actions
	 * If we get to a point where the transition has prob < 1, we stop
	 * because it is probably the environment making a stochastic transition,
	 * which means that the system turn ended
	 */
	//printf("state %d, table size = %u\n", state, table.size());

	AdversaryTable::iterator rowIt;
	while ((rowIt = table.find(state)) != table.end()) {
		AdversaryRow& row = rowIt->second;
		if (!row.action.empty()) {
			actions.push_back(row.action);
		}
		//cout << row.action << "#" << row.probability << "##" << row.next << endl;
		if (row.probability < 1.0) {
			break;;
		}
		state = row.next;
	}

	return actions;
}

bool AdaptationPlanner::generateModel(string environmentModel, string initialState, const char* modelPath, bool returnPlan) {
	const string ENVIRONMENT_TAG = "//#environment";
	const string INIT_TAG = "//#init";

	ofstream fout(modelPath);
	if (!fout) {
		//cout << "Could not write output file " << modelPath << endl;
		return false;
	}

	string spec_file = modelTemplatePath;

	// TODO HACK Ashutosh
	// This is done to provide more flexibility in
	// deciding the planning specification for reactive planning
	if (returnPlan) {
	    spec_file = modelTemplatePath + "_fast";
	}

	ifstream fin(spec_file.c_str());

	if (!fin) {
		//cout << "Could not read input file " << get_current_dir_name() << '/' << modelTemplatePath << endl;
		//cout << "Error is: " << strerror(errno) << endl;
		//cout << "Retrying..." << endl;
	    fin.open(modelTemplatePath.c_str(), ifstream::in);
	    if (!fin) {
	        //cout << "Could not read input file " << get_current_dir_name() << '/' << modelTemplatePath << endl;
	        //cout << "Error is: " << strerror(errno) << endl;
	        return false;
	    }
		//cout << "it worked!" << endl;
	}

	string line;
	while (getline(fin, line)) {
		if (line == ENVIRONMENT_TAG) {
			fout << environmentModel << endl;
		} else if (line == INIT_TAG) {
			fout << initialState <<  endl;
		} else {
			fout << line << endl;
		}
	}

	fin.close();
	fout.close();
	return true;
}

vector<string> AdaptationPlanner::plan(string environmentModel, string initialState, std::string* path, bool returnPlan) {
	const char* modelPath = "ttimemodel.prism";
	const char* adversaryPath = "result.adv";
	const char* statesPath = "result.sta";
	const char* labelsPath = "result.lab";
    //checkPwd();

	// create temp directory
	char tempDirTemplate[] = "modelXXXXXX";
	char* tempDir = mkdtemp(tempDirTemplate);
	if (!tempDir) {
		// TODO improve error handling
		throw runtime_error("error AdaptationPlanner::plan mkdtemp");
	}

	// save current dir and chdir into the temp
	int currentDir = open(".", O_RDONLY);
	if (currentDir == -1) {
		throw runtime_error("error AdaptationPlanner::plan chdir");
	}

	printf("%s\n", tempDir);

	if (chdir(tempDir) != 0) {
		// TODO improve error handling
		close(currentDir);
		throw runtime_error("error AdaptationPlanner::plan chdir");
	}
	vector<string> actions;

	try {

        // generate model
        /*
         * perhaps this could take a probability tree as an argument, and then generate the
         * prism model with the DTMC for the env and the right initial state, reflecting the
         * state of the system and running tactics
         *
         * The tactics should also be read from some input file, because the
         * latency information needs to be used in the model and also here to compute
         * the initial state reflecting the progress of the tactics.
         * Also, if we monitor that the tactic has not finished, even though it should have, we
         * can set the initial state so that it seems that it has one period to go.
         */
	    //checkPwd();
	    //static int i = 1;
        if (generateModel(environmentModel, initialState, modelPath, returnPlan)) {
            if (runPrism(modelPath, adversaryPath, statesPath, labelsPath, returnPlan)) { // #TODO Ashutosh uncomment the code
                if (returnPlan) {
                    set<int> states = getNowStates(statesPath);
                    //printf("State Size %u\n", states.size());
                    actions = getActions(adversaryPath, labelsPath, states);
                    //printf("Actions in the plan = %u\n", actions.size());
                    for (vector<string>::iterator it = actions.begin();
                            it != actions.end(); it++) {
                        //printf("%s\n", it->c_str());
                    }
                    if (actions.size() == 0) {
                        //printf("Hi\n");
                    }
                }
            } else {
                throw runtime_error("error AdaptationPlanner::plan runPrism");
            }
            //string a = "divert_100_0_0";
            //actions.push_back(a); // TODO Remove this statement
        } else {
            throw runtime_error("error AdaptationPlanner::plan generateModel");
        }

        // restore current dir and remove temp if needed
        if (path) {
            path->assign(tempDir);
        } else {
            remove(modelPath);
            remove(adversaryPath);
            remove(statesPath);
            remove(labelsPath);
        }

        fchdir(currentDir);
        close(currentDir);

        if (!path) {
            rmdir(tempDir);
        }
	} catch(...) {
	    fchdir(currentDir);
	    close(currentDir);
	    throw;
	}

	get_absolute_path(*path);

	return actions;
}

void AdaptationPlanner::test() {
	set<int> states = getNowStates("test/s.sta");
	vector<string> actions = getActions("test/a.adv", "test/l.lab", states);
	for (unsigned i = 0; i < actions.size(); i++) {
		//cout << "Action: " << actions[i] << endl;
	}
}

AdaptationPlanner::~AdaptationPlanner() {
	// TODO Auto-generated destructor stub
}

