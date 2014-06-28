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

                if (mps.isOut()) {
                    QFile f (QString::fromStdString(mps.fileName()));
                    f.remove();
                }
                QFile f1 (QString::fromStdString(gsolFile()));
                f1.remove();
                if (osil.isOut()) {
                    QFile f2 (QString::fromStdString(osil.fileName()));
                    f2.remove();
                }
                QFile f3 (QString::fromStdString(osrlFile()));
                f3.remove();
                QFile f4 (QString::fromStdString(cFconfFile()));
                f4.remove();
                if (!osolFile().empty()) {
                    QFile f5 (QString::fromStdString(osolFile()));
                    f5.remove();
                }
                QFile f6 (QString::fromStdString(scipSetFile()));
                f6.remove();

            }
        }
#endif


    } // end ~CmplArgs


    /* **************** setDefaults ************* */
    void CmplArgs::setDefaults() {

        _cmplFile="";

        _scriptMode=false;
        _checkOnlySyntax=false;

        _integerRelaxation=false;

        _errorOut=false;
        _errorFile="";

        _protocolOut=false;
        _protocolFile="";

        osil.setDefaults();
        mps.setDefaults();
        stat.setDefaults();
        varprod.setDefaults();
        matrix.setDefaults();
        cmplInfo.setDefaults();

        _mpsOut=false;

        _silent=false;

        _freeMps = true;
        _doubleNumberFormat = "%f";
        _isVarProdFollow = 0;
        _isNoReduct = true;
        _constDefNoWarn = true;
        _assignOldNoWarn = false;
        _numOpMode = 1;

#ifdef SOLVERSERVICE

        _solverMode = true;
        _modelMode = false;
        _remoteMode = false;

        _maxDecimals=5;
        _dontRemoveTmpFiles=false;
        _includesForbidden=false;
        _ignoreZeros = false;
        _solutionPool = false;
        _ignoreVars = false;
        _ignoreCons = false;

        _solverOpts="";

        _solverName = "";
        _solverService = "";

        _osolFile="";
        _alias = "";

        _csvOut = false;
        _csvName = "";
        _asciiOut = false;
        _asciiName = "";
        _cmplSolOut = false;
        _cmplSolName = "";

        _stdioOut = true;
        _noOutput = false;
        _objName ="";
        _objSense=0;

        _resultZeroPrecision=1e-9;
        _negInfinity=1e-12;

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

        if ( _scriptMode && _cmplDataElementsList.size()==0 ) {
            if (dataFileNameListNumber(_cmplDataFileName)==-1) {
                newDataElement(_cmplDataFileName);
            }
        }
#ifdef SOLVERSERVICE
        writeSolverOpts();
#endif

        return ret;
    } // end checkArgs


    /* **************** dataFileNameListNumber ************* */
    int CmplArgs::dataFileNameListNumber(string str) {
        int element=-1;
        for (int i=0; i<_cmplDataElementsList.size(); i++) {
            if (_cmplDataElementsList[i].fileName==str)
                element=i;
        }
        return element;
    } // end dataFileNameListNumber

     /* **************** newDataElement ************* */
    int CmplArgs::newDataElement(string fName){
        cmplDataElements tmpData;
        tmpData.fileName=fName;
        _cmplDataElementsList.push_back(tmpData);
        return _cmplDataElementsList.size()-1;
    } // end newDataElement

    /* **************** readCmplHeader ************* */
    void CmplArgs::readCmplHeader() throw (ArgError) {

        ifstream cplFile (_cmplFile.c_str(), ifstream::in );
        string line;
        int lineNr=0;
        int argCount;
        osolOpts tmpOsol;
        numberOfOpts=0;

        int elementFile;
        bool isNameSpaceSet, nameSectionClosed;

        string defaultDataFileName=_cmplDataFileName;

        string tmpNameSpace="default";
        string tmpName="";

        string nameStr, elementStr;
        size_t pos,pos1,l;
        string subs, subs1, subs2, subs3;

        bool commentSection=false;

        if (cplFile) {
            while ( getline(cplFile , line ) ) {
                if (CmplTools::stringStartsWith(CmplTools::lTrim(line),"%"))  {
                    _headerOptList.push_back(line);
                }
            }
            cplFile.close();
        } else
            throw ArgError(false, "Can't read cmpl file :" + _cmplFile);


        for (int i=0; i< _headerOptList.size(); i++)   {

            line = CmplTools::lTrim(_headerOptList[i]);
            lineNr++;

            if (CmplTools::stringStartsWith(line,"/*")) {
                commentSection=true;
                continue;
            }

            if (CmplTools::stringContains(line,"*/")) {
                commentSection=false;
            }
            
            if (commentSection)
                continue;

            if (!CmplTools::stringStartsWith(line,"%"))
                continue;

            if (CmplTools::stringStartsWith(line,"%arg")) {
                istringstream iss(line);
                argCount=0;
                while (iss) {
                    subs="";
                    iss >>  subs;
                    if (CmplTools::stringStartsWith(subs,"\"")) {
                        size_t start = line.find_first_of("\"");
                        size_t stop = line.find_last_of("\"");
                        if (stop==string::npos) {
                            throw ArgError( modelName() ,lineNr, subs, "Missing closing quotation mark" );
                        }
                        subs=line.substr(start+1,stop-start-1);
                        argList.push_back( subs );
                        break;
                    }

                    if (!subs.empty() && !CmplTools::stringStartsWith(subs,"%arg")) {
                        argCount++;
                        if ( argCount<2 || ( argCount==2 && CmplTools::stringStartsWith(argList.at(argList.size()-1),"-") && !CmplTools::stringStartsWith(subs,"-") ) )
                            argList.push_back( subs );
                        else
                            throw ArgError( modelName() ,lineNr, subs, "Please put this argument in a new line" );
                    }
                }
                continue;
            }

            if (CmplTools::stringStartsWith(line,"%data")) {

                elementFile=-1;
                tmpNameSpace="default";
                if (_scriptMode && _cmplDataFileName!="")
                    tmpName=_cmplDataFileName;
                else
                    tmpName=defaultDataFileName;

                nameSectionClosed=true;
                isNameSpaceSet=false;

                if ((pos=line.find_first_of(":"))!=string::npos) {
                    nameStr=line.substr(0,pos);
                    elementStr=CmplTools::lrTrim(line.substr(pos+1));
                } else {
                    nameStr=line;
                    elementStr="";
                }

                nameStr=CmplTools::lTrim(nameStr);
                nameStr=nameStr.substr(5);

                argCount=0;
                istringstream nstr(nameStr);
                while (nstr) {
                    subs="";
                    nstr >>  subs;
                    if (!subs.empty()) {

                        argCount++;

                        if ( argCount==1 && CmplTools::stringStartsWith(subs,"\"")  ) {
                            if (!CmplTools::stringEndsWith(subs,"\""))
                                nameSectionClosed=false;
                            tmpName=CmplTools::lrTrim(subs);
                            continue;
                        }

                        if ( argCount==1 && !CmplTools::stringStartsWith(subs,"\"")  ) {
                            tmpName=CmplTools::lrTrim(subs);
                            continue;
                        }

                        if (argCount>1 && !nameSectionClosed ) {
                            tmpName+=subs;
                            if (CmplTools::stringEndsWith(subs,"\""))
                                nameSectionClosed=true;
                            continue;
                        }

                        if (argCount>1 && nameSectionClosed && !isNameSpaceSet) {
                            tmpNameSpace=CmplTools::lrTrim(subs);
                            isNameSpaceSet=true;
                            continue;
                        }

                        if (argCount>1 && isNameSpaceSet )
                            throw ArgError( modelName() ,lineNr, subs, "Wrong cmplData file argument" );
                    }
                }

                if (tmpName!=defaultDataFileName) {
                    tmpName=CmplTools::lrTrim(tmpName);
                    CmplTools::cleanDoubleQuotes(tmpName);

                    tmpName=CmplTools::replaceString(tmpName,"\\","/");

                    if ( (l =tmpName.find_first_of(":"))!=string::npos)
                        tmpName=tmpName.substr(l+1);

                    if ( !CmplTools::stringStartsWith(tmpName,"/") && !CmplTools::stringStartsWith(tmpName,".."))
                        tmpName=CmplTools::problemPath(_cmplFile.c_str())+tmpName;
                } else {
                    tmpName=CmplTools::replaceString(tmpName,"\\","/");
                }

                if ((elementFile=dataFileNameListNumber(tmpName))==-1) {
                    elementFile=newDataElement(tmpName);
                }

                istringstream estr(elementStr);

                string whitespaces (" \t\f\v\n\r");
                bool openSetsExpr=false;

                while (getline(estr,subs,',')) {
                    subs1="";
                    subs2="";

                    if (openSetsExpr) {
                        subs=subs3+","+subs;
                        openSetsExpr=false;
                    } else
                        subs3="";

                    if (!subs.empty()) {
                        subs=CmplTools::lrTrim(subs);
                        if ((pos=subs.find_first_of(whitespaces))!=string::npos) {
                            subs1=subs.substr(0,pos);
                            subs2=CmplTools::lrTrim(subs.substr(pos));
                        } else
                            subs1=subs;

                        subs1=CmplTools::lrTrim(subs1);
                        if ( CmplTools::stringStartsWith( subs2, "set" )) {

                            if ((pos=subs2.find_first_of("["))!=string::npos) {
                                if ((pos1=subs2.find_first_of("]"))!=string::npos)
                                    subs3=CmplTools::lrTrim(subs2.substr(pos+1,pos1-pos-1));
                                else
                                    throw ArgError( modelName() ,lineNr, subs1, "Wrong definition of the set rank, missing ] " );
                            } else {
                                subs3="1";
                            }

                            _cmplDataElementsList[elementFile].elements.push_back(subs1);
                            _cmplDataElementsList[elementFile].rank.push_back(subs3);
                            _cmplDataElementsList[elementFile].isSet.push_back(true);
                            _cmplDataElementsList[elementFile].arrSetStr.push_back("");


                        }  else  {

                            if ((pos=subs.find_first_of("["))!=string::npos) {

                                subs1=subs.substr(0,pos);
                                subs2=CmplTools::lrTrim(subs.substr(pos));

                                if ((pos=subs2.find_first_of("]"))!=string::npos)
                                    subs3=CmplTools::lrTrim(subs2.substr(1,pos-1));
                                else {
                                    openSetsExpr=true;
                                    subs3=subs;
                                    //throw ArgError( modelName() ,lineNr, subs1, "Wrong array definition, missing ] " );
                                    continue;
                                }

                            } else
                                subs1=subs;

                            CmplTools::cleanStringFromWhiteSpaces(subs3);

                            subs1=CmplTools::lrTrim(subs1);
                            if ( subs1.find_first_of(whitespaces)!=string::npos )
                                throw ArgError( modelName() ,lineNr, subs1, "Wrong parameter array definition " );

                            _cmplDataElementsList[elementFile].elements.push_back(subs1);
                            _cmplDataElementsList[elementFile].rank.push_back("0");
                            _cmplDataElementsList[elementFile].isSet.push_back(false);
                            _cmplDataElementsList[elementFile].arrSetStr.push_back(subs3);
                        }

                        _cmplDataElementsList[elementFile].elementFound.push_back(false);
                        _cmplDataElementsList[elementFile].nameSpace.push_back(tmpNameSpace);

                    }

                    continue;
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

                        if (argCount>3 )
                            throw ArgError( modelName() ,lineNr, subs, "To much options per line" );

                        if( !(tmpOsol.solver=="cbc" || tmpOsol.solver=="glpk" || tmpOsol.solver=="gurobi" || tmpOsol.solver=="scip" || tmpOsol.solver=="cplex") )
                            throw ArgError( modelName() ,lineNr, subs,  "Only CBC, CPLEX, GLPK, Gurobi or Scip solver parameters permitted in this version." );
                    }
                }

                if (argCount<1 )
                    throw ArgError( modelName() ,lineNr,  "%opt", "No solver defined" );
                if (argCount<2 )
                    throw ArgError(modelName() ,lineNr, tmpOsol.solver, "No option defined" );

                if (tmpOsol.solver=="glpk")
                    tmpOsol.name="--"+tmpOsol.name;

                if (( tmpOsol.solver=="gurobi" || tmpOsol.solver=="scip" || tmpOsol.solver=="cplex") && argCount != 3)
                    throw ArgError(modelName() ,lineNr,  tmpOsol.name, "No parameter value defined" );
                if (tmpOsol.solver=="gurobi" && ( CmplTools::lowerCase(tmpOsol.name)=="nodefiledir" || CmplTools::lowerCase(tmpOsol.name)=="logfile" || CmplTools::lowerCase(tmpOsol.name)=="resultfile" ))
                    throw ArgError(modelName() ,lineNr, tmpOsol.name, "Parameter not supported" );

                if (argCount!=0) {
                    numberOfOpts++;
                    osolList.push_back(tmpOsol);
                }
                continue;
            }

            if (CmplTools::stringStartsWith(line,"%display")) {
                QStringList optList = QString::fromStdString(line).split(" ", QString::SkipEmptyParts);

                if (optList[1].toLower()=="nonzeros") {
                    argList.push_back( "-ignoreZeros" );
                } else if (optList[1].toLower()=="solutionpool") {
                    argList.push_back( "-solutionpool" );
                } else if (optList[1].toLower()=="ignorevars") {
                    argList.push_back( "-ignoreVars" );
                } else if (optList[1].toLower()=="ignorecons") {
                    argList.push_back( "-ignoreCons" );
                }
                else {

                    if (optList.size()<2) throw ArgError(modelName() ,lineNr, "%display", "Neither var or con or nonZeros are defined" );
                    if (optList.size()<3) throw ArgError(modelName() ,lineNr, "%display", "No variable or constraint name defined" );

                    if (optList[1].toLower()=="var") {
                        for (int i=2;i<optList.length();i++)
                            _displayVarList.push_back(optList[i].toStdString());

                    } else  if (optList[1].toLower()=="con") {
                        for (int i=2;i<optList.length();i++)
                            _displayConList.push_back(optList[i].toStdString());

                    } else throw ArgError(modelName() ,lineNr, optList[1].toStdString(), "Wrong display parameter" );
                }
                continue;
            }

        }

#endif

    } // end readCmplHeader


#ifdef SOLVERSERVICE
    /* **************** writeSolverOpts ************* */
    void CmplArgs::writeSolverOpts() throw (ArgError) {

        if (_solverName=="cbc" || _solverName.empty()) {

            for (int i=0; i<osolList.size(); i++) {
                if (osolList[i].solver=="cbc")
                    _solverOpts+=" "+osolList[i].name + " " +osolList[i].value;
            }

        } else if (_solverName=="glpk" ) {

            for (int i=0; i<osolList.size(); i++) {
                if (osolList[i].solver=="glpk")
                    _solverOpts+=" "+osolList[i].name + " " +osolList[i].value;
            }

        } else if (CmplTools::lowerCase(_solverName)=="gurobi" ) {

            for (int i=0; i<osolList.size(); i++) {
                if (osolList[i].solver=="gurobi")
                     _solverOpts+=" "+osolList[i].name + "=" +osolList[i].value;
            }

        } else if (CmplTools::lowerCase(_solverName)=="cplex" ) {

            for (int i=0; i<osolList.size(); i++) {
                if (osolList[i].solver=="cplex") {
                     QStringList optList = QString::fromStdString(osolList[i].name).split("/", QString::SkipEmptyParts);
                     _solverOpts+="set ";
                     for (int i=0;i<optList.length();i++)
                        _solverOpts+= optList[i].toStdString() + " ";
                     _solverOpts+=osolList[i].value+"\n\n";
                 }
            }

        } else if (CmplTools::lowerCase(_solverName)=="scip" ) {

            if ( osolList.size()>0 ) {

                ofstream oOut;
                string _setFile = scipSetFile();
                oOut.open( _setFile.c_str()  );

                if (!oOut)
                    throw ArgError(true, "Internal Error - Can write the Scip settings file: "+ _setFile  );

                for (int i=0; i<osolList.size(); i++) {
                    if (osolList[i].solver=="scip")
                        oOut << osolList[i].name << " = " << osolList[i].value << endl;
                }
                oOut.close();
            }
        }

    } // end writeSolverOpts
#endif


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

        cArg=CmplTools::lrTrim(argList[1]);
        if ( argList.size()==2 && !CmplTools::stringStartsWith(cArg, "-")  ) {
            _cmplFile=cArg;
            _freeMps=true;
            _solverMode=true;

            mps.setOutputActive(true);
            mps.setOutMode(FILE_OUT);
            mps.setFileName(CmplTools::problemName(_cmplFile.c_str())+".mps");

            _cmplDataFileName= CmplTools::problemName(_cmplFile.c_str())+".cdat";

            return 0;
        }

        for (  int i = 1; i < argList.size(); i++ ) {

            cArg=CmplTools::lrTrim(argList[i]);

            if ( cArg==""  ) continue;

            if (i>0)
                bArg=CmplTools::lrTrim(argList[i-1]);
            nArg="";
            if(i<argList.size()-1)
                nArg=CmplTools::lrTrim(argList[i+1]);

            isParameterContent = ( i+1>=argList.size() ? false :  !(nArg.substr(0, 1) == "-" ) );

            if ( cArg =="-h" ) {
                ArgError::usage(0);
                return 1;
            }

            if (cArg =="-v" ) {
                version();
                return 1;
            }

            if ( cArg =="-remote" ) {
               _remoteMode=true;
               _solverMode=true;
               _stdioOut=false;
               //_cmplSolOut=true;
               continue;
            }


            /* Kennzeichen fuer Generierung */
            if ( CmplTools::stringStartsWith(cArg,"-g") ) {
                if (cArg == "-gn")
                    _isNoReduct = false;
                else if (cArg == "-gf")
                    _isVarProdFollow = true;
                else
                    throw ArgError(cArg,"Invalid generation option");
                continue;
            }

            /* Kennzeichen fuer Berechnungen waehrend der Generierung */

            if ( CmplTools::stringStartsWith(cArg, "-ci") ) {

                if ( !isParameterContent )
                    throw ArgError(cArg,  "No mode for integer expressions" );

                if (!CmplTools::stringToInt( nArg,  _numOpMode ))
                    throw ArgError(nArg, "Invalid option for integer expressions");
                else i++;

                if (_numOpMode<0 || _numOpMode>3  )
                    throw ArgError(nArg, "Invalid option for integer expressions");
                continue;
            }
            if (cArg == "-cd") {
                _constDefNoWarn = false;
                continue;
            }
            if (cArg == "-ca") {
                _assignOldNoWarn = true;
                continue;
            }

            /* Format für numerische Ausgaben in MPS OSiL  */
            if ( CmplTools::stringStartsWith( cArg, "-f%") ) {
                if ( !isParameterContent )
                    throw ArgError(cArg,  "No expression for format spezifier for float number output" );
                _doubleNumberFormat =  nArg;
                i++;
                continue;
            }


            if (!_remoteMode) {

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

                /* cmplDataFile */
                if (cArg == "-data")  {

                    if (!_scriptMode) {
                        _cmplDataFileName="";
                        if ( !isParameterContent ) {
                            //  throw ArgError(cArg, "No cmplData file name specified");
                        }
                        else {
                            _cmplDataFileName = nArg;
                            i++;
                        }

                        _scriptMode=true;

                    } else throw ArgError(cArg, "Multiple cmplDataFiles files");

                    continue;
                }

                if (cArg == "-m" || cArg == "-fm") {
                    if ( isParameterContent ) {
                        if ( !mps.fileName().empty())
                            throw ArgError(cArg, "Multiple MPS files");
                        mps.setFileName(nArg);
                        mps.setOutMode(FILE_OUT);

                        i++;
                    } else mps.setOutMode(STD_OUT);

                    mps.setOutputActive(true);
                    if (cArg=="-m") {
                        _freeMps=false;
                    }
                    else
                        _freeMps=true;
                    _modelMode=true;
                    _solverMode=false;
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
                if (_remoteMode)
                    matrix.setOutMode(STRING_OUT);
                continue;
            }

            if (cArg == "-e") {
                if ( isParameterContent ) {
                    if (!_remoteMode) {
                        if ( !_errorFile.empty())
                            throw ArgError(cArg, "Multiple error files");
                        _errorFile = nArg;
                        i++;
                    } else {
                        throw ArgError(cArg, "It is not allowed to specify an error file if using CmplServer");
                    }

                }
                _errorOut=true;
                continue;
            }

            if (cArg == "-integerRelaxation") {
                _integerRelaxation=true;
                continue;
            }

            if (!_remoteMode) {

                if (cArg == "-syntax") {
                    _checkOnlySyntax=true;
                    _noOutput=true;
                    //_modelMode=true;
                    //_solverMode=false;
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

                if (_remoteMode)
                    varprod.setOutMode(STRING_OUT);
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

                if (_remoteMode)
                    stat.setOutMode(STRING_OUT);

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

            if (cArg=="-zeroPrecision") {
                if ( !isParameterContent  )
                    throw ArgError(cArg, "zero tolerance decimals not defined");
                if (!CmplTools::stringToDouble( nArg,  _resultZeroPrecision ))
                    throw ArgError(nArg, "Invalid option for max decimals");
                else i++;
                continue;
            }

            if (!remoteMode()) {

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

            }

            if (cArg=="-includesForbidden") {
                _includesForbidden=true;
                continue;
            }


            if (cArg=="-ignoreZeros") {
                _ignoreZeros=true;
                continue;
            }

            if (cArg=="-ignoreVars") {
                _ignoreVars=true;
                continue;
            }

            if (cArg=="-ignoreCons") {
                _ignoreCons=true;
                continue;
            }

            if (cArg=="-solutionpool") {
                _solutionPool=true;
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
                if (_solverName != "cbc" && _solverName != "glpk" && _solverName != "scip"  && _solverName != "gurobi" && _solverName != "cplex")
                       throw ArgError(nArg, "Unsupported solver");
                continue;
            }

            if (cArg=="-cmplUrl") {
                 if ( isParameterContent )
                     i++;
                continue;
            }

            if (cArg=="-maxServerTries") {
                 if ( isParameterContent )
                     i++;
                continue;
            }

            if (cArg=="-maxQueuingTime") {
                 if ( isParameterContent )
                     i++;
                continue;
            }

            if (cArg=="-cmplJava") {
                 if ( isParameterContent )
                     i++;
                continue;
            }

            if (!remoteMode()) {

                if (cArg=="-alias") {
                    if ( isParameterContent  ) {
                        if ( !_alias.empty())
                            throw ArgError(cArg, "Multiple aliases");
                        _alias = nArg;
                        i++;
                    } else
                        throw ArgError(nArg, "No alias specified" );
                    continue;
                }

            }

            if (cArg=="-headerOpt") {
                if ( isParameterContent  ) {

                    //stringstream optList(nArg);
                    string tmpOpt=CmplTools::replaceString(nArg,"#"," ");

                   // while ( getline(optList,tmpOpt,'#')) {
                     _headerOptList.push_back(tmpOpt);
                    //}

                    i++;
                } else
                    throw ArgError(nArg, "No header option specified" );
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

            if (cArg=="-solution") {
                if ( isParameterContent  ) {
                    if ( !_cmplSolName.empty())
                        throw ArgError(cArg, "Multiple cmplSolution files");
                    _cmplSolName = nArg;
                    i++;
                }
                _cmplSolOut=true;
                _solverMode=true;
                _stdioOut=false;
                continue;
            }

            if (cArg=="-solutionCsv") {
                if ( isParameterContent  ) {
                    if ( !_csvName.empty())
                        throw ArgError(cArg, "Multiple CSV solution files");
                    _csvName = nArg;
                    i++;
                }
                _solverMode=true;
                _stdioOut=false;
                _csvOut=true;
                continue;
            }

            if (cArg=="-solutionAscii") {
                if ( isParameterContent  ) {
                    if ( !_asciiName.empty())
                        throw ArgError(cArg, "Multiple Ascii solution files");
                    _asciiName = nArg;
                    i++;
                }
                _solverMode=true;
                _stdioOut=false;
                _asciiOut=true;
                continue;
            }

#endif
            if (  i==1 && !CmplTools::stringStartsWith( cArg, "-")   ) {
                if (!_cmplFile.empty())
                    throw ArgError(cArg, "Multiple input files");
                _cmplFile = cArg;
            } else
                if (!_remoteMode)
                    throw ArgError(cArg, "Invalid option" );
                else
                    throw ArgError(cArg, "Invalid option for using a CmplServer" );
        }

#ifdef SOLVERSERVICE

        if (_cmplFile.empty())
            throw ArgError("No input file" );
        else if(!_modelMode) {
            _freeMps=true;
            _solverMode=true;

            mps.setOutputActive(true);
            mps.setOutMode(FILE_OUT);
            if (_alias.empty())
                mps.setFileName(CmplTools::problemName(_cmplFile.c_str())+".mps");
            else
                mps.setFileName(CmplTools::problemPath(_cmplFile.c_str())+_alias+".mps");

        }

        if (_csvName.empty()) {
            if (_alias.empty())
                _csvName= CmplTools::problemName(_cmplFile.c_str())+".csv";
            else
                _csvName=CmplTools::problemPath(_cmplFile.c_str())+_alias+".csv";
        }

        if (_asciiName.empty() ) {
            if (_alias.empty())
                _asciiName= CmplTools::problemName(_cmplFile.c_str())+".sol";
            else
                _asciiName=CmplTools::problemPath(_cmplFile.c_str())+_alias+".sol";
        }

        if (_cmplSolName.empty()) {
            if (_alias.empty())
                _cmplSolName= CmplTools::problemName(_cmplFile.c_str())+".csol";
            else
                _cmplSolName=CmplTools::problemPath(_cmplFile.c_str())+_alias+".csol";
        }

        if (_errorFile.empty()) {
            if (_alias.empty())
                _errorFile= CmplTools::problemName(_cmplFile.c_str())+".cmsg";
            else
                _errorFile=CmplTools::problemPath(_cmplFile.c_str())+_alias+".cmsg";
        }

        if (_cmplDataFileName.empty()) {
            if (_alias.empty())
                _cmplDataFileName= CmplTools::problemName(_cmplFile.c_str())+".cdat";
            else
                _cmplDataFileName=CmplTools::problemPath(_cmplFile.c_str())+_alias+".cdat";
        }

        cmplInfo.setOutputActive(true);
        cmplInfo.setOutMode(FILE_OUT);
        cmplInfo.setFileName(CmplTools::problemName(_cmplFile.c_str())+".cinf");

        if (_silent) {
            _errorOut=true;
        }

        if (_solverMode && _modelMode)
             throw ArgError( "Don´t use solver and model mode at the same time" );

        if (_solverMode || osil.isOut() )
            _freeMps=true;
            //if an osil file is to be created, the internal freeMps format is to be set

        if (_solverName=="cplex" && QString::fromStdString(_cmplFile).contains(" "))
              throw ArgError( true,  "Sorry - CPLEX does not support file names that contains spaces." );

        if (_solutionPool && !(_solverName=="cplex" || _solverName=="gurobi"))
              throw ArgError( true,  "Sorry - The solutionPool feature is only implemented for Cplex and Gurobi yet." );

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


