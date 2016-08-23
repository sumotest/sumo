/****************************************************************************/
/// @file    GNEChange_LaneTransformation.cpp
/// @author  Pablo Alvarez Lopez
/// @date    August 2016
/// @version $Id: GNEChange_LaneTransformation.cpp 21230 2016-07-25 11:10:42Z palcraft $
///
// A network change in which a single lane is transformated
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

#include <cassert>
#include <utils/common/MsgHandler.h>
#include "GNEChange_LaneTransformation.h"
#include "GNEEdge.h"
#include "GNELane.h"
#include "GNENet.h"
#include "GNEAdditionalSet.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif


// ===========================================================================
// FOX-declarations
// ===========================================================================
FXIMPLEMENT_ABSTRACT(GNEChange_LaneTransformation, GNEChange, NULL, 0)

// ===========================================================================
// member method definitions
// ===========================================================================


// Constructor for creating an edge
GNEChange_LaneTransformation::GNEChange_LaneTransformation(GNELane* lane, SUMOVehicleClass typeOfTransformation, bool forward):
    GNEChange(0, forward),
    myLane(lane),
    myTypeOfTransformation(typeOfTransformation) {
}


GNEChange_LaneTransformation::~GNEChange_LaneTransformation() {

}


void 
GNEChange_LaneTransformation::undo() {
    if (myForward) {
        switch (myTypeOfTransformation) {
            case SVC_BICYCLE:
                undoTransformLaneToBikeLane();
                break;
            case SVC_PEDESTRIAN:
                undoTransformLaneToSidewalk();
                break;
            case SVC_BUS:
                undoTransformLaneToBuslane();
                break;
            default:
                break;
        }
    } else {
        switch (myTypeOfTransformation) {
            case SVC_BICYCLE:
                doTransformLaneToBikeLane();
                break;
            case SVC_PEDESTRIAN:
                doTransformLaneToSidewalk();
                break;
            case SVC_BUS:
                doTransformLaneToBuslane();
                break;
            default:
                break;
        }
    }
}


void 
GNEChange_LaneTransformation::redo() {
    if (myForward) {
        switch (myTypeOfTransformation) {
            case SVC_BICYCLE:
                doTransformLaneToBikeLane();
                break;
            case SVC_PEDESTRIAN:
                doTransformLaneToSidewalk();
                break;
            case SVC_BUS:
                doTransformLaneToBuslane();
                break;
            default:
                break;
        }
    } else {
        switch (myTypeOfTransformation) {
            case SVC_BICYCLE:
                undoTransformLaneToBikeLane();
                break;
            case SVC_PEDESTRIAN:
                undoTransformLaneToSidewalk();
                break;
            case SVC_BUS:
                undoTransformLaneToBuslane();
                break;
            default:
                break;
        }
    }
}


FXString 
GNEChange_LaneTransformation::undoName() const {
    if (myForward) {
        switch (myTypeOfTransformation) {
            case SVC_BICYCLE:
                return ("Undo transform to bikelane");
                break;
            case SVC_PEDESTRIAN:
                return ("Undo transform to sidewalk");
                break;
            case SVC_BUS:
                return ("Undo transform to buslane");
                break;
            default:
                return ("Undo not supported");
                break;
        }
    } else {
        switch (myTypeOfTransformation) {
            case SVC_BICYCLE:
                return ("Undo remain bikelane");
                break;
            case SVC_PEDESTRIAN:
                return ("Undo remain sidewalk");
                break;
            case SVC_BUS:
                return ("Undo remain buslane");
                break;
            default:
                return ("Undo not supported");
                break;
        }
    }
}


FXString 
GNEChange_LaneTransformation::redoName() const {
    if (myForward) {
        switch (myTypeOfTransformation) {
            case SVC_BICYCLE:
                return ("Redo transform to bikelane");
                break;
            case SVC_PEDESTRIAN:
                return ("Redo transform to sidewalk");
                break;
            case SVC_BUS:
                return ("Redo transform to buslane");
                break;
            default:
                return ("Redo not supported");
                break;
        }
    } else {
        switch (myTypeOfTransformation) {
            case SVC_BICYCLE:
                return ("Redo remain bikelane");
                break;
            case SVC_PEDESTRIAN:
                return ("Redo remain sidewalk");
                break;
            case SVC_BUS:
                return ("Redo remain buslane");
                break;
            default:
                return ("Redo not supported");
                break;
        }
    }
}


void 
GNEChange_LaneTransformation::doTransformLaneToSidewalk() {
    // get NBEdge of lane
    NBEdge *nb = myLane->getParentEdge().getNBEdge();
    // Save current values of NBEdge
    myVectorOfLanes = nb->getLanes();
    myGeometry = nb->getGeometry();
    myConnections = nb->getConnections();
    // add Sidewalk
    nb->addSidewalk(nb->getLaneWidth());
    // Refresh element
    myLane->getNet()->refreshElement(myLane);
}


void 
GNEChange_LaneTransformation::undoTransformLaneToSidewalk() {
    // Restore values of lane
    myLane->getParentEdge().getNBEdge()->restoreSidewalk(myVectorOfLanes, myGeometry, myConnections);
    // Refresh element
    myLane->getNet()->refreshElement(myLane);
}


void 
GNEChange_LaneTransformation::doTransformLaneToBikeLane() {
    // get NBEdge of lane
    NBEdge *nb = myLane->getParentEdge().getNBEdge();
    // Save current values of NBEdge
    myVectorOfLanes = nb->getLanes();
    myGeometry = nb->getGeometry();
    myConnections = nb->getConnections();
    // add Sidewalk
    nb->addBikeLane(nb->getLaneWidth());
    // Refresh element
    myLane->getNet()->refreshElement(myLane);
}


void 
GNEChange_LaneTransformation::undoTransformLaneToBikeLane() {
    // Restore values of lane
    myLane->getParentEdge().getNBEdge()->restoreBikelane(myVectorOfLanes, myGeometry, myConnections);
    // Refresh element
    myLane->getNet()->refreshElement(myLane);
}


void 
GNEChange_LaneTransformation::doTransformLaneToBuslane() {
    std::cout << "doTransformLaneToBuslane not implemented" << std::endl;
}


void 
GNEChange_LaneTransformation::undoTransformLaneToBuslane() {
    std::cout << "undoTransformLaneToBuslane not implemented" << std::endl;
}