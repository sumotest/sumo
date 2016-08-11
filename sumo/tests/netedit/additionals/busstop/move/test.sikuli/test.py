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

#create busstop in mode "reference left"
click(Pattern(netEditResources + "neteditIcon.png").targetOffset(365, 400))

# Change to 
type("m")

# Check all possible movement combinations
dragDrop(Pattern(netEditResources + "neteditIcon.png").targetOffset(375, 407), Pattern(netEditResources + "neteditIcon.png").targetOffset(500, 407))
dragDrop(Pattern(netEditResources + "neteditIcon.png").targetOffset(500, 407), Pattern(netEditResources + "neteditIcon.png").targetOffset(375, 407))

dragDrop(Pattern(netEditResources + "neteditIcon.png").targetOffset(375, 407), Pattern(netEditResources + "neteditIcon.png").targetOffset(500, 300))
dragDrop(Pattern(netEditResources + "neteditIcon.png").targetOffset(500, 407), Pattern(netEditResources + "neteditIcon.png").targetOffset(375, 300))

dragDrop(Pattern(netEditResources + "neteditIcon.png").targetOffset(375, 407), Pattern(netEditResources + "neteditIcon.png").targetOffset(500, 500))
dragDrop(Pattern(netEditResources + "neteditIcon.png").targetOffset(500, 407), Pattern(netEditResources + "neteditIcon.png").targetOffset(375, 500))

dragDrop(Pattern(netEditResources + "neteditIcon.png").targetOffset(375, 407), Pattern(netEditResources + "neteditIcon.png").targetOffset(275, 407))
dragDrop(Pattern(netEditResources + "neteditIcon.png").targetOffset(325, 407), Pattern(netEditResources + "neteditIcon.png").targetOffset(375, 407))

dragDrop(Pattern(netEditResources + "neteditIcon.png").targetOffset(375, 407), Pattern(netEditResources + "neteditIcon.png").targetOffset(700, 407))
dragDrop(Pattern(netEditResources + "neteditIcon.png").targetOffset(650, 407), Pattern(netEditResources + "neteditIcon.png").targetOffset(375, 407))

#Check UndoRedo 
dragDrop(Pattern(netEditResources + "neteditIcon.png").targetOffset(375, 407), Pattern(netEditResources + "neteditIcon.png").targetOffset(500, 407))
type("z", Key.CTRL)

# save additional
type("f", Key.ALT)
click(netEditResources + "toolbar/toolbar-file/file-saveAdditionals.png")

#quit
type("q", Key.CTRL)
wait(netEditResources + "dialogs/dialog-confirmClosingNetwork.png")
type("y", Key.ALT)


