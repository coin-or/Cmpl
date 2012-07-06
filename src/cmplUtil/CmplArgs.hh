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

#ifndef CMPLARGS_HH
#define CMPLARGS_HH

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

#include <vector>
using namespace std;

#include "gentext.h";
#include "CmplTools.hh"
#include "ArgError.hh"
#include "CmplFileType.hh"

#ifdef SOLVERSERVICE
#include <QCoreApplication>
#include <QStringList>
#include <QFile>
#endif


#define VERSION	  "1.7.0"

namespace cmplUtil {

    typedef struct
    {
        string name;
        string solver;
        string value;
    } osolOpts;

    typedef struct
    {
        string fileName;
        vector <string> nameSpace;
        vector <string> elements;
        vector <bool> isSet;
        vector <string> arrSetStr;
        vector <string> rank;
        vector <bool> elementFound;
        ifstream* fH;
    } cmplDataElements;

    /**
     * <code>CmplArgs</code> implements the handling of the
     * command line arguments and the CMPL header arguments
     *
     * @author  Mike Steglich
     */
    class CmplArgs
    {
    public:

        /**
         * constructor
         */
        CmplArgs();


        /**
         * destructor
         */
        ~CmplArgs();


        /**
         * Sets the CMPL argument defaults
         */
        void setDefaults();

        /**
         * Syntax and semantic check of the CMPL arguments
         * @param  argc
         * @param  *argv[]
         */
        int checkArgs(int argc, char *argv[]) ;

        /**
         * Returns the cmpl filename
         * @return  cmpl filename
         */
        inline string cmplFile() {
            return _cmplFile;
        }

        inline string modelName() {
            return _cmplFile.erase(0,_cmplFile.find_last_of("/")+1);

        }

        /**
         * Sets the cmpl filename
         * @param model -  cmpl filename
         */
        inline void setCmplFile(string model) {
            _cmplFile=model;
        }

        /**
         * Returns true|false if a error file is defined via command line argument or CMPL header
         * @return  true|false if a error file is defined
         */
        inline bool errorOut() {
            return _errorOut;
        }

        /**
         * Returns the error filename
         * @return  error filename
         */
        inline string errorFile() {
            return _errorFile;
        }

        /**
         * Returns true|false if a protocol file is defined  via command line argument or CMPL header
         * @return  true|false if a error file is defined
         */
        inline bool protocolOut() {
            return _protocolOut;
        }

        /**
         * Returns the protocol filename
         * @return  protocol filename
         */
        inline string protocolFile() {
            return _protocolFile;
        }

        /**
         * Returns true|false if a mps file is defined  via command line argument or CMPL header
         * @return  true|false if a mps file is defined
         */
        inline bool mpsOut() {
            return _mpsOut;
        }

        /**
         * Returns the mps filename
         * @return  mps filename
         */
        inline string mpsFile() {
            return _mpsFile;
        }

        /**
         * Returns true|false if free mps is chosen as mps format
         * @return  true|false
         */
        inline bool freeMps()  {
            return _freeMps;
        }

        /**
         * Sets free mps as mps format or not
         * @param  x true|false if free mps is to be chosen as mps format or not
         */
        inline void setFreeMps(bool x) {
            _freeMps=x;
        }

        /**
         * Returns the format of the double numbers
         * @return  format of double numbers
         */
        inline string doubleNumberFormat() {
            return _doubleNumberFormat;
        }

        /**
         * Returns true|false if the generated constraints for products of variables have to follow the product
         * @return  true|false
         */
        inline bool isVarProdFollow() {
            return _isVarProdFollow;
        }

        /**
         * Returns true|false if no matrix reductions are to be made or not
         * @return  true|false
         */
        inline bool isNoReduct() {
            return _isNoReduct;
        }

        /**
         * Returns true|false for warnings at multiple parameter definition
         * @return  true|false
         */
        inline bool constDefWarnings() {
            return !_constDefNoWarn;
        }

        /**
         * Returns true|false for warnings at deprecated '=' assignment
         * @return  true|false
         */
        inline bool assignOldWarn() {
            return !_assignOldNoWarn;
        }

        /**
         * Returns the mode for integer expressions (0 - 3)
         * @return  mode
         */
        inline int numOpMode() {
            return _numOpMode;
        }

        /**
         * Returns true|false if CMPL and solver messages are suppressed
         * @return  true|false
         */
        inline bool silent() {
            return _silent;
        }

        /**
         * Returns true|false if only the syntax of the CMPL model is to be checked  w/o generating of a MPS or OSiL file
         * @return  true|false
         */
        inline bool checkOnlySyntax() {
            return _checkOnlySyntax;
        }

 #ifdef SOLVERSERVICE

        /**
         * Returns true|false if solver mode is chosen
         * @return  true|false
         */
        inline bool solverMode() {
            return _solverMode;
        }

        /**
         * Returns true|false if model mode is chosen
         * @return  true|false
         */
        inline bool modelMode() {
            return _modelMode;
        }


        /**
         * Returns the solver name
         * @return  solver name
         */
        inline string solverName() {
            return _solverName;
        }

        /**
         * Sets the solver name
         */
        inline void setSolverName(string name) {
            _solverName=name;
        }

        /**
         * Returns the the url of the solverservice
         * @return  url of the solverservice
         */
        inline string solverService() {
            return _solverService;
        }

        /**
         * Returns the osol file name
         * @return  osol file name
         */
        inline string osolFile()  {
            return _osolFile;
        }

        /**
         * Returns true|false if the solver results are to be written in a CSV file
         * @return  true|false
         */
        inline bool csvOut() {
            return _csvOut;
        }

        /**
         * Returns true|false if the solver results are to be written in an ASCII file
         * @return  true|false
         */
        inline bool asciiOut() {
            return _asciiOut;
        }

        /**
         * Returns true|false if the solver results are to be written to stdOut
         * @return  true|false
         */
        inline bool stdioOut() {
            return _stdioOut;
        }

        /**
         * Returns true|false if neither MPS nor OSiL file is to be generated
         * @return  true|false
         */
        inline bool noOutput() {
            return _noOutput;
        }

        /**
         * Returns the objective function name
         * @return  name of the objective function
         */
        inline string objName() {
            return _objName;
        }

        /**
         * Returns the objective sense
         * @return  objective sense
         */
        inline int objSense() {
            return _objSense;
        }

        /**
         * Returns the options for non-COIN solvers
         */
        inline string nonCoinSolverOpts() {
            return _nonCoinSolverOpts;
        }

        /**
         * Returns maximal number of decimals in the solution report
         */
        inline int maxDecimals() {
            return _maxDecimals;
        }

        /**
         * Returns true ore false whether the zeros values are to be ignored in the solution reports
         * @return  true|false
         */
        inline int ingnoreZeros() {
            return _ignoreZeros;
        }

        /**
         * Returns the lenght of the list of the variables that are to be displayed
         */
        inline int displayVarListSize() {
            return _displayVarList.size();
        }

        /**
         * Returns the name of a variable that is to be displayed
         * @param  i  - position
         * @return variable name
         */
        inline string displayVarList(int i) {
            if (i<_displayVarList.size())
                return _displayVarList[i];
            else
                return "";
        }

        /**
         * Returns the lenght of the list of the constraints that are to be displayed
         */
        inline int displayConListSize() {
            return _displayConList.size();
        }

        /**
         * Returns the name of a contraint that is to be displayed
         * @param  i  - position
         * @return constraint name
         */
        inline string displayConList(int i) {
            if (i<_displayConList.size())
                return _displayConList[i];
            else
                return "";
        }

        /**
         * Returns the precision of zero numbers
         */
        inline double resultZeroPrecision() {
            return _resultZeroPrecision;
        }

#endif

        inline int dataFileListSize() {
            return _cmplDataElementsList.size();
        }

        inline string dataFileName(int i) {
            if (i<_cmplDataElementsList.size())
                return _cmplDataElementsList[i].fileName;
            else
                return "";
        }

        inline int dataFileNrOfElements(int i) {
            return _cmplDataElementsList[i].elements.size();
        }

        inline string dataFileElement(int i, int j) {
            if (i<_cmplDataElementsList.size()) {
                if (j<_cmplDataElementsList[i].elements.size())
                    return _cmplDataElementsList[i].elements[j];
               else  return "";
            }
            else
                return "";
        }

        inline bool dataFileElementIsSet(int i, int j) {
            if (i<_cmplDataElementsList.size()) {
                if (j<_cmplDataElementsList[i].elements.size())
                    return _cmplDataElementsList[i].isSet[j];
               else  return false;
            }
            else
                return false;
        }

        inline string dataFileElementArraySetString(int i, int j) {
            if (i<_cmplDataElementsList.size()) {
                if (j<_cmplDataElementsList[i].elements.size())
                    return _cmplDataElementsList[i].arrSetStr[j];
               else  return "";
            }
            else
                return "";
        }

        inline string dataFileElementSetRankString(int i, int j) {
            if (i<_cmplDataElementsList.size()) {
                if (j<_cmplDataElementsList[i].elements.size())
                    return _cmplDataElementsList[i].rank[j];
               else  return "";
            }
            else
                return "";
        }


        inline void setDataFileElementFound(int i, int j) {
            if (i<_cmplDataElementsList.size()) {
                if (j<_cmplDataElementsList[i].elements.size())
                    _cmplDataElementsList[i].elementFound[j]=true;

            }
        }

        inline bool dataFileElementFound(int i, int j) {
            if (i<_cmplDataElementsList.size()) {
                if (j<_cmplDataElementsList[i].elements.size())
                    return _cmplDataElementsList[i].elementFound[j];
            }
            return false;
        }

        inline string dataFileNameSpace(int i, int j) {
            if (i<_cmplDataElementsList.size()) {
                if (j<_cmplDataElementsList[i].nameSpace.size())
                    return _cmplDataElementsList[i].nameSpace[j];
                else  return "";
            }
            else
                return "";
        }

        inline void openDataFile(int i) {
            _cmplDataElementsList[i].fH = new ifstream(_cmplDataElementsList[i].fileName.c_str(), ifstream::binary );
        }

        inline void closeDataFile(int i) {
            _cmplDataElementsList[i].fH->close();
        }

        inline ifstream* dataFile(int i) {
            return _cmplDataElementsList[i].fH;
        }




        /**
         * Writes the CMPL version to stdout
         */
        void version();

        CmplFileType mps;
        CmplFileType osil;
        CmplFileType stat;
        CmplFileType varprod;
        CmplFileType matrix;


    private:
        vector<string> argList;
        vector<osolOpts> osolList;
        int numberOfOpts;

        vector<cmplDataElements> _cmplDataElementsList;

#ifdef SOLVERSERVICE
        vector<string> _displayConList;
        vector<string> _displayVarList;
#endif
        string _cmplFile;

        bool _checkOnlySyntax;

        bool _errorOut;
        string _errorFile;

        bool _protocolOut;
        string _protocolFile;

        bool _mpsOut;
        string _mpsFile;

        bool _silent;

        bool _freeMps;		/* -ff beim Aufruf, Matrix im Free-MPS-Format (0/1) */
        string _doubleNumberFormat;		/* 1.1.2 TS: format spezifier for double output */

        bool _isVarProdFollow;	/* -gf beim Aufruf, zusaetzliche Restriktionen   direkt nach Erzeugung in Matrix */
        bool _isNoReduct;		/* -gn beim Aufruf, jede Restriktion wird auch  in eine Matrixzeile umgesetzt */

        bool _constDefNoWarn;	/* -cd beim Aufruf: keine Warnung bei mehrfacher Definition einer Konstanten */
        bool _assignOldNoWarn;	/* -ca beim Aufruf: Alter Zuweisungsoperator '=' ohne Warnung erlaubt */
        int _numOpMode;		/* -ci<Modus> beim Aufruf, fuer Umgang mit ganzzahligen Werten in nummerischen Operationen
                                      Wert fuer Modus entsprechend NUM_OP_* */
#ifdef SOLVERSERVICE
        bool _solverMode;             /* after creating the osil file the model is to be solved*/
        bool _modelMode;

        int  _maxDecimals;
        bool _ignoreZeros;
        bool _dontRemoveTmpFiles;

        string _solverName;
        string _solverService;
        string _osolFile;
        bool _csvOut;
        bool _asciiOut;
        bool _stdioOut;
        bool _noOutput;

        string _nonCoinSolverOpts;

        string _objName;
        int _objSense; /* 0 - nothing , 1 - max , 2 - min */

        double _resultZeroPrecision;
#endif

        void fillArgList(int argc, char *argv[]);
        int parseArgs() throw (ArgError);

        void readCmplHeader() throw (ArgError);

#ifdef SOLVERSERVICE
        void writeSolverOpts() throw (ArgError);
#endif

        int dataFileNameListNumber(string str);
        int newDataElement(string fName);



    };

}

#endif // CMPLARGS_HH
