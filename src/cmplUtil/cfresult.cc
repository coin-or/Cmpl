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

        } else {
            if (!cfReadOsrl()) return false;
            calculateConstraintCoefficients();
        }
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


        rOut.precision( glvar->args->maxDecimals()+1 );
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

                rOut << setw(20) <<   left <<  varName(i);
                rOut << setw(5) <<  right << varType(i);
                rOut << setw(20) << right << varActivity(n,i);

                if (varHasLowerBound(i))
                    rOut << setw(20) << right << varLowerBound(i);
                else
                    rOut << setw(20)  << right<< "-Infinity";
                if (varHasUpperBound(i))
                    rOut << setw(20) << right << varUpperBound(i);
                else
                    rOut << setw(20)  << right<< "Infinity";
                if (varHasMarginal(n,i))
                    rOut << setw(20)  << right<< varMarginal(n,i);
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
                rOut << setw(20) << left <<  constraintName(i);
                rOut << setw(5) <<  right << constraintType(i);
                rOut << setw(20) << right<< constraintActivity(n,i);
                if (constraintHasLowerBound(i))
                    rOut << setw(20)  << right << constraintLowerBound(i);
                else
                    rOut << setw(20) << right<< "-Infinity";
                if (constraintHasUpperBound(i))
                    rOut << setw(20) << right << constraintUpperBound(i);
                else
                    rOut << setw(20) << right<< "Infinity";
                if (constraintHasMarginal(n,i))
                    rOut << setw(20) << right << constraintMarginal(n,i);
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
