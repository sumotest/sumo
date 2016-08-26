/****************************************************************************/
/// @file    MSCFModel.cpp
/// @author  Tobias Mayer
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @author  Laura Bieker
/// @author  Leonhard Lücken
/// @date    Mon, 27 Jul 2009
/// @version $Id$
///
// The car-following model abstraction
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2016 DLR (http://www.dlr.de/) and contributors
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

#include <math.h>
#include <microsim/MSGlobals.h>
#include <microsim/MSVehicleType.h>
#include <microsim/MSVehicle.h>
#include <microsim/MSLane.h>
#include <microsim/lcmodels/MSAbstractLaneChangeModel.h>
#include "MSCFModel.h"


// ===========================================================================
// method definitions
// ===========================================================================
MSCFModel::MSCFModel(const MSVehicleType* vtype, const SUMOReal accel,
                     const SUMOReal decel, const SUMOReal headwayTime)
    : myType(vtype), myAccel(accel), myDecel(decel), myHeadwayTime(headwayTime) {
}


MSCFModel::~MSCFModel() {}


MSCFModel::VehicleVariables::~VehicleVariables() {}


SUMOReal
MSCFModel::brakeGap(const SUMOReal speed, const SUMOReal decel, const SUMOReal headwayTime) {
	if(MSGlobals::gSemiImplicitEulerUpdate){
		/* one possibility to speed this up is to calculate speedReduction * steps * (steps+1) / 2
			for small values of steps (up to 10 maybe) and store them in an array */
		const SUMOReal speedReduction = ACCEL2SPEED(decel);
		const int steps = int(speed / speedReduction);
		return SPEED2DIST(steps * speed - speedReduction * steps * (steps + 1) / 2) + speed * headwayTime;
	} else {
		// ballistic
		return speed * (headwayTime + 0.5*speed/decel);
	}
}


SUMOReal
MSCFModel::freeSpeed(const SUMOReal currentSpeed, const SUMOReal decel, const SUMOReal dist, const SUMOReal targetSpeed, const bool onInsertion) {
	// XXX: (Leo) This seems to be exclusively called with decel = myDecel (max deceleration) and is not overridden
	// by any specific CFModel. That may cause undesirable hard braking (at junctions where the vehicle
	// changes to a road with a lower speed limit). It relies on the same logic as the maximalSafeSpeed calculations.

	if(MSGlobals::gSemiImplicitEulerUpdate){
		// adapt speed to succeeding lane, no reaction time is involved
		// when breaking for y steps the following distance g is covered
		// (drive with v in the final step)
		// g = (y^2 + y) * 0.5 * b + y * v
		// y = ((((sqrt((b + 2.0*v)*(b + 2.0*v) + 8.0*b*g)) - b)*0.5 - v)/b)
		const SUMOReal v = SPEED2DIST(targetSpeed);
		if (dist < v) {
			return targetSpeed;
		}
		const SUMOReal b = ACCEL2DIST(decel);
		const SUMOReal y = MAX2(0.0, ((sqrt((b + 2.0 * v) * (b + 2.0 * v) + 8.0 * b * dist) - b) * 0.5 - v) / b);
		const SUMOReal yFull = floor(y);
		const SUMOReal exactGap = (yFull * yFull + yFull) * 0.5 * b + yFull * v + (y > yFull ? v : 0.0);
		const SUMOReal fullSpeedGain = (yFull + (onInsertion ? 1. : 0.)) * ACCEL2SPEED(decel);
		return DIST2SPEED(MAX2((SUMOReal)0.0, dist - exactGap) / (yFull + 1)) + fullSpeedGain + targetSpeed;
	} else {
		// ballistic update (Leo)
		// calculate maximum next speed vN that is adjustable to vT=targetSpeed after a distance d=dist
		// and given a maximal deceleration b=decel, denote the current speed by v0.
		// the distance covered by a trajectory that attains vN in the next timestep and decelerates afterwards
		// with b is given as
		// d = 0.5*dt*(v0+vN) + (t-dt)*vN - 0.5*b*(t-dt)^2, (1)
		// where time t of arrival at d with speed vT is
		// t = dt + (vN-vT)/b.  (2)
		// We insert (2) into (1) to obtain
		// d = 0.5*dt*(v0+vN) + vN*(vN-vT)/b - 0.5*b*((vN-vT)/b)^2
		// 0 = (dt*b*v0 - vT*vT - 2*b*d) + dt*b*vN + vN*vN
		// and solve for vN

		assert(currentSpeed >= 0);
		assert(targetSpeed >= 0);

		SUMOReal dt = onInsertion ? 0 : TS; // handles case that vehicle is inserted just now (at the end of move)
		SUMOReal v0 = currentSpeed;
		SUMOReal vT = targetSpeed;
		SUMOReal b = decel;

		// Solvability for positive vN (if d is small relative to v0):
		// 1) If 0.5*(v0+vT)*dt > d, we set vN=vT.
		// (In case vT<v0, this implies that on the interpolated trajectory there are points beyond d where
		//  the interpolated velocity is larger than vT, but at least on the temporal discretization grid, vT is not exceeded)
		// 2) We ignore the (possible) constraint vN >= v0 - b*dt, which could lead to a problem if v0 - t*b > vT.
		//    (moveHelper() is responsible for assuring that the next velocity is chosen in accordance with maximal decelerations)

		if(0.5*(v0+vT)*dt >= dist) return vT; // (#)

		SUMOReal d = dist - NUMERICAL_EPS; // prevent returning a value > targetSpeed due to rounding errors

		SUMOReal q = ((dt*v0 - 2*d)*b - vT*vT); // (q < 0 is fulfilled because of (#))
		SUMOReal p = 0.5*b*dt;
		return -p + sqrt(p*p - q);
	}
}

SUMOReal
MSCFModel::moveHelper(MSVehicle* const veh, SUMOReal vPos) const {
    const SUMOReal oldV = veh->getSpeed(); // save old v for optional acceleration computation
    const SUMOReal vSafe = MIN2(vPos, veh->processNextStop(vPos)); // process stops
    // we need the acceleration for emission computation;
    //  in this case, we neglect dawdling, nonetheless, using
    //  vSafe does not incorporate speed reduction due to interaction
    //  on lane changing
    SUMOReal vMin;
    if(MSGlobals::gSemiImplicitEulerUpdate){
    	vMin = getSpeedAfterMaxDecel(oldV);
    } else {
    	// for ballistic update, negative vnext must be allowed to
    	// indicate a stop within the coming timestep (i.e., to attain negative values)
    	vMin = oldV - ACCEL2SPEED(getMaxDecel());
    }
    const SUMOReal vMax = MIN3(veh->getLane()->getVehicleMaxSpeed(veh), maxNextSpeed(oldV, veh), vSafe);
    assert(vMin <= vMax);

    SUMOReal vNext = veh->getLaneChangeModel().patchSpeed(vMin, vMax, vMax, *this);

	// (Leo) moveHelper() is responsible for assuring that the next
    // velocity is chosen in accordance with maximal decelerations.
    // At this point vNext may also be negative indicating a stop within next step.
    // Moreover, because maximumSafeStopSpeed() does not consider deceleration bounds
    // vNext can be a large negative value at this point. We cap vNext here.
	vNext = MAX2(vNext, veh->getSpeed() - ACCEL2SPEED(getMaxDecel()));

    return vNext;
}


SUMOReal
MSCFModel::interactionGap(const MSVehicle* const veh, SUMOReal vL) const {
    // Resolve the vsafe equation to gap. Assume predecessor has
    // speed != 0 and that vsafe will be the current speed plus acceleration,
    // i.e that with this gap there will be no interaction.
    const SUMOReal vNext = MIN2(maxNextSpeed(veh->getSpeed(), veh), veh->getLane()->getVehicleMaxSpeed(veh));
    const SUMOReal gap = (vNext - vL) *
                         ((veh->getSpeed() + vL) / (2.*myDecel) + myHeadwayTime) +
                         vL * myHeadwayTime;

    // Don't allow timeHeadWay < deltaT situations.
    return MAX2(gap, SPEED2DIST(vNext));
}


SUMOReal
MSCFModel::maxNextSpeed(SUMOReal speed, const MSVehicle* const /*veh*/) const {
    return MIN2(speed + (SUMOReal) ACCEL2SPEED(getMaxAccel()), myType->getMaxSpeed());
}


SUMOReal
MSCFModel::freeSpeed(const MSVehicle* const veh, SUMOReal speed, SUMOReal seen, SUMOReal maxSpeed, const bool onInsertion) const {

//    // Debug (Leo)
//	if(gDebugFlag1){
//    	std::cout
//    	<< "called freeSpeed() for vehicle " << veh->getID()
//    	<< (onInsertion ? " on insertion":"")
//    	<< "\nspeed = " << speed
//    	<< "\ndist = " << seen
//    	<< "\nmax speed = " << maxSpeed
//    	<< "\nmyDecel = " << myDecel
//    	<< std::endl;
//    }

	SUMOReal vSafe = freeSpeed(speed, myDecel, seen, maxSpeed, onInsertion);

//    // Debug (Leo)
//	if(gDebugFlag1){
//    	std::cout
//    	<< "vSafe " << vSafe
//    	<< std::endl;
//	}

	return vSafe;

}


SUMOReal
MSCFModel::insertionFollowSpeed(const MSVehicle* const v, SUMOReal speed, SUMOReal gap2pred, SUMOReal predSpeed, SUMOReal predMaxDecel) const {
	if(MSGlobals::gSemiImplicitEulerUpdate){
		return maximumSafeFollowSpeed(gap2pred, speed, predSpeed, predMaxDecel);
	} else {
		// NOTE: Even for ballistic update, the current speed is irrelevant at insertion, therefore passing 0. (Leo)
		return maximumSafeFollowSpeed(gap2pred, 0., predSpeed, predMaxDecel, true);
	}
}


SUMOReal
MSCFModel::getMinimalArrivalTime(SUMOReal dist, SUMOReal currentSpeed, SUMOReal arrivalSpeed) const{
    const SUMOReal accel = (arrivalSpeed >= currentSpeed) ? getMaxAccel() : -getMaxDecel();
    const SUMOReal accelTime = (arrivalSpeed - currentSpeed) / accel;
    const SUMOReal accelWay = accelTime * (arrivalSpeed + currentSpeed) * 0.5;
    const SUMOReal nonAccelWay = MAX2(SUMOReal(0), dist - accelWay);
    // will either drive as fast as possible and decelerate as late as possible
    // or accelerate as fast as possible and then hold that speed
    const SUMOReal nonAccelSpeed = MAX3(currentSpeed, arrivalSpeed, SUMO_const_haltingSpeed);
    const SUMOTime arrivalTime = TIME2STEPS(accelTime + nonAccelWay / nonAccelSpeed);
    return arrivalTime;
}


SUMOReal
MSCFModel::getMinimalArrivalSpeed(SUMOReal dist, SUMOReal currentSpeed) const{
	// ballistic update
	return estimateSpeedAfterDistance(dist - currentSpeed * getHeadwayTime(), currentSpeed, -getMaxDecel());
}


SUMOReal
MSCFModel::getMinimalArrivalSpeedEuler(SUMOReal dist, SUMOReal currentSpeed) const{
	SUMOReal arrivalSpeedBraking;
	// Because we use a continuous formula for computing the possible slow-down
	// we need to handle the mismatch with the discrete dynamics
	if (dist < currentSpeed) {
		arrivalSpeedBraking = std::numeric_limits<SUMOReal>::max(); // no time left for braking after this step
																	//	(inserted max() to get rid of arrivalSpeed dependency within method) (Leo)
	} else if (2 * (dist - currentSpeed * getHeadwayTime()) * -getMaxDecel() + currentSpeed * currentSpeed >= 0) {
		arrivalSpeedBraking = estimateSpeedAfterDistance(dist - currentSpeed * getHeadwayTime(), currentSpeed, -getMaxDecel());
	} else {
		arrivalSpeedBraking = getMaxDecel();
	}
	return arrivalSpeedBraking;
}



SUMOReal
MSCFModel::estimateSpeedAfterDistance(const SUMOReal dist, const SUMOReal v, const SUMOReal accel) const {
    // dist=v*t + 0.5*accel*t^2, solve for t and multiply with accel, then add v
    return MAX2(0., MIN2(myType->getMaxSpeed(),
                (SUMOReal)sqrt(2 * dist * accel + v * v)));
}



SUMOReal
MSCFModel::maximumSafeStopSpeed(SUMOReal g /*gap*/, SUMOReal v /*currentSpeed*/, bool onInsertion, SUMOReal headway) const {
	if(MSGlobals::gSemiImplicitEulerUpdate){
		return maximumSafeStopSpeedEuler(g);
	} else {
		return maximumSafeStopSpeedBallistic(g, v, onInsertion, headway);
	}
}


SUMOReal
MSCFModel::maximumSafeStopSpeedEuler(SUMOReal gap) const {
	gap -= NUMERICAL_EPS; // lots of code relies on some slack
	if (gap <= 0) {
		return 0;
	} else if (gap <= ACCEL2SPEED(myDecel)) {
        // workaround for #2310
        return MIN2(ACCEL2SPEED(myDecel), DIST2SPEED(gap));
	}
	const SUMOReal g = gap;
	const SUMOReal b = ACCEL2SPEED(myDecel);
	const SUMOReal t = myHeadwayTime;
	const SUMOReal s = TS;


	// h = the distance that would be covered if it were possible to stop
	// exactly after gap and decelerate with b every simulation step
	// h = 0.5 * n * (n-1) * b * s + n * b * t (solve for n)
	//n = ((1.0/2.0) - ((t + (pow(((s*s) + (4.0*((s*((2.0*h/b) - t)) + (t*t)))), (1.0/2.0))*sign/2.0))/s));
	const SUMOReal n = floor(.5 - ((t + (sqrt(((s * s) + (4.0 * ((s * (2.0 * g / b - t)) + (t * t))))) * -0.5)) / s));
	const SUMOReal h = 0.5 * n * (n - 1) * b * s + n * b * t;
	assert(h <= g + NUMERICAL_EPS);
	// compute the additional speed that must be used during deceleration to fix
	// the discrepancy between g and h
	const SUMOReal r = (g - h) / (n * s + t);
	const SUMOReal x = n * b + r;
	assert(x >= 0);
	return x;
}


SUMOReal
MSCFModel::maximumSafeStopSpeedBallistic(SUMOReal g /*gap*/, SUMOReal v /*currentSpeed*/, bool onInsertion, SUMOReal headway) const {

	// (Leo) Note that in contrast to the Euler update, for the ballistic update
	// the distance covered in the coming step depends on the current velocity, in general.
	// one exception is the situation when the vehicle is just being inserted.
	// In that case, it will not cover any distance until the next timestep by convention.
	//
	// Stopping time from time t+tau (braking full strength) on is given as
	// ts = vn/b,
	// with maximal deceleration b and the next step's velocity vn = v(t+dt).
	// the distance covered in [t, t+tau+ts] is then
	// ds = (v*dt + a*dt*dt/2) + vn*(tau-dt) + (vn*ts - b*ts*ts/2).
	// We wish to choose the acceleration a=(vn-v)/dt such that ds == gap (this gives the safe acceleration).
	// Thus, we solve
	// gap = (v*dt + a*dt*dt/2) + vn*(tau-dt) + (vn*ts - b*ts*ts/2)
	//     = (v*dt/2 + vn*dt/2) + vn*(tau-dt) + (vn*vn/b - vn*vn/(2b))
	//     = v*dt/2 + vn*dt/2 + vn*(tau-dt) + vn*vn/(2b)
	// i.e.,
	// 0 = vn*vn + vn*(dt*b + (tau-dt)*2b) + v*dt*b - g*2b
	// giving
	// vn = -(dt*b + 2*b*(tau-dt))/2 +- sqrt( (dt + 2*(tau-dt))^2*b^2/4 + 2b*g - v*dt*b ),
	// The desired speed is the positive root, if it exists. If it does not exist,
	// We find that g - v*dt/2 < 0, i.e., a constant deceleration from v to 0 in the
	// next step would still lead to a positional advance larger than g.
	// This indicates a required stop within [t, t+dt], which is reported as
	// a negative return value for the speed at the next time step (linearly extrapolating the required deceleration).
	// This gives responsibility to the caller to interpret it correctly,
	// e.g. process a stop in the middle of a time step (currently done in MSVehicle::executeMove()).
    headway = headway<0 ? myHeadwayTime : headway;
	SUMOReal b = myDecel;
	SUMOReal dt;
	if(onInsertion) {
		dt = 0; // this assures the correct determination of insertion speed (see above)
	} else {
		dt = STEPS2TIME(DELTA_T);
	}

	// decrease gap slightly (to avoid passing end of lane by values of magnitude ~1e-12, when exact stop is required)
	g = MAX2(0., g - NUMERICAL_EPS);

	// if there is no gap, we just demand to brake as hard as possible
	if(g == 0) return -std::numeric_limits<double>::max();

	SUMOReal D = 2*g - v*dt;
	if(D < 0){
		// deceleration -v/dt (i.e. stopping a the end of the coming timestep)
		// is not sufficient to stop within gap. Therefore a stop has to take place
		// within the timestep. Denoting the corresponding deceleration by d = -(v+vn)/2,
		// we have a corresponding stopping time is t_s = v/d, and a covered distance v^2/(2d).
		// Equating this to g, we find 2gd = v^2, i.e. 2d = -(v+vn) = -v^2/g.
		// Thus vn = -v^2/g + v
		return -v*v/g + v;
	} else {
		assert(myHeadwayTime - dt >= 0.);
		// if myHeadwayTime < dt, there are different options to deal with that.
		// 1) Set myHeadwayTime equal dt. (chosen here)
		// 2) Replace the acceleration phase (chosen as dt) by myHeadwayTime,
		//    then either (i) return the resulting vN (exceeding the appropriate distance, when braking) or
		//    (ii) adapt it to fit the distance that would be covered in the next time step, when accelerating
		//    for time myHeadwayTime and then going on with vN for (dt-myHeadwayTime)
		//    ((ii) exceeds the appropriate next speed, when braking).
		SUMOReal constantSpeedTime = MAX2(headway - dt, 0.);
		SUMOReal p = (dt/2 + constantSpeedTime)*b;
		return -p + sqrt(p*p + D*b);
	}
}


/** Returns the SK-vsafe. */
SUMOReal
MSCFModel::maximumSafeFollowSpeed(SUMOReal gap, SUMOReal egoSpeed, SUMOReal predSpeed, SUMOReal predMaxDecel, bool onInsertion) const {
    // the speed is safe if allows the ego vehicle to come to a stop behind the leader even if
    // the leaders starts braking hard until stopped
    // unfortunately it is not sufficient to compare stopping distances if the follower can brake harder than the leader
    // (the trajectories might intersect before both vehicles are stopped even if the follower has a shorter stopping distance than the leader)
    // To make things safe, we ensure that the leaders brake distance is computed with an deceleration that is at least as high as the follower's.
    // @todo: this is a conservative estimate for safe speed which could be increased


    // if leader is stopped, calculate stopSpeed without time-headway to prevent creeping stop
    // NOTE: this can lead to the strange phenomenon (for the Krauss-model at least) that if the leader comes to a stop,
    //       the follower accelerates for a short period of time. (Leo)
    const SUMOReal headway = predSpeed > 0. ? myHeadwayTime : 0.;
	const SUMOReal x = maximumSafeStopSpeed(gap + brakeGap(predSpeed, MAX2(myDecel, predMaxDecel), 0), egoSpeed, onInsertion, headway);
	assert(x >= 0);
	assert(!ISNAN(x));
	return x;
}


/****************************************************************************/
