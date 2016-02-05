/****************************************************************************/
/// @file    GNEAdditional.h
/// @author  Pablo Alvarez Lopez
/// @date    Jan 2016
/// @version $Id$
///
/// A abstract class for representation of additional elements
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
#ifndef GNEAdditional_h
#define GNEAdditional_h


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


// ===========================================================================
// class declarations
// ===========================================================================
class GUIGLObjectPopupMenu;
class PositionVector;
class GNELane;
class GNENet;
class GNEViewNet;

// ===========================================================================
// class definitions
// ===========================================================================

/**
 * @class GNEAdditional
 * @brief A lane area vehicles can halt at (netedit-version)
 *
 * @see MSStoppingPlace
 */

class GNEAdditional : public GUIGlObject, public GNEAttributeCarrier
{
public:

    /** @brief Constructor.
     * @param[in] id Gl-id of the additional element (Must be unique)
     * @param[in] lane GNELane of this additional element belongs
     * @param[in] viewNet pointer to GNEViewNet of this additional element belongs
     * @param[in] tag Type of xml tag that define the additional element (SUMO_TAG_BUS_STOP, SUMO_TAG_REROUTER, etc...)
     * @param[in] blocked enable or disable blocking. By default additional element isn't blocked (i.e. value is false)
     */
    GNEAdditional(const std::string& id, GNELane& lane, GNEViewNet* viewNet, SumoXMLTag tag, bool blocked = false);

    /// @brief Destructor
    ~GNEAdditional();

    /// @brief update pre-computed geometry information
    //  @note: must be called when geometry changes (i.e. lane moved)
    virtual void updateGeometry() = 0;

    /** @brief Returns parent lane
     * @return The GNElane parent lane
     */
    GNELane &getLane() const;

    /** @brief Returns View Net
     * @return The GNEViewNet in which additional element is located
     */
    GNEViewNet* getViewNet() const;

    /** @brief Returns string with the information of the shape
     * @return The string with the shape of the additional element
     */
    std::string getShape() const;

    /** @brief Check if item is blocked (i.e. cannot be moved with mouse)
     * @return true if element is blocked, false in other case
     */
    bool isBlocked() const;

    /** @brief Block or unblock additional element(i.e. cannot be moved with mouse)
     * @param[in] value true or false
     */
    void setBlocked(bool value);

    /** @brief change the position of the additional geometry without registering undo/redo
     * @param[in] distance value for the movement. Positive for right, negative for left
     * @param[in] undoList pointer to the undo list
     * @return newPos if something was moved, oldPos if nothing was moved
     */
    virtual void moveAdditional(SUMOReal distance, GNEUndoList *undoList) = 0;

    /** @brief writte additional element into a xml file
     * @param[in] device device in which write parameters of additional element
     */
    virtual void writeAdditional(OutputDevice& device) = 0;

    /// @name inherited from GUIGlObject
    //@{
    /** @brief Returns the name of the parent object (if any)
     * @return This object's parent id
     */
    const std::string& getParentName() const; 

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
    Boundary getCenteringBoundary() const;

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
    /// @brief The lane this additional element is located at
    GNELane& myLane;

    /// @brief The GNEViewNet this additional element belongs
    GNEViewNet* myViewNet;

    /// @brief The shape of the additional element
    PositionVector myShape;

    /// @brief The position of the block icon
    Position myBlockIconPos;

    /// @brief boolean to check if additional element is blocked (i.e. cannot be moved with mouse)
    bool myBlocked; 

    // @brief draw lock icon
    void drawLockIcon() const;

private:
    /// @brief variable to keep GLId of the additional lock image
    static GUIGlID additionalLockGlID;

    /// @brief boolean to check if additional lock image was inicializated
    static bool additionalLockInitialized;

    /// @brief set attribute after validation
    virtual void setAttribute(SumoXMLAttr key, const std::string& value) = 0;

    /// @brief Invalidated copy constructor.
    GNEAdditional(const GNEAdditional&);

    /// @brief Invalidated assignment operator.
    GNEAdditional& operator=(const GNEAdditional&);
};


#endif
