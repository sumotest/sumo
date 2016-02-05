/****************************************************************************/
/// @file    GNEStoppingPlace.h
/// @author  Pablo Alvarez Lopez
/// @date    Dec 2015
/// @version $Id$
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
     * @param[in] viewNet pointer to GNEViewNet of this additional element belongs
     * @param[in] tag Type of xml tag that define the StoppingPlace (SUMO_TAG_BUS_STOP, SUMO_TAG_CHARGING_STATION, etc...)
     * @param[in] fromPos From position of the StoppingPlace
     * @param[in] toPos To position of the StoppingPlace
     */
    GNEStoppingPlace(const std::string& id, GNELane& lane, GNEViewNet* viewNet, SumoXMLTag tag, SUMOReal fromPos, SUMOReal toPos);

    /// @brief Destructorc
    ~GNEStoppingPlace();

    /// @brief update pre-computed geometry information
    virtual void updateGeometry() = 0;

    /** @brief change the position of the additonal geometry without registering undo/redo
     * @param[in] distance value for the movement. Positive for right, negative for left
     * @param[in] undoList pointer to the undo list
     * @return newPos if something was moved, oldPos if nothing was moved
     */
    void moveAdditional(SUMOReal distance, GNEUndoList *undoList);

    /** @brief writte additional element into a xml file
     * @param[in] device device in which write parameters of additional element
     */
    virtual void writeAdditional(OutputDevice& device) = 0;

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

protected:

    /// @name computed only once (for performance) in updateGeometry()
    //@{
    /// The rotations of the shape parts
    std::vector<SUMOReal> myShapeRotations;

    /// The lengths of the shape parts
    std::vector<SUMOReal> myShapeLengths;
    //@}

    /// @brief The begin position this stopping place is located at
    SUMOReal myFromPos;

    /// @brief The end position this stopping place is located at
    SUMOReal myToPos;

    /// @brief The position of the sign
    Position mySignPos;

    /// @brief The rotation of the sign
    SUMOReal mySignRot;

    /// @brief vector with the different colors
    std::vector<RGBColor> myRGBColors;

private:

    /// @brief set attribute after validation
    virtual void setAttribute(SumoXMLAttr key, const std::string& value) = 0;

    /// @brief set colors of scheme
    virtual void setColors() = 0;
};


#endif
