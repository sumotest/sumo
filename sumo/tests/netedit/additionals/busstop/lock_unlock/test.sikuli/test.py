#** Common parameters **#
Settings.MoveMouseDelay = 0.1
Settings.DelayBeforeDrop = 0.1
Settings.DelayAfterDrag = 0.1
netEditResources = os.environ['SUMO_HOME'] + "/tests/netedit/imageResources/"

# abort function
def abort(process, reason): 
	process.kill()
	sys.exit("Killed netedit process. '" + reason + "' not found")

# undo operation
def undo(netEditProcess):
	try:
		click(netEditResources + "toolbar/toolbar-edit.png")
		click(netEditResources + "toolbar/toolbar-edit/edit-undo.png")
	except:
		abort(netEditProcess, "edit-undo.png")
		
# redo operation
def redo(netEditProcess):
	try:
		click(netEditResources + "toolbar/toolbar-edit.png")
		click(netEditResources + "toolbar/toolbar-edit/edit-redo.png")
	except:
		abort(netEditProcess, "edit-redo.png")
#****#

# Import libraries
import os, sys, subprocess

#Open netedit
netEditProcess = subprocess.Popen([os.environ['NETEDIT_BINARY'], 
								  '--window-size', '800,600',
								  '--new', 
								  '--additionals-output', 'additionals.xml'], 
								  env=os.environ, stdout=sys.stdout, stderr=sys.stderr)

#Settings.MinSimilarity = 0.1
try:
	wait(netEditResources + "neteditIcon.png", 60)
except:
	abort(netEditProcess, "neteditIcon.png")
	
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

# Change mouse move delay
Settings.MoveMouseDelay = 1

# Move unlocked
dragDrop(Pattern(netEditResources + "neteditIcon.png").targetOffset(375, 407), Pattern(netEditResources + "neteditIcon.png").targetOffset(500, 407))
dragDrop(Pattern(netEditResources + "neteditIcon.png").targetOffset(500, 407), Pattern(netEditResources + "neteditIcon.png").targetOffset(375, 407))

# Change back mouse move delay
Settings.MoveMouseDelay = 0.1

# Change to inspect
type("i")

# inspect busStop
click(Pattern(netEditResources + "neteditIcon.png").targetOffset(375, 407))

# Block movement of  busStop
try:
	click(netEditResources + "additionals/editorParameters/blockMovement.png")
except:
	abort(netEditProcess, "blockMovement.png")
	
# Change to move
type("m")

# Change mouse move delay
Settings.MoveMouseDelay = 1

# try to move blocked (cannot be possible)
dragDrop(Pattern(netEditResources + "neteditIcon.png").targetOffset(375, 407), Pattern(netEditResources + "neteditIcon.png").targetOffset(500, 407))

# Change back mouse move delay
Settings.MoveMouseDelay = 0.1

# undo block movement again
undo(netEditProcess)

# Change mouse move delay
Settings.MoveMouseDelay = 1

# now can be moved 
dragDrop(Pattern(netEditResources + "neteditIcon.png").targetOffset(375, 407), Pattern(netEditResources + "neteditIcon.png").targetOffset(500, 407))

# Change back mouse move delay
Settings.MoveMouseDelay = 0.1

# Change to inspect
type("i")

# inspect again busStop
click(Pattern(netEditResources + "neteditIcon.png").targetOffset(500, 407))

# Block movement again of  busStop
try:
	click(netEditResources + "additionals/editorParameters/blockMovement.png")
except:
	abort(netEditProcess, "blockMovement.png")
	
# save additional
type("f", Key.ALT)
try:
	click(netEditResources + "toolbar/toolbar-file/file-saveAdditionals.png")
except:
	abort(netEditProcess, "file-saveAdditionals.png")

#quit
type("q", Key.CTRL)
try:
	find(netEditResources + "dialogs/dialog-confirmClosingNetwork.png")
	type("y", Key.ALT)
except:
	abort(netEditProcess, "dialog-confirmClosingNetwork")