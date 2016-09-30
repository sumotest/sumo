# Import libraries
import os, sys, subprocess

#** Common parameters **#
Settings.MoveMouseDelay = 0.1
Settings.DelayBeforeDrop = 0.1
Settings.DelayAfterDrag = 0.1
# SUMO Folder
SUMOFolder = os.environ.get('SUMO_HOME', '.')
# NetEdit Resources
netEditResources = SUMOFolder + "/tests/netedit/imageResources/"
# Current environment
currentEnvironmentFile = open(SUMOFolder + "/tests/netedit/currentEnvironment.tmp", "r")
# Path to netEdit app
neteditApp = currentEnvironmentFile.readline().replace("\n", "")
# SandBox folder
textTestSandBox = currentEnvironmentFile.readline().replace("\n", "")
currentEnvironmentFile.close()
#****#

#Open netedit
subprocess.Popen([neteditApp, 
                  '--window-size', '800,600',
                  '--new', 
                  '-o', textTestSandBox + "/net.net.xml"], 
                  env=os.environ, stdout=sys.stdout, stderr=sys.stderr)

#Settings.MinSimilarity = 0.1
match = wait(netEditResources + "neteditToolbar.png", 10)

# focus
click(Pattern(netEditResources + "neteditIcon.png").targetOffset(30,0))

# Change to create mode
type("e")

# Create two nodes
click(match.getTarget().offset(100,300))
click(match.getTarget().offset(500,300))

# save network and quit
type("s", Key.CTRL)
type("q", Key.CTRL)
