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
		edge.set_fromjunctionid(prev->getFromJunction()->getID());
		edge.set_tojunctionid(prev->getToJunction()->getID());
		request.set_allocated_prev(&edge);
	} else {
//		noninteracting::PBEdge edge;
//		edge.set_length(1.);
//		edge.set_fromjunctionid("a");
//		edge.set_tojunctionid("b");
//		request.set_allocated_prev(&edge);
	}



	noninteracting::PBEdge stEdge;
	stEdge.set_length(stage.getEdge()->getLength());
	stEdge.set_fromjunctionid(stage.getEdge()->getFromJunction()->getID());
	stEdge.set_tojunctionid(stage.getEdge()->getToJunction()->getID());


	//	if (stage.getNextRouteEdge() != 0) {
	noninteracting::PBEdge nxtStEdge;
	nxtStEdge.set_length(stage.getNextRouteEdge()->getLength());
	nxtStEdge.set_fromjunctionid((stage.getNextRouteEdge()->getFromJunction()->getID()));
	nxtStEdge.set_tojunctionid(stage.getNextRouteEdge()->getToJunction()->getID());
	noninteracting::PBMSPersonStage_Walking st;
	st.set_allocated_edge(&stEdge);
	st.set_allocated_nextrouteedge(&nxtStEdge);
	//	} else {
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
		SUMOTime ret = MAX2((SUMOTime)1, TIME2STEPS(replay.sumotime()));
		std::cout << ret << std::endl;

		return  ret;
	} else {
		std::cerr << "something went wrong!" << std::endl;
		return 0;
	}
}

