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
GNEAdditionalHandler::myStartElement(int element, const SUMOSAXAttributes& attrs) {
	try {
		switch (element) {
			case SUMO_TAG_BUS_STOP:
				parseAndBuildBusStop(myNet, attrs);
				break;
			case SUMO_TAG_CHARGING_STATION:
				parseAndBuildChargingStation(myNet, attrs);
				break;
			default:
				break;
		}
	} catch (InvalidArgument& e) {
		WRITE_ERROR(e.what());
	}
}


void 
GNEAdditionalHandler::buildVaporizer(const SUMOSAXAttributes& attrs)
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
GNEAdditionalHandler::parseAndBuildLaneSpeedTrigger(GNENet* net, const SUMOSAXAttributes& attrs, 
                                                    const std::string& base)
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
		MSLaneSpeedTrigger* trigger = buildLaneSpeedTrigger(net, id, lanes, file);
		if (file == "") {
			trigger->registerParent(SUMO_TAG_VSS, myHandler);
		}
	} catch (ProcessError& e) {
		throw InvalidArgument(e.what());
	}
*/
}


void 
GNEAdditionalHandler::parseAndBuildRerouter(GNENet* net, const SUMOSAXAttributes& attrs, 
                                            const std::string& base)
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
	MSTriggeredRerouter* trigger = buildRerouter(net, id, edges, prob, file, off);
	// read in the trigger description
	if (file == "") {
		trigger->registerParent(SUMO_TAG_REROUTER, myHandler);
	} else if (!XMLSubSys::runParser(*trigger, file)) {
		throw ProcessError();
	}
*/
}


void 
GNEAdditionalHandler::parseAndBuildBusStop(GNENet* net, const SUMOSAXAttributes& attrs)
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
	SUMOReal topos = attrs.getOpt<SUMOReal>(SUMO_ATTR_ENDPOS, id.c_str(), ok, lane->getLength());
	const bool friendlyPos = attrs.getOpt<bool>(SUMO_ATTR_FRIENDLY_POS, id.c_str(), ok, false);
	if (!ok || !checkStopPos(frompos, topos, lane->getLength(), POSITION_EPS, friendlyPos)) {
		throw InvalidArgument("Invalid position for bus stop '" + id + "'.");
	}
	// get the lines
	std::vector<std::string> lines;
	SUMOSAXAttributes::parseStringVector(attrs.getOpt<std::string>(SUMO_ATTR_LINES, id.c_str(), ok, "", false), lines);
	// build the bus stop
	buildBusStop(net, id, lines, lane, frompos, topos);
}


void 
GNEAdditionalHandler::parseAndBuildContainerStop(GNENet* net, const SUMOSAXAttributes& attrs)
{
	bool ok = true;
	// get the id, throw if not given or empty...
	std::string id = attrs.get<std::string>(SUMO_ATTR_ID, 0, ok);
	if (!ok) {
		throw ProcessError();
	}
	// get the lane
	GNELane* lane = getLane(attrs, "containerStop", id);
	// get the positions
	SUMOReal frompos = attrs.getOpt<SUMOReal>(SUMO_ATTR_STARTPOS, id.c_str(), ok, 0);
	SUMOReal topos = attrs.getOpt<SUMOReal>(SUMO_ATTR_ENDPOS, id.c_str(), ok, lane->getLength());
	const bool friendlyPos = attrs.getOpt<bool>(SUMO_ATTR_FRIENDLY_POS, id.c_str(), ok, false);
	if (!ok || !checkStopPos(frompos, topos, lane->getLength(), POSITION_EPS, friendlyPos)) {
		throw InvalidArgument("Invalid position for container stop '" + id + "'.");
	}
	// get the lines
	std::vector<std::string> lines;
	SUMOSAXAttributes::parseStringVector(attrs.getOpt<std::string>(SUMO_ATTR_LINES, id.c_str(), ok, "", false), lines);
	// build the container stop
	buildContainerStop(net, id, lines, lane, frompos, topos);
}


void 
GNEAdditionalHandler::parseAndBuildChargingStation(GNENet* net, const SUMOSAXAttributes& attrs)
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
	SUMOReal topos = attrs.getOpt<SUMOReal>(SUMO_ATTR_ENDPOS, id.c_str(), ok, lane->getLength());
	SUMOReal chrgpower = attrs.getOpt<SUMOReal>(SUMO_ATTR_CHARGINGPOWER, id.c_str(), ok, 0);
	SUMOReal efficiency = attrs.getOpt<SUMOReal>(SUMO_ATTR_EFFICIENCY, id.c_str(), ok, 0);
	SUMOReal chargeInTransit = attrs.getOpt<SUMOReal>(SUMO_ATTR_CHARGEINTRANSIT, id.c_str(), ok, 0);
	SUMOReal chargeDelay = attrs.getOpt<SUMOReal>(SUMO_ATTR_CHARGEDELAY, id.c_str(), ok, 0);

	const bool friendlyPos = attrs.getOpt<bool>(SUMO_ATTR_FRIENDLY_POS, id.c_str(), ok, false);

	if (!ok || !checkStopPos(frompos, topos, lane->getLength(), POSITION_EPS, friendlyPos)) {
		throw InvalidArgument("Invalid position for Charging Station '" + id + "'.");
	}

	// get the lines
	std::vector<std::string> lines;
	SUMOSAXAttributes::parseStringVector(attrs.getOpt<std::string>(SUMO_ATTR_LINES, id.c_str(), ok, "", false), lines);

	// build the Charging Station
	buildChargingStation(net, id, lane, frompos, topos, chrgpower, efficiency, chargeInTransit, chargeDelay);
}


void 
GNEAdditionalHandler::parseAndBuildCalibrator(GNENet* net, const SUMOSAXAttributes& attrs, 
                                              const std::string& base)
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
		METriggeredCalibrator* trigger = buildMECalibrator(net, id, &lane->getEdge(), pos, file, outfile, freq, probe);
		if (file == "") {
			trigger->registerParent(SUMO_TAG_CALIBRATOR, myHandler);
		}
#endif
	} else {
		MSCalibrator* trigger = buildCalibrator(net, id, &lane->getEdge(), pos, file, outfile, freq, probe);
		if (file == "") {
			trigger->registerParent(SUMO_TAG_CALIBRATOR, myHandler);
		}
	}*/
}


GNELaneSpeedTrigger* 
GNEAdditionalHandler::buildLaneSpeedTrigger(GNENet* net, const std::string& id, const std::vector<GNELane*>& destLanes, 
                                            const std::string& file)
{
	std::cout << "Function buildLaneSpeedTrigger of class GNEAdditionalHandler not implemented yet";

	return NULL;
}


void 
GNEAdditionalHandler::buildBusStop(GNENet* net, const std::string& id, const std::vector<std::string>& lines, 
                                   GNELane* lane, SUMOReal frompos, SUMOReal topos)
{
    GNEBusStop* stop = new GNEBusStop(id, lines, *lane, frompos, topos);
    if (!net->addBusStop(stop)) {
        delete stop;
    throw InvalidArgument("Could not build bus stop in netEdit '" + id + "'; probably declared twice.");
	}
}

void 
GNEAdditionalHandler::buildChargingStation(GNENet* net, const std::string& id, GNELane* lane, SUMOReal frompos, SUMOReal topos, 
                                           SUMOReal chargingPower, SUMOReal efficiency, SUMOReal chargeInTransit, SUMOReal chargeDelay)
{
    GNEChargingStation* chargingStation = new GNEChargingStation(id, *lane, frompos, topos, chargingPower, efficiency, chargeInTransit, chargeDelay);
    if (!net->addChargingStation(chargingStation)) {
        delete chargingStation;
    throw InvalidArgument("Could not build charging station in netEdit '" + id + "'; probably declared twice.");
	}
}

void 
GNEAdditionalHandler::buildContainerStop(GNENet* net, const std::string& id, const std::vector<std::string>& lines, 
                                         GNELane* lane, SUMOReal frompos, SUMOReal topos)
{

}


GNECalibrator* 
GNEAdditionalHandler::buildCalibrator(GNENet* net, const std::string& id, GNEEdge* edge, SUMOReal pos, const std::string& file, 
                                      const std::string& outfile, const SUMOTime freq, const MSRouteProbe* probe)
{
	std::cout << "Function buildCalibrator of class GNEAdditionalHandler not implemented yet";

	return NULL;
}


GNETriggeredRerouter* 
GNEAdditionalHandler::buildRerouter(GNENet* net, const std::string& id, MSEdgeVector& edges, SUMOReal prob, 
                                    const std::string& file, bool off)
{
	std::cout << "Function buildRerouter of class GNEAdditionalHandler not implemented yet";

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
	std::vector<GNELane*> vectorOfLanes = myNet->retrieveLanes(); 
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
GNEAdditionalHandler::getPosition(const SUMOSAXAttributes& attrs, GNELane* lane, const std::string& tt, const std::string& tid)
{
	bool ok = true;
	SUMOReal pos = attrs.get<SUMOReal>(SUMO_ATTR_POSITION, 0, ok);
	const bool friendlyPos = attrs.getOpt<bool>(SUMO_ATTR_FRIENDLY_POS, 0, ok, false);
	if (!ok) {
		throw InvalidArgument("Error on parsing a position information.");
	}
	if (pos < 0) {
		pos = lane->getLength() + pos;
	}
	if (pos > lane->getLength()) {
		if (friendlyPos) {
			pos = lane->getLength() - (SUMOReal) 0.1;
		} else {
			throw InvalidArgument("The position of " + tt + " '" + tid + "' lies beyond the lane's '" + lane->getID() + "' length.");
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
