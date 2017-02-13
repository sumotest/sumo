/****************************************************************************/
/// @file    MSLaneSequenceDetector.cpp
/// @author  Leonhard Luecken
/// @date    Mon Feb 03 2014 10:13 CET
/// @version $Id: MSLaneSequenceDetector.cpp 22841 2017-02-03 22:10:57Z luecken $
///
// An areal detector corresponding to a sequence of consecutive lanes
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2017 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/


/* TODO:
 * tests:
 *  - detector area one complete lane, on one lane with startPos!=0, endPos!=length, two complete lanes, two lanes with endpos and startpos
 *  - detector area covering an area of several, very short lanes, detector very short
 *  - detector specification with range for branching upstream road
 *  - driving, parking, stopping, teleporting, lane-changing on detector area
 *  - leaving detector along junction before end
 *  - many vehicles
 *  - detector starting on internal lane
 *  - subsecond variant, ballistic variant
 *
 *
 * Meso-compatibility? (esp. enteredLane-argument for MSBaseVehicle::notifyEnter() is not treated)
 * Compatibility without internal lanes?
 *
 * Clean-up on destruction (perhaps multiple calls are attempted from the different lanes?)
*/

// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <cassert>
#include <algorithm>
#include "MSLaneSequenceDetector.h"
#include <microsim/MSLane.h>
#include <microsim/MSNet.h>
#include <microsim/MSVehicle.h>
#include <microsim/MSVehicleType.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS

#define DEBUG_LSD_NOTIFY_MOVE
#define DEBUG_LSD_CONSTRUCTOR
#define DEBUG_LSD_NOTIFY_ENTER_AND_LEAVE
#define DEBUG_LSD_MAKE_VEHINFO
#define DEBUG_LSD_DETECTOR_UPDATE
#define DEBUG_LSD_TIME_ON_DETECTOR

// ===========================================================================
// method definitions
// ===========================================================================
MSLaneSequenceDetector::MSLaneSequenceDetector(const std::string& id, DetectorUsage usage,
        std::vector<MSLane*> lanes, SUMOReal startPos, SUMOReal endPos,
        const std::string& vTypes, bool /*showDetector*/) :
    MSMoveReminder(id, lanes[lanes.size()-1], false), // TODO: assure lanes.size()>0 at caller
    MSDetectorFileOutput(id, vTypes),
    myStartPos(startPos), myEndPos(endPos),
    myUsage(usage) {

#ifdef DEBUG_LSD_CONSTRUCTOR
    std::cout << "\n" << "Creating MSLaneSequenceDetector " << id
                    << "over lanes ";
    for (std::vector<MSLane*>::const_iterator i = lanes.begin(); i != lanes.end(); ++i) {
        std::cout << "\n" << (*i)->getID();
    }
    std::cout << "\nendPos = " << endPos << " startPos = " << startPos << std::endl;
#endif

    assert(lanes.size() != 0);

    initAuxiliaries(lanes);

    assert(myStartPos >= 0 && myStartPos < myFirstLane->getLength());
    assert(myEndPos >= 0 && myEndPos < myLastLane->getLength());
    assert(myFirstLane != myLastLane || (myEndPos - myStartPos > 0 && myEndPos <= myFirstLane->getLength()));

    addDetectorToLanes(lanes);
}


MSLaneSequenceDetector::MSLaneSequenceDetector(const std::string& id, DetectorUsage usage,
        std::vector<MSLane*> lanes, SUMOReal startPos, SUMOReal endPos,
        const std::set<std::string>& vTypes, bool /*showDetector*/) :
    MSMoveReminder(id, lanes[lanes.size()-1], false), // TODO: assure lanes.size()>0 at caller
    MSDetectorFileOutput(id, vTypes),
    myStartPos(startPos), myEndPos(endPos),
    myUsage(usage) {

#ifdef DEBUG_LSD_CONSTRUCTOR
    std::cout << "\n" << "Creating MSLaneSequenceDetector " << id
                    << "over lanes ";
    for (std::vector<MSLane*>::const_iterator i = lanes.begin(); i != lanes.end(); ++i) {
        std::cout << "\n" << (*i)->getID();
    }
    std::cout << "\nendPos = " << endPos << " startPos = " << startPos << std::endl;
#endif

    assert(lanes.size() != 0);

    initAuxiliaries(lanes);

    assert(myStartPos >= 0 && myStartPos < myFirstLane->getLength());
    assert(myEndPos >= 0 && myEndPos < myLastLane->getLength());
    assert(myFirstLane != myLastLane || (myEndPos - myStartPos > 0 && myEndPos <= myFirstLane->getLength()));

    addDetectorToLanes(lanes);
}


MSLaneSequenceDetector::MSLaneSequenceDetector(const std::string& id,
        DetectorUsage usage, MSLane* endLane, SUMOReal endPos, SUMOReal upstreamRange,
        const std::string& vTypes, bool /*showDetector*/) :
    MSMoveReminder(id, endLane, false),
    MSDetectorFileOutput(id, vTypes),
    myEndPos(endPos),
    myUsage(usage) {

#ifdef DEBUG_LSD_CONSTRUCTOR
    std::cout << "\n" << "Creating MSLaneSequenceDetector " << id
                    << " with endLane = " << endLane->getID()
                    << " endPos = " << endPos
                    << " upstreamRange = " << upstreamRange << std::endl;
#endif

    assert(endLane != 0);
    assert(endPos <= endLane->getLength());

    std::vector<MSLane*> lanes = selectLanes(endLane, endPos, upstreamRange);

    initAuxiliaries(lanes);

    assert(myStartPos >= 0 && myStartPos < myFirstLane->getLength());
    assert(myEndPos >= 0 && myEndPos < myLastLane->getLength());
    assert(myFirstLane != myLastLane || (myEndPos - myStartPos > 0 && myEndPos <= myFirstLane->getLength()));

    addDetectorToLanes(lanes);
}


std::vector<MSLane*>
MSLaneSequenceDetector::selectLanes(MSLane* endLane, SUMOReal endPos, SUMOReal upstreamRange){
#ifdef DEBUG_LSD_CONSTRUCTOR
    std::cout << "\n" << "selectLanes()" << std::endl;
#endif
    std::vector<MSLane*> lanes;
    // Selected lanes are stacked into myLanes. After this is done myLanes will be reversed.
    MSLane* lane = endLane;

    // adjust upstreamRange (in the first iteration, the whole length of the endLane is substracted)
    upstreamRange += lane->getLength() - myEndPos;
    while(upstreamRange > 0 && lane != 0){
        lanes.push_back(lane);
#ifdef DEBUG_LSD_CONSTRUCTOR
        std::cout << "Added lane " << lane->getID()
                << " (length: "<< lane->getLength() << ") with pred: ";
#endif
        upstreamRange -= lane->getLength();

        // proceed to upstream predecessor
        lane = lane->getLogicalPredecessorLane();

#ifdef DEBUG_LSD_CONSTRUCTOR
        if (lane!=0) {
            std::cout << "'" << lane->getID() << "'";
        }
        std::cout << std::endl;
#endif
    }
    myStartPos = -upstreamRange;
    std::reverse(lanes.begin(), lanes.end());
    return lanes;
}

void
MSLaneSequenceDetector::addDetectorToLanes(std::vector<MSLane*>& lanes) {
#ifdef DEBUG_LSD_CONSTRUCTOR
    std::cout << "\n" << "Adding detector " << myID << "to lanes:" << std::endl;
#endif
    for (std::vector<MSLane*>::iterator l = lanes.begin(); l != lanes.end(); ++l){
        (*l)->addMoveReminder(this);
#ifdef DEBUG_LSD_CONSTRUCTOR
    std::cout << (*l)->getID() << std::endl;
#endif
    }
}

void
MSLaneSequenceDetector::initAuxiliaries(std::vector<MSLane*>& lanes){
    // Checks integrity of myLanes, adds internal-lane information, inits myLength, myFirstLane, myLastLane, myOffsets
    myFirstLane = lanes[0];
    myLastLane = lanes[lanes.size()-1];

#ifdef DEBUG_LSD_CONSTRUCTOR
    std::cout << "\n" << "Initializing auxiliaries:"
            << "\nFirst lane: " << myFirstLane->getID() << " (startPos = " << myStartPos << ")"
            << "\nLast lane: " << myLastLane->getID() << " (endPos = " << myEndPos << ")"
            << std::endl;
#endif

    // Init myOffsets and myDetectorLength.
    // The loop below runs through the given lanes assuming the possibility that only non-internal lanes are given
    // or at least not all relevant internal lanes are considered. During this a new, complete list of lane ids is
    // built into myLanes.
    myLanes.clear();

    // myDetectorLength will be increased in the loop below, always giving
    // the offset of the currently considered lane to the detector start
    myDetectorLength = -myStartPos;
    myOffsets.clear();

    // loop over detector lanes and accumulate offsets with respect to the first lane's begin
    // (these will be corrected afterwards by substracting the start position.)
    std::vector<MSLane*>::iterator il = lanes.begin();

    // start on an internal lane?
    // (This may happen if specifying the detector by its upstream
    //  length starting from a given end position)
    const MSLane* internal = (*il)->isInternal() ? *il : 0;

#ifdef DEBUG_LSD_CONSTRUCTOR
    std::cout << "\n" << "Initializing offsets:"<<std::endl;
#endif

    while (true) {
        // Consider the next internal lanes
        while(internal != 0){
            myLanes.push_back(internal->getID());
            myOffsets.push_back(myDetectorLength);

#ifdef DEBUG_LSD_CONSTRUCTOR
    std::cout << "Offset for lane " << internal->getID() << " = " << myDetectorLength
            << std::endl;
#endif

            myDetectorLength += internal->getLength();
            if(internal->getID() == myLastLane->getID()){
                break;
            }

            // There should be a unique continuation for each internal lane
            assert(internal->getLinkCont().size()==1);

            internal = internal->getLinkCont()[0]->getViaLaneOrLane();
            if (!internal->isInternal()){
                // passed the junction
                internal = 0;
                break;
            }
        }

        // Consider the next non-internal lane
        // This is the first lane in the first iteration, if it is non-internal
        // However, it can equal myLanes.end() if the myLastLane is internal. In that case we break.

        // Move il to next non-internal
        while (il != lanes.end() && (*il)->isInternal()) il++;
        if (il == lanes.end()) break;

        // There is still a non-internal lane to consider
        MSLane* lane = *il;
        myLanes.push_back(lane->getID());

#ifdef DEBUG_LSD_CONSTRUCTOR
    std::cout << "Offset for lane " << lane->getID() << " = " << myDetectorLength
            << std::endl;
#endif

        // Offset to detector start for this lane
        myOffsets.push_back(myDetectorLength);

        // Add the lanes length to the detector offset
        myDetectorLength += lane->getLength();

        // Get the next lane if this lane isn't the last one
        if (++il == lanes.end()) break;

        if ((*il)->isInternal()) {
            // next lane in myLanes is internal
            internal = *il;
            continue;
        }

        // find the connection to next
        const MSLink* link = lane->getLinkTo(*il);
        assert(link!=0);

        if (!MSGlobals::gUsingInternalLanes){
            myDetectorLength += link->getLength();
        } else {
            internal = link->getViaLane();
        }
    }

    // Substract distance not covered on the last lane
    myDetectorLength -= myLastLane->getLength() - myEndPos;


#ifdef DEBUG_LSD_CONSTRUCTOR
    std::cout << "Total detector length = "<< myDetectorLength << std::endl;
#endif
}

MSLaneSequenceDetector::~MSLaneSequenceDetector() {}



bool
MSLaneSequenceDetector::notifyMove(SUMOVehicle& veh, SUMOReal oldPos,
                          SUMOReal newPos, SUMOReal newSpeed) {
    VehicleInfoMap::const_iterator vi = myVehicleInfos.find(veh.getID());
    assert(vi != myVehicleInfos.end()); // all vehicles calling notifyMove() should have called notifyEnter() before

    const std::string& vehID = veh.getID();
    const VehicleInfo& vehInfo = vi->second;

    // position relative to the detector start
    SUMOReal relPos = vehInfo.entryOffset + newPos;

#ifdef DEBUG_LSD_NOTIFY_MOVE
        std::cout << "\n" << SIMTIME
        << " MSLaneSequenceDetector::notifyMove()"
        << " called by vehicle '" << vehID << "'"
        << " at relative position " << relPos << std::endl;
#endif

    // Check whether vehicle has reached the detector begin
    if (relPos <= 0) {
        // detector not yet reached, request being informed further
#ifdef DEBUG_LSD_NOTIFY_MOVE
        std::cout << "Vehicle has not yet reached the detector start position." << std::endl;
#endif
        return true;
    }

    myMoveNotifications.push_back(makeMoveNotification(veh, oldPos, newPos, newSpeed, vehInfo));

    // determine whether vehicle has moved beyond the detector's end
    bool vehPassedDetectorEnd = relPos - veh.getVehicleType().getLength() > vehInfo.exitOffset;

    if (vehPassedDetectorEnd) {
        // Vehicle is beyond the detector, unsubscribe and register removal from myVehicleInfos
        myLeftVehicles.insert(vehID);
        return false;
    } else {
        // Receive further notifications
        return true;
    }
}

bool
MSLaneSequenceDetector::notifyLeave(SUMOVehicle& veh, SUMOReal /* lastPos */, MSMoveReminder::Notification reason) {
#ifdef DEBUG_LSD_NOTIFY_ENTER_AND_LEAVE
        std::cout << "\n" << SIMTIME << " notifyLeave() called by vehicle '" << veh.getID() << "'" << std::endl;
#endif

    if (reason == MSMoveReminder::NOTIFICATION_JUNCTION) {
        // vehicle left lane via junction, unsubscription and registering in myLeftVehicles when
        // moving beyond the detector end is controlled in notifyMove.
#ifdef DEBUG_LSD_NOTIFY_ENTER_AND_LEAVE
        std::cout << SIMTIME << " Left longitudinally (along junction) -> keep subscription" << std::endl;
#endif
        return true;
    } else {
        myLeftVehicles.insert(veh.getID());
#ifdef DEBUG_LSD_NOTIFY_ENTER_AND_LEAVE
        std::cout << SIMTIME << " Left not longitudinally (lanechange, teleport, parking, etc) -> discard subscription" << std::endl;
#endif
        return false;
    }
}


bool
MSLaneSequenceDetector::notifyEnter(SUMOVehicle& veh, MSMoveReminder::Notification /* reason */, const MSLane* enteredLane) {
#ifdef DEBUG_LSD_NOTIFY_ENTER_AND_LEAVE
        std::cout << "\n" << SIMTIME << " notifyEnter() called by vehicle '" << veh.getID()
                << "' entering lane '" << enteredLane->getID() << "'" << std::endl;
#endif
    if (!veh.isOnRoad()) {
        // Vehicle is teleporting over the edge
        return false;
    }
    if (!vehicleApplies(veh)) {
        // That's not my type...
        return false;
    }
    const std::string& vehID = veh.getID();
    VehicleInfoMap::iterator vi = myVehicleInfos.find(vehID);
    if (vi != myVehicleInfos.end()) {
#ifdef DEBUG_LSD_NOTIFY_ENTER_AND_LEAVE
        std::cout << SIMTIME << " Vehicle '" << veh.getID() << "' already known. No new VehicleInfo is created." << std::endl;
#endif

        if (std::find(myLanes.begin(), myLanes.end(), enteredLane->getID()) == myLanes.end()){
            // Entered lane is not part of the detector
            // Determine exit offset, where vehicle left the detector
            SUMOReal exitOffset = myOffsets[vi->second.currentOffsetIndex];
            for (std::vector<const MSLane*>::const_iterator i = vi->second.enteredLanes.begin(); i != vi->second.enteredLanes.begin(); i++){
                exitOffset += (*i)->getLength();
            }
            vi->second.exitOffset = MIN2(vi->second.exitOffset, exitOffset);
#ifdef DEBUG_LSD_NOTIFY_ENTER_AND_LEAVE
        std::cout << SIMTIME << " Vehicle '" << veh.getID() << "' leaves myLanes. Exit offset = " << exitOffset << std::endl;
#endif
        }

        // Register the entered lane
        vi->second.enteredLanes.push_back(enteredLane);

        // but don't add a second subscription for another lane
        return false;
    }

#ifdef DEBUG_LSD_NOTIFY_ENTER_AND_LEAVE
        std::cout << SIMTIME << " Adding VehicleInfo for vehicle '" << veh.getID() << "'." << std::endl;
#endif

    // Add vehicle info
    myVehicleInfos.insert(std::make_pair(vehID, makeVehicleInfo(veh, enteredLane)));
    myNumberOfEnteredVehicles++;
    // Subscribe to vehicle's movement notifications
    return true;
}

MSLaneSequenceDetector::VehicleInfo
MSLaneSequenceDetector::makeVehicleInfo(const SUMOVehicle& veh, const MSLane* enteredLane) const {
    // The vehicle's distance to the detector end
    std::size_t j = std::find(myLanes.begin(), myLanes.end(), enteredLane->getID()) - myLanes.begin();
    assert(j >= 0 && j < myLanes.size());
    SUMOReal entryOffset = myOffsets[j];
    SUMOReal distToDetectorEnd = myDetectorLength - (entryOffset + veh.getPositionOnLane());

#ifdef DEBUG_LSD_MAKE_VEHINFO
    std::cout << SIMTIME << " Making VehicleInfo for vehicle '" << veh.getID() << "'."
            << "\ndistToDetectorEnd = " << distToDetectorEnd
            << "\nveh.getPositionOnLane() = " << veh.getPositionOnLane()
            << "\nentry lane offset (lane begin from detector begin) = " << entryOffset
            << std::endl;
#endif
    return VehicleInfo(veh.getID(), veh.getVehicleType().getID(), veh.getVehicleType().getLength(), enteredLane, entryOffset, j, myDetectorLength, distToDetectorEnd);
}

void
MSLaneSequenceDetector::detectorUpdate(const SUMOTime /* step */) {

#ifdef DEBUG_LSD_DETECTOR_UPDATE
    std::cout << "\n" << SIMTIME << " detectorUpdate() for detector '" << myID << "'" << std::endl;
#endif

    // go through the list of vehicles positioned on the detector
    for (std::vector<MoveNotificationInfo>::iterator i = myMoveNotifications.begin(); i != myMoveNotifications.end(); ++i) {
        /* Update the quantities accumulated over all vehicles which the detector should track */
        // ...

        // The ID of the vehicle that has sent this notification in the last step
        const std::string& vehID = i->id;
        updateVehicleInfo(vehID, *i);
    }

    // reset move notifications
    myMoveNotifications.clear();

#ifdef DEBUG_LSD_DETECTOR_UPDATE
    std::cout << SIMTIME << " Discarding vehicles that have left the detector:" << std::endl;
#endif
    // Remove the vehicles that have left the detector
    std::set<std::string>::const_iterator i;
    for(i = myLeftVehicles.begin(); i != myLeftVehicles.end(); ++i) {
        myVehicleInfos.erase(*i);
        myNumberOfLeftVehicles++;
#ifdef DEBUG_LSD_DETECTOR_UPDATE
        std::cout << "Erased vehicle '" << *i << "'" << std::endl;
#endif
    }
    myLeftVehicles.clear();

    myMaxVehicleNumber = MAX2(myMaxVehicleNumber, myVehicleInfos.size());

#ifdef DEBUG_LSD_DETECTOR_UPDATE
    std::cout << SIMTIME << " Updating current lanes for vehicles still on the detector:" << std::endl;
#endif
    // update current and entered lanes for remaining vehicles
    VehicleInfoMap::iterator iv;
    for (iv = myVehicleInfos.begin(); iv != myVehicleInfos.end(); ++iv) {
        if (iv->second.enteredLanes.size() == 0){
#ifdef DEBUG_LSD_DETECTOR_UPDATE
            std::cout << " Vehicle '" << iv->second.id << "'" << ": '"
                    << iv->second.currentLane->getID() << "' (equals previous)"
                    << std::endl;
#endif
            continue;
        }
        iv->second.currentOffsetIndex += iv->second.enteredLanes.size();
        iv->second.currentLane = *(iv->second.enteredLanes.rbegin());
        iv->second.enteredLanes.clear();
#ifdef DEBUG_LSD_DETECTOR_UPDATE
    std::cout << " Vehicle '" << iv->second.id << "'" << ": '"
            << iv->second.currentLane->getID() << "'"
            << std::endl;
#endif
    }
}

void
MSLaneSequenceDetector::updateVehicleInfo(const std::string& vehID, const MoveNotificationInfo& mni){

#ifdef DEBUG_LSD_DETECTOR_UPDATE
    std::cout << SIMTIME << " updateVehicleInfo() for vehicle '" << vehID << "'" << std::endl;
#endif

    // Accumulate detector values
    myVehicleSamples += mni.timeOnDetector;
    myTotalTimeLoss += mni.timeLoss;


    /* Accumulate individual values for the vehicle */
    VehicleInfoMap::iterator vi = myVehicleInfos.find(vehID);
    // Vehicle ID must already be a valid key in myVehicleInfos (is added in notify enter)
    assert(vi != myVehicleInfos.end());
    vi->second.totalTimeOnDetector += mni.timeOnDetector;
    vi->second.accumulatedTimeLoss += mni.timeLoss;
}


MSLaneSequenceDetector::MoveNotificationInfo
MSLaneSequenceDetector::makeMoveNotification(const SUMOVehicle& veh, SUMOReal oldPos, SUMOReal newPos, SUMOReal newSpeed, const VehicleInfo& vehInfo) const {
#ifdef DEBUG_LSD_NOTIFY_MOVE
    std::cout << SIMTIME << " makeMoveNotification() for vehicle '" << veh.getID() << "'"
            << " oldPos = " << oldPos << " newPos = " << newPos << " newSpeed = " << newSpeed
            << std::endl;
#endif

    SUMOReal timeOnDetector = calculateTimeOnDetector(veh, oldPos, newPos, vehInfo);

    // Note that at this point, vehInfo.currentLane points to the lane at the beginning of the last timestep,
    // and vehInfo.enteredLanes is a list of lanes entered in the last timestep
    SUMOReal timeLoss = calculateStepTimeLoss(veh, oldPos, newPos, vehInfo);

    /* Store new infos */
    return MoveNotificationInfo(veh.getID(), oldPos, newPos, newSpeed, myDetectorLength - (vehInfo.entryOffset + newPos), timeOnDetector, timeLoss);
}


SUMOReal
MSLaneSequenceDetector::calculateTimeOnDetector(const SUMOVehicle& veh, SUMOReal oldPos, SUMOReal newPos, const VehicleInfo& vi) const {
    assert(veh.getID() == vi.id);
    assert(newPos + vi.entryOffset >= 0);

    SUMOReal entryTime = 0;
    SUMOReal entrySpeed, accel;

    // Select the last step's speed and acceleration according to the update scheme
    if (MSGlobals::gSemiImplicitEulerUpdate) {
        // euler update
        entrySpeed = veh.getSpeed();
        accel = 0.;
    } else {
        // ballistic update
        entrySpeed = veh.getPreviousSpeed();
        accel = (veh.getSpeed()-veh.getPreviousSpeed())/TS;
    }

    // Take into account the time before entering the detector, if there is.
    if (oldPos < 0) {
        // Vehicle entered the detector in the last step, either traversing the detector start or somewhere in the middle.
        entryTime = MSCFModel::passingTime(oldPos, 0., newPos, veh.getPreviousSpeed(), veh.getSpeed());
        entrySpeed = entrySpeed + entryTime*accel;
    }

    // Calculate time spent on detector until reaching newPos or a detector exit
    SUMOReal exitOffset = MIN2(newPos, vi.exitOffset + vi.length - vi.entryOffset);
    // calculate vehicle's time spent on the detector
    SUMOReal exitTime = MSCFModel::passingTime(oldPos, exitOffset, newPos, veh.getPreviousSpeed(), veh.getSpeed());
    // Time spent on the detector
    SUMOReal timeOnDetector = exitTime - entryTime;

#ifdef DEBUG_LSD_TIME_ON_DETECTOR
    std::cout << SIMTIME << " calculateTimeOnDetector() for vehicle '" << veh.getID() << "'"
            << " oldPos = " << oldPos << " newPos = " << newPos
            << " exitOffset = " << exitOffset << " timeOnDetector = " << timeOnDetector
            << std::endl;
#endif

    return timeOnDetector;
}


SUMOReal
MSLaneSequenceDetector::calculateStepTimeLoss(const SUMOVehicle& veh, SUMOReal oldPos, SUMOReal newPos, const VehicleInfo& vi) const {

    SUMOReal passingTime, lastPassingTime=0;
    SUMOReal passingSpeed, accel, offset;
    SUMOReal totalTimeLoss = 0.;

    if (MSGlobals::gSemiImplicitEulerUpdate) {
        // euler update
        passingSpeed = veh.getSpeed();
        accel = 0.;
    } else {
        // ballistic update
        passingSpeed = veh.getPreviousSpeed();
        accel = (veh.getSpeed()-veh.getPreviousSpeed())/TS;
    }
    if (oldPos < 0){
        // vehicle entered the detector in the last step
        // calculate entry time, but disregard timeloss for that segment
        passingTime = MSCFModel::passingTime(oldPos, 0., newPos, veh.getPreviousSpeed(), veh.getSpeed());
        passingSpeed = passingSpeed + passingTime*accel;
        lastPassingTime = passingTime;
    }


    // set offset to lane subsequent to the current
    std::size_t oi = vi.currentOffsetIndex;
    // offset to current lane's end, resp. the vehicle's newPos, resp. the detector length measured from vi's entry lane's begin.
    if (oi < myLanes.size()) {
        offset = MIN3(myOffsets[oi] + vi.currentLane->getLength() - vi.entryOffset, newPos, vi.exitOffset + vi.length - vi.entryOffset);
    } else {
        // vehicle has left the detector's lanes, but may still have its back on the detector
        offset = MIN2(newPos, vi.exitOffset + vi.length - vi.entryOffset);
    }
    // calculate vehicle's timeloss on the detector segment of the current lane
    passingTime = MSCFModel::passingTime(oldPos, offset, newPos, veh.getPreviousSpeed(), veh.getSpeed());
    SUMOReal vmax = vi.currentLane->getVehicleMaxSpeed(&veh); // maximal speed on the lane (this is the reason why we need to seperately consider all the lanes)
    SUMOReal timeLoss = calculateSegmentTimeLoss(passingTime, passingSpeed, accel, vmax);
    // add to total time loss
    totalTimeLoss += timeLoss;

    // Treat subsequent lanes to vi->currentLane on the detector if the vehicle entered some
    std::vector<const MSLane*>::const_iterator li = vi.enteredLanes.begin();
    while(li != vi.enteredLanes.end()) {
        oi++;
        // offset to lane's end, resp. the vehicle's newPos, resp. the detector length measured from vi's entry lane's begin.
        if (oi < myOffsets.size() && myLanes[oi] == (*li)->getID()) {
            // Vehicle is on one of the detector's lanes
            offset = MIN3(myOffsets[oi] + (*li)->getLength() - vi.entryOffset, newPos, vi.exitOffset + vi.length - vi.entryOffset);
            // Maximal speed on the lane
            vmax = (*li)->getVehicleMaxSpeed(&veh);
        } else {
            // Vehicle has left the detector's lanes, but may still have its back on the detector
            offset = MIN2(newPos, vi.exitOffset + vi.length - vi.entryOffset);
            // Use maximal speed on the lane with the vehicle's front as reference
            vmax = (*li)->getVehicleMaxSpeed(&veh);
        }
        // calculate vehicle's timeloss on the detector segment of the lane
        passingTime = MSCFModel::passingTime(oldPos, offset, newPos, veh.getPreviousSpeed(), veh.getSpeed());
        timeLoss = calculateSegmentTimeLoss(passingTime, passingSpeed, accel, vmax);
        // add to total time loss
        totalTimeLoss += timeLoss;
        ++li;
    }
    return timeLoss;
}

SUMOReal
MSLaneSequenceDetector::calculateSegmentTimeLoss(SUMOReal timespan, SUMOReal initialSpeed, SUMOReal accel, SUMOReal vmax) {
    return MAX2(0., timespan*(vmax - initialSpeed - 0.5*accel)/vmax);
}



void
MSLaneSequenceDetector::writeXMLDetectorProlog(OutputDevice& dev) const{
    dev.writeXMLHeader("detector", "det_lanesequence_file.xsd");
}

void
MSLaneSequenceDetector::writeXMLOutput(OutputDevice& dev, SUMOTime startTime, SUMOTime stopTime){
    dev << "   <interval begin=\"" << time2string(startTime) << "\" end=\"" << time2string(stopTime) << "\" " << "id=\"" << getID() << "\" ";

    dev << "sampledSeconds=\"" << myVehicleSamples << "\" "
            << "nVehEntered=\"" << myNumberOfEnteredVehicles<< "\" "
            << "nVehLeft=\"" << myNumberOfLeftVehicles<< "\" "
            << "maxVehicleNumber=\"" << myMaxVehicleNumber << "\" "
            << "/>\n";
    reset();
}

void
MSLaneSequenceDetector::reset() {
    myVehicleSamples = 0;
    myTotalTimeLoss = 0.;
    myNumberOfEnteredVehicles = 0;
    myNumberOfLeftVehicles = 0;
    myMaxVehicleNumber = 0;
}


/****************************************************************************/

