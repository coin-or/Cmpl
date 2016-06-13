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

#!/usr/bin/python -i

import os
import sys
from pyCmpl import *

def pyCmplShell():
	if sys.version_info[:2] == (2,7):

		
		print "CMPL\'s Python API"
	
		sys.ps1 = "pyCmpl> "
		sys.ps2 = "....... "
	
		pyCmplAnswer=""
	
		while pyCmplAnswer!="quit()" and pyCmplAnswer!="exit()" :
			try:
				pyCmplAnswer = raw_input("pyCmpl> ")
				if pyCmplAnswer!="quit()" and pyCmplAnswer!="exit()":
					exec(pyCmplAnswer) 
			except CmplException, e:
				print e.msg
			except:
				print "Python error : " +str(sys.exc_info()[1])
	else:
		print "ERROR - pyCmpl only supports Python 2.7"
		
	exit(0)

if __name__ == "__main__":
	pyCmplShell()		
		
		
		

		