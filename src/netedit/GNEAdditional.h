/****************************************************************************/
/// @file    GNEAdditional.h
/// @author  Pablo Alvarez Lopez
/// @date    Dec 2015
/// @version $Id: $
///
/// The Widget for setting default parameters of additional elements
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
#ifndef GNEADDITIONAL_H
#define GNEADDITIONAL_H


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <fx.h>
#include <GL/gl.h>
#include <utils/foxtools/FXRealSpinDial.h>
#include <utils/xml/SUMOXMLDefinitions.h>
#include <utils/gui/div/GUISelectedStorage.h>

// ===========================================================================
// class declarations
// ===========================================================================
class GNEViewNet;
class GNEAttributeCarrier;
class GNEUndoList;

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GNEAdditional
 * The Widget for setting default parameters of additional elements
 */
class GNEAdditional : public FXScrollWindow {
    // FOX-declarations
    FXDECLARE(GNEAdditional)

public:

    /// @brief list of additional types
    enum additionalType {
        GNE_ADDITIONAL_BUSSTOP,
        GNE_ADDITIONAL_CHARGINGSTATION,
        GNE_ADDITIONAL_E1,                  // NOT YET IMPLEMENTED
        GNE_ADDITIONAL_E2,                  // NOT YET IMPLEMENTED
        GNE_ADDITIONAL_E3,                  // NOT YET IMPLEMENTED
        GNE_ADDITIONAL_REROUTERS,           // NOT YET IMPLEMENTED
        GNE_ADDITIONAL_CALIBRATORS,         // NOT YET IMPLEMENTED
        GNE_ADDITIONAL_VARIABLESPEEDSIGNS   // NOT YET IMPLEMENTED
    };

    /// @brief list of the reference points
    enum additionalReferencePoint {
        GNE_ADDITIONALREFERENCEPOINT_LEFT,
        GNE_ADDITIONALREFERENCEPOINT_RIGHT,
        GNE_ADDITIONALREFERENCEPOINT_CENTER
    };

    /** @brief Constructor
     * @param[in] parent The parent window
     */
    GNEAdditional(FXComposite* parent, GNEViewNet* updateTarget, GNEUndoList* undoList);


    /// @brief Destructor
    ~GNEAdditional();


    FXFont* getHeaderFont() {
        return myHeaderFont;
    }

    /// @name FOX-callbacks
    /// @{
    /** @brief Called when the user enters a new selection expression
     * validates expression and modifies current selection
     */
    long onCmdSelectAdditional(FXObject*, FXSelector, void*);

    /** @brief 
     * 
     */
    long onCmdSelectReferencePoint(FXObject*, FXSelector, void*);
    /// @}

    void show();

    void hide();

protected:
    /// @brief FOX needs this
    GNEAdditional() {}

private:

    /// @brief set parameters depending of the additionalType selected
    void setParameters();

    /// @brief struct for text field parameters             // QUESTION ERDMANN 01
    struct additionalParameterTextField {
        FXHorizontalFrame *horizontalFrame;
        FXLabel *label;
        FXTextField *textField;
    };

    /// @brief struct for boolean (menuCheck) parameters    // QUESTION ERDMANN 02
    struct additionalParameterCheckButton {
        FXHorizontalFrame *horizontalFrame;
        FXLabel *label;
        FXMenuCheck *menuCheck;
    };

    /// @brief the panel to hold all member widgets
    FXVerticalFrame* myContentFrame;

    /// @brief Font for the widget
    FXFont* myHeaderFont;

    /// @brief the label for the frame
    FXLabel* myFrameLabel;

    /// @brief combo box with the list of additional elements
    FXComboBox* myAdditionalMatchBox;

    /// @brief the window to inform 
    GNEViewNet* myUpdateTarget;

    /// @brief vector of Labels for the name of default parameters                  // QUESTION ERDMANN 03
    std::vector<additionalParameterTextField> myVectorOfParametersTextFields;

    /// @brief vector of Text fiels with the default text parameters                // QUESTION ERDMANN 04
    std::vector<additionalParameterCheckButton> myVectorOfParameterCheckButton;

    /// @brief the box for the reference point match Box
    FXGroupBox* myReferencePointBox;

    /// @brief match box with the list of reference points
    FXComboBox* myReferencePointMatchBox;

    /// @brief actual additional type selected in the match Box
    additionalType myActualAdditionalType;

    /// @brief actual additional reference point selected in the match Box
    additionalReferencePoint myActualAdditionalReferencePoint;

    /// @brief Width of frame
    static const int WIDTH;

    /// @brief Maximun number (Size of vector) of additionalParameterTextField
    static const int maximumNumberOfAdditionalParameterTextField;

    /// @brief Maximun number (Size of vector) of additionalParameterCheckButton
    static const int maximumNumberOfAdditionalParameterCheckButton;

    // @brief undo 
    GNEUndoList* myUndoList;
};


#endif

/****************************************************************************/