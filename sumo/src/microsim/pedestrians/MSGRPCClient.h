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
#include <utils/geom/PositionVector.h>
#include <microsim/MSEdge.h>
#include <microsim/MSLane.h>
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
	MSGRPCClient(std::shared_ptr<Channel> channel, MSNet* net);
	~MSGRPCClient();

	//hybrid simulation
	void simulateTimeInterval(SUMOTime fromIncl, SUMOTime toExcl);
	bool transmitPedestrian(MSPRCPState* st);
	void receiveTrajectories(std::map<const std::string,MSPRCPState*>& pstates, SUMOTime time);
	void retrieveAgents(std::map<const std::string,MSPRCPState*>& pstates,MSNet* net, SUMOTime time);

private:
	std::unique_ptr<hybridsim::HybridSimulation::Stub> hybridsimStub;
	MSNet* net;


	//	void extractEnterAndLeaveCoordinate(hybridsim::Coordinate * enterC, hybridsim::Coordinate * leaveC, MSPRCPState * st);
	void extractCoordinate(hybridsim::Coordinate *c,const MSLane * l, SUMOReal pos);
	//	int inSim = 0;

	void initalized();

	class TransitionComparator {
	public:
		TransitionComparator(const PositionVector * ref): ref(ref) {};
		bool operator() (hybridsim::Transition t1,hybridsim::Transition t2) {
			double o1 = ref->nearest_offset_to_point2D(vert2Pos(t1.vert2()),false);
			double o2 = ref->nearest_offset_to_point2D(vert2Pos(t2.vert2()),false);
			return o1 < o2;
		}

	private:
		const PositionVector * ref;
	};

	static inline Position vert2Pos(const hybridsim::Coordinate& vert) {
		return Position(vert.x(),vert.y());
	}

	void createWalkingAreaSubroom(hybridsim::Subroom * subroom, const PositionVector shape, std::vector<hybridsim::Transition>& vec);
};
#endif /*MSGRPC_CLIENT_H*/
