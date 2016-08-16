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

# Create first two nodes
click(Pattern(netEditResources + "neteditIcon.png").targetOffset(200,200))
click(Pattern(netEditResources + "neteditIcon.png").targetOffset(600,200))

# Create second two another
click(Pattern(netEditResources + "neteditIcon.png").targetOffset(200,300))
click(Pattern(netEditResources + "neteditIcon.png").targetOffset(600,300))

# Change to inspect
type("i")

# duplicate lane in the first nodes
rightClick(Pattern(netEditResources + "neteditIcon.png").targetOffset(300,205))
try:
	click(netEditResources + "netElements/edges/menu-duplicateLane.png")
except:
	abort(netEditProcess, "menu-duplicateLane.png")

# Change to create additional
type("a")

# by default, additional is busstop, then isn't needed to select "busstop"

#change reference to center
try:
	click(netEditResources + "additionals/editorParameters/comboBox-referenceRight.png")
	click(netEditResources + "additionals/editorParameters/referenceCenter.png")
except:
	abort(netEditProcess, "comboBox-referenceRight.png or referenceCenter.png")

#create busstop in mode "reference center" in the first two nodes
click(Pattern(netEditResources + "neteditIcon.png").targetOffset(500,210))

#create busstop in mode "reference center" in the second two nodes
click(Pattern(netEditResources + "neteditIcon.png").targetOffset(400,310))

# Change to inspect
type("i")

#inspect busStop
click(Pattern(netEditResources + "neteditIcon.png").targetOffset(400,315))

#Change parameter 1 with a non valid value (Duplicated ID)
try:
	doubleClick(Pattern(netEditResources + "additionals/busstop/parameter-id.png").targetOffset(75,0))
except:
	abort(netEditProcess, "parameter-id.png")
type("busStop_gneE0_1_0" + Key.ENTER)

#Change parameter 1 with a valid value
try:
	doubleClick(Pattern(netEditResources + "additionals/busstop/parameter-id.png").targetOffset(75,0))
except:
	abort(netEditProcess, "parameter-id.png")
type("correct ID" + Key.ENTER)

#Change parameter 2 with a non valid value (dummy lane)
try:
	doubleClick(Pattern(netEditResources + "additionals/busstop/parameter-lane.png").targetOffset(75,0))
except:
	abort(netEditProcess, "parameter-lane.png")
type("dummyLane" + Key.ENTER)

#Change parameter 2 with a valid value (different edge)
try:
	doubleClick(Pattern(netEditResources + "additionals/busstop/parameter-lane.png").targetOffset(75,0))
except:
	abort(netEditProcess, "parameter-lane.png")
type("gneE0_0" + Key.ENTER)

#Change parameter 2 with a valid value (same edge, different lane)
try:
	doubleClick(Pattern(netEditResources + "additionals/busstop/parameter-lane.png").targetOffset(75,0))
except:
	abort(netEditProcess, "parameter-lane.png")
type("gneE0_1" + Key.ENTER)

#Change parameter 3 with a non valid value (negative)
try:
	doubleClick(Pattern(netEditResources + "additionals/busstop/parameter-startPos.png").targetOffset(75,0))
except:
	abort(netEditProcess, "parameter-startPos.png")
type("-5" + Key.ENTER)

#Change parameter 3 with a non valid value (> endPos)
try:
	doubleClick(Pattern(netEditResources + "additionals/busstop/parameter-startPos.png").targetOffset(75,0))
except:
	abort(netEditProcess, "parameter-startPos.png")
type("60" + Key.ENTER)

#Change parameter 3 with a valid value
try:
	doubleClick(Pattern(netEditResources + "additionals/busstop/parameter-startPos.png").targetOffset(75,0))
except:
	abort(netEditProcess, "parameter-startPos.png")
type("20" + Key.ENTER)

#Change parameter 4 with a non valid value (out of range)
try:
	doubleClick(Pattern(netEditResources + "additionals/busstop/parameter-endPos.png").targetOffset(75,0))
except:
	abort(netEditProcess, "parameter-endPos.png")
type("3000" + Key.ENTER)

#Change parameter 4 with a non valid value (<startPos)
try:
	doubleClick(Pattern(netEditResources + "additionals/busstop/parameter-endPos.png").targetOffset(75,0))
except:
	abort(netEditProcess, "parameter-endPos.png")
type("10" + Key.ENTER)

#Change parameter 4 with a valid value
try:
	doubleClick(Pattern(netEditResources + "additionals/busstop/parameter-endPos.png").targetOffset(75,0))
except:
	abort(netEditProcess, "parameter-endPos.png")
type("30" + Key.ENTER)

#Change parameter 5 with a non valid value (throw warning)
try:
	doubleClick(Pattern(netEditResources + "additionals/busstop/parameter-lines.png").targetOffset(75,0))
except:
	abort(netEditProcess, "parameter-lines.png")
type("line1, line2" + Key.ENTER)

#Change parameter 5 with a valid value
try:
	doubleClick(Pattern(netEditResources + "additionals/busstop/parameter-lines.png").targetOffset(75,0))
except:
	abort(netEditProcess, "parameter-lines.png")
type("line1 line2" + Key.ENTER)

#go to a empty area
click(Pattern(netEditResources + "neteditIcon.png").targetOffset(350,350))

#Check UndoRedo (8 attribute changes, 1 creation)
for x in range(0, 9):
    undo(netEditProcess)
	
for x in range(0, 9):
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