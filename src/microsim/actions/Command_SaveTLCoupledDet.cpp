/****************************************************************************/
/// @file    Command_SaveTLCoupledDet.cpp
/// @author  Daniel Krajzewicz
/// @date    15 Feb 2004
/// @version $Id$
///
// Realises the output of a tls values on each switch
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

#include "Command_SaveTLCoupledDet.h"
#include <microsim/MSNet.h>
#include <microsim/traffic_lights/MSTrafficLightLogic.h>
#include <microsim/MSEventControl.h>
#include <microsim/output/MSDetectorFileOutput.h>
#include <utils/common/UtilExceptions.h>
#include <utils/common/MsgHandler.h>
#include <utils/iodevices/OutputDevice.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;


// ===========================================================================
// method definitions
// ===========================================================================
Command_SaveTLCoupledDet::Command_SaveTLCoupledDet(
    const MSTLLogicControl::TLSLogicVariants &tlls,
    MSDetectorFileOutput *dtf, unsigned int begin,
    OutputDevice *device)
        : myDevice(device), myLogics(tlls), myDetector(dtf),
        myStartTime(begin)
{
    for (std::map<std::string, MSTrafficLightLogic*>::const_iterator i=tlls.ltVariants.begin(); i!=tlls.ltVariants.end(); ++i) {
        (*i).second->addSwitchAction(this);
    }
}


Command_SaveTLCoupledDet::~Command_SaveTLCoupledDet()
{}


bool
Command_SaveTLCoupledDet::execute()
{
    SUMOTime end =
        MSNet::getInstance()->getCurrentTimeStep();
    myDetector->writeXMLOutput(*myDevice, myStartTime, end);
    myStartTime = end;
    return true;
}



/****************************************************************************/

