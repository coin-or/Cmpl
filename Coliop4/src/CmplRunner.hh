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

#ifndef CMPLRUNNER_H
#define CMPLRUNNER_H

#include <QObject>
#include <QThread>
#include <QProcess>
#include <QPlainTextEdit>
#include <QString>
#include <QDir>
#include <QDomDocument>
#include <QCoreApplication>

//#include <QtDebug>

#include "ClpProblem.hh"

typedef struct
{
    QString file;
    int line;
    QString type;
    QString description;
} cmplMessages;


class CmplRunner : public QThread
{
    Q_OBJECT
public:

    explicit CmplRunner(QObject *parent = 0, QPlainTextEdit *clpOut=0, QPlainTextEdit *cmplSol=0, ClpProblem *problem=0);

    ~CmplRunner();

    void run( ) Q_DECL_OVERRIDE ;
    void cancelProblem();

    QString getError();
    QString getGeneralStatus();
    QString getStatusMessage();
    QList<cmplMessages> getCmplMessages();
    int getNrOfMessages();
    bool isSolution();


private:
    QObject *_parent;
    bool _isCanceled;
    bool _isFinished;

    QPlainTextEdit *_clpOut;
    QPlainTextEdit *_clpSol;
    ClpProblem *_problem;

    QString _cmplBinary;
    QStringList  _cmplArgs;
    QProcess * _cmplHandler;

    QString _cmplErrorMessage;

    int _exitCode;

    QList<cmplMessages> _cmplMessages;
    QString _generalStatus;
    QString _statusMessage;
    int _nrOfMessages;
    bool _solutionFound ;

    bool readCmplMessages();


signals:
    void CmplRunnerError();
    void CmplRunnerStarted();
    void CmplRunnerFinished();
    void CmplRunnerCanceled();
    void NoSolutionFound();

public slots:
    void writeCmplOutput();
    void getSolutionAndMessages();


};

#endif // CMPLRUNNER_H
