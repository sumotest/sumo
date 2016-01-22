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
#include <noninteracting.grpc.pb.h>
#include <utils/common/StdDefs.h>
#include <microsim/MSJunction.h>
#include "MSGRPCClient.h"

MSGRPCClient::MSGRPCClient(std::shared_ptr<Channel> channel) : stub_(noninteracting::PBPState::NewStub(channel)) {


}

MSGRPCClient::~MSGRPCClient() {
	// TODO Auto-generated destructor stub
}

SUMOTime MSGRPCClient::computeWalkingTime(const MSEdge * prev, const MSPerson::MSPersonStage_Walking & stage,const SUMOTime currentTime){
	noninteracting::CMPWlkgTm request;



	if (prev != 0){
		noninteracting::PBEdge edge;
		edge.set_length(prev->getLength());
		std::string from =prev->getFromJunction()->getID();
		edge.set_allocated_fromjunctionid(&from);
		std::string to = prev->getToJunction()->getID();
		edge.set_allocated_tojunctionid(&to);
		request.set_allocated_prev(&edge);
	} else {
		//		noninteracting::PBEdge edge;
		//		edge.set_length(1.);
		//		edge.set_fromjunctionid("a");
		//		edge.set_tojunctionid("b");
		//		request.set_allocated_prev(&edge);
	}


	noninteracting::PBMSPersonStage_Walking st;

	noninteracting::PBEdge stEdge;
	stEdge.set_length(stage.getEdge()->getLength());
	std::string from = new std::string(prev->getFromJunction()->getID());
	stEdge.set_fromjunctionid(from);
	std::string to = new std::string(stage.getEdge()->getToJunction()->getID());
	stEdge.set_tojunctionid(to);
	st.set_allocated_edge(&stEdge);


	if (stage.getNextRouteEdge() != 0) {
		noninteracting::PBEdge nxtStEdge;
		nxtStEdge.set_length(stage.getNextRouteEdge()->getLength());
//		std::string from2 = stage.getNextRouteEdge()->getFromJunction()->getID();
		std::string from2 = "aaaa";
		nxtStEdge.set_fromjunctionid(from2);
//		std::string to2 =stage.getNextRouteEdge()->getToJunction()->getID();
		std::string to2 ="bbbb";
		nxtStEdge.set_tojunctionid(to2);
		st.set_allocated_nextrouteedge(&nxtStEdge);
	}
	//else {
	//		noninteracting::PBEdge df = noninteracting::PBEdge::default_instance();
	//		st.set_allocated_nextrouteedge(&df);
	//	}

	st.set_departpos(stage.getDepartPos());
	st.set_arrivalpos(stage.getArrivalPos());
	st.set_maxspeed(stage.getMaxSpeed());

	request.set_allocated_stage(&st);

	noninteracting::PBSUMOTime time;
	time.set_sumotime(currentTime);
	request.set_allocated_sumotime(&time);

	noninteracting::PBSUMOTime replay;


	ClientContext context;

	Status status = stub_->computeWalkingTime(&context,request, &replay);

	if (status.ok()){
		std::cout << MAX2((SUMOTime)1, TIME2STEPS(replay.sumotime())) << std::endl;
		return  MAX2((SUMOTime)1, TIME2STEPS(replay.sumotime()));
	} else {
		std::cerr << "something went wrong!" << std::endl;
		return 0;
	}
}

