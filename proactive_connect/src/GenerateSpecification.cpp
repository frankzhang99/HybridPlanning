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
 * GenerateSpecification.cpp
 *
 *  Created on: Feb 16, 2016
 *      Author: ashutosp
 */

#include "GenerateSpecification.h"
#include "assert.h"
#include <sstream>

using namespace std;

GenerateSpecification::GenerateSpecification(
        const std::string fileName,
        const ServerTactic* serverTactic,
        const DimmerTactic* dimmerTactic,
        const DivertTactic* divertTactic) :
        mFile(NULL),
        mServerTactic(serverTactic),
        mDimmerTactic(dimmerTactic),
        mDivertTactic(divertTactic) {
    // TODO Auto-generated constructor stub
       // mFile = file;
    mFile = new std::ofstream(fileName.c_str());
    assert(mFile != NULL);
    //mFile(fileName.c_str());
    //mFile.open(fileName.c_str());
}

GenerateSpecification::~GenerateSpecification() {
    // TODO Auto-generated destructor stub
    delete mFile;
}

void GenerateSpecification::Generate(const std::string& initialState,
        const std::string& envModel) {
    // Write initial state
    *mFile << initialState << endl;
    *mFile << endl;
    *mFile << endl;

    // Write environment model
    *mFile << envModel << endl;
    *mFile << endl;
    *mFile << endl;

    //mFile << envModel << std::endl;
    WriteClk();
    *mFile << endl;
    *mFile << endl;

    *mFile << "label \"final\" = time = HORIZON + 1;" << endl;
    *mFile << "formula sys_go = readyToTick;" << endl;
    *mFile << "const int addServer_LATENCY_PERIODS = ceil(addServer_LATENCY / PERIOD);" << endl;

    *mFile << endl;
    *mFile << endl;

    WriteController();
    *mFile << endl;
    *mFile << endl;

    if (mServerTactic != NULL) {
        if (mServerTactic->mAddServer) {
            WriteAddServer();
        } else {
            WriteRemoveServer();
        }
    }

    *mFile << endl;
    *mFile << endl;

    if (mDimmerTactic != NULL) {
        if (mDimmerTactic->mIncreaseDimmer) {
            WriteIncDimmer();
        } else {
            WriteDecDimmer();
        }
    }

    *mFile << endl;
    *mFile << endl;

    // TODO check for divert traffic

    WriteUtilityCalculation();

    mFile->close();
}

void GenerateSpecification::WriteClk() {
    stringstream clkModule;

    clkModule << "module clk" << endl;
    clkModule << "    time : [0..HORIZON + 1] init 0;"  << endl;
    clkModule << "    readyToTick : bool init true;" << endl;
    clkModule << "    [tick] readyToTick & time < HORIZON + 1 -> 1 : (time' = time + 1) & (readyToTick'=false);" << endl;
    clkModule << "    [tack] !readyToTick -> 1 : (readyToTick'=true);" << endl;
    clkModule << "endmodule" << endl;

    *mFile << clkModule.str();
}

void GenerateSpecification::WriteController() {
    *mFile << "module controller" << endl;
    *mFile << "   active_servers_A : [0..MAX_SERVERS_A] init ini_servers_A;" << endl;
    *mFile << "   active_servers_B : [0..MAX_SERVERS_B] init ini_servers_B;" << endl;
    *mFile << "   active_servers_C : [0..MAX_SERVERS_C] init ini_servers_C;" << endl;


    *mFile << "   traffic_A : [0..4] init ini_traffic_A;" << endl;
    *mFile << "   traffic_B : [0..4] init ini_traffic_B;" << endl;
    *mFile << "   traffic_C : [0..4] init ini_traffic_C;" << endl;

//        [addServerC_complete] active_servers_C < MAX_SERVERS_C -> 1 : (active_servers_C' = active_servers_C + 1)
//            & (traffic_A' = 1) & (traffic_B' = 1) & (traffic_C' = 2);
//
//        [op] dimmer > 1 -> 1 : (dimmer' = dimmer - 1);
//        [no_op] dimmer < 3 -> 1 : (dimmer' = dimmer + 1); // like increase fidelity
     *mFile << "endmodule" << endl;
}

// TODO remove duplicate code
void GenerateSpecification::WriteAddServer() {
    *mFile << "module addServer" << endl;
    *mFile << "addServerA_state : [0..addServer_LATENCY_PERIODS] init ini_addServerA_state;" << endl;
    *mFile << "addServerB_state : [0..addServer_LATENCY_PERIODS] init ini_addServerB_state;" << endl;
    *mFile << "addServerC_state : [0..addServer_LATENCY_PERIODS] init ini_addServerC_state;" << endl;

    *mFile << "addServer_go : bool init true;" << endl;
    *mFile << "no_addServer : bool init false;" << endl;

    *mFile << "// tactic applicable, start it" << endl;

    switch (mServerTactic->mServerType) {
    case A:
        *mFile << "[op] sys_go & addServer_go & !no_addServer// can go" << endl;
        *mFile << "        & addServerA_state = 0 // tactic has not been started" << endl;
        *mFile << "        -> (addServerA_state' = 1) & (addServer_go' = false);" << endl;

        *mFile << "// tactic applicable, but don't use it" << endl;
        *mFile << "[no_op] sys_go & addServer_go // can go" << endl;
        *mFile << "        & addServerA_state = 0 // tactic has not been started" << endl;
        *mFile << "        -> (addServer_go' = false) & (no_addServer' = true);" << endl;

        *mFile << "[progressA] sys_go & addServer_go" << endl;
        *mFile << "            & addServerA_state > 0 & addServerA_state < addServer_LATENCY_PERIODS" << endl;
        *mFile << "            -> 1 : (addServerA_state' = addServerA_state + 1) & (addServer_go' = false);" << endl;

        *mFile << "[addServerA_complete] sys_go & addServer_go" << endl;
        *mFile << "            & addServerA_state = addServer_LATENCY_PERIODS // completed" << endl;
        *mFile << "            -> 1 : (addServerA_state' = 0) & (addServer_go' = false) & (no_addServer' = true);" << endl;
        break;
    case B:
        *mFile << "[op] sys_go & addServer_go & !no_addServer// can go" << endl;
        *mFile << "        & addServerB_state = 0 // tactic has not been started" << endl;
        *mFile << "        -> (addServerB_state' = 1) & (addServer_go' = false);" << endl;

        *mFile << "// tactic applicable, but don't use it" << endl;
        *mFile << "[no_op] sys_go & addServer_go // can go" << endl;
        *mFile << "        & addServerB_state = 0 // tactic has not been started" << endl;
        *mFile << "        -> (addServer_go' = false) & (no_addServer' = true);" << endl;

        *mFile << "[progressB] sys_go & addServer_go" << endl;
        *mFile << "            & addServerB_state > 0 & addServerB_state < addServer_LATENCY_PERIODS" << endl;
        *mFile << "            -> 1 : (addServerB_state' = addServerB_state + 1) & (addServer_go' = false);" << endl;

        *mFile << "[addServerB_complete] sys_go & addServer_go" << endl;
        *mFile << "            & addServerB_state = addServer_LATENCY_PERIODS // completed" << endl;
        *mFile << "            -> 1 : (addServerB_state' = 0) & (addServer_go' = false) & (no_addServer' = true);" << endl;
        break;
    case C:
        *mFile << "[op] sys_go & addServer_go & !no_addServer// can go" << endl;
        *mFile << "        & addServerC_state = 0 // tactic has not been started" << endl;
        *mFile << "        -> (addServerC_state' = 1) & (addServer_go' = false);" << endl;

        *mFile << "// tactic applicable, but don't use it" << endl;
        *mFile << "[no_op] sys_go & addServer_go // can go" << endl;
        *mFile << "        & addServerC_state = 0 // tactic has not been started" << endl;
        *mFile << "        -> (addServer_go' = false) & (no_addServer' = true);" << endl;

        *mFile << "[progressC] sys_go & addServer_go" << endl;
        *mFile << "            & addServerC_state > 0 & addServerC_state < addServer_LATENCY_PERIODS" << endl;
        *mFile << "            -> 1 : (addServerC_state' = addServerC_state + 1) & (addServer_go' = false);" << endl;

        *mFile << "[addServerC_complete] sys_go & addServer_go" << endl;
        *mFile << "            & addServerC_state = addServer_LATENCY_PERIODS // completed" << endl;
        *mFile << "            -> 1 : (addServerC_state' = 0) & (addServer_go' = false) & (no_addServer' = true);" << endl;
        break;
    default:
        assert(false);
    }

    *mFile << "[tick] !addServer_go | no_addServer -> 1 : (addServer_go' = true);" << endl;
    *mFile << "endmodule" << endl;
}

// TODO Could be reduced. One statement for all the server types
void GenerateSpecification::WriteRemoveServer() {
    // This remove server constraints that only one server could be removed in one monitoring cycle.
    *mFile << "module removeServer" << endl;
    *mFile << "    removeServer_go : bool init false;" << endl;
    *mFile << "    removeServer_used : bool init false;" << endl;

    switch (mServerTactic->mServerType) {
    case A:
        *mFile << "    [op] sys_go & removeServer_go" << endl;
        //*mFile << "        & removeServerA_applicable // applicability conditions" << endl;
        *mFile << "        -> (removeServer_go' = false) & (removeServer_used' = true);" << endl;
        break;
    case B:
        *mFile << "    [op] sys_go & removeServer_go" << endl;
        //*mFile << "        & removeServerB_applicable // applicability conditions" << endl;
        *mFile << "        -> (removeServer_go' = false) & (removeServer_used' = true);" << endl;
        break;
    case C:
        *mFile << "    [op] sys_go & removeServer_go" << endl;
        //*mFile << "        & removeServerC_applicable // applicability conditions" << endl;
        *mFile << "        -> (removeServer_go' = false) & (removeServer_used' = true);" << endl;
        break;
    default:
        assert(false);
    }

    *mFile << "    // tactic applicable but not used" << endl;
    *mFile << "    [no_op] sys_go & removeServer_go // can go" << endl;
    *mFile << "        -> (removeServer_go' = false);" << endl;

    *mFile << "endmodule" << endl;
}

void GenerateSpecification::WriteIncDimmer() {
    *mFile << "module increaseDimmer" << endl;
    *mFile << "    increaseDimmer_go : bool init true;" << endl;

    *mFile << "    [op] sys_go & increaseDimmer_go" << endl;
    *mFile << "        -> (increaseDimmer_go' = false);" << endl;

    *mFile << "   // tactic applicable but not used" << endl;
    *mFile << "    [no_op] sys_go & increaseDimmer_go // can go" << endl;
    *mFile << "        -> (increaseDimmer_go' = false);" << endl;

    *mFile << "endmodule" << endl;
}

void GenerateSpecification::WriteDecDimmer() {
    *mFile << "module decreaseDimmer" << endl;
    *mFile << "    decreaseDimmer_go : bool init true;" << endl;

    *mFile << "    [op] sys_go & decreaseDimmer_go" << endl;
    *mFile << "        -> (decreaseDimmer_go' = false);" << endl;

    *mFile << "   // tactic applicable but not used" << endl;
    *mFile << "    [no_op] sys_go & decreaseDimmer_go // can go" << endl;
    *mFile << "        -> (decreaseDimmer_go' = false);" << endl;

    *mFile << "endmodule" << endl;
}

void GenerateSpecification::WriteUtilityCalculation() {
    *mFile << "formula dimmerFactor = (dimmer - 1) / (DIMMER_LEVELS - 1);" << endl;
    *mFile << "formula interarrivalMean = stateValue * interArrivalScaleFactorForDecision * active_servers;" << endl;

    *mFile << "formula Pa = (traffic_A * 25)/100;" << endl;
    *mFile << "formula Pb = (traffic_B * 25)/100;" << endl;
    *mFile << "formula Pc = (traffic_C * 25)/100;" << endl;

    *mFile << "formula service_rate_A = dimmerFactor * (MAX_ARRIVALA_CAPACITY_LOW)" << endl;
    *mFile << "      + (1 - dimmerFactor) * (MAX_ARRIVALA_CAPACITY);" << endl;
    *mFile << "formula service_rate_B = dimmerFactor * (MAX_ARRIVALB_CAPACITY_LOW)" << endl;
    *mFile << "      + (1 - dimmerFactor) * (MAX_ARRIVALB_CAPACITY);" << endl;
    *mFile << "formula service_rate_C = dimmerFactor * (MAX_ARRIVALC_CAPACITY_LOW)" << endl;
    *mFile << "      + (1 - dimmerFactor) * (MAX_ARRIVALC_CAPACITY);" << endl;

    *mFile << "formula rhoA = Pa/(service_rate_A*interarrivalMean);" << endl;
    *mFile << "formula rhoB = Pb/(service_rate_B*interarrivalMean);" << endl;
    *mFile << "formula rhoC = Pc/(service_rate_C*interarrivalMean);" << endl;

    *mFile << "const double DBL_MAX = 1.5e+300;" << endl;

    *mFile << "formula rt_A = 1/(service_rate_A - (throughput*Pa));" << endl;
    *mFile << "formula rt_B = 1/(service_rate_B - (throughput*Pb));" << endl;
    *mFile << "formula rt_C = 1/(service_rate_C - (throughput*Pc));" << endl;

    //formula expected_wait_time = interarrivalMean * (rhoA/(1-rhoA) + rhoB/(1-rhoB) + rhoC/(1-rhoC));
    *mFile << "formula expected_wait_time = (Pa*rt_A + Pb*rt_B + Pc*rt_C);" << endl;
    *mFile << "formula rt = (interarrivalMean = 0 ? 0 : ((rhoA > 1 | rhoB > 1 | rhoC > 1) ? DBL_MAX : expected_wait_time));" << endl;

    // Response time to clients utility function
    *mFile << "const double RT_THRESHOLD = 1.0;" << endl;

    *mFile << "const double NORMAL_A_REVENUE = (SERVERA_COST_SEC / MAX_ARRIVALA_CAPACITY) * 10;" << endl;
    *mFile << "const double DIMMER_A_REVENUE = (SERVERA_COST_SEC / MAX_ARRIVALA_CAPACITY_LOW) * 3 / 2;" << endl;
    *mFile << "const double NORMAL_B_REVENUE = (SERVERB_COST_SEC / MAX_ARRIVALB_CAPACITY) * 10;" << endl;
    *mFile << "const double DIMMER_B_REVENUE = (SERVERB_COST_SEC / MAX_ARRIVALB_CAPACITY_LOW) * 3 / 2;" << endl;
    *mFile << "const double NORMAL_C_REVENUE = (SERVERC_COST_SEC / MAX_ARRIVALC_CAPACITY) * 10;" << endl;
    *mFile << "const double DIMMER_C_REVENUE = (SERVERC_COST_SEC / MAX_ARRIVALC_CAPACITY_LOW) * 3 / 2;" << endl;

    *mFile << "const double DIMMER_REVENUE = DIMMER_A_REVENUE + DIMMER_B_REVENUE + DIMMER_C_REVENUE;" << endl;
    *mFile << "const double NORMAL_REVENUE = NORMAL_A_REVENUE + NORMAL_B_REVENUE + NORMAL_C_REVENUE;" << endl;

    *mFile << "formula serverA_cost = ((addServerA_state > 0 ? 1 : 0) + active_servers_A) * SERVERA_COST_SEC;" << endl;
    *mFile << "formula serverB_cost = ((addServerB_state > 0 ? 1 : 0) + active_servers_B) * SERVERB_COST_SEC;" << endl;
    *mFile << "formula serverC_cost = ((addServerC_state > 0 ? 1 : 0) + active_servers_C) * SERVERC_COST_SEC;" << endl;
    *mFile << "formula cost = serverA_cost + serverB_cost + serverC_cost;" << endl;

    *mFile << "formula throughput = 1/interarrivalMean;" << endl;

    *mFile << "formula basicUtilityA = throughput * Pa * (dimmerFactor * DIMMER_A_REVENUE + (1 - dimmerFactor) * NORMAL_A_REVENUE);" << endl;
    *mFile << "formula basicUtilityB = throughput * Pb * (dimmerFactor * DIMMER_B_REVENUE + (1 - dimmerFactor) * NORMAL_B_REVENUE);" << endl;
    *mFile << "formula basicUtilityC = throughput * Pc * (dimmerFactor * DIMMER_C_REVENUE + (1 - dimmerFactor) * NORMAL_C_REVENUE);" << endl;

    *mFile << "formula basicUtility = basicUtilityA + basicUtilityB + basicUtilityC;" << endl;
    *mFile << "formula active_servers = active_servers_A + active_servers_B + active_servers_C;" << endl;
    *mFile << "formula poweredServers = (addServerA_state > 0 ? 1 : 0) + (addServerB_state > 0 ? 1 : 0) + (addServerC_state > 0 ? 1 : 0 )" << endl;
    *mFile << "     + active_servers;" << endl;
    *mFile << "formula MAX_SERVERS = MAX_SERVERS_A + MAX_SERVERS_B + MAX_SERVERS_C;" << endl;

    *mFile << "formula MAX_SERVER_COST = MAX_SERVERS_A * SERVERA_COST_SEC" << endl;
    *mFile << "            + MAX_SERVERS_B * SERVERB_COST_SEC" << endl;
    *mFile << "            + MAX_SERVERS_C * SERVERC_COST_SEC;" << endl;

    *mFile << "formula uTotal = (rt = DBL_MAX & (poweredServers < MAX_SERVERS | dimmer < DIMMER_LEVELS))" << endl;
    *mFile << "                    ? -(MAX_SERVER_COST) // avoid unstable solutions" << endl;
    *mFile << "                : (((rt > RT_THRESHOLD | rt <= 0) ? penalty: basicUtility) - cost);" << endl;


    *mFile << "rewards \"util\"" << endl;
    *mFile << "    [tack] true : (100000.0 / HORIZON) + (PERIOD)*(uTotal);" << endl;
    *mFile << "endrewards" << endl;
}
