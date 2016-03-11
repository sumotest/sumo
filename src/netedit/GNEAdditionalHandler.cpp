/****************************************************************************/
/// @file    GNEAdditionalHandler
/// @author  Pablo Alvarez Lopez
/// @date    Nov 2015
/// @version $Id$
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

#include <utils/geom/GeomConvHelper.h>

#include "GNEAdditionalHandler.h"
#include "GNEUndoList.h"
#include "GNEEdge.h"
#include "GNELane.h"
#include "GNEJunction.h"
#include "GNENet.h"
#include "GNEViewNet.h"
#include "GNEChange_Additional.h"
#include "GNEBusStop.h"
#include "GNEChargingStation.h"
#include "GNEDetectorE1.h"
#include "GNEDetectorE2.h"
#include "GNEDetectorE3.h"
#include "GNEDetectorE3EntryExit.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif

// ===========================================================================
// member method definitions
// ===========================================================================


GNEAdditionalHandler::GNEAdditionalHandler(const std::string& file, GNEViewNet* viewNet) : 
    SUMOSAXHandler(file), 
    myViewNet(viewNet) {
}


GNEAdditionalHandler::~GNEAdditionalHandler() {}


void 
GNEAdditionalHandler::myStartElement(int element, const SUMOSAXAttributes& attrs) {
    try {
        switch (element) {
            case SUMO_TAG_BUS_STOP:
                parseAndBuildBusStop(attrs);
                break;
            case SUMO_TAG_CHARGING_STATION:
                parseAndBuildChargingStation(attrs);
                break;
            // Rest of additional elements
            default:
                break;
        }
    } catch (InvalidArgument& e) {
        WRITE_ERROR(e.what());
    }
}


void 
GNEAdditionalHandler::parseAndBuildVaporizer(const SUMOSAXAttributes& attrs)
{
    std::cout << "Function parseAndBuildVaporizer of class GNEAdditionalHandler not implemented yet";
    /*
    bool ok = true;
    // get the id, throw if not given or empty...
    std::string id = attrs.get<std::string>(SUMO_ATTR_ID, 0, ok);
    if (!ok) {
        return;
    }
    MSEdge* e = MSEdge::dictionary(id);
    if (e == 0) {
        WRITE_ERROR("Unknown edge ('" + id + "') referenced in a vaporizer.");
        return;
    }
    SUMOTime begin = attrs.getSUMOTimeReporting(SUMO_ATTR_BEGIN, 0, ok);
    SUMOTime end = attrs.getSUMOTimeReporting(SUMO_ATTR_END, 0, ok);
    if (!ok) {
        return;
    }
    if (begin < 0) {
        WRITE_ERROR("A vaporization begin time is negative (edge id='" + id + "').");
        return;
    }
    if (begin >= end) {
        WRITE_ERROR("A vaporization ends before it starts (edge id='" + id + "').");
        return;
    }
    if (end >= string2time(OptionsCont::getOptions().getString("begin"))) {
        Command* cb = new WrappingCommand< MSEdge >(e, &MSEdge::incVaporization);
        MSNet::getInstance()->getBeginOfTimestepEvents()->addEvent(cb, begin, MSEventControl::ADAPT_AFTER_EXECUTION);
        Command* ce = new WrappingCommand< MSEdge >(e, &MSEdge::decVaporization);
        MSNet::getInstance()->getBeginOfTimestepEvents()->addEvent(ce, end, MSEventControl::ADAPT_AFTER_EXECUTION);
    }
*/
}


void 
GNEAdditionalHandler::parseAndBuildLaneSpeedTrigger(const SUMOSAXAttributes& attrs, const std::string& base)
{
    std::cout << "Function buildLaneSpeedTrigger of class GNEAdditionalHandler not implemented yet";
    /*
    // get the id, throw if not given or empty...
    bool ok = true;
    // get the id, throw if not given or empty...
    std::string id = attrs.get<std::string>(SUMO_ATTR_ID, 0, ok);
    if (!ok) {
        return;
    }
    // get the file name to read further definitions from
    std::string file = getFileName(attrs, base, true);
    std::string objectid = attrs.get<std::string>(SUMO_ATTR_LANES, id.c_str(), ok);
    if (!ok) {
        throw InvalidArgument("The lanes to use within MSLaneSpeedTrigger '" + id + "' are not known.");
    }
    std::vector<MSLane*> lanes;
    std::vector<std::string> laneIDs;
    SUMOSAXAttributes::parseStringVector(objectid, laneIDs);
    for (std::vector<std::string>::iterator i = laneIDs.begin(); i != laneIDs.end(); ++i) {
        MSLane* lane = MSLane::dictionary(*i);
        if (lane == 0) {
            throw InvalidArgument("The lane to use within MSLaneSpeedTrigger '" + id + "' is not known.");
        }
        lanes.push_back(lane);
    }
    if (lanes.size() == 0) {
        throw InvalidArgument("No lane defined for MSLaneSpeedTrigger '" + id + "'.");
    }
    try {
        MSLaneSpeedTrigger* trigger = buildLaneSpeedTrigger(myViewNet->getNet(), id, lanes, file);
        if (file == "") {
            trigger->registerParent(SUMO_TAG_VSS, myHandler);
        }
    } catch (ProcessError& e) {
        throw InvalidArgument(e.what());
    }
*/
}


void 
GNEAdditionalHandler::parseAndBuildRerouter(const SUMOSAXAttributes& attrs, const std::string& base)
{
    std::cout << "Function buildRerouter of class GNEAdditionalHandler not implemented yet";
    /*
    bool ok = true;
    // get the id, throw if not given or empty...
    std::string id = attrs.get<std::string>(SUMO_ATTR_ID, 0, ok);
    if (!ok) {
        throw ProcessError();
    }
    // get the file name to read further definitions from
    std::string file = getFileName(attrs, base, true);
    std::string objectid = attrs.get<std::string>(SUMO_ATTR_EDGES, id.c_str(), ok);
    if (!ok) {
        throw InvalidArgument("The edge to use within MSTriggeredRerouter '" + id + "' is not known.");
    }
    MSEdgeVector edges;
    std::vector<std::string> edgeIDs;
    SUMOSAXAttributes::parseStringVector(objectid, edgeIDs);
    for (std::vector<std::string>::iterator i = edgeIDs.begin(); i != edgeIDs.end(); ++i) {
        MSEdge* edge = MSEdge::dictionary(*i);
        if (edge == 0) {
            throw InvalidArgument("The edge to use within MSTriggeredRerouter '" + id + "' is not known.");
        }
        edges.push_back(edge);
    }
    if (edges.size() == 0) {
        throw InvalidArgument("No edges found for MSTriggeredRerouter '" + id + "'.");
    }
    SUMOReal prob = attrs.getOpt<SUMOReal>(SUMO_ATTR_PROB, id.c_str(), ok, 1);
    bool off = attrs.getOpt<bool>(SUMO_ATTR_OFF, id.c_str(), ok, false);
    if (!ok) {
        throw InvalidArgument("Could not parse MSTriggeredRerouter '" + id + "'.");
    }
    MSTriggeredRerouter* trigger = buildRerouter(myViewNet->getNet(), id, edges, prob, file, off);
    // read in the trigger description
    if (file == "") {
        trigger->registerParent(SUMO_TAG_REROUTER, myHandler);
    } else if (!XMLSubSys::runParser(*trigger, file)) {
        throw ProcessError();
    }
*/
}


void 
GNEAdditionalHandler::parseAndBuildBusStop(const SUMOSAXAttributes& attrs)
{
    bool ok = true;
    // get the id, throw if not given or empty...
    std::string id = attrs.get<std::string>(SUMO_ATTR_ID, 0, ok);
    if (!ok) {
        throw ProcessError();
    } 
    // get the lane
    GNELane* lane = getLane(attrs, "busStop", id);
    // get the positions
    SUMOReal frompos = attrs.getOpt<SUMOReal>(SUMO_ATTR_STARTPOS, id.c_str(), ok, 0);
    SUMOReal topos = attrs.getOpt<SUMOReal>(SUMO_ATTR_ENDPOS, id.c_str(), ok, lane->getLaneShapeLenght());
    const bool friendlyPos = attrs.getOpt<bool>(SUMO_ATTR_FRIENDLY_POS, id.c_str(), ok, false);
    if (!ok || !checkStopPos(frompos, topos, lane->getLaneShapeLenght(), POSITION_EPS, friendlyPos)) {
        throw InvalidArgument("Invalid position for bus stop '" + id + "'.");
    }
    // get the lines
    std::vector<std::string> lines;
    SUMOSAXAttributes::parseStringVector(attrs.getOpt<std::string>(SUMO_ATTR_LINES, id.c_str(), ok, "", false), lines);
    // build the bus stop
    buildBusStop(myViewNet, id, lane, frompos, topos, lines, false);
}


void 
GNEAdditionalHandler::parseAndBuildChargingStation(const SUMOSAXAttributes& attrs)
{
    bool ok = true;
    // get the id, throw if not given or empty...
    std::string id = attrs.get<std::string>(SUMO_ATTR_ID, 0, ok);

    if (!ok) {
        throw ProcessError();
    }   
    // get the lane
    GNELane* lane = getLane(attrs, "chargingStation", id);
    // get the positions
    SUMOReal frompos = attrs.getOpt<SUMOReal>(SUMO_ATTR_STARTPOS, id.c_str(), ok, 0);
    SUMOReal topos = attrs.getOpt<SUMOReal>(SUMO_ATTR_ENDPOS, id.c_str(), ok, lane->getLaneShapeLenght());
    SUMOReal chrgpower = attrs.getOpt<SUMOReal>(SUMO_ATTR_CHARGINGPOWER, id.c_str(), ok, 0);
    SUMOReal efficiency = attrs.getOpt<SUMOReal>(SUMO_ATTR_EFFICIENCY, id.c_str(), ok, 0);
    bool chargeInTransit = attrs.getOpt<bool>(SUMO_ATTR_CHARGEINTRANSIT, id.c_str(), ok, 0);
    SUMOReal chargeDelay = attrs.getOpt<SUMOReal>(SUMO_ATTR_CHARGEDELAY, id.c_str(), ok, 0);
    const bool friendlyPos = attrs.getOpt<bool>(SUMO_ATTR_FRIENDLY_POS, id.c_str(), ok, false);
    if (!ok || !checkStopPos(frompos, topos, lane->getLaneShapeLenght(), POSITION_EPS, friendlyPos)) {
        throw InvalidArgument("Invalid position for Charging Station '" + id + "'.");
    }

    // get the lines
    std::vector<std::string> lines;
    SUMOSAXAttributes::parseStringVector(attrs.getOpt<std::string>(SUMO_ATTR_LINES, id.c_str(), ok, "", false), lines);

    // build the Charging Station
    buildChargingStation(myViewNet, id, lane, frompos, topos, chrgpower, efficiency, chargeInTransit, chargeDelay, false);
}


void 
GNEAdditionalHandler::parseAndBuildCalibrator(const SUMOSAXAttributes& attrs, const std::string& base)
{
    std::cout << "Function buildCalibrator of class GNEAdditionalHandler not implemented yet";
    /*
    bool ok = true;
    // get the id, throw if not given or empty...
    std::string id = attrs.get<std::string>(SUMO_ATTR_ID, 0, ok);
    if (!ok) {
        throw ProcessError();
    }
    // get the file name to read further definitions from
    MSLane* lane = getLane(attrs, "calibrator", id);
    const SUMOReal pos = getPosition(attrs, lane, "calibrator", id);
    const SUMOTime freq = attrs.getOptSUMOTimeReporting(SUMO_ATTR_FREQUENCY, id.c_str(), ok, DELTA_T); // !!! no error handling
    std::string file = getFileName(attrs, base, true);
    std::string outfile = attrs.getOpt<std::string>(SUMO_ATTR_OUTPUT, id.c_str(), ok, "");
    std::string routeProbe = attrs.getOpt<std::string>(SUMO_ATTR_ROUTEPROBE, id.c_str(), ok, "");
    MSRouteProbe* probe = 0;
    if (routeProbe != "") {
        probe = dynamic_cast<MSRouteProbe*>(net.getDetectorControl().getTypedDetectors(SUMO_TAG_ROUTEPROBE).get(routeProbe));
    }
    if (MSGlobals::gUseMesoSim) {
#ifdef HAVE_INTERNAL
        METriggeredCalibrator* trigger = buildMECalibrator(myViewNet->getNet(), id, &lane->getEdge(), pos, file, outfile, freq, probe);
        if (file == "") {
            trigger->registerParent(SUMO_TAG_CALIBRATOR, myHandler);
        }
#endif
    } else {
        MSCalibrator* trigger = buildCalibrator(myViewNet->getNet(), id, &lane->getEdge(), pos, file, outfile, freq, probe);
        if (file == "") {
            trigger->registerParent(SUMO_TAG_CALIBRATOR, myHandler);
        }
    }*/
}


void 
GNEAdditionalHandler::parseAndBuildDetectorE1(const SUMOSAXAttributes& attrs, const std::string& base) {

}


void 
GNEAdditionalHandler::parseAndBuildDetectorE2(const SUMOSAXAttributes& attrs, const std::string& base) {

}


void 
GNEAdditionalHandler::parseAndBuildDetectorE3(const SUMOSAXAttributes& attrs, const std::string& base) {

}


bool 
GNEAdditionalHandler::buildAdditional(GNEViewNet *viewNet, SumoXMLTag tag, std::map<SumoXMLAttr, std::string> values) {
    // Extract common attributes
    std::string id = values[SUMO_ATTR_ID];
    GNELane *lane = viewNet->getNet()->getLane(values[SUMO_ATTR_LANE]);
    bool blocked = GNEAttributeCarrier::parse<SUMOReal>(values[GNE_ATTR_BLOCK_MOVEMENT]);
    // create additional depending of the tag
    switch(tag) {
        case SUMO_TAG_BUS_STOP: {
            // get own attributes of busStop
            SUMOReal fromPos = GNEAttributeCarrier::parse<SUMOReal>(values[SUMO_ATTR_STARTPOS]);
            SUMOReal toPos = GNEAttributeCarrier::parse<SUMOReal>(values[SUMO_ATTR_ENDPOS]);
            std::vector<std::string> lines; /** FALTA **/
            // Build busStop
            if(lane)
                return buildBusStop(viewNet, id, lane, fromPos, toPos, lines, blocked);
            else
                return false;
        }
        case SUMO_TAG_CHARGING_STATION: {
            // get own attributes of chargingStation
            SUMOReal fromPos = GNEAttributeCarrier::parse<SUMOReal>(values[SUMO_ATTR_STARTPOS]);
            SUMOReal toPos = GNEAttributeCarrier::parse<SUMOReal>(values[SUMO_ATTR_ENDPOS]);
            SUMOReal chargingPower = GNEAttributeCarrier::parse<SUMOReal>(values[SUMO_ATTR_CHARGINGPOWER]);
            SUMOReal efficiency = GNEAttributeCarrier::parse<SUMOReal>(values[SUMO_ATTR_EFFICIENCY]);
            bool chargeInTransit = GNEAttributeCarrier::parse<bool>(values[SUMO_ATTR_CHARGEINTRANSIT]);
            int chargeDelay = GNEAttributeCarrier::parse<int>(values[SUMO_ATTR_CHARGEDELAY]);
            // Build chargingStation
            if(lane)
                return buildChargingStation(viewNet, id, lane, fromPos, toPos, chargingPower, efficiency, chargeInTransit, chargeDelay, blocked);
            else
                return false;
        }
        case SUMO_TAG_E1DETECTOR: {
            // get own attributes of detector E1
            SUMOReal pos = GNEAttributeCarrier::parse<SUMOReal>(values[SUMO_ATTR_POSITION]);
            int freq = GNEAttributeCarrier::parse<int>(values[SUMO_ATTR_FREQUENCY]);
            std::string filename = values[SUMO_ATTR_FILE];
            bool splitByType = GNEAttributeCarrier::parse<bool>(values[SUMO_ATTR_SPLIT_VTYPE]);
            // Build detector E1
            if(lane)
                return buildDetectorE1(viewNet, id, lane, pos, freq, filename, splitByType, blocked);
            else
                return false;
        }
        case SUMO_TAG_E2DETECTOR: {
            // get own attributes of detector E2
            SUMOReal pos = GNEAttributeCarrier::parse<SUMOReal>(values[SUMO_ATTR_POSITION]);
            int freq = GNEAttributeCarrier::parse<int>(values[SUMO_ATTR_FREQUENCY]);
            SUMOReal lenght = GNEAttributeCarrier::parse<SUMOReal>(values[SUMO_ATTR_LENGTH]);
            std::string filename = values[SUMO_ATTR_FILE];
            bool cont = GNEAttributeCarrier::parse<bool>(values[SUMO_ATTR_CONT]);
            int timeThreshold = GNEAttributeCarrier::parse<int>(values[SUMO_ATTR_HALTING_TIME_THRESHOLD]);
            SUMOReal speedThreshold = GNEAttributeCarrier::parse<SUMOReal>(values[SUMO_ATTR_HALTING_SPEED_THRESHOLD]);
            SUMOReal jamThreshold = GNEAttributeCarrier::parse<SUMOReal>(values[SUMO_ATTR_JAM_DIST_THRESHOLD]); 
            // Build detector E2
            if(lane)
                return buildDetectorE2(viewNet, id, lane, pos, lenght, freq, filename, cont, timeThreshold, speedThreshold, jamThreshold, blocked);
            else
                return false;
        }
        case SUMO_TAG_E3DETECTOR: {
            // get own attributes of detector E3
            bool ok;
            PositionVector pos = GeomConvHelper::parseShapeReporting(values[SUMO_ATTR_POSITION], "user-supplied position", 0, ok, false);
            int freq = GNEAttributeCarrier::parse<int>(values[SUMO_ATTR_FREQUENCY]);
            std::string filename = values[SUMO_ATTR_FILE];
            // Build detector E3
            if(!lane && (pos.size() == 1))
                return buildDetectorE3(viewNet, id, pos[0], freq, filename, blocked);
            else
                return false;
        }
        case SUMO_TAG_DET_ENTRY: {
            // get own attributes of detector Entry
            SUMOReal pos = GNEAttributeCarrier::parse<SUMOReal>(values[SUMO_ATTR_POSITION]); 
            GNEDetectorE3 *detectorParent = dynamic_cast<GNEDetectorE3*>(viewNet->getNet()->getAdditional(SUMO_TAG_E3DETECTOR ,values[GNE_ATTR_PARENT]));
            // Build detector Entry
            if(lane && detectorParent)
                return buildEntryE3(viewNet, id, lane, pos, detectorParent, blocked);
            else
                return false;
        }
        case SUMO_TAG_DET_EXIT: {
            // get own attributes of Detector Exit
            SUMOReal pos = GNEAttributeCarrier::parse<SUMOReal>(values[SUMO_ATTR_POSITION]); 
            GNEDetectorE3 *detectorParent = dynamic_cast<GNEDetectorE3*>(viewNet->getNet()->getAdditional(SUMO_TAG_E3DETECTOR ,values[GNE_ATTR_PARENT]));
            // Build detector Exit
            if(lane && detectorParent)
                return buildExitE3(viewNet, id, lane, pos, detectorParent, blocked);
            else
                return false;
        }
        case SUMO_TAG_VSS: {
            // get own attributes of variable speed signal
            std::vector<GNELane*> destLanes; 
            std::string file = values[SUMO_ATTR_FILE];
            // Build variable speed signal
            /** NOTA: COMPROBAR SI EFECTIVAMENTE SUMO_TAG_VSS es buildLaneSpeedTrigger **/
            if(lane)
                return buildLaneSpeedTrigger(viewNet, id, destLanes, file, blocked);
            else
                return false;
        }
        case SUMO_TAG_CALIBRATOR: {
            // get own attributes of calibrator
            SUMOReal pos = GNEAttributeCarrier::parse<SUMOReal>(values[SUMO_ATTR_POSITION]); 
            std::string file = values[SUMO_ATTR_FILE];
            MSRouteProbe* probe; 
            std::string outfile;
            SUMOTime freq;
            // Build calibrator
            if(lane)
                return buildCalibrator(viewNet, id, lane->getParentEdge(), pos, file, outfile, freq, probe, blocked);
            else
                return false;
        }
        case SUMO_TAG_REROUTER: {
            // get own attributes of rerouter
            std::vector<GNEEdge*> edges;
            SUMOReal prob;
            std::string file;
            bool off;
            // Build rerouter
            if(lane)
                return buildRerouter(viewNet, id, edges, prob, file, off, blocked);
            else
                return false;
        }
        default:
            return false;
    }
}

bool 
GNEAdditionalHandler::buildBusStop(GNEViewNet *viewNet, const std::string& id, GNELane *lane, SUMOReal frompos, SUMOReal topos, const std::vector<std::string>& lines, bool blocked) {
    if (viewNet->getNet()->getAdditional(SUMO_TAG_BUS_STOP, id) == NULL) {
        GNEBusStop* busStop = new GNEBusStop(id, *lane, viewNet, frompos, topos, lines, blocked);
        viewNet->getUndoList()->p_begin("add " + toString(SUMO_TAG_BUS_STOP));
        viewNet->getUndoList()->add(new GNEChange_Additional(viewNet->getNet(), busStop, true), true);
        viewNet->getUndoList()->p_end();
        return true;
    } else {
        throw InvalidArgument("Could not build " + toString(SUMO_TAG_BUS_STOP) + " in netEdit '" + id + "'; probably declared twice.");
        return false;
    }
}


bool 
GNEAdditionalHandler::buildChargingStation(GNEViewNet *viewNet, const std::string& id, GNELane *lane, SUMOReal frompos, SUMOReal topos, SUMOReal chargingPower, SUMOReal efficiency, bool chargeInTransit, SUMOReal chargeDelay, bool blocked) {
    if (viewNet->getNet()->getAdditional(SUMO_TAG_CHARGING_STATION, id) == NULL) {
        GNEChargingStation* chargingStation = new GNEChargingStation(id, *lane, viewNet, frompos, topos, chargingPower, efficiency, chargeInTransit, chargeDelay, blocked);
        viewNet->getUndoList()->p_begin("add " + toString(SUMO_TAG_CHARGING_STATION));
        viewNet->getUndoList()->add(new GNEChange_Additional(viewNet->getNet(), chargingStation, true), true);
        viewNet->getUndoList()->p_end();
        return true;
    } else {
        throw InvalidArgument("Could not build " + toString(SUMO_TAG_CHARGING_STATION) + " in netEdit '" + id + "'; probably declared twice.");
        return false;
    }
}


bool 
GNEAdditionalHandler::buildDetectorE1(GNEViewNet *viewNet, const std::string& id, GNELane *lane, SUMOReal pos, int freq, const std::string &filename, bool splitByType, bool blocked) {
    if (viewNet->getNet()->getAdditional(SUMO_TAG_E1DETECTOR, id) == NULL) {
        GNEDetectorE1 *detectorE1 = new GNEDetectorE1(id, *lane, viewNet, pos, freq, filename, splitByType, blocked);
        viewNet->getUndoList()->p_begin("add " + toString(SUMO_TAG_E1DETECTOR));
        viewNet->getUndoList()->add(new GNEChange_Additional(viewNet->getNet(), detectorE1, true), true);
        viewNet->getUndoList()->p_end();
        return true;
    } else {
        throw InvalidArgument("Could not build " + toString(SUMO_TAG_E1DETECTOR) + " in netEdit '" + id + "'; probably declared twice.");
        return false;
    }
}


bool 
GNEAdditionalHandler::buildDetectorE2(GNEViewNet* viewNet, const std::string& id, GNELane *lane, SUMOReal pos, SUMOReal length, SUMOReal freq, const std::string& filename, 
                                      bool cont, int timeThreshold, SUMOReal speedThreshold, SUMOReal jamThreshold, bool blocked) {
    if (viewNet->getNet()->getAdditional(SUMO_TAG_E2DETECTOR, id) == NULL) {
        GNEDetectorE2 *detectorE2 = new GNEDetectorE2(id, *lane, viewNet, pos, length, freq, filename, cont, timeThreshold, speedThreshold, jamThreshold, blocked);
        viewNet->getUndoList()->p_begin("add " + toString(SUMO_TAG_E2DETECTOR));
        viewNet->getUndoList()->add(new GNEChange_Additional(viewNet->getNet(), detectorE2, true), true);
        viewNet->getUndoList()->p_end();
        return true;
    } else {
        throw InvalidArgument("Could not build " + toString(SUMO_TAG_E2DETECTOR) + " in netEdit '" + id + "'; probably declared twice.");
        return false;
    }
}


bool 
GNEAdditionalHandler::buildDetectorE3(GNEViewNet *viewNet, const std::string& id, Position pos, int freq, const std::string &filename, bool blocked) {
    if (viewNet->getNet()->getAdditional(SUMO_TAG_E3DETECTOR, id) == NULL) {
        GNEDetectorE3 *detectorE3 = new GNEDetectorE3(id, viewNet, pos, freq, filename, blocked);
        viewNet->getUndoList()->p_begin("add " + toString(SUMO_TAG_E3DETECTOR));
        viewNet->getUndoList()->add(new GNEChange_Additional(viewNet->getNet(), detectorE3, true), true);
        viewNet->getUndoList()->p_end();
        return true;
    } else {
        throw InvalidArgument("Could not build " + toString(SUMO_TAG_E3DETECTOR) + " in netEdit '" + id + "'; probably declared twice.");
        return false;
    }
}


bool 
GNEAdditionalHandler::buildEntryE3(GNEViewNet *viewNet, const std::string& id, GNELane *lane, SUMOReal pos, GNEDetectorE3 *detectorParent, bool blocked) {
    if (viewNet->getNet()->getAdditional(SUMO_TAG_DET_ENTRY, id) == NULL) {
        GNEDetectorE3EntryExit *entry = new GNEDetectorE3EntryExit(id, viewNet, SUMO_TAG_DET_ENTRY, *lane, pos, detectorParent, blocked);
        viewNet->getUndoList()->p_begin("add " + toString(SUMO_TAG_DET_ENTRY));
        viewNet->getUndoList()->add(new GNEChange_Additional(viewNet->getNet(), entry, true), true);
        viewNet->getUndoList()->p_end();
        return true;
    } else {
        throw InvalidArgument("Could not build " + toString(SUMO_TAG_DET_ENTRY) + " in netEdit '" + id + "'; probably declared twice.");
        return false;
    }
}


bool 
GNEAdditionalHandler::buildExitE3(GNEViewNet *viewNet, const std::string& id, GNELane *lane, SUMOReal pos, GNEDetectorE3 *detectorParent, bool blocked) {
    if (viewNet->getNet()->getAdditional(SUMO_TAG_DET_EXIT, id) == NULL) {
        GNEDetectorE3EntryExit *exit = new GNEDetectorE3EntryExit(id, viewNet, SUMO_TAG_DET_EXIT, *lane, pos, detectorParent, blocked);
        viewNet->getUndoList()->p_begin("add " + toString(SUMO_TAG_DET_EXIT));
        viewNet->getUndoList()->add(new GNEChange_Additional(viewNet->getNet(), exit, true), true);
        viewNet->getUndoList()->p_end();
        return true;
    } else {
        throw InvalidArgument("Could not build " + toString(SUMO_TAG_DET_EXIT) + " in netEdit '" + id + "'; probably declared twice.");
        return false;
    }
}


bool
GNEAdditionalHandler::buildCalibrator(GNEViewNet *viewNet, const std::string& id, GNEEdge &edge, SUMOReal pos, const std::string& file, const std::string& outfile, const SUMOTime freq, const MSRouteProbe* probe, bool blocked)
{
    std::cout << "Function buildCalibrator of class GNEAdditionalHandler not implemented yet";

    return NULL;
}


bool
GNEAdditionalHandler::buildRerouter(GNEViewNet *viewNet, const std::string& id, const std::vector<GNEEdge*>& edges, SUMOReal prob, const std::string& file, bool off, bool blocked)
{
    std::cout << "Function buildRerouter of class GNEAdditionalHandler not implemented yet";

    return NULL;
}


bool 
GNEAdditionalHandler::buildLaneSpeedTrigger(GNEViewNet *viewNet,const std::string& id, const std::vector<GNELane*>& destLanes, const std::string& file, bool blocked)
{
    std::cout << "Function buildLaneSpeedTrigger of class GNEAdditionalHandler not implemented yet";

    return NULL;
}


std::string 
GNEAdditionalHandler::getFileName(const SUMOSAXAttributes& attrs, const std::string& base, const bool allowEmpty)
{
    // get the file name to read further definitions from
    bool ok = true;
    std::string file = attrs.getOpt<std::string>(SUMO_ATTR_FILE, 0, ok, "");
    if (file == "") {
        if (allowEmpty) {
            return file;
        }
        throw InvalidArgument("No filename given.");
    }
    // check whether absolute or relative filenames are given
    if (!FileHelpers::isAbsolute(file)) {
        return FileHelpers::getConfigurationRelative(base, file);
    }
    return file;
}


GNELane* 
GNEAdditionalHandler::getLane(const SUMOSAXAttributes& attrs, const std::string& tt, const std::string& tid)
{
    bool ok = true;
    std::string objectid = attrs.get<std::string>(SUMO_ATTR_LANE, tid.c_str(), ok);
    std::vector<GNELane*> vectorOfLanes = myViewNet->getNet()->retrieveLanes(); 
    GNELane* lane = 0;

    for(std::vector<GNELane*>::iterator i = vectorOfLanes.begin(); (i != vectorOfLanes.end()) && (lane == 0); i++) {
        if((*i)->getID() == objectid) {
            lane = (*i);
        }
    }

    if (lane == 0) {
        throw InvalidArgument("The lane " + objectid + " to use within the " + tt + " '" + tid + "' is not known.");
    }
    return lane;
}


SUMOReal 
GNEAdditionalHandler::getPosition(const SUMOSAXAttributes& attrs, GNELane& lane, const std::string& tt, const std::string& tid)
{
    bool ok = true;
    SUMOReal pos = attrs.get<SUMOReal>(SUMO_ATTR_POSITION, 0, ok);
    const bool friendlyPos = attrs.getOpt<bool>(SUMO_ATTR_FRIENDLY_POS, 0, ok, false);
    if (!ok) {
        throw InvalidArgument("Error on parsing a position information.");
    }
    if (pos < 0) {
        pos = lane.getLaneShapeLenght() + pos;         // OJO A ESTO)
    }
    if (pos > lane.getLaneShapeLenght()) {
        if (friendlyPos) {
            pos = lane.getLaneShapeLenght() - (SUMOReal) 0.1;
        } else {
            throw InvalidArgument("The position of " + tt + " '" + tid + "' lies beyond the lane's '" + lane.getID() + "' length.");
        }
    }
    return pos;
}


bool 
GNEAdditionalHandler::checkStopPos(SUMOReal& startPos, SUMOReal& endPos, const SUMOReal laneLength, const SUMOReal minLength, 
                                   const bool friendlyPos) {
    if (minLength > laneLength) {
        return false;
    }
    if (startPos < 0) {
        startPos += laneLength;
    }
    if (endPos < 0) {
        endPos += laneLength;
    }
    if (endPos < minLength || endPos > laneLength) {
        if (!friendlyPos) {
            return false;
        }
        if (endPos < minLength) {
            endPos = minLength;
        }
        if (endPos > laneLength) {
            endPos = laneLength;
        }
    }
    if (startPos < 0 || startPos > endPos - minLength) {
        if (!friendlyPos) {
            return false;
        }
        if (startPos < 0) {
            startPos = 0;
        }
        if (startPos > endPos - minLength) {
            startPos = endPos - minLength;
        }
    }
    return true;
}


/****************************************************************************/
