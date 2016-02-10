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
}

const MSEdge* MSPRCPState::getEdge() const {
	//	const MSEdge * rt =
	return *myRoutePt;
}

MSPRCPState::~MSPRCPState() {
}

const MSEdge* MSPRCPState::getNextEdge() const {
	//	const MSEdge * rt =
	return *(myRoutePt+1);
}
