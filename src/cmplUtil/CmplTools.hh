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


#ifndef CMPLTOOLS_HH
#define CMPLTOOLS_HH

#include <string>
#include <iostream>
#include <sstream>
#include <algorithm>
using namespace std;

namespace cmplUtil {

    /**
    * The <code>CmplTools</code> some internal and simple CMPL tools
    *
    * @author  Mike Steglich
    */
    class CmplTools
    {
    public:
        /**
         * constructor
         */
        CmplTools();

        /**
         * transforms a string to a integer
         *
         * @param str
         * @param t
         * @return true|false
         */
        static bool stringToInt(const string str, int& t );


        /**
         * transforms a string to a double
         *
         * @param str
         * @param t
         * @return true|false
         */
        static bool stringToDouble(const string str, double& t );

        /**
         * Returns true|false if a string starts with a specific character
         *
         * @param str
         * @param start
         * @return true|false
         */
        static bool stringStartsWith(string str, string start);

        /**
         * Returns a left trimmed string
         *
         * @param str
         * @return left trimmed string
         */
        static string lTrim(string str);

        /**
         * Transforms a string into lowercases
         *
         * @param str
         * @return lowercasestring
         */
        static string lowerCase(string str);


        /**
         * Returns a problem name w/o the extension .cmpl
         *
         * @param cmplName
         * @return problem name
         */
        static string problemName(string cmplName);
    };

}
#endif // CMPLTOOLS_HH
