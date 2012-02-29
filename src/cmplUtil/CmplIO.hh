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


#ifndef CmplIOH
#define CmplIOH

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

#include "CmplException.hh"
#include "mprl.hh"
#include "cfresult.hh"
#include "CmplArgs.hh"
#include "CmplCore.hh"
#include "FileIO.hh"
#include "CmplInstance.hh"


namespace cmplUtil {

    /**
    * The <code>CmplIO</code> class defines the cmpl environment.
    * <code>CmplIO</code> provides methods for the communication
    * between CMPL and OSSolverService and methods for analysing and
    * presenting optimization results.
    *
    * @author  Mike Steglich
    *
    */

    #define SOLVE_OK 1
    #define CMPL_FAILED -1
    #define SOLVER_FAILED -2

    #define SEND_OK 1

    #define KNOCK_FAILED -1
    #define IDLE 1
    #define BUSY 2
    #define NORESPONSE 3
    #define ERROR 4

    #define RETRIEVE_OK 1
    #define CANCEL_OK 1

    #define OK 0

    #define CMPLFRAME_CANCELED -99
    #define PROG_DONT_RUN -100

    #define ACTION_SOLVE 1
    #define ACTION_SEND 2
    #define ACTION_KNOCK 3
    #define ACTION_RETRIEVE 4
    #define ACTION_CANCEL 5
    #define ACTION_SOLVE_SCIP 6

    #define CMPL_MODEL 1
    #define CMPL_SOLVE 2

    #define NOT_DEFINED "#nn#"

    #define ASCII   2
    #define STDIO   1



    class CmplIO : public QThread
    {
        Q_OBJECT
	
    public:

        /* *********** methods ************************** */

        /**
        * constructor
        */
        //CmplIO(cmplUtil::CmplArgs *arguments);
        CmplIO(cmpl::CmplCore *m, CmplInstance *mi);

        /**
         * overrides QThread::run and sets isCanceled true
         */
        virtual void run();

        /**
         * destructor
         */
        virtual ~CmplIO();

        /**
         * <p>Solves a generated model instance by using a local solver.  </p>
         * <p>
         * @return  1 status OK; -1 CMPL failed - it exists a MPrL file with messages; -2   solver failed - it exists a >OSrL file with messages
         * </p>
         */
        int solveModel();

        /**
         * <p>Transforms a CMPL model into an OSil file and solves it using a local or remote solver.  </p>
         * <p>Synchronously communication between user program and solver </p>
         * <p>
         * @return  1 status OK; -1 CMPL failed - it exists a MPrL file with messages; -2   solver failed - it exists a >OSrL file with messages
         * @throws CmplException  defined CmplIO errors
         * @throws Exception    other errors
         * </p>
         */
	int solve() throw (CmplException);

        /**
         * <p>Solves a specified model using a local or remote solver.  </p>
         * <p>Synchronously communication between user program and solver </p>
         * <p>
         * @return  1 status OK; -1 CMPL failed - it exists a MPrL file with messages; -2   solver failed - it exists a >OSrL file with messages
         * @throws CmplException  defined CmplIO errors
         * @throws Exception    other errors
         * </p>
         */
        int runOSSolverservice() throw (CmplException);

        /**
         * <p>Solves a specified model using Glpk.  </p>
         * <p>Synchronously communication between user program and solver </p>
         * <p>
         * @return  1 status OK; -1 CMPL failed - it exists a MPrL file with messages; -2   solver failed
         * @throws CmplException  defined CmplIO errors
         * @throws Exception    other errors
         * </p>
         */
        int runGlpkOrGurobiOrScip() throw (CmplException);

        /**
         * <p>Send a specified model to a remote solver.  </p>
         * <p>Asynchronously communication between user program and solver <p>
         * @return  1 status OK; -1 CMPL failed - it exists a MPrL file with messages; -2   solver failed - it exists a >OSrL file with messages
         * @throws CmplException  defined CmplIO errors
         * @throws Exception    other errors
         */

        inline void standardReport(int outType){
            result.standardReport(outType);
        }

        inline void writeCsv() {
            result.writeCsv();
        }

        inline void getSolverMessages() {
            result.getSolverMessages();
        }

        int send() throw (CmplException);

        /**
        * <p>Knocks on the door of the remote solver. "Hello Sir/Mam. Could I retrieve the results of my model?"</p>
        * <p>Asynchronously communication between user program and remote solver</p>
        * @return the jobStatus; 1 - idle; 2 - busy; 3 - noResponse
        * @throws CmplException  defined CmplIO errors
        */
        int knock() throw (CmplException);

        /**
         * <p>Retrieves a solution from a remote solver.  </p>
         * <p>Synchronously communication between user program and solver <p>
         * @return  1 status OK; -1 CMPL failed - it exists a MPrL file with messages; -2   solver failed - it exists a >OSrL file with messages
         * @throws CmplException  defined CmplIO errors
         * @throws Exception    other errors
         */
        int retrieve() throw (CmplException);

        /**
         * <p>Cancels CMPL and the local solver.  </p>
         */
        void cancel();

        /**
         * <p>Cancels a remote solver.  </p>
         */
        int cancelRemote();

        /**
         * <p>Returns a CmplIO message  </p>
         * Necessary if a CmplIO exception was happened
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

        /* *********** variables ************************** */

        mprl cmpLMsg;


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

        cmplUtil::CmplArgs *args;
        cmpl::CmplCore *glvar;
        cfResult result;

        CmplInstance *cmplInst;

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
	
        QString cmplIOPath;
        QString osSolverServicePath;
        QString glpkPath;
        QString gurobiCmplScriptPath;
        QString pythonCommand;
        QString scipFileName;
        //QString soPlexFileName;

        QProcess cmplHandler;
        QProcess solverHandler;

        string cmplOutput;
        string solverOutput;
        bool externalSolverOutput;

        QString osolIdString;

        /* *********** methods ************************** */

        void setDefaults() throw (CmplException);

        /**
          * load the cmpl's options
          */
        void loadOptions() throw (CmplException);

        /**
         * Checks whether a file exists or not.
         * @return true or false
         * @param fName file name of the file */
        bool checkFile(QString &fName);

        /**
         * Checks for .cmpl in the filename of the current CMPL model
         * @return filename of the current CMPL model w/o the extension (used as instance name)
         */
        QString checkDotCmpl() throw (CmplException);

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








    };

}

#endif
