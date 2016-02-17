/****************************************************************************/
/// @file    GNELoadThread.cpp
/// @author  Jakob Erdmann
/// @date    Feb 2011
/// @version $Id$
///
// The thread that performs the loading of a Netedit-net (adapted from
// GUILoadThread)
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

#include <iostream>
#include <ctime>
#include <utils/gui/events/GUIEvent_Message.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/globjects/GUIGlObjectStorage.h>
#include <utils/common/RandHelper.h>
#include <utils/common/UtilExceptions.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/MsgRetrievingFunction.h>
#include <utils/common/SystemFrame.h>
#include <utils/options/OptionsCont.h>
#include <utils/options/Option.h>
#include <utils/options/OptionsIO.h>
#include <utils/geom/GeoConvHelper.h>
#include <netbuild/NBFrame.h>
#include <netimport/NILoader.h>
#include <netimport/NIFrame.h>
#include <netwrite/NWFrame.h>
#include <netbuild/NBFrame.h>

#include "GNELoadThread.h"
#include "GNENet.h"
#include "GNEEvent_NetworkLoaded.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// member method definitions
// ===========================================================================
GNELoadThread::GNELoadThread(FXApp* app, MFXInterThreadEventClient* mw,
                             MFXEventQue<GUIEvent*>& eq, FXEX::FXThreadEvent& ev)
    : FXSingleEventThread(app, mw), myParent(mw), myEventQue(eq),
      myEventThrow(ev) {
    myErrorRetriever = new MsgRetrievingFunction<GNELoadThread>(this,
            &GNELoadThread::retrieveMessage, MsgHandler::MT_ERROR);
    myMessageRetriever = new MsgRetrievingFunction<GNELoadThread>(this,
            &GNELoadThread::retrieveMessage, MsgHandler::MT_MESSAGE);
    myWarningRetriever = new MsgRetrievingFunction<GNELoadThread>(this,
            &GNELoadThread::retrieveMessage, MsgHandler::MT_WARNING);
    MsgHandler::getErrorInstance()->addRetriever(myErrorRetriever);
}


GNELoadThread::~GNELoadThread() {
    delete myErrorRetriever;
    delete myMessageRetriever;
    delete myWarningRetriever;
}


FXint
GNELoadThread::run() {
    GNENet* net = 0;
    OptionsCont& oc = OptionsCont::getOptions();

    // within gui-based applications, nothing is reported to the console
    /*
    MsgHandler::getErrorInstance()->report2cout(false);
    MsgHandler::getErrorInstance()->report2cerr(false);
    MsgHandler::getWarningInstance()->report2cout(false);
    MsgHandler::getWarningInstance()->report2cerr(false);
    MsgHandler::getMessageInstance()->report2cout(false);
    MsgHandler::getMessageInstance()->report2cerr(false);
    */
    // register message callbacks
    MsgHandler::getMessageInstance()->addRetriever(myMessageRetriever);
    MsgHandler::getErrorInstance()->addRetriever(myErrorRetriever);
    MsgHandler::getWarningInstance()->addRetriever(myWarningRetriever);

    // try to load the given configuration
    if (!myOptionsReady && !initOptions()) {
        // the options are not valid
        submitEndAndCleanup(net);
        return 0;
    }
    MsgHandler::initOutputOptions();
    if (!(NIFrame::checkOptions() &&
            NBFrame::checkOptions() &&
            NWFrame::checkOptions())) {
        // options are not valid
        WRITE_ERROR("Invalid Options. Nothing loaded");
        submitEndAndCleanup(net);
        return 0;
    }
    MsgHandler::getErrorInstance()->clear();
    MsgHandler::getWarningInstance()->clear();
    MsgHandler::getMessageInstance()->clear();

    RandHelper::initRandGlobal();
    if (!GeoConvHelper::init(oc)) {
        WRITE_ERROR("Could not build projection!");
        submitEndAndCleanup(net);
        return 0;
    }
    // this netbuilder instance becomes the responsibility of the GNENet
    NBNetBuilder* netBuilder = new NBNetBuilder();

    netBuilder->applyOptions(oc);

    if (myNewNet) {
        // create new network
        net = new GNENet(netBuilder);
    } else {
        NILoader nl(*netBuilder);
        try {
            nl.load(oc);

            if (!myLoadNet) {
                WRITE_MESSAGE("Performing initial computation ...\n");
                // perform one-time processing (i.e. edge removal)
                netBuilder->compute(oc);
                // @todo remove one-time processing options!
            } else {
                // make coordinate conversion usable before first netBuilder->compute()
                GeoConvHelper::computeFinal();
            }

            if (oc.getBool("ignore-errors")) {
                MsgHandler::getErrorInstance()->clear();
            }
            // check whether any errors occured
            if (MsgHandler::getErrorInstance()->wasInformed()) {
                throw ProcessError();
            } else {
                net = new GNENet(netBuilder);
            }
        } catch (ProcessError& e) {
            if (std::string(e.what()) != std::string("Process Error") && std::string(e.what()) != std::string("")) {
                WRITE_ERROR(e.what());
            }
            WRITE_ERROR("Failed to build network.");
            delete net;
            delete netBuilder;
            net = 0;
#ifndef _DEBUG
        } catch (std::exception& e) {
            WRITE_ERROR(e.what());
            delete net;
            delete netBuilder;
            net = 0;
#endif
        }
    }
    // only a single setting file is supported
    submitEndAndCleanup(net, oc.getString("gui-settings-file"), oc.getBool("registry-viewport"));
    return 0;
}



void
GNELoadThread::submitEndAndCleanup(GNENet* net, const std::string& guiSettingsFile, const bool viewportFromRegistry) {
    // remove message callbacks
    MsgHandler::getErrorInstance()->removeRetriever(myErrorRetriever);
    MsgHandler::getWarningInstance()->removeRetriever(myWarningRetriever);
    MsgHandler::getMessageInstance()->removeRetriever(myMessageRetriever);
    // inform parent about the process
    GUIEvent* e = new GNEEvent_NetworkLoaded(net, myFile, guiSettingsFile, viewportFromRegistry);
    myEventQue.add(e);
    myEventThrow.signal();
}


void
GNELoadThread::fillOptions(OptionsCont& oc) {
    oc.clear();
    oc.addCallExample("", "start plain GUI with empty net");
    oc.addCallExample("-c <CONFIGURATION>", "edit net with options read from file");

    SystemFrame::addConfigurationOptions(oc); // this subtopic is filled here, too
    oc.addOptionSubTopic("Input");
    oc.addOptionSubTopic("Output");
    GeoConvHelper::addProjectionOptions(oc);
    oc.addOptionSubTopic("TLS Building");
    oc.addOptionSubTopic("Ramp Guessing");
    oc.addOptionSubTopic("Edge Removal");
    oc.addOptionSubTopic("Unregulated Nodes");
    oc.addOptionSubTopic("Default values"); // PABLO #1916
    oc.addOptionSubTopic("Processing");
    oc.addOptionSubTopic("Building Defaults");
    oc.addOptionSubTopic("Visualisation");

    oc.doRegister("busStop default length", new Option_String("10"));                                   // PABLO #1916
    oc.addDescription("busStop default length", "Default values", "Default lenght of the busStops");    // PABLO #1916

    oc.doRegister("busStop default lines", new Option_String());                                    // PABLO #1916
    oc.addDescription("busStop default lines", "Default values", "Default lines of the busStops");  // PABLO #1916

    oc.doRegister("chargingStation default length", new Option_String("10"));                                       // PABLO #1916
    oc.addDescription("chargingStation default length", "Default values", "Default length of the chargingStation"); // PABLO #1916

    oc.doRegister("chargingStation default charging power", new Option_String("22000"));                                            // PABLO #1916
    oc.addDescription("chargingStation default charging power", "Default values", "Default charging power of the chargingStation"); // PABLO #1916

    oc.doRegister("chargingStation default efficiency", new Option_String("0.95"));                                                     // PABLO #1916
    oc.addDescription("chargingStation default efficiency", "Default values", "Default charging efficiency of the chargingStation");    // PABLO #1916

    oc.doRegister("chargingStation default charge in transit", new Option_Bool(false));                                                     // PABLO #1916
    oc.addDescription("chargingStation default charge in transit", "Default values", "Default charge in transit of the chargingStation");   // PABLO #1916

    oc.doRegister("chargingStation default charge delay", new Option_String("0"));                                              // PABLO #1916
    oc.addDescription("chargingStation default charge delay", "Default values", "Default charge delay of the chargingStation"); // PABLO #1916

    oc.doRegister("detector E1 default frequency", new Option_String("100"));                                                                           // PABLO #1916
    oc.addDescription("detector E1 default frequency", "Default values", "The aggregation period the values the detector collects shall be summed up"); // PABLO #1916

    oc.doRegister("detector E1 default file", new Option_String(""));                               // PABLO #1916
    oc.addDescription("detector E1 default file", "Default values", "The path to the output file"); // PABLO #1916

    oc.doRegister("detector E1 default friendlyPos", new Option_Bool(false));                                                                               // PABLO #1916
    oc.addDescription("detector E1 default friendlyPos", "Default values", "If set, no error will be reported if the detector is placed behind the lane");  // PABLO #1916

    oc.doRegister("detector E1 default splitByType", new Option_Bool(false));                                                                                       // PABLO #1916
    oc.addDescription("detector E1 default splitByType", "Default values", "If set, the collected values will be additionally reported on per-vehicle type base");  // PABLO #1916

    oc.doRegister("detector E2 default length", new Option_String("10"));                                       // PABLO #1916
    oc.addDescription("detector E2 default length", "Default values", "The length of the detector in meters");  // PABLO #1916
    
    oc.doRegister("detector E2 default frequency", new Option_String("100"));                                                                           // PABLO #1916
    oc.addDescription("detector E2 default frequency", "Default values", "The aggregation period the values the detector collects shall be summed up"); // PABLO #1916

    oc.doRegister("detector E2 default file", new Option_String(""));                               // PABLO #1916
    oc.addDescription("detector E2 default file", "Default values", "The path to the output file"); // PABLO #1916
    
    oc.doRegister("detector E2 default cont", new Option_Bool(false));                                                                                              // PABLO #1916
    oc.addDescription("detector E2 default cont", "Default values", "Holds the information whether detectors longer than a lane shall be cut off or continued");    // PABLO #1916
   
    oc.doRegister("detector E2 default timeThreshold", new Option_String("1"));                                                                                                             // PABLO #1916
    oc.addDescription("detector E2 default timeThreshold", "Default values", "The time-based threshold that describes how much time has to pass until a vehicle is recognized as halting"); // PABLO #1916
    
    oc.doRegister("detector E2 default speedThreshold", new Option_String("1.39"));                                                                                                 // PABLO #1916
    oc.addDescription("detector E2 default speedThreshold", "Default values", "The speed-based threshold that describes how slow a vehicle has to be to be recognized as halting"); // PABLO #1916
   
    oc.doRegister("detector E2 jamThreshold", new Option_String("10"));                                                                                                                     // PABLO #1916
    oc.addDescription("detector E2 jamThreshold", "Default values", "The minimum distance to the next standing vehicle in order to make this vehicle count as a participant to the jam");   // PABLO #1916

    oc.doRegister("detector E2 default friendlyPos", new Option_Bool(false));                                                                               // PABLO #1916
    oc.addDescription("detector E2 default friendlyPos", "Default values", "If set, no error will be reported if the detector is placed behind the lane");  // PABLO #1916 

    oc.doRegister("detector E3 default frequency", new Option_String("100"));                                                                           // PABLO #1916
    oc.addDescription("detector E3 default frequency", "Default values", "The aggregation period the values the detector collects shall be summed up"); // PABLO #1916
    
    oc.doRegister("detector E3 default file", new Option_String(""));                               // PABLO #1916
    oc.addDescription("detector E3 default file", "Default values", "The path to the output file"); // PABLO #1916

    oc.doRegister("rerouter default file", new Option_String(""));                                      // PABLO #1916
    oc.addDescription("rerouter default file", "Default values", "The path to the definition file");    // PABLO #1916

    oc.doRegister("rerouter default probability", new Option_String("1"));                                          // PABLO #1916
    oc.addDescription("rerouter default probability", "Default values", "The probability for vehicle rerouting");   // PABLO #1916

    oc.doRegister("rerouter default off", new Option_Bool(false));                                                  // PABLO #1916
    oc.addDescription("rerouter default off", "Default values", "Whether the router should be inactive initially"); // PABLO #1916 

    oc.doRegister("disable-textures", 'T', new Option_Bool(false)); // !!!
    oc.addDescription("disable-textures", "Visualisation", "");
    oc.doRegister("gui-settings-file", new Option_FileName());
    oc.addDescription("gui-settings-file", "Visualisation", "Load visualisation settings from FILE");
    oc.doRegister("registry-viewport", new Option_Bool(false));
    oc.addDescription("registry-viewport", "Visualisation", "Load current viewport from registry");

    SystemFrame::addReportOptions(oc); // this subtopic is filled here, too

    NIFrame::fillOptions();
    NBFrame::fillOptions(false);
    NWFrame::fillOptions(false);
    RandHelper::insertRandOptions();
}


void
GNELoadThread::setDefaultOptions(OptionsCont& oc) {
    oc.set("offset.disable-normalization", "true"); // preserve the given network as far as possible
    oc.set("no-turnarounds", "true"); // otherwise it is impossible to manually removed turn-arounds
}


bool
GNELoadThread::initOptions() {
    OptionsCont& oc = OptionsCont::getOptions();
    fillOptions(oc);
    if (myLoadNet) {
        oc.set("sumo-net-file", myFile);
    } else {
        oc.set("configuration-file", myFile);
    }
    setDefaultOptions(oc);
    OptionsIO::setArgs(0, 0);
    try {
        OptionsIO::getOptions();
        if (!oc.isSet("output-file")) {
            oc.set("output-file", oc.getString("sumo-net-file"));
        }
        return true;
    } catch (ProcessError& e) {
        if (std::string(e.what()) != std::string("Process Error") && std::string(e.what()) != std::string("")) {
            WRITE_ERROR(e.what());
        }
        WRITE_ERROR("Failed to parse options.");
    }
    return false;
}


void
GNELoadThread::loadConfigOrNet(const std::string& file, bool isNet, bool optionsReady, bool newNet) {
    myFile = file;
    myLoadNet = isNet;
    myOptionsReady = optionsReady;
    myNewNet = newNet;
    start();
}


void
GNELoadThread::retrieveMessage(const MsgHandler::MsgType type, const std::string& msg) {
    GUIEvent* e = new GUIEvent_Message(type, msg);
    myEventQue.add(e);
    myEventThrow.signal();
}

/****************************************************************************/

