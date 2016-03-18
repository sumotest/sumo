/****************************************************************************/
/// @file    GNEDetectorE1.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Nov 2015
/// @version $Id: GNEDetectorE1.cpp 19861 2016-02-01 09:08:47Z palcraft $
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

#include "GNEDetector.h"
#include "GNELane.h"
#include "GNEChange_Attribute.h"
#include "GNEUndoList.h"
#include "GNENet.h"
#include "GNEViewNet.h"


#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif


// ===========================================================================
// member method definitions
// ===========================================================================

GNEDetector::GNEDetector(const std::string& id, GNEViewNet* viewNet, SumoXMLTag tag, GNELane* lane, SUMOReal posOverLane, int freq, const std::string &filename, bool blocked, SumoXMLTag parentTag, GNEAdditionalSet *parent) :
    GNEAdditional(id, viewNet, Position(), tag, blocked, lane, parentTag, parent),
    myPosOverLane(posOverLane),
    myFreq(freq),
    myFilename(filename) {
}


GNEDetector::~GNEDetector() {
}


void 
GNEDetector::moveAdditional(SUMOReal posx, SUMOReal posy, GNEUndoList *undoList) {
    // if item isn't blocked
    if(myBlocked == false) {
        // Move to Right if posx is positive, to left if posx is negative
        if( ((posx > 0) && ((myPosOverLane + posx) < myLane->getLaneShapeLenght())) || ((posx < 0) && ((myPosOverLane + posx) > 0)) ) {
            // change attribute
            undoList->p_add(new GNEChange_Attribute(this, SUMO_ATTR_POSITION, toString(myPosOverLane + posx)));
        }
    }
}


SUMOReal 
GNEDetector::getPositionOverLane() const {
    return myPosOverLane;
}


int 
GNEDetector::getFrequency() const {
    return myFreq;
}


std::string 
GNEDetector::getFilename() const {
    return myFilename;
}


void 
GNEDetector::setPosition(SUMOReal pos) {
    if(pos < 0)
        throw InvalidArgument("Position '" + toString(pos) + "' not allowed. Must be greather than 0");
    else if(pos > myLane->getLaneShapeLenght())
        throw InvalidArgument("Position '" + toString(pos) + "' not allowed. Must be smaller than lane length");
    else
        myPosOverLane = pos;
}


void 
GNEDetector::setFrequency(int freq) {
    if (freq >= 0)
        myFreq = freq;
    else
        throw InvalidArgument("Frequency '" + toString(freq) + "' not allowed. Must be greather than 0");
}


void 
GNEDetector::setFilename(std::string filename) {
    myFilename = filename;
}

/****************************************************************************/