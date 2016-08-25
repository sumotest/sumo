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
#include <map>
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

	std::set<MSPRCPState*> getPedestrians(const MSLane * lane);

private:

	std::map<const MSLane*,std::set<MSPRCPState*>> laneMapping;

	std::set<MSPRCPState*> emptySet;


	std::unique_ptr<hybridsim::HybridSimulation::Stub> hybridsimStub;
	MSNet* net;


	//	void extractEnterAndLeaveCoordinate(hybridsim::Coordinate * enterC, hybridsim::Coordinate * leaveC, MSPRCPState * st);
	void extractCoordinate(hybridsim::Coordinate *c,const MSLane * l, SUMOReal pos);
	//	int inSim = 0;

	void initalized();
	void encodeEnvironment(hybridsim::Environment* env);
	void generateGoals(hybridsim::Scenario * sc);
	void generateGoal(hybridsim::Scenario * sc, double & dx, double & dy, double & w, int gId,const Position pos);
	void generateGroupsAndSources(hybridsim::Scenario * sc);
	void generateGroupAndSource(hybridsim::Scenario * sc, MSLane * lane, double & x, double & y, double & w, int & gId);



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

private:
	static void CONFIGURE_STATIC_PARAMS_GOMPERTZ(hybridsim::Scenario * sc) {
		hybridsim::Model * m = sc->mutable_model();
		m->set_type(hybridsim::Model::Gompertz);
		hybridsim::Gompertz * gmp = m->mutable_gompertz();
		gmp->set_solver("euler");
		gmp->set_stepsize(0.04);
		gmp->set_exit_crossing_strategy(3);
		gmp->set_linked_cells_enabled(true);
		gmp->set_cell_size(2.2);
		hybridsim::Force * pf = gmp->mutable_force_ped();
		pf->set_nu(3);
		pf->set_b(0.25);
		pf->set_c(3);
		hybridsim::Force * wf = gmp->mutable_force_wall();
		wf->set_nu(10);
		wf->set_b(0.7);
		wf->set_c(3);

		hybridsim::AgentParams * ap = gmp->mutable_agent_params();
		hybridsim::Distribution * v0 = ap->mutable_v0();
		v0->set_mu(0.5);
		v0->set_sigma(0);
		hybridsim::Distribution * bMax = ap->mutable_b_max();
		bMax->set_mu(0.25);
		bMax->set_sigma(0.001);
		hybridsim::Distribution * bMin = ap->mutable_b_min();
		bMin->set_mu(0.2);
		bMin->set_sigma(0.001);
		hybridsim::Distribution * aMin = ap->mutable_a_min();
		aMin->set_mu(0.18);
		aMin->set_sigma(0.001);
		hybridsim::Distribution * tau = ap->mutable_tau();
		tau->set_mu(0.5);
		tau->set_sigma(0.001);
		hybridsim::Distribution * aTau = ap->mutable_atau();
		aTau->set_mu(0.5);
		aTau->set_sigma(0.001);

		hybridsim::Router * router = sc->add_router();
		router->set_router_id(1);
		router->set_description("global_shortest");

	}
	static void CONFIGURE_STATIC_PARAMS_TORDEUX2015(hybridsim::Scenario * sc) {
		hybridsim::Model * m = sc->mutable_model();
		m->set_type(hybridsim::Model::Tordeux2015);
		hybridsim::Tordeux2015 * gmp = m->mutable_tordeux2015();
		gmp->set_solver("euler");
		gmp->set_stepsize(0.1);
		gmp->set_exit_crossing_strategy(8);
		gmp->set_linked_cells_enabled(true);
		gmp->set_cell_size(30);
		gmp->set_periodic(0);
		hybridsim::Tordeux2015Force * pf = gmp->mutable_force_ped();
		pf->set_a(5);
		pf->set_d(0.1);
		hybridsim::Tordeux2015Force * wf = gmp->mutable_force_wall();
		wf->set_a(5);
		wf->set_d(0.02);
		hybridsim::AgentParams * ap = gmp->mutable_agent_params();
		hybridsim::Distribution * v0 = ap->mutable_v0();
		v0->set_mu(1.34);
		v0->set_sigma(0);
		hybridsim::Distribution * bMax = ap->mutable_b_max();
		bMax->set_mu(0.15);
		bMax->set_sigma(0.0);
		hybridsim::Distribution * bMin = ap->mutable_b_min();
		bMin->set_mu(0.15);
		bMin->set_sigma(0.0);
		hybridsim::Distribution * aMin = ap->mutable_a_min();
		aMin->set_mu(0.15);
		aMin->set_sigma(0.0);
		hybridsim::Distribution * tau = ap->mutable_tau();
		tau->set_mu(0.5);
		tau->set_sigma(0.0);
		hybridsim::Distribution * aTau = ap->mutable_atau();
		aTau->set_mu(0.0);
		aTau->set_sigma(0.0);
		hybridsim::Distribution * t = ap->mutable_t();
		t->set_mu(1.0);
		t->set_sigma(0.0);

		hybridsim::Router * router = sc->add_router();
		router->set_router_id(1);
		router->set_description("ff_global_shortest");

	}
};
#endif /*MSGRPC_CLIENT_H*/
