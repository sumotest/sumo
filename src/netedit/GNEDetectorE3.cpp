/****************************************************************************/
/// @file    GNEDetectorE3.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Nov 2015
/// @version $Id: GNEDetectorE3.cpp 19861 2016-02-01 09:08:47Z palcraft $
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

#include "GNEDetectorE3.h"
#include "GNEDetectorE3EntryExit.h"
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
GUIGlID GNEDetectorE3::detectorE3GlID = 0;
bool GNEDetectorE3::detectorE3Initialized = false;

// ===========================================================================
// member method definitions
// ===========================================================================

GNEDetectorE3::GNEDetectorE3(const std::string& id, GNEViewNet* viewNet, SUMOReal freq, const std::string& filename) :
    myId(id),
    myViewNet(viewNet),
    myFreq(freq),
    myFilename(filename),
    myCounterId(0) {
}


GNEDetectorE3::~GNEDetectorE3() {
}


void 
GNEDetectorE3::addEntry(GNELane& lane, SUMOReal pos) {
    myGNEDetectorE3EntryExits.push_back(new GNEDetectorE3EntryExit("tmpID", this, SUMO_TAG_DET_ENTRY, lane, pos));
}


void 
GNEDetectorE3::removeEntry(GNELane& lane, SUMOReal pos) {
    
}


void 
GNEDetectorE3::addExit(GNELane& lane, SUMOReal pos) {
    myGNEDetectorE3EntryExits.push_back(new GNEDetectorE3EntryExit("tmpID", this, SUMO_TAG_DET_EXIT, lane, pos));
}


void 
GNEDetectorE3::removeExit(GNELane& lane, SUMOReal pos) {
}

/****************************************************************************/