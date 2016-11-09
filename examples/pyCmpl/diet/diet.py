#!/usr/bin/python 

from pyCmpl import *

try: 
	model = Cmpl("diet.cmpl")
	
	nutr = CmplSet("NUTR")
	nutr.setValues(["A", "B1", "B2", "C"])
	
	food = CmplSet("FOOD")
	food.setValues(["BEEF","CHK","FISH","HAM","MCH","MTL","SPG","TUR"])
	
	costs = CmplParameter("costs",food)
	costs.setValues([3.19, 2.59, 2.29, 2.89, 1.89, 1.99, 1.99, 2.49])
	
	vitmin = CmplParameter("vitMin",nutr)
	vitmin.setValues([ 700, 700, 700, 700])
	
	vitamin = CmplParameter("vitamin",nutr, food)
	vitamin.setValues ( [ [60,8,8,40,15,70,25,60] , [20,0,10,40,35,30,50,20] , \
						[10,20,15,35,15,15,25,15] ,[15,20,10,10,15,15,15,10]])
	
	model.setSets(nutr,food)
	model.setParameters(costs,vitmin,vitamin)
	
	model.solve() 
	model.solutionReport()
	
except CmplException, e:
	print e.msg
	
	