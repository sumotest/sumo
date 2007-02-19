/****************************************************************************/
/// @file    PCVisum.cpp
/// @author  Danilot Teta Boyom
/// @date    Mon, 05 Dec 2005
/// @version $Id$
///
// A reader of polygons stored in visum-format
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

#include <string>
#include <map>
#include <math.h>
#include <fstream>
#include <utils/common/UtilExceptions.h>
#include <utils/common/MsgHandler.h>
#include <utils/options/OptionsCont.h>
#include <utils/options/Option.h>
#include <polyconvert/PCPolyContainer.h>
#include "PCVisum.h"
#include <utils/geoconv/GeoConvHelper.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;


// ===========================================================================
// method defintions
// ===========================================================================
PCVisum::PCVisum(PCPolyContainer &toFill)
        : myCont(toFill)
{}


PCVisum::~PCVisum()
{}


void
PCVisum::load(OptionsCont &oc)
{
    std::string file = oc.getString("visum-file");
    ifstream out(file.c_str());
    if (!out) {
        MsgHandler::getErrorInstance()->inform("Can not open visum-file '" + file + "'.");
        throw ProcessError();
    }
    // Polygon's Attributes
    std::string id, index, xKoord, yKoord;
    Position2DVector vec;

    std::string buff, rest;
    std::string tab = "\t";
    int l = 0;

    string lastID;
    bool first = true;
    while (out.good()) {
        getline(out,buff);

        if (buff.find("$BEZIRKPOLY") != string::npos) {

            while (buff.find("*")==string::npos) {
                l++;
                getline(out,buff);
                if (buff.find("*")!=string::npos) {
                    continue;
                }// in the order to not read the last line

                id = buff.substr(0,buff.find(";"));
                if (!first&&lastID!=id) {
                    Polygon2D *poly = new Polygon2D(id, "bezirk", RGBColor(1, 0, 0), vec, false);
                    vec.clear();
                    myCont.insert(id, poly, 1);
                }
                lastID = id;
                first = false;
                rest = buff.substr(buff.find(";")+1, buff.length());
                index = rest.substr(0,rest.find(";"));
                rest = rest.substr(rest.find(";")+1, rest.length());
                xKoord = rest.substr(0,rest.find(";"));
                rest = rest.substr(rest.find(";")+1, rest.length());
                yKoord = rest.substr(0,rest.find(";"));
                Position2D pos2D((SUMOReal) atof(xKoord.c_str()), (SUMOReal) atof(yKoord.c_str()));
                GeoConvHelper::remap(pos2D);
                vec.push_back(pos2D);
            }
        }
    }
}



/****************************************************************************/

