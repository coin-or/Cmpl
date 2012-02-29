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
        loadOptions();
    }//end constructor



    /* *********** run *************** */
    void CmplIO::run() {
        isCanceled=false;
    }//end run


    /* *********** destructor ********* */
    CmplIO::~CmplIO() {
    }// end destructor


     /* ************ solve ************** */
    int CmplIO::solve() throw (CmplException) {

        if (!writeConfigFile(ACTION_SOLVE))
            throw CmplException(getcFMessage());

        /* it is necessary to delete the old osrl file - if the solver failed and an old OSrL file is existing*/
        QFile f (cFProblem + ".osrl");
        f.remove();

        /* it is necessary to delete the old mprl file - if the cmpl failed and an old mprl file is existing*/
        QFile c (cFProblem + ".mprl");
        c.remove();

        cmplStatus=runCmpl(CMPL_SOLVE);
        if (cmplStatus==PROG_DONT_RUN) throw CmplException("can not connect cmpl");
        if (cmplStatus==CMPLFRAME_CANCELED) throw CmplException("CmplIO was canceled by user");

        if (cmplStatus!=OK) {

            if(cmplStatus<6) {

                if (!cmpLMsg.readMPrL())
                    throw CmplException("CMPL failed - The result file can not be read");
                else
                    return CMPL_FAILED;
            } else
                throw CmplException("OSSolverService failed - exit code " );

        } else {

            result.setProblem(cFProblem, glvar, cmplInst);

            if (!result.getResult())
                throw CmplException("Can not read the solution " + cFProblem.toStdString());

            if (result.nrOfSolutions()==0)
                return SOLVER_FAILED;

        }

        return SOLVE_OK;

    }// end solve

    /* **************** solveModel ****************** */
    int CmplIO::solveModel() {

        if (args->solverName()=="glpk" || args->solverName()=="gurobi" || args->solverName()=="scip") {
            runGlpkOrGurobiOrScip();
        } else {
            runOSSolverservice();
        }

    } //end solveModel

    /* **************** runGlpk *********************** */
    int CmplIO::runGlpkOrGurobiOrScip() throw (CmplException) {

        QString mpsFileName = cFProblem + ".mps";
        if (!checkFile(mpsFileName))
            throw CmplException("MPS file <" + mpsFileName.toStdString() + "> does not exist");

        if (!writeConfigFile(ACTION_SOLVE_SCIP))
            throw CmplException(getcFMessage());

        /* it is necessary to delete the old gsol file - if the solver failed and an old .gsol file is existing*/
        QFile f (cFProblem + ".gsol");
        f.remove();

        solverStatus=runSolver(ACTION_SOLVE);


        if (solverStatus==PROG_DONT_RUN) {
            string errorMessage;
            if (args->solverName()=="glpk")
                errorMessage= glpkPath.toStdString()+"glpsol";
            else if (args->solverName()=="gurobi")
                errorMessage= "Gurobi";
            else
                errorMessage="SCIP ->"+scipFileName.toStdString();

            throw CmplException("can not connect the solver: "+errorMessage);
        }
        if (solverStatus==CMPLFRAME_CANCELED) throw CmplException("CmplIO was canceled by user");

        if (solverStatus!=OK) {
            QString tmpMsg = " " + solver + " failed - exit code " + solverStatus;
            throw CmplException( tmpMsg.toStdString() );
        } else {

            result.setProblem(cFProblem, glvar, cmplInst);

            if (!result.getResult())
                throw CmplException("Can not read the solution " + cFProblem.toStdString());

            if (result.nrOfSolutions()==0)
                return SOLVER_FAILED;
        }

        return SOLVE_OK;

    } //end runGlpk

    /* **************** runOSSolverservice ********** */
    int CmplIO::runOSSolverservice() throw (CmplException) {

        QString osilFileName = cFProblem + ".osil";
        if (!checkFile(osilFileName))
            throw CmplException("OSil file <" + osilFileName.toStdString() + "> does not exist");

        if (!writeConfigFile(ACTION_SOLVE))
            throw CmplException(getcFMessage());

        /* it is necessary to delete the old osrl file - if the solver failed and an old OSrL file is existing*/
        QFile f (cFProblem + ".osrl");
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

        return SOLVE_OK;

    }//end runOSSolverservice



    /* ********** send *********************** */
    int CmplIO::send() throw (CmplException) {

        /* it is necessary to delete the old mprl file - if the cmpl failed and an old mprl file is existing*/
        QFile f (cFProblem + ".mprl");
        f.remove();

        cmplStatus=runCmpl(CMPL_MODEL);
        if (cmplStatus==CMPLFRAME_CANCELED) throw CmplException("CmplIO was canceled by user");

        if (cmplStatus!=OK) {

            if (!cmpLMsg.readMPrL())
                throw CmplException("CMPL failed - The result file can not be read");
            else
                return CMPL_FAILED;

        } else {

            if (!writeIdOsolFile())
                throw CmplException(getcFMessage());

            if (!writeConfigFile(ACTION_SEND))
                throw CmplException(getcFMessage());

            solverStatus=runSolver(ACTION_SEND);
            if (solverStatus==PROG_DONT_RUN) throw CmplException("can not connect the solver");
            if (solverStatus==CMPLFRAME_CANCELED) throw CmplException("CmplIO was canceled by user");

            if (solverStatus!=OK)
                throw CmplException("OSSolverService failed - exit code " + solverStatus);

        }
        return SEND_OK;
    }//end send


    /* *************** knock ************** */
    int CmplIO::knock() throw (CmplException) {
        int jobStatus=0;

        if (!writeOSpLInputFile())
            throw CmplException(getcFMessage());

        if (!writeConfigFile(ACTION_KNOCK))
            throw CmplException(getcFMessage());

        solverStatus=runSolver(ACTION_KNOCK);
        if (solverStatus==PROG_DONT_RUN) throw CmplException("can not connect the solver");
        if (solverStatus==CMPLFRAME_CANCELED) throw CmplException("CmplIO was canceled by user");

        if (solverStatus!=OK) {
            throw CmplException("OSSolverService failed - exit code " + solverStatus);
        } else {
            jobStatus=getJobStatus();
            if (jobStatus==KNOCK_FAILED) throw CmplException(getcFMessage());
        }
        return jobStatus;
    }//end knock


    /* ****************** retrieve ***************** */
    int CmplIO::retrieve() throw (CmplException) {

        if (!writeConfigFile(ACTION_RETRIEVE))
            throw CmplException(getcFMessage());

        /* it is necessary to delete the old osrl file - if the solver failed and an old OSrL file is existing*/
        QFile f (cFProblem + ".osrl");
        f.remove();

        solverStatus=runSolver(ACTION_RETRIEVE);
        if (solverStatus==PROG_DONT_RUN) throw CmplException("can not connect the solver");
        if (solverStatus==CMPLFRAME_CANCELED) throw CmplException("CmplIO was canceled by user");

        if (solverStatus!=OK) {
            throw CmplException("OSSolverService failed - exit code " + solverStatus);
        } else {

            result.setProblem(cFProblem, glvar, cmplInst);

            if (!result.getResult())
                throw CmplException("Can not read the solution " + cFProblem.toStdString());

            if (result.nrOfSolutions()==0)
                return SOLVER_FAILED;
        }
        return RETRIEVE_OK;
    }//end solve


    /* *********** cancel ********* */
    void CmplIO::cancel() {
        if (cmplHandler.isOpen()) cmplHandler.kill();
        if (solverHandler.isOpen()) solverHandler.kill();
        isCanceled=true;
        setDefaults();
    }// end terminate


    /* *********** cancelRemote ********* */
    int CmplIO::cancelRemote() {

        if (!writeConfigFile(ACTION_CANCEL))
            throw CmplException(getcFMessage());

        solverStatus=runSolver(ACTION_CANCEL);
        if (solverStatus==PROG_DONT_RUN) throw CmplException("can not connect the solver");
        if (solverStatus==CMPLFRAME_CANCELED) throw CmplException("CmplIO was canceled by user");

        if (solverStatus!=OK)
            throw CmplException("OSSolverService failed - exit code " + solverStatus);

        return CANCEL_OK;

    }//end cancelRemote



    /* *********** writeIdOsolFile *********** */
    bool CmplIO::writeIdOsolFile() {

        QFile cfFile (cFProblem+".id");
        QTextStream cfOsol (&cfFile);

        if (!cfFile.open(QIODevice::Truncate | QIODevice::Text | QIODevice::ReadWrite)) {
            setcFMessage("Internal Error - Can not read and write the id-osol file!" );
            return false;
        }

        cfOsol << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << endl;
        cfOsol << "<osol xmlns=\"os.optimizationservices.org\"" << endl;
        cfOsol << "     xmlns:xs=\"http://www.w3.org/2001/XMLSchema\" " << endl;
        cfOsol << "     xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" " << endl;
        cfOsol << "     xsi:schemaLocation=\"os.optimizationservices.org http://www.optimizationservices.org/schemas/OSoL.xsd\">" << endl;
        cfOsol << "     <general>" << endl;
        cfOsol << "         <jobID>"+setJobId()+"</jobID>" << endl;
        cfOsol << "     </general>" << endl;
        cfOsol << "</osol>" ;

        cfFile.close();

        return true;
    }//end writeIdOsolFile



    /* *********** setJobId *********** */
    QString CmplIO::setJobId() {
        int i,j;
        QString jobId;
        jobId = "cmpl-";
        srand ( time(NULL) );
        for (i=0; i<35; i++) {
            j= rand() % 10;
            jobId+= QString::number(j);
        }
        return jobId;
    }//end setJobId



    /* *********** writeOSpLInputFile *********** */
    bool CmplIO::writeOSpLInputFile() {

        QFile cfFile (cFProblem+".osplin");
        QTextStream cfOsplin(&cfFile);

        if (!cfFile.open(QIODevice::Truncate | QIODevice::Text | QIODevice::ReadWrite)) {
            setcFMessage("Internal Error - Can not read and write the ospl-in file!" );
            return false;
        }

        cfOsplin <<   "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << endl;
        cfOsplin <<   "<ospl xmlns=\"os.optimizationservices.org\">" << endl;
        cfOsplin <<  "  <processHeader>" << endl;
        cfOsplin <<  "     <request action=\"getAll\"/>" << endl;
        cfOsplin <<  "  </processHeader>" << endl;
        cfOsplin <<  "  <processData/>" << endl;
        cfOsplin <<  "</ospl>" << endl;

        cfFile.close();

        QFile cfFile1 (cFProblem+".ospl");
        QTextStream cfOsplOut(&cfFile1);

        if (!cfFile1.open(QIODevice::Truncate | QIODevice::Text | QIODevice::ReadWrite)) {
            setcFMessage("Internal Error - Can not read and write the ospl-out file!" );
            return false;
        }

        cfOsplOut <<  "" ;
        cfFile1.close();

        return true;
    }//end writeOSpLInputFile



    /* *********** getJobStatus *********** */
    int CmplIO::getJobStatus()  {

        QString jobStatus;
        QString osplFile = cFProblem+".ospl";

        QDomDocument doc("myOspl");
        QFile file(osplFile);
        if (!file.open(QIODevice::ReadOnly)) {
            setcFMessage("File could not be open >>" + osplFile) ;
            return KNOCK_FAILED;
        }
        if (!doc.setContent(&file)) {
            file.close();
            setcFMessage("xml error >>" + osplFile );
            return KNOCK_FAILED;
        }
        file.close();

        QDomElement docElem = doc.documentElement();

        if ( docElem.tagName() != "ospl" ) {                                                                            //<osrl>
            setcFMessage("xml file >>" + osplFile  + " is not an ospl file");
            return KNOCK_FAILED;
        }

        QDomNode child1 = docElem.firstChild();
        while(!child1.isNull()) {

            QDomElement elemChild1 = child1.toElement();
            QDomNode child2 = child1.firstChild();

            if ( elemChild1.tagName()=="processData" ) {                                                                        //<general>

                while(!child2.isNull()) {                                                                                      //<generalStatus>
                    QDomElement elemChild2 = child2.toElement();
                    QDomNode child3 = child2.firstChild();

                    if (elemChild2.tagName()=="statistics") {
                        while(!child3.isNull()) {
                            QDomElement elemChild3 = child3.toElement();
                            if ( elemChild3.tagName()=="currentState" )
                                jobStatus=elemChild3.text();
                            child3=child3.nextSibling();
                        }
                    }
                    child2=child2.nextSibling();
                }                                                                                                             //</generalStatus>
            }
            child1=child1.nextSibling();
        }

        if (jobStatus.toLower().contains("idle")) return IDLE;
        if (jobStatus.toLower().contains("busy")) return BUSY;
        if (jobStatus.toLower().contains("noResponse")) return NORESPONSE;
        if (jobStatus.toLower().contains("error")) return ERROR;
    }//end getJobStatus



    /* *********** runCmpl *********** */
    int CmplIO::runCmpl(int cmplMode) {

        QString cmplProgString;

        setCmplOutput();
        if (!isCanceled) {
            if (cmplHandler.isOpen()) cmplHandler.kill();

            if (cmplMode==CMPL_SOLVE)
                cmplProgString = "\""+cmplIOPath+"\"cmpl "+cFProblem+".cmpl -e "+cFProblem+".mprl "+cmplOptions+" -silent";

            if (cmplMode==CMPL_MODEL)
                cmplProgString = "\""+cmplIOPath+"\"cmpl "+cFProblem+".cmpl -x "+cFProblem+".osil -e "+cFProblem+".mprl "+cmplOptions;

            cmplHandler.start( cmplProgString );

            if (!cmplHandler.waitForStarted(15000))
                return PROG_DONT_RUN;
            cmplHandler.waitForFinished(-1);
            return cmplHandler.exitCode();

        } else
            return CMPLFRAME_CANCELED;

    } //end runCmpl


    /* *********** setCmplOutput *********** */
    void CmplIO::setCmplOutput() {
        cmplHandler.setProcessChannelMode(QProcess::MergedChannels);
        cmplOutput="";
        cmplHandler.setReadChannel(QProcess::StandardOutput);
        if (!isCanceled) {
            connect(&cmplHandler, SIGNAL(readyReadStandardOutput()), this , SLOT( writeCmplOutput() ) );
        }
    }//end setCmplOutput



    /* *********** writeCmplOutput *********** */
    void CmplIO::writeCmplOutput() {
        QByteArray data = cmplHandler.readAllStandardOutput();
        cmplOutput = QString(data).toStdString();
        emit readyReadCmplOutput();
    }//end writeCmplOutput


    /* *********** readCmplOutput *********** */
    string CmplIO::readCmplOutput() {
        return cmplOutput;
    }//end readCmplOutput


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

            progString+="\"glpsol --freemps \""+cFProblem+".mps\"  --"+ QString::fromStdString(cmplInst->objSense()) +" --write \""+cFProblem+".gsol\"" + QString::fromStdString(args->nonCoinSolverOpts());

        } else  if (args->solverName()=="gurobi") {

            progString = pythonCommand;

            if (gurobiCmplScriptPath.isEmpty())
                progString += " \""+cmplIOPath;
            else
                if (gurobiCmplScriptPath.startsWith("/"))
                    progString += " \""+gurobiCmplScriptPath;
            else
                progString += " \""+cmplIOPath+gurobiCmplScriptPath;

            progString +=  "\"gurobiCmpl.py \""+ cFProblem+".mps\" \"" + cFProblem + ".gsol\"" +  QString::fromStdString(args->nonCoinSolverOpts());

        } else if (args->solverName()=="scip") {

            if (scipFileName.isEmpty())
               return PROG_DONT_RUN;
            else
                if (scipFileName.startsWith("/"))
                    progString += " \""+scipFileName;
            else
                progString += " \""+cmplIOPath+scipFileName;

            progString +=  "\" -b \""+ cFProblem+".cFconf\"";

        } else {

            if (osSolverServicePath.isEmpty())
                progString = "\""+cmplIOPath;
            else
                if (osSolverServicePath.startsWith("/"))
                    progString = "\""+osSolverServicePath;
            else
                progString = "\""+cmplIOPath+osSolverServicePath;

            progString+="\"OSSolverService -config \""+cFProblem+".cFconf\"";
        }

        setSolverOutput();

        if (!isCanceled) {
            if (solverHandler.isOpen()) solverHandler.kill();

            solverHandler.start( progString );

            if (!solverHandler.waitForStarted(15000))
                return PROG_DONT_RUN;
            solverHandler.waitForFinished(-1);

            return solverHandler.exitCode();

        } else
            return CMPLFRAME_CANCELED;


    }// end runSolver


    /* *********** writeConfigFile *********** */
    bool CmplIO::writeConfigFile(int mode) {

        QFile cfgFile (cFProblem+".cFconf");
        QTextStream cfOpt (&cfgFile);

        QFile setFile (cFProblem + ".set");

        if (!cfgFile.open(QIODevice::Truncate | QIODevice::Text | QIODevice::ReadWrite)) {
            setcFMessage("Internal Error - Cant read and write the config file!\n" );
            return false;
        }


        // Scip solve
        if (mode==ACTION_SOLVE_SCIP) {
            if (setFile.exists())
                cfOpt << "set load \"" << cFProblem << ".set\""<<endl;
            cfOpt << "read \"" << cFProblem << ".mps\"" << endl;
            cfOpt << "optimize" << endl;
            cfOpt << "write solu \"" << cFProblem << ".gsol\"" << endl;
            cfOpt << "quit" << endl;
        }

        // solve
        if (mode==ACTION_SOLVE) {

            cfOpt << "-osil \"" << cFProblem << ".osil\"" << endl;
            cfOpt << "-osrl \"" << cFProblem << ".osrl\"" << endl;

            if (solver!=NOT_DEFINED)
                cfOpt << "-solver " << solver << endl;
            if (serviceLocation!=NOT_DEFINED)
                cfOpt << "-serviceLocation " << serviceLocation << endl;
            if (osol!=NOT_DEFINED )
                if (checkFile(osol))
                    cfOpt << "-osol " << osol << endl;
            cfOpt << "-serviceMethod solve" << endl;
        }

        // send
        if (mode==ACTION_SEND) {

            cfOpt << "-osil \"" << cFProblem << ".osil\"" << endl;

            if (solver!=NOT_DEFINED)
                cfOpt << "-solver " << solver << endl;
            if (serviceLocation==NOT_DEFINED) {
                setcFMessage("serviceLocation not definend" );
                return false;
            }

            cfOpt << "-serviceLocation " << serviceLocation << endl;
            cfOpt << "-serviceMethod send" << endl;
            cfOpt << "-osol \"" << cFProblem << ".id\"" << endl;

            if (osol!=NOT_DEFINED)
                if (checkFile(osol))
                    cfOpt << "-osol \"" << osol << "\"" << endl;
        }

        // knock
        if (mode==ACTION_KNOCK) {

            cfOpt << "-osplInput \"" << cFProblem+".osplin\"" << endl;
            cfOpt << "-osplOutput \""<< cFProblem+".ospl\"" << endl;
            cfOpt << "-osol \"" << cFProblem << ".id\"" << endl;

            if (serviceLocation==NOT_DEFINED) {
                setcFMessage("serviceLocation not definend" );
                return false;
            }

            cfOpt << "-serviceLocation " << serviceLocation << endl;
            cfOpt << "-serviceMethod knock" << endl;
        }

        // retrieve
        if (mode==ACTION_RETRIEVE) {

            cfOpt << "-osol \"" << cFProblem << ".id\"" <<endl;
            cfOpt << "-osrl \"" << cFProblem << ".osrl\"" << endl;

            if (serviceLocation==NOT_DEFINED) {
                setcFMessage("serviceLocation not definend" );
                return false;
            }

            cfOpt << "-serviceLocation " << serviceLocation << endl;
            cfOpt << "-serviceMethod retrieve" << endl;
        }

        // cancel
        if (mode==ACTION_CANCEL) {

            cfOpt << "-osol \"" << cFProblem << ".id\"" << endl;

            if (serviceLocation==NOT_DEFINED) {
                setcFMessage("serviceLocation not definend" );
                return false;
            }

            cfOpt << "-serviceLocation " << serviceLocation << endl;
            cfOpt << "-serviceMethod kill" << endl;
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
        osol="";

        cmplStatus=0;

        cmplIOPath="";
        osSolverServicePath="";
        glpkPath="";
        gurobiCmplScriptPath="";
        pythonCommand="";
        scipFileName="";
       // soPlexFileName="";

        solver=NOT_DEFINED;
        serviceLocation=NOT_DEFINED;
        osol=NOT_DEFINED;

        osolIdString="";

        cmplOutput="";
        solverOutput="";
        externalSolverOutput=false;

        cmplFileName=QString::fromStdString(args->cmplFile());
        if (!checkFile(cmplFileName))
            throw CmplException("CMPL file  <"+cmplFileName.toStdString()+"> does not exist!");
        cFProblem=checkDotCmpl();

        if( !args->osolFile().empty() ) {
            osol=QString::fromStdString(args->osolFile());
            if (!checkFile(osol))
                throw CmplException("OSol file <" + args->osolFile() + "> does not exist");
        }

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
            if ( line.toLower().contains("gurobicmplscriptpath") )
                gurobiCmplScriptPath = line.mid( line.indexOf("=")+1, line.length()).trimmed();
            if ( line.toLower().contains("python") )
                pythonCommand = line.mid( line.indexOf("=")+1, line.length()).trimmed();
            if ( line.toLower().contains("scipfilename") )
                scipFileName = line.mid( line.indexOf("=")+1, line.length()).trimmed();
         /*   if ( line.toLower().contains("soplexfilename") )
                soPlexFileName = line.mid( line.indexOf("=")+1, line.length()).trimmed();
        */
        }
        cFOptFile.close();

    } //end loadOptions


}
