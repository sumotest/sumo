/****************************************************************************/
/// @file    GNELane.h
/// @author  Jakob Erdmann
/// @date    Feb 2011
/// @version $Id$
///
// A class for visualizing Lane geometry (adapted from GUILaneWrapper)
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2015 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef GNELane_h
#define GNELane_h


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
#include "GNEAttributeCarrier.h"


// ===========================================================================
// class declarations
// ===========================================================================
class GUIGLObjectPopupMenu;
class PositionVector;
class GNETLSEditor;
class GNEEdge;
class GNENet;
class GNEAdditional;            // PABLO #1916

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GNELane
 * @brief This lane is powered by an underlying GNEEdge and basically knows how
 * to draw itself
 */
class GNELane : public GUIGlObject, public GNEAttributeCarrier, public FXDelegator {
    // FOX-declarations
    FXDECLARE(GNELane)

public:
    /// Definition of the lane's stoppingPlaces vector
    typedef std::vector<GNEAdditional*> additionalVector;

    /** @brief Constructor
     * @param[in] idStorage The storage of gl-ids to get the one for this lane representation from
     * @param[in] the edge this lane belongs to
     * @param[in] the index of this lane
     */
    GNELane(GNEEdge& edge, const int index);

    /// @brief Destructor
    virtual ~GNELane();

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
    GUIGLObjectPopupMenu* getPopUpMenu(GUIMainWindow& app,
                                       GUISUMOAbstractView& parent);

    /** @brief Returns an own parameter window
     *
     * @param[in] app The application needed to build the parameter window
     * @param[in] parent The parent window needed to build the parameter window
     * @return The built parameter window
     * @see GUIGlObject::getParameterWindow
     */
    GUIParameterTableWindow* getParameterWindow(GUIMainWindow& app,
            GUISUMOAbstractView& parent);

    /// @brief multiplexes message to two targets
    long onDefault(FXObject*, FXSelector, void*);

    /** @brief Returns underlying parent edge
     * @return The underlying GNEEdge
     */
    GNEEdge& getParentEdge();

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
    void drawGL(const GUIVisualizationSettings& s) const;
    //@}

    /// @brief returns the shape of the lane
    const PositionVector& getShape() const;

    /// @brief returns the vector with the shape rotations
    const std::vector<SUMOReal>& getShapeRotations() const;

    /// @brief returns the vector with the shape lengths
    const std::vector<SUMOReal>& getShapeLengths() const;

    /// @brief returns the boundry (including lanes)
    Boundary getBoundary() const;

    /// @brief update pre-computed geometry information
    //  @note: must be called when geometry changes (i.e. junction moved)
    void updateGeometry();

    /// @brief returns the index of the lane    // PABLO #1916
    unsigned int getIndex() const;

    /* @brief method for setting the index of the lane      // PABLO #1916
     * @param[in] index The new index of lane               // PABLO #1916
     */                                                     // PABLO #1916
    void setIndex(unsigned int index);                      // PABLO #1916

    /// @brief returns the length of the lane                           // PABLO #1916
    /// @note length is calculated as the sumatorial of myShapeLengths  // PABLO #1916
    SUMOReal getLength() const;                                         // PABLO #1916

    /* @brief method for adding a reference of a additional element placed in this lane     // PABLO #1916
     * @param[in] additional Pointer to additional element                                  // PABLO #1916
     */                                                                                     // PABLO #1916
    void addAdditional(GNEAdditional *additional);                                          // PABLO #1916

    /* @brief method for remove a reference of a additional element placed in this lane     // PABLO #1916
     * @param[in] additional Pointer to additional element previously added                 // PABLO #1916
       @return true if additional element was sucesfully removed, flase in other case       // PABLO #1916
     */                                                                                     // PABLO #1916
    bool removeAdditional(GNEAdditional *additional);                                       // PABLO #1916

    /* @brief method to obtain a set of additional elements associated to this lane     // PABLO #1916
       @return set with all additional elements                                         // PABLO #1916
       @todo modify to get only a specific type of additional element                   // PABLO #1916
     */                                                                                 // PABLO #1916
    std::set<GUIGlID> getAdditionals();                                                 // PABLO #1916

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

    /* @brief method for setting the special color of the lane
     * @param[in] color Pointer to new special color
     */
    void setSpecialColor(const RGBColor* color);

protected:
    /// @brief FOX needs this
    GNELane();

private:
    /// @brief The Edge that to which this lane belongs
    GNEEdge& myParentEdge;

    /// @brief The index of this lane
    int myIndex;

    /// @name computed only once (for performance) in updateGeometry()
    //@{
    /// @brief The rotations of the shape parts
    std::vector<SUMOReal> myShapeRotations;

    /// @brief The lengths of the shape parts
    std::vector<SUMOReal> myShapeLengths;
    //@}

    /// @brief The lengths of the lane (Calculated from myShapeLengths) // PABLO #1916 (QUESTION ERDMANN 01)
    SUMOReal myLength;                                                  // PABLO #1916 (QUESTION ERDMANN 01)

    /// @brief vector with pointers to additional elements placed in this lane  // PABLO #1916
    additionalVector additionalElements;                                        // PABLO #1916

    /// @brief optional special color
    const RGBColor* mySpecialColor;

        /// @brief The color of the shape parts (cached)
    mutable std::vector<RGBColor> myShapeColors;

    /// @brief the tls-editor for setting multiple links in TLS-mode
    GNETLSEditor* myTLSEditor;

private:
    /// @brief set attribute after validation
    void setAttribute(SumoXMLAttr key, const std::string& value);

    /// @brief Invalidated copy constructor.
    GNELane(const GNELane&);

    /// @brief Invalidated assignment operator.
    GNELane& operator=(const GNELane&);

    /// @brief draw lane markings
    void drawMarkings(const bool& selectedEdge, SUMOReal scale) const;

    /// @brief draw link Number
    void drawLinkNo() const;

    /// @brief draw TLS Link Number
    void drawTLSLinkNo() const;

    /// @brief draw link rules
    void drawLinkRules() const;

    /// @brief draw arrows
    void drawArrows() const;

    /// @brief draw lane to lane connections
    void drawLane2LaneConnections() const;

    /// @brief return value for lane coloring according to the given scheme
    SUMOReal getColorValue(size_t activeScheme) const;

    /// @brief sets the color according to the current scheme index and some lane function
    bool setFunctionalColor(size_t activeScheme) const;

    /// @brief sets multiple colors according to the current scheme index and some lane function
    bool setMultiColor(const GUIColorer& c) const;

    /// @brief whether to draw this lane as a railway
    bool drawAsRailway(const GUIVisualizationSettings& s) const;

    /// @brief whether to draw this lane as a waterways
    bool drawAsWaterway(const GUIVisualizationSettings& s) const;

    /* @brief draw crossties for railroads
     * @todo: XXX This duplicates the code of GUILane::drawCrossties and needs to be */
    void drawCrossties(SUMOReal length, SUMOReal spacing, SUMOReal halfWidth) const;
};


#endif

/****************************************************************************/

