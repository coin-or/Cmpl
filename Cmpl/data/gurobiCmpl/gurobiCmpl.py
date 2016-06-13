#***********************************************************************
 #  This code is part of CMPL
 #
 #  Copyright (C) 2007, 2008, 2009, 2010, 2011
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

import sys

def module_exists(module_name):
    try:
        __import__(module_name)
    except ImportError:
        return False
    else:
        return True
        
def cleanBlanks(str):
	return str.replace('%', ' ').strip()
	
if not module_exists('gurobipy'):
    print 'Cant find Gurobi'
    quit(-1)

from gurobipy import *

i = 1    
for arg in sys.argv:
	#print "%i %s\n" % (i, arg)
	if i == 2:
		os.chdir(cleanBlanks(arg))
	if i == 3:
		solutionPool = arg
	if i == 4:
		model = read(cleanBlanks(arg))
	if i == 5: 
		solFile = cleanBlanks(arg)
	if i > 5:
		s = 'model.params.%s' % arg
		exec(s)
	i += 1
	
nrOfSolutions = 0
	
def writeSol():

	f.write('	<solution>\n')
	f.write('		<header')
	
	s = ' idx="%g"' %  model.SolCount
	if model.status == GRB.status.OPTIMAL:
		s = s + ' value="%g"' % model.objVal
		if model.isMIP == 1:
			s = s + ' status="integer optimal solution"/>\n'
		else:
			s = s + ' status="optimal solution"/>\n'
	else: 	
		s = s + ' value="0"' 
		s = s + ' status="Infeasible or unbounded model"/>\n'

	f.write(s)
	
	if model.status == GRB.status.OPTIMAL:
		f.write('		<variables>\n')
		
		i=0
		for v in model.getVars():
			if model.isMIP == 1:
				s = '			<variable idx="%g" activity="%e"/>\n' % (i,v.x)
			else:
				s = '			<variable idx="%g" activity="%e" marginal="%e"/>\n' % (i,v.x, v.RC)
			f.write(s)	
			i=i+1
		
		f.write('		</variables>\n')
		f.write('		<constraints>\n')
		
		i=0
		for c in model.getConstrs():
			if model.isMIP == 1:
				s = '			<constraint idx="%g" activity="%e"/>\n' % (i,c.RHS-c.Slack)
			else:
				s = '			<constraint idx="%g" activity="%e" marginal="%e"/>\n' % (i,c.RHS-c.Slack, c.Pi)
			f.write(s)
			i=i+1
		
		f.write('		</constraints>\n')
	f.write('	</solution>\n')

		
	
def mycallback(model, where):
	if solutionPool == "1":
		if where == GRB.callback.MIPSOL:
			f.write('	<solution>\n')
			f.write('		<header')
			s = ' idx="%g"' %  int(model.cbGet(GRB.callback.MIPSOL_SOLCNT))
			s = s + ' value="%g"' % model.cbGet(GRB.callback.MIPSOL_OBJ)
			s = s + ' status="integer feasible solution"/>\n' 
			f.write(s)
			f.write('		<variables>\n')
			#print model.cbGetSolution(model.getVars())
			vList = model.cbGetSolution(model.getVars())
			i=0
			for v in vList:
				s = '			<variable idx="%g" activity="%e"/>\n' % (i,v)
				f.write(s)	
				i=i+1
			f.write('		</variables>\n')
			f.write('	</solution>\n')


f = open(solFile, 'w')
f.write('<?xml version = "1.0" encoding="UTF-8" standalone="yes"?>\n')
f.write('<CmplGurobiSolutions>\n')

model.optimize(mycallback)

print 'Write solution'

nrOfSolutions = nrOfSolutions + 1
writeSol()

f.write('</CmplGurobiSolutions>')

f.close()
print '...done'