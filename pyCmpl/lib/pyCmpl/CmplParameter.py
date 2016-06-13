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
from math import *

from CmplException import *
from CmplTools import *
from CmplSet import *

		
#*************** CmplParameter ***********************************		
class CmplParameter(object):

	#*********** constructor *********
	def __init__(self, name, *sets ):
	
		self.__name = ""
		self.__rank = 0
		self.__valueList = []
		self.__count=1
		self.__defaultVal=None
		
		defSets=None
		
		if len(sets) == 0:
			defSets=sets
		else:
			if type(sets[0]) == list:
				defSets=sets[0]
			else:
				defSets=sets
	
		if type(name) != str:
			raise CmplException("not a valid name for a parameter: " + str(name) )
		elif CmplTools.stringContainsWhitespace(name):
			raise CmplException("parameter name " + str(name) + " contains whitespaces")
		else:
			self.__name = name
		
		self._setList = []
		self.__rank = 0
	
		for s in defSets:
			if type(s) != CmplSet:
				raise CmplException(str(s)+ " is not a CMPL set ")
			else: 
				self._setList.append(s)
				self.__rank+=s.rank
		
	#*********** End constructor ****
	
	#*********** valueCount *********	
	def __countElements(self, vList):
		_count=len(vList)
		if type(vList[0]) == list:
			_count *= self.__countElements(vList[0])
		return _count
	#*********** end valueCount *****
		
	
	#*********** values *************	
	def setValues(self, val , defaultVal=None):
		self.__valueList=[]
		if self.__rank == 0:
			if not 'LIST' in str(type(val)).upper():
			#if type(val) != list:
				self.__valueList.append(val)
			else:
				raise CmplException("incompatible data for a scalar parameter : " + str(val) )
		else:
			if  'LIST' in str(type(val)).upper():
			#if type(val) == list:
				self.__valueList=val
				sCount=1
				for s in self._setList:
					sCount*=s.count()
					
				self.__count = self.__countElements(val)		
				if self.__count !=  sCount :
					raise CmplException("The dimension of the paramter "+ self.__name +" doesn't match the dimension of the set(s)."  )

			elif  'DICT' in str(type(val)).upper():
			#elif type(val) == dict:
				self.__valueList=val
				
				# keys are to be checked here or not to be checked in here (then in CMPL)??

				if defaultVal!=None:
					#sparse array with default
					if not (type(defaultVal) != str or type(defaultVal) != int or type(defaultVal) != int or type(defaultVal) != float ):
						raise CmplException("Wrong type of the default value of the paramter "+ self.__name +" : " + str(defaultVal) + " : " + type(defaultVal)  )
						
					self.__defaultVal=defaultVal
				
				else:
					#dense array
					sCount=1
					sRank=0
					for s in self._setList:
						sCount*=s.count()
						
					if sCount!=len(val):
						raise CmplException("The dimension of the paramter "+ self.__name +" doesn't match the dimension of the set(s)."  )
					
					if 'LIST' in str(type(val.values()[0])).upper() or 'DICT' in str(type(val.values()[0])).upper():
					#if type(val.values()[0]) == list or type(val.values()[0])== dict:
						raise CmplException("Unexpected data type for paramter "+ self.__name   )
					
			else:
				raise CmplException("incompatible data for a parameter array: " + str(val) )
	
	#*********** end values *********
	
	#*********** setList ************	
	@property
	def setList(self) :
		return self._setList
	#*********** end setList ********
	
	#*********** valueList **********
	@property
	def values(self):
		return self.__valueList		
	#*********** End valueList ******	
		
	#*********** valueList **********
	@property
	def value(self):
		return self.__valueList[0]		
	#***	
		
	#*********** name ***************
	@property
	def name(self):
		return self.__name		
	#*********** end name ***********	
	
	#*********** rank ***************
	@property
	def rank(self):
		return self.__rank		
	#*********** end rank ***********
	
	#*********** len ***************
	@property
	def len(self):
		return self.__count		
	#*********** end rank ***********

	#*********** defaultValue ************	
	@property
	def defaultValue(self) :
		return self.__defaultVal
	#*********** end setList ********
		
#*************** end CmplParameter *******************************		
	
		
	

		
		
		
		

		