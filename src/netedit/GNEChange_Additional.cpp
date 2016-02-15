/****************************************************************************/
/// @file    GNEChange_Additional.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Nov 2015
/// @version $Id$
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

#include "GNEChange_Additional.h"
#include "GNENet.h"
#include "GNEAdditional.h"
#include "GNEViewNet.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif


// ===========================================================================
// FOX-declarations
// ===========================================================================
FXIMPLEMENT_ABSTRACT(GNEChange_Additional, GNEChange, NULL, 0)

// ===========================================================================
// member method definitions
// ===========================================================================

GNEChange_Additional::GNEChange_Additional(GNENet* net, GNEAdditional* additional, bool forward) :
    GNEChange(net, forward),
    myAdditional(additional),
    myAdditionalSet(NULL) {
    assert(myNet);
    myAdditional->incRef("GNEChange_Additional");
}


GNEChange_Additional::GNEChange_Additional(GNENet* net, GNEAdditionalSet* additionalSet, bool forward) :
    GNEChange(net, forward),
    myAdditional(NULL),
    myAdditionalSet(additionalSet) {
    assert(myNet);
    myAdditionalSet->incRef("GNEChange_Additional");
}


GNEChange_Additional::~GNEChange_Additional() {
    if(myAdditional) {
        assert(myAdditional);
        myAdditional->decRef("GNEChange_Additional");
        if (myAdditional->unreferenced()) {
            delete myAdditional;
        }
    } else {
        assert(myAdditionalSet);
        myAdditionalSet->decRef("GNEChange_Additional");
        if (myAdditionalSet->unreferenced()) {
            delete myAdditionalSet;
        }
    }
}


void GNEChange_Additional::undo() {
    if(myAdditional) {
        if (myForward) {
            myNet->deleteAdditional(myAdditional);
            myAdditional->getViewNet()->removeAdditionalGLVisualisation(myAdditional);
            myAdditional->getViewNet()->update();
        } else {
            myNet->insertAdditional(myAdditional);
            myAdditional->getViewNet()->addAdditionalGLVisualisation(myAdditional);
            myAdditional->getViewNet()->update();
        }
    } else {
        if (myForward) {
            myNet->deleteAdditionalSet(myAdditionalSet);
            myAdditionalSet->getViewNet()->removeAdditionalGLVisualisation(myAdditionalSet);
            myAdditionalSet->getViewNet()->update();
        } else {
            myNet->insertAdditionalSet(myAdditionalSet);
            myAdditionalSet->getViewNet()->addAdditionalGLVisualisation(myAdditionalSet);
            myAdditionalSet->getViewNet()->update();
        }
    }
}


void GNEChange_Additional::redo() {
    if(myAdditional) {
        if (myForward) {
            myNet->insertAdditional(myAdditional);
            myAdditional->getViewNet()->addAdditionalGLVisualisation(myAdditional);
            myAdditional->getViewNet()->update();
        } else {
            myNet->deleteAdditional(myAdditional);
            myAdditional->getViewNet()->removeAdditionalGLVisualisation(myAdditional);
            myAdditional->getViewNet()->update();
        }
    } else {
        if (myForward) {
            myNet->insertAdditionalSet(myAdditionalSet);
            myAdditionalSet->getViewNet()->addAdditionalGLVisualisation(myAdditionalSet);
            myAdditionalSet->getViewNet()->update();
        } else {
            myNet->deleteAdditionalSet(myAdditionalSet);
            myAdditionalSet->getViewNet()->removeAdditionalGLVisualisation(myAdditionalSet);
            myAdditionalSet->getViewNet()->update();
        }
    }
}


FXString GNEChange_Additional::undoName() const {
    if(myAdditional) {    
    if (myForward) {
            return ("Undo create additional");
        } else {
            return ("Undo delete additional");
        }
    } else {
        if (myForward) {
            return ("Undo create additionalSet");
        } else {
            return ("Undo delete additionalSet");
        }
    }
}


FXString GNEChange_Additional::redoName() const {
    if(myAdditional) {  
        if (myForward) {
            return ("Redo create additional");
        } else {
            return ("Redo delete additional");
        }
    } else {
        if (myForward) {
            return ("Redo create additionalSet");
        } else {
            return ("Redo delete additionalSet");
        }
    }
}