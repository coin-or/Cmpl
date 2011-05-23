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

#ifndef ECMPLIO_H
#define ECMPLIO_H


#include <string>
#include <iostream>
#include <QObject>

#include "cmplFrame.hh"

using namespace std;

/**
 * The <code>eCmplIO</code> implements the eCMPL IO handling.
 *
 * @author  Mike Steglich
 */


class eCmplIO : public QObject
{
      Q_OBJECT
public:
    /**
     * constructor
     */
    eCmplIO();

    /**
     * <code>setCmplOut</code> implements a connection to
     * the output stream of the CMPL binary
     * <p>
     * @param mcf  pointer on a cmplFrame object
     * </p>
     */
    void setCmplOut(cmplFrame *mcf);

    /**
     * <code>setSolverOut</code> implements a a connection to
     * the output stream of the OSSolverService binary
     * <p>
     * @param mcf  pointer on a cmplFrame object
     * </p>
     */
    void setSolverOut(cmplFrame *mcf);

private:

    /**
      * pointer on a cmplFrame object
      */
    cmplFrame *cf;

    /**
     * silent mode yes|no
     */
    bool isSilent;

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


#endif // ECMPLIO_H
