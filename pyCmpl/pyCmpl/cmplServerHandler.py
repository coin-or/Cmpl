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

import sys
from pyCmpl.CmplServerException import *
from pyCmpl.CmplServer import *
import xmlrpclib


#*********** startServer *************	
def startServer(port=None):
	try:
			
		if port!=None:
			mPort = int(port)
			m = CmplServer(mPort)
		else: 
			m = CmplServer()
		
		#print "CmplServer has been started..."
		m.startServer()	
		
		
	except CmplServerException, e:
		print e.msg 
		
	except ValueError:
		print "Wrong port: " + port
		 		
	except KeyboardInterrupt:
		print "CmplServer has been stoped"
#*********** end startServer  *********

#*********** isRunning ****************		
def isRunning():
	url = "http://localhost"
	if port!=None:
		url += ":"+port
	else:
		url += ":8008"
	try:
		cmplServer = xmlrpclib.ServerProxy(url) 
		if cmplServer.isRunning():
			print "... OK "
		else:
			print "CmplServer is not running ..."
	except:
		print "CmplServer is not running ..."
#*********** end isRunning ************		
		

#*********** stopServer ***************		
def stopServer():
	url = "http://localhost"
	if port!=None:
		url += ":"+port
	else:
		url += ":8008"
	try:
		cmplServer = xmlrpclib.ServerProxy(url) 
		if cmplServer.stopServer():
			print "CmplServer has been stoped"
	except:
		print "Error: "+ str(sys.exc_info()[1])
#*********** end stopServer ***********		


if len(sys.argv)==3:
	port = sys.argv[2] 
else:
	port = None
	
if sys.argv[1]=="-start":
	startServer(port)
elif sys.argv[1]=="-stop":
	stopServer()
elif sys.argv[1]=="-isRunning":
	isRunning()
else:
	quit()

			
	
