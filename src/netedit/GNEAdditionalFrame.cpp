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
    FXMAPFUNC(SEL_COMMAND, MID_GNE_MODE_ADDITIONAL_ITEM, GNEAdditionalFrame::onCmdSelectAdditional),
};

FXDEFMAP(GNEAdditionalFrame::additionalParameterList) GNEAdditionalParameterListMap[] = {
    FXMAPFUNC(SEL_COMMAND, MID_GNE_ADDROW,    GNEAdditionalFrame::additionalParameterList::onCmdAddRow),
    FXMAPFUNC(SEL_COMMAND, MID_GNE_REMOVEROW, GNEAdditionalFrame::additionalParameterList::onCmdRemoveRow),
};

FXDEFMAP(GNEAdditionalFrame::editorParameter) GNEEditorParameterMap[] = {
    FXMAPFUNC(SEL_COMMAND, MID_GNE_MODE_ADDITIONAL_REFERENCEPOINT, GNEAdditionalFrame::editorParameter::onCmdSelectReferencePoint),
    FXMAPFUNC(SEL_COMMAND, MID_HELP,                               GNEAdditionalFrame::editorParameter::onCmdHelp),
};

FXDEFMAP(GNEAdditionalFrame::additionalSet) GNEAdditionalSetMap[] = {
    FXMAPFUNC(SEL_COMMAND, MID_GNE_SELECTSET, GNEAdditionalFrame::additionalSet::onCmdSelectSet),
    FXMAPFUNC(SEL_COMMAND, MID_HELP,          GNEAdditionalFrame::additionalSet::onCmdHelp),
};

// Object implementation
FXIMPLEMENT(GNEAdditionalFrame,                          FXScrollWindow, GNEAdditionalMap,              ARRAYNUMBER(GNEAdditionalMap))
FXIMPLEMENT(GNEAdditionalFrame::additionalParameterList, FXMatrix,       GNEAdditionalParameterListMap, ARRAYNUMBER(GNEAdditionalParameterListMap))
FXIMPLEMENT(GNEAdditionalFrame::editorParameter,         FXGroupBox,     GNEEditorParameterMap,         ARRAYNUMBER(GNEEditorParameterMap))
FXIMPLEMENT(GNEAdditionalFrame::additionalSet,           FXGroupBox,     GNEAdditionalSetMap,           ARRAYNUMBER(GNEAdditionalSetMap))

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
    myViewNet(updateTarget),
    myActualAdditionalType(SUMO_TAG_BUS_STOP),
    myUndoList(undoList) {
    // Create frame
    myContentFrame = new FXVerticalFrame(this, LAYOUT_FILL);
    
    // Create titel frame
    myFrameLabel = new FXLabel(myContentFrame, "Additionals", 0, JUSTIFY_LEFT | LAYOUT_FILL_X);
    myFrameLabel->setFont(myHeaderFont);
    
    // Create groupBox for myAdditionalMatchBox 
    myGroupBoxForMyAdditionalMatchBox = new FXGroupBox(myContentFrame, "Additional element", GROUPBOX_TITLE_CENTER | FRAME_GROOVE | LAYOUT_FILL_X, 0, 0, 0, 0, 0, 0, 0, 0);
    
    // Create FXListBox in myGroupBoxForMyAdditionalMatchBox
    myAdditionalMatchBox = new FXComboBox(myGroupBoxForMyAdditionalMatchBox, 12, this, MID_GNE_MODE_ADDITIONAL_ITEM, 
                                          FRAME_SUNKEN | LAYOUT_LEFT | LAYOUT_TOP | COMBOBOX_STATIC | LAYOUT_CENTER_Y | LAYOUT_FILL_X);

    // Create groupBox for parameters 
    myGroupBoxForParameters = new FXGroupBox(myContentFrame, "Default parameters", GROUPBOX_TITLE_CENTER | FRAME_GROOVE | LAYOUT_FILL_X, 0, 0, 0, 0, 0, 0, 0, 0);
    
    // Create widgets for parameters
    for (int i = 0; i < maxNumberOfParameters; i++)
        myVectorOfAdditionalParameter.push_back(new additionalParameter(myGroupBoxForParameters, this));

    // Create widgets for list parameters
    for (int i = 0; i < maxNumberOfListParameters; i++)
        myVectorOfadditionalParameterList.push_back(new additionalParameterList(myGroupBoxForParameters, this));

    // Create editor parameter
    myEditorParameter = new GNEAdditionalFrame::editorParameter(myContentFrame, this);

    // Create additional Set
    myAdditionalSet = new GNEAdditionalFrame::additionalSet(myContentFrame, this, myViewNet);

    // Add options to myAdditionalMatchBox
    myAdditionalMatchBox->appendItem("Bus stop");
    myAdditionalMatchBox->appendItem("Charging station");
    myAdditionalMatchBox->appendItem("Detector E1");
    myAdditionalMatchBox->appendItem("Detector E2");
    myAdditionalMatchBox->appendItem("Detector E3");
    myAdditionalMatchBox->appendItem("Detector E3 Entry");
    myAdditionalMatchBox->appendItem("Detector E3 Exit");
    myAdditionalMatchBox->appendItem("Rerouters");
    myAdditionalMatchBox->appendItem("Calibrators");
    myAdditionalMatchBox->appendItem("VSpeed signals");

    // Set visible items
    myAdditionalMatchBox->setNumVisible((int)myAdditionalMatchBox->getNumItems()); 
}


GNEAdditionalFrame::~GNEAdditionalFrame() {
    delete myHeaderFont;
    gSelected.remove2Update();
}


bool
GNEAdditionalFrame::addAdditional(GNELane *lane, GUISUMOAbstractView* parent) {
    // If user clicked over lane, a GNEAdditional Element will be created. In other case, a GNEAdditionalSet will be created
    if(lane) {
        // Position of the mouse in the lane
        SUMOReal position = lane->getShape().nearest_offset_to_point2D(parent->getPositionInformation());
        // Add adittional element depending of myActualAdditionalType
        switch (myActualAdditionalType) {
            case SUMO_TAG_BUS_STOP: {
                    int numberOfBusStops = myViewNet->getNet()->getNumberOfBusStops();
                    // Check that the ID of the new busStop is unique
                    while(myViewNet->getNet()->getBusStop("busStop" + toString(numberOfBusStops)) != NULL)
                        numberOfBusStops++;
                    // Declare position of busStop
                    SUMOReal startPosition;
                    SUMOReal endPosition;
                    // If positions are valid
                    if(setPositions(lane, position, startPosition, endPosition)) {
                        // Extract bus lines
                        std::vector<std::string> lines = myVectorOfadditionalParameterList.at(0)->getVectorOfTextValues();
                        // Create an add new busStop
                        GNEBusStop *busStop = new GNEBusStop("busStop" + toString(numberOfBusStops), *lane, myViewNet, startPosition, endPosition, lines);
                        busStop->setBlocked(myEditorParameter->isBlockEnabled());
                        myUndoList->p_begin("add " + busStop->getDescription());
                        myUndoList->add(new GNEChange_Additional(myViewNet->getNet(), busStop, true), true);
                        myUndoList->p_end();
                        return true;
                    } else 
                        return false;
                }
            case SUMO_TAG_CHARGING_STATION: {
                    int numberOfChargingStations = myViewNet->getNet()->getNumberOfChargingStations();
                    // Check that the ID of the new chargingStation is unique
                    while(myViewNet->getNet()->getChargingStation("chargingStation" + toString(numberOfChargingStations)) != NULL)
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
                        GNEChargingStation *chargingStation = new GNEChargingStation("chargingStation" + toString(numberOfChargingStations), *lane, myViewNet, startPosition, endPosition, chargingPower, chargingEfficiency, chargeInTransit, chargeDelay);
                        chargingStation->setBlocked(myEditorParameter->isBlockEnabled());
                        myUndoList->p_begin("add " + chargingStation->getDescription());
                        myUndoList->add(new GNEChange_Additional(myViewNet->getNet(), chargingStation, true), true);
                        myUndoList->p_end();
                        return true;
                    } else 
                        return false;
                }
            case SUMO_TAG_E1DETECTOR: {
                    int numberOfDetectorE1 = myViewNet->getNet()->getNumberOfDetectorE1();
                    // Check that the ID of the new detector E1 is unique
                    while(myViewNet->getNet()->getdetectorE1("detectorE1_" + toString(numberOfDetectorE1)) != NULL)
                        numberOfDetectorE1++;

                    /** CAMBIAR **/
                    // Get values of text fields
                    SUMOReal chargingPower = GNEAttributeCarrier::parse<SUMOReal>(myVectorOfAdditionalParameter.at(1)->getTextValue());
                    SUMOReal chargingEfficiency = GNEAttributeCarrier::parse<SUMOReal>(myVectorOfAdditionalParameter.at(2)->getTextValue());
                    int chargeDelay = GNEAttributeCarrier::parse<int>(myVectorOfAdditionalParameter.at(3)->getTextValue());
                    bool chargeInTransit = myVectorOfAdditionalParameter.at(4)->getBoolValue();
                    /**/

                    // Create and add a new detector
                    GNEDetectorE1 *detectorE1 = new GNEDetectorE1("detectorE1_" + toString(numberOfDetectorE1), *lane, myViewNet, position, 1, "", true);
                    detectorE1->setBlocked(myEditorParameter->isBlockEnabled());
                    myUndoList->p_begin("add " + detectorE1->getDescription());
                    myUndoList->add(new GNEChange_Additional(myViewNet->getNet(), detectorE1, true), true);
                    myUndoList->p_end();
                    return true;
                }
            case SUMO_TAG_E2DETECTOR: {
                    // Finish
                    return true;
                }
            case SUMO_TAG_DET_ENTRY: {
                    // Finish
                    return true;
                }
            case SUMO_TAG_DET_EXIT: {
                    // Finish
                    return true;
                }
            case SUMO_TAG_REROUTER: {
                    // Finish
                    return true;
                }
            case SUMO_TAG_CALIBRATOR: {
                    // Finish
                    return true;
                }
            case SUMO_TAG_VSS: {
                    // Finish
                    return true;
                }
            default:
                WRITE_WARNING("An additionalSet element of type '" + toString(myActualAdditionalType) + "' cannot be placed over a Lane");
                return false;
        }
    } else {
        // Add adittionalSet element depending of myActualAdditionalType
        switch (myActualAdditionalType) {
            case SUMO_TAG_E3DETECTOR: {
                int numberOfDetectorE3 = myViewNet->getNet()->getNumberOfDetectorE3();
                // Check that the ID of the new detector E3 is unique
                while(myViewNet->getNet()->getdetectorE3("detectorE3_" + toString(numberOfDetectorE3)) != NULL)
                    numberOfDetectorE3++;

                /** CAMBIAR **
                // Get values of text fields
                SUMOReal chargingPower = GNEAttributeCarrier::parse<SUMOReal>(myVectorOfAdditionalParameter.at(1)->getTextValue());
                SUMOReal chargingEfficiency = GNEAttributeCarrier::parse<SUMOReal>(myVectorOfAdditionalParameter.at(2)->getTextValue());
                int chargeDelay = GNEAttributeCarrier::parse<int>(myVectorOfAdditionalParameter.at(3)->getTextValue());
                bool chargeInTransit = myVectorOfAdditionalParameter.at(4)->getBoolValue();
                **/

                // Create and add a new detector
                std::cout << parent->getPositionInformation() << std::endl;

                GNEDetectorE3 *detectorE3 = new GNEDetectorE3("detectorE3_" + toString(numberOfDetectorE3), myViewNet, parent->getPositionInformation(), 100, "");
                detectorE3->setBlocked(myEditorParameter->isBlockEnabled());
                myUndoList->p_begin("add " + detectorE3->getDescription());
                myUndoList->add(new GNEChange_Additional(myViewNet->getNet(), detectorE3, true), true);
                myUndoList->p_end();
                return true;
            } 
            default:
                return false;
        }
    }
}

void 
GNEAdditionalFrame::removeAdditional(GNEAdditional *additional) {
    myUndoList->p_begin("delete " + additional->getDescription());
    myUndoList->add(new GNEChange_Additional(myViewNet->getNet(), additional, false), true);
    myUndoList->p_end();
}

FXFont* 
GNEAdditionalFrame::getHeaderFont() {
    return myHeaderFont;
}


long
GNEAdditionalFrame::onCmdSelectAdditional(FXObject*, FXSelector, void*) {
    // Cast actual additional type
    switch(myAdditionalMatchBox->getCurrentItem()) {
        case 0:
            myActualAdditionalType = SUMO_TAG_BUS_STOP;
            break;
        case 1:
            myActualAdditionalType = SUMO_TAG_CHARGING_STATION;
            break;
        case 2:
            myActualAdditionalType = SUMO_TAG_E1DETECTOR;
            break;
        case 3:
            myActualAdditionalType = SUMO_TAG_E2DETECTOR;
            break;
        case 4:
            myActualAdditionalType = SUMO_TAG_E3DETECTOR;
            break;
        case 5:
            myActualAdditionalType = SUMO_TAG_DET_ENTRY;
            break;
        case 6:
            myActualAdditionalType = SUMO_TAG_DET_EXIT;
            break;
        case 7:
            myActualAdditionalType = SUMO_TAG_REROUTER;
            break;
        case 8:
            myActualAdditionalType = SUMO_TAG_CALIBRATOR;
            break;
        case 9:
            myActualAdditionalType = SUMO_TAG_VSS;
            break;
    }
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
        case SUMO_TAG_BUS_STOP : {
                myGroupBoxForParameters->show();
                myVectorOfAdditionalParameter.at(0)->showParameter("size", oc.getString("busStop default length"));
                std::vector<std::string> lines;
                SUMOSAXAttributes::parseStringVector(oc.getString("busStop default lines"), lines);
                myVectorOfadditionalParameterList.at(0)->showListParameter("line", lines);
                myAdditionalSet->hideList();
            }
            break;
        case SUMO_TAG_CHARGING_STATION : {
                myGroupBoxForParameters->show();
                myVectorOfAdditionalParameter.at(0)->showParameter("size", oc.getString("chargingStation default length"));   
                myVectorOfAdditionalParameter.at(1)->showParameter("power", oc.getString("chargingStation default charging power"));   
                myVectorOfAdditionalParameter.at(2)->showParameter("effic.", oc.getString("chargingStation default efficiency"));   
                myVectorOfAdditionalParameter.at(3)->showParameter("delay", oc.getString("chargingStation default charge delay"));   
                myVectorOfAdditionalParameter.at(4)->showParameter("transit", oc.getBool("chargingStation default charge in transit"));   
                myAdditionalSet->hideList();
            }
            break;
        case SUMO_TAG_E1DETECTOR: {  
                myGroupBoxForParameters->show();
                myVectorOfAdditionalParameter.at(0)->showParameter("frequency.", oc.getString("detector E1 default frequency"));   
                myVectorOfAdditionalParameter.at(1)->showParameter("file", oc.getString("detector E1 default file"));    
                myVectorOfAdditionalParameter.at(2)->showParameter("friendlyPos", oc.getBool("detector E1 default friendlyPos"));
                myVectorOfAdditionalParameter.at(3)->showParameter("split", oc.getBool("detector E1 default splitByType")); 
                myAdditionalSet->hideList();
            }
            break;
        case SUMO_TAG_E2DETECTOR: {
                myGroupBoxForParameters->show();
                myVectorOfAdditionalParameter.at(0)->showParameter("length", oc.getString("detector E2 default length"));   
                myVectorOfAdditionalParameter.at(1)->showParameter("frequency", oc.getString("detector E2 default frequency"));   
                myVectorOfAdditionalParameter.at(2)->showParameter("file", oc.getString("detector E2 default file"));  
                myVectorOfAdditionalParameter.at(3)->showParameter("cont", oc.getBool("detector E2 default cont"));   
                myVectorOfAdditionalParameter.at(4)->showParameter("timeTH", oc.getString("detector E2 default timeThreshold"));   
                myVectorOfAdditionalParameter.at(5)->showParameter("speedTH", oc.getString("detector E2 default speedThreshold"));   
                myVectorOfAdditionalParameter.at(6)->showParameter("jamTH", oc.getString("detector E2 jamThreshold"));   
                myVectorOfAdditionalParameter.at(7)->showParameter("friendlyPos", oc.getBool("detector E2 default friendlyPos"));   
                myAdditionalSet->hideList();
            }
            break;
        case SUMO_TAG_E3DETECTOR: {
                myGroupBoxForParameters->show();
                myVectorOfAdditionalParameter.at(0)->showParameter("frequency", oc.getString("detector E3 default frequency"));   
                myVectorOfAdditionalParameter.at(1)->showParameter("file", oc.getString("detector E3 default file"));  
                myAdditionalSet->hideList();
            }
            break;
        case SUMO_TAG_DET_ENTRY: {
                myGroupBoxForParameters->hide();
                myAdditionalSet->showList(SUMO_TAG_E3DETECTOR);
            }
            break;
        case SUMO_TAG_DET_EXIT: {
                myGroupBoxForParameters->hide();
                myAdditionalSet->showList(SUMO_TAG_E3DETECTOR);
            }
            break;
        case SUMO_TAG_REROUTER: {
                myGroupBoxForParameters->show();
                myVectorOfAdditionalParameter.at(0)->showParameter("file", oc.getString("rerouter default file"));
                myVectorOfAdditionalParameter.at(1)->showParameter("probability", oc.getString("rerouter default probability"));
                myVectorOfAdditionalParameter.at(2)->showParameter("off", oc.getBool("rerouter default off"));
                myVectorOfadditionalParameterList.at(0)->showListParameter("edges", std::vector<std::string>());
                myAdditionalSet->hideList();
            }
            break;
        case SUMO_TAG_CALIBRATOR: {
                myGroupBoxForParameters->show();
                myVectorOfAdditionalParameter.at(0)->showParameter("frequency", oc.getString("calibrator default frequency"));   
                myVectorOfAdditionalParameter.at(1)->showParameter("routeProbe", oc.getString("calibrator default routeProbe"));   
                myVectorOfAdditionalParameter.at(2)->showParameter("output", oc.getString("calibrator default output"));  
                myAdditionalSet->hideList();
            }
            break;
        case SUMO_TAG_VSS: {
                myGroupBoxForParameters->show();
                // Finish
                myAdditionalSet->hideList();
            }
            break;
        default:
            break;
    }
    // Recalc groupBox
    myGroupBoxForParameters->recalc();
}


bool 
GNEAdditionalFrame::setPositions(GNELane *lane, SUMOReal &positionOfTheMouseOverLane, SUMOReal &startPosition, SUMOReal &endPosition) {
    SUMOReal size = GNEAttributeCarrier::parse<SUMOReal>(myVectorOfAdditionalParameter.at(0)->getTextValue());
    switch (myEditorParameter->getActualReferencePoint()) {
        case editorParameter::GNE_ADDITIONALREFERENCEPOINT_LEFT : {
            startPosition = positionOfTheMouseOverLane;
            // Set end position
            if(positionOfTheMouseOverLane + size <= lane->getLaneShapeLenght() - 0.01)
                endPosition = positionOfTheMouseOverLane + size;
            else if(myEditorParameter->isForcePositionEnabled())
                endPosition = lane->getLaneShapeLenght() - 0.01;
            else
                return false;
            break;
        }
        case editorParameter::GNE_ADDITIONALREFERENCEPOINT_RIGHT : {
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
        case editorParameter::GNE_ADDITIONALREFERENCEPOINT_CENTER : {
            // Set startPosition
            if(positionOfTheMouseOverLane - size/2 >= 0.01)
                startPosition = positionOfTheMouseOverLane - size/2;
            else if(myEditorParameter->isForcePositionEnabled())
                startPosition = 0;
            else
                return false;
            // Set endPosition
            if(positionOfTheMouseOverLane + size/2 <= lane->getLaneShapeLenght() - 0.01)
                endPosition = positionOfTheMouseOverLane + size/2;
            else if(myEditorParameter->isForcePositionEnabled())
                endPosition = lane->getLaneShapeLenght() - 0.01;
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

// ---------------------------------------------------------------------------
// GNEAdditionalFrame::additionalParameter - methods
// ---------------------------------------------------------------------------

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


void 
GNEAdditionalFrame::additionalParameter::showTextParameter(const std::string& name, std::string value) {
    myLabel->setText(name.c_str());
    myLabel->show();
    myTextField->setText(value.c_str());
    myTextField->show();
    show();
}


void 
GNEAdditionalFrame::additionalParameter::showIntParameter(const std::string& name, int value) {
    // Finish
}


void 
GNEAdditionalFrame::additionalParameter::showFloatParameter(const std::string& name, SUMOReal value) {
    // Finish
}


void 
GNEAdditionalFrame::additionalParameter::showBoolParameter(const std::string& name, bool value) {
    myLabel->setText(name.c_str());
    myLabel->show();
    myMenuCheck->setCheck(value);
    myMenuCheck->show();
    show();
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
    // Create label Row
    myLabels.push_back(new FXLabel(this, "Rows", 0, JUSTIFY_RIGHT | LAYOUT_FIX_WIDTH, 0, 0, 60, 0));
    FXHorizontalFrame *buttonsFrame = new FXHorizontalFrame(this, LAYOUT_FILL_COLUMN | LAYOUT_FILL_X, 0, 0, 0, 0, 0, 0, 0, 0);
    // Create add button
    add = new FXButton(buttonsFrame, "", GUIIconSubSys::getIcon(ICON_ADD), this, MID_GNE_ADDROW,
        ICON_BEFORE_TEXT | LAYOUT_FIX_WIDTH | LAYOUT_FIX_HEIGHT | FRAME_THICK | FRAME_RAISED,
        0, 0, 20, 20);
    // Create delete buttons
    remove = new FXButton(buttonsFrame, "", GUIIconSubSys::getIcon(ICON_REMOVE), this, MID_GNE_REMOVEROW,
        ICON_BEFORE_TEXT | LAYOUT_FIX_WIDTH | LAYOUT_FIX_HEIGHT | FRAME_THICK | FRAME_RAISED,
        0, 0, 20, 20);
    // Hide all para meters
    hideParameter();
}

// ---------------------------------------------------------------------------
// GNEAdditionalFrame::additionalParameterList - methods
// ---------------------------------------------------------------------------

GNEAdditionalFrame::additionalParameterList::~additionalParameterList() {}


void 
GNEAdditionalFrame::additionalParameterList::showListParameter(const std::string& name, std::vector<std::string> value) {
    if(value.size() < myMaxNumberOfValuesInParameterList) {
        numberOfVisibleTextfields = (int)value.size();
        if(numberOfVisibleTextfields == 0)
            numberOfVisibleTextfields++;
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
        
        myLabels.at(numberOfVisibleTextfields)->show();
        myTextFields.at(numberOfVisibleTextfields)->show();
        numberOfVisibleTextfields++;
        getParent()->recalc();
    }
    return 1;
}


long 
GNEAdditionalFrame::additionalParameterList::onCmdRemoveRow(FXObject*, FXSelector, void*) {
    if(numberOfVisibleTextfields > 1) {
        numberOfVisibleTextfields--;
        myLabels.at(numberOfVisibleTextfields)->hide();
        myTextFields.at(numberOfVisibleTextfields)->hide();
        myTextFields.at(numberOfVisibleTextfields)->setText("");
        
        getParent()->recalc();
    }
    return 1;
}

// ---------------------------------------------------------------------------
// GNEAdditionalFrame::editorParameter - methods
// ---------------------------------------------------------------------------

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

    // Create help button
    helpReferencePoint = new FXButton(this, "Help", 0, this, MID_HELP);

    // Add options to myReferencePointMatchBox
    myReferencePointMatchBox->appendItem("reference left");
    myReferencePointMatchBox->appendItem("reference right");
    myReferencePointMatchBox->appendItem("reference center");

    // Set visible items
    myReferencePointMatchBox->setNumVisible((int)myReferencePointMatchBox->getNumItems());
}


GNEAdditionalFrame::editorParameter::~editorParameter() {}


GNEAdditionalFrame::editorParameter::additionalReferencePoint 
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


long 
GNEAdditionalFrame::editorParameter::onCmdHelp(FXObject*, FXSelector, void*) {
    FXDialogBox* helpDialog = new FXDialogBox(this, "Parameter editor Help", DECOR_CLOSE | DECOR_TITLE);
    std::ostringstream help;
    help
            << "Referece point: Mark the initial position of the additional element.\n"
            << "Example: If you want to create a busStop with a lenght of 30 in the point 100 of the lane:\n"
            << "- Reference Left will create it with fromPos = 70 and toPos = 100.\n"
            << "- Reference Right will create it with fromPos = 100 and toPos = 130.\n"
            << "- Reference Center will create it with fromPos = 85 and toPos = 115.\n"
            << "\n"
            << "Force position: if is enabled, will create the additional adapting size of additional element to lane.\n"
            << "Example: If you have a lane with lenght = 100, but you try to create a busStop with size = 50\n"
            << "in the position 80 of the lane, a busStop with fromPos = 80 and toPos = 100 will be created\n"
            << "instead of a busStop with fromPos = 80 and toPos = 130.\n"
            << "\n"
            << "Block movement: if is enabled, the created additional element will be blocked. i.e. cannot be moved with\n"
            << "the mouse. This option can be modified with the Inspector.";
    new FXLabel(helpDialog, help.str().c_str(), 0, JUSTIFY_LEFT);
    // "OK"
    new FXButton(helpDialog, "OK\t\tclose", 0, helpDialog, FXDialogBox::ID_ACCEPT,
                 ICON_BEFORE_TEXT | LAYOUT_FILL_X | FRAME_THICK | FRAME_RAISED,
                 0, 0, 0, 0, 4, 4, 3, 3);
    helpDialog->create();
    helpDialog->show();
    return 1;
}

// ---------------------------------------------------------------------------
// GNEAdditionalFrame::additionalSet - methods
// ---------------------------------------------------------------------------

GNEAdditionalFrame::additionalSet::additionalSet(FXComposite *parent, FXObject* tgt, GNEViewNet* updateTarget) :
    FXGroupBox(parent, "Additional Set", GROUPBOX_TITLE_CENTER | FRAME_GROOVE | LAYOUT_FILL_X, 0, 0, 0, 0, 0, 0, 0, 0), 
    myViewNet(updateTarget) {
    
    // Create label with the type of additionalSet
    mySetLabel = new FXLabel(this, "Set Type:", 0, JUSTIFY_LEFT | LAYOUT_FILL_X);

    // Create list
    myList = new FXList(this, tgt, MID_GNE_SELECTSET, LAYOUT_FILL_X | LAYOUT_FIX_HEIGHT, 0, 0, 0, 100);

    // Create help button
    helpAdditionalSet = new FXButton(this, "Help", 0, this, MID_HELP);

    // Hide List
    hideList();
}


GNEAdditionalFrame::additionalSet::~additionalSet() {}


GNEAdditionalSet*
GNEAdditionalFrame::additionalSet::getAdditionalSet() {
    return myAdditionalSet;
}


void 
GNEAdditionalFrame::additionalSet::showList(SumoXMLTag type) {
    mySetLabel->setText(("Type of set: " + toString(type)).c_str());
    myList->clearItems();
    std::vector<GNEAdditionalSet*> vectorOfAdditionalSets = myViewNet->getNet()->getAdditionalSets(type);
    for(std::vector<GNEAdditionalSet*>::iterator i = vectorOfAdditionalSets.begin(); i != vectorOfAdditionalSets.end(); i++)
        myList->appendItem((*i)->getID().c_str());
    show();
}


void 
GNEAdditionalFrame::additionalSet::hideList() {
    FXGroupBox::hide();
}


long 
GNEAdditionalFrame::additionalSet::onCmdSelectSet(FXObject*, FXSelector, void*) {
    return 1;
}


long 
GNEAdditionalFrame::additionalSet::onCmdHelp(FXObject*, FXSelector, void*) {
    return 1;
}

/****************************************************************************/
