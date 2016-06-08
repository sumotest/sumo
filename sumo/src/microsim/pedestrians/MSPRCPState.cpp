/*
 * MSPRCPState.cpp
 *
 *  Created on: Feb 10, 2016
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


#include "MSPRCPState.h"


MSPRCPState::MSPRCPState(MSPerson* person,MSPerson::MSPersonStage_Walking* stage) : myPerson(person),
myStage(stage),
myPosition(0,0){
	myRoutePt = stage->getRoute().begin();
	myCurrentEdgeNumericalID = (*myRoutePt)->getNumericalID();
}

const MSEdge* MSPRCPState::getEdge() const {
	return *myRoutePt;
}

MSPRCPState::~MSPRCPState() {
}

const MSEdge* MSPRCPState::getNextEdge() const {

	return myRoutePt == myStage->getRoute().end()-1 ? 0 : *(myRoutePt+1);
}

const MSEdge* MSPRCPState::incrEdge() {
	if (myRoutePt == myStage->getRoute().end()-1) {
		return 0;
	}
	myRoutePt++;
	return *myRoutePt;
}

const MSEdge * MSPRCPState::updateEdge(const int edgeId) {

	std::vector<const MSEdge*>::const_iterator tmp = myRoutePt;
	for (;tmp < myStage->getRoute().end(); tmp++) {
		if ((*tmp)->getNumericalID() == edgeId){
			myRoutePt = tmp;
			return *myRoutePt;
		}
	}
	tmp = myRoutePt-1;

	for (; tmp >= myStage->getRoute().begin(); tmp--) {
		if ((*tmp)->getNumericalID() == edgeId){
			myRoutePt = tmp;
			return *myRoutePt;
		}
	}

	return 0;
}

int MSPRCPState::getCurrentEdgeNumericalID() {
	return myCurrentEdgeNumericalID;
}

double MSPRCPState::getLength() const {
	 return myPerson->getVehicleType().getLength();
}

void MSPRCPState::setCurrentEdgeNumericalID(int numericalID) {
	myCurrentEdgeNumericalID = numericalID;
}
