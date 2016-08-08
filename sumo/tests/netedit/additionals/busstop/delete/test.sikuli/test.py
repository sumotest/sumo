#** Common parameters **#
Settings.MoveMouseDelay = 0.1
Settings.DelayBeforeDrop = 0.1
Settings.DelayAfterDrag = 0.1
netEditResources = os.environ['SUMO_HOME'] + "/tests/netedit/imageResources/"
#****#

# Import libraries
import os, sys, subprocess

#Open netedit
subprocess.Popen([os.environ['NETEDIT_BINARY'], 
                  '--window-size', '800,600',
                  '--new', 
                  '--additionals-output', 'additionals.xml'], 
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

# Change to create additional
type("a")

# by default, additional is busstop, then isn't needed to select "busstop"

#change reference to center
click(netEditResources + "additionals/editorParameters/comboBox-referenceRight.png")
click(netEditResources + "additionals/editorParameters/referenceCenter.png")

#create busstop in mode "reference center"
click(Pattern(netEditResources + "neteditIcon.png").targetOffset(400,400))

# Change to delete
type("d")

#delete busStop
click(Pattern(netEditResources + "neteditIcon.png").targetOffset(400,405))

#Check UndoRedo
type("z", Key.CTRL)
type("y", Key.CTRL)

# save additional
type("f", Key.ALT)
click(netEditResources + "toolbar/toolbar-file/file-saveAdditionals.png")

#quit
type("q", Key.CTRL)
wait(netEditResources + "dialogs/dialog-confirmClosingNetwork.png")
type("y", Key.ALT)


