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
class GNEAdditionalSet;

// ===========================================================================
// class definitions
// ===========================================================================

/**
 * @class GNEAdditional
 * @brief An Element which don't belongs to GNENet but has influency in the simulation
 */

class GNEAdditional : public GUIGlObject, public GNEAttributeCarrier
{
public:
    /** @brief Constructor.
     * @param[in] id Gl-id of the additional element (Must be unique)
     * @param[in] viewNet pointer to GNEViewNet of this additional element belongs
     * @param[in] pos position of view in which additional is located
     * @param[in] tag Type of xml tag that define the additional element (SUMO_TAG_BUS_STOP, SUMO_TAG_REROUTER, etc...)
        * @param[in] blocked enable or disable blocking. By default additional element isn't blocked (i.e. value is false)
     * @param[in] lane Pointer to lane, NULL if additional don't belong to a Lane
     * @param[in[ parentTag type of parent, if this additional belongs to an additionalSet 
     * @param[in] parent pointer to parent, if this additional belongs to an additionalSet
     */
    GNEAdditional(const std::string& id, GNEViewNet* viewNet, Position pos, SumoXMLTag tag, bool blocked = false, GNELane *lane = NULL, SumoXMLTag parentTag = SUMO_TAG_NOTHING, GNEAdditionalSet *parent = NULL);

    /// @brief Destructor
    ~GNEAdditional();

    /** @brief change the position of the additional geometry without registering undo/redo
     * @param[in] posx new x position of idem in the map or over lane
     * @param[in] posy new y position of item in the map
     * @param[in] undoList pointer to the undo list
     * @note if additional belongs to a Lane, posx correspond to position over lane
     */
    virtual void moveAdditional(SUMOReal posx, SUMOReal posy, GNEUndoList *undoList) = 0;

    /// @brief update pre-computed geometry information
    /// @note: must be called when geometry changes (i.e. lane moved)
    virtual void updateGeometry() = 0;

    /// @brief Returns position of additional in view
    const Position &getPositionInView() const;

    /// @brief Returns GNEViewNet in which additional element is located
    GNEViewNet* getViewNet() const;

    /// @brief Returns pointer to Lane (or NULL if additional don't belont to a Lane)
    GNELane* getLane() const;

    /// @brief Returns additional element's shape
    PositionVector getShape() const;

    /// @brief Returns string with the information of additional element's shape
    std::string getShapeInformation() const;

    /** @brief Check if item is blocked (i.e. cannot be moved with mouse)
     * @return true if element is blocked, false in other case
     */
    bool isBlocked() const;

    /** @brief Check if this item belongs to an additionalSet
     * @return true if element belongs to an additionalSet, false in other case
     */
    bool belongToAdditionalSet() const;

    /** @brief get additionalSet parent
     * @return pointer to additionalSet, or NULL if don't belongs to an additionalSet
     */
    GNEAdditionalSet* getAdditionalSetParent() const;

    /// @brief Block or unblock additional element(i.e. cannot be moved with mouse)
    void setBlocked(bool value);

    /// @brief set new position in the view
    void setPositionInView(const Position &pos);

    /** @brief writte additional element into a xml file
     * @param[in] device device in which write parameters of additional element
     */
    virtual void writeAdditional(OutputDevice& device) = 0;

    /// @name inherited from GUIGlObject
    //@{
    /// @brief Returns the name of the parent object (if any)
    /// @return This object's parent id
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
     */
    Boundary getCenteringBoundary() const;

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
    /// @brief The GNEViewNet this additional element belongs
    GNEViewNet* myViewNet;

    /// @brief The GNEViewNet this additional element belongs
    GNELane* myLane;

    /// @brief The position in which this additional element is located
    /// @note if this element belongs to a Lane, x() value will be the position over Lane
    Position myPosition;

    /// @brief The shape of the additional element
    PositionVector myShape;

    /// @brief type of tag of the parent
    SumoXMLTag myParentTag;

    /// @brief pointer to item parent, if belong to set
    GNEAdditionalSet *myParent;

    /// @name computed only once (for performance) in updateGeometry()
    //@{
    /// The rotations of the shape parts
    std::vector<SUMOReal> myShapeRotations;

    /// The lengths of the shape parts
    std::vector<SUMOReal> myShapeLengths;
    //@}

    /// @name members and functions relative to block icon
    //@{
    /// @brief set Rotation of block Icon
    /// @note must be called in updateGeometry() after setting of Shape
    void setBlockIconRotation();

    /// @brief draw lock icon
    void drawLockIcon(SUMOReal size = 0.5) const;

    /// @brief The offSet of the block icon
    /// @note by default position of blockIcon is the center of shape
    Position myBlockIconOffset;

    /// @brief The rotation of the block icon
    SUMOReal myBlockIconRotation;

    /// @brief boolean to check if additional element is blocked (i.e. cannot be moved with mouse)
    bool myBlocked; 
    //@}

    
    /// @brief vector with the different colors
    std::vector<RGBColor> myRGBColors;

private:
    /// @brief variable to keep GLId of the additional lock image
    static GUIGlID additionalLockGlID;

    /// @brief variable to keep GLId of the additional empty image
    static GUIGlID additionalEmptyGlID;

    /// @brief boolean to check if additional lock image was inicializated
    static bool additionalLockInitialized;

    /// @brief boolean to check if additional empty image was inicializated
    static bool additionalEmptyInitialized;

    /// @brief set attribute after validation
    virtual void setAttribute(SumoXMLAttr key, const std::string& value) = 0;

    /// @brief set colors of scheme
    virtual void setColors() = 0;

    /// @brief Invalidated copy constructor.
    GNEAdditional(const GNEAdditional&);

    /// @brief Invalidated assignment operator.
    GNEAdditional& operator=(const GNEAdditional&);
};


#endif
