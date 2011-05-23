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


#ifndef ECMPL_H
#define ECMPL_H

#include <QCoreApplication>
#include <QStringList>

#include <iostream>
using namespace std;

#include "cmplFrame.hh"
#include "ecmplio.hh"
#include "eCmplStatus.h"

/**
 * <code>eCmpl</code> implements the handling of cmplFrame object
 * for using whithin cmpl
 *
 * @author  Mike Steglich
 */

class eCmpl
{
public:
    /**
      * constructor
      */
    eCmpl();

    /**
     * <p>Transforms the current CMPL model into an OSil file and solves it using a local or remote solver.  </p>
     * <p>Synchronously communication between user program and solver </p>
     * <p>
     * @return  1 status OK; -1 CMPL failed - it exists a MPrL file with messages; -2   solver failed - it exists a >OSrL file with messages
     * </p>
     */
    int solve();

    /**
     * <p>Solves a specified model using a local or remote solver.  </p>
     * <p>Synchronously communication between user program and solver </p>
     * <p>
     * @return  1 status OK; -1 CMPL failed - it exists a MPrL file with messages; -2   solver failed - it exists a >OSrL file with messages
     * </p>
     */
    int runOSSolverServices();

    /**
     * Sets the cmpl model.
     *
     * <p>
     * @param cmplFileName  file name of the cmpl model
     * </p>
     */
    void setProblem(string cmplFileName);

    /**
     * <p>Sets the solver.</p>
     *
     * <p>
     * @param solver name of solver (CBC is standard)
     * </p>
     */
    void setSolver(string solver);

    /**
     * <p>Sets the location of a remote system.</p>
     *
     * <p>
     * @param serviceUrl   URL of the remote system
     * </p>
     */
    void setServiceLocation(string serviceUrl);

    /**
     * <p>Sets the the name of a OSoL File.</p>
     *
     * <p>
     * @param osolName   file name of the OSoL file
     * </p>
     */
    void setOsol(string osolName);


    /**
     * <p>Defines whether the solver output is to be written to a CSV file or not</p>
     *
     * <p>
     * @param x  True|False
     * </p>
     */
    void setSolverOutputCsv(bool x);


    /**
     * <p>Defines whether the solver output is to be written to stdOut or not</p>
     *
     * <p>
     * @param x  True|False
     * </p>
     */
    void setSolverOutputStd(bool x);

    /**
     * <p>Defines whether the solver output is to be written to Ascii or not</p>
     *
     * <p>
     * @param x  True|False
     * </p>
     */
    void setSolverOutputAscii(bool x);

    /**
     * <p>Defines whether cmpl has to be silent or not</p>
     *
     * <p>
     * @param x  True|False
     * </p>
     */
     void setSilent(bool x);



private:

     /**
      * <p>Sets the defaults / Used by the constructor</p>
      */
    void setDefaults();

    cmplFrame myProblem;
    eCmplIO myProblemIo;

    int eCmplStatus;

    QString cmplFile;
    bool isCmplFile;

    bool osrl;
    bool csv;
    bool ascii;
    bool std;

    bool isSolver;
    QString solverName;

    bool isServiceLocation;
    QString serviceLocationUrl;

    bool isOsol;
    QString osolFile;

    bool mprl;
    bool help;
    bool ver;
    bool silent;

};

#endif // ECMPL_H
