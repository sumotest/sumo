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

#ifndef SUMO_REMOTEMODEL_H
#define SUMO_REMOTEMODEL_H


#include <utils/options/OptionsCont.h>
#include <microsim/MSNet.h>
#include "MSPModel.h"
#include "MSPGRPCClient.h"

class MSPModel_RemoteControlled : public ::MSPModel {

public:
    MSPModel_RemoteControlled(const OptionsCont &cont, MSNet *pNet);

    virtual PedestrianState *add(MSPerson *person, MSPerson::MSPersonStage_Walking *stage, SUMOTime now);

    virtual bool blockedAtDist(const MSLane *lane, SUMOReal distToCrossing,
                               std::vector<const MSPerson *> *collectBlockers);
public:
    SUMOTime execute(SUMOTime now);

    class RunRemoteStep : public  Command{ ;


    public:
        RunRemoteStep(MSPModel_RemoteControlled * modelRemoteControlled) : myModelRemoteControlled(modelRemoteControlled){}

        SUMOTime execute(SUMOTime currentTime){return myModelRemoteControlled->execute(currentTime);};


    private:
        MSPModel_RemoteControlled * myModelRemoteControlled;
    };
private:
    MSNet * myNet;
    MSGRPCClient *grpcClient;
};


#endif //SUMO_REMOTEMODEL_H
