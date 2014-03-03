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
	prodmix = Cmpl("production-mix.cmpl")
	prodmixSub = Cmpl("production-mix-substitute.cmpl")
	
	products = CmplSet("products")
	products.setValues(1,5)
	
	machines = CmplSet("machines")
	machines.setValues(1,4)
	
	c = CmplParameter("c",products)
	c.setValues([180,540,300,410,620])
	
	a = CmplParameter("a",machines, products)
	a.setValues( [[8,64,40,46,63],[20,40,35,50,70],[30,20,56,12,112],[25,32,28,4,48]])
	
	b = CmplParameter("b", machines)
	b.setValues([20000,25000,10000,15000])
	
	xMax = CmplParameter("xMax", products)
	xMax.setValues([300,160, 1e10, 200, 1e10])
	
	maxDeltaA = CmplParameter("maxDeltaA", machines, products)
	
	prodmix.setSets(machines,products)
	prodmix.setParameters(c,a,b,xMax)
	
	prodmix.solve()
	
	deltaAUpperBounds = [[2,15,0,0,0], [10,10,10,10,10], [10,8,0,0,0],[10,10,10,10,10]]

	maxDeltaAMatrix =[]
	restrictedMachines=[]
	i = 0
	for r in prodmix.solution.constraints:
		if r.marginal>0:
			maxDeltaAMatrix.append(deltaAUpperBounds[i])
		else:
			maxDeltaAMatrix.append([ 0 for j in range(products.len) ] )
		i = i+1
	maxDeltaA.setValues(maxDeltaAMatrix)
			
	prodmixSub.setSets(machines,products)
	prodmixSub.setParameters(c,a,b,xMax,maxDeltaA )
	
	prodmixSub.solve()
	prodmixSub.varByName()
	
	newA = [[0,0,0,0,0], [0,0,0,0,0], [0,0,0,0,0],[0,0,0,0,0]]
	for i in machines.values:
		for j in products.values:
			if prodmixSub.w[i].activity != 0:
				newA[i-1][j-1] = a.values[i-1][j-1]-deltaAUpperBounds[i-1][j-1]* \
								(prodmixSub.w[i].activity/(prodmixSub.s[i].activity+prodmixSub.w[i].activity))
			else:
				newA[i-1][j-1] = a.values[i-1][j-1]
		
	print "%-10s%20s%20s" % ("","original model", "modified model")
	print "%-10s" % ("Variables")
	for v in prodmix.solution.variables:
		i = prodmix.solution.variables.index(v)
		print "%10s%18.2f%18.2f" % (v.name, v.activity, prodmixSub.solution.variables[i].activity )
	
	print "%-10s" % ("Constraints")
	for r in prodmix.solution.constraints:
		i = prodmix.solution.constraints.index(r)
		print "%10s%18.2f%18.2f" % (r.name, r.activity, prodmixSub.solution.constraints[i].activity )
	
	print "\n\n%20s" % ("Original coefficient matrix")
	for i in machines.values:
		row = "%10s" % prodmix.solution.constraints[i-1].name
		for j in products.values:
			row = row + "%8.2f" % a.values[i-1][j-1]
		print row 
	
	print "\n%20s" % ("Modified coefficient matrix")	
	for i in machines.values:
		row = "%10s" % prodmix.solution.constraints[i-1].name
		for j in products.values:
			row = row + "%8.2f" % newA[i-1][j-1]
		print row 
		
	
except CmplException, e:
	print e.msg
	
	