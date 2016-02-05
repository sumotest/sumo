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

#include "GNEDetectorE1.h"
#include "GNELane.h"
#include "GNEViewNet.h"
#include "GNEUndoList.h"
#include "GNENet.h"
#include "GNEChange_Attribute.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif

// ===========================================================================
// static member definitions
// ===========================================================================
GUIGlID GNEDetectorE1::detectorE1GlID = 0;
bool GNEDetectorE1::detectorE1Initialized = false;

// ===========================================================================
// member method definitions
// ===========================================================================

GNEDetectorE1::GNEDetectorE1(const std::string& id, GNELane& lane, GNEViewNet* viewNet, SUMOReal pos, SUMOReal freq, const std::string& filename, bool splitByType) :
    GNEDetector(id, lane, viewNet, SUMO_TAG_E1DETECTOR, pos, freq, filename),
    mySplitByType(splitByType) {
    // Set colors of detector
    setColors();
}


GNEDetectorE1::~GNEDetectorE1() {
}


void 
GNEDetectorE1::writeAdditional(OutputDevice& device) {
    // Write parameters
    device.openTag(getTag());
    device.writeAttr(SUMO_ATTR_ID, getID());
    device.writeAttr(SUMO_ATTR_LANE, getLane().getID());
    device.writeAttr(SUMO_ATTR_POSITION, myPos);
    device.writeAttr(SUMO_ATTR_FREQUENCY, myFreq);
    device.writeAttr(SUMO_ATTR_FILE, myFilename);
    // Rest of parameters
    // Close tag
    device.closeTag();
}


bool 
GNEDetectorE1::getSplitByType() const {
    return mySplitByType;
}


GUIGLObjectPopupMenu* 
GNEDetectorE1::getPopUpMenu(GUIMainWindow& app, GUISUMOAbstractView& parent) {
    GUIGLObjectPopupMenu* ret = new GUIGLObjectPopupMenu(app, parent, *this);
    buildPopupHeader(ret, app);
    buildCenterPopupEntry(ret);
    new FXMenuCommand(ret, "Copy detector E1 name to clipboard", 0, ret, MID_COPY_EDGE_NAME);
    buildNameCopyPopupEntry(ret);
    buildSelectionPopupEntry(ret);
    buildPositionCopyEntry(ret, false);
    // buildShowParamsPopupEntry(ret, false);
    const SUMOReal pos = myShape.nearest_offset_to_point2D(parent.getPositionInformation());
    new FXMenuCommand(ret, ("pos: " + toString(pos)).c_str(), 0, 0, 0);
    // new FXMenuSeparator(ret);
    // buildPositionCopyEntry(ret, false);
    // let the GNEViewNet store the popup position
    (dynamic_cast<GNEViewNet&>(parent)).markPopupPosition();
    return ret;
}


GUIParameterTableWindow* 
GNEDetectorE1::getParameterWindow(GUIMainWindow& app, GUISUMOAbstractView& parent) {
    GUIParameterTableWindow* ret =
        new GUIParameterTableWindow(app, *this, 2);
    /* not supported yet
    // add items
    ret->mkItem("length [m]", false, getLane().getParentEdge().getNBEdge()->getLength());
    // close building
    ret->closeBuilding();
    */
    return ret;
}


void 
GNEDetectorE1::drawGL(const GUIVisualizationSettings& s) const {
    // Additonals element are drawed using a drawGLAdditional
    drawGLAdditional(0, s);
}


void 
GNEDetectorE1::drawGLAdditional(GUISUMOAbstractView* const parent, const GUIVisualizationSettings& s) const {
    
    // Ignore Warning
    UNUSED_PARAMETER(parent);
    
    // Declare variables to get colors depending if the detector is selected
    RGBColor base;

    // Set colors
    if(gSelected.isSelected(getType(), getGlID())) {
        base = myRGBColors[E1_BASE_SELECTED];
    } else {
        base = myRGBColors[E1_BASE];
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
    GLHelper::drawBoxLine(myShape[0], myRotation, myShape[0].distanceTo(myShape[1]), exaggeration);

    // Check if the distance is enought to draw details
    if (s.scale * exaggeration >= 10) {
        
        ;
    }

    // Pop name
    glPopName();

    // Draw name
    drawName(getCenteringBoundary().getCenter(), s.scale, s.addName);
}


std::string 
GNEDetectorE1::getAttribute(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_ID:
            return getMicrosimID();
        case SUMO_ATTR_LANE:
            return toString(myLane.getAttribute(SUMO_ATTR_ID));
        case SUMO_ATTR_POSITION:
            return toString(myPos);
        case SUMO_ATTR_FREQUENCY:
            return toString(myFreq);
        case SUMO_ATTR_FILE:
            return myFilename;
        ///case SUMO_ATTR_LINES:
        
        default:
            throw InvalidArgument("detector E1 attribute '" + toString(key) + "' not allowed");
    }
}


void 
GNEDetectorE1::setAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList) {
if (value == getAttribute(key)) {
        return; //avoid needless changes, later logic relies on the fact that attributes have changed
    }
    switch (key) {
        case SUMO_ATTR_ID:
        case SUMO_ATTR_LANE:
            throw InvalidArgument("modifying detector E1 attribute '" + toString(key) + "' not allowed");
        case SUMO_ATTR_POSITION:
        case SUMO_ATTR_FREQUENCY:
        case SUMO_ATTR_FILE:
        /// case SUMO_ATTR_FILE (RESTO DE ATRIBUTOS):
            undoList->p_add(new GNEChange_Attribute(this, key, value));
            updateGeometry();
            break;
        default:
            throw InvalidArgument("detector E1 attribute '" + toString(key) + "' not allowed");
    }

}


bool 
GNEDetectorE1::isValid(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_ID:
        case SUMO_ATTR_LANE:
            throw InvalidArgument("modifying detector E1 attribute '" + toString(key) + "' not allowed");
        case SUMO_ATTR_POSITION:
            return (canParse<SUMOReal>(value) && parse<SUMOReal>(value) >= 0 && parse<SUMOReal>(value) <= (myLane.getLaneParametricLenght()));
        case SUMO_ATTR_FREQUENCY:
            return (canParse<SUMOReal>(value) && parse<SUMOReal>(value) >= 0);
        case SUMO_ATTR_FILE:
            return canParse<std::string>(value);
        /// case SUMO_ATTR_FILE (RESTO DE ATRIBUTOS):
        default:
            throw InvalidArgument("detector E1 attribute '" + toString(key) + "' not allowed");
    }
}

// ===========================================================================
// private
// ===========================================================================

void
GNEDetectorE1::setAttribute(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_ID:
        case SUMO_ATTR_LANE:
            throw InvalidArgument("modifying detector E1 attribute '" + toString(key) + "' not allowed");
        case SUMO_ATTR_POSITION:
            myPos = parse<SUMOReal>(value);
            updateGeometry();
            getViewNet()->update();
            break;
        case SUMO_ATTR_FREQUENCY:
            myFreq = parse<SUMOReal>(value);
            break;
        case SUMO_ATTR_FILE:
            myFilename = parse<std::string>(value);
            break;
        /// case SUMO_ATTR_FILE (RESTO DE ATRIBUTOS):
        default:
            throw InvalidArgument("detector E1 attribute '" + toString(key) + "' not allowed");
    }
}


void
GNEDetectorE1::setColors() {
    // Color E1_BASE
    myRGBColors.push_back(RGBColor(76, 170, 50, 255));  /** PROVISIONAL **/
    // Color E1_BASE_SELECTED
    myRGBColors.push_back(RGBColor(161, 255, 135, 255));
}


/****************************************************************************/