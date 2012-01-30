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


#include "CmplTools.hh"

namespace cmplUtil {


    /* ********* CmplTools ********* */
    CmplTools::CmplTools()
    {
    } // end


    /* ********* stringToInt ********* */
    bool CmplTools::stringToInt(const string str, int& t ) {
        stringstream ss(str);
        return !( (ss >> t).fail() );
    } // end stringToInt


    /* ********* stringToInt ********* */
    bool CmplTools::stringToDouble(const string str, double& t ) {
        stringstream ss(str);
        return !( (ss >> t).fail() );
    } // end stringToDouble


    /* ********* stringStartsWith ********* */
    bool CmplTools::stringStartsWith(string str, string start) {
       return  ( str.substr(0, start.length() ) == start );
    } // end stringStartsWith

    /* ********* lTrim ********* */
    string CmplTools::lTrim(string str) {
        int pos = str.find_first_not_of(' ');
        if (pos !=-1)
            return str.substr(pos,str.size());
        else
            return str;
    } // end lTrim


    /* ********* lTrim ********* */
    string CmplTools::lowerCase(string str) {
        string tmpString=str;
        transform(tmpString.begin(), tmpString.end(), tmpString.begin(), ::tolower);
        return tmpString;
    } // end lTrim



    /* ********* problemName ********* */
    string CmplTools::problemName(string cmplName){
        size_t t = cmplName.find(".cmpl");
        if (t!=string::npos)
            return cmplName.substr(0,t);
        else
            return cmplName;
    } // end problemName

}
