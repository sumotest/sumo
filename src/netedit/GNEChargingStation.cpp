/****************************************************************************/
/// @file    GNEChargingStation.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Nov 2015
/// @version $Id$
///
/// A class for visualizing chargingStation geometry (adapted from GUILaneWrapper)
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

#include "GNEChargingStation.h"
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

GNEChargingStation::GNEChargingStation(const std::string& id, GNELane& lane, GNEViewNet* viewNet, SUMOReal fromPos, SUMOReal toPos, SUMOReal chargingPower, SUMOReal efficiency, bool chargeInTransit, SUMOReal chargeDelay) :
    GNEStoppingPlace(id, lane, viewNet, SUMO_TAG_CHARGING_STATION, fromPos, toPos),
    myChargingPower(chargingPower), 
    myEfficiency(efficiency), 
    myChargeInTransit(chargeInTransit), 
    myChargeDelay(chargeDelay) {
    // When a new additional element is created, updateGeometry() must be called
    updateGeometry();
    // And color must be configured
    setColors();
}


GNEChargingStation::~GNEChargingStation() {}


void 
GNEChargingStation::updateGeometry() {
    // Clear all containers
    myShapeRotations.clear();
    myShapeLengths.clear();
    
    // Clear shape
    myShape.clear();

    // Get value of option "lefthand"
    SUMOReal offsetSign = OptionsCont::getOptions().getBool("lefthand") ? -1 : 1;

    // Get shape of lane parent
    myShape = myLane.getShape();

    // Cut shape using as delimitators from start position and end position
    myShape = myShape.getSubpart(myLane.getPositionRelativeToParametricLenght(myFromPos), myLane.getPositionRelativeToParametricLenght(myToPos));

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

    // Set position of the block icon
    myBlockIconPos = myShape.getLineCenter();

    // If lenght of the shape is distint to 0
    if (myShape.length() != 0) {

        // Obtain rotation of signal rot
        mySignRot = myShape.rotationDegreeAtOffset(SUMOReal((myShape.length() / 2.)));

        // correct orientation
        mySignRot -= 90;
    }
    else
        // Value of signal rotation is 0
        mySignRot = 0;
}


SUMOReal 
GNEChargingStation::getChargingPower() {
    return myChargingPower;
}


SUMOReal 
GNEChargingStation::getEfficiency() {
    return myEfficiency;
}


bool 
GNEChargingStation::getChargeInTransit() {
    return myChargeInTransit;
}


SUMOReal 
GNEChargingStation::getChargeDelay() {
    return myChargeDelay;
}


PositionVector
GNEChargingStation::getShape() const {
    return myShape;
}


std::vector<SUMOReal>
GNEChargingStation::getShapeRotations() const {
    return myShapeRotations;
}


std::vector<SUMOReal>
GNEChargingStation::getShapeLengths() const {
    return myShapeLengths;
}


void 
GNEChargingStation::setChargingPower(SUMOReal chargingPower) {
    if(chargingPower > 0) {
        myChargingPower = chargingPower;
    } else {
        throw InvalidArgument("Value of charging Power must be greather than 0");
    }
}


void 
GNEChargingStation::setEfficiency(SUMOReal efficiency) {
    if(efficiency >= 0 && efficiency <= 1) {
        myEfficiency = efficiency;
    } else {
        throw InvalidArgument("Value of efficiency must be between 0 and 1");
    }
}


void 
GNEChargingStation::setChargeInTransit(bool chargeInTransit) {
    myChargeInTransit = chargeInTransit;
}


void 
GNEChargingStation::setChargeDelay(SUMOReal chargeDelay) {
    if(chargeDelay < 0) {
        myChargeDelay = chargeDelay;
    } else {
        throw InvalidArgument("Value of chargeDelay cannot be negative");
    }
}


void
GNEChargingStation::drawGL(const GUIVisualizationSettings& s) const {
    // Additonals element are drawed using a drawGLAdditional
    drawGLAdditional(0, s);
}


void 
GNEChargingStation::drawGLAdditional(GUISUMOAbstractView* const parent, const GUIVisualizationSettings& s) const {
    // Ignore Warning
    UNUSED_PARAMETER(parent);

    // Declare variables to get colors depending if the chargingStation is selected
    RGBColor base, sign;

    // Set colors
    if(gSelected.isSelected(getType(), getGlID())) {
        base = myRGBColors[CHARGINGSTATION_BASE_SELECTED];
        sign = myRGBColors[CHARGINGSTATION_SIGN_SELECTED];
    } else {
        base = myRGBColors[CHARGINGSTATION_BASE];
        sign = myRGBColors[CHARGINGSTATION_SIGN];
    }

    // Draw Charging Station
    glPushName(getGlID());
    glPushMatrix();

    // draw the area
    glTranslated(0, 0, getType());
    GLHelper::setColor(base);
    const SUMOReal exaggeration = s.addSize.getExaggeration(s);
    GLHelper::drawBoxLines(myShape, myShapeRotations, myShapeLengths, exaggeration);

    // draw details unless zoomed out to far
    if (s.scale * exaggeration >= 10) {
        // draw the sign
        glTranslated(mySignPos.x(), mySignPos.y(), 0);
        int noPoints = 9;
        if (s.scale * exaggeration > 25) {
            noPoints = MIN2((int)(9.0 + (s.scale * exaggeration) / 10.0), 36);
        }

        glScaled(exaggeration, exaggeration, 1);
        GLHelper::setColor(base);
        GLHelper::drawFilledCircle((SUMOReal) 1.1, noPoints);
        glTranslated(0, 0, .1);

        GLHelper::setColor(sign);
        GLHelper::drawFilledCircle((SUMOReal) 0.9, noPoints);

        if (s.scale * exaggeration >= 4.5) {
            GLHelper::drawText("C", Position(), .1, 1.6, base, mySignRot);
        }

        glPopMatrix();

        if(myBlocked && dynamic_cast<GNEViewNet*>(parent)->showLockIcon())
            GNEAdditional::drawLockIcon();
    }
    glPopName();
    drawName(getCenteringBoundary().getCenter(), s.scale, s.addName);
}


GUIGLObjectPopupMenu*
GNEChargingStation::getPopUpMenu(GUIMainWindow& app, GUISUMOAbstractView& parent) {
    GUIGLObjectPopupMenu* ret = new GUIGLObjectPopupMenu(app, parent, *this);
    buildPopupHeader(ret, app);
    buildCenterPopupEntry(ret);
    new FXMenuCommand(ret, "Copy chargingStation name to clipboard", 0, ret, MID_COPY_EDGE_NAME);
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
GNEChargingStation::getParameterWindow(GUIMainWindow& app,
                            GUISUMOAbstractView&) {

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


Boundary
GNEChargingStation::getCenteringBoundary() const {
    Boundary b = myShape.getBoxBoundary();
    b.grow(20);    // anterior: 10
    return b;
}


void 
GNEChargingStation::writeAdditional(OutputDevice& device) {
    device.openTag(getTag());
    device.writeAttr(SUMO_ATTR_ID, getID());
    device.writeAttr(SUMO_ATTR_LANE, getLane().getID());
    device.writeAttr(SUMO_ATTR_STARTPOS, myFromPos);
    device.writeAttr(SUMO_ATTR_ENDPOS, myToPos);
    device.writeAttr(SUMO_ATTR_CHARGINGPOWER, myChargingPower);
    device.writeAttr(SUMO_ATTR_EFFICIENCY, myEfficiency);
    if(myChargeInTransit)
        device.writeAttr(SUMO_ATTR_CHARGEINTRANSIT, "true");
    else
        device.writeAttr(SUMO_ATTR_CHARGEINTRANSIT, "false");
    device.writeAttr(SUMO_ATTR_CHARGEDELAY, myChargeDelay);
    // Close tag
    device.closeTag();
}


std::string
GNEChargingStation::getAttribute(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_ID:
            return getMicrosimID();
        case SUMO_ATTR_LANE:
            return toString(myLane.getAttribute(SUMO_ATTR_ID));
        case SUMO_ATTR_STARTPOS:
            return toString(myFromPos);
        case SUMO_ATTR_ENDPOS:
            return toString(myToPos);
        case SUMO_ATTR_CHARGINGPOWER:
            return toString(myChargingPower);
        case SUMO_ATTR_EFFICIENCY:
            return toString(myEfficiency);
        case SUMO_ATTR_CHARGEINTRANSIT:
            if(myChargeInTransit == true)
                return "true";
            else
                return "false";
        case SUMO_ATTR_CHARGEDELAY:
            return toString(myChargeDelay);
        default:
            throw InvalidArgument("chargingStation attribute '" + toString(key) + "' not allowed");
    }
}


void
GNEChargingStation::setAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList) {
    if (value == getAttribute(key)) {
        return; //avoid needless changes, later logic relies on the fact that attributes have changed
    }
    switch (key) {
        case SUMO_ATTR_ID:
        case SUMO_ATTR_LANE:
            throw InvalidArgument("modifying chargingStation attribute '" + toString(key) + "' not allowed");
        case SUMO_ATTR_STARTPOS:
        case SUMO_ATTR_ENDPOS:
        case SUMO_ATTR_CHARGINGPOWER:
        case SUMO_ATTR_EFFICIENCY:
        case SUMO_ATTR_CHARGEINTRANSIT:
        case SUMO_ATTR_CHARGEDELAY:
            undoList->p_add(new GNEChange_Attribute(this, key, value));
            break;
        default:
            throw InvalidArgument("chargingStation attribute '" + toString(key) + "' not allowed");
    }
}


bool
GNEChargingStation::isValid(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_ID:
        case SUMO_ATTR_LANE:
            throw InvalidArgument("modifying chargingStation attribute '" + toString(key) + "' not allowed");
        case SUMO_ATTR_STARTPOS:
            return (canParse<SUMOReal>(value) && parse<SUMOReal>(value) >= 0 && parse<SUMOReal>(value) < (myToPos-1));
        case SUMO_ATTR_ENDPOS:
            return (canParse<SUMOReal>(value) && parse<SUMOReal>(value) >= 1 && parse<SUMOReal>(value) > myFromPos);
        case SUMO_ATTR_CHARGINGPOWER:
            return (canParse<SUMOReal>(value) && parse<SUMOReal>(value) >= 0);
        case SUMO_ATTR_EFFICIENCY:
            return (canParse<SUMOReal>(value) && parse<SUMOReal>(value) >= 0 && parse<SUMOReal>(value) <= 1);
        case SUMO_ATTR_CHARGEINTRANSIT:
            return (value == "true" || value == "false");
        case SUMO_ATTR_CHARGEDELAY:
            return (canParse<int>(value) && parse<int>(value) >= 0);
        default:
            throw InvalidArgument("chargingStation attribute '" + toString(key) + "' not allowed");
    }
}

// ===========================================================================
// private
// ===========================================================================

void
GNEChargingStation::setAttribute(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_ID:
        case SUMO_ATTR_LANE:
            throw InvalidArgument("modifying chargingStation attribute '" + toString(key) + "' not allowed");
        case SUMO_ATTR_STARTPOS:
            myFromPos = parse<SUMOReal>(value);
            updateGeometry();
            getViewNet()->update();
            break;
        case SUMO_ATTR_ENDPOS:
            myToPos = parse<SUMOReal>(value);
            updateGeometry();
            getViewNet()->update();
            break;
        case SUMO_ATTR_CHARGINGPOWER:
            myChargingPower = parse<SUMOReal>(value);
            break;
        case SUMO_ATTR_EFFICIENCY:
            myEfficiency = parse<SUMOReal>(value);
            break;
        case SUMO_ATTR_CHARGEINTRANSIT:
            if(value == "true")
                myChargeInTransit = true;
            else
                myChargeInTransit = false;
            break;
        case SUMO_ATTR_CHARGEDELAY:
            myChargeDelay = parse<int>(value);
            break;
        default:
            throw InvalidArgument("chargingStation attribute '" + toString(key) + "' not allowed");
    }
}


void
GNEChargingStation::setColors() {
    // Color CHARGINGSTATION_BASE
    myRGBColors.push_back(RGBColor(114, 210, 252, 255));
    // Color CHARGINGSTATION_BASE_SELECTED
    myRGBColors.push_back(RGBColor(125, 255, 255, 255));
    // Color CHARGINGSTATION_SIGN
    myRGBColors.push_back(RGBColor(255, 235, 0, 255));
    // Color CHARGINGSTATION_SIGN_SELECTED
    myRGBColors.push_back(RGBColor(255, 235, 0, 255));
}

/****************************************************************************/
