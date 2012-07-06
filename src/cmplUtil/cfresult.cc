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
#include "cfresult.hh"

namespace cmplUtil {

    /* ************** cfResult ************ */
    cfResult::cfResult()
    {
        osilName="";
        osrlName="";
        cfOsMessage="";
        gSolName="";
        cSolName="";
        problemName="";

        modelNames.instanceName="";
        modelNames.description="";
        modelNames.objName="";
        modelNames.objSense="";

        modelConstants.nrOfConstraints=0;
        modelConstants.nrOfVariables=0;

        resultStatus.generalStatus="";
        resultStatus.instanceName="";
        resultStatus.serviceName="";
        resultStatus.solverName="";
        resultStatus.message="";
        resultStatus.nrOfConstraints=0;
        resultStatus.nrOfSolutions=0;
        resultStatus.nrOfVars=0;

    }//end cfResult



    /* ************** setProblemName ************ */
    bool cfResult::setProblem(QString &fname, cmpl::CmplCore *m, CmplInstance *c) {
        if (fname.toLower().contains(".osrl") || fname.toLower().contains(".osil") || fname.toLower().contains(".cmpl")) {
            setCfOsMessage("Please define the problemname w/o any file extensions");
            return false;
        }
        osilName=fname+".osil";
        osrlName=fname+".osrl";
        csvName=fname+".csv";
        solName=fname+".sol";
        gSolName=fname+".gsol";
        cSolName=fname+".gsol";
        problemName=fname+".cmpl";

        glvar=m;
        cmplInst=c;

        return true;
    }//end setProblemName


    /* ************** getCfOsMessage ************ */
    string cfResult::getCfOsMessage() {
        return cfOsMessage.toStdString();
    }//end getCfOsMessage


    /* ************** getResult ************ */
    bool cfResult::getResult() {

        readInstance();
        if (glvar->args->solverName()=="glpk" || glvar->args->solverName()=="gurobi") {
            if (!readGSolFile()) return false;
        }
        else if (glvar->args->solverName()=="scip" ) {
            if (!readScipSolFile()) return false;
            calculateConstraintCoefficients();

        } else if (glvar->args->solverName()=="cplex" ) {
            if (!readCplexSolFile()) return false;

        } else {
            if (!cfReadOsrl()) return false;
            calculateConstraintCoefficients();
        }
        return true;
    }//end getResult


    /* ************** writeReportHeader ************ */
    void cfResult::writeReportHeader() {

        cout.precision( glvar->args->maxDecimals()+1 );
        if (glvar->args->solverName()=="cplex")
            cout << endl;
        cout << setw(105) << setfill('-') << "" <<endl;
        cout <<  setfill(' ');
        cout << setw(21) << left << "Problem  " << instanceName() << endl;
        cout << setw(21) << left <<  "Nr. of variables" << nrOfVariables() << endl;
        cout << setw(21) << left <<  "Nr. of constraints" << nrOfConstraints() << endl;
        cout << setw(21) << left << "Status " << generalStatus()<< endl;
        cout << setw(21) << left << "Solver name" << solverName() << endl;

    }//end writeReportHeader


    /* ************** writeVarHeader ************ */
    void cfResult::writeVarHeader(long n){

        cout << setw(21) << left <<  "Objective name  " << objectiveName() << endl;
        cout << setw(21) << left <<  "Objective value  " << objectiveValue(n) << " (" << objectiveSense() << "!)" << endl;
        cout << setw(105) << setfill('-') << "" <<endl;
        cout <<  setfill(' ');
        cout <<endl;

        if (!(glvar->args->displayVarListSize()>0 || glvar->args->displayConListSize()>0) || (glvar->args->displayVarListSize()>0) ) {

            if (glvar->args->ingnoreZeros())
                cout << "Nonzero variables ";
            else
                cout << "Variables ";

            if (glvar->args->displayVarListSize()>0) {
               cout << "(";
               for (int j=0; j<glvar->args->displayVarListSize(); j++)
                    cout << glvar->args->displayVarList(j);
               cout << ")";

            }
            cout <<  endl;

            cout << setw(20) << left <<  "Name";
            cout << setw(5) << right <<  "Type";
            cout << setw(20) << right <<   "Activity";
            cout << setw(20) << right <<  "Lower bound";
            cout << setw(20) << right <<  "Upper bound";
            cout << setw(20) << right <<  "Marginal" << endl;
            cout << setw(105) << setfill('-') << "" <<endl;
            cout <<  setfill(' ');
        }
    }//end writeVarHeader


    /* ************** writeVarValues ************ */
    void cfResult::writeVarValues(long n, long i) {

        cout << setw(20) <<   left <<  varName(i);
        cout << setw(5) <<  right << varType(i);
        cout << setw(20) << right << resultValue(varActivity(n,i));

        if (varHasLowerBound(i))
            cout << setw(20) << right << varLowerBound(i);
        else
            cout << setw(20)  << right<< "-Infinity";
        if (varHasUpperBound(i))
            cout << setw(20) << right << varUpperBound(i);
        else
            cout << setw(20)  << right<< "Infinity";
        if (varHasMarginal(n,i))
            cout << setw(20)  << right<< resultValue(varMarginal(n,i));
        else
            cout << setw(20) << right << "-" ;
        cout << endl;

    }//end writeVarValues


    /* ************** writeVarFooter ************ */
    void cfResult::writeVarFooter() {
        if (!(glvar->args->displayVarListSize()>0 || glvar->args->displayConListSize()>0) || (glvar->args->displayVarListSize()>0) ) {
            cout << setw(105) << setfill('-') << "" <<endl;
            cout <<  setfill(' ');
        }
    }//end writeVarFooter


    /* ************** writeConHeader ************ */
    void cfResult::writeConHeader() {

        if (!(glvar->args->displayVarListSize()>0 || glvar->args->displayConListSize()>0) || (glvar->args->displayConListSize()>0) ) {
            cout <<endl;
            if (glvar->args->ingnoreZeros())
                cout << "Nonzero constraints ";
            else
                cout << "Constraints ";

            if (glvar->args->displayConListSize()>0) {
               cout << "(";
               for (int j=0; j<glvar->args->displayConListSize(); j++)
                    cout << glvar->args->displayConList(j);
               cout << ")";
            }
            cout <<  endl;
            cout << setw(20) << left <<  "Name";
            cout << setw(5) << right <<  "Type";
            cout << setw(20) << right <<   "Activity";
            cout << setw(20) << right <<  "Lower bound";
            cout << setw(20) << right <<  "Upper bound";
            cout << setw(20) << right <<  "Marginal" << endl;
            cout << setw(105) << setfill('-') << "" <<endl;
            cout <<  setfill(' ');
        }
    }//end writeConHeader


    /* ************** writeConValues ************ */
    void cfResult::writeConValues(long n, long i) {

        cout << setw(20) << left <<  constraintName(i);
        cout << setw(5) <<  right << constraintType(i);
        cout << setw(20) << right<< resultValue(constraintActivity(n,i));
        if (constraintHasLowerBound(i))
            cout << setw(20)  << right << constraintLowerBound(i);
        else
            cout << setw(20) << right<< "-Infinity";
        if (constraintHasUpperBound(i))
            cout << setw(20) << right << constraintUpperBound(i);
        else
            cout << setw(20) << right<< "Infinity";
        if (constraintHasMarginal(n,i))
            cout << setw(20) << right << resultValue(constraintMarginal(n,i));
        else
            cout << setw(20) << right << "-";
        cout <<endl;
    }//end writeConValues


    /* ************** writeReportFooter ************ */
    void cfResult::writeReportFooter() {
        if (!(glvar->args->displayVarListSize()>0 || glvar->args->displayConListSize()>0) || (glvar->args->displayConListSize()>0) ) {
            cout << setw(105) << setfill('-') << "" <<endl;
            cout <<  setfill(' ');
        }
    }//end writeReportFooter


    /* ************** standardReport ************ */
    bool cfResult::standardReport(int outType) {

        // outType==1 - stdIio - default
        // outType==2 - Ascii file

        if (glvar->args->displayVarListSize()>0 || glvar->args->displayConListSize()>0)
            return custReport(outType);

        CmplFileType report;
        report.setFileName(solName.toStdString());
        if (outType==1)
            report.setOutMode(STD_OUT);
        else
            report.setOutMode(FILE_OUT);

        if (!report.open()) {
            setCfOsMessage("Internal Error - Can not write the solution file!\n" );
            return false;
        }

        writeReportHeader();
        for (int n=0; n < nrOfSolutions(); n++)   {
            writeVarHeader(n);
            for (long i=0; i < nrOfVariables(); i++) {
                if( !(glvar->args->ingnoreZeros() && resultValue(varActivity(n,i))==0) ) {
                    writeVarValues(n,i);
                }
            }
            writeVarFooter();
            writeConHeader();
            for (long i=0; i < nrOfConstraints(); i++) {
                if( !(glvar->args->ingnoreZeros() && resultValue(constraintActivity(n,i))==0)) {
                    writeConValues(n,i);
                }
            }
            writeReportFooter();
        }
        report.close();

        return true;
    }//end standardReport



    /* ************** standardReport ************ */
    bool cfResult::custReport(int outType) {

        // outType==1 - stdIio - default
        // outType==2 - Ascii file

        CmplFileType report;
        report.setFileName(solName.toStdString());
        if (outType==1)
            report.setOutMode(STD_OUT);
        else
            report.setOutMode(FILE_OUT);

        if (!report.open()) {
            setCfOsMessage("Internal Error - Can not write the solution file!\n" );
            return false;
        }

        writeReportHeader();

        for (int n=0; n < nrOfSolutions(); n++)   {
            writeVarHeader(n);
            if (glvar->args->displayVarListSize()>0) {
                for (int j=0; j<glvar->args->displayVarListSize(); j++ ) {
                    for (long i=0; i < nrOfVariables(); i++) {
                        string tmpStr=glvar->args->displayVarList(j);
                        int starPos=tmpStr.find_first_of("*");
                        tmpStr=tmpStr.substr(0,starPos);
                        if ( (starPos>-1 && CmplTools::stringStartsWith(varName(i),tmpStr)) || (starPos==-1 && varName(i)==tmpStr) ) {
                            if( !(glvar->args->ingnoreZeros() && resultValue(varActivity(n,i))==0) ) {
                                writeVarValues(n,i);
                            }
                        }
                    }
                }
            }

            writeVarFooter();
            if (glvar->args->displayConListSize()>0) {
                writeConHeader();
                for (int j=0; j<glvar->args->displayConListSize(); j++ ) {
                    for (long i=0; i < nrOfConstraints(); i++) {
                        string tmpStr=glvar->args->displayConList(j);
                        int starPos=tmpStr.find_first_of("*");
                        tmpStr=tmpStr.substr(0,starPos);

                        if ( (starPos>-1 && CmplTools::stringStartsWith(constraintName(i),tmpStr)) || (starPos==-1 && constraintName(i)==tmpStr) ) {
                            if( !(glvar->args->ingnoreZeros() && resultValue(constraintActivity(n,i))==0)) {
                               writeConValues(n,i);
                            }
                        }
                    }
                }
               writeReportFooter();
            }
        }
        report.close();
        return true;
    }//end custReport


    /* ************** writeCsvHeader ************ */
    void cfResult::writeCsvHeader() {
        cout << "CMPL csv export" << endl << endl;
        cout << "Problem;" << instanceName() << endl;
        cout << "Nr. of variables;" << nrOfVariables() << endl;
        cout << "Nr. of constraints;" << nrOfConstraints() << endl;
        cout << "Status;" << generalStatus() << endl;
        cout << "Solver;" << solverName() << endl;

    }//end writeCsvHeader


    /* ************** writeCsvVarHeader ************ */
    void cfResult::writeCsvVarHeader(long n) {

        cout <<  "Objective name;" << objectiveName() << endl;
        cout <<  "Objective value;  " << objectiveValue(n) << ";(" << objectiveSense() << "!)" << endl;
        cout <<  endl;

        if (!(glvar->args->displayVarListSize()>0 || glvar->args->displayConListSize()>0) || (glvar->args->displayVarListSize()>0) ) {

            if (glvar->args->ingnoreZeros())
                cout << "Nonzero variables;";
            else
                cout << "Variables;";

            if (glvar->args->displayVarListSize()>0) {
               cout << "(";
               for (int j=0; j<glvar->args->displayVarListSize(); j++)
                    cout << glvar->args->displayVarList(j);
               cout << ")";

            }
            cout << endl;
            cout <<  "Name;Type;Activity;LowerBound;UpperBound;Marginal" << endl;
        }
    }//end writeCsvVarHeader


    /* ************** writeCsvVarValues ************ */
    void cfResult::writeCsvVarValues(long n, long i) {
        cout << varName(i) << ";" ;
        cout << varType(i) << ";";
        cout << resultValue(varActivity(n,i) )<< ";";
        if (varHasLowerBound(i))
            cout << varLowerBound(i) << ";";
        else
            cout << "-Infinity ;" ;
        if (varHasUpperBound(i))
            cout << varUpperBound(i) << ";";
        else
            cout << "Infinity;";
        if (varHasMarginal(n,i))
            cout << resultValue(varMarginal(n,i)) ;
        else
            cout << "-" ;
        cout << endl;
    }//end writeCsvVarValues


    /* ************** writeCsvConHeader ************ */
    void cfResult::writeCsvConHeader() {

        if (!(glvar->args->displayVarListSize()>0 || glvar->args->displayConListSize()>0) || (glvar->args->displayConListSize()>0) ) {
            cout <<endl;
            if (glvar->args->ingnoreZeros())
                cout << "Nonzero constraints;";
            else
                cout << "Constraints;";

            if (glvar->args->displayConListSize()>0) {
               cout << "(";
               for (int j=0; j<glvar->args->displayConListSize(); j++)
                    cout << glvar->args->displayConList(j);
               cout << ")";
            }
            cout << endl;
            cout <<  "Name;Type;Activity;LowerBound;UpperBound;Marginal" << endl;
       }
    }//end writeCsvConHeader


    /* ************** writeCsvConValues ************ */
    void cfResult::writeCsvConValues(long n, long i) {

        cout << constraintName(i) << ";";
        cout << constraintType(i) << ";";
        cout << resultValue(constraintActivity(n,i) )<< ";";
        if (constraintHasLowerBound(i))
            cout << constraintLowerBound(i) << ";";
        else
            cout << "-Infinity" << ";";
        if (constraintHasUpperBound(i))
            cout << constraintUpperBound(i) << ";";
        else
            cout << "Infinity" << ";";
        if (constraintHasMarginal(n,i))
            cout << resultValue(constraintMarginal(n,i));
        else
            cout << "-";
        cout <<endl;
    }//end writeCsvConValues


    /* ************** writeCsv ************ */
    bool cfResult::writeCsv() {

        if (glvar->args->displayVarListSize()>0 || glvar->args->displayConListSize()>0)
            return custWriteCsv();

        CmplFileType report;
        report.setFileName(csvName.toStdString());
        report.setOutMode(FILE_OUT);

        if (!report.open()) {
            setCfOsMessage("Internal Error - Can not write the CSV file!\n" );
            return false;
        }

        writeCsvHeader();

        for (int n=0; n < nrOfSolutions(); n++)   {
            writeCsvVarHeader(n);
            for (long i=0; i < nrOfVariables(); i++) {
                if( !(glvar->args->ingnoreZeros() && resultValue(varActivity(n,i))==0) )  {
                    writeCsvVarValues(n,i);
                }
            }

            writeCsvConHeader();

            for (long i=0; i < nrOfConstraints(); i++) {
                if( !(glvar->args->ingnoreZeros() && resultValue(constraintActivity(n,i))==0))  {
                    writeCsvConValues(n,i);
                }
            }
        }

        report.close();
        return true;
    }//end writeCsv


    /* ************** writeCsv ************ */
    bool cfResult::custWriteCsv() {

        CmplFileType report;
        report.setFileName(csvName.toStdString());
        report.setOutMode(FILE_OUT);

        if (!report.open()) {
            setCfOsMessage("Internal Error - Can not write the CSV file!\n" );
            return false;
        }

        writeCsvHeader();

        for (int n=0; n < nrOfSolutions(); n++)   {
            writeCsvVarHeader(n);
            if (glvar->args->displayVarListSize()>0) {
                for (int j=0; j<glvar->args->displayVarListSize(); j++ ) {
                    for (long i=0; i < nrOfVariables(); i++) {
                        string tmpStr=glvar->args->displayVarList(j);
                        int starPos=tmpStr.find_first_of("*");
                        tmpStr=tmpStr.substr(0,starPos);

                        if ( (starPos>-1 && CmplTools::stringStartsWith(varName(i),tmpStr)) || (starPos==-1 && varName(i)==tmpStr) ) {
                            if( !(glvar->args->ingnoreZeros() && resultValue(varActivity(n,i))==0) ) {
                                writeCsvVarValues(n,i);
                            }
                        }
                    }
                }
            }

            if (glvar->args->displayConListSize()>0) {
                writeCsvConHeader();
                for (int j=0; j<glvar->args->displayConListSize(); j++ ) {
                    for (long i=0; i < nrOfConstraints(); i++) {
                        string tmpStr=glvar->args->displayConList(j);
                        int starPos=tmpStr.find_first_of("*");
                        tmpStr=tmpStr.substr(0,starPos);

                        if ( (starPos>-1 && CmplTools::stringStartsWith(constraintName(i),tmpStr)) || (starPos==-1 && constraintName(i)==tmpStr) ) {
                            if( !(glvar->args->ingnoreZeros() && resultValue(constraintActivity(n,i))==0)) {
                                writeCsvConValues(n,i);
                            }
                        }
                    }
                }
            }
        }
        report.close();
        return true;
    }//end custWriteCsv



    /* ************** getSolverMessages ************ */
    void cfResult::getSolverMessages() {

        cout << endl << setw(20) << left << "Solver failed" <<endl;
        cout << setw(20) << left << "General status: " << generalStatus()<< endl;
        cout << setw(20) << left << "SolverName" << solverName() << endl;
        cout << setw(20) << left << "Message: " << solverMessage() << endl;
        cout << setw(20) << left << "Nr. of Solutions: " << nrOfSolutions() << endl;

    }//end getSolverMessages


    /* *************** readScipSolFile ************* */
    bool cfResult::readScipSolFile() {

        int lineCounter=0;
        string tmpName;
        long tmpIndex=0;

        resultStatus.serviceName="SCIP";
        resultStatus.solverName="SCIP";

        resultStatus.instanceName=problemName;
        resultStatus.message="";

        cfModelResults tmpResults;
        cfResultValues tmpValues;

        tmpResults.status="";
        tmpResults.objValue=0;
        tmpResults.variables.clear();
        tmpResults.constraints.clear();

        resultStatus.nrOfConstraints=cmplInst->rowCount()-1;
        resultStatus.nrOfVars=cmplInst->colCount();

        for (int i=0; i<resultStatus.nrOfConstraints; i++ ) {
            tmpValues.osIdx=0;
            tmpValues.activity=0;
            tmpValues.hasMarginal=false;
            tmpValues.marginal=0;
            tmpResults.constraints.append(tmpValues);
        }

        for (int j=0; j<resultStatus.nrOfVars; j++ ) {
            tmpValues.osIdx=0;
            tmpValues.activity=0;
            tmpValues.hasMarginal=false;
            tmpValues.marginal=0;
            tmpResults.variables.append(tmpValues);
        }

        QFile cFOptFile (gSolName);

        if (!cFOptFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
            setCfOsMessage("Can't open Scip Solution file: " + gSolName);
            return false;
        }

        while (!cFOptFile.atEnd()) {

            if (cFOptFile.error()>0) {
                setCfOsMessage("Can't read Scip Solution file: " + gSolName);
                return false;
            }

            QString line = cFOptFile.readLine();
            QStringList solList = line.split(" ", QString::SkipEmptyParts);
            lineCounter++;

            if (lineCounter==1) {
                tmpResults.status=solList[2];
                resultStatus.generalStatus=solList[2].remove(QRegExp("\n"));
                if (tmpResults.status.toLower()!="optimal"){
                    QString line = cFOptFile.readLine();
                    resultStatus.message = line.remove(QRegExp("\n"));
                    break;
                }
                resultStatus.nrOfSolutions=1;
                continue;
            }
            if (lineCounter==2) {
                tmpResults.objValue=solList[2].toDouble();
                continue;
            }

            if (lineCounter>2) {
                tmpName=solList[0].toStdString();
                tmpIndex=getVarNameIndex(tmpName);
                tmpResults.variables[tmpIndex].activity=solList[1].toDouble();
            }

        }

        results.append(tmpResults);
        cFOptFile.close();

        return true;


    }

    /* ************** getSolverMessages ************ */
    bool cfResult::readGSolFile() {


        int row=0;
        int col=0;
        int colCount=0;

        int realIdx;
        int intIdx;

        bool realVarSection;

        int lineCounter=0;

        cfModelResults tmpResults;
        cfResultValues tmpValues;

        if (glvar->args->solverName()=="glpk" ) {
            resultStatus.serviceName="GLPK";
            resultStatus.solverName="GLPK";
        } else if( glvar->args->solverName()=="gurobi" ){
            resultStatus.serviceName="Gurobi";
            resultStatus.solverName="Gurobi";
        }

        resultStatus.instanceName=problemName;

        resultStatus.message="";

        tmpResults.status="";
        tmpResults.objValue=0;
        tmpResults.variables.clear();
        tmpResults.constraints.clear();

        if (cmplInst->varRealIdxListSize()>0) {
            realVarSection=true;
            realIdx=0;
            intIdx=-1;
        }
        else {
            realVarSection=false;
            realIdx=0;
            intIdx=0;
        }
        QFile cFOptFile (gSolName);

        if (!cFOptFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
            setCfOsMessage("Can't open GLPK Solution file: " + gSolName);
            return false;
        }

        while (!cFOptFile.atEnd()) {

            if (cFOptFile.error()>0) {
                setCfOsMessage("Can't read GLPK Solution file: " + gSolName);
                return false;
            }

            QString line = cFOptFile.readLine();
            QStringList solList = line.split(" ", QString::SkipEmptyParts);
            lineCounter++;

            if (lineCounter==1) {
                resultStatus.nrOfConstraints=solList[0].toLong()-1;
                resultStatus.nrOfVars=solList[1].toLong();

                for (int i=0; i<resultStatus.nrOfConstraints; i++ ) {
                    tmpValues.osIdx=0;
                    tmpValues.activity=0;
                    tmpValues.hasMarginal=false;
                    tmpValues.marginal=0;
                    tmpResults.constraints.append(tmpValues);
                }

                for (int j=0; j<resultStatus.nrOfVars; j++ ) {
                    tmpValues.osIdx=0;
                    tmpValues.activity=0;
                    tmpValues.hasMarginal=false;
                    tmpValues.marginal=0;
                    tmpResults.variables.append(tmpValues);
                }
                continue;
            }

            if (lineCounter==2) {
                if (solList[0]=="2" || solList[0]=="5"  )
                    resultStatus.generalStatus="normal";
                 else if (solList[0]=="1")
                    resultStatus.generalStatus="undefined";
                 else
                    resultStatus.generalStatus="infeasible";

                 if (resultStatus.generalStatus=="normal") {
                     tmpResults.status="optimal";
                     resultStatus.nrOfSolutions=1;
                 }
                 else {
                     tmpResults.status="";
                     resultStatus.nrOfSolutions=0;
                 }

                 if (solList.length()==3)
                     tmpResults.objValue=solList[2].toDouble();
                 else
                     tmpResults.objValue=solList[1].toDouble();

                 continue;
            }

            //constraints
            if (lineCounter>3 && lineCounter<=resultStatus.nrOfConstraints+3) {
               tmpResults.constraints[row].osIdx=row;
                if (solList.length()==1) {
                    tmpResults.constraints[row].activity=solList[0].toDouble();
                }  else if (solList.length()==2 ) {
                    tmpResults.constraints[row].activity=solList[0].toDouble();
                    tmpResults.constraints[row].marginal=solList[1].toDouble();
                    tmpResults.constraints[row].hasMarginal=true;
                }  else if (solList.length()==3 ) {
                    tmpResults.constraints[row].activity=solList[1].toDouble();
                    tmpResults.constraints[row].marginal=solList[2].toDouble();
                    tmpResults.constraints[row].hasMarginal=true;
                }
                row++;
                continue;
            }

            //variables
            if (lineCounter>resultStatus.nrOfConstraints+3 && lineCounter<=resultStatus.nrOfConstraints+3+resultStatus.nrOfVars ) {
               if (realVarSection)
                    col=cmplInst->varRealIdx(realIdx);
                else
                    col=cmplInst->varIntIdx(intIdx);

                tmpResults.variables[col].osIdx=col;
                if (solList.length()==1) {
                    tmpResults.variables[col].activity=solList[0].toDouble();
                }  else if (solList.length()==2 ) {
                    tmpResults.variables[col].activity=solList[0].toDouble();
                    tmpResults.variables[col].marginal=solList[1].toDouble();
                    tmpResults.variables[col].hasMarginal=true;
                }  else if (solList.length()==3 ) {
                    tmpResults.variables[col].activity=solList[1].toDouble();
                    tmpResults.variables[col].marginal=solList[2].toDouble();
                    tmpResults.variables[col].hasMarginal=true;
                }
                colCount++;
                if (colCount<cmplInst->varRealIdxListSize())
                    realIdx++;
                else {
                    realVarSection=false;
                    intIdx++;
                }

                continue;
            }

        }
        results.append(tmpResults);
        cFOptFile.close();

        return true;

    } // end readGlpkSolFile

    /* ************** readCplexSolFile ************ */
    bool cfResult::readCplexSolFile() {

        int row=0;
        int col=0;
        int colCount=0;

        int realIdx;
        int intIdx;

        bool realVarSection;

        resultStatus.serviceName="CPLEX";
        resultStatus.solverName="CPLEX";

        resultStatus.instanceName=problemName;
        resultStatus.message="";

        cfModelResults tmpResults;
        cfResultValues tmpValues;

        tmpResults.status="";
        tmpResults.objValue=0;
        tmpResults.variables.clear();
        tmpResults.constraints.clear();

        if (cmplInst->varRealIdxListSize()>0) {
            realVarSection=true;
            realIdx=0;
            intIdx=-1;
        }
        else {
            realVarSection=false;
            realIdx=0;
            intIdx=0;
        }

        resultStatus.nrOfConstraints=cmplInst->rowCount()-1;
        resultStatus.nrOfVars=cmplInst->colCount();

        for (int i=0; i<resultStatus.nrOfConstraints; i++ ) {
            tmpValues.osIdx=0;
            tmpValues.activity=0;
            tmpValues.hasMarginal=false;
            tmpValues.marginal=0;
            tmpResults.constraints.append(tmpValues);
        }

        for (int j=0; j<resultStatus.nrOfVars; j++ ) {
            tmpValues.osIdx=0;
            tmpValues.activity=0;
            tmpValues.hasMarginal=false;
            tmpValues.marginal=0;
            tmpResults.variables.append(tmpValues);
        }


        QDomDocument doc("myCplexSol");
        QFile file(cSolName);
        if (!file.open(QIODevice::ReadOnly)) {
            setCfOsMessage("File could not be open >>" +cSolName) ;
            if (cmplInst->varIntIdxListSize()>0) {
                results.append(tmpResults);
                return true;
            } else
                return false;
        }
        if (!doc.setContent(&file)) {
            file.close();
            setCfOsMessage("xml error >>" + cSolName );
            return false;
        }
        file.close();


        QDomElement docElem = doc.documentElement();

        if ( docElem.tagName() != "CPLEXSolution" ) {
            setCfOsMessage("xml file >>" + gSolName  + " is not an CPLEX solution file");
            return false;
        }

        QDomNode child1 = docElem.firstChild();
        while(!child1.isNull()) {

            QDomElement elemChild1 = child1.toElement();
            QDomNode child2 = child1.firstChild();

            if ( elemChild1.tagName()=="header" ) {
                resultStatus.nrOfSolutions=1;
                resultStatus.generalStatus=elemChild1.attribute("solutionStatusString");
                if (resultStatus.generalStatus.contains("optimal")) {
                    resultStatus.nrOfSolutions=1;
                    tmpResults.objValue=elemChild1.attribute("objectiveValue").toDouble();
                    tmpResults.status="optimal";

                } else
                    resultStatus.nrOfSolutions=0;
            }


            if ( elemChild1.tagName()=="linearConstraints" ) {

                while(!child2.isNull()) {
                    QDomElement elemChild2 = child2.toElement();

                    row = elemChild2.attribute("index").toInt();

                    if (elemChild2.attribute("dual","nn")!="nn") {
                        tmpResults.constraints[row].marginal=elemChild2.attribute("dual").toDouble();
                        tmpResults.constraints[row].hasMarginal=true;
                    }

                    if (elemChild2.attribute("slack").toDouble()<0)
                        tmpResults.constraints[row].activity=constraintLowerBound(row) + elemChild2.attribute("slack").toDouble() *-1 ;

                    if (elemChild2.attribute("slack").toDouble()>0)
                        tmpResults.constraints[row].activity=constraintUpperBound(row) - elemChild2.attribute("slack").toDouble() ;

                    if (elemChild2.attribute("slack").toDouble()==0) {
                        if (constraintHasLowerBound(row))
                            tmpResults.constraints[row].activity=constraintLowerBound(row) ;
                        else if (constraintHasUpperBound(row))
                            tmpResults.constraints[row].activity=constraintUpperBound(row);
                    }

                    child2=child2.nextSibling();
                }
            }

            if ( elemChild1.tagName()=="variables" ) {

                while(!child2.isNull()) {

                    if (realVarSection)
                         col=cmplInst->varRealIdx(realIdx);
                     else
                         col=cmplInst->varIntIdx(intIdx);

                    QDomElement elemChild2 = child2.toElement();

                   // col = elemChild2.attribute("index").toInt();

                    if (elemChild2.attribute("reducedCost","nn")!="nn") {
                        tmpResults.variables[col].marginal=elemChild2.attribute("reducedCost").toDouble();
                        tmpResults.variables[col].hasMarginal=true;
                    }

                    tmpResults.variables[col].activity=elemChild2.attribute("value").toDouble();

                    child2=child2.nextSibling();

                    colCount++;
                    if (colCount<cmplInst->varRealIdxListSize())
                        realIdx++;
                    else {
                        realVarSection=false;
                        intIdx++;
                    }
                }
            }


            child1 = child1.nextSibling();
        }
        results.append(tmpResults);
        return true;
    }//end readCplexSolFile



    /* ************** cfReadOsrl ************ */
    bool cfResult::cfReadOsrl() {

        if (osrlName.isEmpty()) {
            setCfOsMessage("OSrL name is not set");
            return false;
        }

        QDomDocument doc("myOsrl");
        QFile file(osrlName);
        if (!file.open(QIODevice::ReadOnly)) {
            setCfOsMessage("File could not be open >>" + osrlName) ;
            return false;
        }
        if (!doc.setContent(&file)) {
            file.close();
            setCfOsMessage("xml error >>" + osrlName );
            return false;
        }
        file.close();

        cfModelResults tmpResults;
        cfResultValues tmpValues;

        QDomElement docElem = doc.documentElement();

        if ( docElem.tagName() != "osrl" ) {                                                                            //<osrl>
            setCfOsMessage("xml file >>" + osrlName  + " is not an osrl file");
            return false;
        }

        QDomNode child1 = docElem.firstChild();
        while(!child1.isNull()) {

            QDomElement elemChild1 = child1.toElement();

            QDomNode child2 = child1.firstChild();

            if ( elemChild1.tagName()=="general" ) {                                                                        //<general>

                while(!child2.isNull()) {                                                                                      //<generalStatus>
                    QDomElement elemChild2 = child2.toElement();
                    //if (elemChild2.tagName()=="generalStatus") resultStatus.generalStatus= elemChild2.attribute("type");
                    if (elemChild2.tagName()=="serviceName") resultStatus.serviceName=elemChild2.text();
                    if (elemChild2.tagName()=="instanceName") resultStatus.instanceName=elemChild2.text();
                    if (elemChild2.tagName()=="solverInvoked")  resultStatus.solverName=elemChild2.text();
                    if (elemChild2.tagName()=="message") resultStatus.message=elemChild2.text();

                    child2=child2.nextSibling();
                }                                                                                                             //</generalStatus>
            }                                                                                                               //</general>

            if ( elemChild1.tagName()=="optimization" ) {                                                                   //<optimization>
                resultStatus.nrOfSolutions=elemChild1.attribute("numberOfSolutions").toInt();
                resultStatus.nrOfVars=elemChild1.attribute("numberOfVariables").toLong();
                resultStatus.nrOfConstraints=elemChild1.attribute("numberOfConstraints").toLong();

                QDomNode child2 = child1.firstChild();

                while(!child2.isNull()) {

                    QDomElement elemChild2 = child2.toElement();

                    if ( elemChild2.tagName()=="solution" )  {                                                                  //<solution>

                        tmpResults.status="";
                        tmpResults.objValue=0;
                        tmpResults.variables.clear();
                        tmpResults.constraints.clear();

                        for (int i=0; i<resultStatus.nrOfVars; i++){
                            tmpValues.osIdx=i;
                            tmpValues.activity=0;
                            tmpValues.hasMarginal=false;
                            tmpValues.marginal=0;
                            tmpResults.variables.append(tmpValues);

                        }

                        for (int i=0; i<resultStatus.nrOfConstraints; i++){
                            tmpValues.osIdx=i;
                            tmpValues.activity=0;
                            tmpValues.hasMarginal=false;
                            tmpValues.marginal=0;
                            tmpResults.constraints.append(tmpValues);

                        }

                        QDomNode child3 = child2.firstChild();

                        while(!child3.isNull()) {

                            QDomElement elemChild3 = child3.toElement();

                            if ( elemChild3.tagName()=="status" )   {                                                       //<status ... />
                                resultStatus.generalStatus=elemChild3.attribute("type");
                                //tmpResults.status=elemChild3.attribute("type");
                                if (resultStatus.generalStatus.toLower()=="infeasible") {
                                    resultStatus.message=elemChild3.attribute("description");
                                    resultStatus.nrOfSolutions=0;
                                }

                            }
                            if ( elemChild3.tagName()=="variables" )  {                                                         //<variables>

                                QDomNode child4 = child3.firstChild();
                                while(!child4.isNull()) {

                                    QDomElement elemChild4 = child4.toElement();

                                    if ( elemChild4.tagName()=="values" ) {                                                         //<values>

                                        QDomNode child5 = child4.firstChild();

                                        while(!child5.isNull()) {                                                                   //<var>
                                            QDomElement elemChild5 = child5.toElement();

                                            if ( elemChild5.tagName()=="var" ) {

                                                tmpValues.osIdx=elemChild5.attribute("idx").toLong();
                                                tmpResults.variables[tmpValues.osIdx].osIdx=tmpValues.osIdx;
                                                tmpResults.variables[tmpValues.osIdx].activity=elemChild5.text().toDouble();
                                            }
                                            child5=child5.nextSibling();
                                        }                                                                                           //</var>
                                    }                                                                                              //</values>

                                    if ( (elemChild4.tagName()=="other") && (elemChild4.attribute("name")=="reduced costs")) {     //<others>

                                        QDomNode child5 = child4.firstChild();

                                        while(!child5.isNull()) {                                                                       //<var>
                                            QDomElement elemChild5 = child5.toElement();


                                            if ( elemChild5.tagName()=="var" ) {

                                                tmpValues.osIdx=elemChild5.attribute("idx").toLong();
                                                tmpResults.variables[tmpValues.osIdx].hasMarginal=true;
                                                tmpResults.variables[tmpValues.osIdx].marginal=elemChild5.text().toDouble();

                                            }
                                            child5=child5.nextSibling();
                                        }                                                                                               //</var>
                                    }                                                                                               //</others
                                    child4=child4.nextSibling();
                                }

                            }                                                                                                   //</variables>

                            if ( elemChild3.tagName()=="objectives" )  {                                                        //<objectives>

                                QDomNode child4 = child3.firstChild();
                                while(!child4.isNull()) {                                                                           //<values>

                                    QDomElement elemChild4 = child4.toElement();

                                    if ( elemChild4.tagName()=="values" ) {
                                        QDomNode child5 = child4.firstChild();
                                        int i=0;
                                        while(!child5.isNull()) {                                                                       //<obj>
                                            QDomElement elemChild5 = child5.toElement();

                                            if ( ( elemChild5.tagName()=="obj" ) && (i<1)) {
                                                tmpResults.objValue=elemChild5.text().toDouble();
                                                i++;
                                            }

                                            child5=child5.nextSibling();
                                        }                                                                                               //</obj>
                                    }
                                    child4=child4.nextSibling();
                                }                                                                                                    //</values>
                            }                                                                                                   //</objectives>

                            if ( elemChild3.tagName()=="constraints" )  {                                                       //<constraints>

                                QDomNode child4 = child3.firstChild();                                                               //<dualValues>
                                while(!child4.isNull()) {

                                    QDomElement elemChild4 = child4.toElement();

                                    if ( elemChild4.tagName()=="dualValues" ) {
                                        QDomNode child5 = child4.firstChild();

                                        while(!child5.isNull()) {                                                                       //<con>
                                            QDomElement elemChild5 = child5.toElement();

                                            if ( elemChild5.tagName()=="con" ) {
                                                tmpValues.osIdx=elemChild5.attribute("idx").toLong();

                                                tmpResults.constraints[tmpValues.osIdx].osIdx=tmpValues.osIdx;
                                                tmpResults.constraints[tmpValues.osIdx].activity=0;
                                                tmpResults.constraints[tmpValues.osIdx].marginal=elemChild5.text().toDouble();
                                                if (tmpResults.constraints[tmpValues.osIdx].marginal==0)
                                                    tmpResults.constraints[tmpValues.osIdx].hasMarginal=false;
                                                else
                                                    tmpResults.constraints[tmpValues.osIdx].hasMarginal=true;
                                            }
                                            child5=child5.nextSibling();
                                        }                                                                                               //</con>
                                    }
                                    child4=child4.nextSibling();
                                }                                                                                                   //</dualValues>
                            }
                            child3=child3.nextSibling();
                        }
                        results.append(tmpResults);
                    }                                                                                                           //</solution>
                    child2=child2.nextSibling();
                }
            }                                                                                                               //</optimization>
            child1 = child1.nextSibling();
        }                                                                                                           //</osrl>
        return true;
    }//end cfReadOsrl



    /* ************** readInstance ************ */
    void cfResult::readInstance() {

        int n, col;
        cfConstantsValues tmpValues;

        modelNames.instanceName=problemName;
        modelNames.description="";
        modelConstants.nrOfVariables=cmplInst->colCount();
        modelConstants.nrOfConstraints=cmplInst->rowCount()-1;

        modelNames.objName=QString::fromStdString(cmplInst->objName());
        modelNames.objSense=QString::fromStdString(cmplInst->objSense());

        for (col=0; col<cmplInst->columnsListSize(); col++) {

            modelNames.varNames.append(QString::fromStdString(cmplInst->varName(col)));
            tmpValues.type=QString::fromStdString(cmplInst->varType(col));
            tmpValues.hasLB=cmplInst->varHasLowerBound(col);
            if (!CmplTools::stringToDouble( cmplInst->varLowerBound(col),  tmpValues.lowerBound ) )
                tmpValues.lowerBound=0;
            tmpValues.hasUB=cmplInst->varHasUpperBound(col);
            if (!CmplTools::stringToDouble( cmplInst->varUpperBound(col), tmpValues.upperBound) )
                tmpValues.upperBound=0;

            modelConstants.variables.append(tmpValues);
        }

        for (n=0; n<cmplInst->conListSize(); n++) {

            //if (cmplInst->conType(n)!=MODE_N) {

                modelNames.conNames.append( QString::fromStdString(cmplInst->conName(n)) );

                tmpValues.type="";

                tmpValues.hasLB=false;
                tmpValues.lowerBound=0;
                tmpValues.hasUB=false;
                tmpValues.upperBound=0;

                switch(cmplInst->conType(n))

                {
                case MODE_G:
                    tmpValues.hasLB=true;
                    if (!CmplTools::stringToDouble(cmplInst->conLowerBound(n), tmpValues.lowerBound))
                          tmpValues.lowerBound=0;
                    tmpValues.hasUB=true;
                    if (!CmplTools::stringToDouble(cmplInst->conUpperBound(n), tmpValues.upperBound))
                        tmpValues.upperBound=0;
                    break;
                case MODE_U:
                    tmpValues.hasLB=true;
                    if (!CmplTools::stringToDouble(cmplInst->conLowerBound(n), tmpValues.lowerBound))
                          tmpValues.lowerBound=0;
                    break;
                case MODE_O:
                    tmpValues.hasUB=true;
                    if (!CmplTools::stringToDouble(cmplInst->conUpperBound(n), tmpValues.upperBound))
                        tmpValues.upperBound=0;
                    break;
                case MODE_N:
                    break;
                }

                if ( tmpValues.hasLB && tmpValues.hasUB ) tmpValues.type="E";
                if ( !tmpValues.hasLB && tmpValues.hasUB ) tmpValues.type="L";
                if ( tmpValues.hasLB && !tmpValues.hasUB ) tmpValues.type="G";

                modelConstants.constraints.append(tmpValues);
           // }

       }

    }



    /* ************** calculateConstraintCoefficients ************ */
    void cfResult::calculateConstraintCoefficients() {

        long tmpCol=0;
        long tmpRow=0;
        double tmpValue;
        int j, n, n1;

        for ( j=0; j < resultStatus.nrOfSolutions; j++) {
           for ( n=0; n<cmplInst->varListSize(); n++) {
                for (  n1=0; n1<cmplInst->columnsCoefListSize(n) ; n1++) {
                    tmpRow=cmplInst->colCoefRowIdx(n,n1);
                    CmplTools::stringToDouble(cmplInst->colCoef(n,n1), tmpValue);
                    results[j].constraints[tmpRow].activity += tmpValue * results[j].variables[n].activity ;
                }
            }
        }

    }//end calculateConstraintCoefficients



    /* ************** getVarNameIndex ************ */
    long cfResult::getVarNameIndex(string varName) {
        QString tmpName = QString::fromStdString(varName);
        return modelNames.varNames.indexOf(tmpName);
    }//end getVarNameIndex


    /* ************** getConstraintNameIndex ************ */
    long cfResult::getConstraintNameIndex(string conName) {
        QString tmpName = QString::fromStdString(conName);
        return modelNames.conNames.indexOf(tmpName);
    }//end getConstraintNameIndex


    /* ************** varType ************ */
    string cfResult::varType(string varName) {
        long index=getVarNameIndex(varName);
        return modelConstants.variables[index].type.toStdString();
    }//end varType


    /* ************** varHasLowerBound ************ */
    bool cfResult::varHasLowerBound(string varName) {
        long index=getVarNameIndex(varName);
        return modelConstants.variables[index].hasLB;
    }//end varHasLowerBound


    /* ************** varLowerBound ************ */
    double cfResult::varLowerBound(string varName) {
        long index=getVarNameIndex(varName);
        return modelConstants.variables[index].lowerBound;
    }//end varLowerBound


    /* ************** varHasUpperBound ************ */
    bool cfResult::varHasUpperBound(string varName) {
        long index=getVarNameIndex(varName);
        return modelConstants.variables[index].hasUB;
    }//end varHasUpperBound


    /* ************** varUpperBound ************ */
    double cfResult::varUpperBound(string varName) {
        long index=getVarNameIndex(varName);
        return modelConstants.variables[index].upperBound;
    }//end varUpperBound



    /* ************** varActivity ************ */
    double cfResult::varActivity(int solutionNumber, string varName) {
        long index=getVarNameIndex(varName);
        return results[solutionNumber].variables[index].activity;
    }//end varActivity


    /* ************** varHasMarginal ************ */
    bool cfResult::varHasMarginal(int solutionNumber, string varName) {
        long index=getVarNameIndex(varName);
        return  results[solutionNumber].variables[index].hasMarginal;
    }//end varHasMarginal


    /* ************** varMarginal ************ */
    double cfResult::varMarginal(int solutionNumber, string varName) {
        long index=getVarNameIndex(varName);
        return  results[solutionNumber].variables[index].marginal;
    }//end varMarginal


    /* ************** constraintType ************ */
    string cfResult::constraintType(string conName) {
        long index=getConstraintNameIndex(conName);
        return modelConstants.constraints[index].type.toStdString();
    }//end constraintType


    /* ************** constraintHasLowerBound ************ */
    bool cfResult::constraintHasLowerBound(string conName) {
        long index=getConstraintNameIndex(conName);
        return modelConstants.constraints[index].hasLB;
    }//end constraintHasLowerBound


    /* ************** constraintLowerBound ************ */
    double cfResult::constraintLowerBound(string conName) {
        long index=getConstraintNameIndex(conName);
        return modelConstants.constraints[index].lowerBound;
    }//end constraintLowerBound


    /* ************** constraintHasUpperBound ************ */
    bool cfResult::constraintHasUpperBound(string conName) {
        long index=getConstraintNameIndex(conName);
        return modelConstants.constraints[index].hasUB;
    }//end constraintHasUpperBound



    /* ************** constraintUpperBound ************ */
    double cfResult::constraintUpperBound(string conName) {
        long index=getConstraintNameIndex(conName);
        return modelConstants.constraints[index].upperBound;
    }//end constraintUpperBound


    /* ************** constraintActivity ************ */
    double cfResult::constraintActivity(int solutionNumber, string conName) {
        long index=getConstraintNameIndex(conName);
        return results[solutionNumber].constraints[index].activity;
    }//end constraintActivity


    /* ************** constraintHasMarginal ************ */
    bool cfResult::constraintHasMarginal(int solutionNumber, string conName) {
        long index=getConstraintNameIndex(conName);
        return results[solutionNumber].constraints[index].hasMarginal;
    }//end constraintHasMarginal


    /* ************** constraintMarginal ************ */
    double cfResult::constraintMarginal(int solutionNumber, string conName) {
        long index=getConstraintNameIndex(conName);
        return results[solutionNumber].constraints[index].marginal;
    }//end constraintMarginal




}
