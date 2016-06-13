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

from CmplMsg import *
import HTMLParser
import cStringIO

#*************** CmplException ***********************************
class CmplException(Exception):
	
	def __init__(self, msg, msgList=None):
		self.__msgStr=cStringIO.StringIO()
		
		if not "pyCmpl error:" in msg:
			self.__msgStr.write("pyCmpl error: " +msg)
		else:	
			self.__msgStr.write(msg)
		
		if msgList!=None:
			for m in msgList:
				self.__msgStr.write("\n" + m.type + " in file " + m.file + " at line " + m.line + " : " + HTMLParser.HTMLParser().unescape(m.description))
	
	def __str__(self):
		return "pyCmpl error:"
	
	def __del__(self):
		self.__msgStr.close()
	
	@property
	def msg(self):
		return self.__msgStr.getvalue()
#*************** end CmplException *******************************


		
		
		
		

		