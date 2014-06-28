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


#ifndef CMPLFILETYPE_HH
#define CMPLFILETYPE_HH

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
using namespace std;


namespace cmplUtil {

#define STD_OUT 1
#define STRING_OUT 2
#define FILE_OUT 3

    /**
     * <code>CmplFileType</code> implements the handling of the
     * MPS, OSiL, OSoL, MPrL and statistics files
     *
     * @author  Mike Steglich
     */
    class CmplFileType
    {
    public:

        /**
         * constructor
         */
        CmplFileType()
            { setDefaults(); }

        /**
         * Returns true|false if the file is an active file
         *
         * @return  true|false
         */
        inline bool isOut()
            { return _isOut; }

        /**
         * Sets the file either as an active file or not
         *
         * @param o
         */
        inline void setOutputActive(bool o)
            {  _isOut=o; }

        /**
         * Returns the output mode of the file
         *
         * @return  STD_OUT 1 | STRING_OUT 2 |  FILE_OUT 3
         */
        inline int outMode()
            { return _outMode;  }

        /**
         * Sets the output mode of the file
         *
         * @param mode  STD_OUT 1 | STRING_OUT 2 |  FILE_OUT 3
         */
        inline void setOutMode(int mode)
            { _outMode=mode; }

        /**
         * Returns the file name
         *
         * @return  file name
         */
        inline string fileName()
            { return _fileName; }

        /**
         * Sets the file name
         *
         * @param  fname
         */
        inline void setFileName(string fname)
            { _fileName=fname;}

        /**
         * Returns the a string if STRING_OUT is chosen as output format
         *
         * @return  string output
         */
        inline string stringOutput()
            { return _stringOutput; }

        /**
         * sets a string for STRING_OUT
         *
         * @param  str
         */
        inline void setStringOutput(string str)
            { _stringOutput=str; }

        /**
         * sets file defaults
         */
        inline void setDefaults() {
            _isOut=false;
            _outMode=0;
            _fileName="";
            _stringOutput="";
        }

        /**
         * opens the file
         */
        bool open();

        /**
         * close the file
         */
        void close();


    private:
        bool _isOut;
        int _outMode;
        string _fileName;
        string _stringOutput;

        streambuf* _coutBuf;
        ofstream _oOut;
        ostringstream _sOut;


    };

}

#endif // CMPLFILETYPE_HH
