#***********************************************************************
 #  This code is part of Cmpl 
 #
 #  Copyright (C) 2013, 2014, 2015
 #  Mike Steglich - Technical University of Applied Sciences
 #  Wildau, Germany 
 #
 #  Cmpl is a project of the Technical University of 
 #  Applied Sciences Wildau and the Institute for Operations Research 
 #  and Business Management at the Martin Luther University 
 #  Halle-Wittenberg.
 #  Please visit the project homepage <www.coliop.org>
 # 
 #  Cmpl is free software; you can redistribute it and/or modify it 
 #  under the terms of the GNU General Public License as published by 
 #  the Free Software Foundation; either version 3 of the License, or 
 #  (at your option) any later version.
 # 
 #  Cmpl is distributed in the hope that it will be useful, but WITHOUT
 #  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 #  or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public 
 #  License for more details.
 #  
 #  You should have received a copy of the GNU General Public License
 #  along with this program; if not, see <http://www.gnu.org/licenses/>.
 #
 #**********************************************************************


# SolverStudio CpyMPL Processor
# v1.0.0, March 2015
# Mike Steglich


language = "pyCMPL"

import re
import shutil
import sys
import os
import traceback


#************* pyCmpl and cmplserver includes ****************
pyCmplPath=SolverStudio.LanguageDirectory().replace("PyCMPL","CMPL")
#print pyCmplPath
sys.path.append(pyCmplPath)
from pyCmpl import *
#************* end pyCmpl and cmplserver includes ************


#********************* main ************************************

try: 
	
	pyCmplFile = SolverStudio.ModelFileName
	
	cmplPath=SolverStudio.LanguageDirectory().replace("PyCMPL","CMPL")+"\\bin\\cmpl.exe"
	print "cmplPath="+cmplPath
	os.environ.update({'CMPLBINARY':cmplPath })
	os.environ.update({'SOLVERSTUDIO':'1' })
	
	print "## Running "+language+"..."
	print "## "+language+" model file:", pyCmplFile
			
	execfile(pyCmplFile)

	print "## "+language+" run completed."
	
except CmplException as e:
	# Report an exception detected by the CMLP parser (which includes a line number).
	print "## "+language+ " ERRORS"
	print "## > ", e.msg
        #traceback.print_exc(file=sys.stdout)
	print "## "+language+" did not complete; no solution is available. The sheet has not been changed."
except Exception as e:
	# Report an exception caused by our own code; the user should never see this.
	print "## ERRORS"
	print "## > ", e
	traceback.print_exc(file=sys.stdout)
	print "## "+language+" did not complete; no solution is available. The sheet has not been changed."
	raise e; # Throw it to report the line number in our underlying code
print "## Done"
os.environ.update({'SOLVERSTUDIO':'0' })

#************* end main ***************************************************

