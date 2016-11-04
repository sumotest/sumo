/****************************************************************************/
/// @file    GUITriggerBuilder.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    Mon, 26.04.2004
/// @version $Id$
///
// Builds trigger objects for guisim
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

#include <string>
#include <fstream>
#include <guisim/GUILaneSpeedTrigger.h>
#include <guisim/GUINet.h>
#include <guisim/GUITriggeredRerouter.h>
#include <guisim/GUIBusStop.h>
#include <guisim/GUIContainerStop.h>
#include <guisim/GUIParkingArea.h>
#include <guisim/GUICalibrator.h>
#include <guisim/GUIChargingStation.h>
#include "GUITriggerBuilder.h"


#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
GUITriggerBuilder::GUITriggerBuilder() {}


GUITriggerBuilder::~GUITriggerBuilder() {}


MSLaneSpeedTrigger*
GUITriggerBuilder::buildLaneSpeedTrigger(MSNet& net,
        const std::string& id, const std::vector<MSLane*>& destLanes,
        const std::string& file) {
    GUILaneSpeedTrigger* lst = new GUILaneSpeedTrigger(id, destLanes, file);
    static_cast<GUINet&>(net).getVisualisationSpeedUp().addAdditionalGLObject(lst);
    return lst;
}


MSTriggeredRerouter*
GUITriggerBuilder::buildRerouter(MSNet& net, const std::string& id,
                                 MSEdgeVector& edges,
                                 SUMOReal prob, const std::string& file, bool off) {
    GUITriggeredRerouter* rr = new GUITriggeredRerouter(id, edges, prob, file, off,
            dynamic_cast<GUINet&>(net).getVisualisationSpeedUp());
    return rr;
}

void
GUITriggerBuilder::buildBusStop(MSNet& net, const std::string& id,
                                const std::vector<std::string>& lines,
                                MSLane* lane,
                                SUMOReal frompos, SUMOReal topos) {

    GUIBusStop* stop = new GUIBusStop(id, lines, *lane, frompos, topos);
    if (!net.addBusStop(stop)) {
        delete stop;
        throw InvalidArgument("Could not build bus stop '" + id + "'; probably declared twice.");
    }
    static_cast<GUINet&>(net).getVisualisationSpeedUp().addAdditionalGLObject(stop);
}

void
GUITriggerBuilder::buildContainerStop(MSNet& net, const std::string& id,
                                      const std::vector<std::string>& lines,
                                      MSLane* lane,
                                      SUMOReal frompos, SUMOReal topos) {
    GUIContainerStop* stop = new GUIContainerStop(id, lines, *lane, frompos, topos);
    if (!net.addContainerStop(stop)) {
        delete stop;
        throw InvalidArgument("Could not build container stop '" + id + "'; probably declared twice.");
    }
    static_cast<GUINet&>(net).getVisualisationSpeedUp().addAdditionalGLObject(stop);
}

void
GUITriggerBuilder::beginParkingArea(MSNet& net, const std::string& id,
                                    const std::vector<std::string>& lines,
                                    MSLane* lane,
                                    SUMOReal frompos, SUMOReal topos, 
                                    unsigned int capacity,
                                    SUMOReal width, SUMOReal length, SUMOReal angle) {
    // Close previous parking area if there are not lots inside
    if (myParkingArea != 0) endParkingArea();
    
    GUIParkingArea* stop = new GUIParkingArea(id, lines, *lane, frompos, topos, capacity, width, length, angle);
    if (!net.addParkingArea(stop)) {
        delete stop;
        throw InvalidArgument("Could not build parking area '" + id + "'; probably declared twice.");
    } else
        myParkingArea = stop;
    static_cast<GUINet&>(net).getVisualisationSpeedUp().addAdditionalGLObject(stop);
}

void
GUITriggerBuilder::addLotEntry(SUMOReal x, SUMOReal y, SUMOReal z,
                              SUMOReal width, SUMOReal length, SUMOReal angle) {                          
    if (myParkingArea != 0) {
        if (!myParkingArea->addLotEntry(x, y, z, width, length, angle))
            throw InvalidArgument("Could not add lot entry to parking area '" + myParkingArea->getID() + "'; probably declared twice.");
    } else
        throw InvalidArgument("Could not add lot entry outside a parking area.");
}


void
GUITriggerBuilder::endParkingArea() {
    if (myParkingArea != 0) {
        myParkingArea = 0;
    } else
        throw InvalidArgument("Could not end a parking area that is not opened.");
}


void
GUITriggerBuilder::buildChargingStation(MSNet& net, const std::string& id,
                                        const std::vector<std::string>& lines,
                                        MSLane* lane,
                                        SUMOReal frompos, SUMOReal topos, SUMOReal chrgpower, SUMOReal efficiency, SUMOReal chargeInTransit, SUMOReal chargeDelay) {
    GUIChargingStation* chrg = new GUIChargingStation(id, lines, *lane, frompos, topos, chrgpower, efficiency, chargeInTransit, chargeDelay);

    if (!net.addChargingStation(chrg)) {
        delete chrg;
        throw InvalidArgument("Could not build charging station '" + id + "'; probably declared twice.");
    }

    static_cast<GUINet&>(net).getVisualisationSpeedUp().addAdditionalGLObject(chrg);
}

MSCalibrator*
GUITriggerBuilder::buildCalibrator(MSNet& net, const std::string& id,
                                   MSEdge* edge, SUMOReal pos,
                                   const std::string& file,
                                   const std::string& outfile,
                                   const SUMOTime freq,
                                   const MSRouteProbe* probe) {
    GUICalibrator* cali = new GUICalibrator(id, edge, pos, file, outfile, freq, probe);
    static_cast<GUINet&>(net).getVisualisationSpeedUp().addAdditionalGLObject(cali);
    return cali;
}


/****************************************************************************/

