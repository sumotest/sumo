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
#include "GNEEdge.h"
#include "GNEJunction.h"
#include "GNEUndoList.h"
#include "GNENet.h"
#include "GNEChange_Attribute.h"
#include "GNEViewNet.h"
#include "GNELogo_Lock.cpp"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif


// ===========================================================================
// static member definitions
// ===========================================================================
GUIGlID GNEAdditional::additionalLockGlID = 0;
bool GNEAdditional::additionalLockInitialized = false;

// ===========================================================================
// member method definitions
// ===========================================================================

GNEAdditional::GNEAdditional(const std::string& id, GNELane& lane, GNEViewNet* viewNet, SumoXMLTag tag, bool blocked) :
    GUIGlObject(GLO_ADDITIONAL, id),
    myLane(lane),
    myViewNet(viewNet),
    myBlocked(blocked),
    GNEAttributeCarrier(tag) {
    // Add a reference to this new additional to laneParent
    myLane.addAdditional(this);
}


GNEAdditional::~GNEAdditional() {
    // Remove reference to this new additional to laneParent
    myLane.removeAdditional(this);
}


GNELane&
GNEAdditional::getLane() const {
    return myLane;
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


void 
GNEAdditional::setBlocked(bool value) {
    myBlocked = value;
}


const std::string& 
GNEAdditional::getParentName() const {
    return myLane.getMicrosimID();
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
    glPushMatrix();
    glTranslated(myBlockIconPos.x(), myBlockIconPos.y(), .2);
    glColor3d(1, 1, 1);
    GUITexturesHelper::drawTexturedBox(additionalLockGlID, 0.5);
    glPopMatrix();
}


/****************************************************************************/
