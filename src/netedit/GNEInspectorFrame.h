/****************************************************************************/
/// @file    GNEInspectorFrame.h
/// @author  Jakob Erdmann
/// @date    Mar 2011
/// @version $Id: GNEInspectorFrame.h 20785 2016-05-24 10:51:45Z palcraft $
///
// The Widget for modifying network-element attributes (i.e. lane speed)
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
#ifndef GNEInspectorFrame_h
#define GNEInspectorFrame_h


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
class GNENet;
class GNEEdge;
class GNEAttributeCarrier;
class GNEAdditional;

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GNEInspectorFrame
 * The Widget for modifying network-element attributes (i.e. lane speed)
 */
class GNEInspectorFrame : public GNEFrame {
    /// @brief FOX-declaration
    FXDECLARE(GNEInspectorFrame)

public:
    // ===========================================================================
    // class AttrPanel
    // ===========================================================================
    class AttrPanel : public FXVerticalFrame {
        /// @brief FOX-declaration
        FXDECLARE(GNEInspectorFrame::AttrPanel)

    public:
        /// @brief constructor
        AttrPanel(GNEInspectorFrame* parent, const std::vector<GNEAttributeCarrier*>& ACs);

        /// @brief try to set new attribute value
        long onCmdSetBlocking(FXObject*, FXSelector, void*);

    protected:
        /// @brief FOX needs this
        AttrPanel() {}

    private:
        /// @brief pointer to check button block
        FXCheckButton *myCheckBlocked;

        /// @brief pointer to additional element
        GNEAdditional *myAdditional;
    };

    // ===========================================================================
    // class AttrInput
    // ===========================================================================
    class AttrInput : public FXHorizontalFrame {
        /// @brief FOX-declaration
        FXDECLARE(GNEInspectorFrame::AttrInput)

    public:
        /// @brief constructor
        AttrInput(FXComposite* parent, GNEInspectorFrame *inspectorFrameParent, const std::vector<GNEAttributeCarrier*>& ACs, SumoXMLAttr attr, std::string initialValue);

        /// @brief try to set new attribute value
        long onCmdSetAttribute(FXObject*, FXSelector, void*);

        /// @brief open model dialog for more comfortable attribute editing
        long onCmdOpenAttributeEditor(FXObject*, FXSelector, void*);

    protected:
        /// @brief FOX needs this
        AttrInput() {}

    private:
        /// @brief pointer to GNEInspectorFrame parent
        GNEInspectorFrame *myInspectorFrameParent;

        /// @brief actual tag
        SumoXMLTag myTag;

        /// @brief attributos
        SumoXMLAttr myAttr;

        /// @brief attribute carriers
        const std::vector<GNEAttributeCarrier*>* myACs;

        /// @brief pointer to text field
        FXTextField* myTextField;

        /// @brief pointer to combo box choices
        FXComboBox* myChoicesCombo;
    };

public:
    /**@brief Constructor
     * @brief parent FXFrame in which this GNEFrame is placed
     * @brief viewNet viewNet that uses this GNEFrame
     */
    GNEInspectorFrame(FXComposite* parent, GNEViewNet* viewNet);

    /// @brief Destructor
    ~GNEInspectorFrame();

    /// @brief show Frame
    void show();

    /// @brief hide Frame
    void hide();

    /// @brief Inspect the given multi-selection
    void inspect(const std::vector<GNEAttributeCarrier*>& ACs);

    /// @brief Creates the widget
    void create();

    /// @brief update the widget
    void update();

    /// @brief get Header Font
    FXFont* getHeaderFont() const;

    /// @brief get the template edge (to copy attributes from)
    GNEEdge* getEdgeTemplate() const;

    /// @brief seh the template edge (we assume shared responsibility via reference counting)
    void setEdgeTemplate(GNEEdge* tpl);

    /// @brief copy edge attributes from edge template
    long onCmdCopyTemplate(FXObject*, FXSelector, void*);

    /// @brief set current edge as new template
    long onCmdSetTemplate(FXObject*, FXSelector, void*);

    /// @brief update the copy button with the name of the template
    long onUpdCopyTemplate(FXObject*, FXSelector, void*);

protected:
    /// @brief FOX needs this
    GNEInspectorFrame() {}

private:
    /// @brief Font for the widget
    FXFont* myHeaderFont;

    /// @brief panel of attributes
    AttrPanel* myPanel;

    /// @brief the edge template
    GNEEdge* myEdgeTemplate;

    /// @brief the multi-selection currently being inspected
    std::vector<GNEAttributeCarrier*> myACs;
};


#endif

/****************************************************************************/

