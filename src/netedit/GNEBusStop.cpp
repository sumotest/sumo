/****************************************************************************/
/// @file    GNEBusStop.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Nov 2015
/// @version $Id$
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
GNEBusStop::GNEBusStop(const std::string& id, GNELane& lane, GNEViewNet* viewNet, SUMOReal fromPos, SUMOReal toPos, const std::vector<std::string>& lines) : 
    GNEStoppingPlace(id, lane, viewNet, SUMO_TAG_BUS_STOP, fromPos, toPos),
    myLines(lines) {
    // When a new additional element is created, updateGeometry() must be called
    updateGeometry();
    // And colors must be configured
    setColors();
}


GNEBusStop::~GNEBusStop() {}


void 
GNEBusStop::updateGeometry() {
    // Clear all containers
    myShapeRotations.clear();
    myShapeLengths.clear();
    
    // Get value of option "lefthand"
    SUMOReal offsetSign = OptionsCont::getOptions().getBool("lefthand") ? -1 : 1;

    // Get shape of lane parent
    myShape = myLane.getShape();

    // Move shape to side
    myShape.move2side(1.65 * offsetSign);

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

    // Get position of the sign
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


void 
GNEBusStop::writeAdditional(OutputDevice& device) {
    // Write parameters
    device.openTag(getTag());
    device.writeAttr(SUMO_ATTR_ID, getID());
    device.writeAttr(SUMO_ATTR_LANE, getLane().getID());
    device.writeAttr(SUMO_ATTR_STARTPOS, myFromPos);
    device.writeAttr(SUMO_ATTR_ENDPOS, myToPos);
    device.writeAttr(SUMO_ATTR_LINES, getAttribute(SUMO_ATTR_LINES));
    // Close tag
    device.closeTag();
}


const  std::vector<std::string>&
GNEBusStop::getLines() const {
    return myLines;
}


void
GNEBusStop::drawGL(const GUIVisualizationSettings& s) const {
    // Additonals element are drawed using a drawGLAdditional
    drawGLAdditional(0, s);
}


void 
GNEBusStop::drawGLAdditional(GUISUMOAbstractView* const parent, const GUIVisualizationSettings& s) const {
    // Ignore Warning
    UNUSED_PARAMETER(parent);
    
    // Declare variables to get colors depending if the busStop is selected
    RGBColor base, sign, letter;

    // Set colors
    if(gSelected.isSelected(getType(), getGlID())) {
        base = myRGBColors[BUSSTOP_BASE_SELECTED];
        sign = myRGBColors[BUSSTOP_SIGN_SELECTED];
        letter = myRGBColors[BUSSTOP_LETTER_SELECTED];
    } else {
        base = myRGBColors[BUSSTOP_BASE];
        sign = myRGBColors[BUSSTOP_SIGN];
        letter = myRGBColors[BUSSTOP_LETTER];
    }

    // Start drawing adding an gl identificator
    glPushName(getGlID());
    
    // Add a draw matrix
    glPushMatrix();

    // Start with the drawing of the area traslating matrix to origin 
    glTranslated(0, 0, getType());

    // Set color of the base
    GLHelper::setColor(base);

    // Obtain exaggeration of the draw
    const SUMOReal exaggeration = s.addSize.getExaggeration(s);

    // Draw the area using shape, shapeRotations, shapeLenghts and value of exaggeration
    GLHelper::drawBoxLines(myShape, myShapeRotations, myShapeLengths, exaggeration);

    // Check if the distance is enought to draw details
    if (s.scale * exaggeration >= 10) {
        
        // Obtain rotation of the sing depeding of the option "lefthand"
        SUMOReal rotSign = OptionsCont::getOptions().getBool("lefthand");

        // Set color of the lines
        GLHelper::setColor(letter);

        // Iterate over every line
        for (size_t i = 0; i != myLines.size(); ++i) {

            // Add a new push matrix
            glPushMatrix();

            // Traslate to positionof signal
            glTranslated(mySignPos.x(), mySignPos.y(), 0);
            
            // Rotate 180 (Eje X -> Mirror)
            glRotated(180, 1, 0, 0);

            // Rotate again depending of the option rotSign
            glRotated(rotSign * mySignRot, 0, 0, 1);
            
            // Set poligon mode
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

            // set polyfront position ot 0 
            pfSetPosition(0, 0);

            // Set polyfront scale to 1
            pfSetScale(1.f);
            
            // traslate matrix for every line
            glTranslated(1.2, -(double)i, 0);

            // draw line
            pfDrawString(myLines[i].c_str());

            // pop matrix
            glPopMatrix();
        }

        // Start drawing sign traslating matrix to signal position
        glTranslated(mySignPos.x(), mySignPos.y(), 0);

        // Define nº points (for efficiency)
        int noPoints = 9;

        // If the scale * exaggeration is more than 25, recalculate nº points
        if (s.scale * exaggeration > 25) 
            noPoints = MIN2((int)(9.0 + (s.scale * exaggeration) / 10.0), 36);
        
        // scale matrix depending of the exaggeration
        glScaled(exaggeration, exaggeration, 1);

        // Draw green circle
        GLHelper::drawFilledCircle((SUMOReal) 1.1, noPoints);

        // Traslate to front
        glTranslated(0, 0, .1);

        // Set color of the lines
        GLHelper::setColor(sign);

        // draw another circle in the same position, but a little bit more small
        GLHelper::drawFilledCircle((SUMOReal) 0.9, noPoints);

        // If the scale * exageration is equal or more than 4.5, draw H
        if (s.scale * exaggeration >= 4.5)
            GLHelper::drawText("H", Position(), .1, 1.6, letter, mySignRot);

        // pop draw matrix
        glPopMatrix();

        // Show Lock icon depending of the Edit mode
        if(dynamic_cast<GNEViewNet*>(parent)->showLockIcon())
            drawLockIcon();
    } else
        // pop draw matrix
        glPopMatrix();

    // Pop name
    glPopName();

    // Draw name
    drawName(getCenteringBoundary().getCenter(), s.scale, s.addName);
};


GUIGLObjectPopupMenu*
GNEBusStop::getPopUpMenu(GUIMainWindow& app, GUISUMOAbstractView& parent) {
    GUIGLObjectPopupMenu* ret = new GUIGLObjectPopupMenu(app, parent, *this);
    buildPopupHeader(ret, app);
    buildCenterPopupEntry(ret);
    new FXMenuCommand(ret, "Copy busStop name to clipboard", 0, ret, MID_COPY_EDGE_NAME);
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
GNEBusStop::getParameterWindow(GUIMainWindow& app, GUISUMOAbstractView& parent) {
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

std::string
GNEBusStop::getAttribute(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_ID:
            return getMicrosimID();
        case SUMO_ATTR_LANE:
            return toString(myLane.getAttribute(SUMO_ATTR_ID));
        case SUMO_ATTR_STARTPOS:
            return toString(myFromPos);
        case SUMO_ATTR_ENDPOS:
            return toString(myToPos);
        case SUMO_ATTR_LINES: {
            // Convert myLines vector into String with the schema "line1 line2 ... lineN"
            std::string myLinesStr;
            for(std::vector<std::string>::const_iterator i = myLines.begin(); i != myLines.end(); i++) {
                if((*i) != myLines.back())
                    myLinesStr += (*i) + " ";
                else
                    myLinesStr += (*i);
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
        case SUMO_ATTR_LANE:
            throw InvalidArgument("modifying busStop attribute '" + toString(key) + "' not allowed");
        case SUMO_ATTR_STARTPOS:
        case SUMO_ATTR_ENDPOS:
        case SUMO_ATTR_LINES:
            undoList->p_add(new GNEChange_Attribute(this, key, value));
            updateGeometry();
            break;
        default:
            throw InvalidArgument("busStop attribute '" + toString(key) + "' not allowed");
    }
}


bool
GNEBusStop::isValid(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_ID:
        case SUMO_ATTR_LANE:
            throw InvalidArgument("modifying busStop attribute '" + toString(key) + "' not allowed");
        case SUMO_ATTR_STARTPOS:
            return (canParse<SUMOReal>(value) && parse<SUMOReal>(value) >= 0 && parse<SUMOReal>(value) < (myToPos-1));
        case SUMO_ATTR_ENDPOS:
            return (canParse<SUMOReal>(value) && parse<SUMOReal>(value) >= 1 && parse<SUMOReal>(value) > myFromPos);
        case SUMO_ATTR_LINES:
            return isValidStringVector(value);
        default:
            throw InvalidArgument("busStop attribute '" + toString(key) + "' not allowed");
    }
}

// ===========================================================================
// private
// ===========================================================================

void
GNEBusStop::setAttribute(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_ID:
        case SUMO_ATTR_LANE:
            throw InvalidArgument("modifying busStop attribute '" + toString(key) + "' not allowed");
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
        case SUMO_ATTR_LINES: 
            myLines.clear();
            SUMOSAXAttributes::parseStringVector(value, myLines);
            getViewNet()->update();
            break;
        default:
            throw InvalidArgument("busStop attribute '" + toString(key) + "' not allowed");
    }
}


void
GNEBusStop::setColors() {
    // Color BUSSTOP_BASE
    myRGBColors.push_back(RGBColor(76, 170, 50, 255));
    // Color BUSSTOP_BASE_SELECTED
    myRGBColors.push_back(RGBColor(161, 255, 135, 255));
    // Color BUSSTOP_SIGN
    myRGBColors.push_back(RGBColor(255, 235, 0, 255));
    // Color BUSSTOP_SIGN_SELECTED
    myRGBColors.push_back(RGBColor(255, 235, 0, 255));
    // Color BUSSTOP_LINES
    myRGBColors.push_back(RGBColor(76, 170, 50, 255));
    // Color BUSSTOP_LINES_SELECTED
    myRGBColors.push_back(RGBColor(161, 255, 135, 255));
}

/****************************************************************************/
