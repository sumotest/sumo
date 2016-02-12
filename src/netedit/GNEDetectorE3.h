/****************************************************************************/
/// @file    GNEDetectorE3.h
/// @author  Pablo Alvarez Lopez
/// @date    Nov 2015
/// @version $Id: GNEDetectorE3.h 19790 2016-01-25 11:59:12Z palcraft $
///
/// 
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
#ifndef GNEDetectorE3_h
#define GNEDetectorE3_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include "GNEAdditionalSet.h"

// ===========================================================================
// class declarations
// ===========================================================================
class GNEDetectorE3EntryExit;
class GNELane;

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GNEDetectorE3
 * ------------
 */
class GNEDetectorE3 {
public:
    /** @brief GNEDetectorE3 Constructor
     * @param[in] id The storage of gl-ids to get the one for this lane representation from
     * @param[in] viewNet pointer to GNEViewNet of this additional element belongs
     * @param[in] freq the aggregation period the values the detector collects shall be summed up.
     * @param[in] filename The path to the output file
     * @exception InvalidArgument If the detector can not be added to the net (is duplicate)
     */
    GNEDetectorE3(const std::string& id, GNEViewNet* viewNet, SUMOReal freq, const std::string& filename);

    /// @brief GNEDetectorE3 6Destructor
    ~GNEDetectorE3();

    /** @brief add a entry
     * @param[in] lane Lane of this StoppingPlace belongs
     * @param[in] pos position of the detector on the lane
     **/
    void addEntry(GNELane& lane, SUMOReal pos);

    /** @brief remove a entry
     * @param[in] lane Lane of this StoppingPlace belongs
     * @param[in] pos position of the detector on the lane
     **/
    void removeEntry(GNELane& lane, SUMOReal pos);

    /** @brief add an entry
     * @param[in] lane Lane of this StoppingPlace belongs
     * @param[in] pos position of the detector on the lane
     **/
    void addExit(GNELane& lane, SUMOReal pos);

    /** @brief remove an entry
     * @param[in] lane Lane of this StoppingPlace belongs
     * @param[in] pos position of the detector on the lane
     **/
    void removeExit(GNELane& lane, SUMOReal pos);

protected:

    /// @brief ID of E3 detector
    std::string myId;

    /// @brief pointer to viewNet
    GNEViewNet* myViewNet;
    
    /// @brief frequency of E3 detector
    SUMOReal myFreq;

    /// @brief fielname of E3 detector
    std::string myFilename;

    /// @brief counter for Id's of GNEDetectorE3EntryExit
    int myCounterId;

    /// @brief vector with the GNEDetectorE3EntryExits of the detector
    std::vector<GNEDetectorE3EntryExit*> myGNEDetectorE3EntryExits;

    /// @brief variable to save detectorE2 icon
    static GUIGlID detectorE3GlID;

    /// @brief check if detectorE2 icon was inicilalizated
    static bool detectorE3Initialized;

    /// @brief Invalidated copy constructor.
    GNEDetectorE3(const GNEDetectorE3&);

    /// @brief Invalidated assignment operator.
    GNEDetectorE3& operator=(const GNEDetectorE3&);
};

#endif
/****************************************************************************/