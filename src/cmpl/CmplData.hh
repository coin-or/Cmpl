/***********************************************************************
 *  This code is part of CMPL
 *
 *  Copyright (C) 2007, 2008, 2009, 2010, 2011, 2012
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

#ifndef CMPLDATA_HH
#define CMPLDATA_HH

#include "CmplArgs.hh"
#include "CmplCore.hh"
#include "Compiler.hh"
#include "IntFuncDef.hh"
#include "CmplTools.hh"

#include <iostream>
#include <fstream>
#include <string>


using namespace cmplUtil;

namespace cmpl {


    typedef struct
    {   string name;
        int symNr;
        vector<int> setSymNr;
        int fileNr;
        streampos filePos;
        int lineNr;
        bool isSet;
        int setRank;
        bool isIndices;
        bool isDefault;
        val_str defaultVal;
    } dataElement;

    class IntFuncInitCmplData;

    /**
    * The <code>CmplData</code> class provides the functionality of the cmplData file format
    *
    * @author  Mike Steglich
    */
    class CmplData
    {
    public:
        /**
         * constructor
         *
         * @param *a - pointer on a CmplCore object
         *
         */
        CmplData(cmplUtil::CmplArgs *a);

        /**
         * destructor
         */
        ~CmplData();

        /**
         * cmplDataCompiler
         * Reads the elements of a cmplDate file and defines the parameters and sets
         *
         * @param *comp - pointer on a Compiler object
         * @return true|false
         */
        bool cmplDataCompiler(Compiler *comp);


        /**
         * cmplDataInterpreter
         * Reads the values of parameters, sets or tupel-sets
         *
         * @param *intp - pointer on a Interpreter object
         * @return true|false
         */
        bool cmplDataInterpreter( IntFuncInitCmplData *intp) ;

        /**
         * isOpenFiles
         * returns true or false whether all cmplData files are opened (or not)
         *
         * @return true|false
         */
        inline bool isOpenFiles() {
            return _isOpenFiles;
        }

        /**
         * errorMsg
         * Handling of cmplData error meessages
         *
         * @return a string with the current error message
         */
        inline string errorMsg() {
            return _errorMsg;
        }

        /**
         * setErrorMsg
         * Definition of a cmplData error message that occurs during the compiler processes
         *
         * @param *comp - pointer on a Compiler object
         * @param fileName - name of the cmplData files that causes the error
         * @param lineNr  - number of the line of the cmplData files that causes the error
         * @param str - error message
         */
        void setErrorMsg(Compiler *comp, string fileName, int lineNr, string str);

        /**
         * setErrorMsg
         * Definition of a cmplData error message that occurs during the interpreter processes
         *
         * @param *intp - pointer on a Interpreter object
         * @param fileName - name of the cmplData files that causes the error
         * @param lineNr  - number of the line of the cmplData files that causes the error
         * @param str - error message
         */
        void setErrorMsg(IntFuncInitCmplData *intp, string fileName, int lineNr, string str);


    private:

        cmplUtil::CmplArgs *args;

        bool _isOpenFiles;
        string _errorMsg;

        bool openDataFiles();
        void closeDataFiles();

        vector<dataElement> _dataElements;

        bool activeDataElement(int file, string name);

        bool checkRankDef(int file, string name, int rank );
        int getSetRank(string str);

        bool checkArraySetDef(int file, string name, string arraySetDef);
        int getArrayDim(string str, vector<int> &tmpSym);

        void stringToValStr(string str, val_str &valS );

        bool foundAllDataElements(int file, string &errMsg);

        bool insertElementsForSyntaxCheck(Compiler *comp) ;

    };

}

#endif // CMPLDATA_HH
