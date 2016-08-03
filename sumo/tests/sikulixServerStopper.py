#!/usr/bin/env python
"""
@file    sikulixServerStopper.py
@author  Pablo Alvarez Lopez
@date    2016-08-01
@version $Id: toolrunner.py 20433 2016-04-13 08:00:14Z behrisch $

Wrapper script for stop Server of SikuliX

SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
Copyright (C) 2008-2016 DLR (http://www.dlr.de/) and contributors

This file is part of SUMO.
SUMO is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.
"""
import os
import subprocess
import sys

#Create stop socket
stopSocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

#Connect to localhost throught port 50001 (Default port of SikuliX)
stopSocket.connect(("localhost", 50001))

#Send stop command
stopSocket.send("STOP")

#Close stop socket
stopSocket.close()