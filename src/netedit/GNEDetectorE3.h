/****************************************************************************/
/// @file    GNEDetectorE3.h
/// @author  Pablo Alvarez Lopez
/// @date    Nov 2015
/// @version $Id: GNEDetectorE3.h 19790 2016-01-25 11:59:12Z palcraft $
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
#ifndef GNEDetectorE3_h
#define GNEDetectorE3_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include "GNEDetector.h"

// ===========================================================================
// class declarations
// ===========================================================================
class GNEDetector;

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GNEDetectorE3
 * ------------
 */
class GNEDetectorE3 : public GNEDetector {
public:
    /** @brief Constructor
     * @param[in] id The storage of gl-ids to get the one for this lane representation from
     * @param[in] lane Lane of this StoppingPlace belongs
     * @param[in] viewNet pointer to GNEViewNet of this additional element belongs
     * @param[in] pos position of the detector on the lane
     * @param[in] freq the aggregation period the values the detector collects shall be summed up.
     
     * @param[in] splitByType If set, the collected values will be additionally reported on per-vehicle type base.

     * @exception InvalidArgument If the detector can not be added to the net (is duplicate)
     */
    GNEDetectorE3(const std::string& id, GNELane& lane, GNEViewNet* viewNet, SUMOReal pos, SUMOReal freq, const std::string& filename, bool splitByType);

    /// @brief Destructor
    ~GNEDetectorE3();

    /** @brief writte additional element into a xml file
     * @param[in] device device in which write parameters of additional element
     */
    void writeAdditional(OutputDevice& device);

    /** @brief get split by type
     * @return true if split by type is enabled, false in other case
     */
    bool getSplitByType() const;

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
    /// @brief attribute to enable or disable splitByType
    bool mySplitByType;

private:
    /// @brief variable to save detectorE3 icon
    static GUIGlID detectorE3GlID;

    /// @brief check if detectorE3 icon was inicilalizated
    static bool detectorE3Initialized;

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
    GNEDetectorE3(const GNEDetectorE3&);

    /// @brief Invalidated assignment operator.
    GNEDetectorE3& operator=(const GNEDetectorE3&);
};

#endif
/****************************************************************************/