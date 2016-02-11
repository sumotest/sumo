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
#ifndef MSGRPC_CLIENT_H
#define MSGRPC_CLIENT_H

#include <memory>
#include <iostream>
#include <grpc++/grpc++.h>
#include <utils/common/SUMOTime.h>
#include <utils/geom/Position.h>
#include <microsim/MSEdge.h>
#include "MSPerson.h"
#include "MSPRCPState.h"
#include "hybridsim.grpc.pb.h"
using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;




class MSGRPCClient {
public :
	struct CmpWlkTmStruct {
		SUMOTime wlkTm;
		SUMOReal currentBeginPos;
		SUMOReal currentEndPos;
		SUMOReal lastEntrTm;
	};
public:
	MSGRPCClient(std::shared_ptr<Channel> channel);
	virtual ~MSGRPCClient();

	//hybrid simulation
	void simulateTimeInterval(SUMOTime fromIncl, SUMOTime toExcl);
	bool transmitPedestrian(std::string& id, std::string& fromId, std::string& toId);
	void receiveTrajectories(std::map<const std::string,MSPRCPState*>& pstates);
	void retrieveAgents(std::map<const std::string,MSPRCPState*>& pstates,MSNet* net, SUMOTime time);

private:
	std::unique_ptr<hybridsim::HybridSimulation::Stub> hybridsimStub;

};
#endif /*MSGRPC_CLIENT_H*/
