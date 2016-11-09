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

	
#*************** CmplSet *****************************************		
class CmplSet(object):
	
	#*********** constructor ********
	def __init__(self, name, rank=1):
	
		self.__name = ""
		self.__type = 0   # 0 enumeration set - 1 tuple set - 2 alg set n..m - 3 alg set n(k)m
		self.__valueList = []
		self.__len = 0
		
		if type(name) != str:
			raise CmplException("not a valid name for set: " + str(name) )
		elif CmplTools.stringContainsWhitespace(name):
			raise CmplException("set name " + str(name) + " contains whitespaces")
		else:
			self.__name = name
			
		self.__rank=rank
	
	#*********** End constructor ****

	#*********** count **************	
	def count(self):
		_count = 0
		if self.__type == 0:  
			_count = len(self.__valueList)/self.__rank
		elif self.__type == 1:
			_count = len(self.__valueList)
		elif self.__type == 2:
			_count = self.__valueList[1]-self.__valueList[0]+1
		elif self.__type == 3:
			_count = ceil(  (fabs( self.__valueList[2] - self.__valueList[0])  +1 ) / fabs(self.__valueList[1]))
		return _count
	#*********** end count **********
		
			
	#*********** values *************	
	def setValues(self, val1, val2=None, val3=None):
		self.__valueList = []
	
		if val2 == None and val3 == None:
			if not 'LIST' in str(type(val1)).upper():
			#if type(val1) != list:
				raise CmplException("unexpected values for set " + self.__name + " : " + str(val1) + " is not a list")
			else:
				if 'LIST' in str(type(val1[0])).upper()  or 'TUPLE' in str(type(val1[0])).upper():
				#if type(val1[0]) == list or type(val1[0]) == tuple:
					if len(val1[0])!=self.__rank:
						raise CmplException("Rank and number of indexing entries for set " + self.__name + " : " + str(val1[0]) + " don't match.")
					self.__type = 1
					self.__len = len(val1) / self.__rank
				else:
					self.__type = 0
					if  self.__rank>1:
						raise CmplException( "incorrect definition of a tuple set " + self.__name)
				
				self.__valueList = val1
	
		elif val2 != None and val3 == None:
				self.__type = 2
				self.__rank = 1
				if type(val1) == int and type(val2) == int:
					if val2 < val1:
							raise CmplException( "unexpected values for set " + self.__name + " : end value "+ str(val2) + " is less then start value " + str(val1) )
					self.__valueList.append(val1)
					self.__valueList.append(val2)
				else:
					raise CmplException( "unexpected values for set " + self.__name + " : "+ str(val1) + "/" + str(val2) + "is not an valid integer combination")
	
		else:		
			self.__type = 3
			self.__rank = 1
			if type(val1) == int and type(val2) == int and type(val3) == int:
				self.__valueList.append(val1)
				self.__valueList.append(val2)	 
				self.__valueList.append(val3)

				if val2>0:
					if val3 <= val1:
						raise CmplException( "unexpected values for set " + self.__name + " : end value "+ str(val3) + " is less than or equal to start value " + str(val1) +" and the increment is positive "+ str(val2) )
				else: 
					if val2<0:
						if val1 <= val3:
							raise CmplException( "unexpected values for set " + self.__name + " : end value "+ str(val3) + " is greater than or equal to start value " + str(val1) +" and the increment is negative "+ str(val2) )
					else: 
						raise CmplException( "unexpected values for the set " + self.__name + " :  increment/decrement equals zero  " )
			else:
				raise CmplException( "unexpected values for set " + self.__name + " : "+ str(val1) + "/" + str(val2) + "/" + str(val3) + "is not an valid integer combination")
	#*********** end values *********
	
	
	#*********** valueList **********
	@property
	def valueList(self):
		return self.__valueList		
	#*********** End valueList ******	
	
	#*********** set **********
	@property
	def values(self):
	#	if self.__type == 0 or self.__type == 1:
	#		return self.__valueList		
		if self.__type == 0: 
			return self.__valueList		
			
		elif self.__type == 1:
			tmpList=[]
			for c in self.__valueList:
				tmpTuple="("
				
				for e in range(len(c)):
					if e>0:
						tmpTuple=tmpTuple+","
					if type(c[e])==str:	
						tmpTuple=tmpTuple+"\""+c[e]+"\""
					else:
						tmpTuple=tmpTuple+str(c[e])
				
				tmpTuple=tmpTuple+")"	
				tmpList.append(eval(tmpTuple))
			
			return tmpList
				
		elif self.__type == 2:
			return range(self.__valueList[0],self.__valueList[1]+1)
		elif self.__type == 3:
			if self.__valueList[1]>0:
				return range(self.__valueList[0],self.__valueList[2]+1,self.__valueList[1])
			else:
				return range(self.__valueList[0],self.__valueList[2]-1,self.__valueList[1])
	#*********** End valueList ******	
		
	#*********** name ***************
	@property
	def name(self):
		return self.__name		
	#*********** end name ***********	
	
	#*********** type ***************
	@property
	def type(self):
		return self.__type		
	#*********** end type ***********
	
	#*********** rank ***************
	@property
	def rank(self):
		return self.__rank		
	#*********** end rank ***********
	
	#*********** len ****************
	@property
	def len(self):
		if self.__type == 0 or self.__type == 1:
			return len(self.__valueList)	
		elif self.__type == 2:
			return len(range(self.__valueList[0],self.__valueList[1]+1))
		elif self.__type == 3:
			if self.__valueList[1]>0:
				return len(range(self.__valueList[0],self.__valueList[2]+1,self.__valueList[1]))
			else:
				return len(range(self.__valueList[0],self.__valueList[2]-1,self.__valueList[1]))
	#*********** end len ************
	
	
#*************** End CmplSet *****************************************		
	

	


		
		
		
		

		