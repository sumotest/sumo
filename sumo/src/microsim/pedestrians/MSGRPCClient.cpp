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

MSGRPCClient::MSGRPCClient(std::shared_ptr<Channel> channel) :
	stub_(noninteracting::PBPState::NewStub(channel)),
	hybridsimStub(hybridsim::HybridSimulation::NewStub(channel))
{

}

MSGRPCClient::~MSGRPCClient() {

}

MSGRPCClient::CmpWlkTmStruct MSGRPCClient::computeWalkingTime(const MSEdge * prev, const MSPerson::MSPersonStage_Walking & stage,const SUMOTime currentTime){


	noninteracting::CMPWlkgTm request;
	if (prev != 0){
		noninteracting::PBEdge * edge = request.mutable_prev();
		edge->set_length(prev->getLength());
		edge->set_fromjunctionid(prev->getFromJunction()->getID());
		edge->set_tojunctionid(prev->getToJunction()->getID());

	}

	noninteracting::PBEdge * stEdge = request.mutable_stage()->mutable_edge();
	stEdge->set_length(stage.getEdge()->getLength());
	stEdge->set_fromjunctionid(stage.getEdge()->getFromJunction()->getID());
	stEdge->set_tojunctionid(stage.getEdge()->getToJunction()->getID());


	if (stage.getNextRouteEdge() != 0) {
		noninteracting::PBEdge * nxtStEdge = request.mutable_stage()->mutable_nextrouteedge();
		nxtStEdge->set_length(stage.getNextRouteEdge()->getLength());
		nxtStEdge->set_fromjunctionid((stage.getNextRouteEdge()->getFromJunction()->getID()));
		nxtStEdge->set_tojunctionid(stage.getNextRouteEdge()->getToJunction()->getID());
	}
	noninteracting::PBMSPersonStage_Walking * st = request.mutable_stage();

	st->set_departpos(stage.getDepartPos());
	st->set_arrivalpos(stage.getArrivalPos());
	st->set_maxspeed(stage.getMaxSpeed());

	noninteracting::PBSUMOTime * time = request.mutable_sumotime();
	time->set_sumotime(currentTime);

	noninteracting::CMPWlkgTmRpl reply;

	ClientContext context;

	Status status = stub_->computeWalkingTime(&context,request, &reply);


		CmpWlkTmStruct ret;
	if (status.ok()){
//		SUMOTime ret = MAX2((SUMOTime)1, TIME2STEPS(replay.sumotime()));
		ret.wlkTm = MAX2((SUMOTime)1, TIME2STEPS(reply.duration().sumotime()));;
		ret.currentBeginPos = reply.mycurrentbeginpos();
		ret.currentEndPos = reply.mycurrentendpos();
		ret.lastEntrTm = reply.mylastentrytime();
		return  ret;
	} else {
		std::cerr << "something went wrong!" << std::endl;
		return ret;
	}


}

SUMOReal MSGRPCClient::getEdgePos(SUMOReal myCurrentBeginPos, SUMOReal myCurrentEndPos, SUMOReal myCurrentDuration, SUMOTime myLastEntryTime, SUMOTime now){
	noninteracting::GetEdgePos req;
	req.mutable_time()->set_sumotime(now);
	req.set_mycurrentbeginpos(myCurrentBeginPos);
	req.set_mycurrentendpos(myCurrentEndPos);
	req.set_mycurrentduration(myCurrentDuration);
	req.mutable_mylastentrytime()->set_sumotime(myLastEntryTime);

	noninteracting::PBSUMOReal rpl;
	ClientContext context;

	Status status = stub_->getEdgePost(&context,req,&rpl);

	if (status.ok()) {
		return rpl.sumoreal();
	} else{
		std::cerr << "something went wrong!" << std::endl;
		return 0;
	}

}

SUMOTime MSGRPCClient::getWaitingTime(){
	noninteracting::PBSUMOTime req;
	noninteracting::PBSUMOTime rpl;
	ClientContext context;

	Status status = stub_->getWaitingTime(&context,req,&rpl);

	if (status.ok()) {
		return 0;
	} else {
		std::cerr << "something went wrong!" << std::endl;
		return -1;
	}
}
SUMOReal MSGRPCClient::getMaxSpeed(const MSPerson::MSPersonStage_Walking& stage){
	noninteracting::PBMSPersonStage_Walking req;
	req.set_maxspeed(stage.getMaxSpeed());

	noninteracting::PBSUMOReal rpl;
	ClientContext context;
	Status status = stub_->getSpeed(&context,req,&rpl);

	if (status.ok()){
		return rpl.sumoreal();
	} else {
		std::cerr << "something went wrong!" << std::endl;
		return 0;
	}
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
