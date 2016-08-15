#** Common parameters **#
Settings.MoveMouseDelay = 0.1
Settings.DelayBeforeDrop = 0.1
Settings.DelayAfterDrag = 0.1
netEditResources = os.environ['SUMO_HOME'] + "/tests/netedit/imageResources/"

# abort function
def abort(reason): 
	neteditApp.close()
	sys.exit("'" + reason + "' not found")
#****#

# Import libraries
import os, sys

# get netedit route and parameters
neteditRoute = [os.environ['NETEDIT_BINARY'], 
				'--window-size', '800,600',
				'--new', 
				'--additionals-output', 'additionals.xml']

# open netedit 
neteditApp = App.open(' '.join(neteditRoute))
				  	  
#Settings.MinSimilarity = 0.1
try:
	wait(netEditResources + "neteditIcon.png", 20)
except:
	abort("neteditIcon.png")
	
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

# Change to move
type("m")

# Move unlocked
dragDrop(Pattern(netEditResources + "neteditIcon.png").targetOffset(375, 407), Pattern(netEditResources + "neteditIcon.png").targetOffset(500, 407))
dragDrop(Pattern(netEditResources + "neteditIcon.png").targetOffset(500, 407), Pattern(netEditResources + "neteditIcon.png").targetOffset(375, 407))

# Change to inspect
type("i")

# inspect busStop
click(Pattern(netEditResources + "neteditIcon.png").targetOffset(375, 407))

# Block movement of  busStop
try:
	click(netEditResources + "additionals/editorParameters/blockMovement.png")
except:
	abort("blockMovement.png")

# Change to move
type("m")

# try to move blocked (cannot be possible)
dragDrop(Pattern(netEditResources + "neteditIcon.png").targetOffset(375, 407), Pattern(netEditResources + "neteditIcon.png").targetOffset(500, 407))

# redo block movement 
type("z", Key.CTRL)

# now can be moved 
dragDrop(Pattern(netEditResources + "neteditIcon.png").targetOffset(375, 407), Pattern(netEditResources + "neteditIcon.png").targetOffset(500, 407))

# Change to inspect
type("i")

# inspect again busStop
click(Pattern(netEditResources + "neteditIcon.png").targetOffset(500, 407))

# Block movement again of  busStop
try:
	click(netEditResources + "additionals/editorParameters/blockMovement.png")
except:
	abort("blockMovement.png")
	
# save additional
type("f", Key.ALT)
try:
	click(netEditResources + "toolbar/toolbar-file/file-saveAdditionals.png")
except:
	abort("file-saveAdditionals.png")

#quit
type("q", Key.CTRL)
try:
	find(netEditResources + "dialogs/dialog-confirmClosingNetwork.png")
	type("y", Key.ALT)
except:
	abort("dialog-confirmClosingNetwork")