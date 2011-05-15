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


#include "mprl.hh"

/* **************** mprl ************* */
mprl::mprl()
{

    mprlGeneralStatus="";
    mprlMplName="";
    mprlInstanceName="";
    mprlMessage="";
    mprlNumberOfMessages=0;
    mprlFile = "";

    mprlErrorMsg="";

}//end mprl


/* **************** ~mprl ************* */
mprl::~mprl(){
   msg.clear();
}//end

/* **************** setMprl ************* */
void mprl::setMprl(const QString &fName){
    mprlFile = fName;
}//end setMprl


/* **************** readMPrL ************* */
bool mprl::readMPrL(){

    if (mprlFile.isEmpty()) {
        mprlErrorMsg="mprl file is not set" ;
        return false;
    }

    mprlStructure msgTmp;

    QDomDocument doc("myMprl");
    QFile file(mprlFile);
    if (!file.open(QIODevice::ReadOnly)) {
        mprlErrorMsg="File could not be open >>" + mprlFile ;
        return false;
    }
    if (!doc.setContent(&file)) {
        file.close();
        mprlErrorMsg="xmpl error >>" + mprlFile ;
        return false;
    }
    file.close();

   QDomElement docElem = doc.documentElement();

   if ( (docElem.tagName() != "mprl") && (docElem.tagName() != "mplr") ) {
        mprlErrorMsg="xmpl file >>" + mprlFile  + " is not a mprl file";
        return false;
    }

    QDomNode child1 = docElem.firstChild();
    while(!child1.isNull()) {

        QDomElement elemChild1 = child1.toElement();

        if(!elemChild1.isNull()) {

            QDomNode child2 = child1.firstChild();
            if ( elemChild1.tagName()=="general" ) {

                 while(!child2.isNull()) {
                      QDomElement elemChild2 = child2.toElement(); // try to convert the node to an element.
                      if (elemChild2.tagName()=="generalStatus")  mprlGeneralStatus=elemChild2.text();
                      if (elemChild2.tagName()=="mplName") mprlMplName=elemChild2.text();
                      if (elemChild2.tagName()=="instanceName") mprlInstanceName=elemChild2.text();
                      if (elemChild2.tagName()=="message")   mprlMessage=elemChild2.text();
                      child2=child2.nextSibling();
                  }
            }

            if ( elemChild1.tagName()=="mplResult" ) {

                mprlNumberOfMessages=elemChild1.attribute("numberOfMessages").toInt();

                while(!child2.isNull()) {

                    QDomElement elemChild2 = child2.toElement();

                    msgTmp.type=elemChild2.attribute("type");
                    msgTmp.file=elemChild2.attribute("file");
                    msgTmp.line=elemChild2.attribute("line").toInt();
                    msgTmp.description=elemChild2.attribute("description");
                    msg.append(msgTmp);

                    child2=child2.nextSibling();
                  }
            }
        }
        child1 = child1.nextSibling();
    }
    return true;
}//end readMPrL


/* **************** standardReport ************* */
void mprl::standardReport() {

    cout << setw(10) << left << "CMPL failed " << endl;
    cout << setw(20) << left << "General status: " << generalStatus()<< endl;
    cout << setw(20) << left << "Problem: " << instanceName() << endl;
    cout << setw(20) << left << "General Message: " << generalMessage()  << endl;
    cout << setw(20) << left << "Number of Messages: " << numberOfMessages() << endl;

    cout << endl;
    cout << setw(10) << left <<  "Type";
    cout << setw(30) << left <<  "File";
    cout << setw(5) << left <<   "Line";
    cout << setw(75) << left <<  "Description" << endl;
    cout << setw(120) << setfill('-') << "" <<endl;
    cout <<  setfill(' ');

    for (int i=0; i < numberOfMessages() ; i++)   {

             cout << setw(10) << left <<  messageType(i);
             cout << setw(30) << left << messageFile(i);
             cout << setw(5) << left << messageLine(i);
             cout << setw(75) << left << messageDescribtion(i);

             cout << endl;
    }
    cout << setw(120) << setfill('-') << "" <<endl;
}//end standardReport


/* **************** generalStatus ************* */
string mprl::generalStatus() {
    return mprlGeneralStatus.toStdString();
}//end generalStatus


/* **************** mplName ************* */
string mprl::mplName() {
    return mprlMplName.toStdString();
}//end mplName


/* **************** instanceName ************* */
string mprl::instanceName(){
    return mprlInstanceName.toStdString();
}//end instanceName


/* **************** generalMessage ************* */
string mprl::generalMessage(){
    return mprlMessage.toStdString();
}//end generalMessage


/* **************** numberOfMessages ************* */
int mprl::numberOfMessages(){
    return mprlNumberOfMessages;
}//end numberOfMessages


/* **************** messageType ************* */
string mprl::messageType(int msgNr) {
    if (msgNr>mprlNumberOfMessages) return "Index out of range";
    else return msg[msgNr].type.toStdString();
}//end messageType


/* **************** messageFile ************* */
string mprl::messageFile(int msgNr) {
    if (msgNr>mprlNumberOfMessages) return "Index out of range";
    else return msg[msgNr].file.toStdString();
}//end messageFile


/* **************** messageLine ************* */
int mprl::messageLine(int msgNr) {
    if (msgNr>mprlNumberOfMessages) return -99;
    else return msg[msgNr].line;
}//end messageLine


/* **************** messageDescribtion ************* */
string mprl::messageDescribtion(int msgNr) {
    if (msgNr>mprlNumberOfMessages) return "Index out of range";
    else return msg[msgNr].description.toStdString();
}//end messageDescribtion


/* **************** mprlError ************* */
QString mprl::mprlError() {
    return mprlErrorMsg;
}//end mprlError
