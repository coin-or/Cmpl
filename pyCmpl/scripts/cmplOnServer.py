#!/usr/bin/python 

import sys
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
isMessage=False
messageFile=""
isError=False

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
	
	if i > 4:
		if arg.strip().startswith("-") :
			
			if arg.strip().startswith("-e") :
				isMessageFlag=True
				isMessage=True
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
				messageFile="\""+arg+"\""
	i+=1
	
if tmpStr!="" :
	if fileFlag:
		tmpStr=tmpStr + "\""
	pos = len(optionsList)
	optionsList.update({pos:tmpStr})	
	

try:
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
		
		
except CmplException, e:
	print e.msg
	isError=True
	
if isMessage and not isError:
	model.saveCmplMessageFile(messageFile)

