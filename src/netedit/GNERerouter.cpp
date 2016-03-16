/****************************************************************************/
/// @file    GNERerouter.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Nov 2015
/// @version $Id: GNERerouter.cpp 19861 2016-02-01 09:08:47Z palcraft $
///
/// 
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

#include <string>
#include <iostream>
#include <utility>
#include <foreign/polyfonts/polyfonts.h>
#include <utils/geom/PositionVector.h>
#include <utils/common/RandHelper.h>
#include <utils/common/SUMOVehicleClass.h>
#include <utils/common/ToString.h>
#include <utils/geom/GeomHelper.h>
#include <utils/gui/windows/GUISUMOAbstractView.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/images/GUIIconSubSys.h>
#include <utils/gui/div/GUIParameterTableWindow.h>
#include <utils/gui/globjects/GUIGLObjectPopupMenu.h>
#include <utils/gui/div/GUIGlobalSelection.h>
#include <utils/gui/div/GLHelper.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/images/GUITexturesHelper.h>
#include <utils/xml/SUMOSAXHandler.h>

#include "GNERerouter.h"
#include "GNELane.h"
#include "GNEViewNet.h"
#include "GNEUndoList.h"
#include "GNENet.h"
#include "GNEChange_Attribute.h"
#include "GNELogo_E3.cpp"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif

// ===========================================================================
// static member definitions
// ===========================================================================
GUIGlID GNERerouter::rerouterGlID = 0;
bool GNERerouter::rerouterInitialized = false;

// ===========================================================================
// member method definitions
// ===========================================================================

GNERerouter::GNERerouter(const std::string& id, GNEViewNet* viewNet, Position pos, std::vector<GNERoute*> routes, const std::string& filename, SUMOReal probability, bool off, bool blocked) :
    GNEAdditionalSet(id, viewNet, pos, SUMO_TAG_E3DETECTOR, blocked),
    myRoutes(routes),
    myFilename(filename),
    myProbability(probability), 
    myOff(off) {
    // Update geometry;
    updateGeometry();
    // Set colors
    setColors();
}


GNERerouter::~GNERerouter() {
}


void 
GNERerouter::updateGeometry() {
    myShape.clear();
    myShape.push_back(myPosition);
}


void 
GNERerouter::moveAdditional(SUMOReal posx, SUMOReal posy, GNEUndoList *undoList) {
    // if item isn't blocked
    if(myBlocked == false) {
        // change Position
        undoList->p_add(new GNEChange_Attribute(this, SUMO_ATTR_POSITION, toString(Position(posx, posy, 0))));
    }
}


void 
GNERerouter::writeAdditional(OutputDevice& device) {

}


const std::string& 
GNERerouter::getParentName() const {
    return "";
}

GUIGLObjectPopupMenu* 
GNERerouter::getPopUpMenu(GUIMainWindow& app, GUISUMOAbstractView& parent) {
    GUIGLObjectPopupMenu* ret = new GUIGLObjectPopupMenu(app, parent, *this);
    buildPopupHeader(ret, app);
    buildCenterPopupEntry(ret);
    new FXMenuCommand(ret, "Copy detector E1 name to clipboard", 0, ret, MID_COPY_EDGE_NAME);
    buildNameCopyPopupEntry(ret);
    buildSelectionPopupEntry(ret);
    buildPositionCopyEntry(ret, false);
    // buildShowParamsPopupEntry(ret, false);
    new FXMenuCommand(ret, ("pos: " + toString(myPosition)).c_str(), 0, 0, 0);
    // new FXMenuSeparator(ret);
    // buildPositionCopyEntry(ret, false);
    // let the GNEViewNet store the popup position
    (dynamic_cast<GNEViewNet&>(parent)).markPopupPosition();
    return ret;
}


GUIParameterTableWindow* 
GNERerouter::getParameterWindow(GUIMainWindow& app, GUISUMOAbstractView& parent) {
    GUIParameterTableWindow* ret =
        new GUIParameterTableWindow(app, *this, 2);
    /* not supported yet
    // add items
    ret->mkItem("length [m]", false, getLane().getParentEdge().getNBEdge()->getLength());
    // close building
    ret->closeBuilding();
    */
    return ret;
}


void 
GNERerouter::drawGL(const GUIVisualizationSettings& s) const {
    // Additonals element are drawed using a drawGLAdditional
    drawGLAdditional(0, s);
}


void 
GNERerouter::drawGLAdditional(GUISUMOAbstractView* const parent, const GUIVisualizationSettings& s) const {
    // Ignore Warning
    UNUSED_PARAMETER(parent);

    // Declare variables to get colors depending if the detector is selected
    RGBColor base;
    /*
    // Set colors
    if(gSelected.isSelected(getType(), getGlID())) {
        base = myRGBColors[E3_BASE_SELECTED];
    } else {
        base = myRGBColors[E3_BASE];
    }
    */
    // Start drawing adding an gl identificator
    glPushName(getGlID());
    
    // load detector logo, if wasn't inicializated
    if (!rerouterInitialized) {
        FXImage* i = new FXGIFImage(getViewNet()->getNet()->getApp(), GNELogo_E3, IMAGE_KEEP | IMAGE_SHMI | IMAGE_SHMP);
        rerouterGlID = GUITexturesHelper::add(i);
        rerouterInitialized = true;
        delete i;
    }

    // Add a draw matrix and draw E1 logo
    glPushMatrix();
    glTranslated(myShape[0].x(), myShape[0].y(), getType() + 0.1);
    glColor3d(1, 1, 1);
    glRotated(180, 0, 0, 1);
    GUITexturesHelper::drawTexturedBox(rerouterGlID, 1);

    // Pop draw matrix
    glPopMatrix();

    // Pop name
    glPopName();

    // Draw name
    drawName(getCenteringBoundary().getCenter(), s.scale, s.addName);
}


std::string 
GNERerouter::getAttribute(SumoXMLAttr key) const {
    switch (key) {
        case SUMO_ATTR_ID:
            return getMicrosimID();
        //case SUMO_ATTR_FREQUENCY:
        //    return toString(myFreq);
        case SUMO_ATTR_FILE:
            return myFilename;
        default:
            throw InvalidArgument("detector E3 attribute '" + toString(key) + "' not allowed");
    }
}


void 
GNERerouter::setAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList) {
    if (value == getAttribute(key)) {
        return; //avoid needless changes, later logic relies on the fact that attributes have changed
    }
    switch (key) {
        case SUMO_ATTR_ID:
            throw InvalidArgument("modifying detector E3 attribute '" + toString(key) + "' not allowed");
        case SUMO_ATTR_FREQUENCY:
        case SUMO_ATTR_FILE:
            undoList->p_add(new GNEChange_Attribute(this, key, value));
            updateGeometry();
            break;
        default:
            throw InvalidArgument("detector E3 attribute '" + toString(key) + "' not allowed");
    }
}


bool 
GNERerouter::isValid(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_ID:
            throw InvalidArgument("modifying detector E3 attribute '" + toString(key) + "' not allowed");
        case SUMO_ATTR_FREQUENCY:
            return (canParse<SUMOReal>(value) && parse<SUMOReal>(value) >= 0);
        case SUMO_ATTR_FILE:
            return isValidFileValue(value);
        case SUMO_ATTR_LINES:
            return isValidStringVector(value);
        default:
            throw InvalidArgument("detector E3 attribute '" + toString(key) + "' not allowed");
    }
}


void 
GNERerouter::setAttribute(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case SUMO_ATTR_ID:
        case SUMO_ATTR_LANE:
            throw InvalidArgument("modifying detector E3 attribute '" + toString(key) + "' not allowed");
        case SUMO_ATTR_FREQUENCY:
            //myFreq = parse<SUMOReal>(value);
            break;
        case SUMO_ATTR_FILE:
            myFilename = value;
            break;
        default:
            throw InvalidArgument("detector E3 attribute '" + toString(key) + "' not allowed");
    }
}


void 
GNERerouter::setColors() {
    // Color E3_BASE
    myRGBColors.push_back(RGBColor(76, 170, 50, 255));
    // Color E3_BASE_SELECTED
    myRGBColors.push_back(RGBColor(161, 255, 135, 255));

}

/****************************************************************************/