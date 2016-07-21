#***********************************************************************
 #  This code is part of pyCMPL 
 #
 #  Copyright (C) 2013, 2014
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
	
#*************** general definitions  ****************************
#don' change the order of the entries, because this would cause a lot of confusions

COMPATIBILITY = 2  # CMPL 1.10 

CMPL_UNKNOWN = 0
CMPL_OK = 1
CMPL_WARNINGS = 2
CMPL_FAILED = 3
SOLVER_OK = 4
SOLVER_FAILED = 5

CMPLSERVER_OK = 6
CMPLSERVER_ERROR = 7
CMPLSERVER_BUSY = 8
CMPLSERVER_CLEANED = 9 
CMPLSERVER_WARNING = 10

PROBLEM_RUNNING = 11
PROBLEM_FINISHED = 12
PROBLEM_CANCELED = 13 
PROBLEM_NOTRUNNING = 14 

CMPLGRID_SCHEDULER_UNKNOWN = 15
CMPLGRID_SCHEDULER_OK = 16
CMPLGRID_SCHEDULER_ERROR = 17
CMPLGRID_SCHEDULER_BUSY = 18
CMPLGRID_SCHEDULER_SOLVER_NOT_AVAILABLE = 19
CMPLGRID_SCHEDULER_WARNING = 20
CMPLGRID_SCHEDULER_PROBLEM_DELETED = 21

CMPL_GRID  = 22
STANDALONE_SERVER = 23
SERVER_UNKNOWN = 24

CMPL_STATUS_TXT = { CMPL_UNKNOWN    : "CMPL_UNKNOWN" , \
                    CMPL_OK         : "CMPL_OK", \
                    CMPL_WARNINGS   : "CMPL_WARNINGS ", \
                    CMPL_FAILED     : "CMPL_FAILED", \
                    SOLVER_OK       : "SOLVER_OK", \
                    SOLVER_FAILED   : "SOLVER_FAILED", \
                    CMPLSERVER_OK   : "CMPLSERVER_OK", \
                    CMPLSERVER_ERROR: "CMPLSERVER_ERROR", \
                    CMPLSERVER_BUSY : "CMPLSERVER_BUSY",  \
                    CMPLSERVER_CLEANED   : "CMPLSERVER_CLEANED", \
                    CMPLSERVER_WARNING   : "CMPLSERVER_WARNING ", \
                    PROBLEM_RUNNING : "PROBLEM_RUNNING", \
                    PROBLEM_FINISHED  : "PROBLEM_FINISHED", \
                    PROBLEM_CANCELED  : "PROBLEM_CANCELED", \
                    PROBLEM_NOTRUNNING  : "PROBLEM_NOTRUNNING", \
                    CMPLGRID_SCHEDULER_UNKNOWN  : "CMPLGRID_SCHEDULER_UNKNOWN", \
                    CMPLGRID_SCHEDULER_OK  : "CMPLGRID_SCHEDULER_OK", \
                    CMPLGRID_SCHEDULER_ERROR  : "CMPLGRID_SCHEDULER_ERROR", \
                    CMPLGRID_SCHEDULER_BUSY  : "CMPLGRID_SCHEDULER_BUSY", \
                    CMPLGRID_SCHEDULER_SOLVER_NOT_AVAILABLE  : "CMPLGRID_SCHEDULER_SOLVER_NOT_AVAILABLE", \
                    CMPLGRID_SCHEDULER_WARNING  : "CMPLGRID_SCHEDULER_WARNING", \
                    CMPLGRID_SCHEDULER_PROBLEM_DELETED  : "CMPLGRID_SCHEDULER_PROBLEM_DELETED", \
                    CMPL_GRID  : "CMPL_GRID", \
                    STANDALONE_SERVER  : "STANDALONE_SERVER", \
                    SERVER_UNKNOWN  : "SERVER_UNKNOWN"}


#*************** end general definitions  ************************

		

		