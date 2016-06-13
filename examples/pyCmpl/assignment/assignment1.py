#!/usr/bin/python 

from pyCmpl import *

try: 
	m = Cmpl("assignment.cmpl")
	
	locations = CmplSet("locations")
	locations.setValues(1,4)
	
	machines = CmplSet("machines")
	machines.setValues(1,3)
	
	combinations = CmplSet("A", 2)
	combinations.setValues([ [1,1],[1,2],[1,3],[1,4], [2,1],[2,3],[2,4], [3,1],[3,2],[3,3],[3,4]])
		
	c = CmplParameter("c",combinations)
	c.setValues([13,16,12,11,15,13,20,5,7,10,6])
	
	m.setSets(machines,locations,combinations)
	m.setParameters(c)
	
	m.solve()

	print "Objective value: " , m.solution.value
 	print "Objective status: " , m.solution.status
 	
 	print "Variables:"  		
	for v in m.solution.variables:
		print "%10s %3s %8i %8i %8i" % (v.name, v.type, v.activity,v.lowerBound,v.upperBound  )
	
	print "Constraints:" 
	for c in m.solution.constraints:
		print "%10s %3s %8.0f %8.0f %8.0f" % (c.name, c.type, c.activity,c.lowerBound,c.upperBound)
except CmplException, e:
	print e.msg
	
	