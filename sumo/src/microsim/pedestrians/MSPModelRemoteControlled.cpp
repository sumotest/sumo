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
#include <utils/options/OptionsCont.h>
#include "MSPModelRemoteControlled.h"

//#define DEBUG1 1

//params
const SUMOReal MSPModelRemoteControlled::BLOCKER_LOOKAHEAD(10.0); // meters
const int FWD(1);
const int BWD(-1);

MSPModelRemoteControlled::MSPModelRemoteControlled(const OptionsCont& oc,
		MSNet* net) :
		myNet(net){
	const std::string address = oc.getString("pedestrian.remote.address");
	grpcClient = new MSGRPCClient(grpc::CreateChannel(address, grpc::InsecureChannelCredentials()), net);

	Event * e = new Event(this);
	net->getBeginOfTimestepEvents()->addEvent(e,net->getCurrentTimeStep() + DELTA_T, MSEventControl::ADAPT_AFTER_EXECUTION);
}

MSPModelRemoteControlled::~MSPModelRemoteControlled() {
	grpcClient->~MSGRPCClient();
}


PedestrianState* MSPModelRemoteControlled::add(MSPerson* person,
		MSPerson::MSPersonStage_Walking* stage, SUMOTime now) {


	assert(person->getCurrentStageType() == MSTransportable::MOVING_WITHOUT_VEHICLE);

	MSPRCPState * state = new MSPRCPState(person,stage);
	const MSEdge* edge = *(stage->getRoute().begin());;
	if (buffers.find(edge->getID()) == buffers.end()) {
		std::queue<MSPRCPState*> buffer;
		buffer.push(state);


		std::string str =  edge->getID();
		buffers[edge->getID()] = buffer;
	} else {
		std::queue<MSPRCPState*> * buffer = &buffers[edge->getID()];
		buffer->push(state);
	}

	pstates[person->getID()] = state;

	return state;
}

bool MSPModelRemoteControlled::blockedAtDist(const MSLane* lane,
		double distToCrossing, std::vector<const MSPerson*>* collectBlockers) {

//	assert(lane->getEdge().isCrossing());

#ifdef DEBUG1
	std::cout << lane->getEdge().getID() << std::endl;
#endif
	const std::set<MSPRCPState*> pedestrians = grpcClient->getPedestrians(lane);

	if (lane->getEdge().isCrossing() && pedestrians.size() > 0) {

		const PositionVector shape = lane->getShape();
		double dx = shape[1].x()-shape[0].x();
		double dy = shape[1].y()-shape[0].y();

		Position p0 = Position(dy,-dx);

		for (MSPRCPState * ped : pedestrians) {

#ifdef DEBUG1
		std::cout << "MSPModelRemoteControlled::blockedAtDist pers:" << ped->getPerson()->getID() << std::endl;
#endif

			//			Position p1 = Position(ped->ge)
			Position cross = p0.crossProduct(ped->getVelocity());

			int dir = cross.z() > 0 ? FWD : BWD;

			double relX = shape.nearest_offset_to_point2D(ped->getPosition(),false);

			const SUMOReal halfVehicleWidth = 1.0; // @note could get the actual value from the vehicle
			const SUMOReal leaderBackDist = (dir == FWD
					? distToCrossing - (relX- ped->getLength() - MSPModel::SAFETY_GAP - halfVehicleWidth)
							: (relX + ped->getLength() + MSPModel::SAFETY_GAP + halfVehicleWidth) - distToCrossing);
			//std::cout << SIMTIME << " foe=" << foeLane->getID() << " dir=" << p.myDir << " pX=" << ped.myRelX << " pL=" << ped.getLength() << " fDTC=" << distToCrossing << " lBD=" << leaderBackDist << "\n";
			if (leaderBackDist >= 0 && leaderBackDist <= BLOCKER_LOOKAHEAD) {
				// found one pedestrian that is not completely past the crossing point
				//std::cout << SIMTIME << " blocking pedestrian foeLane=" << lane->getID() << " ped=" << ped.myPerson->getID() << " dir=" << ped.myDir << " pX=" << ped.myRelX << " pL=" << ped.getLength() << " fDTC=" << distToCrossing << " lBD=" << leaderBackDist << "\n";
				if (collectBlockers == 0) {
					return true;
				} else {
					collectBlockers->push_back(ped->getPerson());
				}
			}
		}
	}
	if (collectBlockers == 0) {
		return false;
	} else {
		return collectBlockers->size() > 0;
	}
	//	return pedestrians.size() > 0;
}

SUMOTime MSPModelRemoteControlled::execute(SUMOTime currentTime) {

	//do all the handling here

	//1. transfer agents as long as there is space (SUMO --> external sim)
	std::map<const std::string,std::queue<MSPRCPState*>>::iterator it = buffers.begin();
	while(it != buffers.end()) {
		std::queue<MSPRCPState*> buffer = (*it).second;
		handleBuffer(&buffer);
		if (buffer.empty()){
			it = buffers.erase(it);
		} else {
			it++;
		}
	}

	//2. simulate for one SUMO time step
	grpcClient->simulateTimeInterval(currentTime,currentTime+DELTA_T);

	//3. receive events, trajectories ...
	grpcClient->receiveTrajectories(pstates, currentTime);

	//4. transfer agents as long as there is space (external sim --> SUMO)
	grpcClient->retrieveAgents(pstates,myNet,currentTime);

	return DELTA_T;
}

void MSPModelRemoteControlled::handleBuffer(std::queue<MSPRCPState*>* buffer) {
	//	if (buffer->size() > 1) {
	//		std::cout << "gt 1" << std::endl;
	//	}
	while(!buffer->empty()){
		MSPRCPState* st = buffer->front();
		if (transmitPedestrian(st)) {
			buffer->pop();
		} else {
			return;
		}
	}
}


bool MSPModelRemoteControlled::transmitPedestrian(MSPRCPState* st) {


	return grpcClient->transmitPedestrian(st);
}

