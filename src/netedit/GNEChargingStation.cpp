/****************************************************************************/
/// @file    GNEChargingStation.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Nov 2015
/// @version $Id: $
///
/// A class for visualizing chargingStation geometry (adapted from GUILaneWrapper)
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
// Copyright (C) 2001-2013 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/

// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include "GNEChargingStation.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif

// ===========================================================================
// static member definitions
// ===========================================================================


// ===========================================================================
// member method definitions
// ===========================================================================


GNEChargingStation::GNEChargingStation(GNELane& lane) :
	GNEAttributeCarrier(SUMO_TAG_CHARGING_STATION),	
	myParentLane(lane){

}

GNEChargingStation::~GNEChargingStation() {
}

GNELane &GNEChargingStation::getLane() const {
	return myParentLane;
}

SUMOReal GNEChargingStation::getBeginLanePosition() const {
	return begin;
}
		
SUMOReal GNEChargingStation::getEndLanePosition() const {
	return end;
}

/****************************************************************************/
