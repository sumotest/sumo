# Import libraries
import os, sys, subprocess

#** Common parameters **#
Settings.MoveMouseDelay = 0.1
Settings.DelayBeforeDrop = 0.1
Settings.DelayAfterDrag = 0.1
# get paths 
SUMOFolder = os.environ.get('SUMO_HOME', '.')
netEditResources = SUMOFolder + "/tests/netedit/imageResources/"
file = open(SUMOFolder + "/tests/netEdit/currentEnvironment.tmp", "r")
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
wait(netEditResources + "neteditIcon.png")

# focus
click(Pattern(netEditResources + "neteditIcon.png").targetOffset(30,0))

# Change to create mode
type("e")

# Create two nodes
click(Pattern(netEditResources + "neteditIcon.png").targetOffset(200,400))
click(Pattern(netEditResources + "neteditIcon.png").targetOffset(600,400))

# save network and quit
type("s", Key.CTRL)
type("q", Key.CTRL)


