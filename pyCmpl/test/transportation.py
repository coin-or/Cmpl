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
	model = Cmpl("transportation.cmpl")
	
	routes = CmplSet("routes",2)
	routes.setValues([[1,1],[1,2],[1,4],[2,2],[2,3],[2,4],[3,1],[3,3]])

	plants = CmplSet("plants")
	plants.setValues(1,3)
	
	centers = CmplSet("centers")
	centers.setValues(1,4)
	
	costs = CmplParameter("c",routes)
	costs.setValues([3,2,6,5,2,3,2,4])
	
	s = CmplParameter("s",plants)
	s.setValues([5000,6000,2500])
	
	d = CmplParameter("d",centers)
	d.setValues([6000,4000,2000,2500])
	
	model.setSets(routes, plants, centers)
	model.setParameters(costs,s,d)
		
	model.setOption("%display nonZeros")
	model.solve() 
			
	if model.solverStatus == SOLVER_OK:
		model.solutionReport()
	else:
		print "Solver failed " + model.solver + " " + model.solverMessage
	
except CmplException, e:
	print e.msg
	
	
