#***********************************************************************
 #  This code is part of pyCMPL 
 #
 #  Copyright (C) 2013
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

from SocketServer import ThreadingMixIn
from SimpleXMLRPCServer import SimpleXMLRPCServer 
from time import gmtime, strftime
import thread 


from CmplDefs import *
from CmplServerException import *
from CmplInstance import *
from CmplMsg import *
from CmplSolution import *

import threading
import random
import os
import sys
import subprocess
import time

#*************** CmplXmlRpcServer ********************************
class CmplXmlRpcServer(ThreadingMixIn, SimpleXMLRPCServer):

	#*********** process_request **********
    def process_request(self, request, client_address):
        self.client_address = client_address
        return SimpleXMLRPCServer.process_request(self, request, client_address)
	#*********** end process_request ******
           
#*************** end CmplXmlRpcServer ****************************        


#*************** CmplProblemHandler ******************************
class CmplProblemHandler(object):
	
	#*********** constructor **********
	def __init__(self, cmplName  ):
		self.__cmplName = cmplName
		self.__processHandler = None
		self.__status = CMPL_UNKNOWN 
		self.__statusMessage = ""
		self.__output = []
		self.lineNr = 0
	#*********** end constructor *****
	
	# getter and setter ***************	

	@property
	def processHandler(self):
		return self.__processHandler
		
	def setProcessHandler(self, pH):
		if type(pH) != subprocess.Popen:
			 raise CmplServerException("Internal CmplServer error  <CmplServer subprocessHandler> ")
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
		
	# end getter and setter *************
#*************** end CmplProblemHandler **************************		
		


#*************** CmplServer **************************************
class CmplServer(object):
	"CmplServer"
	 
	#*********** constructor **********
	def __init__(self, port = 8008 , path = None  ):
	
		self.__server = None
		self.__cmplPort  = port
		self.__problemList = {} 
		
		if path == None:
			self.__cmplServerPath = os.path.expanduser("~") + os.sep+ "CmplServer" +os.sep
			self.__logFileName = self.__cmplServerPath + "cmplServer.log"
		else:
			self.__cmplServerPath = path
		
		if os.path.exists(self.__cmplServerPath) == False:
			try:
				os.mkdir(self.__cmplServerPath)
			except OSError, e:
						raise CmplServerException("Cannot create CmplServer path <"+self.__cmplServerPath+">")
			
		self.__logFile = open(self.__logFileName, "a")
	#*********** end constructor *******	
	
	#*********** destructor ************
	def __del__(self ):
		for p in self.__problemList:
			if p.processHandler!=None:
				p.__processHandler.kill()
	
		self.__logFile.close()
	#*********** end destructor ********	
	
	#*********** checkId ***************
	def checkId(self, id) :
		return id in self.__problemList
	#*********** end checkId ***********
	
	#*********** cmplLogging ***********
	def cmplLogging(self, msg ,  id=None , name=None,   ):
		try:
			if id==None and name==None:
				self.__logFile.write("[" + strftime("%Y-%m-%d %H:%M:%S", gmtime())+ "] - " + msg+"\n")
			else:
				self.__logFile.write(self.__server.client_address[0] + " - [" + strftime("%Y-%m-%d %H:%M:%S", gmtime())+ "] - " + id + " - " + name + " <"+msg+">\n")
			self.__logFile.flush()
		except IOError, e:
			raise CmplServerException("IO error for solution or log file ")
	#*********** end cmplLogging *******
	
	#*********** getJobId **************		
	def getJobId(self, cmplName):
		id = self.__server.client_address[0] + "-" + strftime("%Y-%m-%d-%H-%M-%S", gmtime()) + "-" + str(random.randint(100000, 999999))
	
		self.__problemList.update( {id: CmplProblemHandler(cmplName) } ) 
		self.__problemList[id].setStatus(CMPLSERVER_OK)
	
		try:
			os.mkdir(self.__cmplServerPath+os.sep+id)
		except OSError, e:
			self.__problemList[id].setStatus(CMPLSERVER_ERROR)
			self.__problemList[id].setStatusMessage("Cannot create path <CmplServerHome/"+id +"> ")
			
		if self.__problemList[id].status != CMPLSERVER_ERROR:	
			self.cmplLogging( "Problem has been registered" , id, cmplName )
		else:
			self.cmplLogging("Registration failed: " + self.__problemList[id].statusMessage , id, cmplName  )
				
		return [ self.__problemList[id].status, self.__problemList[id].statusMessage, id]
	#*********** end getJobId ************		
		
	#*********** removeProblem ***********		
	def removeProblem(self, id):	
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
		
		self.cmplLogging( logTxt , id, self.__problemList[id].name  )
		del self.__problemList[id]
		 
		return [status, logTxt]
	#*********** end removeProblem ********		
		
	#*********** readFileContent **********	
	def readFileContent(self, fileName):
		try:
			f = open(fileName, "r")
			lines = f.read()
			f.close()
			return lines
		except IOError, e:
			raise CmplServerException("IO error for solution or message file ")
	#*********** end readFileContent ******	
	
	#*********** getSolution **************
	def getSolutions(self, id):
		if self.checkId(id):
			try:
				problem =  os.path.splitext(self.__cmplServerPath+id+os.sep+self.__problemList[id].name)[0]
				cmplSolFile = problem+".csol"
				
				solStr=""
				if not (self.__problemList[id].status==CMPLSERVER_ERROR or self.__problemList[id].status==CMPL_FAILED) :
					solStr = self.readFileContent(cmplSolFile)
					
			except CmplServerException, e:
				self.__problemList[id].setStatus(CMPLSERVER_ERROR)
				self.__problemList[id].setStatusMessage(e.msg)
				
			self.cmplLogging( "Sending solution(s)" , id, self.__problemList[id].name)
		else:
			self.__problemList[id].setStatus(CMPLSERVER_ERROR)
			self.__problemList[id].setStatusMessage("Sending solution(s) failed -> unknown id" + id)
				
		return [ self.__problemList[id].status, self.__problemList[id].statusMessage, solStr]
	#*********** end getSolution *********
		
	#*********** getCmplMessages *********
	def getCmplMessages(self, id):
		if self.checkId(id):
			try:
				problem =  os.path.splitext(self.__cmplServerPath+id+os.sep+self.__problemList[id].name)[0]
				cmplMsgFile = problem+".cmsg"
					
				msgStr=""
				if not (self.__problemList[id].status==CMPLSERVER_ERROR or self.__problemList[id].status==CMPL_FAILED) :
					msgStr = self.readFileContent(cmplMsgFile)
					
			except CmplServerException, e:
				self.__problemList[id].setStatus(CMPLSERVER_ERROR)
				self.__problemList[id].setStatusMessage(e.msg)
				
			self.cmplLogging( "Sending cmplMessages", id, self.__problemList[id].name )	
		else:
			self.__problemList[id].setStatus(CMPLSERVER_ERROR)
			self.__problemList[id].setStatusMessage("Sending messages failed -> unknown id" + id)
			
		return [ self.__problemList[id].status, self.__problemList[id].statusMessage, msgStr]
	#*********** end getCmplMessages *****
		
	#*********** storeOutput *****************
	def storeOutput(self, id):
		#self.__problemList[id].setOutputProcessHandler(True)
		for line in iter( self.__problemList[id].processHandler.stdout.readline, ''):
			self.__problemList[id].setOutput(line.rstrip()) 
	#*********** end storeOutput *************
	
	#*********** runSolver ***************	
	def runSolver(self, instanceStr):	
		inst = CmplInstance()
		inst.writeCmplInstance(self.__cmplServerPath, instanceStr)
			
		problem = self.__cmplServerPath+inst.jobId+os.sep+self.__problemList[inst.jobId].name
		
		try:
			cmplBin=os.environ['CMPLBINARY']
			if not os.path.exists(cmplBin):
				raise CmplException("Cant't find Cmpl binary: " + cmplBin )
		except:
				raise CmplException("Cant't find Cmpl binary" )
					
		cmdList = [cmplBin, problem, "-e", "-solution", "-includesForbidden"]
		if len(inst.options) != 0:
			for opt in inst.options:
				cmdList.append("-headerOpt")
				cmdList.append(opt)
				
		tmpP= subprocess.Popen(cmdList, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
		self.__problemList[inst.jobId].setProcessHandler(tmpP )
		self.__problemList[inst.jobId].setStatus(PROBLEM_RUNNING)
			
		thread.start_new_thread(self.storeOutput, (inst.jobId,)) 
				
		return inst.jobId
	#*********** end runSolver ************		
		

	#*********** send ********************	
	def send(self, instanceStr):
		try:
			id = self.runSolver(instanceStr)
			self.cmplLogging( "Solving problem has been started" , id, self.__problemList[id].name)
			
		except CmplServerException, e:
			self.__problemList[id].setStatus(CMPLSERVER_ERROR)
			self.__problemList[id].setStatusMessage(e.msg)
			
		return [ self.__problemList[id].status, self.__problemList[id].statusMessage]
	#*********** end send ****************	
	
	#*********** knock *******************	
	def knock(self, id):
		if self.checkId(id):
			tmpOut=""
			if self.__problemList[id].processHandler!=None:
				ret = self.__problemList[id].processHandler.poll()
				i = 0
				for line in self.__problemList[id].output[self.__problemList[id].lineNr : ] :
					tmpOut += line  + "\n"
					i += 1
				self.__problemList[id].lineNr += i
				
				if ret == None:
					self.__problemList[id].setStatus(PROBLEM_RUNNING)
					self.__problemList[id].setStatusMessage("Problem is still running")
				else:
					if ret!= 0:
						self.__problemList[id].setStatus(CMPLSERVER_ERROR)
						ant = self.__problemList[id].processHandler.stderr.read()
						self.__problemList[id].setStatusMessage(ant)
						self.cmplLogging( "Solving problem failed", id, self.__problemList[id].name)
					else:	
						self.__problemList[id].setStatus(PROBLEM_FINISHED)
						self.__problemList[id].setStatusMessage("Problem has finished")
						self.cmplLogging( "Solving problem has been finished" , id, self.__problemList[id].name)
			else:
				self.__problemList[id].setStatus(PROBLEM_NOTRUNNING) 
				self.__problemList[id].setStatusMessage("Problem is not running")
		else:
			self.__problemList[id].setStatus(CMPLSERVER_ERROR)
			self.__problemList[id].setStatusMessage("Knocking failed -> unknown id" + id)
					
		return [ self.__problemList[id].status, self.__problemList[id].statusMessage, tmpOut]
	#*********** end knock ***************	
	
	#*********** cancel ******************	
	def cancel(self, id):
		if self.checkId(id):
			if self.__problemList[id].processHandler!=None:
				ret = self.__problemList[id].processHandler.poll()
				if ret == None:
					self.__problemList[id].processHandler.kill()
					self.cmplLogging( "Problem has been canceled by user" , id, self.__problemList[id].name)
					del self.__problemList[id]
					return [PROBLEM_CANCELED, "Problem has been canceled by user"]
		else:
			self.__problemList[id].setStatus(CMPLSERVER_ERROR)
			self.__problemList[id].setStatusMessage("Canceling failed -> Unknown id" + id)	
		
		return [ self.__problemList[id].status, self.__problemList[id].statusMessage]
	#*********** end cancel **************	

	#*********** stopServer **************
	def	stopServer(self):
		if  self.__server.client_address[0] == "127.0.0.1":
			self.shutdown = True
			self.cmplLogging( "CmplServer has been stoped" )	
			return True
		else:
			return False
	#*********** end stopServer **********	
	
	#*********** isRunning ***************
	def	isRunning(self):
		return True
	#*********** end isRunning ***********	
	
	#*********** startServer *************
	def startServer(self): 
		try:
			self.__server = CmplXmlRpcServer(("", self.__cmplPort), logRequests=False) 
			self.__server.register_function(self.getJobId) 
			self.__server.register_function(self.send)
			self.__server.register_function(self.knock)
			self.__server.register_function(self.cancel)
			self.__server.register_function(self.getSolutions)
			self.__server.register_function(self.getCmplMessages)
			self.__server.register_function(self.removeProblem)
			self.__server.register_function(self.stopServer)
			self.__server.register_function(self.isRunning)
			self.cmplLogging( "CmplServer has been started" )	
			self.shutdown = False
			while not self.shutdown:
				self.__server.handle_request()
		except:
			raise CmplServerException(str(sys.exc_info()[1]))
			
	#*********** end startServer **********

	
#*************** end CmplServer ***********************************		  
	


	
	



		
		
		
		

		