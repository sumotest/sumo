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
class GNEAdditionalSet;

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

    // ===========================================================================
    // class additionalParameter
    // ===========================================================================

    class additionalParameter : public FXMatrix {

    public:
        /// @brief constructor
        additionalParameter(FXComposite *parent, FXObject* tgt);

        /// @brief destructor
        ~additionalParameter();

        /// @brief show name and value of attribute of type string
        void showParameter(SumoXMLAttr attr, std::string value);

        /// @brief show name and value of parameters of type int
        void showParameter(SumoXMLAttr attr, int value);

        /// @brief show name and value of parameters of type float/real
        void showParameter(SumoXMLAttr attr, SUMOReal value);

        /// @brief show name and value of parameters of type bool
        void showParameter(SumoXMLAttr attr, bool value);

        /// @brief hide all parameters
        void hideParameter();

        /// @brief return Attr
        SumoXMLAttr getAttr() const;

        /// @brief return value
        std::string getValue() const ;

    private:
        /// @brief XML attribute
        SumoXMLAttr myAttr;

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
        additionalParameterList(FXComposite *parent, FXObject* tgt);

        /// @brief destructor
        ~additionalParameterList();

        /// @brief show name and value of parameters of type int
        void showListParameter(SumoXMLAttr attr, std::vector<int> value);

        /// @brief show name and value of parameters of type float
        void showListParameter(SumoXMLAttr attr, std::vector<SUMOReal> value);

        /// @brief show name and value of parameters of type bool
        void showListParameter(SumoXMLAttr attr, std::vector<bool> value);

        /// @brief show name and value of parameters of type string
        void showListParameter(SumoXMLAttr attr, std::vector<std::string> value);

        /// @brief hide all parameters
        void hideParameter();

        /// @brief return attribute of list
        SumoXMLAttr getAttr() const;

        /// @brief return the value of list
        std::string getListValues();

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
        /// @brief XML attribute
        SumoXMLAttr myAttr;

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
    // class additionalParameters
    // ===========================================================================

    class additionalParameters : public FXGroupBox {
        // FOX-declaration
        FXDECLARE(GNEAdditionalFrame::additionalParameters)

    public:
        /// @brief constructor
        additionalParameters(FXComposite *parent, FXObject* tgt);

        /// @brief destructor
        ~additionalParameters();

        /// @brief clear attributes
        void clearAttributes();

        /// @brief add attribute
        void addAttribute(SumoXMLTag additional, SumoXMLAttr attribute);

        /// @brief show group box
        void showAdditionalParameters();

        /// @brief hide group box
        void hideAdditionalParameters();

        /// @brief get attributes
        std::map<SumoXMLAttr, std::string> getAttributes() const;

        /// @brief get number of added attributes
        int getNumberOfAddedAttributes() const;

        /// @name FOX-callbacks
        /// @{
        /// @brief Called when help button is pressed
        long onCmdHelp(FXObject*, FXSelector, void*);
        /// @}

    protected:
        /// @brief FOX needs this
        additionalParameters() {}

    private:
        /// @brief vector with the additional parameters
        std::vector<additionalParameter*> myVectorOfAdditionalParameter;

        /// @brief Index for myVectorOfAdditionalParameter
        int myIndexParameter;

        /// @brief vector with the additional parameters of type list
        std::vector<additionalParameterList*> myVectorOfAdditionalParameterList;

        /// @brief index for myIndexParameterList
        int myIndexParameterList;

        /// @brief max number of parameters (Defined in constructor)
        int maxNumberOfParameters;
    };

    // ===========================================================================
    // class editorParameters
    // ===========================================================================

    class editorParameters : public FXGroupBox {
        // FOX-declaration
        FXDECLARE(GNEAdditionalFrame::editorParameters)

    public:
        /// @brief list of the reference points
        enum additionalReferencePoint {
            GNE_ADDITIONALREFERENCEPOINT_LEFT,
            GNE_ADDITIONALREFERENCEPOINT_RIGHT,
            GNE_ADDITIONALREFERENCEPOINT_CENTER
        };

        /// @brief constructor
        editorParameters(FXComposite *parent, FXObject* tgt);

        /// @brief destructor
        ~editorParameters();

        /// @brief show length field
        void showLengthField();

        /// @brief hide length field
        void hideLengthField();

        /// @brief show refence point comboBox
        void showReferencePoint();

        /// @brief hide refence point comboBox
        void hideReferencePoint();

        /// @brief get actual reference point
        additionalReferencePoint getActualReferencePoint();

        /// @brief get value of lenght
        SUMOReal getLenght();

        /// @brief check if block is enabled
        bool isBlockEnabled();

        /// @brief check if force position is enabled
        bool isForcePositionEnabled();

        /// @name FOX-callbacks
        /// @{
        /// @brief Called when the user enters another reference point
        long onCmdSelectReferencePoint(FXObject*, FXSelector, void*);

/*** HAY QUE HACER UN CALLBACK PARA EL LENGTH **/


        /// @brief Called when help button is pressed
        long onCmdHelp(FXObject*, FXSelector, void*);
        /// @}

    protected:
        /// @brief FOX needs this
        editorParameters() {}

    private:
        /// @brief match box with the list of reference points
        FXComboBox* myReferencePointMatchBox;

        /// @brief Button for help about the reference point
        FXButton *helpReferencePoint;

        /// @brief actual additional reference point selected in the match Box
        additionalReferencePoint myActualAdditionalReferencePoint;

        /// @brief Label for lenght
        FXLabel* myLengthLabel;

        /// @brief textField for lenght
        FXTextField* myLengthTextField;

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
        additionalSet(FXComposite *parent, FXObject* tgt, GNEViewNet* updateTarget);

        /// @brief destructor
        ~additionalSet();

        /// @brief get if currently additional Set
        std::string getIdSelected() const;

        /// @brief get current tag
        SumoXMLTag getCurrentlyTag() const;

        /// @brief Show list of additionalSet
        void showList(SumoXMLTag type);

        /// @brief hide additionalSet
        void hideList();

        /// @name FOX-callbacks
        /// @{
        /// @brief add a new row int the list
        long onCmdSelectSet(FXObject*, FXSelector, void*);

        /// @brief Called when help button is pressed
        long onCmdHelp(FXObject*, FXSelector, void*);
        /// @}

    protected:
        /// @brief FOX needs this
        additionalSet() {}

    private:
        /// @brief current type
        SumoXMLTag myType;

        /// @brief List of Set
        FXList *myList;

        /// @brief Label with the name of Set
        FXLabel *mySetLabel;

        /// @brief button for help
        FXButton *helpAdditionalSet;

        /// @brief viewNet associated to GNEAdditionalFrame
        GNEViewNet* myViewNet;
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
     * @param[in] lane clicked lane. if user dind't clicked over lane, will be NULL
     * @param[in] parent AbstractView to obtain the position of the mouse over the lane.
     * @return true if an additional (GNEAdditional or GNEAdditionalSet) was added, false in other case
     */
    bool addAdditional(GNELane *lane, GUISUMOAbstractView* parent);

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
    /// @brief set parameters depending of the new additionalType
    void setParametersOfAdditional(SumoXMLTag actualAdditionalType);

    /// @brief obtain the Start position values of StoppingPlaces and E2 detector over the lane
    SUMOReal setStartPosition(SUMOReal positionOfTheMouseOverLane, SUMOReal lenghtOfAdditional);

    /// @brief obtain the End position values of StoppingPlaces and E2 detector over the lane
    SUMOReal setEndPosition(SUMOReal laneLenght, SUMOReal positionOfTheMouseOverLane, SUMOReal lenghtOfAdditional);

    /// @brief the panel to hold all member widgets
    FXVerticalFrame* myContentFrame;

    /// @brief Font for the widget
    FXFont* myHeaderFont;

    /// @brief the label for the frame
    FXLabel* myFrameLabel;

    /// @brief groupBox for Match Box of additionals
    FXGroupBox* myGroupBoxForMyAdditionalMatchBox;

    /// @brief combo box with the list of additional elements
    FXComboBox* myAdditionalMatchBox;

    /// @brief additional default parameters
    GNEAdditionalFrame::additionalParameters *myAdditionalParameters;

    /// @brief editor parameter
    GNEAdditionalFrame::editorParameters *myEditorParameters;

    /// @brief additional Set
    GNEAdditionalFrame::additionalSet *myAdditionalSet;

    /// @brief the window to inform
    GNEViewNet* myViewNet;

    /// @brief actual additional type selected in the match Box
    SumoXMLTag myActualAdditionalType;

    /// @brief Width of frame
    static const int WIDTH;

    /// @brief undo
    GNEUndoList* myUndoList;
};


#endif

/****************************************************************************/
