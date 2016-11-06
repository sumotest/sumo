/****************************************************************************/
/// @file    MSTriggeredRerouter.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    Mon, 25 July 2005
/// @version $Id$
///
// Reroutes vehicles passing an edge
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

#include <string>
#include <algorithm>
#include <utils/options/OptionsCont.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/Command.h>
#include <utils/xml/SUMOXMLDefinitions.h>
#include <utils/common/UtilExceptions.h>
#include <utils/common/ToString.h>
#include <utils/common/TplConvert.h>
#include <utils/xml/SUMOSAXHandler.h>
#include <utils/vehicle/DijkstraRouterTT.h>
#include <utils/common/RandHelper.h>
#include <utils/common/WrappingCommand.h>
#include <microsim/MSEdgeWeightsStorage.h>
#include <microsim/MSLane.h>
#include <microsim/MSVehicle.h>
#include <microsim/MSRoute.h>
#include <microsim/MSEdge.h>
#include <microsim/MSNet.h>
#include <microsim/MSGlobals.h>
#include <microsim/devices/MSDevice_Example.h>
#include "MSTriggeredRerouter.h"

#ifdef HAVE_INTERNAL
#include <mesosim/MELoop.h>
#include <mesosim/MESegment.h>
#endif

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// static member defintion
// ===========================================================================
MSEdge MSTriggeredRerouter::mySpecialDest_keepDestination("MSTriggeredRerouter_keepDestination", -1, MSEdge::EDGEFUNCTION_UNKNOWN, "", "", -1);
MSEdge MSTriggeredRerouter::mySpecialDest_terminateRoute("MSTriggeredRerouter_terminateRoute", -1, MSEdge::EDGEFUNCTION_UNKNOWN, "", "", -1);

// ===========================================================================
// method definitions
// ===========================================================================
MSTriggeredRerouter::MSTriggeredRerouter(const std::string& id,
        const MSEdgeVector& edges,
        SUMOReal prob, const std::string& file, bool off) :
    MSTrigger(id),
    MSMoveReminder(id),
    SUMOSAXHandler(file),
    myProbability(prob), myUserProbability(prob), myAmInUserMode(false) {
    // build actors
    for (MSEdgeVector::const_iterator j = edges.begin(); j != edges.end(); ++j) {
#ifdef HAVE_INTERNAL
        if (MSGlobals::gUseMesoSim) {
            MESegment* s = MSGlobals::gMesoNet->getSegmentForEdge(**j);
            s->addDetector(this);
            continue;
        }
#endif
        const std::vector<MSLane*>& destLanes = (*j)->getLanes();
        for (std::vector<MSLane*>::const_iterator i = destLanes.begin(); i != destLanes.end(); ++i) {
            (*i)->addMoveReminder(this);
        }
    }
    if (off) {
        setUserMode(true);
        setUserUsageProbability(0);
    }
}


MSTriggeredRerouter::~MSTriggeredRerouter() {
}

// ------------ loading begin
void
MSTriggeredRerouter::myStartElement(int element,
                                    const SUMOSAXAttributes& attrs) {
    if (element == SUMO_TAG_INTERVAL) {
        bool ok = true;
        myCurrentIntervalBegin = attrs.getOptSUMOTimeReporting(SUMO_ATTR_BEGIN, 0, ok, -1);
        myCurrentIntervalEnd = attrs.getOptSUMOTimeReporting(SUMO_ATTR_END, 0, ok, -1);
    }
    if (element == SUMO_TAG_DEST_PROB_REROUTE) {
        // by giving probabilities of new destinations
        // get the destination edge
        std::string dest = attrs.getStringSecure(SUMO_ATTR_ID, "");
        if (dest == "") {
            throw ProcessError("MSTriggeredRerouter " + getID() + ": No destination edge id given.");
        }
        MSEdge* to = MSEdge::dictionary(dest);
        if (to == 0) {
            if (dest == "keepDestination") {
                to = &mySpecialDest_keepDestination;
            } else if (dest == "terminateRoute") {
                to = &mySpecialDest_terminateRoute;
            } else {
                throw ProcessError("MSTriggeredRerouter " + getID() + ": Destination edge '" + dest + "' is not known.");
            }
        }
        // get the probability to reroute
        bool ok = true;
        SUMOReal prob = attrs.getOpt<SUMOReal>(SUMO_ATTR_PROB, getID().c_str(), ok, 1.);
        if (!ok) {
            throw ProcessError();
        }
        if (prob < 0) {
            throw ProcessError("MSTriggeredRerouter " + getID() + ": Attribute 'probability' for destination '" + dest + "' is negative (must not).");
        }
        // add
        myCurrentEdgeProb.add(prob, to);
    }
    

    if (element == SUMO_TAG_CLOSING_REROUTE) {
        // by closing
        std::string closed_id = attrs.getStringSecure(SUMO_ATTR_ID, "");
        MSEdge* closed = MSEdge::dictionary(closed_id);
        if (closed == 0) {
            throw ProcessError("MSTriggeredRerouter " + getID() + ": Edge '" + closed_id + "' to close is not known.");
        }
        myCurrentClosed.push_back(closed);
        bool ok;
        const std::string allow = attrs.getOpt<std::string>(SUMO_ATTR_ALLOW, getID().c_str(), ok, "", false);
        const std::string disallow = attrs.getOpt<std::string>(SUMO_ATTR_DISALLOW, getID().c_str(), ok, "");
        myCurrentPermissions = parseVehicleClasses(allow, disallow);
    }

    if (element == SUMO_TAG_ROUTE_PROB_REROUTE) {
        // by explicit rerouting using routes
        // check if route exists
        std::string routeStr = attrs.getStringSecure(SUMO_ATTR_ID, "");
        if (routeStr == "") {
            throw ProcessError("MSTriggeredRerouter " + getID() + ": No route id given.");
        }
        const MSRoute* route = MSRoute::dictionary(routeStr);
        if (route == 0) {
            throw ProcessError("MSTriggeredRerouter " + getID() + ": Route '" + routeStr + "' does not exist.");
        }

        // get the probability to reroute
        bool ok = true;
        SUMOReal prob = attrs.getOpt<SUMOReal>(SUMO_ATTR_PROB, getID().c_str(), ok, 1.);
        if (!ok) {
            throw ProcessError();
        }
        if (prob < 0) {
            throw ProcessError("MSTriggeredRerouter " + getID() + ": Attribute 'probability' for route '" + routeStr + "' is negative (must not).");
        }
        // add
        myCurrentRouteProb.add(prob, route);
    }
    
    if (element == SUMO_TAG_PARKING_ZONE_REROUTE) {
        // by giving probabilities of new destinations
        // get the destination edge
        std::string parkingarea = attrs.getStringSecure(SUMO_ATTR_ID, "");
        if (parkingarea == "") {
            throw ProcessError("MSTriggeredRerouter " + getID() + ": No parking area id given.");
        }
        MSParkingArea* pa = MSNet::getInstance()->getParkingArea(parkingarea);
        if (pa == 0) {
            throw ProcessError("MSTriggeredRerouter " + getID() + ": Parking area '" + parkingarea + "' is not known.");
        }
        // get the probability to reroute
        bool ok = true;
        SUMOReal prob = attrs.getOpt<SUMOReal>(SUMO_ATTR_PROB, getID().c_str(), ok, 1.);
        if (!ok) {
            throw ProcessError();
        }
        if (prob < 0) {
            throw ProcessError("MSTriggeredRerouter " + getID() + ": Attribute 'probability' for destination '" + parkingarea + "' is negative (must not).");
        }
        // add
        myCurrentParkProb.add(prob, pa);
        //MSEdge* to = &(pa->getLane().getEdge());
        //myCurrentEdgeProb.add(prob, to);
    }
}


void
MSTriggeredRerouter::myEndElement(int element) {
    if (element == SUMO_TAG_INTERVAL) {
        RerouteInterval ri;
        ri.begin = myCurrentIntervalBegin;
        ri.end = myCurrentIntervalEnd;
        ri.closed = myCurrentClosed;
        ri.parkProbs = myCurrentParkProb;
        ri.edgeProbs = myCurrentEdgeProb;
        ri.routeProbs = myCurrentRouteProb;
        ri.permissions = myCurrentPermissions;
        myCurrentClosed.clear();
        myCurrentParkProb.clear();
        myCurrentEdgeProb.clear();
        myCurrentRouteProb.clear();
        myIntervals.push_back(ri);
        if (!ri.closed.empty() && ri.permissions != SVCAll) {
            MSNet::getInstance()->getBeginOfTimestepEvents()->addEvent(
                new WrappingCommand<MSTriggeredRerouter>(this, &MSTriggeredRerouter::setPermissions), ri.begin,
                MSEventControl::ADAPT_AFTER_EXECUTION);
        }
    }
}


// ------------ loading end


SUMOTime
MSTriggeredRerouter::setPermissions(const SUMOTime currentTime) {
    for (std::vector<RerouteInterval>::iterator i = myIntervals.begin(); i != myIntervals.end(); ++i) {
        if (i->begin == currentTime && !i->closed.empty() && i->permissions != SVCAll) {
            for (MSEdgeVector::iterator e = i->closed.begin(); e != i->closed.end(); ++e) {
                for (std::vector<MSLane*>::const_iterator l = (*e)->getLanes().begin(); l != (*e)->getLanes().end(); ++l) {
                    i->prevPermissions.push_back((*l)->getPermissions());
                    (*l)->setPermissions(i->permissions);
                }
                (*e)->rebuildAllowedLanes();
            }
            MSNet::getInstance()->getBeginOfTimestepEvents()->addEvent(
                new WrappingCommand<MSTriggeredRerouter>(this, &MSTriggeredRerouter::setPermissions), i->end,
                MSEventControl::ADAPT_AFTER_EXECUTION);
        }
        if (i->end == currentTime && !i->closed.empty() && i->permissions != SVCAll) {
            for (MSEdgeVector::iterator e = i->closed.begin(); e != i->closed.end(); ++e) {
                std::vector<SVCPermissions>::const_iterator p = i->prevPermissions.begin();
                for (std::vector<MSLane*>::const_iterator l = (*e)->getLanes().begin(); l != (*e)->getLanes().end(); ++l, ++p) {
                    (*l)->setPermissions(*p);
                }
                (*e)->rebuildAllowedLanes();
            }
        }
    }
    return 0;
}


const MSTriggeredRerouter::RerouteInterval*
MSTriggeredRerouter::getCurrentReroute(SUMOTime time, SUMOVehicle& veh) const {
    for (std::vector<RerouteInterval>::const_iterator i = myIntervals.begin(); i != myIntervals.end(); ++i) {
        if (i->begin <= time && i->end > time) {
            if (
                // affected by closingReroute, possibly combined with destProbReroute (route prob makes no sense)
                veh.getRoute().containsAnyOf(i->closed) ||
                // no closingReroute but destProbReroute or routeProbReroute
                i->parkProbs.getOverallProb() > 0 || i->edgeProbs.getOverallProb() > 0 || i->routeProbs.getOverallProb() > 0) {
                return &*i;
            }
        }
    }
    return 0;
}


const MSTriggeredRerouter::RerouteInterval*
MSTriggeredRerouter::getCurrentReroute(SUMOTime time) const {
    for (std::vector<RerouteInterval>::const_iterator i = myIntervals.begin(); i != myIntervals.end(); ++i) {
        if (i->begin <= time && i->end > time) {
            if (i->parkProbs.getOverallProb() != 0 || i->edgeProbs.getOverallProb() != 0 || i->routeProbs.getOverallProb() != 0 || !i->closed.empty()) {
                return &*i;
            }
        }
    }
    return 0;
}



bool
MSTriggeredRerouter::notifyEnter(SUMOVehicle& veh, MSMoveReminder::Notification reason) {
    if (reason == MSMoveReminder::NOTIFICATION_LANE_CHANGE) {
        return false;
    }

    // check whether the vehicle shall be rerouted
    const SUMOTime time = MSNet::getInstance()->getCurrentTimeStep();
    const MSTriggeredRerouter::RerouteInterval* rerouteDef = getCurrentReroute(time, veh);
    if (rerouteDef == 0) {
        return false;
    }

    SUMOReal prob = myAmInUserMode ? myUserProbability : myProbability;
    if (RandHelper::rand() > prob) {
        return false;
    }

    // get vehicle params
    const MSRoute& route = veh.getRoute();
    const MSEdge* lastEdge = route.getLastEdge();
    
    const MSEdge* newEdge = lastEdge;
    // ok, try using a new destination
    SUMOReal newArrivalPos = -1;
    bool stopsAreChanged = false;

    if (rerouteDef->parkProbs.getOverallProb() > 0) {
        MSParkingArea *nearParkArea = rerouteParkingZone(rerouteDef, veh);
        if (nearParkArea != 0) {
            stopsAreChanged = true;
            veh.replaceParkingArea(nearParkArea);
            newEdge = &(nearParkArea->getLane().getEdge());
        }
    }

    // If the event doesn't trigger reroute for parking zone check other types
    if (!stopsAreChanged) {
        // get rerouting params
        const MSRoute* newRoute = rerouteDef->routeProbs.getOverallProb() > 0 ? rerouteDef->routeProbs.get() : 0;
        // we will use the route if given rather than calling our own dijsktra...
        if (newRoute != 0) {
            veh.replaceRoute(newRoute);
            return false;
        }
        const bool destUnreachable = std::find(rerouteDef->closed.begin(), rerouteDef->closed.end(), lastEdge) != rerouteDef->closed.end();
        // if we have a closingReroute, only assign new destinations to vehicles which cannot reach their original destination
        if (rerouteDef->closed.size() == 0 || destUnreachable) {
            newEdge = rerouteDef->edgeProbs.getOverallProb() > 0 ? rerouteDef->edgeProbs.get() : route.getLastEdge();
            if (newEdge == &mySpecialDest_terminateRoute) {
                newEdge = veh.getEdge();
                newArrivalPos = veh.getPositionOnLane(); // instant arrival
            } else if (newEdge == &mySpecialDest_keepDestination || newEdge == lastEdge) {
                if (destUnreachable && rerouteDef->permissions == SVCAll) {
                    // if permissions aren't set vehicles will simply drive through
                    // the closing unless terminated. If the permissions are specified, assume that the user wants
                    // vehicles to stand and wait until the closing ends
                    WRITE_WARNING("Cannot keep destination edge '" + lastEdge->getID() + "' for vehicle '" + veh.getID() + "' due to closed edges. Terminating route.");
                    newEdge = veh.getEdge();
                } else {
                    newEdge = lastEdge;
                }
            } else if (newEdge == 0) {
                assert(false); // this should never happen
                newEdge = veh.getEdge();
            }
        }
    }
    // we have a new destination, let's replace the vehicle route
    ConstMSEdgeVector edges;
    MSNet::getInstance()->getRouterTT(rerouteDef->closed).compute(
        veh.getEdge(), newEdge, &veh, MSNet::getInstance()->getCurrentTimeStep(), edges);
    veh.replaceRouteEdges(edges, false, stopsAreChanged);
    if (newArrivalPos != -1) {
        // must be called here because replaceRouteEdges may also set the arrivalPos
        veh.setArrivalPos(newArrivalPos);
    }
    return false;
}


void
MSTriggeredRerouter::setUserMode(bool val) {
    myAmInUserMode = val;
}


void
MSTriggeredRerouter::setUserUsageProbability(SUMOReal prob) {
    myUserProbability = prob;
}


bool
MSTriggeredRerouter::inUserMode() const {
    return myAmInUserMode;
}


SUMOReal
MSTriggeredRerouter::getProbability() const {
    return myAmInUserMode ? myUserProbability : myProbability;
}


SUMOReal
MSTriggeredRerouter::getUserProbability() const {
    return myUserProbability;
}


SUMOReal
MSTriggeredRerouter::getWeight(SUMOVehicle& veh, const std::string param, const SUMOReal defaultWeight) const {
    OptionsCont& oc = OptionsCont::getOptions();
    
    // get default parameter
    SUMOReal customParameter1 = -1;
    if (oc.exists(param)) {
        try {
            customParameter1 = oc.getFloat(param);
        } catch (...) {
            WRITE_WARNING("Invalid value for parameter '" + param + "'");
        }

    } else {
        std::cout << "vehicle '" << veh.getID() << "' does not supply vehicle parameter '" + param + "'. Using default of " << defaultWeight << "\n";
    }
    
    // get custom vType parameter
    SUMOReal customParameter2 = -1;
    if (veh.getVehicleType().getParameter().knowsParameter(param)) {
        try {
            customParameter2 = TplConvert::_2SUMOReal(veh.getVehicleType().getParameter().getParameter(param, "-1").c_str());
        } catch (...) {
            WRITE_WARNING("Invalid value '" + veh.getVehicleType().getParameter().getParameter(param, "-1") + "' for vType parameter '" + param + "'");
        }

    } else {
        std::cout << "vehicle '" << veh.getID() << "' does not supply vType parameter '" + param + "'. Using default of " << customParameter1 << "\n";
    }
    
    // get custom vehicle parameter
    SUMOReal customParameter3 = -1;
    if (veh.getParameter().knowsParameter(param)) {
        try {
            customParameter3 = TplConvert::_2SUMOReal(veh.getParameter().getParameter(param, "-1").c_str());
        } catch (...) {
            WRITE_WARNING("Invalid value '" + veh.getParameter().getParameter(param, "-1") + "' for vehicle parameter '" + param + "'");
        }

    } else {
        std::cout << "vehicle '" << veh.getID() << "' does not supply vehicle parameter '" + param + "'. Using default of " << customParameter2 << "\n";
    }

    return (customParameter3 != -1 ? customParameter3 : 
           (customParameter2 != -1 ? customParameter2 : 
           (customParameter1 != -1 ? customParameter1 : defaultWeight)));
}


MSParkingArea*
MSTriggeredRerouter::rerouteParkingZone(const MSTriggeredRerouter::RerouteInterval* rerouteDef, SUMOVehicle& veh) const {
    
    MSParkingArea *nearParkArea = 0;

    // get vehicle params
    MSParkingArea *destParkArea = veh.getNextParkingArea();
    const MSRoute& route = veh.getRoute();

    // I reroute destination from initial parking area to the near parking area
    // if the next stop is a parking area, if the current parking area is on the same edge of the vehicle and if it is full
    if (destParkArea != 0 && 
        &(destParkArea->getLane().getEdge()) == veh.getEdge()  && 
        destParkArea->getOccupancy() == destParkArea->getCapacity()) {

        double maxParkCapacity = 0;
        double maxParkAbsFreeSpace = 0;
        double maxParkRelFreeSpace = 0;
        double maxRouteLengthToPark = 0.0;
        double maxRouteLengthToEnd = 0.0;
        double maxRouteCostToPark = 0.0;
        double maxRouteCostToEnd = 0.0;
        double minRouteLength = 0.0;

        SUMOAbstractRouter<MSEdge, SUMOVehicle>& router = MSNet::getInstance()->getRouterTT(rerouteDef->closed);

        std::vector<MSParkingArea*> parks = rerouteDef->parkProbs.getVals();

        for (std::vector<MSParkingArea*>::const_iterator i = parks.begin(); i != parks.end(); ++i) {
            if ((*i)->getOccupancy() < (*i)->getCapacity()) {
                    
                const SUMOReal parkCapacity = (double)((*i)->getCapacity());
                const SUMOReal parkAbsFreeSpace = (double)((*i)->getCapacity() - (*i)->getOccupancy());
                const SUMOReal parkRelFreeSpace = parkAbsFreeSpace / parkCapacity;

                const RGBColor& c = route.getColor();
                const MSEdge* parkEdge = &((*i)->getLane().getEdge());
                const bool includeInternalLengths = MSGlobals::gUsingInternalLanes && MSNet::getInstance()->hasInternalLinks();
                
                // Compute the route from the current edge to the parking area edge
                ConstMSEdgeVector edgesToPark;
                router.compute(veh.getEdge(), parkEdge, &veh, MSNet::getInstance()->getCurrentTimeStep(), edgesToPark);
                MSRoute routeToPark(route.getID() + "!topark#1", edgesToPark, false, &c == &RGBColor::DEFAULT_COLOR ? 0 : new RGBColor(c), route.getStops());

                // The distance from the current edge to the new parking area
                const SUMOReal routeLengthToPark = routeToPark.getDistanceBetween(veh.getPositionOnLane(), (*i)->getBeginLanePosition(),
                                                                            routeToPark.begin(), routeToPark.end(), includeInternalLengths);
                    
                // The time to reach the new parking area
                const SUMOReal routeCostToPark = router.recomputeCosts(edgesToPark, &veh, MSNet::getInstance()->getCurrentTimeStep());

                // Compute the route from the current edge to the parking area edge
                ConstMSEdgeVector edgesFromPark;
                router.compute(parkEdge, route.getLastEdge(), &veh, MSNet::getInstance()->getCurrentTimeStep(), edgesFromPark);
                MSRoute routeFromPark(route.getID() + "!frompark#1", edgesFromPark, false, &c == &RGBColor::DEFAULT_COLOR ? 0 : new RGBColor(c), route.getStops());
                    
                // The distance from the new parking area to the end of the route
                const SUMOReal routeLengthFromPark = routeFromPark.getDistanceBetween((*i)->getBeginLanePosition(), routeFromPark.getLastEdge()->getLength(),
                                                                                        routeFromPark.begin(), routeFromPark.end(), includeInternalLengths);
                    
                // The time to reach this area
                const SUMOReal routeCostFromPark = router.recomputeCosts(edgesFromPark, &veh, MSNet::getInstance()->getCurrentTimeStep());

                // get the parking area near to vehicle
                // (should be a minimum cost function with prob, distance, and free space?)
                if (nearParkArea == 0 || routeLengthToPark < minRouteLength) {
                    minRouteLength = routeLengthToPark;
                    nearParkArea = (*i);
                }
            }
        }
    }

    return nearParkArea;
}

/****************************************************************************/

