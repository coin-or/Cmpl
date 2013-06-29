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

#include "CmplIO.hh"


namespace cmplUtil {

    /* *********** constructor ******* */
    //CmplIO::CmplIO(cmplUtil::CmplArgs *arguments) {
   CmplIO::CmplIO( cmpl::CmplCore *m , CmplInstance *mi) {

        glvar=m;
        args=glvar->args;

        cmplInst = mi;

        setDefaults();
        setTerminationEnabled(false);
        cmplIOPath =  QCoreApplication::applicationDirPath()+"/";
        workingDir = QDir::currentPath();

        loadOptions();
    }//end constructor



    /* *********** run *************** */
    void CmplIO::run() {
       isCanceled=false;
    }//end run


    /* *********** destructor ********* */
    CmplIO::~CmplIO() {
    }// end destructor



    /* **************** solveModel ****************** */
    int CmplIO::solveModel() {

        if (args->solverName()=="glpk" || args->solverName()=="gurobi" || args->solverName()=="scip" || args->solverName()=="cplex") {
            return runMpsSolver();
        } else {
            return runOSSolverservice();
        }

    } //end solveModel

    /* **************** runMpsSolver *********************** */
    int CmplIO::runMpsSolver() throw (CmplException) {

        QString mpsFileName = QString::fromStdString(args->mps.fileName());
        if (!checkFile(mpsFileName))
            throw CmplException("MPS file <" + args->mps.fileName() + "> does not exist");


        if (args->solverName()=="scip") {
            if (!writeConfigFile(ACTION_SOLVE_SCIP))
                throw CmplException(getcFMessage());
        } else if (args->solverName()=="cplex") {
            if (!writeConfigFile(ACTION_SOLVE_CPLEX))
                throw CmplException(getcFMessage());
        }

        /* it is necessary to delete the old gsol file - if the solver failed and an old .gsol file is existing*/
        QFile f (QString::fromStdString(args->gsolFile()));
        f.remove();

        solverStatus=runSolver(ACTION_SOLVE);

        if (solverStatus==PROG_DONT_RUN) {
            string errorMessage;
            if (args->solverName()=="glpk")
                errorMessage= glpkPath.toStdString()+"glpsol";
            else if (args->solverName()=="gurobi")
                errorMessage= "Gurobi";
            else if (args->solverName()=="scip")
                errorMessage="SCIP ->"+scipFileName.toStdString();
            else if (args->solverName()=="cplex")
                 errorMessage="Cplex ->"+cplexFileName.toStdString();

            throw CmplException("can not connect the solver: "+errorMessage);
        }
        if (solverStatus==CMPLFRAME_CANCELED) throw CmplException("CMPL was canceled by user");

        if (solverStatus!=OK) {
            QString tmpMsg = " " + solver + " failed - exit code " + solverStatus;
            throw CmplException( tmpMsg.toStdString() );
        } else {

            result.setProblem(cFProblem, glvar, cmplInst);

            if (!result.getResult())
                throw CmplException(result.getCfOsMessage());

            if (result.nrOfSolutions()==0)
                return SOLVER_FAILED;
        }

        return SOLVER_OK;

    } //end runMpsSolver

    /* **************** runOSSolverservice ********** */
    int CmplIO::runOSSolverservice() throw (CmplException) {

        QString osilFileName = QString::fromStdString(args->osil.fileName());
        if (!checkFile(osilFileName))
            throw CmplException("OSil file <" + args->osil.fileName() + "> does not exist");

        if (!writeConfigFile(ACTION_SOLVE))
            throw CmplException(getcFMessage());

        /* it is necessary to delete the old osrl file - if the solver failed and an old OSrL file is existing*/
        QFile f (QString::fromStdString(args->osrlFile()));
        f.remove();

        solverStatus=runSolver(ACTION_SOLVE);
        if (solverStatus==PROG_DONT_RUN) throw CmplException("can not connect the solver: "+osSolverServicePath.toStdString());
        if (solverStatus==CMPLFRAME_CANCELED) throw CmplException("CmplIO was canceled by user");

        if (solverStatus!=OK) {
            throw CmplException(" OSSolverService failed - exit code " + solverStatus);
        } else {

            result.setProblem(cFProblem, glvar, cmplInst);

            if (!result.getResult())
                throw CmplException("Can not read the solution " + cFProblem.toStdString());

            if (result.nrOfSolutions()==0)
                return SOLVER_FAILED;
        }

        return SOLVER_OK;

    }//end runOSSolverservice

    /* *********** cancel ********* */
    void CmplIO::cancel() {
        if (solverHandler.isOpen()) solverHandler.kill();
        isCanceled=true;
        setDefaults();
    }// end terminate


    /* *********** setSolverOutput *********** */
    void CmplIO::setSolverOutput() {
        solverHandler.setProcessChannelMode(QProcess::MergedChannels);
        solverOutput="";
        solverHandler.setReadChannel(QProcess::StandardOutput);
        if (!isCanceled) {
            connect(&solverHandler, SIGNAL(readyReadStandardOutput()), this , SLOT( writeSolverOutput() ) );
        }
    }//end setSolverOutput


    /* *********** writeSolverOutput *********** */
    void CmplIO::writeSolverOutput() {
        QByteArray data = solverHandler.readAllStandardOutput();
        solverOutput = QString(data).toStdString();
        emit readyReadSolverOutput();
    }//end writeSolverOutput


    /* *********** readSolverOutput *********** */
    string CmplIO::readSolverOutput() {
        if (externalSolverOutput)
            return solverOutput;
        else return "";
    }//end readSolverOutput



    /* *********** runSolver *********** */
    int CmplIO::runSolver(int actionMode) {

        QString progString;
        QString solPool;

        if (actionMode==ACTION_SOLVE)
            externalSolverOutput=true;
        else
            externalSolverOutput=false;

        if (args->solverName()=="glpk") {

            if (glpkPath.isEmpty())
                progString = "\""+cmplIOPath;
            else
                if (glpkPath.startsWith("/"))
                    progString = "\""+glpkPath;
            else
                progString = "\""+cmplIOPath+glpkPath;

            progString+="\"glpsol --freemps \""+ QString::fromStdString(args->mps.fileName())+"\"  --"+ QString::fromStdString(cmplInst->objSense()) +" --write \""+QString::fromStdString(args->gsolFile())+"\"" + QString::fromStdString(args->nonCoinSolverOpts());

        } else  if (args->solverName()=="gurobi") {

            QString tmpWorkingDir=workingDir;
            QString tmpMps=QString::fromStdString(args->mps.fileName()).replace(" ","%");
            QString tmpGsol=QString::fromStdString(args->gsolFile()).replace(" ","%");

            if (args->solutionPool())
                solPool="1";
            else
                solPool="0";

#ifdef WIN
            progString +=  "\""+cmplIOPath+"\"gurobiPython.bat "+tmpWorkingDir.replace(" ","%")+" "+solPool+" "+ tmpMps+" " + tmpGsol+ " " +  QString::fromStdString(args->nonCoinSolverOpts());
#else
            progString +=  "\""+cmplIOPath+"\"gurobiPython "+cmplIOPath+" "+tmpWorkingDir.replace(" ","%")+" "+solPool+" "+ tmpMps+" " + tmpGsol+ " " +  QString::fromStdString(args->nonCoinSolverOpts());
#endif

        } else if (args->solverName()=="scip") {

            if (scipFileName.isEmpty())
               return PROG_DONT_RUN;
            else
                if (scipFileName.startsWith("/"))
                    progString += " \""+scipFileName;
            else
                progString += " \""+cmplIOPath+scipFileName;
            progString +=  "\" -b \""+ QString::fromStdString(args->cFconfFile())+"\"";

        }   else if(args->solverName()=="cplex") {

            if (cplexFileName.isEmpty())
                return PROG_DONT_RUN;
            progString = "\""+cplexFileName+"\"";

        } else {

            if (osSolverServicePath.isEmpty())
                progString = "\""+cmplIOPath;
            else
                if (osSolverServicePath.startsWith("/"))
                    progString = "\""+osSolverServicePath;
            else
                progString = "\""+cmplIOPath+osSolverServicePath;

            progString+="\"OSSolverService -config \""+QString::fromStdString(args->cFconfFile())+"\"";

        }

        setSolverOutput();

        if (!isCanceled) {
            if (solverHandler.isOpen()) solverHandler.kill();
            solverHandler.start( progString );

            if (!solverHandler.waitForStarted(15000))
                return PROG_DONT_RUN;
            else if(args->solverName()=="cplex")
                solverHandler.write(cplexConfig.c_str());

            solverHandler.waitForFinished(-1);
            return solverHandler.exitCode();
        } else
            return CMPLFRAME_CANCELED;


    }// end runSolver


    /* *********** writeConfigFile *********** */
    bool CmplIO::writeConfigFile(int mode) {

        QFile cfgFile (QString::fromStdString(args->cFconfFile()));
        QTextStream cfOpt (&cfgFile);

        QFile setFile (QString::fromStdString(args->scipSetFile()));

        if (!cfgFile.open(QIODevice::Truncate | QIODevice::Text | QIODevice::ReadWrite)) {
            setcFMessage("Internal Error - Cant read and write the config file!\n" );
            return false;
        }


        // Scip solve
        if (mode==ACTION_SOLVE_SCIP) {
            if (setFile.exists())
                cfOpt << "set load \"" << QString::fromStdString(args->scipSetFile()) << "\""<<endl;
            cfOpt << "read \"" << QString::fromStdString(args->mps.fileName()) << "\"" << endl;
            cfOpt << "optimize" << endl;
            cfOpt << "write solu \"" << QString::fromStdString(args->gsolFile()) << "\"" << endl;
            cfOpt << "quit" << endl;
        }

        // Cplex solve
        if (mode==ACTION_SOLVE_CPLEX) {
            cplexConfig= "read " + args->mps.fileName()+ "\n";
            cplexConfig+=args->nonCoinSolverOpts();
            if (args->solutionPool() && cmplInst->varIntIdxListSize()>0) {
                cplexConfig+= "mipopt\n";
                cplexConfig+="write  " + args->gsolFile() +" sol all \n" ;
            } else {
                cplexConfig+= "opt\n";
                cplexConfig+="write  " + args->gsolFile() +" sol \n" ;
            }
            cplexConfig+= " quit \n \n" ;
        }


        // solve
        if (mode==ACTION_SOLVE) {

            cfOpt << "-osil \"" << QString::fromStdString(args->osil.fileName()) << "\"" << endl;
            cfOpt << "-osrl \"" << QString::fromStdString(args->osrlFile()) << "\"" << endl;

            if (solver!=NOT_DEFINED)
                cfOpt << "-solver " << solver << endl;
            if (serviceLocation!=NOT_DEFINED)
                cfOpt << "-serviceLocation " << serviceLocation << endl;
            if (osol!=NOT_DEFINED )
                if (checkFile(osol))
                    cfOpt << "-osol \"" << QString::fromStdString(args->osolFile()) << "\"" << endl;
            cfOpt << "-serviceMethod solve" << endl;
        }


        cfgFile.close();
        return true;
    }//end writeConfigFile



    /* *********** setcFMessage *********** */
    void CmplIO::setcFMessage(const QString &cFMsg) {
        cFMessage=cFMsg;
    }//end setcFMessage


    /* *********** getcFMessage *********** */
    string CmplIO::getcFMessage() {
        return cFMessage.toStdString();
    }//end getcFMessage


    /* *********** setDefaults ********* */
    void CmplIO::setDefaults() throw (CmplException){

        isCanceled=false;
        cmplStatus=0;
        solverStatus=0;

        cFProblem="";
        cmplFileName="";

        cFMessage="";

        cmplOptions="";

        solver="";
        serviceLocation="";


        cmplStatus=0;

        cmplIOPath="";
        workingDir="";

        osSolverServicePath="";
        glpkPath="";
        scipFileName="";
       // soPlexFileName="";
        cplexFileName="";
        cplexConfig="";

        solver=NOT_DEFINED;
        serviceLocation=NOT_DEFINED;

        if (args->osolFile().empty())
            osol=NOT_DEFINED;
        else
            osol=QString::fromStdString(args->osolFile());

        cmplOutput="";
        solverOutput="";
        externalSolverOutput=false;

        cmplFileName=QString::fromStdString(args->cmplFile());
        if (!checkFile(cmplFileName))
            throw CmplException("CMPL file  <"+cmplFileName.toStdString()+"> does not exist!");
        cFProblem=checkDotCmpl();

        if (!args->solverName().empty())
            solver=QString::fromStdString(args->solverName());

        if(!args->solverService().empty())
            serviceLocation=QString::fromStdString(args->solverService());

    }//end setDefaults


    /* *********** checkFile *********** */
    bool CmplIO::checkFile(QString &fName) {
        if ( fName.trimmed().isEmpty() ) return false;
        QFile f (fName);
        return f.exists();
    } //end checkFile



    /* *********** setModel *********** */
    QString CmplIO::checkDotCmpl() throw (CmplException) {
        int dotGen = cmplFileName.toLower().indexOf(".cmpl");
        if (dotGen==-1) throw CmplException("CMPL file name does not include \".cmpl\"");
        return cmplFileName.left(dotGen);
    }//end checkDotCmpl



    /* ************** loadOptions ***************** */
    void CmplIO::loadOptions() throw (CmplException)  {

        QString optFName=cmplIOPath + "cmpl.opt";
        QFile cFOptFile (optFName);

        if (!cFOptFile.open(QIODevice::ReadOnly | QIODevice::Text))
            throw CmplException("Internal Error - Cant read the option file!\n" );

        while (!cFOptFile.atEnd()) {
            if (cFOptFile.error()>0)
                throw CmplException("Internal Error - Cant read the option file! >> "+ cFOptFile.error() );

            QString line = cFOptFile.readLine();
            if ( line.toLower().contains("ossolverservicepath") )
                osSolverServicePath = line.mid( line.indexOf("=")+1, line.length()).trimmed();
            if ( line.toLower().contains("glpkpath") )
                glpkPath = line.mid( line.indexOf("=")+1, line.length()).trimmed();
            if ( line.toLower().contains("scipfilename") )
                scipFileName = line.mid( line.indexOf("=")+1, line.length()).trimmed();
         /*   if ( line.toLower().contains("soplexfilename") )
                soPlexFileName = line.mid( line.indexOf("=")+1, line.length()).trimmed();
        */
            if ( line.toLower().contains("cplexfilename") )
                cplexFileName = line.mid( line.indexOf("=")+1, line.length()).trimmed();
        }
        cFOptFile.close();

    } //end loadOptions


}
