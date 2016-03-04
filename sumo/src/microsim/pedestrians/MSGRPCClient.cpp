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
#include <microsim/MSLane.h>
#include <utils/geom/PositionVector.h>
#include <utils/geom/Position.h>
#include <regex>
#include "MSGRPCClient.h"


const int FWD(1);
const int BWD(-1);
const int UNDEF(0);

MSGRPCClient::MSGRPCClient(std::shared_ptr<Channel> channel) :
hybridsimStub(hybridsim::HybridSimulation::NewStub(channel))
{

}

MSGRPCClient::~MSGRPCClient() {
	hybridsim::Empty req;
	hybridsim::Empty rpl;
	ClientContext context;
	Status st = hybridsimStub->shutdown(&context,req,&rpl);
	if(!st.ok()){
		std::cerr << "something went wrong!" << std::endl;
		exit(-1);
	}
}



//hybrid simulation
void MSGRPCClient::simulateTimeInterval(SUMOTime fromIncl, SUMOTime toExcl) {
	hybridsim::LeftClosedRightOpenTimeInterval req;

	req.set_fromtimeincluding(fromIncl/DELTA_T);//hybrid interface requires SI units
	req.set_totimeexcluding(toExcl/DELTA_T);//hybrid interface requires SI units
	hybridsim::Empty rpl;

	ClientContext context;

	Status st = hybridsimStub->simulatedTimeInerval(&context,req,&rpl);
	if(!st.ok()){
		std::cerr << "something went wrong!" << std::endl;
		exit(-1);
	}

//	std::cout << "in JuPedSim:" << inSim << std::endl;

}


//void MSGRPCClient::extractEnterAndLeaveCoordinate(hybridsim::Coordinate * enterC, hybridsim::Coordinate * leaveC, MSPRCPState * st) {
//    const MSEdge* fromEdge = st->getEdge();
////    const MSEdge* toEdge = st->getMyStage()->getRoute().end()-1;
//    double dPos = st->getMyStage()->getDepartPos();
//    double aPos = st->getMyStage()->getArrivalPos();
//    std::cout << "dPos: " << dPos << " aPos: " << aPos << std::endl;
//
////    fromEdge
//
//    enterC->set_x(199.);
//    enterC->set_y(110.1);
//
//    leaveC->set_x(114.0);
//    leaveC->set_y(110.1);
//
//}

void MSGRPCClient::extractCoordinate(hybridsim::Coordinate *c,const MSLane * l, SUMOReal pos) {
	Position p = l->getShape().positionAtOffset(l->interpolateLanePosToGeometryPos(pos),0.);
	c->set_y(p.y());
	c->set_x(p.x());
}

bool MSGRPCClient::transmitPedestrian(MSPRCPState* st) {

	std::string id = st->getPerson()->getID();
	std::string fromId = st->getEdge()->getID();
	std::string toId = (*(st->getMyStage()->getRoute().end()-1))->getID();



	hybridsim::Agent req;
	req.set_id(id);
	//	std::cout << "from" << fromId << " to" << toId << std::endl;
	hybridsim::Coordinate * enterC = req.mutable_enterlocation();
	hybridsim::Coordinate * leaveC = req.mutable_leavelocation();





	req.set_x(st->getEdge()->getFromJunction()->getPosition().x());
	req.set_y(st->getEdge()->getFromJunction()->getPosition().y());

	bool first = true;
	bool last = false;
	for (const MSEdge * e : st->getMyStage()->getRoute()) {
		if (e == (*(st->getMyStage()->getRoute().end()-1))) {
			last = true;
		}
		bool fnd = false;
		for (const MSLane * ln : e->getLanes()) {
			if (ln->allowsVehicleClass(SUMOVehicleClass::SVC_PEDESTRIAN)) {
				if (first){
					extractCoordinate(enterC,ln,st->getMyStage()->getDepartPos());
					first = false;
				}
				if (last) {
					extractCoordinate(leaveC,ln,st->getMyStage()->getArrivalPos());
				}
				hybridsim::Link * l =  req.mutable_leg()->add_link();
				Position p = ln->getShape().getCentroid();
				hybridsim::Coordinate * cr = l->mutable_centroid();
				cr->set_x(p.x());
				cr->set_y(p.y());
				l->set_id(e->getID());
				fnd = true;
				break;
			}
		}
		if (!fnd) {
			std::cerr << "no pedestrian lane found for edge " << e->getID() << std::endl;
			exit(-1);
		}
	}


	hybridsim::Boolean rpl;

	ClientContext context;

	Status status = hybridsimStub->transferAgent(&context,req,&rpl);
//	inSim++;
	if (status.ok()) {
		return rpl.val();
	} else {
		std::cerr << "something went wrong!" << std::endl;
		exit(-1);
	}

}

void MSGRPCClient::receiveTrajectories(std::map<const std::string,MSPRCPState*>& pstates,SUMOTime time) {
	hybridsim::Empty req;
	hybridsim::Trajectories rpl;
	ClientContext context;
	Status st = hybridsimStub->receiveTrajectories(&context,req,&rpl);
	if (st.ok()){
		for (int i = 0; i < rpl.trajectories_size(); i++) {
			const hybridsim::Trajectory t = rpl.trajectories(i);
			MSPRCPState* st = pstates[t.id()];
			st->setXY(t.x(),t.y());
			st->setSpeed(t.spd());
			st->setAngle(t.phi());
			if (t.linkid() != "" && t.linkid() != st->getEdge()->getID()) {
				const MSEdge * oldEdge = st->getEdge();
				const MSEdge * newEdge = st->incrEdge();
				st->getMyStage()->moveToNextEdge(st->getPerson(),time,oldEdge,newEdge);
			}
		}
	} else {
		std::cerr << "something went wrong!" << std::endl;
		exit(-1);
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
			st->getMyStage()->moveToNextEdge(st->getPerson(),time,st->getEdge(),0);
			int s0 = pstates.size();
			pstates.erase(a.id());
			int s1 = pstates.size();
			if (s0 == s1) {
				std::cerr << "something went wrong!" << std::endl;
			}
			delete st;
		}
	} else {
		std::cerr << "something went wrong!" << std::endl;
		exit(-1);
	}
}

