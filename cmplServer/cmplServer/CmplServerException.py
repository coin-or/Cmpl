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

from CmplServerTools import *
import cStringIO

#*************** CmplServerException ***********************************
class CmplServerException(Exception):
	
	def __init__(self, msg, logFile=None ):
		self.__msgStr=cStringIO.StringIO()
	
		if not "CmplServer error:" in msg: 
			self.__msgStr.write("CmplServer error: " +msg)
		else: 
			self.__msgStr.write(msg)
	
		if logFile != None:
			CmplServerTools.cmplLogging( logFile,  msg )
		
					
	def __str__(self):
		return "CmplServer error:"
	
	def __del__(self):
		self.__msgStr.close()
	
	@property
	def msg(self):
		return self.__msgStr.getvalue()
#*************** end CmplServerException *******************************


		
		
		
		

		