/****************************************************************************/
/// @file    GNEChange_BusStop.h
/// @author  Pablo Alvarez Lopez
/// @date    Nov 2015
/// @version $Id: $
///
/// A network change in which a busStop is created or deleted
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
#ifndef GNEChange_BusStop_h
#define GNEChange_BusStop_h

// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <fx.h>
#include <utils/foxtools/fxexdefs.h>
#include <netbuild/NBEdge.h>
#include "GNEChange.h"

// ===========================================================================
// class declarations
// ===========================================================================
class GNEBusStop;

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GNEChange_BusStop
 * A network change in which a single BusStop is created or deleted
 */
class GNEChange_BusStop : public GNEChange {
    FXDECLARE_ABSTRACT(GNEChange_BusStop)

public:
    /** @brief Constructor for creating/deleting an edge
     * @param[in] net The net on which to apply changes
     * @param[in] busStop The busStop to be created/deleted
     * @param[in] forward Whether to create/delete (true/false)
     */
    GNEChange_BusStop(GNENet* net, GNEBusStop* busStop, bool forward);

    /// @brief Destructor
    ~GNEChange_BusStop();

    FXString undoName() const;
    FXString redoName() const;
    void undo();
    void redo();


private:
    /** @brief full information regarding the busStop that is to be created/deleted
     * we assume shared responsibility for the pointer (via reference counting)
     */
    GNEBusStop* myBusStop;
};

#endif
/****************************************************************************/