/*
 * MSGRPCClient.h
 *
 *  Created on: Jan 19, 2016
 *      Author: laemmel
 */
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2015 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#include <memory>
#include <iostream>
#include <grpc++/grpc++.h>
#include <utils/common/SUMOTime.h>
#include <utils/geom/Position.h>
#include <microsim/MSEdge.h>
#include "MSPerson.h"
#include "noninteracting.grpc.pb.h"
using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;




class MSGRPCClient {
public :
	struct CmpWlkTmStruct {
		SUMOTime wlkTm;
		SUMOTime currentDuration;
		SUMOReal currentBeginPos;
		SUMOReal currentEndPos;
		SUMOReal lastEntrTm;
	};
public:
	MSGRPCClient(std::shared_ptr<Channel> channel);
	virtual ~MSGRPCClient();
	MSGRPCClient::CmpWlkTmStruct computeWalkingTime(const MSEdge* prev, const MSPerson::MSPersonStage_Walking& stage, SUMOTime currentTime);
	SUMOReal getEdgePos(SUMOReal myCurrentBeginPos, SUMOReal myCurrentEndPos, SUMOReal myCurrentDuration, SUMOTime myLastEntryTime, SUMOTime now);
	SUMOTime getWaitingTime();
	SUMOReal getMaxSpeed(const MSPerson::MSPersonStage_Walking& stage);



private:
	std::unique_ptr<noninteracting::PBPState::Stub> stub_;



};
