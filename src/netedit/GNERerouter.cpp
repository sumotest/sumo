/****************************************************************************/
/// @file    GNERerouter.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Nov 2015
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

#include "GNERerouter.h"
#include "GNELane.h"
#include "GNEViewNet.h"
#include "GNEUndoList.h"
#include "GNENet.h"
#include "GNEChange_Attribute.h"
#include "GNELogo_E3.cpp"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif

// ===========================================================================
// static member definitions
// ===========================================================================
GUIGlID GNERerouter::rerouterGlID = 0;
bool GNERerouter::rerouterInitialized = false;

// ===========================================================================
// member method definitions
// ===========================================================================

// ---------------------------------------------------------------------------
// GNERerouter::closingReroute - methods
// ---------------------------------------------------------------------------

GNERerouter::closingReroute::closingReroute(std::string closedEdgeId, std::vector<std::string> allowVehicles, std::vector<std::string> disallowVehicles) :
    myClosedEdgeId(closedEdgeId),
    myAllowVehicles(allowVehicles),
    myDisallowVehicles(disallowVehicles) {
}


GNERerouter::closingReroute::~closingReroute() {
}


void
GNERerouter::closingReroute::insertAllowVehicle(std::string vehicleid) {
    // Check if was already inserted
    for(std::vector<std::string>::iterator i = myAllowVehicles.begin(); i != myAllowVehicles.end(); i++)
        if((*i) == vehicleid)
            throw ProcessError(vehicleid + " already inserted");
    // insert in vector
    myAllowVehicles.push_back(vehicleid);
}


void
GNERerouter::closingReroute::removeAllowVehicle(std::string vehicleid) {
    // find and remove
    for(std::vector<std::string>::iterator i = myAllowVehicles.begin(); i != myAllowVehicles.end(); i++)
        if((*i) == vehicleid) {
            myAllowVehicles.erase(i);
            return;
        }
    // Throw error if don't exist
    throw ProcessError(vehicleid + " not exist");
}


void
GNERerouter::closingReroute::insertDisallowVehicle(std::string vehicleid) {
    // Check if was already inserted
    for(std::vector<std::string>::iterator i = myDisallowVehicles.begin(); i != myDisallowVehicles.end(); i++)
        if((*i) == vehicleid)
            throw ProcessError(vehicleid + " already inserted");
    // insert in vector
    myDisallowVehicles.push_back(vehicleid);
}


void
GNERerouter::closingReroute::removeDisallowVehicle(std::string vehicleid) {
    // find and remove
    for(std::vector<std::string>::iterator i = myDisallowVehicles.begin(); i != myDisallowVehicles.end(); i++)
        if((*i) == vehicleid) {
            myDisallowVehicles.erase(i);
            return;
        }
    // Throw error if don't exist
    throw ProcessError(vehicleid + " not exist");
}


std::vector<std::string>
GNERerouter::closingReroute::getAllowVehicles() const {
    return myAllowVehicles;
}


std::vector<std::string>
GNERerouter::closingReroute::getDisallowVehicles() const {
    return myDisallowVehicles;
}


std::string
GNERerouter::closingReroute::getClosedEdgeId() const {
    return myClosedEdgeId;
}

// ---------------------------------------------------------------------------
// GNERerouter::destProbReroute - methods
// ---------------------------------------------------------------------------

GNERerouter::destProbReroute::destProbReroute(std::string newDestinationId, SUMOReal probability):
    myNewDestinationId(newDestinationId),
    myProbability(probability) {
}


GNERerouter::destProbReroute::~destProbReroute() {
}


std::string
GNERerouter::destProbReroute::getNewDestinationId() const {
    return myNewDestinationId;
}


SUMOReal
GNERerouter::destProbReroute::getProbability() const {
    return myProbability;
}


void
GNERerouter::destProbReroute::setProbability(SUMOReal probability) {
    if(probability >= 0 && probability <= 1)
        myProbability = probability;
    else
        throw InvalidArgument(toString(probability) + " isn't a probability");
}

// ---------------------------------------------------------------------------
// GNERerouter::routeProbReroute - methods
// ---------------------------------------------------------------------------

GNERerouter::routeProbReroute::routeProbReroute(std::string newRouteId, SUMOReal probability) :
    myNewRouteId(newRouteId),
    myProbability(probability) {
}


GNERerouter::routeProbReroute::~routeProbReroute() {
}


std::string
GNERerouter::routeProbReroute::getNewRouteId() const {
    return myNewRouteId;
}


SUMOReal
GNERerouter::routeProbReroute::getProbability() const {
    return myProbability;
}


void
GNERerouter::routeProbReroute::setProbability(SUMOReal probability) {
    if(probability >= 0 && probability <= 1)
        myProbability = probability;
    else
        throw InvalidArgument(toString(probability) + " isn't a probability");
}

// ---------------------------------------------------------------------------
// GNERerouter::rerouterInterval - methods
// ---------------------------------------------------------------------------

GNERerouter::rerouterInterval::rerouterInterval(SUMOTime begin, SUMOTime end) :
    myBegin(begin),
    myEnd(end) {
}


GNERerouter::rerouterInterval::~rerouterInterval() {
}


void
GNERerouter::rerouterInterval::insertClosingReroutes(GNERerouter::closingReroute *cr) {
    // Check if was already inserted
    for(std::vector<closingReroute*>::iterator i = myClosingReroutes.begin(); i != myClosingReroutes.end(); i++)
        if((*i) == cr)
            throw ProcessError("closing reroute " + cr->getClosedEdgeId() + " already inserted");
    // insert in vector
    myClosingReroutes.push_back(cr);
}


void
GNERerouter::rerouterInterval::removeClosingReroutes(GNERerouter::closingReroute *cr) {
    // find and remove
    for(std::vector<closingReroute*>::iterator i = myClosingReroutes.begin(); i != myClosingReroutes.end(); i++)
        if((*i) == cr) {
            myClosingReroutes.erase(i);
            return;
        }
    // Throw error if don't exist
    throw ProcessError("closing reroute " + cr->getClosedEdgeId() + " not exist");
}


void
GNERerouter::rerouterInterval::insertDestProbReroutes(GNERerouter::destProbReroute *dpr) {
    // Check if was already inserted
    for(std::vector<destProbReroute*>::iterator i = myDestProbReroutes.begin(); i != myDestProbReroutes.end(); i++)
        if((*i) == dpr)
            throw ProcessError("destiny probability reroute " + dpr->getNewDestinationId() + " already inserted");
    // insert in vector
    myDestProbReroutes.push_back(dpr);
}


void
GNERerouter::rerouterInterval::removeDestProbReroutes(GNERerouter::destProbReroute *dpr) {
    // find and remove
    for(std::vector<destProbReroute*>::iterator i = myDestProbReroutes.begin(); i != myDestProbReroutes.end(); i++)
        if((*i) == dpr) {
            myDestProbReroutes.erase(i);
            return;
        }
    // Throw error if don't exist
    throw ProcessError("destiny probability reroute " + dpr->getNewDestinationId() + " not exist");
}


void
GNERerouter::rerouterInterval::insertRouteProbReroute(GNERerouter::routeProbReroute *rpr) {
    // Check if was already inserted
    for(std::vector<routeProbReroute*>::iterator i = myRouteProbReroutes.begin(); i != myRouteProbReroutes.end(); i++)
        if((*i) == rpr)
            throw ProcessError("route probability reroute " + rpr->getNewRouteId() + " already inserted");
    // insert in vector
    myRouteProbReroutes.push_back(rpr);
}


void
GNERerouter::rerouterInterval::removeRouteProbReroute(GNERerouter::routeProbReroute *rpr) {
    // find and remove
    for(std::vector<routeProbReroute*>::iterator i = myRouteProbReroutes.begin(); i != myRouteProbReroutes.end(); i++)
        if((*i) == rpr) {
            myRouteProbReroutes.erase(i);
            return;
        }
    // Throw error if don't exist
    throw ProcessError("route probability reroute " + rpr->getNewRouteId() + " not exist");
}


SUMOTime
GNERerouter::rerouterInterval::getBegin() const {
    return myBegin;
}


SUMOTime
GNERerouter::rerouterInterval::getEnd() const {
    return myEnd;
}


std::vector<GNERerouter::closingReroute*>
GNERerouter::rerouterInterval::getClosingReroutes() const {
    return myClosingReroutes;
}


std::vector<GNERerouter::destProbReroute*>
GNERerouter::rerouterInterval::getDestProbReroutes() const {
    return myDestProbReroutes;
}


std::vector<GNERerouter::routeProbReroute*>
GNERerouter::rerouterInterval::getRouteProbReroutes() const {
    return myRouteProbReroutes;
}


void
GNERerouter::rerouterInterval::setBegin(SUMOTime begin) {
    if(begin >= 0 && begin >= myEnd)
        myBegin = begin;
    else
        throw InvalidArgument("Begin time not valid");
}


void
GNERerouter::rerouterInterval::setEnd(SUMOTime end) {
    if(end >= 0 && myBegin >= end)
        myEnd = myEnd;
    else
        throw InvalidArgument("Begin time not valid");
}

// ---------------------------------------------------------------------------
// GNERerouter - methods
// ---------------------------------------------------------------------------

GNERerouter::GNERerouter(const std::string& id, GNEViewNet* viewNet, Position pos, std::vector<GNEEdge*> edges, const std::string& filename, SUMOReal probability, bool off, bool blocked) :
    GNEAdditionalSet(id, viewNet, pos, SUMO_TAG_E3DETECTOR, blocked),
    myEdges(edges),
    myFilename(filename),
    myProbability(probability),
    myOff(off) {
    // Update geometry;
    updateGeometry();
    // Set colors
    myBaseColor = RGBColor(76, 170, 50, 255);
    myBaseColorSelected = RGBColor(161, 255, 135, 255);
}


GNERerouter::~GNERerouter() {
}


void
GNERerouter::updateGeometry() {
    myShape.clear();
    myShape.push_back(myPosition);
}


void
GNERerouter::moveAdditional(SUMOReal posx, SUMOReal posy, GNEUndoList *undoList) {
    // if item isn't blocked
    if(myBlocked == false) {
        // change Position
        undoList->p_add(new GNEChange_Attribute(this, SUMO_ATTR_POSITION, toString(Position(posx, posy, 0))));
    }
}


void
GNERerouter::writeAdditional(OutputDevice& device) {

}


GUIParameterTableWindow*
GNERerouter::getParameterWindow(GUIMainWindow& app, GUISUMOAbstractView& parent) {
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
GNERerouter::drawGL(const GUIVisualizationSettings& s) const {
    // Additonals element are drawed using a drawGLAdditional
    drawGLAdditional(0, s);
}


void
GNERerouter::drawGLAdditional(GUISUMOAbstractView* const parent, const GUIVisualizationSettings& s) const {
    // Ignore Warning
    UNUSED_PARAMETER(parent);

    // Start drawing adding an gl identificator
    glPushName(getGlID());

    // load detector logo, if wasn't inicializated
    if (!rerouterInitialized) {
        FXImage* i = new FXGIFImage(getViewNet()->getNet()->getApp(), GNELogo_E3, IMAGE_KEEP | IMAGE_SHMI | IMAGE_SHMP);
        rerouterGlID = GUITexturesHelper::add(i);
        rerouterInitialized = true;
        delete i;
    }

    // Add a draw matrix and draw E1 logo
    glPushMatrix();
    glTranslated(myShape[0].x(), myShape[0].y(), getType() + 0.1);
    glColor3d(1, 1, 1);
    glRotated(180, 0, 0, 1);
    GUITexturesHelper::drawTexturedBox(rerouterGlID, 1);

    // Pop draw matrix
    glPopMatrix();

    // Pop name
    glPopName();

    // Draw name
    drawName(getCenteringBoundary().getCenter(), s.scale, s.addName);
}


std::string
GNERerouter::getAttribute(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_ID:
            return getMicrosimID();
        case SUMO_ATTR_EDGES:
            /** completar **/
        case SUMO_ATTR_FILE:
            return myFilename;
        case SUMO_ATTR_PROB:
            return toString(myProbability);
        case SUMO_ATTR_OFF:
            return toString(myOff);
        default:
            throw InvalidArgument(toString(getType()) + " attribute '" + toString(key) + "' not allowed");
    }
}


void
GNERerouter::setAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList) {
    if (value == getAttribute(key)) {
        return; //avoid needless changes, later logic relies on the fact that attributes have changed
    }
    switch (key) {
        case SUMO_ATTR_ID:
            throw InvalidArgument("modifying " + toString(getType()) + " attribute '" + toString(key) + "' not allowed");
        case SUMO_ATTR_EDGES:
        case SUMO_ATTR_FILE:
        case SUMO_ATTR_PROB:
        case SUMO_ATTR_OFF:
            undoList->p_add(new GNEChange_Attribute(this, key, value));
            updateGeometry();
            break;
        default:
            throw InvalidArgument(toString(getType()) + " attribute '" + toString(key) + "' not allowed");
    }
}


bool
GNERerouter::isValid(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_ID:
            throw InvalidArgument("modifying " + toString(getType()) + " attribute '" + toString(key) + "' not allowed");
        case SUMO_ATTR_FREQUENCY:
            return (canParse<SUMOReal>(value) && parse<SUMOReal>(value) >= 0);
        case SUMO_ATTR_EDGES:
            /** completar **/
        case SUMO_ATTR_FILE:
            return isValidFileValue(value);
        case SUMO_ATTR_PROB:
            return canParse<SUMOReal>(value);
        case SUMO_ATTR_OFF:
            return canParse<bool>(value);
        default:
            throw InvalidArgument(toString(getType()) + " attribute '" + toString(key) + "' not allowed");
    }
}


void
GNERerouter::setAttribute(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_ID:
        case SUMO_ATTR_LANE:
            throw InvalidArgument("modifying detector E3 attribute '" + toString(key) + "' not allowed");
        case SUMO_ATTR_EDGES:
            /** completar **/
            break;
        case SUMO_ATTR_FILE:
            myFilename = value;
            break;
        case SUMO_ATTR_PROB:
            myProbability = parse<SUMOReal>(value);
            break;
        case SUMO_ATTR_OFF:
            myOff = parse<bool>(value);
            break;
        default:
            throw InvalidArgument("detector E3 attribute '" + toString(key) + "' not allowed");
    }
}

/****************************************************************************/