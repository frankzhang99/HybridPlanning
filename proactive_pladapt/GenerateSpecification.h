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
 * GenerateSpecification.h
 *
 *  Created on: Feb 16, 2016
 *      Author: ashutosp
 */

#ifndef GENERATESPECIFICATION_H_
#define GENERATESPECIFICATION_H_

#include <fstream>
#include <iostream>

class GenerateSpecification {
public:
    /*enum Tactics {
        addServerA = 0x01,
        addServerB = 0x02 ,
        addServerC = 0x04 ,
        removeServerA = 0x08 ,
        removeServerB = 0x016 ,
        removeServerC = 0x032 ,
        increaseDimmer = 0x064,
        decreaseDimmer = 0x0128
    };*/

    enum ServerType {
            NONE,
            A,
            B,
            C
    };

    enum TrafficLoad {
            ZERO, TWENTYFIVE, FIFTY, SEVENTYFIVE, HUNDRED, INVALID
    };

    struct ServerTactic {
        ServerTactic(bool addServer, ServerType type) : mAddServer(addServer), mServerType(type) {}
        const bool mAddServer;
        const ServerType mServerType;
    };

    struct DimmerTactic {
        DimmerTactic(bool increase) : mIncreaseDimmer(increase){}
        const bool mIncreaseDimmer;
    };

    struct DivertTactic {
        DivertTactic(TrafficLoad a, TrafficLoad b, TrafficLoad c) : mServerA(a), mServerB(b), mServerC(c) {}

        const TrafficLoad mServerA;
        const TrafficLoad mServerB;
        const TrafficLoad mServerC;
    };

    GenerateSpecification(
                    const std::string file,
                    const ServerTactic* serverTactic = NULL,
                    const DimmerTactic* dimmerTactic = NULL,
                    const DivertTactic* divertTactic = NULL
                    );

    virtual ~GenerateSpecification();

    void Generate(const std::string& initialState,
            const std::string& envModel);
    //void setServerTactic();
    //void setDimmerTactic();
    //void setDivertTactic();

private:
    void WriteController();
    void WriteClk();
    void WriteAddServer();
    void WriteRemoveServer();
    void WriteIncDimmer();
    void WriteDecDimmer();
    void WriteUtilityCalculation();

    std::ofstream* mFile;

    const ServerTactic* mServerTactic;
    const DimmerTactic* mDimmerTactic;
    const DivertTactic* mDivertTactic;

    //const std::string mFileName;
};

#endif /* GENERATESPECIFICATION_H_ */
