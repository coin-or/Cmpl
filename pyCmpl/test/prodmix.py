#!/usr/bin/python 

from pyCmpl import *

try: 
	m = Cmpl("prodmix.cmpl")
	
	products = CmplSet("products")
	products.setValues(1,3)
	
	machines = CmplSet("machines")
	machines.setValues(1,2)
	
	c = CmplParameter("c",products)
	c.setValues([75,80,50])
	
	xmax = CmplParameter("xMax",products)
	xmax.setValues([250,240,250])
	
	b = CmplParameter("b",machines)
	b.setValues([1000,1000])
	
	a = CmplParameter("a",machines, products)
	a.setValues([[ 8,15,12],[15,10,8]])
	
	m.setSets(products,machines)
	m.setParameters(c,a,b,xmax)
	
	m.solve() 
	m.solutionReport()
	m.saveSolution()
	m.saveSolutionAscii()
	m.saveSolutionCsv()
	
except CmplException, e:
	print e.msg
	
	