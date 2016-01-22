/****************************************************************************/
/// @file    GNEBusStop.h
/// @author  Pablo Alvarez Lopez
/// @date    Nov 2015
/// @version $Id$
///
/// A class for visualizing busStop geometry (adapted from GUILaneWrapper)
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
#ifndef GNEBusStop_h
#define GNEBusStop_h

// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <vector>
#include <utils/gui/globjects/GUIGlObject.h>
#include <utils/gui/settings/GUIPropertySchemeStorage.h>
#include <utils/geom/PositionVector.h>
#include "GNEAttributeCarrier.h"
#include "GNEStoppingPlace.h"


// ===========================================================================
// class declarations
// ===========================================================================
class GUIGLObjectPopupMenu;
class PositionVector;
class GNELane;
class GNENet;

// ===========================================================================
// class definitions
// ===========================================================================

/**
 * @class GNEBusStop
 * @brief A lane area vehicles can halt at (netedit-version)
 *
 * @see MSStoppingPlace
 */
class GNEBusStop : public GNEStoppingPlace {
public:
    /** @brief Constructor
     * @param[in] id The storage of gl-ids to get the one for this lane representation from
     * @param[in] lines lines of the busStop
     * @param[in] lane Lane of this StoppingPlace belongs
     * @param[in] frompos From position of the StoppingPlace
     * @param[in] topos To position of the StoppingPlace
     */
    GNEBusStop(const std::string& id, const std::vector<std::string>& lines, GNELane& lane, SUMOReal frompos, SUMOReal topos);

    /// @brief Destructor
    ~GNEBusStop();

    /// @brief update pre-computed geometry information
    ///  @note: must be called when geometry changes (i.e. junction moved)
    void updateGeometry();

    /** @brief change the position of the additonal geometry without registering undo/redo
     * @param[in] oldPos The origin of the mouse movement
     * @param[in] newPos The destination of the mouse movenent
     * @return newPos if something was moved, oldPos if nothing was moved
     */
    void moveAdditional(SUMOReal distance);

    /** @brief get lines of busStop
     * @return vector of strings with the lines of the busStop 
     */
    const std::vector<std::string> &getLines() const;

    /** @brief Returns a copy of the Shape of the stoppingPlace
     * @return The Shape of the stoppingPlace
     */
    PositionVector getShape() const;

    /** @brief Returns a copy of the ShapeRotations of the stoppingPlace
     * @return The ShapeRotations of the stoppingPlace
     */
    std::vector<SUMOReal> getShapeRotations() const;

    /** @brief Returns a copy of the ShapeLengths of the stoppingPlace
     * @return The ShapeLengths of the stoppingPlace
     */
    std::vector<SUMOReal> getShapeLengths() const;

    /// @name inherited from GUIGlObject
    //@{
    /** @brief Returns an own popup-menu
     *
     * @param[in] app The application needed to build the popup-menu
     * @param[in] parent The parent window needed to build the popup-menu
     * @return The built popup-menu
     * @see GUIGlObject::getPopUpMenu
     */
    GUIGLObjectPopupMenu* getPopUpMenu(GUIMainWindow& app, GUISUMOAbstractView& parent);

    /** @brief Returns an own parameter window
     *
     * @param[in] app The application needed to build the parameter window
     * @param[in] parent The parent window needed to build the parameter window
     * @return The built parameter window
     * @see GUIGlObject::getParameterWindow
     */
    GUIParameterTableWindow* getParameterWindow(GUIMainWindow& app, GUISUMOAbstractView& parent);

    /** @brief Returns the boundary to which the view shall be centered in order to show the object
     *
     * @return The boundary the object is within
     * @see GUIGlObject::getCenteringBoundary
     */
    Boundary getCenteringBoundary() const;

    /** @brief Draws the object
     * @param[in] s The settings for the current view (may influence drawing)
     * @see GUIGlObject::drawGL
     */
    void drawGL(const GUIVisualizationSettings& s) const;

    /** @brief Draws additionally triggered visualisations
     * @param[in] parent The view
     * @param[in] s The settings for the current view (may influence drawing)
     */
    void drawGLAdditional(GUISUMOAbstractView* const parent, const GUIVisualizationSettings& s) const;
    //@}

    //@name inherited from GNEAttributeCarrier
    //@{
    /* @brief method for getting the Attribute of an XML key
     * @param[in] key The attribute key
     * @return string with the value associated to key
     */
    std::string getAttribute(SumoXMLAttr key) const;

    /* @brief method for setting the attribute and letting the object perform additional changes
     * @param[in] key The attribute key
     * @param[in] value The new value
     * @param[in] undoList The undoList on which to register changes
     */
    void setAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList);

    /* @brief method for checking if the key and their correspond attribute are valids
     * @param[in] key The attribute key
     * @param[in] value The value asociated to key key
     * @return true if the value is valid, false in other case
     */
    bool isValid(SumoXMLAttr key, const std::string& value);
    //@}

private:
    /// @brief The list of lines that are assigned to this stop
    std::vector<std::string> myLines;

private:
    /// @brief set attribute after validation
    void setAttribute(SumoXMLAttr key, const std::string& value);

    /// @brief set colors of scheme
    void setColors();

    /// @brief list of colors QUESTION ERDMANN 01
    enum colorTypes {
        BUSSTOP_BASE = 0,
        BUSSTOP_BASE_SELECTED = 1,
        BUSSTOP_SIGN = 2,
        BUSSTOP_SIGN_SELECTED = 3,
        BUSSTOP_LETTER = 4,             // Currently the same as BUSSTOP_BASE
        BUSSTOP_LETTER_SELECTED = 5,    // Currently the same as BUSSTOP_BASE_SELECTED
    };

    /// @brief Invalidated copy constructor.
    GNEBusStop(const GNEBusStop&);

    /// @brief Invalidated assignment operator.
    GNEBusStop& operator=(const GNEBusStop&);
};


#endif
