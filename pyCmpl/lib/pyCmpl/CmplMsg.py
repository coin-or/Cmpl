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

from xml.sax.saxutils import unescape
from CmplDefs import *
from CmplException import *


#*************** CmplMsg *****************************************
class CmplMsg(object):
	def __init__(self):
		self.__type = ""
		self.__file = ""
		self.__line = ""
		self.__description = ""
		
	# getter and setter 	
	@property
	def type(self):
		return self.__type
	
	def setType(self, type):
		self.__type=type
		
	@property
	def file(self):
		return self.__file
		
	def setFile(self, file):
		self.__file=file
	
	@property
	def line(self):
		return self.__line
		
	def setLine(self, line):
		self.__line=line
		
	@property
	def description(self):
		return self.__description
		
	def setDescription(self, msg):
		self.__description=msg
	#end getter and setter
	
			
#*************** end CmplMsg *************************************


#*************** CmplMsg *****************************************
class CmplMessages(object):	

	def __init__(self, msgFile=None):
		self.__cmplStatus = ""
		self.__cmplVersion = ""
		self.__cmplMessage = ""
		self.__msgFile = msgFile
		self.__cmplMessageList = []
		
	#*********** cmplStatus ********	
	@property
	def cmplStatus(self):
		if self.__cmplStatus=="normal":
			return CMPL_OK
		elif self.__cmplStatus=="warning":
			return CMPL_WARNINGS
		elif self.__cmplStatus=="error":
			return CMPL_FAILED
		else:
			return CMPL_UNKNOWN
	#*********** end cmplStatus *****	
	
	#*********** cmplMessageList ****	
	@property
	def cmplMessageList(self):
		return self.__cmplMessageList
	#******* end cmplMessageList ****
	
	#*********** cmplMessage ********	
	@property
	def cmplMessage(self):
		return self.__cmplMessage
	#******* end cmplMessage ********
	
		
	#*********** readCmplMessages ********	
	def readCmplMessages(self, msgStr=None):
	
		if msgStr == None and self.__msgFile == None:
			raise CmplException("Neither cmplMessageFile nor cmplMessageString defined")
		
		if 	self.__msgFile != None:
			if not os.path.isfile(self.__msgFile):
				raise CmplException("CMPL failed."  )
	
			try:	
				f = open(self.__msgFile, "r")
				msgStr = f.read()
				f.close()
			except IOError, e:
				raise CmplServerException("IO error for instance file ")
				
				
		lines = cStringIO.StringIO(msgStr) 
		
		lineNr=1
		generalSection=False
		msgSection=False
		x = CmplMsg()
		
		for line in lines:
			if lineNr == 1:
				if not "<?xml version" in line: 
					raise CmplException("File " + self.__msgFile + " - is not a XML file!")
				else:
					lineNr += 1
					continue
			if lineNr == 2:
				if not "<CmplMessages" in line: 
					raise CmplException("Cant't read cmplMessage file " + self._cmplMsgFile + " - wrong file type!")
				else:
					lineNr += 1
					continue
	
			if "<general>" in line:
				generalSection=True
				msgSection=False
				continue
			if "</general>" in line:
				generalSection=False
				continue
			if "<messages" in line:
				generalSection=False
				msgSection=True
				continue
			if "</messages" in line:
				msgSection=False
				continue
				
			if generalSection:
				if "<generalStatus" in line:
					self.__cmplStatus = re.findall("<generalStatus>([^\"]*)</generalStatus>", line)[0]
					continue
				if "<message" in line:
					self.__cmplMessage = re.findall("<message>([^\"]*)</message>", line)[0]
					continue
				if "<cmplVersion" in line:
					self.__cmplVersion = re.findall("<cmplVersion>([^\"]*)</cmplVersion>", line)[0]
					continue
		
			if msgSection:
				if "<message" in line:
					tmpList=re.findall("\"([^\"]*)\"", line)
					x = CmplMsg()
					x.setType(tmpList[0])
					x.setFile(tmpList[1])
					x.setLine(tmpList[2])
					x.setDescription(unescape(tmpList[3]))
					self.__cmplMessageList.append(x)

						
							
	#*********** end readCmplMessages ****	
		
	

		
		
		
		

		