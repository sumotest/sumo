/****************************************************************************/
/// @file    MSChargingStation_h.h
/// @author  Daniel Krajzewicz
/// @author  Tamas Kurczveil
/// @author  Pablo Alvarez Lopez
/// @date    20-12-13
/// @version $Id: MSChargingStation.cpp 18762 2015-09-01 16:27:09Z behrisch $
///
// Chargin Station for Electric vehicles
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

// ===========================================================================
// included modules
// ===========================================================================

#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <cassert>
#include <utils/vehicle/SUMOVehicle.h>
#include <microsim/MSVehicleType.h>
#include <microsim/MSStoppingPlace.h>
#include "MSChargingStation.h"
#include "MSTrigger.h"


#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS

// ===========================================================================
// member method definitions
// ===========================================================================

MSChargingStation::MSChargingStation(const std::string& chargingStationID, MSLane& lane, SUMOReal startPos, SUMOReal endPos, 
	               SUMOReal chargingPower, SUMOReal efficency, bool chargeInTransit, SUMOReal chargeDelay) :
    MSStoppingPlace(chargingStationID, std::vector<std::string>(), lane, startPos, endPos),
    myChargingPower(chargingPower),
    myEfficiency(efficency),
    myChargeInTransit(chargeInTransit),
    myChargeDelay(chargeDelay) {
    if (getChargingPower() < 0) {
        std::cout << std::string("Warning: Charging Station with ID = " + getID() + " doesn't have a valid charging power(" + SUMOReal_str(getChargingPower()) + ").");
    }

    if (getEfficency() < 0 || getEfficency() > 1) {
        std::cout << std::string("Warning: Charging Station with ID = " + getID() + " doesn't have a valid myEfficiency (" + SUMOReal_str(getEfficency()) + ").");
    }

    if (getBeginLanePosition() > getEndLanePosition()) {
        std::cout << std::string("Warning: Charging Station with ID = " + getID() + " doesn't have a valid range (" + SUMOReal_str(getBeginLanePosition()) + " < " + SUMOReal_str(getEndLanePosition()) + ").");
    }
}


MSChargingStation::~MSChargingStation()
{}


// GET FUNCTIONS


SUMOReal
MSChargingStation::getChargingPower() const {
    return myChargingPower;
}

SUMOReal
MSChargingStation::getEfficency() const {
    return myEfficiency;
}

SUMOReal
MSChargingStation::getChargeInTransit() const {
    return myChargeInTransit;
}

SUMOReal
MSChargingStation::getChargeDelay() const {
    return myChargeDelay;
}


// SET FUNCTIONS

void MSChargingStation::setChrgPower(SUMOReal new_ChrgPower) {
    myChargingPower = new_ChrgPower;

    if (new_ChrgPower < 0) {
        std::cout << std::string("Warning: Chargin Station with ID = " + getID() + " doesn't have a valid charging power(" + SUMOReal_str(getChargingPower()) + ").");
    }
}


void MSChargingStation::setEfficency(SUMOReal new_Efficency) {
    if (new_Efficency < 0 || new_Efficency > 1) {
        std::cout << std::string("Warning: Chargin Station with ID = " + getID() + " doesn't have a valid myEfficiency (" + SUMOReal_str(getEfficency()) + ").");
    }

    myEfficiency = new_Efficency;
}

bool MSChargingStation::vehicleIsInside(const SUMOReal position) {
    if ((position >= getBeginLanePosition()) && (position <= getEndLanePosition())) {
        return true;
    } else {
        return false;
    }
}

// Private methods

std::string MSChargingStation::SUMOReal_str(const SUMOReal& var) {
    std::ostringstream convert;
    convert << var;
    return convert.str();
}
