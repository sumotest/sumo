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

# Change to movement mode
type("m")

# Change mouse move delay
Settings.MoveMouseDelay = 1

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

# Change back mouse move delay
Settings.MoveMouseDelay = 0.1

#Check UndoRedo (10 movements)
for x in range(0, 10):
    undo(netEditProcess)
	
for x in range(0, 10):
    redo(netEditProcess)
	
# Change to inspect
type("i")

# inspect busStop
click(Pattern(netEditResources + "neteditIcon.png").targetOffset(375,407))

# Change parameter startPos
try:
	doubleClick(Pattern(netEditResources + "additionals/busstop/parameter-startPos.png").targetOffset(75,0))
except:
	abort(netEditProcess, "parameter-startPos.png")
type("13.79" + Key.ENTER)

# Change parameter endPos
try:
	doubleClick(Pattern(netEditResources + "additionals/busstop/parameter-endPos.png").targetOffset(75,0))
except:
	abort(netEditProcess, "parameter-endPos.png")
type("23.79" + Key.ENTER)

# save additional
try:
	click(netEditResources + "toolbar/toolbar-file.png")
	click(netEditResources + "toolbar/toolbar-file/file-saveAdditionals.png")
except:
	abort(netEditProcess, "file-saveAdditionals.png or toolbar-file.png")

#quit
type("q", Key.CTRL)
try:
	find(netEditResources + "dialogs/dialog-confirmClosingNetwork.png")
	type("y", Key.ALT)
except:
	abort(netEditProcess, "dialog-confirmClosingNetwork")