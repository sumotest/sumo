/****************************************************************************/
/// @file    GNERouteProbe.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Nov 2015
/// @version $Id: GNERouteProbe.cpp 19861 2016-02-01 09:08:47Z palcraft $
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
#include <utils/geom/GeomConvHelper.h>
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

#include "GNEViewNet.h"
#include "GNERouteProbe.h"
#include "GNEEdge.h"
#include "GNELane.h"
#include "GNEViewNet.h"
#include "GNEUndoList.h"
#include "GNENet.h"
#include "GNEChange_Attribute.h"
#include "GNELogo_RouteProbe.cpp"
#include "GNELogo_RouteProbeSelected.cpp"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif

// ===========================================================================
// static member definitions
// ===========================================================================
GUIGlID GNERouteProbe::myRouteProbeGlID = 0;
GUIGlID GNERouteProbe::myRouteProbeSelectedGlID = 0;
bool GNERouteProbe::myRouteProbeInitialized = false;
bool GNERouteProbe::myRouteProbeSelectedInitialized = false;

// ===========================================================================
// member method definitions
// ===========================================================================


GNERouteProbe::GNERouteProbe(const std::string& id, GNEViewNet* viewNet, GNEEdge &edge, int frequency, const std::string& filename, int begin, bool blocked) :
    GNEAdditional(id, viewNet, Position(), SUMO_TAG_ROUTEPROBE, NULL, SUMO_TAG_NOTHING, NULL, blocked),
    myEdge(edge),
    myFrequency(frequency),
    myFilename(filename),
    myBegin(begin) {
    // Add additional to edge parent
    myEdge.addAdditional(this);
    // Update geometry;
    updateGeometry();
    // Set colors
    // load RouteProbe logo, if wasn't inicializated
    if (!myRouteProbeInitialized) {
        FXImage* i = new FXGIFImage(getViewNet()->getNet()->getApp(), GNELogo_RouteProbe, IMAGE_KEEP | IMAGE_SHMI | IMAGE_SHMP);
        myRouteProbeGlID = GUITexturesHelper::add(i);
        myRouteProbeInitialized = true;
        delete i;
    }

    // load RouteProbe selected logo, if wasn't inicializated
    if (!myRouteProbeSelectedInitialized) {
        FXImage* i = new FXGIFImage(getViewNet()->getNet()->getApp(), GNELogo_RouteProbeSelected, IMAGE_KEEP | IMAGE_SHMI | IMAGE_SHMP);
        myRouteProbeSelectedGlID = GUITexturesHelper::add(i);
        myRouteProbeSelectedInitialized = true;
        delete i;
    }
}


GNERouteProbe::~GNERouteProbe() {
    // remove additional from edge parent
    myEdge.removeAdditional(this);
}


void
GNERouteProbe::updateGeometry() {
    // Clear shape
    myShape.clear();

    // Set position
    myShape.push_back(myPosition);

    // Set block icon offset
    myBlockIconOffset = Position(-0.5, -0.5);

    // Set block icon rotation, and using their rotation for draw logo
    setBlockIconRotation();

}


void
GNERouteProbe::moveAdditional(SUMOReal posx, SUMOReal posy, GNEUndoList *undoList) {
    // if item isn't blocked
    if(myBlocked == false) {
        // change Position
        undoList->p_add(new GNEChange_Attribute(this, SUMO_ATTR_POSITION, toString(Position(posx, posy, 0))));
    }
}


void
GNERouteProbe::writeAdditional(OutputDevice& device) {
    // Write parameters
    device.openTag(getTag());
    device.writeAttr(SUMO_ATTR_ID, getID());
    device.writeAttr(SUMO_ATTR_FREQUENCY, myFrequency);
    if(!myFilename.empty())
        device.writeAttr(SUMO_ATTR_FILE, myFilename);
    device.writeAttr(SUMO_ATTR_BEGIN, myBegin);
    // Close tag
    device.closeTag();
}


std::string 
GNERouteProbe::getFilename() const {
    return myFilename;
}


int 
GNERouteProbe::getFrequency() const {
    return myFrequency;
}


int 
GNERouteProbe::getBegin() const {
    return myBegin;
}


void 
GNERouteProbe::setFilename(std::string filename) {
    myFilename = filename;
}


void 
GNERouteProbe::setFrequency(int frequency) {
    myFrequency = frequency;
}


void 
GNERouteProbe::setBegin(int begin) {
    myBegin = begin;
}


GUIParameterTableWindow*
GNERouteProbe::getParameterWindow(GUIMainWindow& app, GUISUMOAbstractView& parent) {
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
GNERouteProbe::drawGL(const GUIVisualizationSettings& s) const {
    // Additonals element are drawed using a drawGLAdditional
    drawGLAdditional(0, s);
}


void
GNERouteProbe::drawGLAdditional(GUISUMOAbstractView* const parent, const GUIVisualizationSettings& s) const {
    // Ignore Warning
    UNUSED_PARAMETER(parent);

    // Start drawing adding an gl identificator
    glPushName(getGlID());

    // Add a draw matrix for drawing logo
    glPushMatrix();
    glTranslated(myShape[0].x(), myShape[0].y(), getType());
    glColor3d(1, 1, 1);
    glRotated(180, 0, 0, 1);

    // Draw icon depending of RouteProbe is or isn't selected
    if(isAdditionalSelected()) 
        GUITexturesHelper::drawTexturedBox(myRouteProbeSelectedGlID, 1);
    else
        GUITexturesHelper::drawTexturedBox(myRouteProbeGlID, 1);

    // Pop draw matrix
    glPopMatrix();
/***
    // Add a draw matrix for id
    glPushMatrix();
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glTranslated(myShape.getLineCenter().x(), myShape.getLineCenter().y(), getType() + 0.1);
    glColor3d(1, 1, 1);
    glRotated(180, 0, 0, 1);
    glTranslated(myIdTextOffset.x(), myIdTextOffset.y(), 0);

    std::string drawText = getID();
    for(int i = 0; i < drawText.size(); i++)
        if(drawText[i] == '_')
            drawText[i] = ' ';

    // Draw id depending of RouteProbe is or isn't selected
    if(isAdditionalSelected()) 
        GLHelper::drawText(drawText, Position(), 0, .45, myBaseColorSelected, 180);
    else
        GLHelper::drawText(drawText, Position(), 0, .45, myBaseColor, 180);
    
    // Pop matrix id
    glPopMatrix();


    // Show Lock icon depending of the Edit mode
    if(dynamic_cast<GNEViewNet*>(parent)->showLockIcon())
        drawLockIcon(0.4);


    // Pop name
    glPopName();

    // Draw name
    drawName(getCenteringBoundary().getCenter(), s.scale, s.addName);
    ***/
}


std::string
GNERouteProbe::getAttribute(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_ID:
            return getMicrosimID();
        case SUMO_ATTR_EDGES:
            /** completar **/
        case SUMO_ATTR_POSITION:
            return toString(myPosition);
        case SUMO_ATTR_FILE:
            return myFilename;
        case SUMO_ATTR_FREQUENCY:
            return toString(myFrequency);
        case SUMO_ATTR_BEGIN:
            return toString(myBegin);
        default:
            throw InvalidArgument(toString(getType()) + " attribute '" + toString(key) + "' not allowed");
    }
}


void
GNERouteProbe::setAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList) {
    if (value == getAttribute(key)) {
        return; //avoid needless changes, later logic relies on the fact that attributes have changed
    }
    switch (key) {
        case SUMO_ATTR_ID:
            throw InvalidArgument("modifying " + toString(getType()) + " attribute '" + toString(key) + "' not allowed");
        case SUMO_ATTR_EDGE:
        case SUMO_ATTR_FILE:
        case SUMO_ATTR_FREQUENCY:
        case SUMO_ATTR_BEGIN:
            undoList->p_add(new GNEChange_Attribute(this, key, value));
            updateGeometry();
            break;
        default:
            throw InvalidArgument(toString(getType()) + " attribute '" + toString(key) + "' not allowed");
    }
}


bool
GNERouteProbe::isValid(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_ID:
            throw InvalidArgument("modifying " + toString(getType()) + " attribute '" + toString(key) + "' not allowed");
        case SUMO_ATTR_POSITION:
            bool ok;
            return GeomConvHelper::parseShapeReporting(value, "user-supplied position", 0, ok, false).size() == 1;
        case SUMO_ATTR_EDGE:
            /** completar **/
        case SUMO_ATTR_FILE:
            return isValidFileValue(value);
        case SUMO_ATTR_FREQUENCY:
            return canParse<int>(value);
        case SUMO_ATTR_BEGIN:
            return canParse<int>(value);
        default:
            throw InvalidArgument(toString(getType()) + " attribute '" + toString(key) + "' not allowed");
    }
}


void
GNERouteProbe::setAttribute(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_ID:
        case SUMO_ATTR_LANE:
            throw InvalidArgument("modifying " + toString(getType()) + " attribute '" + toString(key) + "' not allowed");
        case SUMO_ATTR_EDGE:
            /** completar **/
            break;
        case SUMO_ATTR_FILE:
            myFilename = value;
            break;
        case SUMO_ATTR_FREQUENCY:
            myFrequency = parse<int>(value);
            break;
        case SUMO_ATTR_BEGIN:
            myBegin = parse<int>(value);
            break;
        default:
            throw InvalidArgument(toString(getType()) + " attribute '" + toString(key) + "' not allowed");
    }
}

/****************************************************************************/