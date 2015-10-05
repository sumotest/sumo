/****************************************************************************/
/// @file    MSLeaderInfo.h
/// @author  Jakob Erdmann
/// @date    Oct 2015
/// @version $Id: MSLeaderInfo.h 18095 2015-03-17 09:39:00Z behrisch $
///
// Information about vehicles ahead (may be multiple vehicles if
// lateral-resolution is active)
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
#ifndef MSLeaderInfo_h
#define MSLeaderInfo_h


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


// ===========================================================================
// class declarations
// ===========================================================================
class MSVehicle;
class MSLane;


// ===========================================================================
// types definitions
// ===========================================================================

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class MSLeaderInfo
 */
class MSLeaderInfo {
public:
    /// Constructor
    MSLeaderInfo(SUMOReal width);

    /// Destructor
    virtual ~MSLeaderInfo();

    /* @brief adds this vehicle as a leader in the appropriate sublanes
     * @param[in] veh The vehicle to add
     * @param[in] beyond Whether the vehicle is beyond the existing leaders (and thus may be shadowed by them)
     * @return The number of free sublanes
     */
    int addLeader(const MSVehicle* veh, bool beyond);

    /* @brief adds this vehicle as a leader in the appropriate sublanes
     * @param[in] veh The vehicle to check
     * @param[out] rightmost The rightmost sublane occupied by veh 
     * @param[out] leftmost The rightmost sublane occupied by veh 
     */
    void getSubLanes(const MSVehicle* veh, int& rightmost, int& leftmost) const;

    /// @brief return the vehicle for the given sublane
    const MSVehicle* operator[](int sublane) const;

    const int numFreeSublanes() const {
        return myFreeSublanes;
    }
private:

    /// @brief the width of the lane to which this instance applies
    // @note: not const to simplify assignment
    SUMOReal myWidth;

    std::vector<const MSVehicle*> myVehicles;

    /// @brief the number of free sublanes
    int myFreeSublanes;

};


#endif

/****************************************************************************/

