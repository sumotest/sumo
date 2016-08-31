//
// Created by Gregor Lämmel on 31/08/16.
//

#ifndef SUMO_REMOTEMODEL_H
#define SUMO_REMOTEMODEL_H


#include <utils/options/OptionsCont.h>
#include <microsim/MSNet.h>
#include "MSPModel.h"

class RemoteModel : public ::MSPModel {

public:
    RemoteModel(const OptionsCont &cont, MSNet *pNet);

    virtual PedestrianState *add(MSPerson *person, MSPerson::MSPersonStage_Walking *stage, SUMOTime now);

    virtual bool blockedAtDist(const MSLane *lane, SUMOReal distToCrossing,
                               std::vector<const MSPerson *> *collectBlockers);
};


#endif //SUMO_REMOTEMODEL_H
