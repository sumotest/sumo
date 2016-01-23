/****************************************************************************/
/// @file    MSPModel_NonInteracting_GRPC.h
/// @author  Jakob Erdmann
/// @author  Gregor Laemmel
///
// The pedestrian following model (prototype)
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

// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <math.h>
#include <algorithm>
#include <utils/common/RandHelper.h>
#include <utils/geom/GeomHelper.h>
#include <utils/options/OptionsCont.h>
#include <microsim/MSNet.h>
#include <microsim/MSEdge.h>
#include <microsim/MSLane.h>
#include <microsim/MSJunction.h>
#include "MSPModel_NonInteracting_GRPC.h"


// ===========================================================================
// DEBUGGING HELPERS
// ===========================================================================
//
#define DEBUG1 "disabled"
#define DEBUG2 "disabled"
#define DEBUGCOND(PEDID) (PEDID == DEBUG1 || PEDID == DEBUG2)

// ===========================================================================
// named (internal) constants
// ===========================================================================


// ===========================================================================
// static members
// ===========================================================================


// ===========================================================================
// MSPModel_NonInteracting method definitions
// ===========================================================================

MSPModel_NonInteracting_GRPC::MSPModel_NonInteracting_GRPC(const OptionsCont& oc, MSNet* net) :
    myNet(net) {
    assert(myNet != 0);
    UNUSED_PARAMETER(oc);
}


MSPModel_NonInteracting_GRPC::~MSPModel_NonInteracting_GRPC() {
}


PedestrianState*
MSPModel_NonInteracting_GRPC::add(MSPerson* person, MSPerson::MSPersonStage_Walking* stage, SUMOTime now) {
	MSGRPCClient client(grpc::CreateChannel("localhost:50051", grpc::InsecureChannelCredentials()));
    PState* state = new PState(&client);
    const SUMOTime firstEdgeDuration = state->computeWalkingTime(0, *stage, now);
    myNet->getBeginOfTimestepEvents()->addEvent(new MoveToNextEdge(person, *stage),
            now + firstEdgeDuration, MSEventControl::ADAPT_AFTER_EXECUTION);

    //if DEBUGCOND(person->getID()) std::cout << SIMTIME << " " << person->getID() << " inserted on " << stage->getEdge()->getID() << "\n";
    return state;
}


bool
MSPModel_NonInteracting_GRPC::blockedAtDist(const MSLane*, SUMOReal, std::vector<const MSPerson*>*) {
    return false;
}


SUMOTime
MSPModel_NonInteracting_GRPC::MoveToNextEdge::execute(SUMOTime currentTime) {
    PState* state = dynamic_cast<PState*>(myParent.getPedestrianState());
    const MSEdge* old = myParent.getEdge();
    const bool arrived = myParent.moveToNextEdge(myPerson, currentTime);
    if (arrived) {
        // walk finished. clean up state
        delete state;
        //if DEBUGCOND(myPerson->getID()) std::cout << SIMTIME << " " << myPerson->getID() << " arrived on " << old->getID() << "\n";
        return 0;
    } else {
        //if DEBUGCOND(myPerson->getID()) std::cout << SIMTIME << " " << myPerson->getID() << " moves to " << myParent.getEdge()->getID() << "\n";
        return state->computeWalkingTime(old, myParent, currentTime);
    }
}


SUMOTime
MSPModel_NonInteracting_GRPC::PState::computeWalkingTime(const MSEdge* prev, const MSPerson::MSPersonStage_Walking& stage, SUMOTime currentTime) {
	SUMOTime t = grpcClient->computeWalkingTime(prev,stage,currentTime);
    return t;
}


SUMOReal
MSPModel_NonInteracting_GRPC::PState::getEdgePos(const MSPerson::MSPersonStage_Walking&, SUMOTime now) const {
    //std::cout << SIMTIME << " pos=" << (myCurrentBeginPos + (myCurrentEndPos - myCurrentBeginPos) / myCurrentDuration * (now - myLastEntryTime)) << "\n";
    return myCurrentBeginPos + (myCurrentEndPos - myCurrentBeginPos) / myCurrentDuration * (now - myLastEntryTime);
}


Position
MSPModel_NonInteracting_GRPC::PState::getPosition(const MSPerson::MSPersonStage_Walking& stage, SUMOTime now) const {
    const MSLane* lane = getSidewalk(stage.getEdge());
    const SUMOReal lateral_offset = lane->allowsVehicleClass(SVC_PEDESTRIAN) ? 0 : SIDEWALK_OFFSET;
    return stage.getLanePosition(lane, getEdgePos(stage, now), lateral_offset);
}


SUMOReal
MSPModel_NonInteracting_GRPC::PState::getAngle(const MSPerson::MSPersonStage_Walking& stage, SUMOTime now) const {
    //std::cout << SIMTIME << " rawAngle=" << stage.getEdgeAngle(stage.getEdge(), getEdgePos(stage, now)) << " angle=" << stage.getEdgeAngle(stage.getEdge(), getEdgePos(stage, now)) + (myCurrentEndPos < myCurrentBeginPos ? 180 : 0) << "\n";
    SUMOReal angle = stage.getEdgeAngle(stage.getEdge(), getEdgePos(stage, now)) + (myCurrentEndPos < myCurrentBeginPos ? M_PI : 0);
    if (angle > M_PI) {
        angle -= 2 * M_PI;
    }
    return angle;
}


SUMOTime
MSPModel_NonInteracting_GRPC::PState::getWaitingTime(const MSPerson::MSPersonStage_Walking&, SUMOTime) const {
    return 0;
}


SUMOReal
MSPModel_NonInteracting_GRPC::PState::getSpeed(const MSPerson::MSPersonStage_Walking& stage) const {
    return stage.getMaxSpeed();
}


const MSEdge*
MSPModel_NonInteracting_GRPC::PState::getNextEdge(const MSPerson::MSPersonStage_Walking& stage) const {
    return stage.getNextRouteEdge();
}

/****************************************************************************/
