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
	
	m.setOption("%arg -solver cplex")
	m.setOption("%display solutionPool")

	m.solve()

	for s in m.solutionPool:
	
		m.varByName(s.idx)
		m.conByName(s.idx)
	
		print "\nSolution number: " , s.idx
		print "Objective value: " , s.value
		print "Objective status: " , s.status
			
		print "Variables:"  
		for c in combinations.values:
			print "%10s %3s %8i %8i %8i" % (m.x[c].name, m.x[c].type, m.x[c].activity, m.x[c].lowerBound, m.x[c].upperBound )
	
		print "Constraints:" 
		for i in m.sos_m:
			print "%10s %3s %8.0f %8.0f %8.0f" % (m.sos_m[i].name, m.sos_m[i].type, m.sos_m[i].activity, m.sos_m[i].lowerBound, m.sos_m[i].upperBound)

		for j in m.sos_l:
			print "%10s %3s %8.0f %8.0f %8.0f" % (m.sos_l[j].name, m.sos_l[j].type, m.sos_l[j].activity, m.sos_l[j].lowerBound, m.sos_l[j].upperBound)
			
except CmplException, e:
	print e.msg
	
	