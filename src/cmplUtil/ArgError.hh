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

#ifndef ARGERROR_HH
#define ARGERROR_HH

#include <string>
#include <iostream>
using namespace std;

namespace cmplUtil {


    /**
     * <code>ArgError</code> implements the handling of the
     * error that have been caused by command line arguments or
     * by arguments in a CMPL header
     *
     * @author  Mike Steglich
     */
    class ArgError
    {

    public:
            /**
             * constructor
             *
             * @param arg argument that causes the the error
             * @param msg text of the error message
             */
            ArgError(string arg, string msg) {
                _cmplHeader=false;
                _message=msg;
                _eArg = arg;
            }

            /**
             * constructor
             *
             * @param arg argument that causes the the error
             * @param msg text of the error message
             */
            ArgError(bool c , string arg, string msg) {
                _cmplHeader=c;
                _message=msg;
                _eArg = arg;
            }


            /**
             * constructor
             *
             * @param msg text of the error message
             */
            ArgError(string msg) {
                _cmplHeader=false;
                _message=msg;
                _eArg = "";
            }

            /**
             * <p>Returns a  message  </p>
             * Necessary if an ArgError exception was happened
             */
            void getMessage();

            /**
             * shows the CMPL usage
             *
             */
            static void usage( int mode);   // 0 cout, 1 cerr

            /**
             * destructor
             */
            virtual  ~ArgError();

    private:
            /**
              * string of the current cmplFrameMessage
              */
            string _message;

            /**
              * string of the current argument that causes an error
              */
            string _eArg;

            bool _cmplHeader;

    };


}

#endif // ARGERROR_HH
