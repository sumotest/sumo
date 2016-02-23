/****************************************************************************/
/// @file    GNEDetectorE1.h
/// @author  Pablo Alvarez Lopez
/// @date    Nov 2015
/// @version $Id: GNEDetectorE1.h 19790 2016-01-25 11:59:12Z palcraft $
///
/// A abstract class to define common parameters of detectors 
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
#ifndef GNEDetector_h
#define GNEDetector_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include "GNEAdditional.h"

// ===========================================================================
// class declarations
// ===========================================================================
class GNEAdditional;
class GNEViewNet;

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GNEdetector
 * @briefA abstract class to define common parameters and functions of detectors
 */
class GNEDetector : public GNEAdditional {
public:
    /** @brief Constructor.
     * @param[in] id Gl-id of the detector (Must be unique)
     * @param[in] lane Lane of this detector belongs
     * @param[in] viewNet pointer to GNEViewNet of this additional element belongs
     * @param[in] tag Type of xml tag that define the detector (SUMO_TAG_E1DETECTOR, SUMO_TAG_LANE_AREA_DETECTOR, etc...)
     * @param[in] pos position of detector in lane
     * @param[in] length length of the detector
     * @param[in] freq the aggregation period the values the detector collects shall be summed up.
     * @param[in] filename The path to the output file.
     */
    GNEDetector(const std::string& id, GNELane& lane, GNEViewNet* viewNet, SumoXMLTag tag, SUMOReal pos, SUMOReal length, int freq, const std::string &filename);

    /// @brief Destructor
    ~GNEDetector();

    /// @brief update pre-computed geometry information
    void updateGeometry();

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

    /// @brief Returns the position of the detector over lane
    SUMOReal getPosition() const;

    /// @brief returns the aggregation period the values the detector collects shall be summed up.
    int getFrequency() const;

    /// @brief returns the path to the output file
    std::string getFilename() const;

    /** @brief Set a new position in detector
     * @param[in] pos new position of detector
     * @throws InvalidArgument if value of pos isn't valid
     */
    void setPosition(SUMOReal pos);

    /** @brief Set a new frequency in detector
     * @param[in] freq new frequency of detector
     * @throws InvalidArgument if value of frequency isn't valid
     */
    void setFrequency(int freq);

    /** @brief Set a new filename in detector
     * @param[in] filename new filename of detector
     */
    void setFilename(std::string filename);

    /** @brief Returns a copy of the Shape of the detector
     * @return The Shape of the detector
     */
    PositionVector getShape() const;

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
    /// @brief The length of the Detector (Only as parameter in E2)
    SUMOReal myLength;
    
    /// @brief The aggregation period the values the detector collects shall be summed up.
    int myFreq;

    /// @brief The path to the output file
    std::string myFilename;

    /// @brief The position of detector
    Position myDetectorLogoPosition;

    /// @brief The rotation of detector
    SUMOReal myRotation;

    /// @brief The rotation of signal detector
    SUMOReal mySignRotation;

private:

    /// @brief set attribute after validation
    virtual void setAttribute(SumoXMLAttr key, const std::string& value) = 0;

    /// @brief set colors of scheme
    virtual void setColors() = 0;
};

#endif
/****************************************************************************/