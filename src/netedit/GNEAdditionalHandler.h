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
#ifndef GNEAdditionalHandler_h
#define GNEAdditionalHandler_h


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


// ===========================================================================
// class declarations
// ===========================================================================
class GNENet;
class GNEViewNet;
class GNEUndoList;
class GNEJunction;
class GNEEdge;
class GNELane;
class GNEBusStop;
class GNEChargingStation;
class GNELaneSpeedTrigger;
class GNETriggeredRerouter;
class GNECalibrator;
class GNEDetectorE1;
class GNEDetectorE2;
class GNEDetectorE3;
class MSRouteProbe;    // Equivalence in GNE?
class MSEdgeVector;    // Equivalence in GNE?

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
    GNEAdditionalHandler(const std::string& file, GNEViewNet* viewNet, GNENet *net, GNEUndoList* undoList);

    /// @brief Destructor
    ~GNEAdditionalHandler();

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
    void buildVaporizer(const SUMOSAXAttributes& attrs);

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
                                       const std::string& base);

    /** @brief Parses his values and builds a rerouter
     *
     * @param[in] net The network the rerouter belongs to
     * @param[in] attrs SAX-attributes which define the trigger
     * @param[in] base The base path
     * @exception InvalidArgument If a parameter (edge) is not valid
     */
    void parseAndBuildRerouter(GNENet* net, const SUMOSAXAttributes& attrs,
                               const std::string& base);

    /** @brief Parses his values and builds a bus stop
     *
     * @param[in] net The network the bus stop belongs to
     * @param[in] attrs SAX-attributes which define the trigger
     * @exception InvalidArgument If a parameter (lane/position) is not valid
     */
    void parseAndBuildBusStop(GNENet* net, const SUMOSAXAttributes& attrs);

    /** @brief Parses his values and builds a charging station
     *
     * @param[in] net The network the charging station belongs to
     * @param[in] attrs SAXattributes which define the trigger
     * @exception InvalidArgument If a parameter (lane/position) is not valid
     */
    void parseAndBuildChargingStation(GNENet* net, const SUMOSAXAttributes& attrs);

    /** @brief Parses his values and builds a mesoscopic or microscopic calibrator
     *
     * @param[in] net The network the calibrator belongs to
     * @param[in] attrs SAX-attributes which define the trigger
     * @param[in] base The base path
     * @exception InvalidArgument If a parameter (edge/position) is not valid
     */
    void parseAndBuildCalibrator(GNENet* net, const SUMOSAXAttributes& attrs,
                                 const std::string& base);

    /** @brief Parses his values and builds a induction loop detector (E1)
     *
     * @param[in] net The network the calibrator belongs to
     * @param[in] attrs SAX-attributes which define the trigger
     * @param[in] base The base path
     * @exception InvalidArgument If a parameter (edge/position) is not valid
     */
    void parseAndBuildDetectorE1(GNENet* net, const SUMOSAXAttributes& attrs,
                                 const std::string& base);

    /** @brief Parses his values and builds a lane area detector (E2)
     *
     * @param[in] net The network the calibrator belongs to
     * @param[in] attrs SAX-attributes which define the trigger
     * @param[in] base The base path
     * @exception InvalidArgument If a parameter (edge/position) is not valid
     */
    void parseAndBuildDetectorE2(GNENet* net, const SUMOSAXAttributes& attrs,
                                 const std::string& base);

    /** @brief Parses his values and builds a multi entry exit detector (E3)
     *
     * @param[in] net The network the calibrator belongs to
     * @param[in] attrs SAX-attributes which define the trigger
     * @param[in] base The base path
     * @exception InvalidArgument If a parameter (edge/position) is not valid
     */
    void parseAndBuildDetectorE3(GNENet* net, const SUMOSAXAttributes& attrs,
                                 const std::string& base);
    //@}

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
    GNELaneSpeedTrigger* buildLaneSpeedTrigger(GNENet* net, const std::string& id, 
                                               const std::vector<GNELane*>& destLanes,
                                               const std::string& file);

    /** @brief Builds a bus stop
     *
     * Simply calls the GNEBusStop constructor.
     *
     * @param[in] net The net the bus stop belongs to
     * @param[in] id The id of the bus stop
     * @param[in] lane The lane the bus stop is placed on
     * @param[in] frompos Begin position of the bus stop on the lane
     * @param[in] topos End position of the bus stop on the lane
     * @param[in] lines Names of the bus lines that halt on this bus stop
     * @exception InvalidArgument If the bus stop can not be added to the net (is duplicate)
     */
    void buildBusStop(GNENet* net, const std::string& id, GNELane& lane, SUMOReal frompos, SUMOReal topos, 
                      const std::vector<std::string>& lines);

    /** @brief Builds a charging Station
     *
     * Simply calls the GNEChargingStation constructor.
     *
     * @param[in] net The net the charging Station belongs to
     * @param[in] id The id of the charging Station
     * @param[in] lane The lane the charging Station is placed on
     * @param[in] frompos Begin position of the charging Station on the lane
     * @param[in] topos End position of the charging Station on the lane
     * @param[in] chargingPower power charged in every timeStep
     * @param[in] efficiency efficiency of the charge
     * @param[in] chargeInTransit enable or disable charge in transit
     * @param[in] chargeDelay delay in the charge
     * @exception InvalidArgument If the charging Station can not be added to the net (is duplicate)
     */
    void buildChargingStation(GNENet* net, const std::string& id, GNELane& lane, SUMOReal frompos, SUMOReal topos, 
                              SUMOReal chargingPower, SUMOReal efficiency, bool chargeInTransit, SUMOReal chargeDelay);

    /** @brief Builds a induction loop detector (E1)
     *
     * Simply calls the GNEDetectorE1 constructor.
     *
     * @param[in] net The net the charging Station belongs to
     * @param[in] id The id of the detector
     * @param[in] lane The lane the detector is placed on
     * @param[in] pos position of the detector on the lane
     * @param[in] freq the aggregation period the values the detector collects shall be summed up.
     * @param[in] filename The path to the output file.
     * @param[in] splitByType If set, the collected values will be additionally reported on per-vehicle type base.
     * @exception InvalidArgument If the detector can not be added to the net (is duplicate)
     */
    void buildDetectorE1(GNENet* net, const std::string& id, GNELane& lane, SUMOReal pos, int freq, const std::string &filename, 
                         bool splitByType);

    /** @brief Builds a lane Area Detector (E2)
     *
     * Simply calls the GNEDetectorE2 constructor.
     *
     * @param[in] net The net the charging Station belongs to
     * @param[in] id The id of the detector
     * @param[in] lane The lane the detector is placed on
     * @param[in] pos position of the detector on the lane
     * @param[in] freq the aggregation period the values the detector collects shall be summed up.
     * @param[in] filename The path to the output file.
     * @param[in] cont Holds the information whether detectors longer than a lane shall be cut off or continued
     * @param[in] timeThreshold The time-based threshold that describes how much time has to pass until a vehicle is recognized as halting
     * @param[in] speedThreshold The speed-based threshold that describes how slow a vehicle has to be to be recognized as halting
     * @param[in] jamThreshold 	The minimum distance to the next standing vehicle in order to make this vehicle count as a participant to the jam
     * @param[in] splitByType If set, the collected values will be additionally reported on per-vehicle type base.
     * @exception InvalidArgument If the detector can not be added to the net (is duplicate)
     */
    void buildDetectorE2(GNENet* net, const std::string& id, GNELane& lane, SUMOReal pos, int freq, const std::string &filename, 
                         bool cont, int timeThreshold, SUMOReal speedThreshold, SUMOReal jamThreshold, bool splitByType);

    /** @brief Builds a multi entry exit detector (E3)
     *
     * Simply calls the GNEDetectorE3 constructor.
     *
     * @param[in] net The net the charging Station belongs to
     * @param[in] id The id of the detector
     * @param[in] lane The lane the detector is placed on
     * @param[in] pos position of the detector on the lane
     * @param[in] freq the aggregation period the values the detector collects shall be summed up.
     * @param[in] filename The path to the output file.
     * @exception InvalidArgument If the detector can not be added to the net (is duplicate)
     */
    void buildDetectorE3(GNENet* net, const std::string& id, GNELane& lane, SUMOReal pos, int freq, const std::string &filename);

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
    GNECalibrator* buildCalibrator(GNENet* net, const std::string& id, GNEEdge* edge, SUMOReal pos,
                                   const std::string& file, const std::string& outfile,
                                   const SUMOTime freq, const MSRouteProbe* probe);

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
    GNETriggeredRerouter* buildRerouter(GNENet* net, const std::string& id, MSEdgeVector& edges,
                                        SUMOReal prob, const std::string& file, bool off);

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
    std::string getFileName(const SUMOSAXAttributes& attrs, const std::string& base,
                            const bool allowEmpty = false);

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
    GNELane* getLane(const SUMOSAXAttributes& attrs,  const std::string& tt, const std::string& tid);

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
    SUMOReal getPosition(const SUMOSAXAttributes& attrs, GNELane& lane, const std::string& tt, 
                         const std::string& tid);

    /** @brief check start and end position of a stop
    *
    * This method check if the position of an Stop over a lane is valid
    *
    * @param[in] startPos Start position of Stop
    * @param[in] endPos End position of Stop
    * @param[in] laneLength Lenght of the lane
    * @param[in] minLength Min length of the Stop
    * @param[in] friendlyPos Attribute of Stop
    * @return true if the Stop position is valid, false in otherweise
    */
    bool checkStopPos(SUMOReal& startPos, SUMOReal& endPos, const SUMOReal laneLength,
                      const SUMOReal minLength, const bool friendlyPos);

protected:
    /// @brief Net in which add the additional elements
    GNENet* myNet;

    /// @brief View of the Net
    GNEViewNet* myViewNet;

    /// @brief Undo/redo list
    GNEUndoList* myUndoList;
};


#endif
