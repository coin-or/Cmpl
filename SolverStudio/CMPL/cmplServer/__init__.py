from CmplServerException import *
from CmplServer import *
from CmplGridScheduler import *


if not sys.version_info[:2] == (2,7):
	print "ERROR - pyCmpl only supports Python 2.7"
	exit(-1)
	
	
	
"CMPLServer package"
__author__ = "Mike Steglich"
__copyright__ = "Copyright (C) 2013,2014 Mike Steglich"
__license__ = "LGPLv3"
__version__ = "1.3"