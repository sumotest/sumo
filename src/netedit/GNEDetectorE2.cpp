/****************************************************************************/
/// @file    GNEDetectorE2.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Nov 2015
/// @version $Id: GNEDetectorE2.cpp 19861 2016-02-01 09:08:47Z palcraft $
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

#include "GNEDetectorE2.h"
#include "GNELane.h"
#include "GNEViewNet.h"
#include "GNEUndoList.h"
#include "GNENet.h"
#include "GNEChange_Attribute.h"
#include "GNELogo_E2.cpp"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif

// ===========================================================================
// static member definitions
// ===========================================================================
GUIGlID GNEDetectorE2::detectorE2GlID = 0;
bool GNEDetectorE2::detectorE2Initialized = false;

// ===========================================================================
// member method definitions
// ===========================================================================

GNEDetectorE2::GNEDetectorE2(const std::string& id, GNELane& lane, GNEViewNet* viewNet, SUMOReal pos, SUMOReal length, SUMOReal freq, const std::string& filename, 
                             bool cont, int timeThreshold, SUMOReal speedThreshold, SUMOReal jamThreshold, bool blocked) : 
    GNEDetector(id, viewNet, SUMO_TAG_E2DETECTOR, lane, pos, freq, filename, blocked),
    myLength(length),
    myCont(cont),
    myTimeThreshold(timeThreshold),
    mySpeedThreshold(speedThreshold),
    myJamThreshold(jamThreshold) {
    // Update geometry;
    updateGeometry();
    // Set colors of detector
    setColors();
}


GNEDetectorE2::~GNEDetectorE2() {
}


void 
GNEDetectorE2::updateGeometry() {
    // Clear all containers
    myShapeRotations.clear();
    myShapeLengths.clear();
   
    // Get shape of lane parent
    myShape = myLane.getShape();

    // Cut shape using as delimitators myPos and their length (myPos + length)
    myShape = myShape.getSubpart(myLane.getPositionRelativeToParametricLenght(myPosOverLane), myLane.getPositionRelativeToParametricLenght(myPosOverLane + myLength));

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

    // Set position of logo
    myDetectorLogoPosition = myShape.getLineCenter() - Position(0.5, 0);

    // Set position of the block icon
    myBlockIconPos = myShape.getLineCenter() + Position(0.5, 0);

    // Get value of option "lefthand"
    SUMOReal offsetSign = OptionsCont::getOptions().getBool("lefthand") ? -1 : 1;

    // Set rotation of the detector icon
    mySignRotation = (myRotation * offsetSign) - 90;
}


void 
GNEDetectorE2::writeAdditional(OutputDevice& device) {
    // Write parameters
    device.openTag(getTag());
    device.writeAttr(SUMO_ATTR_ID, getID());
    device.writeAttr(SUMO_ATTR_LANE, getLane().getID());
    device.writeAttr(SUMO_ATTR_POSITION, myPosOverLane);
    device.writeAttr(SUMO_ATTR_FREQUENCY, myFreq);
    device.writeAttr(SUMO_ATTR_FILE, myFilename);
    // Rest of parameters
    // Close tag
    device.closeTag();
}


GUIGLObjectPopupMenu* 
GNEDetectorE2::getPopUpMenu(GUIMainWindow& app, GUISUMOAbstractView& parent) {
    GUIGLObjectPopupMenu* ret = new GUIGLObjectPopupMenu(app, parent, *this);
    buildPopupHeader(ret, app);
    buildCenterPopupEntry(ret);
    new FXMenuCommand(ret, "Copy detector E2 name to clipboard", 0, ret, MID_COPY_EDGE_NAME);
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
GNEDetectorE2::getParameterWindow(GUIMainWindow& app, GUISUMOAbstractView& parent) {
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
GNEDetectorE2::drawGL(const GUIVisualizationSettings& s) const {
    // Additonals element are drawed using a drawGLAdditional
    drawGLAdditional(0, s);
}


void 
GNEDetectorE2::drawGLAdditional(GUISUMOAbstractView* const parent, const GUIVisualizationSettings& s) const {
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
    GLHelper::drawBoxLines(myShape, myShapeRotations, myShapeLengths, exaggeration);

    // Pop last matrix
    glPopMatrix();

    // Check if the distance is enought to draw details
    if (s.scale * exaggeration >= 10) {

        // Add a draw matrix
        glPushMatrix();

        // load detector logo, if wasn't inicializated
        if (!detectorE2Initialized) {
            FXImage* i = new FXGIFImage(getViewNet()->getNet()->getApp(), GNELogo_E2, IMAGE_KEEP | IMAGE_SHMI | IMAGE_SHMP);
            detectorE2GlID = GUITexturesHelper::add(i);
            detectorE2Initialized = true;
            delete i;
        }

        // draw detector logo
        glPushMatrix();
        glTranslated(myDetectorLogoPosition.x(), myDetectorLogoPosition.y(), getType() + 0.1);
        glColor3d(1, 1, 1);
        glRotated(180, 0, 0, 1);
        GUITexturesHelper::drawTexturedBox(detectorE2GlID, 0.5);
        glPopMatrix();

        // Show Lock icon depending of the Edit mode
        if(dynamic_cast<GNEViewNet*>(parent)->showLockIcon())
            drawLockIcon();
    }

    // Draw name
    drawName(getCenteringBoundary().getCenter(), s.scale, s.addName);
    
    // Pop name
    glPopName();
}


std::string 
GNEDetectorE2::getAttribute(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_ID:
            return getMicrosimID();
        case SUMO_ATTR_LANE:
            return toString(myLane.getAttribute(SUMO_ATTR_ID));
        case SUMO_ATTR_POSITION:
            return toString(myPosOverLane);
        case SUMO_ATTR_FREQUENCY:
            return toString(myFreq);
        case SUMO_ATTR_FILE:
            return myFilename;
        case SUMO_ATTR_CONT:
            return (myCont? "true" : "false");
        case SUMO_ATTR_HALTING_TIME_THRESHOLD:
            return toString(myTimeThreshold);
        case SUMO_ATTR_HALTING_SPEED_THRESHOLD:
            return toString(mySpeedThreshold);
        case SUMO_ATTR_JAM_DIST_THRESHOLD:
            return toString(myJamThreshold);
        default:
            throw InvalidArgument("detector E2 attribute '" + toString(key) + "' not allowed");
    }
}


void 
GNEDetectorE2::setAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList) {
if (value == getAttribute(key)) {
        return; //avoid needless changes, later logic relies on the fact that attributes have changed
    }
    switch (key) {
        case SUMO_ATTR_ID:
        case SUMO_ATTR_LANE:
            throw InvalidArgument("modifying detector E2 attribute '" + toString(key) + "' not allowed");
        case SUMO_ATTR_POSITION:
        case SUMO_ATTR_FREQUENCY:
        case SUMO_ATTR_FILE:
        case SUMO_ATTR_CONT:
        case SUMO_ATTR_HALTING_TIME_THRESHOLD:
        case SUMO_ATTR_HALTING_SPEED_THRESHOLD:
        case SUMO_ATTR_JAM_DIST_THRESHOLD:
            undoList->p_add(new GNEChange_Attribute(this, key, value));
            updateGeometry();
            break;
        default:
            throw InvalidArgument("detector E2 attribute '" + toString(key) + "' not allowed");
    }

}


bool 
GNEDetectorE2::isValid(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_ID:
        case SUMO_ATTR_LANE:
            throw InvalidArgument("modifying detector E2 attribute '" + toString(key) + "' not allowed");
        case SUMO_ATTR_POSITION:
            return (canParse<SUMOReal>(value) && parse<SUMOReal>(value) >= 0 && parse<SUMOReal>(value) <= (myLane.getLaneParametricLenght()));
        case SUMO_ATTR_FREQUENCY:
            return (canParse<SUMOReal>(value) && parse<SUMOReal>(value) >= 0);
        case SUMO_ATTR_FILE:
            return isValidFileValue(value);
        case SUMO_ATTR_CONT:
            return canParse<bool>(value);
        case SUMO_ATTR_HALTING_TIME_THRESHOLD:
            return canParse<int>(value);
        case SUMO_ATTR_HALTING_SPEED_THRESHOLD:
            return canParse<SUMOReal>(value);
        case SUMO_ATTR_JAM_DIST_THRESHOLD:
            return canParse<SUMOReal>(value);
        default:
            throw InvalidArgument("detector E2 attribute '" + toString(key) + "' not allowed");
    }
}

// ===========================================================================
// private
// ===========================================================================

void
GNEDetectorE2::setAttribute(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_ID:
        case SUMO_ATTR_LANE:
            throw InvalidArgument("modifying detector E2 attribute '" + toString(key) + "' not allowed");
        case SUMO_ATTR_POSITION:
            myPosOverLane = parse<SUMOReal>(value);
            updateGeometry();
            getViewNet()->update();
            break;
        case SUMO_ATTR_FREQUENCY:
            myFreq = parse<SUMOReal>(value);
            break;
        case SUMO_ATTR_FILE:
            myFilename = value;
            break;
        case SUMO_ATTR_CONT:
            myCont = parse<bool>(value);
            break;
        case SUMO_ATTR_HALTING_TIME_THRESHOLD:
            myTimeThreshold = parse<int>(value);
            break;
        case SUMO_ATTR_HALTING_SPEED_THRESHOLD:
            mySpeedThreshold = parse<SUMOReal>(value);
            break;
        case SUMO_ATTR_JAM_DIST_THRESHOLD:
            myJamThreshold = parse<SUMOReal>(value);
            break;
        default:
            throw InvalidArgument("detector E2 attribute '" + toString(key) + "' not allowed");
    }
}


void
GNEDetectorE2::setColors() {
    // Color E1_BASE
    myRGBColors.push_back(RGBColor(0, 204, 204, 255));
    // Color E1_BASE_SELECTED
    myRGBColors.push_back(RGBColor(125, 204, 204, 255));
}


/****************************************************************************/