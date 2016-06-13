#***********************************************************************
 #  This code is part of CmplServer 
 #
 #  Copyright (C) 2013, 2014
 #  Mike Steglich - Technical University of Applied Sciences
 #  Wildau, Germany 
 #
 #  CmplServer is a project of the Technical University of 
 #  Applied Sciences Wildau and the Institute for Operations Research 
 #  and Business Management at the Martin Luther University 
 #  Halle-Wittenberg.
 #  Please visit the project homepage <www.coliop.org>
 # 
 #  CmplServer is free software; you can redistribute it and/or modify it 
 #  under the terms of the GNU Lesser General Public License as published by 
 #  the Free Software Foundation; either version 3 of the License, or 
 #  (at your option) any later version.
 # 
 #  CmplServer is distributed in the hope that it will be useful, but WITHOUT
 #  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 #  or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public 
 #  License for more details.
 #  
 #  You should have received a copy of the GNU Lesser General Public License
 #  along with this program; if not, see <http://www.gnu.org/licenses/>.
 #
 #**********************************************************************


#!/usr/bin/python 

import clr
clr.AddReference('System.Windows.Forms')
clr.AddReference('System.Drawing')

from System.Windows.Forms import  Form, Label, TabAlignment, TabControl, TabPage, DockStyle, Panel, ToolStripMenuItem, Button, TextBox, FormBorderStyle,FolderBrowserDialog,DialogResult,MessageBox,MessageBoxButtons, ScrollBars
from System.Drawing import Point, Font

from System.IO import FileStream, FileMode

import sys
import os
import thread 



import xmlrpclib
import socket
import threading


#************* pyCmpl and cmplserver includes ****************
try:
	cmplPath=os.environ['CMPLPATH']	
except:
	raise Exception("Internal error - cannot find CMPL path") 

cmplbin=cmplPath+os.sep+"bin"+os.sep+"cmpl.exe"
os.chdir(cmplPath)
os.environ.update({'CMPLBINARY':cmplbin })

cmplServerPath=cmplPath+os.sep+"cmplServer"
os.environ.update({'CMPLSERVERPATH':cmplServerPath })

sys.path.append(cmplPath)

from cmplServer import *
from pyCmpl.CmplDefs import *
#************* end pyCmpl and cmplserver includes ****************


#************* CmplServerHandler  ********************************
class CmplServerHandler():
	
	#*********** constructor ************
	def __init__(self):
		self.__serverMode = STANDALONE_SERVER
		self.__port=8008
		self.__grid=False
		self.__getPort()
		self.__ipyPath=os.path.dirname(os.path.abspath(sys.argv[0])) + os.sep + ".." + os.sep + ".." + os.sep+ "IronPython"+os.sep+"ipyw.exe"
		self.__serverScript=os.path.dirname(os.path.abspath(sys.argv[0])) +os.sep +"cmplServerHandler.py"
	#*********** end constructor *********
			
	
	#*********** gridMode ****************
	@property
	def gridMode(self):
		self.__getPort()
		return self.__grid
	#*********** end gridMode ************
	
	
	#*********** port ********************
	@property
	def port(self):
		return self.__port
	#*********** port ********************
	
	
	#*********** getport *****************
	def __getPort(self):
		try:
			self.__optFileName=os.environ['CMPLSERVERPATH']+ os.sep + "cmplServer.opt"	
		except:
			self.__optFileName=os.path.dirname(os.path.abspath(sys.argv[0])) + os.sep + ".." + os.sep + "cmplServer.opt"
		
		self.__grid=False
		
		try:	
			f = open(self.__optFileName, "r") 
			for line in f: 
				ret=line.split("=")
				
				if ret[0].strip().lower() == "cmplserverport":
					if CmplTools.strIsNumber(ret[1].strip()):
						self.__port = int(ret[1].strip())
	
					else:
						MessageBox.Show("Wrong option maxProblems in CmplServer option file <"+self.__optFileName+"> default value is used" )
						self.__port=8008
					continue	
					
				if ret[0].strip().lower() == "cmplgridscheduler":
					self.__grid=True
					
				
			f.close()
		except IOError, e:
			MessageBox.Show("Cannot read CmplServer option file <"+self.__optFileName+"> "  )
	#*********** end getport *************
			
	
	#*********** startServer *************	
	def startCmplServer(self):
		self.__getPort()
		if not self.__grid:
			os.popen(self.__ipyPath+" "+self.__serverScript+ " -start " + str(self.__port) )
		else:
			os.popen(self.__ipyPath+" "+self.__serverScript+ " -startInGrid " + str(self.__port) )
	#*********** end startServer  *********
	
	#*********** startScheduler *************	
	def startScheduler(self):
		self.__getPort()
		os.popen(self.__ipyPath+" "+self.__serverScript+ " -startScheduler " + str(self.__port))
	#*********** end startScheduler  *********
	
	#*********** stopServer ***************		
	def stopServer(self):
		self.__getPort()
		os.popen(self.__ipyPath+" "+self.__serverScript+ " -stop " + str(self.__port))
		
	#*********** end stopServer ***********
	
	#*********** status ****************		
	def status(self):
		self.__getPort()
		url = "http://127.0.0.1:" +str(self.__port)
		
		ret=None
		try:
			cmplServer = xmlrpclib.ServerProxy(url)
			ret = cmplServer.status()[0]
			
			if ret==None:
				ret=CMPL_UNKNOWN 
		except:
			ret=CMPL_UNKNOWN 
	
		return ret
	#*********** end status ************		
		
#************* end CmplServerHandler  ****************************



#************* CMPLServerUI  *************************************
class CMPLServerUI(Form):
  
	#*************** constructor *****************************
  	def __init__(self):
		
		
		
		self.__serverHandler=None
		
		self.__cmplServerPath = os.path.expanduser("~") + os.sep+ "CmplServer" +os.sep
		self.__logFileName = self.__cmplServerPath + "cmplServer.log"
		self.__schedulerLogFileName = self.__cmplServerPath + "cmplGridScheduler.log"
		
		if os.path.exists(self.__cmplServerPath) == False:
			try:
				os.mkdir(self.__cmplServerPath)
			except OSError, e:
				MessageBox.Show("Cannot create CmplServer path <"+self.__cmplServerPath+">"  )
				sys.exit()
		try:
			f1 = open(self.__logFileName, "a")
			f2 = open(self.__schedulerLogFileName, "a")
			f1.close()
			f2.close()
			
			self.__logFile=open(self.__logFileName, "r")
			self.__logFile.seek(0, 2)
			self.__schedulerLogFile=open(self.__schedulerLogFileName, "r")
			self.__schedulerLogFile.seek(0, 2)
			
			
		except IOError, e:
			MessageBox.Show( "Cannot read CmplServer log file <"+self.__logFile+"> " + str(e) )
			sys.exit()
		
		
			
		self.__stopLogging=False
			
		self.Text = "CMPLServer"
		self.Height=500
		self.Width = 950

		self.infoLabel=Label()
		self.infoLabel.Location = Point(10,10)
		self.infoLabel.Width = 600
		self.infoLabel.Font=Font("Monaco",10)
	
		self.bStart=Button()
		self.bStart.Text = "Start Server"
		self.bStart.Location = Point(10,50)
		self.bStart.Width = 150
		self.bStart.Height = 35
		self.bStart.Click += self.startServer
	
	
		self.bStartScheduler=Button()
		self.bStartScheduler.Text = "Start Scheduler"
		self.bStartScheduler.Location = Point(170,50)
		self.bStartScheduler.Width = 150
		self.bStartScheduler.Height = 35
		self.bStartScheduler.Click += self.startScheduler
		
	
		self.bStop=Button()
		self.bStop.Text = "Stop"
		self.bStop.Location = Point(330,50)
		self.bStop.Width = 150
		self.bStop.Height = 35
		self.bStop.Click += self.stopServer
		self.bStop.Enabled = False
		
		self.__serverHandler=CmplServerHandler()
		if self.__serverHandler.status()!=CMPL_UNKNOWN and self.__serverHandler.status()!=CMPLSERVER_ERROR and self.__serverHandler.status()!=CMPLGRID_SCHEDULER_ERROR:
			self.bStop.Enabled = True
			self.bStartScheduler.Enabled = False
			self.bStart.Enabled = False
			if not self.__serverHandler.gridMode:
				self.infoLabel.Text="CmplServer is running under http://"+socket.gethostbyname(socket.gethostname())+":"+str(self.__serverHandler.port)
			else:
				self.infoLabel.Text="CmplServer is running CMPLGrid"
		else:
			self.bStop.Enabled = False
			self.bStartScheduler.Enabled = True
			self.bStart.Enabled = True
			self.infoLabel.Text="CmplServer is not running"
	
			
		self.AcceptButton = self.bStart
		self.AcceptButton = self.bStartScheduler
		self.AcceptButton = self.bStop
	
	
		self.buttonPanel=Panel()
		self.buttonPanel.Dock=DockStyle.Bottom
		self.buttonPanel.Height=100
		self.buttonPanel.Controls.Add(self.infoLabel)
		self.buttonPanel.Controls.Add(self.bStart)
		self.buttonPanel.Controls.Add(self.bStartScheduler)
		self.buttonPanel.Controls.Add(self.bStop)
	
	
		self.tabControl = TabControl()
		self.tabControl.Location = Point(10,60)
		self.tabControl.Dock = DockStyle.Fill 
				
		self.Controls.Add(self.tabControl)
		self.addTabPage("Server log")
		self.addTabPage("Scheduler log")
		self.addTabPage("Options")
		self.Controls.Add(self.buttonPanel)
		
		self.readOptionFile()
		
		self.CenterToScreen()
	
		self.Closing += self.onExit
		
		self.tabControl.TabPages[0].Controls[0].Font=Font("Courier New",8)
		self.tabControl.TabPages[1].Controls[0].Font=Font("Courier New",8)
		
		thread.start_new_thread(self.storeOutput, (0,) )
		thread.start_new_thread(self.storeOutput, (1,) )
		
	#*************** end constructor **************************
	
	#*************** addTabPage *******************************	
 	def addTabPage(self, label, text=""): 
		textBox = TextBox()	
		textBox.Multiline = True
		textBox.Dock = DockStyle.Fill 
		textBox.ScrollBars = ScrollBars.Vertical 
		textBox.AcceptsReturn = True 
		textBox.AcceptsTab = True 
		textBox.WordWrap = True
		textBox.Font=Font("Courier New",10)
		textBox.Text = text
		
		tPage = TabPage() 
		tPage.Text = label
		tPage.Controls.Add(textBox)
		self.tabControl.TabPages.Add(tPage)
	#*************** end addTabPage ****************************	
      	
	
	#*************** startServer *******************************	
	def startServer(self,key,e):
		self.infoLabel.Text="starting CmplServer ..."
		self.tabControl.TabPages[2].Controls[0].Enabled=False
		self.writeOptionFile()
		
		self.bStartScheduler.Enabled = False
		self.bStart.Enabled = False
		
		self.tabControl.SelectTab(0)
		
		self.__serverHandler.startCmplServer()
		
		tries=0
		while self.__serverHandler.status()==CMPL_UNKNOWN:
			time.sleep(1)
			tries+=1
			if tries>90:
				break
				
		if self.__serverHandler.status()!=CMPL_UNKNOWN and self.__serverHandler.status()!=CMPLSERVER_ERROR and self.__serverHandler.status()!=CMPLGRID_SCHEDULER_ERROR:
		
			if not self.__serverHandler.gridMode:
				self.infoLabel.Text="CmplServer is running under http://"+socket.gethostbyname(socket.gethostname())+":"+str(self.__serverHandler.port)
			else:
				self.infoLabel.Text="CmplServer is running CMPLGrid"
			self.bStop.Enabled = True
			self.bStartScheduler.Enabled = False
			self.bStart.Enabled = False
			self.tabControl.TabPages[2].Controls[0].Enabled=False
			self.__stopLogging=False
			
			
		else:
			self.infoLabel.Text+="failed ... "
			self.tabControl.TabPages[2].Controls[0].Enabled=True
			self.bStartScheduler.Enabled = True
			self.bStart.Enabled = True
			self.infoLabel.Text+="CmplServer is not running"
	#*************** startServer *******************************
	
  
	#*************** startScheduler ****************************	
  	def startScheduler(self, key,e):
		self.infoLabel.Text="starting CmplGridScheduler ..."
		self.tabControl.TabPages[2].Controls[0].Enabled=False
		self.writeOptionFile()
		
		self.bStartScheduler.Enabled = False
		self.bStart.Enabled = False
		
		self.tabControl.SelectTab(1)
		
		self.__serverHandler.startScheduler()
		
		tries=0
		while self.__serverHandler.status()!=CMPLGRID_SCHEDULER_OK:
			time.sleep(1)
			tries+=1
			if tries>90:
				break
				
		if self.__serverHandler.status()!=CMPL_UNKNOWN and self.__serverHandler.status()!=CMPLSERVER_ERROR and self.__serverHandler.status()!=CMPLGRID_SCHEDULER_ERROR:
			self.infoLabel.Text="CmplGridScheduler is running under http://"+socket.gethostbyname(socket.gethostname())+":"+str(self.__serverHandler.port)
			self.bStop.Enabled = True
			self.bStartScheduler.Enabled = False
			self.bStart.Enabled = False
			self.tabControl.TabPages[2].Controls[0].Enabled=False
			self.__stopLogging=False
			
		else:
 			self.infoLabel.Text+="failed ... "
			self.tabControl.TabPages[2].Controls[0].Enabled=True
			self.bStartScheduler.Enabled = True
			self.bStart.Enabled = True
			self.infoLabel.Text+="CmplServer is not running"
	#*************** end startScheduler ************************
	
	
	#*************** stopServerAction **************************	
	def stopServerAction(self):
		if self.__serverHandler.status()!=CMPL_UNKNOWN :
			self.__serverHandler.stopServer()
			time.sleep(2)
			#self.__stopLogging=True
			self.infoLabel.Text="CmplServer is not running"
			self.bStartScheduler.Enabled = True
			self.bStart.Enabled = True
			self.bStop.Enabled = False
			self.tabControl.TabPages[2].Controls[0].Enabled=True
	#*************** end stopServerAction **********************
	
		
	#*************** stopServer ********************************		
	def stopServer(self,key,e):
		self.stopServerAction()
	#*************** end stopServer ****************************
	
	
	#*************** readOptionFile ****************************			
	def readOptionFile(self):
		fContent=""
		try:
			self.__optFileName=os.environ['CMPLSERVERPATH']+ os.sep + "cmplServer.opt"	
		except:
			self.__optFileName=os.path.dirname(os.path.abspath(sys.argv[0])) + os.sep + ".." + os.sep + "cmplServer.opt"
		
		try:	
			f = open(self.__optFileName, 'r')
			fContent = f.read()
			f.close()
			self.tabControl.TabPages[2].Controls[0].Text=fContent
			
		except Exception,e :
			MessageBox.Show("Cannot read CmplServer option file <"+self.__optFileName+"> " + str(e)  )
			sys.exit()
	#*************** end readOptionFile ************************
	
			
	#*************** writeOptionFile ***************************	
	def writeOptionFile(self):
		fContent=""
		try:
			self.__optFileName=os.environ['CMPLSERVERPATH']+ os.sep + "cmplServer.opt"	
		except:
			self.__optFileName=os.path.dirname(os.path.abspath(sys.argv[0])) + os.sep + ".." + os.sep + "cmplServer.opt"
		
		try:	
			f = open(self.__optFileName, 'w')
			fContent=self.tabControl.TabPages[2].Controls[0].Text
			f.write(fContent)
			f.close()
			
			
		except Exception,e :
			MessageBox.Show("Cannot write CmplServer option file <"+self.__optFileName+"> " + str(e)  )
			sys.exit()
	#*************** end writeOptionFile ***********************	
			
	#*********** storeOutput ***********************************
	def storeOutput(self, tabId):
		try:
			#logFile = open(fName, "r")
			#logFile.seek(-250, 2)
		
			while True:
				if tabId==0:
					line = self.__logFile.readline()
				else:
					line = self.__schedulerLogFile.readline()
					
				if len(line)>0 and not self.__stopLogging:
					self.tabControl.TabPages[tabId].Controls[0].Text+=line+"\r\n"
					self.tabControl.TabPages[tabId].Controls[0].SelectionStart=self.tabControl.TabPages[0].Controls[0].Text.Length
					self.tabControl.TabPages[tabId].Controls[0].ScrollToCaret()
					
			#logFile.close()
		except:
			self.tabControl.TabPages[tabId].Controls[0].Text="Error - Cannot read log file" +str(sys.exc_info()[1]) 
		
	#*********** end __storeOutput ****************************
			
	def onExit(self, key,e ):
		self.stopServerAction()
		
#************* end CMPLServerUI  ********************************** 
 
 
 
#************* main   *********************************************
# A File is locked to ensure that the UI is only oppened one time.
try:
	f = FileStream( cmplPath+os.sep+'AUTHORS.txt', FileMode.Open)
	f.Lock(0, f.Length)
except:
	MessageBox.Show("The CmplServer console can be opened only once." )
	sys.exit()
	
ui=CMPLServerUI()
ui.ShowDialog()

f.Unlock(0, f.Length)
f.Close()
#************* main   ********************************************* 