/****************************************************************************/
/// @file    GNELane.cpp
/// @author  Jakob Erdmann
/// @date    Feb 2011
/// @version $Id$
///
// A class for visualizing Lane geometry (adapted from GNELaneWrapper)
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2016 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
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
#include <utils/foxtools/MFXUtils.h>
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
#include <utils/gui/images/GUITextureSubSys.h>

#include "GNELane.h"
#include "GNEEdge.h"
#include "GNEJunction.h"
#include "GNETLSEditorFrame.h"
#include "GNEInternalLane.h"
#include "GNEUndoList.h"
#include "GNENet.h"
#include "GNEChange_Attribute.h"
#include "GNEViewNet.h"
#include "GNEViewParent.h"
#include "GNEConnection.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS

// ===========================================================================
// FOX callback mapping
// ===========================================================================

// Object implementation
FXIMPLEMENT(GNELane, FXDelegator, 0, 0)

// ===========================================================================
// method definitions
// ===========================================================================

GNELane::GNELane(GNEEdge& edge, const int index) :
    GNENetElement(edge.getNet(), edge.getNBEdge()->getLaneID(index), GLO_LANE, SUMO_TAG_LANE),
    myParentEdge(edge),
    myIndex(index),
    mySpecialColor(0),
    myIcon(0),  // PABLO #1568
    myTLSEditor(0) {
    updateGeometry();
}

GNELane::GNELane() :
    GNENetElement(NULL, "dummyConstructorGNELane", GLO_LANE, SUMO_TAG_LANE),
    myParentEdge(*static_cast<GNEEdge*>(0)),
    myIndex(-1),
    mySpecialColor(0),
    myIcon(0),  // PABLO #1568
    myTLSEditor(0) {
}


GNELane::~GNELane() {
    // Remove all references to this lane in their additionals
    for (AdditionalVector::iterator i = myAdditionals.begin(); i != myAdditionals.end(); i++) {
        (*i)->removeLaneReference();
    }
}


void
GNELane::drawLinkNo() const {
}


void
GNELane::drawTLSLinkNo() const {
}


void
GNELane::drawLinkRules() const {
}


void
GNELane::drawArrows() const {
    const Position& end = getShape().back();
    const Position& f = getShape()[-2];
    SUMOReal rot = (SUMOReal) atan2((end.x() - f.x()), (f.y() - end.y())) * (SUMOReal) 180.0 / (SUMOReal) PI;
    glPushMatrix();
    glPushName(0);
    glTranslated(0, 0, GLO_JUNCTION + .1); // must draw on top of junction shape
    glColor3d(1, 1, 1);
    glTranslated(end.x(), end.y(), 0);
    glRotated(rot, 0, 0, 1);

    // draw all links
    const std::vector<NBEdge::Connection>& edgeCons = myParentEdge.getNBEdge()->myConnections;
    NBNode* dest = myParentEdge.getNBEdge()->myTo;
    for (std::vector<NBEdge::Connection>::const_iterator i = edgeCons.begin(); i != edgeCons.end(); ++i) {
        if ((*i).fromLane == myIndex) {
            LinkDirection dir = dest->getDirection(myParentEdge.getNBEdge(), i->toEdge, OptionsCont::getOptions().getBool("lefthand"));
            switch (dir) {
                case LINKDIR_STRAIGHT:
                    GLHelper::drawBoxLine(Position(0, 4), 0, 2, .05);
                    GLHelper::drawTriangleAtEnd(Position(0, 4), Position(0, 1), (SUMOReal) 1, (SUMOReal) .25);
                    break;
                case LINKDIR_LEFT:
                    GLHelper::drawBoxLine(Position(0, 4), 0, 1.5, .05);
                    GLHelper::drawBoxLine(Position(0, 2.5), 90, 1, .05);
                    GLHelper::drawTriangleAtEnd(Position(0, 2.5), Position(1.5, 2.5), (SUMOReal) 1, (SUMOReal) .25);
                    break;
                case LINKDIR_RIGHT:
                    GLHelper::drawBoxLine(Position(0, 4), 0, 1.5, .05);
                    GLHelper::drawBoxLine(Position(0, 2.5), -90, 1, .05);
                    GLHelper::drawTriangleAtEnd(Position(0, 2.5), Position(-1.5, 2.5), (SUMOReal) 1, (SUMOReal) .25);
                    break;
                case LINKDIR_TURN:
                    GLHelper::drawBoxLine(Position(0, 4), 0, 1.5, .05);
                    GLHelper::drawBoxLine(Position(0, 2.5), 90, .5, .05);
                    GLHelper::drawBoxLine(Position(0.5, 2.5), 180, 1, .05);
                    GLHelper::drawTriangleAtEnd(Position(0.5, 2.5), Position(0.5, 4), (SUMOReal) 1, (SUMOReal) .25);
                    break;
                case LINKDIR_TURN_LEFTHAND:
                    GLHelper::drawBoxLine(Position(0, 4), 0, 1.5, .05);
                    GLHelper::drawBoxLine(Position(0, 2.5), -90, 1, .05);
                    GLHelper::drawBoxLine(Position(-0.5, 2.5), -180, 1, .05);
                    GLHelper::drawTriangleAtEnd(Position(-0.5, 2.5), Position(-0.5, 4), (SUMOReal) 1, (SUMOReal) .25);
                    break;
                case LINKDIR_PARTLEFT:
                    GLHelper::drawBoxLine(Position(0, 4), 0, 1.5, .05);
                    GLHelper::drawBoxLine(Position(0, 2.5), 45, .7, .05);
                    GLHelper::drawTriangleAtEnd(Position(0, 2.5), Position(1.2, 1.3), (SUMOReal) 1, (SUMOReal) .25);
                    break;
                case LINKDIR_PARTRIGHT:
                    GLHelper::drawBoxLine(Position(0, 4), 0, 1.5, .05);
                    GLHelper::drawBoxLine(Position(0, 2.5), -45, .7, .05);
                    GLHelper::drawTriangleAtEnd(Position(0, 2.5), Position(-1.2, 1.3), (SUMOReal) 1, (SUMOReal) .25);
                    break;
                case LINKDIR_NODIR:
                    GLHelper::drawBoxLine(Position(1, 5.8), 245, 2, .05);
                    GLHelper::drawBoxLine(Position(-1, 5.8), 115, 2, .05);
                    glTranslated(0, 5, 0);
                    GLHelper::drawOutlineCircle(0.9, 0.8, 32);
                    glTranslated(0, -5, 0);
                    break;
            }
        }
    }
    glPopName();
    glPopMatrix();
}


void
GNELane::drawLane2LaneConnections() const {
    glPushMatrix();
    glPushName(0);
    glTranslated(0, 0, GLO_JUNCTION + .1); // must draw on top of junction shape
    std::vector<NBEdge::Connection> connections = myParentEdge.getNBEdge()->getConnectionsFromLane(myIndex);
    NBNode* node = myParentEdge.getNBEdge()->getToNode();
    const Position& startPos = getShape()[-1];
    for (std::vector<NBEdge::Connection>::iterator it = connections.begin(); it != connections.end(); it++) {
        const LinkState state = node->getLinkState(
                                    myParentEdge.getNBEdge(), it->toEdge, it->fromLane, it->toLane, it->mayDefinitelyPass, it->tlID);
        switch (state) {
            case LINKSTATE_TL_OFF_NOSIGNAL:
                glColor3d(1, 1, 0);
                break;
            case LINKSTATE_TL_OFF_BLINKING:
                glColor3d(0, 1, 1);
                break;
            case LINKSTATE_MAJOR:
                glColor3d(1, 1, 1);
                break;
            case LINKSTATE_MINOR:
                glColor3d(.4, .4, .4);
                break;
            case LINKSTATE_STOP:
                glColor3d(.7, .4, .4);
                break;
            case LINKSTATE_EQUAL:
                glColor3d(.7, .7, .7);
                break;
            case LINKSTATE_ALLWAY_STOP:
                glColor3d(.7, .7, 1);
            case LINKSTATE_ZIPPER:
                glColor3d(.75, .5, 0.25);
                break;
            default:
                throw ProcessError("Unexpected LinkState '" + toString(state) + "'");
        }
        const Position& endPos = it->toEdge->getLaneShape(it->toLane)[0];
        glBegin(GL_LINES);
        glVertex2f(startPos.x(), startPos.y());
        glVertex2f(endPos.x(), endPos.y());
        glEnd();
        GLHelper::drawTriangleAtEnd(startPos, endPos, (SUMOReal) 1.5, (SUMOReal) .2);
    }
    glPopName();
    glPopMatrix();
}


void
GNELane::drawGL(const GUIVisualizationSettings& s) const {
    glPushMatrix();
    glPushName(getGlID());
    glTranslated(0, 0, getType());
    const bool selectedEdge = gSelected.isSelected(myParentEdge.getType(), myParentEdge.getGlID());
    const bool selected = gSelected.isSelected(getType(), getGlID());
    if (mySpecialColor != 0) {
        GLHelper::setColor(*mySpecialColor);
    } else if (selected && s.laneColorer.getActive() != 1) {
        // override with special colors (unless the color scheme is based on selection)
        GLHelper::setColor(GNENet::selectedLaneColor);
    } else if (selectedEdge && s.laneColorer.getActive() != 1) {
        // override with special colors (unless the color scheme is based on selection)
        GLHelper::setColor(GNENet::selectionColor);
    } else {
        const GUIColorer& c = s.laneColorer;
        if (!setFunctionalColor(c.getActive()) && !setMultiColor(c)) {
            GLHelper::setColor(c.getScheme().getColor(getColorValue(c.getActive())));
        }
    }

    // draw lane
    // check whether it is not too small
    const SUMOReal selectionScale = selected || selectedEdge ? s.selectionScale : 1;
    SUMOReal exaggeration = selectionScale * s.laneWidthExaggeration; // * s.laneScaler.getScheme().getColor(getScaleValue(s.laneScaler.getActive()));
    // XXX apply usefull scale values
    //exaggeration *= s.laneScaler.getScheme().getColor(getScaleValue(s.laneScaler.getActive()));

    // recognize full transparency and simply don't draw
    GLfloat color[4];
    glGetFloatv(GL_CURRENT_COLOR, color);
    if (color[3] == 0 || s.scale * exaggeration < s.laneMinSize) {
        glPopMatrix();
    } else if (s.scale * exaggeration < 1.) {
        // draw as lines
        if (myShapeColors.size() > 0) {
            GLHelper::drawLine(getShape(), myShapeColors);
        } else {
            GLHelper::drawLine(getShape());
        }
        glPopMatrix();
    } else {
        if (drawAsRailway(s)) {
            // draw as railway
            const SUMOReal halfRailWidth = 0.725 * exaggeration;
            if (myShapeColors.size() > 0) {
                GLHelper::drawBoxLines(getShape(), myShapeRotations, myShapeLengths, myShapeColors, halfRailWidth);
            } else {
                GLHelper::drawBoxLines(getShape(), myShapeRotations, myShapeLengths, halfRailWidth);
            }
            RGBColor current = GLHelper::getColor();
            glColor3d(1, 1, 1);
            glTranslated(0, 0, .1);
            GLHelper::drawBoxLines(getShape(), myShapeRotations, myShapeLengths, halfRailWidth - 0.2);
            GLHelper::setColor(current);
            drawCrossties(0.3 * exaggeration, 1 * exaggeration, 1 * exaggeration);
        } else {
            // the actual lane
            // reduce lane width to make sure that a selected edge can still be seen
            const SUMOReal halfWidth = selectionScale * (myParentEdge.getNBEdge()->getLaneWidth(myIndex) / 2 - (selectedEdge ? .3 : 0));
            if (myShapeColors.size() > 0) {
                GLHelper::drawBoxLines(getShape(), myShapeRotations, myShapeLengths, myShapeColors, halfWidth);
            } else {
                GLHelper::drawBoxLines(getShape(), myShapeRotations, myShapeLengths, halfWidth);
            }
        }
        glPopMatrix();
        if (exaggeration == 1) {
            drawMarkings(selectedEdge, exaggeration);
        }

        // draw ROWs only if target junction has a valid logic)
        if (myParentEdge.getGNEJunctionDest()->isLogicValid() && s.scale > 3) {
            drawArrows();
        }
        if (s.showLaneDirection) {
            drawDirectionIndicators();
        }
        // If there are icons to draw:                                                                                  // PABLO #1568
        if((OptionsCont::getOptions().getBool("disable-laneIcons") == false) && myLaneIconsPositions.size() > 0) {      // PABLO #1568
            // Draw list of icons                                                                                       // PABLO #1568
            for(int i = 0; i < (int)myLaneIconsPositions.size(); i++) {                                                 // PABLO #1568
                // Push draw matrix                                                                                     // PABLO #1568
                glPushMatrix();                                                                                         // PABLO #1568
                // Set draw color                                                                                       // PABLO #1568
                glColor3d(1, 1, 1);                                                                                     // PABLO #1568
                // Traslate matrix                                                                                      // PABLO #1568
                glTranslated(myLaneIconsPositions.at(i).x(), myLaneIconsPositions.at(i).y(), getType() + 0.1);          // PABLO #1568
                // Rotate matrix                                                                                        // PABLO #1568
                glRotated(myLaneIconsRotations.at(i), 0, 0, -1);                                                        // PABLO #1568
                glRotated(90, 0, 0, 1);                                                                                 // PABLO #1568
                // draw texture box depending                                                                           // PABLO #1568
                GUITexturesHelper::drawTexturedBox(myIcon, 1);                                                          // PABLO #1568
                // Pop logo matrix                                                                                      // PABLO #1568
                glPopMatrix();                                                                                          // PABLO #1568
            }                                                                                                           // PABLO #1568
        }                                                                                                               // PABLO #1568
    }
    glPopName();
}


void
GNELane::drawMarkings(const bool& selectedEdge, SUMOReal scale) const {
    glPushMatrix();
    glTranslated(0, 0, GLO_EDGE);

    const SUMOReal halfWidth = myParentEdge.getNBEdge()->getLaneWidth(myIndex) * 0.5;
    // optionally draw inverse markings
    if (myIndex > 0 && (myParentEdge.getNBEdge()->getPermissions(myIndex - 1) & myParentEdge.getNBEdge()->getPermissions(myIndex)) != 0) {
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

}


GUIGLObjectPopupMenu*
GNELane::getPopUpMenu(GUIMainWindow& app, GUISUMOAbstractView& parent) {
    GUIGLObjectPopupMenu* ret = new GUIGLObjectPopupMenu(app, parent, *this);
    buildPopupHeader(ret, app);
    buildCenterPopupEntry(ret);
    new FXMenuCommand(ret, "Copy edge name to clipboard", 0, ret, MID_COPY_EDGE_NAME);
    buildNameCopyPopupEntry(ret);
    buildSelectionPopupEntry(ret);
    buildPositionCopyEntry(ret, false);
    const int editMode = parent.getVisualisationSettings()->editMode;
    myTLSEditor = 0;
    if (editMode != GNE_MODE_CONNECT && editMode != GNE_MODE_TLS && editMode != GNE_MODE_CREATE_EDGE) {
        // Get icons                                                            // PABLO #1568
        FXIcon* pedestrianIcon = GUIIconSubSys::getIcon(ICON_LANEPEDESTRIAN);   // PABLO #1568
        FXIcon* bikeIcon = GUIIconSubSys::getIcon(ICON_LANEBIKE);               // PABLO #1568
        FXIcon* busIcon = GUIIconSubSys::getIcon(ICON_LANEBUS);                 // PABLO #1568
        // Create basic commands
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
            // Create panel for lane operations                                                                                                                 // PABLO #1568
            FXMenuPane *addSpecialLanes = new FXMenuPane(ret);                                                                                                  // PABLO #1568
            FXMenuPane *removeSpecialLanes = new FXMenuPane(ret);                                                                                               // PABLO #1568
            FXMenuPane *transformSlanes = new FXMenuPane(ret);                                                                                                  // PABLO #1568 
            // Create menu comands for all add special lanes                                                                                                    // PABLO #1568
            new FXMenuCommand(addSpecialLanes, "Sidewalks", pedestrianIcon, &parent, MID_GNE_ADD_LANE_SIDEWALK);                                                // PABLO #1568
            new FXMenuCommand(addSpecialLanes, "Bikelanes", bikeIcon, &parent, MID_GNE_ADD_LANE_BIKE);                                                          // PABLO #1568
            new FXMenuCommand(addSpecialLanes, "Buslanes", busIcon, &parent, MID_GNE_ADD_LANE_BUS);                                                             // PABLO #1568
            // Create menu comands for all remove special lanes and disable it                                                                                  // PABLO #1568
            new FXMenuCommand(removeSpecialLanes, "Sidewalks", pedestrianIcon, &parent, MID_GNE_REMOVE_LANE_SIDEWALK);                                          // PABLO #1568
            new FXMenuCommand(removeSpecialLanes, "Bikelanes", bikeIcon, &parent, MID_GNE_REMOVE_LANE_BIKE);                                                    // PABLO #1568
            new FXMenuCommand(removeSpecialLanes, "Buslanes", busIcon, &parent, MID_GNE_REMOVE_LANE_BUS);                                                       // PABLO #1568
            // Create menu comands for all trasform special lanes and disable it                                                                                // PABLO #1568
            new FXMenuCommand(transformSlanes, "Sidewalsk", pedestrianIcon, &parent, MID_GNE_TRANSFORM_LANE_SIDEWALK);                                          // PABLO #1568
            new FXMenuCommand(transformSlanes, "Bikelanes", bikeIcon, &parent, MID_GNE_TRANSFORM_LANE_BIKE);                                                    // PABLO #1568
            new FXMenuCommand(transformSlanes, "Buslanes", busIcon, &parent, MID_GNE_TRANSFORM_LANE_BUS);                                                       // PABLO #1568
            new FXMenuCommand(transformSlanes, "revert transformations", 0, &parent, MID_GNE_REVERT_TRANSFORMATION);                                            // PABLO #1568
            // add menuCascade for lane operations                                                                                                              // PABLO #1568
            new FXMenuCascade(ret, "add special lanes", 0, addSpecialLanes);                                                                                    // PABLO #1568
            new FXMenuCascade(ret, "remove special lanes", 0, removeSpecialLanes);                                                                              // PABLO #1568
            new FXMenuCascade(ret, "transform to special lanes", 0, transformSlanes);                                                                           // PABLO #1568
        } else {
            new FXMenuCommand(ret, "Duplicate lane", 0, &parent, MID_GNE_DUPLICATE_LANE);
            // Declare flags                                                                                                                                    // PABLO #1568
            bool edgeHasSidewalk = myParentEdge.hasSidewalk();                                                                                                  // PABLO #1568
            bool edgeHasBikelane = myParentEdge.hasBikelane();                                                                                                  // PABLO #1568
            bool edgeHasBuslane = myParentEdge.hasBuslane();                                                                                                    // PABLO #1568
            // Create panel for lane operations                                                                                                                 // PABLO #1568
            FXMenuPane *addSpecialLanes = new FXMenuPane(ret);                                                                                                  // PABLO #1568
            FXMenuPane *removeSpecialLanes = new FXMenuPane(ret);                                                                                               // PABLO #1568
            FXMenuPane *transformSlanes = new FXMenuPane(ret);                                                                                                  // PABLO #1568 
            // Create menu comands for all add special lanes                                                                                                    // PABLO #1568
            FXMenuCommand *addSidewalk = new FXMenuCommand(addSpecialLanes, "Sidewalk", pedestrianIcon, &parent, MID_GNE_ADD_LANE_SIDEWALK);                    // PABLO #1568
            FXMenuCommand *addBikelane = new FXMenuCommand(addSpecialLanes, "Bikelane", bikeIcon, &parent, MID_GNE_ADD_LANE_BIKE);                              // PABLO #1568
            FXMenuCommand *addBuslane = new FXMenuCommand(addSpecialLanes, "Buslane", busIcon, &parent, MID_GNE_ADD_LANE_BUS);                                  // PABLO #1568
            // Create menu comands for all remove special lanes and disable it                                                                                  // PABLO #1568
            FXMenuCommand *removeSidewalk = new FXMenuCommand(removeSpecialLanes, "Sidewalk", pedestrianIcon, &parent, MID_GNE_REMOVE_LANE_SIDEWALK);           // PABLO #1568
            removeSidewalk->disable();                                                                                                                          // PABLO #1568 
            FXMenuCommand *removeBikelane = new FXMenuCommand(removeSpecialLanes, "Bikelane", bikeIcon, &parent, MID_GNE_REMOVE_LANE_BIKE);                     // PABLO #1568
            removeBikelane->disable();                                                                                                                          // PABLO #1568 
            FXMenuCommand *removeBuslane = new FXMenuCommand(removeSpecialLanes, "Buslane", busIcon, &parent, MID_GNE_REMOVE_LANE_BUS);                         // PABLO #1568
            removeBuslane->disable();                                                                                                                           // PABLO #1568 
            // Create menu comands for all trasform special lanes and disable it                                                                                // PABLO #1568
            FXMenuCommand *transformLaneToSidewalk = new FXMenuCommand(transformSlanes, "Sidewalk", pedestrianIcon, &parent, MID_GNE_TRANSFORM_LANE_SIDEWALK);  // PABLO #1568
            FXMenuCommand *transformLaneToBikelane = new FXMenuCommand(transformSlanes, "Bikelane", bikeIcon, &parent, MID_GNE_TRANSFORM_LANE_BIKE);            // PABLO #1568
            FXMenuCommand *transformLaneToBuslane = new FXMenuCommand(transformSlanes, "Buslane", busIcon, &parent, MID_GNE_TRANSFORM_LANE_BUS);                // PABLO #1568
            FXMenuCommand *revertTransformation = new FXMenuCommand(transformSlanes, "revert transformation", 0, &parent, MID_GNE_REVERT_TRANSFORMATION);       // PABLO #1568
            // add menuCascade for lane operations                                                                                                              // PABLO #1568
            FXMenuCascade* cascadeAddSpecialLane = new FXMenuCascade(ret, "add special lane", 0, addSpecialLanes);                                              // PABLO #1568
            FXMenuCascade* cascadeRemoveSpecialLane = new FXMenuCascade(ret, "remove special lane", 0, removeSpecialLanes);                                     // PABLO #1568
            new FXMenuCascade(ret, "transform to special lane", 0, transformSlanes);                                                                            // PABLO #1568
            // Enable and disable options depending of current transform of the lane                                                                            // PABLO #1568
            if(edgeHasSidewalk) {                                                                                                                               // PABLO #1568
                transformLaneToSidewalk->disable();                                                                                                             // PABLO #1568
                addSidewalk->disable();                                                                                                                         // PABLO #1568
                removeSidewalk->enable();                                                                                                                       // PABLO #1568
            }                                                                                                                                                   // PABLO #1568
            if (edgeHasBikelane) {                                                                                                                              // PABLO #1568
                transformLaneToBikelane->disable();                                                                                                             // PABLO #1568
                addBikelane->disable();                                                                                                                         // PABLO #1568
                removeBikelane->enable();                                                                                                                       // PABLO #1568
            }                                                                                                                                                   // PABLO #1568
            if (edgeHasBuslane){                                                                                                                                // PABLO #1568
                transformLaneToBuslane->disable();                                                                                                              // PABLO #1568
                addBuslane->disable();                                                                                                                          // PABLO #1568
                removeBuslane->enable();                                                                                                                        // PABLO #1568
            }                                                                                                                                                   // PABLO #1568
            // Check if cascade menus must be disabled                                                                                                          // PABLO #1568
            if(edgeHasSidewalk && edgeHasBikelane && edgeHasBuslane) {                                                                                          // PABLO #1568
                cascadeAddSpecialLane->disable();                                                                                                               // PABLO #1568
            }                                                                                                                                                   // PABLO #1568
            if(!edgeHasSidewalk && !edgeHasBikelane && !edgeHasBuslane) {                                                                                       // PABLO #1568
                cascadeRemoveSpecialLane->disable();                                                                                                            // PABLO #1568
            }                                                                                                                                                   // PABLO #1568
            // Enable or disable revert transformation                                                                                                          // PABLO #1568
            if(isSidewalk() || isBikelane() || isBuslane()) {                                                                                                   // PABLO #1568
                revertTransformation->enable();                                                                                                                 // PABLO #1568
            } else {                                                                                                                                            // PABLO #1568
                revertTransformation->disable();                                                                                                                // PABLO #1568
            }                                                                                                                                                   // PABLO #1568
        }
    } else if (editMode == GNE_MODE_TLS) {
        myTLSEditor = static_cast<GNEViewNet&>(parent).getViewParent()->getTLSEditorFrame();
        if (myTLSEditor->controlsEdge(myParentEdge)) {
            new FXMenuCommand(ret, "Select state for all links from this edge:", 0, 0, 0);
            const std::vector<std::string> names = GNEInternalLane::LinkStateNames.getStrings();
            for (std::vector<std::string>::const_iterator it = names.begin(); it != names.end(); it++) {
                FXuint state = GNEInternalLane::LinkStateNames.get(*it);
                FXMenuRadio* mc = new FXMenuRadio(ret, (*it).c_str(), this, FXDataTarget::ID_OPTION + state);
                mc->setSelBackColor(MFXUtils::getFXColor(GNEInternalLane::colorForLinksState(state)));
                mc->setBackColor(MFXUtils::getFXColor(GNEInternalLane::colorForLinksState(state)));
            }
        }
    } else {
        FXMenuCommand* mc = new FXMenuCommand(ret, "Additional options available in 'Inspect Mode'", 0, 0, 0);
        mc->handle(&parent, FXSEL(SEL_COMMAND, FXWindow::ID_DISABLE), 0);
    }
    // buildShowParamsPopupEntry(ret, false);
    new FXMenuSeparator(ret);
    const SUMOReal pos = getShape().nearest_offset_to_point2D(parent.getPositionInformation());
    const SUMOReal height = getShape().positionAtOffset2D(getShape().nearest_offset_to_point2D(parent.getPositionInformation())).z();
    new FXMenuCommand(ret, ("Shape pos: " + toString(pos)).c_str(), 0, 0, 0);
    new FXMenuCommand(ret, ("Length pos: " + toString(getPositionRelativeToShapeLenght(pos))).c_str(), 0, 0, 0);
    new FXMenuCommand(ret, ("Height: " + toString(height)).c_str(), 0, 0, 0);
    // new FXMenuSeparator(ret);
    // buildPositionCopyEntry(ret, false);

    // let the GNEViewNet store the popup position
    (dynamic_cast<GNEViewNet&>(parent)).markPopupPosition();
    return ret;
}


GUIParameterTableWindow*
GNELane::getParameterWindow(GUIMainWindow& app, GUISUMOAbstractView&) {
    GUIParameterTableWindow* ret =
        new GUIParameterTableWindow(app, *this, 2);
    // add items
    ret->mkItem("length [m]", false, myParentEdge.getNBEdge()->getLength());
    // close building
    ret->closeBuilding();
    return ret;
}


Boundary
GNELane::getCenteringBoundary() const {
    Boundary b = getShape().getBoxBoundary();
    b.grow(10);
    return b;
}


const PositionVector&
GNELane::getShape() const {
    return myParentEdge.getNBEdge()->getLaneShape(myIndex);
}


const std::vector<SUMOReal>&
GNELane::getShapeRotations() const {
    return myShapeRotations;
}


const std::vector<SUMOReal>&
GNELane::getShapeLengths() const {
    return myShapeLengths;
}


Boundary
GNELane::getBoundary() const {
    return myParentEdge.getNBEdge()->getLaneStruct(myIndex).shape.getBoxBoundary();
}


void
GNELane::updateGeometry() {
    // Clear containers
    myShapeRotations.clear();
    myShapeLengths.clear();
    myLaneIconsPositions.clear();   // PABLO #1568
    myLaneIconsRotations.clear();   // PABLO #1568
    //SUMOReal length = myParentEdge.getLength(); // @todo see ticket #448
    // may be different from length
    
    // Obtain lane and shape rotations
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
    // Update geometry of additionals vinculated with this lane
    for (AdditionalVector::iterator i = myAdditionals.begin(); i != myAdditionals.end(); i++) {
        (*i)->updateGeometry();
    }
    // Update geometry of additionalSets vinculated to this lane
    for (AdditionalSetVector::iterator i = myAdditionalSets.begin(); i != myAdditionalSets.end(); ++i) {
        (*i)->updateGeometry();
    }
    // Update incoming connections of this lane                                                                         // PABLO #2067
    std::vector<GNEConnection*> incomingConnections = getGNEIncomingConnections();                                      // PABLO #2067
    for(std::vector<GNEConnection*>::iterator i = incomingConnections.begin(); i != incomingConnections.end(); i++) {   // PABLO #2067
        (*i)->updateGeometry();                                                                                         // PABLO #2067
    }                                                                                                                   // PABLO #2067
    // Update outgoings connections of this lane                                                                        // PABLO #2067
    std::vector<GNEConnection*> outGoingConnections = getGNEOutcomingConnections();                                     // PABLO #2067
    for(std::vector<GNEConnection*>::iterator i = outGoingConnections.begin(); i != outGoingConnections.end(); i++) {   // PABLO #2067
        (*i)->updateGeometry();                                                                                         // PABLO #2067
    }                                                                                                                   // PABLO #2067
    // If lane has enought lenght                                                       // PABLO #1568
    if((getLaneShapeLenght() > 4)) {                                                    // PABLO #1568
        bool calculatePositionAndRotations = true;                                      // PABLO #1568
        // check if lane is special                                                     // PABLO #1568
        if(isSidewalk()) {                                                              // PABLO #1568
            myIcon = GUITextureSubSys::getGif(GNETEXTURE_LANEPEDESTRIAN);               // PABLO #1568
        } else if (isBikelane()) {                                                      // PABLO #1568
            myIcon = GUITextureSubSys::getGif(GNETEXTURE_LANEBIKE);                     // PABLO #1568
        } else if (isBuslane()) {                                                       // PABLO #1568
            myIcon = GUITextureSubSys::getGif(GNETEXTURE_LANEBUS);                      // PABLO #1568
        } else {                                                                        // PABLO #1568
            calculatePositionAndRotations = false;                                      // PABLO #1568
            myIcon = 0;                                                                 // PABLO #1568
        }                                                                               // PABLO #1568
        // If lane is special                                                           // PABLO #1568
        if (calculatePositionAndRotations) {                                            // PABLO #1568
            // get values for position and rotation of icons                            // PABLO #1568
            for(int i = 2; i < getLaneShapeLenght() - 1; i += 15) {                     // PABLO #1568
                // @todo optimice getting it in UpdateGeometry()                        // PABLO #1568
                myLaneIconsPositions.push_back(getShape().positionAtOffset(i));         // PABLO #1568
                myLaneIconsRotations.push_back(getShape().rotationDegreeAtOffset(i));   // PABLO #1568
            }                                                                           // PABLO #1568
        }                                                                               // PABLO #1568
    }                                                                                   // PABLO #1568
}

int
GNELane::getIndex() const {
    return myIndex;
}

void
GNELane::setIndex(int index) {
    myIndex = index;
    setMicrosimID(myParentEdge.getNBEdge()->getLaneID(index));
}


SUMOReal
GNELane::getSpeed() const {
    return myParentEdge.getNBEdge()->getLaneSpeed(myIndex);
}


SUMOReal
GNELane::getLaneParametricLenght() const  {
    return myParentEdge.getNBEdge()->getLoadedLength();
}


SUMOReal
GNELane::getLaneShapeLenght() const {
    return getShape().length();
}


SUMOReal
GNELane::getPositionRelativeToParametricLenght(SUMOReal position) const {
    return (position * getLaneShapeLenght()) / getLaneParametricLenght();
}


SUMOReal
GNELane::getPositionRelativeToShapeLenght(SUMOReal position) const {
    return (position * getLaneParametricLenght()) / getLaneShapeLenght();
}


void
GNELane::addAdditional(GNEAdditional* additional) {
    myAdditionals.push_back(additional);
}


bool
GNELane::removeAdditional(GNEAdditional* additional) {
    // Find and remove stoppingPlace
    for (AdditionalVector::iterator i = myAdditionals.begin(); i != myAdditionals.end(); i++) {
        if (*i == additional) {
            myAdditionals.erase(i);
            return true;
        }
    }
    return false;
}


const std::vector<GNEAdditional*>&
GNELane::getAdditionals() const {
    return myAdditionals;
}


bool
GNELane::addAdditionalSet(GNEAdditionalSet* additionalSet) {
    // Check if additionalSet already exists before insertion
    for (AdditionalSetVector::iterator i = myAdditionalSets.begin(); i != myAdditionalSets.end(); i++) {
        if ((*i) == additionalSet) {
            return false;
        }
    }
    // Insert it and retur true
    myAdditionalSets.push_back(additionalSet);
    return true;
}


bool
GNELane::removeAdditionalGeometrySet(GNEAdditionalSet* additionalSet) {
    // search additionalSet and remove it
    for (AdditionalSetVector::iterator i = myAdditionalSets.begin(); i != myAdditionalSets.end(); i++) {
        if ((*i) == additionalSet) {
            myAdditionalSets.erase(i);
            return true;
        }
    }
    // If additionalSet wasn't found, return false
    return false;
}


const std::vector<GNEAdditionalSet*>&
GNELane::getAdditionalSets() {
    return myAdditionalSets;
}


bool                                                                    // PABLO #1568
GNELane::isSidewalk() const {                                           // PABLO #1568
    if(myParentEdge.getNBEdge()->getPermissions(myIndex) == 1048576) {  // PABLO #1568
        return true;                                                    // PABLO #1568
    } else {                                                            // PABLO #1568
        return false;                                                   // PABLO #1568
    }                                                                   // PABLO #1568
}                                                                       // PABLO #1568


bool                                                                    // PABLO #1568
GNELane::isBikelane() const {                                           // PABLO #1568
    if(myParentEdge.getNBEdge()->getPermissions(myIndex) == 524288) {   // PABLO #1568
        return true;                                                    // PABLO #1568
    } else {                                                            // PABLO #1568
        return false;                                                   // PABLO #1568
    }                                                                   // PABLO #1568
}                                                                       // PABLO #1568


bool                                                                    // PABLO #1568
GNELane::isBuslane() const {                                            // PABLO #1568
    if(myParentEdge.getNBEdge()->getPermissions(myIndex) == 256) {      // PABLO #1568
        return true;                                                    // PABLO #1568
    } else {                                                            // PABLO #1568
        return false;                                                   // PABLO #1568
    }                                                                   // PABLO #1568
}                                                                       // PABLO #1568


std::string
GNELane::getAttribute(SumoXMLAttr key) const {
    const NBEdge* edge = myParentEdge.getNBEdge();
    switch (key) {
        case SUMO_ATTR_ID:
            return getMicrosimID();
        case SUMO_ATTR_SPEED:
            return toString(edge->getLaneSpeed(myIndex));
        case SUMO_ATTR_ALLOW:
            // return all allowed classes (may differ from the written attributes)
            return getVehicleClassNames(edge->getPermissions(myIndex));
        case SUMO_ATTR_DISALLOW:
            // return all disallowed classes (may differ from the written attributes)
            return getVehicleClassNames(~(edge->getPermissions(myIndex)));
        case SUMO_ATTR_WIDTH:
            return toString(edge->getLaneStruct(myIndex).width);
        case SUMO_ATTR_ENDOFFSET:
            return toString(edge->getLaneStruct(myIndex).endOffset);
        case SUMO_ATTR_INDEX:
            return toString(myIndex);
        default:
            throw InvalidArgument("lane attribute '" + toString(key) + "' not allowed");
    }
}


void
GNELane::setAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList) {
    switch (key) {
        case SUMO_ATTR_ID:
            throw InvalidArgument("modifying lane attribute '" + toString(key) + "' not allowed");
        case SUMO_ATTR_SPEED:
        case SUMO_ATTR_ALLOW:
        case SUMO_ATTR_DISALLOW:
        case SUMO_ATTR_WIDTH:
        case SUMO_ATTR_ENDOFFSET:
        case SUMO_ATTR_INDEX:
            // no special handling
            undoList->p_add(new GNEChange_Attribute(this, key, value));
            break;
        default:
            throw InvalidArgument("lane attribute '" + toString(key) + "' not allowed");
    }
}


bool
GNELane::isValid(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_ID:
            return false;
        case SUMO_ATTR_SPEED:
            return canParse<SUMOReal>(value);
        case SUMO_ATTR_ALLOW:
        case SUMO_ATTR_DISALLOW:
            return canParseVehicleClasses(value);
        case SUMO_ATTR_WIDTH:
            return canParse<SUMOReal>(value) && (isPositive<SUMOReal>(value) || parse<SUMOReal>(value) == NBEdge::UNSPECIFIED_WIDTH);
        case SUMO_ATTR_ENDOFFSET:
            return canParse<SUMOReal>(value);
        case SUMO_ATTR_INDEX:
            return value == toString(myIndex);
        default:
            throw InvalidArgument("lane attribute '" + toString(key) + "' not allowed");
    }
}


void
GNELane::setSpecialColor(const RGBColor* color) {
    mySpecialColor = color;
}

// ===========================================================================
// private
// ===========================================================================

void
GNELane::setAttribute(SumoXMLAttr key, const std::string& value) {
    NBEdge* edge = myParentEdge.getNBEdge();
    switch (key) {
        case SUMO_ATTR_ID:
            throw InvalidArgument("modifying lane attribute '" + toString(key) + "' not allowed");
        case SUMO_ATTR_SPEED:
            edge->setSpeed(myIndex, parse<SUMOReal>(value));
            break;
        case SUMO_ATTR_ALLOW:
            edge->setPermissions(parseVehicleClasses(value), myIndex);
            updateGeometry();               // PABLO #1568
            myNet->getViewNet()->update();  // PABLO #1568
            break;
        case SUMO_ATTR_DISALLOW:
            edge->setPermissions(~parseVehicleClasses(value), myIndex); // negation yields allowed
            updateGeometry();               // PABLO #1568
            myNet->getViewNet()->update();  // PABLO #1568
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
}


bool
GNELane::setFunctionalColor(int activeScheme) const {
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
GNELane::setMultiColor(const GUIColorer& c) const {
    const int activeScheme = c.getActive();
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
GNELane::getColorValue(int activeScheme) const {
    const SVCPermissions myPermissions = myParentEdge.getNBEdge()->getPermissions(myIndex);
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
                   gSelected.isSelected(GLO_EDGE, dynamic_cast<GNEEdge*>(&myParentEdge)->getGlID());
        case 2:
            return (SUMOReal)myPermissions;
        case 3:
            return myParentEdge.getNBEdge()->getLaneSpeed(myIndex);
        case 4:
            return myParentEdge.getNBEdge()->getNumLanes();
        case 5: {
            return myParentEdge.getNBEdge()->getLoadedLength() / myParentEdge.getNBEdge()->getLength();
        }
        // case 6: by angle (functional)
        case 7: {
            return myParentEdge.getNBEdge()->getPriority();
        }
        case 8: {
            // color by z of first shape point
            return getShape()[0].z();
        }
        // case 9: by segment height
        case 10: {
            // color by incline
            return (getShape()[-1].z() - getShape()[0].z()) /  myParentEdge.getNBEdge()->getLength();
        }
    }
    return 0;
}


bool
GNELane::drawAsRailway(const GUIVisualizationSettings& s) const {
    return isRailway(myParentEdge.getNBEdge()->getPermissions(myIndex)) && s.showRails;
}


bool
GNELane::drawAsWaterway(const GUIVisualizationSettings& s) const {
    return isWaterway(myParentEdge.getNBEdge()->getPermissions(myIndex)) && s.showRails; // reusing the showRails setting
}


void
GNELane::drawCrossties(SUMOReal length, SUMOReal spacing, SUMOReal halfWidth) const {
    glPushMatrix();
    // draw on top of of the white area between the rails
    glTranslated(0, 0, 0.1);
    int e = (int) getShape().size() - 1;
    for (int i = 0; i < e; ++i) {
        glPushMatrix();
        glTranslated(getShape()[i].x(), getShape()[i].y(), 0.0);
        glRotated(myShapeRotations[i], 0, 0, 1);
        for (SUMOReal t = 0; t < myShapeLengths[i]; t += spacing) {
            glBegin(GL_QUADS);
            glVertex2d(-halfWidth, -t);
            glVertex2d(-halfWidth, -t - length);
            glVertex2d(halfWidth, -t - length);
            glVertex2d(halfWidth, -t);
            glEnd();
        }
        glPopMatrix();
    }
    glPopMatrix();
}


void
GNELane::drawDirectionIndicators() const {
    const SUMOReal width = myParentEdge.getNBEdge()->getLaneWidth(myIndex);
    glColor3d(0.3, 0.3, 0.3);
    glPushMatrix();
    glTranslated(0, 0, GLO_JUNCTION + 0.1);
    int e = (int) getShape().size() - 1;
    for (int i = 0; i < e; ++i) {
        glPushMatrix();
        glTranslated(getShape()[i].x(), getShape()[i].y(), 0.1);
        glRotated(myShapeRotations[i], 0, 0, 1);
        for (SUMOReal t = 0; t < myShapeLengths[i]; t += width) {
            const SUMOReal length = MIN2(width * (SUMOReal)0.5, myShapeLengths[i] - t);
            glBegin(GL_TRIANGLES);
            glVertex2d(0, -t - length);
            glVertex2d(-width * 0.25, -t);
            glVertex2d(+width * 0.25, -t);
            glEnd();
        }
        glPopMatrix();
    }
    glPopMatrix();
}



const std::string&
GNELane::getParentName() const {
    return myParentEdge.getMicrosimID();
}


long
GNELane::onDefault(FXObject* obj, FXSelector sel, void* data) {
    if (myTLSEditor != 0) {
        myTLSEditor->handleMultiChange(this, obj, sel, data);
    }
    return 1;
}


GNEEdge&
GNELane::getParentEdge() {
    return myParentEdge;
}


std::vector<GNEConnection*>                                                                                                                     // PABLO #2067
GNELane::getGNEIncomingConnections() {                                                                                                          // PABLO #2067
    // Declare a vector to save incoming connections                                                                                            // PABLO #2067
    std::vector<GNEConnection*> incomingConnections;                                                                                            // PABLO #2067
    // Obtain incoming edges if junction source was already created                                                                             // PABLO #2067
    GNEJunction *junctionSource =  myParentEdge.getGNEJunctionSource();                                                                         // PABLO #2067
    if(junctionSource) {                                                                                                                        // PABLO #2067
        std::vector<GNEEdge*> incomingEdges = junctionSource->getIncomingGNEEdges();                                                            // PABLO #2067
        // Iterate over incoming edges                                                                                                          // PABLO #2067
        for(std::vector<GNEEdge*>::iterator i = incomingEdges.begin(); i != incomingEdges.end(); i++) {                                         // PABLO #2067
            // Iterate over connection of incoming edges                                                                                        // PABLO #2067
            for(std::vector<GNEConnection*>::const_iterator j = (*i)->getGNEConnections().begin(); j != (*i)->getGNEConnections().end(); j++) { // PABLO #2067
                if((*j)->getNBEdgeConnection().fromLane == getIndex()) {                                                                        // PABLO #2067
                    incomingConnections.push_back(*j);                                                                                          // PABLO #2067
                }                                                                                                                               // PABLO #2067
            }                                                                                                                                   // PABLO #2067
        }                                                                                                                                       // PABLO #2067
    }                                                                                                                                           // PABLO #2067
    return incomingConnections;                                                                                                                 // PABLO #2067
}                                                                                                                                               // PABLO #2067


std::vector<GNEConnection*>                                                                                         // PABLO #2067
GNELane::getGNEOutcomingConnections() {                                                                             // PABLO #2067
    // Obtain GNEConnection of edge parent                                                                          // PABLO #2067
    const std::vector<GNEConnection*>& edgeConnections = myParentEdge.getGNEConnections();                          // PABLO #2067
    std::vector<GNEConnection*> outcomingConnections;                                                               // PABLO #2067
    // Obtain outgoing connections                                                                                  // PABLO #2067
    for(std::vector<GNEConnection*>::const_iterator i = edgeConnections.begin(); i != edgeConnections.end(); i++) { // PABLO #2067
        if((*i)->getNBEdgeConnection().fromLane == getIndex()) {                                                    // PABLO #2067
            outcomingConnections.push_back(*i);                                                                     // PABLO #2067
        }                                                                                                           // PABLO #2067
    }                                                                                                               // PABLO #2067
    return outcomingConnections;                                                                                    // PABLO #2067
}                                                                                                                   // PABLO #2067

/****************************************************************************/
