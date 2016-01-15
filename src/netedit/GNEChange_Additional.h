/****************************************************************************/
/// @file    GNEChange_Additional.h
/// @author  Pablo Alvarez Lopez
/// @date    Nov 2015
/// @version $Id: $
///
/// A network change in which a additional element is created or deleted
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
#ifndef GNEChange_Additional_h
#define GNEChange_Additional_h

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
class GNEAdditional;
class GNEViewNet;

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GNEChange_Additional
 * A network change in which a single BusStop is created or deleted
 */
class GNEChange_Additional : public GNEChange {
    FXDECLARE_ABSTRACT(GNEChange_Additional)

public:
    /** @brief Constructor for creating/deleting an edge
     * @param[in] net The net on which to apply changes
     * @param[in] busStop The busStop to be created/deleted
     * @param[in] forward Whether to create/delete (true/false)
COMPLETAR
     */
    GNEChange_Additional(GNENet* net, GNEAdditional* additional, bool forward, GNEViewNet* viewNet);

    /// @brief Destructor
    ~GNEChange_Additional();

    FXString undoName() const;
    FXString redoName() const;
    void undo();
    void redo();


private:
    /// @brief full information regarding the additional element that is to be created/deleted
    GNEAdditional* myAdditional;

    GNEViewNet* myViewNet;
};

#endif
/****************************************************************************/