#!/usr/bin/python 


from pyCmpl import *


try: 

	model = Cmpl("shortest-path.cmpl")
	
	routes = CmplSet("A",2)
	routes.setValues([ [1,2],[1,4],[2,1],[2,3],[2,4],[2,5],\
				 [3,2],[3,5],[4,1],[4,2],[4,5],[4,6],\
				 [5,2],[5,3],[5,4],[5,6],[5,7],\
				 [6,4],[6,5],[6,7],[7,5],[7,6] ])

	nodes = CmplSet("V")
	nodes.setValues(1,7)
	
	c = CmplParameter("c", routes)
	c.setValues([7,5,7,8,9,7,8,5,5,9,15,6,7,5,15,8,9,6,8,11,9,11])
	
	sNode = CmplParameter("s")
	sNode.setValues(1)
	
	tNode = CmplParameter("t")
	tNode.setValues(7)
	
	model.setSets(routes, nodes)
	model.setParameters(c,sNode,tNode)
	
	model.solve() 
	print "Objective Value: ", model.solution.value
	
	for v in model.solution.variables:
		if v.activity>0:
			print v.name , " " , v.activity
except CmplException, e:
	print e.msg
	
	