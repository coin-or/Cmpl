/***********************************************************************
 *  This code is part of CMPL
 *
 *  Copyright (C) 2007, 2008, 2009, 2010 Thomas Schleiff - Halle(Saale),
 *  Germany and Mike Steglich - Technical University of Applied Sciences
 *  Wildau, Germany
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


#include "CmplInstance.hh"
#include "QRegExp"
#include "QString"


namespace cmplUtil {


    /* **************** CmplInstance ************* */
    CmplInstance::CmplInstance(cmpl::CmplCore *m)
    {
        glvar=m;

        _objHasConstants=false;
        _objSense="";
        _isObj=false;
        _objFound=false;
        _nzCount=0;
        _colCount=0;
        _numberOfObjCoef=0;


    } //end CmplInstance


    void CmplInstance::writeInstanceFiles() throw (CmplException) {

        if (!glvar->args->noOutput()) {
            readInstanceLists();

            /* if scip is chosen and the problem is a LP then CBC/CLP is used as solver*/
            if ( glvar->args->solverName()=="scip" && varIntIdxListSize()==0) {
                glvar->args->setSolverName("cbc");
                if (!glvar->args->silent())
                    cout << "For a LP CBC is used instead of Scip to show the marginals." <<endl;
            }

            if (colCount()==0)
                throw CmplException( "No model was created");

            if (objHasConstants())
                cerr << "Objective function is not well formed - Contains a constant parameter, which is ignored." <<endl;

            if (!objFound())
                throw CmplException( "Objective function not found.");

            if (!isObj())
               throw CmplException( "No objective function found. ");

            if (glvar->args->mps.isOut())
                writeMps();

            if (glvar->args->osil.isOut())
                writeOsil();

            if (glvar->args->matrix.isOut())
                writeMatrix();

            if (glvar->args->stat.isOut())
                writeStatistics();
        }
    }

    /* **************** readInstanceLists ************* */
    void CmplInstance::readInstanceLists() {
        int n, n1,  z;
        double v;
        cmpl::val_str *vp;
        cmpl::l_elem_str *le;
        //cmpl::dest_str *objRow=NULL;
        objRow=NULL;

        int tmpColPos;
        int tmpRow=0;
        int tmpConPos=1;

        string tmpName, tmpName1, tmpName2, tmpPattern;

        n=0;
        z=0;

        if (glvar->args->objName().length()==0)
            _objFound=true;

        //column loop
        for(n=0; n<glvar->akt_defnum; n++)
        {

            tmpColPos=-99;

            if( glvar->dest_names[n].n_is_koeff ) {

                tmpColPos=_colCount;

                columnList tmpCols;
                tmpCols.objCol=false;

                colList.push_back(tmpCols);
                varList.push_back(&glvar->dest_names[n]);

                if (glvar->dest_names[n].n_type == cmpl::TP_VAR_DBL)
                    varRealIdxList.push_back(_colCount);
                else
                    varIntIdxList.push_back(_colCount);

                _colCount++;
            }

            colPos.push_back(tmpColPos);
        }

        glvar->dest_akt = glvar->dest_grd;
        //row loop
        while (glvar->dest_akt) {

            le=glvar->dest_akt->d_valp;

            if ( (!_isObj && glvar->dest_akt->d_name == glvar->args->objName() ) || ( glvar->dest_akt->d_mode==MODE_N && !_isObj && glvar->args->objName().length()==0) ) {
                // objective function

                v = DBL_VAL(glvar->dest_akt->d_fonds);
                if(! IS_EQ(v, 0.))
                    _objHasConstants=true;

                if (glvar->args->objSense()!=0) {
                    if ( glvar->args->objSense()  > 0)
                        _objSense = "max";
                    else
                        _objSense =  "min";
                } else if (glvar->dest_akt->d_line_obj_to != 0)   {
                    if ( glvar->dest_akt->d_line_obj_to  > 0)
                        _objSense = "max";
                    else
                        _objSense =  "min";
                } else
                    _objSense = "max"; /* max as default */

                if (glvar->args->objName().length()>0)
                    _objFound=true;

                _objName=glvar->dest_akt->d_name;
                _isObj=true;
                objRow=glvar->dest_akt;

            } else if (glvar->dest_akt!=objRow)  {

                tmpName2=CmplTools::replaceString(glvar->dest_akt->d_name,"],[",",");

                if (tmpName2.find("[")==0)
                    tmpName2="line"+tmpName2;

                QRegExp rx("\\[([a-zA-Z0-9,]*)\\]");
                QString tmp2 = QString::fromStdString(tmpName2);
                QStringList idxList;
                int pos = 0;

                while ((pos = rx.indexIn(tmp2, pos)) != -1) {
                    idxList << rx.cap(1);
                    pos += rx.matchedLength();
                }

                if (idxList.size()>0) {
                    tmp2 = tmp2.left(tmp2.indexOf("[")+1);
                    for ( int i=0; i< idxList.size(); i++) {
                        if (i>0)
                            tmp2+=",";
                        tmp2+=idxList[i];
                    }
                    tmp2+="]";
                    tmpName2=tmp2.toStdString();
                }


                sprintf(glvar->dest_akt->d_name, "%s", tmpName2.c_str());


                conList.push_back(glvar->dest_akt);

                if (conList.size()==1)
                    tmpName=glvar->dest_akt->d_name;

                tmpName1=glvar->dest_akt->d_name;



                if (conList.size()>1 && tmpName==tmpName1) {

                    if (CmplTools::stringContains(tmpName,"[")) {
                        tmpPattern = "%s,%d]";
                        tmpName1=tmpName.substr(0,tmpName.length()-1);
                     }
                    else
                        tmpPattern = "%s[%d]";

                    if (tmpConPos==1)
                        sprintf(conList[tmpRow-1]->d_name, tmpPattern.c_str(), tmpName1.c_str(), tmpConPos);
                    tmpConPos++;
                    sprintf(glvar->dest_akt->d_name, tmpPattern.c_str(), tmpName1.c_str(), tmpConPos);
                } else {
                    tmpConPos=1;
                    tmpName=glvar->dest_akt->d_name;
                }


                for(n1=0, le=glvar->dest_akt->d_valp; n1<glvar->dest_akt->d_anz_val; n1++, le++)
                {
                    vp = &(le->le_val);
                    columnCoef tmpCol;
                    tmpCol.row=glvar->dest_akt->d_lineno - z -1;
                    tmpCol.coef= vp;
                    colList[colListPos(le->le_col)].colValues.push_back(tmpCol);
                    _nzCount++;
                }

                tmpRow++;

            }

            if (glvar->dest_akt==objRow) {
                z++;
                // coefficients - column loop
                for(n1=0, le=glvar->dest_akt->d_valp; n1<glvar->dest_akt->d_anz_val; n1++, le++)
                {
                    vp = &(le->le_val);

                    colList[colListPos(le->le_col)].objCol=true;
                    _numberOfObjCoef++;
                    colList[colListPos(le->le_col)].objCoef= vp;
                }

            }

            glvar->dest_akt = glvar->dest_akt->d_nextline;
        }

        _rowCount = glvar->akt_s_line - 1;
    }//end readInstanceLists


    /* **************** checkLengthOfParameter ************* */
    void CmplInstance::checkLengthOfParameter(string str, string name) throw (CmplException){
        if (!glvar->args->freeMps() ) {
            if (str.length()>12 )
                throw CmplException( "MPS error - parameter " + name +" is to long: " +str );
            if (name.length()>8 )
                throw CmplException( "MPS error - name " + name +" is to long"  );
        }
    }//end checkLengthOfParameter



    /* **************** writeMpsObjCols ************* */
    void CmplInstance::writeMpsObjCols(int col, int m) {
        checkLengthOfParameter(objCoef(col), objName() );
        if(m==0) {
            if (glvar->args->freeMps()) cout << " " << left << varName(col) <<  " " << objName() << " " << objCoef(col);
            else cout << setw(4) << " " << setw(8)  << left << varName(col) <<  "  " << setw(8) << left << objName() << "  " << right <<  setw(12) <<  objCoef(col);
        }
        else {
            if (glvar->args->freeMps()) cout <<  " " << objName() << " " <<  objCoef(col) << endl;
            else cout <<  "   " << setw(8) << left << objName() << "  " << setw(12) << right << objCoef(col)<< endl;
        }
    }//end writeMpsObjCols


    /* **************** writeMpsCols ************* */
    void CmplInstance::writeMpsCols(int col, int n1, int m) {
        checkLengthOfParameter(colCoef(col,n1), conName(n1) );
        int row=colCoefRowIdx(col,n1);
        if(m==0) {
            if (glvar->args->freeMps()) cout << " " << left << varName(col) <<  " " << conName(row) << " " << colCoef(col,n1);
            else cout << setw(4) << " " << setw(8)  << left << varName(col)  <<  "  " << setw(8) << left << conName(row) << "  " << right <<  setw(12) <<  colCoef(col,n1);
        }
        else {
            if (glvar->args->freeMps()) cout <<  " " << conName(row) << " " <<  colCoef(col,n1) << endl;
            else cout <<  "   " << setw(8) << left << conName(row) << "  " << setw(12) << right << colCoef(col,n1) << endl;
        }
    }//end writeMpsCols


    /* ****** writeMps ********************************* */
    void CmplInstance::writeMps() throw (CmplException)
    {
        int n, m, n1, col;
        string tmpObjSense;

        if (!glvar->args->mps.open())
            throw CmplException("Can't write "  + glvar->args->mps.fileName());

        if (glvar->args->freeMps()) cout <<  "* CMPL - Free-MPS - Export" << endl ;
        else  cout <<  "* CMPL - MPS - Export" << endl ;


        cout << "NAME" << setw(10) <<  " " << glvar->args->cmplFile().c_str() <<endl;

        if (glvar->args->solverName()=="gurobi"  ) {
            cout <<  "OBJSENSE" << endl << "\t" <<  objSense()  << endl ;

        } else if  (glvar->args->solverName()=="cplex" ) {
            if (objSense()=="min")
                tmpObjSense="MIN";
            else
                tmpObjSense="MAX";
            cout <<  "OBJSENSE" << endl << "\t" <<  tmpObjSense << endl ;
            cout <<  "OBJNAME "  << endl << "\t" <<  objName() << endl;
        }
        else if  (glvar->args->solverName()=="scip") {
            if (objSense()=="min")
                tmpObjSense="MIN";
            else
                tmpObjSense="MAX";
            cout <<  "OBJSEN" << endl << "\t" <<  tmpObjSense << endl ;
        }


        cout <<  "ROWS" << endl ;

        if (glvar->args->freeMps()) cout <<  " " << 'N' << " " <<  objName() << endl;
        else  cout <<  " " << 'N' << setw(2) << " " << left << setw(8)  << objName() << endl;

        for (n=0; n<conListSize(); n++) {

            char md;
            switch(conType(n)) {
            case MODE_G:
                md = 'E';
                break;
            case MODE_U:
                md = 'G';
                break;
            case MODE_O:
                md = 'L';
                break;
            case MODE_N:
                //md = 'N';
                md = 'G';
                break;
            }

            if (glvar->args->freeMps()) cout <<  " " << md << " " <<  conName(n) << endl;
            else  cout <<  " " << md << setw(2) << " " << left << setw(8)  << conName(n) << endl;

        }

        cout <<  "COLUMNS" << endl ;

        for (n=0; n<varRealIdxListSize();n++) {
            m=0;
            col=varRealIdx(n);

            if (colIsInObj(col) ) {
                writeMpsObjCols(col, m);
                m++;
                if(m==2) m=0;
            }
            for ( n1=0; n1<columnsCoefListSize(col); n1++) {
                writeMpsCols(col,n1,m);
                m++;
                if(m==2) m=0;
            }
            if(m==1) cout <<  "" << endl ;

        }

        if (varIntIdxListSize()>0) {
            if (!glvar->args->integerRelaxation()) {
                if (glvar->args->freeMps()) cout << " GVANF 'MARKER' 'INTORG'" << endl;
                else cout << setw(4) << " " <<"GVANF" << setw(5) << " " << "'MARKER'" << setw(17) << " " <<"'INTORG'" << endl;
            }

            for (n=0; n<varIntIdxListSize();n++) {
                m=0;
                col=varIntIdx(n);

                if (colIsInObj(col) ) {
                    writeMpsObjCols(col, m);
                    m++;
                    if(m==2) m=0;

                }
                for ( n1=0; n1<columnsCoefListSize(col); n1++) {
                    writeMpsCols(col,n1,m);
                    m++;
                    if(m==2) m=0;
                }
                if(m==1) cout <<  "" << endl ;

            }
            if (!glvar->args->integerRelaxation()) {
                if (glvar->args->freeMps()) cout << " GVEND 'MARKER' 'INTEND'" << endl;
                else cout << setw(4) << " " <<"GVEND" << setw(5) << " " << "'MARKER'" << setw(17) << " " <<"'INTEND'" << endl;
            }
        }

        cout <<  "RHS" << endl ;
        m=0;
        for (n=0; n<conListSize(); n++) {
            if (conType(n)!=MODE_N) {
                checkLengthOfParameter(conRhs(n), conName(n));
                if (!conBoundIsZero(n)) {
                    if(m==0) {
                        if (glvar->args->freeMps()) cout <<  " " << left << "RHS" << " " << conName(n) << " " << right << conRhs(n);
                        else cout <<  "    " << setw(8) << left << "RHS" << "  " << setw(8) << conName(n) << "  " << setw(12) << right << conRhs(n);
                    }
                    else {
                        if (glvar->args->freeMps()) cout <<  " " << left << conName(n) << " " << right << conRhs(n) << endl;
                        else cout <<  "   " << setw(8) << left << conName(n) << "  " << setw(12) << right << conRhs(n) << endl;
                    }
                    m++;
                    if(m==2) m=0;
                }
            } else {
                // handling of N constrainsts
                if(m==0) {
                    if (glvar->args->freeMps()) cout <<  " " << left << "RHS" << " " << conName(n) << " " << right << glvar->args->negInfinity();
                    else cout <<  "    " << setw(8) << left << "RHS" << "  " << setw(8) << conName(n) << "  " << setw(12) << right << glvar->args->negInfinity();
                }
                else {
                    if (glvar->args->freeMps()) cout <<  " " << left << conName(n) << " " << right << glvar->args->negInfinity() << endl;
                    else cout <<  "   " << setw(8) << left << conName(n) << "  " << setw(12) << right << glvar->args->negInfinity() << endl;
                }
                m++;
                if(m==2) m=0;
                // end handling of N constrainsts

            }
        }
        if(m==1) cout  << endl ;

#ifdef RANGE
        cout <<  "RANGES" << endl ;
        m=0;
        for (n=0; n<conListSize(); n++) {
            if (conHasRange(n)) {
                checkLengthOfParameter(conRange(n), conName(n));
                if(m==0) {
                    if (glvar->args->freeMps()) cout <<  " " <<  left << RANGE_NAME << " " <<  conName(n) << " " << right << conRange(n);
                    else cout <<  "    " << setw(8) << left << setw(8) <<  RANGE_NAME << "  " << setw(8) << conName(n) << "  " << setw(12) << right << conRange(n);
                }
                else {
                    if (glvar->args->freeMps()) cout <<  " " <<  left << conName(n) << " " << right << conRange(n) << endl;
                    else cout <<  "   " << setw(8) << left <<  conName(n) << "  " << setw(12) << right << conRange(n) << endl;
                }
                m++;
                if(m==2) m=0;
            }
        }
        if(m==1) cout << endl ;
#endif

        cout <<  "BOUNDS" << endl ;
        for (n=0; n<varListSize(); n++)
        {
            if ( varHasLowerBound(n) || varHasUpperBound(n)  ) {
                if (varHasLowerBound(n)) {
                    if (!varLowerBoundIsZero(n) ) {
                        checkLengthOfParameter(varLowerBound(n) , varName(n) );
                        if (glvar->args->freeMps()) cout <<  " LO " << left <<  "BOUND" << " " << varName(n) << " " <<  right << varLowerBound(n) << endl;
                        else cout <<  " LO " << left  << setw(8) << "BOUND" << "  " << setw(8) << varName(n) << "  " << setw(12) << right << varLowerBound(n) << endl;
                    }
                } else {
                    if (glvar->args->freeMps()) cout <<  " MI " <<  left << "BOUND" << " " << varName(n) << endl;
                    else cout << " MI " << left  << setw(8) << "BOUND" << "  " << setw(8) << varName(n)  << endl;
                }
                if (varHasUpperBound(n)  ) {
                    checkLengthOfParameter(varUpperBound(n), varName(n));
                    if (glvar->args->freeMps()) cout <<  " UP " <<  left << "BOUND" << " " <<  varName(n) << " " << right << varUpperBound(n) << endl;
                    else cout <<  " UP " << left << setw(8) << "BOUND" << "  " << setw(8) << varName(n) << "  " << setw(12) << right <<  varUpperBound(n) << endl;
                } else {
                    if (glvar->args->freeMps()) cout <<  " PL " <<  left << "BOUND" << " " << varName(n)  << endl;
                    else cout <<  " PL " << left << setw(8) << "BOUND" << "  " << setw(8) << varName(n)  << endl;
                }

            } else {
                if (glvar->args->freeMps()) cout <<  " FR BOUND " << left << varName(n) << endl;
                else cout <<  " FR " << setw(8) << left << setw(8) << "BOUND" << "  " << setw(8) << varName(n) << endl;
            }

        }

        cout <<  "ENDATA" << endl ;
        glvar->args->mps.close();
    }//end writeMps




    /* ****** writeOsil ****************************** */
    void CmplInstance::writeOsil() throw (CmplException)
    {
        int n, n1, nStart;

        if (!glvar->args->osil.open())
            throw CmplException("Can't write "  + glvar->args->osil.fileName());

        cout << "<?xml version=\"1.0\" encoding=\"UTF-8\"?> " << endl ;
        cout << "<osil xmlns=\"os.optimizationservices.org\"  xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:schemaLocation=\"os.optimizationservices.org http://www.optimizationservices.org/schemas/2.0/OSiL.xsd\">" << endl ;

        cout << "\t<instanceHeader>" << endl ;
        cout << "\t\t<name>" << glvar->args->cmplFile().c_str() << "</name>" << endl ;
        cout << "\t\t<description>generated by CMPL</description>" << endl ;
        cout << "\t</instanceHeader>" << endl ;

        cout << "\t<instanceData>" << endl ;

        cout << "\t\t<variables numberOfVariables=\"" << colCount() << "\">" << endl;


        for (n=0; n<varListSize(); n++)
        {
            cout << "\t\t\t<var name=\"" << varName(n)  << "\" " ;
            cout << " type=\"" << varType(n) << "\"" ;

            cout << " lb=\"" << varLowerBound(n) << "\"";
            if (varHasUpperBound(n))
                cout << " ub=\"" << varUpperBound(n) << "\"";

            cout << "/>" << endl ;
        }
        cout << "\t\t</variables>" << endl ;

        cout << "\t\t<objectives numberOfObjectives=\"1\">" << endl;

        cout << "\t\t\t<obj name=\"" << objName() << "\" maxOrMin=\"" << objSense() <<"\"";

        cout << " numberOfObjCoef=\"" << numberOfObjCoef() << "\">" << endl;

        n1=0;
        for (n=0; n<columnsListSize(); n++) {
            if (colIsInObj(n)) {
                cout << "\t\t\t\t<coef idx=\"" << n1 << "\">" << objCoef(n) << "</coef>" << endl;

            }
            n1++;
        }

        cout << "\t\t\t</obj>" << endl ;
        cout << "\t\t</objectives>" << endl ;

        cout << "\t\t<constraints numberOfConstraints=\"" << rowCount() -1 << "\">" << endl;

        for (n=0; n<conListSize(); n++) {

            cout << "\t\t\t<con ";
            cout << "name=\"" <<  conName(n) << "\"" ;

            switch(conType(n))

            {
            case MODE_G:
                cout << " lb=\"" << conLowerBound(n) << "\" ub=\"" << conUpperBound(n) <<  "\"/>" << endl ;
                break;
            case MODE_U:
                cout << " lb=\"" << conLowerBound(n) << "\"/>" << endl  ;
                break;
            case MODE_O:
                cout << " ub=\"" << conUpperBound(n) << "\"/>" << endl ;
                break;
            case MODE_N:
                cout << "/>" << endl;
                break;
            }
        }

        cout << "\t\t</constraints>" << endl ;

        cout << "\t\t<linearConstraintCoefficients numberOfValues=\"" << conCoefCount() << "\">" << endl;


        cout << "\t\t\t<start>" << endl ;
        nStart=0;
        cout << "\t\t\t\t<el>" << nStart << "</el>" << endl;

        for (n=0; n<columnsListSize(); n++) {
            nStart+=columnsCoefListSize(n);
            cout << "\t\t\t\t<el>" << nStart << "</el>" << endl;
        }
        cout << "\t\t\t</start>" << endl ;

        cout << "\t\t\t<rowIdx>" << endl ;
        for (n=0; n<columnsListSize(); n++) {
            for ( n1=0; n1<columnsCoefListSize(n); n1++)
                cout << "\t\t\t\t<el>" << colCoefRowIdx(n,n1) << "</el>" << endl;
        }
        cout << "\t\t\t</rowIdx>" << endl ;


        cout << "\t\t\t<value>" << endl ;
        for (n=0; n<columnsListSize(); n++) {
            for ( n1=0; n1<columnsCoefListSize(n); n1++)
                cout << "\t\t\t\t<el>" << colCoef(n,n1) << "</el>" << endl;

        }
        cout << "\t\t\t</value>" << endl ;

        cout << "\t\t</linearConstraintCoefficients>" << endl ;

        cout << "\t</instanceData>" << endl ;
        cout <<  "</osil>" ;


        glvar->args->osil.close();

    }//end writeOsil




    /* ****** writeOsil ****************************** */
    void CmplInstance::writeMatrix() throw (CmplException)
    {
        int col, row, n1,n ,tmpCol;
        cmpl::val_str *vp;
        cmpl::l_elem_str *le;

        if (!glvar->args->matrix.open())
            throw CmplException("Can't read"  + glvar->args->matrix.fileName());

        cout << setw(20) << left << "Variable name" << setw(4) << "";
        for ( col=0; col<varListSize(); col++)
             cout << right << setw(15) << varName(col) ;
        cout <<endl;

        cout << setw(20) << left << "Variable type" << setw(4) << "";
        for ( col=0; col<varListSize(); col++)
             cout << right << setw(15) << varType(col) ;
        cout << endl;
        cout << endl;

        cout << setw(20) << left << objName() << setw(4) << objSense();

        for ( col=0; col<varListSize(); col++)
             cout << right << setw(15) << objCoef(col) ;

        cout << endl;

        cout << left << setw(24) << "   Subject to " ;
        cout << setw( varListSize()* 15 ) << "" << right << setw(15) << "RHS" << endl;

        glvar->dest_akt = glvar->dest_grd;
        row=0;
        while (glvar->dest_akt) {

            if (glvar->dest_akt!=objRow)  {

                cout << setw(20) << left << conName(row) << setw(4) ;
                switch(conType(row)) {
                case MODE_G:
                    cout << 'E';
                    break;
                case MODE_U:
                    cout << 'G';
                    break;
                case MODE_O:
                    cout << 'L';
                    break;
                case MODE_N:
                    cout <<'N';
                    break;
                }
                col=0;

                for(n1=0, le=glvar->dest_akt->d_valp; n1<glvar->dest_akt->d_anz_val; n1++, le++)
                {
                    tmpCol=colListPos(le->le_col);
                    for ( ; col< tmpCol; col++)
                        cout << right << setw(15) << "" ;
                    col++;
                    vp = &(le->le_val);


                    if ( vp->t == cmpl::TP_DBL)
                        cout << right << setw(15) << vp->v.f;
                    else
                        cout << right << setw(15) << vp->v.i;
                }

                for ( ; col< colCount(); col++)
                    cout << right << setw(15) << "" ;


                cout << right << setw(15) << conRhs(row) << endl;
                row++;

           }

           glvar->dest_akt = glvar->dest_akt->d_nextline;
        }
        cout << endl;

        cout << setw(20) << left << "Lower Bound" << setw(4) << "";
        for ( col=0; col<varListSize(); col++)
             cout << right << setw(15) << varLowerBound(col) ;
        cout <<endl;

        cout << setw(20) << left << "Upper Bound" << setw(4) << "";
        for ( col=0; col<varListSize(); col++)
             cout << right << setw(15) << varUpperBound(col) ;
        cout <<endl;

        glvar->args->matrix.close();

    } // end writeMatrix

    /* ****** writeStatistics ****************************************** */
    void CmplInstance::writeStatistics() throw (CmplException)
    {
        double pNZ;
        int anzCoef;

        if (!glvar->args->stat.open())
            throw CmplException("Can't read"  + glvar->args->stat.fileName());

        cout << endl << endl<<  "File: " << glvar->args->cmplFile() << endl;
        cout <<  colCount() << " Columns (variables), " << rowCount() << " Rows (constraints + objective function) " << endl;
        anzCoef=colCount()*(rowCount()-1);
        pNZ=conCoefCount();
        pNZ=pNZ/anzCoef*100;
        cout << conCoefCount() << setprecision( 4 ) << " ("<< pNZ <<"%) of " <<  anzCoef << " coefficients of the constraints are non-zero. " << endl << endl;

        glvar->args->stat.close();
    }//end writeStatistics

     /* ****** writeCmplInfo ****************************************** */
    void CmplInstance::writeCmplInfo() throw (CmplException) {

        string cmplName;
        size_t i;
        i=glvar->args->cmplFile().find_last_of("/");
        if (i==string::npos)
            i=glvar->args->cmplFile().find_last_of("\\");
        if (i!=string::npos)
            cmplName=glvar->args->cmplFile().substr(i+1);
        else
            cmplName=glvar->args->cmplFile();

        if (!glvar->args->cmplInfo.open())
            throw CmplException("Can't read"  + glvar->args->cmplInfo.fileName());

        cout << "<?xml version=\"1.0\" encoding=\"UTF-8\"?> " << endl ;
        cout << "<CmplInfo version=\"1.0\">" << endl ;

        cout << "\t<general>" << endl ;
        cout << "\t\t<instancename>" << cmplName << "</instancename>" << endl ;
        cout << "\t<general>" << endl ;

        if (glvar->args->stat.isOut()) {
            cout << "\t<statistics file=\"" ;
            if (!glvar->args->stat.fileName().empty())
                cout <<  glvar->args->stat.fileName();
            else
                cout << "stdOut" ;
            cout << "\">" << endl ;
            cout << glvar->args->stat.stringOutput() <<endl;
            cout << "\t</statistics>" << endl ;
        }

        if (glvar->args->varprod.isOut()) {
            cout << "\t<variableProducts file=\"";
            if (!glvar->args->varprod.fileName().empty())
                cout << glvar->args->varprod.fileName();
            else
                cout << "stdOut" ;
            cout << "\">" << endl ;
            cout << glvar->args->varprod.stringOutput() <<endl;
            cout << "\t</variableProducts>" << endl ;
        }

        if (glvar->args->matrix.isOut()) {
            cout << "\t<matrix file=\"" ;
            if (!glvar->args->matrix.fileName().empty())
                cout << glvar->args->matrix.fileName() ;
            else
                cout << "stdOut" ;
            cout << "\">" << endl ;
            cout << glvar->args->matrix.stringOutput() <<endl;
            cout << "\t</matrix>" << endl ;
        }

        cout << "</CmplInfo>" << endl ;

        glvar->args->cmplInfo.close();

    } // end writeCmplInfo


    /* **************** varName ************* */
    string CmplInstance::varName(int pos) {
        string str = varList[pos]->n_name;
        return str;
    }//end varName


    /* **************** varType ************* */
    string CmplInstance::varType(int pos) {
        string str;
        if (VAR_WITH_TYPE(varList[pos]->n_type))
        {
            switch(varList[pos]->n_type)
            {
            case cmpl::TP_VAR_DBL:
                str="C";
                break;
            case cmpl::TP_VAR_INT:
                if (!glvar->args->integerRelaxation())
                    str="I";
                else
                    str="C";
                break;
            case cmpl::TP_VAR_BIN:
                if (!glvar->args->integerRelaxation())
                    str="B";
                else
                    str="C";
                break;
            }
        }
        return str;
    }//end varType


    /* **************** varLowerBound ************* */
    string CmplInstance::varLowerBound(int pos) {
        string str;
        char vstr[101];
        cmpl::val_str *vp;
        double v;

        if(varList[pos]->n_is_dn_b)
        {
            vp = &(varList[pos]->n_dn_b);

            v = DBL_VAL(*vp);
            if(!IS_EQ(v, 0.))
            {
                if (vp->t == cmpl::TP_DBL)
                    snprintf(vstr, 100, glvar->args->doubleNumberFormat().c_str(), v);
                else
                    snprintf(vstr, 100, "%ld", vp->v.i);
                str=vstr;
            }
            else
                str= "0";
        }
        else
        {
            str= "-INF";  /* w/o lower bound definition -> -INF as default*/
        }
        return str;
    }//end varLowerBound



    /* **************** varUpperBound ************* */
    string CmplInstance::varUpperBound(int pos) {
        string str;
        char vstr[101];
        cmpl::val_str *vp;

        if(varList[pos]->n_is_up_b)
        {
            vp = &(varList[pos]->n_up_b);
            if (vp->t == cmpl::TP_DBL)
                snprintf(vstr, 100, glvar->args->doubleNumberFormat().c_str(), vp->v.f);
            else
                snprintf(vstr, 100, "%ld", vp->v.i);
            str=vstr;
        }

        return str;
    }//end varUpperBound


    /* **************** varLowerBoundIsZero ************* */
    bool CmplInstance::varLowerBoundIsZero(int pos) {
        cmpl::val_str *vp;
        double v;
        if(varList[pos]->n_is_dn_b) {
            vp = &(varList[pos]->n_dn_b);
            v = DBL_VAL(*vp);
            if(IS_EQ(v, 0.))
                return true;
            else
                return false;
        } else return false;

    }//end varLowerBoundIsZero


    /* **************** objCoef ************* */
    string CmplInstance::objCoef(int pos) {
        char vstr[101];
        string str;

        if (colList[pos].objCol!=true )
            return "";

        if (colList[pos].objCoef->t == cmpl::TP_DBL)
            snprintf(vstr, 100, glvar->args->doubleNumberFormat().c_str(), colList[pos].objCoef->v.f);
        else
            snprintf(vstr, 100, "%ld", colList[pos].objCoef->v.i);
        str=vstr;
        return str;

    }//end objCoef



    /* **************** colCoef ************* */
    string CmplInstance::colCoef(int colpos, int rowpos) {
        char vstr[101];
        string str;
        if ( colList[colpos].colValues[rowpos].coef->t == cmpl::TP_DBL)
            snprintf(vstr, 100, glvar->args->doubleNumberFormat().c_str(), colList[colpos].colValues[rowpos].coef->v.f);
        else
            snprintf(vstr, 100, "%ld", colList[colpos].colValues[rowpos].coef->v.i);
        str=vstr;
        return str;
    }//end colCoef


    /* **************** conBound ************* */
    string CmplInstance::conBound(int pos) {
        char vstr[101];
        string str;
        if(conList[pos]->d_fonds.t != cmpl::TP_DBL)
            snprintf(vstr, 100, "%ld", conList[pos]->d_fonds.v.i);
        else
            snprintf(vstr, 100, glvar->args->doubleNumberFormat().c_str(), conList[pos]->d_fonds.v.f);
        str=vstr;
        return str;
    }//end conBound


    /* **************** conBoundIsZero ************* */
    bool CmplInstance::conBoundIsZero(int pos) {
        cmpl::val_str *vp;
        double v;
        vp = &(conList[pos]->d_fonds);
        v = DBL_VAL(*vp);
        if(IS_EQ(v, 0.))
            return true;
        else
            return false;
    }//end conBoundIsZero


    /* **************** conRange ************* */
    string CmplInstance::conRange(int pos) {
        char vstr[101];
        string str="";
        if (conList[pos]->d_is_range) {

            if(conList[pos]->d_range.t != cmpl::TP_DBL)
                snprintf(vstr, 100, "%ld", conList[pos]->d_range.v.i);
            else
                snprintf(vstr, 100, glvar->args->doubleNumberFormat().c_str(), conList[pos]->d_range.v.f);
        }
        str=vstr;
        return str;
    }//end conRange


    /* **************** conName ************* */
    string CmplInstance::conName(int pos) {
        string str = conList[pos]->d_name;
        return str;
    }//end conName



}
