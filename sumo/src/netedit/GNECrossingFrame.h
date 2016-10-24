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

    // ===========================================================================
    // class CrossingParameter
    // ===========================================================================

    class CrossingParameter : public FXMatrix {

    public:
        /// @brief constructor
        CrossingParameter(FXComposite* parent, FXObject* tgt);

        /// @brief destructor
        ~CrossingParameter();

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
        FXLabel* myLabel;

        /// @brief textField to modify the value of parameter
        FXTextField* myTextField;

        /// @brief menuCheck to enable/disable the value of parameter
        FXMenuCheck* myMenuCheck;
    };

    // ===========================================================================
    // class edgesSelector
    // ===========================================================================

    class edgesSelector : public FXGroupBox {
        /// @brief FOX-declaration
        FXDECLARE(GNECrossingFrame::edgesSelector)

    public:
        /// @brief constructor
        edgesSelector(FXComposite* parent, GNEViewNet* viewNet);

        /// @brief destructor
        ~edgesSelector();

        /// @brief get list of selecte id's in string format
        std::string getIdsSelected() const;

        /// @brief Show list of edgesSelector
        void showList(std::string search = "");

        /// @brief hide edgesSelector
        void hideList();

        /// @brief Update use selectedEdges
        void updateUseSelectedEdges();

        /// @brief get status of checkBox UseSelectedEdges
        bool isUseSelectedEdgesEnable() const;

        /// @name FOX-callbacks
        /// @{
        /// @brief called when user trigger checkBox of useSelectedEdges
        long onCmdUseSelectedEdges(FXObject*, FXSelector, void*);

        /// @brief called when user type in search box
        long onCmdTypeInSearchBox(FXObject*, FXSelector, void*);

        /// @brief called when user select a edge of the list
        long onCmdSelectEdge(FXObject*, FXSelector, void*);

        /// @brief called when clear selection button is pressed
        long onCmdClearSelection(FXObject*, FXSelector, void*);

        /// @brief called when invert selection button is pressed
        long onCmdInvertSelection(FXObject*, FXSelector, void*);

        /// @brief Called when help button is pressed
        long onCmdHelp(FXObject*, FXSelector, void*);
        /// @}

    protected:
        /// @brief FOX needs this
        edgesSelector() {}

    private:
        /// @brief CheckBox for selected edges
        FXMenuCheck* myUseSelectedEdges;

        /// @brief List of edgesSelector
        FXList* myList;

        /// @brief text field for search edge IDs
        FXTextField* myEdgesSearch;

        /// @brief button for help
        FXButton* helpEdges;

        /// @brief button for clear selection
        FXButton* clearEdgesSelection;

        /// @brief button for invert selection
        FXButton* invertEdgesSelection;

        /// @brief viewNet associated to GNECrossingFrame
        GNEViewNet* myViewNet;
    };

    // ===========================================================================
    // class lanesSelector
    // ===========================================================================

    class lanesSelector : public FXGroupBox {
        /// @brief FOX-declaration
        FXDECLARE(GNECrossingFrame::lanesSelector)

    public:
        /// @brief constructor
        lanesSelector(FXComposite* parent, GNEViewNet* viewNet);

        /// @brief destructor
        ~lanesSelector();

        /// @brief get list of selecte id's in string format
        std::string getIdsSelected() const;

        /// @brief Show list of lanesSelector
        void showList(std::string search = "");

        /// @brief hide lanesSelector
        void hideList();

        // @brief Update use selectedLanes
        void updateUseSelectedLanes();

        /// @brief get status of checkBox UseSelectedLanes
        bool isUseSelectedLanesEnable() const;

        /// @name FOX-callbacks
        /// @{
        /// @brief called when user trigger checkBox of useSelectedLanes
        long onCmdUseSelectedLanes(FXObject*, FXSelector, void*);

        /// @brief called when user type in search box
        long onCmdTypeInSearchBox(FXObject*, FXSelector, void*);

        /// @brief called when user select a lane of the list
        long onCmdSelectLane(FXObject*, FXSelector, void*);

        /// @brief called when clear selection button is pressed
        long onCmdClearSelection(FXObject*, FXSelector, void*);

        /// @brief called when invert selection button is pressed
        long onCmdInvertSelection(FXObject*, FXSelector, void*);

        /// @brief Called when help button is pressed
        long onCmdHelp(FXObject*, FXSelector, void*);
        /// @}

    protected:
        /// @brief FOX needs this
        lanesSelector() {}

    private:
        /// @brief CheckBox for selected lanes
        FXMenuCheck* myUseSelectedLanes;

        /// @brief List of lanesSelector
        FXList* myList;

        /// @brief text field for search lane IDs
        FXTextField* myLanesSearch;

        /// @brief button for help
        FXButton* helpLanes;

        /// @brief button for clear selection
        FXButton* clearLanesSelection;

        /// @brief button for invert selection
        FXButton* invertLanesSelection;

        /// @brief viewNet associated to GNECrossingFrame
        GNEViewNet* myViewNet;
    };

    /**@brief Constructor
     * @brief parent FXFrame in which this GNEFrame is placed
     * @brief viewNet viewNet that uses this GNEFrame
     */
    GNECrossingFrame(FXComposite* parent, GNEViewNet* viewNet);

    /// @brief Destructor
    ~GNECrossingFrame();

    /**@brief add Crossing element
     * @param[in] netElement clicked netElement. if user dind't clicked over a GNENetElement in view, netElement will be NULL
     * @param[in] parent AbstractView to obtain the position of the mouse over the lane.
     * @return true if an Crossing (GNECrossing or GNECrossingSet) was added, false in other case
     */
    bool addCrossing(GNENetElement* netElement, GUISUMOAbstractView* parent);

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
    /// @brief set parameters depending of the new CrossingType
    void setParametersOfCrossing(SumoXMLTag actualCrossingType);

    /// @brief generate a ID for an additiona element
    std::string generateID(GNENetElement* netElement) const;

    /// @brief obtain the Start position values of StoppingPlaces and E2 detector over the lane
    SUMOReal setStartPosition(SUMOReal positionOfTheMouseOverLane, SUMOReal lenghtOfCrossing);

    /// @brief obtain the End position values of StoppingPlaces and E2 detector over the lane
    SUMOReal setEndPosition(SUMOReal laneLenght, SUMOReal positionOfTheMouseOverLane, SUMOReal lenghtOfCrossing);

    /// @brief groupBox for Match Box of Crossings
    FXGroupBox* myGroupBoxForMyCrossingMatchBox;

    /// @brief combo box with the list of Crossing elements
    FXComboBox* myCrossingMatchBox;

    /// @brief list of edgesSelector
    GNECrossingFrame::edgesSelector* myEdgesSelector;

    /// @brief list of lanesSelector
    GNECrossingFrame::lanesSelector* myLanesSelector;

    /// @brief actual Crossing type selected in the match Box
    SumoXMLTag myActualCrossingType;
};


#endif

/****************************************************************************/
