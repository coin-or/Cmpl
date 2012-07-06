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

    /* ********* stringToLong ********* */
    bool CmplTools::stringToLong(const string str, long& t ) {
        stringstream ss(str);
        return !( (ss >> t).fail() );
    } // end stringToLong



    /* ********* stringToInt ********* */
    bool CmplTools::stringToDouble(const string str, double& t ) {
        stringstream ss(str);
        return !( (ss >> t).fail() );
    } // end stringToDouble


    /* ********* stringStartsWith ********* */
    bool CmplTools::stringStartsWith(string str, string start) {
       return  ( str.substr(0, start.length() ) == start );
    } // end stringStartsWith

    /* ********* stringEndsWith ********* */
    bool CmplTools::stringEndsWith(string str, string end) {
        if (str.length()<end.length())
            return false;
        else
           return  ( str.substr(str.length()-end.length() ) == end );
    } // end stringEndsWith


    /* ********* stringContains ********* */
    bool CmplTools::stringContains(string str, string str1) {
       return  ( str.find_first_of(str1) != string::npos );
    } // end stringContains


    /* ********* lTrim ********* */
    string CmplTools::lTrim(string str) {
        string whitespaces (" \t\f\v\n\r");

        int pos = str.find_first_not_of(whitespaces);

        if (pos !=-1)
            return str.substr(pos);
            //return str.substr(pos,str.size());
        else
            return "";
    } // end lTrim

    /* ********* lTrim ********* */
    string CmplTools::lrTrim(string str) {
        string whitespaces (" \t\f\v\n\r");

        int pos1 = str.find_first_not_of(whitespaces);
        int pos2 = str.find_last_not_of(whitespaces);

        if (pos1 !=-1)
            return str.substr(pos1,pos2-pos1+1);
        else
            return "";
    } // end lrTrim

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


    /* ********* problemPath ********* */
    string CmplTools::problemPath(string cmplName){
        size_t t;
        string str = replaceString(cmplName,"\\","/");

        t = str.find_last_of("/");
        if (t!=string::npos)
            return str.substr(0,t+1);
        else
            return "";
    } // end problemPath

    /* ********* cleanDoubleQuotes ********* */
    void CmplTools::cleanDoubleQuotes(string &str)
    {   size_t firstQuote, lastQuote;
        if ((firstQuote=str.find_first_of("\""))!=string::npos)
           if ((lastQuote=str.find_last_of("\""))!=string::npos)
              str=str.substr(firstQuote+1,lastQuote-firstQuote-1);
              // str=str.substr(firstQuote+1,lastQuote-1);
    }//end cleanDoubleQuotes

    /* ********* cleanColon ********* */
    void CmplTools::cleanColon(string &str)
    {   size_t colonPos;
        if ((colonPos=str.find_first_of(":"))!=string::npos)
            str.erase(colonPos,colonPos+1);
    }//end cleanColon

    /* ********* cleanComment ********* */
    void CmplTools::cleanComment(string &str)
    {   size_t colonPos;
        if ((colonPos=str.find_first_of("/*"))!=string::npos)
            str.erase(colonPos);
    }//end cleanComment obsolet

    /* ********* cleanDataElementName ********* */
    string CmplTools::cleanDataElementName(string str)
    {
        size_t arrayBegin;
        string name;

        if ((arrayBegin=str.find_first_of("["))!=string::npos)
            name=str.substr(0,arrayBegin-1);
        else
            name=str.substr(0,str.length());

        return name;

    }//end cleanDataElementName obsolet

    /* ********* cleanStringFromString ********* */
    string CmplTools::cleanStringFromString(string str, string str1)
    {
        size_t pos;
        string tmpString=str;
        pos = tmpString.find(str1);

        while (pos!=string::npos) {
            tmpString.erase(pos,str1.length());
            pos = tmpString.find(str1);
        }

        return tmpString;

    }//end cleanStringFromString obsolet?

    /* ********* replaceString ********* */
    string CmplTools::replaceString( string str, string targ, string repl)
    {
        if ( targ.empty() || repl.empty() )
              return str;

        size_t idx = 0;
        while (true) {
            idx = str.find( targ, idx);
            if (idx == string::npos)
                break;
            str.replace( idx, targ.length(), repl);
            idx += repl.length();
        }
        return str;
    } // end replaceString


    /* ********* cleanStringFromWhiteSpaces ********* */
    void CmplTools::cleanStringFromWhiteSpaces(string &str)
    {
        string whitespaces (" \t\f\v\n\r");
        size_t pos;
        string tmpString=str;
        pos = tmpString.find_first_of(whitespaces);

        while (pos!=string::npos) {
            tmpString.erase(pos,1);
            pos = tmpString.find_first_of(whitespaces);
        }

        str=tmpString;

    }//end cleanStringFromWhiteSpaces




}
