/***********************************************************************
 *  This code is part of CMPL
 *
 *  Copyright (C) 2007, 2008, 2009, 2010, 2011
 *  Thomas Schleiff - Halle(Saale), Germany and
 *  Mike Steglich - Technical University of Applied Sciences
 *  Wildau, Germany 
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


/*************************************************************************************
 *
 * main() fuer Start als separates Programm
 *
 *************************************************************************************/

#include "CmplCore.hh"
#include "CmplArgs.hh"
#include "CmplInstance.hh"


#ifdef SOLVERSERVICE
#include "CmplIO.hh"
#include "CmplIOStream.hh"
#endif



int main(int argc, char *argv[])
{

#ifdef SOLVERSERVICE
        QCoreApplication app(argc, argv);
#ifdef UNIX
        setlocale(LC_ALL, "");
        setlocale(LC_NUMERIC, "C");
#endif
#endif

        try {
            cmplUtil::CmplArgs cH;
            if (cH.checkArgs(argc, argv)==0) {

				
#ifdef SOLVERSERVICE
                if ( cH.solverMode()  && !cH.silent() && !cH.checkOnlySyntax()) {
                     cout << "CMPL model generation - running" << endl;
                     if (!cH.silent() ) cH.version();
                } else  if (cH.checkOnlySyntax()) {
                    cout << "CMPL model syntax check - running" << endl;
                }
#endif
                cmpl::CmplCore cC(&cH);
                if ( cH.solverMode()  && !cH.silent() && !cH.checkOnlySyntax() )
                    cout << "create model instance ..." << endl;
                int ret =  cC.makeModell();

                try {
                    cmplUtil::CmplInstance cM(&cC);
                    if (ret==0) {
                        if ( cH.solverMode()  && !cH.silent() && !cH.checkOnlySyntax())
                            cout << "write model instance ..." << endl;
                        cM.writeInstanceFiles();
                    }


#ifdef SOLVERSERVICE
                    if (cH.solverMode() ) {
                        if ( ret!=0 ) {
                            if ( !cH.silent())
                                cout << "CMPL model generation - failed" <<endl;
                        }
                        else {
                            if ( !cH.silent()) {
                                cout << "CMPL model generation - finished" <<endl;
                                cout << endl << "Solver - running" << endl <<endl ;
                            }

                            cmplUtil::CmplIO cI(&cC, &cM);
                            cmplUtil::CmplIOStream cS;

                            if (!cH.silent() )
                                cS.setSolverOut(&cI);

                            ret = cI.solveModel();

                            switch (ret) {
                            case SOLVE_OK: // status OK
                                if (cH.stdioOut()) cI.standardReport(STDIO);
                                if (cH.asciiOut()) {
                                    cI.standardReport(ASCII);
                                    if (!cH.silent())
                                        cout << endl <<  "Solution written to ASCII file" <<endl;
                                }
                                if (cH.csvOut()) {
                                    cI.writeCsv();
                                    if (!cH.silent())
                                        cout << endl <<  "Solution written to CSV file" <<endl;

                                }
                                if (!cH.silent())
                                    cout << endl <<  "Solver - finished" <<endl;
                                ret=0;
                                break;
                            case SOLVER_FAILED: //  Solver failed with results
                                cI.getSolverMessages();
                                if ( !cH.silent())
                                    cout <<  "Solver - failed" <<endl;
                                break;
                            }
                        }
                    }

                }
                catch ( cmplUtil::CmplException& cE ) {
                    cE.getMessage();
                }
#endif
            return ret;
            }
        }
        catch ( cmplUtil::ArgError& eArg ) {
            eArg.getMessage();
        }

}

