#!/usr/bin/env python
"""
@file    sikulixTestRunner.py
@date    2016-0-01
@author  Pablo Alvarez Lopez
@version $Id: sikulixTestRunner.py 20433 2016-04-13 08:00:14Z behrisch $

Wrapper script for running gui tests with SikuliX and TextTest.

SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
Copyright (C) 2008-2016 DLR (http://www.dlr.de/) and contributors

This file is part of SUMO.
SUMO is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.
"""
import socket
import os
import sys
import httplib
import time
import subprocess


if len(sys.argv) != 2:
	print "missing argument. script folder must be specified"
else :
	# get value of TEXTTEST_HOME
	neteditTestFolder = os.environ.get('SUMO_HOME', '.') + "/test/netedit/"

	# Run sikulix server (if isn't already running)
	#subprocess.Popen(["python"] + [os.environ.get('TEXTTEST_HOME', '.') + "/runSikulixServerWindows.py"])
	
	#IMAGES

	imagesSocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

	imagesSocket.connect(("localhost", 50001))

	imagesSocket.send("IMAGES/" + neteditTestFolder + "imageResources")

	imagesSocket.close()
	
	#SCRIPT
		
	scriptSocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

	scriptSocket.connect(("localhost", 50001))

	scriptSocket.send("scripts/C:/Proyectos/Sumo_BranchPablo/tests/netedit/basic/new_net_1edge")

	scriptSocket.close()
	
	#RUN

	runSocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

	runSocket.connect(("localhost", 50001))

	runSocket.send("RUN/test.sikuli")

	runSocket.close();

	# wait to the end
	stop = False;
	stopHTTP = httplib.HTTPConnection('localhost:50001')

	while (stop == False):
		stopHTTP.request("GET", "/")

		if (stopHTTP.getresponse().status == 200):
			stop = True
		# Other conditions
		else:
			time.sleep(0.1)
