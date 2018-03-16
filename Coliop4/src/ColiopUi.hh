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

#ifndef COLIOPUI_H
#define COLIOPUI_H

#include <QMainWindow>
#include <QtGui>
#include <QTextDocumentWriter>
#include <QFileDialog>
#include <QInputDialog>
#include <QTextCursor>
#include <QPrinter>
#include <QPrintDialog>
#include <QMessageBox>
#include <QCoreApplication>
#include <QRegExp>

#include "CmplHighlighter.hh"
#include "ClpAbout.hh"
#include "ClpProblem.hh"
#include "CmplRunner.hh"

namespace Ui {
    class coliopUi;
}

class ColiopUi : public QMainWindow
{
    Q_OBJECT

public:
    explicit ColiopUi(QWidget *parent = 0);
    ~ColiopUi();

    void openFirstProblem( QString &fileName );

private:
    Ui::coliopUi *ui;
    CmplHighlighter *cmplSyntaxHighlighter;
    ClpProblem *problem;
    ClpAbout *coliopAbout;
    CmplRunner *_cmplHandler;
    QList<cmplMessages> _cmplMessages;
    bool _noOutput;

    QProcess *_cmplManualHandler;

    bool saveProblem(QString &fileName);
    void openProblem( QString &fileName );
    void setupFonts();
    void gotoLine(int lineNumber);
    void enableEditorActions(bool enable);
    void enableSaveActions(bool enable);
    void cmplList();

    bool clpAnswerMessage(QString title, QString msg);
    void clpWarning(QString title,QString msg);

    int checkAsyncMode();

private slots:
    void on_cmplListClicked();
    void on_cmplMessageListClicked();
    void on_actionPrint();
    void on_actionPaste();
    void on_actionCut();
    void on_actionCopy();
    void on_actionRedo();
    void on_actionUndo();
    void on_cmplEditorChanged();
    void on_clpProblemTab_currentChanged();
    void on_actionZoom_out();
    void on_actionZoom_in();
    void on_actionGo_to_line();
    void on_actionSave_Problem();
    void on_actionSave_Problem_as();
    void on_actionAbout();
    void on_actionNewColiopWindow();
    void on_actionOpen_Problem();
    void on_actionNew_Problem();
    void on_actionSolve();
    void on_actionCancel();
    void on_cmplFinished();
    void on_cmplStarted();
    void on_cmplError();
    void on_cmplCanceled();
    void on_cmplHelp();
    void on_actionQuit();

};

#endif // COLIOPUI_H
