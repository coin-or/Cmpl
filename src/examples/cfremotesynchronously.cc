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

#include "cfremotesynchronously.hh"

cFRemoteSynchronously::cFRemoteSynchronously() {}


void cFRemoteSynchronously::solve(string pName, string serviceName)
{

    cmplFrame myProblem;
    QString myModel= QString::fromStdString(pName) ;
    QString serviceUrl= QString::fromStdString(serviceName);
	
	if (!myProblem.isStarted())
        cerr << "cmpl environment error :" << myProblem.getcFMessage() << endl;
	
    try {

        myProblem.setModel(myModel);

        myProblemIo.setCmplOut(&myProblem);

        myProblem.setServiceLocation(serviceUrl);

        int solveResult = myProblem.solve();
        switch (solveResult) {
            case SOLVE_OK: // status OK
                    myProblem.result.standardReport();
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

}
