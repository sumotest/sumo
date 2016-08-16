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

#change reference to center
try:
	click(netEditResources + "additionals/editorParameters/comboBox-referenceRight.png")
	click(netEditResources + "additionals/editorParameters/referenceCenter.png")
except:
	abort(netEditProcess, "comboBox-referenceRight.png or referenceCenter.png")
	
#create busstop in mode "reference center"
click(Pattern(netEditResources + "neteditIcon.png").targetOffset(400,400))

# Change to delete
type("d")

#delete busStop
click(Pattern(netEditResources + "neteditIcon.png").targetOffset(400,405))

#Check UndoRedo
undo(netEditProcess)
redo(netEditProcess)

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