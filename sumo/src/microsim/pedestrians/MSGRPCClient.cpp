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

SUMOTime MSGRPCClient::computeWalkingTime(const MSEdge * prev, const MSPerson::MSPersonStage_Walking & stage, SUMOTime currentTime){
	noninteracting::CMPWlkgTm request;

	noninteracting::PBMSPersonStage_Walking st;
	st.set_maxspeed(stage.getMaxSpeed());
	std::cout << "setting speed to " << stage.getMaxSpeed() << std::endl;
	st.set_departpos(stage.getDepartPos());
	std::cout << "setting dpartpos to " << stage.getDepartPos() << std::endl;
	st.set_arrivalpos(stage.getArrivalPos());
	std::cout << "setting arrival pos to " << stage.getArrivalPos() << std::endl;

	noninteracting::PBEdge stEdge;

	stEdge.set_length((double)stage.getEdge()->getLength());
	std::cout << "setting length to " << stage.getEdge()->getLength() << std::endl;
	stEdge.set_fromjunctionid(stage.getEdge()->getFromJunction()->getID());
	std::cout << "setting fromjunctionid to " << stage.getEdge()->getFromJunction()->getID() << std::endl;
	stEdge.set_tojunctionid(stage.getEdge()->getToJunction()->getID());
	std::cout << "setting tojunctionid to " << stage.getEdge()->getToJunction()->getID() << std::endl;
	st.set_allocated_edge(&stEdge);

	noninteracting::PBEdge nxtStEdge;
	nxtStEdge.set_length((double)stage.getNextRouteEdge()->getLength());
	std::cout << "setting length to " << stage.getNextRouteEdge()->getLength() << std::endl;
	nxtStEdge.set_fromjunctionid(stage.getNextRouteEdge()->getFromJunction()->getID());
	std::cout << "setting fromjunctionid to " << stage.getNextRouteEdge()->getFromJunction()->getID() << std::endl;
	nxtStEdge.set_tojunctionid(stage.getNextRouteEdge()->getToJunction()->getID());
	std::cout << "setting tojunctionid to " << stage.getNextRouteEdge()->getToJunction()->getID() << std::endl;
	st.set_allocated_nextrouteedge(&nxtStEdge);

	noninteracting::PBEdge edge;
	//edge.set_length((double)prev->getLength());
	edge.set_length(1.0);
//	edge.set_fromjunctionid(prev->getFromJunction()->getID());
	edge.set_fromjunctionid("from");
//	edge.set_tojunctionid(prev->getToJunction()->getID());
	edge.set_tojunctionid("to");
	request.set_allocated_prev(&edge);

	noninteracting::PBSUMOTime replay;


	ClientContext context;

	Status status = stub_->computeWalkingTime(&context,request, &replay);

	if (status.ok()){
		return  MAX2((SUMOTime)1, TIME2STEPS(replay.sumotime()));
	} else {
		std::cerr << "something went wrong!" << std::endl;
		return 0;
	}
}

