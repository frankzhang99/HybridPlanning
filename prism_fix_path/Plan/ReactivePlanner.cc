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
 * ReactivePlanner.cpp
 *
 *  Created on: Apr 12, 2015
 *      Author: ashutosh
 */

#include <Plan/ReactivePlanner.h>
#include <assert.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fstream>
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

ReactivePlanner::ReactivePlanner(
        const char* planner_path,
        const char* run_dir,
        const char* domain_file,
        const char* problem_file,
        int total_plans,
        bool debug) :
                m_planner_path(planner_path),
                m_run_dir(run_dir),
                m_domain_file(domain_file),
                m_problem_file(problem_file),
                m_total_plans(total_plans),
                m_debug(debug) {
    set_output_file_name();
}

// To be on safer side.
// However, this call is not expected
ReactivePlanner::ReactivePlanner():
        m_planner_path(""),
        m_run_dir(""),
        m_domain_file(""),
        m_problem_file(""),
        m_total_plans(0),
        m_debug(false),
        m_output_file("") {
    assert(false);

}

ReactivePlanner::~ReactivePlanner() {
    // TODO Auto-generated destructor stub
}

void ReactivePlanner::generate_plan(Plan& plan) {
    bool status = execute_planner();
    assert(status);

    parse_plan(plan);
}

void ReactivePlanner::set_output_file_name() {
    // Format- plan_problem.pddl_1.SOL

    m_output_file = "plan_";
    m_output_file += m_problem_file;
    m_output_file += "_";
    m_output_file += std::to_string(m_total_plans);
    m_output_file += ".SOL";
}

void ReactivePlanner::parse_plan(Plan& plan) {
    // TODO Check if m_output_file exists
    std::string line;
    ifstream myfile(m_output_file);
    int line_no = 0;

    if (m_debug) {
        const char * output = m_output_file.c_str();
        printf("%s\n", output);
    }

    if (myfile.is_open()) {
        while (getline(myfile, line)) {
            ++line_no;
            //cout << line << '\n';
            if (line_no == 12) {
                Plan::iterator it = plan.begin();
                if (line.find("ADD-SERVER") != std::string::npos) {
                    plan.insert(it, "ADD-SERVER");
                } else if (line.find("INCREASE-BROWNOUT") != std::string::npos) {
                    plan.insert(it, "INCREASE-BROWNOUT");
                } else {
                    assert(false);
                }
            }
        }
        myfile.close();
    } else {
        //cout << "Unable to open file";
    }

    if (plan.size() > 0) {
        printf("action = %s\n", plan[0].c_str());
    }
}

bool ReactivePlanner::execute_planner() {
    bool status = true;

    if (m_debug) {
        static int i=0;
        printf("%d\n",i++);
    }

    pid_t pid = fork();
    if (pid == 0) {
        // child
        // TODO perhaps we need to pipe the output to get errorr descriptions
        // see this url for example: http://www.cs.uleth.ca/~holzmann/C/system/pipeforkexec.html
        int status = execlp(m_planner_path.c_str(), m_planner_path.c_str(),
                "-p", m_run_dir.c_str(),
                "-o", m_domain_file.c_str(),
                "-f", m_problem_file.c_str(),
                "-n", std::to_string(m_total_plans).c_str(),
                (char*) 0);
        if (status == -1) { // the only option really, otherwise execlp doesn't return
            //std::cout << "LPG-td() error: " << strerror(errno) << std::endl;
            status = false; // error
        }
    } else if (pid == -1) {
        // error
        status = false;
    } else {
        // parent
        int status;
        int rval = waitpid(pid, &status, 0);
        if (!(rval > 0 && WIFEXITED(status) && WEXITSTATUS(status) == 0)) {
            status = false;
        }
    }

    return status;
}


