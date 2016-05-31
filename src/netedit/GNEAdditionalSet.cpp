/****************************************************************************/
/// @file    GNEAdditionalSet.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Feb 2015
/// @version $Id: GNEAdditionalSet.cpp 19909 2016-02-08 12:22:59Z palcraft $
///
/// A abstract class for representation of set of additional elements
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

#include "GNEAdditionalSet.h"
#include "GNEAdditional.h"
#include "GNEUndoList.h"
#include "GNELane.h"
#include "GNEEdge.h"
#include "GNENet.h"
#include "GNEViewNet.h"
#include "GNELogo_Lock.cpp"
#include "GNELogo_Empty.cpp"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif


// ===========================================================================
// member method definitions
// ===========================================================================

GNEAdditionalSet::GNEAdditionalSet(const std::string& id, GNEViewNet* viewNet, Position pos, SumoXMLTag tag, bool blocked) :
    GNEAdditional(id, viewNet, pos, tag, NULL, blocked) {
}


GNEAdditionalSet::~GNEAdditionalSet() {
}


bool
GNEAdditionalSet::addAdditionalChild(GNEAdditional *additional) {
    for(std::list<GNEAdditional*>::iterator i = myChildAdditionals.begin(); i != myChildAdditionals.end(); i++)
        if((*i) == additional)
            return false;
    // If wasn't found, insert it
    myChildAdditionals.push_back(additional);
}


bool
GNEAdditionalSet::removeAdditionalChild(GNEAdditional *additional) {
    return false;
}


bool
GNEAdditionalSet::addEdgeChild(GNEEdge *edge, SUMOReal position) {
    if(myChildEdges.count(edge) == 0) {
        myChildEdges[edge] = std::pair<Position, SUMOReal>(Position(), 0);
        updateConnections();
        return true;
    }
    else
        return false;
}


bool
GNEAdditionalSet::removeEdgeChild(GNEEdge *edge) {
    return false;
}


bool
GNEAdditionalSet::addLaneChild(GNELane *lane, SUMOReal position) {
    if(myChildLanes.count(lane) == 0) {
        myChildLanes[lane] = std::pair<Position, SUMOReal>(Position(), 0);
        updateConnections();
        return true;
    }
    else
        return false;
}


bool
GNEAdditionalSet::removeLaneChild(GNELane *lane) {
    return false;
}


void
GNEAdditionalSet::updateConnections() {
    // Clear map with the middle positions
    myConnectionMiddlePosition.clear();

    // Iterate over additonals
    for(std::list<GNEAdditional*>::iterator i = myChildAdditionals.begin(); i != myChildAdditionals.end(); i++) {
        // If shape isn't empty, calculate middle point.
        if((*i)->getShape().size() > 0) {
            Position PositionOfChild = (*i)->getShape()[0];
            SUMOReal angleBetweenParentAndChild = myPosition.angleTo2D(PositionOfChild);
            SUMOReal distancieBetweenParentAndChild = myPosition.distanceTo2D(PositionOfChild);
            myConnectionMiddlePosition[*i] = Position(myPosition.x() + cos(angleBetweenParentAndChild) * distancieBetweenParentAndChild, myPosition.y());
        }
    }

    // Iterate over lanes
    for(std::map<GNELane*, std::pair<Position, SUMOReal> >::iterator i = myChildLanes.begin(); i != myChildLanes.end(); i++) {
        // Set position of figure over Lane
        i->second.first = i->first->getShape().positionAtOffset(i->first->getShape().length() - 10);
        // Set rotation of figure in Lane
        i->second.second = i->first->getShape().rotationDegreeAtOffset(i->first->getShape().length() - 10) * -1;
        // Set position of connection
        SUMOReal angleBetweenParentAndChild = myPosition.angleTo2D(i->second.first);
        SUMOReal distancieBetweenParentAndChild = myPosition.distanceTo2D(i->second.first);
        myConnectionMiddlePosition[i->first] = Position(myPosition.x() + cos(angleBetweenParentAndChild) * distancieBetweenParentAndChild, myPosition.y());
    }
}


void
GNEAdditionalSet::drawConnections() const {
    // Iterate over list of additionals
    for(std::list<GNEAdditional*>::const_iterator i = myChildAdditionals.begin(); i != myChildAdditionals.end(); i++) {
        // Draw only if additional GL Visualitation is enabled
        if(myViewNet->isAdditionalGLVisualisationEnabled(*i) && ((*i)->getShape().size() > 0)) {
            // Add a draw matrix
            glPushMatrix();
            // Set color of the base
            GLHelper::setColor(RGBColor(255, 235, 0, 255));
            // Draw Line
            GLHelper::drawLine(myPosition, myConnectionMiddlePosition.at(*i));
            GLHelper::drawLine(myConnectionMiddlePosition.at(*i), (*i)->getPositionInView());
            // Pop draw matrix
            glPopMatrix();
        }
    }
    // Iterate over lanes
    for(std::map<GNELane*, std::pair<Position, SUMOReal> >::const_iterator i = myChildLanes.begin(); i != myChildLanes.end(); i++) {
        // Add a draw matrix
        glPushMatrix();
        // Set color of the base
        GLHelper::setColor(RGBColor(255, 235, 0, 255));
        // Draw Line
        GLHelper::drawLine(myPosition, myConnectionMiddlePosition.at(i->first));
        GLHelper::drawLine(myConnectionMiddlePosition.at(i->first), i->second.first);
        // Pop draw matrix
        glPopMatrix();
    }
}


void
GNEAdditionalSet::writeAdditionalChildrens(OutputDevice& device) {
    /*
    for(int i = myChild; i != myAdditionals.end(); i++)
        i->writeAdditional(device);
        */
}


int 
GNEAdditionalSet::getNumberOfAdditionalChilds() const {
    return myChildAdditionals.size();
}

/****************************************************************************/
