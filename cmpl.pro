##########################################################################
#  This code is part of CMPL
#
#  Copyright (C) 2007, 2008, 2009, 2010 Thomas Schleiff - Halle(Saale), 
#  Germany and Mike Steglich - Technical University of Applied Sciences
#  Wildau, Germany 
#
#  Coliop3 and CMPL are projects of the Technical University of 
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
##########################################################################


QT      += xml
QT      -= gui

TEMPLATE = app

TARGET =  cmpl
DESTDIR = bin/

OBJECTS_DIR = obj
MOC_DIR = obj

win32 { 
	CONFIG  += console
	DEFINES +=MINGW32
}

macx {
    CONFIG(debug, debug|release):  CONFIG -= app_bundle
    CONFIG(release, debug|release): CONFIG += app_bundle
}

unix: DEFINES += UNIX

CONFIG(release, debug|release): CONFIG += warn_off

DEFINES += PROTO CHDIR LANG_EN
DEFINES += SOLVERSERVICE

DEPENDPATH += . src/cmpl src/cmplFrame src/eCMPL
INCLUDEPATH += . src/cmpl src/cmplFrame src/eCMPL

#Flex and Bison handling
unix {
    system(data/check_bison_flex) {
        system(bison -o src/cmpl/_CMPL_Parser.cc src/cmpl/CMPL_Parser.yy )
        system(flex -o src/cmpl/_CMPL_Scanner.cc src/cmpl/CMPL_Scanner.l)
    } else :  message( FLEX and/or BISON not installed )
}


HEADERS += src/cmpl/Compiler.hh \
           src/cmpl/FileIO.hh \
           src/cmpl/gen.h \
           src/cmpl/gencode.h \
           src/cmpl/genstart.h \
           src/cmpl/gentext.h \
           src/cmpl/Interpreter.hh \
           src/cmpl/IntFuncBase.hh \
           src/cmpl/IntFuncCode.hh \
           src/cmpl/IntFuncCtrl.hh \
           src/cmpl/IntFuncDef.hh \
           src/cmpl/IntFuncEnv.hh \
           src/cmpl/IntFuncIndex.hh \
           src/cmpl/IntFuncLine.hh \
           src/cmpl/IntFuncOper.hh \
           src/cmpl/IntFuncTerm.hh \
           src/cmpl/IntFuncValue.hh \
           src/cmpl/Main.hh \
           src/cmpl/SetIter.hh \
           src/cmplFrame/mprlstructure.h \
           src/cmplFrame/mprl.hh \
           src/cmplFrame/cmplFrame.hh \
           src/cmplFrame/cfStatus.h \
           src/cmplFrame/cfResultStructure.h \
           src/cmplFrame/cfresult.hh \
           src/cmplFrame/cFException.hh \
           src/eCMPL/eCmplStatus.h \
           src/eCMPL/ecmplio.hh \
           src/eCMPL/ecmpl.hh \
           src/cmpl/_CMPL_Parser.hh



SOURCES += src/cmpl/Compiler.cc \
           src/cmpl/FileIO.cc \
           src/cmpl/Interpreter.cc \
           src/cmpl/IntFuncBase.cc \
           src/cmpl/IntFuncCode.cc \
           src/cmpl/IntFuncCtrl.cc \
           src/cmpl/IntFuncDef.cc \
           src/cmpl/IntFuncEnv.cc \
           src/cmpl/IntFuncIndex.cc \
           src/cmpl/IntFuncLine.cc \
           src/cmpl/IntFuncOper.cc \
           src/cmpl/IntFuncTerm.cc \
           src/cmpl/IntFuncValue.cc \
           src/cmpl/Main.cc \
           src/cmpl/SetIter.cc \
           src/cmpl/Start.cc \
           src/cmplFrame/mprl.cc \
           src/cmplFrame/cmplFrame.cc \
           src/cmplFrame/cfresult.cc \
           src/cmplFrame/cFException.cc \
           src/eCMPL/ecmplio.cc \
           src/eCMPL/ecmpl.cc \
           src/cmpl/_CMPL_Parser.cc \
           src/cmpl/_CMPL_Scanner.cc


OTHER_FILES += \
   src/cmpl/CMPL_Scanner.l \
   src/cmpl/CMPL_Parser.yy



