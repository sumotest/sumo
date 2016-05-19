/****************************************************************************/
/// @file    MSLCM_LC2013.h
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Friedemann Wesner
/// @author  Sascha Krieg
/// @author  Michael Behrisch
/// @date    Fri, 08.10.2013
/// @version $Id$
///
// A lane change model developed by D. Krajzewicz, J. Erdmann et al. between 2004 and 2013
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

#include <iostream>
#include <utils/common/RandHelper.h>
#include <microsim/MSEdge.h>
#include <microsim/MSLane.h>
#include <microsim/MSNet.h>
#include "MSLCM_LC2013.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS

//#define DEBUG_VEHICLE_GUI_SELECTION 1

// ===========================================================================
// variable definitions
// ===========================================================================
// 80km/h will be the threshold for dividing between long/short foresight
#define LOOK_FORWARD_SPEED_DIVIDER (SUMOReal)14.

#define LOOK_FORWARD_RIGHT (SUMOReal)10.
#define LOOK_FORWARD_LEFT  (SUMOReal)20.

#define JAM_FACTOR (SUMOReal)1.

#define LCA_RIGHT_IMPATIENCE (SUMOReal)-1.
#define CUT_IN_LEFT_SPEED_THRESHOLD (SUMOReal)27.

#define LOOK_AHEAD_MIN_SPEED (SUMOReal)0.0
#define LOOK_AHEAD_SPEED_MEMORY (SUMOReal)0.9
#define LOOK_AHEAD_SPEED_DECREMENT 6.

#define HELP_DECEL_FACTOR (SUMOReal)1.0

#define HELP_OVERTAKE  (SUMOReal)(10.0 / 3.6)
#define MIN_FALLBEHIND  (SUMOReal)(14.0 / 3.6)

#define URGENCY (SUMOReal)2.0

#define ROUNDABOUT_DIST_BONUS (SUMOReal)80.0

#define KEEP_RIGHT_TIME (SUMOReal)5.0 // the number of seconds after which a vehicle should move to the right lane
#define KEEP_RIGHT_ACCEPTANCE (SUMOReal)2.0 // calibration factor for determining the desire to keep right

#define RELGAIN_NORMALIZATION_MIN_SPEED (SUMOReal)10.0

// ===========================================================================
// member method definitions
// ===========================================================================
MSLCM_LC2013::MSLCM_LC2013(MSVehicle& v) :
    MSAbstractLaneChangeModel(v, LCM_LC2013),
    mySpeedGainProbability(0),
    myKeepRightProbability(0),
    myLeadingBlockerLength(0),
    myLeftSpace(0),
    myLookAheadSpeed(LOOK_AHEAD_MIN_SPEED),
    myStrategicParam(v.getVehicleType().getParameter().getLCParam(SUMO_ATTR_LCA_STRATEGIC_PARAM, 1)),
    myCooperativeParam(v.getVehicleType().getParameter().getLCParam(SUMO_ATTR_LCA_COOPERATIVE_PARAM, 1)),
    mySpeedGainParam(v.getVehicleType().getParameter().getLCParam(SUMO_ATTR_LCA_SPEEDGAIN_PARAM, 1)),
    myKeepRightParam(v.getVehicleType().getParameter().getLCParam(SUMO_ATTR_LCA_KEEPRIGHT_PARAM, 1)),
    myChangeProbThresholdRight(2.0 * myKeepRightParam / MAX2(NUMERICAL_EPS, mySpeedGainParam)),
    myChangeProbThresholdLeft(0.2 / MAX2(NUMERICAL_EPS, mySpeedGainParam)) {
}

MSLCM_LC2013::~MSLCM_LC2013() {
    changed();
}


int
MSLCM_LC2013::wantsChange(
    int laneOffset,
    MSAbstractLaneChangeModel::MSLCMessager& msgPass,
    int blocked,
    const std::pair<MSVehicle*, SUMOReal>& leader,
    const std::pair<MSVehicle*, SUMOReal>& neighLead,
    const std::pair<MSVehicle*, SUMOReal>& neighFollow,
    const MSLane& neighLane,
    const std::vector<MSVehicle::LaneQ>& preb,
    MSVehicle** lastBlocked,
    MSVehicle** firstBlocked) {
    const int result = _wantsChange(laneOffset, msgPass, blocked, leader, neighLead, neighFollow, neighLane, preb, lastBlocked, firstBlocked);
    return result;
}


SUMOReal
MSLCM_LC2013::patchSpeed(const SUMOReal min, const SUMOReal wanted, const SUMOReal max, const MSCFModel& cfModel) {
    const SUMOReal newSpeed = _patchSpeed(min, wanted, max, cfModel);
    return newSpeed;
}


SUMOReal
MSLCM_LC2013::_patchSpeed(const SUMOReal min, const SUMOReal wanted, const SUMOReal max, const MSCFModel& cfModel) {
    int state = myOwnState;
    //gDebugFlag2 = (myVehicle.getID() == "XXI_Aprile_1_452");
    if (gDebugFlag2) {
        std::cout << SIMTIME << " patchSpeed state=" << state << " myVSafes=" << toString(myVSafes) << "\n";
    }

    // Debug (Leo)
    gDebugFlag1 = myVehicle.getID() == "type1.40";
    gDebugFlag1 = false;
    if(gDebugFlag1){
    std::cout << "patchSpeed() at time" <<STEPS2TIME(MSNet::getInstance()->getCurrentTimeStep())
              << " \nveh=" << myVehicle.getID()
//              << " \nlane=" << myVehicle.getLane()->getID()
//              << " \npos=" << myVehicle.getPositionOnLane()
              << " \nv=" << myVehicle.getSpeed()
              << " min=" << min
              << " wanted=" << wanted<< std::endl;
    }


    // letting vehicles merge in at the end of the lane in case of counter-lane change, step#2
    SUMOReal MAGIC_offset = 1.;
    //   if we want to change and have a blocking leader and there is enough room for him in front of us
    if (myLeadingBlockerLength != 0) {
//        // Debug (Leo)
//        if(gDebugFlag1){
//        	std::cout << "myLeadingBlockerLength != 0" << std::endl;
//        }
        SUMOReal space = myLeftSpace - myLeadingBlockerLength - MAGIC_offset - myVehicle.getVehicleType().getMinGap();

//        // Debug (Leo)
//        if(gDebugFlag1){
//        	std::cout << "space = " << space << std::endl;
//        }

        if (space > 0) {
            // compute speed for decelerating towards a place which allows the blocking leader to merge in in front
            SUMOReal safe = cfModel.stopSpeed(&myVehicle, myVehicle.getSpeed(), space);
            // if we are approaching this place
            if (safe < wanted) {
                // return this speed as the speed to use
                return MAX2(min, safe);
            }
        }
    }

    SUMOReal nVSafe = wanted;
    bool gotOne = false;
    for (std::vector<SUMOReal>::const_iterator i = myVSafes.begin(); i != myVSafes.end(); ++i) {
        SUMOReal v = (*i);

        if(MSGlobals::gSemiImplicitEulerUpdate){
            if (v >= min && v <= max) {
            	nVSafe = MIN2(v * myCooperativeParam + (1 - myCooperativeParam) * wanted, nVSafe);
                gotOne = true;
            }
        } else {
            // ballistic update: (negative speeds may appear, e.g. min<0, v<0), BUT:
            // XXX: LaneChanging returns -1 to indicate no restrictions, which leads to probs here (Leo)
        	//      As a quick fix, we just dismiss cases where v=-1
        	//      Very rarely (whenever a requested help-acceleration is really indicated by v=-1)
        	//      this can lead to failing lane-change attempts, though)
            if (v >= min && v!=-1 && v <= max) {
//        	if (v >= MAX2(min,0.) && v <= max) {
            	nVSafe = MIN2(v * myCooperativeParam + (1 - myCooperativeParam) * wanted, nVSafe);
                gotOne = true;
            }
        }

//        // Debug (Leo)
//        if(gDebugFlag1){
//        	std::cout << "nVSafe = " << nVSafe << std::endl;
//        }
    }

    if (gotOne && !myDontBrake) {
//        // Debug (Leo)
//        if(gDebugFlag1){
//        	std::cout << "returning here" << std::endl;
//        }
        return nVSafe;
    }

    // check whether the vehicle is blocked
    if ((state & LCA_WANTS_LANECHANGE) != 0 && (state & LCA_BLOCKED) != 0) {
        if ((state & LCA_STRATEGIC) != 0) {
            // necessary decelerations are controlled via vSafe. If there are
            // none it means we should speed up
        	// XXX: It seems that this could lead to returning a value > wanted (==vSafe, if we use Krauss)
        	//      (at least if myDontBrake==true and max>wanted. However, the latter is never true when using Krauss,
        	//       so this didn't expose itself, yet) (Leo)
            return (max + wanted) / (SUMOReal) 2.0;
        } else if ((state & LCA_COOPERATIVE) != 0) {
            // only minor adjustments in speed should be done
            if ((state & LCA_BLOCKED_BY_LEADER) != 0) {
                return (min + wanted) / (SUMOReal) 2.0;
            }
            if ((state & LCA_BLOCKED_BY_FOLLOWER) != 0) {
                return (max + wanted) / (SUMOReal) 2.0;
            }
        }
    }

    // accelerate if being a blocking leader or blocking follower not able to brake
    //  (and does not have to change lanes)
    if ((state & LCA_AMBLOCKINGLEADER) != 0) {
        return (max + wanted) / (SUMOReal) 2.0;
    }

    if ((state & LCA_AMBLOCKINGFOLLOWER_DONTBRAKE) != 0) {
    }
    if (myVehicle.getLane()->getEdge().getLanes().size() == 1) {
        // remove changing information if on a road with a single lane
        changed();
    }
    // Debug (Leo)
    gDebugFlag1=false;
    return wanted;
}


void*
MSLCM_LC2013::inform(void* info, MSVehicle* sender) {

	//	Debug (Leo)
	gDebugFlag1 = sender->getID() == "type1.54";
	if(gDebugFlag1) std::cout << "vehicle " << myVehicle.getID() << " informed by " << sender->getID() << std::endl;

    Info* pinfo = (Info*) info;

    // Debug (Leo)
	if(gDebugFlag1) std::cout << "pushing vSafe " << pinfo->first << std::endl;

    myVSafes.push_back(pinfo->first);
    myOwnState |= pinfo->second;
    //gDebugFlag2 = (myVehicle.getID() == "XXI_Aprile_1_452");
    if (gDebugFlag2) {
        std::cout << SIMTIME << " informed by " << sender->getID() << " speed=" << pinfo->first << " state=" << pinfo->second << "\n";
    }
    delete pinfo;
    gDebugFlag1 = false;
    return (void*) true;
}


SUMOReal
MSLCM_LC2013::estimateOvertakeTime(const MSVehicle* v1, const MSVehicle* v2, SUMOReal overtakeDist, SUMOReal remainingSeconds) {
	SUMOReal overtakeTime;
	// estimate overtakeTime (calculating with constant acceleration accelX)
	// TODO: for congested traffic the estimated mean velocity should also be taken into account (instead of max speed)
	const SUMOReal accelEgo = 0.5*SPEED2ACCEL(v1->getSpeed() - v1->getPreviousSpeed()) + 0.3; // the + 0.3 let's vehicles proceed alongside a waiting line
	const SUMOReal accelLead = 0.9*MAX2(0., SPEED2ACCEL(v2->getSpeed() - v2->getPreviousSpeed()));
	// times until reaching maximal or minimal velocities
	const SUMOReal tmEgo = accelEgo != 0 ?
			(accelEgo > 0 ? (v1->getMaxSpeed() - v1->getSpeed())/accelEgo : -v1->getSpeed()/accelEgo)
			: remainingSeconds + 10000;
	const SUMOReal tmLead = accelLead != 0 ?
			(accelLead > 0 ? (v2->getMaxSpeed() - v2->getSpeed())/accelLead : -v2->getSpeed()/accelLead)
			: remainingSeconds + 10000;
	// initial speed difference
	const SUMOReal dv0 = v1->getSpeed() - v2->getSpeed();
	// non-smooth points for the gap-evolution
	const SUMOReal t1 = MIN2(tmEgo, tmLead), t2 = MAX2(tmEgo, tmLead);

	// distance covered until t1, t2
	SUMOReal d1, d2;
	// accel in [0,t1]
	const SUMOReal a1 = accelEgo - accelLead;
	// accel in [t1,t2]
	const SUMOReal a2 = tmEgo == t1 ? - accelLead : accelEgo;
	// speed differences at t1, t2
	const SUMOReal dv1= dv0 + a1*t1, dv2= dv1 + a2*(t2-t1);

	// flag to indicate overtake success
	bool overtaken = false;
	if(a1 < 0 && dv0 >0){
		// there's a maximum at tmax = -dv0/a1.
		const SUMOReal tmax = -dv0/a1;
		const SUMOReal dmax = dv0*tmax + tmax*tmax*a1/2.;
    	if(dmax > overtakeDist) {
    		// solve for smaller root: overtakeDist = dv0*t + t*t*a1/2
    		overtakeTime = -dv0/a1 - sqrt((dv0*dv0/a1 + 2*overtakeDist)/a1);
    	} else {
    		overtakeTime = t1+1.; // indicate no overtaking until t1
    	}
	} else if(a1 < 0 && dv0 < 0){
		// no solution until t1
		overtakeTime = t1+1.; // indicate no overtaking until t1
	} else {
		// there's a single positive solution
		overtakeTime = -dv0/a1 + sqrt((dv0*dv0/a1 + 2*overtakeDist)/a1);
	}

	if(overtakeTime <= t1){
		overtaken = true;
	} else {
		// distance after t1
		d1 = dv0*t1 + t1*t1*a1/2.;
	}

	if(!overtaken) {
		// until time t1, overtaking didn't succeed
		if(t1 > remainingSeconds || d1 > myLeftSpace) {
			// -> set overtakeTime to something indicating impossibility of overtaking
			overtakeTime = remainingSeconds + 1;
		} else {
        	if(a2 < 0 && dv1 >0){
        		// there's a maximum at t1 + tmax = t1 - dv1/a2.
        		const SUMOReal tmax = -dv1/a2;
        		const SUMOReal dmax = dv1*tmax + tmax*tmax*a2/2.;
            	if(dmax > overtakeDist - d1) {
            		// solve for smaller root: overtakeDist - d1 = dv1*t + t*t*a2/2
            		overtakeTime = t1 - dv1/a2 - sqrt((dv1*dv1/a2 + 2*(overtakeDist - d1))/a2);
            	} else {
            		overtakeTime = t2 + 1.; // indicate no overtaking until t2
            	}
        	} else if(a2 < 0 && dv1 < 0){
        		// no solution until t2
        		overtakeTime = t2+1.; // indicate no overtaking until t2
        	} else {
        		// there's a single positive solution
        		overtakeTime = t1 - dv1/a2 + sqrt((dv1*dv1/a2 + 2*(overtakeDist - d1))/a2);
        	}
        	if(overtakeTime <= t2){
        		overtaken = true;
        	} else {
        		// distance after t2
        		d2 = d1 + dv1*(t2-t1) + (t2-t1)*(t2-t1)*a2/2.;
        	}
		}
	}

	if(!overtaken){
		// no overtaking until both reach stationary velocities
		if(t2 > remainingSeconds || d2 > myLeftSpace || dv2 <= 0){
			overtakeTime = remainingSeconds + 1;
		} else {
			overtakeTime = t2 + (overtakeDist - d2)/dv2;
		}
	}

	return overtakeTime;
}


SUMOReal
MSLCM_LC2013::overtakeDistance(const MSVehicle* follower, const MSVehicle* leader, SUMOReal gap) {
	SUMOReal overtakeDist = (gap // drive to back of leader
			+ leader->getVehicleType().getLengthWithGap() // drive to front of leader
			+ follower->getVehicleType().getLength() // follower back reaches leader front
			+ leader->getCarFollowModel().getSecureGap( // save gap to leader
					leader->getSpeed(), follower->getSpeed(), follower->getCarFollowModel().getMaxDecel()));
	return MAX2(overtakeDist, 0.);
}

SUMOReal
MSLCM_LC2013::informLeader(MSAbstractLaneChangeModel::MSLCMessager& msgPass,
                           int blocked,
                           int dir,
                           const std::pair<MSVehicle*, SUMOReal>& neighLead,
                           SUMOReal remainingSeconds) {

    SUMOReal plannedSpeed = MIN2(myVehicle.getSpeed(),
                                 myVehicle.getCarFollowModel().stopSpeed(&myVehicle, myVehicle.getSpeed(), myLeftSpace - myLeadingBlockerLength));
    for (std::vector<SUMOReal>::const_iterator i = myVSafes.begin(); i != myVSafes.end(); ++i) {
        SUMOReal v = (*i);
        if (v >= myVehicle.getSpeed() - ACCEL2SPEED(myVehicle.getCarFollowModel().getMaxDecel())) {
            plannedSpeed = MIN2(plannedSpeed, v);
        }
    }
    if ((blocked & LCA_BLOCKED_BY_LEADER) != 0) {
        assert(neighLead.first != 0);
        MSVehicle* nv = neighLead.first;

        // decide whether we want to overtake the leader or follow it
        SUMOReal overtakeTime;
        const SUMOReal overtakeDist = overtakeDistance(&myVehicle, nv, neighLead.second);
        if(MSGlobals::gSemiImplicitEulerUpdate){
            const SUMOReal dv = plannedSpeed - nv->getSpeed();
        	if(dv > 0){
        		overtakeTime = overtakeDist/dv;
        	} else {
        		// -> set overtakeTime to something indicating impossibility of overtaking
        		overtakeTime = remainingSeconds + 1;
        	}
        } else {
        	overtakeTime = estimateOvertakeTime(&myVehicle, nv, overtakeDist, remainingSeconds);
        }

        // Debug (Leo)
        gDebugFlag2 = myVehicle.getID() == "type1.54";
        if (gDebugFlag2) {
        	std::cout << SIMTIME << " informLeader() of " << myVehicle.getID()
        			<< "\nnv = " << nv->getID()
        			<< "\nplannedSpeed = " << plannedSpeed
        			<< "\nleaderSpeed = " << nv->getSpeed()
        			<< "\nmyLeftSpace = " << myLeftSpace
        			<< "\nremainingSeconds = " << remainingSeconds
        			<< "\novertakeDist = " << overtakeDist
        			<< "\novertakeTime = " << overtakeTime
        			<< std::endl;
        }

        if (// overtaking on the right on an uncongested highway is forbidden (noOvertakeLCLeft)
			(dir == LCA_MLEFT && !myVehicle.congested() && !myAllowOvertakingRight)
			// not enough space to overtake?
			|| myLeftSpace < overtakeDist
			// not enough time to overtake?
			|| remainingSeconds < overtakeTime) {

            // Debug (Leo)
            if(gDebugFlag2) std::cout << "no overtaking" << std::endl;

            // cannot overtake
            msgPass.informNeighLeader(new Info(-1, dir | LCA_AMBLOCKINGLEADER), &myVehicle); // XXX: using -1 is ambiguous! (Leo)
            // slow down smoothly to follow leader
            const SUMOReal targetSpeed = myCarFollowModel.followSpeed(
                                             &myVehicle, myVehicle.getSpeed(), neighLead.second, nv->getSpeed(), nv->getCarFollowModel().getMaxDecel());
            if (targetSpeed < myVehicle.getSpeed()) {
                // slow down smoothly to follow leader
                const SUMOReal decel = ACCEL2SPEED(MIN2(myVehicle.getCarFollowModel().getMaxDecel(),
                                                        MAX2(MIN_FALLBEHIND, (myVehicle.getSpeed() - targetSpeed) / remainingSeconds)));
                const SUMOReal nextSpeed = MIN2(plannedSpeed, myVehicle.getSpeed() - decel);
                myVSafes.push_back(nextSpeed);
                return nextSpeed;
            } else {
                // leader is fast enough anyway
                myVSafes.push_back(targetSpeed);
                return plannedSpeed;
            }
        } else {
            // overtaking, leader should not accelerate

            // Debug (Leo)
            if(gDebugFlag1) std::cout << "vehicle " << myVehicle.getID() << "informing NeighLeader (overtaking) " << std::endl;

            msgPass.informNeighLeader(new Info(nv->getSpeed(), dir | LCA_AMBLOCKINGLEADER), &myVehicle);
            return -1;  // XXX: using -1 is ambiguous! (Leo)
        }
    } else if (neighLead.first != 0) { // (remainUnblocked)
        // we are not blocked now. make sure we stay far enough from the leader
        MSVehicle* nv = neighLead.first;
        const SUMOReal nextNVSpeed = nv->getSpeed() - HELP_OVERTAKE; // conservative
        const SUMOReal dv = SPEED2DIST(myVehicle.getSpeed() - nextNVSpeed);
        const SUMOReal targetSpeed = myCarFollowModel.followSpeed(
                                         &myVehicle, myVehicle.getSpeed(), neighLead.second - dv, nextNVSpeed, nv->getCarFollowModel().getMaxDecel());
        myVSafes.push_back(targetSpeed);
        return MIN2(targetSpeed, plannedSpeed);
    } else {
        // not overtaking
        return plannedSpeed;
    }

}


void
MSLCM_LC2013::informFollower(MSAbstractLaneChangeModel::MSLCMessager& msgPass,
                             int blocked,
                             int dir,
                             const std::pair<MSVehicle*, SUMOReal>& neighFollow,
                             SUMOReal remainingSeconds,
                             SUMOReal plannedSpeed) {
    if ((blocked & LCA_BLOCKED_BY_FOLLOWER) != 0) {
        assert(neighFollow.first != 0);
        MSVehicle* nv = neighFollow.first;

        // Debug (Leo)
        gDebugFlag2 = myVehicle.getID() == "type1.54";
        if (gDebugFlag2) {
            std::cout << SIMTIME << " informFollower() of " << myVehicle.getID() << "\nnv = " << nv->getID() << std::endl;
        }

        // are we fast enough to cut in without any help?
        if (plannedSpeed - nv->getSpeed() >= HELP_OVERTAKE) {
            const SUMOReal neededGap = nv->getCarFollowModel().getSecureGap(nv->getSpeed(), plannedSpeed, myVehicle.getCarFollowModel().getMaxDecel());
            if ((neededGap - neighFollow.second) / remainingSeconds < (plannedSpeed - nv->getSpeed())) {
                // follower might even accelerate but not to much
                msgPass.informNeighFollower(new Info(plannedSpeed - HELP_OVERTAKE, dir | LCA_AMBLOCKINGFOLLOWER), &myVehicle);
                return;
            }
            // Debug (Leo)
            if (gDebugFlag2) {
                std::cout << "1" << std::endl;
            }
        }
        // decide whether we will request help to cut in before the follower or allow to be overtaken

        // PARAMETERS
        // assume other vehicle will assume the equivalent of 1 second of
        // maximum deceleration to help us (will probably be spread over
        // multiple seconds)
        // -----------
        const SUMOReal helpDecel = nv->getCarFollowModel().getMaxDecel() * HELP_DECEL_FACTOR ;

        // follower's new speed in next step
        SUMOReal neighNewSpeed;
        // follower's new speed after 1s. (resp. followerBreakTime)
        SUMOReal neighNewSpeed1s;
    	const SUMOReal plannedAccel = SPEED2ACCEL(plannedSpeed - myVehicle.getSpeed());
        SUMOReal dv, decelGap;

        // (ballistic) breaking times of follower (tf) and leader (tl) till coming to a v=0
        // have to be taken into account for calculation of possible gap after some possibly longer time.
    	SUMOReal tf, tl;

        if(MSGlobals::gSemiImplicitEulerUpdate){
        	// euler
            neighNewSpeed = MAX2((SUMOReal)0, nv->getSpeed() - ACCEL2SPEED(helpDecel));
            neighNewSpeed1s = MAX2((SUMOReal)0, nv->getSpeed() - helpDecel);
            // change in the gap between ego and blocker over 1 second (not STEP!) XXX: though here it is calculated as if it were one step!? (Leo)
            dv = plannedSpeed - neighNewSpeed1s;
        	// new gap between follower and self in case the follower does brake for 1s
            // XXX: if the step-length is not 1s., this is not the gap after 1s. deceleration (Leo)
            // XXX: Below, it seems that decelGap > secureGap is taken to indicate the possibility
            //      to cut in within the next time-step. However, this is not the case, if TS<1s.!
        	decelGap = neighFollow.second + dv;

        } else {
        	// ballistic
        	// negative newSpeed-extrapolation possible, if stop lies within the next time-step
            neighNewSpeed = nv->getSpeed() - ACCEL2SPEED(helpDecel);
            neighNewSpeed1s = nv->getSpeed() - helpDecel;

        	// the follower's breaktime investment for helping (too large values might
        	// corrupt the below assumption on constant acceleration - see NOTE)
        	const SUMOReal followerBreakTime = 1.; // NOTE: below, we assume: followerBreakTime > TS (when taking tl=MAX2(tl,TS))
        	dv = myVehicle.getSpeed()- nv->getSpeed();

        	// determine times tf, tl for which vehicle can break until stop (within followerBreakTime)
        	if(helpDecel > 0 && nv->getSpeed() > 0){
        		const SUMOReal followerStopTime = nv->getSpeed()*nv->getSpeed()/(2*helpDecel);
        		tf = MIN2(followerStopTime, followerBreakTime);
        	} else if (nv->getSpeed() == 0) {
        		tf = 0;
        	} else if (helpDecel <= 0){
        		tf = followerBreakTime;
        	}

        	if(plannedAccel < 0 && myVehicle.getSpeed() > 0){
        		const SUMOReal leaderStopTime =  - myVehicle.getSpeed()*myVehicle.getSpeed()/(2*plannedAccel);
        		tl = MIN2(leaderStopTime, followerBreakTime);
        	} else if (plannedAccel <= 0 && myVehicle.getSpeed()==0) {
        		tl = 0;
        	} else if (plannedAccel >= 0) {
        		tl = followerBreakTime;
        	}

        	// NOTE: this assumes that the planned acceleration is performed over the next followerBreakTime seconds
        	decelGap = neighFollow.second + dv*MAX2(tf,tl) + (tf*tf*helpDecel + tl*tl*plannedAccel)/2.;
        }

        // same line for euler and ballistic
    	const SUMOReal secureGap = nv->getCarFollowModel().getSecureGap(MAX2(neighNewSpeed1s,0.), plannedSpeed, myVehicle.getCarFollowModel().getMaxDecel());

    	// Debug (Leo)
    	if (gDebugFlag2) {
    		std::cout
    		<< "plannedSpeed = " << plannedSpeed
    		<< " mySpeed = " << myVehicle.getSpeed()
    		<< " dv = " << dv
    		<< " gap = " << neighFollow.second
    		<< " decelGap = " << decelGap
    		<< " tf = " << tf
    		<< " tl = " << tl
    		<< " secureGap = " << secureGap
    		<< std::endl;
    	}


        if (decelGap > 0 && decelGap >= secureGap) {
        	// XXX: This does not assure that the leader can cut in in the next step if TS < 1,
        	//      this seems to be supposed in the following...?!

            // if the blocking neighbor brakes it could help
            // how hard does it actually need to be?
            // to be safe in the next step the following equation has to hold for the follower's vsafe:
            //   vsafe <= followSpeed(gap=currentGap - SPEED2DIST(vsafe), ...)
        	SUMOReal vsafe;

        	if(MSGlobals::gSemiImplicitEulerUpdate){
        		// euler
                // we compute an upper bound on vsafe by doing the computation twice
				const SUMOReal vsafe1 = MAX2(neighNewSpeed, nv->getCarFollowModel().followSpeed(
												 nv, nv->getSpeed(), neighFollow.second + SPEED2DIST(plannedSpeed), plannedSpeed, myVehicle.getCarFollowModel().getMaxDecel()));
				vsafe = MAX2(neighNewSpeed, nv->getCarFollowModel().followSpeed(
												nv, nv->getSpeed(), neighFollow.second + SPEED2DIST(plannedSpeed - vsafe1), plannedSpeed, myVehicle.getCarFollowModel().getMaxDecel()));
				assert(vsafe <= vsafe1);
        	} else {
        		// ballistic
                // we compute an upper bound on vsafe
        		// adjust tl to TS instead of followerBreakTime
            	tl = MIN2(tl,TS);
        		// gap after next time step (assuming no deceleration for follower) => small gap
        		const SUMOReal gap1 = neighFollow.second + dv*tl + (tl*tl*plannedAccel)/2.;
        		// safe speed when this gap is assumed (=> small vsafe)
				const SUMOReal vsafe1 = MAX2(neighNewSpeed, nv->getCarFollowModel().followSpeed(
												 nv, nv->getSpeed(), gap1, plannedSpeed, myVehicle.getCarFollowModel().getMaxDecel()));
        		// gap in next step (assuming vnext=vsafe1 for follower)
				const SUMOReal helpDecel2 = -SPEED2ACCEL(vsafe1-nv->getSpeed());
				if (helpDecel2 > 0){
					tf = MIN2(nv->getSpeed()*nv->getSpeed()/(2*helpDecel2), TS);
				} else {
					tf = TS;
				}
				const SUMOReal gap2 = neighFollow.second + dv*MAX2(tl,tf) + (tf*tf*helpDecel2 + tl*tl*plannedAccel)/2.;
				// safe speed assuming larger gap (=> larger vsafe)
				vsafe = MAX2(neighNewSpeed, nv->getCarFollowModel().followSpeed(
												nv, nv->getSpeed(), gap2, plannedSpeed, myVehicle.getCarFollowModel().getMaxDecel()));
				assert(vsafe <= vsafe1);

		        // Debug (Leo)
	            if (gDebugFlag2) {
	                std::cout << "trying to cut in before follower in next step."
	                		<< " \nRequesting follower vsafe: " << vsafe
	                		<< std::endl;
	            }
        	}
        	msgPass.informNeighFollower(new Info(vsafe, dir | LCA_AMBLOCKINGFOLLOWER), &myVehicle);
        } else if (MSGlobals::gSemiImplicitEulerUpdate && dv > 0 && dv * remainingSeconds > (secureGap - decelGap + POSITION_EPS)) {
        	// euler
        	// NOTE: This case corresponds to the situation, where some time is left to perform the lc
        	// decelerating once is sufficient to open up a large enough gap in time
        	// XXX: decelerating *once* does not necessarily lead to the gap decelGap! (if TS<1s.)
        	msgPass.informNeighFollower(new Info(neighNewSpeed, dir | LCA_AMBLOCKINGFOLLOWER), &myVehicle);
        } else if (!MSGlobals::gSemiImplicitEulerUpdate && dv > 0 && dv * remainingSeconds > secureGap + POSITION_EPS) {
        	// ballistic
        	// NOTE: This case corresponds to the situation, where some time is left to perform the lc
        	msgPass.informNeighFollower(new Info(nv->getSpeed(), dir | LCA_AMBLOCKINGFOLLOWER), &myVehicle);
        } else {
            SUMOReal vhelp = MAX2(nv->getSpeed(), myVehicle.getSpeed() + HELP_OVERTAKE);
            //if (dir == LCA_MRIGHT && myVehicle.getWaitingSeconds() > LCA_RIGHT_IMPATIENCE &&
            //        nv->getSpeed() > myVehicle.getSpeed()) {
            if (nv->getSpeed() > myVehicle.getSpeed() &&
                    ((dir == LCA_MRIGHT && myVehicle.getWaitingSeconds() > LCA_RIGHT_IMPATIENCE)
                     || (dir == LCA_MLEFT && plannedSpeed > CUT_IN_LEFT_SPEED_THRESHOLD) // VARIANT_22 (slowDownLeft)
                    )) {
                // let the follower slow down to increase the likelyhood that later vehicles will be slow enough to help
                // follower should still be fast enough to open a gap
                vhelp = MAX2(neighNewSpeed, myVehicle.getSpeed() + HELP_OVERTAKE);
                if ((nv->getSpeed() - myVehicle.getSpeed()) / helpDecel < remainingSeconds) {
                    msgPass.informNeighFollower(new Info(vhelp, dir | LCA_AMBLOCKINGFOLLOWER), &myVehicle);
                    return;
                }
            }
            msgPass.informNeighFollower(new Info(vhelp, dir | LCA_AMBLOCKINGFOLLOWER), &myVehicle);
            // this follower is supposed to overtake us. slow down smoothly to allow this
            const SUMOReal overtakeDist = (neighFollow.second // follower reaches ego back
                                           + myVehicle.getVehicleType().getLengthWithGap() // follower reaches ego front
                                           + nv->getVehicleType().getLength() // follower back at ego front
                                           + myVehicle.getCarFollowModel().getSecureGap( // follower has safe dist to ego
                                               plannedSpeed, vhelp, nv->getCarFollowModel().getMaxDecel()));
            // speed difference to create a sufficiently large gap
            const SUMOReal needDV = overtakeDist / remainingSeconds;
            // make sure the deceleration is not to strong
            myVSafes.push_back(MAX2(vhelp - needDV, myVehicle.getSpeed() - ACCEL2SPEED(myVehicle.getCarFollowModel().getMaxDecel())));

	        // Debug (Leo)
            if (gDebugFlag2) {
                std::cout << "trying to merge in after follower."
                		<< " \nPushing own vsafe: " << myVSafes.back()
                		<< std::endl;
            }
        }
    }
    // Debug (Leo)
    gDebugFlag2 = false;
}


void
MSLCM_LC2013::prepareStep() {
    // keep information about strategic change direction
    myOwnState = (myOwnState & LCA_STRATEGIC) ? (myOwnState & LCA_WANTS_LANECHANGE) : 0;
    myLeadingBlockerLength = 0;
    myLeftSpace = 0;
    myVSafes.clear();
    myDontBrake = false;
    // truncate to work around numerical instability between different builds
    mySpeedGainProbability = ceil(mySpeedGainProbability * 100000.0) * 0.00001;
    myKeepRightProbability = ceil(myKeepRightProbability * 100000.0) * 0.00001;
}


void
MSLCM_LC2013::changed() {
    myOwnState = 0;
    mySpeedGainProbability = 0;
    myKeepRightProbability = 0;
    if (myVehicle.getBestLaneOffset() == 0) {
        // if we are not yet on our best lane there might still be unseen blockers
        // (during patchSpeed)
        myLeadingBlockerLength = 0;
        myLeftSpace = 0;
    }
    myLookAheadSpeed = LOOK_AHEAD_MIN_SPEED;
    myVSafes.clear();
    myDontBrake = false;
}


int
MSLCM_LC2013::_wantsChange(
    int laneOffset,
    MSAbstractLaneChangeModel::MSLCMessager& msgPass,
    int blocked,
    const std::pair<MSVehicle*, SUMOReal>& leader,
    const std::pair<MSVehicle*, SUMOReal>& neighLead,
    const std::pair<MSVehicle*, SUMOReal>& neighFollow,
    const MSLane& neighLane,
    const std::vector<MSVehicle::LaneQ>& preb,
    MSVehicle** lastBlocked,
    MSVehicle** firstBlocked) {
    assert(laneOffset == 1 || laneOffset == -1);
    const SUMOTime currentTime = MSNet::getInstance()->getCurrentTimeStep();
    // compute bestLaneOffset
    MSVehicle::LaneQ curr, neigh, best;
    int bestLaneOffset = 0;
    SUMOReal currentDist = 0;
    SUMOReal neighDist = 0;
    int currIdx = 0;
    MSLane* prebLane = myVehicle.getLane();
    if (prebLane->getEdge().getPurpose() == MSEdge::EDGEFUNCTION_INTERNAL) {
        // internal edges are not kept inside the bestLanes structure
        prebLane = prebLane->getLinkCont()[0]->getLane();
    }
    const bool checkOpposite = &neighLane.getEdge() != &myVehicle.getLane()->getEdge();
    const int prebOffset = (checkOpposite ? 0 : laneOffset);
    for (int p = 0; p < (int) preb.size(); ++p) {
        if (preb[p].lane == prebLane && p + laneOffset >= 0) {
            assert(p + prebOffset < (int)preb.size());
            curr = preb[p];
            neigh = preb[p + prebOffset];
            currentDist = curr.length;
            neighDist = neigh.length;
            bestLaneOffset = curr.bestLaneOffset;
            if (bestLaneOffset == 0 && preb[p + prebOffset].bestLaneOffset == 0) {
                bestLaneOffset = prebOffset;
            }
            best = preb[p + bestLaneOffset];
            currIdx = p;
            break;
        }
    }
    // direction specific constants
    const bool right = (laneOffset == -1);
    if (isOpposite() && right) {
        neigh = preb[preb.size() - 1];
        curr = neigh;
        bestLaneOffset = -1;
        curr.bestLaneOffset = -1;
        neighDist = neigh.length;
        currentDist = curr.length;
    }
    const SUMOReal posOnLane = isOpposite() ? myVehicle.getLane()->getLength() - myVehicle.getPositionOnLane() : myVehicle.getPositionOnLane();
    const int lca = (right ? LCA_RIGHT : LCA_LEFT);
    const int myLca = (right ? LCA_MRIGHT : LCA_MLEFT);
    const int lcaCounter = (right ? LCA_LEFT : LCA_RIGHT);
    const bool changeToBest = (right && bestLaneOffset < 0) || (!right && bestLaneOffset > 0);
    // keep information about being a leader/follower
    int ret = (myOwnState & 0xffff0000);
    int req = 0; // the request to change or stay

    ret = slowDownForBlocked(lastBlocked, ret);
    if (lastBlocked != firstBlocked) {
        ret = slowDownForBlocked(firstBlocked, ret);
    }

    //gDebugFlag2 = (myVehicle.getID() == "XXI_Aprile_1_452");
    if (gDebugFlag2) std::cout << SIMTIME
                                   << " veh=" << myVehicle.getID()
                                   << " _wantsChange state=" << myOwnState
                                   << " myVSafes=" << toString(myVSafes)
                                   << " firstBlocked=" << Named::getIDSecure(*firstBlocked)
                                   << " lastBlocked=" << Named::getIDSecure(*lastBlocked)
                                   << " leader=" << Named::getIDSecure(leader.first)
                                   << " leaderGap=" << leader.second
                                   << " neighLead=" << Named::getIDSecure(neighLead.first)
                                   << " neighLeadGap=" << neighLead.second
                                   << " neighFollow=" << Named::getIDSecure(neighFollow.first)
                                   << " neighFollowGap=" << neighFollow.second
                                   << "\n";

    // we try to estimate the distance which is necessary to get on a lane
    //  we have to get on in order to keep our route
    // we assume we need something that depends on our velocity
    // and compare this with the free space on our wished lane
    //
    // if the free space is somehow less than the space we need, we should
    //  definitely try to get to the desired lane
    //
    // this rule forces our vehicle to change the lane if a lane changing is necessary soon
    // lookAheadDistance:
    // we do not want the lookahead distance to change all the time so we discrectize the speed a bit

    if (myVehicle.getSpeed() > myLookAheadSpeed) {
        myLookAheadSpeed = myVehicle.getSpeed();
    } else {
        myLookAheadSpeed = MAX2(LOOK_AHEAD_MIN_SPEED,
                                (LOOK_AHEAD_SPEED_MEMORY * myLookAheadSpeed + (1 - LOOK_AHEAD_SPEED_MEMORY) * myVehicle.getSpeed()));
    }
    SUMOReal laDist = myLookAheadSpeed * (right ? LOOK_FORWARD_RIGHT : LOOK_FORWARD_LEFT) * myStrategicParam;
    laDist += myVehicle.getVehicleType().getLengthWithGap() * (SUMOReal) 2.;

    // react to a stopped leader on the current lane
    if (bestLaneOffset == 0 && leader.first != 0 && leader.first->isStopped()) {
        // value is doubled for the check since we change back and forth
        laDist = 0.5 * (myVehicle.getVehicleType().getLengthWithGap()
                        + leader.first->getVehicleType().getLengthWithGap());
    }

    // free space that is available for changing
    //const SUMOReal neighSpeed = (neighLead.first != 0 ? neighLead.first->getSpeed() :
    //        neighFollow.first != 0 ? neighFollow.first->getSpeed() :
    //        best.lane->getSpeedLimit());
    // @note: while this lets vehicles change earlier into the correct direction
    // it also makes the vehicles more "selfish" and prevents changes which are necessary to help others

    int roundaboutEdgesAhead = 0;
    for (std::vector<MSLane*>::iterator it = curr.bestContinuations.begin(); it != curr.bestContinuations.end(); ++it) {
        if ((*it) != 0 && (*it)->getEdge().isRoundabout()) {
            roundaboutEdgesAhead += 1;
        } else if (roundaboutEdgesAhead > 0) {
            // only check the next roundabout
            break;
        }
    }
    int roundaboutEdgesAheadNeigh = 0;
    for (std::vector<MSLane*>::iterator it = neigh.bestContinuations.begin(); it != neigh.bestContinuations.end(); ++it) {
        if ((*it) != 0 && (*it)->getEdge().isRoundabout()) {
            roundaboutEdgesAheadNeigh += 1;
        } else if (roundaboutEdgesAheadNeigh > 0) {
            // only check the next roundabout
            break;
        }
    }
    if (roundaboutEdgesAhead > 1) {
        currentDist += roundaboutEdgesAhead * ROUNDABOUT_DIST_BONUS * myCooperativeParam;
        neighDist += roundaboutEdgesAheadNeigh * ROUNDABOUT_DIST_BONUS * myCooperativeParam;
    }

    const SUMOReal usableDist = (currentDist - posOnLane - best.occupation *  JAM_FACTOR);
    const SUMOReal maxJam = MAX2(preb[currIdx + prebOffset].occupation, preb[currIdx].occupation);
    const SUMOReal neighLeftPlace = MAX2((SUMOReal) 0, neighDist - posOnLane - maxJam);

    if (changeToBest && bestLaneOffset == curr.bestLaneOffset
            && currentDistDisallows(usableDist, bestLaneOffset, laDist)) {
        /// @brief we urgently need to change lanes to follow our route
        ret = ret | lca | LCA_STRATEGIC | LCA_URGENT;
    } else {

        if (!myAllowOvertakingRight && !right && !myVehicle.congested() && neighLead.first != 0) {
            // check for slower leader on the left. we should not overtake but
            // rather move left ourselves (unless congested)
            MSVehicle* nv = neighLead.first;
            if (nv->getSpeed() < myVehicle.getSpeed()) {
                const SUMOReal vSafe = myCarFollowModel.followSpeed(
                                           &myVehicle, myVehicle.getSpeed(), neighLead.second, nv->getSpeed(), nv->getCarFollowModel().getMaxDecel());
                myVSafes.push_back(vSafe);
                if (vSafe < myVehicle.getSpeed()) {
                    mySpeedGainProbability += TS * myChangeProbThresholdLeft / 3;
                }
            }
        }

        if (!changeToBest && (currentDistDisallows(neighLeftPlace, abs(bestLaneOffset) + 2, laDist))) {
            // the opposite lane-changing direction should be done than the one examined herein
            //  we'll check whether we assume we could change anyhow and get back in time...
            //
            // this rule prevents the vehicle from moving in opposite direction of the best lane
            //  unless the way till the end where the vehicle has to be on the best lane
            //  is long enough
            ret = ret | LCA_STAY | LCA_STRATEGIC;
        } else if (bestLaneOffset == 0 && (neighLeftPlace * 2. < laDist)) {
            // the current lane is the best and a lane-changing would cause a situation
            //  of which we assume we will not be able to return to the lane we have to be on.
            // this rule prevents the vehicle from leaving the current, best lane when it is
            //  close to this lane's end
            ret = ret | LCA_STAY | LCA_STRATEGIC;
        }
    }
    // check for overriding TraCI requests
    ret = myVehicle.influenceChangeDecision(ret);
    if ((ret & lcaCounter) != 0) {
        // we are not interested in traci requests for the opposite direction here
        ret &= ~(LCA_TRACI | lcaCounter | LCA_URGENT);
    }

    if ((ret & LCA_STAY) != 0) {
        return ret;
    }
    if ((ret & LCA_URGENT) != 0) {
        // prepare urgent lane change maneuver
        // save the left space
        myLeftSpace = currentDist - posOnLane;
        if (changeToBest && abs(bestLaneOffset) > 1) {
            // there might be a vehicle which needs to counter-lane-change one lane further and we cannot see it yet
            myLeadingBlockerLength = MAX2((SUMOReal)(right ? 20.0 : 40.0), myLeadingBlockerLength);
        }

        // letting vehicles merge in at the end of the lane in case of counter-lane change, step#1
        //   if there is a leader and he wants to change to the opposite direction
        saveBlockerLength(neighLead.first, lcaCounter);
        if (*firstBlocked != neighLead.first) {
            saveBlockerLength(*firstBlocked, lcaCounter);
        }

        const SUMOReal remainingSeconds = ((ret & LCA_TRACI) == 0 ?
                                           MAX2((SUMOReal)STEPS2TIME(TS), myLeftSpace / MAX2(myLookAheadSpeed, NUMERICAL_EPS) / abs(bestLaneOffset) / URGENCY) :
                                           myVehicle.getInfluencer().changeRequestRemainingSeconds(currentTime));
        const SUMOReal plannedSpeed = informLeader(msgPass, blocked, myLca, neighLead, remainingSeconds);
        if (plannedSpeed >= 0) {
            // maybe we need to deal with a blocking follower
            informFollower(msgPass, blocked, myLca, neighFollow, remainingSeconds, plannedSpeed);
        }

        return ret;
    }

    if (roundaboutEdgesAhead > 1) {
        // try to use the inner lanes of a roundabout to increase throughput
        // unless we are approaching the exit
        if (lca == LCA_LEFT) {
            req = ret | lca | LCA_COOPERATIVE;
        } else {
            req = ret | LCA_STAY | LCA_COOPERATIVE;
        }
        if (!cancelRequest(req)) {
            return ret | req;
        }
    }

    // let's also regard the case where the vehicle is driving on a highway...
    //  in this case, we do not want to get to the dead-end of an on-ramp
    if (right) {
        if (bestLaneOffset == 0 && myVehicle.getLane()->getSpeedLimit() > 80. / 3.6 && myLookAheadSpeed > SUMO_const_haltingSpeed) {
            req = ret | LCA_STAY | LCA_STRATEGIC;
            if (!cancelRequest(req)) {
                return ret | req;
            }
        }
    }
    // --------

    // -------- make place on current lane if blocking follower
    //if (amBlockingFollowerPlusNB()) {
    //    std::cout << myVehicle.getID() << ", " << currentDistAllows(neighDist, bestLaneOffset, laDist)
    //        << " neighDist=" << neighDist
    //        << " currentDist=" << currentDist
    //        << "\n";
    //}

    const SUMOReal inconvenience = MIN2((SUMOReal)1.0, (laneOffset < 0
                                        ? mySpeedGainProbability / myChangeProbThresholdRight
                                        : -mySpeedGainProbability / myChangeProbThresholdLeft));
    if (amBlockingFollowerPlusNB()
            && (inconvenience <= myCooperativeParam)
            //&& ((myOwnState & myLcaCounter) == 0) // VARIANT_6 : counterNoHelp
            && (changeToBest || currentDistAllows(neighDist, abs(bestLaneOffset) + 1, laDist))) {
        req = ret | lca | LCA_COOPERATIVE | LCA_URGENT ;//| LCA_CHANGE_TO_HELP;
        if (!cancelRequest(req)) {
            return ret | req;
        }
    }

    // --------


    //// -------- security checks for krauss
    ////  (vsafe fails when gap<0)
    //if ((blocked & LCA_BLOCKED) != 0) {
    //    return ret;
    //}
    //// --------

    // -------- higher speed
    //if ((congested(neighLead.first) && neighLead.second < 20) || predInteraction(leader.first)) { //!!!
    //    return ret;
    //}
    SUMOReal thisLaneVSafe = myVehicle.getLane()->getVehicleMaxSpeed(&myVehicle);
    SUMOReal neighLaneVSafe = neighLane.getVehicleMaxSpeed(&myVehicle);
    if (neighLead.first == 0) {
        neighLaneVSafe = MIN2(neighLaneVSafe, myCarFollowModel.followSpeed(&myVehicle, myVehicle.getSpeed(), neighDist, 0, 0));
    } else {
        // @todo: what if leader is below safe gap?!!!
        neighLaneVSafe = MIN2(neighLaneVSafe, myCarFollowModel.followSpeed(
                                  &myVehicle, myVehicle.getSpeed(), neighLead.second, neighLead.first->getSpeed(), neighLead.first->getCarFollowModel().getMaxDecel()));
    }
    if (leader.first == 0) {
        thisLaneVSafe = MIN2(thisLaneVSafe, myCarFollowModel.followSpeed(&myVehicle, myVehicle.getSpeed(), currentDist, 0, 0));
    } else {
        // @todo: what if leader is below safe gap?!!!
        thisLaneVSafe = MIN2(thisLaneVSafe, myCarFollowModel.followSpeed(&myVehicle, myVehicle.getSpeed(), leader.second, leader.first->getSpeed(), leader.first->getCarFollowModel().getMaxDecel()));
    }

    const SUMOReal vMax = MIN2(myVehicle.getVehicleType().getMaxSpeed(), myVehicle.getLane()->getVehicleMaxSpeed(&myVehicle));
    thisLaneVSafe = MIN2(thisLaneVSafe, vMax);
    neighLaneVSafe = MIN2(neighLaneVSafe, vMax);
    const SUMOReal relativeGain = (neighLaneVSafe - thisLaneVSafe) / MAX2(neighLaneVSafe,
                                  RELGAIN_NORMALIZATION_MIN_SPEED);

    if (right) {
        // ONLY FOR CHANGING TO THE RIGHT
        if (thisLaneVSafe - 5 / 3.6 > neighLaneVSafe) {
            // ok, the current lane is faster than the right one...
            if (mySpeedGainProbability < 0) {
                mySpeedGainProbability *= pow(0.5, TS);
                //myKeepRightProbability /= 2.0;
            }
        } else {
            // ok, the current lane is not (much) faster than the right one
            // @todo recheck the 5 km/h discount on thisLaneVSafe

            // do not promote changing to the left just because changing to the
            // right is bad
            if (mySpeedGainProbability < 0 || relativeGain > 0) {
                mySpeedGainProbability -= TS * relativeGain;
            }

            // honor the obligation to keep right (Rechtsfahrgebot)
            // XXX consider fast approaching followers on the current lane
            //const SUMOReal vMax = myLookAheadSpeed;
            const SUMOReal acceptanceTime = KEEP_RIGHT_ACCEPTANCE * vMax * MAX2((SUMOReal)1, myVehicle.getSpeed()) / myVehicle.getLane()->getSpeedLimit();
            SUMOReal fullSpeedGap = MAX2((SUMOReal)0, neighDist - myVehicle.getCarFollowModel().brakeGap(vMax));
            SUMOReal fullSpeedDrivingSeconds = MIN2(acceptanceTime, fullSpeedGap / vMax);
            if (neighLead.first != 0 && neighLead.first->getSpeed() < vMax) {
                fullSpeedGap = MAX2((SUMOReal)0, MIN2(fullSpeedGap,
                                                      neighLead.second - myVehicle.getCarFollowModel().getSecureGap(
                                                              vMax, neighLead.first->getSpeed(), neighLead.first->getCarFollowModel().getMaxDecel())));
                fullSpeedDrivingSeconds = MIN2(fullSpeedDrivingSeconds, fullSpeedGap / (vMax - neighLead.first->getSpeed()));
            }
            const SUMOReal deltaProb = (myChangeProbThresholdRight
                                        * STEPS2TIME(DELTA_T)
                                        * (fullSpeedDrivingSeconds / acceptanceTime) / KEEP_RIGHT_TIME);
            myKeepRightProbability -= TS * deltaProb;

            if (gDebugFlag2) {
                std::cout << STEPS2TIME(currentTime)
                          << " veh=" << myVehicle.getID()
                          << " vMax=" << vMax
                          << " neighDist=" << neighDist
                          << " brakeGap=" << myVehicle.getCarFollowModel().brakeGap(myVehicle.getSpeed())
                          << " leaderSpeed=" << (neighLead.first == 0 ? -1 : neighLead.first->getSpeed())
                          << " secGap=" << (neighLead.first == 0 ? -1 : myVehicle.getCarFollowModel().getSecureGap(
                                                myVehicle.getSpeed(), neighLead.first->getSpeed(), neighLead.first->getCarFollowModel().getMaxDecel()))
                          << " acceptanceTime=" << acceptanceTime
                          << " fullSpeedGap=" << fullSpeedGap
                          << " fullSpeedDrivingSeconds=" << fullSpeedDrivingSeconds
                          << " dProb=" << deltaProb
                          << "\n";
            }
            if (myKeepRightProbability * myKeepRightParam < -myChangeProbThresholdRight) {
                req = ret | lca | LCA_KEEPRIGHT;
                if (!cancelRequest(req)) {
                    return ret | req;
                }
            }
        }

        if (mySpeedGainProbability < -myChangeProbThresholdRight
                && neighDist / MAX2((SUMOReal) .1, myVehicle.getSpeed()) > 20.) { //./MAX2((SUMOReal) .1, myVehicle.getSpeed())) { // -.1
            req = ret | lca | LCA_SPEEDGAIN;
            if (!cancelRequest(req)) {
                return ret | req;
            }
        }
    } else {
        // ONLY FOR CHANGING TO THE LEFT
        if (thisLaneVSafe > neighLaneVSafe) {
            // this lane is better
            if (mySpeedGainProbability > 0) {
                mySpeedGainProbability *= pow(0.5, TS);
            }
        } else {
            // left lane is better
            mySpeedGainProbability += TS * relativeGain;
        }
        if (mySpeedGainProbability > myChangeProbThresholdLeft && neighDist / MAX2((SUMOReal) .1, myVehicle.getSpeed()) > 20.) { // .1
            req = ret | lca | LCA_SPEEDGAIN;
            if (!cancelRequest(req)) {
                return ret | req;
            }
        }
    }
    // --------
    if (changeToBest && bestLaneOffset == curr.bestLaneOffset
            && (right ? mySpeedGainProbability < 0 : mySpeedGainProbability > 0)) {
        // change towards the correct lane, speedwise it does not hurt
        req = ret | lca | LCA_STRATEGIC;
        if (!cancelRequest(req)) {
            return ret | req;
        }
    }

    return ret;
}


int
MSLCM_LC2013::slowDownForBlocked(MSVehicle** blocked, int state) {
    //  if this vehicle is blocking someone in front, we maybe decelerate to let him in
    if ((*blocked) != 0) {
        SUMOReal gap = (*blocked)->getPositionOnLane() - (*blocked)->getVehicleType().getLength() - myVehicle.getPositionOnLane() - myVehicle.getVehicleType().getMinGap();
        if (gap > POSITION_EPS) {
            if (myVehicle.getSpeed() < ACCEL2SPEED(myVehicle.getCarFollowModel().getMaxDecel())) {
                if ((*blocked)->getSpeed() < SUMO_const_haltingSpeed) {
                    state |= LCA_AMBACKBLOCKER_STANDING;
                } else {
                    state |= LCA_AMBACKBLOCKER;
                }
                myVSafes.push_back(myCarFollowModel.followSpeed(
                                       &myVehicle, myVehicle.getSpeed(),
                                       (SUMOReal)(gap - POSITION_EPS), (*blocked)->getSpeed(),
                                       (*blocked)->getCarFollowModel().getMaxDecel()));
            }
        }
    }
    return state;
}


void
MSLCM_LC2013::saveBlockerLength(MSVehicle* blocker, int lcaCounter) {
    if (blocker != 0 && (blocker->getLaneChangeModel().getOwnState() & lcaCounter) != 0) {
        // is there enough space in front of us for the blocker?
        const SUMOReal potential = myLeftSpace - myVehicle.getCarFollowModel().brakeGap(
                                       myVehicle.getSpeed(), myVehicle.getCarFollowModel().getMaxDecel(), 0);
        if (blocker->getVehicleType().getLengthWithGap() <= potential) {
            // save at least his length in myLeadingBlockerLength
            myLeadingBlockerLength = MAX2(blocker->getVehicleType().getLengthWithGap(), myLeadingBlockerLength);
        } else {
            // we cannot save enough space for the blocker. It needs to save
            // space for ego instead
            blocker->getLaneChangeModel().saveBlockerLength(myVehicle.getVehicleType().getLengthWithGap());
        }
    }
}
/****************************************************************************/

