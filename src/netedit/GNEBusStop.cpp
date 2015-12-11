/****************************************************************************/
/// @file    GNEBusStop.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Nov 2015
/// @version $Id: $
///
/// A lane area vehicles can halt at (GNE version)
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

#include "GNEBusStop.h"
#include "GNELane.h"
#include "GNEEdge.h"
#include "GNEJunction.h"
#include "GNEUndoList.h"
#include "GNENet.h"
#include "GNEChange_Attribute.h"
#include "GNEViewNet.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// static member definitions
// ===========================================================================


// ===========================================================================
// method definitions
// ===========================================================================
GNEBusStop::GNEBusStop(const std::string& id, const std::vector<std::string>& lines, GNELane& lane, SUMOReal frompos, SUMOReal topos) : 
	GNEStoppingPlace(id, lane, frompos, topos, SUMO_TAG_BUS_STOP),
	myLines(lines) {
	getLane().addBusStop(this);
    updateGeometry();
}


GNEBusStop::~GNEBusStop() {
	getLane().removeBusStop(this);
}


void
GNEBusStop::updateGeometry() {
    //const SUMOReal offsetSign = MSNet::getInstance()->lefthand() ? -1 : 1;
	SUMOReal offsetSign = 1;
    myShape = getLane().getShape();
    myShape.move2side(1.65 * offsetSign);
    myShape = myShape.getSubpart(getFromPosition(), getToPosition());
    myShapeRotations.reserve(myShape.size() - 1);
    myShapeRotations.reserve(myShape.size() - 1);
    int e = (int) myShape.size() - 1;
    for (int i = 0; i < e; ++i) {
        const Position& f = myShape[i];
        const Position& s = myShape[i + 1];
        myShapeLengths.push_back(f.distanceTo(s));
        myShapeRotations.push_back((SUMOReal) atan2((s.x() - f.x()), (f.y() - s.y())) * (SUMOReal) 180.0 / (SUMOReal) PI);
    }
    PositionVector tmp = myShape;
    tmp.move2side(1.5 * offsetSign);
    mySignPos = tmp.getLineCenter();
    mySignRot = 0;
    if (tmp.length() != 0) {
        mySignRot = myShape.rotationDegreeAtOffset(SUMOReal((myShape.length() / 2.)));
        mySignRot -= 90;
    }
}


const 
std::vector<std::string> &GNEBusStop::getLines() const {
	return myLines;
}

PositionVector
GNEBusStop::getShape() const {
    return myShape;
}


std::vector<SUMOReal>
GNEBusStop::getShapeRotations() const {
    return myShapeRotations;
}


std::vector<SUMOReal>
GNEBusStop::getShapeLengths() const {
    return myShapeLengths;
}


void
GNEBusStop::drawGL(const GUIVisualizationSettings& s) const {
	glPushName(getGlID());
    glPushMatrix();
    RGBColor green(76, 170, 50, 255);
    RGBColor yellow(255, 235, 0, 255);
    // draw the area
    size_t i;
    glTranslated(0, 0, getType());
    GLHelper::setColor(green);
    const SUMOReal exaggeration = s.addSize.getExaggeration(s);
    GLHelper::drawBoxLines(myShape, myShapeRotations, myShapeLengths, exaggeration);
    // draw details unless zoomed out to far
    if (s.scale * exaggeration >= 10) {
        // draw the lines
        //const SUMOReal rotSign = MSNet::getInstance()->lefthand() ? -1 : 1;
		SUMOReal rotSign = 1;
        for (i = 0; i != myLines.size(); ++i) {
            glPushMatrix();
            glTranslated(mySignPos.x(), mySignPos.y(), 0);
            glRotated(180, 1, 0, 0);
            glRotated(rotSign * mySignRot, 0, 0, 1);
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            pfSetPosition(0, 0);
            pfSetScale(1.f);
            glTranslated(1.2, -(double)i, 0);
            pfDrawString(myLines[i].c_str());
            glPopMatrix();
        }
        // draw the sign
        glTranslated(mySignPos.x(), mySignPos.y(), 0);
        int noPoints = 9;
        if (s.scale * exaggeration > 25) {
            noPoints = MIN2((int)(9.0 + (s.scale * exaggeration) / 10.0), 36);
        }
        glScaled(exaggeration, exaggeration, 1);
        GLHelper::drawFilledCircle((SUMOReal) 1.1, noPoints);
        glTranslated(0, 0, .1);
        GLHelper::setColor(yellow);
        GLHelper::drawFilledCircle((SUMOReal) 0.9, noPoints);
        if (s.scale * exaggeration >= 4.5) {
            GLHelper::drawText("H", Position(), .1, 1.6, green, mySignRot);
        }
    }
    glPopMatrix();
    glPopName();
    drawName(getCenteringBoundary().getCenter(), s.scale, s.addName);
}


GUIGLObjectPopupMenu*
GNEBusStop::getPopUpMenu(GUIMainWindow& app, GUISUMOAbstractView& parent) {
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
    const SUMOReal pos = myShape.nearest_offset_to_point2D(parent.getPositionInformation());
    const SUMOReal height = myShape.positionAtOffset2D(myShape.nearest_offset_to_point2D(parent.getPositionInformation())).z();
    new FXMenuCommand(ret, ("pos: " + toString(pos) + " height: " + toString(height)).c_str(), 0, 0, 0);
    // new FXMenuSeparator(ret);
    // buildPositionCopyEntry(ret, false);

    // let the GNEViewNet store the popup position
    (dynamic_cast<GNEViewNet&>(parent)).markPopupPosition();
	*/
    return ret;
}


GUIParameterTableWindow*
GNEBusStop::getParameterWindow(GUIMainWindow& app,
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
GNEBusStop::getCenteringBoundary() const {
    Boundary b = myShape.getBoxBoundary();
    b.grow(20);	// anterior: 10
    return b;

    //Boundary
    //GNEBusStop::getBoundary() const {
    //return myLane.myShape.getBoxBoundary();
}


std::string
GNEBusStop::getAttribute(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_ID:
            return getMicrosimID();
        case SUMO_ATTR_LANE:
            return toString(getLane().getAttribute(SUMO_ATTR_ID));
        case SUMO_ATTR_STARTPOS:
            return toString(getFromPosition());
        case SUMO_ATTR_ENDPOS:
            return toString(getToPosition());
		case SUMO_ATTR_LINES: {
			// Convert myLines vector into String with the schema "line1 line2 ... lineN"
			std::string myLinesStr;
			for(std::vector<std::string>::const_iterator i = myLines.begin(); i != myLines.end(); i++) {
				if((*i) != myLines.back())
					myLinesStr += (myLinesStr + (*i) + " ");
				else
					myLinesStr += (myLinesStr + (*i));
			}
			return myLinesStr;
		}
        default:
            throw InvalidArgument("busStop attribute '" + toString(key) + "' not allowed");
    }
}


void
GNEBusStop::setAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList) {
	if (value == getAttribute(key)) {
        return; //avoid needless changes, later logic relies on the fact that attributes have changed
    }
    switch (key) {
        case SUMO_ATTR_ID:
            throw InvalidArgument("modifying busStop attribute '" + toString(key) + "' not allowed");
			/// CLASS GNEBUSSTOP_CHANGE has to be finished
			
			/*
        case SUMO_ATTR_LANE:
            return toString(getLane().getAttribute(SUMO_ATTR_ID));
        case SUMO_ATTR_STARTPOS:
            return toString(getFromPosition());
        case SUMO_ATTR_ENDPOS:
            return toString(getToPosition());
			*/
        default:
            throw InvalidArgument("busStop attribute '" + toString(key) + "' not allowed");
    }
}


bool
GNEBusStop::isValid(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_ID:
            return false;
        case SUMO_ATTR_LANE:
            return canParse<std::string>(value);
        case SUMO_ATTR_STARTPOS:
			return canParse<SUMOReal>(value);
        case SUMO_ATTR_ENDPOS:
            return canParse<SUMOReal>(value);
        case SUMO_ATTR_LINES:
			return canParse<std::string>(value);
        default:
            throw InvalidArgument("busStop attribute '" + toString(key) + "' not allowed");
    }
}

// ===========================================================================
// private
// ===========================================================================

void
GNEBusStop::setAttribute(SumoXMLAttr key, const std::string& value) {
    NBEdge* edge = getLane().getParentEdge().getNBEdge();
    switch (key) {
        case SUMO_ATTR_ID:
            throw InvalidArgument("modifying busStop attribute '" + toString(key) + "' not allowed");
        case SUMO_ATTR_LANE:
            throw InvalidArgument("modifying busStop attribute '" + toString(key) + "' not allowed");
            break;
        case SUMO_ATTR_STARTPOS:
            setFromPosition(parse<SUMOReal>(value));
            break;
        case SUMO_ATTR_ENDPOS:
            setToPosition(parse<SUMOReal>(value));
            break;
        case SUMO_ATTR_LINES:
            SUMOSAXAttributes::parseStringVector(value, myLines);
            break;
        default:
            throw InvalidArgument("busStop attribute '" + toString(key) + "' not allowed");
    }
}


bool
GNEBusStop::setFunctionalColor(size_t activeScheme) const {
    switch (activeScheme) {
        case 6: {
            SUMOReal hue = GeomHelper::naviDegree(myShape.beginEndAngle()); // [0-360]
            GLHelper::setColor(RGBColor::fromHSV(hue, 1., 1.));
            return true;
        }
        default:
            return false;
    }
}


bool
GNEBusStop::setMultiColor(const GUIColorer& c) const {
    const size_t activeScheme = c.getActive();
    myShapeColors.clear();
    switch (activeScheme) {
        case 9: // color by height at segment start
            for (PositionVector::const_iterator ii = myShape.begin(); ii != myShape.end() - 1; ++ii) {
                myShapeColors.push_back(c.getScheme().getColor(ii->z()));
            }
            return true;
        case 11: // color by inclination  at segment start
            for (int ii = 1; ii < (int)myShape.size(); ++ii) {
                const SUMOReal inc = (myShape[ii].z() - myShape[ii - 1].z()) / MAX2(POSITION_EPS, myShape[ii].distanceTo2D(myShape[ii - 1]));
                myShapeColors.push_back(c.getScheme().getColor(inc));
            }
            return true;
        default:
            return false;
    }
}


SUMOReal
GNEBusStop::getColorValue(size_t activeScheme) const {
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
            return myShape[0].z();
        }
		// case 9: by segment height
        case 10: {
            // color by incline
            return (myShape[-1].z() - myShape[0].z()) /  getLane().getParentEdge().getNBEdge()->getLength();
        }        
    }
	*/
    return 0;
}


/****************************************************************************/
