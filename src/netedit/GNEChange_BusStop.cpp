/****************************************************************************/
/// @file    GNEChange_BusStop.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Nov 2015
/// @version $Id: $
///
/// A network change in which a busStop is created or deleted
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

#include "GNEChange_BusStop.h"
#include "GNENet.h"
#include "GNEBusStop.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif


// ===========================================================================
// FOX-declarations
// ===========================================================================
FXIMPLEMENT_ABSTRACT(GNEChange_BusStop, GNEChange, NULL, 0)

// ===========================================================================
// member method definitions
// ===========================================================================


// Constructor for creating an BusStop
GNEChange_BusStop::GNEChange_BusStop(GNENet* net, GNEBusStop* busStop, bool forward):
    GNEChange(net, forward),
    myBusStop(busStop) {
    assert(myNet);
    busStop->incRef("GNEChange_BusStop");
}


GNEChange_BusStop::~GNEChange_BusStop() {
    assert(myBusStop);
    myBusStop->decRef("GNEChange_BusStop");
    if (myBusStop->unreferenced()) {
        delete myBusStop;
    }
}


void GNEChange_BusStop::undo() {
    if (myForward) {
        myNet->removeBusStop(myBusStop);
    } else {
        myNet->addBusStop(myBusStop);
    }
}


void GNEChange_BusStop::redo() {
    if (myForward) {
        myNet->addBusStop(myBusStop);
    } else {
        myNet->removeBusStop(myBusStop);
    }
}


FXString GNEChange_BusStop::undoName() const {
    if (myForward) {
        return ("Undo create BusStop");
    } else {
        return ("Undo delete BusStop");
    }
}


FXString GNEChange_BusStop::redoName() const {
    if (myForward) {
        return ("Redo create BusStop");
    } else {
        return ("Redo delete BusStop");
    }
}