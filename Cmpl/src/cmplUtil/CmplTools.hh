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


#ifndef CMPLTOOLS_HH
#define CMPLTOOLS_HH

//#include "CmplCore.hh"

#include <string>
#include <iostream>
#include <sstream>
#include <algorithm>
using namespace std;

//#include <QString>
//#include <QRegExp>

namespace cmplUtil {

    /**
    * <code>CmplTools</code> - some internal and simple CMPL tools
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

        static bool stringToLong(const string str, long& t );


        /**
         * transforms a string to a double
         *
         * @param str
         * @param t
         * @return true|false
         */
        static bool stringToDouble(const string str, double& t );

        /**
         * Returns true|false if a string starts with a specific string
         *
         * @param str
         * @param start
         * @return true|false
         */
        static bool stringStartsWith(string str, string start);

        /**
         * Returns true|false if a string ends with a specific string
         *
         * @param str
         * @param start
         * @return true|false
         */
        static bool stringEndsWith(string str, string start);

        /**
         * Returns true|false if a string contains a specific string
         *
         * @param str
         * @param str1
         * @return true|false
         */
        static bool stringContains(string str, string str1);


        /**
         * Returns a left trimmed string
         *
         * @param str
         * @return left trimmed string
         */
        static string lTrim(string str);

        /**
         * Returns a left and right trimmed string
         *
         * @param str
         * @return left trimmed string
         */
        static string lrTrim(string str);

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

        /**
         * Returns the path of a problem
         *
         * @param cmplName
         * @return problem path
         */
        static string problemPath(string cmplName);

        /**
         * Returns the name of a model
         *
         * @param cmplName
         * @return model name w/o extension
         */
        static string modelName(string cmplName);


        /**
         * If a string is enclosed in double quotes then
         * the doubles quotes will be cleaned.
         */
        static void cleanDoubleQuotes(string &str);

        /**
         * If a string contains a colon
         * the colon will be cleaned.
         */
        static void cleanColon(string &str);

        /**
         * If a string contains a CMPL comment
         * the comment will be cleaned one time.
         */
        static void cleanComment(string &str);


        /**
         * Cleans a data element name from the beginning %
         * and the array brackets
         */
        static string cleanDataElementName(string str);

        /**
         * Cleans a string from a specific string
          */
        static string cleanStringFromString(string str, string str1);

        /**
         * Replaces a string by another in a string
         *
         * @param str  - original string
         * @param targ - string to be replaced
         * @param repl - by this string
         * @return changed string
         */
        static string replaceString( string str, string targ, string repl);

        /**
         * encodes a string into a XML string
         *
         * @param str  - original string
         */
        static string encodeXml(string str);


        /**
         * Cleans a string from whitespaces
          */
        static void cleanStringFromWhiteSpaces(string &str);
    };

}
#endif // CMPLTOOLS_HH
