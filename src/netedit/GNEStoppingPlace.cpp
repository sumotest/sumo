/****************************************************************************/
/// @file    GNEStoppingPlace.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Dec 2015
/// @version $Id: $
///
/// A abstract class to define common parameters of lane area in which vehicles can halt (GNE version)
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

#include "GNEStoppingPlace.h"
#include "GNELane.h"
#include "GNEEdge.h"
#include "GNEJunction.h"
#include "GNEUndoList.h"
#include "GNENet.h"
#include "GNEChange_Attribute.h"
#include "GNEViewNet.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif


// ===========================================================================
// member method definitions
// ===========================================================================

GNEStoppingPlace::GNEStoppingPlace(const std::string& id, GNELane& lane, SUMOReal fromPos, SUMOReal toPos, SumoXMLTag tag) :
	myLane(lane),
	myFromPos(fromPos),
	myToPos(toPos),
	GUIGlObject(GLO_TRIGGER, id),
    GNEAttributeCarrier(tag) {
}


GNEStoppingPlace::~GNEStoppingPlace() {
}


GNELane&
GNEStoppingPlace::getLane() const {
	return myLane;
}


SUMOReal 
GNEStoppingPlace::getFromPosition() const {
	return myFromPos;
}

		
SUMOReal 
GNEStoppingPlace::getToPosition() const {
	return myToPos;
}


void 
GNEStoppingPlace::setFromPosition(SUMOReal fromPos) {
    if(fromPos < 0)
        throw InvalidArgument("From position '" + toString(fromPos) + "' not allowed. Must be greather than 0");
    else if(fromPos >= myToPos)
        throw InvalidArgument("From position '" + toString(fromPos) + "' not allowed. Must be smaller than than ToPos '" + toString(myToPos) + "'");
    else if ( (myToPos - fromPos) < 1)
        throw InvalidArgument("From position '" + toString(fromPos) + "' not allowed. Lenght of StoppingPlace must be equal or greather than 1");
    else
        myFromPos = fromPos;
}


void 
GNEStoppingPlace::setToPosition(SUMOReal toPos) {
    if(toPos > myLane.getLength())
        throw InvalidArgument("To position '" + toString(toPos) + "' not allowed. Must be smaller than lane length");
    else if(myFromPos >= toPos)
        throw InvalidArgument("To position '" + toString(toPos) + "' not allowed. Must be smaller than than ToPos '" + toString(myToPos) + "'");
    else if ( (myFromPos - toPos) < 1)
        throw InvalidArgument("To position '" + toString(toPos) + "' not allowed. Lenght of StoppingPlace must be equal or greather than 1");
    else
        myToPos = toPos;
}


const std::string& 
GNEStoppingPlace::getParentName() const {
    return myLane.getMicrosimID();
}


/****************************************************************************/
