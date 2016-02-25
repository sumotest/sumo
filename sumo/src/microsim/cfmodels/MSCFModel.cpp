/****************************************************************************/
/// @file    MSCFModel.cpp
/// @author  Tobias Mayer
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @author  Laura Bieker
/// @date    Mon, 27 Jul 2009
/// @version $Id$
///
// The car-following model abstraction
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


SUMOReal
MSCFModel::brakeGap(const SUMOReal speed, const SUMOReal decel, const SUMOReal headwayTime) {
	if(MSGlobals::gSemiImplicitEulerUpdate){
		/* one possiblity to speed this up is to precalculate speedReduction * steps * (steps+1) / 2
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
MSCFModel::freeSpeed(const SUMOReal decel, const SUMOReal dist, const SUMOReal maxSpeed, const bool onInsertion) {
	if(MSGlobals::gSemiImplicitEulerUpdate){
		// adapt speed to succeeding lane, no reaction time is involved
		// when breaking for y steps the following distance g is covered
		// (drive with v in the final step)
		// g = (y^2 + y) * 0.5 * b + y * v
		// y = ((((sqrt((b + 2.0*v)*(b + 2.0*v) + 8.0*b*g)) - b)*0.5 - v)/b)
		const SUMOReal v = SPEED2DIST(maxSpeed);
		if (dist < v) {
			return maxSpeed;
		}
		const SUMOReal b = ACCEL2DIST(decel);
		const SUMOReal y = MAX2(0.0, ((sqrt((b + 2.0 * v) * (b + 2.0 * v) + 8.0 * b * dist) - b) * 0.5 - v) / b);
		const SUMOReal yFull = floor(y);
		const SUMOReal exactGap = (yFull * yFull + yFull) * 0.5 * b + yFull * v + (y > yFull ? v : 0.0);
		const SUMOReal fullSpeedGain = (yFull + (onInsertion ? 1. : 0.)) * ACCEL2SPEED(decel);
		return DIST2SPEED(MAX2((SUMOReal)0.0, dist - exactGap) / (yFull + 1)) + fullSpeedGain + maxSpeed;
	} else {
		// ballistic update (Leo)
		// calculate maximum speed that is adjustable to maxSpeed after a distance dist and given a maximal deceleration decel
		// we have v(t) = v0 - decel*t == maxSpeed iff t = (v0 - maxSpeed)/decel,
		// hence dist(v0) = v0*t - decel*t*t/2 = v0*(v0 - maxSpeed)/decel - (v0 - maxSpeed)* (v0 - maxSpeed)/(2*decel)
		//				  = 0.5*(v0 + maxSpeed)*(v0 - maxSpeed)/decel = 0.5*(v0*v0 - maxSpeed*maxSpeed)/decel
		// Solving dist == dist(v0), we obtain
		// v0 = sqrt( 2*decel*dist + pow(maxSpeed,2) )
		// XXX: disregarding onInsertion (what's its purpose? Probably needs adjustment)
		return sqrt(2*decel*dist + pow(maxSpeed,2));
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
    const SUMOReal vMin = getSpeedAfterMaxDecel(oldV);
    const SUMOReal vMax = MIN3(veh->getLane()->getVehicleMaxSpeed(veh), maxNextSpeed(oldV, veh), vSafe);
    assert(vMin <= vMax);
    return veh->getLaneChangeModel().patchSpeed(vMin, vMax, vMax, *this);
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
MSCFModel::freeSpeed(const MSVehicle* const /* veh */, SUMOReal /* speed */, SUMOReal seen, SUMOReal maxSpeed, const bool onInsertion) const {
    return freeSpeed(myDecel, seen, maxSpeed, onInsertion);
}


SUMOReal
MSCFModel::insertionFollowSpeed(const MSVehicle* const, SUMOReal speed, SUMOReal gap2pred, SUMOReal predSpeed, SUMOReal predMaxDecel) const {
    return maximumSafeFollowSpeed(gap2pred, speed, predSpeed, predMaxDecel);
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
MSCFModel::maximumSafeStopSpeed(SUMOReal g /*gap*/, SUMOReal v /*currentSpeed*/) const {
	if(MSGlobals::gSemiImplicitEulerUpdate){
		return maximumSafeStopSpeedEuler(g);
	} else {
		return maximumSafeStopSpeedBallistic(g, v);
	}
}


SUMOReal
MSCFModel::maximumSafeStopSpeedEuler(SUMOReal gap) const {
	gap -= NUMERICAL_EPS; // lots of code relies on some slack
	if (gap <= 0) {
		return 0;
	} else if (gap <= ACCEL2SPEED(myDecel)) {
		return gap;
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
MSCFModel::maximumSafeStopSpeedBallistic(SUMOReal g /*gap*/, SUMOReal v /*currentSpeed*/) const {
	// stopping time from time t+dt+tau (braking full strength) on is given as
	// ts = vn/b,
	// with maximal deceleration b and the next step's velocity vn = v(t+dt).
	// the distance covered in [t, t+dt+tau+ts] is then
	// ds = (v*dt + a*dt*dt/2) + vn*tau + (vn*ts - b*ts*ts/2).
	// We wish to choose the acceleration a=(vn-v)/dt such that ds == gap (this gives the safe acceleration).
	// Thus, we solve
	// gap = (v*dt + a*dt*dt/2) + vn*tau + (vn*ts - b*ts*ts/2)
	//     = (v*dt/2 + vn*dt/2) + vn*tau + (vn*vn/b - vn*vn/(2b))
	//     = v*dt/2 + vn*dt/2 + vn*tau + vn*vn/(2b)
	// i.e.,
	// 0 = vn*vn + vn*(dt*b + tau*2b) + v*dt*b - g*2b
	// giving
	// vn = -(dt*b + 2*b*tau)/2 + sqrt( (dt + 2*tau)^2*b^2/4 + 2b*g - v*dt*b ),
	// where the vehicle cannot brake in time if the discriminant is negative, i.e.,
	// v*dt*b - 2b*g >  (dt + 2*tau)^2*b^2/4 (is this a good interpretation?)
	// In that case, we return the minimal possible next velocity ( == v-b*dt)
	// Further, vn can become negative if v > 2g/d, but the discriminant is positive.
	// This indicates a required stop within [t, t+dt], I think.
	// Such a negative return value gives responsibility to the caller to interpret it
	// correctly, e.g. process a stop in the middle of a time step.
	SUMOReal b = myDecel;
	SUMOReal dt = STEPS2TIME(DELTA_T);
	SUMOReal D = pow((dt + 2*myHeadwayTime)*b, 2)/4 + 2*b*g - v*dt*b;
	if(D < 0){
		return v - ACCEL2SPEED(b);
	} else {
		return -(dt/2 + myHeadwayTime)*b + sqrt(D);
	}
}


/** Returns the SK-vsafe. */
SUMOReal
MSCFModel::maximumSafeFollowSpeed(SUMOReal gap, SUMOReal egoSpeed, SUMOReal predSpeed, SUMOReal predMaxDecel) const {
    // the speed is safe if allows the ego vehicle to come to a stop behind the leader even if
    // the leaders starts braking hard until stopped
    // unfortunately it is not sufficient to compare stopping distances if the follower can brake harder than the leader
    // (the trajectories might intersect before both vehicles are stopped even if the follower has a shorter stopping distance than the leader)
    // To make things safe, we ensure that the leaders brake distance is computed with an deceleration that is at least as high as the follower's.
    // @todo: this is a conservative estimate for safe speed which could be increased
	const SUMOReal x = maximumSafeStopSpeed(gap + brakeGap(predSpeed, MAX2(myDecel, predMaxDecel), 0), egoSpeed);
	assert(x >= 0);
	assert(!ISNAN(x));
	return x;
}


/****************************************************************************/
