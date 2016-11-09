/***********************************************************************
 *  This code is part of CMPL
 *
 *  Copyright (C) 2007 - 2016
 *  Thomas Schleiff - Halle(Saale), Germany and
 *  Mike Steglich - Technical University of Applied Sciences
 *  Wildau, Germany
 *
 *  Coliop and CMPL are projects of the Technical University of
 *  Applied Sciences Wildau.
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

#include "CmplRunner.hh"


CmplRunner::CmplRunner(QObject *parent, QPlainTextEdit *clpOut, QPlainTextEdit *clpSol, ClpProblem *problem) {
    _clpOut=clpOut;
    _clpSol=clpSol;
    _problem=problem;

    _parent=parent;

    _isCanceled=false;
    _isFinished=false;

    QString runningOs = QSysInfo::productType();
    QString progPath = QCoreApplication::applicationDirPath();



    if (runningOs == "windows" ) {
        _cmplBinary = "\""+progPath+"/../cmpl.bat\"";

    } else if (runningOs == "osx"  or runningOs== "macos") {
        _cmplBinary = "\""+progPath+"/../../../../cmpl\"";
    } else {
        _cmplBinary = "\""+progPath+"/../cmpl\"";
    }

    //_cmplBinary = "/Applications/Cmpl/cmpl" ;


    _exitCode=0;

    _generalStatus="";
    _statusMessage="";
    _nrOfMessages=0;
    _solutionFound = false;

    QString fileName=_problem->getProblemPath()+"/"+_problem->getProblemBaseName()+".sol";
    QFile file(fileName);
    if (file.exists()) {
        file.remove();
    }

    fileName=_problem->getProblemPath()+"/"+_problem->getProblemBaseName()+".cmsg";
    file.setFileName(fileName);
    if (file.exists()) {
        file.remove();
    }
}

CmplRunner::~CmplRunner()
{
    //qDebug() << "### isrunning vor destructor";
    if (_cmplHandler->isOpen() ) {
        _cmplHandler->close();
    }

    delete _cmplHandler;
    //qDebug() << "### isrunning nach destructor";
}

void CmplRunner::run()
{
    if (QDir::setCurrent(_problem->getProblemPath())) {

        _cmplArgs.append('\"'+_problem->getProblem() + "\"");
        _cmplArgs.append("-solutionAscii");
        _cmplArgs.append("-e");

        QString runProg=_cmplBinary+" "+_cmplArgs.join(" ");


        //_cmplHandler = new QProcess(_parent);
        _cmplHandler = new QProcess();
        _cmplHandler->setProcessChannelMode(QProcess::MergedChannels);
        _cmplHandler->start(runProg);

        if (!_cmplHandler->waitForStarted()) {
            _cmplErrorMessage="Cannot execute <"+_cmplBinary+">";
            emit CmplRunnerError( );
        }
        emit CmplRunnerStarted();

        connect(_cmplHandler, &QProcess::readyReadStandardOutput, this, &CmplRunner::writeCmplOutput );
        connect(_cmplHandler, SIGNAL(finished(int)), this , SLOT(getSolutionAndMessages()));

       // qDebug() << "### isrunning vor waitForFinished";
        _cmplHandler->waitForFinished(-1);
        //qDebug() << "### isrunning nach waitForFinished";

        /*if (!_isCanceled) {
           // _exitCode=_cmplHandler->exitCode();
            getSolutionAndMessages();
           // emit CmplRunnerFinished();
        }*/

    } else {
        _cmplErrorMessage="Cannot access the folder <"+_problem->getProblemPath()+">";
        emit CmplRunnerError();
    }
}

void CmplRunner::cancelProblem()
{
    emit CmplRunnerCanceled();
    _isCanceled=true;
    //_cmplHandler->terminate();
    _cmplHandler->kill();
    //_cmplHandler->close();
    //qDebug() << "### isrunning nach cancel";
}

QString CmplRunner::getError()
{
    return _cmplErrorMessage;
}

QString CmplRunner::getGeneralStatus()
{
    return _generalStatus;
}

QString CmplRunner::getStatusMessage()
{
    return _statusMessage;
}

QList<cmplMessages> CmplRunner::getCmplMessages()
{
    return _cmplMessages;
}

int CmplRunner::getNrOfMessages()
{
    return _nrOfMessages;
}

bool CmplRunner::isSolution()
{
    return _solutionFound ;
}

bool CmplRunner::readCmplMessages()
{
    QDomDocument doc("cmplMessages");

    QString fName = _problem->getProblemPath()+"/"+_problem->getProblemBaseName()+".cmsg";

    QFile file(fName);
    if (file.open(QIODevice::ReadOnly)) {

        if (doc.setContent(&file)) {

            QDomElement docElem = doc.documentElement();

            if ( docElem.tagName() == "CmplMessages") {

                QDomNode child1 = docElem.firstChild();
                while(!child1.isNull()) {

                    QDomElement elemChild1 = child1.toElement();
                    QDomNode child2 = child1.firstChild();

                    if ( elemChild1.tagName()=="general" ) {
                        while(!child2.isNull()) {
                            QDomElement elemChild2 = child2.toElement();

                            if (elemChild2.tagName()=="generalStatus") _generalStatus=elemChild2.text();
                            if (elemChild2.tagName()=="message") _statusMessage=elemChild2.text();
                            child2=child2.nextSibling();
                        }
                    }

                    if ( elemChild1.tagName()=="messages" ) {
                        _nrOfMessages=elemChild1.attribute("numberOfMessages").toInt();

                        while(!child2.isNull()) {
                            QDomElement elemChild2 = child2.toElement();

                            cmplMessages cmplMessage;
                            cmplMessage.type=elemChild2.attribute("type");
                            cmplMessage.file=elemChild2.attribute("file");
                            cmplMessage.line=elemChild2.attribute("line").toInt();
                            cmplMessage.description=elemChild2.attribute("description");

                            _cmplMessages.append(cmplMessage);

                            child2=child2.nextSibling();
                        }
                    }
                    child1=child1.nextSibling();
                }
            } else {
                file.close();
                _cmplErrorMessage="CmplMessages cannot be read - File is not a CmplMessages file";
                return false;
            }
        } else {
            file.close();
            _cmplErrorMessage="CmplMessages cannot be read - XML errors";
            return false;
        }
        file.close();
        file.remove();
    } else {
        _cmplErrorMessage="CMPL has finished with errors";
       return false;
    }
    return true;
}


void CmplRunner::getSolutionAndMessages()
{
    if (!_isCanceled) {
        if (readCmplMessages()) {
            QString fileName ;
            fileName=_problem->getProblemPath()+"/"+_problem->getProblemBaseName()+".sol";

            if (!fileName.isEmpty()) {
                QFile file(fileName);
                if (file.open(QFile::ReadOnly | QFile::Text)) {
                    _clpSol->setPlainText(file.readAll());
                    file.close();
                    file.remove();
                    _solutionFound = true;
                } else {
                    _solutionFound = false;
                }
            }
        } else {
            _generalStatus="normal";
            _statusMessage="";
            _nrOfMessages=0;
            _solutionFound = false;
        }
        emit CmplRunnerFinished();
        _isFinished=true;
    }
}


void CmplRunner::writeCmplOutput() {
    _clpOut->appendPlainText(QString(_cmplHandler->readAllStandardOutput()));
}
