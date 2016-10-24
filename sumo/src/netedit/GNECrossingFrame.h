/****************************************************************************/
/// @file    GNECrossingFrame.h
/// @author  Pablo Alvarez Lopez
/// @date    Oct 2016
/// @version $Id: GNECrossingFrame.h 21131 2016-07-08 07:59:22Z behrisch $
///
/// The Widget for add Crossing elements
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
#ifndef GNECrossingFrame_h
#define GNECrossingFrame_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include "GNEFrame.h"

// ===========================================================================
// class declarations
// ===========================================================================
class GNEAttributeCarrier;
class GNENetElement;
class GNECrossing;
class GNECrossingSet;

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GNECrossingFrame
 * The Widget for setting default parameters of Crossing elements
 */
class GNECrossingFrame : public GNEFrame {
    /// @brief FOX-declaration
    FXDECLARE(GNECrossingFrame)

public:

    /**@brief Constructor
     * @brief parent FXFrame in which this GNEFrame is placed
     * @brief viewNet viewNet that uses this GNEFrame
     */
    GNECrossingFrame(FXComposite* parent, GNEViewNet* viewNet);

    /// @brief Destructor
    ~GNECrossingFrame();

    /**@brief add Crossing element
     * @param[in] netElement clicked netElement. if user dind't clicked over a GNENetElement in view, netElement will be NULL
     * @return true if a GNECrossing was added, false in other case
     */
    bool addCrossing(GNENetElement* netElement);

    /**@brief remove an Crossing element previously added
     * @param[in] Crossing element to erase
     */
    void removeCrossing(GNECrossing* Crossing);

    /// @name FOX-callbacks
    /// @{
    /// @brief Called when the user select another Crossing Type
    long onCmdSelectCrossing(FXObject*, FXSelector, void*);
    /// @}

    /// @brief show Crossing frame
    void show();

    /// @brief hidde Crossing frame
    void hide();

    /// @brief get list of selecte id's in string format
    static std::string getIdsSelected(const FXList* list);

protected:
    /// @brief FOX needs this
    GNECrossingFrame() {}

private:

};


#endif

/****************************************************************************/
