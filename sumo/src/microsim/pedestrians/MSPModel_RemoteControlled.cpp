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
    assert(person->getCurrentStageType() == MSTransportable::MOVING_WITHOUT_VEHICLE);

    MSPGRCPState * state = new MSPGRCPState(person,stage);
    const MSEdge* edge = *(stage->getRoute().begin());
    if (buffers.find(edge->getID()) == buffers.end()) {
        std::queue<MSPGRCPState*> buffer;
        buffer.push(state);


        std::string str =  edge->getID();
        buffers[edge->getID()] = buffer;
    } else {
        std::queue<MSPGRCPState*> * buffer = &buffers[edge->getID()];
        buffer->push(state);
    }

    pstates[person->getID()] = state;

    return state;
}

bool MSPModel_RemoteControlled::blockedAtDist(const MSLane *lane, SUMOReal distToCrossing,
                                std::vector<const MSPerson *> *collectBlockers) {
    return false;
}

SUMOTime MSPModel_RemoteControlled::execute(SUMOTime now) {
    //do all the handling here

    //1. transfer agents as long as there is space (SUMO --> external sim)
    std::map<const std::string,std::queue<MSPGRCPState*>>::iterator it = buffers.begin();
    while(it != buffers.end()) {
        std::queue<MSPGRCPState*> buffer = (*it).second;
        handleBuffer(&buffer);
        if (buffer.empty()){
            it = buffers.erase(it);
        } else {
            it++;
        }
    }

    //2. simulate for one SUMO time step
    grpcClient->simulateTimeInterval(now,now+DELTA_T);

    //3. receive events, trajectories ...
    grpcClient->receiveTrajectories(pstates, now);

    //4. transfer agents as long as there is space (external sim --> SUMO)
    grpcClient->retrieveAgents(pstates,myNet,now);

    return DELTA_T;
}

void MSPModel_RemoteControlled::handleBuffer(std::queue<MSPGRCPState*>* buffer) {
    while(!buffer->empty()){
        MSPGRCPState* st = buffer->front();
        if (transmitPedestrian(st)) {
            buffer->pop();
        } else {
            return;
        }
    }
}

bool MSPModel_RemoteControlled::transmitPedestrian(MSPGRCPState *st) {
    return grpcClient->transmitPedestrian(st);
}
