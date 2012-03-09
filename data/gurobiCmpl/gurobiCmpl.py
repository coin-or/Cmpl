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
        
def cleanValue(number):
	zeroBound= 1e-9
	if number < zeroBound and number > -zeroBound:
		return 0
	else:
		return number

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
		model = read(cleanBlanks(arg))
	if i == 4: 
		solFile = cleanBlanks(arg)
	if i > 4:
		s = 'model.params.%s' % arg
		exec(s)
	i += 1

model.optimize()

print 'Write solution'

f = open(solFile, 'w')
s = '%g %g\n' % (model.NumConstrs+1, model.NumVars)
f.write(s)

if model.status == GRB.status.OPTIMAL:
	if model.isMIP == 1:
		m = 5
	elif model.isMIP == 0:
		m = 2
	print 'Optimal objective:', model.objVal
elif model.status == GRB.status.INF_OR_UNBD:	
	m = 3
	print 'Model is infeasible or unbounded'
elif model.status == GRB.status.INFEASIBLE:
	m = 3
	print 'Model is infeasible'
elif model.status == GRB.status.UNBOUNDED:
	m = 3
	print 'Model is unbounded'
else:
	print 'Optimization ended with status', model.status
    #exit(-1)
    
if model.isMIP == 1:
	if m == 5: 
		s = '%g %e\n' % (m, cleanValue(model.objVal))
	else:
		s = '3 0\n'
elif model.isMIP == 0:	
	if m == 2:
		s = '%g %g %e\n' % (m, m, cleanValue(model.objVal))
	else:
		s = '3 3 0\n'
f.write(s)

if model.isMIP == 1:
	if m == 5: 
		s = '%e\n' % (cleanValue(model.objVal))
	else:
		s= '0\n'
elif model.isMIP == 0:
	if m == 2:
		s = '0 %e 0\n' % (cleanValue(model.objVal))
	else:
		s = '0 0\n'
f.write(s)

for c in model.getConstrs():
	if model.isMIP == 1:
		if m == 5: 
			s = '%e\n' % (cleanValue(c.RHS-c.Slack))
		else:
			s = '0\n'
	elif model.isMIP == 0:
		if m == 2:
			s = '0 %e %e\n' % (cleanValue(c.RHS-c.Slack) , c.Pi)
		else:
			s = '0 0 0\n'
	f.write(s)

for v in model.getVars():
	if model.isMIP == 1:
		if m == 5: 
			s = '%e\n' % (cleanValue(v.x))
		else:
			s = '0\n'
	elif model.isMIP == 0:
		if m == 2:
			s = '0 %e %e\n' % (cleanValue(v.x), cleanValue(v.RC))
		else:
			s = '0 0 0\n'
	f.write(s)

f.close()
print '...done'