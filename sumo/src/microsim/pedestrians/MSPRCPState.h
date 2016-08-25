/*
 * MSPRCPState.h
 *
 *  Created on: Feb 8, 2016
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
#ifndef SRC_MICROSIM_PEDESTRIANS_MSPRCPState_H_
#define SRC_MICROSIM_PEDESTRIANS_MSPRCPState_H_


#include <queue>
#include <vector>
#include <utils/common/SUMOTime.h>
#include <utils/common/Command.h>
#include <microsim/MSEdge.h>
#include "MSPModel.h"


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class MSPRCPState
 *
 */
class MSPRCPState : public PedestrianState {
	public:
		MSPRCPState(MSPerson * person,MSPerson::MSPersonStage_Walking* stage);//: myPerson(person), myRoutePt(rp){};
		virtual ~MSPRCPState();

		/// @brief abstract methods inherited from PedestrianState
		/// @{
		inline SUMOReal getEdgePos(const MSPerson::MSPersonStage_Walking& stage, SUMOTime now) const {return myEdgePos;};
		inline Position getPosition(const MSPerson::MSPersonStage_Walking& stage, SUMOTime now)const {return myPosition;};
		inline SUMOReal getAngle(const MSPerson::MSPersonStage_Walking& stage, SUMOTime now)const{return myAngle;};
		inline SUMOTime getWaitingTime(const MSPerson::MSPersonStage_Walking& stage, SUMOTime now)const{return 0.;};
		inline SUMOReal getSpeed(const MSPerson::MSPersonStage_Walking& stage)const{return mySpeed;};
		const MSEdge* getEdge()const;// const {const MSEdge * rt = *myRoutePt;return rt;};
		const MSEdge* getNextEdge()const; //const{const MSEdge * rt = *(myRoutePt+1); return rt;};
		//		/// @}
		//
		//		/// @brief compute walking time on edge and update state members
		inline SUMOTime computeWalkingTime(const MSEdge* prev, const MSPerson::MSPersonStage_Walking& stage, SUMOTime currentTime){return 0.;};


	    /// @brief return the length of the pedestrian
	    SUMOReal getLength() const;

		void setXY(const double x, const double y) {myPosition.set(x,y);};
		void setSpeed(const double spd) {mySpeed = spd;};
		void setAngle(const double angle) {myAngle = angle;};
		const MSEdge* incrEdge();
		const MSEdge* updateEdge(const int edgeId);

		int getCurrentEdgeNumericalID();
		void setCurrentEdgeNumericalID(int numericalID);

		void setV(double vx, double vy) {myVelocity.set(vx,vy);};
		const Position getVelocity() {return myVelocity;};
		const Position getPosition() {return myPosition;};


	private:
		SUMOReal myEdgePos;
		Position myPosition;
		SUMOReal myAngle;
		SUMOReal mySpeed;
		Position myVelocity;
		MSPerson * myPerson;
		MSPerson::MSPersonStage_Walking* myStage;
		std::vector<const MSEdge*>::const_iterator myRoutePt;

		int myCurrentEdgeNumericalID;


	public:
	MSPerson * getPerson()  {
		return myPerson;
	}
	MSPerson::MSPersonStage_Walking* getMyStage() {
		return myStage;
	}


	;
};

//inline const MSEdge* MSPRCPState::getEdge() const {
//	const MSEdge * rt = *myRoutePt;
//	return rt;
//}
//
//inline const MSEdge* MSPRCPState::getNextEdge() const {
//	const MSEdge * rt = *(myRoutePt+1);
//	return rt;
//}

#endif
