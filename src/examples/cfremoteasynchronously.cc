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

#include "cfremoteasynchronously.hh"


void cFRemoteAsynchronously::runService( string pName, string serviceName)
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


        int result = myProblem.send();
        if (result==SEND_OK) {
            result=myProblem.knock();
            int waitCounter=0;
            while (result!=IDLE && waitCounter<20) {
                //wait 10 seconds
                QTime stopTime = QTime::currentTime().addSecs(10);
                while( QTime::currentTime() < stopTime ) {}
                cout << "iteration " << waitCounter << " status " << result <<endl;
                waitCounter++;
                result=myProblem.knock();
            }
            if (result==IDLE) {
                result=myProblem.retrieve();
                if (result=RETRIEVE_OK)
                     myProblem.result.standardReport();
            }
        } else
             myProblem.cmpLMsg.standardReport();

    }
    catch ( cFException& cFE ) {
            cFE.getMessage();
    }


}
