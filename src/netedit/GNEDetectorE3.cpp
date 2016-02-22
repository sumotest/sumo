/****************************************************************************/
/// @file    GNEDetectorE3.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Nov 2015
/// @version $Id: GNEDetectorE3.cpp 19861 2016-02-01 09:08:47Z palcraft $
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

#include "GNEDetectorE3.h"
#include "GNEDetectorE3EntryExit.h"
#include "GNELane.h"
#include "GNEViewNet.h"
#include "GNEUndoList.h"
#include "GNENet.h"
#include "GNEChange_Attribute.h"
#include "GNELogo_E3.cpp"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif

// ===========================================================================
// static member definitions
// ===========================================================================
GUIGlID GNEDetectorE3::detectorE3GlID = 0;
bool GNEDetectorE3::detectorE3Initialized = false;

// ===========================================================================
// member method definitions
// ===========================================================================

GNEDetectorE3::GNEDetectorE3(const std::string& id, GNEViewNet* viewNet, Position pos, SUMOReal freq, const std::string& filename) :
    GNEAdditionalSet(id, viewNet, SUMO_TAG_E3DETECTOR),
    myPos(pos),
    myFreq(freq),
    myFilename(filename),
    myCounterId(0) {
    // Update geometry;
    updateGeometry();
    // Set colors
    setColors();
}


GNEDetectorE3::~GNEDetectorE3() {
}


void 
GNEDetectorE3::updateGeometry() {
    myShape.clear();
    myShape.push_back(myPos - Position(1, 0));
    myShape.push_back(myPos + Position(1, 0));
}


void 
GNEDetectorE3::moveAdditional(Position pos, GNEUndoList *undoList) {

}


void 
GNEDetectorE3::writeAdditional(OutputDevice& device) {

}


GUIGLObjectPopupMenu* 
GNEDetectorE3::getPopUpMenu(GUIMainWindow& app, GUISUMOAbstractView& parent) {
    return NULL;
}


GUIParameterTableWindow* 
GNEDetectorE3::getParameterWindow(GUIMainWindow& app, GUISUMOAbstractView& parent) {
    return NULL;
}


void 
GNEDetectorE3::drawGL(const GUIVisualizationSettings& s) const {
    // Additonals element are drawed using a drawGLAdditional
    drawGLAdditional(0, s);
}


void 
GNEDetectorE3::drawGLAdditional(GUISUMOAbstractView* const parent, const GUIVisualizationSettings& s) const {
    // Ignore Warning
    UNUSED_PARAMETER(parent);

    // Declare variables to get colors depending if the detector is selected
    RGBColor base;
    
    // Set colors
    if(gSelected.isSelected(getType(), getGlID())) {
        base = myRGBColors[E3_BASE_SELECTED];
    } else {
        base = myRGBColors[E3_BASE];
    }
    
    // Start drawing adding an gl identificator
    glPushName(getGlID());
    
    // Add a draw matrix
    glPushMatrix();

    // Start with the drawing of the area traslating matrix to origing 
    glTranslated(0, 0, getType());

    // Set color of the base
    GLHelper::setColor(base);

    // Obtain exaggeration of the draw
    const SUMOReal exaggeration = s.addSize.getExaggeration(s);
   
    // Draw the area using shape, shapeRotations, shapeLenghts and value of exaggeration
    GLHelper::drawBoxLine(myShape[0], 0, myShape[0].distanceTo(myShape[1]), 3);

    // Pop name
    glPopName();

    // Draw name
    drawName(getCenteringBoundary().getCenter(), s.scale, s.addName);
}


std::string 
GNEDetectorE3::getAttribute(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_ID:
            return getMicrosimID();
        case SUMO_ATTR_FREQUENCY:
            return toString(myFreq);
        case SUMO_ATTR_FILE:
            return myFilename;
        default:
            throw InvalidArgument("detector E3 attribute '" + toString(key) + "' not allowed");
    }
}


void 
GNEDetectorE3::setAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList) {
    if (value == getAttribute(key)) {
        return; //avoid needless changes, later logic relies on the fact that attributes have changed
    }
    switch (key) {
        case SUMO_ATTR_ID:
            throw InvalidArgument("modifying detector E3 attribute '" + toString(key) + "' not allowed");
        case SUMO_ATTR_FREQUENCY:
        case SUMO_ATTR_FILE:
            undoList->p_add(new GNEChange_Attribute(this, key, value));
            updateGeometry();
            break;
        default:
            throw InvalidArgument("detector E3 attribute '" + toString(key) + "' not allowed");
    }
}


bool 
GNEDetectorE3::isValid(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_ID:
            throw InvalidArgument("modifying detector E3 attribute '" + toString(key) + "' not allowed");
        case SUMO_ATTR_FREQUENCY:
            return (canParse<SUMOReal>(value) && parse<SUMOReal>(value) >= 0);
        case SUMO_ATTR_FILE:
            return isValidFileValue(value);
        case SUMO_ATTR_LINES:
            return isValidStringVector(value);
        default:
            throw InvalidArgument("detector E3 attribute '" + toString(key) + "' not allowed");
    }
}


void 
GNEDetectorE3::setAttribute(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_ID:
        case SUMO_ATTR_LANE:
            throw InvalidArgument("modifying detector E3 attribute '" + toString(key) + "' not allowed");
        case SUMO_ATTR_FREQUENCY:
            myFreq = parse<SUMOReal>(value);
            break;
        case SUMO_ATTR_FILE:
            myFilename = value;
            break;
        default:
            throw InvalidArgument("detector E3 attribute '" + toString(key) + "' not allowed");
    }
}


void 
GNEDetectorE3::setColors() {
    // Color E3_BASE
    myRGBColors.push_back(RGBColor(76, 170, 50, 255));
    // Color E3_BASE_SELECTED
    myRGBColors.push_back(RGBColor(161, 255, 135, 255));

}

/****************************************************************************/