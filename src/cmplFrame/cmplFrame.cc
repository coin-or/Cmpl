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

#include "cmplFrame.hh"


/* *********** constructor ******* */
cmplFrame::cmplFrame() {
    setTerminationEnabled(false);
    setDefaults();
    cmplFramePath =  QCoreApplication::applicationDirPath()+"/";
    cFStatus=loadOptions();
}//end constructor


/* *********** run *************** */
void cmplFrame::run() {
    isCanceled=false;
}//end run


/* *********** destructor ********* */
cmplFrame::~cmplFrame() {
}// end destructor


/* *********** setModel *********** */
void cmplFrame::setModel(QString &cmplFile ) throw (cFException) {
        cmplFileName=cmplFile;
	
        if (!checkFile(cmplFileName))
            throw cFException("CMPL file  <"+cmplFileName.toStdString()+"> does not exist!");
	
        cFProblem=checkDotCmpl();

        cmpLMsg.setMprl(cFProblem+".mprl");

}//end setModel
	

/* *********** setOSol *********** */
void cmplFrame::setOsol(QString &osolName) throw (cFException)  {
     osol=osolName;
     if (!checkFile(osolName))
         throw cFException("OSol file <" + osolName.toStdString() + "> does not exist");

}//end setOsol

/* *********** setSolver ********* */
void cmplFrame::setSolver(QString &solverName) {
      solver=solverName;
}//end setSolver


/* *********** setServiceLocation ***********  */
void cmplFrame::setServiceLocation(QString &serviceLocationName) {
       serviceLocation=serviceLocationName;
}//end setServiceLocation



/* *********** setCmplOptions ***** */
bool cmplFrame::setCmplOptions(QString cmplOpts) {

    bool argCheck=true;
    /* allowed options
        -l [<File>]         : output for replacements for products of variables
        -s [<File>]         : output for short statistic info
        -p [<File>]         : output for protocol
        -gn                 : generation option: don't make reductions
        -gf                 : generation option: constraints for products of variables follow the product
        -cd                 : no warning at multiple parameter definition
        -ca                 : no warning at deprecated '=' assignment
        -ci<X>              : mode for integer expressions (0 - 3), defaults to 1
        -fc<X>              : format option: maximal length of comment, defaults to 60
        -ff                 : format option: generate free MPS
        -f%<format>         : format option: format spezifier for float number output, defaults to %f
    */

    QStringList cmplArgs;
    cmplArgs=cmplOpts.split(QRegExp("\\s+"), QString::SkipEmptyParts);

    for(int i=0; i<cmplArgs.length(); i++) {
        if (cmplArgs[i].startsWith("-")) {
            if  ( (cmplArgs[i]=="-l" ) ||
                  (cmplArgs[i]=="-s" ) ||
                  (cmplArgs[i]=="-p" ) ||
                  (cmplArgs[i]=="-gn" ) ||
                  (cmplArgs[i]=="-gf" ) ||
                  (cmplArgs[i]=="-cd" ) ||
                  (cmplArgs[i]=="-ca" ) ||
                  (cmplArgs[i].startsWith("-ci") ) ||
                  (cmplArgs[i].startsWith("-fc") ) ||
                  (cmplArgs[i]=="-ff" ) ||
                  (cmplArgs[i].startsWith("-f%") ) )
                continue;
            else {
                setcFMessage("Wrong cmpl option at: "+cmplArgs[i] );
                argCheck=false;
                break;
            }
        } else {
            if ( (i=1) || ! ( cmplArgs[i].startsWith("-l") ||
                              cmplArgs[i].startsWith("-s") ||
                              cmplArgs[i].startsWith("-p")
                             )
                ) {
                setcFMessage("Wrong cmpl option at: "+cmplArgs[i] );
                argCheck=false;
                break;
            }
        }
    }

    if (argCheck)
        cmplOptions=cmplOpts;
    else
        cmplOptions="";

    return argCheck;
}// end setCmplOptions



/* ************ solve ************** */
int cmplFrame::solve() throw (cFException) {

    if (!writeConfigFile(ACTION_SOLVE))
        throw cFException(getcFMessage());

    /* it is necessary to delete the old osrl file - if the solver failed and an old OSrL file is existing*/
    QFile f (cFProblem + ".osrl");
    f.remove();

    /* it is necessary to delete the old mprl file - if the cmpl failed and an old mprl file is existing*/
    QFile c (cFProblem + ".mprl");
    c.remove();

    cmplStatus=runCmpl(CMPL_SOLVE);
    if (cmplStatus==PROG_DONT_RUN) throw cFException("can not connect cmpl");
    if (cmplStatus==CMPLFRAME_CANCELED) throw cFException("cmplFrame was canceled by user");

    if (cmplStatus!=OK) {

        if(cmplStatus<6) {

            if (!cmpLMsg.readMPrL())
                throw cFException("CMPL failed - The result file can not be read");
            else
                return CMPL_FAILED;
        } else
            throw cFException("OSSolverService failed - exit code " );

    } else {
         result.setProblemName(cFProblem);

            if (!result.getResult())
                throw cFException("Can not read the solution " + cFProblem.toStdString());

            if (result.nrOfSolutions()==0)
                return SOLVER_FAILED;

    }

    return SOLVE_OK;

}// end solve


/* **************** runOSSolverservice ********** */
int cmplFrame::runOSSolverservice() throw (cFException) {

    QString osilFileName = cFProblem + ".osil";
    if (!checkFile(osilFileName))
        throw cFException("OSil file <" + osilFileName.toStdString() + "> does not exist");

    if (!writeConfigFile(ACTION_SOLVE))
        throw cFException(getcFMessage());

    /* it is necessary to delete the old osrl file - if the solver failed and an old OSrL file is existing*/
    QFile f (cFProblem + ".osrl");
    f.remove();

    solverStatus=runSolver(ACTION_SOLVE);
    if (solverStatus==PROG_DONT_RUN) throw cFException("can not connect the solver: "+osSolverServicePath.toStdString());
    if (solverStatus==CMPLFRAME_CANCELED) throw cFException("cmplFrame was canceled by user");

    if (solverStatus!=OK) {
        throw cFException("OSSolverService failed - exit code " + solverStatus);
    } else {

        result.setProblemName(cFProblem);

        if (!result.getResult())
            throw cFException("Can not read the solution " + cFProblem.toStdString());

        if (result.nrOfSolutions()==0)
            return SOLVER_FAILED;
    }

    return SOLVE_OK;

}//end runOSSolverservice



/* ********** send *********************** */
int cmplFrame::send() throw (cFException) {


    /* it is necessary to delete the old mprl file - if the cmpl failed and an old mprl file is existing*/
    QFile f (cFProblem + ".mprl");
    f.remove();

    cmplStatus=runCmpl(CMPL_MODEL);
    if (cmplStatus==CMPLFRAME_CANCELED) throw cFException("cmplFrame was canceled by user");

    if (cmplStatus!=OK) {

        if (!cmpLMsg.readMPrL())
            throw cFException("CMPL failed - The result file can not be read");
        else
            return CMPL_FAILED;

    } else {

        if (!writeIdOsolFile())
            throw cFException(getcFMessage());

        if (!writeConfigFile(ACTION_SEND))
            throw cFException(getcFMessage());

        solverStatus=runSolver(ACTION_SEND);
        if (solverStatus==PROG_DONT_RUN) throw cFException("can not connect the solver");
        if (solverStatus==CMPLFRAME_CANCELED) throw cFException("cmplFrame was canceled by user");

        if (solverStatus!=OK)
            throw cFException("OSSolverService failed - exit code " + solverStatus);

    }
    return SEND_OK;
}//end send


/* *************** knock ************** */
int cmplFrame::knock() throw (cFException) {
    int jobStatus=0;

    if (!writeOSpLInputFile())
        throw cFException(getcFMessage());

    if (!writeConfigFile(ACTION_KNOCK))
        throw cFException(getcFMessage());

    solverStatus=runSolver(ACTION_KNOCK);
    if (solverStatus==PROG_DONT_RUN) throw cFException("can not connect the solver");
    if (solverStatus==CMPLFRAME_CANCELED) throw cFException("cmplFrame was canceled by user");

    if (solverStatus!=OK) {
        throw cFException("OSSolverService failed - exit code " + solverStatus);
    } else {
        jobStatus=getJobStatus();
        if (jobStatus==KNOCK_FAILED) throw cFException(getcFMessage());
    }
    return jobStatus;
}//end knock


/* ****************** retrieve ***************** */
int cmplFrame::retrieve() throw (cFException) {

    if (!writeConfigFile(ACTION_RETRIEVE))
        throw cFException(getcFMessage());

    /* it is necessary to delete the old osrl file - if the solver failed and an old OSrL file is existing*/
    QFile f (cFProblem + ".osrl");
    f.remove();

    solverStatus=runSolver(ACTION_RETRIEVE);
    if (solverStatus==PROG_DONT_RUN) throw cFException("can not connect the solver");
    if (solverStatus==CMPLFRAME_CANCELED) throw cFException("cmplFrame was canceled by user");

    if (solverStatus!=OK) {
        throw cFException("OSSolverService failed - exit code " + solverStatus);
    } else {

        result.setProblemName(cFProblem);

        if (!result.getResult())
           throw cFException("Can not read the solution " + cFProblem.toStdString());

        if (result.nrOfSolutions()==0)
            return SOLVER_FAILED;
    }
    return RETRIEVE_OK;
}//end solve


/* *********** cancel ********* */
void cmplFrame::cancel() {
    if (cmplHandler.isOpen()) cmplHandler.kill();
    if (solverHandler.isOpen()) solverHandler.kill();
    isCanceled=true;
    setDefaults();
}// end terminate


/* *********** cancelRemote ********* */
int cmplFrame::cancelRemote() {

    if (!writeConfigFile(ACTION_CANCEL))
        throw cFException(getcFMessage());

    solverStatus=runSolver(ACTION_CANCEL);
    if (solverStatus==PROG_DONT_RUN) throw cFException("can not connect the solver");
    if (solverStatus==CMPLFRAME_CANCELED) throw cFException("cmplFrame was canceled by user");

    if (solverStatus!=OK)
        throw cFException("OSSolverService failed - exit code " + solverStatus);

    return CANCEL_OK;

}//end cancelRemote



/* *********** writeIdOsolFile *********** */
bool cmplFrame::writeIdOsolFile() {

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
QString cmplFrame::setJobId() {
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
bool cmplFrame::writeOSpLInputFile() {

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
int cmplFrame::getJobStatus()  {

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
int cmplFrame::runCmpl(int cmplMode) {

    QString cmplProgString;

    setCmplOutput();
    if (!isCanceled) {
        if (cmplHandler.isOpen()) cmplHandler.kill();

        if (cmplMode==CMPL_SOLVE)
            cmplProgString = "\""+cmplFramePath+"\"cmpl "+cFProblem+".cmpl -e "+cFProblem+".mprl "+cmplOptions+" -silent";

        if (cmplMode==CMPL_MODEL)
            cmplProgString = "\""+cmplFramePath+"\"cmpl "+cFProblem+".cmpl -x "+cFProblem+".osil -e "+cFProblem+".mprl "+cmplOptions;

        cmplHandler.start( cmplProgString );

        if (!cmplHandler.waitForStarted(15000))
            return PROG_DONT_RUN;
        cmplHandler.waitForFinished(-1);
        return cmplHandler.exitCode();

    } else
        return CMPLFRAME_CANCELED;

} //end runCmpl


/* *********** setCmplOutput *********** */
void cmplFrame::setCmplOutput() {
    cmplOutput="";
    cmplHandler.setReadChannel(QProcess::StandardOutput);
    if (!isCanceled) {
        connect(&cmplHandler, SIGNAL(readyReadStandardOutput()), this , SLOT( writeCmplOutput() ) );
    }
}//end setCmplOutput



/* *********** writeCmplOutput *********** */
void cmplFrame::writeCmplOutput() {
      QByteArray data = cmplHandler.readAllStandardOutput();
      cmplOutput = QString(data).toStdString();
      emit readyReadCmplOutput();
}//end writeCmplOutput


/* *********** readCmplOutput *********** */
string cmplFrame::readCmplOutput() {
    return cmplOutput;
}//end readCmplOutput


/* *********** setSolverOutput *********** */
void cmplFrame::setSolverOutput() {
    solverOutput="";
    solverHandler.setReadChannel(QProcess::StandardOutput);
    if (!isCanceled) {
        connect(&solverHandler, SIGNAL(readyReadStandardOutput()), this , SLOT( writeSolverOutput() ) );
    }
}//end setSolverOutput


/* *********** writeSolverOutput *********** */
void cmplFrame::writeSolverOutput() {
      QByteArray data = solverHandler.readAllStandardOutput();
      solverOutput = QString(data).toStdString();
      emit readyReadSolverOutput();
 }//end writeSolverOutput


/* *********** readSolverOutput *********** */
string cmplFrame::readSolverOutput() {
    if (externalSolverOutput)
        return solverOutput;
    else return "";
}//end readSolverOutput



/* *********** runSolver *********** */
int cmplFrame::runSolver(int actionMode) {

    QString oSSProgString;

    if (actionMode==ACTION_SOLVE)
        externalSolverOutput=true;
    else
        externalSolverOutput=false;

    setSolverOutput();

    if (!isCanceled) {
        if (solverHandler.isOpen()) solverHandler.kill();

        if (osSolverServicePath.isEmpty())
               oSSProgString = "\""+cmplFramePath;
        else
            if (osSolverServicePath.startsWith("/"))
                oSSProgString = "\""+osSolverServicePath;
            else
                oSSProgString = "\""+cmplFramePath+osSolverServicePath;

        oSSProgString+="\"OSSolverService -config "+cFProblem+".cFconf";

        solverHandler.start( oSSProgString );
        if (!solverHandler.waitForStarted(15000))
            return PROG_DONT_RUN;
        solverHandler.waitForFinished(-1);
        return solverHandler.exitCode();

    } else
        return CMPLFRAME_CANCELED;

}// end runSolver


/* *********** writeConfigFile *********** */
bool cmplFrame::writeConfigFile(int mode) {

    QFile cfgFile (cFProblem+".cFconf");
    QTextStream cfOpt (&cfgFile);

    if (!cfgFile.open(QIODevice::Truncate | QIODevice::Text | QIODevice::ReadWrite)) {
        setcFMessage("Internal Error - Cant read and write the config file!\n" );
        return false;
    }

    // solve
    if (mode==ACTION_SOLVE) {

        cfOpt << "-osil " << cFProblem << ".osil" << endl;
        cfOpt << "-osrl " << cFProblem << ".osrl" << endl;

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

        cfOpt << "-osil " << cFProblem << ".osil" << endl;

        if (solver!=NOT_DEFINED)
            cfOpt << "-solver " << solver << endl;
        if (serviceLocation==NOT_DEFINED) {
            setcFMessage("serviceLocation not definend" );
            return false;
        }

        cfOpt << "-serviceLocation " << serviceLocation << endl;
        cfOpt << "-serviceMethod send" << endl;
        cfOpt << "-osol " << cFProblem << ".id" << endl;

        if (osol!=NOT_DEFINED)
            if (checkFile(osol))
            cfOpt << "-osol " << osol << endl;
        }

    // knock
    if (mode==ACTION_KNOCK) {

        cfOpt << "-osplInput " << cFProblem+".osplin" << endl;
        cfOpt << "-osplOutput "<< cFProblem+".ospl" << endl;
        cfOpt << "-osol " << cFProblem << ".id" << endl;

        if (serviceLocation==NOT_DEFINED) {
            setcFMessage("serviceLocation not definend" );
            return false;
        }

        cfOpt << "-serviceLocation " << serviceLocation << endl;
        cfOpt << "-serviceMethod knock" << endl;
    }

    // retrieve
    if (mode==ACTION_RETRIEVE) {

        cfOpt << "-osol " << cFProblem << ".id" <<endl;
        cfOpt << "-osrl " << cFProblem << ".osrl" << endl;

        if (serviceLocation==NOT_DEFINED) {
            setcFMessage("serviceLocation not definend" );
            return false;
        }

        cfOpt << "-serviceLocation " << serviceLocation << endl;
        cfOpt << "-serviceMethod retrieve" << endl;
    }

    // cancel
    if (mode==ACTION_CANCEL) {

        cfOpt << "-osol " << cFProblem << ".id" << endl;

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
void cmplFrame::setcFMessage(const QString &cFMsg) {
      cFMessage=cFMsg;
}//end setcFMessage


/* *********** getcFMessage *********** */
string cmplFrame::getcFMessage() {
      return cFMessage.toStdString();
}//end getcFMessage


/* *********** setDefaults ********* */
void cmplFrame::setDefaults() {

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

    cmplFramePath="";
    osSolverServicePath="";

    solver=NOT_DEFINED;
    serviceLocation=NOT_DEFINED;
    osol=NOT_DEFINED;

    osolIdString="";

    cmplOutput="";
    solverOutput="";
    externalSolverOutput=false;

}//end setDefaults



/* *********** checkFile *********** */
bool cmplFrame::checkFile(QString &fName) {
    if ( fName.trimmed().isEmpty() ) return false;
    QFile f (fName);
    return f.exists();
} //end checkFile



/* *********** setModel *********** */
QString cmplFrame::checkDotCmpl() throw (cFException) {
    int dotGen = cmplFileName.toLower().indexOf(".cmpl");
    if (dotGen==-1) throw cFException("CMPL file name does not include \".cmpl\"");
    return cmplFileName.left(dotGen);
}//end checkDotCmpl



 /* ************** loadOptions ***************** */
 bool cmplFrame::loadOptions()  {

    QString optFName=cmplFramePath + "cmpl.opt";
    QFile cFOptFile (optFName);
    QString tmpoSSProgString;

    if (!cFOptFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        setcFMessage("Internal Error - Cant read the option file!\n" + optFName);
        return false;
    }

    while (!cFOptFile.atEnd()) {

        if (cFOptFile.error()>0) {
            setcFMessage("Internal Error - Cant read the option file! >> "+ cFOptFile.error() );
            return false;
        }

        QString line = cFOptFile.readLine();
        if ( line.toLower().contains("ossolverservicepath") ) {
            osSolverServicePath = line.mid( line.indexOf("=")+1, line.length()).trimmed();
            continue;
        }
    }
    cFOptFile.close();
    return true;

} //end loadOptions

 /* ************** isStarted ***************** */
 bool cmplFrame::isStarted()  {
     return cFStatus;
 }// end isStarted
