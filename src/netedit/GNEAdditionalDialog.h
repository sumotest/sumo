/****************************************************************************/
/// @file    GNEAdditionalDialog.h
/// @author  Pablo Alvarez Lopez
/// @date    April 2016
/// @version $Id: GNEAdditionalDialog.h 20472 2016-04-15 15:36:45Z palcraft $
///
/// A abstract class for editing additional elements
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
#ifndef GNEAdditionalDialog_h
#define GNEAdditionalDialog_h

// ===========================================================================
// included modules
// ===========================================================================

#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <fx.h>


// ===========================================================================
// class declarations
// ===========================================================================


// ===========================================================================
// class definitions
// ===========================================================================

/**
 * @class GNEAdditionalDialog
 * @brief Dialog to edit sequences, parameters, etc.. of Additionals
 */

class GNEAdditionalDialog : public FXDialogBox {
public:
    // Constructor
    GNEAdditionalDialog(FXWindow* parent,const FXString& name);

    // destructor
    ~GNEAdditionalDialog();

    // open Dialog
    void openAdditionalDialog();

    // close Dialog
    void closeAdditionalDialog();

private:
};

#endif
