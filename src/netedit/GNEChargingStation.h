/****************************************************************************/
/// @file    GNEChargingStation.h
/// @author  Pablo Alvarez Lopez
/// @date    Nov 2015
/// @version $Id: $
///
/// A class for visualizing chargingStation geometry (adapted from GUILaneWrapper)
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
#ifndef GNEChargingStation_h
#define GNEChargingStation_h

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

class GNEChargingStation : public GNEStoppingPlace
{
public:

	/** @brief Constructor.
     * @param[in] 
     * @param[in] 
     */
    GNEChargingStation(const std::string& id, GNELane& lane, SUMOReal fromPpos, SUMOReal toPos, 
		               SUMOReal chargingPower, SUMOReal efficiency, bool chargeInTransit, SUMOReal chargeDelay);

	~GNEChargingStation();

    ///@brief update pre-computed geometry information
    //  @note: must be called when geometry changes (i.e. junction moved)
    void updateGeometry();


    SUMOReal getChargingPower();

    SUMOReal getEfficiency(); 

    bool getChargeInTransit(); 

    SUMOReal getChargeDelay();

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

    void setChargingPower(SUMOReal chargingPower);

    void setEfficiency(SUMOReal efficiency); 

    void setChargeInTransit(bool chargeinTransit); 

    void setChargeDelay(SUMOReal chargeDelay);



    /// @name inherited from GUIGlObject
    //@{
    /** @brief Returns an own popup-menu
     *
     * @param[in] app The application needed to build the popup-menu
     * @param[in] parent The parent window needed to build the popup-menu
     * @return The built popup-menu
     * @see GUIGlObject::getPopUpMenu
     */
    GUIGLObjectPopupMenu* getPopUpMenu(GUIMainWindow& app, GUISUMOAbstractView& parent) ;


    /** @brief Returns an own parameter window
     *
     * @param[in] app The application needed to build the parameter window
     * @param[in] parent The parent window needed to build the parameter window
     * @return The built parameter window
     * @see GUIGlObject::getParameterWindow
     */
    GUIParameterTableWindow* getParameterWindow(GUIMainWindow& app, GUISUMOAbstractView& parent) ;


    /** @brief Returns the boundary to which the view shall be centered in order to show the object
     *
     * @return The boundary the object is within
     * @see GUIGlObject::getCenteringBoundary
     */
    Boundary getCenteringBoundary() const ;


    /** @brief Draws the object
     * @param[in] s The settings for the current view (may influence drawing)
     * @see GUIGlObject::drawGL
     */
    void drawGL(const GUIVisualizationSettings& s) const ;
    //@}


    //@name inherited from GNEAttributeCarrier
    //@{
    std::string getAttribute(SumoXMLAttr key) const;

    /* @brief method for setting the attribute and letting the object perform additional changes
     * @param[in] key The attribute key
     * @param[in] value The new value
     * @param[in] undoList The undoList on which to register changes
     */
    void setAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList);

    bool isValid(SumoXMLAttr key, const std::string& value);
    //@}

private:
    /// @brief Charging power pro timestep
    SUMOReal myChargingPower;

    /// @brief efficiency of the charge
    SUMOReal myEfficiency; 

    /// @brief enable or disable charge in transit
    bool myChargeInTransit; 

    /// @brief delay in the starting of charge
    SUMOReal myChargeDelay;

    /// @brief The shape
    PositionVector myShape;

    /// @brief The position of the sign
    Position mySignPos;

    /// @brief The rotation of the sign
    SUMOReal mySignRot;

    /// @name computed only once (for performance) in updateGeometry()
    //@{
    /// The rotations of the shape parts
    std::vector<SUMOReal> myShapeRotations;

    /// The lengths of the shape parts
    std::vector<SUMOReal> myShapeLengths;
    //@}

private:
    /// @brief set attribute after validation
    void setAttribute(SumoXMLAttr key, const std::string& value);

    /// @brief Invalidated copy constructor.
    GNEChargingStation(const GNEChargingStation&);

    /// @brief Invalidated assignment operator.
    GNEChargingStation& operator=(const GNEChargingStation&);

    // @brief return value for lane coloring according to the given scheme
    SUMOReal getColorValue(size_t activeScheme) const;
		
    /// @brief sets the color according to the current scheme index and some lane function
    bool setFunctionalColor(size_t activeScheme) const;

    /// @brief sets multiple colors according to the current scheme index and some lane function
    bool setMultiColor(const GUIColorer& c) const;
	
    /// The color of the shape parts (cached)
    mutable std::vector<RGBColor> myShapeColors;
};


#endif
