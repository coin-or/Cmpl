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


#include "CmplArgs.hh"

namespace cmplUtil {

    /* **************** CmplArgs ************* */
    CmplArgs::CmplArgs()
    {
        setDefaults();
    } // end CmplArgs


    /* **************** CmplArgs ************* */
    CmplArgs::~CmplArgs()
    {

#ifdef SOLVERSERVICE
        if (_solverMode) {
            if( !_dontRemoveTmpFiles) {
                QString fName = QString::fromStdString(CmplTools::problemName(_cmplFile.c_str()));
                QFile f (fName + ".mps");
                f.remove();
                QFile f1 (fName + ".gsol");
                f1.remove();
                QFile f2 (fName + ".osil");
                f2.remove();
                QFile f3 (fName + ".osrl");
                f3.remove();
                QFile f4 (fName + ".cFconf");
                f4.remove();
                QFile f5 (fName + ".osol");
                f5.remove();
                QFile f6 (fName + ".set");
                f6.remove();
            }
        }
#endif


    } // end ~CmplArgs


    /* **************** setDefaults ************* */
    void CmplArgs::setDefaults() {

        _cmplFile="";

        _errorOut=false;
        _errorFile="";

        _protocolOut=false;
        _protocolFile="";

        osil.setDefaults();
        mps.setDefaults();
        stat.setDefaults();
        varprod.setDefaults();
        matrix.setDefaults();

        _mpsOut=false;
        _mpsFile="";

        _silent=false;

         _freeMps = 0;
        _doubleNumberFormat = "%f";
        _isVarProdFollow = 0;
        _isNoReduct = 0;
        _constDefNoWarn = false;
        _assignOldNoWarn = false;
        _numOpMode = 1;

#ifdef SOLVERSERVICE

        _solverMode = true;
        _modelMode = false;

        _maxDecimals=5;
        _dontRemoveTmpFiles=false;
        _nonCoinSolverOpts="";

        _solverName = "";
        _solverService = "";
        _osolFile = "";
        _csvOut = false;
        _asciiOut = false;
        _stdioOut = true;
        _noOutput = false;
        _objName ="";
        _objSense=0;
#endif

    } // end setDefaults



    /* **************** fillArgList ************* */
    void CmplArgs::fillArgList(int argc, char *argv[]) {
        for (int i=0; i < argc; i++ )
            argList.push_back(argv[i]);
    } // end fillArgList



    /* **************** checkArgs ************* */
    int CmplArgs::checkArgs(int argc, char *argv[])  {

        int ret;
        fillArgList(argc, argv);
        ret = parseArgs();
        if (ret!=0) return ret;
        readCmplHeader();
        ret = parseArgs();
#ifdef SOLVERSERVICE
        writeSolverOpts();
#endif
        return ret;
    } // end checkArgs



    /* **************** readCmplHeader ************* */
    void CmplArgs::readCmplHeader() throw (ArgError) {

        ifstream cplFile (_cmplFile.c_str(), ifstream::in );
        string line;
        int argCount;
        osolOpts tmpOsol;
        numberOfOpts=0;

        if (cplFile) {
            while ( getline(cplFile , line ) ) {
                line = CmplTools::lTrim(line);
                if (CmplTools::stringStartsWith(line,"%arg")) {
                    istringstream iss(line);
                    argCount=0;
                    while (iss) {
                        string subs;
                        iss >>  subs;
                        if (!subs.empty() && !CmplTools::stringStartsWith(subs,"%arg")) {
                            argCount++;
                            if ( argCount<2 || ( argCount==2 && CmplTools::stringStartsWith(argList.at(argList.size()-1),"-") && !CmplTools::stringStartsWith(subs,"-") ) )
                               argList.push_back( subs );
                            else
                               throw ArgError(true, subs, "Please put this argument in a new line" );
                        }
                    }
                }

#ifdef SOLVERSERVICE
                if (CmplTools::stringStartsWith(line,"%opt")) {
                    tmpOsol.name="";
                    tmpOsol.solver="";
                    tmpOsol.value="";

                    istringstream iss(line);
                    argCount=0;
                    while (iss) {
                        string subs;
                        iss >>  subs;

                        if (!subs.empty() && !CmplTools::stringStartsWith(subs,"%opt")) {
                            argCount++;

                            switch (argCount) {
                                case 1: tmpOsol.solver=CmplTools::lowerCase(subs); break;
                                case 2: tmpOsol.name=subs; break;
                                case 3: tmpOsol.value=subs; break;
                            }

                            if (argCount>3 ) throw ArgError(true, subs, "To much options per line" );

                            if( !(tmpOsol.solver=="cbc" || tmpOsol.solver=="glpk" || tmpOsol.solver=="gurobi" || tmpOsol.solver=="scip" || tmpOsol.solver=="osi") )
                                throw ArgError(true, subs, "Only CBC, GLPK, Gurobi or Scip solver parameters permitted in this version." );
                        }
                    }

                    if (argCount<1 ) throw ArgError(true, "%opt", "No solver defined" );
                    if (argCount<2 ) throw ArgError(true, tmpOsol.solver, "No option defined" );

                    if (tmpOsol.solver=="glpk")
                        tmpOsol.name="--"+tmpOsol.name;

                    if (( tmpOsol.solver=="gurobi" || tmpOsol.solver=="scip") && argCount != 3)
                        throw ArgError(true, tmpOsol.name, "No parameter value defined" );
                    if (tmpOsol.solver=="gurobi" && ( CmplTools::lowerCase(tmpOsol.name)=="nodefiledir" || CmplTools::lowerCase(tmpOsol.name)=="logfile" || CmplTools::lowerCase(tmpOsol.name)=="resultfile" ))
                        throw ArgError(true, tmpOsol.name, "Parameter not supported" );

                    if (argCount!=0) {
                       numberOfOpts++;
                       osolList.push_back(tmpOsol);
                   }

                }
#endif
            }
            cplFile.close();
        } else
            throw ArgError(true, " ", "Can't read cmpl file :" + _cmplFile);
    } // end readCmplHeader



    /* **************** writeSolverOpts ************* */
    void CmplArgs::writeSolverOpts() throw (ArgError) {

        if (_solverName=="cbc" || _solverName.empty()) {
            osolOpts tmpOsol;
            int nbrOfOpts=0;

            if ( osolList.size()>0 ) {
                if (!_osolFile.empty() )
                    throw ArgError( "OSol handling error - Do not use -osol and %opt at the same time" );

                ofstream oOut;
                _osolFile = CmplTools::problemName(_cmplFile.c_str())+".osol";
                oOut.open( _osolFile.c_str() );

                if (!oOut)
                    throw ArgError("Internal Error - Can write the osol file: "+_osolFile  );

                oOut << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << endl;
                oOut << "<osol xmlns=\"os.optimizationservices.org\"" << endl;
                oOut << "     xmlns:xs=\"http://www.w3.org/2001/XMLSchema\" " << endl;
                oOut << "     xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" " << endl;
                oOut << "     xsi:schemaLocation=\"os.optimizationservices.org http://www.optimizationservices.org/schemas/OSoL.xsd\">" << endl;

                oOut << "       <optimization>" << endl;
                for (int i=0; i<osolList.size(); i++) {
                    if (osolList[i].solver=="cbc" || osolList[i].solver=="osi")
                        nbrOfOpts++;
                }
                oOut << "          <solverOptions numberOfSolverOptions=\"" << nbrOfOpts << "\">" << endl;

                for (int i=0; i<osolList.size(); i++) {
                    tmpOsol=osolList[i];
                    if (tmpOsol.solver=="cbc" || tmpOsol.solver=="osi") {
                        oOut << "          <solverOption name=\"" << tmpOsol.name << "\" "  ;
                        oOut << " solver=\"" << tmpOsol.solver << "\" "  ;
                        if (!tmpOsol.value.empty())
                            oOut << " value=\"" << tmpOsol.value << "\" "  ;
                        oOut << " />" << endl;
                    }
                }
                oOut << "       </solverOptions>" << endl;
                oOut << "   </optimization>" << endl;
                oOut << "</osol>" << endl;

                oOut.close();

            }
        } else if (_solverName=="glpk" ) {

            for (int i=0; i<osolList.size(); i++) {
                if (osolList[i].solver=="glpk")
                    _nonCoinSolverOpts+=" "+osolList[i].name + " " +osolList[i].value;
            }

        } else if (CmplTools::lowerCase(_solverName)=="gurobi" ) {

            for (int i=0; i<osolList.size(); i++) {
                if (osolList[i].solver=="gurobi")
                     _nonCoinSolverOpts+=" "+osolList[i].name + "=" +osolList[i].value;
            }

        } else if (CmplTools::lowerCase(_solverName)=="scip" ) {

            if ( osolList.size()>0 ) {

                ofstream oOut;
                string _setFile = CmplTools::problemName(_cmplFile.c_str())+".set";
                oOut.open( _setFile.c_str()  );

                if (!oOut)
                    throw ArgError("Internal Error - Can write the Scip settings file: "+ _setFile  );

                for (int i=0; i<osolList.size(); i++) {
                    if (osolList[i].solver=="scip")
                        oOut << osolList[i].name << " = " << osolList[i].value << endl;
                }
                oOut.close();
            }
        }

    } // end writeSolverOpts



    /* **************** parseArgs ************* */
    int CmplArgs::parseArgs()  throw (ArgError) {

        string bArg;
        string nArg;
        string cArg;

        setDefaults();

        bool isParameterContent;

        if (argList.size()==1) {
            ArgError::usage(0);
            return 1;
        }

        cArg=argList[1];
        if ( argList.size()==2 && !CmplTools::stringStartsWith(cArg, "-")  ) {
            _cmplFile=cArg;
            _freeMps=true;
            _solverMode=true;

            osil.setOutputActive(true);
            osil.setOutMode(FILE_OUT);
            osil.setFileName(CmplTools::problemName(_cmplFile.c_str())+".osil");

            /*_solverName="glpk";
            mps.setOutputActive(true);
            mps.setOutMode(FILE_OUT);
            mps.setFileName(CmplTools::problemName(_cmplFile.c_str())+".mps");
            */
            return 0;
        }

        for (  int i = 1; i < argList.size(); i++ ) {

            cArg=argList[i];
            if (i>0)
                bArg=argList[i-1];
            nArg="";
            if(i<argList.size()-1)
                nArg=argList[i+1];

            isParameterContent = ( i+1>=argList.size() ? false :  !(nArg.substr(0, 1) == "-" ) );

            if ( cArg =="-h" ) {
                ArgError::usage(0);
                return 1;
            }

            if (cArg =="-v" ) {
                version();
                return 1;
            }

            /* Kennzeichen fuer Generierung */
            if ( CmplTools::stringStartsWith(cArg,"-g") ) {
                if (cArg == "-gn")
                    _isNoReduct = true;
                else if (cArg == "-gf")
                    _isVarProdFollow = true;
                else
                    throw ArgError(cArg,"Invalid generation option");
                continue;
            }

            /* Kennzeichen fuer Berechnungen waehrend der Generierung */
            if ( CmplTools::stringStartsWith(cArg,"-c") )  {
                if ( CmplTools::stringStartsWith(cArg, "-ci") ) {

                    if ( !isParameterContent )
                        throw ArgError(cArg,  "No mode for integer expressions" );

                    if (!CmplTools::stringToInt( nArg,  _numOpMode ))
                        throw ArgError(nArg, "Invalid option for integer expressions");
                    else i++;

                    if (_numOpMode<0 || _numOpMode>3  )
                        throw ArgError(nArg, "Invalid option for integer expressions");
                }
                else if (cArg == "-cd")
                    _constDefNoWarn = true;
                else if (cArg == "-ca")
                    _assignOldNoWarn = true;
                else
                    throw ArgError(cArg, "Invalid option for parameters");
                continue;
            }

            /* Kennzeichen fuer Ausgabedatei */
            if ( CmplTools::stringStartsWith(cArg,"-f") )  {

                if (cArg == "-ff")
                    _freeMps = true;

                else if ( CmplTools::stringStartsWith( cArg, "-f%") ) {
                    if ( !isParameterContent )
                        throw ArgError(cArg,  "No expression for format spezifier for float number output" );
                    _doubleNumberFormat =  nArg;
                    i++;
                }
                else
                    throw ArgError(cArg, "Invalid format option");
                continue;
            }

            /* Eingabedatei */
            if (cArg == "-i")  {
                if ( !isParameterContent )
                    throw ArgError(cArg,  "No input file" );

                if (!_cmplFile.empty())
                    throw ArgError(cArg, "Multiple input files");

                _cmplFile = nArg;
                i++;
                continue;
            }

            if (cArg == "-m") {
                if ( isParameterContent ) {
                    if ( !mps.fileName().empty())
                        throw ArgError(cArg, "Multiple MPS files");
                    mps.setFileName(nArg);
                    mps.setOutMode(FILE_OUT);
                    i++;
                } else mps.setOutMode(STD_OUT);

                mps.setOutputActive(true);

                _modelMode=true;
                _solverMode=false;
                continue;
            }

            if (cArg == "-matrix") {
                if ( isParameterContent ) {
                    if ( !matrix.fileName().empty())
                        throw ArgError(cArg, "Multiple matrix files");
                    matrix.setFileName(nArg);
                    matrix.setOutMode(FILE_OUT);
                    i++;
                } else matrix.setOutMode(STD_OUT);

                matrix.setOutputActive(true);
                continue;
            }

            if (cArg == "-x") {
                if ( isParameterContent ) {
                    if ( !osil.fileName().empty())
                        throw ArgError(cArg, "Multiple OSiL files");
                    osil.setFileName(nArg);
                    osil.setOutMode(FILE_OUT);
                    i++;
                } else osil.setOutMode(STD_OUT);

                osil.setOutputActive(true);
                _modelMode=true;
                _solverMode=false;
                continue;
            }

            if (cArg == "-e") {
                if ( isParameterContent ) {
                    if ( !_errorFile.empty())
                        throw ArgError(cArg, "Multiple error files");
                    _errorFile = nArg;
                    i++;
                }
                _errorOut=true;
                continue;
            }

            if (cArg == "-l") {
               if ( isParameterContent ) {
                    if ( !varprod.fileName().empty())
                        throw ArgError(cArg, "Multiple files for output for replacements for products of variables");
                    varprod.setFileName(nArg);
                    varprod.setOutMode(FILE_OUT);
                    i++;
                } else varprod.setOutMode(STD_OUT);
                varprod.setOutputActive(true);
                continue;
            }

            if (cArg == "-s") {
                if ( isParameterContent ) {
                    if ( !stat.fileName().empty())
                        throw ArgError(cArg, "Multiple Statistic files");
                    stat.setFileName(nArg);
                    stat.setOutMode(FILE_OUT);
                    i++;
                } else stat.setOutMode(STD_OUT);
                stat.setOutputActive(true);
                continue;
            }

            if (cArg == "-p") {
                if ( isParameterContent ) {
                    if ( !_protocolFile.empty())
                        throw ArgError(cArg, "Multiple protocol files");
                    _protocolFile = nArg;
                    i++;
                }
                _protocolOut=true;
                continue;
            }

            if (cArg == "-silent") {
                _silent=true;
                continue;
            }

#ifdef SOLVERSERVICE

            if (cArg=="-maxDecimals") {
                if ( !isParameterContent  )
                    throw ArgError(cArg, "Max decimals not defined");
                if (!CmplTools::stringToInt( nArg,  _maxDecimals ))
                    throw ArgError(nArg, "Invalid option for max decimals");
                else i++;
                if (_maxDecimals>12)
                     throw ArgError(nArg, "Invalid option for max decimals");

                continue;
            }

            if (cArg=="-dontRemoveTmpFiles") {
                _dontRemoveTmpFiles=true;
                continue;
            }

            if (cArg=="-noOutput") {
                _noOutput=true;
                _modelMode=true;
                _solverMode=false;
                continue;
            }

            if (cArg=="-solver") {
                if ( !isParameterContent  )
                    throw ArgError(cArg, "No solver defined");

                if ( !_solverName.empty())
                  throw ArgError(cArg, "Multiple solver definition");

                _solverMode=true;
                i++;
                _solverName=CmplTools::lowerCase(nArg);
                continue;
            }

            if (cArg=="-solverUrl") {

                if ( !isParameterContent  )
                    throw ArgError(cArg, "No solver location defined");

                if (!_solverService.empty())
                    throw ArgError(cArg, "Multiple solver URL definition");

                _solverMode=true;
                i++;
                _solverService=nArg;
                continue;
            }

            if (cArg=="-osol") {
                if ( !isParameterContent  )
                    throw ArgError(cArg, "No osol file defined");

                if (!_osolFile.empty())
                    throw ArgError(cArg, "Multiple osol file ");

                _solverMode=true;
                i++;
                _osolFile=nArg;
                continue;
            }

            if (cArg=="-obj") {
                if ( !isParameterContent  )
                    throw ArgError(cArg, "No objective function defined");

                if (!_objName.empty())
                    throw ArgError(cArg, "Multiple objective function definition");

                _solverMode=true;
                i++;
                _objName=nArg;
                continue;
            }

            if (cArg=="-objSense") {
                if ( !isParameterContent  )
                    throw ArgError(cArg, "No objective sense defined");

                if (_objSense!=0)
                    throw ArgError(cArg, "Multiple objective sense definition");

                _solverMode=true;

                if ( nArg=="max" )
                    _objSense=1;
                else if ( nArg=="min" )
                    _objSense=-1;
                else
                    throw ArgError(nArg, "Wrong objective sense" );
                i++;
                continue;
            }

            if (cArg=="-solutionCsv") {
                _solverMode=true;
                _stdioOut=false;
                _csvOut=true;
                continue;
            }

            if (cArg=="-solutionAscii") {
                _solverMode=true;
                _stdioOut=false;
                _asciiOut=true;
                continue;
            }

#endif
            if ( ( (i==1) || (i==argList.size()-1) || !CmplTools::stringStartsWith( bArg, "-") || CmplTools::stringStartsWith( nArg, "-") ) && !CmplTools::stringStartsWith( cArg, "-")   ) {
                if (!_cmplFile.empty())
                    throw ArgError(cArg, "Multiple input files");
                _cmplFile = cArg;
            } else
                throw ArgError(cArg, "Invalid option" );
        }

#ifdef SOLVERSERVICE

        if (_cmplFile.empty())
            throw ArgError("No input file" );
        else if(!_modelMode) {
            _freeMps=true;
            _solverMode=true;

            if (_solverName=="glpk" || _solverName=="gurobi" || _solverName=="scip") {
                mps.setOutputActive(true);
                mps.setOutMode(FILE_OUT);
                mps.setFileName(CmplTools::problemName(_cmplFile.c_str())+".mps");

            } else if (_solverName=="cbc" || _solverName=="clp"){
                osil.setOutputActive(true);
                osil.setOutMode(FILE_OUT);
                osil.setFileName(CmplTools::problemName(_cmplFile.c_str())+".osil");

            } else {

                osil.setOutputActive(true);
                osil.setOutMode(FILE_OUT);
                osil.setFileName(CmplTools::problemName(_cmplFile.c_str())+".osil");

                /*_solverName="glpk";
                mps.setOutputActive(true);
                mps.setOutMode(FILE_OUT);
                mps.setFileName(CmplTools::problemName(_cmplFile.c_str())+".mps");
                */

            }
        }

        if (_silent) {
            _errorOut=true;
            _errorFile = _cmplFile.substr(0,_cmplFile.find_last_of('.'))+".mprl";
        }

        if ( ( _solverName=="glpk" || _solverName=="gurobi") && !_solverService.empty())
            throw ArgError( "A remote solver service can only be used with COIN-OR solvers." );

        if (_solverMode && _modelMode)
             throw ArgError( "Don´t use solver and model mode at the same time" );

        if (_solverMode || osil.isOut() )
            _freeMps=true;
            //if an osil file is to be created, the internal freeMps format is to be set

#endif
        return 0;

    } // end parseArgs



    /* **************** version ************* */
    void CmplArgs::version() {
        cout << endl << "CMPL version: " << VERSION << endl;
        cout << "Authors: Thomas Schleiff, Mike Steglich" << endl;
        cout << "Distributed under the GPLv3 " << endl <<endl;
    } // end version



}


