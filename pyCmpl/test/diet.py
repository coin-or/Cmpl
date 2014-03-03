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
	
	model.saveSolution()
	model.saveSolutionAscii()
	model.saveSolutionCsv()
	
except CmplException, e:
	print e.msg
	
	