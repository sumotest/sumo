/****************************************************************************/
/// @file    GNEInspectorFrame.cpp
/// @author  Jakob Erdmann
/// @date    Mar 2011
/// @version $Id: GNEInspectorFrame.cpp 20474 2016-04-16 09:10:57Z palcraft $
///
// The Widget for modifying network-element attributes (i.e. lane speed)
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2016 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
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

#include <cmath>
#include <cassert>
#include <iostream>
#include <utils/foxtools/MFXUtils.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/images/GUIIconSubSys.h>
#include "GNEInspectorFrame.h"
#include "GNEUndoList.h"
#include "GNEEdge.h"
#include "GNEAttributeCarrier.h"
#include "GNEAdditional.h"
#include "GNEViewNet.h"
#include "GNEViewParent.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// FOX callback mapping
// ===========================================================================
FXDEFMAP(GNEInspectorFrame) GNEInspectorFrameMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_COPY_TEMPLATE, GNEInspectorFrame::onCmdCopyTemplate),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SET_TEMPLATE,  GNEInspectorFrame::onCmdSetTemplate),
    FXMAPFUNC(SEL_UPDATE,   MID_GNE_COPY_TEMPLATE, GNEInspectorFrame::onUpdCopyTemplate),
};

/*
FXDEFMAP(GNEInspectorFrame::AttrPanel) AttrPanelMap[]= {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SET_BLOCKING,  GNEInspectorFrame::AttrPanel::onCmdSetBlocking),
};
*/

FXDEFMAP(GNEInspectorFrame::AttrInput) AttrInputMap[] = {
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_SET_ATTRIBUTE,         GNEInspectorFrame::AttrInput::onCmdSetAttribute),
    FXMAPFUNC(SEL_COMMAND,  MID_GNE_OPEN_ATTRIBUTE_EDITOR, GNEInspectorFrame::AttrInput::onCmdOpenAttributeEditor)
};

// Object implementation
FXIMPLEMENT(GNEInspectorFrame, FXScrollWindow, GNEInspectorFrameMap, ARRAYNUMBER(GNEInspectorFrameMap))
//FXIMPLEMENT(GNEInspectorFrame::AttrPanel, FXVerticalFrame, AttrPanelMap, ARRAYNUMBER(AttrPanelMap))
FXIMPLEMENT(GNEInspectorFrame::AttrInput, FXMatrix, AttrInputMap, ARRAYNUMBER(AttrInputMap))


// ===========================================================================
// method definitions
// ===========================================================================
GNEInspectorFrame::GNEInspectorFrame(FXComposite* parent, GNEViewNet* viewNet):
    GNEFrame(parent, viewNet, "Inspector"),
    myEdgeTemplate(0) {

    // Create groupBox for attributes
    myGroupBoxForAttributes = new FXGroupBox(myContentFrame, "attributes", GROUPBOX_TITLE_CENTER | FRAME_GROOVE | LAYOUT_FILL_X);
    myGroupBoxForAttributes->hide();

    // Create AttrInput
    for(int i = 0; i < GNEAttributeCarrier::getHigherNumberOfAttributes(); i++)
        listOfAttrInput.push_back(new AttrInput(myGroupBoxForAttributes, this));

    // Create groupBox for templates
    myGroupBoxForTemplates = new FXGroupBox(myContentFrame, "templates", GROUPBOX_TITLE_CENTER | FRAME_GROOVE | LAYOUT_FILL_X);
    myGroupBoxForTemplates->hide();

    // Create copy template button
    myCopyTemplateButton = new FXButton(myGroupBoxForTemplates, "", 0, this, MID_GNE_COPY_TEMPLATE, ICON_BEFORE_TEXT | LAYOUT_FILL_X | FRAME_THICK | FRAME_RAISED, 0, 0, 0, 0, 4, 4, 3, 3);
    myCopyTemplateButton->hide();

    // Create set template button
    mySetTemplateButton = new FXButton(myGroupBoxForTemplates, "Set as Template\t\t", 0, this, MID_GNE_SET_TEMPLATE, ICON_BEFORE_TEXT | LAYOUT_FILL_X | FRAME_THICK | FRAME_RAISED, 0, 0, 0, 0, 4, 4, 3, 3);
    mySetTemplateButton->hide();

    // Create groupBox for editor parameters
    myGroupBoxForEditor = new FXGroupBox(myContentFrame, "editor", GROUPBOX_TITLE_CENTER | FRAME_GROOVE | LAYOUT_FILL_X);
    myGroupBoxForEditor->hide();

    // Create check blocked button
    myCheckBlocked = new FXCheckButton(myGroupBoxForEditor, "Block movement", this, MID_GNE_SET_BLOCKING);
    myCheckBlocked->hide();
}

GNEInspectorFrame::~GNEInspectorFrame() {
    if (myEdgeTemplate) {
        myEdgeTemplate->decRef("GNEInspectorFrame::~GNEInspectorFrame");
        if (myEdgeTemplate->unreferenced()) {
            delete myEdgeTemplate;
        }
    }
}


void
GNEInspectorFrame::show() {
    // Show Scroll window
    FXScrollWindow::show();
    // Show Frame Area in which this GNEFrame is placed
    myViewNet->getViewParent()->showFramesArea();
}


void
GNEInspectorFrame::hide() {
    // Hide ScrollWindow
    FXScrollWindow::hide();
    // Hide Frame Area in which this GNEFrame is placed
    myViewNet->getViewParent()->hideFramesArea();
}


void
GNEInspectorFrame::inspect(const std::vector<GNEAttributeCarrier*>& ACs) {
    // If vector of attribute Carriers contain data
    if (ACs.size() > 0) {
        // Set header
        std::string headerString = toString(ACs[0]->getTag());
        if (ACs.size() > 1)
            headerString = toString(ACs.size()) + " " + headerString + "s";
        getFrameHeaderLabel()->setText(headerString.c_str());
        
        //Show myGroupBoxForAttributes
        myGroupBoxForAttributes->show();

        // Hide all AttrInput
        for(std::list<GNEInspectorFrame::AttrInput*>::iterator i = listOfAttrInput.begin(); i != listOfAttrInput.end(); i++)
           (*i)->hide();
        
        // Gets attributes of element
        const std::vector<SumoXMLAttr>& attrs = ACs[0]->getAttrs();
        
        // Declare iterator over AttrImput
        std::list<GNEInspectorFrame::AttrInput*>::iterator itAttrs = listOfAttrInput.begin();

        // Show attributes
        for (std::vector<SumoXMLAttr>::const_iterator i = attrs.begin(); i != attrs.end(); i++) {
            (*itAttrs)->showAttribute(*i);
            itAttrs++;
        }
            
        // If attributes correspond to an Edge
        if (dynamic_cast<GNEEdge*>(ACs[0])) {
            // show groupBox for templates
            myGroupBoxForTemplates->show();

            // show "Copy Template" (caption supplied via onUpdate)
            myCopyTemplateButton->show();

            // show "Set As Template"
            if (ACs.size() == 1)
                mySetTemplateButton->show();
        }

        // If attributes correspond to an Additional
        if(dynamic_cast<GNEAdditional*>(ACs[0])) {
            // Get pointer to additional
            myAdditional = dynamic_cast<GNEAdditional*>(ACs[0]);
            // Show groupBox for editor Attributes
            myGroupBoxForEditor->show();
            // Show check blocked
            myCheckBlocked->setCheck(myAdditional->isBlocked());
            myCheckBlocked->show();
        }

    } else {
        getFrameHeaderLabel()->setText("No Object selected");
        myGroupBoxForAttributes->hide();
        myGroupBoxForTemplates->hide();
        myCopyTemplateButton->hide();
        mySetTemplateButton->hide();
    }
}

GNEEdge*
GNEInspectorFrame::getEdgeTemplate() const {
    return myEdgeTemplate;
}


void
GNEInspectorFrame::setEdgeTemplate(GNEEdge* tpl) {
    if (myEdgeTemplate) {
        myEdgeTemplate->decRef("GNEInspectorFrame::setEdgeTemplate");
        if (myEdgeTemplate->unreferenced()) {
            delete myEdgeTemplate;
        }
    }
    myEdgeTemplate = tpl;
    myEdgeTemplate->incRef("GNEInspectorFrame::setEdgeTemplate");
}


long
GNEInspectorFrame::onCmdCopyTemplate(FXObject*, FXSelector, void*) {
    for (std::vector<GNEAttributeCarrier*>::iterator it = myACs.begin(); it != myACs.end(); it++) {
        GNEEdge* edge = dynamic_cast<GNEEdge*>(*it);
        assert(edge);
        edge->copyTemplate(myEdgeTemplate, myViewNet->getUndoList());
    }
    return 1;
}


long
GNEInspectorFrame::onCmdSetTemplate(FXObject*, FXSelector, void*) {
    assert(myACs.size() == 1);
    GNEEdge* edge = dynamic_cast<GNEEdge*>(myACs[0]);
    assert(edge);
    setEdgeTemplate(edge);
    updateAttributes(myACs);
    return 1;
}


long
GNEInspectorFrame::onUpdCopyTemplate(FXObject* sender, FXSelector, void*) {
    FXString caption;
    if (myEdgeTemplate) {
        caption = ("Copy '" + myEdgeTemplate->getMicrosimID() + "'").c_str();
        sender->handle(this, FXSEL(SEL_COMMAND, ID_ENABLE), NULL);
    } else {
        caption = "No Template Set";
        sender->handle(this, FXSEL(SEL_COMMAND, ID_DISABLE), NULL);
    }
    sender->handle(this, FXSEL(SEL_COMMAND, FXLabel::ID_SETSTRINGVALUE), (void*)&caption);
    return 1;
}

/*
long
GNEInspectorFrame::onCmdSetBlocking(FXObject*, FXSelector, void*) {
    myAdditional->setBlocked(myCheckBlocked->getCheck() == 1? true : false);
    myAdditional->getViewNet()->update();
    return 1;
}*/
 

void
GNEInspectorFrame::updateAttributes(const std::vector<GNEAttributeCarrier*>& ACs) {

}

// ===========================================================================
// AttrInput method definitions
// ===========================================================================

GNEInspectorFrame::AttrInput::AttrInput(FXComposite* parent, GNEInspectorFrame *inspectorFrameParent) :
    FXMatrix(parent, 4, MATRIX_BY_COLUMNS | LAYOUT_FILL_X | PACK_UNIFORM_WIDTH),
    myInspectorFrameParent(inspectorFrameParent) {
    // Create and hide label
    myLabel = new FXLabel(this, "attributeLabel", 0, TEXTFIELD_NORMAL | LAYOUT_FILL_COLUMN | LAYOUT_FILL_X);
    myLabel->hide();
    // Create and hide textField
    myTextField = new FXTextField(this, 1, this, MID_GNE_SET_ATTRIBUTE, TEXTFIELD_NORMAL | LAYOUT_FILL_COLUMN | LAYOUT_FILL_X);
    myTextField->hide();
    // Create and hide ComboBox
    myChoicesCombo = new FXComboBox(this, 12, this, MID_GNE_SET_ATTRIBUTE, FRAME_SUNKEN | COMBOBOX_STATIC | LAYOUT_CENTER_Y | LAYOUT_FILL_COLUMN | LAYOUT_FILL_X);
    myChoicesCombo->hide();
    // Create and hide checkButton
    myCheck = new FXCheckButton(this, "AttributeBool", this, MID_GNE_SET_ATTRIBUTE, JUSTIFY_LEFT | ICON_BEFORE_TEXT | LAYOUT_FILL_COLUMN | LAYOUT_FILL_X);
    myCheck->hide();
}

    
void 
GNEInspectorFrame::AttrInput::showAttribute(SumoXMLAttr attr) {
    show();
    myLabel->show();
    myTextField->show();

    /**
    FXuint opts;
    std::string label;
    
    // Obtain discrete and combinable choices
    const std::vector<std::string>& choices = GNEAttributeCarrier::discreteChoices(myTag, myAttr);
    const bool combinableChoices = choices.size() > 0 && GNEAttributeCarrier::discreteCombinableChoices(myTag, myAttr);



    if (combinableChoices != NULL) {
        opts = BUTTON_NORMAL;
        label = toString(attr) + "\t\tOpen edit dialog for attribute '" + toString(attr) + "'";
    } else {
        opts = 0;
        label = toString(attr);
    }

    
    FXButton* but = new FXButton(this, label.c_str(), 0, inspectorFrameParent, MID_GNE_OPEN_ATTRIBUTE_EDITOR, opts);
    /*
    int cols =  1;//(parent->getWidth() - but->getDefaultWidth() - 6) / 9;
    std::cout << cols << std::endl;
    
    
    if (choices.size() == 0 || combinableChoices) {
        // rudimentary input restriction
        unsigned int numerical = GNEAttributeCarrier::isNumerical(attr) ? TEXTFIELD_REAL : 0;
        myTextField = new FXTextField(this, cols, this, MID_GNE_SET_ATTRIBUTE, TEXTFIELD_NORMAL | LAYOUT_RIGHT | LAYOUT_FILL_X | numerical, 0, 0, 0, 0, 4, 2, 0, 2);
        myTextField->setText(initialValue.c_str());
    } else {
        myChoicesCombo = new FXComboBox(this, 12, this, MID_GNE_SET_ATTRIBUTE, FRAME_SUNKEN | LAYOUT_LEFT | LAYOUT_TOP | COMBOBOX_STATIC | LAYOUT_CENTER_Y);
        for (std::vector<std::string>::const_iterator it = choices.begin(); it != choices.end(); ++it) {
            myChoicesCombo->appendItem(it->c_str());
        }
        myChoicesCombo->setNumVisible((int)choices.size());
        myChoicesCombo->setCurrentItem(myChoicesCombo->findItem(initialValue.c_str()));
    }
    */
}


long
GNEInspectorFrame::AttrInput::onCmdOpenAttributeEditor(FXObject*, FXSelector, void*) {
    /*
    FXDialogBox* editor = new FXDialogBox(getApp(),
                                          ("Select " + toString(myAttr) + "ed").c_str(),
                                          DECOR_CLOSE | DECOR_TITLE);
    FXMatrix* m1 = new FXMatrix(editor, 2, MATRIX_BY_COLUMNS);
    const std::vector<std::string>& choices = GNEAttributeCarrier::discreteChoices(myTag, myAttr);
    std::vector<FXCheckButton*> vClassButtons;
    const std::string oldValue(myTextField->getText().text());
    for (std::vector<std::string>::const_iterator it = choices.begin(); it != choices.end(); ++it) {
        vClassButtons.push_back(new FXCheckButton(m1, (*it).c_str()));
        if (oldValue.find(*it) != std::string::npos) {
            vClassButtons.back()->setCheck(true);
        }
    }
    // buttons
    new FXHorizontalSeparator(m1, SEPARATOR_GROOVE | LAYOUT_FILL_X, 0, 0, 0, 2, 2, 2, 4, 4);
    new FXHorizontalSeparator(m1, SEPARATOR_GROOVE | LAYOUT_FILL_X, 0, 0, 0, 2, 2, 2, 4, 4);
    // "Cancel"
    new FXButton(m1, "Cancel\t\tDiscard modifications", 0, editor, FXDialogBox::ID_CANCEL,
                 ICON_BEFORE_TEXT | LAYOUT_FILL_X | FRAME_THICK | FRAME_RAISED,
                 0, 0, 0, 0, 4, 4, 3, 3);
    // "OK"
    new FXButton(m1, "OK\t\tSave modifications", 0, editor, FXDialogBox::ID_ACCEPT,
                 ICON_BEFORE_TEXT | LAYOUT_FILL_X | FRAME_THICK | FRAME_RAISED,
                 0, 0, 0, 0, 4, 4, 3, 3);
    editor->create();
    if (editor->execute()) {
        std::vector<std::string> vClasses;
        for (std::vector<FXCheckButton*>::const_iterator it = vClassButtons.begin(); it != vClassButtons.end(); ++it) {
            if ((*it)->getCheck()) {
                vClasses.push_back(std::string((*it)->getText().text()));
            }
        }
        myTextField->setText(joinToString(vClasses, " ").c_str());
        onCmdSetAttribute(0, 0, 0);
    }
    */
    return 1;
}


long
GNEInspectorFrame::AttrInput::onCmdSetAttribute(FXObject*, FXSelector, void* data) {
    /*
    std::string newVal(myTextField != 0 ? myTextField->getText().text() : (char*) data);
    const std::vector<GNEAttributeCarrier*>& ACs = *myACs;
    if (ACs[0]->isValid(myAttr, newVal)) {
        // if its valid for the first AC than its valid for all (of the same type)
        if (ACs.size() > 1) {
            myInspectorFrameParent->getViewNet()->getUndoList()->p_begin("Change multiple attributes");
        }
        for (std::vector<GNEAttributeCarrier*>::const_iterator it_ac = ACs.begin(); it_ac != ACs.end(); it_ac++) {
            (*it_ac)->setAttribute(myAttr, newVal, myInspectorFrameParent->getViewNet()->getUndoList());
        }
        if (ACs.size() > 1) {
            myInspectorFrameParent->getViewNet()->getUndoList()->p_end();
        }
        if (myTextField != 0) {
            myTextField->setTextColor(FXRGB(0, 0, 0));
            myTextField->killFocus();
        }
    } else {
        if (myTextField != 0) {
            myTextField->setTextColor(FXRGB(255, 0, 0));
        }
    }
    */
    return 1;
}


/****************************************************************************/
