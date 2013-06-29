#!/usr/bin/python 

from pyCmpl import *
import math

try: 
	cuttingOpt = Cmpl("cut.cmpl")
	patternGen = Cmpl("cut-pattern.cmpl")
	
	cuttingOpt.setOption("%arg -solver cplex")	
	patternGen.setOption("%arg -solver cplex")
	
	r = CmplParameter("rollWidth")
	r.setValues(110)
	
	w = CmplSet("widths")
	w.setValues([ 20, 45, 50, 55, 75])
	
	o = CmplParameter("orders",w)
	o.setValues([ 48, 35, 24, 10, 8 ])
	
	nPat=w.len
	p = CmplSet("patterns")
	p.setValues(1,nPat)
	
	nbr = []
	for i in range(nPat):
		nbr.append( [ 0 for j in range(nPat) ] )
	
	for i in w.values:
		pos = w.values.index(i)
		nbr[pos][pos] = int(math.floor( r.value / i ))
	
	n = CmplParameter("nbr", w, p)
	n.setValues(nbr)
	
	price = [] 
	for i in range(w.len):
		price.append(0)
	
	pr = CmplParameter("price", w)
	pr.setValues(price)
	
	cuttingOpt.setSets(w,p)
	cuttingOpt.setParameters(r, o, n)
	
	patternGen.setSets(w)
	patternGen.setParameters(r,pr)

	ri = cuttingOpt.setOption("%arg -integerRelaxation")	
	
	while True:
		cuttingOpt.solve() 
		cuttingOpt.conByName()
		
		for i in w.values:
			pos = w.values.index(i)
			price[pos] = cuttingOpt.fill[i].marginal
	
		pr.setValues(price)
					
		patternGen.solve()
		patternGen.varByName()
			
		if (1-patternGen.solution.value) < -0.00001:
			nPat = nPat + 1
			p.setValues(1,nPat)
			for i in w.values:
				pos = w.values.index(i)
				nbr[pos].append(patternGen.use[i].activity)
			n.setValues(nbr)
		else:
			break
			
	cuttingOpt.delOption(ri)
	
	cuttingOpt.solve() 	
	cuttingOpt.varByName()
	
	print "Objective value: " , cuttingOpt.solution.value
	for j in p.values:
		if cuttingOpt.cut[j].activity>0:
			print cuttingOpt.cut[j].activity, " pieces of pattern: "
			for i in range(len(w.values)):
				print "\twidth ", w.values[i] , " - " , nbr[i][j-1] 
	
except CmplException, e:
	print e.msg
	
	