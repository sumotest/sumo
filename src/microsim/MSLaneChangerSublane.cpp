/****************************************************************************/
/// @file    MSLaneChangerSublane.cpp
/// @author  Jakob Erdmann
/// @date    Oct 2015
/// @version $Id: MSLaneChangerSublane.cpp 19043 2015-10-07 13:56:04Z namdre $
///
// Performs sub-lane changing of vehicles
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

#include "MSLaneChangerSublane.h"
#include "MSNet.h"
#include "MSVehicle.h"
#include "MSVehicleType.h"
#include "MSVehicleTransfer.h"
#include "MSGlobals.h"
#include <cassert>
#include <iterator>
#include <cstdlib>
#include <cmath>
#include <microsim/lcmodels/MSAbstractLaneChangeModel.h>
#include <utils/common/MsgHandler.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// member method definitions
// ===========================================================================
MSLaneChangerSublane::MSLaneChangerSublane(const std::vector<MSLane*>* lanes, bool allowSwap) : 
    MSLaneChanger(lanes, allowSwap)
{
}


MSLaneChangerSublane::~MSLaneChangerSublane() {}


bool
MSLaneChangerSublane::change() {
    // variant of change() for the sublane case
    myCandi = findCandidate();
    MSVehicle* vehicle = veh(myCandi);
#ifdef DEBUG_VEHICLE_GUI_SELECTION
    if (gDebugSelectedVehicle == vehicle->getID()) {
        int bla = 0;
    }
#endif
    if (vehicle->getLane() != (*myCandi).lane || vehicle->getLaneChangeModel().isChangingLanes()) {
        // vehicles shadows and changing vehicles are not eligible
        registerUnchanged(vehicle);
        return false;
    }
#ifndef NO_TRACI
    if (vehicle->hasInfluencer() && vehicle->getInfluencer().isVTDControlled()) {
        return false; // !!! temporary; just because it broke, here
    }
#endif
    vehicle->updateBestLanes(); // needed?
    for (int i = 0; i < (int) myChanger.size(); ++i) {
        vehicle->adaptBestLanesOccupation(i, myChanger[i].dens);
    }
    const std::vector<MSVehicle::LaneQ>& preb = vehicle->getBestLanes();
    // XXX get leader(s)
    std::pair<MSVehicle* const, SUMOReal> leader = getRealThisLeader(myCandi);
    // check whether the vehicle wants and is able to change to right lane
    int state1 = 0;
    SUMOReal latDist = 0;
    if (mayChange(-1) ||
            vehicle->getLateralPositionOnLane() + 0.5 * myCandi->lane->getWidth() - 0.5 * vehicle->getVehicleType().getWidth() > 0) {
        state1 = checkChange(mayChange(-1) ? -1 : 0, leader, preb, latDist);
        bool changingAllowed1 = (state1 & LCA_BLOCKED) == 0;
        // change if the vehicle wants to and is allowed to change
        if ((state1 & LCA_RIGHT) != 0 && changingAllowed1) {
            startChange(vehicle, myCandi, latDist);
            return true;
        }
        if ((state1 & LCA_RIGHT) != 0 && (state1 & LCA_URGENT) != 0) {
            (myCandi - 1)->lastBlocked = vehicle;
            if ((myCandi - 1)->firstBlocked == 0) {
                (myCandi - 1)->firstBlocked = vehicle;
            }
        }
    }


    // check whether the vehicle wants and is able to change to left lane
    int state2 = 0;
    if (mayChange(1) ||
            vehicle->getLateralPositionOnLane() + 0.5 * myCandi->lane->getWidth() + 0.5 * vehicle->getVehicleType().getWidth() < myCandi->lane->getWidth()) {
        state2 = checkChange(mayChange(1) ? 1 : 0, leader, preb, latDist);
        bool changingAllowed2 = (state2 & LCA_BLOCKED) == 0;
        // change if the vehicle wants to and is allowed to change
        if ((state2 & LCA_LEFT) != 0 && changingAllowed2) {
            startChange(vehicle, myCandi, latDist);
            return true;
        }
        if ((state2 & LCA_LEFT) != 0 && (state2 & LCA_URGENT) != 0) {
            (myCandi + 1)->lastBlocked = vehicle;
            if ((myCandi + 1)->firstBlocked == 0) {
                (myCandi + 1)->firstBlocked = vehicle;
            }
        }
    }

    if ((state1 & (LCA_URGENT)) != 0 && (state2 & (LCA_URGENT)) != 0) {
        // ... wants to go to the left AND to the right
        // just let them go to the right lane...
        state2 = 0;
    }
    vehicle->getLaneChangeModel().setOwnState(state2 | state1);

    registerUnchanged(vehicle);
    return false;
}


void
MSLaneChangerSublane::startChange(MSVehicle* vehicle, ChangerIt& from, SUMOReal latDist) {
    // 1) update vehicles lateral position according to latDist and target lane
    vehicle->myState.myPosLat += latDist;

    // 2) distinguish several cases 
    //   a) vehicle moves completely within the same lane
    //   b) vehicle intersects another lane
    //      - vehicle must be moved to the lane where it's midpoint is (either old or new)
    //      - shadow vehicle must be created/moved to the other lane if the vehicle intersects it
    // 3) updated dens of all lanes that hold the vehicle or its shadow
    if (fabs(vehicle->getLateralPositionOnLane()) > 0.5 * vehicle->getLane()->getWidth()) {
        const int direction = vehicle->getLateralPositionOnLane() < 0 ? -1 : 1;
        ChangerIt to = from + direction;
        vehicle->myState.myPosLat -= direction * 0.5 * (from->lane->getWidth() + to->lane->getWidth());
        to->lane->myTmpVehicles.insert(to->lane->myTmpVehicles.begin(), vehicle);
        to->dens += vehicle->getVehicleType().getLengthWithGap();
        vehicle->getLaneChangeModel().startLaneChangeManeuver(from->lane, to->lane, direction);
        // vehicle moved to a new lane
    } else {
        registerUnchanged(vehicle);
    }
    

    // XXX updated vehicles lane and handle the shadow vehicle
    // XXX updated dens for affected lanes
}


int 
MSLaneChangerSublane::checkChange(
        int laneOffset,
        const std::pair<MSVehicle* const, SUMOReal>& leader,
        const std::vector<MSVehicle::LaneQ>& preb,
        SUMOReal& latDist) const {
    std::pair<MSVehicle* const, SUMOReal> neighLead;
    std::pair<MSVehicle* const, SUMOReal> neighFollow;
    MSVehicle* vehicle = veh(myCandi);
    ChangerIt target = myCandi + laneOffset;
    int blocked = 0;
    int blockedByLeader = (laneOffset == -1 ? LCA_BLOCKED_BY_RIGHT_LEADER : LCA_BLOCKED_BY_LEFT_LEADER);
    int blockedByFollower = (laneOffset == -1 ? LCA_BLOCKED_BY_RIGHT_FOLLOWER : LCA_BLOCKED_BY_LEFT_FOLLOWER);
    // overlap
    if (neighFollow.first != 0 && neighFollow.second < 0) {
        blocked |= (blockedByFollower | LCA_OVERLAPPING);
    }
    if (neighLead.first != 0 && neighLead.second < 0) {
        blocked |= (blockedByLeader | LCA_OVERLAPPING);
    }
    // safe back gap
    if (neighFollow.first != 0) {
        // !!! eigentlich: vsafe braucht die Max. Geschwindigkeit beider Spuren
        if (neighFollow.second < neighFollow.first->getCarFollowModel().getSecureGap(neighFollow.first->getSpeed(), vehicle->getSpeed(), vehicle->getCarFollowModel().getMaxDecel())) {
            blocked |= blockedByFollower;
        }
    }

    // safe front gap
    if (neighLead.first != 0) {
        // !!! eigentlich: vsafe braucht die Max. Geschwindigkeit beider Spuren
        if (neighLead.second < vehicle->getCarFollowModel().getSecureGap(vehicle->getSpeed(), neighLead.first->getSpeed(), neighLead.first->getCarFollowModel().getMaxDecel())) {
            blocked |= blockedByLeader;
        }
    }

    MSAbstractLaneChangeModel::MSLCMessager msg(leader.first, neighLead.first, neighFollow.first);
    int state = blocked | vehicle->getLaneChangeModel().wantsChangeSublane(
                    laneOffset, msg, blocked, leader, neighLead, neighFollow, *(target->lane), preb, &(myCandi->lastBlocked), &(myCandi->firstBlocked), latDist);

    // XXX
    // do are more carefull (but expensive) check to ensure that a
    // safety-critical leader is not being overloocked

    // XXX
    // ensure that a continuous lane change manoeuvre can be completed
    // before the next turning movement

#ifndef NO_TRACI
    // let TraCI influence the wish to change lanes and the security to take
    //const int oldstate = state;
    state = vehicle->influenceChangeDecision(state);
    //if (vehicle->getID() == "150_2_36000000") {
    //    std::cout << STEPS2TIME(MSNet::getInstance()->getCurrentTimeStep()) << " veh=" << vehicle->getID() << " oldstate=" << oldstate << " newstate=" << state << "\n";
    //}
#endif
    return state;
}

/****************************************************************************/

