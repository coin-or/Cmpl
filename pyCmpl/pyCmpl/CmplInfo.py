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


#import os
import re
import cStringIO 

#from CmplDefs import *
from CmplException import *


#*************** CmplInfo *****************************************
class CmplInfo(object):
		
	#*********** constructor ******
	def __init__(self, cmplInfo=None):
		self.__cmplInfoFile=cmplInfo
		self.__instanceName=""
		self.__varProdFileName = ""
		self.__varProdText = ""
		self.__statisticsFileName = ""
		self.__statisticsText = ""
		self.__matrixFileName = ""
		self.__matrixText = ""
	#*********** end constructor ******
	
	# getter and setter 	
	@property
	def varProdFileName(self):
		return self.__varProdFileName
	
	@property
	def varProdText(self):
		return self.__varProdText
		
	@property
	def statisticsFileName(self):
		return self.__statisticsFileName
		
	@property
	def statisticsText(self):
		return self.__statisticsText
		
	@property
	def matrixFileName(self):
		return self.__matrixFileName
		
	@property
	def matrixText(self):
		return self.__matrixText
	#end getter and setter
	
		
	#*********** readCmplInfo ********	
	def readCmplInfo(self, cmplInfoStr=None):
	
		if cmplInfoStr == None and self.__cmplInfoFile == None:
			raise CmplException("Neither cmplInfoFile nor cmplInfoString defined")
		
		if 	self.__cmplInfoFile != None:
			if not os.path.isfile(self.__cmplInfoFile):
				raise CmplException("Can't read the CmplInfo file: " + self.__cmplInfoFile  )
	
			try:	
				f = open(self.__cmplInfoFile, "r")
				cmplInfoStr = f.read()
				f.close()
			except IOError, e:
				raise CmplServerException("IO error for CmplInfo file: "+self.__cmplInfoFile)
				
				
		lines = cStringIO.StringIO(cmplInfoStr) 
		
		lineNr=1
		generalSection=False
		varProdSection=False
		statSection=False
		matrixSection=False
			
		for line in lines:
			
			if lineNr == 1:
				if not "<?xml version" in line: 
					raise CmplException("File - is not a XML file!")
				else:
					lineNr += 1
					continue
			if lineNr == 2:
				if not "<CmplInfo" in line: 
					raise CmplException("Cant't read CmplInfo file - wrong file type!")
				else:
					lineNr += 1
					continue
	
			if "<general>" in line:
				generalSection=True
				varProdSection=False
				statSection=False
				matrixSection=False
				continue
			if "</general>" in line:
				generalSection=False
				continue
			if "<statistics" in line:
				self.__statisticsFileName=re.findall("\"([^\"]*)\"", line)[0]
				generalSection=False
				varProdSection=False
				statSection=True
				matrixSection=False
				continue
			if "</statistics" in line:
				statSection=False
				continue
			if "<variableProducts" in line:
				self.__varProdFileName=re.findall("\"([^\"]*)\"", line)[0]
				generalSection=False
				varProdSection=True
				statSection=False
				matrixSection=False
				continue
			if "</variableProducts" in line:
				varProdSection=False
				continue
			if "<matrix" in line:
				self.__matrixFileName=re.findall("\"([^\"]*)\"", line)[0]
				generalSection=False
				varProdSection=False
				statSection=False
				matrixSection=True
				continue
			if "</matrix" in line:
				matrixSection=False
				continue
				
			if generalSection:
				if "<generalStatus" in line:
					self.__instanceName = re.findall("<instanceName>([^\"]*)</instanceName>", line)[0]
					continue
		
			if statSection:
				self.__statisticsText=self.__statisticsText+line
		
			if varProdSection:
				self.__varProdText=self.__varProdText+line
				
			if matrixSection:
				self.__matrixText=self.__matrixText+line
							
	#*********** end readCmplInfo ****	
		
	

#*************** CmplInfo *****************************************		
		
		
		

		