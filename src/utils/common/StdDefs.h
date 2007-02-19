/****************************************************************************/
/// @file    StdDefs.h
/// @author  Daniel Krajzewicz
/// @date    Fri, 29.04.2005
/// @version $Id$
///
//
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// copyright : (C) 2001-2007
//  by DLR (http://www.dlr.de/) and ZAIK (http://www.zaik.uni-koeln.de/AFS)
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef StdDefs_h
#define StdDefs_h
// ===========================================================================
// compiler pragmas
// ===========================================================================
#ifdef _MSC_VER
#pragma warning(disable: 4786)
#endif


// ===========================================================================
// included modules
// ===========================================================================
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif


/* -------------------------------------------------------------------------
 * some constant defaults used by SUMO
 * ----------------------------------------------------------------------- */
const SUMOReal SUMO_const_laneWidth = (SUMOReal) 3.2;
const SUMOReal SUMO_const_halfLaneWidth = (SUMOReal) 1.6;
const SUMOReal SUMO_const_laneOffset = (SUMOReal) .1;
const SUMOReal SUMO_const_laneWidthAndOffset = (SUMOReal) 3.3;
const SUMOReal SUMO_const_halfLaneAndOffset = (SUMOReal)(3.2/2.+.1);


/* -------------------------------------------------------------------------
 * definitions of common used conversions
 * ----------------------------------------------------------------------- */
#define mBYs2kmBYh(x) ((SUMOReal) (x/3.6))


/* -------------------------------------------------------------------------
 * templates for mathematical functions missing in some c++-implementations
 * ----------------------------------------------------------------------- */
template<typename T>
inline T
MIN2(T a, T b)
{
    return a<b?a:b;
}

template<typename T>
inline T
MAX2(T a, T b)
{
    return a>b?a:b;
}


template<typename T>
inline T
MIN3(T a, T b, T c)
{
    return MIN2(c, a<b?a:b);
}


template<typename T>
inline T
MAX3(T a, T b, T c)
{
    return MAX2(c, a>b?a:b);
}


template<typename T>
inline T
MIN4(T a, T b, T c, T d)
{
    return MIN2(MIN2(a,b),MIN2(c,d));
}


template<typename T>
inline T
MAX4(T a, T b, T c, T d)
{
    return MAX2(MAX2(a,b),MAX2(c,d));
}


#endif

/****************************************************************************/

