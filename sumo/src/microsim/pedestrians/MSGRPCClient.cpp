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
#include <algorithm>

#include <utils/common/StdDefs.h>
#include <microsim/MSEdgeControl.h>
#include <microsim/MSJunction.h>
#include <microsim/MSLane.h>
#include <utils/geom/PositionVector.h>
#include <utils/geom/Position.h>
#include <regex>
#include "MSGRPCClient.h"



const int FWD(1);
const int BWD(-1);
const int UNDEF(0);
const SUMOReal EPSILON(0.001*0.001);//1mm^2

MSGRPCClient::MSGRPCClient(std::shared_ptr<Channel> channel, MSNet* net) :
														hybridsimStub(hybridsim::HybridSimulation::NewStub(channel)), net(net)
{
	initalized();
}



void MSGRPCClient::initalized() {
	hybridsim::Scenario req;
	req.set_seed(42);
	hybridsim::Environment* env = req.mutable_environment();

	int trId = 0;

	std::vector<MSEdge*> walkingAreas;
	std::map<MSEdge*,std::vector<hybridsim::Transition>> walkingAreasTransitionMapping;

	//1. for all edges
	for (MSEdge * e : net->getEdgeControl().getEdges()){
		if (e->isInternal()) {
			continue;
		}
		if (e->isWalkingArea()) {
			walkingAreas.push_back(e);
		} else {
			for (MSLane * l : e->getLanes()) {
				if (l->allowsVehicleClass(SUMOVehicleClass::SVC_PEDESTRIAN)) {

					hybridsim::Room * room = env->add_room();
					room->set_caption(e->getID());
					room->set_id(e->getNumericalID());
					hybridsim::Subroom * subroom = room->add_subroom();
					subroom->set_id(l->getNumericalID());
					subroom->set_closed(0);
					subroom->set_class_("TODO: figure out what class means!");

					hybridsim::Transition * t1 = env->add_transition();
					t1->set_id(trId++);
					t1->set_caption("transition");
					t1->set_type("other");
					t1->set_room1_id(e->getNumericalID());
					t1->set_subroom1_id(l->getNumericalID());

					hybridsim::Transition * t2 = env->add_transition();
					t2->set_id(trId++);
					t2->set_caption("transition");
					t2->set_type("other");
					t2->set_room1_id(e->getNumericalID());
					t2->set_subroom1_id(l->getNumericalID());

					const std::vector<MSLane::IncomingLaneInfo,
					std::allocator<MSLane::IncomingLaneInfo> > incoming = l->getIncomingLanes();
					if (incoming.size() > 1) {
						std::cerr << "inconsistent network, cannot create jupedsim scenario!" << std::endl;
						exit(-1);
					}
					t1->set_room2_id((incoming.begin())->lane->getEdge().getNumericalID());
					t1->set_subroom2_id((incoming.begin())->lane->getNumericalID());
					std::vector<const MSLane*>  outgoing = l->getOutgoingLanes();
					if (outgoing.size() > 1) {
						std::cerr << "inconsistent network, cannot create jupedsim scenario!" << std::endl;
						exit(-1);
					}
					t2->set_room2_id((*(outgoing.begin()))->getEdge().getNumericalID());
					t2->set_subroom2_id((*(outgoing.begin()))->getNumericalID());

					PositionVector  s = PositionVector(l->getShape());
					double width = l->getWidth();
					s.move2side(width/2);
					hybridsim::Polygon * p1 = subroom->add_polygon();
					p1->set_caption("wall");
					for (Position p : s) {
						hybridsim::Coordinate * c = p1->add_coordinate();
						c->set_x(p.x());
						c->set_y(p.y());
					}
					hybridsim::Coordinate * c12 = t1->mutable_vert2();
					c12->set_x((s.begin())->x());
					c12->set_y((s.begin())->y());

					hybridsim::Coordinate * c21 = t2->mutable_vert1();
					c21->set_x((s.end()-1)->x());
					c21->set_y((s.end()-1)->y());
					s.move2side(-width);
					hybridsim::Polygon * p2 = subroom->add_polygon();
					p1->set_caption("wall");
					for (Position p : s) {
						hybridsim::Coordinate * c = p1->add_coordinate();
						c->set_x(p.x());
						c->set_y(p.y());
					}
					hybridsim::Coordinate * c11 = t1->mutable_vert1();
					c11->set_x((s.begin())->x());
					c11->set_y((s.begin())->y());

					hybridsim::Coordinate * c22 = t2->mutable_vert2();
					c22->set_x((s.end()-1)->x());
					c22->set_y((s.end()-1)->y());
					if ((*outgoing.begin())->getEdge().isWalkingArea()) {
						std::vector<hybridsim::Transition> * vec = &(walkingAreasTransitionMapping[&((*outgoing.begin())->getEdge())]);
						vec->push_back(*t2);
					}
					if ((incoming.begin())->lane->getEdge().isWalkingArea()) {
						std::vector<hybridsim::Transition> * vec = &(walkingAreasTransitionMapping[&((incoming.begin())->lane->getEdge())]);
						vec->push_back(*t1);
					}

				}
			}
		}
	}

	//		std::cout <<  walkingAreasTransitionMapping.size()<< "\n";
	//}

	//2nd for the walking areas
	for (MSEdge * w : walkingAreas) {
		if (walkingAreasTransitionMapping.find(w) == walkingAreasTransitionMapping.end()) {
			continue;
		}


		std::vector<hybridsim::Transition> vec = walkingAreasTransitionMapping[w];
		MSLane * l = *(w->getLanes().begin());
		hybridsim::Room * room = env->add_room();
		room->set_caption(w->getID());
		room->set_id(w->getNumericalID());
		hybridsim::Subroom * subroom = room->add_subroom();
		subroom->set_id(l->getNumericalID());
		subroom->set_closed(0);
		subroom->set_class_("TODO: figure out what class means!");

		for (hybridsim::Transition t : vec) {
			hybridsim::Transition * tr = env->add_transition();
			tr->set_id(trId++);
			tr->set_caption("transition");
			tr->set_type("other");
			tr->set_room1_id(t.room2_id());
			tr->set_subroom1_id(t.subroom2_id());
			tr->set_room2_id(t.room1_id());
			tr->set_subroom2_id(t.subroom1_id());
			hybridsim::Coordinate * c1 = tr->mutable_vert1();
			c1->set_x(t.vert2().x());
			c1->set_y(t.vert2().y());
			hybridsim::Coordinate * c2 = tr->mutable_vert2();
			c2->set_x(t.vert1().x());
			c2->set_y(t.vert1().y());
		}



		if (l->getShape().isPolyCW()){
			createWalkingAreaSubroom(subroom,l->getShape(),vec);
		} else {
			createWalkingAreaSubroom(subroom,l->getShape().reverse(),vec);
		}




	}


	hybridsim::Empty rpl;
	ClientContext context;
	Status st = hybridsimStub->initScenario(&context,req,&rpl);
	if(!st.ok()){
		std::cerr << "something went wrong!" << std::endl;
		exit(-1);
	}
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


void MSGRPCClient::createWalkingAreaSubroom(hybridsim::Subroom * subroom, const PositionVector shape,std::vector<hybridsim::Transition>& vec){
	TransitionComparator comp = TransitionComparator(&(shape));
	std::sort(vec.begin(),vec.end(),comp);
	hybridsim::Polygon * p = subroom->add_polygon();
	p->set_caption("wall");
	std::vector< hybridsim::Transition>::iterator itTr = vec.begin();
	std::vector<Position>::const_iterator itShp = shape.begin();
	SUMOReal sqrDist = itShp->distanceSquaredTo(vert2Pos(itTr->vert2()));
	std::cout<<"sqrDist" << sqrDist << std::endl;



	//		for (Position)


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

