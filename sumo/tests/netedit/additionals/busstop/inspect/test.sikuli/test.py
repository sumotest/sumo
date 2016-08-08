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
click(netEditResources + "netElements/edges/menu-duplicateLane.png")

# Change to create additional
type("a")

# by default, additional is busstop, then isn't needed to select "busstop"

#change reference to center
click(netEditResources + "additionals/editorParameters/comboBox-referenceRight.png")
click(netEditResources + "additionals/editorParameters/referenceCenter.png")

#create busstop in mode "reference center" in the first two nodes
click(Pattern(netEditResources + "neteditIcon.png").targetOffset(500,210))

#create busstop in mode "reference center" in the second two nodes
click(Pattern(netEditResources + "neteditIcon.png").targetOffset(400,310))

# Change to inspect
type("i")

#inspect busStop
click(Pattern(netEditResources + "neteditIcon.png").targetOffset(400,315))

#Change parameter 1 with a non valid value (Duplicated ID)
doubleClick(Pattern(netEditResources + "additionals/busstop/parameter-id.png").targetOffset(75,0))
type("busStop_gneE0_1_0" + Key.ENTER)

#Change parameter 1 with a valid value
doubleClick(Pattern(netEditResources + "additionals/busstop/parameter-id.png").targetOffset(75,0))
type("correct ID" + Key.ENTER)

#Change parameter 2 with a non valid value (dummy lane)
doubleClick(Pattern(netEditResources + "additionals/busstop/parameter-lane.png").targetOffset(75,0))
type("dummyLane" + Key.ENTER)

#Change parameter 2 with a valid value (different edge)
doubleClick(Pattern(netEditResources + "additionals/busstop/parameter-lane.png").targetOffset(75,0))
type("gneE0_0" + Key.ENTER)

#Change parameter 2 with a valid value (same edge, different lane)
doubleClick(Pattern(netEditResources + "additionals/busstop/parameter-lane.png").targetOffset(75,0))
type("gneE0_1" + Key.ENTER)

#Change parameter 3 with a non valid value (negative)
doubleClick(Pattern(netEditResources + "additionals/busstop/parameter-startPos.png").targetOffset(75,0))
type("-5" + Key.ENTER)

#Change parameter 3 with a non valid value (> endPos)
doubleClick(Pattern(netEditResources + "additionals/busstop/parameter-startPos.png").targetOffset(75,0))
type("60" + Key.ENTER)

#Change parameter 3 with a valid value
doubleClick(Pattern(netEditResources + "additionals/busstop/parameter-startPos.png").targetOffset(75,0))
type("20" + Key.ENTER)

#Change parameter 4 with a non valid value (out of range)
doubleClick(Pattern(netEditResources + "additionals/busstop/parameter-endPos.png").targetOffset(75,0))
type("3000" + Key.ENTER)

#Change parameter 4 with a non valid value (<startPos)
doubleClick(Pattern(netEditResources + "additionals/busstop/parameter-endPos.png").targetOffset(75,0))
type("10" + Key.ENTER)

#Change parameter 4 with a valid value
doubleClick(Pattern(netEditResources + "additionals/busstop/parameter-endPos.png").targetOffset(75,0))
type("30" + Key.ENTER)

#Change parameter 5 with a non valid value (throw warning)
doubleClick(Pattern(netEditResources + "additionals/busstop/parameter-lines.png").targetOffset(75,0))
type("line1, line2" + Key.ENTER)

#Change parameter 5 with a valid value
doubleClick(Pattern(netEditResources + "additionals/busstop/parameter-lines.png").targetOffset(75,0))
type("line1 line2" + Key.ENTER)

#go to a empty area
click(Pattern(netEditResources + "neteditIcon.png").targetOffset(350,350))

#Check UndoRedo (5 attributes -> 5 times)
#type("z", Key.CTRL)
#type("z", Key.CTRL)
#type("z", Key.CTRL)
#type("z", Key.CTRL)
#type("z", Key.CTRL)
#type("y", Key.CTRL)
#type("y", Key.CTRL)
#type("y", Key.CTRL)
#type("y", Key.CTRL)
#type("y", Key.CTRL)

#save additional
type("f", Key.ALT)
click(netEditResources + "toolbar/toolbar-file/file-saveAdditionals.png")

#quit
type("q", Key.CTRL)
wait(netEditResources + "dialogs/dialog-confirmClosingNetwork.png")
type("y", Key.ALT)