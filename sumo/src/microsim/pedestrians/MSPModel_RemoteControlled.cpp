/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2014-2016 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/

//
// Created by Gregor Lämmel on 31/08/16.
//

#include <microsim/MSNet.h>
#include "MSPModel_RemoteControlled.h"
#include "MSPGRPCClient.h"

MSPModel_RemoteControlled::MSPModel_RemoteControlled(const OptionsCont &oc, MSNet *pNet) :
    myNet(pNet){
    const std::string address = oc.getString("pedestrian.remote.address");
    grpcClient = new MSGRPCClient(grpc::CreateChannel(address, grpc::InsecureChannelCredentials()), pNet);

    RunRemoteStep * e = new RunRemoteStep(this);
    pNet->getBeginOfTimestepEvents()->addEvent(e,pNet->getCurrentTimeStep() + DELTA_T, MSEventControl::ADAPT_AFTER_EXECUTION);
}

PedestrianState *MSPModel_RemoteControlled::add(MSPerson *person, MSPerson::MSPersonStage_Walking *stage, SUMOTime now) {
    return nullptr;
}

bool MSPModel_RemoteControlled::blockedAtDist(const MSLane *lane, SUMOReal distToCrossing,
                                std::vector<const MSPerson *> *collectBlockers) {
    return false;
}

SUMOTime MSPModel_RemoteControlled::execute(SUMOTime now) {
    return 0;
}
