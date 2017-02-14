/****************************************************************************/
/// @file    MSMultiLaneE2Collector.h
/// @author  Leonhard Luecken
/// @date    Mon Feb 03 2014 14:13 CET
/// @version $Id: MSMultiLaneE2Collector.h 22841 2017-02-03 22:10:57Z luecken $
///
// An areal detector corresponding to a sequence of consecutive lanes
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2017 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef MSMultiLaneE2Collector_h
#define MSMultiLaneE2Collector_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <vector>
#include <list>
#include <microsim/MSLane.h>
#include <microsim/MSNet.h>
#include <microsim/MSMoveReminder.h>
#include <microsim/output/MSDetectorFileOutput.h>
#include <utils/common/UtilExceptions.h>
#include <utils/vehicle/SUMOVehicle.h>
#include <cassert>

// ===========================================================================
// class declarations
// ===========================================================================
class OutputDevice;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class MSMultiLaneE2Collector
 * @brief An areal detector corresponding to a sequence of consecutive lanes
 *
 * This detector traces vehicles which are on a sequence of consecutive lanes. A
 *  vehicle that enters the detector is stored and the stored vehicles' speeds
 *  are used within each timestep to compute the detector values. As soon as the
 *  vehicle leaves the detector, it is no longer tracked.
 *
 * Determining entering and leaving vehicles is done via the MSMoveReminder
 *  interface. The values are computed by an event-callback (at the end of
 *  a time step).
 *
 * @note As soon as a vehicle enters the detector, a VehicleInfo object is created
 *        and stored in myVehicleInfos. This is constantly updated as long as the
 *        vehicle stays on the detector (i.e. calls notifyMove()). All movement
 *        notifications sent by vehicles on the detector are temporarily stored
 *        in myMoveNotifications, see notifyMove(). Finally they are integrated
 *        into myVehicleInfos when updateDetector is called.
 * @note When subclassing this detector, it is probably sufficient to adapt the
 *        definition of the structs VehicleInfo and the MoveNotification, as well as
 *        the methods that define and create those structs, i.e., makeVehicleInfo()
 *        and makeMoveNotification(). Further the integration of new movement
 *        notifications of the last time step into the vehicle infos is done
 *        in updateVehicleInfos().
 *
 */


class MSMultiLaneE2Collector : public MSMoveReminder, public MSDetectorFileOutput {
public:
    /** @brief A VehicleInfo stores values that are tracked for the individual vehicles on the detector,
     *         e.g., accumulated timeloss. These infos are stored in myVehicles. If a vehicle leaves the detector
     *         (may it be temporarily), the entry in myVehicles is discarded, i.e. all information on the vehicle is reset.
    */
    struct VehicleInfo {
        /** @note Constructor expects an _entryLane argument corresponding to a lane, which is part of the detector.
        */
        VehicleInfo(std::string id, std::string type, SUMOReal length, const MSLane* entryLane, SUMOReal entryOffset,
                std::size_t currentOffsetIndex, SUMOReal exitOffset, SUMOReal distToDetectorEnd) :
            id(id),
            type(type),
            length(length),
            enteredLanes(),
            currentLane(entryLane),
            currentOffsetIndex(currentOffsetIndex),
            entryLaneID(entryLane->getID()),
            entryOffset(entryOffset),
            exitOffset(exitOffset),
            totalTimeOnDetector(0.),
            accumulatedTimeLoss(0.),
            distToDetectorEnd(distToDetectorEnd)
        {}
        virtual ~VehicleInfo(){};
        /// vehicle's ID
        std::string id;
        /// vehicle's type
        std::string type;
        /// vehicle's length
        SUMOReal length;
        /// ID of the lane, on which the vehicle entered the detector
        std::string entryLaneID;
        /// Lanes, which the vehicle entered in the last step (only used if the vehicle already was on the detector in the last step)
        std::vector<const MSLane*> enteredLanes;
        /// Lane, on which the vehicle currently resides (always the one for which the last notifyEnter was received)
        const MSLane* currentLane;
        /// Index of currentLane in the detector's myLanes vector.
        std::size_t currentOffsetIndex;
        /// Distance of that lane's beginning to the detector start (can be negative for the first lane)
        SUMOReal entryOffset;
        /// Offset from the detector start, where the vehicle has leaves the detector (defaults to detector length and is updated
        /// if the vehicle leaves the detector via a junction before reaching its end, i.e. enters a lane not part of the detector)
        SUMOReal exitOffset;
        /// Accumulated time that this vehicle has spent on the detector since its last entry
        SUMOReal totalTimeOnDetector;
        /// Accumulated time loss that this vehicle suffered since it entered the detector
        SUMOReal accumulatedTimeLoss;
        /// Distance left till the detector end after the last integration step (may become negative if the vehicle passes beyond the detector end)
        SUMOReal distToDetectorEnd;
    };

    typedef std::map<std::string, VehicleInfo> VehicleInfoMap;


private:
    /** @brief Values collected in notifyMove and needed in detectorUpdate() to
     *          calculate the accumulated quantities for the detector. These are
     *          temporarily stored in myMoveNotifications for each step.
    */
    struct MoveNotificationInfo {
        MoveNotificationInfo(std::string _vehID, SUMOReal _oldPos, SUMOReal _newPos, SUMOReal _speed, SUMOReal _distToDetectorEnd, SUMOReal _timeOnDetector, SUMOReal _lengthOnDetector, SUMOReal _timeLoss) :
        id(_vehID),
        oldPos(_oldPos),
        newPos(_newPos),
        speed(_speed),
        distToDetectorEnd(_distToDetectorEnd),
        timeOnDetector(_timeOnDetector),
        lengthOnDetector(_lengthOnDetector),
        timeLoss(_timeLoss){}

        virtual ~MoveNotificationInfo() {};

        /// Vehicle's id
        std::string id;
        /// Position before the last integration step (relative to the vehicle's entry lane on the detector)
        SUMOReal oldPos;
        /// Position after the last integration step (relative to the vehicle's entry lane on the detector)
        SUMOReal newPos;
        /// Speed after the last integration step
        SUMOReal speed;
        /// Time spent on the detector during the last integration step
        SUMOReal timeOnDetector;
        /// The length of the part of the vehicle on the detector at the end of the last time step
        SUMOReal lengthOnDetector;
        /// timeloss during the last integration step
        SUMOReal timeLoss;
        /// Distance left till the detector end after the last integration step (may become negative if the vehicle passes beyond the detector end)
        SUMOReal distToDetectorEnd;
    };



    /** @brief Internal representation of a jam
     *
     * Used in execute, instances of this structure are used to track
     *  begin and end positions (as vehicles) of a jam.
     */
    struct JamInfo {
        /// @brief The first standing vehicle
        std::vector<MoveNotificationInfo>::const_iterator firstStandingVehicle;

        /// @brief The last standing vehicle
        std::vector<MoveNotificationInfo>::const_iterator lastStandingVehicle;
    };


public:

    /** @brief Constructor
    *
    * @param[in] id The detector's unique id.
    * @param[in] usage Information how the detector is used
    * @param[in] endLane The lane the detector ends
    * @param[in] endPos The position on the endLane lane the detector is placed at
    * @param[in] upstreamRange The length the detector has (heuristic placement is done if the upstream continuation is not unique)
    * @param[in] vTypes Vehicle types, that the detector takes into account
    */
    MSMultiLaneE2Collector(const std::string& id,
            DetectorUsage usage, MSLane* endLane, SUMOReal endPos, SUMOReal upstreamRange,
            SUMOReal haltingTimeThreshold, SUMOReal haltingSpeedThreshold, SUMOReal jamDistThreshold,
            const std::string& vTypes, bool /*showDetector*/);



    /// @brief Destructor
    virtual ~MSMultiLaneE2Collector() {};

    /** @brief Returns the detector's usage type
     *
     * @see DetectorUsage
     * @return How the detector is used.
     */
    virtual DetectorUsage getUsageType() const {
        return myUsage;
    }



    /// @name Methods inherited from MSMoveReminder
    /// @{

    /** @brief Adds/removes vehicles from the list of vehicles to regard
     *
     * As soon as the reported vehicle enters the detector area (position>myStartPos)
     *  it is added to the list of vehicles to regard (myKnownVehicles). It
     *  is removed from this list if it leaves the detector (position<length>myEndPos).
     * The method returns true as long as the vehicle is not beyond the detector.
     *
     * @param[in] veh The vehicle in question.
     * @param[in] oldPos Position before the move-micro-timestep.
     * @param[in] newPos Position after the move-micro-timestep.
     *                   Note that this position is given in reference
     *                   to the begin of the entry lane of the vehicle.
     * @param[in] newSpeed Unused here.
     * @return False, if vehicle passed the detector entirely, else true.
     * @see MSMoveReminder
     * @see MSMoveReminder::notifyMove
     */
    bool notifyMove(SUMOVehicle& veh, SUMOReal oldPos, SUMOReal newPos,
                    SUMOReal newSpeed);


    /** @brief Removes a known vehicle due to its lane-change
     *
     * If the reported vehicle is known, it is removed from the list of
     *  vehicles to regard (myKnownVehicles).
     *
     * @param[in] veh The leaving vehicle.
     * @param[in] lastPos Position on the lane when leaving.
     * @param[in] isArrival whether the vehicle arrived at its destination
     * @param[in] isLaneChange whether the vehicle changed from the lane
     * @see MSMoveReminder::notifyLeave
     */
    bool notifyLeave(SUMOVehicle& veh, SUMOReal lastPos, MSMoveReminder::Notification reason);


    /** @brief Adds the vehicle to known vehicles if not beyond the dector
     *
     * If the vehicles is within the detector are, it is added to the list
     *  of known vehicles.
     * The method returns true as long as the vehicle is not beyond the detector.
     *
     * @param[in] veh The entering vehicle.
     * @param[in] reason how the vehicle enters the lane
     * @return False, if vehicle passed the detector entirely, else true.
     * @see MSMoveReminder::notifyEnter
     * @see MSMoveReminder::Notification
     */
    bool notifyEnter(SUMOVehicle& veh, MSMoveReminder::Notification reason, const MSLane* enteredLane);
    /// @}





    /// @name Methods inherited from MSDetectorFileOutput.
    /// @{

    /** @brief Computes the detector values in each time step
     *
     * This method should be called at the end of a simulation step, when
     *  all vehicles have moved. The current values are computed and
     *  summed up with the previous.
     *
     * @param[in] currentTime The current simulation time
     */
    void detectorUpdate(const SUMOTime step);


    /** @brief Write the generated output to the given device
     * @param[in] dev The output device to write the data into
     * @param[in] startTime First time step the data were gathered
     * @param[in] stopTime Last time step the data were gathered
     * @exception IOError If an error on writing occurs
     */
    virtual void writeXMLOutput(OutputDevice& dev, SUMOTime startTime, SUMOTime stopTime);


    /** @brief Open the XML-output
     *
     * The implementing function should open an xml element using
     *  OutputDevice::writeXMLHeader.
     *
     * @param[in] dev The output device to write the root into
     * @exception IOError If an error on writing occurs
     */
    virtual void writeXMLDetectorProlog(OutputDevice& dev) const;

    /// @}


    /** @brief Returns the begin position of the detector
     *
     * @return The detector's begin position
     */
    SUMOReal getStartPos() const {
        return myStartPos;
    }


    /** @brief Returns the end position of the detector
     *
     * @return The detector's end position
     */
    SUMOReal getEndPos() const {
        return myEndPos;
    }

    /// @name Methods returning current values
    /// @{


    /** @brief Returns the VehicleInfos for the vehicles currently on the detector
     */
    const std::map<std::string, VehicleInfo>& getVehicleInfos() const{
        return myVehicleInfos;
    }

    /// @}



private:

    /** @brief checks whether the vehicle stands in a jam
     *  TODO: check whether the order of vehicles was important here (discarded by replacing myKnownVehicles with myVehicleInfos)
     *
     * @param[in] mni
     * @param[in/out] haltingVehicles
     * @param[in/out] intervalHaltingVehicles
     * @return Whether vehicle is in a jam.
     */
    bool checkJam(std::vector<MoveNotificationInfo>::const_iterator mni, std::map<std::string, SUMOTime>& haltingVehicles, std::map<std::string, SUMOTime>& intervalHaltingVehicles);


    /** @brief Either adds the vehicle to the end of an existing jam, or closes the last jam, and/or creates a new jam
     *
     * @param isInJam
     * @param mni
     * @param[in/out] currentJam
     * @param[in/out] jams
     */
    void buildJam(bool isInJam, std::vector<MoveNotificationInfo>::const_iterator mni, JamInfo*& currentJam, std::vector<JamInfo*>& jams);


    /** @brief Calculates aggregated values from the given jam structure, deletes all jam-pointers
     *
     * @param jams
     */
    void processJams(std::vector<JamInfo*>& jams);


    /** @brief calculates the time spent on the detector in the last step for the given vehicle
     *
     * @param entryOffset distance of the beginning of the lane, where the vehicle entered the detector to the detector begin
     * @param mni actual MoveNotification
     * @return Total time spent on the detector during the last step
     */
    SUMOReal calculateTimeOnDetector(const SUMOVehicle& veh, SUMOReal oldPos, SUMOReal newPos, const VehicleInfo& vi) const;

    /** @brief calculates the timeloss suffered in the last step for the given vehicle
     *
     * @param entryOffset distance of the beginning of the lane, where the vehicle entered the detector to the detector begin
     * @param mni actual MoveNotification
     * @return Total time loss suffered during the last step
     */
    SUMOReal calculateTimeLoss(const SUMOVehicle& veh, SUMOReal oldPos, SUMOReal newPos, const VehicleInfo& vi) const;

    /** @brief Checks integrity of myLanes, adds internal-lane information, inits myLength, myFirstLane, myLastLane, myOffsets
     *         Called once at construction.
     *  @requires myLanes should form a continuous sequence.
     */
    void initAuxiliaries(std::vector<MSLane*>& lanes);

    /** @brief Builds myLanes from the given information. Also inits startPos.
     *          Selects lanes heuristically if no unambiguous upstream continuation exists.
     *
     *  @param[in] endLane Lane, where the detector ends
     *  @param[in] endPos  End position on the endLane
     *  @param[in] upstreamRange Length of the detector
     */
    std::vector<MSLane*> selectLanes(MSLane* endLane, SUMOReal endPos, SUMOReal upstreamRange);


    /** @brief This adds the detector as a MoveReminder to the associated lanes.
     */
    void addDetectorToLanes(std::vector<MSLane*>& lanes);


    /** @brief Aggregates and normalize some values for the detector output during detectorUpdate()
     */
    void aggregateOutputValues();


    /** @brief This updates the detector values and the VehicleInfo of a vehicle on the detector
     *          with the given MoveNotificationInfo generated by the vehicle during the last time step.
     *
     * @param[in/out] vi VehicleInfo corresponding to the notifying vehicle
     * @param[in] mni MoveNotification for the vehicle
     */
    void integrateMoveNotification(VehicleInfo& vi, const MoveNotificationInfo& mni);

    /** @brief Creates and returns a MoveNotificationInfo containing detector specific information on the vehicle's last movement
     *
     * @param veh The vehicle sending the notification
     * @param oldPos The vehicle's position before the last integration step
     * @param newPos The vehicle's position after the last integration step
     * @param newSpeed The vehicle's speed after the last integration step
     * @param vehInfo Info on the detector's memory of the vehicle
     * @return A MoveNotificationInfo containing quantities of interest for the detector
     */
    MoveNotificationInfo makeMoveNotification(const SUMOVehicle& veh, SUMOReal oldPos, SUMOReal newPos, SUMOReal newSpeed, const VehicleInfo& vehInfo) const;

    /** @brief Creates and returns a VehicleInfo (called at the vehicle's entry)
     *
     * @param veh The entering vehicle
     * @param enteredLane The entry lane
     * @return A vehicle info which can be used to store information about the vehicle's stay on the detector
     */
    VehicleInfo makeVehicleInfo(const SUMOVehicle& veh, const MSLane* enteredLane) const;

    /** @brief Calculates the time loss for a segment with constant vmax
     *
     * @param timespan time needed to cover the segment
     * @param initialSpeed speed at segment entry
     * @param accel constant acceleration assumed during movement on the segment
     * @param vmax Maximal possible speed for the considered vehicle on the segment
     * @return Time loss (== MAX(timespan*(vmax - (initialSpeed + accel/2))/vmax), 0)
     */
    static SUMOReal calculateSegmentTimeLoss(SUMOReal timespan, SUMOReal initialSpeed, SUMOReal accel, SUMOReal vmax);


    /** @brief Resets the accumulated detector values after flushing them to the xml-file
     */
    virtual void reset();

    /** brief returns true if the vehicle corresponding to mni1 is closer to the detector end than the vehicle corresponding to mni2
     */
    static bool compareMoveNotification(MoveNotificationInfo& mni1, MoveNotificationInfo& mni2){
        return mni1.distToDetectorEnd < mni2.distToDetectorEnd;
    }


private:

    /// @brief Information about how this detector is used
    DetectorUsage myUsage;

    /// @name Detector parameter
    /// @{
    /// @brief The detector's lane sequence
    std::vector<std::string> myLanes;
    /// @brief The distances of the lane-beginnings from the detector start-point
    std::vector<SUMOReal> myOffsets;
    /// @brief The first lane of the detector's lane sequence
    MSLane* myFirstLane;
    /// @brief The last lane of the detector's lane sequence
    MSLane* myLastLane;
    /// @brief The position the detector starts at on the first lane
    SUMOReal myStartPos;
    /// @brief The position the detector ends at on the last lane
    SUMOReal myEndPos;
    /// @brief The total detector length
    SUMOReal myDetectorLength;

    /// @brief A vehicle must driver slower than this to be counted as a part of a jam
    SUMOReal myJamHaltingSpeedThreshold;
    /// @brief A vehicle must be that long beyond myJamHaltingSpeedThreshold to be counted as a part of a jam
    SUMOTime myJamHaltingTimeThreshold;
    /// @brief Two standing vehicles must be closer than this to be counted into the same jam
    SUMOReal myJamDistanceThreshold;
    /// @}


    /// @name Container
    /// @{
    /// @brief List of informations about the vehicles currently on the detector
    VehicleInfoMap myVehicleInfos;

    /// @brief Temporal storage for notifications from vehicles that did call the
    ///        detector's notifyMove() in the last time step.
    std::vector<MoveNotificationInfo> myMoveNotifications;

    /// @brief Keep track of vehicles that left the detector and should be removed from myVehicleInfos
    std::set<std::string> myLeftVehicles;

    /// @brief Storage for halting durations of known vehicles (for halting vehicles)
    std::map<std::string, SUMOTime> myHaltingVehicleDurations;

    /// @brief Storage for halting durations of known vehicles (current interval)
    std::map<std::string, SUMOTime> myIntervalHaltingVehicleDurations;

    /// @brief Halting durations of ended halts [s]
    std::vector<SUMOTime> myPastStandingDurations;

    /// @brief Halting durations of ended halts for the current interval [s]
    std::vector<SUMOTime> myPastIntervalStandingDurations;
    /// @}



    /// @name Values generated for aggregated file output
    /// @{
    /// @brief The number of collected samples [time x vehicle] since the last reset
    SUMOReal myVehicleSamples;
    /// @brief The total amount of all time losses [time x vehicle] since the last reset
    SUMOReal myTotalTimeLoss;
    /// @brief The number of vehicles, which have entered the detector since the last reset
    std::size_t myNumberOfEnteredVehicles;
    /// @brief The sum of collected vehicle speeds [m/s]
    SUMOReal mySpeedSum;
    /// @brief The number of started halts [#]
    SUMOReal myStartedHalts;
    /// @brief The sum of jam lengths [m]
    SUMOReal myJamLengthInMetersSum;
    /// @brief The sum of jam lengths [#veh]
    int myJamLengthInVehiclesSum;
    /// @brief The current aggregation duration [#steps]
    int myTimeSamples;
    /// @brief The sum of occupancies [%]
    SUMOReal myOccupancySum;
    /// @brief The maximum occupancy [%]
    SUMOReal myMaxOccupancy;
    /// @brief The mean jam length [#veh]
    int myMeanMaxJamInVehicles;
    /// @brief The mean jam length [m]
    SUMOReal myMeanMaxJamInMeters;
    /// @brief The max jam length [#veh]
    int myMaxJamInVehicles;
    /// @brief The max jam length [m]
    SUMOReal myMaxJamInMeters;
    /// @brief The mean number of vehicles [#veh]
    int myMeanVehicleNumber;
    /// @}


    /// @name Values generated describing the current state
    /// @{
    /// @brief The number of vehicles, which have left the detector since the last reset
    std::size_t myNumberOfLeftVehicles;
    /// @brief The maximal number of vehicles located on the detector simultaneously since the last reset
    std::size_t myMaxVehicleNumber;

    /// @brief The current occupancy
    SUMOReal myCurrentOccupancy;
    /// @brief The current mean speed
    SUMOReal myCurrentMeanSpeed;
    /// @brief The current mean length
    SUMOReal myCurrentMeanLength;
    /// @brief The current jam number
    int myCurrentJamNo;
    /// @brief the current maximum jam length in meters
    SUMOReal myCurrentMaxJamLengthInMeters;
    /// @brief The current maximum jam length in vehicles
    int myCurrentMaxJamLengthInVehicles;
    /// @brief The overall jam length in meters
    SUMOReal myCurrentJamLengthInMeters;
    /// @brief The overall jam length in vehicles
    int myCurrentJamLengthInVehicles;
    /// @brief The number of started halts in the last step
    int myCurrentStartedHalts;
    /// @brief The number of halted vehicles [#]
    int myCurrentHaltingsNumber;
    /// @}


private:
    /// @brief Invalidated copy constructor.
    MSMultiLaneE2Collector(const MSMultiLaneE2Collector&);

    /// @brief Invalidated assignment operator.
    MSMultiLaneE2Collector& operator=(const MSMultiLaneE2Collector&);
};


#endif

/****************************************************************************/

