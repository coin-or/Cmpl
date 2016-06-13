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

import sys
import os

#************* pyCmpl and cmplserver includes ****************

isSolverStudio=False
try:		
	isSolverStudio = os.environ['SOLVERSTUDIO']=='1'
	if isSolverStudio:
		try:
			cmplPath=os.environ['CMPLPATH']	
		except:
			raise Exception("Internal error - cannot find CMPL path") 
		
		cmplbin=cmplPath+os.sep+"bin"+os.sep+"cmpl.exe"
		os.environ.update({'CMPLBINARY':cmplbin })
		os.chdir(cmplPath)
		
		cmplServerPath=cmplPath+os.sep+"cmplServer"
		os.environ.update({'CMPLSERVERPATH':cmplServerPath })
		
		sys.path.append(cmplPath)
except:
	pass



from cmplServer import *
from pyCmpl.CmplDefs import *
#************* end pyCmpl and cmplserver includes ****************


import xmlrpclib
import socket


#*********** startServer *************	
def startCmplServer(port, mode):
	try:
		if port!=None:		
			mPort = int(port)
		else:
			mPort=port
		m = CmplServer(mPort, mode)
		m.startServer()	
			
	except socket.error, e:
		print "CmplServer error: ",e 
		
	except CmplServerException, e:
		print e.msg 
		
	except ValueError:
		print "Wrong port: " + port
		 		
	except KeyboardInterrupt:
		print "CmplServer has been stopped"
#*********** end startServer  *********


#*********** startScheduler *************	
def startScheduler(port):
	try:
			
		mPort = int(port)
		m = CmplGridScheduler(mPort)
		
		m.startCmplGridScheduler()	
		
	except socket.error, e:
		print "CmplGrid scheduler error: ",e 
		
	except CmplServerException, e:
		print e.msg 
		
	except ValueError:
		print "Wrong port: " + port
		 		
	except KeyboardInterrupt:
		print "CmplGrid scheduler has been stopped"
#*********** end startScheduler  *********


#*********** isRunning ****************		
def status():
	url = "http://localhost"
	if port!=None:
		url += ":"+port
	else:
		url += ":8008"
	try:
		cmplServer = xmlrpclib.ServerProxy(url) 
		ret = cmplServer.status()
		print "CmplServer is running: "+ str(ret)
		
	except:
		print "CmplServer is not running ..." + str(sys.exc_info()[1])
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
			print "CmplServer has been stopped"
	except:
		print "Error: "+ str(sys.exc_info()[1])
#*********** end stopServer ***********		


if len(sys.argv)==3:
	port = sys.argv[2] 
else:
	port = None
	
print port
	
if sys.argv[1]=="-start":
	startCmplServer(port, STANDALONE_SERVER )
elif sys.argv[1]=="-startInGrid":
	startCmplServer(port, CMPL_GRID)
elif sys.argv[1]=="-startScheduler":
	startScheduler(port)
elif sys.argv[1]=="-stop":
	stopServer()
elif sys.argv[1]=="-status":
	status()
else:
	quit()


