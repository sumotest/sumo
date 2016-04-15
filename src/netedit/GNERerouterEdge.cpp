/****************************************************************************/
/// @file    GNERerouterEdge.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Mar 2016
/// @version $Id: GNERerouter.cpp 19861 2016-02-01 09:08:47Z palcraft $
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

#include "GNERerouterEdge.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif

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

#include "GNERerouterEdge.h"
#include "GNERerouter.h"
#include "GNELane.h"
#include "GNEViewNet.h"
#include "GNEUndoList.h"
#include "GNENet.h"
#include "GNEChange_Attribute.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif

// ===========================================================================
// member method definitions
// ===========================================================================

GNERerouterEdge::GNERerouterEdge(const std::string &id, GNEViewNet* viewNet, GNELane *lane, GNERerouter *parent, bool amClosedEdge) :
    GNEAdditional(id, viewNet, Position(0,0), SUMO_TAG_REROUTEREDGE, lane, SUMO_TAG_REROUTER, parent, false, false, false),
     myAmClosedEdge(amClosedEdge) {
    // Update geometry;
    updateGeometry();
    // Set colors
/** CAMBIAR **/
    myBaseColor = RGBColor(0, 204, 0, 255);
    myBaseColorSelected = RGBColor(125, 204, 0, 255);
}


GNERerouterEdge::~GNERerouterEdge() {}


void
GNERerouterEdge::updateGeometry() {
    // Clear all containers
    myShapeRotations.clear();
    myShapeLengths.clear();

    // clear Shape
    myShape.clear();

    if(myAmClosedEdge)
        myPosition = Position(3,0);
    else
        myPosition = Position(myLane->getShape().length() - (SUMOReal) 6., 0);

    myShape.push_back(myLane->getShape().positionAtOffset(myPosition.x()));

    myShapeRotations.push_back(myLane->getShape().rotationDegreeAtOffset(myLane->getPositionRelativeToParametricLenght(myPosition.x())) * -1);

    //myBoundary.add(myFGPositions.back());

    // Set block icon rotation
    setBlockIconRotation();

    // Update parent geometry
    myParent->updateGeometry();
}


void
GNERerouterEdge::moveAdditional(SUMOReal posx, SUMOReal posy, GNEUndoList *undoList) {
    // Due a rerouterEdge is placed over a concrete position of lane, ignore Warnings
    UNUSED_PARAMETER(posx);
    UNUSED_PARAMETER(posy);
    UNUSED_PARAMETER(undoList);
}

void
GNERerouterEdge::writeAdditional(OutputDevice& device) {
    // Write parameters
    device.openTag(getTag());
    device.writeAttr(SUMO_ATTR_LANE, myLane->getID());
    device.writeAttr(SUMO_ATTR_POSITION, myPosition.x());
    // Close tag
    device.closeTag();
}


GUIParameterTableWindow*
GNERerouterEdge::getParameterWindow(GUIMainWindow& app, GUISUMOAbstractView& parent) {
    /** NOT YET SUPPORTED **/
    // Ignore Warning
    UNUSED_PARAMETER(parent);
    GUIParameterTableWindow* ret = new GUIParameterTableWindow(app, *this, 2);
    // add items
    ret->mkItem("id", false, getID());
    /** @TODO complet with the rest of parameters **/
    // close building
    ret->closeBuilding();
    return ret;
}


void
GNERerouterEdge::drawGL(const GUIVisualizationSettings& s) const {
    // Additonals element are drawed using a drawGLAdditional
    drawGLAdditional(0, s);
}


void
GNERerouterEdge::drawGLAdditional(GUISUMOAbstractView* const parent, const GUIVisualizationSettings& s) const {
    // Ignore Warning
    UNUSED_PARAMETER(parent);
    // Get exaggeration
    const SUMOReal exaggeration = s.addSize.getExaggeration(s);
    // If Z distance is correct for drawing
    if (s.scale * exaggeration >= 3) {
        // Push draw matrix
        glPushName(getGlID());
        // Obtain probability of RerouterParent
        const SUMOReal prob = dynamic_cast<GNERerouter*>(myParent)->getProbability();
        // If this is an closed edge
        if (myAmClosedEdge) {
            ;
            /*
            // draw closing symbol onto all lanes
            const RerouteInterval* const ri =
                myParent->getCurrentReroute(MSNet::getInstance()->getCurrentTimeStep());
            if (ri != 0 && prob > 0) {
                // draw only if the edge is closed at this time
                if (std::find(ri->closed.begin(), ri->closed.end(), myEdge) != ri->closed.end()) {
                    const size_t noLanes = myFGPositions.size();
                    for (size_t j = 0; j < noLanes; ++j) {
                        Position pos = myFGPositions[j];
                        SUMOReal rot = myFGRotations[j];
                        glPushMatrix();
                        glTranslated(pos.x(), pos.y(), 0);
                        glRotated(rot, 0, 0, 1);
                        glTranslated(0, -1.5, 0);
                        int noPoints = 9;
                        if (s.scale > 25) {
                            noPoints = (int)(9.0 + s.scale / 10.0);
                            if (noPoints > 36) {
                                noPoints = 36;
                            }
                        }
                        glTranslated(0, 0, getType());
                        //glScaled(exaggeration, exaggeration, 1);
                        glColor3d(0.7, 0, 0);
                        GLHelper::drawFilledCircle((SUMOReal) 1.3, noPoints);
                        glTranslated(0, 0, .1);
                        glColor3d(1, 0, 0);
                        GLHelper::drawFilledCircle((SUMOReal) 1.3, noPoints, 0, prob * 360);
                        glTranslated(0, 0, .1);
                        glColor3d(1, 1, 1);
                        glRotated(-90, 0, 0, 1);
                        glBegin(GL_TRIANGLES);
                        glVertex2d(0 - .3, -1.);
                        glVertex2d(0 - .3, 1.);
                        glVertex2d(0 + .3, 1.);
                        glVertex2d(0 + .3, -1.);
                        glVertex2d(0 - .3, -1.);
                        glVertex2d(0 + .3, 1.);
                        glEnd();
                        glPopMatrix();
                    }
                }
            }
            */
        } else {
            // draw rerouter symbol over lane
            glPushMatrix();
            glTranslated(myShape[0].x(), myShape[0].y(), 0);
            glRotated(myShapeRotations[0], 0, 0, 1);
            glTranslated(0, 0, getType());
            glScaled(exaggeration, exaggeration, 1);
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

            glBegin(GL_TRIANGLES);
            glColor3d(1, .8f, 0);
            // base
            glVertex2d(0 - 1.4, 0);
            glVertex2d(0 - 1.4, 6);
            glVertex2d(0 + 1.4, 6);
            glVertex2d(0 + 1.4, 0);
            glVertex2d(0 - 1.4, 0);
            glVertex2d(0 + 1.4, 6);
            glEnd();

            glTranslated(0, 0, .1);
            glColor3d(0, 0, 0);
            pfSetPosition(0, 0);
            pfSetScale(3.f);
            SUMOReal w = pfdkGetStringWidth("U");
            glRotated(180, 0, 1, 0);
            glTranslated(-w / 2., 2, 0);
            pfDrawString("U");

            glTranslated(w / 2., -2, 0);
            std::string str = toString((int)(prob * 100)) + "%";
            pfSetPosition(0, 0);
            pfSetScale(.7f);
            w = pfdkGetStringWidth(str.c_str());
            glTranslated(-w / 2., 4, 0);
            pfDrawString(str.c_str());
            glPopMatrix();
        }
        glPopName();
    }
}


std::string
GNERerouterEdge::getAttribute(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_ID:
            return getMicrosimID();
        case SUMO_ATTR_LANE:
            return toString(myLane->getAttribute(SUMO_ATTR_ID));
        case SUMO_ATTR_POSITION:
            return toString(myPosition.x());
        default:
            throw InvalidArgument(toString(getType()) + " attribute '" + toString(key) + "' not allowed");
    }
}


void
GNERerouterEdge::setAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList) {
    // ignore Warnings
    UNUSED_PARAMETER(undoList);
    UNUSED_PARAMETER(value);
    if (value == getAttribute(key)) {
        return; //avoid needless changes, later logic relies on the fact that attributes have changed
    }
    switch (key) {
        case SUMO_ATTR_ID:
        case SUMO_ATTR_LANE:
            throw InvalidArgument("modifying " + toString(getType()) + " attribute '" + toString(key) + "' not allowed");
        default:
            throw InvalidArgument(toString(getType()) + " attribute '" + toString(key) + "' not allowed");
    }
}


bool
GNERerouterEdge::isValid(SumoXMLAttr key, const std::string& value) {
    // ignore Warning
    UNUSED_PARAMETER(value);
    switch (key) {
        case SUMO_ATTR_ID:
        case SUMO_ATTR_LANE:
            throw InvalidArgument("modifying " + toString(getType()) + " attribute '" + toString(key) + "' not allowed");
        default:
            throw InvalidArgument(toString(getType()) + " attribute '" + toString(key) + "' not allowed");
    }
}

void
GNERerouterEdge::setAttribute(SumoXMLAttr key, const std::string& value) {
    // ignore Warning
    UNUSED_PARAMETER(value);
    switch (key) {
        case SUMO_ATTR_ID:
        case SUMO_ATTR_LANE:
            throw InvalidArgument("modifying " + toString(getType()) + " attribute '" + toString(key) + "' not allowed");
        default:
            throw InvalidArgument(toString(getType()) + " attribute '" + toString(key) + "' not allowed");
    }
}


/****************************************************************************/