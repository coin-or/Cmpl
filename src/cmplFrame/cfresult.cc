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

#include "cfresult.hh"


/* ************** cfResult ************ */
cfResult::cfResult()
{
    osilName="";
    osrlName="";
    cfOsMessage="";

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
bool cfResult::setProblemName(QString &fname) {
    if (fname.toLower().contains(".osrl") || fname.toLower().contains(".osil") || fname.toLower().contains(".cmpl")) {
        setCfOsMessage("Please define the problemname w/o any file extensions");
        return false;
    }
    osilName=fname+".osil";
    osrlName=fname+".osrl";
    csvName=fname+".csv";
    solName=fname+".sol";
    return true;
}//end setProblemName


/* ************** getCfOsMessage ************ */
string cfResult::getCfOsMessage() {
    return cfOsMessage.toStdString();
}//end getCfOsMessage


/* ************** getResult ************ */
bool cfResult::getResult() {
    if (!cfReadOsrl()) return false;
    if (!cfReadOsil()) return false;
    calculateConstraintCoefficients();
    return true;
}//end getResult



/* ************** standardReport ************ */
bool cfResult::standardReport(int outType) {

    // outType==1 - stdIio - default
    // outType==2 - Ascii file

    ofstream rFile;
    QByteArray b = solName.toLatin1();
    const char *fname = b.data();


    if (outType==2) {
         rFile.open(fname, ios::trunc);
         if (!rFile.is_open()) {
             setCfOsMessage("Internal Error - Cant read and write the csv file!\n" );
             return false;
         }
    }

    #define rOut (outType==2 ? rFile : cout )

    rOut << setw(105) << setfill('-') << "" <<endl;
    rOut <<  setfill(' ');
    rOut << setw(20) << left << "General status: " << generalStatus()<< endl;
    rOut << setw(20) << left << "Problem : " << instanceName() << endl;
    // rOut << setw(20) << left << "ServiceName" << serviceName() << endl;
    rOut << setw(20) << left << "SolverName" << solverName() << endl;
    rOut << setw(20) << left << "Nr. of Solutions: " << nrOfSolutions() << endl;
    rOut << setw(105) << setfill('-') << "" <<endl;
    rOut <<  setfill(' ');

    for (int n=0; n < nrOfSolutions(); n++)   {

        rOut << setw(20) << left <<  "Solution nr.: " << n+1 << endl;
        rOut << setw(20) << left <<  "Objective name : " << objectiveName() << endl;
        rOut << setw(20) << left <<  "Objective value : " << objectiveValue(n) << " (" << objectiveSense() << "!)" << endl;

        rOut <<endl;

        rOut << setw(20) << left <<  "Variables " << endl;
        rOut << setw(20) << left <<  "Nr. of variables:" << nrOfVariables() << endl;


        rOut << setw(20) << left <<  "Name";
        rOut << setw(5) << right <<  "Type";
        rOut << setw(20) << right <<   "Activity";
        rOut << setw(20) << right <<  "Lower Bound";
        rOut << setw(20) << right <<  "Upper Bound";
        rOut << setw(20) << right <<  "Marginal" << endl;
        rOut << setw(105) << setfill('-') << "" <<endl;
        rOut <<  setfill(' ');

        for (long i=0; i < nrOfVariables(); i++) {
             rOut << setw(20) << setprecision(2) << fixed << left <<  varName(i);
             rOut << setw(5) << setprecision(2) << fixed << right << varType(i);
             rOut << setw(20) << setprecision(2) << fixed << right << varActivity(n,i);
             if (varHasLowerBound(i))
                    rOut << setw(20) << setprecision(2) << fixed << right << varLowerBound(i);
             else
                    rOut << setw(20)  << right<< "-Infinity";
             if (varHasUpperBound(i))
                    rOut << setw(20) << setprecision(2) << fixed << right << varUpperBound(i);
             else
                    rOut << setw(20)  << right<< "Infinity";
             if (varHasMarginal(n,i))
                    rOut << setw(20) << setprecision(2) << fixed << right<< varMarginal(n,i);
             else
                    rOut << setw(20) << right << "-" ;
             rOut << endl;
         }
        rOut << setw(105) << setfill('-') << "" <<endl;
        rOut <<  setfill(' ');

        rOut <<endl;

        rOut << setw(20) << left <<  "Constraints " << endl;
        rOut << setw(20) << left <<  "Nr. of constraints:" << nrOfConstraints() << endl;

        rOut << setw(20) << left <<  "Name";
        rOut << setw(5) << right <<  "Type";
        rOut << setw(20) << right <<   "Activity";
        rOut << setw(20) << right <<  "Lower Bound";
        rOut << setw(20) << right <<  "Upper Bound";
        rOut << setw(20) << right <<  "Marginal" << endl;
        rOut << setw(105) << setfill('-') << "" <<endl;
        rOut <<  setfill(' ');


        for (long i=0; i < nrOfConstraints(); i++) {
             rOut << setw(20) << setprecision(2) << fixed << left <<  constraintName(i);
             rOut << setw(5) << setprecision(2) << fixed << right << constraintType(i);
             rOut << setw(20) << setprecision(2) << fixed << right<< constraintActivity(n,i);
             if (constraintHasLowerBound(i))
                    rOut << setw(20) << setprecision(2) << fixed << right << constraintLowerBound(i);
             else
                    rOut << setw(20) << right<< "-Infinity";
             if (constraintHasUpperBound(i))
                    rOut << setw(20) << setprecision(2) << fixed << right << constraintUpperBound(i);
             else
                    rOut << setw(20) << right<< "Infinity";
             if (constraintHasMarginal(n,i))
                    rOut << setw(20) << setprecision(2) << fixed << right << constraintMarginal(n,i);
             else
                    rOut << setw(20) << right << "-";
             rOut <<endl;
         }
        rOut << setw(105) << setfill('-') << "" <<endl;
        rOut <<  setfill(' ');
    }

    if (outType==2)
        rFile.close();

    return true;
}//end standardReport



/* ************** writeCsv ************ */
bool cfResult::writeCsv() {

    QFile csvFile (csvName);
    QTextStream csvFH (&csvFile);

    if (!csvFile.open(QIODevice::Truncate | QIODevice::Text | QIODevice::ReadWrite)) {
        setCfOsMessage("Internal Error - Cant read and write the csv file!\n" );
        return false;
    }

    csvFH << "CMPL csv export" << endl << endl;

    csvFH << "General status;" << resultStatus.generalStatus << endl;
    csvFH << "Solver;" << resultStatus.solverName << endl;
    csvFH << "Nr. of solutions;" << nrOfSolutions() << endl << endl;

    csvFH << "Problem;" << resultStatus.instanceName << endl;

    for (int n=0; n < nrOfSolutions(); n++)   {

        csvFH <<  "Solution nr.;" << n+1 << endl;
        csvFH <<  "Objective name;" << modelNames.objName << endl;
        csvFH <<  "Objective value;" << objectiveValue(n) << "; (" << modelNames.objSense << "!)" << endl <<endl;



        csvFH <<  "Variables " << endl;
        csvFH <<  "Nr. of variables;" << nrOfVariables() << endl;


        csvFH <<  "Name;Type;Activity;LowerBound;UpperBound; Marginal" << endl;

        for (long i=0; i < nrOfVariables(); i++) {
             csvFH << modelNames.varNames[i] << ";" ;
             csvFH << modelConstants.variables[i].type << ";";
             csvFH << varActivity(n,i) << ";";
             if (varHasLowerBound(i))
                    csvFH << varLowerBound(i) << ";";
             else
                    csvFH << "-Infinity ;" ;
             if (varHasUpperBound(i))
                    csvFH << varUpperBound(i) << ";";
             else
                    csvFH << "Infinity;";
             if (varHasMarginal(n,i))
                    csvFH << varMarginal(n,i) ;
             else
                    csvFH << "-" ;
             csvFH << endl;
         }


        csvFH <<endl;

        csvFH <<  "Constraints " << endl;
        csvFH <<  "Nr. of constraints;" << nrOfConstraints() << endl;

        csvFH <<  "Name;Type;Activity;LowerBound;UpperBound; Marginal" << endl;


        for (long i=0; i < nrOfConstraints(); i++) {
             csvFH << modelNames.conNames[i] << ";";
             csvFH << modelConstants.constraints[i].type << ";";
             csvFH << constraintActivity(n,i) << ";";
             if (constraintHasLowerBound(i))
                    csvFH << constraintLowerBound(i) << ";";
             else
                    csvFH << "-Infinity" << ";";
             if (constraintHasUpperBound(i))
                    csvFH << constraintUpperBound(i) << ";";
             else
                    csvFH << "Infinity" << ";";
             if (constraintHasMarginal(n,i))
                    csvFH << constraintMarginal(n,i);
             else
                    csvFH << "-";
             csvFH <<endl;
         }

    }

    csvFile.close();
    return true;
}//end writeCsv



/* ************** getSolverMessages ************ */
void cfResult::getSolverMessages() {

    cout << setw(20) << left << "OSSolverService failed" <<endl;
    cout << setw(20) << left << "General status: " << generalStatus()<< endl;
    //cout << setw(20) << left << "Problem : " << instanceName() << endl;
    //cout << setw(20) << left << "ServiceName" << serviceName() << endl;
    //cout << setw(20) << left << "SolverName" << solverName() << endl;
    cout << setw(20) << left << "OSSolverService: " << solverMessage() << endl;
    cout << setw(20) << left << "Nr. of Solutions: " << nrOfSolutions() << endl;

}//end getSolverMessages


/* ************** generalStatus ************ */
string cfResult::generalStatus() {
    return resultStatus.generalStatus.toStdString();
}//end generalStatus


/* ************** serviceName ************ */
string cfResult::serviceName() {
    return resultStatus.serviceName.toStdString();
}//end serviceName


/* ************** instanceName ************ */
string cfResult::instanceName() {
    return resultStatus.instanceName.toStdString();
}//end instanceName


/* ************** solverName ************ */
string cfResult::solverName() {
    return resultStatus.solverName.toStdString();
}//end solverName


/* ************** cfResult ************ */
string cfResult::solverMessage() {
    return resultStatus.message.toStdString();
}//end


/* ************** nrOfVariables ************ */
long cfResult::nrOfVariables() {
    return resultStatus.nrOfVars;
}//end nrOfVariables


/* ************** nrOfConstraints ************ */
long cfResult::nrOfConstraints() {
    return resultStatus.nrOfConstraints;
}//end nrOfConstraints


/* ************** nrOfSolutions ************ */
int cfResult::nrOfSolutions() {
    return resultStatus.nrOfSolutions;
}//end


/* ************** cfResult ************ */
string cfResult::objectiveName(){
    return  modelNames.objName.toStdString();
}//end nrOfSolutions


/* ************** objectiveSense ************ */
string cfResult::objectiveSense(){
    return  modelNames.objSense.toStdString();
}//end objectiveSense


/* ************** objectiveValue ************ */
double cfResult::objectiveValue(int solutionNumber){
    return results[solutionNumber].objValue;
}//end objectiveValue


/* ************** varName ************ */
string cfResult::varName(long index) {
    return modelNames.varNames[index].toStdString();
}//end varName


/* ************** varType ************ */
string cfResult::varType(long index) {
    return modelConstants.variables[index].type.toStdString();
}//end varType


/* ************** varType ************ */
string cfResult::varType(string varName) {
    long index=getVarNameIndex(varName);
    return modelConstants.variables[index].type.toStdString();
}//end varType


/* ************** varHasLowerBound ************ */
bool cfResult::varHasLowerBound(long index) {
    return modelConstants.variables[index].hasLB;
}//end varHasLowerBound


/* ************** varHasLowerBound ************ */
bool cfResult::varHasLowerBound(string varName) {
    long index=getVarNameIndex(varName);
    return modelConstants.variables[index].hasLB;
}//end varHasLowerBound


/* ************** varLowerBound ************ */
double cfResult::varLowerBound(long index) {
    return modelConstants.variables[index].lowerBound;
}//end varLowerBound


/* ************** varLowerBound ************ */
double cfResult::varLowerBound(string varName) {
    long index=getVarNameIndex(varName);
    return modelConstants.variables[index].lowerBound;
}//end varLowerBound


/* ************** varHasUpperBound ************ */
bool cfResult::varHasUpperBound(long index) {
    return modelConstants.variables[index].hasUB;
}//end varHasUpperBound


/* ************** varHasUpperBound ************ */
bool cfResult::varHasUpperBound(string varName) {
    long index=getVarNameIndex(varName);
    return modelConstants.variables[index].hasUB;
}//end varHasUpperBound


/* ************** varUpperBound ************ */
double cfResult::varUpperBound(long index) {
    return modelConstants.variables[index].upperBound;
}//end varUpperBound


/* ************** varUpperBound ************ */
double cfResult::varUpperBound(string varName) {
    long index=getVarNameIndex(varName);
    return modelConstants.variables[index].upperBound;
}//end varUpperBound


/* ************** varActivity ************ */
double cfResult::varActivity(int solutionNumber, long index) {
    return results[solutionNumber].variables[index].activity;
}//end varActivity


/* ************** varActivity ************ */
double cfResult::varActivity(int solutionNumber, string varName) {
    long index=getVarNameIndex(varName);
    return results[solutionNumber].variables[index].activity;
}//end varActivity


/* ************** varHasMarginal ************ */
bool cfResult::varHasMarginal(int solutionNumber, long index) {
    return  results[solutionNumber].variables[index].hasMarginal;
}//end varHasMarginal


/* ************** varHasMarginal ************ */
bool cfResult::varHasMarginal(int solutionNumber, string varName) {
    long index=getVarNameIndex(varName);
    return  results[solutionNumber].variables[index].hasMarginal;
}//end varHasMarginal


/* ************** varMarginal ************ */
double cfResult::varMarginal(int solutionNumber, long index) {
    return  results[solutionNumber].variables[index].marginal;
}//end varMarginal


/* ************** varMarginal ************ */
double cfResult::varMarginal(int solutionNumber, string varName) {
    long index=getVarNameIndex(varName);
    return  results[solutionNumber].variables[index].marginal;
}//end varMarginal


/* ************** constraintName ************ */
string cfResult::constraintName(long index) {
    return modelNames.conNames[index].toStdString();
}//end constraintName


/* ************** constraintType ************ */
string cfResult::constraintType(long index) {
    return modelConstants.constraints[index].type.toStdString();
}//end constraintType

/* ************** constraintType ************ */
string cfResult::constraintType(string conName) {
    long index=getConstraintNameIndex(conName);
    return modelConstants.constraints[index].type.toStdString();
}//end constraintType


/* ************** constraintHasLowerBound ************ */
bool cfResult::constraintHasLowerBound(long index) {
    return modelConstants.constraints[index].hasLB;
}//end constraintHasLowerBound


/* ************** constraintHasLowerBound ************ */
bool cfResult::constraintHasLowerBound(string conName) {
    long index=getConstraintNameIndex(conName);
    return modelConstants.constraints[index].hasLB;
}//end constraintHasLowerBound


/* ************** constraintLowerBound ************ */
double cfResult::constraintLowerBound(long index) {
    return modelConstants.constraints[index].lowerBound;
}//end constraintLowerBound


/* ************** constraintLowerBound ************ */
double cfResult::constraintLowerBound(string conName) {
    long index=getConstraintNameIndex(conName);
    return modelConstants.constraints[index].lowerBound;
}//end constraintLowerBound


/* ************** constraintHasUpperBound ************ */
bool cfResult::constraintHasUpperBound(long index) {
    return modelConstants.constraints[index].hasUB;
}//end constraintHasUpperBound


/* ************** constraintHasUpperBound ************ */
bool cfResult::constraintHasUpperBound(string conName) {
    long index=getConstraintNameIndex(conName);
    return modelConstants.constraints[index].hasUB;
}//end constraintHasUpperBound


/* ************** constraintUpperBound ************ */
double cfResult::constraintUpperBound(long index) {
    return modelConstants.constraints[index].upperBound;
}//end constraintUpperBound


/* ************** constraintUpperBound ************ */
double cfResult::constraintUpperBound(string conName) {
    long index=getConstraintNameIndex(conName);
    return modelConstants.constraints[index].upperBound;
}//end constraintUpperBound


/* ************** constraintActivity ************ */
double cfResult::constraintActivity(int solutionNumber, long index) {
    return results[solutionNumber].constraints[index].activity;
}//end constraintActivity


/* ************** constraintActivity ************ */
double cfResult::constraintActivity(int solutionNumber, string conName) {
    long index=getConstraintNameIndex(conName);
    return results[solutionNumber].constraints[index].activity;
}//end constraintActivity


/* ************** constraintHasMarginal ************ */
bool cfResult::constraintHasMarginal(int solutionNumber, long index) {
    return results[solutionNumber].constraints[index].hasMarginal;
}//end constraintHasMarginal


/* ************** constraintHasMarginal ************ */
bool cfResult::constraintHasMarginal(int solutionNumber, string conName) {
    long index=getConstraintNameIndex(conName);
    return results[solutionNumber].constraints[index].hasMarginal;
}//end constraintHasMarginal


/* ************** constraintMarginal ************ */
double cfResult::constraintMarginal(int solutionNumber, long index) {
    return results[solutionNumber].constraints[index].marginal;
}//end constraintMarginal


/* ************** constraintMarginal ************ */
double cfResult::constraintMarginal(int solutionNumber, string conName) {
    long index=getConstraintNameIndex(conName);
    return results[solutionNumber].constraints[index].marginal;
}//end constraintMarginal


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
                      if (elemChild2.tagName()=="generalStatus") resultStatus.generalStatus= elemChild2.attribute("type");
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

                                if ( elemChild3.tagName()=="status" )   {                                                            //<status ... />
                                    tmpResults.status=elemChild3.attribute("type");
                                    if (tmpResults.status.toLower()=="infeasible") {
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



/* ************** cfReadOsil ************ */
bool cfResult::cfReadOsil() {

    if (osilName.isEmpty()) {
        setCfOsMessage("OSiL name is not set");
        return false;
    }

    QDomDocument doc("myOsil");
    QFile file(osilName);
    if (!file.open(QIODevice::ReadOnly)) {
        setCfOsMessage("File could not be open >>" + osilName) ;
        return false;
    }
    if (!doc.setContent(&file)) {
        file.close();
        setCfOsMessage("xml error >>" + osilName );
        return false;
    }
    file.close();

    QDomElement docElem = doc.documentElement();

    if ( docElem.tagName() != "osil" ) {                                                                            //<osil>
        setCfOsMessage("xml file >>" + osilName  + " is not an osil file");
        return false;
    }

    // tmpValues;
    cfConstantsValues tmpValues;
    QList<long> start;
    QList<long> rowIdx;
    cflinearConstraintCoefficients tmpCoeff;

    QDomNode child1 = docElem.firstChild();
    while(!child1.isNull()) {

        QDomElement elemChild1 = child1.toElement();

        QDomNode child2 = child1.firstChild();

        if ( elemChild1.tagName()=="instanceHeader" ) {                                                                 //<instanceHeader>

            while(!child2.isNull()) {
                      QDomElement elemChild2 = child2.toElement();
                      if (elemChild2.tagName()=="name")  modelNames.instanceName=elemChild2.text();
                      if (elemChild2.tagName()=="description") modelNames.description=elemChild2.text();

                      child2=child2.nextSibling();
            }
        }                                                                                                               //</instanceHeader>

        if ( elemChild1.tagName()=="instanceData" ) {                                                                   //<instanceData>

            while(!child2.isNull()) {
                      QDomElement elemChild2 = child2.toElement();
                      if (elemChild2.tagName()=="variables") {                                                             //<variables>

                            modelConstants.nrOfVariables=elemChild2.attribute("numberOfVariables").toLong();

                            QDomNode child3 = child2.firstChild();

                            while(!child3.isNull()) {

                                QDomElement elemChild3 = child3.toElement();

                                if (elemChild3.tagName()=="var") {                                                              //<var>
                                        modelNames.varNames.append(elemChild3.attribute("name"));

                                        tmpValues.type="";

                                        tmpValues.type=elemChild3.attribute("type");

                                        if (elemChild3.attribute("lb","ndef")=="ndef"){
                                            tmpValues.hasLB=false;
                                            tmpValues.lowerBound=0 ;
                                        }
                                        else {
                                            tmpValues.hasLB=true;
                                            tmpValues.lowerBound=elemChild3.attribute("lb").toDouble();
                                        }

                                        if (elemChild3.attribute("ub","ndef")=="ndef"){
                                            tmpValues.hasUB=false;
                                            tmpValues.upperBound=0 ;
                                        }
                                        else {
                                            tmpValues.hasUB=true;
                                            tmpValues.upperBound=elemChild3.attribute("ub").toDouble();
                                        }

                                        modelConstants.variables.append(tmpValues);
                                }                                                                                               //</var>
                               child3=child3.nextSibling();
                            }
                      }                                                                                                   //</variables>

                      if (elemChild2.tagName()=="objectives") {                                                           //<objectives>

                            modelConstants.nrOfConstraints=elemChild2.attribute("numberOfConstraints").toLong();

                            QDomNode child3 = child2.firstChild();
                            QDomElement elemChild3 = child3.toElement();
                            // cmpl uses only one objective
                            modelNames.objName=elemChild3.attribute("name");
                            modelNames.objSense=elemChild3.attribute("maxOrMin");

                      }                                                                                                   //</objectives>

                      if (elemChild2.tagName()=="constraints") {                                                          //<constraints>

                            modelConstants.nrOfConstraints=elemChild2.attribute("numberOfConstraints").toLong();

                            QDomNode child3 = child2.firstChild();

                            while(!child3.isNull()) {

                                QDomElement elemChild3 = child3.toElement();

                                if (elemChild3.tagName()=="con") {                                                              //<con>
                                        modelNames.conNames.append(elemChild3.attribute("name"));

                                        tmpValues.type="";

                                        if (elemChild3.attribute("lb","ndef")=="ndef"){
                                            tmpValues.hasLB=false;
                                            tmpValues.lowerBound=0 ;
                                        }
                                        else {
                                            tmpValues.hasLB=true;
                                            tmpValues.lowerBound=elemChild3.attribute("lb").toDouble();
                                        }

                                        if (elemChild3.attribute("ub","ndef")=="ndef"){
                                            tmpValues.hasUB=false;
                                            tmpValues.upperBound=0 ;
                                        }
                                        else {
                                            tmpValues.hasUB=true;
                                            tmpValues.upperBound=elemChild3.attribute("ub").toDouble();
                                        }

                                        if ( tmpValues.hasLB && tmpValues.hasUB ) tmpValues.type="E";
                                        if ( !tmpValues.hasLB && tmpValues.hasUB ) tmpValues.type="L";
                                        if ( tmpValues.hasLB && !tmpValues.hasUB ) tmpValues.type="G";


                                        modelConstants.constraints.append(tmpValues);
                                }                                                                                               //</con>
                               child3=child3.nextSibling();
                            }
                      }                                                                                                      //</constraints>

                      if (elemChild2.tagName()=="linearConstraintCoefficients") {                                            //<linearConstraintCoefficients>

                            QDomNode child3 = child2.firstChild();

                            while(!child3.isNull()) {

                                QDomElement elemChild3 = child3.toElement();

                                if (elemChild3.tagName()=="start") {                                                                  //<start>

                                    QDomNode child4 = child3.firstChild();
                                    while(!child4.isNull()) {

                                        QDomElement elemChild4 = child4.toElement();
                                        if (elemChild4.tagName()=="el")
                                             start.append(elemChild4.text().toLong());

                                        child4=child4.nextSibling();
                                    }
                                }                                                                                                     //</start>

                                if (elemChild3.tagName()=="rowIdx") {                                                                  //<rowIdx>

                                    QDomNode child4 = child3.firstChild();
                                    while(!child4.isNull()) {

                                        QDomElement elemChild4 = child4.toElement();
                                        if (elemChild4.tagName()=="el")
                                             rowIdx.append(elemChild4.text().toLong());

                                        child4=child4.nextSibling();
                                    }
                                }                                                                                                     //</rowIdx>

                                if (elemChild3.tagName()=="value") {                                                                  //<value>

                                    QDomNode child4 = child3.firstChild();
                                    int row=0;
                                    int col=0;

                                    while(!child4.isNull()) {

                                        QDomElement elemChild4 = child4.toElement();

                                        if (elemChild4.tagName()=="el") {
                                            tmpCoeff.row=rowIdx[row];

                                            if (row>0)
                                                //if (rowIdx[row]<rowIdx[row-1])
                                                if (row==start[col+1])
                                                    col++;
                                            if (start[col]==start[col+1])
                                                    col++;

                                            tmpCoeff.col=col;
                                            tmpCoeff.value=elemChild4.text().toDouble();

                                            cfModelConstCoefficients.append(tmpCoeff);

                                            row++;
                                        }
                                        child4=child4.nextSibling();
                                    }
                                }                                                                                                     //</value>

                                child3=child3.nextSibling();
                            }
                      }                                                                                                         //</linearConstraintCoefficients>

                      child2=child2.nextSibling();
                  }
              }                                                                                                             //</instanceData>

        child1=child1.nextSibling();
   }                                                                                                              //</osil>

   start.clear();
   rowIdx.clear();

   return true;

}//end cfReadOsil


/* ************** calculateConstraintCoefficients ************ */
void cfResult::calculateConstraintCoefficients() {

    long tmpCol=0;
    long tmpRow=0;
    double tmpValue;

    for (long j=0; j < resultStatus.nrOfSolutions; j++) {
        for (long i=0; i< cfModelConstCoefficients.length() ; i++) {
            tmpCol=cfModelConstCoefficients[i].col;
            tmpRow=cfModelConstCoefficients[i].row;
            tmpValue=cfModelConstCoefficients[i].value;

            results[j].constraints[tmpRow].activity += tmpValue * results[j].variables[tmpCol].activity ;
         }
    }

    cfModelConstCoefficients.clear();

}//end calculateConstraintCoefficients



/* ************** setCfOsMessage ************ */
void cfResult::setCfOsMessage(const QString &msg) {
    cfOsMessage=msg;
}//end setCfOsMessage


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
