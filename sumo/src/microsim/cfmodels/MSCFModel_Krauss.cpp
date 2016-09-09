/****************************************************************************/
/// @file    MSCFModel_Krauss.cpp
/// @author  Tobias Mayer
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @author  Laura Bieker
/// @date    Mon, 04 Aug 2009
/// @version $Id$
///
// Krauss car-following model, with acceleration decrease and faster start
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2016 DLR (http://www.dlr.de/) and contributors
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

#include <microsim/MSVehicle.h>
#include <microsim/MSLane.h>
#include <microsim/MSGlobals.h>
#include "MSCFModel_Krauss.h"
#include <microsim/lcmodels/MSAbstractLaneChangeModel.h>
#include <utils/common/RandHelper.h>

// ===========================================================================
// method definitions
// ===========================================================================
MSCFModel_Krauss::MSCFModel_Krauss(const MSVehicleType* vtype, SUMOReal accel, SUMOReal decel,
                                   SUMOReal dawdle, SUMOReal headwayTime)
    : MSCFModel_KraussOrig1(vtype, accel, decel, dawdle, headwayTime) {
}


MSCFModel_Krauss::~MSCFModel_Krauss() {}


SUMOReal
MSCFModel_Krauss::stopSpeed(const MSVehicle* const veh, const SUMOReal speed, SUMOReal gap) const {
    return MIN2(maximumSafeStopSpeed(gap, speed, false, 0.), maxNextSpeed(speed, veh)); // NOTE: allows return of smaller values than minNextSpeed()
}


SUMOReal
MSCFModel_Krauss::followSpeed(const MSVehicle* const veh, SUMOReal speed, SUMOReal gap, SUMOReal predSpeed, SUMOReal predMaxDecel) const {
//
//	// Debug (Leo)
//	if(veh->getID() == "flow.2"){
//		MSGlobals::gSemiImplicitEulerUpdate = true;
//		SUMOReal maxSafeFollowSpeedEuler = maximumSafeFollowSpeed(gap, speed, predSpeed, predMaxDecel);
//		MSGlobals::gSemiImplicitEulerUpdate = false;
//		SUMOReal maxSafeFollowSpeedBallistic =  maximumSafeFollowSpeed(gap, 0., predSpeed, predMaxDecel);
//		std::cout << "\nin MSCFModel_Krauss::followSpeed(veh flow.2):"
//				<< "\nfollowSpeedEuler = " <<  maxSafeFollowSpeedEuler
//				<< "\nfollowSpeedBallistic = " <<  maxSafeFollowSpeedBallistic << std::endl;
//	}

	SUMOReal vsafe = maximumSafeFollowSpeed(gap, speed, predSpeed, predMaxDecel), vmin=minNextSpeed(speed), vmax =maxNextSpeed(speed, veh);

////	 Debug (Leo)
//	if(veh->getID() == "f.28"){
//		std::cout
//		<< " MSCF_Krauss::followSpeed()\n"
//		<< "vsafe = " << vsafe
//		<< " vmin = " << vmin
//		<< " vmax = " << vmax
//		<< "\n";
//	}
	if(MSGlobals::gSemiImplicitEulerUpdate){
		return MIN2(vsafe, vmax); // XXX: the euler variant can break as strong as it wishes -- immediately!
	} else {
		return MAX2(MIN2(vsafe, vmax), vmin);
	}
}


SUMOReal
MSCFModel_Krauss::dawdle(SUMOReal speed) const {
	if(!MSGlobals::gSemiImplicitEulerUpdate){
		// in case of the ballistic update, negative speeds indicate
		// a desired stop before the completion of the next timestep.
		// We do not allow dawdling to overwrite this indication
		if(speed < 0) return speed;
	}
    // generate random number out of [0,1)
    const SUMOReal random = RandHelper::rand();
    // Dawdle.
    if (speed < myAccel) {
        // we should not prevent vehicles from driving just due to dawdling
        //  if someone is starting, he should definitely start
        // (but what about slow-to-start?)!!!
        speed -= ACCEL2SPEED(myDawdle * speed * random);
    } else {
        speed -= ACCEL2SPEED(myDawdle * myAccel * random);
    }
    return MAX2(SUMOReal(0), speed);
}


MSCFModel*
MSCFModel_Krauss::duplicate(const MSVehicleType* vtype) const {
    return new MSCFModel_Krauss(vtype, myAccel, myDecel, myDawdle, myHeadwayTime);
}


/****************************************************************************/
