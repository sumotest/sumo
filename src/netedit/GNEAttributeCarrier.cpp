/****************************************************************************/
/// @file    GNEAttributeCarrier.cpp
/// @author  Jakob Erdmann
/// @date    Feb 2011
/// @version $Id$
///
// Abstract Base class for gui objects which carry attributes
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


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <algorithm>
#include <utils/common/MsgHandler.h>
#include <utils/xml/SUMOSAXAttributes.h>
#include "GNEAttributeCarrier.h"
#include "GNEUndoList.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS

// ===========================================================================
// static members
// ===========================================================================
std::map<SumoXMLTag, std::vector<std::pair <SumoXMLAttr, std::string> > > GNEAttributeCarrier::_allowedAttributes;
std::vector<SumoXMLTag> GNEAttributeCarrier::myAllowedTags;
std::vector<SumoXMLTag> GNEAttributeCarrier::myAllowedAdditionalTags;                       // PABLO #1916
std::set<SumoXMLAttr> GNEAttributeCarrier::myNumericalIntAttrs;                             // PABLO #1916
std::set<SumoXMLAttr> GNEAttributeCarrier::myNumericalFloatAttrs;                           // PABLO #1916
std::set<SumoXMLAttr> GNEAttributeCarrier::myListAttrs;                                     // PABLO #1916
std::set<SumoXMLAttr> GNEAttributeCarrier::myUniqueAttrs;
std::map<SumoXMLTag, SumoXMLTag> GNEAttributeCarrier::myAllowedAdditionalWithParentTags;    // PABLO #1916
std::map<SumoXMLTag, std::map<SumoXMLAttr, std::vector<std::string> > > GNEAttributeCarrier::myDiscreteChoices;

const std::string GNEAttributeCarrier::LOADED = "loaded";
const std::string GNEAttributeCarrier::GUESSED = "guessed";
const std::string GNEAttributeCarrier::MODIFIED = "modified";
const std::string GNEAttributeCarrier::APPROVED = "approved";

// ===========================================================================
// method definitions
// ===========================================================================
GNEAttributeCarrier::GNEAttributeCarrier(SumoXMLTag tag) :
    myTag(tag) {
}


template<> int
GNEAttributeCarrier::parse(const std::string& string) {
    return TplConvert::_str2int(string);
}


template<> SUMOReal
GNEAttributeCarrier::parse(const std::string& string) {
    return TplConvert::_str2SUMOReal(string);
}


template<> bool
GNEAttributeCarrier::parse(const std::string& string) {
    return TplConvert::_str2bool(string);
}


bool
GNEAttributeCarrier::isValid(SumoXMLAttr key, const std::string& value) {
    UNUSED_PARAMETER(value);
    return std::find(getAttrs().begin(), getAttrs().end(), key) != getAttrs().end();
}


std::string
GNEAttributeCarrier::getDescription() {
    return toString(myTag);
}


SumoXMLTag
GNEAttributeCarrier::getTag() const {
    return myTag;
}


std::vector<SumoXMLAttr>
GNEAttributeCarrier::getAttrs() const {
    std::vector<SumoXMLAttr> attr;
    for(std::vector<std::pair <SumoXMLAttr, std::string> >::const_iterator i = allowedAttributes(myTag).begin(); i != allowedAttributes(myTag).end(); i++)
        attr.push_back(i->first);
    return attr;
}


const std::string
GNEAttributeCarrier::getID() const {
    return getAttribute(SUMO_ATTR_ID);
}


SumoXMLTag                                              // PABLO #1916
GNEAttributeCarrier::getParentType(SumoXMLTag tag) {    // PABLO #1916
    if(hasParent(tag))                                  // PABLO #1916
        return myAllowedAdditionalWithParentTags[tag];  // PABLO #1916
    else                                                // PABLO #1916
        return SUMO_TAG_NOTHING;                        // PABLO #1916
}                                                       // PABLO #1916


bool
GNEAttributeCarrier::isValidID(const std::string& value) {
    return value.find_first_of(" \t\n\r@$%^&/|\\{}*'\";:<>") == std::string::npos;
}


bool                                                                                // PABLO #1916
GNEAttributeCarrier::isValidFileValue(const std::string& value) {                   // PABLO #1916
    // @note Only characteres that aren't permited in a file path or belong         // PABLO #1916
    // to XML sintax                                                                // PABLO #1916
    return value.find_first_of("\t\n\r@$%^&|\\{}*'\";:<>") == std::string::npos;    // PABLO #1916
}                                                                                   // PABLO #1916


bool                                                                        // PABLO #1916
GNEAttributeCarrier::isValidStringVector(const std::string& value) {        // PABLO #1916
    // 1) check if value is empty                                           // PABLO #1916
    if(value.empty())                                                       // PABLO #1916
        return true;                                                        // PABLO #1916
    // 2) Check if there are duplicated spaces                              // PABLO #1916
    for(int i = 1; i < value.size(); i++)                                   // PABLO #1916
        if(value.at(i-1) == ' ' && value.at(i) == ' ')                      // PABLO #1916
            return false;                                                   // PABLO #1916
    // 3) Check if the first and last character aren't spaces               // PABLO #1916
    if((value.at(0) == ' ') || (value.at(value.size()-1) == ' '))           // PABLO #1916
        return false;                                                       // PABLO #1916
    // 4) Check if every sub-string is valid                                // PABLO #1916
    int index = 0;                                                          // PABLO #1916
    std::string subString;                                                  // PABLO #1916
    while(index < value.size()) {                                           // PABLO #1916
        if(value.at(index) == ' ') {                                        // PABLO #1916
            if(!isValidFileValue(subString))                                // PABLO #1916
                return false;                                               // PABLO #1916
            else                                                            // PABLO #1916
                subString.clear();                                          // PABLO #1916
        }                                                                   // PABLO #1916
        else                                                                // PABLO #1916
            subString.push_back(value.at(index));                           // PABLO #1916
        index++;                                                            // PABLO #1916
    }                                                                       // PABLO #1916
    // 5) All right, then return true                                       // PABLO #1916
    return true;                                                            // PABLO #1916
}                                                                           // PABLO #1916

// ===========================================================================
// static methods
// ===========================================================================

const std::vector<std::pair <SumoXMLAttr, std::string> >&
GNEAttributeCarrier::allowedAttributes(SumoXMLTag tag) {
    // define on first access
    if (!_allowedAttributes.count(tag)) {
        std::vector<std::pair<SumoXMLAttr, std::string> >& attrs = _allowedAttributes[tag];
        switch (tag) {
            case SUMO_TAG_EDGE:
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_ID, ""));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_FROM, ""));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_TO, ""));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_SPEED, ""));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_PRIORITY, ""));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_NUMLANES, ""));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_TYPE, ""));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_ALLOW, ""));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_DISALLOW, ""));
                //attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_PREFER, ));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_SHAPE, ""));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_LENGTH, ""));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_SPREADTYPE, ""));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_NAME, ""));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_WIDTH, ""));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_ENDOFFSET, ""));
                break;
            case SUMO_TAG_JUNCTION:
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_ID, ""));
                /* virtual attribute from the combination of the actuall
                 * attributes SUMO_ATTR_X, SUMO_ATTR_Y */
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_POSITION, ""));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_TYPE, ""));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_SHAPE, ""));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_RADIUS, ""));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_KEEP_CLEAR, ""));
                break;
            case SUMO_TAG_LANE:
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_ID, ""));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_SPEED, ""));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_ALLOW, ""));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_DISALLOW, ""));
                //attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_PREFER, ));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_WIDTH, ""));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_ENDOFFSET, ""));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_INDEX, "")); // read-only attribute
                break;
            case SUMO_TAG_POI:
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_ID, ""));
                /* virtual attribute from the combination of the actuall
                 * attributes SUMO_ATTR_X, SUMO_ATTR_Y */
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_POSITION, ""));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_TYPE, ""));
                break;
            case SUMO_TAG_CROSSING:
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_ID, ""));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_PRIORITY, ""));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_WIDTH, ""));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_EDGES, ""));
                break;
            case SUMO_TAG_BUS_STOP:                                                                                 // PABLO #1916
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_ID, ""));                             // PABLO #1916
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_LANE, ""));                           // PABLO #1916
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_STARTPOS, ""));                       // PABLO #1916
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_ENDPOS, "10"));                       // PABLO #1916
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_LINES, ""));                          // PABLO #1916
                break;                                                                                              // PABLO #1916
            case SUMO_TAG_CHARGING_STATION:                                                                         // PABLO #1916
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_ID, ""));                             // PABLO #1916
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_LANE, ""));                           // PABLO #1916
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_STARTPOS, ""));                       // PABLO #1916
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_ENDPOS, "10"));                       // PABLO #1916
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_CHARGINGPOWER, "22000"));             // PABLO #1916
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_EFFICIENCY, "0.95"));                 // PABLO #1916
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_CHARGEINTRANSIT, "false"));           // PABLO #1916
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_CHARGEDELAY, "0"));                   // PABLO #1916
                break;                                                                                              // PABLO #1916
            case SUMO_TAG_E1DETECTOR:                                                                               // PABLO #1916
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_ID, ""));                             // PABLO #1916
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_LANE, ""));                           // PABLO #1916
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_POSITION, ""));                       // PABLO #1916
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_FREQUENCY, "100"));                   // PABLO #1916
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_FILE, ""));                           // PABLO #1916
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_SPLIT_VTYPE, "false"));               // PABLO #1916
                break;                                                                                              // PABLO #1916
            case SUMO_TAG_E2DETECTOR:                                                                               // PABLO #1916
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_ID, ""));                             // PABLO #1916
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_LANE, ""));                           // PABLO #1916
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_POSITION, ""));                       // PABLO #1916
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_LENGTH, "10"));                       // PABLO #1916
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_FREQUENCY, "100"));                   // PABLO #1916
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_FILE, ""));                           // PABLO #1916
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_CONT, "false"));                      // PABLO #1916
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_HALTING_TIME_THRESHOLD, "1"));        // PABLO #1916
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_HALTING_SPEED_THRESHOLD, "1.39"));    // PABLO #1916
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_JAM_DIST_THRESHOLD, "10"));           // PABLO #1916
                break;                                                                                              // PABLO #1916
            case SUMO_TAG_E3DETECTOR:                                                                               // PABLO #1916
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_ID, ""));                             // PABLO #1916
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_FREQUENCY, "100"));                   // PABLO #1916
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_FILE, ""));                           // PABLO #1916
                break;                                                                                              // PABLO #1916
            case SUMO_TAG_DET_ENTRY:                                                                                // PABLO #1916
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_LANE, ""));                           // PABLO #1916
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_POSITION, ""));                       // PABLO #1916
                break;                                                                                              // PABLO #1916
            case SUMO_TAG_DET_EXIT:                                                                                 // PABLO #1916
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_LANE, ""));                           // PABLO #1916
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_POSITION, ""));                       // PABLO #1916
                break;                                                                                              // PABLO #1916
            case SUMO_TAG_VSS:                                                                                      // PABLO #1916
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_ID, ""));                             // PABLO #1916
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_LANES, ""));                          // PABLO #1916
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_FILE, ""));                           // PABLO #1916
                break;                                                                                              // PABLO #1916
            case SUMO_TAG_CALIBRATOR:                                                                               // PABLO #1916
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_ID, ""));                             // PABLO #1916
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_LANE, ""));                           // PABLO #1916
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_POSITION, "0"));                      // PABLO #1916
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_FREQUENCY, "100"));                   // PABLO #1916
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_ROUTEPROBE, ""));                     // PABLO #1916
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_OUTPUT, ""));                         // PABLO #1916
                break;                                                                                              // PABLO #1916
            case SUMO_TAG_REROUTER:                                                                                 // PABLO #1916
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_ID, ""));                             // PABLO #1916
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_EDGES, ""));                          // PABLO #1916
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_FILE, ""));                           // PABLO #1916
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_PROB, "1"));                          // PABLO #1916
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_OFF, "false"));                       // PABLO #1916
                break;                                                                                              // PABLO #1916
            default:
                WRITE_WARNING("allowed attributes for tag '" + toString(tag) + "' not defined");
        }
    }
    return _allowedAttributes[tag];
}


const std::vector<SumoXMLTag>&
GNEAttributeCarrier::allowedTags() {
    // define on first access
    if (myAllowedTags.empty()) {
        myAllowedTags.push_back(SUMO_TAG_JUNCTION);
        myAllowedTags.push_back(SUMO_TAG_EDGE);
        myAllowedTags.push_back(SUMO_TAG_LANE);
        myAllowedTags.push_back(SUMO_TAG_BUS_STOP);              // PABLO #1916
        myAllowedTags.push_back(SUMO_TAG_CHARGING_STATION);      // PABLO #1916
        myAllowedTags.push_back(SUMO_TAG_E1DETECTOR);            // PABLO #1916
        myAllowedTags.push_back(SUMO_TAG_E2DETECTOR);            // PABLO #1916
        myAllowedTags.push_back(SUMO_TAG_E3DETECTOR);            // PABLO #1916
        myAllowedTags.push_back(SUMO_TAG_DET_ENTRY);             // PABLO #1916
        myAllowedTags.push_back(SUMO_TAG_DET_EXIT);              // PABLO #1916
        myAllowedTags.push_back(SUMO_TAG_VSS);                   // PABLO #1916
        myAllowedTags.push_back(SUMO_TAG_CALIBRATOR);            // PABLO #1916
        myAllowedTags.push_back(SUMO_TAG_REROUTER);              // PABLO #1916
    }
    return myAllowedTags;
}


const std::vector<SumoXMLTag>&                                          // PABLO #1916
GNEAttributeCarrier::allowedAdditionalTags() {                          // PABLO #1916
    // define on first access                                           // PABLO #1916
    if (myAllowedAdditionalTags.empty()) {                              // PABLO #1916
        myAllowedAdditionalTags.push_back(SUMO_TAG_BUS_STOP);           // PABLO #1916
        myAllowedAdditionalTags.push_back(SUMO_TAG_CHARGING_STATION);   // PABLO #1916
        myAllowedAdditionalTags.push_back(SUMO_TAG_E1DETECTOR);         // PABLO #1916
        myAllowedAdditionalTags.push_back(SUMO_TAG_E2DETECTOR);         // PABLO #1916
        myAllowedAdditionalTags.push_back(SUMO_TAG_E3DETECTOR);         // PABLO #1916
        myAllowedAdditionalTags.push_back(SUMO_TAG_DET_ENTRY);          // PABLO #1916
        myAllowedAdditionalTags.push_back(SUMO_TAG_DET_EXIT);           // PABLO #1916
        myAllowedAdditionalTags.push_back(SUMO_TAG_VSS);                // PABLO #1916
        myAllowedAdditionalTags.push_back(SUMO_TAG_CALIBRATOR);         // PABLO #1916
        myAllowedAdditionalTags.push_back(SUMO_TAG_REROUTER);           // PABLO #1916
    }                                                                   // PABLO #1916
    return myAllowedAdditionalTags;                                     // PABLO #1916
}                                                                       // PABLO #1916


bool
GNEAttributeCarrier::isNumerical(SumoXMLAttr attr) {
    return (isInt(attr) || isFloat(attr));
}


bool                                                                    // PABLO #1916
GNEAttributeCarrier::isInt(SumoXMLAttr attr) {                          // PABLO #1916
    // define on first access                                           // PABLO #1916
    if (myNumericalIntAttrs.empty()) {                                  // PABLO #1916
        myNumericalIntAttrs.insert(SUMO_ATTR_NUMLANES);                 // PABLO #1916
        myNumericalIntAttrs.insert(SUMO_ATTR_PRIORITY);                 // PABLO #1916
        myNumericalIntAttrs.insert(SUMO_ATTR_INDEX);                    // PABLO #1916
        myNumericalIntAttrs.insert(SUMO_ATTR_CHARGEDELAY);              // PABLO #1916
        myNumericalIntAttrs.insert(SUMO_ATTR_FREQUENCY);                // PABLO #1916
        myNumericalIntAttrs.insert(SUMO_ATTR_HALTING_TIME_THRESHOLD);   // PABLO #1916
    }                                                                   // PABLO #1916
    return myNumericalIntAttrs.count(attr) == 1;                        // PABLO #1916
}                                                                       // PABLO #1916


bool                                                                    // PABLO #1916
GNEAttributeCarrier::isFloat(SumoXMLAttr attr) {                        // PABLO #1916
    // define on first access                                           // PABLO #1916
    if (myNumericalFloatAttrs.empty()) {                                // PABLO #1916
    myNumericalFloatAttrs.insert(SUMO_ATTR_SPEED);                      // PABLO #1916
    myNumericalFloatAttrs.insert(SUMO_ATTR_LENGTH);                     // PABLO #1916
    myNumericalFloatAttrs.insert(SUMO_ATTR_WIDTH);                      // PABLO #1916
    myNumericalFloatAttrs.insert(SUMO_ATTR_ENDOFFSET);                  // PABLO #1916
    myNumericalFloatAttrs.insert(SUMO_ATTR_RADIUS);                     // PABLO #1916
    myNumericalFloatAttrs.insert(SUMO_ATTR_STARTPOS);                   // PABLO #1916
    myNumericalFloatAttrs.insert(SUMO_ATTR_ENDPOS);                     // PABLO #1916
    myNumericalFloatAttrs.insert(SUMO_ATTR_CHARGINGPOWER);              // PABLO #1916
    myNumericalFloatAttrs.insert(SUMO_ATTR_EFFICIENCY);                 // PABLO #1916
    myNumericalFloatAttrs.insert(SUMO_ATTR_POSITION);                   // PABLO #1916
    myNumericalFloatAttrs.insert(SUMO_ATTR_LENGTH);                     // PABLO #1916
    myNumericalFloatAttrs.insert(SUMO_ATTR_HALTING_SPEED_THRESHOLD);    // PABLO #1916
    myNumericalFloatAttrs.insert(SUMO_ATTR_JAM_DIST_THRESHOLD);         // PABLO #1916
    myNumericalFloatAttrs.insert(SUMO_ATTR_PROB);                       // PABLO #1916
    }                                                                   // PABLO #1916
    return myNumericalFloatAttrs.count(attr) == 1;                      // PABLO #1916
}                                                                       // PABLO #1916


bool                                                                                                                                                        // PABLO #1916
GNEAttributeCarrier::isBool(SumoXMLAttr attr) {                                                                                                             // PABLO #1916
    // Iterate over map                                                                                                                                     // PABLO #1916
    for(std::map<SumoXMLTag, std::map<SumoXMLAttr, std::vector<std::string> > >::iterator i = myDiscreteChoices.begin(); i != myDiscreteChoices.end(); i++)   // PABLO #1916
        if (i->second.find(attr) != i->second.end()) {                                                                                                      // PABLO #1916
            // Obtain values of discrete attribute attr;                                                                                                    // PABLO #1916
            std::vector<std::string> values = i->second.find(attr)->second;                                                                                 // PABLO #1916
            // Check if values are exactly "true" and "false"                                                                                               // PABLO #1916
            return (values.size() == 2) && (values.at(0) == "true") && (values.at(1) == "false")?  true : false;                                            // PABLO #1916
        }                                                                                                                                                   // PABLO #1916
    return false;                                                                                                                                           // PABLO #1916
}                                                                                                                                                           // PABLO #1916

bool                                                // PABLO #1916
GNEAttributeCarrier::isString(SumoXMLAttr attr) {   // PABLO #1916
    return !isNumerical(attr) || !isBool(attr);     // PABLO #1916
}                                                   // PABLO #1916


bool                                                        // PABLO #1916
GNEAttributeCarrier::isList(SumoXMLAttr attr) {             // PABLO #1916
    // define on first access                               // PABLO #1916
    if (myListAttrs.empty()) {                              // PABLO #1916
        myListAttrs.insert(SUMO_ATTR_LINES);                // PABLO #1916
        myListAttrs.insert(SUMO_ATTR_EDGES);                // PABLO #1916
    }                                                       // PABLO #1916
    return myListAttrs.count(attr) == 1;                    // PABLO #1916
}                                                           // PABLO #1916


bool
GNEAttributeCarrier::isUnique(SumoXMLAttr attr) {
    // define on first access
    if (myUniqueAttrs.empty()) {
        myUniqueAttrs.insert(SUMO_ATTR_ID);
        myUniqueAttrs.insert(SUMO_ATTR_FROM);
        myUniqueAttrs.insert(SUMO_ATTR_TO);
        myUniqueAttrs.insert(SUMO_ATTR_SHAPE);
        myUniqueAttrs.insert(SUMO_ATTR_POSITION);
        myUniqueAttrs.insert(SUMO_ATTR_EDGES);
        myUniqueAttrs.insert(SUMO_ATTR_STARTPOS);   // PABLO #1916
        myUniqueAttrs.insert(SUMO_ATTR_ENDPOS);     // PABLO #1916
        myUniqueAttrs.insert(SUMO_ATTR_LANE);       // PABLO #1916
    }
    return myUniqueAttrs.count(attr) == 1;
}


bool                                                                                    // PABLO #1916
GNEAttributeCarrier::hasParent(SumoXMLTag tag) {                                        // PABLO #1916
    // define on first access                                                           // PABLO #1916
    if (myAllowedAdditionalWithParentTags.empty()) {                                    // PABLO #1916
        myAllowedAdditionalWithParentTags[SUMO_TAG_DET_ENTRY] = SUMO_TAG_E3DETECTOR;    // PABLO #1916
        myAllowedAdditionalWithParentTags[SUMO_TAG_DET_EXIT] = SUMO_TAG_E3DETECTOR;     // PABLO #1916
    }                                                                                   // PABLO #1916
    return myAllowedAdditionalWithParentTags.count(tag) == 1;                           // PABLO #1916
}                                                                                       // PABLO #1916


bool                                                                                                                    // PABLO #1916
GNEAttributeCarrier::hasAttribute(SumoXMLTag tag, SumoXMLAttr attr) {                                                   // PABLO #1916
    const std::vector<std::pair <SumoXMLAttr, std::string> >& attrs = allowedAttributes(tag);                           // PABLO #1916
    for(std::vector<std::pair <SumoXMLAttr, std::string> >::const_iterator i = attrs.begin(); i != attrs.end(); i++)    // PABLO #1916
        if(i->first == attr)                                                                                            // PABLO #1916
            return true;                                                                                                // PABLO #1916
    return false;                                                                                                       // PABLO #1916
}                                                                                                                       // PABLO #1916


const std::vector<std::string>&
GNEAttributeCarrier::discreteChoices(SumoXMLTag tag, SumoXMLAttr attr) {
    // define on first access
    if (myDiscreteChoices.empty()) {
        std::vector<std::string> choices;
        choices = SUMOXMLDefinitions::NodeTypes.getStrings();
        for (std::vector<std::string>::const_iterator it = choices.begin(); it != choices.end(); ++it) {
            if (*it != toString(NODETYPE_DEAD_END_DEPRECATED)) {
                myDiscreteChoices[SUMO_TAG_JUNCTION][SUMO_ATTR_TYPE].push_back(*it);
            }
        }

        myDiscreteChoices[SUMO_TAG_JUNCTION][SUMO_ATTR_KEEP_CLEAR].push_back("true");
        myDiscreteChoices[SUMO_TAG_JUNCTION][SUMO_ATTR_KEEP_CLEAR].push_back("false");

        choices = SUMOXMLDefinitions::LaneSpreadFunctions.getStrings();
        for (std::vector<std::string>::const_iterator it = choices.begin(); it != choices.end(); ++it) {
            myDiscreteChoices[SUMO_TAG_EDGE][SUMO_ATTR_SPREADTYPE].push_back(*it);
        }

        choices = SumoVehicleClassStrings.getStrings();
        for (std::vector<std::string>::const_iterator it = choices.begin(); it != choices.end(); ++it) {
            myDiscreteChoices[SUMO_TAG_EDGE][SUMO_ATTR_ALLOW].push_back(*it);
            myDiscreteChoices[SUMO_TAG_EDGE][SUMO_ATTR_DISALLOW].push_back(*it);
            myDiscreteChoices[SUMO_TAG_LANE][SUMO_ATTR_ALLOW].push_back(*it);
            myDiscreteChoices[SUMO_TAG_LANE][SUMO_ATTR_DISALLOW].push_back(*it);
        }

        myDiscreteChoices[SUMO_TAG_CROSSING][SUMO_ATTR_PRIORITY].push_back("true");
        myDiscreteChoices[SUMO_TAG_CROSSING][SUMO_ATTR_PRIORITY].push_back("false");

        myDiscreteChoices[SUMO_TAG_CHARGING_STATION][SUMO_ATTR_CHARGEINTRANSIT].push_back("true");     // PABLO #1916
        myDiscreteChoices[SUMO_TAG_CHARGING_STATION][SUMO_ATTR_CHARGEINTRANSIT].push_back("false");    // PABLO #1916

        myDiscreteChoices[SUMO_TAG_E1DETECTOR][SUMO_ATTR_SPLIT_VTYPE].push_back("true");     // PABLO #1916
        myDiscreteChoices[SUMO_TAG_E1DETECTOR][SUMO_ATTR_SPLIT_VTYPE].push_back("false");    // PABLO #1916

        myDiscreteChoices[SUMO_TAG_E2DETECTOR][SUMO_ATTR_CONT].push_back("true");     // PABLO #1916
        myDiscreteChoices[SUMO_TAG_E2DETECTOR][SUMO_ATTR_CONT].push_back("false");    // PABLO #1916

        myDiscreteChoices[SUMO_TAG_REROUTER][SUMO_ATTR_OFF].push_back("true");     // PABLO #1916
        myDiscreteChoices[SUMO_TAG_REROUTER][SUMO_ATTR_OFF].push_back("false");    // PABLO #1916
    }

    return myDiscreteChoices[tag][attr];
}


bool
GNEAttributeCarrier::discreteCombinableChoices(SumoXMLTag, SumoXMLAttr attr) {
    return (attr == SUMO_ATTR_ALLOW || attr == SUMO_ATTR_DISALLOW);
}


template<> int                                                                                                                                          // PABLO #1916
GNEAttributeCarrier::getDefaultValue(SumoXMLTag tag, SumoXMLAttr attr) {                                                                                // PABLO #1916
    for(std::vector<std::pair<SumoXMLAttr, std::string> >::iterator i = _allowedAttributes.at(tag).begin(); i != _allowedAttributes.at(tag).end(); i++) // PABLO #1916
        if((*i).first == attr)                                                                                                                          // PABLO #1916
            return TplConvert::_str2int((*i).second);                                                                                                   // PABLO #1916
}                                                                                                                                                       // PABLO #1916


template<> SUMOReal                                                                                                                                     // PABLO #1916
GNEAttributeCarrier::getDefaultValue(SumoXMLTag tag, SumoXMLAttr attr) {                                                                                // PABLO #1916
    for(std::vector<std::pair<SumoXMLAttr, std::string> >::iterator i = _allowedAttributes.at(tag).begin(); i != _allowedAttributes.at(tag).end(); i++) // PABLO #1916
        if((*i).first == attr)                                                                                                                          // PABLO #1916
            return TplConvert::_str2SUMOReal((*i).second);                                                                                              // PABLO #1916
}                                                                                                                                                       // PABLO #1916


template<> bool                                                                                                                                         // PABLO #1916
GNEAttributeCarrier::getDefaultValue(SumoXMLTag tag, SumoXMLAttr attr) {                                                                                // PABLO #1916
    for(std::vector<std::pair<SumoXMLAttr, std::string> >::iterator i = _allowedAttributes.at(tag).begin(); i != _allowedAttributes.at(tag).end(); i++) // PABLO #1916
        if((*i).first == attr)                                                                                                                          // PABLO #1916
            return TplConvert::_str2bool((*i).second);                                                                                                  // PABLO #1916
}                                                                                                                                                       // PABLO #1916


template<> std::string                                                                                                                                  // PABLO #1916
GNEAttributeCarrier::getDefaultValue(SumoXMLTag tag, SumoXMLAttr attr) {                                                                                // PABLO #1916
    for(std::vector<std::pair<SumoXMLAttr, std::string> >::iterator i = _allowedAttributes.at(tag).begin(); i != _allowedAttributes.at(tag).end(); i++) // PABLO #1916
        if((*i).first == attr)                                                                                                                          // PABLO #1916
            return (*i).second;                                                                                                                         // PABLO #1916
}                                                                                                                                                       // PABLO #1916

template<> std::vector<int>                                                                                                                             // PABLO #1916
GNEAttributeCarrier::getDefaultValue(SumoXMLTag tag, SumoXMLAttr attr) {                                                                                // PABLO #1916
    std::cout << "FINISH" << std::endl;
    return std::vector<int>();                                                                                                 // PABLO #1916
}

template<> std::vector<SUMOReal>                                                                                                                        // PABLO #1916
GNEAttributeCarrier::getDefaultValue(SumoXMLTag tag, SumoXMLAttr attr) {                                                                                // PABLO #1916
    std::cout << "FINISH" << std::endl;
    return std::vector<SUMOReal>();                                                                                           // PABLO #1916
}                                                                                                                                                       // PABLO #1916


template<> std::vector<bool>                                                                                                                            // PABLO #1916
GNEAttributeCarrier::getDefaultValue(SumoXMLTag tag, SumoXMLAttr attr) {                                                                                // PABLO #1916
    std::cout << "FINISH" << std::endl;
    return std::vector<bool>();
}                                                                                                                                                       // PABLO #1916


template<> std::vector<std::string>                                                                                                                     // PABLO #1916
GNEAttributeCarrier::getDefaultValue(SumoXMLTag tag, SumoXMLAttr attr) {                                                                                // PABLO #1916
    for(std::vector<std::pair<SumoXMLAttr, std::string> >::iterator i = _allowedAttributes.at(tag).begin(); i != _allowedAttributes.at(tag).end(); i++) // PABLO #1916
        if((*i).first == attr) {                                                                                                                        // PABLO #1916
            std::vector<std::string> myVectorString;                                                                                                    // PABLO #1916
            SUMOSAXAttributes::parseStringVector((*i).second, myVectorString);                                                                          // PABLO #1916
            return myVectorString;                                                                                                                      // PABLO #1916
        }                                                                                                                                               // PABLO #1916
}                                                                                                                                                       // PABLO #1916

/****************************************************************************/

