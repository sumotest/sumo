/****************************************************************************/
/// @file    GNEChargingStation.h
/// @author  Pablo Alvarez Lopez
/// @date    Nov 2015
/// @version $Id: $
///
/// A class for visualizing chargingStation geometry (adapted from GUILaneWrapper)
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
#ifndef GNEChargingStation_h
#define GNEChargingStation_h

// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <vector>
#include <string>
#include <utils/gui/globjects/GLIncludes.h>
#include <utils/geom/Position.h>
#include <utils/geom/Boundary.h>
#include <utils/gui/globjects/GUIGlObject.h>
#include <utils/gui/globjects/GUIGlObjectStorage.h>
#include "GNEAttributeCarrier.h"


// ===========================================================================
// class declarations
// ===========================================================================

class GUIGLObjectPopupMenu;
class PositionVector;
class GNELane;
class GNENet;

// ===========================================================================
// class definitions
// ===========================================================================

class GNEChargingStation : public GNEAttributeCarrier
{
public:

	/** @brief Constructor.
     * @param[in] nbe The represented edge
     * @param[in] net The net to inform about gui updates
     */
    GNEChargingStation(GNELane& lane);

	~GNEChargingStation();

	GNELane &getLane() const;

	SUMOReal getBeginLanePosition() const;
		
	SUMOReal getEndLanePosition() const;

private:

	GNELane &myParentLane;

	SUMOReal begin;

	SUMOReal end;

};


#endif
