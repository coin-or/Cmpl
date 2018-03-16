#***********************************************************************
 #  This code is part of CmplServer 
 #
 #  Copyright (C) 2013, 2014
 #  Mike Steglich - Technical University of Applied Sciences
 #  Wildau, Germany 
 #
 #  CmplServer is a project of the Technical University of 
 #  Applied Sciences Wildau and the Institute for Operations Research 
 #  and Business Management at the Martin Luther University 
 #  Halle-Wittenberg.
 #  Please visit the project homepage <www.coliop.org>
 # 
 #  CmplServer is free software; you can redistribute it and/or modify it 
 #  under the terms of the GNU Lesser General Public License as published by 
 #  the Free Software Foundation; either version 3 of the License, or 
 #  (at your option) any later version.
 # 
 #  CmplServer is distributed in the hope that it will be useful, but WITHOUT
 #  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 #  or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public 
 #  License for more details.
 #  
 #  You should have received a copy of the GNU Lesser General Public License
 #  along with this program; if not, see <http://www.gnu.org/licenses/>.
 #
 #**********************************************************************

#!/usr/bin/python 

from SocketServer import ThreadingMixIn
from SimpleXMLRPCServer import SimpleXMLRPCServer 
from time import gmtime, strftime


from pyCmpl.CmplDefs import *

import os
import time
import string
import platform

#*************** CmplXmlRpcServer ********************************
class CmplXmlRpcServer(ThreadingMixIn, SimpleXMLRPCServer):

	#*********** process_request **********
    def process_request(self, request, client_address):
        self.client_address = client_address
        return SimpleXMLRPCServer.process_request(self, request, client_address)
	#*********** end process_request ******
           
#*************** end CmplXmlRpcServer ****************************        


#*************** CmplServerTools ***************************************
class CmplServerTools(object):
	
	#*********** cmplLogging ***********
	@staticmethod
	def cmplLogging(logFile, msg , id=None , name=None   ):
		try:
			if id==None and name==None:
				logFile.write("[" + strftime("%Y-%m-%d %H:%M:%S", gmtime())+ "] - " + msg+"\n")
			elif name==None:
				#logFile.write( string.split(id,"-")[0] + " - [" + strftime("%Y-%m-%d %H:%M:%S", gmtime())+ "] - " + id + " <"+msg+">\n")
				logFile.write( "[" + strftime("%Y-%m-%d %H:%M:%S", gmtime())+ "] - " + id + " -  <"+msg+">\n")
			else:
				#logFile.write( string.split(id,"-")[0] + " - [" + strftime("%Y-%m-%d %H:%M:%S", gmtime())+ "] - " + id + " - " + name + " <"+msg+">\n")
				logFile.write(  "[" + strftime("%Y-%m-%d %H:%M:%S", gmtime())+ "] - " + id + " - " + name + " <"+msg+">\n")

			logFile.flush()
		except IOError, e:
			pass
			#raise Exception("IO error for solution or log file ")
	#*********** end cmplLogging *******
	
	#*********** readFileContent **********	
	@staticmethod
	def readFileContent(fileName):
		try:
			f = open(fileName, "r")
			lines = f.read()
			f.close()
			return lines
		except IOError, e:
			raise Exception("IO error for file "+fileName)
	#*********** end readFileContent ******	
	
	
#*************** end CmplServerTools ***********************************		  
	


	
	



		
		
		
		

		