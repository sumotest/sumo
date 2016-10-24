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

FXDEFMAP(GNECrossingFrame::lanesSelector) GNELanesMap[] = {
    FXMAPFUNC(SEL_COMMAND, MID_GNE_USESELECTEDLANES,    GNECrossingFrame::lanesSelector::onCmdUseSelectedLanes),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_CLEARLANESELECTION,  GNECrossingFrame::lanesSelector::onCmdClearSelection),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_INVERTLANESELECTION, GNECrossingFrame::lanesSelector::onCmdInvertSelection),
    FXMAPFUNC(SEL_CHANGED, MID_GNE_SEARCHLANE,          GNECrossingFrame::lanesSelector::onCmdTypeInSearchBox),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_SELECTLANE,          GNECrossingFrame::lanesSelector::onCmdSelectLane),
    FXMAPFUNC(SEL_COMMAND, MID_HELP,                    GNECrossingFrame::lanesSelector::onCmdHelp),
};

// Object implementation
FXIMPLEMENT(GNECrossingFrame,                          FXScrollWindow, GNECrossingMap,              ARRAYNUMBER(GNECrossingMap))
FXIMPLEMENT(GNECrossingFrame::edgesSelector,           FXGroupBox,     GNEEdgesMap,                   ARRAYNUMBER(GNEEdgesMap))
FXIMPLEMENT(GNECrossingFrame::lanesSelector,           FXGroupBox,     GNELanesMap,                   ARRAYNUMBER(GNELanesMap))

// ===========================================================================
// method definitions
// ===========================================================================

GNECrossingFrame::GNECrossingFrame(FXComposite* parent, GNEViewNet* viewNet):
    GNEFrame(parent, viewNet, "Crossings"),
    myActualCrossingType(SUMO_TAG_NOTHING) {

    // Create groupBox for myCrossingMatchBox
    myGroupBoxForMyCrossingMatchBox = new FXGroupBox(myContentFrame, "Crossing element", GROUPBOX_TITLE_CENTER | FRAME_GROOVE | LAYOUT_FILL_X);

    // Create FXListBox in myGroupBoxForMyCrossingMatchBox
    myCrossingMatchBox = new FXComboBox(myGroupBoxForMyCrossingMatchBox, 12, this, MID_GNE_MODE_ADDITIONAL_ITEM,  FRAME_SUNKEN | LAYOUT_LEFT | LAYOUT_TOP | COMBOBOX_STATIC | LAYOUT_CENTER_Y | LAYOUT_FILL_X);

    /// Create list for edgesSelector
    myEdgesSelector = new GNECrossingFrame::edgesSelector(myContentFrame, myViewNet);

    /// Create list for lanesSelector
    myLanesSelector = new GNECrossingFrame::lanesSelector(myContentFrame, myViewNet);

    // Set visible items
    myCrossingMatchBox->setNumVisible((int)myCrossingMatchBox->getNumItems());
}


GNECrossingFrame::~GNECrossingFrame() {
    gSelected.remove2Update();
}


bool
GNECrossingFrame::addCrossing(GNENetElement* netElement, GUISUMOAbstractView* parent) {
   
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
    // Update UseAelectedLane CheckBox
    myEdgesSelector->updateUseSelectedEdges();
    // Update UseAelectedLane CheckBox
    myLanesSelector->updateUseSelectedLanes();
}


void
GNECrossingFrame::hide() {
    // Hide ScrollWindow
    FXScrollWindow::hide();
    // Hide Frame Area in which this GNEFrame is placed
    myViewNet->getViewParent()->hideFramesArea();
}


void
GNECrossingFrame::setParametersOfCrossing(SumoXMLTag actualCrossingType) {
   
}


std::string
GNECrossingFrame::generateID(GNENetElement* netElement) const {
    return "";
}


SUMOReal
GNECrossingFrame::setStartPosition(SUMOReal positionOfTheMouseOverLane, SUMOReal lenghtOfCrossing) {
    return false;
}


SUMOReal
GNECrossingFrame::setEndPosition(SUMOReal laneLenght, SUMOReal positionOfTheMouseOverLane, SUMOReal lenghtOfCrossing) {
    return false;
}

// ---------------------------------------------------------------------------
// GNECrossingFrame::edgesSelector - methods
// ---------------------------------------------------------------------------

GNECrossingFrame::edgesSelector::edgesSelector(FXComposite* parent, GNEViewNet* viewNet) :
    FXGroupBox(parent, "Edges", GROUPBOX_TITLE_CENTER | FRAME_GROOVE | LAYOUT_FILL_X),
    myViewNet(viewNet) {
    // Create CheckBox for selected edges
    myUseSelectedEdges = new FXMenuCheck(this, "Use selected Edges", this, MID_GNE_USESELECTEDEDGES);

    // Create search box
    myEdgesSearch = new FXTextField(this, 10, this, MID_GNE_SEARCHEDGE, LAYOUT_FILL_X);

    // Create list
    myList = new FXList(this, this, MID_GNE_SELECTEDGE, LAYOUT_FILL_X | LAYOUT_FIX_HEIGHT, 0, 0, 0, 100);

    // Create horizontal frame
    FXHorizontalFrame* buttonsFrame = new FXHorizontalFrame(this, LAYOUT_FILL_X);

    // Create button for clear selection
    clearEdgesSelection = new FXButton(buttonsFrame, "clear", 0, this, MID_GNE_CLEAREDGESELECTION);

    // Create button for invert selection
    invertEdgesSelection = new FXButton(buttonsFrame, "invert", 0, this, MID_GNE_INVERTEDGESELECTION);

    // Create help button
    helpEdges = new FXButton(this, "Help", 0, this, MID_HELP);

    // Hide List
    hideList();
}


GNECrossingFrame::edgesSelector::~edgesSelector() {}


std::string
GNECrossingFrame::edgesSelector::getIdsSelected() const {
    return "";
}


void
GNECrossingFrame::edgesSelector::showList(std::string search) {
    // FIll list
    myList->clearItems();
    std::vector<GNEEdge*> vectorOfEdges = myViewNet->getNet()->retrieveEdges(false);
    for (std::vector<GNEEdge*>::iterator i = vectorOfEdges.begin(); i != vectorOfEdges.end(); i++) {
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
GNECrossingFrame::edgesSelector::hideList() {
    FXGroupBox::hide();
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
    showList(myEdgesSearch->getText().text());
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
// GNECrossingFrame::lanesSelector - methods
// ---------------------------------------------------------------------------

GNECrossingFrame::lanesSelector::lanesSelector(FXComposite* parent, GNEViewNet* viewNet) :
    FXGroupBox(parent, "lanesSelector", GROUPBOX_TITLE_CENTER | FRAME_GROOVE | LAYOUT_FILL_X),
    myViewNet(viewNet) {
    // Create CheckBox for selected lanes
    myUseSelectedLanes = new FXMenuCheck(this, "Use selected Lanes", this, MID_GNE_USESELECTEDLANES);

    // Create search box
    myLanesSearch = new FXTextField(this, 10, this, MID_GNE_SEARCHLANE, LAYOUT_FILL_X);

    // Create list
    myList = new FXList(this, this, MID_GNE_SELECTLANE, LAYOUT_FILL_X | LAYOUT_FIX_HEIGHT, 0, 0, 0, 100);

    // Create horizontal frame
    FXHorizontalFrame* buttonsFrame = new FXHorizontalFrame(this, LAYOUT_FILL_X);

    // Create button for clear selection
    clearLanesSelection = new FXButton(buttonsFrame, "clear", 0, this, MID_GNE_CLEARLANESELECTION);

    // Create button for invert selection
    invertLanesSelection = new FXButton(buttonsFrame, "invert", 0, this, MID_GNE_INVERTLANESELECTION);

    // Create help button
    helpLanes = new FXButton(this, "Help", 0, this, MID_HELP);

    // Hide List
    hideList();
}


GNECrossingFrame::lanesSelector::~lanesSelector() {}


std::string
GNECrossingFrame::lanesSelector::getIdsSelected() const {
    return "";
}


void
GNECrossingFrame::lanesSelector::showList(std::string search) {
    myList->clearItems();
    std::vector<GNELane*> vectorOfLanes = myViewNet->getNet()->retrieveLanes(false);
    for (std::vector<GNELane*>::iterator i = vectorOfLanes.begin(); i != vectorOfLanes.end(); i++) {
        if ((*i)->getID().find(search) != std::string::npos) {
            myList->appendItem((*i)->getID().c_str());
        }
    }
    // By default, CheckBox for useSelectedLanes isn't checked
    myUseSelectedLanes->setCheck(false);
    // Show list
    show();
}


void
GNECrossingFrame::lanesSelector::hideList() {
    hide();
}


void
GNECrossingFrame::lanesSelector::updateUseSelectedLanes() {
    // Enable or disable use selected Lanes
    if (myViewNet->getNet()->retrieveLanes(true).size() > 0) {
        myUseSelectedLanes->enable();
    } else {
        myUseSelectedLanes->disable();
    }
}


bool
GNECrossingFrame::lanesSelector::isUseSelectedLanesEnable() const {
    if (myUseSelectedLanes->getCheck()) {
        return true;
    } else {
        return false;
    }
}


long
GNECrossingFrame::lanesSelector::onCmdUseSelectedLanes(FXObject*, FXSelector, void*) {
    if (myUseSelectedLanes->getCheck()) {
        myLanesSearch->hide();
        myList->hide();
        clearLanesSelection->hide();
        invertLanesSelection->hide();
        helpLanes->hide();
    } else {
        myLanesSearch->show();
        myList->show();
        clearLanesSelection->show();
        invertLanesSelection->show();
        helpLanes->show();
    }
    // Recalc Frame
    recalc();
    // Update Frame
    update();
    return 1;
}


long
GNECrossingFrame::lanesSelector::onCmdTypeInSearchBox(FXObject*, FXSelector, void*) {
    // Show only Id's of lanesSelector that contains the searched string
    showList(myLanesSearch->getText().text());
    return 1;
}


long
GNECrossingFrame::lanesSelector::onCmdSelectLane(FXObject*, FXSelector, void*) {
    return 1;
}


long
GNECrossingFrame::lanesSelector::onCmdClearSelection(FXObject*, FXSelector, void*) {
    for (int i = 0; i < myList->getNumItems(); i++) {
        if (myList->getItem(i)->isSelected()) {
            myList->deselectItem(i);
        }
    }
    return 1;
}


long
GNECrossingFrame::lanesSelector::onCmdInvertSelection(FXObject*, FXSelector, void*) {
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
GNECrossingFrame::lanesSelector::onCmdHelp(FXObject*, FXSelector, void*) {
    return 1;
}

/****************************************************************************/
