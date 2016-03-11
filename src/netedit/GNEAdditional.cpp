/****************************************************************************/
/// @file    GNEAdditional.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Dec 2015
/// @version $Id$
///
/// A abstract class for representation of additional elements
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

#include "GNEAdditional.h"
#include "GNELane.h"
#include "GNEUndoList.h"
#include "GNENet.h"
#include "GNEViewNet.h"
#include "GNELogo_Lock.cpp"
#include "GNELogo_Empty.cpp"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif


// ===========================================================================
// static member definitions
// ===========================================================================
GUIGlID GNEAdditional::additionalLockGlID = 0;
GUIGlID GNEAdditional::additionalEmptyGlID = 0;
bool GNEAdditional::additionalLockInitialized = false;
bool GNEAdditional::additionalEmptyInitialized = false;

// ===========================================================================
// member method definitions
// ===========================================================================

GNEAdditional::GNEAdditional(const std::string& id, GNEViewNet* viewNet, Position pos, SumoXMLTag tag, bool blocked, SumoXMLTag parentTag, GNEAdditionalSet *parent) :
    GUIGlObject(GLO_ADDITIONAL, id),
    GNEAttributeCarrier(tag),
    myViewNet(viewNet),
    myPosition(pos),
    myBlocked(blocked),
    myParentTag(parentTag),
    myParent(parent) {
    // If this additional belongs to a set, add it.
    if(myParent)
        myParent->addAdditional(this);
}


GNEAdditional::~GNEAdditional() {
    // If this additional belongs to a set, remove it.
    if(myParent)
        myParent->removeAdditional(this);
}


const Position&
GNEAdditional::getPositionInView() const {
    return myPosition;
}


GNEViewNet* 
GNEAdditional::getViewNet() const {
    return myViewNet;
}


std::string
GNEAdditional::getShape() const {
    return toString(myShape);
}


bool 
GNEAdditional::isBlocked() const {
    return myBlocked;
}


bool 
GNEAdditional::belongToAdditionalSet() const {
    return (myParentTag != SUMO_TAG_NOTHING);
}


GNEAdditionalSet* 
GNEAdditional::getAdditionalSetParent() const {
    return myParent;
}

void 
GNEAdditional::setBlocked(bool value) {
    myBlocked = value;
}


void 
GNEAdditional::setPositionInView(const Position &pos) {
    myPosition = pos;
}


Boundary 
GNEAdditional::getCenteringBoundary() const {
    Boundary b = myShape.getBoxBoundary();
    b.grow(20);
    return b;
}


void 
GNEAdditional::drawLockIcon() const {
    // load additional lock, if wasn't inicializated
    if (!additionalLockInitialized) {
        FXImage* i = new FXGIFImage(getViewNet()->getNet()->getApp(), GNELogo_Lock, IMAGE_KEEP | IMAGE_SHMI | IMAGE_SHMP);
        additionalLockGlID = GUITexturesHelper::add(i);
        additionalLockInitialized = true;
        delete i;
    }
    // load additional empty, if wasn't inicializated
    if (!additionalEmptyInitialized) {
        FXImage* i = new FXGIFImage(getViewNet()->getNet()->getApp(), GNELogo_Empty, IMAGE_KEEP | IMAGE_SHMI | IMAGE_SHMP);
        additionalEmptyGlID = GUITexturesHelper::add(i);
        additionalEmptyInitialized = true;
        delete i;
    }
    // Draw icon
    glPushMatrix();
    glTranslated(myBlockIconPos.x(), myBlockIconPos.y(), getType() + 0.1);
    glColor3d(1, 1, 1);
    glRotated(180, 0, 0, 1);
    // If myBlocked is enable, draw lock, in other case, draw empty square
    if(myBlocked)
        GUITexturesHelper::drawTexturedBox(additionalLockGlID, 0.5);
    else
        GUITexturesHelper::drawTexturedBox(additionalEmptyGlID, 0.5);
    // Pop matrix
    glPopMatrix();
}


/****************************************************************************/
