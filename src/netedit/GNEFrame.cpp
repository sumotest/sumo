/****************************************************************************/
/// @file    GNEFrame.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Jun 2016
/// @version $Id: GNEFrame.cpp 20831 2016-05-31 15:13:48Z palcraft $
///
/// The Widget for add additional elements
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

#ifdef HAVE_VERSION_H
#include <version.h>
#endif

#include <iostream>
#include <utils/foxtools/fxexdefs.h>
#include <utils/foxtools/MFXUtils.h>
#include <utils/common/MsgHandler.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/div/GUIIOGlobals.h>
#include <utils/gui/div/GUIGlobalSelection.h>
#include <utils/gui/globjects/GUIGlObjectStorage.h>
#include <utils/gui/images/GUIIconSubSys.h>
#include "GNEFrame.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// FOX callback mapping
// ===========================================================================

//FXDEFMAP(GNEFrame) AttrInputMap[] = {};


//FXIMPLEMENT(GNEFrame, FXScrollWindow);
//FXIMPLEMENT(GNEInspectorFrame, FXScrollWindow, GNEInspectorFrameMap, ARRAYNUMBER(GNEInspectorFrameMap))

//FXDECLARE

// ===========================================================================
// static members
// ===========================================================================
const int GNEFrame::myDefaultWidth = 140;

// ===========================================================================
// method definitions
// ===========================================================================

GNEFrame::GNEFrame(FXComposite* parent, GNEViewNet* viewNet) : 
    FXScrollWindow(parent, LAYOUT_FILL_Y | LAYOUT_FIX_WIDTH, 0, 0, myDefaultWidth, 0), 
    myViewNet(viewNet),
    myFrameWidth(myDefaultWidth) {
}


GNEFrame::~GNEFrame() {}


GNEViewNet*
GNEFrame::getViewNet() const {
    return myViewNet;
}


int 
GNEFrame::getFrameWidth() const {
    return myFrameWidth;
}

/****************************************************************************/
