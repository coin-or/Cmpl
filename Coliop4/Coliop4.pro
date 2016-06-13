##########################################################################
#  This code is part of CMPL
#
#  Copyright (C) 2007 -2016 Thomas Schleiff - Halle(Saale),
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

QT      += gui widgets printsupport
QT      += xml

TARGET = Coliop4
TEMPLATE = app

DESTDIR = bin/
OBJECTS_DIR = obj
MOC_DIR = obj
UI_DIR = obj
RCC_DIR = obj


SOURCES +=\
    src/ClpAbout.cpp \
    src/ClpProblem.cpp \
    src/CmplHighlighter.cpp \
    src/Main.cpp \
    src/ColiopUi.cpp \
    src/CmplRunner.cpp

HEADERS  += \
    src/ClpAbout.hh \
    src/ClpProblem.hh \
    src/CmplHighlighter.hh \
    src/ColiopUi.hh \
    src/CmplRunner.hh

FORMS    += src/coliopui.ui \
    src/clpabout.ui

RESOURCES += \
    Ressources/clpRessources.qrc
