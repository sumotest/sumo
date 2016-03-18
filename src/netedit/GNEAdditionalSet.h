/****************************************************************************/
/// @file    GNEAdditionalSet.h
/// @author  Pablo Alvarez Lopez
/// @date    Feb 2016
/// @version $Id: GNEAdditionalSet.h 19909 2016-02-08 12:22:59Z palcraft $
///
/// A abstract class for representation of additionalSet elements
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
#ifndef GNEAdditionalSet_h
#define GNEAdditionalSet_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif
/*
#include <string>
#include <vector>
#include <utils/gui/globjects/GUIGlObject.h>
#include <utils/gui/settings/GUIPropertySchemeStorage.h>
#include <utils/geom/PositionVector.h>
*/
#include "GNEAdditional.h"

// ===========================================================================
// class declarations
// ===========================================================================

class GUIGLObjectPopupMenu;
class PositionVector;
class GNEAdditional;
class GNENet;
class GNEViewNet;

// ===========================================================================
// class definitions
// ===========================================================================

/**
 * @class GNEAdditionalSet
 * @brief An Element wich group additionalSet elements
 */
class GNEAdditionalSet : public GNEAdditional
{
public:

    /** @brief Constructor.
     * @param[in] id Gl-id of the additionalSet element (Must be unique)
     * @param[in] viewNet pointer to GNEViewNet of this additionalSet element belongs
     * @param[in] tag Type of xml tag that define the additionalSet element (SUMO_TAG_DETECTORE3, SUMO_TAG_REROUTER, etc...)
     * @param[in] blocked enable or disable blocking. By default additionalSet element isn't blocked (i.e. value is false)
     * @param[in] lane Pointer to lane, NULL if additional don't belong to a Lane
     */
    GNEAdditionalSet(const std::string& id, GNEViewNet* viewNet, Position pos, SumoXMLTag tag, bool blocked = false, GNELane *lane = NULL);

    /// @brief Destructor
    ~GNEAdditionalSet();

    /// @brief update pre-computed geometry information
    //  @note: must be called when geometry changes (i.e. lane moved)
    virtual void updateGeometry() = 0;

    /** @brief add additional element to this set
     * @param[in] additionalSet pointer to GNEadditionalSet element to add
     * @throw ProcessError if this additionalSet element was already vinculated with another additionalSet
     */
    void addAdditionalChild(GNEAdditional *additional);

    /** @brief remove additional element to this set
     * @param[in] additionalSet pointer to GNEadditionalSet element to remove
     * @throw ProcessError if this additionalSet element isn't  vinculated with this additionalSet
     */
    void removeAdditionalChild(GNEAdditional *additional);

    /** @brief writte additionalSet element into a xml file
     * @param[in] device device in which write parameters of additionalSet element
     */
    virtual void writeAdditional(OutputDevice& device) = 0;

    /// @name inherited from GUIGlObject
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

    /** @brief Draws the object
     * @param[in] s The settings for the current view (may influence drawing)
     * @see GUIGlObject::drawGL
     */
    virtual void drawGL(const GUIVisualizationSettings& s) const = 0;

    /** @brief Draws additionally triggered visualisations
     * @param[in] parent The view
     * @param[in] s The settings for the current view (may influence drawing)
     */
    virtual void drawGLAdditional(GUISUMOAbstractView* const parent, const GUIVisualizationSettings& s) const = 0;
    //@}

    //@name inherited from GNEAttributeCarrier
    //@{
    /* @brief method for getting the Attribute of an XML key
     * @param[in] key The attribute key
     * @return string with the value associated to key
     */
    virtual std::string getAttribute(SumoXMLAttr key) const = 0;

    /* @brief method for setting the attribute and letting the object perform additionalSet changes
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
    /// @brief Map with the GNEAdditionals elementen vinculated to this AdditionalSet and their middle point
    std::map<GNEAdditional*, Position> myAdditionals;

    /// @brief update connections.
    /// @note must be called at end of function updateGeometry of AdditionalSet parent
    void updateConnections();

    /// @brief draw connections.
    void drawConnections() const;

    /// @brief write children of this additionalSet
    void writeAdditionalChildrens(OutputDevice& device);

private:
    /// @brief set attribute after validation
    virtual void setAttribute(SumoXMLAttr key, const std::string& value) = 0;

    /// @brief set colors of scheme
    virtual void setColors() = 0;

    /// @brief Invalidated copy constructor.
    GNEAdditionalSet(const GNEAdditionalSet&);

    /// @brief Invalidated assignment operator.
    GNEAdditionalSet& operator=(const GNEAdditionalSet&);
};


#endif
