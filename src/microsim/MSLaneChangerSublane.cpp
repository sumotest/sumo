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


void
MSLaneChangerSublane::updateChanger(bool vehHasChanged) {
    MSLaneChanger::updateChanger(vehHasChanged);
    if (!vehHasChanged) {
        MSVehicle* lead = myCandi->lead;
        myCandi->ahead.addLeader(lead, false, 0);
        MSLane* shadowLane = lead->getLaneChangeModel().getShadowLane();
        if (shadowLane != 0) {
            const SUMOReal latOffset = lead->getLane()->getRightSideOnEdge() - shadowLane->getRightSideOnEdge();
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
        state1 = checkChangeSublane(mayChange(-1) ? -1 : 0, preb, latDist);
        bool changingAllowed1 = (state1 & LCA_BLOCKED) == 0;
        // change if the vehicle wants to and is allowed to change
        if ((state1 & LCA_WANTS_LANECHANGE) != 0 && changingAllowed1) {
            startChangeSublane(vehicle, myCandi, latDist);
            return true;
        }
        if ((state1 & LCA_WANTS_LANECHANGE) != 0 && (state1 & LCA_URGENT) != 0) {
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
        state2 = checkChangeSublane(mayChange(1) ? 1 : 0, preb, latDist);
        bool changingAllowed2 = (state2 & LCA_BLOCKED) == 0;
        // change if the vehicle wants to and is allowed to change
        if ((state2 & LCA_WANTS_LANECHANGE) != 0 && changingAllowed2) {
            startChangeSublane(vehicle, myCandi, latDist);
            return true;
        }
        if ((state2 & LCA_WANTS_LANECHANGE) != 0 && (state2 & LCA_URGENT) != 0) {
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
MSLaneChangerSublane::startChangeSublane(MSVehicle* vehicle, ChangerIt& from, SUMOReal latDist) {
    // 1) update vehicles lateral position according to latDist and target lane
    vehicle->myState.myPosLat += latDist;

    // 2) distinguish several cases 
    //   a) vehicle moves completely within the same lane
    //   b) vehicle intersects another lane
    //      - vehicle must be moved to the lane where it's midpoint is (either old or new)
    //      - shadow vehicle must be created/moved to the other lane if the vehicle intersects it
    // 3) updated dens of all lanes that hold the vehicle or its shadow
    MSLane* shadowLane = 0;
    const int direction = vehicle->getLateralPositionOnLane() < 0 ? -1 : 1;
    ChangerIt to = from + direction;
    if (fabs(vehicle->getLateralPositionOnLane()) > 0.5 * vehicle->getLane()->getWidth()) {
        // vehicle moved to a new lane
        vehicle->myState.myPosLat -= direction * 0.5 * (from->lane->getWidth() + to->lane->getWidth());
        to->lane->myTmpVehicles.insert(to->lane->myTmpVehicles.begin(), vehicle);
        to->dens += vehicle->getVehicleType().getLengthWithGap();
        vehicle->getLaneChangeModel().startLaneChangeManeuver(from->lane, to->lane, direction);
        to->ahead.addLeader(vehicle, false, 0);
        shadowLane = from->lane;
    } else {
        registerUnchanged(vehicle);
        from->ahead.addLeader(vehicle, false, 0);
        shadowLane = to->lane;
    }
    MSLane* oldShadowLane = vehicle->getLaneChangeModel().getShadowLane();
    const SUMOReal overlap = (fabs(vehicle->getLateralPositionOnLane() + 0.5 * vehicle->getVehicleType().getWidth()) 
            - 0.5 * vehicle->getLane()->getWidth());
    if (overlap > 0) {
        assert(to != from);
        // sorted later in MSLane::integrateNewVehicle
        if (shadowLane != oldShadowLane) {
            shadowLane->setPartialOccupation(vehicle);
            const SUMOReal latOffset = vehicle->getLane()->getRightSideOnEdge() - shadowLane->getRightSideOnEdge();
            (myChanger.begin() + shadowLane->getIndex())->ahead.addLeader(vehicle, false, latOffset);
        }
    } else {
        shadowLane = 0;
    }
    vehicle->getLaneChangeModel().setShadowLane(shadowLane);
    if (oldShadowLane != 0 && oldShadowLane != shadowLane) {
        oldShadowLane->resetPartialOccupation(vehicle);
    }
}


MSLeaderDistanceInfo
MSLaneChangerSublane::getLeaders(const ChangerIt& target, const MSVehicle* ego) const {
    // get the leading vehicle on the lane to change to
    if (gDebugFlag1) std::cout << SIMTIME << " getLeaders lane=" << target->lane->getID() << " ego=" << ego->getID() << "\n";
    MSLeaderDistanceInfo result(target->lane->getWidth(), 0);
    for (int i = 0; i < target->ahead.numSublanes(); ++i) {
            if (gDebugFlag1 && target->ahead[i] != 0) std::cout << " dist=" << -ego->getPositionOnLane() << " lead=" << target->ahead[i]->getID() << " leadBack=" << target->ahead[i]->getBackPositionOnLane() << "\n";
        result.addLeader(target->ahead[i], -(ego->getPositionOnLane() + ego->getVehicleType().getMinGap()), 0, i);
    }

    //if (veh(myCandi)->getID() == "flow.21") std::cout << SIMTIME << " neighLead=" << Named::getIDSecure(neighLead) << " (416)\n";
    // check whether the hopped vehicle became the leader
    //if (target->hoppedVeh != 0) {
    //    SUMOReal hoppedPos = target->hoppedVeh->getPositionOnLane();
    //    if (hoppedPos > veh(myCandi)->getPositionOnLane() && (neighLead == 0 || neighLead->getPositionOnLane() > hoppedPos)) {
    //        neighLead = target->hoppedVeh;
    //        //if (veh(myCandi)->getID() == "flow.21") std::cout << SIMTIME << " neighLead=" << Named::getIDSecure(neighLead) << " (422)\n";
    //    }
    //}
    //if (neighLead == 0) {
    //    MSLane* targetLane = target->lane;
    //    if (targetLane->myPartialVehicles.size() > 0) {
    //        MSVehicle* leader = targetLane->myPartialVehicles.front();
    //        return std::pair<MSVehicle*, SUMOReal>(leader, leader->getBackPositionOnLane(targetLane) - veh(myCandi)->getPositionOnLane() - veh(myCandi)->getVehicleType().getMinGap());
    //    }
    //    SUMOReal seen = myCandi->lane->getLength() - veh(myCandi)->getPositionOnLane();
    //    SUMOReal speed = veh(myCandi)->getSpeed();
    //    SUMOReal dist = veh(myCandi)->getCarFollowModel().brakeGap(speed) + veh(myCandi)->getVehicleType().getMinGap();
    //    if (seen > dist) {
    //        return std::pair<MSVehicle* const, SUMOReal>(static_cast<MSVehicle*>(0), -1);
    //    }
    //    const std::vector<MSLane*>& bestLaneConts = veh(myCandi)->getBestLanesContinuation(targetLane);
    //    //if (veh(myCandi)->getID() == "flow.21") std::cout << SIMTIME << " calling getLeaderOnConsecutive (443)\n";
    //    return target->lane->getLeaderOnConsecutive(dist, seen, speed, *veh(myCandi), bestLaneConts);
    //} else {
    //    MSVehicle* candi = veh(myCandi);
    //    return std::pair<MSVehicle* const, SUMOReal>(neighLead, neighLead->getBackPositionOnLane(target->lane) - candi->getPositionOnLane() - candi->getVehicleType().getMinGap());
    //}
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
    MSLeaderDistanceInfo neighBlockers(neighLane.getWidth(), vehicle);
    MSLeaderDistanceInfo leaders = getLeaders(myCandi, vehicle);
    MSLeaderDistanceInfo followers = myCandi->lane->getFollowersOnConsecutive(vehicle, true);
    MSLeaderDistanceInfo blockers(vehicle->getLane()->getWidth(), vehicle);
 
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

