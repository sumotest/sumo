/*
 * MSGRPCClient.cpp
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
#include <grpc++/grpc++.h>
#include <utils/common/StdDefs.h>
#include <microsim/MSJunction.h>
#include "MSGRPCClient.h"

MSGRPCClient::MSGRPCClient(std::shared_ptr<Channel> channel) :
hybridsimStub(hybridsim::HybridSimulation::NewStub(channel))
{

}

MSGRPCClient::~MSGRPCClient() {

}



//hybrid simulation
void MSGRPCClient::simulateTimeInterval(SUMOTime fromIncl, SUMOTime toExcl) {
	hybridsim::LeftClosedRightOpenTimeInterval req;
	req.set_fromtimeincluding(fromIncl);
	req.set_totimeexcluding(toExcl);
	hybridsim::Empty rpl;

	ClientContext context;

	Status st = hybridsimStub->simulatedTimeInerval(&context,req,&rpl);
	if(!st.ok()){
		std::cerr << "something went wrong!" << std::endl;
	}



}

bool MSGRPCClient::transmitPedestrian(std::string& id, std::string& fromId,
		std::string& toId) {
	hybridsim::Agent req;
	req.set_id(id);
	req.set_enterid(fromId);
	req.set_leaveid(toId);
	hybridsim::Boolean rpl;

	ClientContext context;

	Status st = hybridsimStub->transferAgent(&context,req,&rpl);
	if (st.ok()) {
		return rpl.val();
	} else {
		std::cerr << "something went wrong!" << std::endl;
	}
}

void MSGRPCClient::receiveTrajectories(std::map<const std::string,MSPRCPState*>& pstates) {
	hybridsim::Empty req;
	hybridsim::Trajectories rpl;
	ClientContext context;
	Status st = hybridsimStub->receiveTrajectories(&context,req,&rpl);
	if (st.ok()){
		for (int i = 0; i < rpl.trajectories_size(); i++) {
			const hybridsim::Trajectory t = rpl.trajectories(i);
			std::cout << "tr | id: " << t.id() << " | x = " << t.x() << " | y = " << t.y() << " | spd = " << t.spd() << "\n";
			MSPRCPState* st = pstates[t.id()];
			st->setXY(t.x(),t.y());
			st->setSpeed(t.spd());

		}
	} else {
		std::cerr << "something went wrong!" << std::endl;
	}
}

void MSGRPCClient::retrieveAgents(std::map<const std::string, MSPRCPState*>& pstates,MSNet* net, SUMOTime time) {
	hybridsim::Empty req;
	hybridsim::Agents rpl;
	ClientContext context;

	Status st = hybridsimStub->retrieveAgents(&context,req,&rpl);
	if (st.ok()) {
		for (int i = 0; i < rpl.agents_size(); i++) {
			const hybridsim::Agent a = rpl.agents(i);
			MSPRCPState* st = pstates[a.id()];
//			st->getEdge()->removePerson(pstates[a.id()]->getPerson());
//			st->getPerson()->proceed(net,time);
			st->getMyStage()->moveToNextEdge(st->getPerson(),time,st->getEdge(),0);
//			st->getMyStage()->moveToNextEdge(0,0,0,0);
			pstates.erase(a.id());
			delete st;
		}
	} else {
		std::cerr << "something went wrong!" << std::endl;
	}
}

