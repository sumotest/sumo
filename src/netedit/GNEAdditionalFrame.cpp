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

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// FOX callback mapping
// ===========================================================================
FXDEFMAP(GNEAdditionalFrame) GNEAdditionalMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_MODE_ADDITIONAL_ITEM,           GNEAdditionalFrame::onCmdSelectAdditional),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_MODE_ADDITIONAL_REFERENCEPOINT, GNEAdditionalFrame::onCmdSelectReferencePoint),
};

FXDEFMAP(GNEAdditionalFrame::additionalParameterList) additionalParameterListMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_ADDROW,     GNEAdditionalFrame::additionalParameterList::onCmdAddRow),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_REMOVEROW,  GNEAdditionalFrame::additionalParameterList::onCmdRemoveRow),
};

// Object implementation
FXIMPLEMENT(GNEAdditionalFrame, FXScrollWindow, GNEAdditionalMap, ARRAYNUMBER(GNEAdditionalMap))
FXIMPLEMENT(GNEAdditionalFrame::additionalParameterList, FXHorizontalFrame, additionalParameterListMap, ARRAYNUMBER(additionalParameterListMap))

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
    myActualAdditionalReferencePoint(GNE_ADDITIONALREFERENCEPOINT_LEFT),
    myUndoList(undoList) {
    // Create frame
    myContentFrame = new FXVerticalFrame(this, LAYOUT_FILL_Y | LAYOUT_FIX_WIDTH, 0, 0, WIDTH, 0);
    
    // Create titel frame
    myFrameLabel = new FXLabel(myContentFrame, "Additionals", 0, JUSTIFY_LEFT);
    myFrameLabel->setFont(myHeaderFont);
    
    // Create groupBox for myAdditionalMatchBox 
    groupBoxForMyAdditionalMatchBox = new FXGroupBox(myContentFrame, "Additional element",
                                                      GROUPBOX_TITLE_CENTER | FRAME_GROOVE | LAYOUT_FILL_X);
    
    // Create FXListBox in groupBoxForMyAdditionalMatchBox
    myAdditionalMatchBox = new FXComboBox(groupBoxForMyAdditionalMatchBox, 12, this, MID_GNE_MODE_ADDITIONAL_ITEM, 
                                          FRAME_SUNKEN | LAYOUT_LEFT | LAYOUT_TOP | COMBOBOX_STATIC | LAYOUT_CENTER_Y | LAYOUT_FILL_X);

    // Create groupBox for parameters 
    groupBoxForParameters = new FXGroupBox(myContentFrame, "Default parameters",
                                           GROUPBOX_TITLE_CENTER | FRAME_GROOVE | LAYOUT_FILL_X);
    
    // Create widgets for parameters
    for (int i = 0; i < maxNumberOfParameters; i++)
        myVectorOfAdditionalParameter.push_back(new additionalParameter(groupBoxForParameters, this));

    // Create widgets for list parameters
    for (int i = 0; i < maxNumberOfListParameters; i++)
        myVectorOfAdditionalParameterList.push_back(new additionalParameterList(groupBoxForParameters, this));

    // Create groupBox for editor parameters
    myReferencePointBox = new FXGroupBox(myContentFrame, "editor",
                                         GROUPBOX_TITLE_CENTER | FRAME_GROOVE | LAYOUT_FILL_X);
    
    // Create FXListBox for the reference points
    myReferencePointMatchBox = new FXComboBox(myReferencePointBox, 12, this, MID_GNE_MODE_ADDITIONAL_REFERENCEPOINT,
                                              FRAME_SUNKEN | LAYOUT_LEFT | LAYOUT_TOP | COMBOBOX_STATIC | LAYOUT_FILL_X);

    // Create FXMenuCheck for the force option
    myCheckForcePosition = new FXMenuCheck(myReferencePointBox, "Force position", this, MID_GNE_MODE_ADDITIONAL_FORCEPOSITION,
                                           LAYOUT_LEFT | LAYOUT_TOP | LAYOUT_FILL_X);

    // Create FXMenuCheck for the force option
    myCheckBlock = new FXMenuCheck(myReferencePointBox, "Block movement", this, MID_GNE_SET_BLOCKING,
                                   LAYOUT_LEFT | LAYOUT_TOP);

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

    // Add options to myReferencePointMatchBox
    myReferencePointMatchBox->appendItem("reference left");
    myReferencePointMatchBox->appendItem("reference right");
    myReferencePointMatchBox->appendItem("reference center");

    // Set visible items
    myReferencePointMatchBox->setNumVisible((int)myReferencePointMatchBox->getNumItems());

    // By default, reference point is left
    myActualAdditionalReferencePoint = GNE_ADDITIONALREFERENCEPOINT_LEFT;
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
                std::vector<std::string> lines = myVectorOfAdditionalParameterList.at(1)->getVectorOfTextValues();
                
                for(int i = 0; i < lines.size(); i++)
                    std::cout << lines[i] << " ";
                // Create an add new busStop
                GNEBusStop *busStop = new GNEBusStop("busStop" + toString(numberOfBusStops), lane, myUpdateTarget, startPosition, endPosition, lines);
                myUndoList->p_begin("add " + busStop->getDescription());
                myUndoList->add(new GNEChange_Additional(myUpdateTarget->getNet(), busStop, true), true);
                myUndoList->p_end();
            } 
        }
        break;
        case GNE_ADDITIONAL_CHARGINGSTATION: {
                int numberOfBusStops = myUpdateTarget->getNet()->getNumberOfChargingStations();
                // Check that the ID of the new chargingStation is unique
                while(myUpdateTarget->getNet()->getBusStop("chargingStation" + toString(numberOfBusStops)) != NULL)
                    numberOfBusStops++;
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
                    // Create an add new chargingStation
                    GNEChargingStation *chargingStation = new GNEChargingStation("chargingStation" + toString(numberOfBusStops), lane, myUpdateTarget, startPosition, endPosition, chargingPower, chargingEfficiency, chargeInTransit, chargeDelay);
                    myUndoList->p_begin("add " + chargingStation->getDescription());
                    myUndoList->add(new GNEChange_Additional(myUpdateTarget->getNet(), chargingStation, true), true);
                    myUndoList->p_end();
                }
            }
            
            break;
        case GNE_ADDITIONAL_E1: {
                /** Finish **/
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


long 
GNEAdditionalFrame::onCmdSelectReferencePoint(FXObject*, FXSelector, void*) {
    // Cast actual reference point type
    myActualAdditionalReferencePoint = static_cast<additionalReferencePoint>(myReferencePointMatchBox->getCurrentItem());
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
        myVectorOfAdditionalParameterList.at(i)->hideParameter();

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
            myVectorOfAdditionalParameterList.at(0)->showListParameter("lines", lines);
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
    switch (myActualAdditionalReferencePoint) {
        case GNE_ADDITIONALREFERENCEPOINT_LEFT : {
            startPosition = positionOfTheMouseOverLane;
            // Set end position
            if(positionOfTheMouseOverLane + size <= lane.getLaneShapeLenght() - 0.01)
                endPosition = positionOfTheMouseOverLane + size;
            else if(myCheckForcePosition->getCheck())
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
            else if(myCheckForcePosition->getCheck())
                endPosition = 0.01;
            else
                return false;
        }
        break;
        case GNE_ADDITIONALREFERENCEPOINT_CENTER : {
            // Set startPosition
            if(positionOfTheMouseOverLane - size/2 >= 0.01)
                startPosition = positionOfTheMouseOverLane - size/2;
            else if(myCheckForcePosition->getCheck())
                startPosition = 0;
            else
                return false;
            // Set endPosition
            if(positionOfTheMouseOverLane + size/2 <= lane.getLaneShapeLenght() - 0.01)
                endPosition = positionOfTheMouseOverLane + size/2;
            else if(myCheckForcePosition->getCheck())
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
    FXHorizontalFrame(parent) {
    // Create elements
    myLabel = new FXLabel(this, "name", 0, JUSTIFY_RIGHT | LAYOUT_FIX_WIDTH, 0, 0, 40, 0);
    myTextField = new FXTextField(this, 10, tgt, MID_GNE_MODE_ADDITIONAL_CHANGEPARAMETER_TEXT);
    myMenuCheck = new FXMenuCheck(this, "", tgt, MID_GNE_MODE_ADDITIONAL_CHANGEPARAMETER_BOOL);
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
    FXHorizontalFrame(parent),
    numberOfVisibleTextfields(1),
    myMaxNumberOfValuesInParameterList(maxNumberOfValuesInParameterList) {
    // Create elements
    myLabel = new FXLabel(this, "name", 0, JUSTIFY_RIGHT | LAYOUT_FIX_WIDTH, 0, 0, 40, 0);
    FXVerticalFrame *myVerticalFrame = new FXVerticalFrame(this);
    for(int i = 0; i < myMaxNumberOfValuesInParameterList; i++)
        myTextFields.push_back(new FXTextField(myVerticalFrame, 10, tgt, MID_GNE_MODE_ADDITIONAL_CHANGEPARAMETER_TEXT));

    FXHorizontalFrame *buttonsFrame = new FXHorizontalFrame(myVerticalFrame);
    add = new FXButton(buttonsFrame, " ", GUIIconSubSys::getIcon(ICON_OPEN_ADDITIONALS), this, MID_GNE_ADDROW,
        ICON_BEFORE_TEXT | LAYOUT_FIX_WIDTH | LAYOUT_FIX_HEIGHT | FRAME_THICK | FRAME_RAISED,
        0, 0, 30, 30);

    remove = new FXButton(buttonsFrame, " ", GUIIconSubSys::getIcon(ICON_OPEN_ADDITIONALS), this, MID_GNE_REMOVEROW,
        ICON_BEFORE_TEXT | LAYOUT_FIX_WIDTH | LAYOUT_FIX_HEIGHT | FRAME_THICK | FRAME_RAISED,
        0, 0, 30, 30);
    }


GNEAdditionalFrame::additionalParameterList::~additionalParameterList() {}


void 
GNEAdditionalFrame::additionalParameterList::showListParameter(const std::string& name, std::vector<std::string> value) {
    myLabel->setText(name.c_str());
    myLabel->show();
    if(value.size() < myMaxNumberOfValuesInParameterList) {
        numberOfVisibleTextfields = (int)value.size();
        for(int i = 0; i < numberOfVisibleTextfields; i++)
            myTextFields.at(i)->show();
        add->show();
        remove->show();
        show();
    }
}


void 
GNEAdditionalFrame::additionalParameterList::hideParameter() {
    myLabel->hide();
    for(int i = 0; i < myMaxNumberOfValuesInParameterList; i++) {
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
        if(myTextFields.at(i)->getText() != "") {
            listOfStrings.push_back(myTextFields.at(i)->getText().text());
    return listOfStrings;
}


long 
GNEAdditionalFrame::additionalParameterList::onCmdAddRow(FXObject*, FXSelector, void*) {
    if(numberOfVisibleTextfields < (myMaxNumberOfValuesInParameterList-1)) {
        numberOfVisibleTextfields++;
        myTextFields.at(numberOfVisibleTextfields)->show();
        getParent()->recalc();
    }
    return 1;
}


long 
GNEAdditionalFrame::additionalParameterList::onCmdRemoveRow(FXObject*, FXSelector, void*) {
    if(numberOfVisibleTextfields > 0) {
        myTextFields.at(numberOfVisibleTextfields)->hide();
        myTextFields.at(numberOfVisibleTextfields)->setText("");
        numberOfVisibleTextfields--;
        getParent()->recalc();
    }
    return 1;
}


/****************************************************************************/