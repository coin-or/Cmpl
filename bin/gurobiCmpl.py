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
import traceback



#*************** Gurobi related stuff *************************************
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
    print('Cant find Gurobi')
    quit(-1)

from gurobipy import *
#*************** End Gurobi related stuff *************************************




#*************** SolElement *************************************
class SolElement(object):
        #*********** constructor **********
        def __init__(self):
                self.__name = ""
                self.__activity = 0
                self.__marginal = 0
                self.__hasMarginal = False
        #*********** end constructor ******

        # getter and setter
        @property
        def name(self):
                return self.__name

        def setName(self, name):
                self.__name = name

        @property
        def activity(self):
                return self.__activity

        def setActivity(self, activity):
                self.__activity=activity

        @property
        def marginal(self):
                return self.__marginal

        def setMarginal(self, marginal):
                self.__marginal=marginal
                self.__hasMarginal= True
        @property
        def hasMarginal(self):
            return self.__hasMarginal

        # end getter and setter
#*************** end SolElement ******************************

#*************** CmplSolution  ***********************************
class GurobiSolution(object):

        #*********** constructor **********
        def __init__(self):
                self.__status = ""
                self.__value = 0
                self.__variables =[]
                self.__constraints = []
        #*********** end constructor ******

        # getter and setter
        @property
        def status(self):
                return self.__status

        def setStatus(self, status):
                self.__status=status

        @property
        def value(self):
                return self.__value

        def setValue(self, val):
                self.__value=val

        @property
        def variables(self):
                return self.__variables

        def setVar(self, var):
                self.__variables.append(var)

        @property
        def constraints(self):
                return self.__constraints

        def setCon(self, con):
                self.__constraints.append(con)

        # end getter and setter

#*************** end Solution ********************************



#*********************** write solution *****************************
def writeSol(solFile, solutions):

    f = open(solFile, 'w')
    f.write('<?xml version = "1.0" encoding="UTF-8" standalone="yes"?>\n')
    f.write('<CmplGurobiSolutions version = "2.0">\n')


    idx=1
    for sol in solutions:

        f.write('	<solution idx="%g">\n' % idx)
        f.write('		<header')
        f.write(' value="%g"' % sol.value)
        f.write(' status="%s" />\n' % sol.status)

        f.write('		<variables>\n')
        for v in sol.variables:
            f.write('			<variable name="%s" activity="%e"' % (v.name, v.activity))
            if v.hasMarginal:
                f.write(' marginal="%e" />\n'% (v.marginal))
            else:
                f.write(' />\n')

        f.write('		</variables>\n')
        f.write('		<constraints>\n')

        for c in sol.constraints:
            f.write('			<constraint name="%s" activity="%e"' % (c.name, c.activity))
            if c.hasMarginal:
                f.write(' marginal="%e" />\n'% (c.marginal))
            else:
                f.write(' />\n')

        f.write('		</constraints>\n')
        f.write('	</solution>\n')

        idx+=1

    f.write('</CmplGurobiSolutions>')

    f.close()
#*********************** end write solution *****************************
		



#*************** main ****************************************
def main():
    i = 0
    #print sys.argv

    modFile=""
    for arg in sys.argv:
            print( "%i %s\n" % (i, arg) )

            if i == 1:
                    solutionPool = arg
            if i == 2:
                    modFile = cleanBlanks(arg)
                    model = read(modFile)
            if i == 3:
                    solFile = cleanBlanks(arg)
            if i > 3:
                    try:
                        s = 'model.params.%s' % arg
                        exec(s)
                    except:
                        print("wrong gurobi parameter %s" % arg)
            i += 1


    nrOfSolutions = 0

    try:

        #model = read(modFile)
        model.optimize()

        solutions=[]

        sol=GurobiSolution()

        status = model.Status
        if status == GRB.Status.INF_OR_UNBD or \
           status == GRB.Status.INFEASIBLE  or \
           status == GRB.Status.UNBOUNDED:
            nrOfSolutions=0

        else:
            if solutionPool=="0":
                nrOfSolutions=1
            else:
                model.Params.outputFlag = 0
                nrOfSolutions = model.SolCount

            sol.setValue(model.objVal)
            if model.isMIP == 1:
                sol.setStatus('integer_optimal_solution')
            else:
                sol.setStatus('optimal_solution')

            for v in model.getVars():
                sElem=SolElement()
                sElem.setName(v.VarName)
                sElem.setActivity(v.x)
                if model.isMIP != 1:
                    sElem.setMarginal(v.RC)
                sol.setVar(sElem)

            for c in model.getConstrs():
                sElem=SolElement()
                sElem.setName(c.ConstrName)
                sElem.setActivity(c.RHS-c.Slack)
                if model.isMIP != 1:
                    sElem.setMarginal(c.Pi)
                sol.setCon(sElem)

            solutions.append(sol)

            oldObj=sol.value

            if solutionPool=="1" and nrOfSolutions>1:
                for e in range(nrOfSolutions):

                    model.setParam(GRB.Param.SolutionNumber, e)

                    if model.PoolObjVal!=oldObj:
                        oldObj=model.PoolObjVal

                        sol=GurobiSolution()

                        sol.setValue(model.PoolObjVal)
                        sol.setStatus('integer_feasible_solution')

                        for v in model.getVars():
                            sElem=SolElement()
                            sElem.setName(v.VarName)
                            sElem.setActivity(v.Xn)
                            sol.setVar(sElem)

                        solutions.append(sol)

        print('Writing solution')
        writeSol(solFile, solutions)
        model.Params.outputFlag = 1

        print('...done')

    except:
        traceback.print_exc(file=sys.stdout)
        print(sys.exc_info()[1])

#*************** end main ****************************************


if __name__ == "__main__":
    main()
