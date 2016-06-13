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

from pyCmpl.CmplDefs import *
from pyCmpl.CmplTools import *

from CmplServerException import *
from CmplServerTools import *

import xmlrpclib
import thread 
import threading
import random
import os
import sys
import subprocess
import socket
import time


#################################################################################
#						
# CmplServerHandler																
#																				
#################################################################################
class CmplServerHandler(object):
	
	#*********** constructor **********
	def __init__(self, cmplServerUrl , maxProb, performanceIndex  ):
		self.__cmplServerUrl = cmplServerUrl
		self.__cmplServer = None
		self.__emptyProblems = maxProb
		self.__maxProblems = maxProb 
		self.__performanceIndex = performanceIndex
		self.__lastActivity = 0
		self.__isActive = True
		self.__solvers = []
	#*********** end constructor *****
	
	# getter and setter ***************	
	@property
	def cmplServerUrl(self):
		return self.__cmplServerUrl
		
	def setCmplServerUrl(self, url):
		self.__cmplServerUrl = url	
		
	@property
	def cmplServer(self):
		return self.__cmplServer
		
	def setCmplServer(self, server):
		self.__cmplServer = server	

	@property
	def emptyProblems(self):
		return self.__emptyProblems
		
	def setEmptyProblems(self, nr):
		self.__emptyProblems=nr
		
	def addEmptyProblem(self):
		 self.__emptyProblems += 1	
		
	def removeEmptyProblem(self):
		self.__emptyProblems -= 1	
		
	@property
	def maxProblems(self):
		return self.__maxProblems
		
	def setMaxProblems(self, nr):
		self.__maxProblems=nr
		
	@property
	def performanceIndex(self):
		return self.__performanceIndex
		
	def setPerformanceIndex(self, nr):
		self.__performanceIndex=nr
			
	@property
	def lastActivityTime(self):
		return self.__lastActivityTime
	
	def setLastActivityTime(self, timeStamp):		
		self.__lastActivityTime=timeStamp
		
	@property
	def isActive(self):
		return self.__isActive
	
	def setActive(self, val):		
		self.__isActive=val
		
	@property
	def solvers(self):
		return self.__solvers
	
	def setSolvers(self, solvers):
		self.__solvers=solvers
	# end getter and setter *************

#################################################################################
# End CmplServerHandler								
#################################################################################	



#################################################################################
#										
# ProblemQueueHandler								
#										
#################################################################################		
class ProblemQueueHandler(object):
	
	#*********** constructor **********
	def __init__(self, cmplName , solver):
		self.__cmplServerId = None
		self.__cmplName = cmplName
		self.__status = CMPLGRID_SCHEDULER_UNKNOWN
		self.__solver = solver
		self.setLastActivityTime(time.time())
	#*********** end constructor *****
	
	# getter and setter ***************	
	@property
	def cmplServerId(self):
		return self.__cmplServerId
		
	def setCmplServerId(self, id):
		self.__cmplServerId = id	
	
	@property
	def cmplName(self):
		return self.__cmplName
		
	def setCmplName(self, name):
		self.__cmplName = url	
		
	@property
	def status(self):
		return self.__status
		
	def setStatus(self, status):
		self.__status = status
		
	@property
	def solver(self):
		return self.__solver
	
	@property
	def lastActivityTime(self):
		return self.__lastActivityTime
	
	def setLastActivityTime(self, timeStamp):		
		self.__lastActivityTime=timeStamp
	
	# end getter and setter *************

#################################################################################
# End ProblemQueueHandler							
#################################################################################			



#################################################################################
#										
# CmplGridScheduler								
#										
#################################################################################		
class CmplGridScheduler(object):
		 
	#****************************************************************************
	# Constructor and destructor					        
	#****************************************************************************
		 
	#*********** constructor **********
	def __init__(self, port = None ):
	
		self.__compatibility = COMPATIBILITY
		
		self.__server = None
		self.__serverList = {} 
		self.__problemQueueInfos = {}
		self.__problemQueue = []
		
		self.__maxCmplServerTries = 10
		self.__maxInactivityTime=60*60*12 # half a day 
		
		self.__schedulerStatus = CMPLGRID_SCHEDULER_OK
		self.__schedulerStatusTxt = "CMPLGrid scheduler is running"
		
		self.__solvers = ["cbc","glpk","scip","gurobi","cplex"]
	
		if port == None:
			self.__cmplPort  = 8008
		else:
			self.__cmplPort  = port
			
		self.__serviceIntervall = 0.1
		self.__serviceIntervall2 = 30
		
		self.__serviceThreadHandler = None
		
		self.__cmplServerPath = os.path.expanduser("~") + os.sep+ "CmplServer" +os.sep
		self.__logFileName = self.__cmplServerPath + "cmplGridScheduler.log"
		
			
		if os.path.exists(self.__cmplServerPath) == False:
			try:
				os.mkdir(self.__cmplServerPath)
			except OSError, e:
				raise CmplServerException( "Cannot create CmplServer path <"+self.__cmplServerPath+">")
		
		try:
			self.__logFile = open(self.__logFileName, "a")
		except IOError, e:
			raise CmplServerException( "Cannot read CmplServer option file <"+self.__optFileName+"> " + str(e) )
		
		try:
			try:
				self.__optFileName=os.environ['CMPLSERVERPATH']+ os.sep + "cmplServer.opt"	
			except:
				self.__optFileName=os.path.dirname(os.path.abspath(sys.argv[0])) + os.sep + ".." + os.sep + "cmplServer.opt"
			
			f = open(self.__optFileName, "r") 
			for line in f: 
				ret=line.split("=")
				
				if ret[0].strip().lower() == "schedulerserviceintervall":
					if CmplTools.strIsNumber(ret[1].strip()):
						self.__serviceIntervall = float(ret[1].strip())
					else:
						CmplServerTools.cmplLogging( self.__logFile,  "Wrong schedulerServiceIntervall in CmplServer option file <"+str(self.__serviceIntervall)+"> default value is used" )
				
				if ret[0].strip().lower() == "sserviceintervall":
					if CmplTools.strIsNumber(ret[1].strip()):
						self.__serviceIntervall2 = float(ret[1].strip())
					else:
						CmplServerTools.cmplLogging( self.__logFile,  "Wrong serviceIntervall in CmplServer option file <"+str(self.__serviceIntervall2)+"> default value is used" )
				
				if ret[0].strip().lower() == "maxinactivitytime":
					if CmplTools.strIsNumber(ret[1].strip()):
						self.__maxInactivityTime = int(ret[1].strip())
						continue
					else:
						CmplServerTools.cmplLogging( self.__logFile,  "Wrong option maxInactivityTime in CmplServer option file <"+self.__optFileName+"> default value is used" )
			
				if ret[0].strip().lower() == "maxservertries":
					if CmplTools.strIsNumber(ret[1].strip()):
						self.__maxServerTries = int(ret[1].strip())
					else:
						CmplServerTools.cmplLogging( self.__logFile,  "Wrong maxServerTries in CmplServer option file <"+str(self.__maxServerTries)+"> default value is used" )
						
				"""if ret[0].strip().lower() == "solvers":
					self.__solvers=ret[1].split()"""
					
			f.close()
		
			#undocumented - only used for the pyCmpl test script
			if port != None:
				self.__cmplPort = port
		
		except IOError, e:
			raise CmplServerException( "Cannot read CmplServer option file <"+self.__optFileName+"> " + str(e), self.__logFile )
	#*********** end constructor *******	
	
	#*********** destructor ************
	def __del__(self ):
		if self.__serviceThreadHandler!=None:
			self.__serviceThreadHandler.kill()
	
		self.__logFile.close()
	#*********** end destructor ********	
	
	#****************************************************************************
	# public methods   							
	#****************************************************************************
	
		
	#*********** startCmplGridScheduler *************
	def startCmplGridScheduler(self): 
		try:
			self.__server = CmplXmlRpcServer(("", self.__cmplPort), logRequests=False) 
		
			self.__server.register_function(self.getJobId) 
			self.__server.register_function(self.knock) 
			self.__server.register_function(self.cmplServerFailed)
			self.__server.register_function(self.getServerId) 
			self.__server.register_function(self.addEmptyProblem)
			self.__server.register_function(self.disconnectServer) 
			self.__server.register_function(self.disconnectProblem) 
			
			self.__server.register_function(self.stopServer)
			self.__server.register_function(self.status)
			
			CmplServerTools.cmplLogging( self.__logFile,  "CmplGridScheduler has been started | port: "  + str(self.__cmplPort) + " | serviceIntervall: " + str(self.__serviceIntervall)  )	
			self.shutdown = False
			thread.start_new_thread(self.__serviceThread, () ) 
			while not self.shutdown:
				self.__server.handle_request()
		except:
			print "CmplGridScheduler error: " , str(sys.exc_info()[1])
				
	#*********** end startCmplGridScheduler **********
	
	
	#*********** getJobId **************		
	def getJobId(self, cmplName, solver, compatibility=0):
	
		id = "G"+self.__server.client_address[0] + "-" + strftime("%Y-%m-%d-%H-%M-%S", gmtime()) + "-" + str(random.randint(100000, 999999))
			
		status = CMPLGRID_SCHEDULER_OK
				
		statusMessage = ""
		
		if 	int(compatibility)!=self.__compatibility:
			status =  CMPLSERVER_ERROR
			statusMessage = "Incompatible CmplServer client with compatibilty stage "+str(compatibility) + " instead of " + str(self.__compatibility)
		else:
			statusMessage = str(self.__compatibility)
		
		tmpSolver=solver.lower()
		
		if not tmpSolver in self.__solvers:
			status=CMPLSERVER_ERROR
			statusMessage="Unsupported solver <"+ solver + ">"
		
		if status==CMPLGRID_SCHEDULER_OK:
			CmplServerTools.cmplLogging( self.__logFile,  "Problem has been registered" , id, cmplName )
			
			if len(self.__problemQueue)>0:
				status = CMPLGRID_SCHEDULER_BUSY
				self.__problemQueue.append(id)
				self.__problemQueueInfos.update( {id : ProblemQueueHandler(cmplName, tmpSolver) } ) 	
				CmplServerTools.cmplLogging( self.__logFile, "CmplGrid is busy: Problem is moved to the problem queue. " , id, cmplName)
				statusMessage = "CmplGrid is busy: Problem is moved to the problem queue. "
			else:			
				bestServer, status = self.__getBestServer(tmpSolver)
					
				if status==CMPLGRID_SCHEDULER_SOLVER_NOT_AVAILABLE:
					status =  CMPLSERVER_ERROR
					statusMessage = "Solver <"+solver + "> not available in the CmplGrid "
					
				elif status==CMPLGRID_SCHEDULER_BUSY:
					
					self.__problemQueue.append(id)
					self.__problemQueueInfos.update( {id : ProblemQueueHandler(cmplName, tmpSolver) } )
					self.__problemQueueInfos[id].setStatus(status)
					status = CMPLGRID_SCHEDULER_BUSY
	
					CmplServerTools.cmplLogging( self.__logFile, "CmplGrid is busy: Problem is moved to the problem queue. ", id, cmplName)
					
					if self.__nrOfActiveServers()>0:
						statusMessage = "CmplGrid is busy: Problem is moved to the problem queue. "
					else:
						statusMessage = "No server available at the moment in the CMPLGrid: Problem is moved to the problem queue. "
						
					self.__schedulerStatus = CMPLGRID_SCHEDULER_BUSY
					self.__schedulerStatusTxt = "CMPLGrid scheduler is busy"
			
				elif status==CMPLGRID_SCHEDULER_OK: 	
					if self.__sendProblemToServer( bestServer, id, cmplName):
						statusMessage = self.__serverList[bestServer].cmplServerUrl
						
			
					
		return [ status, statusMessage, id]
	#*********** end getJobId ************		
			
	#*********** knock **************		
	def knock(self, id):
		status=CMPLGRID_SCHEDULER_UNKNOWN
		statusMessage=""
		serverUrl=""
		
		if id in self.__problemQueueInfos:
			status = self.__problemQueueInfos[id].status
			serverId = self.__problemQueueInfos[id].cmplServerId
			
			if status == CMPLGRID_SCHEDULER_OK:
				if serverId == None:
					status = CMPLGRID_SCHEDULER_BUSY
					statusMessage = "CmplGrid scheduler is waiting for a free CmplServer"
				else:
					if self.__checkId(serverId):
						statusMessage=self.__serverList[serverId].cmplServerUrl
						del self.__problemQueueInfos[id]
					else:
						status = CMPLGRID_SCHEDULER_ERROR
						statusMessage = "...Server isn't connected"
			elif status==CMPLGRID_SCHEDULER_PROBLEM_DELETED:
				status = CMPLGRID_SCHEDULER_ERROR
				statusMessage = "The problem was to long inactive and was therefore deleted. "
				del self.__problemQueueInfos[id]
				
		else: 
			status = CMPLGRID_SCHEDULER_ERROR
			statusMessage = "Problem is not connected to CMPLGrid <"+id+">"
		
			
		return [status, statusMessage, id ]
	
	#*********** end knock **************	
	
	
	#*********** cmplServerFailed **************		
	def cmplServerFailed(self, cmplUrl):
	
		status=CMPLGRID_SCHEDULER_WARNING
		statusMessage="Unknown CmplServer can't registred as inactive <"+cmplUrl+">"
	
		for s in self.__serverList:
			if self.__serverList[s].cmplServerUrl==cmplUrl:
				self.__serverList[s].setActive(False)
				status=CMPLGRID_SCHEDULER_OK
				statusMessage="CmplServer is now registred as inactive <"+cmplUrl+">"
		
		CmplServerTools.cmplLogging( self.__logFile,  statusMessage )
			
		return [status, statusMessage, "" ]
	
	#*********** end cmplServerFailed **************				
		
	#*********** getServerId **************		
	def getServerId(self, port, maxProblems, performanceIndex, solvers, compatibility=0):
	
		tmpUrl = self.__server.client_address[0]+":"+str(port)
		id = tmpUrl + "-"+ str(random.randint(100000, 999999))
			
		status = CMPLGRID_SCHEDULER_OK
		statusMessage=""
				
		if type(port)!=int:
			status= CMPLGRID_SCHEDULER_ERROR
			statusMessage = "Wrong CmplServer port ", port
		else:
			tmpUrl= "http://"+tmpUrl
	
			self.__serverList.update( { id: CmplServerHandler(tmpUrl, int(maxProblems) , int(performanceIndex) ) } ) 
			
			if 	int(compatibility)!=self.__compatibility:
				status= CMPLGRID_SCHEDULER_ERROR
				statusMessage = "Incompatible CmplServer with compatibilty stage "+str(compatibility) + " instead of " + str(self.__compatibility)
			else:
				statusMessage = str(self.__compatibility)
				
			self.__serverList[id].setLastActivityTime(time.time())
			
			self.__serverList[id].setSolvers(solvers)
			
			try:
				self.__serverList[id].setCmplServer( xmlrpclib.ServerProxy( self.__serverList[id].cmplServerUrl , allow_none=False) )
			except:
				CmplServerTools.cmplLogging( self.__logFile,  "CmplServer can't connect - no bi-directional connection :"+ str(sys.exc_info()[1]) , id )
				status = CMPLGRID_SCHEDULER_ERROR
				statusMessage = "CmplServer can't connect - no bi-directional connect :"+ str(sys.exc_info()[1])
				
			if status == CMPLGRID_SCHEDULER_OK:
				CmplServerTools.cmplLogging( self.__logFile,   "CmplServer has been connected: solver "+str(self.__serverList[id].solvers)+" : maxProblems :" + str(self.__serverList[id].emptyProblems) + ">" , id )
	
	
		return [ status, statusMessage, id]
	#*********** end getServerId ************	
	
	
	#*********** addEmptyProblem **************		
	def addEmptyProblem(self, serverId):
		
		if self.__checkId(serverId):
			self.__serverList[serverId].addEmptyProblem()
			status = CMPLGRID_SCHEDULER_OK
			statusMessage = "Empty problem has added"
			
			CmplServerTools.cmplLogging( self.__logFile,  "CmplServer has added empty problem " , serverId )
			
		else:
			status = CMPLGRID_SCHEDULER_ERROR
			statusMessage = "Server isn't connected"
		
		return [status, statusMessage, "" ]
	
	#*********** end addEmptyProblem **************		
	
		
	#*********** disconnectServer **************		
	def disconnectServer(self, id):
		status=None
		statusMessage=None
		
		if id in self.__serverList:
			del self.__serverList[id]
			status = CMPLGRID_SCHEDULER_OK
			statusMessage = "CmplServer <" + id +"> disconnected"
			CmplServerTools.cmplLogging( self.__logFile,  "CmplServer has been disconnected " , id )
		else:
			status = CMPLGRID_SCHEDULER_WARNING
			statusMessage = "CmplServer <" + id +"> wasn't connected"
			
		return [ status, statusMessage, ""]
	
	#*********** end disconnectServer ************	
	

	#*********** disconnectProblem **************		
	def disconnectProblem(self, id):
		status=None
		statusMessage=None
		
		if id in self.__problemQueue:
			del self.__problemQueue[self.__problemQueue.index(id)]
			status = CMPLGRID_SCHEDULER_OK
			statusMessage = "Problem <" + id +"> disconnected"
			CmplServerTools.cmplLogging( self.__logFile,  "Problem has been disconnected from problem queue." , id )
		else:
			status = CMPLGRID_SCHEDULER_WARNING
			statusMessage = "Problem <" + id +"> wasn't connected"
			
		return [ status, statusMessage, ""]
	
	#*********** end disconnectProblem ************	
	
	
	#*********** stopServer **************
	def stopServer(self):
		if  self.__server.client_address[0] == "127.0.0.1":
			
			while len( self.__serverList) >0:
				
				id = self.__serverList.keys()[0] 
				
				ret=self.__cmplServerExecute(self.__serverList[id].cmplServer, "disconnectFromScheduler", id)
				if ret[0]==CMPLSERVER_OK:
					self.disconnectServer(id)
				else:
					CmplServerTools.cmplLogging( self.__logFile,  "Can't disconnect CmplServer <" + id +"> : " + ret[1])
					
			self.__serverList.clear()
						
			self.shutdown = True
			CmplServerTools.cmplLogging( self.__logFile,  "CmplGridScheduler has been stopped" )	
			return True
		else:
			return False
	#*********** end stopServer **********	
	
	
	#*********** status ***************
	def	status(self):
		#CmplServerTools.cmplLogging( self.__logFile,  "Status check: " + str(self.__schedulerStatus)  )
		return [self.__schedulerStatus, self.__schedulerStatusTxt, ""]
	#*********** end status ***********	
	
	
	#****************************************************************************
	# private methods							
	#****************************************************************************
	
	#*********** checkId ***************
	def __checkId(self, id) :
		return id in self.__serverList
	#*********** end checkId ***********
		
	#*********** nrOfActiveServers ***************
	def __nrOfActiveServers(self) :
		i=0
		for s in self.__serverList:
			if self.__serverList[s].isActive:
				i+=1
		return i
	#*********** end __nrOfActiveServers ***********
	
	
	#*********** __serviceThread ******
	def __serviceThread(self):
		
		lastActivityTime=time.time()
		status = CMPLGRID_SCHEDULER_OK
		
		while True:
			
			ret=[]
			if self.shutdown==True:
				break
			
			time.sleep(self.__serviceIntervall)
			
			if time.time()-lastActivityTime>self.__serviceIntervall2:
				self.__cleanOldProblems()
				lastActivityTime=time.time()
			
			if len(self.__problemQueue)>0:
				
				tmpId=self.__problemQueue.pop(0)
				bestServer, status = self.__getBestServer(self.__problemQueueInfos[tmpId].solver)
				
				if status==CMPLGRID_SCHEDULER_SOLVER_NOT_AVAILABLE:
					self.__problemQueueInfos[tmpId].setStatus=CMPLGRID_SCHEDULER_PROBLEM_DELETED
					status =  CMPLSERVER_ERROR
					statusMessage = "Solver <"+solver + "> not available in the CmplGrid "

				elif status==CMPLGRID_SCHEDULER_BUSY:
					self.__problemQueue.insert(0,tmpId)
					self.__problemQueueInfos[tmpId].setStatus(status)

				elif status==CMPLGRID_SCHEDULER_OK: 
			
					ans = self.__sendProblemToServer(bestServer, tmpId, self.__problemQueueInfos[tmpId].cmplName)
			
					if ans==True:
						self.__problemQueueInfos[tmpId].setStatus(CMPLGRID_SCHEDULER_OK)
						self.__problemQueueInfos[tmpId].setCmplServerId(bestServer)
					else:
						self.__problemQueue.insert(0,tmpId)
						self.__problemQueueInfos[tmpId].setStatus(CMPLGRID_SCHEDULER_BUSY)
				
			if len(self.__problemQueue)>0:		
				self.__schedulerStatus = CMPLGRID_SCHEDULER_BUSY
				self.__schedulerStatusTxt = "CMPLGrid scheduler is busy"
			else: 
				self.__schedulerStatus = CMPLGRID_SCHEDULER_OK
				self.__schedulerStatusTxt = "CMPLGrid scheduler is running"
			
			for s in self.__serverList:
				if self.__serverList[s].isActive==False:
		
					oldMaxTries=self.__maxCmplServerTries
					self.__maxCmplServerTries=1
					ret=self.__cmplServerExecute(self.__serverList[s].cmplServer, "status")
					self.__maxCmplServerTries=oldMaxTries
					
					if ret[0]==CMPLSERVER_OK:
						self.__serverList[s].setActive(True)
						self.__serverList[s].setEmptyProblems(ret[2])
						CmplServerTools.cmplLogging( self.__logFile,  "CmplServer is now registred as active <"+self.__serverList[s].cmplServerUrl+"> " + str(self.__serverList[s].emptyProblems) )
			
	#******* end __serviceThread *******
	
	
	#*********** cleanOldProblems ******
	def __cleanOldProblems(self):
		delList = []

		for id in self.__problemQueue:	
			if (time.time()-self.__problemQueueInfos[id].lastActivityTime)>self.__maxInactivityTime:
				delList.append(id)
		
		for id in delList:
			self.__problemQueueInfos[id].setStatus=CMPLGRID_SCHEDULER_PROBLEM_DELETED
			del self.__problemQueue[id]
			CmplServerTools.cmplLogging( self.__logFile,  "Inactive problem has been canceled and deregistered" , id, self.__problemQueue[id].cmplName)
		
	#******* end __cleanOldProblems ******
	
	
	#*********** cmplServerExecute *******
	def __cmplServerExecute(self, server, method="", *args):
		ret=[]
		tries=0
		while True:
			try:
				if method=="status":
					ret = server.status()
				if method=="sendJobId":
					ret = server.sendJobId(args[0], args[1], args[2])
				if method=="disconnectFromScheduler":
					ret = server.disconnectFromScheduler(args[0])	
	
			except :
				tries+=1
				if tries==self.__maxCmplServerTries:
					ret=[CMPLSERVER_ERROR,  str(sys.exc_info()[1]) , ""]
					break
				else:	
					continue		
			break
			
		return ret
	#******** end cmplServerExecute *******
	
	#*********** __sendProblemToServer **************		
	def __sendProblemToServer(self, bestServer, id, cmplName):
	
		ans=True
		ret = self.__cmplServerExecute(self.__serverList[bestServer].cmplServer, "sendJobId", cmplName,  id, bestServer)	
		if ret[0] == CMPLSERVER_ERROR:
			self.__serverList[bestServer].setActive(False)
			ans = False
		else: 
			CmplServerTools.cmplLogging( self.__logFile,  "Problem has been sent to CmplServer <"+ bestServer +"> " , id, cmplName )
			ans=True
	
		return ans
	#*********** end __sendProblemToServer ************	
	
	#*********** getBestServer **************		
	def __getBestServer(self, solver):
		bestServer=None
		status=CMPLGRID_SCHEDULER_OK
		bestFactor=0
		activeServerFound=False
			
		for s in self.__serverList:
			if self.__serverList[s].isActive:
				if solver in self.__serverList[s].solvers:
					activeServerFound=True
					tmpFactor=(self.__serverList[s].emptyProblems/self.__serverList[s].maxProblems * self.__serverList[s].performanceIndex) 
					if tmpFactor > bestFactor:
						bestFactor = tmpFactor
						bestServer = s
		
		if  bestServer!=None:
			self.__serverList[bestServer].removeEmptyProblem()
		else:
			if not activeServerFound:
				status=CMPLGRID_SCHEDULER_SOLVER_NOT_AVAILABLE 
			else:
				status=CMPLGRID_SCHEDULER_BUSY 
		
		return (bestServer, status)
	#*********** end getBestServer ************		
		


#################################################################################
# End CmplGridScheduler								
#################################################################################	  
	


	
	



		
		
		
		

		