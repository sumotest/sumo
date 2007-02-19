/****************************************************************************/
/// @file    MSPhoneNet.cpp
/// @author  Eric Nicolay
/// @date    2006
/// @version $Id$
///
// The cellular network (GSM)
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

#include <cassert>
#include "MSPhoneNet.h"
#include "MSCORN.h"
#include <iostream>
#include <fstream>
#include "utils/options/OptionsCont.h"
#include "utils/options/OptionsSubSys.h"
#include "MSPhoneCell.h"
#include "MSPhoneLA.h"


// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;


// ===========================================================================
// method definitions
// ===========================================================================
MSPhoneNet::MSPhoneNet()
{
    _LAIntervall = 300;
    _CellIntervall = 300;
}


MSPhoneNet::~MSPhoneNet()
{
    std::map< int, MSPhoneCell* >::iterator cit;
    for (cit = _mMSPhoneCells.begin(); cit != _mMSPhoneCells.end(); cit++) {
        delete cit->second;
    }
    std::map< int, MSPhoneLA* >::iterator lit;
    for (lit = _mMSPhoneLAs.begin(); lit != _mMSPhoneLAs.end(); lit++) {
        delete lit->second;
    }
}


MSPhoneCell*
MSPhoneNet::getMSPhoneCell(int id)
{
    if (_mMSPhoneCells.find(id) != _mMSPhoneCells.end()) {
        return _mMSPhoneCells[id];
    }
    return 0;
}


MSPhoneCell*
MSPhoneNet::getCurrentVehicleCell(const std::string &id)
{
    MSPhoneCell * ret = 0;
    std::map< int, MSPhoneCell* >::iterator cit;
    for (cit = _mMSPhoneCells.begin(); cit != _mMSPhoneCells.end() ; cit++) {
        if (cit->second->hasCPhone(id)) {
            ret =  cit->second;
            break;
        }
    }
    return ret;
}


MSPhoneLA*
MSPhoneNet::getCurrentVehicleLA(const std::string &id)
{
    MSPhoneLA * ret = 0;
    std::map< int, MSPhoneLA* >::iterator lit;
    for (lit=_mMSPhoneLAs.begin(); lit!=_mMSPhoneLAs.end(); lit++) {
        if (lit->second->hasCall(id)) {
            ret = lit->second;
            break;
        }
    }
    return ret;
}


void
MSPhoneNet::addMSPhoneCell(int id)
{
    /*MSPhoneCell* c = new MSPhoneCell( id );
    _mMSPhoneCells[id] = c;*/
    std::map< int, MSPhoneCell* >::iterator cit = _mMSPhoneCells.find(id);
    if (cit == _mMSPhoneCells.end()) {
        MSPhoneCell* c = new MSPhoneCell(id);
        _mMSPhoneCells[id] = c;
    }
}


void
MSPhoneNet::addMSPhoneCell(int id, int la)
{
    std::map< int, MSPhoneCell* >::iterator cit = _mMSPhoneCells.find(id);
    if (cit == _mMSPhoneCells.end()) {
        MSPhoneCell* c = new MSPhoneCell(id);
        _mMSPhoneCells[id] = c;
    }
    std::map< int, MSPhoneLA* >::iterator lit = _mMSPhoneLAs.find(la);
    if (lit == _mMSPhoneLAs.end()) {
        MSPhoneLA* l = new MSPhoneLA(la, 0);
        _mMSPhoneLAs[la] = l;
    }
    _mCell2LA[id] = la;
}


void
MSPhoneNet::connectLA2Cell(int cell_ID, int la_ID)
{
    if (_mMSPhoneCells.find(cell_ID) != _mMSPhoneCells.end() && _mMSPhoneLAs.find(la_ID) != _mMSPhoneLAs.end()) {
        _mCell2LA[cell_ID] = la_ID;
    }
}


void
MSPhoneNet::remMSPhoneCell(int id)
{
    std::map< int, MSPhoneCell* >::iterator cit = _mMSPhoneCells.find(id);
    if (cit != _mMSPhoneCells.end()) {
        delete cit->second;
        _mMSPhoneCells.erase(id);
    }
}


MSPhoneLA*
MSPhoneNet::getMSPhoneLA(int id)
{
    std::map<int, int>::iterator it;
    it = _mCell2LA.find(id);
    assert(it!=_mCell2LA.end());
    if (it != _mCell2LA.end()) {
        if (_mMSPhoneLAs.find(it->second) != _mMSPhoneLAs.end()) {
            return _mMSPhoneLAs[it->second];
        }
        return 0;
    }
    return 0;
}


void
MSPhoneNet::addMSPhoneLA(int id, int dir)
{
    MSPhoneLA* la = new MSPhoneLA(id, dir);
    _mMSPhoneLAs[id] = la;
}


void
MSPhoneNet::remMSPhoneLA(int id)
{
    std::map< int, MSPhoneLA* >::iterator lit = _mMSPhoneLAs.find(id);
    if (lit != _mMSPhoneLAs.end()) {
        delete lit->second;
        _mMSPhoneLAs.erase(id);
    }
}


void
MSPhoneNet::writeOutput(SUMOTime t)
{
    std::map< int, MSPhoneCell* >::iterator cit;
    for (cit = _mMSPhoneCells.begin(); cit != _mMSPhoneCells.end(); cit++) {
        cit->second->setnextexpectData(t);
    }

    if (MSCORN::wished(MSCORN::CORN_OUT_CELL_TO_SS2) && (t % _CellIntervall) == 0) {
        std::map< int, MSPhoneCell* >::iterator cit;
        for (cit = _mMSPhoneCells.begin(); cit != _mMSPhoneCells.end(); cit++) {
            cit->second->writeOutput(t);
        }
    }

    if (MSCORN::wished(MSCORN::CORN_OUT_LA_TO_SS2) && (t % _LAIntervall) == 0) {
        std::map< int, MSPhoneLA* >::iterator lit;
        for (lit = _mMSPhoneLAs.begin(); lit != _mMSPhoneLAs.end(); lit++) {
            lit->second->writeOutput(t);
        }
    }
    /*the same but for the sql version*/
    if (MSCORN::wished(MSCORN::CORN_OUT_CELL_TO_SS2_SQL) && (t % _CellIntervall) == 0) {
        std::map< int, MSPhoneCell* >::iterator cit;
        for (cit = _mMSPhoneCells.begin(); cit != _mMSPhoneCells.end(); cit++) {
            cit->second->writeSQLOutput(t);
        }
    }

    if (MSCORN::wished(MSCORN::CORN_OUT_LA_TO_SS2_SQL) && (t % _LAIntervall) == 0) {
        /*std::map< int, MSPhoneLA* >::iterator lit;
        for ( lit = _mMSPhoneLAs.begin(); lit != _mMSPhoneLAs.end(); lit++ ) {
            lit->second->writeSQLOutput( t );
        }*/
        std::map< std::string, int >::iterator lit;
        for (lit = myLAChanges.begin(); lit != myLAChanges.end(); lit++) {
            MSCORN::saveTOSS2SQL_LA_ChangesData(t, atoi(lit->first.c_str()), 0, lit->second, 30, _LAIntervall);
        }
        myLAChanges.clear();
    }
    //setCellStatData(t);
}


void
MSPhoneNet::setCellStatData(SUMOTime t)
{
    std::map< int, MSPhoneCell* >::iterator cit;
    for (cit = _mMSPhoneCells.begin(); cit != _mMSPhoneCells.end(); cit++) {
        cit->second->setnextexpectData(t);
    }
}

void
MSPhoneNet::addLAChange(const std::string & pos_id)
{
    std::map< std::string, int >::iterator it = myLAChanges.find(pos_id);
    if (it == myLAChanges.end())
        myLAChanges.insert(make_pair(pos_id, 1));
    else
        myLAChanges[pos_id]++;
}



/****************************************************************************/

