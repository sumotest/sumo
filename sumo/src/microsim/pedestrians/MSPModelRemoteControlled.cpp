/*
 * MSPModelRemoteControlled.cpp
 *
 *  Created on: Feb 4, 2016
 *      Author: laemmel
 */
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2014-2016 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#include "MSPModelRemoteControlled.h"


MSPModelRemoteControlled::MSPModelRemoteControlled(const OptionsCont& oc,
		MSNet* net) :
		myNet(net){
	grpcClient = new MSGRPCClient(grpc::CreateChannel("localhost:50051", grpc::InsecureChannelCredentials()));
	net->getBeginOfTimestepEvents()->addEvent(this,net->getCurrentTimeStep() + DELTA_T, MSEventControl::ADAPT_AFTER_EXECUTION);
}

MSPModelRemoteControlled::~MSPModelRemoteControlled() {

}

PedestrianState* MSPModelRemoteControlled::add(MSPerson* person,
		MSPerson::MSPersonStage_Walking* stage, SUMOTime now) {


	 assert(person->getCurrentStageType() == MSTransportable::MOVING_WITHOUT_VEHICLE);
	 std::cout << "MSPModelRemoteControlled::add was called" << std::endl;

	 PState * state = new PState(person);

	 return 0;
}

bool MSPModelRemoteControlled::blockedAtDist(const MSLane* lane,
		double distToCrossing, std::vector<const MSPerson*>* collectBlockers) {
	//for now no interaction btwn cars and peds
	return false;
}

SUMOTime MSPModelRemoteControlled::execute(SUMOTime currentTime) {

	std::cout << "tick" << std::endl;
	//do all the handling here

	//1. transfer agents as long there is space (SUMO --> external sim)

	//2. simulate for one SUMO time step
	grpcClient->simulateTimeInterval(currentTime,currentTime+DELTA_T);

	//3. receive agents as long SUMO has space ( external sim --> SUMO )

	//4. receive events, trajectories ...

	std::cout << "tock" << std::endl;
	return DELTA_T;
}

//double MSPModelRemoteControlled::PState::getEdgePos(
//		const MSPerson::MSPersonStage_Walking& stage, SUMOTime now) const {
//		return myEdgePos;
//}
//double MSPModelRemoteControlled::PState::getEdgePos(
//		const MSPerson::MSPersonStage_Walking& stage, SUMOTime now) {
//	return myEdgePos;
//}
//
//Position MSPModelRemoteControlled::PState::getPosition(
//		const MSPerson::MSPersonStage_Walking& stage, SUMOTime now) {
//	return myPosition;
//}
//
//double MSPModelRemoteControlled::PState::getAngle(
//		const MSPerson::MSPersonStage_Walking& stage, SUMOTime now) {
//	return myAngle;
//}
//
//SUMOTime MSPModelRemoteControlled::PState::getWaitingTime(
//		const MSPerson::MSPersonStage_Walking& stage, SUMOTime now) {
//	return 0.;
//}
//
//double MSPModelRemoteControlled::PState::getSpeed(
//		const MSPerson::MSPersonStage_Walking& stage) {
//	return mySpeed;
//}
//
//const MSEdge* MSPModelRemoteControlled::PState::getNextEdge(
//		const MSPerson::MSPersonStage_Walking& stage) {
//	return myPerson->getNextEdgePtr();
//}
