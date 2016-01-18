/****************************************************************************/
/// @file    GNEAdditional.h
/// @author  Pablo Alvarez Lopez
/// @date    Jan 2016
/// @version $Id: $
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
     * @param[in] id Gl-id of the stopping place (Must be unique)
     * @param[in] lane Lane of this StoppingPlace belongs
     * @param[in] tag Type of xml tag that define the StoppingPlace (SUMO_TAG_BUS_STOP, SUMO_TAG_CHARGING_STATION, etc...)
     */
    GNEAdditional(const std::string& id, GNELane& lane, SumoXMLTag tag);


    /// @brief Destructor
	~GNEAdditional();


    /// @brief update pre-computed geometry information
    //  @note: must be called when geometry changes (i.e. lane moved)
    virtual void updateGeometry() = 0;


	/** @brief Returns parent lane
     * @return The GNElane parent lane
     */
	GNELane &getLane() const;


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

    /// @brief The lane this stopping place is located at
    GNELane& myLane;

private:
    /// @brief set attribute after validation
    virtual void setAttribute(SumoXMLAttr key, const std::string& value) = 0;

    /// @brief Invalidated copy constructor.
    GNEAdditional(const GNEAdditional&);

    /// @brief Invalidated assignment operator.
    GNEAdditional& operator=(const GNEAdditional&);

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
