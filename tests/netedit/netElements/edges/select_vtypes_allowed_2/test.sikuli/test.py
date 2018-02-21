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

# go to select mode
netedit.selectMode()

# select all edges with allow = "moped" and remove it
netedit.selectItems("Net Element", "edge", "allow", "=moped")
netedit.deleteSelectedItems()

# select all edges with allow = "bicycle" and remove it
netedit.selectItems("Net Element", "edge", "allow", "=bicycle")
netedit.deleteSelectedItems()

# select all edges with allow = "pedestrian" and remove it
netedit.selectItems("Net Element", "edge", "allow", "=pedestrian")
netedit.deleteSelectedItems()

# select all edges with allow = "tram" and remove it
netedit.selectItems("Net Element", "edge", "allow", "=tram")
netedit.deleteSelectedItems()

# select all edges with allow = "rail_urban" and remove it
netedit.selectItems("Net Element", "edge", "allow", "=rail_urban")
netedit.deleteSelectedItems()

# select all edges with allow = "rail" and remove it
netedit.selectItems("Net Element", "edge", "allow", "=rail")
netedit.deleteSelectedItems()

# select all edges with allow = "rail_electric" and remove it
netedit.selectItems("Net Element", "edge", "allow", "=rail_electric")
netedit.deleteSelectedItems()

# select all edges with allow = "ship" and remove it
netedit.selectItems("Net Element", "edge", "allow", "=ship")
netedit.deleteSelectedItems()

# select all edges with allow = "evehicle" and remove it
netedit.selectItems("Net Element", "edge", "allow", "=evehicle")
netedit.deleteSelectedItems()

# select all edges with allow = "private" and remove it
netedit.selectItems("Net Element", "edge", "allow", "=private")
netedit.deleteSelectedItems()

# save network
netedit.saveNetwork()

# quit netedit
netedit.quit(neteditProcess)
