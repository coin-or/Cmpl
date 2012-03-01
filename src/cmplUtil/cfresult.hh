/***********************************************************************
 *  This code is part of CMPL
 *
 *  Copyright (C) 2007, 2008, 2009, 2010, 2011
 *  Mike Steglich - Technical University of Applied Sciences
 *  Wildau, Germany and Thomas Schleiff - Halle(Saale),
 *  Germany
 *
 *  Coliop3 and CMPL are projects of the Technical University of
 *  Applied Sciences Wildau and the Institute for Operations Research
 *  and Business Management at the Martin Luther University
 *  Halle-Wittenberg.
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



#ifndef CFRESULT_H
#define CFRESULT_H

#include <QDomDocument>
#include <QFile>
#include <string>
#include <QTextStream>
#include <iostream>
#include <iomanip>
#include <fstream>
using namespace std;

#include "CmplTools.hh"
#include "CmplCore.hh"
#include "CmplArgs.hh"
#include "CmplInstance.hh"
#include "FileIO.hh"

namespace cmplUtil {

    /**
    * The <code>cfResult</code> implements the handling of the OSSolverService results
    * <code>cfResult</code> implements methods for analysing and
    * presenting optimization results.
    *
    * @author  Mike Steglich
    *
    */
/*
    typedef struct
    {
        long row;
        long col;
        double value;
    } cflinearConstraintCoefficients;

*/
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



    class cfResult
    {
    public:

        /* *********** methods ******************** */
        /**
       * constructor
       */
       cfResult();

       /**
        * Setting the problem
        */
        bool setProblem(QString &fname, cmpl::CmplCore *m, CmplInstance *c);

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
       * @param outType - 1 stdOut 2 - Ascii
       */
        bool standardReport(int outType);

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
        inline string generalStatus() {
            return resultStatus.generalStatus.toStdString();
        }

        /**
       * returns the serviceName
       */
        inline string serviceName() {
            return resultStatus.serviceName.toStdString();
        }

        /**
       * returns the instanceName
       */
        inline string instanceName() {
            return resultStatus.instanceName.toStdString();
        }


        /**
       * returns the solverName
       */
        inline string solverName() {
            return resultStatus.solverName.toStdString();
        }


        /**
       * returns the solverMessage
       */
        inline string solverMessage() {
            return resultStatus.message.toStdString();
        }


        /**
       * returns the number of variables
       */
        inline long nrOfVariables() {
            return resultStatus.nrOfVars;
        }


        /**
       * returns the number of constraints
       */
        inline long nrOfConstraints() {
            return resultStatus.nrOfConstraints;
        }


        /**
       * returns the number of solutions
       */
        inline int nrOfSolutions() {
            return resultStatus.nrOfSolutions;
        }


        /**
       * returns the objective name
       */
        inline string objectiveName() {
            return  modelNames.objName.toStdString();
        }


        /**
       * returns the objective sense
       */
        inline string objectiveSense() {
            return  modelNames.objSense.toStdString();
        }

        /**
       * returns the obective function value of the current solution
       * @param number of the solution
       */
        inline double objectiveValue(int solutionNumber) {
            return results[solutionNumber].objValue;
        }

        /**
       * returns the variables name
       * @param index of the variable
       */
        inline string varName(long index) {
            return modelNames.varNames[index].toStdString();
        }

        /**
       * returns the variables type
       * @param index of the variable
       */
        inline string varType(long index) {
            return modelConstants.variables[index].type.toStdString();
        }

        /**
       * returns whether the variable has a lower bound or not
       * @param index of the variable
       */
        inline bool varHasLowerBound(long index) {
            return modelConstants.variables[index].hasLB;
        }

        /**
       * returns the variable lower bound
       * @param index of the variable
       */
        inline double varLowerBound(long index) {
            return modelConstants.variables[index].lowerBound;
        }

        /**
       * returns whether the variable has an upper bound or not
       * @param index of the variable
       */
        inline bool varHasUpperBound(long index) {
            return modelConstants.variables[index].hasUB;
        }

        /**
       * returns the variable upper bound
       * @param index of the variable
       */
        inline double varUpperBound(long index){
            return modelConstants.variables[index].upperBound;
        }

        /**
       * returns the variable activity
       * @param number of the solution
       * @param index of the variable
       */
        inline double varActivity(int solutionNumber, long index) {
            return results[solutionNumber].variables[index].activity;
        }

        /**
       * returns whether the variable has a marginal or not
       * @param number of the solution
       * @param index of the variable
       */
        inline bool varHasMarginal(int solutionNumber, long index) {
            return  results[solutionNumber].variables[index].hasMarginal;
        }

        /**
       * returns the variables marginal value
       * @param number of the solution
       * @param index of the variable
       */
        inline double varMarginal(int solutionNumber, long index) {
            return  results[solutionNumber].variables[index].marginal;
        }

        /**
       * returns the constraints name
       * @param index of the constraint
       */
        inline string constraintName(long index) {
            return modelNames.conNames[index].toStdString();
        }

        /**
       * returns the constraints type
       * @param index of the constraint
       */
        inline string constraintType(long index) {
            return modelConstants.constraints[index].type.toStdString();
        }

        /**
       * returns whether the constraint has a lower bound or not
       * @param index of the constraint
       */
        inline bool constraintHasLowerBound(long index) {
            return modelConstants.constraints[index].hasLB;
        }

        /**
       * returns the constraints lower bound
       * @param index of the constraint
       */
        inline double constraintLowerBound(long index) {
            return modelConstants.constraints[index].lowerBound;
        }

        /**
       * returns whether the constraint has an upper bound or not
       * @param index of the constraint
       */
        inline bool constraintHasUpperBound(long index) {
            return modelConstants.constraints[index].hasUB;
        }

        /**
       * returns the constraint upper bound
       * @param index of the constraint
       */
        inline double constraintUpperBound(long index) {
            return modelConstants.constraints[index].upperBound;
        }

        /**
       * returns the constraint activity
       * @param number of the solution
       * @param index of the constraint
       */
        inline double constraintActivity(int solutionNumber, long index) {
            return results[solutionNumber].constraints[index].activity;
        }


        /**
       * returns whether the constraint has a marginal or not
       * @param number of the solution
       * @param index of the constraint
       */
        inline bool constraintHasMarginal(int solutionNumber, long index) {
            return results[solutionNumber].constraints[index].hasMarginal;
        }

        /**
       * returns the constraint marginal value
       * @param number of the solution
       * @param index of the constraint
       */
       inline double constraintMarginal(int solutionNumber, long index) {
           return results[solutionNumber].constraints[index].marginal;
       }

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
        QString solName;
        QString gSolName;

        QString problemName;

        QString cfOsMessage;

       // QList<cflinearConstraintCoefficients> cfModelConstCoefficients;

        cfModelNames modelNames;
        cfModelConstants modelConstants;
        cfModelResultStatus resultStatus;
        QList<cfModelResults> results;

        cmpl::CmplCore *glvar;
        CmplInstance *cmplInst;

        /* ************* methods ************** */

        /**
        * reads the OSrL file of the current problem in a defined structure
        */
        bool cfReadOsrl();

        /**
        * calculates the activities of the constraints
        */
        void calculateConstraintCoefficients();

        bool readGSolFile();

        bool readScipSolFile();

        void readInstance();

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
        inline void setCfOsMessage(const QString &msg) {
            cfOsMessage=msg;
        }//end setCfOsMessage

    };

}
#endif // CFRESULT_H
