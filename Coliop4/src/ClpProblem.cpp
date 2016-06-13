/***********************************************************************
 *  This code is part of CMPL
 *
 *  Copyright (C) 2007 - 2016
 *  Thomas Schleiff - Halle(Saale), Germany and
 *  Mike Steglich - Technical University of Applied Sciences
 *  Wildau, Germany
 *
 *  Coliop and CMPL are projects of the Technical University of
 *  Applied Sciences Wildau.
 *  Please visit the project homepage <www.coliop.org>
 *
 *  CMPL is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  CMPL is distributed in the hope that it will be useful, but WITHOUT
 *  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 *  or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public
 *  License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, see <http://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

#include "ClpProblem.hh"

ClpProblem::ClpProblem()
{
   setDefaults();
}


void ClpProblem::setDefaults() {
    _problemName="";
    _problem="";
    _isChanged=false;
    _isSolved=false;
    _cmplList.clear();
    _activeCmplIndex=0;
}

QString ClpProblem::getProblemName() {
    return _problemName;
}


QString ClpProblem::getProblemBaseName() {
    return _problemBaseName;
}


QString ClpProblem::getProblem() {
    return _problem;
}

QString ClpProblem::getProblemPath() {
    return _problemPath;
}


void ClpProblem::setProblem(QString &prob){
    _problem=prob;
    QFileInfo fn(_problem);
    _problemName= fn.fileName();
    _problemBaseName = fn.baseName();
    _problemPath = fn.absolutePath();


}

int ClpProblem::getActiveCmplIndex() {
 return _activeCmplIndex;
}

void ClpProblem::setActiveCmplIndex(int idx) {
    _activeCmplIndex = idx;
}



QString ClpProblem::getCmplListElement(int idx) {
    return _cmplList.at(idx);
}

int ClpProblem::cmplListLength() {
    return _cmplList.length();
}

void ClpProblem::addCmplList(QString name) {
    _cmplList.append(name);
}

void ClpProblem::delCmplListItem(int idx)
{
    _cmplList.removeAt(idx);
}

void ClpProblem::clearCmplList() {
    _cmplList.clear();
}

QStringList ClpProblem::getCmplList()
{
    return _cmplList;
}

bool ClpProblem::isProblemChanged() const
{
    return _isChanged;
}

void ClpProblem::setProblemChanged(bool isChanged)
{
    _isChanged = isChanged;
}

