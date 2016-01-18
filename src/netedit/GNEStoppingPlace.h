/****************************************************************************/
/// @file    GNEStoppingPlace.h
/// @author  Pablo Alvarez Lopez
/// @date    Dec 2015
/// @version $Id: $
///
/// A abstract class to define common parameters of lane area in which vehicles can halt (GNE version)
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
#ifndef GNEStoppingPlace_h
#define GNEStoppingPlace_h
// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <vector>
#include <string>
#include <utils/gui/globjects/GLIncludes.h>
#include <utils/geom/Position.h>
#include <utils/geom/Boundary.h>
#include <utils/gui/globjects/GUIGlObject.h>
#include <utils/gui/globjects/GUIGlObjectStorage.h>
#include "GNEAdditional.h"

// ===========================================================================
// class declarations
// ===========================================================================

class GUIGLObjectPopupMenu;
class PositionVector;
class GNENet;

// ===========================================================================
// class definitions
// ===========================================================================

class GNEStoppingPlace : public GNEAdditional
{
public:

    /** @brief Constructor.
     * @param[in] id Gl-id of the stopping place (Must be unique)
     * @param[in] lane Lane of this StoppingPlace belongs
     * @param[in] frompos From position of the StoppingPlace
     * @param[in] topos To position of the StoppingPlace
     * @param[in] tag Type of xml tag that define the StoppingPlace (SUMO_TAG_BUS_STOP, SUMO_TAG_CHARGING_STATION, etc...)
     */
    GNEStoppingPlace(const std::string& id, GNELane& lane, SUMOReal fromPos, SUMOReal toPos, SumoXMLTag tag);

    /// @brief Destructor
    ~GNEStoppingPlace();

    /// @brief update pre-computed geometry information
    //  @note: must be called when geometry changes (i.e. lane moved)
    virtual void updateGeometry() = 0;

    /** @brief Returns the from position of the stoppingPlace
     * @return The from position of the stopping place
     */
    SUMOReal getFromPosition() const;
        
    /** @brief Returns the to position of the stoppingPlace
     * @return The to position of the stopping place
     */
    SUMOReal getToPosition() const;

    /** @brief Set a new from Position in StoppingPlace
     * @param[in] fromPos new From Position of StoppingPlace
     * @throws InvalidArgument if value of fromPos isn't valid
     */
    void setFromPosition(SUMOReal fromPos);

    /** @brief Set a new to Position in StoppingPlace
     * @param[in] toPos new to Position of StoppingPlace
     * @throws InvalidArgument if value of toPos isn't valid
     */
    void setToPosition(SUMOReal toPos);

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

    /// @name inherited from GNEAdditional
    //@{
    /** @brief Returns an own popup-menu
     *
     * @param[in] app The application needed to build the popup-menu
     * @param[in] parent The parent window needed to build the popup-menu
     * @return The built popup-menu
     * @see GUIGlObject::getPopUpMenu
     */
    virtual GUIGLObjectPopupMenu* getPopUpMenu(GUIMainWindow& app, GUISUMOAbstractView& parent) = 0;

    /** @brief Returns an own parameter window
     *
     * @param[in] app The application needed to build the parameter window
     * @param[in] parent The parent window needed to build the parameter window
     * @return The built parameter window
     * @see GUIGlObject::getParameterWindow
     */
    virtual GUIParameterTableWindow* getParameterWindow(GUIMainWindow& app, GUISUMOAbstractView& parent) = 0;

    /** @brief Returns the boundary to which the view shall be centered in order to show the object
     *
     * @return The boundary the object is within
     * @see GUIGlObject::getCenteringBoundary
     */
    virtual Boundary getCenteringBoundary() const = 0;

    /** @brief Draws the object
     * @param[in] s The settings for the current view (may influence drawing)
     * @see GUIGlObject::drawGL
     */
    virtual void drawGL(const GUIVisualizationSettings& s) const = 0;
    //@}

    //@name inherited from GNEAttributeCarrier
    //@{
    /* @brief method for getting the Attribute of an XML key
     * @param[in] key The attribute key
     * @return string with the value associated to key
     */
    virtual std::string getAttribute(SumoXMLAttr key) const = 0;

    /* @brief method for setting the attribute and letting the object perform additional changes
     * @param[in] key The attribute key
     * @param[in] value The new value
     * @param[in] undoList The undoList on which to register changes
     */
    virtual void setAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList) = 0;

    /* @brief method for checking if the key and their conrrespond attribute are valids
     * @param[in] key The attribute key
     * @param[in] value The value asociated to key key
     * @return true if the value is valid, false in other case
     */
    virtual bool isValid(SumoXMLAttr key, const std::string& value) = 0;
    //@}

private:

    /// @brief The begin position this stopping place is located at
    SUMOReal myFromPos;

    /// @brief The end position this stopping place is located at
    SUMOReal myToPos;

/**
    ALL OF THIS FUNCTIONS SHOULD BE ERASED?

    // @brief return value for lane coloring according to the given scheme
    SUMOReal getColorValue(size_t activeScheme) const;
        
    /// @brief sets the color according to the current scheme index and some lane function
    bool setFunctionalColor(size_t activeScheme) const;

    /// @brief sets multiple colors according to the current scheme index and some lane function
    bool setMultiColor(const GUIColorer& c) const;
    
    /// The color of the shape parts (cached)
    mutable std::vector<RGBColor> myShapeColors;
**/

};


#endif
