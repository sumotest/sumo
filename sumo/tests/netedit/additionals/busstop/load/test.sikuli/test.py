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
                  '--sumo-net-file', 'input_net.net.xml',
                  '--sumo-additionals-file', 'input_additionals.add.xml',
                  '--additionals-output', 'additionals.xml'], 
                  env=os.environ, stdout=sys.stdout, stderr=sys.stderr)

#Settings.MinSimilarity = 0.1
wait(netEditResources + "neteditIcon.png")

# save additional
type("f", Key.ALT)
click(netEditResources + "toolbar/toolbar-file/file-saveAdditionals.png")

#quit
type("q", Key.CTRL)
wait(netEditResources + "dialogs/dialog-confirmClosingNetwork.png")
type("y", Key.ALT)


