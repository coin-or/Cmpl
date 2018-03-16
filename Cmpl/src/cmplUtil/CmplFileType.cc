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


#include "CmplFileType.hh"


namespace cmplUtil {

    /* **************** open ************* */
    bool CmplFileType::open() {
        _coutBuf = cout.rdbuf();
        if (_outMode==FILE_OUT) {

            //QString tname = "c:\Users\Mike\Downloads\täst\täst.mps";

            _oOut = new ofstream( _fileName.c_str() , std::ofstream::out);
            //_oOut.open( _fileName.c_str() );
            if (!_oOut)
                return false;
            cout.rdbuf(_oOut->rdbuf());
        }
        if (_outMode==STRING_OUT) {
            cout.rdbuf(_sOut.rdbuf());
        }
        return true;
    } // end open



    /* **************** close ************* */
    void CmplFileType::close() {
       if (_outMode==FILE_OUT) {
            _oOut->close();
            cout.rdbuf(_coutBuf);
        }
        if (_outMode==STRING_OUT) {
           cout.rdbuf(_coutBuf);
           _stringOutput=_sOut.str();
        }
  }  // end close

}
