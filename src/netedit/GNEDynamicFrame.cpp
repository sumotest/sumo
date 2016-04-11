/****************************************************************************/
/// @file    GNEDynamicFrame.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Apr 2016
/// @version $Id: GNESelector.cpp 20306 2016-03-25 14:16:22Z palcraft $
///
// Abstract class for dynamic frames
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2015 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
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

#include "GNEDynamicFrame.h"


#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// FOX callback mapping
// ===========================================================================
/*
FXDEFMAP(GNESelector) GNESelectorMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_CHOOSEN_LOAD,       GNESelector::onCmdLoad),
    FXMAPFUNC(SEL_COMMAND,  MID_CHOOSEN_SAVE,       GNESelector::onCmdSave),
    FXMAPFUNC(SEL_COMMAND,  MID_CHOOSEN_INVERT,     GNESelector::onCmdInvert),
    FXMAPFUNC(SEL_COMMAND,  MID_CHOOSEN_CLEAR,      GNESelector::onCmdClear),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SELMB_TAG,      GNESelector::onCmdSelMBTag),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SELMB_STRING,   GNESelector::onCmdSelMBString),
    FXMAPFUNC(SEL_COMMAND,  MID_HELP,               GNESelector::onCmdHelp),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SELECT_SCALE,   GNESelector::onCmdScaleSelection)
};

// Object implementation
FXIMPLEMENT(GNESelector, FXScrollWindow, GNESelectorMap, ARRAYNUMBER(GNESelectorMap))
*/
// ===========================================================================
// static members
// ===========================================================================


// ===========================================================================
// method definitions
// ===========================================================================



/****************************************************************************/
