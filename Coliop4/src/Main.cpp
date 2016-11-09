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

#include <QtWidgets/QApplication>
#include <QDesktopWidget>
#include "ColiopUi.hh"

//#include <QMessageBox>
//#include <QString>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QStringList args =  QGuiApplication::arguments();

    ColiopUi w;
    w.show();
    QRect rec = QApplication::desktop()->screenGeometry();
    if (rec.width()<1200 || rec.height()<750)
        w.showMaximized();

    if (QGuiApplication::platformName()!="cocoa") {
        if (argc>1 && args.at(1).toLower().endsWith(".cmpl")) {
            QString fName= args.at(1);
            w.openFirstProblem(fName);
        }
    }


    return a.exec();
}
