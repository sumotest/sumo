# Open netEdit using console window
Settings.MoveMouseDelay = 0.1
Settings.DelayBeforeDrop = 0.1
Settings.DelayAfterDrag = 0.1

# I assume that we'll execute the test in %SUMO%/test/netEdit/<NameOfTest>, then the command
# will be changed to netEditOpenCommand = r'../../../bin/netEdit.exe'
netEditOpenCommand = r'c:/Proyectos/SUMO_Oficial/bin/netEdit.exe'

openApp(netEditOpenCommand)

wait("1469712155344.png")

# focus
click(Pattern("1469711973965.png").targetOffset(183,2))


# Create new project
type("a", Key.CTRL)

# Change to create mode
type("e")


# Create two nodes
click(Pattern("1469711973965.png").targetOffset(243,385))
click(Pattern("1469711973965.png").targetOffset(666,423))

# Close netEdit
type("q", Key.CTRL)
# discard changes
type("y", Key.ALT)




