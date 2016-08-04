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
                  '-o', 'net.net.xml'], 
                  env=os.environ, stdout=sys.stdout, stderr=sys.stderr)

#Settings.MinSimilarity = 0.1
wait(netEditResources + "neteditIcon.png")

# 1 - Check Shorcuts of toolbar

# 1.1 - File
type("f", Key.ALT)
wait(netEditResources + "toolbar/toolbar-file-selected.png")
type(Key.ESC)
wait(netEditResources + "toolbar/toolbar-file.png")

# 1.2 - Edit
type("e", Key.ALT)
wait(netEditResources + "toolbar/toolbar-edit-selected.png")
type(Key.ESC)
wait(netEditResources + "toolbar/toolbar-edit.png")

# 1.3 - Processing (NOT READY)
#type("g", Key.ALT)
#wait(netEditResources + "toolbar/toolbar-processing-selected.png")
#type(Key.ESC)
#wait(netEditResources + "toolbar/toolbar-processing.png")

# 1.4 - Locate
type("l", Key.ALT)
wait(netEditResources + "toolbar/toolbar-locate-selected.png")
type(Key.ESC)
wait(netEditResources + "toolbar/toolbar-locate.png")

# 1.5 - Windows
type("w", Key.ALT)
wait(netEditResources + "toolbar/toolbar-windows-selected.png")
type(Key.ESC)
wait(netEditResources + "toolbar/toolbar-windows.png")

# 1.6 - Help
type("h", Key.ALT)
wait(netEditResources + "toolbar/toolbar-help-selected.png")
type(Key.ESC)
wait(netEditResources + "toolbar/toolbar-help.png")

# quit
type("q", Key.CTRL)