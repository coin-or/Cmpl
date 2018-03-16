#***********************************************************************
 #  This code is part of CmplS 
 #
 #  Copyright (C) 2013, 2014
 #  Mike Steglich - Technical University of Applied Sciences
 #  Wildau, Germany 
 #
 #  Cmpl is a project of the Technical University of 
 #  Applied Sciences Wildau and the Institute for Operations Research 
 #  and Business Management at the Martin Luther University 
 #  Halle-Wittenberg.
 #  Please visit the project homepage <www.coliop.org>
 # 
 #  Cmpl is free software; you can redistribute it and/or modify it 
 #  under the terms of the GNU General Public License as published by 
 #  the Free Software Foundation; either version 3 of the License, or 
 #  (at your option) any later version.
 # 
 #  Cmpl is distributed in the hope that it will be useful, but WITHOUT
 #  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 #  or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public 
 #  License for more details.
 #  
 #  You should have received a copy of the GNU General Public License
 #  along with this program; if not, see <http://www.gnu.org/licenses/>.
 #
 #**********************************************************************


# SolverStudio CMPL Processor
# Mike Steglich


language = "CMPL"

import re
import shutil
import sys
import os
import traceback
import cStringIO


# This exception is raised by this SolverStudio runner (i.e. interfacing) code.
class CmplRunnerException(Exception):
    pass

#************* pyCmpl and cmplserver includes ****************
pyCmplPath=SolverStudio.LanguageDirectory()
sys.path.append(pyCmplPath)
from pyCmpl import *

#************* end pyCmpl and cmplserver includes ************


#************* initial values ********************************
Infinity=1e+100
noDual=0
#************* end initial values ****************************


#************* fValue ****************************************
def fValue(val):
    if val==float('Inf'):
        return Infinity
    elif val==float('-Inf'):
        return -Infinity
    else:
        return float(val)
#************* end fValue ************************************

#************* ScanModelFileForDataItems *********************      
def ScanModelFileForDataItems(modelFileName):
    Sets= dict()
    Params = dict()
    dataEntries = list()
    nextEntry = False
    cmplUrl=None
    asyncMode=-1
    isComment=False
  
    if os.path.exists(modelFileName):
        fobj = open(modelFileName,"r")
    else:
        raise Exception ("Cannot find model file " + modelFileName)
    lineCount = 0
    i=0
    for line in fobj:
        lineCount = lineCount + 1

        if not line.lstrip().startswith("#") and CmplTools.strContains(line,"/*"):
            isComment=True

        if isComment and not line.lstrip().startswith("#") and CmplTools.strContains(line,"*/"):
            isComment=False

        if isComment:
            continue

        if line.lstrip().startswith("%arg") and CmplTools.strContains(line,"cmplUrl"):
            pat=re.compile(".*\-cmplUrl\s*((http|HTTP):\/\/.*:[0-9]{4})")
            m=pat.match(line)
            if m!=None:
                cmplUrl=m.group(1)
            else:
                raise CmplRunnerException ( "incorrect cmplUrl <"+line+"> at line "+str(lineCount) )
            asyncMode=0

        if line.lstrip().startswith("%arg") and CmplTools.strContains(line,"send"):
            asyncMode=1

        if line.lstrip().startswith("%arg") and CmplTools.strContains(line,"knock"):
            asyncMode=2

        if line.lstrip().startswith("%arg") and CmplTools.strContains(line,"retrieve"):
            asyncMode=3

        if line.lstrip().startswith("%arg") and CmplTools.strContains(line,"cancel"):
            asyncMode=4
       
        elif line.lstrip().startswith("%data"):
            tmpLine = line.split(":")
      
            if len(tmpLine) == 2:
             
                if tmpLine[0].strip() == "%data":
                    entries = tmpLine[1].split(",")
                    
                    for __e in entries:
                       
                        if nextEntry:
                            dataEntries[i]+=","+__e
                        else:
                            dataEntries.append(__e)
                        
                        if "[" in __e and not "]" in __e:
                            nextEntry=True
                        elif nextEntry and not "]" in __e:
                            nextEntry=True
                        else:
                            nextEntry=False
                            i+=1
                     
                    for __e in dataEntries:
                        
                        if "set" in __e:
                           
                            pat=re.compile("set\[([0-9]*)\]")
                            m=pat.match(__e)
                            if m!=None:
                                rank=m.group(1)
                            else:
                                rank='0'
                            Sets.update({__e.split()[0].strip(): rank} )
                            
                        else:
                           
                            pat=re.compile("(.*)\[(.*)\]")
                            m=pat.match(__e)
                            if m!=None:
                                Params.update({m.group(1).strip() : m.group(2).strip()})
                            else:
                                Params.update({__e.strip() : ""})
                             
                               
    fobj.close()
    
    return Sets, Params, cmplUrl, asyncMode
#************* end ScanModelFileForDataItems ******************   



#********************* main ************************************
def DoRun():
    try: 
        
        cmplFile = SolverStudio.ModelFileName.replace(".txt",".cmpl")
        shutil.move (SolverStudio.ModelFileName, cmplFile)
        
        print "## Scanning model for sets and parameters"
        Sets, Params, cmplUrl, asyncMode = ScanModelFileForDataItems(cmplFile)
        
        __m = Cmpl(cmplFile)
        
        if asyncMode==-1 or asyncMode==0 or asyncMode==1 or asyncMode==3:

            ResultItems=[]
            
            cmplPath=SolverStudio.LanguageDirectory()+"\\bin\\cmpl.exe"
            os.environ.update({'CMPLBINARY':cmplPath })
            
            print "## Creating CmplData file for %s sets and %s parameters"%(len(Sets),len(Params))
            cmplParamList = []
            cmplSetList = []
            
            print "## Creating single CMPL parameters..."
            __pIdx=0
            for (__escapedName,__value) in SolverStudio.SingletonDataItems.iteritems():
                __name = SolverStudio.UnEscapedDataItemName(__escapedName)
                if __name in Params:
                    cmplParamList.append( CmplParameter(__name) )
                    cmplParamList[__pIdx].setValues(__value)
                    __m.setParameter(cmplParamList[__pIdx])
                    __pIdx+=1
                    print "## ... ", __name
                elif __name in Sets:
                    raise CmplRunnerException ("Singleton data item <" + __name+"> cannot be used as a set.")
                else:
                    ResultItems.append(__name)
                                    
    
            print "## Creating CMPL sets..."
            __sIdx=0
            for (__escapedName,__value) in SolverStudio.ListDataItems.iteritems():
                __name = SolverStudio.UnEscapedDataItemName(__escapedName)
                if __name in Sets:
                                         
                    rank=1
                    if type(__value[0]) == tuple or type(__value[0]) == list:
                        rank=len(__value[0])
                        
                        for __e in __value:
                            for __e1 in __e:
                                if type(__e1) == str:
                                    if not CmplTools.strIsValidIndex(__e1):
                                        raise CmplRunnerException ("Index contains non permitted characters <" + __name+"> <" + __e1 + ">")
    
                    else:
                        for __e in __value:
                            if type(__e) == str:
                                if not CmplTools.strIsValidIndex(__e):
                                    raise CmplRunnerException ("Index contains non permitted characters <" + __name+"> <" + __e + ">")
                    
                    cmplSetList.append ( CmplSet(__name, rank) )
                    cmplSetList[__sIdx].setValues(list(__value))
                    
                    __m.setSet(cmplSetList[__sIdx])
                    __sIdx+=1
                    print "# ... ", __name
                    
                elif __name in Params:
                    raise CmplRunnerException ("Data item <" + __name+"> cannot be used as parameter with out a definition set(s)")
                else:
                    raise CmplRunnerException ("Data item <" + __name+"> cannot be used as variable or constraint without a definition set(s)")
                    
    
            print "## Creating CMPL parameter arrays..."
            for (__escapedName,__value) in SolverStudio.DictionaryDataItems.iteritems():
                __name = SolverStudio.UnEscapedDataItemName(__escapedName)
                if __name in Params:
                    tmpSets = Params[__name].split(",")
                                      
                    paramSetList=[]
                    for __e in tmpSets:
                        setFound=False
                        if __e.strip() in Sets:
                            for __s in cmplSetList:
                                if __s.name==__e.strip():
                                    paramSetList.append(__s)
                                    setFound=True
    
                        if not setFound:
                            raise CmplRunnerException ("Cannot find set <" + __e + "> for parameter <" + __name+">") 
            
                    cmplParamList.append ( CmplParameter(__name, paramSetList ) )
                                      
                    for __e in dict(__value).values():
                            
                        if type(__e) == list or type(__e) == tuple:
                            for __e1 in __e:
                                if type(__e1) == str:
                                    if CmplTools.stringContainsWhitespace(__e1):
                                        raise CmplRunnerException ("An index can't contain a whitespace <" + __name+"> <" + __e1 + ">")
                        elif type(__e) == str:
                            if type(__e) == str:
                                if CmplTools.stringContainsWhitespace(__e):
                                    raise CmplRunnerException ("An index can't contain a whitespace <" + __name+"> <" + __e + ">")
      
                    cmplParamList[__pIdx].setValues(dict(__value))
                    
                    __m.setParameters(cmplParamList[__pIdx])
                    __pIdx+=1
                    print "## ... ", __name
                elif __name in Sets:
                    raise CmplRunnerException ("Indexed data item <" + __name +"> cannot be used as set")
                else:
                    ResultItems.append(__name)

        
        
        print "## Running "+language+" ..."
        print "## "+language+" model file:", cmplFile
        
        __m.setOption('%display NonZeros')              
        __m.setOutput(True," > ")
        
        if asyncMode==-1:
            print "## Solving model ..."
            __m.solve()
        elif cmplUrl!=None and asyncMode==0:
            print "## Solving model on CmplServer ..."
            __m.connect(cmplUrl)
            __m.solve()
        elif cmplUrl!=None and asyncMode==1:
            print "## Send model to CmplServer ..."
            __m.setAsyncMode(asyncMode)
            __m.connect(cmplUrl)
            __m.send()
            __m.dump()
            print "#### Problem has been sent to CmplServer"
        elif asyncMode==2:
            print "## Checking status on CmplServer ..."
            __m.load()
            __m.setAsyncMode(asyncMode)
            __m.knock()
            __m.dump()
        elif asyncMode==3:
            print "## Retrieving results from CmplServer ..."
            __m.load()
            __m.setAsyncMode(asyncMode)
            __m.retrieve()
        elif asyncMode==4:
            print "## Canceling model on CmplServer ..."
            __m.load()
            __m.setAsyncMode(asyncMode)
            __m.cancel()
            print "#### Problem canceled by user"

        isInteger=__m.isIntegerProgram

        if asyncMode==-1 or asyncMode==0 or asyncMode==3:
        
            if __m.cmplStatus==CMPL_WARNINGS:
                print "## "+language+ " WARNINGS "
                for msg in __m.cmplMessages:
                    print "##> ",msg.type, msg.file, msg.line, msg.description
                                    
            if __m.solverStatus == SOLVER_OK:
                   
                print "## "+language+" run completed."
                
                itemsLoaded = cStringIO.StringIO()
                          
                for __name in ResultItems:
                  
                    if __name=="model.objName":
                        globals()[SolverStudio.EscapedDataItemName(__name)] = __m.objectiveName
                    elif __name=="model.objSense":
                        globals()[SolverStudio.EscapedDataItemName(__name)] = __m.objectiveSense
                    elif __name=="model.objValue":
                        globals()[SolverStudio.EscapedDataItemName(__name)] = __m.solution.value
                    elif __name=="model.objStatus":
                        globals()[SolverStudio.EscapedDataItemName(__name)] = __m.solution.status
                    elif __name=="model.nrOfVars":
                        globals()[SolverStudio.EscapedDataItemName(__name)] = __m.nrOfVariables
                    elif __name=="model.nrOfCons":
                        globals()[SolverStudio.EscapedDataItemName(__name)] = __m.nrOfConstraints
                    elif __name=="model.solverName":
                        globals()[SolverStudio.EscapedDataItemName(__name)] = __m.solver
                    elif __name=="model.solverMsg":
                        globals()[SolverStudio.EscapedDataItemName(__name)] = __m.solverMessage
                    else:
    
                        entryType = 0 # 1 activity, 2 lowerBound, 3 upperbound, 4 marginal
                        entryName = ""
                        isFound=False
                        
                        if CmplTools.strContains(__name.lower() ,".lowerbound"):
                            entryType=2
                            entryName=__name[0:__name.lower().find(".lowerbound")]
                        elif CmplTools.strContains(__name.lower() ,".upperbound"):
                            entryType=3
                            entryName=__name[0:__name.lower().find(".upperbound")]
                        elif CmplTools.strContains(__name.lower() ,".dual"):
                            entryType=4
                            entryName=__name[0:__name.lower().find(".dual")]
                        else:
                            entryType=1
                            entryName=__name
                            
                    
                        try:
                            solEntry=__m.getVarByName(entryName)
                            isFound=True
                        except:
                            try:
                                solEntry=__m.getConByName(entryName)
                                isFound=True
                            except :
                                print "## Cannot find data for <",entryName,">"   
                
                        if isFound:
                                
                            dataItem=SolverStudio.DataItems[ SolverStudio.EscapedDataItemName(__name) ]
                            t=str(type(dataItem))
                            
                            itemType=0 # 1 singleton, 2 dict
                            if CmplTools.strContains(t,"float"):
                                itemType=1
                            elif CmplTools.strContains(t,"Dict"):
                                itemType=2
                            else:
                                itemType=-1
                                
                
                            if itemType == 1: #singleton
                                globals()[SolverStudio.EscapedDataItemName(__name)] = 0
                                if entryType==1:
                                    globals()[SolverStudio.EscapedDataItemName(__name)] = fValue(solEntry.activity)
                                elif entryType==2:
                                    globals()[SolverStudio.EscapedDataItemName(__name)] = fValue(solEntry.lowerBound)
                                elif entryType==3:
                                    globals()[SolverStudio.EscapedDataItemName(__name)] = fValue(solEntry.upperBound)
                                elif entryType==4:
                                    if isInteger:
                                        globals()[SolverStudio.EscapedDataItemName(__name)] = noDual
                                    else:
                                        globals()[SolverStudio.EscapedDataItemName(__name)] = fValue(solEntry.marginal)
                                     
                            elif itemType == 2: #dict
                                for __e in globals()[SolverStudio.EscapedDataItemName(__name)]:
                                    globals()[SolverStudio.EscapedDataItemName(__name)][__e] = 0
                                    
                                for __x in solEntry:
                                        
                                    pat=re.compile("(.*)\[(.*)\]")
                                    mp=pat.match(__x.name)
                                    
                                    if mp!=None:
                                        vIndices=mp.group(2).strip().split(",")
                                        vTuple=cStringIO.StringIO()
                                        vTuple.write("(")
                                        i=0
                                        for __idx in vIndices:
                                            if i>0:
                                                vTuple.write(",")
                                            if CmplTools.strIsNumber(__idx):
                                                vTuple.write(str(float(__idx)))
                                            else:
                                                vTuple.write("\""+__idx.strip()+"\"")
                                            i+=1
                                        vTuple.write(")")
                                        
                                        if entryType==1:
                                            globals()[SolverStudio.EscapedDataItemName(__name)][eval(vTuple.getvalue())] = fValue(__x.activity) 
                                        elif entryType==2:
                                            globals()[SolverStudio.EscapedDataItemName(__name)][eval(vTuple.getvalue())] = fValue(__x.lowerBound)
                                        elif entryType==3:
                                            globals()[SolverStudio.EscapedDataItemName(__name)][eval(vTuple.getvalue())] = fValue(__x.upperBound)
                                        elif entryType==4:
                                            if isInteger:
                                                globals()[SolverStudio.EscapedDataItemName(__name)][eval(vTuple.getvalue())] = noDual
                                            else:
                                                globals()[SolverStudio.EscapedDataItemName(__name)][eval(vTuple.getvalue())] = fValue(__x.marginal)
                                
                                        vTuple.close() 
                                         
                                    else:
                                        raise CmplRunnerException ("Cannot find indices for variable or constraint <" + __name +">")  
                            if len(itemsLoaded.getvalue())>0:
                                itemsLoaded.write(", ")
                            itemsLoaded.write(__name)            
              
      
                if itemsLoaded.getvalue()=="":
                    print "## No results were loaded into the sheet."
                else :
                    print "## Results loaded for data items <", itemsLoaded.getvalue() ,"> and changed in sheet"
            
                itemsLoaded.close()
            else:
                print "## Solver failed " , __m.solver , " " , __m.solverMessage
                print "## "+language+" did not complete; no solution is available. The sheet has not been changed."
        
            cmplParamList = []
            cmplSetList = []
            paramSetList=[]
            
            __m.saveSolutionAscii()
            __m.saveDataFile()
  
                
    except CmplRunnerException as e:
        # Report an exception detected in this DoRun() code
        print "## "+language+ " ERRORS"
        print "## > ", e
        #traceback.print_exc(file=sys.stdout)
        print "## "+language+" did not complete; no solution is available. The sheet has not been changed."
    except CmplException as e:
                if e.msg.startswith("pyCmpl error: Model is still running, cannot retrieve the solution"):
                        print e.msg
                        if model.cmplStatus!=PROBLEM_FINISHED and model.cmplStatus!=PROBLEM_CANCELED:
                                model.dump()
                else:
                        # Report an exception detected by the CMLP parser (which includes a line number).
                        print "## "+language+ " ERRORS"
                        print "## > ", e.msg
                        print "## "+language+" did not complete; no solution is available. The sheet has not been changed."
    except Exception as e:
        # Report an exception caused by our own code; the user should never see this.
        print "## ERRORS"
        print "## > ", e
        traceback.print_exc(file=sys.stdout)
        print "## "+language+" did not complete; no solution is available. The sheet has not been changed."
        raise e; # Throw it to report the line number in our underlying code
    print "## Done"

#************* end main ***************************************************

DoRun()
