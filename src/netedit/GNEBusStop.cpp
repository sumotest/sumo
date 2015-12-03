/****************************************************************************/
/// @file    GNEBusStop.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Nov 2015
/// @version $Id: $
///
/// A class for visualizing busStop geometry (adapted from GUILaneWrapper)
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
	myLane(lane),
	myLines(lines), 
	myBegPos(frompos),
	myEndPos(topos),
	GUIGlObject(GLO_TRIGGER, id),
    GNEAttributeCarrier(SUMO_TAG_BUS_STOP),
    mySpecialColor(0) {

		std::cout << "SE HA CREADO CON ID = " << getMicrosimID() <<  std::endl;
    updateGeometry();
}


GNEBusStop::~GNEBusStop() {}

GNELane &GNEBusStop::getLane() {
	return myLane;
}

SUMOReal GNEBusStop::getBeginLanePosition() const {
	return myBegPos;
}
		
SUMOReal GNEBusStop::getEndLanePosition() const {
	return myEndPos;
}

void
GNEBusStop::drawGL(const GUIVisualizationSettings& s) const {
	/*
    glPushMatrix();
    glPushName(getGlID());
    glTranslated(0, 0, getType());
    const bool selectedEdge = gSelected.isSelected(myLane.getParentEdge().getType(), myLane.getParentEdge().getGlID());
    const bool selected = gSelected.isSelected(getType(), getGlID());
    if (mySpecialColor != 0) {
        GLHelper::setColor(*mySpecialColor);
    } else if (selected) {
        GLHelper::setColor(GNENet::selectedLaneColor);
    } else if (selectedEdge) {
        GLHelper::setColor(GNENet::selectionColor);
    } else {
		const GUIColorer& c = s.laneColorer;
		if (!setFunctionalColor(c.getActive()) && !setMultiColor(c)) {
			GLHelper::setColor(c.getScheme().getColor(getColorValue(c.getActive())));
		}        
    };

    // draw lane
    // check whether it is not too small
    const SUMOReal selectionScale = selected || selectedEdge ? s.selectionScale : 1;
    const SUMOReal exaggeration = selectionScale * s.laneWidthExaggeration; // * s.laneScaler.getScheme().getColor(getScaleValue(s.laneScaler.getActive()));
    if (s.scale * exaggeration < 1.) {
        if (myShapeColors.size() > 0) {
            GLHelper::drawLine(getShape(), myShapeColors);
        } else {
            GLHelper::drawLine(getShape());
        }
        glPopMatrix();
    } else {
        
        // the actual lane
        // reduce lane width to make sure that a selected edge can still be seen
		const SUMOReal halfWidth = selectionScale * (myLane.getParentEdge().getNBEdge()->getLaneWidth(myIndex) / 2 - (selectedEdge ? .3 : 0));
		if (myShapeColors.size() > 0) {
            GLHelper::drawBoxLines(getShape(), myShapeRotations, myShapeLengths, myShapeColors, halfWidth);
        } else {
            GLHelper::drawBoxLines(getShape(), myShapeRotations, myShapeLengths, halfWidth);
        }

        glPopMatrix();
        if (exaggeration == 1) {
            drawMarkings(selectedEdge, exaggeration);
        }

        // draw ROWs only if target junction has a valid logic)
        if (myLane.getParentEdge().getDest()->isLogicValid() && s.scale > 3) {
            drawArrows();
        }
    }
    glPopName();
	*/
}


void
GNEBusStop::drawMarkings(const bool& selectedEdge, SUMOReal scale) const {
	/*
    glPushMatrix();
    glTranslated(0, 0, GLO_EDGE);

    const SUMOReal halfWidth = myLane.getParentEdge().getNBEdge()->getLaneWidth(myIndex) * 0.5;
    // optionally draw inverse markings
    if (myIndex > 0 && (myLane.getParentEdge().getNBEdge()->getPermissions(myIndex - 1) & myLane.getParentEdge().getNBEdge()->getPermissions(myIndex)) != 0) {
        SUMOReal mw = (halfWidth + SUMO_const_laneOffset + .01) * scale;
        int e = (int) getShape().size() - 1;
        for (int i = 0; i < e; ++i) {
            glPushMatrix();
            glTranslated(getShape()[i].x(), getShape()[i].y(), 0.1);
            glRotated(myShapeRotations[i], 0, 0, 1);
            for (SUMOReal t = 0; t < myShapeLengths[i]; t += 6) {
                const SUMOReal length = MIN2((SUMOReal)3, myShapeLengths[i] - t);
                glBegin(GL_QUADS);
                glVertex2d(-mw, -t);
                glVertex2d(-mw, -t - length);
                glVertex2d(halfWidth * 0.5 * scale, -t - length);
                glVertex2d(halfWidth * 0.5 * scale, -t);
                glEnd();
            }
            glPopMatrix();
        }
    }

    // draw white boundings (and white markings) depending on selection
    if (selectedEdge) {
        glTranslated(0, 0, 0.2); // draw selection on top of regular markings
        GLHelper::setColor(GNENet::selectionColor);
    } else {
        glColor3d(1, 1, 1);
    }

    GLHelper::drawBoxLines(
        getShape(),
        getShapeRotations(),
        getShapeLengths(),
        (halfWidth + SUMO_const_laneOffset) * scale);
    glPopMatrix();
	*/
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
GNEBusStop::getParameterWindow(GUIMainWindow& app,
                            GUISUMOAbstractView&) {

    GUIParameterTableWindow* ret =
        new GUIParameterTableWindow(app, *this, 2);
    // add items
    ret->mkItem("length [m]", false, myLane.getParentEdge().getNBEdge()->getLength());
    // close building
    ret->closeBuilding();
    return ret;

}


Boundary
GNEBusStop::getCenteringBoundary() const {
    Boundary b = getShape().getBoxBoundary();
    b.grow(10);
    return b;
}


const PositionVector&
GNEBusStop::getShape() const {
    return myLane.getShape();
}


const std::vector<SUMOReal>&
GNEBusStop::getShapeRotations() const {
    return myShapeRotations;
}


const std::vector<SUMOReal>&
GNEBusStop::getShapeLengths() const {
    return myShapeLengths;
}


Boundary
GNEBusStop::getBoundary() const {
    return myLane.getShape().getBoxBoundary();
}


void
GNEBusStop::updateGeometry() {
	/*
    myShapeRotations.clear();
    myShapeLengths.clear();
    //SUMOReal length = myLane.getParentEdge().getLength(); // @todo see ticket #448
    // may be different from length
    int segments = (int) getShape().size() - 1;
    if (segments >= 0) {
        myShapeRotations.reserve(segments);
        myShapeLengths.reserve(segments);
        for (int i = 0; i < segments; ++i) {
            const Position& f = getShape()[i];
            const Position& s = getShape()[i + 1];
            myShapeLengths.push_back(f.distanceTo2D(s));
            myShapeRotations.push_back((SUMOReal) atan2((s.x() - f.x()), (f.y() - s.y())) * (SUMOReal) 180.0 / (SUMOReal) PI);
        }
    }
	*/
}

std::string
GNEBusStop::getAttribute(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_ID:
            return getMicrosimID();
        case SUMO_ATTR_LANE:
            return toString(myLane.getAttribute(SUMO_ATTR_ID));
        case SUMO_ATTR_STARTPOS:
            return toString(myBegPos);
        case SUMO_ATTR_ENDPOS:
            return toString(myEndPos);
        default:
            throw InvalidArgument("lane attribute '" + toString(key) + "' not allowed");
    }
}


void
GNEBusStop::setAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList) {
	if (value == getAttribute(key)) {
        return; //avoid needless changes, later logic relies on the fact that attributes have changed
    }
    switch (key) {
        case SUMO_ATTR_ID:
            throw InvalidArgument("modifying lane attribute '" + toString(key) + "' not allowed");
			/// Function has to be finished
			
			/*
        case SUMO_ATTR_LANE:
            return toString(myLane.getAttribute(SUMO_ATTR_ID));
        case SUMO_ATTR_STARTPOS:
            return toString(myBegPos);
        case SUMO_ATTR_ENDPOS:
            return toString(myEndPos);
			*/
        default:
            throw InvalidArgument("lane attribute '" + toString(key) + "' not allowed");
    }
}


bool
GNEBusStop::isValid(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_ID:
            return false;
        case SUMO_ATTR_SPEED:
            return canParse<SUMOReal>(value);
        case SUMO_ATTR_ALLOW:
        case SUMO_ATTR_DISALLOW:
            return canParseVehicleClasses(value);
        case SUMO_ATTR_WIDTH:
            return isPositive<SUMOReal>(value) || parse<SUMOReal>(value) == NBEdge::UNSPECIFIED_WIDTH;
        case SUMO_ATTR_ENDOFFSET:
            return canParse<SUMOReal>(value);
        default:
            throw InvalidArgument("lane attribute '" + toString(key) + "' not allowed");

    }
}

// ===========================================================================
// private
// ===========================================================================

void
GNEBusStop::setAttribute(SumoXMLAttr key, const std::string& value) {
	/*
    NBEdge* edge = myLane.getParentEdge().getNBEdge();
    switch (key) {
        case SUMO_ATTR_ID:
            throw InvalidArgument("modifying lane attribute '" + toString(key) + "' not allowed");
        case SUMO_ATTR_SPEED:
            edge->setSpeed(myIndex, parse<SUMOReal>(value));
            break;
        case SUMO_ATTR_ALLOW:
            edge->setPermissions(parseVehicleClasses(value), myIndex);
            break;
        case SUMO_ATTR_DISALLOW:
            edge->setPermissions(~parseVehicleClasses(value), myIndex); // negation yields allowed
            break;
        case SUMO_ATTR_WIDTH:
            edge->setLaneWidth(myIndex, parse<SUMOReal>(value));
            break;
        case SUMO_ATTR_ENDOFFSET:
            edge->setEndOffset(myIndex, parse<SUMOReal>(value));
            break;
        default:
            throw InvalidArgument("lane attribute '" + toString(key) + "' not allowed");
    }
	*/
}


bool
GNEBusStop::setFunctionalColor(size_t activeScheme) const {
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
GNEBusStop::setMultiColor(const GUIColorer& c) const {
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
GNEBusStop::getColorValue(size_t activeScheme) const {
	/*
	const SVCPermissions myPermissions = myLane.getParentEdge().getNBEdge()->getPermissions(myIndex);
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
                   gSelected.isSelected(GLO_EDGE, dynamic_cast<GNEEdge*>(&myLane.getParentEdge())->getGlID());
        case 2:
            return (SUMOReal)myPermissions;
        case 3:
			return myLane.getParentEdge().getNBEdge()->getLaneSpeed(myIndex);
        case 4:
            return myLane.getParentEdge().getNBEdge()->getNumLanes();        
        case 5: {
            return myLane.getParentEdge().getNBEdge()->getLoadedLength() / myLane.getParentEdge().getNBEdge()->getLength();
        }
		// case 6: by angle (functional)
       case 7: {
            return myLane.getParentEdge().getNBEdge()->getPriority();
        }
        case 8: {
            // color by z of first shape point
            return getShape()[0].z();
        }
		// case 9: by segment height
        case 10: {
            // color by incline
            return (getShape()[-1].z() - getShape()[0].z()) /  myLane.getParentEdge().getNBEdge()->getLength();
        }        
    }
	*/
    return 0;
}

const std::string& 
GNEBusStop::getParentName() const {
    return myLane.getMicrosimID();
}

/****************************************************************************/
