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
// member method definitions
// ===========================================================================

GNEAdditionalSet::GNEAdditionalSet(const std::string& id, GNEViewNet* viewNet, Position pos, SumoXMLTag tag, bool blocked, GNELane *lane) :
    GNEAdditional(id, viewNet, pos, tag, blocked, lane) {
}


GNEAdditionalSet::~GNEAdditionalSet() {
}


void 
GNEAdditionalSet::addAdditional(GNEAdditional *additional) {
    

    std::map<GNEAdditional*, PositionVector> myAdditionals;

    //throw ProcessError("Attempt to delete instance of GNEReferenceCounter with count " + toString(myCount));

}


void 
GNEAdditionalSet::removeAdditional(GNEAdditional *additional) {

}

void 
GNEAdditionalSet::updateConnections() {

}


void 
GNEAdditionalSet::drawConnections() {

}


void 
GNEAdditionalSet::writeAdditionalChildrens(OutputDevice& device) {
    for(std::vector<GNEAdditional*>::iterator i = myAdditionals.begin(); i != myAdditionals.end(); i++)
        (*i)->writeAdditional(device);
}

/****************************************************************************/
