/***********************************************************************
 *  This code is part of cmplFrame
 *
 *  Copyright (C) 2010 Mike Steglich - Technical University
 *  of Applied Sciences  Wildau, Germany
 *  and Thomas Schleiff - Halle(Saale), Germany
 *
 *  cmplFrame and CMPL are projects of the Technical University of
 *  Applied Sciences Wildau and the Institute for Operations Research
 *  and Business Management at the Martin Luther University
 *  Halle-Wittenberg.
 *  Please visit the project homepage <www.coliop.org>
 *
 *  cmplFrame is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  cmplFrame is distributed in the hope that it will be useful, but WITHOUT
 *  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 *  or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public
 *  License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, see <http://www.gnu.org/licenses/>.
 *
 ***********************************************************************/


#include "ecmplio.hh"

/* ********* eCmplIO ********* */
eCmplIO::eCmplIO()
{
    isSilent=true;
}//end eCmplIO

/* ********* setCmplOut ********* */
void eCmplIO::setCmplOut(cmplFrame *mcf) {
    cf=mcf;
    isSilent=false;
    connect(cf, SIGNAL(readyReadCmplOutput()), this , SLOT( writeCmplOutput() ) );
}//end setCmplOut

/* ********* setSolverOut ********* */
void eCmplIO::setSolverOut(cmplFrame *mcf) {
    cf=mcf;
    isSilent=false;
    connect(cf, SIGNAL(readyReadSolverOutput()), this , SLOT( writeSolverOutput() ) );
}//end setSolverOut

/* ********* writeCmplOutput ********* */
void eCmplIO::writeCmplOutput() {
    if (!isSilent)
        cout << cf->readCmplOutput() ;
}//end writeCmplOutput

/* ********* writeSolverOutput ********* */
void eCmplIO::writeSolverOutput() {
    if (!isSilent)
        cout << cf->readSolverOutput() ;
}//end writeSolverOutput

