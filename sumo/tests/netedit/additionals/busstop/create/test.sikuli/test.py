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
click(Pattern(netEditResources + "neteditIcon.png").targetOffset(375, 400))

#change reference to right
click(netEditResources + "additionals/editorParameters/comboBox-referenceLeft.png")
click(netEditResources + "additionals/editorParameters/referenceRight.png")

#create busstop in mode "reference right"
click(Pattern(netEditResources + "neteditIcon.png").targetOffset(475,400))

#change reference to right
click(netEditResources + "additionals/editorParameters/comboBox-referenceRight.png")
click(netEditResources + "additionals/editorParameters/referenceCenter.png")

#create busstop in mode "reference center"
click(Pattern(netEditResources + "neteditIcon.png").targetOffset(575,400))

#change default lenght to 20
doubleClick(netEditResources + "additionals/editorParameters/length-10.png")
type("20" + Key.ENTER)

#change reference to left
click(netEditResources + "additionals/editorParameters/comboBox-referenceCenter.png")
click(netEditResources + "additionals/editorParameters/referenceLeft.png")

#try to create busstop with a different lenght WITHOUT forcing position. BusStop musn't be created
click(Pattern(netEditResources + "neteditIcon.png").targetOffset(625,400))

#change reference to right
click(netEditResources + "additionals/editorParameters/comboBox-referenceRight.png")
click(netEditResources + "additionals/editorParameters/referenceCenter.png")

#try to create busstop with a different lenght WITHOUT forcing position. BusStop musn't be created
click(Pattern(netEditResources + "neteditIcon.png").targetOffset(350,400))

#change forcin position to TRUE
click(Pattern(netEditResources + "additionals/editorParameters/forcePosition.png").targetOffset(45, 0))

#change reference to left
click(netEditResources + "additionals/editorParameters/comboBox-referenceCenter.png")
click(netEditResources + "additionals/editorParameters/referenceLeft.png")

#Create busstop with a different lenght forcing position
click(Pattern(netEditResources + "neteditIcon.png").targetOffset(625,400))

#change reference to right
click(netEditResources + "additionals/editorParameters/comboBox-referenceRight.png")
click(netEditResources + "additionals/editorParameters/referenceCenter.png")

#Create busstop with a different lenght forcing position
click(Pattern(netEditResources + "neteditIcon.png").targetOffset(350,400))

# save additional
type("f", Key.ALT)
click(netEditResources + "toolbar/toolbar-file/file-saveAdditionals.png")

#quit
type("q", Key.CTRL)
wait(netEditResources + "dialogs/dialog-confirmClosingNetwork.png")
type("y", Key.ALT)


