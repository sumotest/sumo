/****************************************************************************/
/// @file    Line2D.h
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
#ifndef Line2D_h
#define Line2D_h
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

#include "Position2D.h"
#include <utils/common/DoubleVector.h>

// ===========================================================================
// class declarations
// ===========================================================================
class Position2DVector;
// ===========================================================================
// class definitions
// ===========================================================================
/**
 *
 */
class Line2D
{
public:
    Line2D();
    Line2D(const Position2D &p1, const Position2D &p2);
    ~Line2D();
    void extrapolateBy(SUMOReal length);
    void extrapolateFirstBy(SUMOReal length);
    void extrapolateSecondBy(SUMOReal length);
    const Position2D &p1() const;
    const Position2D &p2() const;
    Position2D getPositionAtDistance(SUMOReal offset) const;
    void move2side(SUMOReal amount);
    DoubleVector intersectsAtLengths(const Position2DVector &v);
    SUMOReal intersectsAtLength(const Line2D &v);
    SUMOReal atan2Angle() const;
    SUMOReal atan2DegreeAngle() const;
    SUMOReal atan2PositiveAngle() const;
    bool intersects(const Line2D &l) const;
    Position2D intersectsAt(const Line2D &l) const;
    SUMOReal length() const;
    void add(SUMOReal x, SUMOReal y);
    void add(const Position2D &p);
    void sub(SUMOReal x, SUMOReal y);
    SUMOReal distanceTo(const Position2D &p) const;
    Line2D &reverse();
    SUMOReal nearestPositionTo(const Position2D &p);
    void rotateDegAtP1(double degs);

private:
    Position2D myP1, myP2;
};


#endif

/****************************************************************************/

