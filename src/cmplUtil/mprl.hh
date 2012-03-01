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



#ifndef MPRL_H
#define MPRL_H

#include <QList>
#include <QString>
#include <QFile>
#include <QDomDocument>

#include <string>
#include <iostream>
#include <iomanip>
using namespace std;



namespace cmplUtil {

    /**
     * <code>mprl</code> implements the handling of mprl files
     * A mprl (Mathematical Programming language Result Language
     * cosists of the general status and the messages of CMPL
     *
     * @author  Mike Steglich
     */

    typedef struct
    {
        QString type;
        QString file;
        int line;
        QString description;

    } mprlStructure;


    class mprl
    {
    public:
        /**
         * constructor
         */
        mprl();

        /**
         * destructor
         */
        virtual ~mprl();

        /**
         * Sets the mprl file name
         *
         * <p>
         * @param fName  file name of the mprl file
         * </p>
         */
        void setMprl(const QString &fName);

        /**
         * Reads the content of a MPRL XML file in a structure
         *
         * @return  true|false
         */
        bool readMPrL();

        /**
         * performs the a standard report
         */
        void standardReport();

        /**
         * @return  the general status
         */
        string generalStatus();

        /**
         * @return  the mpl name - here CMPL
         */
        string mplName();

        /**
         * @return  the instance name xxx.cmpl
         */
        string instanceName();

        /**
         * @return  the general message
         */
        string generalMessage();

        /**
         * @return number of the message
         */
        int numberOfMessages();

        /**
         * @return  the general type
         * @param number of the message
         */
        string messageType(int msgNr);

        /**
         * @return  the file in that the error/warning occurs
         * @param number of the message
         */
        string messageFile(int msgNr);

        /**
         * @return  the line in the the file in that the error/warning occurs
         * @param number of the message
         */
        int messageLine(int msgNr);

        /**
         * @return  the message text
         * @param number of the message
         */
        string messageDescribtion(int msgNr);

        /**
         * @return  the text of the current MPRL error
         */
        QString mprlError();

    private:
        QList<mprlStructure> msg;

        QString  mprlGeneralStatus;
        QString  mprlMplName;
        QString  mprlInstanceName;
        QString  mprlMessage;
        int      mprlNumberOfMessages;
        QString  mprlFile;

        QString mprlErrorMsg;

    };

}

#endif // MPRL_H


