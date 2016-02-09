/****************************************************************************/
/// @file    GNEAdditionalFrame.cpp
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
#include "GNEAdditionalFrame.h"
#include "GNEViewNet.h"
#include "GNENet.h"
#include "GNEJunction.h"
#include "GNEEdge.h"
#include "GNELane.h"
#include "GNEUndoList.h"
#include "GNEChange_Selection.h"
#include "GNEAttributeCarrier.h"
#include "GNEChange_Additional.h"
#include "GNEBusStop.h"
#include "GNEChargingStation.h"
#include "GNEDetectorE1.h"
#include "GNEDetectorE2.h"
#include "GNEDetectorE3.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// FOX callback mapping
// ===========================================================================
FXDEFMAP(GNEAdditionalFrame) GNEAdditionalMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_MODE_ADDITIONAL_ITEM,           GNEAdditionalFrame::onCmdSelectAdditional),
};

FXDEFMAP(GNEAdditionalFrame::additionalParameterList) additionalParameterListMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_ADDROW,     GNEAdditionalFrame::additionalParameterList::onCmdAddRow),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_REMOVEROW,  GNEAdditionalFrame::additionalParameterList::onCmdRemoveRow),
};

FXDEFMAP(GNEAdditionalFrame::editorParameter) editorParameterMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_MODE_ADDITIONAL_REFERENCEPOINT, GNEAdditionalFrame::editorParameter::onCmdSelectReferencePoint),
};

FXDEFMAP(GNEAdditionalFrame::additionalSet) additionalSetMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_ADDSET,     GNEAdditionalFrame::additionalSet::onCmdAddSet),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_REMOVESET,  GNEAdditionalFrame::additionalSet::onCmdRemoveSet),
};

// Object implementation
FXIMPLEMENT(GNEAdditionalFrame, FXScrollWindow, GNEAdditionalMap, ARRAYNUMBER(GNEAdditionalMap))
FXIMPLEMENT(GNEAdditionalFrame::additionalParameterList, FXMatrix, additionalParameterListMap, ARRAYNUMBER(additionalParameterListMap))
FXIMPLEMENT(GNEAdditionalFrame::editorParameter, FXGroupBox, editorParameterMap, ARRAYNUMBER(editorParameterMap))
FXIMPLEMENT(GNEAdditionalFrame::additionalSet, FXGroupBox, additionalSetMap, ARRAYNUMBER(additionalSetMap))

// ===========================================================================
// static members
// ===========================================================================
const int GNEAdditionalFrame::WIDTH = 140;
const int GNEAdditionalFrame::maxNumberOfParameters = 20;
const int GNEAdditionalFrame::maxNumberOfListParameters = 5;

// ===========================================================================
// method definitions
// ===========================================================================
GNEAdditionalFrame::GNEAdditionalFrame(FXComposite* parent, GNEViewNet* updateTarget, GNEUndoList* undoList):
    FXScrollWindow(parent, LAYOUT_FILL_Y | LAYOUT_FIX_WIDTH, 0, 0, WIDTH, 0),
    myHeaderFont(new FXFont(getApp(), "Arial", 14, FXFont::Bold)),
    myUpdateTarget(updateTarget),
    myActualAdditionalType(GNE_ADDITIONAL_BUSSTOP),
    myUndoList(undoList) {
    // Create frame
    myContentFrame = new FXVerticalFrame(this, LAYOUT_FILL);
    
    // Create titel frame
    myFrameLabel = new FXLabel(myContentFrame, "Additionals", 0, JUSTIFY_LEFT | LAYOUT_FILL_X);
    myFrameLabel->setFont(myHeaderFont);
    
    // Create groupBox for myAdditionalMatchBox 
    groupBoxForMyAdditionalMatchBox = new FXGroupBox(myContentFrame, "Additional element", GROUPBOX_TITLE_CENTER | FRAME_GROOVE | LAYOUT_FILL_X, 0, 0, 0, 0, 0, 0, 0, 0);
    
    // Create FXListBox in groupBoxForMyAdditionalMatchBox
    myAdditionalMatchBox = new FXComboBox(groupBoxForMyAdditionalMatchBox, 12, this, MID_GNE_MODE_ADDITIONAL_ITEM, 
                                          FRAME_SUNKEN | LAYOUT_LEFT | LAYOUT_TOP | COMBOBOX_STATIC | LAYOUT_CENTER_Y | LAYOUT_FILL_X);

    // Create groupBox for parameters 
    groupBoxForParameters = new FXGroupBox(myContentFrame, "Default parameters", GROUPBOX_TITLE_CENTER | FRAME_GROOVE | LAYOUT_FILL_X, 0, 0, 0, 0, 0, 0, 0, 0);
    
    // Create widgets for parameters
    for (int i = 0; i < maxNumberOfParameters; i++)
        myVectorOfAdditionalParameter.push_back(new additionalParameter(groupBoxForParameters, this));

    // Create widgets for list parameters
    for (int i = 0; i < maxNumberOfListParameters; i++)
        myVectorOfadditionalParameterList.push_back(new additionalParameterList(groupBoxForParameters, this));

    // Create editor parameter
    myEditorParameter = new GNEAdditionalFrame::editorParameter(myContentFrame, this);

    // Create additional Set
    myAdditionalSet = new GNEAdditionalFrame::additionalSet(myContentFrame, this);

    // Add options to myAdditionalMatchBox
    myAdditionalMatchBox->appendItem("Bus stop");
    myAdditionalMatchBox->appendItem("Charging station");
    myAdditionalMatchBox->appendItem("Detector E1");
    myAdditionalMatchBox->appendItem("Detector E2");
    myAdditionalMatchBox->appendItem("Detector E3");
    myAdditionalMatchBox->appendItem("Rerouters");
    myAdditionalMatchBox->appendItem("Calibrators");
    myAdditionalMatchBox->appendItem("VSpeed signals");

    // Set visible items
    myAdditionalMatchBox->setNumVisible((int)myAdditionalMatchBox->getNumItems()); 

    // By default, myActualAdditionalType is busStop
    myActualAdditionalType = GNE_ADDITIONAL_BUSSTOP;
}


GNEAdditionalFrame::~GNEAdditionalFrame() {
    delete myHeaderFont;
    gSelected.remove2Update();
}


void
GNEAdditionalFrame::addAdditional(GNELane &lane, GUISUMOAbstractView* parent) {
    // Position of the mouse in the lane
    SUMOReal position = lane.getShape().nearest_offset_to_point2D(parent->getPositionInformation());
    // Add adittional element depending of myActualAdditionalType
    switch (myActualAdditionalType) {
        case GNE_ADDITIONAL_BUSSTOP: {
            int numberOfBusStops = myUpdateTarget->getNet()->getNumberOfBusStops();
            // Check that the ID of the new busStop is unique
            while(myUpdateTarget->getNet()->getBusStop("busStop" + toString(numberOfBusStops)) != NULL)
                numberOfBusStops++;
            // Declare position of busStop
            SUMOReal startPosition;
            SUMOReal endPosition;
            // If positions are valid
            if(setPositions(lane, position, startPosition, endPosition)) {
                // Extract bus lines
                std::vector<std::string> lines = myVectorOfadditionalParameterList.at(0)->getVectorOfTextValues();
                // Create an add new busStop
                GNEBusStop *busStop = new GNEBusStop("busStop" + toString(numberOfBusStops), lane, myUpdateTarget, startPosition, endPosition, lines);
                busStop->setBlocked(myEditorParameter->isBlockEnabled());
                myUndoList->p_begin("add " + busStop->getDescription());
                myUndoList->add(new GNEChange_Additional(myUpdateTarget->getNet(), busStop, true), true);
                myUndoList->p_end();
            } 
        }
        break;
        case GNE_ADDITIONAL_CHARGINGSTATION: {
                int numberOfChargingStations = myUpdateTarget->getNet()->getNumberOfChargingStations();
                // Check that the ID of the new chargingStation is unique
                while(myUpdateTarget->getNet()->getChargingStation("chargingStation" + toString(numberOfChargingStations)) != NULL)
                    numberOfChargingStations++;
                // Declare position of chargingStation
                SUMOReal startPosition;
                SUMOReal endPosition;
                // If positions are valid
                if(setPositions(lane, position, startPosition, endPosition)) {
                    // Get values of text fields
                    SUMOReal chargingPower = GNEAttributeCarrier::parse<SUMOReal>(myVectorOfAdditionalParameter.at(1)->getTextValue());
                    SUMOReal chargingEfficiency = GNEAttributeCarrier::parse<SUMOReal>(myVectorOfAdditionalParameter.at(2)->getTextValue());
                    int chargeDelay = GNEAttributeCarrier::parse<int>(myVectorOfAdditionalParameter.at(3)->getTextValue());
                    bool chargeInTransit = myVectorOfAdditionalParameter.at(4)->getBoolValue();
                    // Create and add a new chargingStation
                    GNEChargingStation *chargingStation = new GNEChargingStation("chargingStation" + toString(numberOfChargingStations), lane, myUpdateTarget, startPosition, endPosition, chargingPower, chargingEfficiency, chargeInTransit, chargeDelay);
                    chargingStation->setBlocked(myEditorParameter->isBlockEnabled());
                    myUndoList->p_begin("add " + chargingStation->getDescription());
                    myUndoList->add(new GNEChange_Additional(myUpdateTarget->getNet(), chargingStation, true), true);
                    myUndoList->p_end();
                }
            }
            break;
        case GNE_ADDITIONAL_E1: {
                int numberOfDetectorE1 = myUpdateTarget->getNet()->getNumberOfDetectorE1();
                // Check that the ID of the new detector E1 is unique
                while(myUpdateTarget->getNet()->getdetectorE1("detectorE1_" + toString(numberOfDetectorE1)) != NULL)
                    numberOfDetectorE1++;

                /** CAMBIAR **/
                // Get values of text fields
                SUMOReal chargingPower = GNEAttributeCarrier::parse<SUMOReal>(myVectorOfAdditionalParameter.at(1)->getTextValue());
                SUMOReal chargingEfficiency = GNEAttributeCarrier::parse<SUMOReal>(myVectorOfAdditionalParameter.at(2)->getTextValue());
                int chargeDelay = GNEAttributeCarrier::parse<int>(myVectorOfAdditionalParameter.at(3)->getTextValue());
                bool chargeInTransit = myVectorOfAdditionalParameter.at(4)->getBoolValue();
                /**/


                // Create and add a new detector
                GNEDetectorE1 *detectorE1 = new GNEDetectorE1("detectorE1_" + toString(numberOfDetectorE1), lane, myUpdateTarget, position, 1, "", true);
                detectorE1->setBlocked(myEditorParameter->isBlockEnabled());
                myUndoList->p_begin("add " + detectorE1->getDescription());
                myUndoList->add(new GNEChange_Additional(myUpdateTarget->getNet(), detectorE1, true), true);
                myUndoList->p_end();
            }
            break;
        case GNE_ADDITIONAL_E2: {
                /** Finish **/
            }
            break;
        case GNE_ADDITIONAL_E3: {
                /** Finish **/
            }
            break;
        case GNE_ADDITIONAL_REROUTERS: {
                /** Finish **/
            }
            break;
        case GNE_ADDITIONAL_CALIBRATORS: {
                /** Finish **/
            
            }
            break;
        case GNE_ADDITIONAL_VARIABLESPEEDSIGNS: {
                /** Finish **/
            }
            break;
        default:
            throw ProcessError("Tag of additional object not supported");
            break;
    }
}

void 
GNEAdditionalFrame::removeAdditional(GNEAdditional *additional) {
    myUndoList->p_begin("delete " + additional->getDescription());
    myUndoList->add(new GNEChange_Additional(myUpdateTarget->getNet(), additional, false), true);
    myUndoList->p_end();
}

FXFont* 
GNEAdditionalFrame::getHeaderFont() {
    return myHeaderFont;
}


long
GNEAdditionalFrame::onCmdSelectAdditional(FXObject*, FXSelector, void*) {
    // Cast actual additional type
    myActualAdditionalType = static_cast<additionalType>(myAdditionalMatchBox->getCurrentItem());
    // Set parameters
    setParameters();
    return 1;
}


void
GNEAdditionalFrame::show() {
    // Set parameters
    setParameters();
    // Show Scroll window
    FXScrollWindow::show();
}


void
GNEAdditionalFrame::hide() {
    // Hide ScrollWindow
    FXScrollWindow::hide();
}


void 
GNEAdditionalFrame::setParameters() {
    // Hide all parameters
    for(int i = 0; i < maxNumberOfParameters; i++)
        myVectorOfAdditionalParameter.at(i)->hideParameter();
    for(int i = 0; i < maxNumberOfListParameters; i++)
        myVectorOfadditionalParameterList.at(i)->hideParameter();

    // Obtain options
    OptionsCont &oc = OptionsCont::getOptions();

    // Set parameters depending of myActualAdditionalType
    switch (myActualAdditionalType) {
        case GNE_ADDITIONAL_BUSSTOP : {
            myVectorOfAdditionalParameter.at(0)->showTextParameter("size", oc.getString("busStop default length").c_str());
            std::string defaultLinesWithoutParse = oc.getString("busStop default lines");
            std::vector<std::string> lines;
            SUMOSAXAttributes::parseStringVector(defaultLinesWithoutParse, lines);
            if(lines.empty())
                lines.push_back("");
            myVectorOfadditionalParameterList.at(0)->showListParameter("line", lines);
            }
            break;
        case GNE_ADDITIONAL_CHARGINGSTATION :
            myVectorOfAdditionalParameter.at(0)->showTextParameter("size", oc.getString("chargingStation default length").c_str());   
            myVectorOfAdditionalParameter.at(1)->showTextParameter("power", oc.getString("chargingStation default charging power").c_str());   
            myVectorOfAdditionalParameter.at(2)->showTextParameter("effic.", oc.getString("chargingStation default efficiency").c_str());   
            myVectorOfAdditionalParameter.at(3)->showTextParameter("delay", oc.getString("chargingStation default charge delay").c_str());   
            myVectorOfAdditionalParameter.at(4)->showBoolParameter("transit", oc.getBool("chargingStation default charge in transit"));   
            break;
        default:
            break;
    }
    // Recalc groupBox
    groupBoxForParameters->recalc();
}


bool 
GNEAdditionalFrame::setPositions(GNELane &lane, SUMOReal &positionOfTheMouseOverLane, SUMOReal &startPosition, SUMOReal &endPosition) {
    SUMOReal size = GNEAttributeCarrier::parse<SUMOReal>(myVectorOfAdditionalParameter.at(0)->getTextValue());
    switch (myEditorParameter->getActualReferencePoint()) {
        case GNE_ADDITIONALREFERENCEPOINT_LEFT : {
            startPosition = positionOfTheMouseOverLane;
            // Set end position
            if(positionOfTheMouseOverLane + size <= lane.getLaneShapeLenght() - 0.01)
                endPosition = positionOfTheMouseOverLane + size;
            else if(myEditorParameter->isForcePositionEnabled())
                endPosition = lane.getLaneShapeLenght() - 0.01;
            else
                return false;
            break;
        }
        case GNE_ADDITIONALREFERENCEPOINT_RIGHT : {
            endPosition = positionOfTheMouseOverLane;
            // Set end position
            if(positionOfTheMouseOverLane - size >= 0.01)
                 startPosition = positionOfTheMouseOverLane - size;
            else if(myEditorParameter->isForcePositionEnabled())
                endPosition = 0.01;
            else
                return false;
        }
        break;
        case GNE_ADDITIONALREFERENCEPOINT_CENTER : {
            // Set startPosition
            if(positionOfTheMouseOverLane - size/2 >= 0.01)
                startPosition = positionOfTheMouseOverLane - size/2;
            else if(myEditorParameter->isForcePositionEnabled())
                startPosition = 0;
            else
                return false;
            // Set endPosition
            if(positionOfTheMouseOverLane + size/2 <= lane.getLaneShapeLenght() - 0.01)
                endPosition = positionOfTheMouseOverLane + size/2;
            else if(myEditorParameter->isForcePositionEnabled())
                endPosition = lane.getLaneShapeLenght() - 0.01;
            else
                return false;
        }
        break;
    }
    // check if the size of busStop/ChargingStation is enough (>= 1)
    if(endPosition - startPosition >= 1)
        return true;
    else
        return false;
}


GNEAdditionalFrame::additionalParameter::additionalParameter(FXComposite *parent, FXObject* tgt) :
    FXMatrix(parent, 3, MATRIX_BY_COLUMNS | LAYOUT_FILL_X, 0, 0, 0, 0, 0, 0, 0, 0) {
    // Create elements 
    myLabel = new FXLabel(this, "name", 0, JUSTIFY_RIGHT | LAYOUT_FIX_WIDTH, 0, 0, 60, 0);
    myTextField = new FXTextField(this, 10, tgt, MID_GNE_MODE_ADDITIONAL_CHANGEPARAMETER_TEXT, LAYOUT_FILL_COLUMN | LAYOUT_FILL_X);
    myMenuCheck = new FXMenuCheck(this, "", tgt, MID_GNE_MODE_ADDITIONAL_CHANGEPARAMETER_BOOL, LAYOUT_FIX_WIDTH);
    // Set widht of menuCheck manually
    myMenuCheck->setWidth(20);
    // Hide elements
    hideParameter();
}


GNEAdditionalFrame::additionalParameter::~additionalParameter() {}


void 
GNEAdditionalFrame::additionalParameter::showTextParameter(const std::string& name, const std::string &value) {
    myLabel->setText(name.c_str());
    myLabel->show();
    myTextField->setText(value.c_str());
    myTextField->show();
    show();
}


void 
GNEAdditionalFrame::additionalParameter::showBoolParameter(const std::string& name, bool value) {
    myLabel->setText(name.c_str());
    myLabel->show();
    myMenuCheck->setCheck(value);
    myMenuCheck->show();
    show();
}


void 
GNEAdditionalFrame::additionalParameter::hideParameter() {
    myLabel->hide();
    myTextField->hide();
    myMenuCheck->hide();
    hide();
}


std::string 
GNEAdditionalFrame::additionalParameter::getTextValue() {
    return myTextField->getText().text();
}


bool
GNEAdditionalFrame::additionalParameter::getBoolValue() {
    return (myMenuCheck->getCheck() == 1)? true : false;
}


GNEAdditionalFrame::additionalParameterList::additionalParameterList(FXComposite *parent, FXObject* tgt, int maxNumberOfValuesInParameterList) :
    FXMatrix(parent, 2, MATRIX_BY_COLUMNS | LAYOUT_FILL_X, 0, 0, 0, 0, 0, 0, 0, 0),
    numberOfVisibleTextfields(1),
    myMaxNumberOfValuesInParameterList(maxNumberOfValuesInParameterList) {
    // Create elements
    for(int i = 0; i < myMaxNumberOfValuesInParameterList; i++) {
        myLabels.push_back(new FXLabel(this, "name", 0, JUSTIFY_RIGHT | LAYOUT_FIX_WIDTH, 0, 0, 60, 0));
        myTextFields.push_back(new FXTextField(this, 10, tgt, MID_GNE_MODE_ADDITIONAL_CHANGEPARAMETER_TEXT, LAYOUT_FILL_COLUMN | LAYOUT_FILL_X));
    }

    myLabels.push_back(new FXLabel(this, "Rows", 0, JUSTIFY_RIGHT | LAYOUT_FIX_WIDTH, 0, 0, 60, 0));
    FXHorizontalFrame *buttonsFrame = new FXHorizontalFrame(this, LAYOUT_FILL_COLUMN | LAYOUT_FILL_X, 0, 0, 0, 0, 0, 0, 0, 0);
    
    add = new FXButton(buttonsFrame, "", GUIIconSubSys::getIcon(ICON_ADD), this, MID_GNE_ADDROW,
        ICON_BEFORE_TEXT | LAYOUT_FIX_WIDTH | LAYOUT_FIX_HEIGHT | FRAME_THICK | FRAME_RAISED,
        0, 0, 20, 20);

    remove = new FXButton(buttonsFrame, "", GUIIconSubSys::getIcon(ICON_REMOVE), this, MID_GNE_REMOVEROW,
        ICON_BEFORE_TEXT | LAYOUT_FIX_WIDTH | LAYOUT_FIX_HEIGHT | FRAME_THICK | FRAME_RAISED,
        0, 0, 20, 20);
}


GNEAdditionalFrame::additionalParameterList::~additionalParameterList() {}


void 
GNEAdditionalFrame::additionalParameterList::showListParameter(const std::string& name, std::vector<std::string> value) {
    if(value.size() < myMaxNumberOfValuesInParameterList) {
        numberOfVisibleTextfields = (int)value.size();
        for(int i = 0; i < myMaxNumberOfValuesInParameterList; i++) 
            myLabels.at(i)->setText((name + " " + toString(i)).c_str());
        for(int i = 0; i < numberOfVisibleTextfields; i++) {
            myLabels.at(i)->show();
            myTextFields.at(i)->show();
        }
        add->show();
        remove->show();
        show();
    }
}


void 
GNEAdditionalFrame::additionalParameterList::hideParameter() {
    
    for(int i = 0; i < myMaxNumberOfValuesInParameterList; i++) {
        myLabels.at(i)->hide();
        myTextFields.at(i)->hide();
        myTextFields.at(i)->setText("");
    }
    add->hide();
    remove->hide();
    hide();
}


std::vector<std::string> 
GNEAdditionalFrame::additionalParameterList::getVectorOfTextValues() {
    // Declare, fill and return a list of string
    std::vector<std::string> listOfStrings;
    for(int i = 0; i < myMaxNumberOfValuesInParameterList; i++)
        if(myTextFields.at(i)->getText() != "")
            listOfStrings.push_back(myTextFields.at(i)->getText().text());
    return listOfStrings;
}


long 
GNEAdditionalFrame::additionalParameterList::onCmdAddRow(FXObject*, FXSelector, void*) {
    if(numberOfVisibleTextfields < (myMaxNumberOfValuesInParameterList-1)) {
        numberOfVisibleTextfields++;
        myLabels.at(numberOfVisibleTextfields)->show();
        myTextFields.at(numberOfVisibleTextfields)->show();
        getParent()->recalc();
    }
    return 1;
}


long 
GNEAdditionalFrame::additionalParameterList::onCmdRemoveRow(FXObject*, FXSelector, void*) {
    if(numberOfVisibleTextfields > 0) {
        myLabels.at(numberOfVisibleTextfields)->hide();
        myTextFields.at(numberOfVisibleTextfields)->hide();
        myTextFields.at(numberOfVisibleTextfields)->setText("");
        numberOfVisibleTextfields--;
        getParent()->recalc();
    }
    return 1;
}


GNEAdditionalFrame::editorParameter::editorParameter(FXComposite *parent, FXObject* tgt) :
    FXGroupBox(parent, "editor parameters", GROUPBOX_TITLE_CENTER | FRAME_GROOVE | LAYOUT_FILL_X, 0, 0, 0, 0, 0, 0, 0, 0),
    myActualAdditionalReferencePoint(GNE_ADDITIONALREFERENCEPOINT_LEFT) {

    // Create FXListBox for the reference points
    myReferencePointMatchBox = new FXComboBox(this, 12, this, MID_GNE_MODE_ADDITIONAL_REFERENCEPOINT,
                                              FRAME_SUNKEN | LAYOUT_LEFT | LAYOUT_TOP | COMBOBOX_STATIC | LAYOUT_FILL_X);

    // Create FXMenuCheck for the force option
    myCheckForcePosition = new FXMenuCheck(this, "Force position", this, MID_GNE_MODE_ADDITIONAL_FORCEPOSITION,
                                           LAYOUT_LEFT | LAYOUT_TOP | LAYOUT_FILL_X);

    // Create FXMenuCheck for the force option
    myCheckBlock = new FXMenuCheck(this, "Block movement", this, MID_GNE_SET_BLOCKING,
                                   LAYOUT_LEFT | LAYOUT_TOP | LAYOUT_FILL_X);

    // Add options to myReferencePointMatchBox
    myReferencePointMatchBox->appendItem("reference left");
    myReferencePointMatchBox->appendItem("reference right");
    myReferencePointMatchBox->appendItem("reference center");

    // Set visible items
    myReferencePointMatchBox->setNumVisible((int)myReferencePointMatchBox->getNumItems());
}


GNEAdditionalFrame::editorParameter::~editorParameter() {}


GNEAdditionalFrame::additionalReferencePoint 
GNEAdditionalFrame::editorParameter::getActualReferencePoint() {
    return myActualAdditionalReferencePoint;
}


bool 
GNEAdditionalFrame::editorParameter::isBlockEnabled() {
    return myCheckBlock->getCheck() == 1? true : false;
}


bool 
GNEAdditionalFrame::editorParameter::isForcePositionEnabled() {
    return myCheckForcePosition->getCheck() == 1? true : false;
}


long 
GNEAdditionalFrame::editorParameter::onCmdSelectReferencePoint(FXObject*, FXSelector, void*) {
    // Cast actual reference point type
    myActualAdditionalReferencePoint = static_cast<additionalReferencePoint>(myReferencePointMatchBox->getCurrentItem());
    return 1;
}


GNEAdditionalFrame::additionalSet::additionalSet(FXComposite *parent, FXObject* tgt) :
    FXGroupBox(parent, "Additional Set", GROUPBOX_TITLE_CENTER | FRAME_GROOVE | LAYOUT_FILL_X, 0, 0, 0, 0, 0, 0, 0, 0)
{

}


GNEAdditionalFrame::additionalSet::~additionalSet() {}


long 
GNEAdditionalFrame::additionalSet::onCmdAddSet(FXObject*, FXSelector, void*) {
    return 1;
}


long 
GNEAdditionalFrame::additionalSet::onCmdRemoveSet(FXObject*, FXSelector, void*) {
    return 1;
}

/****************************************************************************/
