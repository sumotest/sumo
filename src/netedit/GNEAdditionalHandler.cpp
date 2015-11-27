/****************************************************************************/
/// @file    GNEAdditionalHandler
/// @author  Pablo Alvarez Lopez
/// @date    Nov 2015
/// @version $Id: $
///
/// Builds trigger objects for netEdit
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

#include "GNEAdditionalHandler.h"
#include "GNEBusStop.h"
#include "GNEChargingStation.h"
#include "GNEEdge.h"
#include "GNELane.h"
#include "GNEJunction.h"
#include "GNENet.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif

// ===========================================================================
// static member definitions
// ===========================================================================



// ===========================================================================
// member method definitions
// ===========================================================================


GNEAdditionalHandler::GNEAdditionalHandler(const std::string& file, GNENet *net) : SUMOSAXHandler(file), myNet(net)
{

}


GNEAdditionalHandler::~GNEAdditionalHandler()
{

}


void 
GNEAdditionalHandler::myStartElement(int element,
                        const SUMOSAXAttributes& attrs) {
	try {
		switch (element) {
			case SUMO_TAG_BUS_STOP:
				parseAndBuildBusStop(myNet, attrs);
				break;
			case SUMO_TAG_CHRG_STN:
				parseAndBuildChargingStation(myNet, attrs);
				break;
			default:
				break;
		}
	} catch (InvalidArgument& e) {
		WRITE_ERROR(e.what());
	}
}

/****************************************************************************/