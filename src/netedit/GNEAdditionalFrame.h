/****************************************************************************/
/// @file    GNEAdditionalFrame.h
/// @author  Pablo Alvarez Lopez
/// @date    Dec 2015
/// @version $Id$
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
#ifndef GNEAdditionalFrame_h
#define GNEAdditionalFrame_h


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
#include <utils/xml/SUMOSAXHandler.h>

// ===========================================================================
// class declarations
// ===========================================================================
class GNEViewNet;
class GNEAttributeCarrier;
class GNEUndoList;
class GNELane;
class GNEAdditional;

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GNEAdditionalFrame
 * The Widget for setting default parameters of additional elements
 */
class GNEAdditionalFrame : public FXScrollWindow {
    // FOX-declaration
    FXDECLARE(GNEAdditionalFrame)

public:

    /// @brief list of additional types
    enum additionalType {
        GNE_ADDITIONAL_BUSSTOP,
        GNE_ADDITIONAL_CHARGINGSTATION,
        GNE_ADDITIONAL_E1,
        GNE_ADDITIONAL_E2,
        GNE_ADDITIONAL_E3,
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

    // ===========================================================================
    // class additionalParameter
    // ===========================================================================

    class additionalParameter : public FXMatrix{

    public:
        /// @brief constructor
        additionalParameter(FXComposite *parent, FXObject* tgt);

        /// @brief destructor
        ~additionalParameter();

        /// @brief show name and value of parameters of type textField
        void showTextParameter(const std::string& name, const std::string &value);

        /// @brief show name and value of parameters of type bool
        void showBoolParameter(const std::string& name, bool value);

        /// @brief hide all parameters
        void hideParameter();

        /// @brief return the value of the FXTextField
        std::string getTextValue();

        /// @brief return the value of the FXMenuCheck
        bool getBoolValue();

    private:        
        /// @brief lael with the name of the parameter
        FXLabel *myLabel;
        
        /// @brief textField to modify the value of parameter
        FXTextField *myTextField;
        
        /// @brief menuCheck to enable/disable the value of parameter
        FXMenuCheck *myMenuCheck;
    };

    // ===========================================================================
    // class additionalParameterList
    // ===========================================================================

    class additionalParameterList : public FXMatrix {
        // FOX-declaration
        FXDECLARE(GNEAdditionalFrame::additionalParameterList)

    public:
        /// @brief constructor
        additionalParameterList(FXComposite *parent, FXObject* tgt, int maxNumberOfValuesInParameterList = 10);

        /// @brief destructor
        ~additionalParameterList();

        /// @brief show name and value of parameters of type textField
        void showListParameter(const std::string& name, std::vector<std::string> value);

        /// @brief hide all parameters
        void hideParameter();

        /// @brief return the value of the FXTextField 
        std::vector<std::string> getVectorOfTextValues();

        /// @name FOX-callbacks
        /// @{
        /// @brief add a new row int the list
        long onCmdAddRow(FXObject*, FXSelector, void*);

        /// @brief add a new row int the list
        long onCmdRemoveRow(FXObject*, FXSelector, void*);
        /// @}

    protected:
        /// @brief FOX needs this
        additionalParameterList() {}

    private:        
        /// @brief vector with with the name of every parameter
        std::vector<FXLabel*> myLabels;
        
        /// @brief vector textField to modify the value of parameter
        std::vector<FXTextField*> myTextFields;

        /// @brief Button to increase the number of textFields
        FXButton *add;

        /// @brief Button to decrease the number of textFields
        FXButton *remove;

        /// @brief number of visible text fields
        int numberOfVisibleTextfields;

        /// @brief Number max of values in a parameter of type list
        int myMaxNumberOfValuesInParameterList;
    };

    // ===========================================================================
    // class editorParameter
    // ===========================================================================

    class editorParameter : public FXGroupBox {
        // FOX-declaration
        FXDECLARE(GNEAdditionalFrame::editorParameter)

    public:
        /// @brief constructor
        editorParameter(FXComposite *parent, FXObject* tgt);

        /// @brief destructor
        ~editorParameter();

        /// @brief get actual reference point
        additionalReferencePoint getActualReferencePoint();

        /// @brief check if block is enabled
        bool isBlockEnabled();

        /// @brief check if force position is enabled
        bool isForcePositionEnabled();

        /// @name FOX-callbacks
        /// @{
        /// @brief Called when the user enters another reference point
        long onCmdSelectReferencePoint(FXObject*, FXSelector, void*);
        /// @}

    protected:
        /// @brief FOX needs this
        editorParameter() {}

    private:
        /// @brief the box for the reference point match Box
        FXGroupBox* myReferencePointBox;

        /// @brief match box with the list of reference points
        FXComboBox* myReferencePointMatchBox;

        /// @brief actual additional reference point selected in the match Box
        additionalReferencePoint myActualAdditionalReferencePoint;

        /// @brief checkBox for the option "force position"
        FXMenuCheck* myCheckForcePosition;

        /// @brief checkBox for blocking movement
        FXMenuCheck* myCheckBlock;
    };

    // ===========================================================================
    // class additionalSet
    // ===========================================================================

    class additionalSet : public FXGroupBox {
        // FOX-declaration
        FXDECLARE(GNEAdditionalFrame::additionalSet)

    public:
        /// @brief constructor
        additionalSet(FXComposite *parent, FXObject* tgt);

        /// @brief destructor
        ~additionalSet();

        /// @name FOX-callbacks
        /// @{
        /// @brief add a new row int the list
        long onCmdAddSet(FXObject*, FXSelector, void*);

        /// @brief add a new row int the list
        long onCmdRemoveSet(FXObject*, FXSelector, void*);
        /// @}

    protected:
        /// @brief FOX needs this
        additionalSet() {}

    private:        
        /// @brief 
        FXList *myList;
        
        /// @brief textField to modify the value of parameter
        std::vector<FXTextField*> myTextFields;

        /// @brief Button to increase the number of textFields
        FXButton *addSet;

        /// @brief Button to decrease the number of textFields
        FXButton *removeSet;
    };

    /** @brief Constructor
     * @param[in] parent The parent window
     * @param[in] updateTarget view Net of the netEdit
     * @param[in] undoList pointer to undoList
     */
    GNEAdditionalFrame(FXComposite* parent, GNEViewNet* updateTarget, GNEUndoList* undoList);

    /// @brief Destructor
    ~GNEAdditionalFrame();

    /** @brief add additional element
     * @param[in] lane lane in which the new element will be inserted
     * @param[in] parent AbstractView to obtain the position of the mouse over the lane
     */
    void addAdditional(GNELane &lane, GUISUMOAbstractView* parent);

    /** @brief remove an additional element previously added
     * @param[in] additional element to erase
     */
    void removeAdditional(GNEAdditional *additional);

    ///@brief return font of the header
    FXFont* getHeaderFont();

    /// @name FOX-callbacks
    /// @{
    /// @brief Called when the user select another additional Type
    long onCmdSelectAdditional(FXObject*, FXSelector, void*);
    /// @}

    /// @brief show additional frame
    void show();

    /// @brief hidde additional frame
    void hide();

protected:
    /// @brief FOX needs this
    GNEAdditionalFrame() {}

private:

    /// @brief set parameters depending of the additionalType selected
    void setParameters();

    /// @brief obtain the position values of busStop and chargingStation over the lane (return false if isn't possible)
    bool setPositions(GNELane &lane, SUMOReal &positionOfTheMouseOverLane, SUMOReal &startPosition, SUMOReal &endPosition);

    /// @brief the panel to hold all member widgets
    FXVerticalFrame* myContentFrame;

    /// @brief Font for the widget
    FXFont* myHeaderFont;

    /// @brief the label for the frame
    FXLabel* myFrameLabel;

    /// @brief groupBox for Match Box of additionals
    FXGroupBox* groupBoxForMyAdditionalMatchBox;
    
    /// @brief combo box with the list of additional elements
    FXComboBox* myAdditionalMatchBox;
    
    /// @briefgroupBox for parameters 
    FXGroupBox* groupBoxForParameters;

    /// @brief vector with the additional parameters
    std::vector<additionalParameter*> myVectorOfAdditionalParameter;
    
    /// @brief vector with the additional parameters of type list
    std::vector<additionalParameterList*> myVectorOfadditionalParameterList;

    /// @brief editor parameter 
    GNEAdditionalFrame::editorParameter *myEditorParameter;

    /// @brief additional Set
    GNEAdditionalFrame::additionalSet *myAdditionalSet;

    /// @brief the window to inform 
    GNEViewNet* myUpdateTarget;

    /// @brief actual additional type selected in the match Box
    additionalType myActualAdditionalType;

    /// @brief Width of frame
    static const int WIDTH;

    /// @brief Number max of parameters
    static const int maxNumberOfParameters;

    /// @brief Number max of parameters of type list
    static const int maxNumberOfListParameters;

    /// @brief undo 
    GNEUndoList* myUndoList;
};


#endif

/****************************************************************************/
