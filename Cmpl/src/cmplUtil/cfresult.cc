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
    if ( fname.toLower().contains(".cmpl")) {
        setCfOsMessage("Please define the problemname w/o any file extensions");
        return false;
    }

    glvar=m;
    cmplInst=c;

    csvName=QString::fromStdString(glvar->args->csvName());
    solName=QString::fromStdString(glvar->args->asciiName());
    gSolName=QString::fromStdString(glvar->args->gsolFile());

    if (!glvar->args->remoteMode())
        cSolName=QString::fromStdString(glvar->args->cmplSolName());
    else
        cSolName=fname+".csol";

    problem=fname;
    problemName=fname+".cmpl";
    return true;

}//end setProblemName


/* ************** getCfOsMessage ************ */
string cfResult::getCfOsMessage() {
    return cfOsMessage.toStdString();
}//end getCfOsMessage


/* ************** getResult ************ */
bool cfResult::getResult() {

    readInstance();
    if (glvar->args->solverName()=="glpk" || glvar->args->solverName()=="cbc" || glvar->args->solverName().empty() ) {

        if (!readGSolFile()) return false;

    } else if (glvar->args->solverName()=="gurobi") {

        if (!readGurobiSolFile()) return false;
        if (glvar->args->solutionPool())
            calculateConstraintCoefficients();

    }
    else if (glvar->args->solverName()=="scip" ) {
        if (!readScipSolFile()) return false;
        calculateConstraintCoefficients();

    } else if (glvar->args->solverName()=="cplex" ) {
        if (!readCplexSolFile()) return false;

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

    //cout << setw(21) << left << "Problem  " << instanceName() << endl;
    cout << setw(21) << left << "Problem  " <<  QFileInfo(QString::fromStdString(instanceName())).fileName().toStdString() << endl;

    cout << setw(21) << left <<  "Nr. of variables" << nrOfVariables() << endl;
    cout << setw(21) << left <<  "Nr. of constraints" << nrOfConstraints() << endl;
    cout << setw(21) << left <<  "Objective name   " << objectiveName() << endl;
    if (glvar->args->solutionPool()) {
        cout << setw(21) << left <<  "Nr. of solutions " << nrOfSolutions() << endl;
    }
    cout << setw(21) << left << "Solver name" << solverName() << endl;


    cout << setw(21) << left << "Display variables" ;

    if (glvar->args->ignoreVars())
        cout << "ignore all variables ";
    else {

        if (glvar->args->ignoreZeros())
            cout << "nonzero variables ";

        cout << "(";
        if (glvar->args->displayVarListSize()==0)
            cout << "all";
        else {
            for (int j=0; j<glvar->args->displayVarListSize(); j++) {
                if (j>0) cout << ",";
                cout << glvar->args->displayVarList(j);
            }
        }
        cout << ")";
    }
    cout << endl;


    cout << setw(21) << left << "Display constraints" ;

    if (glvar->args->ignoreCons())
        cout << "ignore all constraints ";
    else {
        if (glvar->args->ignoreZeros())
            cout << "nonzero constraints ";

        cout << "(";
        if(glvar->args->displayConListSize()==0)
            cout << "all";
        else {
            for (int j=0; j<glvar->args->displayConListSize(); j++) {
                if (j>0) cout << ",";
                cout << glvar->args->displayConList(j);
            }
        }
        cout << ")";
    }


    cout << endl;


    cout << setw(105) << setfill('-') << "" <<endl;
    cout <<  setfill(' ');



}//end writeReportHeader


/* ************** writeVarHeader ************ */
void cfResult::writeVarHeader(long n){
    int n1;

    if (glvar->args->solutionPool() && glvar->args->solverName()=="gurobi")
        n1=nrOfSolutions()-n-1;
    else
        n1=n;

    cout << endl;
    if (glvar->args->solutionPool()) {
        cout << setw(21) << left <<  "Solution nr. " << n1+1 << endl;
    }
    cout << setw(21) << left <<  "Objective status " << objectiveStatus(n) << endl;
    cout << setw(21) << left <<  "Objective value  " << objectiveValue(n) << " (" << objectiveSense() << "!)" << endl;
    cout <<endl;


    cout << "Variables " <<  endl;

    cout << setw(20) << left <<  "Name";
    cout << setw(5) << right <<  "Type";
    cout << setw(20) << right <<   "Activity";
    cout << setw(20) << right <<  "Lower bound";
    cout << setw(20) << right <<  "Upper bound";
    cout << setw(20) << right <<  "Marginal" << endl;
    cout << setw(105) << setfill('-') << "" <<endl;
    cout <<  setfill(' ');
}//end writeVarHeader


/* ************** writeVarValues ************ */
void cfResult::writeVarValues(long n, long i) {

    cout << setw(20) <<   left <<  varName(i);
    cout << setw(5) <<  right << varType(i);
    cout << setw(20) << right << ( varType(i)=="C" ? resultValue(varActivity(n,i)) : round(resultValue(varActivity(n,i))));

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
    cout << setw(105) << setfill('-') << "" <<endl;
    cout <<  setfill(' ');
}//end writeVarFooter


/* ************** writeConHeader ************ */
void cfResult::writeConHeader() {

    cout <<endl;

    cout << "Constraints " <<  endl;
    cout << setw(20) << left <<  "Name";
    cout << setw(5) << right <<  "Type";
    cout << setw(20) << right <<   "Activity";
    cout << setw(20) << right <<  "Lower bound";
    cout << setw(20) << right <<  "Upper bound";
    cout << setw(20) << right <<  "Marginal" << endl;
    cout << setw(105) << setfill('-') << "" <<endl;
    cout <<  setfill(' ');

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
    cout << setw(105) << setfill('-') << "" <<endl;
    cout <<  setfill(' ');
}//end writeReportFooter


/* ************** standardReport ************ */
bool cfResult::standardReport(int outType) {

    int n1;

    CmplFileType report;
    report.setFileName(solName.toStdString());
    if (outType==1)
        report.setOutMode(STD_OUT);
    else
        report.setOutMode(FILE_OUT);

    if (!report.open()) {
        setCfOsMessage("Internal Error - Can not write the solution file <" + solName +">\n" );
        return false;
    }

    writeReportHeader();

    for (int n=0; n < nrOfSolutions(); n++)   {

        if (glvar->args->solutionPool() && glvar->args->solverName()=="gurobi")
            n1=nrOfSolutions()-n-1;
        else
            n1=n;


        if (!glvar->args->ignoreVars()) {
            writeVarHeader(n1);
            if (glvar->args->displayVarListSize()==0) {
                for (long i=0; i < nrOfVariables(); i++) {
                    if( !(glvar->args->ignoreZeros() && resultValue(varActivity(n1,i))==0) ) {
                        writeVarValues(n1,i);
                    }
                }

            } else {
                for (int j=0; j<glvar->args->displayVarListSize(); j++ ) {
                    for (long i=0; i < nrOfVariables(); i++) {
                        string tmpStr=glvar->args->displayVarList(j);
                        int starPos=tmpStr.find_first_of("*");
                        tmpStr=tmpStr.substr(0,starPos);
                        if ( (starPos>-1 && CmplTools::stringStartsWith(varName(i),tmpStr)) || (starPos==-1 && varName(i)==tmpStr) ) {
                            if( !(glvar->args->ignoreZeros() && resultValue(varActivity(n1,i))==0) ) {
                                writeVarValues(n1,i);
                            }
                        }
                    }
                }
            }
            writeVarFooter();
        }


        if (!glvar->args->ignoreCons()) {

            writeConHeader();
            if (glvar->args->displayConListSize()==0) {
                for (long i=0; i < nrOfConstraints(); i++) {
                    if( !(glvar->args->ignoreZeros() && resultValue(constraintActivity(n1,i))==0)) {
                        writeConValues(n1,i);
                    }
                }
            } else {

                for (int j=0; j<glvar->args->displayConListSize(); j++ ) {
                    for (long i=0; i < nrOfConstraints(); i++) {
                        string tmpStr=glvar->args->displayConList(j);
                        int starPos=tmpStr.find_first_of("*");
                        tmpStr=tmpStr.substr(0,starPos);
                        if ( (starPos>-1 && CmplTools::stringStartsWith(constraintName(i),tmpStr)) || (starPos==-1 && constraintName(i)==tmpStr) ) {
                            if( !(glvar->args->ignoreZeros() && resultValue(constraintActivity(n1,i))==0)) {
                                writeConValues(n1,i);
                            }
                        }
                    }
                }
            }
            writeReportFooter();
        }
    }
    report.close();
    return true;
}//end standardReport


/* ************** writeReportHeader ************ */
void cfResult::writeCmplSolutionHeader() {

    string cmplName = QFileInfo(QString::fromStdString(instanceName())).fileName().toStdString();
    /*
    size_t i;
    i=instanceName().find_last_of("/");
    if (i==string::npos)
         i=instanceName().find_last_of("\\");
    if (i!=string::npos && glvar->args->remoteMode())
        cmplName=instanceName().substr(i+1);
    else
        cmplName=instanceName();
    */


    cout.precision( glvar->args->maxDecimals()+1 );
    cout << "<?xml version = \"1.1\" encoding=\"UTF-8\" standalone=\"yes\"?>" << endl;
    cout << "<CmplSolutions version=\"1.0\">" << endl;
    cout << "   <general>" << endl;
    cout << "       <instanceName>" <<  cmplName << "</instanceName>" << endl;
    cout << "       <nrOfVariables>" << nrOfVariables() << "</nrOfVariables>" << endl;
    cout << "       <nrOfConstraints>" << nrOfConstraints() << "</nrOfConstraints>" << endl;
    cout << "       <objectiveName>" << objectiveName() << "</objectiveName>" << endl;
    cout << "       <objectiveSense>" << objectiveSense() << "</objectiveSense>" << endl;
    cout << "       <nrOfSolutions>" << nrOfSolutions() << "</nrOfSolutions>" <<  endl;

    if (!glvar->args->solverService().empty()) {
         cout << "       <solverName>" << solverName() << " (OSServer)" << "</solverName>" << endl;
         cout << "       <solverUrl>" << glvar->args->solverService() << "</solverUrl>" << endl;
    }
    else
         cout << "       <solverName>" << solverName() << "</solverName>" << endl;

    if (nrOfSolutions()==0)
        cout << "       <solverMsg>" <<  generalStatus() << "</solverMsg>" << endl;

    cout << "       <variablesDisplayOptions>" ;
    if (glvar->args->ignoreVars())
        cout << "ignoreAllVariables ";
    else {
        if (glvar->args->ignoreZeros())
            cout << "nonZeroVariables ";

        cout << "(";
        if(glvar->args->displayVarListSize()==0)
            cout << "all";
        else {
            for (int j=0; j<glvar->args->displayVarListSize(); j++){
                if (j>0) cout << ",";
                cout << glvar->args->displayVarList(j);
            }
        }
        cout << ")";
    }
    cout  << "</variablesDisplayOptions>" << endl;


    cout << "       <constraintsDisplayOptions>" ;
    if (glvar->args->ignoreCons())
        cout << "ignoreAllConstraints ";
    else {
        if (glvar->args->ignoreZeros())
            cout << "nonZeroConstraints ";
        cout << "(";
        if(glvar->args->displayConListSize()==0)
            cout << "all";
        else {
            for (int j=0; j<glvar->args->displayConListSize(); j++) {
                if (j>0) cout << ",";
                cout << glvar->args->displayConList(j);
            }
        }
        cout << ")";
    }
    cout << "</constraintsDisplayOptions>" <<endl;

    if (glvar->args->remoteMode()) {
        if (glvar->args->cmplSolOut()) {
            cout  << "       <cmplSolFile>" ;
            if ( glvar->args->cmplSolName() == cSolName.toStdString() )
                cout << "cmplStandard" ;
            else
                cout << glvar->args->cmplSolName() ;
            cout <<  "</cmplSolFile>"  <<endl;
        }

        if (glvar->args->asciiOut()) {
            cout  << "       <asciiSolFile>" ;
            if (glvar->args->asciiName() ==  (problem+".sol").toStdString() )
                cout << "cmplStandard" ;
            else
                cout << glvar->args->asciiName() ;
            cout << "</asciiSolFile>" <<endl ;
        }
        if (glvar->args->csvOut()) {
            cout  << "       <csvSolFile>" ;
            if (glvar->args->csvName() ==  (problem+".csv").toStdString() )
                cout << "cmplStandard" ;
            else
                cout << glvar->args->csvName();
            cout  << "</csvSolFile>" <<endl;
        }
    }
    cout << "   </general>" << endl;
}//end writeCmplSolutionHeader

/* ************** writeCmplSolutionSolHeader ************ */
void cfResult::writeCmplSolutionSolHeader(long n){

    int n1;
    if (glvar->args->solutionPool() && glvar->args->solverName()=="gurobi")
        n1=nrOfSolutions()-n-1;
    else
        n1=n;

//    cout << "   <solution idx=\""<< n1+1 << "\" status=\""<<  objectiveStatus(n) << "\" value=\"" << objectiveValue(n) << "\">" << endl;
    cout << "   <solution idx=\""<< n1 << "\" status=\""<<  objectiveStatus(n) << "\" value=\"" << objectiveValue(n) << "\">" << endl;

}//end writeCmplSolutionSolHeader

/* ************** writeCmplSolutionVarHeader ************ */
void cfResult::writeCmplSolutionVarHeader(){
    cout << "       <variables>"<< endl;
}//end writeCmplSolutionVarHeader


/* ************** writeCmplSolutionVarValues ************ */
void cfResult::writeCmplSolutionVarValues(long n, long i) {

   // cout << "           <variable idx=\""<< i << "\" name=\""<< varName(i) << "\" type=\"" << varType(i) << "\" activity=\"" << resultValue(varActivity(n,i)) << "\"";
    cout << "           <variable idx=\""<< i << "\" name=\""<< varName(i) << "\" type=\"" << varType(i) << "\" activity=\"" << ( varType(i)=="C" ? resultValue(varActivity(n,i)) : round(resultValue(varActivity(n,i)))) << "\"";

    cout <<  " lowerBound=\"";
    if (varHasLowerBound(i))
        cout << varLowerBound(i);
    else
        cout <<  "-INF";
        //cout <<  "-Infinity";


    cout << "\" upperBound=\"";
    if (varHasUpperBound(i))
        cout << varUpperBound(i);
    else
        cout <<  "INF";
       // cout << "Infinity";

    cout << "\" marginal=\"";
    if (varHasMarginal(n,i))
        cout <<  resultValue(varMarginal(n,i));
    else
        cout << "NaN" ;
        //cout << "-" ;
    cout  << "\"/>" << endl;

}//end writeCmplSolutionVarValues

/* ************** writeCmplSolutionVarHeader ************ */
void cfResult::writeCmplSolutionVarFooter() {
    cout << "       </variables>" << endl;
}//end writeCmplSolutionVarHeader


/* ************** writeCmplSolutionConHeader ************ */
void cfResult::writeCmplSolutionConHeader() {
    cout << "       <linearConstraints> " << endl;
}//end writeCmplSolutionConHeader


/* ************** writeCmplSolutionConValues ************ */
void cfResult::writeCmplSolutionConValues(long n, long i) {

    cout << "           <constraint idx=\""<< i << "\" name=\""<< constraintName(i) << "\" type=\"" << constraintType(i) << "\" activity=\"" << resultValue(constraintActivity(n,i)) << "\"";

    cout <<  " lowerBound=\"";
    if (constraintHasLowerBound(i))
        cout << constraintLowerBound(i);
    else
       cout <<  "-INF";
       // cout <<  "-Infinity";

    cout << "\" upperBound=\"";
    if (constraintHasUpperBound(i))
        cout << constraintUpperBound(i);
    else
        cout <<  "INF";
       // cout << "Infinity";

    cout << "\" marginal=\"";
    if (constraintHasMarginal(n,i))
        cout << resultValue(constraintMarginal(n,i));
    else
        cout <<  "NaN";
        //cout << "-" ;
    cout  << "\"/>" << endl;

}//end writeCmplSolutionConValues


/* ************** writeCmplSolutionConFooter ************ */
void cfResult::writeCmplSolutionConFooter() {

    cout << "       </linearConstraints>" << endl;

} // end writeCmplSolutionConFooter

/* ************** writeCmplSolutionFooter ************ */
void cfResult::writeCmplSolutionFooter() {

    cout << "   </solution>"<< endl;

} // end writeCmplSolutionFooter

/* ************** writeCmplSolutionReportFooter ************ */
void cfResult::writeCmplSolutionReportFooter() {

    cout << "</CmplSolutions>"<< endl;

} // end writeCmplSolutionReportFooter


/* ************** writeCmplSol ************ */
bool cfResult::writeCmplSol() {

    int n1;

    CmplFileType report;
    report.setFileName(cSolName.toStdString());
    report.setOutMode(FILE_OUT);

    if (!report.open()) {
        setCfOsMessage("Internal Error - Can not write the cmplSolution file <" + solName +">\n" );
        return false;
    }

    writeCmplSolutionHeader();

    for (int n=0; n < nrOfSolutions(); n++)   {

        if (glvar->args->solutionPool() && glvar->args->solverName()=="gurobi")
            n1=nrOfSolutions()-n-1;
        else
            n1=n;

        writeCmplSolutionSolHeader(n1);

        if (!glvar->args->ignoreVars()) {
            writeCmplSolutionVarHeader();
            if (glvar->args->displayVarListSize()==0) {
                for (long i=0; i < nrOfVariables(); i++) {
                    if( !(glvar->args->ignoreZeros() && resultValue(varActivity(n1,i))==0) ) {
                        writeCmplSolutionVarValues(n1,i);
                    }
                }

            } else {
                for (int j=0; j<glvar->args->displayVarListSize(); j++ ) {
                    for (long i=0; i < nrOfVariables(); i++) {
                        string tmpStr=glvar->args->displayVarList(j);
                        int starPos=tmpStr.find_first_of("*");
                        tmpStr=tmpStr.substr(0,starPos);
                        if ( (starPos>-1 && CmplTools::stringStartsWith(varName(i),tmpStr)) || (starPos==-1 && varName(i)==tmpStr) ) {
                            if( !(glvar->args->ignoreZeros() && resultValue(varActivity(n1,i))==0) ) {
                                writeCmplSolutionVarValues(n1,i);
                            }
                        }
                    }
                }
            }
            writeCmplSolutionVarFooter();
        }

        if (!glvar->args->ignoreCons()) {
            writeCmplSolutionConHeader();
            if (glvar->args->displayConListSize()==0) {
                for (long i=0; i < nrOfConstraints(); i++) {
                    if( !(glvar->args->ignoreZeros() && resultValue(constraintActivity(n1,i))==0)) {
                        writeCmplSolutionConValues(n1,i);
                    }
                }
            } else {

                for (int j=0; j<glvar->args->displayConListSize(); j++ ) {
                    for (long i=0; i < nrOfConstraints(); i++) {
                        string tmpStr=glvar->args->displayConList(j);
                        int starPos=tmpStr.find_first_of("*");
                        tmpStr=tmpStr.substr(0,starPos);

                        if ( (starPos>-1 && CmplTools::stringStartsWith(constraintName(i),tmpStr)) || (starPos==-1 && constraintName(i)==tmpStr) ) {
                            if( !(glvar->args->ignoreZeros() && resultValue(constraintActivity(n1,i))==0)) {
                                writeCmplSolutionConValues(n1,i);
                            }
                        }
                    }
                }

            }
            writeCmplSolutionConFooter();
        }
        writeCmplSolutionFooter();
    }
    writeCmplSolutionReportFooter();
    report.close();
    return true;

}//end writeCmplSol


/* ************** writeCsvHeader ************ */
void cfResult::writeCsvHeader() {

    string cmplName = QFileInfo(QString::fromStdString(instanceName())).fileName().toStdString();

    cout.precision( glvar->args->maxDecimals()+1 );
    cout << "CMPL csv export" << endl << endl;

    //cout << "Problem;" << instanceName() << endl;
    cout << "Problem;" << cmplName << endl;

    cout << "Nr. of variables;" << nrOfVariables() << endl;
    cout << "Nr. of constraints;" << nrOfConstraints() << endl;
    cout << "Objective name;" << objectiveName() << endl;
    cout << "Objective sense;" << objectiveSense() << endl;
    if (glvar->args->solutionPool()) {
        cout <<  "Nr. of solutions;" << nrOfSolutions() << endl;
    }
    cout << "Solver;" << solverName() << endl;


    cout  << "Display variables;" ;

    if (glvar->args->ignoreVars())
        cout << "ignoreAllVariables ";
    else {

        if (glvar->args->ignoreZeros())
            cout << "nonZeroVariables ";

        cout << "(";
        if (glvar->args->displayVarListSize()==0)
            cout << "all";
        else {
            for (int j=0; j<glvar->args->displayVarListSize(); j++) {
                if (j>0) cout << " ";
                cout << glvar->args->displayVarList(j);
            }
        }
        cout << ")";
    }
    cout << endl ;


    cout << "Display constraints;" ;

    if (glvar->args->ignoreCons())
        cout << "ignoreAllConstraints ";
    else {
        if (glvar->args->ignoreZeros())
            cout << "nonZeroConstraints ";

        cout << "(";
        if(glvar->args->displayConListSize()==0)
            cout << "all";
        else {
            for (int j=0; j<glvar->args->displayConListSize(); j++) {
                if (j>0) cout << " ";
                cout << glvar->args->displayConList(j);
            }
        }
        cout << ")";
    }
    cout << endl ;

}//end writeCsvHeader


/* ************** writeCsvVarHeader ************ */
void cfResult::writeCsvVarHeader(long n) {

    int n1;

    if (glvar->args->solutionPool() && glvar->args->solverName()=="gurobi")
        n1=nrOfSolutions()-n-1;
    else
        n1=n;

    cout << endl;

    if (glvar->args->solutionPool()) {
        cout << "Solution nr.;" << n1+1 << endl;
    }

    cout <<  "Objective status;" << objectiveStatus(n) << endl;
    cout <<  "Objective value;" << objectiveValue(n) <<  endl;
    if (!glvar->args->ignoreVars()) {
        cout << "Variables;" << endl;
        cout <<  "Name;Type;Activity;LowerBound;UpperBound;Marginal" << endl;
    }
}//end writeCsvVarHeader


/* ************** writeCsvVarValues ************ */
void cfResult::writeCsvVarValues(long n, long i) {
    cout << varName(i) << ";" ;
    cout << varType(i) << ";";
    cout << ( varType(i)=="C" ? resultValue(varActivity(n,i)) :  round(resultValue(varActivity(n,i)))) << ";";
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

    if (!glvar->args->ignoreCons()) {
        cout << "Constraints;"<< endl;
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
    CmplFileType report;
    int n1;

    report.setFileName(csvName.toStdString());
    report.setOutMode(FILE_OUT);

    if (!report.open()) {
        setCfOsMessage("Internal Error - Can not write the CSV file <" + solName +">\n" );
        return false;
    }

    writeCsvHeader();

    for (int n=0; n < nrOfSolutions(); n++)   {

        if (glvar->args->solutionPool() && glvar->args->solverName()=="gurobi")
            n1=nrOfSolutions()-n-1;
        else
            n1=n;

        writeCsvVarHeader(n1);

        if (!glvar->args->ignoreVars()) {
            if (glvar->args->displayVarListSize()==0) {
                for (long i=0; i < nrOfVariables(); i++) {
                    if( !(glvar->args->ignoreZeros() && resultValue(varActivity(n1,i))==0) )  {
                        writeCsvVarValues(n1,i);
                    }
                }
            } else {
                for (int j=0; j<glvar->args->displayVarListSize(); j++ ) {
                    for (long i=0; i < nrOfVariables(); i++) {
                        string tmpStr=glvar->args->displayVarList(j);
                        int starPos=tmpStr.find_first_of("*");
                        tmpStr=tmpStr.substr(0,starPos);

                        if ( (starPos>-1 && CmplTools::stringStartsWith(varName(i),tmpStr)) || (starPos==-1 && varName(i)==tmpStr) ) {
                            if( !(glvar->args->ignoreZeros() && resultValue(varActivity(n1,i))==0) ) {
                                writeCsvVarValues(n1,i);
                            }
                        }
                    }
                }
            }
        }

        writeCsvConHeader();
        if (!glvar->args->ignoreCons()) {
            if (glvar->args->displayConListSize()==0) {
                for (long i=0; i < nrOfConstraints(); i++) {
                    if( !(glvar->args->ignoreZeros() && resultValue(constraintActivity(n1,i))==0))  {
                        writeCsvConValues(n1,i);
                    }
                }
            } else {

                    for (int j=0; j<glvar->args->displayConListSize(); j++ ) {
                        for (long i=0; i < nrOfConstraints(); i++) {
                            string tmpStr=glvar->args->displayConList(j);
                            int starPos=tmpStr.find_first_of("*");
                            tmpStr=tmpStr.substr(0,starPos);

                            if ( (starPos>-1 && CmplTools::stringStartsWith(constraintName(i),tmpStr)) || (starPos==-1 && constraintName(i)==tmpStr) ) {
                                if( !(glvar->args->ignoreZeros() && resultValue(constraintActivity(n1,i))==0)) {
                                    writeCsvConValues(n1,i);
                                }
                            }
                        }
                    }
                }
            }
        }

    report.close();
    return true;
}//end writeCsv



/* ************** getSolverMessages ************ */
void cfResult::getSolverMessages() {

    if (!glvar->args->cmplSolOut()) {

        cout << endl << setw(20) << left << "Solver failed" <<endl;
        cout << setw(20) << left << "General status: " << generalStatus()<< endl;
        cout << setw(20) << left << "SolverName" << solverName() << endl;
        //cout << setw(20) << left << "Message: " << solverMessage() << endl;
        cout << setw(20) << left << "Nr. of Solutions: " << nrOfSolutions() << endl;

    } else {
        CmplFileType report;
        report.setFileName(cSolName.toStdString());
        report.setOutMode(FILE_OUT);

        if (report.open()) {
            writeCmplSolutionHeader();
            writeCmplSolutionReportFooter();
            report.close();
        }

    }

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

    int nrOfHeadLines=0;

    int lineCounter=0;

    cfModelResults tmpResults;
    cfResultValues tmpValues;

    if (glvar->args->solverName()=="glpk" ) {
        resultStatus.serviceName="GLPK";
        resultStatus.solverName="GLPK";
    } else if( glvar->args->solverName()=="cbc" || glvar->args->solverName().empty()){
        resultStatus.serviceName="CBC";
        resultStatus.solverName="CBC";
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
            if (glvar->args->solverName()=="glpk" ) {
                resultStatus.nrOfConstraints=solList[0].toLong();
                nrOfHeadLines=2;
            } else {
                resultStatus.nrOfConstraints=solList[0].toLong()-1;
                nrOfHeadLines=3;
            }
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
        if (lineCounter>nrOfHeadLines && lineCounter<=resultStatus.nrOfConstraints+nrOfHeadLines) {
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
        if (lineCounter>resultStatus.nrOfConstraints+nrOfHeadLines && lineCounter<=resultStatus.nrOfConstraints+nrOfHeadLines+resultStatus.nrOfVars ) {
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


/* ************** readGurobiSolFile ************ */
bool cfResult::readGurobiSolFile() {

    cfModelResults tmpResults;
    cfResultValues tmpValues;

    int row=0;
    int col=0;
    int colCount=0;

    int realIdx;
    int intIdx;

    bool realVarSection;

    resultStatus.serviceName="Gurobi";
    resultStatus.solverName="Gurobi";

    resultStatus.instanceName=problemName;
    resultStatus.message="";

    resultStatus.nrOfConstraints=cmplInst->rowCount()-1;
    resultStatus.nrOfVars=cmplInst->colCount();

    resultStatus.nrOfSolutions=0;

    QDomDocument doc("myGurobiSol");
    QFile file(gSolName);
    if (!file.open(QIODevice::ReadOnly)) {
        setCfOsMessage("File could not be open >>" +gSolName) ;
        return false;
    }
    if (!doc.setContent(&file)) {
        file.close();
        setCfOsMessage("xml error >>" + gSolName );
        return false;
    }
    file.close();


    QDomElement docElem = doc.documentElement();

    if ( docElem.tagName() != "CmplGurobiSolutions") {
        setCfOsMessage("xml file >>" + gSolName  + " is not a CmplGurobiSolutions file");
        file.close();
        return false;
    }

    QDomNode child1 = docElem.firstChild();
    while(!child1.isNull()) {

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

        tmpResults.status="";
        tmpResults.objValue=0;
        tmpResults.variables.clear();
        tmpResults.constraints.clear();

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

        //QDomElement elemChild1 = child1.toElement();
        QDomNode child2 = child1.firstChild();

        while(!child2.isNull()) {

            QDomElement elemChild2 = child2.toElement();

            if ( elemChild2.tagName()=="header" ) {

                resultStatus.generalStatus=elemChild2.attribute("status");
                //                  if (resultStatus.generalStatus.contains("optimal") || resultStatus.generalStatus.contains("feasible") ) {
                if (!resultStatus.generalStatus.toLower().contains("infeasible") ) {
                    resultStatus.nrOfSolutions++;
                    tmpResults.objValue=elemChild2.attribute("value").toDouble();
                    tmpResults.status=resultStatus.generalStatus;
                } else
                    resultStatus.nrOfSolutions=0;
            }

            if ( elemChild2.tagName()=="variables" ) {
                QDomNode child3 = child2.firstChild();
                colCount=0;
                while(!child3.isNull()) {

                    if (realVarSection)
                        col=cmplInst->varRealIdx(realIdx);
                    else
                        col=cmplInst->varIntIdx(intIdx);

                    QDomElement elemChild3 = child3.toElement();

                    if (elemChild3.attribute("marginal","nn")!="nn") {
                        tmpResults.variables[col].marginal=elemChild3.attribute("marginal").toDouble();
                        tmpResults.variables[col].hasMarginal=true;
                    }

                    tmpResults.variables[col].activity=elemChild3.attribute("activity").toDouble();

                    child3=child3.nextSibling();

                    colCount++;
                    if (colCount<cmplInst->varRealIdxListSize())
                        realIdx++;
                    else {
                        realVarSection=false;
                        intIdx++;
                    }
                }
            }

            if ( elemChild2.tagName()=="constraints" ) {
                QDomNode child3 = child2.firstChild();

                while(!child3.isNull()) {
                    QDomElement elemChild3 = child3.toElement();

                    row = elemChild3.attribute("idx").toInt();

                    if (elemChild3.attribute("marginal","nn")!="nn") {
                        tmpResults.constraints[row].marginal=elemChild3.attribute("marginal").toDouble();
                        tmpResults.constraints[row].hasMarginal=true;
                    }

                    tmpResults.constraints[row].activity=elemChild3.attribute("activity").toDouble() ;

                    child3=child3.nextSibling();
                }
            }
            child2 = child2.nextSibling();
        }
        results.append(tmpResults);
        child1 = child1.nextSibling();
    }

    return true;

} //end readGurobiSolFile


/* ************** readCplexSolFile ************ */
bool cfResult::readCplexSolFile() {

    resultStatus.serviceName="CPLEX";
    resultStatus.solverName="CPLEX";

    resultStatus.instanceName=problemName;
    resultStatus.message="";

    resultStatus.nrOfConstraints=cmplInst->rowCount()-1;
    resultStatus.nrOfVars=cmplInst->colCount();

    resultStatus.nrOfSolutions=0;

    QDomDocument doc("myCplexSol");
    QFile file(gSolName);
    if (!file.open(QIODevice::ReadOnly)) {
        //setCfOsMessage("File could not be open >>" +gSolName) ;
        resultStatus.generalStatus="No solution exists";
        //return false;
        return true;
    }
    if (!doc.setContent(&file)) {
        file.close();
        setCfOsMessage("xml error >>" + gSolName );
        return false;
    }
    file.close();

    QDomElement docElem = doc.documentElement();

    if ( docElem.tagName() != "CPLEXSolution" && docElem.tagName() != "CPLEXSolutions") {
        setCfOsMessage("xml file >>" + gSolName  + " is not a CPLEX solution file");
        file.close();
        return false;
    }

    if ( docElem.tagName() == "CPLEXSolution") {
        readCplexSolutionSection(docElem);
    } else {
        QDomNode child1 = docElem.firstChild();
        while(!child1.isNull()) {
            QDomElement elemChild1 = child1.toElement();
            readCplexSolutionSection(elemChild1);
            child1 = child1.nextSibling();
        }
    }

    return true;
}//end readCplexSolFile

/* ************** readCplexSolutionSection ************ */
void cfResult::readCplexSolutionSection(QDomElement docElem) {

    cfModelResults tmpResults;
    cfResultValues tmpValues;

    int row=0;
    int col=0;
    int colCount=0;

    int realIdx;
    int intIdx;

    bool realVarSection;


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

    tmpResults.status="";
    tmpResults.objValue=0;
    tmpResults.variables.clear();
    tmpResults.constraints.clear();

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


    QDomNode child1 = docElem.firstChild();
    while(!child1.isNull()) {

        QDomElement elemChild1 = child1.toElement();
        QDomNode child2 = child1.firstChild();

        if ( elemChild1.tagName()=="header" ) {

            resultStatus.generalStatus=elemChild1.attribute("solutionStatusString");
            if (resultStatus.generalStatus.contains("optimal") || resultStatus.generalStatus.contains("feasible") ) {
                resultStatus.nrOfSolutions++;
                tmpResults.objValue=elemChild1.attribute("objectiveValue").toDouble();
                tmpResults.status=resultStatus.generalStatus;
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

} //end readCplexSolutionSection



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
            tmpValues.type="N";
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

    long tmpRow=0;
    double tmpValue;
    int j, n, n1;

    for ( j=0; j < resultStatus.nrOfSolutions; j++) {

        if ( !(glvar->args->solverName()=="gurobi" && QString::fromStdString(objectiveStatus(j)).contains("optimal")) ) {
            for ( n=0; n<cmplInst->varListSize(); n++) {
                for (  n1=0; n1<cmplInst->columnsCoefListSize(n) ; n1++) {
                    tmpRow=cmplInst->colCoefRowIdx(n,n1);
                    CmplTools::stringToDouble(cmplInst->colCoef(n,n1), tmpValue);
                    results[j].constraints[tmpRow].activity += tmpValue * results[j].variables[n].activity ;
                }
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
