#!/usr/bin/env python
# Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
# Copyright (C) 2008-2018 German Aerospace Center (DLR) and others.
# This program and the accompanying materials
# are made available under the terms of the Eclipse Public License v2.0
# which accompanies this distribution, and is available at
# http://www.eclipse.org/legal/epl-v20.html
# SPDX-License-Identifier: EPL-2.0

# @file    runner.py
# @author  Daniel Krajzewicz
# @author  Michael Behrisch
# @date    2011-06-15
# @version $Id$

from __future__ import absolute_import
from __future__ import print_function

import os
import subprocess
import sys
sumoHome = os.path.abspath(
    os.path.join(os.path.dirname(__file__), '..', '..', '..', '..'))
if "SUMO_HOME" in os.environ:
    sumoHome = os.environ["SUMO_HOME"]
sys.path.append(os.path.join(sumoHome, "tools"))
import sumolib  # noqa

sumoBinary = os.environ.get(
    "SUMO_BINARY", os.path.join(sumoHome, 'bin', 'sumo'))


def call(command):
    retCode = subprocess.call(command, stdout=sys.stdout, stderr=sys.stderr)
    if retCode != 0:
        print("Execution of %s failed." % command, file=sys.stderr)
        sys.exit(retCode)

PERIOD = 5
DEPARTSPEED = "max"

fdo = open("results.csv", "w")
for departPos in "random free random_free base pwagSimple pwagGeneric maxSpeedGap".split():
    print(">>> Building the routes (for departPos %s)" % departPos)
    fd = open("input_routes.rou.xml", "w")
    print("""<routes>
    <flow id="vright" route="right" departPos="%s" departSpeed="%s" begin="0" end="10000" period="%s"/>
    <flow id="vleft" route="left" departPos="%s" departSpeed="%s" begin="0" end="10000" period="%s"/>
    <flow id="vhorizontal" route="horizontal" departPos="%s" departSpeed="%s" begin="0" end="10000" period="%s"/>
</routes>""" % (3 * (departPos, DEPARTSPEED, PERIOD)), file=fd)
    fd.close()

    print(">>> Simulating ((for departPos %s)" % departPos)
    call([sumoBinary, "-c", "sumo.sumocfg", "-v"])

    dump = sumolib.output.dump.readDump("aggregated.xml", ["entered"])
    print("%s;%s" % (departPos, dump.get("entered")[-1]["1si"]), file=fdo)

    if os.path.exists(departPos + "_aggregated.xml"):
        os.remove(departPos + "_aggregated.xml")
    os.rename("aggregated.xml", departPos + "_aggregated.xml")
fdo.close()
