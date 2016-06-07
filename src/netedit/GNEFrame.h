/****************************************************************************/
/// @file    GNEFrame.h
/// @author  Pablo Alvarez Lopez
/// @date    Jun 2016
/// @version $Id: GNEFrame.h 20819 2016-05-30 15:06:19Z palcraft $
///
/// Abstract class for lateral frames in NetEdit
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
#ifndef GNEFrame_h
#define GNEFrame_h

// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <fx.h>
#include <list>
#include <netbuild/NBEdge.h>
#include <netbuild/NBTrafficLightLogic.h>
#include <utils/foxtools/FXRealSpinDial.h>
#include <utils/gui/div/GUISelectedStorage.h>
#include <utils/xml/SUMOSAXHandler.h>
#include <utils/xml/SUMOXMLDefinitions.h>

// ===========================================================================
// class declarations
// ===========================================================================
class GNEViewNet;
class GNEUndoList;

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GNEFrame
 * Abstract class for lateral frames in NetEdit
 */
class GNEFrame : public FXScrollWindow {
    /// @brief FOX-declaration
    //FXDECLARE(GNEFrame)

public:
    /**@brief Constructor
     * @brief parent FXFrame in which this GNEFrame is placed
     * @brief viewNet viewNet that uses this GNEFrame
     */
    GNEFrame(FXComposite* parent, GNEViewNet* viewNet);

    /// @brief destructor
    ~GNEFrame();

    /// @brief show Frame
    virtual void show() = 0;

    /// @brief hide Frame
    virtual void hide() = 0;

    /// @brief get view net
    GNEViewNet *getViewNet() const;

    /// @brief get width of frame
    int getFrameWidth() const;

protected:
    /// @brief FOX needs this
    GNEFrame() {}

    /// @brief @brief the window to inform when the tls is modfied
    GNEViewNet *myViewNet;

    /// @brief width of frame
    int myFrameWidth;

private:
    /// @brief default width
    static const int myDefaultWidth;

    /// @brief Invalidated copy constructor.
    GNEFrame(const GNEFrame&);

    /// @brief Invalidated assignment operator.
    GNEFrame& operator=(const GNEFrame&);
};


#endif

/****************************************************************************/
