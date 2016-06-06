/****************************************************************************/
/// @file    GNEViewParent.h
/// @author  Jakob Erdmann
/// @date    Feb 2011
/// @version $Id$
///
// A single child window which contains a view of the edited network (adapted
// from GUISUMOViewParent)
// While we don't actually need MDI for netedit it is easier to adapt existing
// structures than to write everything from scratch.
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2016 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef GNEViewParent_h
#define GNEViewParent_h


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
#include <fx.h>
#include <utils/geom/Position.h>
#include <utils/geom/Boundary.h>
#include <utils/gui/globjects/GUIGlObjectTypes.h>
#include <utils/gui/windows/GUIGlChildWindow.h>

// ===========================================================================
// class declarations
// ===========================================================================
class GUISUMOAbstractView;
class GNENet;
class GNEApplicationWindow;
class GNEInspectorFrame;
class GNESelectorFrame;
class GNEConnectorFrame;
class GNETLSEditorFrame;
class GNEAdditionalFrame;

// ===========================================================================
// class declarations
// ===========================================================================
/**
 * @class GNEViewParent
 * @brief A single child window which contains a view of the simulation area
 *
 * It is made of a tool-bar containing a field to change the type of display,
 *  buttons that allow to choose an artifact and some other view controlling
 *  options.
 *
 * The rest of the window is a canvas that contains the display itself
 */
class GNEViewParent : public GUIGlChildWindow {
    // FOX-declarations
    FXDECLARE(GNEViewParent)

public:
    /**@brief Constructor
     * also builds the view and calls create()
     *
     * @param[in] p The MDI-pane this window is shown within
     * @param[in] mdimenu The MDI-menu for alignment
     * @param[in] name The name of the window
     * @param[in] parentWindow The main window
     * @param[in] ic The icon of this window
     * @param[in] opts Window options
     * @param[in] x Initial x-position
     * @param[in] y Initial x-position
     * @param[in] w Initial width
     * @param[in] h Initial height
     * @param[in] share A canvas tor get the shared context from
     * @param[in] net The network to show
     */
    GNEViewParent(FXMDIClient* p, FXMDIMenu* mdimenu,
                  const FXString& name, GNEApplicationWindow* parentWindow,
                  FXGLCanvas* share, GNENet* net,
                  FXIcon* ic = NULL, FXuint opts = 0, FXint x = 0, FXint y = 0, FXint w = 0, FXint h = 0);

    /// @brief Destructor
    ~GNEViewParent();

    /// @brief get frame for GNE_MODE_INSPECT       // PABLO #2036
    GNEInspectorFrame* getInspectorFrame() const;   // PABLO #2036

    /// @brief get frame for GNE_MODE_SELECT        // PABLO #2036
    GNESelectorFrame* getSelectorFrame() const;     // PABLO #2036

    /// @brief get frame for GNE_MODE_CONNECT       // PABLO #2036
    GNEConnectorFrame* getConnectorFrame() const;   // PABLO #2036

    /// @brief get frame for GNE_MODE_TLS           // PABLO #2036
    GNETLSEditorFrame* getTLSEditorFrame() const;   // PABLO #2036

    /// @brief get frame for GNE_MODE_ADDITIONAL    // PABLO #2036
    GNEAdditionalFrame* getAdditionalFrame() const; // PABLO #2036

    /// @name FOX-callbacks
    /// @{
    /// @brief Called if the user wants to make a snapshot (screenshot)
    long onCmdMakeSnapshot(FXObject* sender, FXSelector, void*);

    /// @brief Called when the user hits the close button (x)
    long onCmdClose(FXObject*, FXSelector, void*);

    /// @brief locator-callback
    long onCmdLocate(FXObject*, FXSelector, void*);

    /// @brief Called when user press a key
    long onKeyPress(FXObject* o, FXSelector sel, void* data);

    /// @brief Called when user releases a key
    long onKeyRelease(FXObject* o, FXSelector sel, void* data);
    /// @}

    /// @brief true if the object is selected (may include extra logic besides calling gSelected)
    bool isSelected(GUIGlObject* o) const;

protected:
    /// @brief FOX needs this
    GNEViewParent() {}

private:
    /// @brief frame to hold myView and myAttributePanel
    FXHorizontalFrame* myViewArea;

    /// @brief frame to hold GNEFrames      // PABLO #2036
    FXHorizontalFrame* myFramesArea;        // PABLO #2036

    /// @brief Splitter to divide ViewNet und GNEFrames     // PABLO #2036
    FXSplitter *myFramesSplitter;                           // PABLO #2036

    /// @brief the panel for GNE_MODE_INSPECT           // PABLO #2036
    GNEInspectorFrame* myInspectorFrame;                // PABLO #2036

    /// @brief the panel for GNE_MODE_SELECT            // PABLO #2036
    GNESelectorFrame* mySelectorFrame;                  // PABLO #2036

    /// @brief the panel for GNE_MODE_CONNECT           // PABLO #2036
    GNEConnectorFrame* myConnectorFrame;                // PABLO #2036

    /// @brief the panel for GNE_MODE_TLS               // PABLO #2036
    GNETLSEditorFrame* myTLSEditorFrame;                // PABLO #2036

    /// @brief the panel for GNE_MODE_ADDITIONAL        // PABLO #2036
    GNEAdditionalFrame* myAdditionalFrame;              // PABLO #2036
};


#endif

/****************************************************************************/
