/****************************************************************************/
/// @file    GNEVariableSpeedSignalDialog.cpp
/// @author  Pablo Alvarez Lopez
/// @date    April 2016
/// @version $Id: GNEVariableSpeedSignalDialog.cpp 20472 2016-04-15 15:36:45Z palcraft $
///
/// A class for edit phases of Variable Speed Signals
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

#include <iostream>
#include <utils/gui/windows/GUIAppEnum.h>
#include "GNEVariableSpeedSignalDialog.h"
#include "GNEVariableSpeedSignal.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif


// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(GNEVariableSpeedSignalDialog) GNERerouterDialogMap[] = {
    FXMAPFUNC(SEL_COMMAND, MID_GNE_MODE_ADDITIONALDIALOG_ACCEPT, GNEVariableSpeedSignalDialog::onCmdAccept),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_MODE_ADDITIONALDIALOG_CANCEL, GNEVariableSpeedSignalDialog::onCmdCancel),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_MODE_ADDITIONALDIALOG_RESET,  GNEVariableSpeedSignalDialog::onCmdReset),
};

// Object implementation
FXIMPLEMENT(GNEVariableSpeedSignalDialog, FXDialogBox, GNERerouterDialogMap, ARRAYNUMBER(GNERerouterDialogMap))

// ===========================================================================
// static member definitions
// ===========================================================================


// ===========================================================================
// member method definitions
// ===========================================================================

GNEVariableSpeedSignalDialog::GNEVariableSpeedSignalDialog(GNEVariableSpeedSignal *variableSpeedSignalParent) : 
    GNEAdditionalDialog(variableSpeedSignalParent),
    myVariableSpeedSignalParent(variableSpeedSignalParent) {
}

GNEVariableSpeedSignalDialog::~GNEVariableSpeedSignalDialog() {
}


long 
GNEVariableSpeedSignalDialog::onCmdAccept(FXObject*, FXSelector, void*) {
    // Close dialog
    closeAdditionalDialog();
    return 1;
}


long
GNEVariableSpeedSignalDialog::onCmdCancel(FXObject*, FXSelector, void*) {
    // Close dialog
    closeAdditionalDialog();
    return 1;
}


long
GNEVariableSpeedSignalDialog::onCmdReset(FXObject*, FXSelector, void*) {
    return 1;
}

/****************************************************************************/
