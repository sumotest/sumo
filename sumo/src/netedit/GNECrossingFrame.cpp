/****************************************************************************/
/// @file    GNECrossingFrame.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Oct 2016
/// @version $Id: GNECrossingFrame.cpp 21640 2016-10-09 20:28:52Z palcraft $
///
/// The Widget for add additional elements
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

// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#ifdef HAVE_VERSION_H
#include <version.h>
#endif

#include <iostream>
#include <utils/foxtools/fxexdefs.h>
#include <utils/foxtools/MFXUtils.h>
#include <utils/common/MsgHandler.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/div/GUIIOGlobals.h>
#include <utils/gui/div/GUIGlobalSelection.h>
#include <utils/gui/globjects/GUIGlObjectStorage.h>
#include <utils/gui/images/GUIIconSubSys.h>
#include "GNEFrameDesigns.h"
#include "GNECrossingFrame.h"
#include "GNEViewNet.h"
#include "GNEViewParent.h"
#include "GNENet.h"
#include "GNEJunction.h"
#include "GNEEdge.h"
#include "GNELane.h"
#include "GNECrossing.h"
#include "GNEUndoList.h"
#include "GNEChange_Selection.h"
#include "GNEAttributeCarrier.h"
#include "GNEChange_Crossing.h"
#include "GNECrossing.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// FOX callback mapping
// ===========================================================================

FXDEFMAP(GNECrossingFrame) GNECrossingMap[] = {
    FXMAPFUNC(SEL_COMMAND, MID_GNE_MODE_ADDITIONAL_ITEM, GNECrossingFrame::onCmdSelectCrossing),
};

FXDEFMAP(GNECrossingFrame::edgesSelector) GNEEdgesMap[] = {
    FXMAPFUNC(SEL_COMMAND, MID_GNE_USESELECTEDEDGES,    GNECrossingFrame::edgesSelector::onCmdUseSelectedEdges),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_CLEAREDGESELECTION,  GNECrossingFrame::edgesSelector::onCmdClearSelection),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_INVERTEDGESELECTION, GNECrossingFrame::edgesSelector::onCmdInvertSelection),
    FXMAPFUNC(SEL_CHANGED, MID_GNE_SEARCHEDGE,          GNECrossingFrame::edgesSelector::onCmdTypeInSearchBox),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_SELECTEDGE,          GNECrossingFrame::edgesSelector::onCmdSelectEdge),
    FXMAPFUNC(SEL_COMMAND, MID_HELP,                    GNECrossingFrame::edgesSelector::onCmdHelp),
};

FXDEFMAP(GNECrossingFrame::crossingParameters) GNECrossingParametersMap[] = {
    FXMAPFUNC(SEL_COMMAND, MID_HELP, GNECrossingFrame::crossingParameters::onCmdHelp),
};

// Object implementation
FXIMPLEMENT(GNECrossingFrame,                     FXScrollWindow, GNECrossingMap,           ARRAYNUMBER(GNECrossingMap))
FXIMPLEMENT(GNECrossingFrame::edgesSelector,      FXGroupBox,     GNEEdgesMap,              ARRAYNUMBER(GNEEdgesMap))
FXIMPLEMENT(GNECrossingFrame::crossingParameters, FXGroupBox,     GNECrossingParametersMap, ARRAYNUMBER(GNECrossingParametersMap))

// ===========================================================================
// method definitions
// ===========================================================================

// ---------------------------------------------------------------------------
// GNECrossingFrame::edgesSelector - methods
// ---------------------------------------------------------------------------

GNECrossingFrame::edgesSelector::edgesSelector(FXComposite* parent, GNEViewNet* viewNet) :
    FXGroupBox(parent, "Edges", GNEDesignGroupBox),
    myCurrentJunction(0),
    myViewNet(viewNet) {
    // Create search box
    myEdgesSearch = new FXTextField(this, 10, this, MID_GNE_SEARCHEDGE, GNEDesignTextField);

    // Create list
    myList = new FXList(this, this, MID_GNE_SELECTEDGE, GNEDesignList, 0, 0, 0, 100);

    // Create horizontal frame
    FXHorizontalFrame* buttonsFrame = new FXHorizontalFrame(this, GNEDesignHorizontalFrame);

    // Create button for clear selection
    clearEdgesSelection = new FXButton(buttonsFrame, "clear", 0, this, MID_GNE_CLEAREDGESELECTION, GNEDesignButton);

    // Create button for invert selection
    invertEdgesSelection = new FXButton(buttonsFrame, "invert", 0, this, MID_GNE_INVERTEDGESELECTION, GNEDesignButton);

    // Create CheckBox for selected edges
    myUseSelectedEdges = new FXMenuCheck(this, "Use selected Edges", this, MID_GNE_USESELECTEDEDGES, GNEDesignCheckButton);

    // Create help button
    helpEdges = new FXButton(this, "Help", 0, this, MID_HELP, GNEDesignButtonLittle);

    // Hide List
    disableEdgeSelector();
}


GNECrossingFrame::edgesSelector::~edgesSelector() {}


std::vector<std::string>
GNECrossingFrame::edgesSelector::getEdgeIDSSelected() const {
    std::vector<std::string> IDsSelected;

    ///////////////////////////

    return IDsSelected;
}


std::vector<GNEEdge*>
GNECrossingFrame::edgesSelector::getGNEEdgesSelected() const {
    std::vector<GNEEdge*> GNEEdgesSelected;

    ///////////////////////////

    return GNEEdgesSelected;
}


GNEJunction*
GNECrossingFrame::edgesSelector::getCurrentJunction() const {
    return myCurrentJunction;
}


void 
GNECrossingFrame::edgesSelector::filterListOfEdges(std::string search) {
    // clear list of egdge ids
    myList->clearItems();
    // get all edges of junction
    std::vector<GNEEdge*> vectorOfEdges = myCurrentJunction->getGNEEdges();
    // iterate over edges of junction
    for (std::vector<GNEEdge*>::iterator i = vectorOfEdges.begin(); i != vectorOfEdges.end(); i++) {
        // If search criterium is correct, then append ittem
        if ((*i)->getID().find(search) != std::string::npos) {
            myList->appendItem((*i)->getID().c_str());
        }
    }
    // By default, CheckBox for useSelectedEdges isn't checked
    myUseSelectedEdges->setCheck(false);
    // Recalc Frame
    recalc();
    // Update Frame
    update();
    // Show dialog
    show();
}


void 
GNECrossingFrame::edgesSelector::clearList() {
    // Clear all item of the FXList
    myList->clearItems();
}


void 
GNECrossingFrame::edgesSelector::enableEdgeSelector(GNEJunction *currentJunction) {
    // Set current junction
    myCurrentJunction = currentJunction;
    // Enable all elements of the edgesSelector
    myUseSelectedEdges->enable();
    myList->enable();
    myEdgesSearch->enable();
    helpEdges->enable();
    clearEdgesSelection->enable();
    invertEdgesSelection->enable();
    // show edges in list
    filterListOfEdges();
}


void 
GNECrossingFrame::edgesSelector::disableEdgeSelector() {
    // disable current junction
    myCurrentJunction = NULL;
    // clear list of edges
    clearList();
    // disable all elements of the edgesSelector
    myUseSelectedEdges->disable();
    myList->disable();
    myEdgesSearch->disable();
    helpEdges->disable();
    clearEdgesSelection->disable();
    invertEdgesSelection->disable();
}



void
GNECrossingFrame::edgesSelector::updateUseSelectedEdges() {
    // Enable or disable use selected edges
    if (myViewNet->getNet()->retrieveEdges(true).size() > 0) {
        myUseSelectedEdges->enable();
    } else {
        myUseSelectedEdges->disable();
    }
}


bool
GNECrossingFrame::edgesSelector::isUseSelectedEdgesEnable() const {
    if (myUseSelectedEdges->getCheck()) {
        return true;
    } else {
        return false;
    }
}


long
GNECrossingFrame::edgesSelector::onCmdUseSelectedEdges(FXObject*, FXSelector, void*) {
    if (myUseSelectedEdges->getCheck()) {
        myEdgesSearch->hide();
        myList->hide();
        clearEdgesSelection->hide();
        invertEdgesSelection->hide();
        helpEdges->hide();
    } else {
        myEdgesSearch->show();
        myList->show();
        clearEdgesSelection->show();
        invertEdgesSelection->show();
        helpEdges->show();
    }
    // Recalc Frame
    recalc();
    // Update Frame
    update();
    return 1;
}


long
GNECrossingFrame::edgesSelector::onCmdTypeInSearchBox(FXObject*, FXSelector, void*) {
    // Show only Id's of edgesSelector that contains the searched string
    filterListOfEdges(myEdgesSearch->getText().text());
    return 1;
}


long
GNECrossingFrame::edgesSelector::onCmdSelectEdge(FXObject*, FXSelector, void*) {
    return 1;
}


long
GNECrossingFrame::edgesSelector::onCmdClearSelection(FXObject*, FXSelector, void*) {
    for (int i = 0; i < myList->getNumItems(); i++) {
        if (myList->getItem(i)->isSelected()) {
            myList->deselectItem(i);
        }
    }
    return 1;
}


long
GNECrossingFrame::edgesSelector::onCmdInvertSelection(FXObject*, FXSelector, void*) {
    for (int i = 0; i < myList->getNumItems(); i++) {
        if (myList->getItem(i)->isSelected()) {
            myList->deselectItem(i);
        } else {
            myList->selectItem(i);
        }
    }
    return 1;
}


long
GNECrossingFrame::edgesSelector::onCmdHelp(FXObject*, FXSelector, void*) {
    std::cout << "IMPLEMENT" << std::endl;
    return 1;
}


// ---------------------------------------------------------------------------
// GNECrossingFrame::editorParameters- methods
// ---------------------------------------------------------------------------

GNECrossingFrame::crossingParameters::crossingParameters(FXComposite* parent, FXObject* tgt, GNECrossingFrame::edgesSelector *es) :
    FXGroupBox(parent, "Crossing parameters", GNEDesignGroupBox),
    myEdgeSelector(es) {
    // Create a Matrix for parameters
    attributesMatrix = new FXMatrix(this, 2, GNEDesignMatrix);
    // create label for Crossing ID
    crossingIDLabel = new FXLabel(attributesMatrix, toString(SUMO_ATTR_ID).c_str(), 0, GNEDesignLabelAttribute);
    // create string textField for Crossing ID
    crossingID = new FXTextField(attributesMatrix, 10, tgt, 0, GNEDesignTextFieldAttributeStr);;
    // create label for edges
    crossingEdgesLabel = new FXLabel(attributesMatrix, toString(SUMO_ATTR_EDGES).c_str(), 0, GNEDesignLabelAttribute);
    // create string textField for edges
    crossingEdges = new FXTextField(attributesMatrix, 10, tgt, 0, GNEDesignTextFieldAttributeStr);
    // create label for Priority
    crossingPriorityLabel = new FXLabel(attributesMatrix, toString(SUMO_ATTR_PRIORITY).c_str(), 0, GNEDesignLabelAttribute);
    // create CheckBox for Priority
    crossingPriority = new FXMenuCheck(attributesMatrix, "", tgt, 0, GNEDesignCheckButtonAttribute);
    // create label for width
    crossingWidthLabel = new FXLabel(attributesMatrix, toString(SUMO_ATTR_WIDTH).c_str(), 0, GNEDesignLabelAttribute);
    // create extField for width
    crossingWidth = new FXTextField(attributesMatrix, 10, tgt, 0, GNEDesignTextFieldAttributeReal);
    // Create help button
    helpAdditional = new FXButton(this, "Help", 0, this, MID_HELP, GNEDesignButtonLittle);
    // At start crossing parameters is disabled
    disableCrossingParameters();
}


GNECrossingFrame::crossingParameters::~crossingParameters() {
}


void
GNECrossingFrame::crossingParameters::enableCrossingParameters() {
    // Enable all elements of the crossing frames
    crossingIDLabel->enable();
    crossingID->enable();
    crossingEdgesLabel->enable();
    crossingEdges->enable();
    crossingPriorityLabel->enable();
    crossingPriority->enable();
    crossingWidthLabel->enable();
    crossingWidth->enable();
    helpAdditional->enable();
    // set values of parameters
    crossingID->setText("TEMPORALID");
    crossingEdges->setText(joinToString(myEdgeSelector->getEdgeIDSSelected(), " ").c_str());
    crossingPriority->setCheck(GNEAttributeCarrier::getDefaultValue<bool>(SUMO_TAG_CROSSING, SUMO_ATTR_PRIORITY));
    crossingWidth->setText(GNEAttributeCarrier::getDefaultValue<std::string>(SUMO_TAG_CROSSING, SUMO_ATTR_WIDTH).c_str());    
}


void
GNECrossingFrame::crossingParameters::disableCrossingParameters() {
    // clear all values of parameters
    crossingID->setText("");
    crossingEdges->setText("");
    crossingPriority->setCheck(false);
    crossingWidth->setText("");    
    // Disable all elements of the crossing frames
    crossingIDLabel->disable();
    crossingID->disable();
    crossingEdgesLabel->disable();
    crossingEdges->disable();
    crossingPriorityLabel->disable();
    crossingPriority->disable();
    crossingWidthLabel->disable();
    crossingWidth->disable();
    helpAdditional->disable();
}


std::string 
GNECrossingFrame::crossingParameters::getCrossingID() const {
    return crossingID->getText().text();
}


std::vector<GNEEdge*> 
GNECrossingFrame::crossingParameters::getCrossingEdges() const {
/*******************************************/
    return std::vector<GNEEdge*>();
}


bool 
GNECrossingFrame::crossingParameters::getCrossingPriority() const {
    if (crossingPriority->getCheck()) {
        return true;
    } else {
        return false;
    }
}

SUMOReal 
GNECrossingFrame::crossingParameters::getCrossingWidth() const {
    return GNEAttributeCarrier::parse<SUMOReal>(crossingWidth->getText().text());
}


long
GNECrossingFrame::crossingParameters::onCmdHelp(FXObject*, FXSelector, void*) {
    // Create help dialog
    FXDialogBox* helpDialog = new FXDialogBox(this, ("Parameters of " + toString(SUMO_TAG_CROSSING)).c_str(), GNEDesignDialogBox);
    // Create FXTable
    FXTable* myTable = new FXTable(helpDialog, this, MID_TABLE, TABLE_READONLY);
    //myTable->setVisibleRows((FXint)(myIndexParameter + myIndexParameterList));
    myTable->setVisibleColumns(3);
    //myTable->setTableSize((FXint)(myIndexParameter + myIndexParameterList), 3);
    myTable->setBackColor(FXRGB(255, 255, 255));
    myTable->setColumnText(0, "Name");
    myTable->setColumnText(1, "Value");
    myTable->setColumnText(2, "Definition");
    myTable->getRowHeader()->setWidth(0);
    FXHeader* header = myTable->getColumnHeader();
    header->setItemJustify(0, JUSTIFY_CENTER_X);
    header->setItemSize(0, 120);
    header->setItemJustify(1, JUSTIFY_CENTER_X);
    header->setItemSize(1, 80);
    int maxSizeColumnDefinitions = 0;
    
/*
    // Iterate over vector of additional parameters
    for (int i = 0; i < myIndexParameter; i++) {
        SumoXMLAttr attr = myVectorOfAdditionalParameter.at(i)->getAttr();
        // Set name of attribute
        myTable->setItem(i, 0, new FXTableItem(toString(attr).c_str()));
        // Set type
        FXTableItem* type = new FXTableItem("");
        if (GNEAttributeCarrier::isInt(attr)) {
            type->setText("int");
        } else if (GNEAttributeCarrier::isFloat(attr)) {
            type->setText("float");
        } else if (GNEAttributeCarrier::isBool(attr)) {
            type->setText("bool");
        } else if (GNEAttributeCarrier::isString(attr)) {
            type->setText("string");
        }
        type->setJustify(FXTableItem::CENTER_X);
        myTable->setItem(i, 1, type);
        // Set definition
        FXTableItem* definition = new FXTableItem(GNEAttributeCarrier::getDefinition(myAdditional, attr).c_str());
        definition->setJustify(FXTableItem::LEFT);
        myTable->setItem(i, 2, definition);
        if ((int)GNEAttributeCarrier::getDefinition(myAdditional, attr).size() > maxSizeColumnDefinitions) {
            maxSizeColumnDefinitions = int(GNEAttributeCarrier::getDefinition(myAdditional, attr).size());
        }
    }
    // Iterate over vector of additional parameters list
    for (int i = 0; i < myIndexParameterList; i++) {
        SumoXMLAttr attr = myVectorOfAdditionalParameterList.at(i)->getAttr();
        // Set name of attribute
        myTable->setItem(i, 0, new FXTableItem(toString(attr).c_str()));
        // Set type
        FXTableItem* type = new FXTableItem("");
        if (GNEAttributeCarrier::isInt(attr)) {
            type->setText("list of int");
        } else if (GNEAttributeCarrier::isFloat(attr)) {
            type->setText("list of float");
        } else if (GNEAttributeCarrier::isBool(attr)) {
            type->setText("list of bool");
        } else if (GNEAttributeCarrier::isString(attr)) {
            type->setText("list of string");
        }
        type->setJustify(FXTableItem::CENTER_X);
        myTable->setItem(i, 1, type);
        // Set definition
        FXTableItem* definition = new FXTableItem(GNEAttributeCarrier::getDefinition(myAdditional, attr).c_str());
        definition->setJustify(FXTableItem::LEFT);
        myTable->setItem(i, 2, definition);
        if ((int)GNEAttributeCarrier::getDefinition(myAdditional, attr).size() > maxSizeColumnDefinitions) {
            maxSizeColumnDefinitions = int(GNEAttributeCarrier::getDefinition(myAdditional, attr).size());
        }
    }
*/

    // Set size of column
    header->setItemJustify(2, JUSTIFY_CENTER_X);
    header->setItemSize(2, maxSizeColumnDefinitions * 6);
    // Button Close
    new FXButton(helpDialog, "OK\t\tclose", 0, helpDialog, FXDialogBox::ID_ACCEPT, GNEDesignButtonDialog, 0, 0, 0, 0, 4, 4, 3, 3);
    helpDialog->create();
    helpDialog->show();
    return 1;
}

// ---------------------------------------------------------------------------
// GNECrossingFrame - methods
// ---------------------------------------------------------------------------

GNECrossingFrame::GNECrossingFrame(FXComposite* parent, GNEViewNet* viewNet):
    GNEFrame(parent, viewNet, "Crossings") {
    // Create edge Selector
    myEdgeSelector = new edgesSelector(myContentFrame, myViewNet);
    // Create crossingParameters
    myCrossingParameters = new crossingParameters(myContentFrame, myViewNet, myEdgeSelector);
}


GNECrossingFrame::~GNECrossingFrame() {
    gSelected.remove2Update();
}


bool
GNECrossingFrame::addCrossing(GNENetElement* netElement) {
   
    return false;
}

void
GNECrossingFrame::removeCrossing(GNECrossing* additional) {
    myViewNet->getUndoList()->p_begin("delete " + additional->getDescription());
    myViewNet->getUndoList()->add(new GNEChange_Crossing(myViewNet->getNet(), additional, false), true);
    myViewNet->getUndoList()->p_end();
}


long
GNECrossingFrame::onCmdSelectCrossing(FXObject*, FXSelector, void*) {

    return 1;
}


void
GNECrossingFrame::show() {
    // Show Scroll window
    FXScrollWindow::show();
    // Show Frame Area in which this GNEFrame is placed
    myViewNet->getViewParent()->showFramesArea();
}


void
GNECrossingFrame::hide() {
    // Hide ScrollWindow
    FXScrollWindow::hide();
    // Hide Frame Area in which this GNEFrame is placed
    myViewNet->getViewParent()->hideFramesArea();
}

/****************************************************************************/
