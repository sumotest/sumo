/****************************************************************************/
/// @file    GNEAdditional.cpp
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
#include "GNEAdditional.h"
#include "GNEViewNet.h"
#include "GNENet.h"
#include "GNEJunction.h"
#include "GNEEdge.h"
#include "GNELane.h"
#include "GNEUndoList.h"
#include "GNEChange_Selection.h"
#include "GNEAttributeCarrier.h"
#include "GNEBusStop.h"
#include "GNEChargingStation.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// FOX callback mapping
// ===========================================================================
FXDEFMAP(GNEAdditional) GNEAdditionalMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_MODE_ADDITIONAL_ITEM,           GNEAdditional::onCmdSelectAdditional),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_MODE_ADDITIONAL_REFERENCEPOINT, GNEAdditional::onCmdSelectReferencePoint),
};

// Object implementation
FXIMPLEMENT(GNEAdditional, FXScrollWindow, GNEAdditionalMap, ARRAYNUMBER(GNEAdditionalMap))

// ===========================================================================
// static members
// ===========================================================================
const int GNEAdditional::WIDTH = 150;
const int GNEAdditional::maximumNumberOfAdditionalParameterTextField = 10;
const int GNEAdditional::maximumNumberOfAdditionalParameterCheckButton = 10;

// ===========================================================================
// method definitions
// ===========================================================================
GNEAdditional::GNEAdditional(FXComposite* parent, GNEViewNet* updateTarget, GNEUndoList* undoList):
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
    FXGroupBox* groupBoxForMyAdditionalMatchBox = new FXGroupBox(myContentFrame, "Additional element",
                                                      GROUPBOX_TITLE_CENTER | FRAME_GROOVE | LAYOUT_FILL_X, 2, 0, 0, 0, 4, 2, 2, 2);
    
    // Create FXListBox in groupBoxForMyAdditionalMatchBox
    myAdditionalMatchBox = new FXComboBox(groupBoxForMyAdditionalMatchBox, 12, this, MID_GNE_MODE_ADDITIONAL_ITEM, 
                                          FRAME_SUNKEN | LAYOUT_LEFT | LAYOUT_TOP | COMBOBOX_STATIC | LAYOUT_CENTER_Y);

    // Create groupBox for parameters 
    FXGroupBox* groupBoxForParameters = new FXGroupBox(myContentFrame, "Default parameters",
                                            GROUPBOX_TITLE_CENTER | FRAME_GROOVE | LAYOUT_FILL_X, 2, 0, 0, 0, 4, 2, 2, 2);
    // Create groupBox for reference point
    FXGroupBox* myReferencePointBox = new FXGroupBox(myContentFrame, "Reference point",
                                            GROUPBOX_TITLE_CENTER | FRAME_GROOVE | LAYOUT_FILL_X, 2, 0, 0, 0, 4, 2, 2, 2);
    
    // Create FXListBox for the reference points
    myReferencePointMatchBox = new FXComboBox(myReferencePointBox, 12, this, MID_GNE_MODE_ADDITIONAL_REFERENCEPOINT,
                                              FRAME_SUNKEN | LAYOUT_LEFT | LAYOUT_TOP | COMBOBOX_STATIC | LAYOUT_CENTER_Y);

    // Create additionalParameterTextField
    for(int i = 0; i < maximumNumberOfAdditionalParameterTextField; i++) {
        additionalParameterTextField additionalParameter;
        // Create elements
        additionalParameter.horizontalFrame = new FXHorizontalFrame(groupBoxForParameters, LAYOUT_SIDE_TOP | LAYOUT_FIX_WIDTH | PACK_UNIFORM_WIDTH, 0, 0, WIDTH - 20/**revisar**/, 0);
        //additionalParameter.horizontalFrame->setPackingHints(PACK_UNIFORM_WIDTH);
        
        additionalParameter.label = new FXLabel(additionalParameter.horizontalFrame, "param", 0, JUSTIFY_RIGHT);
        additionalParameter.textField = new FXTextField(additionalParameter.horizontalFrame, 10, this, MID_GNE_SELMB_STRING, TEXTFIELD_NORMAL, 0, 0, 0, 0, 4, 2, 0, 2);
        // Add to myVectorOfParametersTextFields
        myVectorOfParametersTextFields.push_back(additionalParameter);
    }

    // Create additionalParameterCheckButton/
    for(int i = 0; i < maximumNumberOfAdditionalParameterCheckButton; i++) {
        additionalParameterCheckButton additionalParameter;
        additionalParameter.horizontalFrame = new FXHorizontalFrame(groupBoxForParameters, LAYOUT_SIDE_TOP | LAYOUT_FIX_WIDTH, 0, 0, WIDTH, 0);
        additionalParameter.label = new FXLabel(additionalParameter.horizontalFrame, "param", 0, JUSTIFY_RIGHT);
        additionalParameter.menuCheck = new FXMenuCheck(additionalParameter.horizontalFrame, "", this, 0);
        // Add to myVectorOfParameterCheckButton
        myVectorOfParameterCheckButton.push_back(additionalParameter);
    }

    // Add options to myAdditionalMatchBox
    myAdditionalMatchBox->appendItem("Bus stop");
    myAdditionalMatchBox->appendItem("Charging station");
    myAdditionalMatchBox->appendItem("Detector E1");
    myAdditionalMatchBox->appendItem("Detector E2");
    myAdditionalMatchBox->appendItem("Detector E3");
    myAdditionalMatchBox->appendItem("Rerouters");
    myAdditionalMatchBox->appendItem("Calibrators");
    myAdditionalMatchBox->appendItem("VSpeed signals");

    myAdditionalMatchBox->setNumVisible((int)myAdditionalMatchBox->getNumItems()); 

    // By default, myActualAdditionalType is busStop
    myActualAdditionalType = GNE_ADDITIONAL_BUSSTOP;

    // Add options to myReferencePointMatchBox
    myReferencePointMatchBox->appendItem("left");
    myReferencePointMatchBox->appendItem("right");
    myReferencePointMatchBox->appendItem("center");

    myReferencePointMatchBox->setNumVisible((int)myReferencePointMatchBox->getNumItems());

    // By default, reference point is left
    myActualAdditionalReferencePoint = GNE_ADDITIONALREFERENCEPOINT_LEFT;
}


GNEAdditional::~GNEAdditional() {
    delete myHeaderFont;
    gSelected.remove2Update();
}


void
GNEAdditional::addAdditional(GNELane &lane, GUISUMOAbstractView* parent) {
    // Position of the mouse in the lane
    SUMOReal position = lane.getShape().nearest_offset_to_point2D(parent->getPositionInformation());
    switch(myActualAdditionalType) {
        case GNE_ADDITIONAL_BUSSTOP: {
            int numberOfBusStops = myUpdateTarget->getNet()->getNumberOfBusStops();
            // Check that the ID of the new busStop is unique
            while(myUpdateTarget->getNet()->getBusStop("busStop" + toString(numberOfBusStops)) != NULL)
                numberOfBusStops++;
            // Create an add new busStop
            /** TERMINAR **/
            GNEBusStop *busStop = new GNEBusStop("busStop" + toString(numberOfBusStops), std::vector<std::string>(), lane, position, position + 10);
            myUpdateTarget->getNet()->addBusStop(busStop);
            parent->addAdditionalGLVisualisation(busStop);
            break;
        }
        case GNE_ADDITIONAL_CHARGINGSTATION: {
            int numberOfBusStops = myUpdateTarget->getNet()->getNumberOfChargingStations();
            // Check that the ID of the new chargingStation is unique
            while(myUpdateTarget->getNet()->getBusStop("chargingStation" + toString(numberOfBusStops)) != NULL)
                numberOfBusStops++;
            // Create an add new chargingStation
            /** TERMINAR **/
            GNEChargingStation *chargingStation = new GNEChargingStation("chargingStation" + toString(numberOfBusStops), lane, position, position + 10, 32000, 0.95, true, 5);
            myUpdateTarget->getNet()->addChargingStation(chargingStation);
            parent->addAdditionalGLVisualisation(chargingStation);
            break;
        }
        case GNE_ADDITIONAL_E1:{
            /** Finish **/
            break;
        }
        case GNE_ADDITIONAL_E2:{
            /** Finish **/
            break;
        }
        case GNE_ADDITIONAL_E3:{
            /** Finish **/
            break;
        }
        case GNE_ADDITIONAL_REROUTERS:{
            /** Finish **/
            break;
        }
        case GNE_ADDITIONAL_CALIBRATORS:{
            /** Finish **/
            break;
        }
        case GNE_ADDITIONAL_VARIABLESPEEDSIGNS:{
            /** Finish **/
            break;
        }
    }
}


long
GNEAdditional::onCmdSelectAdditional(FXObject*, FXSelector, void*) {
    // cast actual additional type
    myActualAdditionalType = static_cast<additionalType>(myAdditionalMatchBox->getCurrentItem());
    // Set parameters
    setParameters();

    return 1;
}

long 
GNEAdditional::onCmdSelectReferencePoint(FXObject*, FXSelector, void*) {
    return 1;
}

void
GNEAdditional::show() {
    // Set parameters
    setParameters();
    // Show 
    FXScrollWindow::show();
}


void
GNEAdditional::hide() {
    FXScrollWindow::hide();
}

void 
GNEAdditional::setParameters() {
    // Hide all additionalParameterTextField
    for(std::vector<additionalParameterTextField>::iterator i = myVectorOfParametersTextFields.begin(); i != myVectorOfParametersTextFields.end(); i++) {
        (*i).horizontalFrame->hide();
    }
    // Hide all additionalParameterCheckButton
    for(std::vector<additionalParameterCheckButton>::iterator i = myVectorOfParameterCheckButton.begin(); i != myVectorOfParameterCheckButton.end(); i++) {
        (*i).horizontalFrame->hide();
    }
    // Set parameters depending of myActualAdditionalType
    switch(myActualAdditionalType) {
        case GNE_ADDITIONAL_BUSSTOP :
            myVectorOfParametersTextFields.at(0).label->setText("size");
            myVectorOfParametersTextFields.at(0).horizontalFrame->show();
            myVectorOfParametersTextFields.at(1).label->setText("lines");
            myVectorOfParametersTextFields.at(1).horizontalFrame->show();
            break;

        case GNE_ADDITIONAL_CHARGINGSTATION :
            myVectorOfParametersTextFields.at(0).label->setText("size");
            myVectorOfParametersTextFields.at(0).horizontalFrame->show();
            myVectorOfParametersTextFields.at(1).label->setText("crg. Power");
            myVectorOfParametersTextFields.at(1).horizontalFrame->show();
            myVectorOfParametersTextFields.at(2).label->setText("efficiency");
            myVectorOfParametersTextFields.at(2).horizontalFrame->show();
            break;

        default:
            break;
    }
}
/****************************************************************************/