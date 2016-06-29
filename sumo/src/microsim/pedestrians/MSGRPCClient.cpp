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

#define DEBUG 0

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
	encodeEnvironment(env);
	generateGoals(&req);
	generateGroupsAndSources(&req);

	MSGRPCClient::CONFIGURE_STATIC_PARAMS(&req);


	hybridsim::Empty rpl;
	ClientContext context;
	Status st = hybridsimStub->initScenario(&context,req,&rpl);
	if(!st.ok()){
		std::cerr << "Remote side could not initialize scenario!" << std::endl;
		exit(-1);
	}
}

void MSGRPCClient::generateGroupsAndSources(hybridsim::Scenario * sc){
	int gId = -1;

	//1. for all edges
	for (MSEdge * e : net->getEdgeControl().getEdges()){
		if (e->isCrossing() || e->isInternal() || e->isWalkingArea()) {
			continue;
		}
		for (MSLane * l : e->getLanes()) {
			if (l->allowsVehicleClass(SUMOVehicleClass::SVC_PEDESTRIAN)) {
				const PositionVector v = l->getShape();
				double w = l->getWidth();
				double denom = v[0].distanceTo(v[1]);
				double dx1 = (v[1].x()-v[0].x())/denom;
				double dy1 = (v[1].y()-v[0].y())/denom;
				double x1 = v[0].x();//+dx1*w;
				double y1 = v[0].y();//+dy1*w;
				generateGroupAndSource(sc,l,x1,y1,w,++gId);

				denom = v[v.size()-2].distanceTo(v[v.size()-1]);
				double dx2 = (v[v.size()-2].x()-v[v.size()-1].x())/denom;
				double dy2 = (v[v.size()-2].y()-v[v.size()-1].y())/denom;
				double x2 = v[v.size()-1].x();//+dx2*w;
				double y2 = v[v.size()-1].y();//+dy2*w;
				generateGroupAndSource(sc,l,x2,y2,w,++gId);
			}
		}
	}
}

void MSGRPCClient::generateGroupAndSource(hybridsim::Scenario * sc, MSLane * lane, double & x, double & y, double & w, int &gId){
	hybridsim::Group * goal = sc->add_group();
	goal->set_group_id(gId);
	goal->set_room_id(lane->getEdge().getNumericalID());
	goal->set_subroom_id(lane->getNumericalID());
	goal->set_number(0);
	goal->set_router_id(1);


	//TODO: distributes agents in bounding box --> change to distribute agents in subroom; construct subrooms accordingly [gl Apr '16]
	hybridsim::Coordinate * mx = goal->mutable_max_x_y();
	mx->set_x(x+w);
	mx->set_y(y+w);
	hybridsim::Coordinate * mn = goal->mutable_min_x_y();
	mn->set_x(x-w);
	mn->set_y(y-w);
	hybridsim::Source * source = sc->add_source();
	source->set_id(gId);
	source->set_caption("SUMO generated source");
	source->set_frequency(5);//TODO check inpact of this parameter [gl Apr '16]
	source->set_max_agents(5);//TODO check inpact of this parameter [gl Apr '16]
	source->set_group_id(gId);


}

void MSGRPCClient::generateGoals(hybridsim::Scenario * sc) {

	int gId = -1;

	//1. for all edges
	for (MSEdge * e : net->getEdgeControl().getEdges()){
		if (e->isCrossing() || e->isInternal() || e->isWalkingArea()) {
			continue;
		}
		for (MSLane * l : e->getLanes()) {
			if (l->allowsVehicleClass(SUMOVehicleClass::SVC_PEDESTRIAN)) {
				const PositionVector v = l->getShape();
				double w = l->getWidth();

				double denom = v[0].distanceTo(v[1]);
				double dx1 = (v[1].x()-v[0].x())/denom;
				double dy1 = (v[1].y()-v[0].y())/denom;
				generateGoal(sc,dx1,dy1,w,++gId,v[0]);

				denom = v[v.size()-2].distanceTo(v[v.size()-1]);
				double dx2 = (v[v.size()-2].x()-v[v.size()-1].x())/denom;
				double dy2 = (v[v.size()-2].y()-v[v.size()-1].y())/denom;
				generateGoal(sc,dx2,dy2,w,++gId,v[v.size()-1]);
			}
		}
	}
}

void MSGRPCClient::generateGoal(hybridsim::Scenario * sc, double & dx, double & dy, double & w, int gId,const Position pos){
	hybridsim::Goal * g = sc->add_goal();
	g->set_id(gId);
	g->set_final(true); //TODO: check what effect this has [gl Apr '16]
	g->set_caption("SUMO generated");

	hybridsim::Polygon * p = g->mutable_p();
	hybridsim::Coordinate * c0 = p->add_coordinate();
	c0->set_x(pos.x()+dy*w/2.0+dx*w);
	c0->set_y(pos.y()-dx*w/2.0+dy*w);

	hybridsim::Coordinate * c1 = p->add_coordinate();
	c1->set_x(pos.x()+dy*w/2.0-dx*w);
	c1->set_y(pos.y()-dx*w/2.0-dy*w);

	hybridsim::Coordinate * c2 = p->add_coordinate();
	c2->set_x(pos.x()-dy*w/2.0-dx*w);
	c2->set_y(pos.y()+dx*w/2.0-dy*w);

	hybridsim::Coordinate * c3 = p->add_coordinate();
	c3->set_x(pos.x()-dy*w/2.0+dx*w);
	c3->set_y(pos.y()+dx*w/2.0+dy*w);

	hybridsim::Coordinate * c4 = p->add_coordinate();
	c4->set_x(pos.x()+dy*w/2.0+dx*w);
	c4->set_y(pos.y()-dx*w/2.0+dy*w);

}

void MSGRPCClient::encodeEnvironment(hybridsim::Environment* env) {
	int trId = 0;
	int hlId = -1;

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
					t1->set_caption("SUMO generated transition");
					t1->set_type("emergency");
					t1->set_room1_id(e->getNumericalID());
					t1->set_subroom1_id(l->getNumericalID());

					hybridsim::Transition * t2 = env->add_transition();
					t2->set_id(trId++);
					t2->set_caption("SUMO generated transition");
					t2->set_type("emergency");
					t2->set_room1_id(e->getNumericalID());
					t2->set_subroom1_id(l->getNumericalID());

					const std::vector<MSLane::IncomingLaneInfo,
					std::allocator<MSLane::IncomingLaneInfo> > incoming = l->getIncomingLanes();
					if (incoming.size() > 1) {
						std::cerr << "inconsistent network, cannot create jupedsim scenario!" << std::endl;
						exit(-1);
					}
					//TODO: check if walking_area && walking_area.area() == 0 --> set_room2_id(-1) && set_subroom2_id(-1)
					MSLane * nb1 = 0;
					if (incoming.size() > 0){
						nb1 = (incoming.begin())->lane;
					}


					if (!nb1 ||( nb1->getEdge().isWalkingArea() && nb1->getShape().area() <= EPSILON)) {//dead end
						t1->set_room2_id(-1);
						t1->set_subroom2_id(-1);
					} else {
						t1->set_room2_id(nb1->getEdge().getNumericalID());
						t1->set_subroom2_id(nb1->getNumericalID());
					}


					std::vector<const MSLane*>  outgoing = l->getOutgoingLanes();
					if (outgoing.size() > 1) {
						std::cerr << "inconsistent network, cannot create jupedsim scenario!" << std::endl;
						exit(-1);
					}
					const MSLane * nb2;
					bool notNb2 = true;
					if (outgoing.size() > 0) {
						nb2 = (*(outgoing.begin()));
						notNb2 = false;
					}
					if (notNb2 || (nb2->getEdge().isWalkingArea() && nb2->getShape().area() <= EPSILON)) {//dead end
						t2->set_room2_id(-1);
						t2->set_subroom2_id(-1);
					} else {
						t2->set_room2_id(nb2->getEdge().getNumericalID());
						t2->set_subroom2_id(nb2->getNumericalID());
					}

					PositionVector  s = PositionVector(l->getShape());
					double width = l->getWidth();
					s.move2side(-width/2);

					std::vector<hybridsim::Hline*> lines;
					for (int i = 1; i < s.size()-1; i++){
						hybridsim::Hline * hl = env->add_hline();
						hl->set_id(++hlId);
						hl->set_room_id(e->getNumericalID());
						hl->set_subroom_id(l->getNumericalID());
						hybridsim::Coordinate* vert1 = hl->mutable_vert1();
						vert1->set_x(s[i].x());
						vert1->set_y(s[i].y());
						lines.push_back(hl);
					}


					hybridsim::Polygon * p1 = subroom->add_polygon();
					p1->set_caption("SUMO generated wall");


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
					s.move2side(width);


					for (int i = 1; i < s.size()-1; i++){
						hybridsim::Hline * hl = lines.at(i-1);
						hybridsim::Coordinate* vert2 = hl->mutable_vert2();
						vert2->set_x(s[i].x());
						vert2->set_y(s[i].y());

					}
					hybridsim::Polygon * p2 = subroom->add_polygon();
					p2->set_caption("SUMO generated wall");


					for (int i = s.size()-1; i >= 0; i-- ) {
						Position p = s[i];
						hybridsim::Coordinate * c = p2->add_coordinate();
						c->set_x(p.x());
						c->set_y(p.y());
					}
					hybridsim::Coordinate * c11 = t1->mutable_vert1();
					c11->set_x((s.begin())->x());
					c11->set_y((s.begin())->y());

					hybridsim::Coordinate * c22 = t2->mutable_vert2();
					c22->set_x((s.end()-1)->x());
					c22->set_y((s.end()-1)->y());
					if (!notNb2 && (*outgoing.begin())->getEdge().isWalkingArea()) {
						std::vector<hybridsim::Transition> * vec = &(walkingAreasTransitionMapping[&((*outgoing.begin())->getEdge())]);
						vec->push_back(*t2);
					}
					if (nb1 && (incoming.begin())->lane->getEdge().isWalkingArea()) {
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
#ifdef DEBUG
		if (walkingAreasTransitionMapping.find(w) == walkingAreasTransitionMapping.end()) {
			std::cerr << "This should never happen! check and remove [gl apr '16]" << std::endl;
			continue;
		}
#endif
		MSLane * l = *(w->getLanes().begin());
		if (l->getShape().area() <= EPSILON) {//dead end
			continue;
		}


		std::vector<hybridsim::Transition> vec = walkingAreasTransitionMapping[w];
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
			tr->set_caption("SUMO generated transition");
			tr->set_type("emergency");

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
}

MSGRPCClient::~MSGRPCClient() {
	hybridsim::Empty req;
	hybridsim::Empty rpl;
	ClientContext context;
	Status st = hybridsimStub->shutdown(&context,req,&rpl);
	if(!st.ok()){
		std::cerr << "client shutdown went wrong!" << std::endl;
		exit(-1);
	}
}



void MSGRPCClient::createWalkingAreaSubroom(hybridsim::Subroom * subroom, const PositionVector shape,std::vector<hybridsim::Transition>& vec){
	//TODO: this needs to be handled upstream!
	if (shape.area() <= 0.) {//dbl precision issue?! [gl apr '16]
		return;
	}

	TransitionComparator comp = TransitionComparator(&(shape));
	std::sort(vec.begin(),vec.end(),comp);





	std::vector< hybridsim::Transition>::iterator itTr = vec.begin();
	do {


		hybridsim::Polygon * p = subroom->add_polygon();
		p->set_caption("SUMO generated wall");
		hybridsim::Coordinate * c = p->add_coordinate();
		c->set_x((*itTr).vert1().x());
		c->set_y((*itTr).vert1().y());
		*itTr++;

		c = p->add_coordinate();
		c->set_x((*itTr).vert2().x());
		c->set_y((*itTr).vert2().y());
	} while (itTr != vec.end()-1);



	hybridsim::Polygon * p = subroom->add_polygon();
	p->set_caption("SUMO generated wall");
	hybridsim::Coordinate * c = p->add_coordinate();
	c->set_x((*itTr).vert1().x());
	c->set_y((*itTr).vert1().y());

	c = p->add_coordinate();
	c->set_x((vec.begin())->vert2().x());
	c->set_y((vec.begin())->vert2().y());

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
		std::cerr << "Error simulating time interval on external simulation" << std::endl;
		exit(-1);
	}

	//	std::cout << "in JuPedSim:" << inSim << std::endl;

}


void MSGRPCClient::extractCoordinate(hybridsim::Coordinate *c,const MSLane * l, SUMOReal pos) {
	Position p = l->getShape().positionAtOffset(l->interpolateLanePosToGeometryPos(pos),0.);
	c->set_y(p.y());
	c->set_x(p.x());
}

bool MSGRPCClient::transmitPedestrian(MSPRCPState* st) {
#ifdef DEBUG
	std::cout << "MSGRPCClient::transmitPedestrian" << std::endl;
#endif	

	std::string id = st->getPerson()->getID();
	std::string fromId = st->getEdge()->getID();
	std::string toId = (*(st->getMyStage()->getRoute().end()-1))->getID();



	hybridsim::Agent req;
	req.set_id(id);
	//	std::cout << "from" << fromId << " to" << toId << std::endl;
	hybridsim::Coordinate * enterC = req.mutable_enterlocation();
	hybridsim::Coordinate * leaveC = req.mutable_leavelocation();




	//TODO: this  is not how it works! derive coords from departure pos [gl Apr '16]
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
				Position p = ln->getShape().getLineCenter();
				hybridsim::Coordinate * cr = l->mutable_centroid();
				cr->set_x(p.x());
				cr->set_y(p.y());
				l->set_id(e->getNumericalID());
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
		std::cerr << "Error transferring pedestrian to external simulation" << std::endl;
		exit(-1);
	}

}

std::set<MSPRCPState*> MSGRPCClient::getPedestrians(const MSLane* lane) {

	auto ret = laneMapping[(lane)];
#ifdef DEBUG1
	std::cout << "**************************************************" << std::endl;
	std::cout << "queried edge:" << lane->getEdge().getID() << " queried lane:" << lane->getID() << " pedestrians:" << ret.size() << std::endl;
	std::cout << "**************************************************" << std::endl;
#endif
	return ret;
}

void MSGRPCClient::receiveTrajectories(std::map<const std::string,MSPRCPState*>& pstates,SUMOTime time) {

#ifdef DEBUG
	std::cout << "MSGRPCClient::receiveTrajectories" << std::endl;
#endif
	hybridsim::Empty req;
	hybridsim::Trajectories rpl;
	ClientContext context;
	Status st = hybridsimStub->receiveTrajectories(&context,req,&rpl);
	if (st.ok()){
		for (int i = 0; i < rpl.trajectories_size(); i++) {
			const hybridsim::Trajectory t = rpl.trajectories(i);

			if (pstates.find(t.id()) == pstates.end()) {
				std::cerr << "Error receiving pedestrian trajectories from external simulation" << std::endl;
				exit(-1);
			}
			MSPRCPState* st = pstates[t.id()];
			st->setXY(t.x(),t.y());
			st->setSpeed(t.spd());
			st->setAngle(t.phi());
			st->setV(t.vx(),t.vy());


			//			net->getEdgeControl().getEdges()[]

			//			if (!oldEdge) {
			//				continue;
			//			}

			if (t.linkid() != st->getCurrentEdgeNumericalID()) {
				const MSEdge * last = net->getEdgeControl().getEdges()[st->getCurrentEdgeNumericalID()];
				const MSEdge * current = net->getEdgeControl().getEdges()[t.linkid()];
				for (const MSLane * ln : last->getLanes()) {
					if (ln->allowsVehicleClass(SUMOVehicleClass::SVC_PEDESTRIAN)) {
						//unmap ped from lane
						std::set<MSPRCPState*> * set = &laneMapping[ln];
#ifdef DEBUG
						std::cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << std::endl;
						std::cout << set->size() << std::endl;
#endif
						set->erase(st);
#ifdef DEBUG
						std::cout << set->size() << std::endl;
						std::cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << std::endl;
#endif
						break;
					}
				}


				for (const MSLane * ln : current->getLanes()) {
					if (ln->allowsVehicleClass(SUMOVehicleClass::SVC_PEDESTRIAN)) {
#ifdef DEBUG
						std::cout << "+++++++++++++++++++++++++++++++" << std::endl;
						std::cout << "ped: " << t.id() <<" mapped to lane: " << ln->getID() << " on edge: " << current->getID() << std::endl;
#endif
						std::set<MSPRCPState*> * set = &laneMapping[ln];
						set->insert(st);
						break;
					}
				}
				st->setCurrentEdgeNumericalID(current->getNumericalID());



				const MSEdge * oldEdge = st->getEdge();
				if (oldEdge->getNumericalID() != t.linkid()) {
					const MSEdge * newEdge = st->updateEdge(t.linkid());
					if (newEdge != 0) {
						st->getMyStage()->moveToNextEdge(st->getPerson(),time,oldEdge,newEdge);

					}
				}
			}
		}
	} else {
		std::cerr << "Error receiving pedestrian trajectories from external simulation" << std::endl;
		exit(-1);
	}
}

void MSGRPCClient::retrieveAgents(std::map<const std::string, MSPRCPState*>& pstates,MSNet* net, SUMOTime time) {

#ifdef DEBUG
	std::cout << "MSGRPCClient::retrieveAgents" << std::endl;
#endif
	hybridsim::Empty req;
	hybridsim::Agents rpl;
	ClientContext context;

	Status st = hybridsimStub->retrieveAgents(&context,req,&rpl);
	if (st.ok()) {
#ifdef DEBUG
	std::cout << "MSGRPCClient::retrieveAgents st.ok() nr:" << rpl.agents_size() << std::endl;
#endif
		for (int i = 0; i < rpl.agents_size(); i++) {
			const hybridsim::Agent a = rpl.agents(i);
			MSPRCPState* st = pstates[a.id()];
				for (const MSLane * ln : st->getEdge()->getLanes()) {
					if (ln->allowsVehicleClass(SUMOVehicleClass::SVC_PEDESTRIAN)) {
						//unmap ped from lane
						std::set<MSPRCPState*> * set = &laneMapping[ln];
#ifdef DEBUG
						std::cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << std::endl;
						std::cout << set->size() << std::endl;
#endif
						set->erase(st);
#ifdef DEBUG
						std::cout << set->size() << std::endl;
						std::cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << std::endl;
#endif
						break;
					}
				}
			//std::set<MSPRCPState*> * set = &laneMapping[ln];
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
		std::cerr << "Error retrieving pedestrians from external simulation" << std::endl;
		exit(-1);
	}
#ifdef DEBUG
	std::cout << "MSGRPCClient::retrieveAgents done." << std::endl;
#endif
}

