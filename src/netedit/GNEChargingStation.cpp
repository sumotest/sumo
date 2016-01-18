/****************************************************************************/
/// @file    GNEChargingStation.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Nov 2015
/// @version $Id: $
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

GNEChargingStation::GNEChargingStation(const std::string& id, GNELane& lane, SUMOReal fromPos, SUMOReal toPos, SUMOReal chargingPower, SUMOReal efficiency, bool chargeInTransit, SUMOReal chargeDelay) :
    GNEStoppingPlace(id, lane, fromPos, toPos, SUMO_TAG_CHARGING_STATION),
    myChargingPower(chargingPower), 
    myEfficiency(efficiency), 
    myChargeInTransit(chargeInTransit), 
    myChargeDelay(chargeDelay) {
    updateGeometry();
}


GNEChargingStation::~GNEChargingStation() {}


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
    // Draw Charging Station
    glPushName(getGlID());
    glPushMatrix();
    RGBColor blue(114, 210, 252, 255);
    RGBColor green(76, 170, 50, 255);
    RGBColor yellow(255, 235, 0, 255);
    // draw the area
    glTranslated(0, 0, getType());
    GLHelper::setColor(blue);
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
        GLHelper::setColor(blue);
        GLHelper::drawFilledCircle((SUMOReal) 1.1, noPoints);
        glTranslated(0, 0, .1);

        GLHelper::setColor(yellow);
        GLHelper::drawFilledCircle((SUMOReal) 0.9, noPoints);

        if (s.scale * exaggeration >= 4.5) {
            GLHelper::drawText("C", Position(), .1, 1.6, green, mySignRot);
        }

        glTranslated(5, 0, 0);

    }
    glPopMatrix();
    glPopName();
    drawName(getCenteringBoundary().getCenter(), s.scale, s.addName);
}


void 
GNEChargingStation::drawGLAdditional(GUISUMOAbstractView* const parent, const GUIVisualizationSettings& s) const {
    // Ignore Warning
    UNUSED_PARAMETER(parent);
    // Draw Charging Station
    glPushName(getGlID());
    glPushMatrix();
    RGBColor blue(114, 210, 252, 255);
    RGBColor green(76, 170, 50, 255);
    RGBColor yellow(255, 235, 0, 255);
    // draw the area
    glTranslated(0, 0, getType());
    GLHelper::setColor(blue);
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
        GLHelper::setColor(blue);
        GLHelper::drawFilledCircle((SUMOReal) 1.1, noPoints);
        glTranslated(0, 0, .1);

        GLHelper::setColor(yellow);
        GLHelper::drawFilledCircle((SUMOReal) 0.9, noPoints);

        if (s.scale * exaggeration >= 4.5) {
            GLHelper::drawText("C", Position(), .1, 1.6, green, mySignRot);
        }

        glTranslated(5, 0, 0);

    }
    glPopMatrix();
    glPopName();
    drawName(getCenteringBoundary().getCenter(), s.scale, s.addName);
}


GUIGLObjectPopupMenu*
GNEChargingStation::getPopUpMenu(GUIMainWindow& app, GUISUMOAbstractView& parent) {
    GUIGLObjectPopupMenu* ret = new GUIGLObjectPopupMenu(app, parent, *this);
    /*
    buildPopupHeader(ret, app);
    buildCenterPopupEntry(ret);
    new FXMenuCommand(ret, "Copy edge name to clipboard", 0, ret, MID_COPY_EDGE_NAME);
    buildNameCopyPopupEntry(ret);
    buildSelectionPopupEntry(ret);
    buildPositionCopyEntry(ret, false);
    if (parent.getVisualisationSettings()->editMode != GNE_MODE_CONNECT) {
        new FXMenuCommand(ret, "Split edge here", 0, &parent, MID_GNE_SPLIT_EDGE);
        new FXMenuCommand(ret, "Split edges in both direction here", 0, &parent, MID_GNE_SPLIT_EDGE_BIDI);
        new FXMenuCommand(ret, "Reverse edge", 0, &parent, MID_GNE_REVERSE_EDGE);
        new FXMenuCommand(ret, "Add reverse direction", 0, &parent, MID_GNE_ADD_REVERSE_EDGE);
        new FXMenuCommand(ret, "Set geometry endpoint here", 0, &parent, MID_GNE_SET_EDGE_ENDPOINT);
        new FXMenuCommand(ret, "Restore geometry endpoint", 0, &parent, MID_GNE_RESET_EDGE_ENDPOINT);
        if (gSelected.isSelected(GLO_LANE, getGlID())) {
            new FXMenuCommand(ret, "Straighten selected Edges", 0, &parent, MID_GNE_STRAIGHTEN);
        } else {
            new FXMenuCommand(ret, "Straighten edge", 0, &parent, MID_GNE_STRAIGHTEN);
        }
        if (gSelected.isSelected(GLO_LANE, getGlID())) {
            new FXMenuCommand(ret, "Duplicate selected lanes", 0, &parent, MID_GNE_DUPLICATE_LANE);
        } else {
            new FXMenuCommand(ret, "Duplicate lane", 0, &parent, MID_GNE_DUPLICATE_LANE);
        }
    }
    // buildShowParamsPopupEntry(ret, false);
    const SUMOReal pos = getShape().nearest_offset_to_point2D(parent.getPositionInformation());
    const SUMOReal height = getShape().positionAtOffset2D(getShape().nearest_offset_to_point2D(parent.getPositionInformation())).z();
    new FXMenuCommand(ret, ("pos: " + toString(pos) + " height: " + toString(height)).c_str(), 0, 0, 0);
    // new FXMenuSeparator(ret);
    // buildPositionCopyEntry(ret, false);

    // let the GNEViewNet store the popup position
    (dynamic_cast<GNEViewNet&>(parent)).markPopupPosition();
    */
    return ret;
}


GUIParameterTableWindow*
GNEChargingStation::getParameterWindow(GUIMainWindow& app,
                            GUISUMOAbstractView&) {

    GUIParameterTableWindow* ret =
        new GUIParameterTableWindow(app, *this, 2);
    // add items
    ret->mkItem("length [m]", false, getLane().getParentEdge().getNBEdge()->getLength());
    // close building
    ret->closeBuilding();
    return ret;
}


Boundary
GNEChargingStation::getCenteringBoundary() const {
    Boundary b = myShape.getBoxBoundary();
    b.grow(20);    // anterior: 10
    return b;
}

void
GNEChargingStation::updateGeometry() {
    myShapeRotations.clear();
    //const SUMOReal offsetSign = MSNet::getInstance()->lefthand() ? -1 : 1;
    SUMOReal offsetSign = 1;
    myShape = getLane().getShape();
    myShape = myShape.getSubpart(getFromPosition(), getToPosition());
    myShapeRotations.reserve(myShape.size() - 1);
    myShapeLengths.reserve(myShape.size() - 1);
    int e = (int) myShape.size() - 1;
    for (int i = 0; i < e; ++i) {
        const Position& f = myShape[i];
        const Position& s = myShape[i + 1];
        myShapeLengths.push_back(f.distanceTo(s));
        myShapeRotations.push_back((SUMOReal) atan2((s.x() - f.x()), (f.y() - s.y())) * (SUMOReal) 180.0 / (SUMOReal) PI);
    }
    PositionVector tmp = myShape;
    tmp.move2side(1.5);
    mySignPos = tmp.getLineCenter();
    mySignRot = 0;
    if (tmp.length() != 0) {
        mySignRot = myShape.rotationDegreeAtOffset(SUMOReal((myShape.length() / 2.)));
        mySignRot -= 90;
    }
}

std::string
GNEChargingStation::getAttribute(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_ID:
            return getMicrosimID();
        case SUMO_ATTR_LANE:
            return toString(getLane().getAttribute(SUMO_ATTR_ID));
        case SUMO_ATTR_STARTPOS:
            return toString(getFromPosition());
        case SUMO_ATTR_ENDPOS:
            return toString(getToPosition());
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
            return (canParse<SUMOReal>(value) && parse<SUMOReal>(value) >= 0 && parse<SUMOReal>(value) < (getToPosition()-1));
        case SUMO_ATTR_ENDPOS:
            return (canParse<SUMOReal>(value) && parse<SUMOReal>(value) >= 1 && parse<SUMOReal>(value) > getFromPosition());
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
    NBEdge* edge = getLane().getParentEdge().getNBEdge();
    switch (key) {
        case SUMO_ATTR_ID:
            throw InvalidArgument("modifying busStop attribute '" + toString(key) + "' not allowed");
        case SUMO_ATTR_LANE:
            throw InvalidArgument("modifying busStop attribute '" + toString(key) + "' not allowed");
        case SUMO_ATTR_STARTPOS:
            setFromPosition(parse<SUMOReal>(value));
            break;
        case SUMO_ATTR_ENDPOS:
            setToPosition(parse<SUMOReal>(value));
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
            throw InvalidArgument("busStop attribute '" + toString(key) + "' not allowed");
    }
}


bool
GNEChargingStation::setFunctionalColor(size_t activeScheme) const {
    switch (activeScheme) {
        case 6: {
            SUMOReal hue = GeomHelper::naviDegree(getShape().beginEndAngle()); // [0-360]
            GLHelper::setColor(RGBColor::fromHSV(hue, 1., 1.));
            return true;
        }
        default:
            return false;
    }
}


bool
GNEChargingStation::setMultiColor(const GUIColorer& c) const {
    const size_t activeScheme = c.getActive();
    myShapeColors.clear();
    switch (activeScheme) {
        case 9: // color by height at segment start
            for (PositionVector::const_iterator ii = getShape().begin(); ii != getShape().end() - 1; ++ii) {
                myShapeColors.push_back(c.getScheme().getColor(ii->z()));
            }
            return true;
        case 11: // color by inclination  at segment start
            for (int ii = 1; ii < (int)getShape().size(); ++ii) {
                const SUMOReal inc = (getShape()[ii].z() - getShape()[ii - 1].z()) / MAX2(POSITION_EPS, getShape()[ii].distanceTo2D(getShape()[ii - 1]));
                myShapeColors.push_back(c.getScheme().getColor(inc));
            }
            return true;
        default:
            return false;
    }
}


SUMOReal
GNEChargingStation::getColorValue(size_t activeScheme) const {
    /*
    const SVCPermissions myPermissions = getLane().getParentEdge().getNBEdge()->getPermissions(myIndex);
    switch (activeScheme) {
        case 0:
            switch (myPermissions) {
                case SVC_PEDESTRIAN:
                    return 1;
                case SVC_BICYCLE:
                    return 2;
                case 0:
                    return 3;
                case SVC_SHIP:
                    return 4;
                default:
                    break;
            }
            if ((myPermissions & SVC_PASSENGER) != 0 || isRailway(myPermissions)) {
                return 0;
            } else {
                return 5;
            }
        case 1:
            return gSelected.isSelected(getType(), getGlID()) ||
                   gSelected.isSelected(GLO_EDGE, dynamic_cast<GNEEdge*>(&getLane().getParentEdge())->getGlID());
        case 2:
            return (SUMOReal)myPermissions;
        case 3:
            return getLane().getParentEdge().getNBEdge()->getLaneSpeed(myIndex);
        case 4:
            return getLane().getParentEdge().getNBEdge()->getNumLanes();        
        case 5: {
            return getLane().getParentEdge().getNBEdge()->getLoadedLength() / getLane().getParentEdge().getNBEdge()->getLength();
        }
        // case 6: by angle (functional)
       case 7: {
            return getLane().getParentEdge().getNBEdge()->getPriority();
        }
        case 8: {
            // color by z of first shape point
            return getShape()[0].z();
        }
        // case 9: by segment height
        case 10: {
            // color by incline
            return (getShape()[-1].z() - getShape()[0].z()) /  getLane().getParentEdge().getNBEdge()->getLength();
        }        
    }
    */
    return 0;
}


/****************************************************************************/
