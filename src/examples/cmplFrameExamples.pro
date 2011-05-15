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

TARGET =  cmplFrame
DESTDIR = ../../bin/

OBJECTS_DIR = ../../obj
MOC_DIR = ../../obj

win32: CONFIG += console
macx : CONFIG -= app_bundle
  
win32: DEFINES +=MINGW32
unix: DEFINES += UNIX


DEPENDPATH += .  ../cmplFrame ../eCMPL
INCLUDEPATH += . ../cmplFrame ../eCMPL

HEADERS += \
    cfremotesynchronously.hh \
    cfremoteasynchronously.hh \
    ../cmplFrame/mprlstructure.h \
    ../cmplFrame/mprl.hh \
    ../cmplFrame/cmplFrame.hh \
    ../cmplFrame/cfStatus.h \
    ../cmplFrame/cfResultStructure.h \
    ../cmplFrame/cfresult.hh \
    ../cmplFrame/cFException.hh \
    ../eCMPL/eCmplStatus.h \
    ../eCMPL/ecmplio.hh \
    ../eCMPL/ecmpl.hh \
    cflocal.hh

SOURCES += \
    cmplFrameExamplesMain.cc \
    cfremotesynchronously.cc \
    cfremoteasynchronously.cc \
    ../cmplFrame/mprl.cc \
    ../cmplFrame/cmplFrame.cc \
    ../cmplFrame/cfresult.cc \
    ../cmplFrame/cFException.cc \
    ../eCMPL/ecmplio.cc \
    ../eCMPL/ecmpl.cc \
    cflocal.cc
