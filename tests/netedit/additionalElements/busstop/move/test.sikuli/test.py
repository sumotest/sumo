#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2009-2018 German Aerospace Center (DLR) and others.
# This program and the accompanying materials
# are made available under the terms of the Eclipse Public License v2.0
# which accompanies this distribution, and is available at
# http://www.eclipse.org/legal/epl-v20.html
# SPDX-License-Identifier: EPL-2.0

# @file    test.py
# @author  Pablo Alvarez Lopez
# @date    2016-11-25
# @version $Id$

# import common functions for netedit tests
import os
import sys

testRoot = os.path.join(os.environ.get('SUMO_HOME', '.'), 'tests')
neteditTestRoot = os.path.join(
    os.environ.get('TEXTTEST_HOME', testRoot), 'netedit')
sys.path.append(neteditTestRoot)
import neteditTestFunctions as netedit  # noqa

# Open netedit
neteditProcess, match = netedit.setupAndStart(neteditTestRoot)

# go to additional mode
netedit.additionalMode()

# select busStop
netedit.changeAdditional("busStop")

# change reference to center
netedit.modifyAdditionalDefaultValue(8, "reference center")

# create busStop in mode "reference center"
netedit.leftClick(match, 250, 250)

# change to move mode
netedit.moveMode()

# move busStop to left
netedit.moveElement(match, 150, 275, 50, 275)

# move back
netedit.moveElement(match, 50, 275, 150, 275)

# move busStop to right
netedit.moveElement(match, 150, 275, 250, 275)

# move back
netedit.moveElement(match, 250, 275, 150, 275)

# move busStop to left overpassing lane
netedit.moveElement(match, 150, 275, -100, 275)

# move back
netedit.moveElement(match, -90, 275, 150, 275)

# move busStop to right overpassing lane
netedit.moveElement(match, 150, 275, 550, 275)

# move back to another different position of initial
netedit.moveElement(match, 500, 275, 300, 275)

# Check undos and redos
netedit.undo(match, 10)
netedit.redo(match, 10)

# save additionals
netedit.saveAdditionals()

# save network
netedit.saveNetwork()

# quit netedit
netedit.quit(neteditProcess)
