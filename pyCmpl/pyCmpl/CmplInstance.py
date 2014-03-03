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
import xml.dom.minidom as dom
import random

from xml.sax.saxutils import unescape, escape


from CmplException import *
from CmplServerException import *


#*************** CmplInstance ***********************************
class CmplInstance(object):

	#*********** constructor **********
	def __init__(self):
		
		self.__cmplFile = ""
		self.__optionsList = []
		self.__dataString = ""
		self.__cmplDataList = {}
		self.__instStr = ""
		self.__jobId = ""
		self.__cmplName = ""
		
	#*********** end constructor ******
	
	# getter **************************	
	@property
	def jobId(self):
		return self.__jobId
		
	@property
	def options(self):
		return self.__optionsList
	
	# end getter ***********************	
	
		
	#*********** cmplInstanceStr **********
	def cmplInstanceStr(self, cmplFileName, optList, dataString, jobId):
	
		if not os.path.isfile(cmplFileName):
			raise CmplException("CMPL file " + cmplFileName + " does not exist."  )
		
		self.__cmplFile = cmplFileName
		self.__optionsList = optList
		self.__jobId = jobId
		self.__dataString = dataString
	
		if self.__dataString!="":
			self.__cmplDataList.update({ "__cmplData__"+os.path.basename(os.path.splitext(self.__cmplFile)[0])+".cdat" :   self.__dataString  })
		
		try:
			f = open(self.__cmplFile, "r")
			lines = f.readlines()
			f.close()

			commentSection=False
			
			lineNr = 0	
			for line in lines:
			
				line = line.strip()
			
				# besser contains
				if line.startswith('/*'):
					commentSection = True
					continue
					
				# besser contains	
				if line.startswith('*/'):
					commentSection = False
					
				if commentSection == True:
					continue
					
				if line.startswith('%data'):
					if line.find(':') != -1:
						tmpName = line[5:line.find(':')].strip()
					else:
						tmpName = line[5:].strip()
						
					if tmpName == "":	
						if self.__dataString!="":
							lines[lineNr] = line.replace("%data", "%data __cmplData__"+os.path.basename(os.path.splitext(self.__cmplFile)[0])+".cdat ") + "\n"
							tmpName="__cmplData__"
						else:				
							tmpName=os.path.basename(os.path.splitext(self.__cmplFile)[0])+".cdat" 
				
								
					if not (tmpName in self.__cmplDataList or tmpName=="__cmplData__"):
					
						tmpName1=os.path.dirname(os.path.abspath(cmplFileName))+"/"+tmpName
						
						if not os.path.isfile(tmpName1):
							raise CmplException("cmplDataFile " + tmpName1 + " does not exist."  )
						
						f = open(tmpName1, "r")
						dataLines = f.readlines()
						f.close()		
					
						tmpString = ""
						for dline in dataLines:
							tmpString += dline
								
						self.__cmplDataList.update({ tmpName :  tmpString  })
				
				lineNr += 1
				
					
			self.__instStr +=  "<?xml version = \"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>\n"
			self.__instStr +=  "<CmplInstance version=\"1.0\">\n" 			
			self.__instStr +=  "<general>\n"
			self.__instStr +=  "<name>"+os.path.basename(self.__cmplFile)+"</name>\n"
			self.__instStr +=  "<jobId>"+self.__jobId+"</jobId>\n"
			self.__instStr +=  "</general>\n"
			
			if len(self.__optionsList) > 0:
				self.__instStr +=  "<options>\n"
				for opt in self.__optionsList:
					self.__instStr +=  "<opt>"+opt+"</opt>\n"
				self.__instStr +=  "</options>\n"
			
			self.__instStr +=  "<problemFiles>\n"
			self.__instStr +=  "<file name=\""+ os.path.basename(self.__cmplFile)+ "\" type=\"cmplMain\">\n"	
			tmpStr=""
			for line in lines:
				#if line.strip() != "":
				tmpStr += line	
			#self.__instStr += b64encode(tmpStr)
			self.__instStr += escape(tmpStr)
			
			self.__instStr +=  "\n"	
			self.__instStr +=  "</file>\n"	
			
			for d in self.__cmplDataList:
				self.__instStr +=  "<file name=\""+ d + "\" type=\"cmplData\">\n"	
				#self.__instStr +=  b64encode(self.__cmplDataList[d])
				self.__instStr +=  escape(self.__cmplDataList[d])
				self.__instStr +=  "\n"	
				self.__instStr +=  "</file>\n"
			self.__instStr +=  "</problemFiles>\n"	
			self.__instStr +=  "</CmplInstance>\n" 	
    		
		except IOError, e:
			raise CmplException("IO error : "+e)
		
		return self.__instStr
		
	#*********** end cmplInstanceStr ******	
		
	#*********** writeCmplInstance **********
	def writeCmplInstance(self, folder, instStr):	
	
		if os.path.exists(folder) == False:
			raise CmplServerException("Path <"+self.__cmplServerPath+"> doesn't exist.")
			
		instDom = dom.parseString(instStr)

		if instDom.firstChild.nodeName!="CmplInstance":
			raise CmplServerException("Cant't read cmplInstance file - wrong file type!")
			
		for entry in instDom.firstChild.childNodes: 
		
				if entry.nodeName == "general":
					for entry1 in entry.childNodes:
						if entry1.nodeName == "name":
							self.__cmplName = entry1.firstChild.data.strip()
							continue
						if entry1.nodeName == "jobId":
							self.__jobId = entry1.firstChild.data.strip()
							continue
							
				if entry.nodeName == "options":
					for entry1 in entry.childNodes: 
						if entry1.nodeName == "opt":
							self.__optionsList.append(entry1.firstChild.data.strip())
							
				if entry.nodeName == "problemFiles":
					for entry1 in entry.childNodes: 
						if entry1.nodeName == "file":
							tmpName = folder+self.__jobId+os.sep+entry1.getAttribute("name")
							#tmpContent = b64decode(entry1.firstChild.data.strip())
							tmpContent = unescape(entry1.firstChild.data.strip())
							
							try:
								f = open(tmpName, 'w')
								f.write(tmpContent)		
								f.close()
							except IOError, e:
								raise CmplServerException("IO error for file " + tmpName + ": "+e)
	#*********** end writeCmplInstance *******

#*************** end CmplInstance  ******************************

		