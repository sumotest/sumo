/****************************************************************************/
/// @file    GNEChange_ChargingStation.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Nov 2015
/// @version $Id: $
///
/// A network change in which a chargingStation is created or deleted
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

#include "GNEChange_ChargingStation.h"
#include "GNENet.h"
#include "GNEChargingStation.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif


// ===========================================================================
// FOX-declarations
// ===========================================================================
FXIMPLEMENT_ABSTRACT(GNEChange_ChargingStation, GNEChange, NULL, 0)

// ===========================================================================
// member method definitions
// ===========================================================================


// Constructor for creating an ChargingStation
GNEChange_ChargingStation::GNEChange_ChargingStation(GNENet* net, GNEChargingStation* chargingStation, bool forward):
    GNEChange(net, forward),
    myChargingStation(chargingStation) {
    assert(myNet);
    chargingStation->incRef("GNEChange_ChargingStation");
}


GNEChange_ChargingStation::~GNEChange_ChargingStation() {
    assert(myChargingStation);
    myChargingStation->decRef("GNEChange_ChargingStation");
    if (myChargingStation->unreferenced()) {
        delete myChargingStation;
    }
}


void GNEChange_ChargingStation::undo() {
    if (myForward) {
        myNet->removeChargingStation(myChargingStation);
    } else {
        myNet->addChargingStation(myChargingStation);
    }
}


void GNEChange_ChargingStation::redo() {
    if (myForward) {
        myNet->addChargingStation(myChargingStation);
    } else {
        myNet->removeChargingStation(myChargingStation);
    }
}


FXString GNEChange_ChargingStation::undoName() const {
    if (myForward) {
        return ("Undo create ChargingStation");
    } else {
        return ("Undo delete ChargingStation");
    }
}


FXString GNEChange_ChargingStation::redoName() const {
    if (myForward) {
        return ("Redo create ChargingStation");
    } else {
        return ("Redo delete ChargingStation");
    }
}