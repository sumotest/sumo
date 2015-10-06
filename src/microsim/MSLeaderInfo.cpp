/****************************************************************************/
/// @file    MSLeaderInfo.cpp
/// @author  Jakob Erdmann
/// @date    Oct 2015
/// @version $Id: MSLeaderInfo.cpp 18663 2015-08-19 13:02:24Z namdre $
///
// Information about vehicles ahead (may be multiple vehicles if
// lateral-resolution is active)
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2002-2015 DLR (http://www.dlr.de/) and contributors
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
#include <math.h>
#include <microsim/MSGlobals.h>
#include <microsim/MSVehicle.h>
#include "MSLeaderInfo.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// static member variables
// ===========================================================================


// ===========================================================================
// member method definitions
// ===========================================================================
MSLeaderInfo::MSLeaderInfo(SUMOReal width) : 
    myWidth(width),
    myVehicles(MAX2(1, int(ceil(width / MSGlobals::gLateralResolution))), (MSVehicle*)0),
    myFreeSublanes((int)myVehicles.size())
{ }


MSLeaderInfo::~MSLeaderInfo() { }


int 
MSLeaderInfo::addLeader(const MSVehicle* veh, bool beyond) {
    if (veh == 0) {
        return myFreeSublanes;
    }
    if (myVehicles.size() == 1) {
        // speedup for the simple case
        if (!beyond || myVehicles[0] == 0) {
            myVehicles[0] = veh;
            myFreeSublanes = 0;
        }
        return myFreeSublanes;
    } 
    // map center-line based coordinates into [0, myWidth] coordinates
    const SUMOReal vehCenter = veh->getLateralPositionOnLane() + 0.5 * myWidth;
    const SUMOReal vehHalfWidth = 0.5 * veh->getVehicleType().getWidth();
    const SUMOReal rightVehSide = MAX2((SUMOReal)0,  vehCenter - vehHalfWidth);
    const SUMOReal leftVehSide = MIN2(myWidth, vehCenter + vehHalfWidth);
    for (SUMOReal posLat = rightVehSide; posLat < leftVehSide; posLat+= MSGlobals::gLateralResolution) {
        const int subLane = (int)floor(posLat / MSGlobals::gLateralResolution);
        if (!beyond || myVehicles[subLane] == 0) {
            myVehicles[subLane] = veh;
            myFreeSublanes--;
        }
    }
    return myFreeSublanes;
}


void 
MSLeaderInfo::getSubLanes(const MSVehicle* veh, int& rightmost, int& leftmost) const {
    if (myVehicles.size() == 1) {
        // speedup for the simple case
        rightmost = 0;
        leftmost = 0;
        return;
    }
    // map center-line based coordinates into [0, myWidth] coordinates
    const SUMOReal vehCenter = veh->getLateralPositionOnLane() + 0.5 * myWidth;
    const SUMOReal vehHalfWidth = 0.5 * veh->getVehicleType().getWidth();
    const SUMOReal rightVehSide = MAX2((SUMOReal)0,  vehCenter - vehHalfWidth);
    const SUMOReal leftVehSide = MIN2(myWidth, vehCenter + vehHalfWidth);
    rightmost = (int)floor(rightVehSide / MSGlobals::gLateralResolution);
    leftmost = MIN2((int)myVehicles.size() - 1,(int)floor(leftVehSide / MSGlobals::gLateralResolution));
}


const MSVehicle*
MSLeaderInfo::operator[](int sublane) const {
    assert(sublane >= 0);
    assert(sublane < myVehicles.size());
    return myVehicles[sublane];
}


/****************************************************************************/

