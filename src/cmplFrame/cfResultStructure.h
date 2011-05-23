/***********************************************************************
 *  This code is part of cmplFrame
 *
 *  Copyright (C) 2010 Mike Steglich - Technical University
 *  of Applied Sciences  Wildau, Germany
 *  and Thomas Schleiff - Halle(Saale), Germany
 *
 *  cmplFrame and CMPL are projects of the Technical University of
 *  Applied Sciences Wildau and the Institute for Operations Research
 *  and Business Management at the Martin Luther University
 *  Halle-Wittenberg.
 *  Please visit the project homepage <www.coliop.org>
 *
 *  cmplFrame is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  cmplFrame is distributed in the hope that it will be useful, but WITHOUT
 *  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 *  or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public
 *  License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, see <http://www.gnu.org/licenses/>.
 *
 ***********************************************************************/


#ifndef RESULTSTRUCTURE_H
#define RESULTSTRUCTURE_H

#include <QString>
#include <QList>

// OSiL related structures

typedef struct
{
    long row;
    long col;
    double value;
} cflinearConstraintCoefficients;


typedef struct
{
    //long osIdx;
    bool hasLB;
    bool hasUB;
    double lowerBound;
    double upperBound;
    QString type;

} cfConstantsValues;


typedef struct
{
    long nrOfVariables;
    long nrOfConstraints;

    QList<cfConstantsValues> variables;
    QList<cfConstantsValues> constraints;

} cfModelConstants;

typedef struct
{
   QString instanceName;
   QString description;
   QString objName;
   QString objSense;
   QList<QString> varNames;
   QList<QString> conNames;

} cfModelNames;


// OSrL related structures
typedef struct
{
    long osIdx;
    double activity;
    bool hasMarginal;
    double marginal;

} cfResultValues;

typedef struct
{
    QString status;
    double objValue;
    QList<cfResultValues> variables;
    QList<cfResultValues> constraints;

} cfModelResults;

typedef struct
{
    QString generalStatus;
    QString instanceName;
    QString serviceName;
    QString solverName;
    QString message;
    int nrOfSolutions;
    long nrOfVars;
    long nrOfConstraints;

} cfModelResultStatus;





#endif // RESULTSTRUCTURE_H
