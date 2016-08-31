import wx
import os
import subprocess
import sys
import socket

import socket

# Define global variables for Server
TRAY_TOOLTIP = 'Sikulix server'
TRAY_ICON = os.environ.get('SUMO_Home', '.') + "/tests/sikulixicon.png"
LOG_PATH = os.environ.get('SUMO_Home', '.') + "/tests/netEdit/sikulixLog.tmp"
ERROR_PATH = os.environ.get('SUMO_Home', '.') + "/tests/netEdit/sikulixError.tmp"

# function to create a menu item
def createMenuItem(menu, label, func):
	item = wx.MenuItem(menu, -1, label)
	menu.Bind(wx.EVT_MENU, func, id=item.GetId())
	menu.AppendItem(item)
	return item


def openSikulixServer():
	# open Sikulix
	subprocess.Popen([os.environ.get('SIKULIX', '.')] + ["-s"], 
					env=os.environ, stdout=open(LOG_PATH, 'w'), stderr=open(ERROR_PATH, 'w'), shell=True)
	
	#get status of sikulixServer
	statusSocket = socket.socket()
	statusSocket.connect(("localhost", 50001))
	statusSocket.send("GET / HTTP/1.1\n\n")
	statusReceived = (statusSocket.recv(1024))
	statusSocket.close()
	if "200 OK" in statusReceived:
		return True
	else:
		return False

# TaskBarIcon
class TaskBarIcon(wx.TaskBarIcon):
	#constructor
	def __init__(self):
		# construct using wx.TaskBarIcon
		super(TaskBarIcon, self).__init__()
		# set icon
		self.set_icon(TRAY_ICON)

		# try open sikulix server
		if (openSikulixServer()):
			# show ball information
			self.ShowBalloon("Running", "Sikulix server is now running", 1, wx.ICON_INFORMATION)
		else:
			# show message error and close app
			wx.MessageBox("error starting sikulix server", "ERROR")
			wx.CallAfter(self.Destroy)

	# create poput menu
	def CreatePopupMenu(self):
		menu = wx.Menu()
		createMenuItem(menu, 'check status', self.on_checkStatus)
		menu.AppendSeparator()
		createMenuItem(menu, 'Exit', self.on_exit)
		return menu

	# set icon
	def set_icon(self, path):
		icon = wx.IconFromBitmap(wx.Bitmap(path))
		self.SetIcon(icon, TRAY_TOOLTIP)

	# check sikulix status
	def on_checkStatus(self, event):
		print 'not implemented yet'
	
	# Execute when user click over exit menu
	def on_exit(self, event):
		#Create stop socket
		stopSocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
		#Connect to localhost throught port 50001 (Default port of SikuliX)
		stopSocket.connect(("localhost", 50001))
		#Send stop command
		stopSocket.send("STOP")
		#Close stop socket
		stopSocket.close()
		#Close taskbar icon and app
		wx.CallAfter(self.Destroy)

# SingleApp
class SingleApp(wx.App):
	def OnInit(self):
		# give unique name to app
		self.name = "SikulixServer"
		# get a single instance of 
		self.instance = wx.SingleInstanceChecker(self.name)
		#only create a TaskBarIcon app if currently there isn't another running
		if self.instance.IsAnotherRunning() == False:
			TaskBarIcon()
		#return true
		return True

# Main function. Will be called before every test
def main():
	# create a single app function
	app = SingleApp()
	# wait in the loop
	app.MainLoop()

if __name__ == '__main__':
    main()