/*
 * MSPModelRemoteControlled.h
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
#ifndef SRC_MICROSIM_PEDESTRIANS_MSPMODELREMOTECONTROLLED_H_
#define SRC_MICROSIM_PEDESTRIANS_MSPMODELREMOTECONTROLLED_H_

#include <utils/common/SUMOTime.h>
#include <utils/common/Command.h>
#include "MSPModel.h"
#include "MSGRPCClient.h"

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class MSPModelRemoteControlled
 * @brief Interface class for remote pedestrian simulation
 *
 */
class MSPModelRemoteControlled : public MSPModel, Command {
public:
	MSPModelRemoteControlled(const OptionsCont& oc, MSNet* net);
	virtual ~MSPModelRemoteControlled();

	/// @brief register the given person as a pedestrian
	PedestrianState* add(MSPerson* person, MSPerson::MSPersonStage_Walking* stage, SUMOTime now);

	/// @brief whether a pedestrian is blocking the crossing of lane at offset distToCrossing
	bool blockedAtDist(const MSLane* lane, SUMOReal distToCrossing, std::vector<const MSPerson*>* collectBlockers);

	SUMOTime execute(SUMOTime currentTime);

private:
	class PState : public PedestrianState {
	public:
		PState(MSPerson * person): myPerson(person){};

		/// @brief abstract methods inherited from PedestrianState
		/// @{
		SUMOReal getEdgePos(const MSPerson::MSPersonStage_Walking& stage, SUMOTime now) const {return myEdgePos;};
		Position getPosition(const MSPerson::MSPersonStage_Walking& stage, SUMOTime now)const {return myPosition;};
		SUMOReal getAngle(const MSPerson::MSPersonStage_Walking& stage, SUMOTime now)const{return myAngle;};
		SUMOTime getWaitingTime(const MSPerson::MSPersonStage_Walking& stage, SUMOTime now)const{return 0.;};
		SUMOReal getSpeed(const MSPerson::MSPersonStage_Walking& stage)const{return mySpeed;};
		const MSEdge* getNextEdge(const MSPerson::MSPersonStage_Walking& stage)const{return myPerson->getNextEdgePtr();};
		//		/// @}
		//
		//		/// @brief compute walking time on edge and update state members
		SUMOTime computeWalkingTime(const MSEdge* prev, const MSPerson::MSPersonStage_Walking& stage, SUMOTime currentTime){return 0.;};
	private:
		SUMOReal myEdgePos;
		Position myPosition;
		SUMOReal myAngle;
		SUMOReal mySpeed;
		MSPerson * myPerson;
	};

	private:
	MSGRPCClient * grpcClient;
	MSNet* myNet;
};

#endif /* SRC_MICROSIM_PEDESTRIANS_MSPMODELREMOTECONTROLLED_H_ */
