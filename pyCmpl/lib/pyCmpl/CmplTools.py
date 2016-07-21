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

import re


#*************** CmplTools ***************************************
class CmplTools(object):

	#*********** stringContainsWhitespace **********
	@staticmethod
	def stringContainsWhitespace(string):
		regex = re.compile('[\t\n\x0b\x0c\r ]+')
		if regex.search(string):
			return True
		else:
			return False
	#*********** end stringContainsWhitespace ******
	
	#*********** strIsNumber ***********************
	@staticmethod
	def strIsNumber(str):
		r = re.compile('[^0-9.]+')
		if  r.search(str):
			return False
		else:
			return True
	#*********** end strIsNumber *******************
	
	#*********** strIsValidIndex ***********************
	@staticmethod
	def strIsValidIndex(str):
		r = re.compile('[^0-9a-zA-Z\-_.]+')
		if  r.search(str):
			return False
		else:
			return True
	#*********** end strIsValidIndex *******************
	
	
	
	#*********** strContains ***********************
	@staticmethod
	def strContains(str0, str1):
		return str1 in str0
	#*********** end strContains *******************

#*************** end CmplTools **********************************


		
		
		

		