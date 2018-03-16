#!/usr/bin/python 

#Cmpl 1.12.0
#March 2018
#Stegger


import sys
import os
import pickle
import tempfile
import traceback

from pyCmpl import *

i = 0 
cmplFile=""
cmplUrl=""
maxTries=0
maxQueuingTime=0
optionsList = {}
tmpStr=""
fileFlag=False
isMessageFlag=False
messageFile=""
isError=False
asyncMode=0
tmpPath = tempfile.gettempdir()+os.sep

for arg in sys.argv:
	
	if arg.strip()=="":
		continue
		
	if i == 1:
		cmplFile = arg
	if i == 2:
		cmplUrl = arg
	if i == 3:
		if CmplTools.strIsNumber(arg):
			maxTries = int(arg)
	if i == 4:
		if CmplTools.strIsNumber(arg):
			maxQueuingTime = int(arg)
			
	if i == 5:
		if CmplTools.strIsNumber(arg):
			asyncMode = int(arg)
	
	if i > 5:
		if arg.strip().startswith("-") :
			
			if arg.strip().startswith("-e") :
				isMessageFlag=True
			else:
				isMessageFlag=False
			
			if not isMessageFlag:	
				pos = len(optionsList)
				if tmpStr!="":
					if fileFlag:
						tmpStr=tmpStr + "\""
						fileFlag=False
					optionsList.update({pos:tmpStr})
					
				tmpStr = "%arg " +arg + " "	
				if arg.strip().startswith("-solution") or arg.strip().startswith("-matrix") or arg.strip().startswith("-l") or arg.strip().startswith("-s") or arg.strip().startswith("-e")  :
					tmpStr = tmpStr + "\""
					fileFlag=True
		else:
			if not isMessageFlag:	
				if tmpStr.endswith("\""):
					tmpStr = tmpStr + arg 
				else:
					tmpStr = tmpStr + " " +arg
			else:
				#messageFile="\""+arg+"\""
				messageFile=arg
				
	i+=1
	
if tmpStr!="" :
	if fileFlag:
		tmpStr=tmpStr + "\""
	pos = len(optionsList)
	optionsList.update({pos:tmpStr})	
	

try:
	
	if asyncMode == 0:

                model = Cmpl(cmplFile)
		model.setOptionsList(optionsList)
		model.setOutput(True, "")
	
		if maxQueuingTime!=0:
			model.setMaxServerQueuingTime(maxQueuingTime)
		
		if maxTries!=0:
			model.setMaxServerTries(maxTries)
		
		model.connect(cmplUrl)
		#model.debug()
		model.solve()
		
		if model.cmplStatus==CMPL_WARNINGS:
			for m in model.cmplMessages:
				print m.type, m.file, m.line, m.description
	
		if model.solverStatus == SOLVER_OK:
			if not (model.cmplSolFile != "" or model.asciiSolFile != "" or model.csvSolFile != ""):
				model.solutionReport()
				
	elif asyncMode == 1:
		model = Cmpl(cmplFile)
		model.setOptionsList(optionsList)
		model.setOutput(True, "")
		model.setAsyncMode(asyncMode)
	
		if maxQueuingTime!=0:
			model.setMaxServerQueuingTime(maxQueuingTime)
		
		if maxTries!=0:
			model.setMaxServerTries(maxTries)
		
		model.connect(cmplUrl)
		#model.debug()
		model.send()
		
		model.dump()
		
	elif asyncMode == 2:

		model = Cmpl(cmplFile)
		
		model.load()
		model.setAsyncMode(asyncMode)
		model.knock()
		model.dump()

			
	elif asyncMode == 3:
		model = Cmpl(cmplFile)
		
		model.load()
		model.setAsyncMode(asyncMode)
		model.retrieve()
			
		if model.cmplStatus==CMPL_WARNINGS:
			for m in model.cmplMessages:
				print m.type, m.file, m.line, m.description

		if model.solverStatus == SOLVER_OK:
			if not (model.cmplSolFile != "" or model.asciiSolFile != "" or model.csvSolFile != ""):
				model.solutionReport()
			
	elif asyncMode == 4:
		model = Cmpl(cmplFile)
		
		model.load()
		model.setAsyncMode(asyncMode)
		model.cancel()

		print "Problem is canceled ..."
		
			
			
except CmplException, e:
	if e.msg.startswith("pyCmpl error: Model is still running, cannot retrieve the solution"):
		print e.msg
		if model.cmplStatus!=PROBLEM_FINISHED and model.cmplStatus!=PROBLEM_CANCELED:
			model.dump()

	elif not isMessageFlag :
		print e.msg
		isError=True
	elif not e.msg.startswith("pyCmpl error: Cmpl finished with errors"):
		print e.msg
		isError=True
		
	
except IOError, e:
	print str(sys.exc_info()[1])
	isError=True
	
except: 
	isError=True
	print str(sys.exc_info()[1])
	#traceback.print_exc(file=sys.stdout)
	
try:	
        if isMessageFlag and (asyncMode==0 or asyncMode==3) and not isError:
		model.saveCmplMessageFile(messageFile)
	
except CmplException, e:
	print e.msg
except:
	print str(sys.exc_info()[1])
	traceback.print_exc(file=sys.stdout)
