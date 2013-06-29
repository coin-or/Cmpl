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

from __future__ import division

"pyCmpl - CMPL's python API"
__author__ = "Mike Steglich"
__copyright__ = "Copyright (C) 2013 Mike Steglich"
__license__ = "LGPLv3"
__version__ = "1.0"


from math import *
from multiprocessing import Process

import os
import threading
import random
import subprocess
import sys
import xmlrpclib
import time

from CmplDefs import *
from CmplException import *
from CmplMsg import *
from CmplTools import *
from CmplSolution import *
from CmplSets import *
from CmplParameters import *
from CmplSets import *
from CmplParameters import *
from CmplInstance import *

	

#*************** Cmpl *******************************************
class Cmpl(threading.Thread):

	#*********** constructor **********
	def __init__(self, model):
		
		threading.Thread.__init__(self)
		
		self.__problem = ""
		self.__cmplDataFile = ""
		self.__cmplSolFile = ""
		self.__cmplDataStr = ""
			
		self.__setList = []
		self.__parameterList = []
		self.__optionsList = {}
				
		self.__status = None 
		self.__solutions = None 
		self.__solutionString = ""
		self.__saveSolFile = False
		
		self.__cmplFileList = []
		self.__dataFileList = []
		
		self.__remoteMode = False
		self.__remoteStatus = CMPL_UNKNOWN
		self.__cmplUrl = ""
		self.__jobId = ""
		self.__cmplServer = None
		self.__cmplServerRunning = False
		
		self.__cmplDataFile = None
		self.__cmplMsgFile = None
		self.__cmplSolFile = None
		self.__cmplProc = None
		
		self.__refreshTime = 0.5
		self.__printOutput = False
		self.__outputString = ""
		
		self.__debug = False
		
		if type(model) != str:
			raise CmplException( str(model) + " is not a valid file name for a Cmpl file" )
		else:
			self.__model = model
		
		if not os.path.isfile(model):
			raise CmplException("CMPL file " + model + " does not exist."  )
		else:
			self.__id = str(random.randint(100000, 999999))
		
		self.__outputLeadString = os.path.basename(os.path.splitext(self.__model)[0]) + ">" 
	
	#*********** end constructor ********
		
	#*********** cleanUp ****************
	def __cleanUp(self):
	
		if self.__debug:
			raw_input("zum beenden taste druecken")
	
		if self.__remoteMode:
			if not (self.__remoteStatus==PROBLEM_FINISHED or self.__remoteStatus==CMPLSERVER_ERROR or self.__remoteStatus==PROBLEM_CANCELED):
				if self.__cmplServerRunning:
					self.__cmplServer.cancel(self.__jobId)
			if self.__remoteStatus!=CMPLSERVER_CLEANED:
				self.__cmplServer.removeProblem(self.__jobId )
				self.__remoteStatus=CMPLSERVER_CLEANED
				
		elif self.__cmplProc!=None:
			if self.__cmplProc.poll() == None:
				self.__cmplProc.kill()
				
		if not (self.__cmplDataFile == None and self.__cmplMsgFile == None and self.__cmplSolFile == None):
			if os.path.isfile(self.__cmplDataFile):
				os.remove(self.__cmplDataFile)
			if os.path.isfile(self.__cmplMsgFile):
				os.remove(self.__cmplMsgFile)
			if not self.__saveSolFile:	
				if os.path.isfile(self.__cmplSolFile):
					os.remove(self.__cmplSolFile)
					
	#*********** end cleanUp ************
		
	#*********** model (getter) *********	
	@property
	def model(self):
		return self.__model
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


	#*********** cmplStatusMessage *******	
	@property
	def statusMessage(self):
		return self.__status.cmplMessage 
		#old - outdated
	#*********** statusMessage ***********	
	
	#*********** cmplStatus **************	
	@property
	def cmplStatus(self):
		if self.__remoteMode:
			if self.__remoteStatus==CMPL_UNKNOWN:
				return self.__status.cmplStatus
			else:
				return self.__remoteStatus
		else:
			if self.__status.cmplStatus==CMPL_UNKNOWN:
				return CMPL_UNKNOWN
			else:
				return self.__status.cmplStatus
	#*********** end cmplStatus ***********	
	
	#*********** cmplStatusText ***********	
	@property
	def cmplStatusText(self):
		if self.__remoteMode and self.__remoteStatus!=CMPL_UNKNOWN:
			if self.__remoteStatus==CMPLSERVER_OK:
				ret = "CMPLSERVER_OK"
			elif self.__remoteStatus==CMPLSERVER_ERROR:
				ret = "CMPLSERVER_ERROR"
			elif self.__remoteStatus==PROBLEM_RUNNING:
				ret = "PROBLEM_RUNNING"
			elif self.__remoteStatus==PROBLEM_FINISHED:
				ret = "PROBLEM_FINISHED"
			elif self.__remoteStatus==PROBLEM_CANCELED:
				ret = "PROBLEM_CANCELED"
			elif self.__remoteStatus==PROBLEM_NOTRUNNING:
				ret = "PROBLEM_NOTRUNNING"
		else:
			if self.__status.cmplStatus==CMPL_UNKNOWN:
				ret = "CMPL_UNKNOWN"
			elif self.__status.cmplStatus==CMPL_OK:
				ret = "CMPL_OK"
			elif self.__status.cmplStatus==CMPL_WARNINGS:
				ret = "CMPL_WARNINGS"
			elif self.__status.cmplStatus==CMPL_FAILED:
				ret = "CMPL_FAILED"
		return ret
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
	
	#*********** solver ******************	
	@property
	def solver(self):
		return self.__solutions.solver
	#*********** end solver **************
		
	#*********** solver ******************	
	@property
	def solverMessage(self):
		return self.__solutions.solverMessage
	#*********** end solver **************
	
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
	
	
	#*********** debug *******************	
	def debug(self, mode=True):
		self.__debug = mode
	#*********** end debug ***************
	
	
	#*********** writeCmplDataFile *******	
	def __cmplDataElements(self):
		try:
			
			self.__cmplDataStr = '#This cmplData file was generated by pyCmpl\n'
			
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
			
			if self.__remoteMode:
				pass
			else:
				f = open(self.__cmplDataFile, 'w')
				f.write(self.__cmplDataStr)		
				f.close()
		except IOError, e:
			raise CmplException("IO error for cmplDateFile " + self.__cmplDataFile + ": "+e)
	
	#*********** end writeCmplDataFile ***	
	
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
		
	#*********** varByName ***************	
	def varByName(self, solNr=0):
		if solNr<0 or solNr>self.__solutions.nrOfSolutions-1:
			raise CmplException("Solution with index" + str(solNr) + " doesn't exist.")
		s = self.__solByNr(solNr)
		self.__elementByName(s.variables)
	#*********** end varByName ***********
	
	#*********** conByName ***************	
	def conByName(self, solNr=0):
		if solNr<0 or solNr>self.__solutions.nrOfSolutions-1:
			raise CmplException("Solution with index" + str(solNr) + " doesn't exist.")
		s = self.__solByNr(solNr)
		self.__elementByName(s.constraints)
	#*********** end conByName ***********
	
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
	
	#*********** solve *******************	
	def solve(self):
	
		if 	self.__remoteMode == True:
			self.__problem = os.path.splitext(self.__model)[0]
		else:
			self.__problem = os.path.splitext(self.__model)[0] + "_" + self.__id 
		
		self.__cmplDataFile = self.__problem+".cdat"
		self.__cmplMsgFile = self.__problem+".cmsg"
		self.__cmplSolFile = self.__problem+".csol"
	
		self.__cmplDataElements()
			
		if self.__remoteMode:
		
			if not self.__cmplServerRunning:
				raise CmplException("Model is not connected to a CmplServer" )
				
			self.__status = CmplMessages()
			self.__solutions = CmplSolutions()
			
			if self.__remoteStatus == CMPLSERVER_CLEANED:
				self.connect(self.__cmplUrl)
		
			self.__cmplInstance=CmplInstance()
			instStr = self.__cmplInstance.cmplInstanceStr(self.__model, self.__optionsList.values(),self.__cmplDataStr, self.__jobId )
			
			if self.__debug:
				instFile = self.__problem+".cinst"
				try:
					f = open(instFile, 'w')
					f.write(instStr)		
					f.close()
				except IOError, e:
					raise CmplException("IO error for file " + instFile + ": "+e)
			
			ret = self.__cmplServer.send(instStr)
			self.__remoteStatus = ret[0]
			
			if ret[0] == CMPLSERVER_ERROR:
				raise CmplException(ret[1])
			else:
				
				while self.__remoteStatus!=PROBLEM_FINISHED:
					ret =  self.__cmplServer.knock(self.__jobId)
					self.__remoteStatus=ret[0]
					if ret[0]== CMPLSERVER_ERROR:
						raise CmplException(ret[1])
					self.handleOutput(ret[2])
					time.sleep(self.__refreshTime)

			
				ret = self.__cmplServer.getCmplMessages(self.__jobId )
				if ret[0] == CMPLSERVER_ERROR:
					raise CmplException(ret[1] )
				else:
					self.__status.readCmplMessages(ret[2])
						
				if self.__status.cmplStatus==CMPL_FAILED:
					raise CmplException("Cmpl finished with errors", self.__status.cmplMessageList )
					
				ret = self.__cmplServer.getSolutions(self.__jobId )
				
				if ret[0] == CMPLSERVER_ERROR:
					raise CmplException(ret[1] )
				else:
					self.__solutionString=ret[2]
					self.__solutions.readSolution(self.__solutionString)
				
				#self.disconnect()
				self.__cleanUp()
			
		else:
			self.__status = CmplMessages(self.__cmplMsgFile)
			self.__solutions = CmplSolutions(self.__cmplSolFile)
			
			tmpAlias = os.path.basename(self.__problem) 
			
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
					cmdList.append(self.__optionsList[opt])
					
			self.__cmplProc = subprocess.Popen(cmdList, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
			
			for line in iter(self.__cmplProc.stdout.readline, ''):
					self.handleOutput(line)
					
			if self.__cmplProc.wait() != 0:
				raise CmplException(self.__cmplProc.stderr.read() )
						
			self.__status.readCmplMessages()
						
			if self.__status.cmplStatus==CMPL_FAILED:
				raise CmplException("Cmpl finished with errors", self.__status.cmplMessageList )
				
			self.__solutions.readSolution()
			self.__cleanUp()
			
	#*********** end solve ***************	
	
	#*********** handleOutput ************	
	def handleOutput(self, oStr):
		if oStr!="":
			if self.__printOutput:
				print self.__outputLeadString + " " + oStr.strip().replace("\n","\n"+self.__outputLeadString+" ")
			self.__outputString += oStr
	#*********** end handleOutput ********	
		
	#*********** send ********************	
	def send(self):
		if self.__remoteMode:

			if self.__remoteStatus == CMPLSERVER_CLEANED:
				self.connect(self.connect)
				
			if not self.__cmplServerRunning:
				raise CmplException("Model is not connected to a CmplServer" )

			self.knock()
			if self.__remoteStatus==PROBLEM_RUNNING:
				raise CmplException("Don't send the problem again before the CmplServer finished the previous one" )
		
			self.__problem = os.path.splitext(self.__model)[0]
			
			self.__cmplDataElements()
			
			self.__status = CmplMessages()
			self.__solutions = CmplSolutions()
			
			self.__cmplInstance=CmplInstance()
			instStr = self.__cmplInstance.cmplInstanceStr(self.__model, self.__optionsList.values(),self.__cmplDataStr, self.__jobId )
			
			ret = self.__cmplServer.send(instStr) 
			
			if ret[0] == CMPLSERVER_ERROR:
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
		
			if self.__remoteMode!=PROBLEM_CANCELED:
				ret = self.__cmplServer.knock(self.__jobId)
				self.__remoteStatus=ret[0]
				if self.__remoteStatus == CMPLSERVER_ERROR:
					raise CmplException(ret[1] )
				self.handleOutput(ret[2])
			
		else:
			raise CmplException("Cmpl::knock can only be used in remote mode" )
	#*********** end knock ***************	
		
	#*********** cancel ******************	
	def cancel(self):
		if self.__remoteMode:
			if not self.__cmplServerRunning:
				raise CmplException("Model is not connected to a CmplServer" )

			if self.__remoteStatus == CMPLSERVER_CLEANED:
				raise CmplException("Model was received and cleaned on the CmplServer" )
			
			if self.__remoteStatus!=PROBLEM_CANCELED:
				self.__remoteStatus=self.__cmplServer.cancel(self.__jobId)[0]
			
		else:
			raise CmplException("Cmpl::cancel can only be used in remote mode" )
	#*********** end cancel **************	
			
	#*********** retrieve ****************	
	def retrieve(self):
		if self.__remoteMode:
		
			if not self.__cmplServerRunning:
				raise CmplException("Model is not connected to a CmplServer" )

			if self.__remoteStatus == CMPLSERVER_CLEANED:
				raise CmplException("Model was received and cleaned on the CmplServer" )
		
			if self.__remoteStatus == CMPL_UNKNOWN: 
				self.knock()
			
			if self.__remoteStatus==PROBLEM_FINISHED:
				ret = self.__cmplServer.getCmplMessages(self.__jobId )
				if ret[0] == CMPLSERVER_ERROR:
					raise CmplException(ret[1] )
				else:
					self.__status.readCmplMessages(ret[2])
						
				if self.__status.cmplStatus==CMPL_FAILED:
					raise CmplException("Cmpl finished with errors", self.__status.cmplMessageList )
					
				ret = self.__cmplServer.getSolutions(self.__jobId )
				
				if ret[0] == CMPLSERVER_ERROR:
					raise CmplException(ret[1] )
				else:
					self.__solutionString=ret[2]
					self.__solutions.readSolution(self.__solutionString)
				
				self.__cleanUp()
					
			else:
				if self.__remoteStatus==PROBLEM_CANCELED:
					raise CmplException("Model has been canceled by user, cannot retrieve the solutions")
				else: 
					raise CmplException("Model is still running, cannot retrieve the solutions")
				
		else:
			raise CmplException("Cmpl::retrieve can only be used in remote mode" )
	#*********** end retrieve ************		
	
	#*********** connect  ****************	
	def connect(self, cmplUrl):
		self.__cmplUrl = cmplUrl
		self.__remoteMode = True
		try:
			self.__cmplServer = xmlrpclib.ServerProxy(cmplUrl) 
			ret = self.__cmplServer.getJobId(os.path.basename(self.__model))
			if ret[0] == CMPLSERVER_ERROR:
				raise CmplException(ret[1])
			else:
				self.__jobId = ret[2]
			self.__cmplServerRunning =True
			print self.__outputLeadString + " is connected with CmplServer at " + cmplUrl + " with jobId " + self.__jobId
		except:
			raise CmplException("CmplServer error: " + str(sys.exc_info()[1]))
	#*********** end connect *************
	
	#*********** connect  ****************	
	def disconnect(self):
		self.__cmplUrl = ""
		self.__remoteStatus = CMPL_UNKNOWN

		if self.__remoteStatus != CMPLSERVER_CLEANED:
			if self.__cmplServerRunning:
				self.__cmplServer.cancel(self.__jobId)
				self.__cmplServerRunning = False
			self.__cmplServer.removeProblem(self.__jobId )

		self.__remoteMode=False
		
	#*********** end connect *************
	
	
	#*********** saveSolution ************	
	def saveSolution(self, solFileName=None):
		if self.__solutions.nrOfSolutions > 0:
			if self.__remoteMode:
				self.__problem = os.path.splitext(self.__model)[0]
				if solFileName==None:
					self.__cmplSolFile = self.__problem+".csol"
				else:
					self.__cmplSolFile=solFileName
				try:
					f = open(self.__cmplSolFile, 'w')
					f.write(self.__solutionString)		
					f.close()
				except IOError, e:
					raise CmplException("IO error for file " + tmpName + ": "+e)
			self.__saveSolFile=True
		else:
			raise CmplException("No Solution found so far")
	#*********** end saveSolution ********	
	
	#*********** saveSolutionAscii *******	
	def saveSolutionAscii(self, solFileName=None):
		if 	self.__remoteMode == True:
			self.__problem = os.path.splitext(self.__model)[0]
		else:
			self.__problem = os.path.splitext(self.__model)[0] + "_" + self.__id 
		
		if solFileName==None:
			solFile = self.__problem+".sol"
		else:
			solFile=solFileName
		self.solutionReport(solFile)
	#*********** saveSolutionAscii *******	
	
	#*********** solutionReport **********	
	def solutionReport(self, fileName=None):
	
		repStr=""
		if self.__solutions.nrOfSolutions > 0:
			repStr += "---------------------------------------------------------------------------------------------------------\n"
			repStr += "%-21s %-s\n" %  ("Problem", os.path.basename(self.__model))
			repStr += "%-21s %-g\n" %  ("Nr. of variables" , self.__solutions.nrOfVariables)
			repStr += "%-21s %-g\n" %  ("Nr. of constraints" , self.__solutions.nrOfConstraints)
			repStr += "%-21s %-s\n" %  ("Objective name" , self.__solutions.objectiveName)
			if self.__solutions.nrOfSolutions > 1:
				repStr += "%-21s %-g\n" %  ("Nr. of solutions" , self.__solutions.nrOfSolutions)
			repStr += "%-21s %-s\n" %  ("Solver name" , self.__solutions.solver)
			repStr += "%-21s %-s\n" %  ("Display variables" , self.__solutions.varDisplayOptions)
			repStr += "%-21s %-s\n" %  ("Display constraints" , self.__solutions.conDisplayOptions)
			repStr += "---------------------------------------------------------------------------------------------------------\n"
			for s in self.__solutions.solutions:
				repStr += "\n"
				repStr += "%-21s %-s\n" %  ("Objective status", s.status)
				repStr += "%-21s %-20.2f(%s!)\n" %  ("Objective value", s.value, self.__solutions.objectiveSense)
				repStr += "\n"
				if len(s.variables)>0:
					repStr += "%-21s\n" %  "Variables"
					repStr += "%-20s%5s%20s%20s%20s%20s\n" % ( "Name" , "Type" , "Activity", "LowerBound", "UpperBound" , "Marginal")
					repStr += "---------------------------------------------------------------------------------------------------------\n"
					for v in s.variables:
						if v.type=="C":
							repStr += "%-20s%5s%20.2f%20.2f%20.2f%20.2f\n" % ( v.name , v.type ,v.activity, v.lowerBound,v.upperBound ,v.marginal)
						else:
							repStr += "%-20s%5s%20g%20.2f%20.2f%20.2f\n" % ( v.name , v.type ,v.activity, v.lowerBound,v.upperBound ,v.marginal)
					repStr += "---------------------------------------------------------------------------------------------------------\n"
				if len(s.constraints)>0:
					repStr += "\n"
					repStr += "%-21s\n" %  "Constraints"
					repStr += "%-20s%5s%20s%20s%20s%20s\n" % ( "Name" , "Type" , "Activity", "LowerBound", "UpperBound" , "Marginal")
					repStr += "---------------------------------------------------------------------------------------------------------\n"
					for c in s.constraints:
						repStr += "%-20s%5s%20.2f%20.2f%20.2f%20.2f\n" % ( c.name , c.type ,c.activity, c.lowerBound,c.upperBound ,c.marginal)
					repStr += "---------------------------------------------------------------------------------------------------------\n"
					
			if fileName!=None:
				try:
					f = open(fileName, 'w')
					f.write(repStr)		
					f.close()
				except IOError, e:
					raise CmplException("IO error for file " + tmpName + ": "+e)
			else:
				print repStr	
			
		else:
			raise CmplException("No Solution found so far")
	#*********** end solutionReport ******	
	
	#*********** saveSolutionCsv **********	
	def saveSolutionCsv(self, solFileName=None):
		if self.__solutions.nrOfSolutions > 0:

			if 	self.__remoteMode == True:
				self.__problem = os.path.splitext(self.__model)[0]
			else:
				self.__problem = os.path.splitext(self.__model)[0] + "_" + self.__id 
		
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
				i=0
				for s in self.__solutions.solutions:
					i+=1
					f.write("\n")
					f.write("%s;%g\n" %  ("Solution nr", i))
					f.write("%s;%s\n" %  ("Objective status", s.status))
					f.write("%s;%f;(%s!)\n" %  ("Objective value", s.value, self.__solutions.objectiveSense))
					if len(s.variables)>0:
						f.write("%s\n" %  "Variables")
						f.write("%s;%s;%s;%s;%s;%s\n" % ( "Name" , "Type" , "Activity", "LowerBound", "UpperBound" , "Marginal"))
						for v in s.variables:
							if v.type=="C":
								f.write("%s;%s;%f;%f;%f;%f\n" % ( v.name , v.type ,v.activity, v.lowerBound,v.upperBound ,v.marginal))
							else:
								f.write("%s;%s;%g;%f;%f;%f\n" % ( v.name , v.type ,v.activity, v.lowerBound,v.upperBound ,v.marginal))
					if len(s.constraints)>0:
						f.write("%s\n" %  "Constraints")
						f.write("%s;%s;%s;%s;%s;%s\n" % ( "Name" , "Type" , "Activity", "LowerBound", "UpperBound" , "Marginal"))
						for c in s.constraints:
							f.write("%s;%s;%f;%f;%f;%f\n" % ( c.name , c.type ,c.activity, c.lowerBound,c.upperBound ,c.marginal))
					
				f.close()
			except IOError, e:
				raise CmplException("IO error for file " + tmpName + ": "+e)					
			
		else:
			raise CmplException("No Solution found so far")
	#*********** end saveSolutionCsv *****				
	
	#*********** run  ********************	
	def run(self):
		self.solve()
	#*********** end run *****************
	


				
#*************** End Cmpl ****************************************		




		
		
		
		

		