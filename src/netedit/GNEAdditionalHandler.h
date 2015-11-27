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
#ifndef GNEADDITIONALHANDLER_H
#define GNEADDITIONALHANDLER_H
// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <vector>
#include <utils/xml/SUMOSAXHandler.h>
#include <utils/common/MsgHandler.h>

/** Quitar **/
#include "GNEBusStop.h"
#include "GNENet.h"
/** **/

// ===========================================================================
// class declarations
// ===========================================================================
class GNENet;
class GNEJunction;
class GNEEdge;
class GNELane;
class GNEBusStop;
class GNEChargingStation;
class GNELaneSpeedTrigger;
class GNETriggeredRerouter;
class GNECalibrator;
class MSRouteProbe;	// Equivalence in GNE?
class MSEdgeVector;	// Equivalence in GNE?

// ===========================================================================
// class definitions
// ===========================================================================

/**
 * @class GNEAdditionalHandler
 * @brief Builds trigger objects for GNENet (busStops, chargingStations, etc..)
 *
 *
 */
class GNEAdditionalHandler : public SUMOSAXHandler {
public:
    /// @brief Constructor
    GNEAdditionalHandler(const std::string& file, GNENet *net);


    /// @brief Destructor
    virtual ~GNEAdditionalHandler();


	/// @name inherited from GenericSAXHandler
    //@{

    /** @brief Called on the opening of a tag;
     *
     * @param[in] element ID of the currently opened element
     * @param[in] attrs Attributes within the currently opened element
     * @exception ProcessError If something fails
     * @see GenericSAXHandler::myStartElement
     * @todo Refactor/describe
     */
	void myStartElement(int element, const SUMOSAXAttributes& attrs);


    /** @brief Builds a vaporization
     *
     * Parses the attributes, reporting errors if the time values are false
     *  or the edge is not known.
     * Instatiates events for enabling and disabling the vaporization otherwise.
     *
     * @param[in] attrs SAX-attributes which define the vaporizer
     * @note recheck throwing the exception
     */
    void buildVaporizer(const SUMOSAXAttributes& attrs)
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


    /// @name parsing methods
    ///
    /// These methods parse the attributes for each of the described trigger
    ///  and call the according methods to build the trigger
    //@{
    /** @brief Parses his values and builds a lane speed trigger
     *
     * If one of the declaration values is errornous, an InvalidArgument is thrown.
     *
     * If the XML-file parsed during initialisation is errornous, and the
     *  GNELaneSpeedTrigger-constructor throws a ProcessError due to this, this
     *  exception is catched and an InvalidArgument with the message given in the
     *  ProcessError is thrown.
     *
     * @param[in] net The network the lane speed trigger belongs to
     * @param[in] attrs SAX-attributes which define the trigger
     * @param[in] base The base path
     * @exception InvalidArgument If a parameter (lane/position) is not valid or the read definition is errornous
     * @see buildLaneSpeedTrigger
     */
    void parseAndBuildLaneSpeedTrigger(GNENet* net, const SUMOSAXAttributes& attrs,
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


    /** @brief Parses his values and builds a rerouter
     *
     * @param[in] net The network the rerouter belongs to
     * @param[in] attrs SAX-attributes which define the trigger
     * @param[in] base The base path
     * @exception InvalidArgument If a parameter (edge) is not valid
     */
    void parseAndBuildRerouter(GNENet* net, const SUMOSAXAttributes& attrs,
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


    /** @brief Parses his values and builds a bus stop
     *
     * @param[in] net The network the bus stop belongs to
     * @param[in] attrs SAX-attributes which define the trigger
     * @exception InvalidArgument If a parameter (lane/position) is not valid
     */
    void parseAndBuildBusStop(GNENet* net, const SUMOSAXAttributes& attrs)
	{
		bool ok = true;
		// get the id, throw if not given or empty...
		std::string id = attrs.get<std::string>(SUMO_ATTR_ID, 0, ok);
		if (!ok) {
			throw ProcessError();
		}
		/*
		// get the lane
		GNELane* lane = getLane(attrs, "busStop", id);
		// get the positions
		SUMOReal frompos = attrs.getOpt<SUMOReal>(SUMO_ATTR_STARTPOS, id.c_str(), ok, 0);
		SUMOReal topos = attrs.getOpt<SUMOReal>(SUMO_ATTR_ENDPOS, id.c_str(), ok, lane->getLength());
		const bool friendlyPos = attrs.getOpt<bool>(SUMO_ATTR_FRIENDLY_POS, id.c_str(), ok, false);
		if (!ok || !myHandler->checkStopPos(frompos, topos, lane->getLength(), POSITION_EPS, friendlyPos)) {
			throw InvalidArgument("Invalid position for bus stop '" + id + "'.");
		}
		// get the lines
		std::vector<std::string> lines;
		SUMOSAXAttributes::parseStringVector(attrs.getOpt<std::string>(SUMO_ATTR_LINES, id.c_str(), ok, "", false), lines);
		// build the bus stop
		
		buildBusStop(net, id, lines, lane, frompos, topos);
		*/
	}


    /** @brief Parses his values and builds a container stop
     *
     * @param[in] net The network the container stop belongs to
     * @param[in] attrs SAX-attributes which define the trigger
     * @exception InvalidArgument If a parameter (lane/position) is not valid
     */
    void parseAndBuildContainerStop(GNENet* net, const SUMOSAXAttributes& attrs)
	{
		bool ok = true;
		// get the id, throw if not given or empty...
		std::string id = attrs.get<std::string>(SUMO_ATTR_ID, 0, ok);
		if (!ok) {
			throw ProcessError();
		}
		/*
		// get the lane
		GNELane* lane = getLane(attrs, "containerStop", id);
		// get the positions
		SUMOReal frompos = attrs.getOpt<SUMOReal>(SUMO_ATTR_STARTPOS, id.c_str(), ok, 0);
		SUMOReal topos = attrs.getOpt<SUMOReal>(SUMO_ATTR_ENDPOS, id.c_str(), ok, lane->getLength());
		const bool friendlyPos = attrs.getOpt<bool>(SUMO_ATTR_FRIENDLY_POS, id.c_str(), ok, false);
		if (!ok || !myHandler->checkStopPos(frompos, topos, lane->getLength(), POSITION_EPS, friendlyPos)) {
			throw InvalidArgument("Invalid position for container stop '" + id + "'.");
		}
		// get the lines
		std::vector<std::string> lines;
		SUMOSAXAttributes::parseStringVector(attrs.getOpt<std::string>(SUMO_ATTR_LINES, id.c_str(), ok, "", false), lines);
		// build the container stop
		buildContainerStop(net, id, lines, lane, frompos, topos);
		*/
	}


    /** @brief Parses his values and builds a charging station
     *
     * @param[in] net The network the charging station belongs to
     * @param[in] attrs SAXattributes which define the trigger
     * @exception InvalidArgument If a parameter (lane/position) is not valid
     */
    void parseAndBuildChrgStn(GNENet* net, const SUMOSAXAttributes& attrs)
	{
		bool ok = true;

		// get the id, throw if not given or empty...
		std::string id = attrs.get<std::string>(SUMO_ATTR_ID, 0, ok);

		if (!ok) {
			throw ProcessError();
		}
		/*
		// get the lane
		GNELane* lane = getLane(attrs, "chargingStation", id);

		// get the positions
		SUMOReal frompos = attrs.getOpt<SUMOReal>(SUMO_ATTR_STARTPOS, id.c_str(), ok, 0);
		SUMOReal topos = attrs.getOpt<SUMOReal>(SUMO_ATTR_ENDPOS, id.c_str(), ok, lane->getLength());
		SUMOReal chrgpower = attrs.getOpt<SUMOReal>(SUMO_ATTR_CHRGPOWER, id.c_str(), ok, 0);
		SUMOReal efficiency = attrs.getOpt<SUMOReal>(SUMO_ATTR_EFFICIENCY, id.c_str(), ok, 0);
		SUMOReal chargeInTransit = attrs.getOpt<SUMOReal>(SUMO_ATTR_CHRGINTRANSIT, id.c_str(), ok, 0);
		SUMOReal ChargeDelay = attrs.getOpt<SUMOReal>(SUMO_ATTR_CHRGDELAY, id.c_str(), ok, 0);

		const bool friendlyPos = attrs.getOpt<bool>(SUMO_ATTR_FRIENDLY_POS, id.c_str(), ok, false);

		if (!ok || !myHandler->checkStopPos(frompos, topos, lane->getLength(), POSITION_EPS, friendlyPos)) {
			throw InvalidArgument("Invalid position for Charging Station '" + id + "'.");
		}

		// get the lines
		std::vector<std::string> lines;
		SUMOSAXAttributes::parseStringVector(attrs.getOpt<std::string>(SUMO_ATTR_LINES, id.c_str(), ok, "", false), lines);

		// build the Charging Station
		buildChrgStn(net, id, lines, lane, frompos, topos, chrgpower, efficiency, chargeInTransit, ChargeDelay);
		*/
	}

    /** @brief Parses his values and builds a mesoscopic or microscopic calibrator
     *
     * @param[in] net The network the calibrator belongs to
     * @param[in] attrs SAX-attributes which define the trigger
     * @param[in] base The base path
     * @exception InvalidArgument If a parameter (edge/position) is not valid
     */
    void parseAndBuildCalibrator(GNENet* net, const SUMOSAXAttributes& attrs,
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
    //@}


protected:
    /// @name building methods
    ///
    /// Called with parsed values, these methods build the trigger.
    ///
    /// These methods should be overriden for the gui loader in order
    ///  to build visualizable versions of the triggers.
    ///
    /// In most cases, these methods only call the constructor.
    //@{

    /** @brief Builds a lane speed trigger
     *
     * Simply calls the GNELaneSpeedTrigger constructor.
     *
     * @param[in] net The net the lane speed trigger belongs to
     * @param[in] id The id of the lane speed trigger
     * @param[in] destLanes List of lanes affected by this speed trigger
     * @param[in] file Name of the file to read the speeds to set from
     * @see GNELaneSpeedTrigger
     * @exception ProcessError If the XML definition file is errornous
     */
    virtual GNELaneSpeedTrigger* buildLaneSpeedTrigger(GNENet* net,
            const std::string& id, const std::vector<GNELane*>& destLanes,
            const std::string& file)
	{
		std::cout << "Function buildLaneSpeedTrigger of class GNEAdditionalHandler not implemented yet";

		return NULL;
	}


    /** @brief Builds a bus stop
     *
     * Simply calls the MSStoppingPlace constructor.
     *
     * @param[in] net The net the bus stop belongs to
     * @param[in] id The id of the bus stop
     * @param[in] lines Names of the bus lines that halt on this bus stop
     * @param[in] lane The lane the bus stop is placed on
     * @param[in] frompos Begin position of the bus stop on the lane
     * @param[in] topos End position of the bus stop on the lane
     * @exception InvalidArgument If the bus stop can not be added to the net (is duplicate)
     */
    void buildBusStop(GNENet* net,
						const std::string& id, const std::vector<std::string>& lines,
                        GNELane* lane, SUMOReal frompos, SUMOReal topos)
	{

        /** OJO! Cambiar esto por stoppingPlace **/
        GNEBusStop* stop = new GNEBusStop(id, lines, *lane, frompos, topos);
        if (!net->addBusStop(stop)) {
            delete stop;
        throw InvalidArgument("Could not build bus stop in netEdit '" + id + "'; probably declared twice.");
		}
	}

    /** @brief Builds a charging Station
     *
     * Simply calls the MSBusStop constructor.
     *
     * @param[in] net The net the charging Station belongs to
     * @param[in] id The id of the charging Station
     * @param[in] lines Names of the bus lines that halt on this charging Station
     * @param[in] lane The lane the charging Station is placed on
     * @param[in] frompos Begin position of the charging Station on the lane
     * @param[in] topos End position of the charging Station on the lane
     * @exception InvalidArgument If the charging Station can not be added to the net (is duplicate)
     */
    virtual void buildChrgStn(GNENet* net,
                              const std::string& id, const std::vector<std::string>& lines,
                              GNELane* lane, SUMOReal frompos, SUMOReal topos, SUMOReal chrgpower, SUMOReal efficiency, SUMOReal chargeInTransit, SUMOReal ChargeDelay)
	{

	}

    /** @brief Builds a container stop
     *
     * Simply calls the MSStoppingPlace constructor.
     *
     * @param[in] net The net the container stop belongs to
     * @param[in] id The id of the container stop
     * @param[in] lines Names of the lines that halt on this container stop
     * @param[in] lane The lane the container stop is placed on
     * @param[in] frompos Begin position of the container stop on the lane
     * @param[in] topos End position of the container stop on the lane
     * @exception InvalidArgument If the container stop can not be added to the net (is duplicate)
     */
    virtual void buildContainerStop(GNENet* net,
                                    const std::string& id, const std::vector<std::string>& lines,
                                    GNELane* lane, SUMOReal frompos, SUMOReal topos)
	{

	}


    /** @brief builds a microscopic calibrator
     *
     * Simply calls the MSCalibrator constructor.
     *
     * @param[in] net The net the calibrator belongs to
     * @param[in] id The id of the calibrator
     * @param[in] edge The edge the calibrator is placed at
     * @param[in] pos The position on the edge the calibrator lies at
     * @param[in] file The file to read the flows from
     * @todo Is the position correct/needed
     */
    virtual GNECalibrator* buildCalibrator(GNENet* net,
                                          const std::string& id, MSEdge* edge, SUMOReal pos,
                                          const std::string& file, const std::string& outfile,
                                          const SUMOTime freq, const MSRouteProbe* probe)
	{
		std::cout << "Function buildCalibrator of class GNEAdditionalHandler not implemented yet";

		return NULL;
	}


    /** @brief builds an rerouter
     *
     * Simply calls the MSTriggeredRerouter constructor.
     *
     * @param[in] net The net the rerouter belongs to
     * @param[in] id The id of the rerouter
     * @param[in] edges The edges the rerouter is placed at
     * @param[in] prob The probability the rerouter reoutes vehicles with
     * @param[in] file The file to read the reroute definitions from
     */
    virtual GNETriggeredRerouter* buildRerouter(GNENet* net,
            const std::string& id, MSEdgeVector& edges,
            SUMOReal prob, const std::string& file, bool off)
	{
		std::cout << "Function buildRerouter of class GNEAdditionalHandler not implemented yet";

		return NULL;
	}
    //@}


protected:
    /// @name helper method for obtaining and checking values
    /// @{

    /** @brief Helper method to obtain the filename
     *
     * Retrieves "file" from attributes, checks whether it is absolute
     *  and extends it by the given base path if not. Returns this
     *  information.
     *
     * @param[in] attrs The attributes to obtain the file name from
     * @param[in] base The base path (the path the loaded additional file lies in)
     * @return The (expanded) path to the named file
     * @todo Recheck usage of the helper class
     */
    std::string getFileName(const SUMOSAXAttributes& attrs,
                            const std::string& base,
                            const bool allowEmpty = false)
	{

	}


    /** @brief Returns the lane defined by attribute "lane"
     *
     * Retrieves the lane id from the given attrs. Tries to retrieve the lane,
     *  throws an InvalidArgument if it does not exist.
     *
     * @param[in] attrs The attributes to obtain the lane id from
     * @param[in] tt The trigger type (for user output)
     * @param[in] tid The trigger id (for user output)
     * @return The named lane if it is known
     * @exception InvalidArgument If the named lane does not exist or a lane is not named
     */
    GNELane* getLane(const SUMOSAXAttributes& attrs,
                    const std::string& tt, const std::string& tid)
	{

	}


    /** @brief returns the position on the lane checking it
     *
     * This method extracts the position, checks whether it shall be mirrored
     *  and checks whether it is within the lane. If not, an error is reported
     *  and a InvalidArgument is thrown.
     *
     * @param[in] attrs The attributes to obtain the position from
     * @param[in] lane The lane the position shall be valid for
     * @param[in] tt The trigger type (for user output)
     * @param[in] tid The trigger id (for user output)
     * @return The position on the lane
     * @exception InvalidArgument If the position is beyond the lane
     */
    SUMOReal getPosition(const SUMOSAXAttributes& attrs,
                         GNELane* lane, const std::string& tt, const std::string& tid)
	{

	}
    /// @}


protected:
    /// @brief Net in which add the additional elements
    GNENet* myNet;

};




#endif