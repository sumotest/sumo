/****************************************************************************/
/// @file    GNEConnection.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Jun 2016
/// @version $Id$
///
// A class for visualizing connections between lanes
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

#include <string>
#include <iostream>
#include <utility>
#include <time.h>
#include <foreign/polyfonts/polyfonts.h>
#include <utils/foxtools/MFXUtils.h>
#include <utils/geom/PositionVector.h>
#include <utils/gui/windows/GUIMainWindow.h>
#include <utils/gui/windows/GUISUMOAbstractView.h>
#include <utils/common/ToString.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/div/GUIParameterTableWindow.h>
#include <utils/gui/globjects/GUIGLObjectPopupMenu.h>
#include <utils/gui/div/GUIGlobalSelection.h>
#include <utils/gui/div/GLHelper.h>
#include <utils/gui/globjects/GLIncludes.h>

#include "GNEConnection.h"
#include "GNEJunction.h"
#include "GNEEdge.h"
#include "GNELane.h"
#include "GNENet.h"
#include "GNEChange_Attribute.h"
#include "GNEUndoList.h"
#include "GNEViewNet.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS



// ===========================================================================
// static member definitions
// ===========================================================================
int NUM_POINTS = 5;

// ===========================================================================
// method definitions
// ===========================================================================

GNEConnection::GNEConnection(GNEEdge *from, int fromLane, GNEEdge *to, int toLane, bool pass, bool keepClear, SUMOReal contPos, bool uncontrolled) :
    myConnection(fromLane, to->getNBEdge(), toLane, pass, keepClear, contPos, false),    
    GNENetElement(from->getNet(), toString(SUMO_TAG_CONNECTION) + from->getID() + toString(fromLane) + to->getID() + toString(toLane), GLO_JUNCTION, SUMO_TAG_CONNECTION),
    myFromEdge(from),
    myUncontrolled(uncontrolled),
    myDrawConnection(true) {
    // Update geometry
    updateGeometry();
}


GNEConnection::GNEConnection(GNEEdge *from, NBEdge::Connection connection, bool uncontrolled) :
    myConnection(connection),
    GNENetElement(from->getNet(), toString(SUMO_TAG_CONNECTION) + from->getID() + toString(connection.fromLane) + connection.toEdge->getID() + toString(connection.toLane), GLO_JUNCTION, SUMO_TAG_CONNECTION),
    myFromEdge(from),
    myUncontrolled(uncontrolled) {
    // Update geometry
    updateGeometry();
}


GNEConnection::~GNEConnection() {}


void
GNEConnection::updateGeometry() {
    myShapeRotations.clear();
    myShapeLengths.clear();
    
    // Get shape of from and to lanes
    PositionVector laneShapeFrom = myFromEdge->getLanes().at(myConnection.fromLane)->getShape();
    PositionVector laneShapeTo = myConnection.toEdge->getLaneShape(myConnection.toLane);

    // Cut shape using as delimitators from start position and end position
    laneShapeFrom = laneShapeFrom.getSubpart(0, laneShapeFrom.length() - 10);
    laneShapeTo = laneShapeTo.getSubpart(10, laneShapeTo.length());

    // Calculate shape
    myShape = myFromEdge->getNBEdge()->getToNode()->computeSmoothShape(laneShapeFrom, laneShapeTo,
                NUM_POINTS, myFromEdge->getNBEdge()->getTurnDestination() == myConnection.toEdge,
                (SUMOReal) 5. * (SUMOReal) myFromEdge->getNBEdge()->getNumLanes(),
                (SUMOReal) 5. * (SUMOReal) myConnection.toEdge->getNumLanes());
    
    int segments = (int) myShape.size() - 1;
    if (segments >= 0) {
        myShapeRotations.reserve(segments);
        myShapeLengths.reserve(segments);
        for (int i = 0; i < segments; ++i) {
            const Position& f = myShape[i];
            const Position& s = myShape[i + 1];
            myShapeLengths.push_back(f.distanceTo2D(s));
            myShapeRotations.push_back((SUMOReal) atan2((s.x() - f.x()), (f.y() - s.y())) * (SUMOReal) 180.0 / (SUMOReal) PI);
        }
    }
}


Boundary 
GNEConnection::getBoundary() const {
    return Boundary();
}


GNEEdge*
GNEConnection::getEdgeFrom() const {
    return myFromEdge;
}


GNEEdge*
GNEConnection::getEdgeTo() const {
    return myFromEdge->getNet()->retrieveEdge(myConnection.toEdge->getID());
}


GNELane* 
GNEConnection::getLaneFrom() const {
    return myFromEdge->getLanes().at(myConnection.fromLane);
}


GNELane* 
GNEConnection::getLaneTo() const {
    return myFromEdge->getNet()->retrieveEdge(myConnection.toEdge->getID())->getLanes().at(myConnection.toLane);
}


int
GNEConnection::getFromLaneIndex() const {
    return myConnection.fromLane;
}


int
GNEConnection::getToLaneIndex() const {
    return myConnection.toLane;
}


bool
GNEConnection::getPass() {
    return myConnection.mayDefinitelyPass;
}


bool
GNEConnection::getKeepClear() {
    return myConnection.keepClear;
}


SUMOReal
GNEConnection::getContPos() {
    return myConnection.contPos;
}


bool
GNEConnection::getUncontrolled() {
    return myUncontrolled;
}


const NBEdge::Connection&
GNEConnection::getNBEdgeConnection() const {
    return myConnection;
}


bool 
GNEConnection::getDrawConnection() const {
    return myDrawConnection;
}


void
GNEConnection::setPass(bool pass) {
    myConnection.mayDefinitelyPass = pass;
}


void
GNEConnection::setKeepClear(bool keepClear) {
    myConnection.keepClear = keepClear;
}


void
GNEConnection::setContPos(SUMOReal contPos) {
    myConnection.contPos = contPos;
}


void
GNEConnection::setUncontrolled(bool uncontrolled) {
    myUncontrolled = uncontrolled ;
}


void 
GNEConnection::setDrawConnection(bool drawConnection) {
    myDrawConnection = drawConnection;
}


GUIGLObjectPopupMenu*
GNEConnection::getPopUpMenu(GUIMainWindow& app, GUISUMOAbstractView& parent) {
    // Currently ignored before implementation to avoid warnings
    UNUSED_PARAMETER(app);
    UNUSED_PARAMETER(parent);
    return NULL;
}


GUIParameterTableWindow*
GNEConnection::getParameterWindow(GUIMainWindow& app, GUISUMOAbstractView& parent) {
    // Currently ignored before implementation to avoid warnings
    UNUSED_PARAMETER(app);
    UNUSED_PARAMETER(parent);
    return NULL;
}


Boundary
GNEConnection::getCenteringBoundary() const {
    return Boundary();
}


void
GNEConnection::drawGL(const GUIVisualizationSettings& s) const {
    // Check if connection must be drawed
    //if((myDrawConnection == true) && (myNet)->getViewNet()->showAttrConnection() == true) {
        glPushMatrix();
        glPushName(getGlID());
        glTranslated(0, 0, GLO_JUNCTION + 0.1); // must draw on top of junction
    //    GLHelper::setColor(colorForLinksState(myState));
        // draw lane
        // check whether it is not too small
        if (s.scale < 1.) {
            GLHelper::drawLine(myShape);
        } else {
            GLHelper::drawBoxLines(myShape, myShapeRotations, myShapeLengths, 0.2);
        }
        glPopName();
        glPopMatrix();
    //}
}


std::string
GNEConnection::getAttribute(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_ID:
            return getMicrosimID();
        case SUMO_ATTR_FROM:
            return myFromEdge->getID();
        case SUMO_ATTR_TO:
            return myConnection.toEdge->getID();
        case SUMO_ATTR_FROM_LANE:
            return toString(myConnection.toLane);
        case SUMO_ATTR_TO_LANE:
            return toString(myConnection.toLane);
        case SUMO_ATTR_PASS:
            return toString(myConnection.mayDefinitelyPass);
        case SUMO_ATTR_KEEP_CLEAR:
            return toString(myConnection.keepClear);
        case SUMO_ATTR_CONTPOS:
            return toString(myConnection.contPos);
        case SUMO_ATTR_UNCONTROLLED:
            return toString(myUncontrolled);
        default:
            throw InvalidArgument("connection attribute '" + toString(key) + "' not allowed");
    }
}


void
GNEConnection::setAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList) {
    switch (key) {
        case SUMO_ATTR_ID:
            throw InvalidArgument("modifying connection attribute '" + toString(key) + "' not allowed");
        case SUMO_ATTR_FROM:
        case SUMO_ATTR_TO:
        case SUMO_ATTR_FROM_LANE:
        case SUMO_ATTR_TO_LANE:
        case SUMO_ATTR_PASS:
        case SUMO_ATTR_KEEP_CLEAR:
        case SUMO_ATTR_CONTPOS:
        case SUMO_ATTR_UNCONTROLLED:
            // no special handling
            undoList->p_add(new GNEChange_Attribute(this, key, value));
            break;
        default:
            throw InvalidArgument("connection attribute '" + toString(key) + "' not allowed");
    }
}


bool
GNEConnection::isValid(SumoXMLAttr key, const std::string& value) {


    // Currently ignored before implementation to avoid warnings
    UNUSED_PARAMETER(key);
    UNUSED_PARAMETER(value);
    return false;
}


void
GNEConnection::setAttribute(SumoXMLAttr key, const std::string& value) {


    // Currently ignored before implementation to avoid warnings
    UNUSED_PARAMETER(key);
    UNUSED_PARAMETER(value);
}


/****************************************************************************/
