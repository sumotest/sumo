/****************************************************************************/
/// @file    GNEDetectorE1.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Nov 2015
/// @version $Id$
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

GNEDetector::GNEDetector(const std::string& id, GNEViewNet* viewNet, SumoXMLTag tag, GNELane* lane, SUMOReal posOverLane, int freq, const std::string& filename, bool blocked, GNEAdditionalSet* parent) :
    GNEAdditional(id, viewNet, Position(posOverLane, 0), tag, parent, blocked),
    myLane(lane),
    myFreq(freq),
    myFilename(filename) {
    myLane->addAdditional(this);
}


GNEDetector::~GNEDetector() {
    if (myLane) {
        myLane->removeAdditionalGeometry(this);
    }
}


void
GNEDetector::moveAdditionalGeometry(SUMOReal offsetx, SUMOReal offsety) {
    // Due a detector is placed over an lane ignore Warning of posy                                                                     // PABLO #501
    UNUSED_PARAMETER(offsety);                                                                                                          // PABLO #501
    // declare start and end positions                                                                                                  // PABLO #501
    SUMOReal startPos = myPosition.x();                                                                                                 // PABLO #501
    SUMOReal endPos = 0;                                                                                                                // PABLO #501
    // set endPos if additional has the attribute lenght                                                                                // PABLO #501
    if(GNEAttributeCarrier::hasAttribute(getTag(), SUMO_ATTR_LENGTH)) {                                                                 // PABLO #501
        endPos = startPos + GNEAttributeCarrier::parse<SUMOReal>(getAttribute(SUMO_ATTR_LENGTH));                                       // PABLO #501
    }                                                                                                                                   // PABLO #501
    // Move to Right if distance is positive, to left if distance is negative                                                           // PABLO #501
    if (((offsetx > 0) && ((endPos + offsetx) < myLane->getLaneShapeLenght())) || ((offsetx < 0) && ((startPos + offsetx) > 0))) {      // PABLO #501
        // change attribute                                                                                                             // PABLO #501
        myPosition.set(myPosition.x() + offsetx, 0);                                                                                    // PABLO #501
        // Update geometry                                                                                                              // PABLO #501
        updateGeometry();                                                                                                               // PABLO #501
    }                                                                                                                                   // PABLO #501
}


void                                                                                                                        // PABLO #501
GNEDetector::commmitAdditionalGeometryMoved(SUMOReal oldPosx, SUMOReal oldPosy, GNEUndoList* undoList) {                    // PABLO #501
    undoList->p_begin("position of " + toString(getTag()));                                                                 // PABLO #501
    undoList->p_add(new GNEChange_Attribute(this, SUMO_ATTR_POSITION, toString(myPosition.x()), true, toString(oldPosx)));  // PABLO #501
    undoList->p_end();                                                                                                      // PABLO #501
    // Refresh element                                                                                                      // PABLO #501
    myViewNet->getNet()->refreshElement(this);                                                                              // PABLO #501
}                                                                                                                           // PABLO #501


GNELane*
GNEDetector::getLane() const {
    return myLane;
}


void
GNEDetector::removeLaneReference() {
    myLane = NULL;
}


SUMOReal
GNEDetector::getPositionOverLane() const {
    return myPosition.x();
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
GNEDetector::setPositionOverLane(SUMOReal pos) {
    if (pos < 0) {
        throw InvalidArgument("Position '" + toString(pos) + "' not allowed. Must be greather than 0");
    } else if (pos > myLane->getLaneShapeLenght()) {
        throw InvalidArgument("Position '" + toString(pos) + "' not allowed. Must be smaller than lane length");
    } else {
        myPosition = Position(pos, 0);
    }
}


void
GNEDetector::setFrequency(int freq) {
    if (freq >= 0) {
        myFreq = freq;
    } else {
        throw InvalidArgument("Frequency '" + toString(freq) + "' not allowed. Must be greather than 0");
    }
}


void
GNEDetector::setFilename(std::string filename) {
    myFilename = filename;
}


void
GNEDetector::changeLane(GNELane* newLane) {
    myLane->removeAdditionalGeometry(this);
    myLane = newLane;
    myLane->addAdditional(this);
    updateGeometry();
    getViewNet()->update();
}


const std::string&
GNEDetector::getParentName() const {
    return myLane->getMicrosimID();
}


void
GNEDetector::drawDetectorIcon(const int GNELogoID, SUMOReal sizex, SUMOReal sizey) const {
    // Add a draw matrix
    glPushMatrix();
    // Traslate to center
    glTranslated(myShape.getLineCenter().x(), myShape.getLineCenter().y(), getType() + 0.1);
    // Set color
    glColor3d(1, 1, 1);
    // Set first rotation
    glRotated(myBlockIconRotation, 0, 0, -1);
    // Set second rotation
    glRotated(180, 0, 0, 1);
    //Traslate offset
    glTranslated(myDetectorLogoOffset.x(), myDetectorLogoOffset.y(), 0);
    // Draw detector logo
    GUITexturesHelper::drawTexturedBox(GNELogoID, sizex, sizey, (-1 * sizex), (-1 * sizey));
    // Pop detector logo matrix
    glPopMatrix();
}

/****************************************************************************/
