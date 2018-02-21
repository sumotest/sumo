#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2008-2018 German Aerospace Center (DLR) and others.
# This program and the accompanying materials
# are made available under the terms of the Eclipse Public License v2.0
# which accompanies this distribution, and is available at
# http://www.eclipse.org/legal/epl-v20.html
# SPDX-License-Identifier: EPL-2.0

# @file    runner.py
# @author  Michael Behrisch
# @date    2010-09-06
# @version $Id$

from __future__ import absolute_import
from __future__ import print_function

import os
import subprocess
import sys

sumoBinary = os.environ.get("SUMO_BINARY", os.path.join(
    os.path.dirname(sys.argv[0]), '..', '..', '..', '..', 'bin', 'sumo'))
for dep1 in range(10):
    for dep2 in range(10):
        for stop in range(1, 10):
            routes = open("input_routes.rou.xml", 'w')
            print("""
<routes>
    <route id="left" edges="1i 4o 4i 2o 2i 3o 3i 1o 1i"/>
    <vehicle id="l" route="left" depart="%s"/>
    <route id="vertical" edges="3i 4o 4i 3o 3i"/>
    <vehicle id="v" route="vertical" depart="%s">
        <stop lane="4o_0" endPos="%s"/>
    </vehicle>
</routes>""" % (dep1, dep2, stop), file=routes)
            routes.close()
            subprocess.call(
                [sumoBinary] + sys.argv[1:], shell=(os.name == "nt"), stdout=sys.stdout, stderr=sys.stderr)
