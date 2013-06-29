#***********************************************************************
 #  This code is part of CMPL
 #
 #  Copyright (C) 2007, 2008, 2009, 2010, 2011, 2012, 2013
 #  Thomas Schleiff - Halle(Saale), Germany and
 #  Mike Steglich - Technical University of Applied Sciences
 #  Wildau, Germany 
 #
 #  CMPL is a project of the Technical University of 
 #  Applied Sciences Wildau and the Institute for Operations Research 
 #  and Business Management at the Martin Luther University 
 #  Halle-Wittenberg.
 #  Please visit the project homepage <www.coliop.org>
 # 
 #  CMPL is free software; you can redistribute it and/or modify it 
 #  under the terms of the GNU General Public License as published by 
 #  the Free Software Foundation; either version 3 of the License, or 
 #  (at your option) any later version.
 # 
 #  CMPL is distributed in the hope that it will be useful, but WITHOUT
 #  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 #  or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public 
 #  License for more details.
 #  
 #  You should have received a copy of the GNU General Public License
 #  along with this program; if not, see <http://www.gnu.org/licenses/>.
 #
 #**********************************************************************

#!/usr/bin/python 

from pyCmpl import *

try: 
	m = Cmpl("assignment.cmpl")
	
	locations = CmplSet("locations")
	locations.setValues(1,4)
	
	machines = CmplSet("machines")
	machines.setValues(1,3)
	
	combinations = CmplSet("combinations", 2)
	combinations.setValues([ [1,1],[1,2],[1,3],[1,4], [2,1],[2,3],[2,4], [3,1],[3,2],[3,3],[3,4]]);
		
	c = CmplParameter("c",combinations)
	c.setValues([13,16,12,11,15,13,20,5,7,10,6])
	
	m.setSets(machines,locations,combinations)
	m.setParameters(c)
	m.setOption("%display nonZeros")
	
	m.solve()

	print "Objective value: " , m.solution.value
 	print "Objective status: " , m.solution.status
	
	for v in m.solution.variables:
		print v.name, " " , v.activity,  " " , v.lowerBound, " " ,v.upperBound  
	
	for c in m.solution.constraints:
		print c.name, " " , c.activity,  " " , c.lowerBound, " " ,c.upperBound
	
except CmplException, e:
	print e.msg
	
	