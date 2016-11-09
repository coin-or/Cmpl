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

import os
import re
import cStringIO

from CmplException import *

#*************** CmplSolElement *************************************
class CmplSolElement(object):
	#*********** constructor **********
	def __init__(self):
		self.__idx = 0
		self.__name = ""
		self.__type = ""
		self.__activity = 0
		self.__lowerBound = 0
		self.__upperBound = 0
		self.__hasMarginal = False
		self.__marginal = 0
	#*********** end constructor ******
	
	# getter and setter 
	@property
	def idx(self):
		return self.__idx 
		
	def setIdx(self, idx):
		self.__idx = idx
	
	@property
	def name(self):
		return self.__name 
	
	def setName(self, name):
		self.__name = name
	
	@property
	def type(self):	
		return self.__type 
		
	def setType(self, type):
		self.__type=type
		
	@property
	def activity(self):
		return self.__activity 
		
	def setActivity(self, activity):
		self.__activity=activity
		
	@property
	def lowerBound(self):		
		return self.__lowerBound
		
	def setLowerBound(self, lB):
		self.__lowerBound = lB
		
	@property
	def upperBound(self):
		return self.__upperBound 
		
	def setUpperBound(self, uB):
		self.__upperBound = uB
		
	@property
	def marginal(self):
		return self.__marginal 
		
	def setMarginal(self, marginal):
		self.__marginal=marginal
	# end getter and setter 
	
		
#*************** end CmplSolElement ******************************

#*************** CmplSolution  ***********************************
class CmplSolution(object):

	#*********** constructor **********
	def __init__(self):
		self.__idx = 0
		self.__status = ""
		self.__value = 0
		self.__variables =[]
		self.__constraints = []
	#*********** end constructor ******
	
	# getter and setter 
	@property
	def idx(self):
		return self.__idx 
		
	def setIdx(self, idx):
		self.__idx = idx
	
	@property
	def status(self):
		return self.__status
		
	def setStatus(self, status):
		self.__status=status
		
	@property
	def value(self):
		return self.__value
		
	def setValue(self, val):
		self.__value=val
	
	@property
	def variables(self):
		return self.__variables
		
	def setVar(self, var):
		self.__variables.append(var)
		
	@property
	def constraints(self):
		return self.__constraints
		
	def setCon(self, con):
		self.__constraints.append(con)
		
	# end getter and setter 	
		
#*************** end CmplSolution ********************************


#*************** CmplSolutions  **********************************
class CmplSolutions(object):

	#*********** constructor **********
	def __init__(self, solFile=None):
		self.__nrOfVariables = 0
		self.__nrOfConstraints = 0
		self.__objectiveName = ""
		self.__objectiveSense = ""
		self.__nrOfSolutions = 0
		self.__solverName = ""
		self.__solverMsg = ""
		self.__varDisplayOptions = ""
		self.__conDisplayOptions = ""
		self.__asciiSolFile = ""
		self.__csvSolFile = "" 
		self.__cmplSolFile = ""
		self.__solList = []
		self.__solutionFile = solFile
		self.__solFileContent = ""
		self.__isIntegerProgram = False
	#*********** end constructor ******
	
	# getter
	@property
	def nrOfVariables(self):
		return self.__nrOfVariables
		
	@property
	def nrOfConstraints(self):
		return self.__nrOfConstraints 
		
	@property
	def nrOfConstraints(self):
		return self.__nrOfConstraints 
		
	@property
	def objectiveName(self):
		return self.__objectiveName 
		
	@property
	def objectiveSense(self):
		return self.__objectiveSense 
		
	@property
	def nrOfSolutions(self):
		return self.__nrOfSolutions 
		
	@property
	def solver(self):
		return self.__solverName 
		
	@property
	def solverMessage(self):
		return self.__solverMsg 
		
	@property
	def varDisplayOptions(self):
		return self.__varDisplayOptions 
		
	@property
	def conDisplayOptions(self):
		return self.__conDisplayOptions
		
	@property
	def asciiSolFile(self):
		return self.__asciiSolFile
		
	@property
	def csvSolFile(self):
		return self.__csvSolFile
			
	@property
	def cmplSolFile(self):
		return self.__cmplSolFile
		
	@property
	def solutions(self):
		return self.__solList
		
	@property
	def solution(self):
		return self.__solList[0]
		
	@property
	def solFileContent(self):
		return self.__solFileContent
	
	def delSolFileContent(self):
		self.__solFileContent=""
	
	@property
	def isIntegerProgram(self):
		return self.__isIntegerProgram
	
	#*********** readSolLine *************	
	def _readSolLine(self, line):
	
		tmpList=re.findall("\"([^\"]*)\"", line)

		x = CmplSolElement()
		x.setIdx(int(tmpList[0]))
		x.setName(tmpList[1])
		x.setType(tmpList[2])
		
		isSolverStudio=False
		try:		
			isSolverStudio = os.environ['SOLVERSTUDIO']=='1'
		except:
			pass
		
		if "<constraint" in line:
			x.setActivity(float(tmpList[3]))
		else:
			if x.type=='I' or x.type=='B':
				self.__isIntegerProgram=True
				
				if not isSolverStudio:	
					x.setActivity(long(tmpList[3]))
				else:
					x.setActivity(int(tmpList[3]))
			else:
				x.setActivity(float(tmpList[3]))
		
		if not isSolverStudio:	
			x.setLowerBound(float(tmpList[4]))
			x.setUpperBound(float(tmpList[5]))
		else:
			if tmpList[4].upper()=='-INF':
				x.setLowerBound(-1e+100)
			else:
				x.setLowerBound(float(tmpList[4]))
			if tmpList[5].upper()=='INF':	
				x.setUpperBound(1e+100)
			else:
				x.setUpperBound(float(tmpList[5]))

		#if tmpList[6]=='-':
		if tmpList[6]=='NaN':
			x.setMarginal(float('NaN'))
		else: 
			x.setMarginal(float(tmpList[6]))
		return x
	
	#*********** end readSolLine *********	
	
	
	#*********** readSolution ************	
	def readSolution(self, solStr=None):
		
		if (solStr == None or len(solStr)==0) and self.__solutionFile == None:
			raise CmplException("No solution found")
		
		if self.__solutionFile != None:
			if not os.path.isfile(self.__solutionFile):
				raise CmplException("No solution found" )
				
			try:	
				f = open(self.__solutionFile, "r")
				solStr = f.read()
				f.close()
			except IOError, e:
				raise CmplServerException("IO error for solution file ")
		
		lines = cStringIO.StringIO(solStr) 
		
		lineNr=1
		generalSection=False
		solutionSection=False
		varSection=False
		conSection=False
		s = CmplSolution()
		self.__solFileContent=solStr
		
		for line in lines:
			
			if lineNr == 1:
				if not "<?xml version" in line: 
					raise CmplException("File " + self.__solutionFile + " - is not a XML file!")
				else:
					lineNr += 1
					continue
			if lineNr == 2:
				if not "<CmplSolutions" in line: 
					raise CmplException("Cant't read cmplSolution file " + self.__solutionFile + " - wrong file type!")
				else:
					lineNr += 1
					continue
			
			if "<general>" in line:
				generalSection=True
				solutionSection=False
				continue
			if "</general>" in line:
				generalSection=False
				continue
				
			if "<solution" in line:
				generalSection=False
				solutionSection=True
				s = CmplSolution()
				tmpList = re.findall("\"([^\"]*)\"", line)
				s.setIdx(int(tmpList[0]))
				s.setStatus(tmpList[1])
				s.setValue(float(tmpList[2]))
				continue
			if "</solution" in line:
				solutionSection=False
				self.__solList.append(s)
				continue
				
			if generalSection:
				if "<nrOfVariables" in line:
					self.__nrOfVariables = int(re.findall("<nrOfVariables>([^\"]*)</nrOfVariables>", line)[0])
					continue
				if "<nrOfConstraints" in line:
					self.__nrOfConstraints = int(re.findall("<nrOfConstraints>([^\"]*)</nrOfConstraints>", line)[0])
					continue
				if "<objectiveName" in line:
					self.__objectiveName = re.findall("<objectiveName>([^\"]*)</objectiveName>", line)[0]
					continue
				if "<objectiveSense" in line:
					self.__objectiveSense = re.findall("<objectiveSense>([^\"]*)</objectiveSense>", line)[0]
					continue
				if "<nrOfSolutions" in line:
					self.__nrOfSolutions = int(re.findall("<nrOfSolutions>([^\"]*)</nrOfSolutions>", line)[0])
					continue
				if "<solverName" in line:
					self.__solverName = re.findall("<solverName>([^\"]*)</solverName>", line)[0]
					continue
				if "<solverMsg" in line:
					self.__solverMsg = re.findall("<solverMsg>([^\"]*)</solverMsg>", line)[0]
					continue
				if "<variablesDisplayOptions" in line:
					self.__varDisplayOptions = re.findall("<variablesDisplayOptions>([^\"]*)</variablesDisplayOptions>", line)[0]
					continue
				if "<constraintsDisplayOptions" in line:
					self.__conDisplayOptions = re.findall("<constraintsDisplayOptions>([^\"]*)</constraintsDisplayOptions>", line)[0]
					continue
				if "<asciiSolFile" in line:
					self.__asciiSolFile = re.findall("<asciiSolFile>([^\"]*)</asciiSolFile>", line)[0]
					continue
				if "<csvSolFile" in line:
					self.__csvSolFile = re.findall("<csvSolFile>([^\"]*)</csvSolFile>", line)[0]
					continue
				if "<cmplSolFile" in line:
					self.__cmplSolFile = re.findall("<cmplSolFile>([^\"]*)</cmplSolFile>", line)[0]
					continue	
				
			
			if solutionSection:
				if "<variables>" in line:
					varSection=True
					conSection=False
					continue
				if "</variables>" in line:
					varSection=False
					continue
				if "<linearConstraints>" in line:
					varSection=False
					conSection=True
					continue
				if "</linearConstraints>" in line:
					conSection=False
					continue
				if varSection:
					if "<variable" in line:
						s.setVar(self._readSolLine(line))
						continue
				if conSection:
					if "<constraint" in line:
						s.setCon(self._readSolLine(line))
						continue
		
	#*********** send readSolution *******




#*************** end CmplSolutions  ******************************

		