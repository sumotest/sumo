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

    // List with the data
    myDataList = new FXList(this, this);

    // Horizontal frame for row elements
    myRowFrame = new FXHorizontalFrame(this);

    // Text field with data
    myRowData = new FXTextField(myRowFrame, 1, this);

    // Button for insert row
    myAddRow = new FXButton(myRowFrame, "Add", NULL, this);

    // Execute additional dialog (To make it modal)
    execute();
}

GNEVariableSpeedSignalDialog::~GNEVariableSpeedSignalDialog() {
}


long 
GNEVariableSpeedSignalDialog::onCMDInsertRow(FXObject*, FXSelector, void*) {
    return 1;
}


long 
GNEVariableSpeedSignalDialog::onCmdRemoveRow(FXObject*, FXSelector, void*) {
    return 1;
}


long 
GNEVariableSpeedSignalDialog::onCmdAccept(FXObject* sender, FXSelector sel, void* ptr) {

    // Stop Modal
    getApp()->stopModal(this,TRUE);
    return 1;
}


long
GNEVariableSpeedSignalDialog::onCmdCancel(FXObject* sender, FXSelector sel, void* ptr) {

    // Stop Modal
    getApp()->stopModal(this,FALSE);
    return 1;
}


long
GNEVariableSpeedSignalDialog::onCmdReset(FXObject*, FXSelector, void*) {
    return 1;
}

/****************************************************************************/
