#include "ColiopUi.hh"
#include "ui_coliopui.h"




ColiopUi::ColiopUi(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::coliopUi)
{
    ui->setupUi(this);

    problem = new ClpProblem();
    coliopAbout = new ClpAbout();
    _noOutput=false;

    setupFonts();
    problem->setProblemChanged(false);

    ui->cmplList->hide();

    connect(ui->actionNew_Problem,  &QAction::triggered, this, &ColiopUi::on_actionNew_Problem );
    connect(ui->actionOpen_Problem, &QAction::triggered, this, &ColiopUi::on_actionOpen_Problem );
    connect(ui->actionSave_Problem, &QAction::triggered, this, &ColiopUi::on_actionSave_Problem );
    connect(ui->actionSave_Problem_as, &QAction::triggered, this, &ColiopUi::on_actionSave_Problem_as );
    connect(ui->actionPrint, &QAction::triggered, this, &ColiopUi::on_actionPrint );

    connect(ui->actionAbout, &QAction::triggered, this, &ColiopUi::on_actionAbout );
    connect(ui->actionQuit, &QAction::triggered, this, &ColiopUi::on_actionQuit );


    connect(ui->cmplEditor, &QPlainTextEdit::textChanged, this, &ColiopUi::on_cmplEditorChanged );

    connect(ui->actionPaste, &QAction::triggered, this,&ColiopUi::on_actionPaste );
    connect(ui->actionCut, &QAction::triggered, this, &ColiopUi::on_actionCut );
    connect(ui->actionCopy, &QAction::triggered, this, &ColiopUi::on_actionCopy );

    connect(ui->actionRedo, &QAction::triggered, this, &ColiopUi::on_actionRedo );
    connect(ui->actionUndo, &QAction::triggered, this, &ColiopUi::on_actionUndo );

    connect(ui->actionZoom_out, &QAction::triggered, this, &ColiopUi::on_actionZoom_out );
    connect(ui->actionZoom_in,  &QAction::triggered, this, &ColiopUi::on_actionZoom_in );
    connect(ui->actionGo_to_line,&QAction::triggered, this, &ColiopUi::on_actionGo_to_line );

    connect(ui->cmplList,&QListWidget::itemClicked, this, &ColiopUi::on_cmplListClicked );
    connect(ui->cmplMessages,&QListWidget::itemClicked, this, &ColiopUi::on_cmplMessageListClicked );
    connect(ui->clpProblemTab, &QTabWidget::currentChanged, this, &ColiopUi::on_clpProblemTab_currentChanged);

    connect(ui->actionSolve, &QAction::triggered, this, &ColiopUi::on_actionSolve);
    connect(ui->actionCancel, &QAction::triggered, this, &ColiopUi::on_actionCancel);

    connect(ui->actionCMPL_Manual, &QAction::triggered, this, &ColiopUi::on_cmplHelp);


    enableSaveActions(false);
    enableEditorActions(true);

    ui->actionCancel->setEnabled(false);
    ui->actionSolve->setEnabled(false);
    ui->clpMessageTab->setVisible(false);

}

ColiopUi::~ColiopUi()
{
    delete coliopAbout;
    delete problem;
    delete ui;

}

void ColiopUi::openFirstProblem(QString &fileName)
{

    problem->setProblem(fileName);
    openProblem(fileName);

    problem->setProblemChanged(false);
    enableSaveActions(false);
    ui->cmplMessages->clear();
    ui->cmplOutPut->setPlainText("");
    ui->cmplSolution->setPlainText("");
    ui->clpMessageTab->setVisible(false);


    problem->setActiveCmplIndex(0);

    setWindowTitle("Coliop4 - "+problem->getProblemName());
    ui->clpProblemTab->setCurrentIndex(0);
    ui->actionSolve->setEnabled(true);

}


void ColiopUi::setupFonts()
{
    QFont font;
    font.setFamily("Monaco");
    font.setFixedPitch(true);
    //font.setPointSize(12);
    ui->cmplEditor->setFont(font);
    cmplSyntaxHighlighter = new CmplHighlighter(ui->cmplEditor->document());
    ui->cmplList->setFont(font);
    ui->cmplOutPut->setFont(font);
    ui->cmplSolution->setFont(font);
    ui->cmplMessages->setFont(font);
}


void ColiopUi::on_actionNew_Problem()
{
    ui->cmplEditor->clear();
    ui->cmplEditor->setPlainText("parameters:\n\nvariables:\n\nobjectives:\n\nconstraints:\n\n");
    problem->setDefaults();

    ui->actionSolve->setEnabled(false);

    problem->setProblemChanged(true);
    enableSaveActions(true);
    ui->cmplMessages->clear();
    ui->cmplOutPut->setPlainText("");
    ui->cmplSolution->setPlainText("");
    ui->clpMessageTab->setVisible(false);
    problem->setActiveCmplIndex(0);

    setWindowTitle("Coliop4 - New problem");
}

void ColiopUi::on_actionSolve()
{
    if (problem->isProblemChanged()) {
        QString sName = problem->getProblemPath()+"/"+problem->getCmplListElement(problem->getActiveCmplIndex());
        if (clpAnswerMessage("Save file", "File <"+sName+"> is changed. \n\nDo you want to save it?") == true) {
            saveProblem(sName);
          }
    }

    _cmplHandler = new CmplRunner(this, ui->cmplOutPut, ui->cmplSolution, problem);
    connect(_cmplHandler, &CmplRunner::CmplRunnerStarted, this, &ColiopUi::on_cmplStarted);
    connect(_cmplHandler, &CmplRunner::CmplRunnerFinished, this, &ColiopUi::on_cmplFinished);
    connect(_cmplHandler, &CmplRunner::CmplRunnerError, this, &ColiopUi::on_cmplError);
    //connect(_cmplHandler, &CmplRunner::CmplRunnerCanceled, this, &ColiopUi::on_cmplCanceled);

    _cmplHandler->start();

    ui->cmplOutPut->setPlainText("");
    ui->cmplSolution->setPlainText("");
    ui->clpProblemTab->setCurrentIndex(1);
    ui->cmplMessages->clear();
    _cmplMessages.clear();
    ui->clpMessageTab->setVisible(false);
}

void ColiopUi::on_actionCancel()
{
    _cmplHandler->cancelProblem();
    qDebug() << "### isrunning nach cancelProblem";

    ui->actionSolve->setEnabled(true);
    ui->actionCancel->setEnabled(false);
    ui->cmplOutPut->appendPlainText("\nCmpl has been canceled");
    ui->cmplMessages->addItem("Cmpl has been canceled");
}

void ColiopUi::on_cmplFinished()
{
    ui->actionSolve->setEnabled(true);
    ui->actionCancel->setEnabled(false);

    if (_cmplHandler->getGeneralStatus()=="normal")  {
        if (!_cmplHandler->isSolution())  {
            ui->clpProblemTab->setCurrentIndex(1);
            if  (!_noOutput) {
                ui->clpMessageTab->setVisible(true);
                ui->cmplMessages->addItem("No solution found");
            }
        } else {
            ui->clpProblemTab->setCurrentIndex(2);
        }
    }

    if (_cmplHandler->getGeneralStatus()=="error") {
        ui->clpMessageTab->setVisible(true);
        QList <int> horizontalSplitterSize;
        horizontalSplitterSize.append(round(0.7*ui->clpUiSplitter->height()));
        horizontalSplitterSize.append(ui->clpUiSplitter->height()-round(0.7*ui->clpUiSplitter->height()));
        ui->clpUiSplitter->setSizes(horizontalSplitterSize);

        ui->clpProblemTab->setCurrentIndex(0);
        ui->cmplMessages->addItem(_cmplHandler->getStatusMessage());

        if (_cmplHandler->getNrOfMessages()>0) {
            _cmplMessages=_cmplHandler->getCmplMessages();
        }

        for (int i=0; i<_cmplMessages.length(); i++) {
            ui->cmplMessages->addItem(_cmplMessages.at(i).type+" in file <"+_cmplMessages.at(i).file+"> at line "+QString::number(_cmplMessages.at(i).line)+" : "+_cmplMessages.at(i).description);
        }
    }

    delete _cmplHandler;
}

void ColiopUi::on_cmplStarted()
{
    ui->actionSolve->setEnabled(false);
    ui->actionCancel->setEnabled(true);
    ui->clpProblemTab->setCurrentIndex(1);
}

void ColiopUi::on_cmplError()
{
    ui->actionSolve->setEnabled(true);
    ui->actionCancel->setEnabled(false);
    clpWarning("ERROR", _cmplHandler->getError()) ;
   // ui->clpMessageTab->setVisible(true);
   // ui->cmplMessages->addItem( _cmplHandler->getError());
    _cmplHandler->cancelProblem();
   // delete _cmplHandler;
}

void ColiopUi::on_cmplCanceled()
{
    qDebug() << "### isrunning in cmplCanceled";
    delete _cmplHandler;
    ui->actionSolve->setEnabled(true);
    ui->actionCancel->setEnabled(false);
    ui->cmplOutPut->appendPlainText("\nCmpl has been canceled");
    ui->cmplMessages->addItem("Cmpl has been canceled");
}

void ColiopUi::on_cmplHelp()
{

    QString runningOs = QSysInfo::productType();
    QString runProg;
    QString progPath = QCoreApplication::applicationDirPath();

    if (runningOs == "windows" ) {
        runProg = "\""+progPath+"/manual.bat\"";
    } else if (runningOs == "osx" ) {
         runProg = "\""+progPath+"/../../../manual\"";
    } else  {
        runProg = "\""+progPath+"/manual\"";
    }

    _cmplManualHandler = new QProcess(this);
    _cmplManualHandler->start(runProg);

    if (!_cmplManualHandler->waitForStarted()) {

    }
}

void ColiopUi::on_actionOpen_Problem()
{
    QString fileName ;

    fileName = QFileDialog::getOpenFileName(this,  "Open File", "", "CMPL Files (*.cmpl )");

    if (!fileName.isEmpty()) {
        problem->setProblem(fileName);
        openProblem(fileName);

        problem->setProblemChanged(false);
        enableSaveActions(false);
        ui->cmplMessages->clear();
        ui->cmplOutPut->setPlainText("");
        ui->cmplSolution->setPlainText("");
        ui->clpMessageTab->setVisible(false);


        problem->setActiveCmplIndex(0);

        setWindowTitle("Coliop4 - "+problem->getProblemName());
        ui->clpProblemTab->setCurrentIndex(0);
        ui->actionSolve->setEnabled(true);
    }
}

void ColiopUi::openProblem( QString &fileName ){

    if (!fileName.isEmpty()) {
        QFile file(fileName);
        if (file.open(QFile::ReadOnly | QFile::Text)) {

            ui->cmplEditor->setPlainText(file.readAll());
            file.close();

            problem->setProblemChanged(false);
            enableEditorActions(true);
            enableSaveActions(false);

        } else {
           clpWarning("ERROR", "Cannot find file <"+fileName+">") ;
        }
    }
}

bool ColiopUi::saveProblem(QString &fileName) {
    QFile cmplFile (fileName);
    QTextStream cmplS(&cmplFile);
    if (!cmplFile.open(QIODevice::Truncate | QIODevice::Text | QIODevice::ReadWrite)) {
            clpWarning("ERROR", "Cannot save file <"+fileName+">") ;
            return false;
    }
    cmplS << ui->cmplEditor->toPlainText();
    cmplFile.close();
    problem->setProblemChanged(false);
    enableSaveActions(false);
    return true;
}

void ColiopUi::gotoLine(int lineNumber) {

      const QTextBlock &block = ui->cmplEditor->document()->findBlockByNumber(lineNumber-1);
      QTextCursor cursor(block);
      cursor.movePosition(QTextCursor::Right, QTextCursor::MoveAnchor, 0);
      cursor.movePosition(QTextCursor::EndOfLine, QTextCursor::KeepAnchor );
      ui->cmplEditor->setTextCursor(cursor);
      ui->cmplEditor->centerCursor();
      ui->cmplEditor->setFocus();

}

void ColiopUi::on_actionAbout()
{
    coliopAbout->show();
}

void ColiopUi::on_actionQuit()
{
    QString sName;

    if (problem->isProblemChanged() && !ui->cmplEditor->toPlainText().trimmed().isEmpty()) {
        if (problem->getProblemName().isEmpty()) {
            if (clpAnswerMessage("Save file", "New Problem is changed. \n\nDo you want to save it?") == true) {
               sName  = QFileDialog::getSaveFileName(this,  "Save File", "", "CMPL or CMPLData Files (*.cmpl *.cdat )");
               if (!sName.isEmpty()) {
                    saveProblem( sName );
               }
            }
        } else {
            sName = problem->getProblemPath()+"/"+problem->getCmplListElement(problem->getActiveCmplIndex());
            if (clpAnswerMessage("Save file", "File <"+sName+"> is changed. \n\nDo you want to save it?") == true) {
                saveProblem(sName);
            }
        }
    }
    this->close();
}


void ColiopUi::on_actionSave_Problem()
{   QString fn;
    if (problem->getProblemName().isEmpty()) {
        fn  = QFileDialog::getSaveFileName(this,  "Save File", "", "CMPL or CMPLData Files (*.cmpl *.cdat )");
    } else {
        fn = problem->getProblemPath()+"/"+problem->getCmplListElement(problem->getActiveCmplIndex());
    }
    if (!fn.isEmpty()) {
        saveProblem( fn );
        if (problem->getActiveCmplIndex()==0) {
            problem->setProblem(fn);
            setWindowTitle("Coliop4 - "+problem->getProblemName());
            ui->clpProblemTab->setCurrentIndex(0);
            ui->actionSolve->setEnabled(true);
        }
    }
}

void ColiopUi::on_actionSave_Problem_as()
{   QString fn  = QFileDialog::getSaveFileName(this,  "Save File", "", "CMPL or CMPLData Files (*.cmpl *.cdat )");
    if (!fn.isEmpty()) {
        saveProblem( fn );
        if (problem->getActiveCmplIndex()==0) {
            problem->setProblem(fn);
            setWindowTitle("Coliop4 - "+problem->getProblemName());
            ui->clpProblemTab->setCurrentIndex(0);
            ui->actionSolve->setEnabled(true);
        }
    }
}


void ColiopUi::on_actionGo_to_line()
{
    QTextCursor cursor = ui->cmplEditor->textCursor();
    int lineNumber = cursor.blockNumber();
    int blockCount = ui->cmplEditor->blockCount();
    bool ok;
    int line = QInputDialog::getInt(this,"Line number", "Enter a line number"  , lineNumber,1,blockCount ,1,&ok);
    if (ok)
        gotoLine(line);
}

void ColiopUi::on_actionZoom_in()
{
    int curPointSize=ui->cmplEditor->fontInfo().pointSize();
    QString curFontFamily = ui->cmplEditor->fontInfo().family();
    QFont font;
    font.setPointSize(curPointSize+1);
    font.setFamily(curFontFamily);
    ui->cmplEditor->setFont(font);
    ui->cmplOutPut->setFont(font);
    ui->cmplSolution->setFont(font);
    ui->cmplMessages->setFont(font);
    ui->cmplList->setFont(font);

}

void ColiopUi::on_actionZoom_out()
{
    int curPointSize=ui->cmplEditor->fontInfo().pointSize();
    QString curFontFamily = ui->cmplEditor->fontInfo().family();
    QFont font;
    font.setPointSize(curPointSize-1);
    font.setFamily(curFontFamily);
    ui->cmplEditor->setFont(font);
    ui->cmplOutPut->setFont(font);
    ui->cmplSolution->setFont(font);
    ui->cmplMessages->setFont(font);
    ui->cmplList->setFont(font);

}

void ColiopUi::enableEditorActions(bool enable) {

    if (enable) {
        ui->actionCopy->setEnabled(true);
        ui->actionCut->setEnabled(true);
        ui->actionPaste->setEnabled(true);
        ui->actionUndo->setEnabled(true);
        ui->actionRedo->setEnabled(true);
        if (!ui->cmplEditor->toPlainText().isEmpty()) {
            ui->actionPrint->setEnabled(true);
        } else {
            ui->actionPrint->setEnabled(false);
        }

    } else {
        ui->actionCopy->setDisabled(false);
        ui->actionCut->setDisabled(true);
        ui->actionPaste->setDisabled(true);
        ui->actionUndo->setDisabled(true);
        ui->actionRedo->setDisabled(true);

        if ( ui->clpProblemTab->currentIndex()==1) {
            if (!ui->cmplOutPut->toPlainText().isEmpty()) {
                ui->actionPrint->setEnabled(true);
            } else {
                ui->actionPrint->setEnabled(false);
            }
        } else  if ( ui->clpProblemTab->currentIndex()==2) {
            if (!ui->cmplSolution->toPlainText().isEmpty()) {
                ui->actionPrint->setEnabled(true);
            } else {
                ui->actionPrint->setEnabled(false);
            }
        }
    }
}


void ColiopUi::on_clpProblemTab_currentChanged()
{
    if ( ui->clpProblemTab->currentIndex()==0)
        enableEditorActions(true);
    else
        enableEditorActions(false);

}


void ColiopUi::enableSaveActions(bool enable){
     if (enable) {
         if (!ui->cmplEditor->document()->isEmpty()) {
            ui->actionSave_Problem->setEnabled(true);
            ui->actionSave_Problem_as->setEnabled(true);
         }
     }
     else {
         ui->actionSave_Problem->setDisabled(true);
         //ui->actionSave_Problem_as->setDisabled(true);
     }
}


void ColiopUi::cmplList() {

    if (!problem->getProblemName().isEmpty() && problem->getActiveCmplIndex()==0) {
        problem->clearCmplList();
        ui->cmplList->clear();

        problem->addCmplList(problem->getProblemName());

        QString actCmplCode =  ui->cmplEditor->document()->toPlainText();

        QString cap="";
        QRegExp rx("%data(\\s*)[\:\n\r]{1,}");
        if(rx.indexIn(actCmplCode, 0) != -1) {
             problem->addCmplList(problem->getProblemBaseName()+".cdat");
        }

        //rx.setPattern("%data\\s*([a-zA-Z\-_\\\\./]*)\\s*[\:\n\r]{1,}");
        rx.setPattern("%data([ a-zA-Z\-_\\\\./]*)[\:\n\r]{1,}");
        //rx.setPattern("%data\\s*([a-zA-Z\-_\\\\./]*)\\s*[\:\n\r]{1,}");

        int pos = 0;
        while ((pos = rx.indexIn(actCmplCode, pos)) != -1) {
            cap=rx.cap(1).trimmed();
            if (!cap.isEmpty()) {
               problem->addCmplList(cap);
            }
            pos += rx.matchedLength();
        }

        rx.setPattern("include\\s*\\(\"([a-zA-Z\-_\\\\./]*)\"\\)");
        pos = 0;
        while ((pos = rx.indexIn(actCmplCode, pos)) != -1) {
            problem->addCmplList( rx.cap(1) );
            pos += rx.matchedLength();
        }

        rx.setPattern("readcsv\\s*\\(\"([a-zA-Z\-_\\\\./]*)\"\\)");
        pos = 0;
        while ((pos = rx.indexIn(actCmplCode, pos)) != -1) {
            problem->addCmplList( rx.cap(1) );
            pos += rx.matchedLength();
        }

        rx.setPattern("%arg(\\s*)-noOutput");
        if(rx.indexIn(actCmplCode, 0) != -1) {
             _noOutput=true;
        } else {
            _noOutput=false;
        }

    }
}


bool ColiopUi::clpAnswerMessage(QString title, QString msg)
{   bool ret;
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, title, msg,QMessageBox::Yes|QMessageBox::No);
      if (reply == QMessageBox::Yes) {
        ret= true;
      } else {
        ret = false;
      }
    return ret;

}

void ColiopUi::clpWarning(QString title, QString msg) {
    QMessageBox::critical(this, title , msg ,QMessageBox::Ok);
}

void ColiopUi::on_cmplEditorChanged()
{
    enableSaveActions(true);
    problem->setProblemChanged(true);

    if (!problem->getProblemName().isEmpty() && problem->getActiveCmplIndex()==0) {

        cmplList();
        if (problem->cmplListLength()>1) {
            for (int i=0; i< problem->cmplListLength(); i++)
                 ui->cmplList->addItem(problem->getCmplListElement(i));

            ui->cmplList->show();

            QList <int> splitterSizeList;
            splitterSizeList.append(round(0.75*ui->splitter->width()));
            splitterSizeList.append(ui->splitter->width()-round(0.75*ui->splitter->width()));
            ui->splitter->setSizes(splitterSizeList);

        } else {
            ui->cmplList->hide();
        }
    }
}


void ColiopUi::on_actionUndo()
{
    ui->cmplEditor->undo();
}

void ColiopUi::on_actionRedo()
{
     ui->cmplEditor->redo();
}

void ColiopUi::on_actionCopy()
{
    if (ui->cmplEditor->hasFocus())
        ui->cmplEditor->copy();
    else if (ui->cmplOutPut->hasFocus())
        ui->cmplOutPut->copy();
    else if (ui->cmplSolution->hasFocus())
        ui->cmplSolution->copy();

}

void ColiopUi::on_actionCut()
{
     ui->cmplEditor->cut();
}

void ColiopUi::on_actionPaste()
{
     ui->cmplEditor->paste();
}

void ColiopUi::on_actionPrint()
{
    QPrinter printer;
    QPrintDialog *dialog = new QPrintDialog(&printer, this);
    dialog->setWindowTitle(tr("Print Document"));
    if (dialog->exec() == QDialog::Accepted) {
        int tab = ui->clpProblemTab->currentIndex();
        switch (tab) {
        case 0:
            ui->cmplEditor->print(&printer);
            break;
        case 1:
            ui->cmplOutPut->print(&printer);
            break;
        case 2:
            ui->cmplSolution->print(&printer);
            break;
        default:
            break;
        }
    }
}

void ColiopUi::on_cmplListClicked()
{
    int actRow =ui->cmplList->currentRow();

    if (actRow!=problem->getActiveCmplIndex()) {
        if (problem->isProblemChanged()) {
            QString sName = problem->getProblemPath()+"/"+problem->getCmplListElement(problem->getActiveCmplIndex());
            if (clpAnswerMessage("Save file", "File <"+sName+"> is changed. \n\nDo you want to save it?") == true) {
                saveProblem(sName);
              }
        }
        QString fName = problem->getProblemPath()+"/"+problem->getCmplListElement(actRow);
        QFile file(fName);
        if (!file.exists()) {
            if (clpAnswerMessage("Save file", "File <"+fName+"> does not exist. \n\nDo you want to create it?") == true) {
                if (!file.open(QIODevice::Truncate | QIODevice::Text | QIODevice::ReadWrite)) {
                    clpWarning("Error","Cannot create file <"+fName+">");
                    actRow=problem->getActiveCmplIndex();
                    ui->cmplList->item(actRow)->setSelected(true);

                } else {
                    file.close();
                    problem->setActiveCmplIndex(actRow);
                    openProblem(fName);
                }

             } else {
                actRow=problem->getActiveCmplIndex();
                ui->cmplList->item(actRow)->setSelected(true);
                problem->setActiveCmplIndex(actRow);
            }
        } else {
                problem->setActiveCmplIndex(actRow);
                openProblem(fName);
        }
    }

    if (problem->getActiveCmplIndex()==0) {
        ui->actionSave_Problem_as->setEnabled(true);

    } else {
        ui->actionSave_Problem_as->setDisabled(true);
    }

}

void ColiopUi::on_cmplMessageListClicked()
{
    int actRow =ui->cmplMessages->currentRow()-1;
    int actCmplIdx =0;

    if (actRow>=0) {
        QString fileName = _cmplMessages.at(actRow).file;

        for (int i=0; i<problem->cmplListLength(); i++ ) {
            if (problem->getCmplListElement(i).contains(fileName)) {
                fileName=problem->getProblemPath()+"/"+problem->getCmplListElement(i);
                actCmplIdx=i;
                break;
            }
        }

        ui->cmplList->setCurrentRow(actCmplIdx);
        problem->setActiveCmplIndex(actCmplIdx);

        openProblem(fileName);

        ui->cmplList->setCurrentRow(actCmplIdx);
        problem->setActiveCmplIndex(actCmplIdx);

        ui->clpProblemTab->setCurrentIndex(0);
        gotoLine(_cmplMessages.at(actRow).line);
    }

}

