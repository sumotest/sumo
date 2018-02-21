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

# select all edges with disallow = "army" and remove it
netedit.selectItems("Net Element", "edge", "disallow", "=army")
netedit.deleteSelectedItems()

# select all edges with disallow = "authority" and remove it
netedit.selectItems("Net Element", "edge", "disallow", "=authority")
netedit.deleteSelectedItems()

# select all edges with disallow = "vip" and remove it
netedit.selectItems("Net Element", "edge", "disallow", "=vip")
netedit.deleteSelectedItems()

# select all edges with disallow = "hov" and remove it
netedit.selectItems("Net Element", "edge", "disallow", "=hov")
netedit.deleteSelectedItems()

# select all edges with disallow = "custom1" and remove it
netedit.selectItems("Net Element", "edge", "disallow", "=custom1")
netedit.deleteSelectedItems()

# select all edges with disallow = "custom2" and remove it
netedit.selectItems("Net Element", "edge", "disallow", "=custom2")
netedit.deleteSelectedItems()

# save network
netedit.saveNetwork()

# quit netedit
netedit.quit(neteditProcess)
