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


#ifndef CFRESULT_H
#define CFRESULT_H

#include <QDomDocument>
#include <QFile>
#include <string>
#include <QTextStream>
#include <iostream>
#include <iomanip>
using namespace std;


#include "cfResultStructure.h"

/**
 * The <code>cfResult</code> implements the handling of the OSSolverService results
 * <code>cfResult</code> implements methods for analysing and
 * presenting optimization results.
 *
 * @author  Mike Steglich
 *
 */


class cfResult
{
public:

    /* *********** methods ******************** */
    /**
     * constructor
     */
    cfResult();

    /**
     * Sets the problem name.
     */
    bool setProblemName(QString &fname);

    /**
     * returns the content of the current cfResult message
     */
    string getCfOsMessage();

    /**
     * reads the OSiL an OSrL file of the current problem into a defined structur
     */
    bool getResult();

    /**
     * performs the a standard report
     */
    void standardReport();

    /**
     * performs the a report about the OSSolverMessages
     * only necessary if OSSolverService failed
     */
    void getSolverMessages();

    /**
     * writes the optimization results in a CSV file
     */
    bool writeCsv();

    /**
     * returns the generalStaus
     */
    string generalStatus();

    /**
     * returns the serviceName
     */
    string serviceName();

    /**
     * returns the instanceName
     */
    string instanceName();

    /**
     * returns the solverName
     */
    string solverName();

    /**
     * returns the solverMessage
     */
    string solverMessage();

    /**
     * returns the number of variables
     */
    long nrOfVariables();

    /**
     * returns the number of constraints
     */
    long nrOfConstraints();

    /**
     * returns the number of solutions
     */
    int nrOfSolutions();

    /**
     * returns the objective name
     */
    string objectiveName();

    /**
     * returns the objective sense
     */
    string objectiveSense();

    /**
     * returns the obective function value of the current solution
     * @param number of the solution
     */
    double objectiveValue(int solutionNumber);

    /**
     * returns the variables name
     * @param index of the variable
     */
    string varName(long index);

    /**
     * returns the variables type
     * @param index of the variable
     */
    string varType(long index);

    /**
     * returns whether the variable has a lower bound or not
     * @param index of the variable
     */
    bool varHasLowerBound(long index);

    /**
     * returns the variable lower bound
     * @param index of the variable
     */
    double varLowerBound(long index);

    /**
     * returns whether the variable has an upper bound or not
     * @param index of the variable
     */
    bool varHasUpperBound(long index);

    /**
     * returns the variable upper bound
     * @param index of the variable
     */
    double varUpperBound(long index);

    /**
     * returns the variable activity
     * @param number of the solution
     * @param index of the variable
     */
    double varActivity(int solutionNumber, long index);

    /**
     * returns whether the variable has a marginal or not
     * @param number of the solution
     * @param index of the variable
     */
    bool varHasMarginal(int solutionNumber, long index);

    /**
     * returns the variables marginal value
     * @param number of the solution
     * @param index of the variable
     */
    double varMarginal(int solutionNumber, long index);


    /**
     * returns the constraints name
     * @param index of the constraint
     */
    string constraintName(long index);

    /**
     * returns the constraints type
     * @param index of the constraint
     */
    string constraintType(long index);

    /**
     * returns whether the constraint has a lower bound or not
     * @param index of the constraint
     */
    bool constraintHasLowerBound(long index);

    /**
     * returns the constraints lower bound
     * @param index of the constraint
     */
    double constraintLowerBound(long index);


    /**
     * returns whether the constraint has an upper bound or not
     * @param index of the constraint
     */
    bool constraintHasUpperBound(long index);

    /**
     * returns the constraint upper bound
     * @param index of the constraint
     */
    double constraintUpperBound(long index);

    /**
     * returns the constraint activity
     * @param number of the solution
     * @param index of the constraint
     */
    double constraintActivity(int solutionNumber, long index);

    /**
     * returns whether the constraint has a marginal or not
     * @param number of the solution
     * @param index of the constraint
     */
    bool constraintHasMarginal(int solutionNumber, long index);

    /**
     * returns the constraint marginal value
     * @param number of the solution
     * @param index of the constraint
     */
    double constraintMarginal(int solutionNumber, long index);

    /**
       * returns the variables type
       * @param name of the variable
       */
    string varType(string varName);

    /**
     * returns whether the variable has a lower bound or not
     * @param name of the variable
     */
    bool varHasLowerBound(string varName);

    /**
     * returns the variable lower bound
     * @param name of the variable
     */
    double varLowerBound(string varName);

    /**
      * returns whether the variable has an upper bound or not
      * @param name of the variable
      */
    bool varHasUpperBound(string varName);

    /**
      * returns the variable upper bound
      * @param name of the variable
      */
    double varUpperBound(string varName);

    /**
      * returns the variable activity
      * @param number of the solution
      * @param name of the variable
      */
    double varActivity(int solutionNumber, string varName);

    /**
     * returns whether the variable has a marginal or not
     * @param number of the solution
     * @param name of the variable
     */
    bool varHasMarginal(int solutionNumber, string varName);


    /**
     * returns the variables marginal value
     * @param number of the solution
     * @param name of the variable
     */
    double varMarginal(int solutionNumber, string varName);

    /**
     * returns the constraints type
     * @param name of the constraint
     */
    string constraintType(string conName);

    /**
     * returns whether the constraint has a lower bound or not
     * @param name of the constraint
     */
    bool constraintHasLowerBound(string conName);

    /**
     * returns the constraints lower bound
     * @param name of the constraint
     */
    double constraintLowerBound(string conName);

    /**
     * returns whether the constraint has an upper bound or not
     * @param name of the constraint
     */
    bool constraintHasUpperBound(string conName);

    /**
     * returns the constraint upper bound
     * @param name of the constraint
     */
    double constraintUpperBound(string conName);

    /**
     * returns the constraint activity
     * @param number of the solution
     * @param name of the constraint
     */
    double constraintActivity(int solutionNumber, string conName);


    /**
     * returns whether the constraint has a marginal or not
     * @param number of the solution
     * @param name of the constraint
     */
    bool constraintHasMarginal(int solutionNumber, string conName);

    /**
      * returns the constraint marginal value
      * @param number of the solution
      * @param name of the constraint
      */
    double constraintMarginal(int solutionNumber, string conName);

private:

    /* ************* variables ************** */
    QString osilName;
    QString osrlName;
    QString csvName;

    QString cfOsMessage;

    QList<cflinearConstraintCoefficients> cfModelConstCoefficients;

    cfModelNames modelNames;
    cfModelConstants modelConstants;
    cfModelResultStatus resultStatus;
    QList<cfModelResults> results;

    /* ************* methods ************** */

    /**
      * reads the OSrL file of the current problem in a defined structure
      */
    bool cfReadOsrl();

    /**
      * reads the OSiL file of the current problem in a defined structure
      */
    bool cfReadOsil();

    /**
      * calculates the activities of the constraints
      */
    void calculateConstraintCoefficients();

    /**
      * returns the variables name identified by the index
      */
    long getVarNameIndex(string varName);

    /**
      * returns the constraints name identified by the index
      */
    long getConstraintNameIndex(string conName);

    /**
      * sets the current cfResult message
      */
    void setCfOsMessage(const QString &msg);

};

#endif // CFRESULT_H
