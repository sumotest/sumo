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
std::vector<SumoXMLTag> GNEAttributeCarrier::_allowedTags;
std::set<SumoXMLAttr> GNEAttributeCarrier::_numericalAttrs;
std::set<SumoXMLAttr> GNEAttributeCarrier::_uniqueAttrs;
std::map<SumoXMLTag, std::map<SumoXMLAttr, std::vector<std::string> > > GNEAttributeCarrier::_discreteChoices;

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


bool
GNEAttributeCarrier::isValidID(const std::string& value) {
    return value.find_first_of(" \t\n\r@$%^&/|\\{}*'\";:<>") == std::string::npos;
}

bool                                                                                // PABLO #1916
GNEAttributeCarrier::isValidFileValue(const std::string& value) {                   // PABLO #1916
    // @note Only characteres that aren't permited in a file path or belong         // PABLO #1916
    // to XML sintax                                                                // PABLO #1916
    return value.find_first_of("\t\n\r@$%^&|\{}*'\";:<>") == std::string::npos;     // PABLO #1916
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
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_ID, "#"));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_FROM, "#"));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_TO, "#"));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_SPEED, "#"));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_PRIORITY, "#"));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_NUMLANES, "#"));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_TYPE, "#"));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_ALLOW, "#"));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_DISALLOW, "#"));
                //attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_PREFER, ));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_SHAPE, "#"));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_LENGTH, "#"));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_SPREADTYPE, "#"));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_NAME, "#"));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_WIDTH, "#"));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_ENDOFFSET, "#"));
                break;
            case SUMO_TAG_JUNCTION:
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_ID, "#"));
                /* virtual attribute from the combination of the actuall
                 * attributes SUMO_ATTR_X, SUMO_ATTR_Y */
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_POSITION, "#"));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_TYPE, "#"));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_SHAPE, "#"));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_RADIUS, "#"));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_KEEP_CLEAR, "#"));
                break;
            case SUMO_TAG_LANE:
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_ID, "#"));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_SPEED, "#"));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_ALLOW, "#"));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_DISALLOW, "#"));
                //attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_PREFER, ));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_WIDTH, "#"));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_ENDOFFSET, "#"));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_INDEX, "#")); // read-only attribute
                break;
            case SUMO_TAG_POI:
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_ID, "#"));
                /* virtual attribute from the combination of the actuall
                 * attributes SUMO_ATTR_X, SUMO_ATTR_Y */
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_POSITION, "#"));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_TYPE, "#"));
                break;
            case SUMO_TAG_CROSSING:
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_ID, "#"));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_PRIORITY, "#"));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_WIDTH, "#"));
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_EDGES, "#"));
                break;
            case SUMO_TAG_BUS_STOP:                                                                                 // PABLO #1916
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_ID, "#"));                            // PABLO #1916
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_LANE, "#"));                          // PABLO #1916
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_STARTPOS, "#"));                      // PABLO #1916
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_ENDPOS, "f10"));                      // PABLO #1916
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_LINES, "l"));                         // PABLO #1916
                break;                                                                                              // PABLO #1916
            case SUMO_TAG_CHARGING_STATION:                                                                         // PABLO #1916
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_ID, "#"));                            // PABLO #1916
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_LANE, "#"));                          // PABLO #1916
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_STARTPOS, "#"));                      // PABLO #1916
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_ENDPOS, "f10"));                      // PABLO #1916
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_CHARGINGPOWER, "i22000"));            // PABLO #1916
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_EFFICIENCY, "f0.95"));                // PABLO #1916
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_CHARGEINTRANSIT, "bfalse"));          // PABLO #1916
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_CHARGEDELAY, "i0"));                  // PABLO #1916
                break;                                                                                              // PABLO #1916
            case SUMO_TAG_E1DETECTOR:                                                                               // PABLO #1916
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_ID, "#"));                            // PABLO #1916
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_LANE, "#"));                          // PABLO #1916
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_POSITION, "#"));                      // PABLO #1916
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_FREQUENCY, "i100"));                  // PABLO #1916
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_FILE, "s"));                          // PABLO #1916
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_SPLIT_VTYPE, "bfalse"));              // PABLO #1916
                break;                                                                                              // PABLO #1916
            case SUMO_TAG_E2DETECTOR:                                                                               // PABLO #1916
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_ID, "#"));                            // PABLO #1916
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_LANE, "#"));                          // PABLO #1916
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_POSITION, "#"));                     // PABLO #1916
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_LENGTH, "f10"));                      // PABLO #1916
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_FREQUENCY, "i100"));                  // PABLO #1916
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_FILE, "s"));                          // PABLO #1916
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_CONT, "bfalse"));                     // PABLO #1916
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_HALTING_TIME_THRESHOLD, "i1"));       // PABLO #1916
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_HALTING_SPEED_THRESHOLD, "f1.39"));   // PABLO #1916
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_JAM_DIST_THRESHOLD, "f10"));          // PABLO #1916
                break;                                                                                              // PABLO #1916
            case SUMO_TAG_E3DETECTOR:                                                                               // PABLO #1916
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_ID, "#"));                            // PABLO #1916
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_FREQUENCY, "i100"));                  // PABLO #1916
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_FILE, "s"));                          // PABLO #1916
                break;                                                                                              // PABLO #1916
            case SUMO_TAG_DET_ENTRY:                                                                                // PABLO #1916
            case SUMO_TAG_DET_EXIT:                                                                                 // PABLO #1916
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_LANE, "#"));                          // PABLO #1916
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_POSITION, "#"));                      // PABLO #1916
                break;                                                                                              // PABLO #1916
            case SUMO_TAG_VSS:                                                                                      // PABLO #1916
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_ID, "#"));                            // PABLO #1916
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_LANES, "#"));                         // PABLO #1916
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_FILE, "s"));                          // PABLO #1916
                break;                                                                                              // PABLO #1916
            case SUMO_TAG_CALIBRATOR:                                                                               // PABLO #1916
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_ID, "#"));                            // PABLO #1916
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_LANE, "#"));                          // PABLO #1916
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_POSITION, "f0"));                     // PABLO #1916
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_FREQUENCY, "i100"));                  // PABLO #1916
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_ROUTEPROBE, "#"));                    // PABLO #1916
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_OUTPUT, "s"));                        // PABLO #1916  
                break;                                                                                              // PABLO #1916
            case SUMO_TAG_REROUTER:                                                                                 // PABLO #1916
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_ID, "#"));                            // PABLO #1916
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_EDGES, "#"));                         // PABLO #1916
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_FILE, "s"));                          // PABLO #1916
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_PROB, "f1"));                         // PABLO #1916
                attrs.push_back(std::pair<SumoXMLAttr, std::string>(SUMO_ATTR_OFF, "bfalse"));                      // PABLO #1916
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
    if (_allowedTags.empty()) {
        _allowedTags.push_back(SUMO_TAG_JUNCTION);
        _allowedTags.push_back(SUMO_TAG_EDGE);
        _allowedTags.push_back(SUMO_TAG_LANE);
        _allowedTags.push_back(SUMO_TAG_BUS_STOP);              // PABLO #1916
        _allowedTags.push_back(SUMO_TAG_CHARGING_STATION);      // PABLO #1916
        _allowedTags.push_back(SUMO_TAG_E1DETECTOR);            // PABLO #1916
        _allowedTags.push_back(SUMO_TAG_E2DETECTOR);            // PABLO #1916
        _allowedTags.push_back(SUMO_TAG_E3DETECTOR);            // PABLO #1916
        _allowedTags.push_back(SUMO_TAG_DET_ENTRY);             // PABLO #1916
        _allowedTags.push_back(SUMO_TAG_DET_EXIT);              // PABLO #1916
        _allowedTags.push_back(SUMO_TAG_VSS);                   // PABLO #1916
        _allowedTags.push_back(SUMO_TAG_CALIBRATOR);            // PABLO #1916
        _allowedTags.push_back(SUMO_TAG_REROUTER);              // PABLO #1916
    }
    return _allowedTags;
}


bool
GNEAttributeCarrier::isNumerical(SumoXMLAttr attr) {
    // define on first access
    if (_numericalAttrs.empty()) {
        _numericalAttrs.insert(SUMO_ATTR_SPEED);
        _numericalAttrs.insert(SUMO_ATTR_PRIORITY);
        _numericalAttrs.insert(SUMO_ATTR_NUMLANES);
        _numericalAttrs.insert(SUMO_ATTR_LENGTH);
        _numericalAttrs.insert(SUMO_ATTR_WIDTH);
        _numericalAttrs.insert(SUMO_ATTR_ENDOFFSET);
        _numericalAttrs.insert(SUMO_ATTR_INDEX);
        _numericalAttrs.insert(SUMO_ATTR_RADIUS);
        _numericalAttrs.insert(SUMO_ATTR_STARTPOS);                 // PABLO #1916
        _numericalAttrs.insert(SUMO_ATTR_ENDPOS);                   // PABLO #1916
        _numericalAttrs.insert(SUMO_ATTR_CHARGINGPOWER);            // PABLO #1916
        _numericalAttrs.insert(SUMO_ATTR_EFFICIENCY);               // PABLO #1916
        _numericalAttrs.insert(SUMO_ATTR_CHARGEDELAY);              // PABLO #1916
        _numericalAttrs.insert(SUMO_ATTR_FREQUENCY);                // PABLO #1916
        _numericalAttrs.insert(SUMO_ATTR_POSITION);                 // PABLO #1916
        _numericalAttrs.insert(SUMO_ATTR_LENGTH);                   // PABLO #1916
        _numericalAttrs.insert(SUMO_ATTR_HALTING_TIME_THRESHOLD);   // PABLO #1916
        _numericalAttrs.insert(SUMO_ATTR_HALTING_SPEED_THRESHOLD);  // PABLO #1916
        _numericalAttrs.insert(SUMO_ATTR_JAM_DIST_THRESHOLD);       // PABLO #1916
        _numericalAttrs.insert(SUMO_ATTR_PROB);                     // PABLO #1916
    }
    return _numericalAttrs.count(attr) == 1;
}


bool
GNEAttributeCarrier::isUnique(SumoXMLAttr attr) {
    // define on first access
    if (_uniqueAttrs.empty()) {
        _uniqueAttrs.insert(SUMO_ATTR_ID);
        _uniqueAttrs.insert(SUMO_ATTR_FROM);
        _uniqueAttrs.insert(SUMO_ATTR_TO);
        _uniqueAttrs.insert(SUMO_ATTR_SHAPE);
        _uniqueAttrs.insert(SUMO_ATTR_POSITION);
        _uniqueAttrs.insert(SUMO_ATTR_EDGES);
    }
    return _uniqueAttrs.count(attr) == 1;
}


const std::vector<std::string>&
GNEAttributeCarrier::discreteChoices(SumoXMLTag tag, SumoXMLAttr attr) {
    // define on first access
    if (_discreteChoices.empty()) {
        std::vector<std::string> choices;
        choices = SUMOXMLDefinitions::NodeTypes.getStrings();
        for (std::vector<std::string>::const_iterator it = choices.begin(); it != choices.end(); ++it) {
            if (*it != toString(NODETYPE_DEAD_END_DEPRECATED)) {
                _discreteChoices[SUMO_TAG_JUNCTION][SUMO_ATTR_TYPE].push_back(*it);
            }
        }

        _discreteChoices[SUMO_TAG_JUNCTION][SUMO_ATTR_KEEP_CLEAR].push_back("true");
        _discreteChoices[SUMO_TAG_JUNCTION][SUMO_ATTR_KEEP_CLEAR].push_back("false");

        choices = SUMOXMLDefinitions::LaneSpreadFunctions.getStrings();
        for (std::vector<std::string>::const_iterator it = choices.begin(); it != choices.end(); ++it) {
            _discreteChoices[SUMO_TAG_EDGE][SUMO_ATTR_SPREADTYPE].push_back(*it);
        }

        choices = SumoVehicleClassStrings.getStrings();
        for (std::vector<std::string>::const_iterator it = choices.begin(); it != choices.end(); ++it) {
            _discreteChoices[SUMO_TAG_EDGE][SUMO_ATTR_ALLOW].push_back(*it);
            _discreteChoices[SUMO_TAG_EDGE][SUMO_ATTR_DISALLOW].push_back(*it);
            _discreteChoices[SUMO_TAG_LANE][SUMO_ATTR_ALLOW].push_back(*it);
            _discreteChoices[SUMO_TAG_LANE][SUMO_ATTR_DISALLOW].push_back(*it);
        }

        _discreteChoices[SUMO_TAG_CROSSING][SUMO_ATTR_PRIORITY].push_back("true");
        _discreteChoices[SUMO_TAG_CROSSING][SUMO_ATTR_PRIORITY].push_back("false");

        _discreteChoices[SUMO_TAG_CHARGING_STATION][SUMO_ATTR_CHARGEINTRANSIT].push_back("true");     // PABLO #1916
        _discreteChoices[SUMO_TAG_CHARGING_STATION][SUMO_ATTR_CHARGEINTRANSIT].push_back("false");    // PABLO #1916

        _discreteChoices[SUMO_TAG_E1DETECTOR][SUMO_ATTR_SPLIT_VTYPE].push_back("true");     // PABLO #1916
        _discreteChoices[SUMO_TAG_E1DETECTOR][SUMO_ATTR_SPLIT_VTYPE].push_back("false");    // PABLO #1916

        _discreteChoices[SUMO_TAG_INDUCTION_LOOP][SUMO_ATTR_SPLIT_VTYPE].push_back("true");     // PABLO #1916
        _discreteChoices[SUMO_TAG_INDUCTION_LOOP][SUMO_ATTR_SPLIT_VTYPE].push_back("false");    // PABLO #1916

        _discreteChoices[SUMO_TAG_E2DETECTOR][SUMO_ATTR_CONT].push_back("true");     // PABLO #1916
        _discreteChoices[SUMO_TAG_E2DETECTOR][SUMO_ATTR_CONT].push_back("false");    // PABLO #1916

        _discreteChoices[SUMO_TAG_LANE_AREA_DETECTOR][SUMO_ATTR_CONT].push_back("true");     // PABLO #1916
        _discreteChoices[SUMO_TAG_LANE_AREA_DETECTOR][SUMO_ATTR_CONT].push_back("false");    // PABLO #1916

        _discreteChoices[SUMO_TAG_REROUTER][SUMO_ATTR_OFF].push_back("true");     // PABLO #1916
        _discreteChoices[SUMO_TAG_REROUTER][SUMO_ATTR_OFF].push_back("false");    // PABLO #1916
    }
    return _discreteChoices[tag][attr];
}


bool
GNEAttributeCarrier::discreteCombinableChoices(SumoXMLTag, SumoXMLAttr attr) {
    return (attr == SUMO_ATTR_ALLOW || attr == SUMO_ATTR_DISALLOW);
}



GNEAttributeCarrier::defaultAttrType                                                                                                                    // PABLO #1916
GNEAttributeCarrier::getDefaultValueType(SumoXMLTag tag, SumoXMLAttr attr) {                                                                            // PABLO #1916
    for(std::vector<std::pair<SumoXMLAttr, std::string> >::iterator i = _allowedAttributes.at(tag).begin(); i != _allowedAttributes.at(tag).end(); i++) // PABLO #1916
        if((*i).first == attr)                                                          // PABLO #1916
            switch((*i).second.at(0)) {                                                 // PABLO #1916
                case 'i':                                                               // PABLO #1916
                    return defaultAttrType_int;                                         // PABLO #1916
                case 'f':                                                               // PABLO #1916
                    return defaultAttrType_float;                                       // PABLO #1916
                case 's':                                                               // PABLO #1916
                    return defaultAttrType_string;                                      // PABLO #1916
                case 'l':                                                               // PABLO #1916
                    return defaultAttrType_stringList;                                  // PABLO #1916
                case 'b':                                                               // PABLO #1916
                    return defaultAttrType_bool;                                        // PABLO #1916
                case '#':                                                               // PABLO #1916
                default:                                                                // PABLO #1916
                    return defaultAttrType_NULL;                                        // PABLO #1916
    }
}


template<> int                                                                                                                                          // PABLO #1916
GNEAttributeCarrier::getDefaultValue(SumoXMLTag tag, SumoXMLAttr attr) {                                                                                // PABLO #1916
    for(std::vector<std::pair<SumoXMLAttr, std::string> >::iterator i = _allowedAttributes.at(tag).begin(); i != _allowedAttributes.at(tag).end(); i++) // PABLO #1916
        if((*i).first == attr)                                                                                                                          // PABLO #1916
            return TplConvert::_str2int((*i).second.substr(1, (*i).second.size() - 1));                                                                 // PABLO #1916
}                                                                                                                                                       // PABLO #1916


template<> SUMOReal                                                                                                                                     // PABLO #1916
GNEAttributeCarrier::getDefaultValue(SumoXMLTag tag, SumoXMLAttr attr) {                                                                                // PABLO #1916
    for(std::vector<std::pair<SumoXMLAttr, std::string> >::iterator i = _allowedAttributes.at(tag).begin(); i != _allowedAttributes.at(tag).end(); i++) // PABLO #1916
        if((*i).first == attr)                                                                                                                          // PABLO #1916
            return TplConvert::_str2SUMOReal((*i).second.substr(1, (*i).second.size() - 1));                                                            // PABLO #1916
}                                                                                                                                                       // PABLO #1916


template<> bool                                                                                                                                         // PABLO #1916
GNEAttributeCarrier::getDefaultValue(SumoXMLTag tag, SumoXMLAttr attr) {                                                                                // PABLO #1916
    for(std::vector<std::pair<SumoXMLAttr, std::string> >::iterator i = _allowedAttributes.at(tag).begin(); i != _allowedAttributes.at(tag).end(); i++) // PABLO #1916
        if((*i).first == attr)                                                                                                                          // PABLO #1916
            return TplConvert::_str2bool((*i).second.substr(1, (*i).second.size() - 1));                                                                // PABLO #1916
}                                                                                                                                                       // PABLO #1916


template<> std::string                                                                                                                                  // PABLO #1916
GNEAttributeCarrier::getDefaultValue(SumoXMLTag tag, SumoXMLAttr attr) {                                                                                // PABLO #1916
    for(std::vector<std::pair<SumoXMLAttr, std::string> >::iterator i = _allowedAttributes.at(tag).begin(); i != _allowedAttributes.at(tag).end(); i++) // PABLO #1916
        if((*i).first == attr)                                                                                                                          // PABLO #1916
            return (*i).second.substr(1, (*i).second.size() - 1);                                                                                       // PABLO #1916
}                                                                                                                                                       // PABLO #1916

template<> std::vector<std::string>                                                                                                                     // PABLO #1916
GNEAttributeCarrier::getDefaultValue(SumoXMLTag tag, SumoXMLAttr attr) {                                                                                // PABLO #1916
    for(std::vector<std::pair<SumoXMLAttr, std::string> >::iterator i = _allowedAttributes.at(tag).begin(); i != _allowedAttributes.at(tag).end(); i++) // PABLO #1916
        if((*i).first == attr) {                                                                                                                        // PABLO #1916
            std::vector<std::string> myVectorString;                                                                                                    // PABLO #1916
            SUMOSAXAttributes::parseStringVector((*i).second.substr(1, (*i).second.size() - 1), myVectorString);                                        // PABLO #1916
            return myVectorString;                                                                                                                      // PABLO #1916
        }                                                                                                                                               // PABLO #1916
}                                                                                                                                                       // PABLO #1916

/****************************************************************************/

