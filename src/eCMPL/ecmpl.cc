/***********************************************************************
 *  This code is part of cmplFrame
 *
 *  Copyright (C) 2010 Mike Steglich - Technical University
 *  of Applied Sciences Wildau, Germany
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

#include "ecmpl.hh"

/* ************ eCmpl ************ */
eCmpl::eCmpl()
{
    setDefaults();
}// end eCmpl

/* ************ setProblem ************ */
void eCmpl::setProblem(string cmplFileName) {
    cmplFile=QString::fromStdString(cmplFileName);
    silent = false;
}//end setProblem


/* ************ setOsol ************ */
void eCmpl::setOsol(string osolName){
    osolFile=QString::fromStdString(osolName);
    isOsol=true;
}//end setOsol


/* ************ setSolver ************ */
void eCmpl::setSolver(string solver) {
    solverName=QString::fromStdString(solver);
    isSolver=true;
}//end setSolver


/* ************ setServiceLocation ************ */
void eCmpl::setServiceLocation(string serviceUrl) {
    serviceLocationUrl=QString::fromStdString(serviceUrl);
    isServiceLocation=true;
}//end setServiceLocation


/* ************ setSolverOutputCsv ************ */
void eCmpl::setSolverOutputCsv(bool x) {
    csv=x;
}//end setSolverOutputCsv


/* ************ setSolverOutputStd ************ */
void eCmpl::setSolverOutputStd(bool x) {
    std=x;
}//end setSolverOutputStd


/* ************ setSilent ************ */
void eCmpl::setSilent(bool x) {
    silent=x;
}//end setSilent



/* ************ solve ************ */
int eCmpl::solve() {

    int solveResult;

    if (!myProblem.isStarted())
        cerr << "cmpl environment error :" << myProblem.getcFMessage() << endl;

    try {

        myProblem.setModel(cmplFile);

        if (!silent) {
            myProblemIo.setCmplOut(&myProblem);
            myProblemIo.setSolverOut(&myProblem);
        }
        if (isOsol) myProblem.setOsol(osolFile);
        if (isSolver) myProblem.setSolver(solverName);
        if (isServiceLocation) myProblem.setServiceLocation(serviceLocationUrl);

        solveResult = myProblem.solve();
        switch (solveResult) {
            case SOLVE_OK: // status OK
                    if (std) myProblem.result.standardReport();
                    if (csv) myProblem.result.writeCsv();
                    break;
            case CMPL_FAILED: //  CMPL failed with messages
                     myProblem.cmpLMsg.standardReport();
                     break;
            case SOLVER_FAILED: //  Solver failed with results
                     myProblem.result.getSolverMessages();
                     break;
        }
    }
    catch ( cFException& cFE ) {
            cFE.getMessage();
    }

    return solveResult;
}//end solve



/* ************ runOSSolverServices ************ */
int eCmpl::runOSSolverServices() {

    int solveResult;

    if (!myProblem.isStarted())
        cerr << "cmpl environment error :" << myProblem.getcFMessage() << endl;

    try {

        myProblem.setModel(cmplFile);

        if (!silent)
            myProblemIo.setSolverOut(&myProblem);


        if (isOsol) myProblem.setOsol(osolFile);
        if (isSolver) myProblem.setSolver(solverName);
        if (isServiceLocation) myProblem.setServiceLocation(serviceLocationUrl);

        solveResult = myProblem.runOSSolverservice();
        switch (solveResult) {
            case SOLVE_OK: // status OK
                    if (std) myProblem.result.standardReport();
                    if (csv) myProblem.result.writeCsv();
                    break;
            case SOLVER_FAILED: //  Solver failed with results
                     myProblem.result.getSolverMessages();
                     break;
        }
    }
    catch ( cFException& cFE ) {
            cFE.getMessage();
    }

    return solveResult;

}//end runOSSolverServices


/* ************ setDefaults ************ */
void eCmpl::setDefaults() {

    cmplFile="";
    isCmplFile=false;

    osrl=false;
    csv=false;
    std=false;

    isSolver=false;
    solverName="";

    isServiceLocation=false;
    serviceLocationUrl="";

    isOsol=false;
    osolFile="";

    mprl=false;
    help=false;
    ver=false;
    silent=false;
}//end setDefaults
