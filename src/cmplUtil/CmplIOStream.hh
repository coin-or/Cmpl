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


#ifndef CMPLIOSTREAM_H
#define CMPLIOSTREAM_H


#include <string>
#include <iostream>
#include <QObject>

#include "CmplIO.hh"

using namespace std;

namespace cmplUtil {

    /**
    * The <code>eCmplIO</code> implements the eCMPL IO handling.
    *
    * @author  Mike Steglich
    */


    class CmplIOStream : public QObject
    {
        Q_OBJECT
    public:
        /**
        * constructor
        */
        CmplIOStream();

        /**
        * <code>setCmplOut</code> implements a connection to
        * the output stream of the CMPL binary
        * <p>
        * @param mcf  pointer on a cmplFrame object
        * </p>
        */
        void setCmplOut(CmplIO *mcf);

        /**
        * <code>setSolverOut</code> implements a a connection to
        * the output stream of the OSSolverService binary
        * <p>
        * @param mcf  pointer on a cmplFrame object
        * </p>
        */
        void setSolverOut(CmplIO *mcf);

    private:

        /**
         * pointer on a cmplFrame object
         */
        CmplIO *cf;


    private slots:
        /**
         * slot of CMPLs output
         */
        void writeCmplOutput();

        /**
         * slot of OSSolverService output
         */
        void writeSolverOutput();


    };

}

#endif // CmplIOStream_H
