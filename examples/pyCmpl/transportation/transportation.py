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
		
	model.setOutput(True)
	model.setOption("%display nonZeros")
	model.solve() 
			
	if model.solverStatus == SOLVER_OK:
		model.solutionReport()
	else:
		print "Solver failed " + model.solver + " " + model.solverMessage
	
except CmplException, e:
	print e.msg
	
	
