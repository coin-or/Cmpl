from CmplDefs import *
from Cmpl import *
from CmplException import *
from CmplMsg import *
from CmplTools import *
from CmplSolution import *
from CmplSet import *
from CmplParameter import *
from CmplInstance import *
from CmplInfo import *

if not sys.version_info[:2] == (2,7):
	print "ERROR - pyCmpl only supports Python 2.7"
	exit(-1)
	