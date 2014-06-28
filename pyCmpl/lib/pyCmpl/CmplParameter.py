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
	
		
		if type(name) != str:
			raise CmplException("not a valid name for a parameter: " + str(name) )
		elif CmplTools.stringContainsWhitespace(name):
			raise CmplException("parameter name " + str(name) + " contains whitespaces")
		else:
			self.__name = name
		
		self._setList = []
		self.__rank = 0
	
		for s in sets:
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
	def setValues(self, val ):
		self.__valueList=[]
		if self.__rank == 0:
			if type(val) != list:
				self.__valueList.append(val)
			else:
				raise CmplException("incompatible data for a scalar parameter : " + str(val) )
		else:
			if type(val) == list:
				self.__valueList=val
				sCount=1
				for s in self._setList:
					sCount*=s.count()
					
				self.__count = self.__countElements(val)		
				if self.__count !=  sCount :
					raise CmplException("The dimension of the paramter "+ self.__name +" doesn't match the dimension of the set(s)."  )
				
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

		
#*************** end CmplParameter *******************************		
	
		
	

		
		
		
		

		