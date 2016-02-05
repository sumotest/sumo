/****************************************************************************/
/// @file    GNEDetectorE1.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Nov 2015
/// @version $Id: GNEDetectorE1.cpp 19861 2016-02-01 09:08:47Z palcraft $
///
/// 
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

#include "GNEDetector.h"
#include "GNELane.h"
#include "GNEChange_Attribute.h"
#include "GNEUndoList.h"
#include "GNENet.h"
#include "GNEViewNet.h"


#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif


// ===========================================================================
// member method definitions
// ===========================================================================

GNEDetector::GNEDetector(const std::string& id, GNELane& lane, GNEViewNet* viewNet, SumoXMLTag tag, SUMOReal pos, int freq, const std::string &filename) :
    GNEAdditional(id, lane, viewNet, tag),
    myPos(pos),
    myFreq(freq),
    myFilename(filename) {
    // Update geometry
    updateGeometry();
}


GNEDetector::~GNEDetector() {}


void 
GNEDetector::updateGeometry() {
    // Set Shape
    myShape = PositionVector(myLane.getShape().positionAtOffset(myPos - 1), myLane.getShape().positionAtOffset(myPos + 1));

    // Set rotation of shape
    myRotation = (SUMOReal) atan2((myShape[1].x() - myShape[0].x()), (myShape[0].y() - myShape[1].y())) * (SUMOReal) 180.0 / (SUMOReal) PI;

    // Get value of option "lefthand"
    SUMOReal offsetSign = OptionsCont::getOptions().getBool("lefthand") ? -1 : 1;

    // Set position of the block icon
    myBlockIconPos = myShape.getLineCenter();

    // If lenght of the shape is distint to 0
    if (myShape.length() != 0) {

        // Obtain rotation
        myRotation = myShape.rotationDegreeAtOffset(SUMOReal((myShape.length() / 2.)));

        // correct orientation
        myRotation -= 90;
    }
    else
        // Value of rotation is 0
        myRotation = 0;
}


void 
GNEDetector::moveAdditional(SUMOReal distance, GNEUndoList *undoList) {
    // if item isn't blocked
    if(myBlocked == false) {
        // Move to Right if distance is positive, to left if distance is negative
        if( ((distance > 0) && ((myPos + distance) < myLane.getLaneShapeLenght())) || ((distance < 0) && ((myPos + distance) > 0)) ) {
            // change attribute
            undoList->p_add(new GNEChange_Attribute(this, SUMO_ATTR_STARTPOS, toString(myPos + distance)));
        }
    }
}


SUMOReal 
GNEDetector::getPosition() const {
    return myPos;
}


int 
GNEDetector::getFrequency() const {
    return myFreq;
}


std::string 
GNEDetector::getFilename() const {
    return myFilename;
}


void 
GNEDetector::setPosition(SUMOReal pos) {
    if(pos < 0)
        throw InvalidArgument("Position '" + toString(pos) + "' not allowed. Must be greather than 0");
    else if(pos > myLane.getLaneShapeLenght())
        throw InvalidArgument("Position '" + toString(pos) + "' not allowed. Must be smaller than lane length");
    else
        myPos = pos;
}


void 
GNEDetector::setFrequency(int freq) {
    if (freq >= 0)
        myFreq = freq;
    else
        throw InvalidArgument("Frequency '" + toString(freq) + "' not allowed. Must be greather than 0");
}


void 
GNEDetector::setFilename(std::string filename) {
    myFilename = filename;
}

/****************************************************************************/