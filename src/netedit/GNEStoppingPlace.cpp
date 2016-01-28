/****************************************************************************/
/// @file    GNEStoppingPlace.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Dec 2015
/// @version $Id$
///
/// A abstract class to define common parameters of lane area in which vehicles can halt (GNE version)
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
// Copyright (C) 2001-2013 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 3 of the License, or
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
#include <foreign/polyfonts/polyfonts.h>
#include <utils/geom/PositionVector.h>
#include <utils/common/RandHelper.h>
#include <utils/common/SUMOVehicleClass.h>
#include <utils/common/ToString.h>
#include <utils/geom/GeomHelper.h>
#include <utils/gui/windows/GUISUMOAbstractView.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/images/GUIIconSubSys.h>
#include <utils/gui/div/GUIParameterTableWindow.h>
#include <utils/gui/globjects/GUIGLObjectPopupMenu.h>
#include <utils/gui/div/GUIGlobalSelection.h>
#include <utils/gui/div/GLHelper.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/images/GUITexturesHelper.h>
#include <utils/xml/SUMOSAXHandler.h>

#include "GNEStoppingPlace.h"
#include "GNELane.h"
#include "GNEEdge.h"
#include "GNEJunction.h"
#include "GNEUndoList.h"
#include "GNENet.h"
#include "GNEChange_Attribute.h"
#include "GNEViewNet.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif


// ===========================================================================
// member method definitions
// ===========================================================================

GNEStoppingPlace::GNEStoppingPlace(const std::string& id, GNELane& lane, GNEViewNet* viewNet, SumoXMLTag tag, SUMOReal fromPos, SUMOReal toPos) :
    GNEAdditional(id, lane, viewNet, tag),
    myFromPos(fromPos),
    myToPos(toPos){
}


GNEStoppingPlace::~GNEStoppingPlace() {
}


void 
GNEStoppingPlace::updateGeometry() {

    // Clear all containers
    myShapeRotations.clear();
    myShapeLengths.clear();
    
    // Clear shape
    myShape.clear();

    // Get value of option "lefthand"
    SUMOReal offsetSign = OptionsCont::getOptions().getBool("lefthand");

    // Get shape of lane parent
    myShape = myLane.getShape();
    
    // Move shape to side
    myShape.move2side(1.65 * offsetSign);

    // Cut shape using as delimitators from start position and end position
    myShape = myShape.getSubpart(myFromPos, myToPos);

    // Get number of parts of the shape
    int numberOfSegments = (int) myShape.size() - 1;

    // If number of segments is more than 0
    if(numberOfSegments >= 0) {

        // Reserve memory (To improve efficiency)
        myShapeRotations.reserve(numberOfSegments);
        myShapeLengths.reserve(numberOfSegments);

        // For every part of the shape
        for (int i = 0; i < numberOfSegments; ++i) {

            // Obtain first position
            const Position& f = myShape[i];

            // Obtain next position
            const Position& s = myShape[i + 1];

            // Save distance between position into myShapeLengths
            myShapeLengths.push_back(f.distanceTo(s));

            // Save rotation (angle) of the vector constructed by points f and s
            myShapeRotations.push_back((SUMOReal) atan2((s.x() - f.x()), (f.y() - s.y())) * (SUMOReal) 180.0 / (SUMOReal) PI);
        }
    }

    // Obtain a copy of the shape
    PositionVector tmpShape = myShape;

    // Move shape to side 
    tmpShape.move2side(1.5 * offsetSign);

    // Get position of the sing
    mySignPos = tmpShape.getLineCenter();

    // If lenght of the shape is distint to 0
    if (tmpShape.length() != 0) {

        // Obtain rotation of signal rot
        mySignRot = myShape.rotationDegreeAtOffset(SUMOReal((myShape.length() / 2.)));

        // correct orientation
        mySignRot -= 90;
    }
    else
        // Value of signal rotation is 0
        mySignRot = 0;
}


void 
GNEStoppingPlace::moveAdditional(SUMOReal distance, GNEUndoList *undoList) {
    // if item isn't blocked
    if(myBlocked == false) {
        // Move to Right if distance is positive, to left if distance is negative
        if( ((distance > 0) && ((myToPos + distance) < myLane.getLength())) || ((distance < 0) && ((myFromPos + distance) > 0)) ) {
            // change attribute
            undoList->p_add(new GNEChange_Attribute(this, SUMO_ATTR_STARTPOS, toString(myFromPos + distance)));
            undoList->p_add(new GNEChange_Attribute(this, SUMO_ATTR_ENDPOS, toString(myToPos + distance)));
        }
    }
}


SUMOReal 
GNEStoppingPlace::getFromPosition() const {
    return myFromPos;
}

        
SUMOReal 
GNEStoppingPlace::getToPosition() const {
    return myToPos;
}


void 
GNEStoppingPlace::setFromPosition(SUMOReal fromPos) {
    if(fromPos < 0)
        throw InvalidArgument("From position '" + toString(fromPos) + "' not allowed. Must be greather than 0");
    else if(fromPos >= myToPos)
        throw InvalidArgument("From position '" + toString(fromPos) + "' not allowed. Must be smaller than than ToPos '" + toString(myToPos) + "'");
    else if ((myToPos - fromPos) < 1)
        throw InvalidArgument("From position '" + toString(fromPos) + "' not allowed. Lenght of StoppingPlace must be equal or greather than 1");
    else
        myFromPos = fromPos;
}


void 
GNEStoppingPlace::setToPosition(SUMOReal toPos) {
    if(toPos > myLane.getLength())
        throw InvalidArgument("To position '" + toString(toPos) + "' not allowed. Must be smaller than lane length");
    else if(myFromPos >= toPos)
        throw InvalidArgument("To position '" + toString(toPos) + "' not allowed. Must be smaller than than ToPos '" + toString(myToPos) + "'");
    else if ((toPos - myFromPos) < 1)
        throw InvalidArgument("To position '" + toString(toPos) + "' not allowed. Lenght of StoppingPlace must be equal or greather than 1");
    else
        myToPos = toPos;
}


/****************************************************************************/
