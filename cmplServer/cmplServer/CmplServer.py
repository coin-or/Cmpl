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

from __future__ import division

"CMPLServer and CMPLGrid"
__author__ = "Mike Steglich"
__copyright__ = "Copyright (C) 2013,2014, 2015, 2016 Mike Steglich"
__license__ = "LGPLv3"
__version__ = "1.3.2"



from pyCmpl.CmplDefs import *
from pyCmpl.CmplInstance import *
from pyCmpl.CmplMsg import *
from pyCmpl.CmplSolution import *
from pyCmpl.CmplTools import *

from CmplServerTools import *
from CmplServerException import *

import xmlrpclib
import thread 
import threading
import random
import os
import sys
import subprocess
import time
import socket
import cStringIO


#################################################################################
#										#
# CmplProblemHandler								#
#										#
#################################################################################
class CmplProblemHandler(object):
	
	#*********** constructor **********
	def __init__(self, cmplName  ):
		self.__cmplName = cmplName
		self.__processHandler = None
		self.__status = CMPL_UNKNOWN 
		self.__statusMessage = ""
		self.__output = []
		self.__lineNr = 0
		self.setLastActivityTime(time.time())
		self.__serverId = ""
		self.__optionsList = []
	#*********** end constructor *****
	
	# getter and setter ***************	
	@property
	def processHandler(self):
		return self.__processHandler
		
	def setProcessHandler(self, pH):
		if type(pH) != subprocess.Popen:
			 raise CmplServerException( "Internal CmplServer error  <CmplServer subprocessHandler> ")
		self.__processHandler = pH
	
	@property
	def status(self):
		return self.__status
		
	def setStatus(self, stat):
		self.__status = stat	
		
	@property
	def statusMessage(self):
		return self.__statusMessage
		
	def setStatusMessage(self, str):
		self.__statusMessage = str
		
	@property
	def name(self):
		return self.__cmplName
		
	def setName(self, name):
		self.__cmplName = name
		
	@property
	def output(self):
		return self.__output
		
	def setOutput(self, str):
		self.__output.append(str)	
		
	def delOutput(self, i,j ):
		del self.__output[i:j]
		
	@property
	def lineNr(self):
		return self.__lineNr
	
	def setLineNr(self , nr):
		self.__lineNr = nr
		
	@property
	def lastActivityTime(self):
		return self.__lastActivityTime
	
	def setLastActivityTime(self, timeStamp):		
		self.__lastActivityTime=timeStamp
		
	@property
	def serverId(self):
		return self.__serverId
	
	def setServerId(self, id):		
		self.__serverId=id	
		
	@property
	def options(self):
		return self.__optionsList
	
	def setOptions(self, optList):
		self.__optionsList=optList	
	# end getter and setter *************
#################################################################################
# end CmplProblemHandler							#
#################################################################################	

#################################################################################
#										#
# CmplGridSchedulerHandler							#
#										#
#################################################################################		
class CmplGridSchedulerHandler(object):
	
	#*********** constructor **********
	def __init__(self, cmplServerUrl , maxProb ):
		self.__cmplUrl = cmplServerUrl
		self.__maxProblems = maxProb
		self.__lastActivity = 0
		self.__scheduler = None
	#*********** end constructor *****
	
	# getter and setter ***************	
	@property
	def cmplUrl(self):
		return self.__cmplUrl
		
	def setCmplUrl(self, url):
		self.__cmplUrl = url	
		
	@property
	def maxProblems(self):
		return self.__maxProblems
		
	def setMaxProblems(self, nr):
		self.__maxProblems=nr

	@property
	def lastActivityTime(self):
		return self.__lastActivityTime
	
	def setLastActivityTime(self, timeStamp):		
		self.__lastActivityTime=timeStamp
		
	@property
	def scheduler(self):
		return self.__scheduler	
	
	def setScheduler(self, schd):
		self.__scheduler=schd
	# end getter and setter *************
#################################################################################
# End CmplGridSchedulerHandler							#
#################################################################################	

#################################################################################
#										#
# CmplServer									#
#										#
#################################################################################	
class CmplServer(object):

	#****************************************************************************
	# Constructor and destructor						    *
	#****************************************************************************
		 
	#*********** constructor **********
	def __init__(self, port = None, mode = STANDALONE_SERVER ):
	
		self.__compatibility = COMPATIBILITY
		
		self.__server = None
		self.__serverStatus = CMPLSERVER_OK
		self.__serverStatusTxt = "CMPLServer is running"
		
		
		self.__problemList = {} 
		self.__schedulerList = {}
		self.__problemQueue = []
		self.__shutDown=False
		
	
		if port != None:
			self.__cmplPort = port
		else:
			self.__cmplPort  = 8008
	
		self.__maxProblems = 8
		self.__runningProblems = 0
		
		self.__maxInactivityTime=60*60*12 # half a day 
		self.__serviceIntervall = 30
		self.__serviceThreadHandler = None
		
		self.__performanceIndex = 1
		
		self.__serverMode = mode
		self.__maxCmplSchedulerTries=10
		
		self.__solvers = []
		
		self.__cmplServerPath = os.path.expanduser("~") + os.sep+ "CmplServer" +os.sep
		self.__logFileName = self.__cmplServerPath + "cmplServer.log"
		
			
		if os.path.exists(self.__cmplServerPath) == False:
			try:
				os.mkdir(self.__cmplServerPath)
			except OSError, e:
				raise CmplServerException( "Cannot create CmplServer path <"+self.__cmplServerPath+">")
			
		try:
			self.__logFile = open(self.__logFileName, "a")
		except IOError, e:
			raise CmplServerException( "Cannot read CmplServer log file <"+self.__logFile+"> " + str(e) )
		
		try:
			try:
				self.__optFileName=os.environ['CMPLSERVERPATH']+ os.sep + "cmplServer.opt"	
			except:
				self.__optFileName=os.path.dirname(os.path.abspath(sys.argv[0])) + os.sep + ".." + os.sep + "cmplServer.opt"
				
			f = open(self.__optFileName, "r") 
			for line in f: 
				ret=line.split("=")
				
				if ret[0].strip().lower() == "maxproblems":
					if CmplTools.strIsNumber(ret[1].strip()):
						self.__maxProblems = int(ret[1].strip())
						continue
					else:
						CmplServerTools.cmplLogging( self.__logFile,  "Wrong option maxProblems in CmplServer option file <"+self.__optFileName+"> default value is used" )
				
				if ret[0].strip().lower() == "maxinactivitytime":
					if CmplTools.strIsNumber(ret[1].strip()):
						self.__maxInactivityTime = int(ret[1].strip())
						continue
					else:
						CmplServerTools.cmplLogging( self.__logFile,  "Wrong option maxInactivityTime in CmplServer option file <"+self.__optFileName+"> default value is used" )
					
				if ret[0].strip().lower() == "serviceintervall":
					if CmplTools.strIsNumber(ret[1].strip()):
						self.__serviceIntervall = int(ret[1].strip())
						continue
					else:
						CmplServerTools.cmplLogging( self.__logFile,  "Wrong serviceIntervall in CmplServer option file <"+self.__optFileName+"> default value is used" )
				
				if ret[0].strip().lower() == "performanceindex":
					if CmplTools.strIsNumber(ret[1].strip()):
						self.__performanceIndex = int(ret[1].strip())
						continue
					else:
						CmplServerTools.cmplLogging( self.__logFile,  "Wrong performanceIndex in CmplServer option file <"+self.__optFileName+"> default value is used" )
						
				
				if ret[0].strip().lower() == "maxservertries":
					if CmplTools.strIsNumber(ret[1].strip()):
						self.__maxCmplSchedulerTries = int(ret[1].strip())
					else:
						CmplServerTools.cmplLogging( self.__logFile,  "Wrong maxServerTries in CmplServer option file <"+str(self.__maxCmplSchedulerTries)+"> default value is used" )

				if ret[0].strip().lower() == "solvers":
					self.__solvers=ret[1].lower().split()

				if self.__serverMode==CMPL_GRID:
					if ret[0].strip().lower() == "cmplgridscheduler":
										
						gridArgs=ret[1].split()
											
						if CmplTools.strIsNumber(gridArgs[1].strip()):
							maxP = int(gridArgs[1].strip())
							self.__schedulerList.update( { "tmp" : CmplGridSchedulerHandler(gridArgs[0] ,maxP) } ) 
							
							self.__schedulerList["tmp"].setScheduler( xmlrpclib.ServerProxy(gridArgs[0]) )
							
							ans = self.__cmplSchedulerExecute(self.__schedulerList["tmp"].scheduler,"getServerId", self.__cmplPort, gridArgs[1], self.__performanceIndex, self.__solvers, self.__compatibility)
							
							if ans[0]==CMPLGRID_SCHEDULER_ERROR:
								self.__serverStatus = CMPLSERVER_ERROR
								self.__serverStatusTxt = "CmplGridScheduler error on <"+gridArgs[0] + "> "  + ans[1]
								#self.__cmplSchedulerExecute(self.__schedulerList[s].scheduler,"disconnectServer",ans[2])
								self.__cleanUp()
								raise CmplServerException( "CmplServer can't connect to CmplGridScheduler <"+gridArgs[0] + "> : "  +  ans[1], self.__logFile)
							
							if ans[0] == CMPLGRID_SCHEDULER_OK:
								self.__schedulerList[ans[2]]=self.__schedulerList.pop("tmp")
								CmplServerTools.cmplLogging( self.__logFile,  "CmplServer is connected to CmplGridScheduler <"+ gridArgs[0]+"> with serverId <"+ ans[2]+ "> and max problems <" +gridArgs[1]+">" )
								
							else:
								self.__serverStatus = CMPLSERVER_ERROR
								self.__serverStatusTxt = "CmplServer can't connect to CmplGridScheduler <"+gridArgs[0]+"> : " + ans[1]
								self.__cleanUp()
								raise CmplServerException( "CmplServer can't connect to CmplGridScheduler <"+gridArgs[0]+"> : " + ans[1], self.__logFile)
						else:
						   CmplServerTools.cmplLogging( self.__logFile,  "Wrong option maxProblems <"+gridArgs[1]+"> for CmplGridScheduler <"+ gridArgs[0] + "> in CmplServer option file" )
						   
				
			
			f.close()
			
			if self.__serverMode==CMPL_GRID and len(self.__schedulerList)==0:
				self.__cleanUp()
				raise CmplServerException( "CmplServer is not connected to a CmplGridScheduler ")
							
				
			
		except IOError, e:
			raise CmplServerException( "Cannot read CmplServer option file <"+self.__optFileName+"> " + str(e) , self.__logFile)
	#*********** end constructor *******	
	
	#*********** destructor ************
	def __del__(self ):
		for p in self.__problemList:
			if p.processHandler!=None:
				p.__processHandler.kill()
		
		if self.__serviceThreadHandler!=None:
			self.__serviceThreadHandler.kill()
	
		self.__logFile.close()
	#*********** end destructor ********	
	
	#****************************************************************************
	# public methods   							    *
	#****************************************************************************
	
	#*********** startServer *************
	def startServer(self): 
		try:
			self.__server = CmplXmlRpcServer(("", self.__cmplPort), logRequests=False) 
			self.__server.register_function(self.getJobId) 
			self.__server.register_function(self.sendJobId)
			self.__server.register_function(self.send) 
			self.__server.register_function(self.knock)
			self.__server.register_function(self.cancel)
			self.__server.register_function(self.getSolutions)
			self.__server.register_function(self.getCmplMessages)
			self.__server.register_function(self.getCmplInfo)
			self.__server.register_function(self.removeProblem)
			self.__server.register_function(self.stopServer)
			self.__server.register_function(self.status)
			self.__server.register_function(self.disconnectFromScheduler)
			
			logMsg=cStringIO.StringIO()
			logMsg.write("CmplServer has been started")
			if self.__serverMode==CMPL_GRID:
				logMsg.write(" in CmplGrid")
			logMsg.write(" | port: "  + str(self.__cmplPort) + " | maxProblems: " + str(self.__maxProblems ) + " | maxInactivityTime: " + str(self.__maxInactivityTime) + " | serviceIntervall: " + str(self.__serviceIntervall))
			logMsg.write(" | solvers: " + str(self.__solvers))
			CmplServerTools.cmplLogging( self.__logFile, logMsg.getvalue() )
			logMsg.close()
			
			self.shutdown = False
			thread.start_new_thread(self.__serviceThread, () ) 
			while not self.shutdown:
				self.__server.handle_request()
		except:
			print "CmplServer error: " , str(sys.exc_info()[1])
			
	#*********** end startServer **********
	
			
	#*********** getJobId **************		
	def getJobId(self, cmplProb, solver, compatibility=0):
		
		cmplName=unescape(cmplProb)
	
		id = "S"+self.__server.client_address[0] + "-" + strftime("%Y-%m-%d-%H-%M-%S", gmtime()) + "-" + str(random.randint(100000, 999999))
		tmpSolver=solver.lower()
		
		if self.__serverMode==CMPL_GRID:
			status=CMPLSERVER_ERROR
			statusMessage="CmplServer is part of a CmplGrid and can only connected via a CmplGridScheduler"
			CmplServerTools.cmplLogging( self.__logFile, "Unauthorized registration failed: " + self.__server.client_address[0] )
		
		else:
			status=CMPLSERVER_OK
		
			if int(compatibility)!=self.__compatibility:
				status=CMPLSERVER_ERROR
				statusMessage="Incompatible CmplServer client with compatibility stage "+str(compatibility) + " instead of " + str(self.__compatibility)
			else:
				statusMessage=str(self.__compatibility)
				
				if not tmpSolver in self.__solvers:
					status=CMPLSERVER_ERROR
					statusMessage="Solver <"+solver + "> is not available on CmplServer."
		
			if status==CMPLSERVER_OK:
				try:
					os.mkdir(self.__cmplServerPath+os.sep+id)
					
					self.__problemList.update( {id: CmplProblemHandler(cmplName) } ) 
					self.__problemList[id].setStatus(status)
					self.__problemList[id].setStatusMessage(statusMessage)
					self.__problemList[id].setLastActivityTime(time.time())		
					CmplServerTools.cmplLogging( self.__logFile,  "Problem has been registered for CMPL and solver <" + str(tmpSolver) + ">", id, cmplName )
					
				except OSError, e:
					status=CMPLSERVER_ERROR
					statusMessage="Cannot create path <CmplServerHome/"+id +"> "
					CmplServerTools.cmplLogging( self.__logFile, "Registration failed: " + statusMessage , id, cmplName  )
				
			else: 
				CmplServerTools.cmplLogging( self.__logFile, "Registration failed: " + statusMessage , id, cmplName  )
				
		
		return [ status, statusMessage, id]
	#*********** end getJobId ************		
	
	#*********** sendJobId **************	
	""" jobId is defined and sent by a CmplGridScheduler """	
	def sendJobId(self, cmplName, jobId, serverId):
	
		id = jobId
		status=CMPLSERVER_OK
		statusMessage=""
						
		if status==CMPLSERVER_OK:
			try:
				os.mkdir(self.__cmplServerPath+os.sep+id)
				
				self.__problemList.update( {id: CmplProblemHandler(cmplName) } ) 
				self.__problemList[id].setStatus(status)
				self.__problemList[id].setStatusMessage(statusMessage)
				self.__problemList[id].setServerId(serverId)	
				self.__problemList[id].setLastActivityTime(time.time())		
				CmplServerTools.cmplLogging( self.__logFile,  "Problem has been received from CmplGridScheduler <"+ self.__server.client_address[0] +"> and registered" , id, cmplName )
				
			except OSError, e:
				status=CMPLSERVER_ERROR
				statusMessage="Cannot create path <CmplServerHome/"+id +"> "
				CmplServerTools.cmplLogging( self.__logFile, "Registration received from CmplGridScheduler <"+ self.__server.client_address[0] +"> failed: " + statusMessage , id, cmplName  )
		
		return [ status, statusMessage, id]
	#*********** end sendJobId ************		
		
	#*********** removeProblem ***********		
	def removeProblem(self, id):
		if self.__checkId(id):
			try:
				if os.path.exists(self.__cmplServerPath+os.sep+id):
					for f in os.listdir(self.__cmplServerPath+os.sep+id):
						os.remove(self.__cmplServerPath+os.sep+id+os.sep+f)
					os.rmdir(self.__cmplServerPath+os.sep+id)	
					logTxt = "Problem has been deregistered" 
					status = CMPLSERVER_OK 
			except OSError, e:
				logTxt = "Problem cannot be deregistered " +str(e)  
				status = CMPLSERVER_ERROR
		
			del self.__problemList[id]
			
		else:
			status=CMPLSERVER_ERROR
			logTxt="Removing problem failed -> unknown id " + id
		 
		return [status, logTxt, ""]
	#*********** end removeProblem ********		
	
	#*********** getSolution **************
	def getSolutions(self, id):
		if self.__checkId(id):
			try:
				
				problem =  os.path.splitext(self.__cmplServerPath+id+os.sep+self.__problemList[id].name)[0]
				cmplSolFile = problem+".csol"
			
				solStr=""
				if not (self.__problemList[id].status==CMPLSERVER_ERROR or self.__problemList[id].status==CMPL_FAILED) :
					solStr = CmplServerTools.readFileContent(cmplSolFile)
				else:
					status = CMPLSERVER_ERROR
					
			except Exception, e:
				self.__problemList[id].setStatus(CMPLSERVER_ERROR)
				self.__problemList[id].setStatusMessage("No solution found")
				solStr=""
				
				
			CmplServerTools.cmplLogging( self.__logFile,  "Sending solution(s)" , id, self.__problemList[id].name)
			self.__problemList[id].setLastActivityTime(time.time())	
			
			status=self.__problemList[id].status
			statusMessage=self.__problemList[id].statusMessage
		else:
			status = CMPLSERVER_ERROR
			statusMessage = "Sending solution(s) failed -> unknown id " + id
			solStr=""
		return [ status, statusMessage, solStr]
	#*********** end getSolution *********
		
	#*********** getCmplMessages *********
	def getCmplMessages(self, id):
		if self.__checkId(id):
			try:
				problem =  os.path.splitext(self.__cmplServerPath+id+os.sep+self.__problemList[id].name)[0]
				cmplMsgFile = problem+".cmsg"
					
				msgStr=""
				if not (self.__problemList[id].status==CMPLSERVER_ERROR or self.__problemList[id].status==CMPL_FAILED) :
					msgStr = CmplServerTools.readFileContent(cmplMsgFile)
					
			except Exception, e:
				self.__problemList[id].setStatus(CMPLSERVER_ERROR)
				self.__problemList[id].setStatusMessage(e)
				
			CmplServerTools.cmplLogging( self.__logFile,  "Sending cmplMessages", id, self.__problemList[id].name )	
			self.__problemList[id].setLastActivityTime(time.time())	
			
			status=self.__problemList[id].status
			statusMessage=self.__problemList[id].statusMessage
		else:
			status = CMPLSERVER_ERROR
			statusMessage = "Sending messages failed -> unknown id " + id
			msgStr=""
		return [ status, statusMessage, msgStr]
	#*********** end getCmplMessages ********
	
	#*********** getCmplInfo ****************
	def getCmplInfo(self, id):
		if self.__checkId(id):
			try:
				problem =  os.path.splitext(self.__cmplServerPath+id+os.sep+self.__problemList[id].name)[0]
				cmplInfoFile = problem+".cinf"
					
				msgStr=""
				if not (self.__problemList[id].status==CMPLSERVER_ERROR or self.__problemList[id].status==CMPL_FAILED) :
					msgStr = CmplServerTools.readFileContent(cmplInfoFile)
					
			except Exception, e:
				self.__problemList[id].setStatus(CMPLSERVER_ERROR)
				self.__problemList[id].setStatusMessage(e)
			
			CmplServerTools.cmplLogging( self.__logFile,  "Sending cmplInfos", id, self.__problemList[id].name )
			self.__problemList[id].setLastActivityTime(time.time())		
			
			status=self.__problemList[id].status
			statusMessage=self.__problemList[id].statusMessage
		else:
			status = CMPLSERVER_ERROR
			statusMessage = "Sending cmplInfo file failed -> unknown id " + id
			msgStr=""
			
		return [ status, statusMessage, msgStr]
	#*********** end getCmplInfo *************
		
	#*********** send ********************	
	def send(self, instanceStr):
			
		status=CMPLSERVER_OK
		statusMessage=""
			
		try:
			inst = CmplInstance()
			inst.writeCmplInstance(self.__cmplServerPath, instanceStr)	
		except CmplException, e:
			status=CMPLSERVER_ERROR
			statusMessage=e.msg
			
		if status!=CMPLSERVER_ERROR:
			self.__problemList[inst.jobId].setOptions(inst.options)
		
			if self.__checkId(inst.jobId):
			
				if self.__runningProblems >= self.__maxProblems:
				
					status=CMPLSERVER_BUSY
					statusMessage="To many parallel problems - Problem is moved to the problem queue. "+ self.__problemList[inst.jobId].name
					
					self.__problemList[inst.jobId].setStatus(status)
					self.__problemList[inst.jobId].setStatusMessage(statusMessage)
					
					self.__problemQueue.append(inst.jobId)
					CmplServerTools.cmplLogging( self.__logFile,  "To many parallel problems - Problem has been moved to the problem queue." , inst.jobId, self.__problemList[inst.jobId].name)
					self.__problemList[inst.jobId].setOutput("CmplServer - To many parallel problems - Problem has been moved to the problem queue.") 

				else:
					self.__runSolver(inst.jobId)

					status=self.__problemList[inst.jobId].status
					statusMessage=self.__problemList[inst.jobId].statusMessage
				
					if status==CMPLSERVER_ERROR or status==CMPL_FAILED:
						CmplServerTools.cmplLogging( self.__logFile,  "Solving problem failed: " + e.msg , inst.jobId, self.__problemList[inst.jobId].name)

			else:
				status=CMPLSERVER_ERROR
				statusMessage="Starting solving failed -> unknown id " + inst.jobId
						
		return [ status, statusMessage, ""]
	#*********** end send ****************	
	
	#*********** knock *******************	
	def knock(self, id):
		tmpOut=cStringIO.StringIO()
		if self.__checkId(id):
			
			if self.__problemList[id].processHandler!=None:
				ret = self.__problemList[id].processHandler.poll()
				i = 0
				for line in self.__problemList[id].output[self.__problemList[id].lineNr : ] :
					tmpOut.write(line  + "\n")
					i += 1
				self.__problemList[id].setLineNr( self.__problemList[id].lineNr + i)
				
				if ret == None:
					self.__problemList[id].setStatus(PROBLEM_RUNNING)
					self.__problemList[id].setStatusMessage("Problem is still running")
				else:
					problem =  os.path.splitext(self.__cmplServerPath+id+os.sep+self.__problemList[id].name)[0]
					cmplMsgFile = problem+".cmsg"
					
					if ret != 0 or not os.path.isfile(cmplMsgFile):
						self.__problemList[id].setStatus(CMPL_FAILED)
						#self.__problemList[id].setStatus(CMPLSERVER_ERROR)
						ant = self.__problemList[id].processHandler.stderr.read()
						self.__problemList[id].setStatusMessage(ant)
						CmplServerTools.cmplLogging( self.__logFile,  "Solving problem failed", id, self.__problemList[id].name)
					else:	
						self.__problemList[id].setStatus(PROBLEM_FINISHED)
						self.__problemList[id].setStatusMessage("Problem has finished")
						
			else:
				if not self.__problemList[id].status==CMPLSERVER_BUSY:
					self.__problemList[id].setStatus(PROBLEM_NOTRUNNING) 
					self.__problemList[id].setStatusMessage("Problem is not running")
				
			status=self.__problemList[id].status
			statusMessage=self.__problemList[id].statusMessage
			self.__problemList[id].setLastActivityTime(time.time())
			
			
		else:
			status = CMPLSERVER_ERROR
			statusMessage = "Knocking failed -> unknown id " + id
								
		tOut=tmpOut.getvalue()
		tmpOut.close()
		return [ status, statusMessage, tOut]
	#*********** end knock ***************	
	
	#*********** cancel ******************	
	def cancel(self, id):
		if self.__checkId(id):
			if self.__problemList[id].processHandler!=None:
				ret = self.__problemList[id].processHandler.poll()
				if ret == None:
					self.__problemList[id].processHandler.kill()
					CmplServerTools.cmplLogging( self.__logFile,  "Problem has been canceled by user" , id, self.__problemList[id].name)
					del self.__problemList[id]
					return [PROBLEM_CANCELED, "Problem has been canceled by user"]
			status=self.__problemList[id].status
			statusMessage=self.__problemList[id].statusMessage
			
			if status != CMPL_FAILED and status != CMPLSERVER_ERROR:
				self.__addEmptyProblemToGrid(id)

		else:
			status = CMPLSERVER_ERROR
			statusMessage = "Canceling failed -> unknown id " + id
			
		return [ status, statusMessage, ""]
	#*********** end cancel **************	

	#*********** stopServer **************
	def stopServer(self):
		if  self.__server.client_address[0] == "127.0.0.1" or self.__shutDown==True:
			for id in self.__problemList:
				if self.__problemList[id].processHandler!=None:
					ret = self.__problemList[id].processHandler.poll()
					if ret == None:
						self.__problemList[id].processHandler.kill()
						CmplServerTools.cmplLogging( self.__logFile,  "Problem has been canceled by CmplServer shutDown" , id, self.__problemList[id].name)
				try:
					if os.path.exists(self.__cmplServerPath+os.sep+id):
						for f in os.listdir(self.__cmplServerPath+os.sep+id):
							os.remove(self.__cmplServerPath+os.sep+id+os.sep+f)
						os.rmdir(self.__cmplServerPath+os.sep+id)	
						CmplServerTools.cmplLogging( self.__logFile,  "Problem has been deregistered" , id, self.__problemList[id].name)
				except OSError, e:
					CmplServerTools.cmplLogging( self.__logFile,  "Problem cannot been deregistered" , id, self.__problemList[id].name)
					
			self.__problemList.clear()
						
			self.shutdown = True
			
			for s in self.__schedulerList:
				ret = self.__cmplSchedulerExecute(self.__schedulerList[s].scheduler,"disconnectServer",s)
				
				if ret[0]==CMPLGRID_SCHEDULER_ERROR:
					CmplServerTools.cmplLogging( self.__logFile, "CmplServer can't disconnect from CmplGridScheduler <"+self.__schedulerList[s].cmplUrl + "> : " +ret[1] )
				else:
					CmplServerTools.cmplLogging( self.__logFile, "CmplServer has been disconnected from CmplGridScheduler" )
			
			CmplServerTools.cmplLogging( self.__logFile,  "CmplServer has been stopped" )	
			return True
		else:
			return False
	#*********** end stopServer **********	
	
	#*********** status ***************
	def status(self):
		#CmplServerTools.cmplLogging( self.__logFile,  "Status check: " + str(self.__serverStatus)  )
		emptyProbs=0
		if self.__runningProblems < self.__maxProblems:
			#emptyProbs=int(self.__runningProblems/len(self.__schedulerList))
			emptyProbs=self.__maxProblems-self.__runningProblems
		
		return [self.__serverStatus, self.__serverStatusTxt, emptyProbs]
	#*********** end status ***********	
	
	#*********** disconnectFromScheduler ***************
	def disconnectFromScheduler(self, serverId):
		if serverId in self.__schedulerList:
			CmplServerTools.cmplLogging( self.__logFile,  "CmplServer has been disconnected from CmplGridScheduler <"+ self.__schedulerList[serverId].cmplUrl +"> with ServerId <"+ serverId + "> because CmplGridScheduler has stopped.")
			del self.__schedulerList[serverId]
			status = CMPLSERVER_OK
			statusMessage = "CmplServer has been disconnect from CmplGridScheduler"
		else:
			status = CMPLSERVER_WARNING
			statusMessage = "CmplServer <" + serverId +"> wasn't connected"
		return [status, statusMessage, ""]
	#*********** end disconnectFromScheduler ***********	
	
	
	#****************************************************************************
	# private methods							
	#****************************************************************************
	
	#*********** cleanUp ************
	def __cleanUp(self ):
	
		if self.__serverMode==CMPL_GRID:
			for s in self.__schedulerList:
				try:
					self.__schedulerList[s].scheduler.disconnect(s)
					CmplServerTools.cmplLogging( self.__logFile,  "CmplServer is disconnected from CmplGridScheduler <"+ self.__schedulerList[s].cmplUrl +"> with ServerId <"+ s + "> ")
				except:
					if s != "tmp":
						CmplServerTools.cmplLogging( self.__logFile,  "Can't disconnect from CmplGridScheduler <"+ self.__schedulerList[s].cmplUrl +"> with ServerId <"+ s + "> "+str(sys.exc_info()[1]) )
		
		for p in self.__problemList:
			if self.__problemList[p].processHandler!=None:
				self.__problemList[p].processHandler.kill()
		
		if self.__serviceThreadHandler!=None:
			self.__serviceThreadHandler.kill()
	
		#self.__logFile.close()
	#*********** end cleanUp ********	
	
	#*********** __checkId ***************
	def __checkId(self, id) :
		return id in self.__problemList
	#*********** end __checkId ***********
	
	#*********** __checkSchedulerId ***************
	def __checkSchedulerId(self, id) :
		return id in self.__schedulerList
	#*********** end __checkSchedulerId ***********
	
	#*********** __serviceThread ******
	def __serviceThread(self):
		while True:
			if self.shutdown==True:
				break
			time.sleep(self.__serviceIntervall)
			self.__cleanOldProblems()
			
			if self.__runningProblems >= self.__maxProblems:
				self.__serverStatus = CMPLSERVER_BUSY
				self.__serverStatusTxt = "CMPLServer is busy"
			else:
				self.__serverStatus = CMPLSERVER_OK
				self.__serverStatusTxt = "CMPLServer is running"
			
			if self.__serverMode==CMPL_GRID:
				if len(self.__schedulerList) == 0:
					if len(	self.__problemList) == 0:
						CmplServerTools.cmplLogging( self.__logFile,  "CmplServer is not longer connected to a CmplGridScheduler and does not handle waiting problems." )
						self.__shutDown=True
						self.stopServer()
				
	#******* end __serviceThread ******
		
	#*********** cmplSchedulerExecute *******
	def __cmplSchedulerExecute(self, server, method="", *args):
		ret=[]
		tries=0
		while True:
			try:
				if method=="getServerId":
					ret = server.getServerId(args[0] , args[1], args[2], args[3], args[4])
				elif method=="addEmptyProblem":
					ret = server.addEmptyProblem(args[0])
				elif method=="disconnectServer":
					ret = server.disconnectServer(args[0])
			except:
				tries+=1
				#CmplServerTools.cmplLogging( self.__logFile,  "exec ERROR" + str(sys.exc_info()[1]) )
				if tries==self.__maxCmplSchedulerTries or method=="getServerId":
					ret=[ CMPLGRID_SCHEDULER_ERROR, str(sys.exc_info()[1]) ,"" ]
					break
				else:	
					continue		
			break
			
		return ret
	#******** end cmplSchedulerExecute *******
	
	
	#*********** cleanOldProblems ******
	def __cleanOldProblems(self):
		delList = []
		for id in self.__problemList:	
			if self.__problemList[id].status in [CMPLSERVER_OK,CMPLSERVER_ERROR,PROBLEM_FINISHED,PROBLEM_CANCELED,PROBLEM_NOTRUNNING]:
				if (time.time()-self.__problemList[id].lastActivityTime)>self.__maxInactivityTime:
					delList.append(id)
				
		for id in delList:
			if self.__problemList[id].processHandler!=None:
				ret = self.__problemList[id].processHandler.poll()
				if ret == None:
					self.__problemList[id].processHandler.kill()
					CmplServerTools.cmplLogging( self.__logFile,  "Inactive problem has been canceled by CmplServer" , id, self.__problemList[id].name)
			try:
				if os.path.exists(self.__cmplServerPath+os.sep+id):
					for f in os.listdir(self.__cmplServerPath+os.sep+id):
						os.remove(self.__cmplServerPath+os.sep+id+os.sep+f)
					os.rmdir(self.__cmplServerPath+os.sep+id)	
					CmplServerTools.cmplLogging( self.__logFile,  "Inactive problem has been deregistered" , id, self.__problemList[id].name)
			except OSError, e:
				CmplServerTools.cmplLogging( self.__logFile,  "Inactive problem cannot been deregistered" , id, self.__problemList[id].name)
		
			if self.__problemList[id].status in [CMPLSERVER_ERROR,PROBLEM_CANCELED,PROBLEM_NOTRUNNING]:
				self.__addEmptyProblemToGrid(id)
					
			del self.__problemList[id]
	#******* end __cleanOldProblems ******
	
	
	#*********** __storeOutput *****************
	def __storeOutput(self, id):
		try:
			while True:
				line = self.__problemList[id].processHandler.stdout.readline()
				if len(line)>0:
					self.__problemList[id].setOutput(line.rstrip())
				else:
					break
			
			#for line in iter( self.__problemList[id].processHandler.stdout.readline, ''):
			#	self.__problemList[id].setOutput(line.rstrip()) 
			
			CmplServerTools.cmplLogging( self.__logFile,  "Solving problem has been finished" , id, self.__problemList[id].name)
			self.__problemList[id].setOutput("CmplServer - Solving problem has been finished") 
			self.__handleProblemQueue(id)
			
		except:
			CmplServerTools.cmplLogging( self.__logFile,  "Canceled by user: " +str(sys.exc_info()[1]) ) 
	#*********** end __storeOutput *************
	
	#*********** __handleProblemQueue *****************
	def __handleProblemQueue(self, id):
		
		self.__runningProblems-=1
		
		if len(self.__problemQueue)>0:
			tmpId=self.__problemQueue.pop(0)
			self.__runSolver(tmpId)
			
			if  self.__problemList[tmpId].status==CMPLSERVER_ERROR or self.__problemList[tmpId].status==CMPL_FAILED:
				CmplServerTools.cmplLogging( self.__logFile,  "Solving problem failed: " + self.__problemList[tmpId].setStatusMessage , tmpId,  self.__problemList[tmpId].name)
				self.__problemQueue.insert(0,tmpId)	
		else:
			self.__addEmptyProblemToGrid(id)
					
	#*********** end __handleProblemQueue *************
	
	#*********** __addEmptyProblemToGrid *****************
	def __addEmptyProblemToGrid(self, id):	
		if self.__serverMode==CMPL_GRID:
			ret = self.__cmplSchedulerExecute(self.__schedulerList[self.__problemList[id].serverId].scheduler,"addEmptyProblem", self.__problemList[id].serverId ) 
			if ret[0] == CMPLGRID_SCHEDULER_OK:
				CmplServerTools.cmplLogging( self.__logFile,  "Add empty problem to CmplGridScheduler <" + self.__schedulerList[self.__problemList[id].serverId].cmplUrl+">" ,  id)
			else:
				CmplServerTools.cmplLogging( self.__logFile,  "Cannot add empty problem to CmplGridScheduler <" + self.__schedulerList[self.__problemList[id].serverId].cmplUrl+">",  id)	
				
	#*********** end __addEmptyProblemToGrid ***************
	
	#*********** runSolver ***************	
	def __runSolver(self, id):	
		
		ret=True
			
		if self.__checkId(id):
		
			try:
				cmplBin=os.environ['CMPLBINARY']
			except:
				self.__problemList[id].setStatus(CMPLSERVER_ERROR)
				self.__problemList[id].setStatusMessage("Starting solving failed: " +str(sys.exc_info()[1]))
				ret = False

			if self.__problemList[id]!=CMPLSERVER_ERROR:
				if not os.path.exists(cmplBin):
					self.__problemList[id].setStatus(CMPLSERVER_ERROR)
					self.__problemList[id].setStatusMessage("Cant't find Cmpl binary: " + cmplBin)
					ret = False
				else:
					problem = self.__cmplServerPath+id+os.sep+self.__problemList[id].name
					cmdList = [cmplBin, problem, "-remote", "-e", "-includesForbidden"]
					#CmplServerTools.cmplLogging( self.__logFile, str(cmdList));
					if len(self.__problemList[id].options) != 0:
						for opt in self.__problemList[id].options:
								cmdList.append("-headerOpt")
								cmdList.append(opt.replace(" ", "#"))
			
					tmpP= subprocess.Popen(cmdList, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
					self.__problemList[id].setProcessHandler(tmpP )
					self.__problemList[id].setStatus(PROBLEM_RUNNING)
	
					thread.start_new_thread(self.__storeOutput, (id,)) 
					self.__runningProblems+=1
					self.__problemList[id].setLastActivityTime(time.time())
					CmplServerTools.cmplLogging( self.__logFile,  "Solving problem has been started" , id, self.__problemList[id].name)	
					self.__problemList[id].setOutput("CmplServer - Solving problem has been started") 
						
					ret = True
		else:
			self.__problemList[id].setStatus(CMPLSERVER_ERROR)
			self.__problemList[id].setStatusMessage("Starting solving failed -> unknown id " + id)
			ret = False
	
			
		return ret 
	#*********** end __runSolver ************
	


#################################################################################
# End CmplServer					   				            				#
#################################################################################	
  
	


	
	



		
		
		
		

		
