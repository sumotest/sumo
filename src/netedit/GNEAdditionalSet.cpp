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
GUIGlID GNEAdditionalSet::additionalSetLockGlID = 0;
GUIGlID GNEAdditionalSet::additionalSetEmptyGlID = 0;
bool GNEAdditionalSet::additionalSetLockInitialized = false;
bool GNEAdditionalSet::additionalSetEmptyInitialized = false;

// ===========================================================================
// member method definitions
// ===========================================================================

GNEAdditionalSet::GNEAdditionalSet(const std::string& id, GNEViewNet* viewNet, SumoXMLTag tag, bool blocked) :
    GUIGlObject(GLO_ADDITIONAL, id),
    myViewNet(viewNet),
    myBlocked(blocked),
    GNEAttributeCarrier(tag) {
}


GNEAdditionalSet::~GNEAdditionalSet() {
}


void 
GNEAdditionalSet::addAdditional(GNEAdditional *additional) {
    
    //throw ProcessError("Attempt to delete instance of GNEReferenceCounter with count " + toString(myCount));

}


void 
GNEAdditionalSet::removeAdditional(GNEAdditional *additional) {

}


GNEViewNet* 
GNEAdditionalSet::getViewNet() const {
    return myViewNet;
}


std::string
GNEAdditionalSet::getShape() const {
    return toString(myShape);
}


Boundary
GNEAdditionalSet::getCenteringBoundary() const {
    Boundary b = myShape.getBoxBoundary();
    b.grow(20);
    return b;
}


bool 
GNEAdditionalSet::isBlocked() const {
    return myBlocked;
}


void 
GNEAdditionalSet::setBlocked(bool value) {
    myBlocked = value;
}

void 
GNEAdditionalSet::drawLockIcon() const {
    
    // load additional lock, if wasn't inicializated
    if (!additionalSetLockInitialized) {
        FXImage* i = new FXGIFImage(getViewNet()->getNet()->getApp(), GNELogo_Lock, IMAGE_KEEP | IMAGE_SHMI | IMAGE_SHMP);
        additionalSetLockGlID = GUITexturesHelper::add(i);
        additionalSetLockInitialized = true;
        delete i;
    }
    // load additional empty, if wasn't inicializated
    if (!additionalSetEmptyInitialized) {
        FXImage* i = new FXGIFImage(getViewNet()->getNet()->getApp(), GNELogo_Empty, IMAGE_KEEP | IMAGE_SHMI | IMAGE_SHMP);
        additionalSetEmptyGlID = GUITexturesHelper::add(i);
        additionalSetEmptyInitialized = true;
        delete i;
    }
    
    // Draw icon
    glPushMatrix();
    glTranslated(myBlockIconPos.x(), myBlockIconPos.y(), getType() + 1);
    glColor3d(1, 1, 1);
    glRotated(180, 0, 0, 1);
    // If myBlocked is enable, draw lock, in other case, draw empty square
    if(myBlocked)
        GUITexturesHelper::drawTexturedBox(additionalSetLockGlID, 0.5);
    else
        GUITexturesHelper::drawTexturedBox(additionalSetEmptyGlID, 0.5);
    // Pop matrix
    glPopMatrix();
}


/****************************************************************************/
