#!/usr/bin/python 


from __future__ import division


from pyCmpl import *
import random


try: 
	
	routes = CmplSet("A",2)
	routes.setValues([ [1,2],[1,4],[2,1],[2,3],[2,4],[2,5],\
				 [3,2],[3,5],[4,1],[4,2],[4,5],[4,6],\
				 [5,2],[5,3],[5,4],[5,6],[5,7],\
				 [6,4],[6,5],[6,7],[7,5],[7,6] ])

	nodes = CmplSet("V")
	nodes.setValues(1,7)
	
	cList = [7,5,7,8,9,7,8,5,5,9,15,6,7,5,15,8,9,6,8,11,9,11]
	
	sNode = CmplParameter("s")
	sNode.setValues(1)
	
	tNode = CmplParameter("t")
	tNode.setValues(7)
	
	models= []
	randC = []
	for i in range(99):
		models.append(Cmpl("shortest-path.cmpl"))
		models[i].setSets(routes, nodes)
		
		tmpC =[]
		for m in cList:
			tmpC.append( m + random.randint(-40,40)/10)
			
		randC.append(CmplParameter("c", routes))
		randC[i].setValues(tmpC)	
			
		models[i].setParameters(randC[i],sNode,tNode)
			
	for m in models:
		m.start()
	
			
	for m in models:
		m.join()
	
	i = 0
	for m in models:
		print "problem : " , i , " needed time " , m.solution.value
		
		for v in m.solution.variables:
			if v.activity>0:
				print v.name , " " , v.activity
		i = i + 1
	
	
except CmplException, e:
	print e.msg
except:
    print "Unexpected error:", sys.exc_info()[0]
    raise
	
	
