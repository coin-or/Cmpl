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
import math

try: 
	cuttingOpt = Cmpl("cut.cmpl")
	patternGen = Cmpl("cut-pattern.cmpl")
	
	#cuttingOpt.setOption("%arg -solver cplex")	
	#patternGen.setOption("%arg -solver cplex")
	
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
	
	ri = CmplParameter("relaxIntegers")
	ri.setValues(1)
			
	cuttingOpt.setSets(w,p)
	cuttingOpt.setParameters(r,o, n, ri )
	
	patternGen.setSets(w)
	patternGen.setParameters(r,pr)
	
	while True:
		cuttingOpt.solve() 
		cuttingOpt.conByName()
		cuttingOpt.varByName()
		cuttingOpt.solutionReport()
		
		for i in w.values:
			pos = w.values.index(i)
			price[pos] = cuttingOpt.fill[i].marginal
	
		pr.setValues(price)
					
		patternGen.solve()
		patternGen.varByName()
			
		if patternGen.reducedCosts.activity < -0.00001:
			nPat = nPat + 1
			p.setValues(1,nPat)
			for i in w.values:
				pos = w.values.index(i)
				nbr[pos].append(patternGen.use[i].activity)
			n.setValues(nbr)
		else:
			break

	ri.setValues(0)
	cuttingOpt.solve() 	
	cuttingOpt.solutionReport()
	cuttingOpt.varByName()
	
	print "Objective value: " , cuttingOpt.solution.value
	for j in p.values:
		print cuttingOpt.cut[j].activity, " pieces of pattern: "
		for i in range(len(w.values)):
			print "\twidth ", w.values[i] , " - " , nbr[i][j-1] 
	
except CmplException, e:
	print e.msg
	
	