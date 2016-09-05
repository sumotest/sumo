# Import libraries
import os, sys, subprocess

#** Common parameters **#
Settings.MoveMouseDelay = 0.1
Settings.DelayBeforeDrop = 0.1
Settings.DelayAfterDrag = 0.1
# get paths 
SUMOFolder = os.environ.get('SUMO_HOME', '.')
netEditResources = SUMOFolder + "/tests/netedit/imageResources/"
file = open(SUMOFolder + "/tests/netedit/currentEnvironment.tmp", "r")
neteditApp = file.readline().replace("\n", "")
textTestSandBox = file.readline().replace("\n", "")
file.close()
#****#

#Open netedit
subprocess.Popen([neteditApp, 
                  '--window-size', '800,600',
                  '--new', 
                  '-o', textTestSandBox + "/net.net.xml"], 
                  env=os.environ, stdout=sys.stdout, stderr=sys.stderr)

#Settings.MinSimilarity = 0.1
match = wait(netEditResources + "neteditNewNet.png", 10)

# focus
click(Pattern(netEditResources + "neteditIcon.png").targetOffset(30,0))

# Change to create mode
type("e")

# Create two nodes
click(match.getTarget().offset(-200,0))
click(match.getTarget().offset(200,0))

# save network and quit
type("s", Key.CTRL)
type("q", Key.CTRL)


