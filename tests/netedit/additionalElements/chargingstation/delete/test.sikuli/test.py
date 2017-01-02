# import common functions for netedit tests
import os
import sys

testRoot = os.path.join(os.environ.get('SUMO_HOME', '.'), 'tests')
neteditTestRoot = os.path.join(os.environ.get('TEXTTEST_HOME', testRoot), 'netedit')
sys.path.append(neteditTestRoot)
import neteditTestFunctions as netedit

# Open netedit
neteditProcess, match = netedit.setupAndStart(neteditTestRoot, False)

# Change to create additional
netedit.additionalMode()

# obtain match for comboboxAdditional
comboboxAdditional = netedit.getComboBoxAdditional(match)

# select chargingStation
netedit.changeAdditional(comboboxAdditional, 2)

# create chargingStation in mode "reference left"
netedit.leftClick(match, 250, 250)

# Change to delete
netedit.deleteMode()

# delete created chargingStation
netedit.leftClick(match, 260, 250)

# delete loaded chargingStation
netedit.leftClick(match, 450, 250)

# Check undo redo
netedit.undo(match, 2)
netedit.redo(match, 2)

# save additionals
netedit.saveAdditionals()

# save newtork
netedit.saveNetwork()

# quit netedit 
netedit.quit(neteditProcess, False, False)
