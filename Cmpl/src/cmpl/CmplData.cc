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


#include "CmplData.hh"
#include "CmplArgs.hh"




namespace cmpl {



    /* ********* CmplData ********* */
    CmplData::CmplData(cmplUtil::CmplArgs *a)
    {
        args=a;
        _isOpenFiles=true;
        _errorMsg="";

       if (!args->checkOnlySyntax()) {
            if (!openDataFiles())
                 _isOpenFiles=false;
        }

    }//end CmplData


    /* ********* ~CmplData ********* */
    CmplData::~CmplData() {

        if (!args->checkOnlySyntax())
            closeDataFiles();

    }//end ~CmplData

    /* ********* insertElementsForSyntaxCheck ********* */
    bool CmplData::insertElementsForSyntaxCheck(Compiler *comp) {

        CmplCore *glvar=comp->glvar;
        int setRank;

        int symNr;
        enum etp elementType;
        int elementTypeDim;

        dataElement tmpElement;

        for (int i=0; i<glvar->args->dataFileListSize(); i++ ) {

            for (int j=0; j<glvar->args->dataFileNrOfElements(i); j++) {

                tmpElement.name=glvar->args->dataFileElement(i,j);
                tmpElement.symNr=-1;
                tmpElement.filePos=-1;
                tmpElement.isDefault=false;
                tmpElement.isIndices=false;
                tmpElement.isSet=false;
                tmpElement.setRank=0;
                tmpElement.setSymNr.clear();

                if (glvar->args->dataFileElementIsSet(i,j)) {

                    setRank=getSetRank(glvar->args->dataFileElementSetRankString(i,j));
                    if (setRank==-99) {
                        setErrorMsg(comp, glvar->args->cmplFile() , 0, "Irregular rank " + glvar->args->dataFileElementArraySetString(i,j) + " of the set: "+tmpElement.name);
                        return false;
                    }

                    if (setRank==0)
                       elementType=TPK_SET;
                    else
                       elementType=TPK_TUPELSET_N(setRank);

                    elementTypeDim=0;
                    tmpElement.setRank=setRank;
                    tmpElement.isSet=true;

                } else {
                    elementType=TPK_VALUE;
                    elementTypeDim=getArrayDim(glvar->args->dataFileElementArraySetString(i,j), tmpElement.setSymNr);
                    if (elementTypeDim==-99) {
                        setErrorMsg(comp, glvar->args->cmplFile() , 0, _errorMsg + tmpElement.name  );
                        return false;
                    }
                    tmpElement.isSet=false;
                    tmpElement.setRank=0;
                }

                symNr=comp->insertCmplDataSymbol(tmpElement.name.c_str(), elementType, elementTypeDim);
                tmpElement.symNr=symNr;

                _dataElements.push_back(tmpElement);

            }
        }
        return true;
    }// end insertElementsForSyntaxCheck

    /* ********* cmplDataCompiler ********* */
    bool CmplData::cmplDataCompiler(Compiler *comp) {


        if (comp->glvar->args->checkOnlySyntax())
            return insertElementsForSyntaxCheck(comp);

        string line;
        int lineNr=0;
        streampos filePos=0;
        bool allElements;

        int setRank;

        int symNr;
        enum etp elementType;
        int elementTypeDim;

        size_t pos,pos1;

        bool nameSection;
        bool arraySection;
        bool defaultSection;
        bool setSection;
        bool dataSection;

        bool activeElement;

        string tmpName;
        string tmpArray;
        string tmpDefault;
        string tmpSet;

        dataElement tmpElement;

        CmplCore *glvar=comp->glvar;


        for (int i=0; i<args->dataFileListSize(); i++ ) {

            // ifstream cdatFile (args->dataFileName(i).c_str(), ifstream::binary );
            //  filePos=cdatFile.tellg();
            filePos=args->dataFile(i)->tellg();

            if (args->dataFileNrOfElements(i)>0)
                allElements=false;
            else
                allElements=true;

            dataSection=false;

            tmpElement.name="";
            tmpElement.symNr=-1;
            tmpElement.filePos=-1;
            tmpElement.isDefault=false;
            tmpElement.isIndices=false;
            tmpElement.isSet=false;
            tmpElement.setRank=0;
            tmpElement.setSymNr.clear();


            dataSection=false;
            lineNr=0;
            line="";

            while ( getline( *args->dataFile(i), line) ) {
                //  while ( getline( cdatFile, line) ) {

                lineNr++;
                line = CmplTools::lTrim(line);


                 if (CmplTools::stringStartsWith(line,"#"))
                     continue;

                 if (!dataSection && !line.empty() && !CmplTools::stringStartsWith(line,"%")) {
                     setErrorMsg(comp, args->dataFileName(i) , lineNr ,"Missing % : The definition of a set or parameter has to Start with \"%\" ");
                     return false;
                 }

                 if (CmplTools::stringStartsWith(line,"%")) {

                     if (dataSection) {
                         setErrorMsg(comp, args->dataFileName(i) , lineNr ,"Missing > : Incomplete data section");
                         return false;
                     }

                     nameSection=true;
                     arraySection=false;
                     defaultSection=false;
                     setSection=false;
                     dataSection=true;

                     activeElement=false;

                     tmpName="";
                     tmpArray="";
                     tmpSet="";
                     tmpDefault="";

                     tmpElement.name="";
                     tmpElement.symNr=-1;
                     tmpElement.filePos=-1;
                     tmpElement.isDefault=false;
                     tmpElement.isIndices=false;
                     tmpElement.isSet=false;
                     tmpElement.setRank=0;
                     tmpElement.setSymNr.clear();

                     if ((pos=line.find("<"))!=string::npos) {
                         if (CmplTools::stringContains(line,">"))
                             dataSection=false;
                         line=line.erase(pos);
                     }


                    istringstream iss(line);

                    while (iss) {
                         string subs;
                         iss >>  subs;

                         if (!subs.empty() ) {

                             CmplTools::cleanDoubleQuotes(subs);

                             if (nameSection) {
                                 tmpName=subs.substr(1);

                                 if ((pos=tmpName.find("["))!=string::npos) {
                                     if((pos1=tmpName.find("]"))!=string::npos) {
                                         tmpArray=tmpName.substr(pos+1,pos1-pos-1);
                                         arraySection=false;
                                     } else {
                                         arraySection=true;
                                         tmpArray=tmpName.substr(pos+1);
                                     }

                                     tmpName=tmpName.substr(0,pos);
                                 }

                                 if (allElements || (!allElements && activeDataElement(i,tmpName)))
                                     activeElement=true;
                                 else
                                     break;

                                 nameSection=false;
                                 continue;
                             }


                             if (CmplTools::stringStartsWith(subs,"[")) {

                                 if (tmpElement.isSet) {
                                     setSection=true;
                                     if((pos1=subs.find("]"))!=string::npos)
                                         tmpSet=subs.substr(1,pos1-1);
                                     else
                                         tmpSet=subs.substr(1);

                                 } else {
                                     if((pos=subs.find("]"))!=string::npos) {
                                         tmpArray=subs.substr(1,pos-1);
                                         arraySection=false;
                                     } else {
                                         arraySection=true;
                                         tmpArray=subs.substr(1);
                                     }
                                 }
                                 continue;
                             }

                             if (CmplTools::stringStartsWith(subs,"=")) {
                                 tmpElement.isDefault=true;
                                 if (subs.length()>1) {
                                     tmpDefault=subs.substr(1);

                                     stringToValStr(tmpDefault, tmpElement.defaultVal);
                                     defaultSection=false;

                                 }
                                 else
                                     defaultSection=true;
                                 continue;
                             }

                             if (CmplTools::stringStartsWith(subs,"indices")) {
                                 arraySection=false;
                                 defaultSection=false;
                                 tmpElement.isIndices=true;
                                 continue;
                             }

                             if (CmplTools::stringStartsWith(subs,"set")) {
                                 arraySection=false;
                                 defaultSection=false;
                                 tmpElement.isSet=true;
                                 setSection=false;
                                 tmpSet=subs.substr(3);

                                 if ((pos=tmpSet.find("["))!=string::npos) {
                                     if((pos1=tmpSet.find("]"))!=string::npos) {
                                         tmpSet=tmpSet.substr(pos+1,pos1-pos-1);
                                         setSection=false;
                                     } else
                                         setSection=true;
                                 }
                                 continue;
                             }

                             if (arraySection) {
                                 if((pos=subs.find("]"))!=string::npos) {
                                     tmpArray=tmpArray+subs.substr(0,pos);
                                     arraySection=false;
                                 } else
                                     tmpArray+=subs;
                                 continue;
                             }

                             if (defaultSection) {
                                tmpDefault=subs;
                                stringToValStr(tmpDefault, tmpElement.defaultVal);
                                continue;
                             }

                             if (setSection) {
                                 if((pos=subs.find("]"))!=string::npos) {
                                     tmpSet=tmpSet+subs.substr(0,pos-1);
                                     setSection=false;
                                 } else
                                     tmpSet+=subs;
                                 continue;
                             }
                         }
                     }



                     if (activeElement) {
                         tmpElement.name=tmpName;
                         tmpElement.filePos=filePos;
                         tmpElement.fileNr=i;
                         tmpElement.lineNr=lineNr;


                         if (tmpElement.isSet) {

                             setRank=getSetRank(tmpSet);
                             if (setRank==-99) {
                                 setErrorMsg(comp, args->dataFileName(i) , lineNr, "Irregular rank >>" + tmpSet + "<< of the set: "+tmpName);
                                 return false;
                             }

                             if (!checkRankDef(i,tmpName,setRank)) {
                                 setErrorMsg(comp, args->dataFileName(i) , lineNr, "Different rank definition in the CMPL file and the cmplData file for the set >> "+tmpName);
                                 return false;
                             }

                             if (setRank==0)
                                elementType=TPK_SET;
                             else
                                elementType=TPK_TUPELSET_N(setRank);

                             elementTypeDim=0;
                             tmpElement.setRank=setRank;

                         } else {
                             elementType=TPK_VALUE;
                             CmplTools::cleanStringFromWhiteSpaces(tmpArray);
                             elementTypeDim=getArrayDim(tmpArray, tmpElement.setSymNr);

                             if (elementTypeDim==-99) {
                                 setErrorMsg(comp, args->dataFileName(i) , lineNr, _errorMsg + tmpName );
                                 return false;
                             }

                             if (elementTypeDim>0 && !checkArraySetDef(i,tmpName,tmpArray)) {
                                 setErrorMsg(comp, args->dataFileName(i) , lineNr, "Different set definition in the CMPL file and the cmplData file for the array >> "+tmpName);
                                 return false;
                            }

                             tmpElement.isSet=false;
                             tmpElement.setRank=0;
                         }

                        symNr=comp->insertCmplDataSymbol(tmpName.c_str(), elementType, elementTypeDim);
                         tmpElement.symNr=symNr;

                         _dataElements.push_back(tmpElement);

                         if( glvar->is_protokoll) {
                             PROTO_OUT_2("\nCmplData insert element: %s " , tmpElement.name.c_str() );
                             PROTO_OUT_2("\n\tSymNr: %d  " , tmpElement.symNr );
                             PROTO_OUT_2("\n\tDimension: %d" , elementTypeDim  );
                             PROTO_OUT_2("\n\tisDefault: %d " ,  tmpElement.isDefault ) ;
                             if (tmpElement.isDefault  ) {
                                 if (tmpElement.defaultVal.t==TP_STR) {
                                     PROTO_OUT_2(" String  %s", tmpElement.defaultVal.v.s);
                                 }
                                 else if ((tmpElement.defaultVal.t==TP_INT)) {
                                     PROTO_OUT_2(" Integer %d " ,tmpElement.defaultVal.v.i);
                                 }
                                 else {
                                     PROTO_OUT_2(" Real %d ", tmpElement.defaultVal.v.f);
                                 }
                             }
                             PROTO_OUT_2("\n\tisIndex %d" , tmpElement.isIndices );
                             PROTO_OUT_2("\n\tisSet %d " , tmpElement.isSet);
                             PROTO_OUT_2("\n\tsetRank %d ", tmpElement.setRank) ;
                             if (!tmpElement.isSet) {
                                 PROTO_OUT_1("\n\tsetNr ");
                                 for (int k=0; k< tmpElement.setSymNr.size(); k++) {
                                     PROTO_OUT_2(" %d " , tmpElement.setSymNr[k]) ;
                                 }

                             }
                          }
                     }
                 }

                 if (tmpElement.isDefault && !tmpElement.isIndices) {
                     setErrorMsg(comp, args->dataFileName(i) , lineNr, "It is not possible to define a default value for dense arrays. Please use indices for the array definition.");
                     return false;
                 }

                filePos=args->dataFile(i)->tellg();
                //filePos=cdatFile.tellg();

                 if (CmplTools::stringContains(line,">"))
                     dataSection=false;
             }

            if (dataSection) {
                setErrorMsg(comp, args->dataFileName(i) , lineNr ,"Missing > : Incomplete data section");
                return false;
            }

            string err="";
            if (!foundAllDataElements(i, err)) {
                setErrorMsg(comp, args->dataFileName(i) , lineNr, "Could not found the following elements" + err);
                return false;
            }


            //cdatFile.close();
        }
      return true;
    }//end cmplDataCompiler


    /* ********* cmplDataInterpreter ********* */
    bool CmplData::cmplDataInterpreter( IntFuncInitCmplData *intp)  {

        int fileNr;
        streampos filePos;
        string line;
        int lineNr;

        bool dataSection;
        bool lastElement;
        bool symbolSection;
        int pos,pos1;

        string tmpVal;
        val_str val;

        int elementCount;
        enum etp setType;
        int sets[MAX_ANZ_IND];

        string tmpStr;

        long tmpSetVal;

        bool isDefaultSet;

        CmplCore *glvar=intp->glvar;

        for (int i=0; i<_dataElements.size(); i++) {

            fileNr=_dataElements[i].fileNr;
            filePos=_dataElements[i].filePos;

            //ifstream cdatFile (args->dataFileName(fileNr).c_str(), ifstream::in );
            //cdatFile.clear();
            //cdatFile.seekg(filePos);

            args->dataFile(fileNr)->clear();
            args->dataFile(fileNr)->seekg(filePos);


            dataSection=false;
            lastElement=false;
            symbolSection=true;

            elementCount=0;
            tmpVal="";

            bool isAlgSet=false;
            bool isAlgSet1=false;

            isDefaultSet=false;

            PROTO_OUT_2("\nCmplData insert data for element %s ", _dataElements[i].name.c_str() );

            lineNr=_dataElements[i].lineNr-1;

            while ( getline( *args->dataFile(fileNr), line)  && !lastElement) {
            //while ( getline( cdatFile , line)  && !lastElement) {
                lineNr++;

                if (CmplTools::stringStartsWith(line,"#"))
                    continue;

                if (_dataElements[i].isSet ) {

                    if ((pos=line.find_first_of("<"))!=string::npos)
                        tmpStr=line.substr(pos+1);
                    else
                        tmpStr=line;

                    if ((pos=tmpStr.find_first_of(">"))!=string::npos)
                        tmpStr=tmpStr.substr(0, pos);

                    tmpStr=CmplTools::lrTrim(tmpStr);

                    tmpSetVal=-1;

                    if (CmplTools::stringContains(tmpStr,"(") && CmplTools::stringContains(tmpStr,")")) {

                        pos=tmpStr.find("(");
                        pos1=tmpStr.find(")");

                        for (int k=1; k<=3 ; k++) {

                            switch (k)  {
                            case 1:
                                tmpVal=tmpStr.substr(0,pos);
                                break;
                            case 2:
                                tmpVal=tmpStr.substr(pos+1,pos1-pos-1);
                                break;
                            case 3:
                                tmpVal=tmpStr.substr(pos1+1);
                                break;
                            }

                            if(!CmplTools::stringToLong(tmpVal, tmpSetVal)) {
                                setErrorMsg(intp, args->dataFileName(fileNr) , lineNr, "Wrong value: "+tmpVal);
                                return false;
                            } else {
                                intp->push_int(tmpSetVal);
                                elementCount++;
                            }
                        }

                        isAlgSet=true;

                    } if ( CmplTools::stringContains(tmpStr,"..") ) {

                        pos=tmpStr.find(".");

                        for (int k=1; k<=2 ; k++) {
                            switch (k)  {
                            case 1:
                                tmpVal=tmpStr.substr(0,pos);
                                break;
                            case 2:
                                tmpVal=tmpStr.substr(pos+2);
                                break;
                            }

                            if(!CmplTools::stringToLong(tmpVal, tmpSetVal)) {
                                setErrorMsg(intp, args->dataFileName(fileNr) , lineNr, "Wrong value: "+tmpVal);
                                return false;
                            } else {
                                intp->push_int(tmpSetVal);
                                elementCount++;
                            }
                        }

                        isAlgSet1=true;
                    }

                    if (isAlgSet || isAlgSet1)
                       break;

                }

                istringstream iss(line);
                while (iss) {
                    string subs, subs1;
                    iss >>  subs;

                    if (CmplTools::stringStartsWith(subs,"\"") && !CmplTools::stringEndsWith(subs,"\"") && !CmplTools::stringEndsWith(subs,">")  ) {
                        while (iss) {
                            iss >>  subs1;
                            subs+=" "+ subs1;
                            if (CmplTools::stringEndsWith(subs1,"\""))
                                break;
                        }
                    }

                    tmpVal=subs;

                    if ((pos=tmpVal.find("<"))!=string::npos) {
                        symbolSection=false;
                        dataSection=true;
                        tmpVal=tmpVal.substr(pos+1);
                    }
                    if ((pos=tmpVal.find(">"))!=string::npos) {
                        tmpVal=tmpVal.erase(pos);
                        if (tmpVal.length()==0) {
                            if ((_dataElements[i].isDefault && elementCount==1) || elementCount==0 ) {
                                setErrorMsg(intp, args->dataFileName(fileNr) , lineNr, "Wrong set or parameter: no data element defined");
                                return false;
                            }
                            dataSection=false;
                            lastElement=true;
                            break;
                        } else
                            lastElement=true;
                    }

                    CmplTools::cleanDoubleQuotes(tmpVal);

                    if (symbolSection && _dataElements[i].isDefault && !isDefaultSet) {
                        elementCount++;
                        isDefaultSet=true;
                        if (_dataElements[i].defaultVal.t==TP_STR) {
                            intp->push_string( _dataElements[i].defaultVal.v.s );
                        } else if ((_dataElements[i].defaultVal.t==TP_INT)) {
                            intp->push_int(_dataElements[i].defaultVal.v.i);
                        } else {
                            intp->push_dbl(_dataElements[i].defaultVal.v.f);
                        }

                    }

                    if (dataSection && CmplTools::lrTrim(tmpVal).length()>0) {
                        elementCount++;
                        if (lastElement)
                            dataSection=false;

                        stringToValStr(tmpVal, val);

                        PROTO_OUT_2("\n\telement count %d",elementCount);

                        if (val.t==TP_STR) {
                            PROTO_OUT_2("\n\tstring value %s ", val.v.s);
                            intp->push_string( val.v.s );
                        }
                        else if ((val.t==TP_INT)) {
                            PROTO_OUT_2("\n\tinteger value %d", val.v.i);
                            intp->push_int(val.v.i);
                        }
                        else {
                            PROTO_OUT_2("\n\treal value %f ", val.v.f);
                            intp->push_dbl(val.v.f);
                        }


                    }
                }

                if (!dataSection && !symbolSection)
                    break;
            }


            for (int i=0; i<MAX_ANZ_IND; i++)
                sets[i]=-1;

            if (_dataElements[i].isSet) {
                if (isAlgSet)
                    setType=TP_SET_ALG;
                else if (isAlgSet1)
                    setType=TP_SET_ALG_S1;
                    else
                    setType=TP_SET_ENUM;
            } else {
                setType=TPP_EMPTY;
                for (int k=0; k < _dataElements[i].setSymNr.size(); k++ )
                    sets[k]=_dataElements[i].setSymNr[k];
            }

            intp->insertCmplDataValues(_dataElements[i].symNr, sets, elementCount, setType, _dataElements[i].isIndices, _dataElements[i].isDefault);

            //cdatFile.close();
        }

    return true;
    }//end cmplDataInterpreter

    /* ********* checkRankDef ********* */
    bool CmplData::checkRankDef(int file, string name, int rank) {

        for (int j=0; j<args->dataFileNrOfElements(file); j++) {
            if (args->dataFileElement(file,j)==name){
                if (getSetRank(args->dataFileElementSetRankString(file, j))!=rank)
                    return false;
                break;
            }

        }
        return true;

    }// end checkRankDef

    /* ********* getSetRank ********* */
    int CmplData::getSetRank(string str) {
        int rank;
        if (CmplTools::lrTrim(str).empty())
            rank=0;
        else
            if (!CmplTools::stringToInt( str,  rank ) )
                rank=-99;
            else if ( rank==1 )
                rank=0;
            else if (rank==0)
                rank=-99;

        return rank;
    }//end getSetRank


    /* ********* checkArraySetDef ********* */
    bool CmplData::checkArraySetDef(int file, string name, string arraySetDef) {

        for (int j=0; j<args->dataFileNrOfElements(file); j++) {
            if (args->dataFileElement(file,j)==name){
                if (args->dataFileElementArraySetString(file,j)!=arraySetDef)
                    return false;
                break;
            }

        }
        return true;

    }// end checkArraySetDef

    /* ********* getArrayDim ********* */
    int CmplData::getArrayDim(string str, vector<int> &tmpSym) {
        int dim=0;
        string tmpSet, str2;

        if (CmplTools::lrTrim(str).empty())
            dim=0;
        else {
            stringstream setS(str);

            while ( getline(setS, tmpSet, ',' )) {
                for (int i=0; i <_dataElements.size(); i++ ) {
                    str2=CmplTools::lrTrim(tmpSet);
                    if ( _dataElements[i].name==str2 && _dataElements[i].isSet) {
                        tmpSym.push_back(_dataElements[i].symNr);
                        if (_dataElements[i].setRank==0)
                            dim++;
                        else
                            dim+=_dataElements[i].setRank;
                        break;
                    }
                }
                if (dim==0) {
                    dim = -99;
                    _errorMsg="Set: " + tmpSet + " is not defined for the array: ";
                    break;
                }
            }
        }

        return dim;
    }//end getArrayDim



    /* ********* activeDataElement ********* */
    bool CmplData::activeDataElement(int file, string name) {
        bool isActive=false;

        for (int j=0; j<args->dataFileNrOfElements(file); j++) {
            if (args->dataFileElement(file,j)==name){
               isActive=true;
               args->setDataFileElementFound(file,j);
               break;
           }
        }
        return isActive;
    }//end activeDataElement


    /* ********* foundAllDataElements ********* */
    bool CmplData::foundAllDataElements(int file, string &errMsg) {
        bool ret = true;
        for (int j=0; j<args->dataFileNrOfElements(file); j++) {
            if (!args->dataFileElementFound(file,j)) {
                ret=false;
                errMsg+= " : "+ args->dataFileElement(file,j);
            }
        }
        return ret;
    }//end foundAllDataElements


    /* ********* openDataFiles ********* */
    bool CmplData::openDataFiles() {
        for (int i=0; i<args->dataFileListSize(); i++ ) {
            args->openDataFile(i);
            if (!args->dataFile(i)->is_open()) {
                _errorMsg="Can't open cmplData file: "+args->dataFileName(i);
                return false;
            }
        }
        return true;
    }//end openDataFiles



    /* ********* closeDataFiles ********* */
    void CmplData::closeDataFiles() {
        for (int i=0; i<args->dataFileListSize(); i++ ) {
            args->closeDataFile(i);
        }
    }//end closeDataFiles


    /* ********* stringToValStr ********* */
    void CmplData::stringToValStr(string str, val_str &valS ) {

        if ( (CmplTools::stringContains(str,".") || CmplTools::stringContains(CmplTools::lowerCase(str),"e"))  && CmplTools::stringToDouble(str, valS.v.f))
            valS.t=TP_DBL;
        else if (CmplTools::stringToLong(str, valS.v.i))
            valS.t=TP_INT;
        else {
            valS.t=TP_STR;
            valS.v.s = const_cast<char*>(str.c_str());
        }
    }//end stringToValStr


    /* ********* setErrorMsg ********* */
    void CmplData::setErrorMsg(Compiler *comp, string fileName, int lineNr, string str) {
        _errorMsg=str;
        string tmpName=fileName.erase(0,fileName.find_last_of("/")+1);
        fileName.erase(0,fileName.find_last_of("/")+1);
        comp->glvar->akt_src_file = tmpName.c_str();
        comp->glvar->akt_src_line = lineNr;
    }//end setErrorMsg


    /* ********* setErrorMsg ********* */
    void CmplData::setErrorMsg(IntFuncInitCmplData *intp, string fileName, int lineNr, string str) {
        _errorMsg=str;
        string tmpName=fileName.erase(0,fileName.find_last_of("/")+1);
        fileName.erase(0,fileName.find_last_of("/")+1);
        intp->glvar->glvar->akt_src_file = tmpName.c_str();
        intp->glvar->akt_src_line = lineNr;
    }//end setErrorMsg

}
