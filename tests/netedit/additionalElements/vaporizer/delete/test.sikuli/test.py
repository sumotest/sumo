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

# select vaporizer
netedit.changeAdditional("vaporizer")

# create vaporizer
netedit.leftClick(match, 350, 200)

# Change to delete
netedit.deleteMode()

# delete loaded vaporizer
netedit.leftClick(match, 310, 200)

# delete created vaporizer (using stack)
netedit.leftClick(match, 310, 200)

# delete lane with the second loaded vaporizer
netedit.leftClick(match, 310, 250)

# Check undo
netedit.undo(match, 3)

# Change to delete
netedit.deleteMode()

# disable 'Automatically delete additionals'
netedit.changeAutomaticallyDeleteAdditionals(match)

# try to delete lane with the second loaded vaporizer (doesn't allowed)
netedit.leftClick(match, 310, 250)

# wait warning
netedit.waitAutomaticallyDeleteAdditionalsWarning()

# check redo
netedit.redo(match, 3)

# save additionals
netedit.saveAdditionals()

# save network
netedit.saveNetwork()

# quit netedit
netedit.quit(neteditProcess)
