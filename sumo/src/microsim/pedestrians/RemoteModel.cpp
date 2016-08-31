//
// Created by Gregor Lämmel on 31/08/16.
//

#include <microsim/MSNet.h>
#include "RemoteModel.h"

RemoteModel::RemoteModel(const OptionsCont &cont, MSNet *pNet) {

}

PedestrianState *RemoteModel::add(MSPerson *person, MSPerson::MSPersonStage_Walking *stage, SUMOTime now) {
    return nullptr;
}

bool RemoteModel::blockedAtDist(const MSLane *lane, SUMOReal distToCrossing,
                                std::vector<const MSPerson *> *collectBlockers) {
    return false;
}
