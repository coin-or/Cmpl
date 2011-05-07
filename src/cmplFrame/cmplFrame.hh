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




#ifndef cmplFrameH
#define cmplFrameH

#include <QString>
#include <QStringList>
#include <QThread>
#include <QProcess>
#include <QFile>
#include <QTextStream>
#include <QDir>
#include <QObject>
#include <QCoreApplication>
#include <QDateTime>

#include <string>
#include <iostream>
#include <stdlib.h>
#include <time.h>

using namespace std;

#include "cFException.hh"
#include "mprl.hh"
#include "cfresult.hh"
#include "cfStatus.h"

/**
 * The <code>cmplFrame</code> class defines the cmpl environment.
 * <code>cmplFrame</code> provides methods for the communication
 * between CMPL and OSSolverService and methods for analysing and
 * presenting optimization results.
 *
 * @author  Mike Steglich
 *
 */


class cmplFrame : public QThread
{
        Q_OBJECT
	
public:

        /* *********** methods ************************** */

       /**
        * constructor
        */
	cmplFrame();

        /**
         * overrides QThread::run and sets isCanceled true
         */
        virtual void run();

        /**
         * destructor
         */
        virtual ~cmplFrame();


        /**
         * Sets the cmpl model.
         *
         * <p>
         * @throws cFException  if CMPL file File not exists
         * @param cmplFile  file name of the cmpl model
         * </p>
         */
        void setModel(QString &cmplFile ) throw (cFException);

        /**
         * <p>Sets the the name of a OSoL File.</p>
         *
         * <p>
         * @throws cFException  if OSoL File not exists
         * @param osolName   file name of the OSoL file
         * </p>
         */
        void setOsol(QString &osolName) throw (cFException);

        /**
         * <p>Sets the favourite solver.</p>
         *
         * <p>
         * @param solverName name of solver (CBC is standard)
         * </p>
         */
        void setSolver(QString &solverName );

        /**
         * <p>Sets the location of a remote system.</p>
         *
         * <p>
         * @param serviceLocationName   URL of the remote system
         * </p>
         */
        void setServiceLocation(QString &serviceLocationName) ;

        /**
         * <p>Sets the location of a remote system.</p>
         * optional method
         *
         * <p>
         * @return  true options OK; false options wrong
         * @param cmplOpts   QString that contains additional CMPL options
         * </p>
         */
        bool setCmplOptions(QString cmplOpts);

        /**
         * <p>Transforms a CMPL model into an OSil file and solves it using a local or remote solver.  </p>
         * <p>Synchronously communication between user program and solver </p>
         * <p>
         * @return  1 status OK; -1 CMPL failed - it exists a MPrL file with messages; -2   solver failed - it exists a >OSrL file with messages
         * @throws cFException  defined cmplFrame errors
         * @throws Exception    other errors
         * </p>
         */
	int solve() throw (cFException);

        /**
         * <p>Solves a specified model using a local or remote solver.  </p>
         * <p>Synchronously communication between user program and solver </p>
         * <p>
         * @return  1 status OK; -1 CMPL failed - it exists a MPrL file with messages; -2   solver failed - it exists a >OSrL file with messages
         * @throws cFException  defined cmplFrame errors
         * @throws Exception    other errors
         * </p>
         */
        int runOSSolverservice() throw (cFException);

        /**
         * <p>Send a specified model to a remote solver.  </p>
         * <p>Asynchronously communication between user program and solver <p>
         * @return  1 status OK; -1 CMPL failed - it exists a MPrL file with messages; -2   solver failed - it exists a >OSrL file with messages
         * @throws cFException  defined cmplFrame errors
         * @throws Exception    other errors
         */
        int send() throw (cFException);

        /**
        * <p>Knocks on the door of the remote solver. "Hello Sir/Mam. Could I retrieve the results of my model?"</p>
        * <p>Asynchronously communication between user program and remote solver</p>
        * @return the jobStatus; 1 - idle; 2 - busy; 3 - noResponse
        * @throws cFException  defined cmplFrame errors
        */
        int knock() throw (cFException);

        /**
         * <p>Retrieves a solution from a remote solver.  </p>
         * <p>Synchronously communication between user program and solver <p>
         * @return  1 status OK; -1 CMPL failed - it exists a MPrL file with messages; -2   solver failed - it exists a >OSrL file with messages
         * @throws cFException  defined cmplFrame errors
         * @throws Exception    other errors
         */
        int retrieve() throw (cFException);

        /**
         * <p>Cancels CMPL and the local solver.  </p>
         */
        void cancel();

        /**
         * <p>Cancels a remote solver.  </p>
         */
        int cancelRemote();

        /**
         * <p>Returns a cmplFramework message  </p>
         * Necessary if a cmplFramework exception was happened
         */
        string getcFMessage();

        /**
         * <p>Returns CMPLs output   </p>
         */
        string readCmplOutput();

        /**
         * <p>Returns OSSolverServices output   </p>
         */
        string readSolverOutput();

        /**
          * returns the cmplFrame status
          */
        bool isStarted();

        /* *********** variables ************************** */

        mprl cmpLMsg;
        cfResult result;

signals:

        /**
         * <p>signal for CMPLs outtput is ready to read   </p>
         */
        void readyReadCmplOutput();

        /**
         * <p>signal for OSSolverServices outtput is ready to read  </p>
         */
        void readyReadSolverOutput();

private:

        /* *********** variables ************************** */
        bool isCanceled;
	int cmplStatus;   
	int solverStatus;
        bool cFStatus;

        QString cFProblem;
	QString cmplFileName;
	
        QString cmplOptions;

        QString cFMessage;

        QString solver;
        QString serviceLocation;
        QString osol;
	
        QString cmplFramePath;
        QString osSolverServicePath;

        QProcess cmplHandler;
        QProcess solverHandler;

        string cmplOutput;
        string solverOutput;
        bool externalSolverOutput;

        QString osolIdString;

         /* *********** methods ************************** */

        void setDefaults();

        /**
         * Checks whether a file exists or not.
         * @return true or false
         * @param fName file name of the file */
        bool checkFile(QString &fName);

        /**
         * Checks for .cmpl in the filename of the current CMPL model
         * @return filename of the current CMPL model w/o the extension (used as instance name)
         */
        QString checkDotCmpl() throw (cFException);

        /**
         * Checks for .cmpl in the filename of the current CMPL model
         * @param cFMsg the message
         */
        void setcFMessage(const QString &cFMsg);

        /**
         * writes the config file for the OSSolverservice
         * @return true or false cFMsg the message
         * @param mode - OSSolverservice - mode - ACTION_xxx - defined in cfStatus.h
         */
        bool writeConfigFile(int mode);

        /**
         * Status of a problem that was sent to a remote server
         * used by knock()
         * @return status - cfStatus.h
         */
        int getJobStatus();

        /**
         * Used for internal purposes -  saves the jobID in a OSoL file
         * used by send()
         */
        bool writeIdOsolFile();

        /**
         * Used for internal purposes
         * used by retrieve()
         */
        bool writeOSpLInputFile();

        /**
         * runs the CMPL binary
         * @param cmplMode - defined in cfStatus.h
         */
        int runCmpl(int cmplMode);

        /**
         * runs the OSSolverservice binary
         * @param actionMode - defined in cfStatus.h
         */
        int runSolver(int actionMode);

        /**
         * <code>setCmplOut</code> implements a connection to
         * the output stream of the CMPL binary
         */
        void setCmplOutput();

        /**
         * <code>setSolverOut</code> implements a a connection to
         * the output stream of the OSSolverService binary
         */
        void setSolverOutput();

        /**
         * Defines a jobID
         * used by send()
         */
        QString setJobId();

    private slots:
        /**
          * slot of CMPLs output
          */
        void writeCmplOutput();

        /**
          * slot of OSSolverService output
          */
        void writeSolverOutput();

        /**
          * load the cmpl's options
          */
        bool loadOptions();




};



#endif
