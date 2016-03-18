/****************************************************************************/
/// @file    GNERerouter.h
/// @author  Pablo Alvarez Lopez
/// @date    Nov 2015
/// @version $Id: GNERerouter.h 19790 2016-01-25 11:59:12Z palcraft $
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
#ifndef GNERerouter_h
#define GNERerouter_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include "GNEAdditionalSet.h"

// ===========================================================================
// class declarations
// ===========================================================================

class GNERoute;

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GNERerouter
 * ------------
 */
class GNERerouter : public GNEAdditionalSet {
public:
    /** @brief Constructor
     * @param[in] id The storage of gl-ids to get the one for this lane representation from
     * @param[in] viewNet pointer to GNEViewNet of this additional element belongs
     * @param[in] pos position (center) of the rerouter in the map
     * @param[in] routes vector of routes
     * @param[in] filename The path to the definition file
     * @param[in] probability The probability for vehicle rerouting 
     * @param[in] off Whether the router should be inactive initially
     * @param[in] blocked set initial blocking state of item 
     */
    GNERerouter(const std::string& id, GNEViewNet* viewNet, Position pos, std::vector<GNERoute*> routes, const std::string& filename, SUMOReal probability, bool off, bool blocked);

    /// @brief Destructor
    ~GNERerouter();

    /// @brief update pre-computed geometry information
    /// @note: must be called when geometry changes (i.e. lane moved)
    void updateGeometry();

    /** @brief change the position of the rerouter geometry without registering undo/redo
     * @param[in] posx new x position of rerouter in the map
     * @param[in] posy new y position of rerouter in the map
     * @param[in] undoList pointer to the undo list
     */
    void moveAdditional(SUMOReal posx, SUMOReal posy, GNEUndoList *undoList);

    /** @brief writte additional element into a xml file
     * @param[in] device device in which write parameters of additional element
     */
    void writeAdditional(OutputDevice& device);

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

protected:
    /// @brief list of routes
    std::vector<GNERoute*> myRoutes; 
    
    /// @brief afilename of rerouter
    std::string myFilename; 
    
    /// @brief probabiliry of rerouter
    SUMOReal myProbability; 
    
    /// @brief attribute to enable or disable off
    bool myOff;

private:
    /// @brief variable to save detectorE1 icon
    static GUIGlID rerouterGlID;

    /// @brief check if detectorE1 icon was inicilalizated
    static bool rerouterInitialized;

    /// @brief set attribute after validation
    void setAttribute(SumoXMLAttr key, const std::string& value);

    /// @brief set colors of scheme
    void setColors();

    /// @brief list of colors
    enum colorTypes {
        E1_BASE = 0,
        E1_BASE_SELECTED = 1,
    };

    /// @brief Invalidated copy constructor.
    GNERerouter(const GNERerouter&);

    /// @brief Invalidated assignment operator.
    GNERerouter& operator=(const GNERerouter&);
};

#endif
/****************************************************************************/