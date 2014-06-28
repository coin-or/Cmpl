#***********************************************************************
 #  This code is part of pyCMPL 
 #
 #  Copyright (C) 2013, 2014
 #  Mike Steglich - Technical University of Applied Sciences
 #  Wildau, Germany 
 #
 #  pyCMPL is a project of the Technical University of 
 #  Applied Sciences Wildau and the Institute for Operations Research 
 #  and Business Management at the Martin Luther University 
 #  Halle-Wittenberg.
 #  Please visit the project homepage <www.coliop.org>
 # 
 #  pyCMPL is free software; you can redistribute it and/or modify it 
 #  under the terms of the GNU Lesser General Public License as published by 
 #  the Free Software Foundation; either version 3 of the License, or 
 #  (at your option) any later version.
 # 
 #  pyCMPL is distributed in the hope that it will be useful, but WITHOUT
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

"pyCmpl - CMPL's python API "
__author__ = "Mike Steglich"
__copyright__ = "Copyright (C) 2013,2014 Mike Steglich"
__license__ = "LGPLv3"
__version__ = "1.2"


from math import *
from multiprocessing import Process

import os
import threading
import random
import subprocess
import sys
import xmlrpclib
import time
import socket
import time

from CmplDefs import *
from CmplException import *
from CmplMsg import *
from CmplTools import *
from CmplSolution import *
from CmplSet import *
from CmplParameter import *
from CmplInstance import *
from CmplInfo import *

#################################################################################
#										
# Cmpl										
#										
#################################################################################	
class Cmpl(threading.Thread):

	#********************************************************************************
	# Constructor and destructor							
	#********************************************************************************
	
	#*********** constructor **********
	def __init__(self, model):
		
		threading.Thread.__init__(self)
		
		self.__compatibility = COMPATIBILITY
		
		self.__problem = ""
		self.__cmplDataStr = ""
			
		self.__setList = []
		self.__parameterList = []
		self.__optionsList = {}
				
		self.__status = None 
		self.__solutions = None
		self.__solutionString = ""
		 
		self.__cmplInfos = None
		self.__cmplInfoString = ""
		
		self.__cmplFileList = []
		self.__dataFileList = []
		
		self.__remoteMode = False
		self.__remoteStatus = CMPL_UNKNOWN
		self.__remoteStatusMessage = ""
	
		self.__jobId = ""
		self.__instStr = ""
		
		self.__solver="cbc"
		
		self.__cmplServer = None
		self.__cmplUrl = ""
		self.__cmplGridScheduler = None
		self.__cmplGridSchedulerUrl = ""
		self.__serverMode = SERVER_UNKNOWN
		
		self.__cmplServerRunning = False
		
		self.__maxCmplServerTries = 10
		self.__maxCmplServerQueuingTime = 5*60 #in seconds
		
		self.__cmplDataFile = None
		self.__cmplMsgFile = None
		self.__cmplSolFile = None
		
		self.__cmplProc = None
		
		self.__refreshTime = 0.5
		self.__printOutput = False
		self.__outputString = ""
		
		self.__isCleaned = False
		self.__debug = False
			
		if type(model) != str:
			raise CmplException( str(model) + " is not a valid file name for a Cmpl file" )
		else:
			self.__model = model
		
		if not os.path.isfile(model):
			raise CmplException("CMPL file " + model + " does not exist."  )
		
		self.__id = str(random.randint(100000, 999999))
		
		self.__outputLeadString = os.path.basename(os.path.splitext(self.__model)[0]) + "> " 
	
	#*********** end constructor ********
		
	#*********** destructor *************
	def __del__(self):
		try:
			self.__cleanUp()
		except:
			pass
	#*********** end destructor *********
	
	#********************************************************************************
	# public methods   								
	#********************************************************************************
	
	#*********** model (getter) *********	
	@property
	def model(self):
		return self.__model
	#*********** end model **************
	
	
	#*********** problem (getter) *********	
	@property
	def problem(self):
		return self.__problem
	#*********** end model **************
	
		
	#*********** refreshTime ************	
	@property
	def refreshTime(self):
		return self.__refreshTime
	#*********** end refreshTime ********
	
	
	#*********** output *****************
	@property
	def output(self):
		return self.__outputString 
	#*********** end output  ************
	
	
	#*********** cmplMessages ************
	@property
	def cmplMessages(self):
		return self.__status.cmplMessageList 
	#*********** end cmplMessages ********	
	
	
	#*********** solutions ***************
	@property
	def solutionPool(self):
		if self.__solutions==None:
			raise CmplException("No Solution found so far")
		elif self.__solutions.nrOfSolutions>0:
			return self.__solutions.solutions
		else: 
			raise CmplException("No Solution found so far")
	#*********** end solutions ***********
	
	
	#*********** solutions ***************
	@property
	def solution(self):
		if self.__solutions==None:
			raise CmplException("No Solution found so far")
		elif self.__solutions.nrOfSolutions>0:
			return self.__solutions.solution
		else: 
			raise CmplException("No Solution found so far")
	#*********** end solutions ***********	
	
	
	#*********** nrOfVariables ***************
	@property
	def nrOfVariables(self):
		if self.__solutions==None:
			raise CmplException("The model isn't generated yet.")
		else:
			return self.__solutions.nrOfVariables
	#*********** end nrOfVariables ***********
	
	
	#*********** nrOfConstraints ***************
	@property
	def nrOfConstraints(self):
		if self.__solutions==None:
			raise CmplException("The model isn't generated yet.")
		else:
			return self.__solutions.nrOfConstraints
	#*********** end nrOfConstraints ***********
	
	#*********** objectiveName ***************
	@property
	def objectiveName(self):
		if self.__solutions==None:
			raise CmplException("No Solution found so far")
		else:
			return self.__solutions.objectiveName
	#*********** end objectiveName ***********
	
			
	#*********** objectiveSense ***************
	@property
	def objectiveSense(self):
		if self.__solutions==None:
			raise CmplException("No Solution found so far")
		else:
			return self.__solutions.objectiveSense
	#*********** end objectiveSense ***********
	

	#*********** nrOfSolutions ****************
	@property
	def nrOfSolutions(self):
		if self.__solutions==None:
			raise CmplException("No Solution found so far")
		else:
			return self.__solutions.nrOfSolutions
	#*********** end nrOfSolutions *************
	

	#*********** solver ************************
	@property
	def solver(self):
		if self.__solutions==None:
			raise CmplException("Since the model isn't solved the solver is not known.")
		else:
			return self.__solutions.solver
	#*********** end solver ********************
	

	#*********** solverMessage *****************
	@property
	def solverMessage(self):
		if self.__solutions==None:
			raise CmplException("Since the model isn't solved the solver message is not known.")
		else:
			return self.__solutions.solverMessage
	#*********** end nrOfSolutions *************
	

	#*********** varDisplayOptions *************
	@property
	def varDisplayOptions(self):
		if self.__solutions==None:
			raise CmplException("Since the model isn't solved this option isn't known.")
		else:
			return self.__solutions.varDisplayOptions
	#*********** end varDisplayOptions *********
	

	#*********** conDisplayOptions *************
	@property
	def conDisplayOptions(self):
		if self.__solutions==None:
			raise CmplException("Since the model isn't solved this option isn't known.")
		else:
			return self.__solutions.conDisplayOptions
	#*********** end conDisplayOptions *********
	
	
	#*********** cmplStatus **************	
	@property
	def cmplStatus(self):
		if self.__remoteMode and self.__remoteStatus!=CMPL_UNKNOWN:
			return self.__remoteStatus
		else:
			return self.__status.cmplStatus
	#*********** end cmplStatus ***********	
	
	
	#*********** cmplStatusText ***********	
	@property
	def cmplStatusText(self):
		status = 0
		if self.__remoteMode and self.__remoteStatus!=CMPL_UNKNOWN:
			status = self.__remoteStatus
		else:
			status = self.__status.cmplStatus
		return CMPL_STATUS_TXT[status] 
	#*********** end cmplStatusText ******	
	
	
	#*********** solverStatus ************	
	@property
	def solverStatus(self):
		if self.__solutions.nrOfSolutions==0:
			return SOLVER_FAILED
		else:
			return SOLVER_OK
	#*********** end solverStatus ********	
	
	
	#*********** solverStatusText ********	
	@property
	def solverStatusText(self):
		if self.__solutions.nrOfSolutions==0:
			return "SOLVER_FAILED"
		else:
			return "SOLVER_OK"
	#*********** end solverStatusText ****	
	
	
	#*********** cmplSolFile *************	
	@property
	def cmplSolFile(self):
		return self.__solutions.cmplSolFile
	#********* end cmplSolFile ***********
	
	
	#*********** csvSolFile **************	
	@property
	def csvSolFile(self):
		return self.__solutions.csvSolFile
	#********* end csvSolFile ************
	
	
	#*********** asciiSolFile ************	
	@property
	def asciiSolFile(self):
		return self.__solutions.asciiSolFile
	#********* end asciiSolFile **********
	

	#*********** jobId *******************	
	@property
	def jobId(self):
		return self.__jobId
	#*********** end jobId ***************
	
	
	#*********** setOutputPipe **********
	def setOutput(self, ok=False, lStr=None):
		self.__printOutput = ok
		if lStr != None:
			self.__outputLeadString = lStr
	#*********** end setOutputPipe ******
	
	
	#*********** setRefreshTime *********	
	def setRefreshTime(self, rTime):
		self.__refreshTime = rTime
	#*********** end setRefreshTime *****
	
		
	#*********** setSet *****************	
	def setSet(self, set):
		if type(set) != CmplSet:
			raise CmplException(str(set)+ " is not a CMPL set ")
		else: 
			if len(set.valueList) != 0:
				self.__setList.append(set)
			else:
				raise CmplException("set " + set.name()+ " contains no elements ")

	#*********** end setSet **************	
	
	
	#*********** setSets *****************	
	def setSets(self, *set):
		for s in set:
			self.setSet(s)
	#*********** end setSets *************	
	
		
	#*********** setParameter ************	
	def setParameter(self, param):
		if type(param) != CmplParameter:
			raise CmplException("Cmpl.setParameter: " + str(param)+ " is not a CMPL parameter ")
		else: 
			if len(param.values) != 0:
				self.__parameterList.append(param)
			else:
				raise CmplException("parameter " + param.name()+ " contains no elements ")

	#*********** end setParameter ********	
	

	#*********** setSets *****************	
	def setParameters(self, *params):
		for p in params:
			self.setParameter(p)
	#*********** end setSets *************	
	
	
	#*********** setOption ***************	
	def setOption(self, option):
		if type(option) != str:
			raise CmplException(str(option)+ " is not a valid CMPL option ")
		else:
			pos = len(self.__optionsList)
			self.__optionsList.update({pos:option})
		return pos
	#*********** end setOption ***********	
	
	
	#*********** setOption ***************	
	def setOptionsList(self, optionList):
		if type(optionList) != dict:
			raise CmplException("Wrong option list ")
		else:
			self.__optionsList=optionList
	#*********** end setOption ***********	
	
		
	#*********** delOption ***************	
	def delOption(self, pos):
		if pos not in self.__optionsList:
			raise CmplException(str(pos)+" is not a valid CMPL option ")
		else:
			del self.__optionsList[pos]
	#*********** end delOption ***********	
	
	
	#*********** delOptions ***************	
	def delOptions(self):
		self.__optionsList = {}
	#*********** end delOptions ***********	
	
	
	#**** setMaxServerQueuingTime *********	
	def setMaxServerQueuingTime(self, qTime):
		self.__maxCmplServerQueuingTime = qTime
	#** end setMaxServerQueuingTime *******		


	#**** maxServerQueuingTime ************
	@property	
	def maxServerQueuingTime(self):
		return self.__maxCmplServerQueuingTime
	#** end maxServerQueuingTime **********	


	#**** setMaxServerTries ***************	
	def setMaxServerTries(self, tries):
		self.__maxCmplServerTries = tries
	#** end setMaxServerTries *************		
	
	
	#**** maxServerTries ******************
	@property		
	def maxServerTries(self):
		return self.__maxCmplServerTries 
	#** end maxServerTries ****************	
	
	
	#*********** debug *******************	
	def debug(self, mode=True):
		self.__debug = mode
	#*********** end debug ***************
	
		
	#*********** getVarByName ************	
	def getVarByName(self, name, solNr=0):
		if solNr<0 or solNr>self.__solutions.nrOfSolutions-1:
			raise CmplException("Solution with index " + str(solNr) + " doesn't exist.")
		s = self.__solByNr(solNr)
		return self.__getElementByName(name,s.variables)
	#*********** end getVarByName ********	
	
	
	#*********** getConByName ************	
	def getConByName(self, name, solNr=0):
		if solNr<0 or solNr>self.__solutions.nrOfSolutions-1:
			raise CmplException("Solution with index " + str(solNr) + " doesn't exist.")
		s = self.__solByNr(solNr)
		return self.__getElementByName(name,s.constraints)
	#*********** end getConByName ********	
	
	
	#*********** varByName ***************	
	def varByName(self, solNr=0):
		if solNr<0 or solNr>self.__solutions.nrOfSolutions-1:
			raise CmplException("Solution with index " + str(solNr) + " doesn't exist.")
		s = self.__solByNr(solNr)
		self.__elementByName(s.variables)
	#*********** end varByName ***********
	
	
	#*********** conByName ***************	
	def conByName(self, solNr=0):
		if solNr<0 or solNr>self.__solutions.nrOfSolutions-1:
			raise CmplException("Solution with index " + str(solNr) + " doesn't exist.")
		s = self.__solByNr(solNr)
		self.__elementByName(s.constraints)
	#*********** end conByName ***********
	
	
	#*********** solve *******************	
	def solve(self):
		
		
		
		if self.__remoteMode:
		
			if not self.__cmplServerRunning:
				raise CmplException("Model is not connected to a CmplServer" )
				
			self.__status = CmplMessages()
			self.__solutions = CmplSolutions()
		
			tries=0
			while True:
			#loop is intended for CMPLGrid
			
				self.__isCleaned=False
				
				try:
					if self.__remoteStatus == CMPLSERVER_CLEANED:
						self.connect(self.__cmplUrl)
					
					if self.__serverMode == CMPL_GRID and self.__remoteStatus == CMPLSERVER_ERROR:
						self.connect(self.__cmplUrl)
								
				
					if self.__remoteStatus == CMPLGRID_SCHEDULER_BUSY: 
						startTime=time.time()
						while self.__remoteStatus != CMPLGRID_SCHEDULER_OK:
							time.sleep(self.__refreshTime)
							self.__knockScheduler()
												
							if (time.time()-startTime)>=self.__maxCmplServerQueuingTime:
								self.__cleanUp()
								raise CmplException("maximum CmplServer queuing time is exceeded.")			
				
					self.send()
												
					if self.__debug:
						instFile = self.__problem+".cinst"
						try:
							f = open(instFile, 'w')
							f.write(self.__instStr)		
							f.close()
						except IOError, e:
							raise CmplException("IO error for file " + instFile + ": "+e)
			
				
					if self.__remoteStatus == CMPLSERVER_BUSY: 
						startTime=time.time()
				
					while self.__remoteStatus!=PROBLEM_FINISHED:
						self.knock()
					
						time.sleep(self.__refreshTime)
						if self.__remoteStatus == CMPLSERVER_BUSY and (time.time()-startTime)>=self.__maxCmplServerQueuingTime:
							raise CmplException("maximum CmplServer queuing time is exceeded.")	
				
					self.retrieve()
				
					break
					
				except CmplException, e:
				
					if self.__serverMode == CMPL_GRID and self.__remoteStatus == CMPLSERVER_ERROR and self.__cmplGridSchedulerUrl!="":
					
						try:
							self.__cmplServerExecute("cmplServerFailed")
							self.__handleOutput("CmplServer failed <"+self.__cmplUrl+">: Problem will be newly connected to CmplGridScheduler and commited to another CmplServer." )
							
							self.__cmplUrl = self.__cmplGridSchedulerUrl
							self.__cmplGridSchedulerUrl = ""
							self.__cmplGridScheduler=None
							self.__cmplServer=None
						
						except  CmplException, e:
							raise CmplException("CmplGridScheduler failed: " + e.msg )
							
						tries+=1
						if tries==self.__maxCmplServerTries:
							raise CmplException(e.msg )
						
						continue	
					else:
						raise CmplException(e.msg)
					
		else:
		
			self.__problem = os.path.splitext(self.__model)[0]  
			self.__cmplDataFile = self.__problem + "_" + self.__id+".cdat"
			self.__cmplMsgFile = self.__problem + "_" + self.__id+".cmsg"
			self.__cmplSolFile = self.__problem + "_" + self.__id+".csol"
			
			self.__cmplDataElements()
			
			self.__status = CmplMessages(self.__cmplMsgFile)
			self.__solutions = CmplSolutions(self.__cmplSolFile)
			
			tmpAlias = os.path.basename(self.__problem + "_" + self.__id) 
			
			try:
				cmplBin=os.environ['CMPLBINARY']
				if not os.path.exists(cmplBin):
					raise CmplException("Cant't find Cmpl binary: " + cmplBin )
			except:
				raise CmplException("Cant't find Cmpl binary" )
	
			cmdList = [cmplBin, self.__model,  "-solution", "-e", "-alias", tmpAlias ]
			if len(self.__optionsList) != 0:
				for opt in self.__optionsList:
					cmdList.append("-headerOpt")
					cmdList.append(self.__optionsList[opt].replace(" ", "#") )
					
			self.__cmplProc = subprocess.Popen(cmdList, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
			
			for line in iter(self.__cmplProc.stdout.readline, ''):
					self.__handleOutput(line)
					
			if self.__cmplProc.wait() != 0:
				raise CmplException(self.__cmplProc.stderr.read() )
						
			self.__status.readCmplMessages()
						
			if self.__status.cmplStatus==CMPL_FAILED:
				raise CmplException("Cmpl finished with errors", self.__status.cmplMessageList )
				
			self.__solutions.readSolution()
			self.__writeSolFiles()
			
			self.__cleanUp()
			
	#*********** end solve ***************	
	
	
	#*********** run  ********************	
	def run(self):
		try:
			self.solve()
		except CmplException, e:
			sys.stderr.write(e.msg+"\n")
			
	#*********** end run *****************
	
	
	#*********** connect  ****************	
	def connect(self, cmplUrl):
		
		if self.__remoteStatus != CMPL_UNKNOWN and self.__remoteStatus != CMPLSERVER_CLEANED  and self.__serverMode != CMPL_GRID:
			raise CmplException("Problem is still connected with CMPLServer: at " + cmplUrl + " with jobId " + self.__jobId)
		
		tries=0
		while True:
			self.__remoteMode = True
			self.__remoteStatus = CMPL_UNKNOWN
			self.__serverMode = STANDALONE_SERVER
			self.__cmplServer=None
			self.__cmplUrl=cmplUrl
			self.__cmplGridSchedulerUrl = ""
			self.__cmplGridScheduler=None
		
			try:
				self.__cmplServer = xmlrpclib.ServerProxy(cmplUrl)
				
				self.__getSolver()	
				
				ret = self.__cmplServer.getJobId(os.path.basename(self.__model), self.__solver, self.__compatibility)
				self.__remoteStatus=ret[0]
				
				if self.__remoteStatus == CMPLSERVER_OK or self.__remoteStatus == CMPLGRID_SCHEDULER_OK or self.__remoteStatus == CMPLGRID_SCHEDULER_BUSY:
					self.__jobId = ret[2]
				
				if self.__remoteStatus == CMPLSERVER_OK or self.__remoteStatus == CMPLSERVER_ERROR:
					self.__serverMode = STANDALONE_SERVER
		
				elif self.__remoteStatus==CMPLGRID_SCHEDULER_OK  or self.__remoteStatus==CMPLGRID_SCHEDULER_ERROR or self.__remoteStatus==CMPLGRID_SCHEDULER_BUSY:
					self.__serverMode = CMPL_GRID
					self.__handleOutput("Connected with CmplGridScheduler at " + self.__cmplUrl + " with jobId " + self.__jobId )
					
					if self.__remoteStatus==CMPLGRID_SCHEDULER_OK: 
						self.__connectServerViaScheduler(ret[1])
						self.__serverMode = CMPL_GRID
				
			except:
				tries+=1
			
				ret = str(sys.exc_info()[1])
				self.__cmplServer=None
				
				#only relevant for pyCmpl 1.0
				if ret.find("getJobId()")> -1:
					raise CmplException("Incompatible CmplServer - please install a newer CMPLServer" )
		
				if self.__remoteStatus != CMPL_UNKNOWN:
					if tries==self.__maxCmplServerTries:
						raise CmplException("CmplServer error: " + ret )
					else:
						continue
				else:
					raise CmplException("CmplServer error: " + ret )
					
			break
				
		if self.__remoteStatus == CMPLSERVER_ERROR or self.__remoteStatus==CMPLGRID_SCHEDULER_ERROR :		
			self.__cleanUp()	
			raise CmplException(ret[1])
								
		self.__cmplServerRunning =True
		
		#if self.__remoteStatus != CMPLGRID_SCHEDULER_BUSY :
		if self.__serverMode != CMPL_GRID:
			self.__handleOutput("Connected with CmplServer at " + self.__cmplUrl + " with jobId " + self.__jobId + \
					" >> maxServerTries <" + str(self.__maxCmplServerTries)+"> maxQueuingTime <"+str(self.__maxCmplServerQueuingTime)+">")
			
		if self.__remoteStatus == CMPLGRID_SCHEDULER_BUSY:	
			self.__handleOutput(ret[1])
		
	#*********** end connect *************
	
	
	#*********** connect  ****************	
	def disconnect(self):
		self.__cleanUp()
		
		self.__remoteStatus = CMPL_UNKNOWN
		self.__cmplServer=None
		self.__cmplUrl=""
		self.__remoteMode=False
		self.__serverMode = STANDALONE_SERVER
	#*********** end connect *************
	
			
	#*********** send ********************	
	def send(self):
		if self.__remoteMode:

			if not self.__cmplServerRunning:
				raise CmplException("Model is not connected to a CmplServer" )
					
			if self.__remoteStatus == CMPLSERVER_CLEANED:
				self.connect(self.__cmplUrl)
		
			self.knock()
			if self.__remoteStatus==PROBLEM_RUNNING:
				raise CmplException("Don't send the problem again before the CmplServer finished the previous one" )
		
			self.__problem = os.path.splitext(self.__model)[0]
			#self.__cmplSolFile = self.__problem+".csol"
			
			self.__cmplDataElements()
				
			self.__status = CmplMessages()
			self.__solutions = CmplSolutions()
			self.__cmplInfos = CmplInfo()
			
			self.__cmplInstance=CmplInstance()
			self.__instStr = self.__cmplInstance.cmplInstanceStr(self.__model, self.__optionsList.values(),self.__cmplDataStr, self.__jobId )
			
			ret = self.__cmplServerExecute("send")
			
			self.__remoteStatus=ret[0]
			if self.__remoteStatus == CMPLSERVER_ERROR:
				self.__cleanUp()
				raise CmplException(ret[1])
		else:
			raise CmplException("Cmpl::send can only be used in remote mode" )
	#*********** end send ****************	
	
	
	#*********** knock *******************	
	def knock(self):
		if self.__remoteMode:
			if not self.__cmplServerRunning:
				raise CmplException("Model is not connected to a CmplServer" )

			if self.__remoteStatus == CMPLSERVER_CLEANED:
				raise CmplException("Model was received and cleaned on the CmplServer" )
				
			if self.__remoteStatus!=PROBLEM_CANCELED:
				
				ret = self.__cmplServerExecute("knock")
				self.__remoteStatus=ret[0]
				
				if self.__remoteStatus == CMPLSERVER_ERROR or self.__remoteStatus == CMPL_FAILED:
					self.__cleanUp()
					raise CmplException(ret[1] )
				self.__handleOutput(ret[2])
		else:
			raise CmplException("Cmpl::knock can only be used in remote mode" )
	#*********** end knock ***************	
	
	
	#*********** retrieve ****************	
	def retrieve(self):
		if self.__remoteMode:
		
			if not self.__cmplServerRunning:
				raise CmplException("Model is not connected to a CmplServer" )

			if self.__remoteStatus == CMPLSERVER_CLEANED:
				raise CmplException("Model was received and cleaned from the CmplServer" )
		
			if self.__remoteStatus == CMPL_UNKNOWN: 
				self.knock()
			
			if self.__remoteStatus==PROBLEM_FINISHED:
			
				ret = self.__cmplServerExecute("getCmplMessages")
				
				self.__remoteStatus=ret[0]
				if self.__remoteStatus == CMPLSERVER_ERROR:
					self.__cleanUp()
					raise CmplException(ret[1] )
				else:
					self.__status.readCmplMessages(ret[2])
					
				if self.__status.cmplStatus==CMPL_FAILED:
					self.__cleanUp()
					raise CmplException("Cmpl finished with errors", self.__status.cmplMessageList )
				
				ret = self.__cmplServerExecute("getSolutions")
			
				if self.__remoteStatus == CMPLSERVER_ERROR:
					self.__cleanUp()
					raise CmplException(ret[1] )
				else:
					self.__solutionString=ret[2]
					self.__solutions.readSolution(self.__solutionString)
				
				self.__writeSolFiles()
			
				ret = self.__cmplServerExecute("getCmplInfo")
				self.__remoteStatus = ret[0]
				if self.__remoteStatus == CMPLSERVER_ERROR:
					self.__cleanUp()
					raise CmplException(ret[1] )
				else:
					self.__cmplInfoString=ret[2]
					self.__cmplInfos.readCmplInfo(self.__cmplInfoString)
				
				self.__writeInfoFiles()	
				
				self.__cleanUp()
					
			else:
				if self.__remoteStatus==PROBLEM_CANCELED:
					raise CmplException("Model has been canceled by user, cannot retrieve the solutions")
				else: 
					raise CmplException("Model is still running, cannot retrieve the solutions")
		else:
			raise CmplException("Cmpl::retrieve can only be used in remote mode" )
			
			
		#if self.__serverMode == CMPL_GRID:
		#	self.disconnect()
	#*********** end retrieve ************	
	
		
	#*********** cancel ******************	
	def cancel(self):
		if self.__remoteMode:
			if not self.__cmplServerRunning:
				raise CmplException("Model is not connected to a CmplServer" )

			if self.__remoteStatus == CMPLSERVER_CLEANED:
				raise CmplException("Model has been received and cleaned from the CmplServer" )
			
			if self.__remoteStatus!=PROBLEM_CANCELED:
		
				ret = self.__cmplServerExecute("cancel")
				self.__remoteStatus=ret[0]
				if self.__remoteStatus == CMPLSERVER_ERROR:
					self.__cleanUp()
					raise CmplException(ret[1])
		
				ret = self.__cmplServerExecute("removeProblem")
				if self.__remoteStatus == CMPLSERVER_ERROR:
					self.__cleanUp()
					raise CmplException(ret[1])
				self.__remoteStatus=CMPLSERVER_CLEANED
		else:
			self.__cleanUp()
	#*********** end cancel **************	
	
		
	#*********** saveSolution ************	
	def saveSolution(self, solFileName=None):
		if self.__solutions.nrOfSolutions > 0:
		
			if solFileName==None:
				solFile = self.__problem+".csol"
			else:
				solFile=solFileName
			
			if not self.__remoteMode:
				self.__solutionString= self.__solutions.solFileContent
								
			try:
				f = open(solFile, 'w')
				f.write(self.__solutionString)		
				f.close()
				self.__solutions.delSolFileContent()
				
			except IOError, e:
				raise CmplException("IO error for file " + solFile + ": "+str(e))
					
			self.__handleOutput( "Solution written to cmplSolution file: " + solFile )
		else:
			raise CmplException("No Solution found so far")
	#*********** end saveSolution ********	
	
	
	#*********** saveSolutionAscii *******	
	def saveSolutionAscii(self, solFileName=None):
		if self.__solutions.nrOfSolutions > 0:
			if solFileName==None:
				solFile = self.__problem+".sol"
			else:
				solFile=solFileName
			self.solutionReport(solFile)
			self.__handleOutput( "Solution written to ASCII file: " + solFile )
		else:
			raise CmplException("No Solution found so far")
	#*********** saveSolutionAscii *******	
	
	
	#*********** solutionReport **********	
	def solutionReport(self, fileName=None):
	
		repStr=""
		if self.__solutions.nrOfSolutions > 0:
			repStr += "---------------------------------------------------------------------------------------------------------\n"
			repStr += "%-20s %-s\n" %  ("Problem", os.path.basename(self.__model))
			repStr += "%-20s %-g\n" %  ("Nr. of variables" , self.__solutions.nrOfVariables)
			repStr += "%-20s %-g\n" %  ("Nr. of constraints" , self.__solutions.nrOfConstraints)
			repStr += "%-20s %-s\n" %  ("Objective name" , self.__solutions.objectiveName)
			if self.__solutions.nrOfSolutions > 1:
				repStr += "%-20s %-g\n" %  ("Nr. of solutions" , self.__solutions.nrOfSolutions)
			repStr += "%-20s %-s\n" %  ("Solver name" , self.__solutions.solver)
			repStr += "%-20s %-s\n" %  ("Display variables" , self.__solutions.varDisplayOptions)
			repStr += "%-20s %-s\n" %  ("Display constraints" , self.__solutions.conDisplayOptions)
			repStr += "---------------------------------------------------------------------------------------------------------\n"
			for s in self.__solutions.solutions:
				repStr += "\n"
				if self.__solutions.nrOfSolutions > 1:
					repStr += "%-20s %-g\n" %  ("Solution nr.", s.idx+1)
				repStr += "%-20s %-s\n" %  ("Objective status", s.status)
				repStr += "%-20s %-20.2f(%s!)\n" %  ("Objective value", s.value, self.__solutions.objectiveSense)
				repStr += "\n"
				if len(s.variables)>0:
					repStr += "%-20s\n" %  "Variables"
					repStr += "%-20s%5s%20s%20s%20s%20s\n" % ( "Name" , "Type" , "Activity", "LowerBound", "UpperBound" , "Marginal")
					repStr += "---------------------------------------------------------------------------------------------------------\n"
					for v in s.variables:
						if v.type=="C":
							repStr += "%-20s%5s%20.2f%20.2f%20.2f" % ( v.name , v.type ,v.activity, v.lowerBound,v.upperBound )
						else:
							repStr += "%-20s%5s%20g%20.2f%20.2f" % ( v.name , v.type ,v.activity, v.lowerBound,v.upperBound )
						if self.__solutions.isIntegerProgram:	
							repStr += "%20s\n" % "-"
						else:
							repStr += "%20.2f\n" % v.marginal
					repStr += "---------------------------------------------------------------------------------------------------------\n"
				if len(s.constraints)>0:
					repStr += "\n"
					repStr += "%-20s\n" %  "Constraints"
					repStr += "%-20s%5s%20s%20s%20s%20s\n" % ( "Name" , "Type" , "Activity", "LowerBound", "UpperBound" , "Marginal")
					repStr += "---------------------------------------------------------------------------------------------------------\n"
					for c in s.constraints:
						repStr += "%-20s%5s%20.2f%20.2f%20.2f" % ( c.name , c.type ,c.activity, c.lowerBound,c.upperBound )
						if self.__solutions.isIntegerProgram:	
							repStr += "%20s\n" % "-"
						else:
							repStr += "%20.2f\n" % c.marginal
					repStr += "---------------------------------------------------------------------------------------------------------\n"
					
			if fileName!=None:
				try:
					f = open(fileName, 'w')
					f.write(repStr)		
					f.close()
				except IOError, e:
					raise CmplException("IO error for file " + fileName + ": " +str(e))
			else:
				print repStr	
			
		else:
			raise CmplException("No Solution found so far")
	#*********** end solutionReport ******	
	
	
	#*********** saveSolutionCsv **********	
	def saveSolutionCsv(self, solFileName=None):
		
		if self.__solutions.nrOfSolutions > 0:
	
			if solFileName==None:
				solFile = self.__problem+".csv"
			else:
				solFile=solFileName
			try:
				f = open(solFile, 'w')
				f.write("CMPL csv export\n")
				f.write("\n")
				f.write("%s;%s\n" %  ("Problem", os.path.basename(self.__model)))
				f.write("%s;%g\n" %  ("Nr. of variables" , self.__solutions.nrOfVariables))
				f.write("%s;%g\n" %  ("Nr. of constraints" , self.__solutions.nrOfConstraints))
				f.write("%s;%s\n" %  ("Objective name" , self.__solutions.objectiveName))
				if self.__solutions.nrOfSolutions > 1:
					f.write("%s;%g\n" %  ("Nr. of solutions" , self.__solutions.nrOfSolutions))
				f.write("%s;%s\n" %  ("Solver name" , self.__solutions.solver))
				f.write("%s;%s\n" %  ("Display variables" , self.__solutions.varDisplayOptions))
				f.write("%s;%s\n" %  ("Display constraints" , self.__solutions.conDisplayOptions))
		
				for s in self.__solutions.solutions:
				
					f.write("\n")
					if self.__solutions.nrOfSolutions > 1:
						f.write("%s;%g\n" %  ("Solution nr", s.idx+1))
					f.write("%s;%s\n" %  ("Objective status", s.status))
					f.write("%s;%f;(%s!)\n" %  ("Objective value", s.value, self.__solutions.objectiveSense))
					if len(s.variables)>0:
						f.write("%s\n" %  "Variables")
						f.write("%s;%s;%s;%s;%s;%s\n" % ( "Name" , "Type" , "Activity", "LowerBound", "UpperBound" , "Marginal"))
						for v in s.variables:
							if v.type=="C":
								f.write("%s;%s;%f;%f;%f" % ( v.name , v.type ,v.activity, v.lowerBound,v.upperBound ))
							else:
								f.write("%s;%s;%g;%f;%f" % ( v.name , v.type ,v.activity, v.lowerBound,v.upperBound ))
							if self.__solutions.isIntegerProgram:	
								f.write(";-\n")
							else:
								f.write (";%f\n" % v.marginal)
					if len(s.constraints)>0:
						f.write("%s\n" %  "Constraints")
						f.write("%s;%s;%s;%s;%s;%s\n" % ( "Name" , "Type" , "Activity", "LowerBound", "UpperBound" , "Marginal"))
						for c in s.constraints:
							f.write("%s;%s;%f;%f;%f" % ( c.name , c.type ,c.activity, c.lowerBound,c.upperBound ))
							if self.__solutions.isIntegerProgram:	
								f.write(";-\n")
							else:
								f.write (";%f\n" % c.marginal)
					
				f.close()
				self.__handleOutput(  "Solution written to CSV file: " + solFile )
			except IOError, e:
				raise CmplException("IO error for file " + tmpName + ": "+e)					
			
		else:
			raise CmplException("No Solution found so far")
	#*********** end saveSolutionCsv *****		
	
	
	#********************************************************************************
	# private methods								*
	#********************************************************************************

		
	#*********** cleanUp ****************
	def __cleanUp(self):
	
		if not self.__isCleaned:
			if self.__debug:
				raw_input("Hit Enter to exit")
		
			if self.__remoteMode:
	
				if not (self.__serverMode == CMPL_GRID and self.__cmplGridSchedulerUrl==""):
					#if not (self.__remoteStatus==PROBLEM_FINISHED or self.__remoteStatus==CMPLSERVER_ERROR or self.__remoteStatus==PROBLEM_CANCELED):
					if (self.__remoteStatus!=PROBLEM_FINISHED and self.__remoteStatus!=CMPLSERVER_ERROR and self.__remoteStatus!=PROBLEM_CANCELED):
						if self.__cmplServerRunning:
							self.__cmplServerExecute("cancel")
							
					if ( self.__remoteStatus!=CMPLSERVER_CLEANED and self.__remoteStatus!=CMPLSERVER_ERROR):
						self.__cmplServerExecute("removeProblem")
						self.__remoteStatus=CMPLSERVER_CLEANED
						
				if self.__serverMode == CMPL_GRID:
					if self.__remoteStatus == CMPLGRID_SCHEDULER_BUSY or self.__remoteStatus == CMPLGRID_SCHEDULER_UNKNOWN:
						self.__cmplServerExecute("disconnectFromScheduler")
						
			elif self.__cmplProc!=None:
				if self.__cmplProc.poll() == None:
					self.__cmplProc.kill()
					
			if self.__cmplDataFile != None and not self.__remoteMode:
				if os.path.isfile(self.__cmplDataFile):
					os.remove(self.__cmplDataFile)
			if self.__cmplMsgFile != None: 
				if os.path.isfile(self.__cmplMsgFile):
					os.remove(self.__cmplMsgFile)
			if self.__cmplSolFile != None:
				if os.path.isfile(self.__cmplSolFile):
					os.remove(self.__cmplSolFile)
					
			self.__isCleaned = True 
					
	#*********** end cleanUp ************
	
		
	#*********** writeCmplDataFile *******	
	def __cmplDataElements(self):
		try:
			
			for s in self.__setList:
				self.__cmplDataStr +=  "%" + s.name 
							
				if s.rank>1:
					self.__cmplDataStr += ' set['+str(s.rank)+'] < ' 
				else:
					self.__cmplDataStr += ' set < ' 

				if s.type==0:
					self.__cmplDataStr +='\n' 
					count = 1
					for i in s.valueList:
						if type(i) == str:
							self.__cmplDataStr += "\""+ i +'\" '
						else:
							self.__cmplDataStr += str(i) +' '
						if count == s.rank:
							self.__cmplDataStr += '\n'
							count=1
						else:
							count+=1

				if s.type==1:
					self.__cmplDataStr +='\n' 
					for i in s.valueList:
						for j in i:
							if type(j) == list:
								raise CmplException("set " + s.name+ " contains unexpected data " + str(j))
							if type(j) == str:
								self.__cmplDataStr += "\""+j+"\" "
							else:
								self.__cmplDataStr += str(j)+' '
						self.__cmplDataStr +='\n'
	
				if s.type==2:
					self.__cmplDataStr += str(s.valueList[0]) + '..' + str(s.valueList[1])  + ' '
					
				if s.type==3:
					self.__cmplDataStr += str(s.valueList[0]) + '(' + str(s.valueList[1]) + ')' + str(s.valueList[2]) + ' '
			
			
				self.__cmplDataStr += '>\n'	
				
			for p in self.__parameterList:
				self.__cmplDataStr += '%'+p.name	
				
				if p.rank > 0:
					self.__cmplDataStr += "["
					
					pos = 0
					for s in p.setList:
						setFound = False
						for j in self.__setList:
							if j.name == s.name:
								setFound = True
								break
						if not setFound:
							raise CmplException("The set " + s.name + " used for the parameter " + p.name+ " doesn't exist.")
						else: 
							self.__cmplDataStr += s.name
							if pos < len (p.setList) -1:
								self.__cmplDataStr += ","
							pos += 1	
					
					self.__cmplDataStr += '] <\n' 
					
					for e in p.values:
						if type(e) == list:
							self.__cmplDataStr += self.__writeListElements(e)	
						else:
							if type(e) == str:
								self.__cmplDataStr += '\"' + e + "\" >\n"
							else:
								self.__cmplDataStr += str(e)+ '\n'
								
					self.__cmplDataStr += ">\n"
					
				else: 
					self.__cmplDataStr += " < " 
					if type(p.values[0]) == str:
						self.__cmplDataStr += '\"' + p.values[0] + "\" >\n"
					else:
						self.__cmplDataStr += str(p.values[0] ) + " >\n"
			
			if self.__cmplDataStr!="":
				if self.__remoteMode:
					pass
				else:
					f = open(self.__cmplDataFile, 'w')
					f.write(self.__cmplDataStr)		
					f.close()
			else:
				self.__cmplDataFile=None
				
		except IOError, e:
			raise CmplException("IO error for cmplDateFile " + self.__cmplDataFile + ": "+e)
	
	#*********** end writeCmplDataFile ***	
	
	
	#*********** writeListElements *******	
	def __writeListElements(self,  valList):
		tmpStr = ""
		for v in valList:
			if type(v) == list:
				tmpStr+=self.__writeListElements( v)
			else:
				if type(v) == str:
					tmpStr+= '\"'+v+'\" ' 
				else:	
					tmpStr+= str(v) + ' ' 
		tmpStr+='\n'
		return tmpStr
	#*********** end __writeListElements ****
	
		
		
	#*********** __getSolver ****************		
	def __getSolver(self):
		solverFound=False
		for o in self.__optionsList.values():
			if "-solver" in o:
				self.__solver=o.split()[2].replace("\"","")
				solverFound=True
				break
		if not solverFound:
			try:
				f = open(self.__model , "r") 
				for line in f:
					if line.strip().startswith("%arg"):
						if "-solver" in line:
							self.__solver=line.split()[2].replace("\"","")
			except IOError, e:
				raise CmplException( "Cannot read Cmpl file <"+self.__model+"> " + str(e) )
	#*********** end __getSolver ***********	
				
			
	#*********** __solByNr ***************		
	def __solByNr(self, solNr):
		if self.__solutions.nrOfSolutions>0:
			if solNr <= self.__solutions.nrOfSolutions:
				return self.__solutions.solutions[solNr]
			else:
				raise CmplException("Solution with number: " + str(solNr) + " doesn't exist.")
		else:
			raise CmplException("No Solution found so far")
	#*********** end __solByNr ***********	
	
	
	#*********** getElementByName ***********
	def __getElementByName(self, name, solObj):
		if self.__solutions.nrOfSolutions>0:
			solElements = []
			solElement = None
			isArray = False
			isFound = False
		
			for e in solObj:
				if e.name.startswith(name):
					if e.name.find("[") != -1:
						isArray = True
						solElements.append(e)
						isFound = True
					else:
						isArray = False
						solElement = e
						isFound = True
						
			if not isFound:
				raise CmplException(name + " does not exist.")			
			if isArray:
				return solElements
			else:
				return solElement
						
		else:
			raise CmplException("No Solution found so far")
	#*********** end getElementByName *****		
			

	#*********** elementByName ***********
	def __elementByName(self, solObj):
		elemFound = []

		for e in solObj:
			tmpName = ""
			tmpSet = ""
			pos = e.name.find("[") 
			
			if pos != -1:
				tmpName =  e.name[:pos]
				tmpSet = e.name[pos+1:-1].split(',')
	
				tmpSetStr = "("
				sNr=1
				for s in tmpSet:
					if sNr>1:
						tmpSetStr += ","
					if CmplTools.strIsNumber(s):
						tmpSetStr += s 
					else:
						tmpSetStr += "\""+ s +"\""
					sNr+=1
				tmpSetStr += ")"
							
				if tmpName in elemFound:
					exec "self."+ tmpName + ".update({"+tmpSetStr+": e})"
				else:
					elemFound.append(tmpName)
					exec "self."+ tmpName + "={"+tmpSetStr+": e}"
			else:
				tmpName = e.name
				exec "self."+tmpName + "=e"
				
	#*********** end elementByName *******
	
	
	#*********** __handleOutput ************	
	def __handleOutput(self, oStr):
		if oStr!="":
			if self.__printOutput:
				if self.__outputLeadString!="":
					print self.__outputLeadString + oStr.strip().replace("\n","\n"+self.__outputLeadString) 
				else:
					print oStr.strip().replace("\n","\n"+self.__outputLeadString)
			self.__outputString += oStr
	#*********** end __handleOutput ********	
	
	
	#*********** __connectServerViaScheduler  ****************
	def __connectServerViaScheduler(self, cmplUrl):
		self.__cmplGridSchedulerUrl = self.__cmplUrl
		self.__cmplUrl=cmplUrl
		self.__cmplGridScheduler=self.__cmplServer
		self.__serverMode = CMPL_GRID
				
		try:
			self.__cmplServer = xmlrpclib.ServerProxy(self.__cmplUrl, allow_none=False)
			self.__handleOutput("Connected with CmplServer at " + self.__cmplUrl + " with jobId " + self.__jobId + \
					" >> maxServerTries <" + str(self.__maxCmplServerTries)+"> maxQueuingTime <"+str(self.__maxCmplServerQueuingTime)+">")
		
		except:
			self.__remoteStatus = CMPLSERVER_ERROR
			raise CmplException(str(sys.exc_info()[1]) )

	#*********** end __connectServerViaScheduler  ****************
		
	
	#*********** __knockScheduler *******************	
	def __knockScheduler(self):
		if self.__remoteMode:
			if not self.__cmplServerRunning:
				raise CmplException("Model is not connected to a CmplScheduler" )
				
			if self.__remoteStatus!=PROBLEM_CANCELED:
				
				ret = self.__cmplServerExecute("knock")
				 				
				self.__remoteStatus=ret[0]
			
				if self.__remoteStatus == CMPLGRID_SCHEDULER_ERROR:
					self.__cleanUp()
					raise CmplException(ret[1] )
				elif self.__remoteStatus == CMPLGRID_SCHEDULER_OK:
					self.__connectServerViaScheduler(ret[1])
		else:
			raise CmplException("Cmpl::knock can only be used in remote mode" )
	#*********** end __knockScheduler ***************	
		
	
	#*********** cmplServerExecute *******
	def __cmplServerExecute(self, method=""):
		ret=[]
		tries=0
		while True:
			try:
				if method=="cancel":
					ret = self.__cmplServer.cancel(self.__jobId)
				elif method=="removeProblem":
					ret = self.__cmplServer.removeProblem(self.__jobId)
				elif method=="send":
					ret = self.__cmplServer.send(self.__instStr)
				elif method=="knock":
					ret = self.__cmplServer.knock(self.__jobId)
				elif method=="getCmplMessages":
					ret = self.__cmplServer.getCmplMessages(self.__jobId)
				elif method=="getSolutions":
					ret = self.__cmplServer.getSolutions(self.__jobId)	
				elif method=="getCmplInfo":
					ret = self.__cmplServer.getCmplInfo(self.__jobId)	
				elif method=="cmplServerFailed":
					ret = self.__cmplGridScheduler.cmplServerFailed(self.__cmplUrl)	
				elif method=="disconnectFromScheduler":
					ret = self.__cmplServer.disconnectProblem(self.__jobId)	
				
			except :
				tries+=1
				if tries==self.__maxCmplServerTries:
					self.__remoteStatus=CMPLSERVER_ERROR
					self.__cleanUp()
					raise CmplException("CmplServer error : "+  str(sys.exc_info()[1]) )
				else:	
					continue		
			break
			
		return ret
	#******** end cmplServerExecute *******

	
	#*********** writeSolFiles **********
	def __writeSolFiles(self):
		self.__handleOutput("\n")
		if self.cmplSolFile != "" :
			if self.cmplSolFile == "cmplStandard":
				fName=self.__problem+".csol" 
			else:
				fName=self.cmplSolFile
			self.saveSolution(fName)
			
		if self.asciiSolFile != "" :
			if self.asciiSolFile == "cmplStandard":
				fName=self.__problem+".sol" 
			else:
				fName=self.asciiSolFile
			self.saveSolutionAscii(fName)
			
		if self.csvSolFile != "" :
			if self.csvSolFile == "cmplStandard":
				fName=self.__problem+".csv" 
			else:
				fName=self.csvSolFile
			self.saveSolutionCsv(fName)
	#*********** end writeSolFiles ******	

	
	#*********** writeInfoFiles **********
	def __writeInfoFiles(self):
		self.__handleOutput("\n")
			
		if self.__cmplInfos.statisticsFileName != "" :
			if self.__cmplInfos.statisticsFileName == "stdOut":
				self.__handleOutput(  self.__cmplInfos.statisticsText)
				self.__handleOutput("\n")
			else:
				self.__writeAsciiFile(self.__cmplInfos.statisticsFileName, self.__cmplInfos.statisticsText)
				self.__handleOutput(  "Statistics written to file: " + self.__cmplInfos.statisticsFileName )
		
		if self.__cmplInfos.varProdFileName != "" :
			if self.__cmplInfos.varProdFileName == "stdOut":
				self.__handleOutput(  self.__cmplInfos.varProdText)
				self.__handleOutput("\n")
			else:
				self.__writeAsciiFile(self.__cmplInfos.varProdFileName, self.__cmplInfos.varProdtext)
				self.__handleOutput(  "Variable products statistics written to file: " + self.__cmplInfos.varProdFileName )
				
		if self.__cmplInfos.matrixFileName != "" :
			if self.__cmplInfos.matrixFileName == "stdOut":
				self.__handleOutput(  self.__cmplInfos.matrixText)
				self.__handleOutput("\n")
			else:
				self.__writeAsciiFile(self.__cmplInfos.matrixFileName, self.__cmplInfos.matrixText)
				self.__handleOutput(  "Generated matrix written to file: " + self.__cmplInfos.matrixFileName )
				
	#*********** end writeInfoFiles ******		
	
	
	#*********** __writeAsciiFile  *******
	def __writeAsciiFile(self, fname, str):
		try:
			f = open(fname, 'w')
			f.write(str)
			f.close()
		except IOError, e:
			raise CmplException("IO error for file " + tmpName + ": "+e)	
	#******** end __writeAsciiFile  *******	
	
	
	

#################################################################################
# End Cmpl																		
#################################################################################		




		
		
		
		

		