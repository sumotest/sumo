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
#include "GNEAdditional.h"
#include "GNEAdditionalSet.h"
#include "GNEAdditionalHandler.h"

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
    const std::vector<SumoXMLTag>& additionalTags = GNEAttributeCarrier::allowedAdditionalTags();
    for(std::vector<SumoXMLTag>::const_iterator i = additionalTags.begin(); i != additionalTags.end(); i++)
        myAdditionalMatchBox->appendItem(toString(*i).c_str());

    // Set visible items
    myAdditionalMatchBox->setNumVisible((int)myAdditionalMatchBox->getNumItems()); 
}


GNEAdditionalFrame::~GNEAdditionalFrame() {
    delete myHeaderFont;
    gSelected.remove2Update();
}


bool
GNEAdditionalFrame::addAdditional(GNELane *lane, GUISUMOAbstractView* parent) {
    // First check if actual type must be placed over a lane
    if(GNEAttributeCarrier::hasAttribute(myActualAdditionalType, SUMO_ATTR_LANE) && lane == NULL)
        return false;
    // Declare map to keep values
    std::map<SumoXMLAttr, std::string> valuesOfElement;
    // obtain a new unique id depending if the element needs or not a lane
    if(lane) {
        SUMOReal positionOfTheMouseOverLane = lane->getShape().nearest_offset_to_point2D(parent->getPositionInformation());
        int additionalIndex = myViewNet->getNet()->getNumberOfAdditionals(myActualAdditionalType);
        while(myViewNet->getNet()->getAdditional(myActualAdditionalType, toString(myActualAdditionalType) + "_" + lane->getID() + "_" + toString(additionalIndex)) != NULL)
            additionalIndex++;
        valuesOfElement[SUMO_ATTR_ID] = toString(myActualAdditionalType) + "_" + lane->getID() + "_" + toString(additionalIndex);
        // Obtain lane ID
        valuesOfElement[SUMO_ATTR_LANE] = lane->getID();
        // If element has a StartPosition and EndPosition over lane, extract attributes
        if(GNEAttributeCarrier::hasAttribute(myActualAdditionalType, SUMO_ATTR_STARTPOS) && GNEAttributeCarrier::hasAttribute(myActualAdditionalType, SUMO_ATTR_ENDPOS)) {
            valuesOfElement[SUMO_ATTR_STARTPOS] = toString(setStartPosition(lane->getLaneShapeLenght(), positionOfTheMouseOverLane, myEditorParameter->getLenght()));
            valuesOfElement[SUMO_ATTR_ENDPOS] = toString(setEndPosition(lane->getLaneShapeLenght(), positionOfTheMouseOverLane, myEditorParameter->getLenght()));
        }
        // Extract position of lane
        valuesOfElement[SUMO_ATTR_POSITION] = toString(positionOfTheMouseOverLane);
    } else {
        int additionalSetIndex = myViewNet->getNet()->getNumberOfAdditionals(myActualAdditionalType);
        while(myViewNet->getNet()->getAdditional(myActualAdditionalType, toString(myActualAdditionalType) + "_" + toString(additionalSetIndex)) != NULL)
            additionalSetIndex++;
        valuesOfElement[SUMO_ATTR_ID] = toString(myActualAdditionalType) + "_" + toString(additionalSetIndex);
        valuesOfElement[SUMO_ATTR_POSITION] = toString(parent->getPositionInformation());
    }
    // Save block value
    valuesOfElement[GNE_ATTR_BLOCK_MOVEMENT] = toString(myEditorParameter->isBlockEnabled());
    // Save all editable parameters
    for(int i = 0; i < this->myIndexParameter; i++)
        valuesOfElement[myVectorOfAdditionalParameter.at(i)->getAttr()] = myVectorOfAdditionalParameter.at(i)->getValue();
    // If element belongst to an additional Set, get id of parent from myAdditionalSet
    if(GNEAttributeCarrier::hasParent(myActualAdditionalType)) {
        if(this->myAdditionalSet->getIdSelected() != "")
            valuesOfElement[GNE_ATTR_PARENT] = myAdditionalSet->getIdSelected();
        else {
            std::cout << "MOSTRAR ERROR EN CONSOLA" << std::endl;
            return false;
        }
    }
/** FALTAN LISTAS **/
    
    // Create additional
    return GNEAdditionalHandler::buildAdditional(myViewNet, myActualAdditionalType, valuesOfElement);
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
    // set myActualAdditionalType
    const std::vector<SumoXMLTag>& additionalTags = GNEAttributeCarrier::allowedAdditionalTags();
    for(std::vector<SumoXMLTag>::const_iterator i = additionalTags.begin(); i != additionalTags.end(); i++)
        if(toString(*i) == myAdditionalMatchBox->getText().text())
            setParameters(*i);
    return 1;
}


void
GNEAdditionalFrame::show() {
    // Show Scroll window
    FXScrollWindow::show();
}


void
GNEAdditionalFrame::hide() {
    // Hide ScrollWindow
    FXScrollWindow::hide();
}


void 
GNEAdditionalFrame::setParameters(SumoXMLTag actualAdditionalType) {
    // Set new actualAdditionalType
    myActualAdditionalType = actualAdditionalType;
    // Hide all parameters boxes
    for(int i = 0; i < maxNumberOfParameters; i++)
        myVectorOfAdditionalParameter.at(i)->hideParameter();
    for(int i = 0; i < maxNumberOfListParameters; i++)
        myVectorOfadditionalParameterList.at(i)->hideParameter();
    // Hide lenght field
    myEditorParameter->hideLengthField();
    // Obtain attributes of actual myActualAdditionalType
    std::vector<std::pair <SumoXMLAttr, std::string> > attrs = GNEAttributeCarrier::allowedAttributes(myActualAdditionalType);
    // reset indexes
    myIndexParameter = 0;
    myIndexParameterList = 0;
    // Iterate over attributes of myActualAdditionalType
    for(std::vector<std::pair <SumoXMLAttr, std::string> >::iterator i = attrs.begin(); i != attrs.end(); i++) {
        SumoXMLAttr attrName = i->first;
        if(!GNEAttributeCarrier::isUnique(attrName)) {
            if(GNEAttributeCarrier::isList(attrName)) {
                // Check type of list    
                if(GNEAttributeCarrier::isInt(attrName))
                    myVectorOfadditionalParameterList.at(myIndexParameterList)->showListParameter(attrName, GNEAttributeCarrier::getDefaultValue< std::vector<int> >(myActualAdditionalType, i->first));
                else if(GNEAttributeCarrier::isFloat(attrName))
                    myVectorOfadditionalParameterList.at(myIndexParameterList)->showListParameter(attrName, GNEAttributeCarrier::getDefaultValue< std::vector<SUMOReal> >(myActualAdditionalType, i->first));
                else if(GNEAttributeCarrier::isBool(attrName))
                    myVectorOfadditionalParameterList.at(myIndexParameterList)->showListParameter(attrName, GNEAttributeCarrier::getDefaultValue< std::vector<bool> >(myActualAdditionalType, i->first));
                else if(GNEAttributeCarrier::isString(attrName))
                    myVectorOfadditionalParameterList.at(myIndexParameterList)->showListParameter(attrName, GNEAttributeCarrier::getDefaultValue< std::vector<std::string> >(myActualAdditionalType, i->first));
                // Update index 
                myIndexParameterList++;
            }
            else if(GNEAttributeCarrier::isInt(attrName))
                myVectorOfAdditionalParameter.at(myIndexParameter)->showParameter(attrName, GNEAttributeCarrier::getDefaultValue<int>(myActualAdditionalType, i->first));
            else if(GNEAttributeCarrier::isFloat(attrName))
                myVectorOfAdditionalParameter.at(myIndexParameter)->showParameter(attrName, GNEAttributeCarrier::getDefaultValue<SUMOReal>(myActualAdditionalType, i->first));
            else if(GNEAttributeCarrier::isBool(attrName))
                myVectorOfAdditionalParameter.at(myIndexParameter)->showParameter(attrName, GNEAttributeCarrier::getDefaultValue<bool>(myActualAdditionalType, i->first));
            else if(GNEAttributeCarrier::isString(attrName))
                myVectorOfAdditionalParameter.at(myIndexParameter)->showParameter(attrName, GNEAttributeCarrier::getDefaultValue<std::string>(myActualAdditionalType, i->first));
            else
                WRITE_WARNING("Attribute '" + toString(attrName) + "' don't have a defined type. Check definition in GNEAttributeCarrier");
            // Update index parameter
            myIndexParameter++;
        } else if(attrName == SUMO_ATTR_ENDPOS)
            myEditorParameter->showLengthField();
    }
        
    // if there are parmeters, show and Recalc groupBox
    if(myIndexParameter > 0 || myIndexParameterList > 0) {
        myGroupBoxForParameters->show();
        myGroupBoxForParameters->recalc();
    }
    else
        myGroupBoxForParameters->hide();

    // Show set parameter if we're adding a additional with parent
    if(GNEAttributeCarrier::hasParent(myActualAdditionalType))
        myAdditionalSet->showList(GNEAttributeCarrier::getParentType(myActualAdditionalType));
    else
        myAdditionalSet->hideList();
}


SUMOReal 
GNEAdditionalFrame::setStartPosition(SUMOReal laneLenght, SUMOReal positionOfTheMouseOverLane, SUMOReal lenghtOfAdditional) {
    switch (myEditorParameter->getActualReferencePoint()) {
        case editorParameter::GNE_ADDITIONALREFERENCEPOINT_LEFT :
            return positionOfTheMouseOverLane;
        case editorParameter::GNE_ADDITIONALREFERENCEPOINT_RIGHT :
            if(positionOfTheMouseOverLane - lenghtOfAdditional >= 0.01)
                 return positionOfTheMouseOverLane - lenghtOfAdditional;
            else if(myEditorParameter->isForcePositionEnabled())
                return 0.01;
            else
                return -1;
        case editorParameter::GNE_ADDITIONALREFERENCEPOINT_CENTER :
            if(positionOfTheMouseOverLane - lenghtOfAdditional/2 >= 0.01)
                return positionOfTheMouseOverLane - lenghtOfAdditional/2;
            else if(myEditorParameter->isForcePositionEnabled())
                return 0;
            else
                return -1;
    }
}


SUMOReal 
GNEAdditionalFrame::setEndPosition(SUMOReal laneLenght, SUMOReal positionOfTheMouseOverLane, SUMOReal lenghtOfAdditional) {
    switch (myEditorParameter->getActualReferencePoint()) {
        case editorParameter::GNE_ADDITIONALREFERENCEPOINT_LEFT:
            if(positionOfTheMouseOverLane + lenghtOfAdditional <= laneLenght - 0.01)
                return positionOfTheMouseOverLane + lenghtOfAdditional;
            else if(myEditorParameter->isForcePositionEnabled())
                return laneLenght - 0.01;
            else
                return -1;
        case editorParameter::GNE_ADDITIONALREFERENCEPOINT_RIGHT:
            return positionOfTheMouseOverLane;
        case editorParameter::GNE_ADDITIONALREFERENCEPOINT_CENTER:
            if(positionOfTheMouseOverLane + lenghtOfAdditional/2 <= laneLenght - 0.01)
                return positionOfTheMouseOverLane + lenghtOfAdditional/2;
            else if(myEditorParameter->isForcePositionEnabled())
                return laneLenght - 0.01;
            else
                return -1;
        default:
            return -1;
    }
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
GNEAdditionalFrame::additionalParameter::showParameter(SumoXMLAttr attr, std::string value) {
    myAttr = attr;
    myLabel->setText(toString(myAttr).c_str());
    myLabel->show();
    myTextField->setText(value.c_str());
    myTextField->show();
    show();
}


void
GNEAdditionalFrame::additionalParameter::showParameter(SumoXMLAttr attr, int value) {
    myAttr = attr;
    myLabel->setText(toString(myAttr).c_str());
    myLabel->show();
    myTextField->setText(toString(value).c_str());
    myTextField->show();
    show();
}


void
GNEAdditionalFrame::additionalParameter::showParameter(SumoXMLAttr attr, SUMOReal value) {
    myAttr = attr;
    myLabel->setText(toString(myAttr).c_str());
    myLabel->show();
    myTextField->setText(toString(value).c_str());
    myTextField->show();
    show();
}


void
GNEAdditionalFrame::additionalParameter::showParameter(SumoXMLAttr attr, bool value) {
    myAttr = attr;
    myLabel->setText(toString(myAttr).c_str());
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


SumoXMLAttr 
GNEAdditionalFrame::additionalParameter::getAttr() const {
    return myAttr;
}


std::string 
GNEAdditionalFrame::additionalParameter::getValue() const {
    if(GNEAttributeCarrier::isBool(myAttr))
        return (myMenuCheck->getCheck() == 1)? "true" : "false";
    else
        return myTextField->getText().text();
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
GNEAdditionalFrame::additionalParameterList::showListParameter(SumoXMLAttr attr, std::vector<int> value) {
    std::cout << "FINISH" << std::endl;
}

void 
GNEAdditionalFrame::additionalParameterList::showListParameter(SumoXMLAttr attr, std::vector<SUMOReal> value) {
    std::cout << "FINISH" << std::endl;
}

void 
GNEAdditionalFrame::additionalParameterList::showListParameter(SumoXMLAttr attr, std::vector<bool> value) {
    std::cout << "FINISH" << std::endl;
}

void 
GNEAdditionalFrame::additionalParameterList::showListParameter(SumoXMLAttr attr, std::vector<std::string> value) {
    if(value.size() < myMaxNumberOfValuesInParameterList) {
        numberOfVisibleTextfields = (int)value.size();
        if(numberOfVisibleTextfields == 0)
            numberOfVisibleTextfields++;
        for(int i = 0; i < myMaxNumberOfValuesInParameterList; i++) 
            myLabels.at(i)->setText((toString(attr) + ": " + toString(i)).c_str());
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
                                              FRAME_SUNKEN | LAYOUT_LEFT  | COMBOBOX_STATIC | LAYOUT_FILL_X);

    // Create Frame for Label and TextField
    FXHorizontalFrame *lengthFrame = new FXHorizontalFrame(this, LAYOUT_FILL_X | LAYOUT_LEFT , 0, 0, 0, 0, 0, 0, 0, 0);

    // Create length label
    myLengthLabel = new FXLabel(lengthFrame, "Length:", 0, JUSTIFY_LEFT | LAYOUT_FILL_X);

    // Create length text field
    myLengthTextField = new FXTextField(lengthFrame, 10, tgt, MID_GNE_MODE_ADDITIONAL_CHANGEPARAMETER_TEXT, LAYOUT_FILL_COLUMN | LAYOUT_FILL_X);

    // Set default value of length
    myLengthTextField->setText("10");

    // Create FXMenuCheck for the force option
    myCheckForcePosition = new FXMenuCheck(this, "Force position", this, MID_GNE_MODE_ADDITIONAL_FORCEPOSITION,
                                           LAYOUT_LEFT | LAYOUT_FILL_X);

    // Create FXMenuCheck for the force option
    myCheckBlock = new FXMenuCheck(this, "Block movement", this, MID_GNE_SET_BLOCKING,
                                   LAYOUT_LEFT | LAYOUT_FILL_X);

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


void 
GNEAdditionalFrame::editorParameter::showLengthField() {
    myLengthLabel->show();
    myLengthTextField->show();
}


void 
GNEAdditionalFrame::editorParameter::hideLengthField() {
    myLengthLabel->hide();
    myLengthTextField->hide();
}


GNEAdditionalFrame::editorParameter::additionalReferencePoint 
GNEAdditionalFrame::editorParameter::getActualReferencePoint() {
    return myActualAdditionalReferencePoint;
}


SUMOReal 
GNEAdditionalFrame::editorParameter::getLenght() {
    return GNEAttributeCarrier::parse<SUMOReal>(myLengthTextField->getText().text());
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


std::string
GNEAdditionalFrame::additionalSet::getIdSelected() {
    if(myList->getCurrentItem() == -1)
        return "";
    else
        return myList->getItem(myList->getCurrentItem())->getText().text();
}


void 
GNEAdditionalFrame::additionalSet::showList(SumoXMLTag type) {
    mySetLabel->setText(("Type of set: " + toString(type)).c_str());
    myList->clearItems();
    std::vector<GNEAdditional*> vectorOfAdditionalSets = myViewNet->getNet()->getAdditionals(type);
    for(std::vector<GNEAdditional*>::iterator i = vectorOfAdditionalSets.begin(); i != vectorOfAdditionalSets.end(); i++)
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
