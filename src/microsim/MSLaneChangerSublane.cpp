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
MSLaneChangerSublane::MSLaneChangerSublane(const std::vector<MSLane*>* lanes, bool allowChanging, bool allowSwap) : 
    MSLaneChanger(lanes, allowChanging, allowSwap)
{
}


MSLaneChangerSublane::~MSLaneChangerSublane() {}


void
MSLaneChangerSublane::updateChanger(bool vehHasChanged) {
    MSLaneChanger::updateChanger(vehHasChanged);
    if (!vehHasChanged) {
        MSVehicle* lead = myCandi->lead;
        //std::cout << SIMTIME << " updateChanger lane=" << myCandi->lane->getID() << " lead=" << Named::getIDSecure(lead) << "\n";
        myCandi->ahead.addLeader(lead, false, 0);
        MSLane* shadowLane = lead->getLaneChangeModel().getShadowLane();
        if (shadowLane != 0) {
            const SUMOReal latOffset = lead->getLane()->getRightSideOnEdge() - shadowLane->getRightSideOnEdge();
            //std::cout << SIMTIME << " updateChanger shadowLane=" << shadowLane->getID() << " lead=" << Named::getIDSecure(lead) << "\n";
            (myChanger.begin() + shadowLane->getIndex())->ahead.addLeader(lead, false, latOffset);
        }
    }
    //std::cout << SIMTIME << " updateChanger: lane=" << myCandi->lane->getID() << " lead=" << Named::getIDSecure(myCandi->lead) << " ahead=" << myCandi->ahead.toString() << " vehHasChanged=" << vehHasChanged << "\n";
    //for (ChangerIt ce = myChanger.begin(); ce != myChanger.end(); ++ce) {
    //    std::cout << " lane=" << ce->lane->getID() << " vehicles=" << toString(ce->lane->myVehicles) << "\n";
    //}
}


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
    assert(vehicle->getLane() == (*myCandi).lane);
    assert(!vehicle->getLaneChangeModel().isChangingLanes());
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

    // update expected speeds
    int sublaneIndex = 0;
    for (ChangerIt ce = myChanger.begin(); ce != myChanger.end(); ++ce) {
        vehicle->getLaneChangeModel().updateExpectedSublaneSpeeds(ce->ahead, sublaneIndex, ce->lane->getIndex());
        sublaneIndex += ce->ahead.numSublanes();
    }

    // check whether the vehicle wants and is able to change to right lane
    int state1 = 0;
    SUMOReal latDist = 0;
    if (mayChange(-1) ||
            vehicle->getLateralPositionOnLane() + 0.5 * myCandi->lane->getWidth() - 0.5 * vehicle->getVehicleType().getWidth() > 0) {
        const int laneOffset = mayChange(-1) ? -1 : 0;
        state1 = checkChangeSublane(laneOffset, preb, latDist);
        bool changingAllowed1 = (state1 & LCA_BLOCKED) == 0;
        // change if the vehicle wants to and is allowed to change
        if ((state1 & LCA_WANTS_LANECHANGE) != 0 && changingAllowed1) {
            startChangeSublane(vehicle, myCandi, latDist);
            return true;
        }
        if ((state1 & LCA_WANTS_LANECHANGE) != 0 && (state1 & LCA_URGENT) != 0) {
            (myCandi + laneOffset)->lastBlocked = vehicle;
            if ((myCandi + laneOffset)->firstBlocked == 0) {
                (myCandi + laneOffset)->firstBlocked = vehicle;
            }
        }
    }
    latDist = 0; // reset for the next query

    // check whether the vehicle wants and is able to change to left lane
    int state2 = 0;
    if (mayChange(1) ||
            vehicle->getLateralPositionOnLane() + 0.5 * myCandi->lane->getWidth() + 0.5 * vehicle->getVehicleType().getWidth() < myCandi->lane->getWidth()) {
        const int laneOffset = mayChange(1) ? 1 : 0;
        state2 = checkChangeSublane(laneOffset, preb, latDist);
        bool changingAllowed2 = (state2 & LCA_BLOCKED) == 0;
        // change if the vehicle wants to and is allowed to change
        if ((state2 & LCA_WANTS_LANECHANGE) != 0 && changingAllowed2) {
            startChangeSublane(vehicle, myCandi, latDist);
            return true;
        }
        if ((state2 & LCA_WANTS_LANECHANGE) != 0 && (state2 & LCA_URGENT) != 0) {
            (myCandi + laneOffset)->lastBlocked = vehicle;
            if ((myCandi + laneOffset)->firstBlocked == 0) {
                (myCandi + laneOffset)->firstBlocked = vehicle;
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
MSLaneChangerSublane::startChangeSublane(MSVehicle* vehicle, ChangerIt& from, SUMOReal latDist) {
    //gDebugFlag4 = vehicle->getID() == "Togliatti_80_26";
    // 1) update vehicles lateral position according to latDist and target lane
    vehicle->myState.myPosLat += latDist;

    // 2) distinguish several cases 
    //   a) vehicle moves completely within the same lane
    //   b) vehicle intersects another lane
    //      - vehicle must be moved to the lane where it's midpoint is (either old or new)
    //      - shadow vehicle must be created/moved to the other lane if the vehicle intersects it
    // 3) updated dens of all lanes that hold the vehicle or its shadow
    const int direction = vehicle->getLateralPositionOnLane() < 0 ? -1 : 1;
    ChangerIt to = from + direction;
    if (fabs(vehicle->getLateralPositionOnLane()) > 0.5 * vehicle->getLane()->getWidth()) {
        // vehicle moved to a new lane
        vehicle->myState.myPosLat -= direction * 0.5 * (from->lane->getWidth() + to->lane->getWidth());
        to->lane->myTmpVehicles.insert(to->lane->myTmpVehicles.begin(), vehicle);
        to->dens += vehicle->getVehicleType().getLengthWithGap();
        vehicle->getLaneChangeModel().startLaneChangeManeuver(from->lane, to->lane, direction);
        to->ahead.addLeader(vehicle, false, 0);
    } else {
        registerUnchanged(vehicle);
        from->ahead.addLeader(vehicle, false, 0);
    }

    MSLane* oldShadowLane = vehicle->getLaneChangeModel().getShadowLane();
    vehicle->getLaneChangeModel().updateShadowLane();
    MSLane* shadowLane = vehicle->getLaneChangeModel().getShadowLane();
    if (shadowLane != 0 && shadowLane != oldShadowLane) {
        assert(to != from);
        const SUMOReal latOffset = vehicle->getLane()->getRightSideOnEdge() - shadowLane->getRightSideOnEdge();
        (myChanger.begin() + shadowLane->getIndex())->ahead.addLeader(vehicle, false, latOffset);
    }
    if (gDebugFlag4) std::cout << SIMTIME << " startChangeSublane shadowLane"
        << " old=" << Named::getIDSecure(oldShadowLane) 
            << " new=" << Named::getIDSecure(vehicle->getLaneChangeModel().getShadowLane()) << "\n";
}


MSLeaderDistanceInfo
MSLaneChangerSublane::getLeaders(const ChangerIt& target, const MSVehicle* ego) const {
    //if (ego->getID() == "C" && SIMTIME == 17) {
    //    std::cout << "DEBUG\n";
    //}
    // get the leading vehicle on the lane to change to
    if (gDebugFlag1) std::cout << SIMTIME << " getLeaders lane=" << target->lane->getID() << " ego=" << ego->getID() << " ahead=" << target->ahead.toString() << "\n";
    MSLeaderDistanceInfo result(target->lane, 0, 0);
    for (int i = 0; i < target->ahead.numSublanes(); ++i) {
        const MSVehicle* veh = target->ahead[i];
        if (veh != 0) {
            assert(veh != 0);
            const SUMOReal gap = veh->getBackPositionOnLane() - ego->getPositionOnLane() - ego->getVehicleType().getMinGap();
            if (gDebugFlag1) std::cout << " ahead lead=" << veh->getID() << " leadBack=" << veh->getBackPositionOnLane() << " gap=" << gap << "\n";
            result.addLeader(veh, gap, 0, i);
        }
    }
    // if there are vehicles on the target lane with the same position as ego,
    // they may not have been added to 'ahead' yet
    const MSLeaderInfo& aheadSamePos = target->lane->getLastVehicleInformation(0, 0, ego->getPositionOnLane(), false);
    for (int i = 0; i < aheadSamePos.numSublanes(); ++i) {
        const MSVehicle* veh = aheadSamePos[i];
        if (veh != 0 && veh != ego) {
            const SUMOReal gap = veh->getBackPositionOnLane(target->lane) - ego->getPositionOnLane() - ego->getVehicleType().getMinGap();
            if (gDebugFlag1) std::cout << " further lead=" << veh->getID() << " leadBack=" << veh->getBackPositionOnLane(target->lane) << " gap=" << gap << "\n";
            result.addLeader(veh, gap, 0, i);
        }
    }

    if (result.numFreeSublanes() > 0) {
        MSLane* targetLane = target->lane;

        SUMOReal seen = ego->getLane()->getLength() - ego->getPositionOnLane();
        SUMOReal speed = ego->getSpeed();
        SUMOReal dist = ego->getCarFollowModel().brakeGap(speed) + ego->getVehicleType().getMinGap();
        if (seen > dist) {
            return result;
        }
        const std::vector<MSLane*>& bestLaneConts = veh(myCandi)->getBestLanesContinuation(targetLane);
        target->lane->getLeadersOnConsecutive(dist, seen, speed, ego, bestLaneConts, result);
    }
    return result;
}


int 
MSLaneChangerSublane::checkChangeSublane(
        int laneOffset,
        const std::vector<MSVehicle::LaneQ>& preb,
        SUMOReal& latDist) const {

    ChangerIt target = myCandi + laneOffset;
    MSVehicle* vehicle = veh(myCandi);
    const MSLane& neighLane = *(target->lane);
    int blocked = 0;

    gDebugFlag1 = vehicle->getID() == "disabled";

    MSLeaderDistanceInfo neighLeaders = getLeaders(target, vehicle);
    MSLeaderDistanceInfo neighFollowers = target->lane->getFollowersOnConsecutive(vehicle, true);
    MSLeaderDistanceInfo neighBlockers(&neighLane, vehicle, vehicle->getLane()->getRightSideOnEdge() - neighLane.getRightSideOnEdge());
    MSLeaderDistanceInfo leaders = getLeaders(myCandi, vehicle);
    MSLeaderDistanceInfo followers = myCandi->lane->getFollowersOnConsecutive(vehicle, true);
    MSLeaderDistanceInfo blockers(vehicle->getLane(), vehicle, 0);
 
    if (gDebugFlag1) std::cout << SIMTIME 
        << " checkChangeSublane: veh=" << vehicle->getID() 
        << " laneOffset=" << laneOffset 
        << "\n  leaders=" << leaders.toString()
        << "\n  neighLeaders=" << neighLeaders.toString()
        << "\n";


    const int wish = vehicle->getLaneChangeModel().wantsChangeSublane(
                    laneOffset, 
                    leaders, followers, blockers,
                    neighLeaders, neighFollowers, neighBlockers,
                    neighLane, preb,
                    &(myCandi->lastBlocked), &(myCandi->firstBlocked), latDist, blocked);
    int state = blocked | wish;

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
    gDebugFlag1 = false;
    return state;
}

/****************************************************************************/

