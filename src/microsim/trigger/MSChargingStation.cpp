/****************************************************************************/
/// @file    MSChargingStation.h
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
#include <utils/common/TplConvert.h>
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
	               SUMOReal chargingPower, SUMOReal efficency, bool chargeInTransit, int chargeDelay) :
    MSStoppingPlace(chargingStationID, std::vector<std::string>(), lane, startPos, endPos),
    myChargingPower(0),
    myEfficiency(0),
    myChargeInTransit(chargeInTransit),
    myChargeDelay(0) {
    if (chargingPower < 0)
        WRITE_WARNING("Charging Station with ID = " + getID() + " doesn't have a valid charging power (" + TplConvert::_SUMOReal2str(getChargingPower()) + ").")
    else
        myChargingPower = chargingPower;

    if (efficency < 0 || efficency > 1)
        WRITE_WARNING("Charging Station with ID = " + getID() + " doesn't have a valid efficiency (" + TplConvert::_SUMOReal2str(getEfficency()) + ").")
    else
        myEfficiency = efficency;

    if (chargeDelay < 0)
        WRITE_WARNING("Charging Station with ID = " + getID() + " doesn't have a valid charge delay (" + TplConvert::_SUMOReal2str(getEfficency()) + ").")
    else
        myChargeDelay = chargeDelay;

    if (getBeginLanePosition() > getEndLanePosition())
        WRITE_WARNING("Charging Station with ID = " + getID() + " doesn't have a valid range (" + TplConvert::_SUMOReal2str(getBeginLanePosition()) + " < " + TplConvert::_SUMOReal2str(getEndLanePosition()) + ").");
}


MSChargingStation::~MSChargingStation()
{}


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


void 
MSChargingStation::setChargingPower(SUMOReal chargingPower) {
    if (chargingPower < 0)
        WRITE_WARNING("new charging power for Chargin Station with ID = " + getID() + " not valid (" + TplConvert::_SUMOReal2str(chargingPower) + ").")
    else
        myChargingPower = chargingPower;
}


void 
MSChargingStation::setEfficency(SUMOReal efficency) {
    if (efficency < 0 || efficency > 1)
        WRITE_WARNING("new efficiency for Chargin Station with ID = " + getID() + " not valid (" + TplConvert::_SUMOReal2str(efficency) + ").")
    else
        myEfficiency = efficency;
}


void
MSChargingStation::setChargeInTransit(bool chargeInTransit) {
        myChargeInTransit = chargeInTransit;
}


void
MSChargingStation::setChargeDelay(int chargeDelay) {
    if (chargeDelay < 0)
        WRITE_WARNING("new charge delay for Chargin Station with ID = " + getID() + " not valid (" + TplConvert::_SUMOReal2str(chargeDelay) + ").")
    else
        myChargeDelay = chargeDelay;
}


bool 
MSChargingStation::vehicleIsInside(const SUMOReal position) {
    if ((position >= getBeginLanePosition()) && (position <= getEndLanePosition()))
        return true;
    else
        return false;
}