/****************************************************************************/
/// @file    MSLaneChangerSublane.cpp
/// @author  Jakob Erdmann
/// @date    Oct 2015
/// @version $Id: MSLaneChangerSublane.cpp 19043 2015-10-07 13:56:04Z namdre $
///
// Performs sub-lane changing of vehicles
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2002-2015 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef MSLaneChangerSublane_h
#define MSLaneChangerSublane_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include "MSLaneChanger.h"


// ===========================================================================
// class declarations
// ===========================================================================


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class MSLaneChangerSublane
 * @brief Performs lane changing of vehicles
 */
class MSLaneChangerSublane : public MSLaneChanger {
public:
    /// Constructor
    MSLaneChangerSublane(const std::vector<MSLane*>* lanes, bool allowSwap);

    /// Destructor.
    ~MSLaneChangerSublane();

protected:

    /** Find a new candidate and try to change it. */
    bool change();

    /** @brief check whether sub-lane changing in the given direction is desirable
     * and possible
     * @param[in] laneOffset The direction in which changing should be checked
     * @param[in] leaders The candidate vehicle's leaders
     * @param[in] preb The bestLanse of the candidaet vehicle
     * @param[out] latDist The distance by which the vehicle changes laterally
     */
    int checkChange(
        int laneOffset,
        const std::pair<MSVehicle* const, SUMOReal>& leader,
        const std::vector<MSVehicle::LaneQ>& preb,
        SUMOReal& latDist) const;

    ///  @brief change by the specified amount
    void startChange(MSVehicle* vehicle, ChangerIt& from, SUMOReal latDist);


private:
    /// Default constructor.
    MSLaneChangerSublane();

    /// Copy constructor.
    MSLaneChangerSublane(const MSLaneChangerSublane&);

    /// Assignment operator.
    MSLaneChangerSublane& operator=(const MSLaneChangerSublane&);
};


#endif

/****************************************************************************/

