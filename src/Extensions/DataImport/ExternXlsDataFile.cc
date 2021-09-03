
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


#include "ExternXlsDataFile.hh"


namespace cmpl
{
    /**
     * read all symbols from a data file
     * @param fh                file handling object
     * @param inStr             stream for reading data file
     * @param fn                name of data file
     * @param nm                namespace name / -1: no namespace
     * @param loc				location of data header line
     */
    void ExternXlsDataFile::readFileAllSymbols(ExternDataFileHandleOpt *fh, istream *inStr, string& fn, int nm, LocationInfo& loc)
    {
        string sep = StringStore::whiteSpaces() + "[<";
        string l, s;
        streampos p;
        size_t i;

        const char *inpstart = "@input";
        bool inpsect = false;

        while (getline(*inStr, l)) {
            if (!l.empty() && (l[0] == '%' || l[0] == '@')) {
                i = l.find_first_of(sep);

                if (l[0] == '@') {
                    inpsect = ((i == string::npos && l == inpstart) || (i == strlen(inpstart) && l.substr(0, i) == inpstart));
                }

                else if (inpsect) {
                    p = inStr->tellg() - (streamoff)(inStr->gcount());

                    s = (i != string::npos ? l.substr(1, i - 1) : l.substr(1));
                    s = StringStore::lTrim(s);

                    insertDataSymbol(fh, nm, s, fn, loc, (long)p);
                }
            }
        }
    }
}


