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

#ifndef CMPLINSTANCE_HH
#define CMPLINSTANCE_HH

#include "CmplArgs.hh"
#include "CmplCore.hh"
#include "CmplException.hh"
#include <iostream>
#include <iomanip>
using namespace std;


namespace cmplUtil {


    typedef struct
    {
       int row;
       cmpl::val_str* coef;
    } columnCoef;

    typedef struct
    {
        bool objCol;
        cmpl::val_str* objCoef;
        vector<columnCoef> colValues;

    } columnList;

    typedef struct
    {   int old;
        int orig;
    } colPosition;

    /**
     * <code>CmplInstance</code> implements the handling of the
     * generated instance of a CMPL model and provides the MPS and OSiL
     * output routines
     *
     * @author  Mike Steglich
     */

    class CmplInstance
    {

    public:

        /**
         * constructor
         *
         * <p>
         * @param *m  pointer of a cmplCore object
         * </p>
         */
        CmplInstance(cmpl::CmplCore *m);

        void writeInstanceFiles() throw (CmplException);

        /**
         * Reads the instance in a structur for the
         * the MPS and OSiL output routines
         */
        void readInstanceLists();

        /**
         * Writes the MPS File
         */
        void writeMps() throw (CmplException);

        /**
         * Writes the OSiL File
         */
        void writeOsil() throw (CmplException);


        /**
         * Writes the generated Matrix
         */
        void writeMatrix() throw (CmplException);


        /**
         * Writes some statistics about the generated
         * instance of a CMPL model
         */
        void writeStatistics() throw (CmplException);

        /**
         * Writes some a CmplInfo file
         */
        void writeCmplInfo() throw (CmplException);

        /**
         * Returns the number of columns of the generated
         * instance of a CMPL model
         *
         * @return  number of columns
         */
        inline int colCount() {
            return _colCount;
        }

        /**
         * Returns the number of rows of the generated
         * instance of a CMPL model
         *
         * @return  number of rows
         */
        inline int rowCount() {
            return _rowCount;
        }

        /**
         * Returns true if the objective function contains constants
         *
         * @return  true|false
         */
        inline bool objHasConstants() {
            return _objHasConstants;
        }

        /**
         * Returns true true|false if an objective exists
         *
         * @return  true|false
         */
        inline bool isObj() {
            return _isObj;
        }

        /**
         * Returns true if a specific (command line arg) objective function is found.
         *
         * @return  true|false
         */
        inline bool objFound() {
            return _objFound;
        }

        /**
         * Returns true the sense of the objective function
         *
         * @return  min|max
         */
        inline string objSense() {
            return _objSense;
        }

        /**
         * Returns true the name of the objective function
         *
         * @return  objName
         */
        inline string objName() {
            return _objName;
        }

        /**
         * Returns the size of the list of the real variables
         */
        inline int varRealIdxListSize() {
            return varRealIdxList.size();
        }

        /**
         * Returns the size of the list of the integer variables
         */
        inline int varIntIdxListSize() {
            return varIntIdxList.size();
        }

        /**
         * Returns the original position of an element of the list of the real variables
         *
         * @param pos  - position in the list of the real variables
         * @return the original position in the variable list
         */
        inline int varRealIdx(int pos) {
            return varRealIdxList[pos];
        }

        /**
         * Returns the original position of an element of the  list of the integer variables
         *
         * @param pos  - position in the list of the integer variables
         * @return the original position in the variable list
         */
        inline int varIntIdx(int pos) {
            return varIntIdxList[pos];
        }

        /**
         * Returns the name of a variable
         *
         * @param pos  - position
         * @return name of the variable
         */
        string varName(int pos);

        /**
         * Returns the type of a variable
         *
         * @param pos  - position
         * @return type of the variable
         */
        string varType(int pos);

        /**
         * Returns the lower bound of a variable
         *
         * @param pos  - position
         * @return lower bound of the variable
         */
        string varLowerBound(int pos);

        /**
         * Returns the upper bound of a variable
         *
         * @param pos  - position
         * @return upper bound of the variable
         */
        string varUpperBound(int pos);

        /**
         * Returns the true or false if a variable has an lower bound
         *
         * @param pos  - position
         * @return true|false
         */
        inline bool varHasLowerBound(int pos){
            return varList[pos]->n_is_dn_b;
        }

        /**
         * Returns the true or false if a variable has an upper bound
         *
         * @param pos  - position
         * @return true|false
         */
        inline bool varHasUpperBound(int pos) {
            return varList[pos]->n_is_up_b;
        }
        /**
         * Returns the size of the list of the constraints
         */
         inline int conListSize() {
            return conList.size();
        }

        /**
         * Returns the name of a constraint
         *
         * @param pos  - position
         * @return name of the constraint
         */
        string conName(int pos);

        /**
         * Returns the type of a constraint
         *
         * @param pos  - position
         * @return type of the constraint
         */
        inline int conType(int pos) {
            return conList[pos]->d_mode;
        }

        /**
         * Returns the upper bound of a constraint
         *
         * @param pos  - position
         * @return upper bound of the constraint
         */
        inline string conUpperBound(int pos) {
            return conBound(pos);
        }

        /**
         * Returns the lower bound of a constraint
         *
         * @param pos  - position
         * @return lower bound of the constraint
         */
        inline string conLowerBound(int pos) {
            return conBound(pos);
        }

        /**
         * Returns the size of the list of the variables
         */
        inline int varListSize() {
            return varList.size();
        }

        /**
         * Returns the number of coefficients of a variable
         *
         * @param pos  - column
         * @return number of coefficients
         */
        inline int columnsCoefListSize(int pos) {
            return colList[pos].colValues.size();
        }

        /**
         * Returns the coefficient
         *
         * @param  colpos,  rowpos
         * @return coefficients
         */
        string colCoef(int colpos, int rowpos);

        /**
         * Returns the original row of an element of the list of coefficients
         *
         * @param colpos,  rowpos
         * @return the original row
         */
        inline int colCoefRowIdx(int colpos, int rowpos) {
            return colList[colpos].colValues[rowpos].row;
        }

        /**
         * Returns the size of the list of the columns
         */
        inline int columnsListSize() {
            return colList.size();
        }


    private:
        cmpl::CmplCore *glvar;

        cmpl::dest_str *objRow;

        vector< columnList > colList;
        vector<cmpl::name_str*> varList;
        vector<cmpl::dest_str*> conList;

        vector<int> varIntIdxList;
        vector<int> varRealIdxList;

        vector<int> colPos;

        cmpl::dest_str *_obj;

        string _objName;
        string _objSense;

        bool _objHasConstants;

        int _colCount;
        int _rowCount;
        bool _isObj;
        bool _objFound;
        int _nzCount;
        int _numberOfObjCoef;

        string conBound(int pos);

        void checkLengthOfParameter(string str, string name) throw (CmplException);
        void writeMpsCols(int col, int n1, int m);
        void writeMpsObjCols(int col, int m);


        string objCoef(int pos);

        bool varLowerBoundIsZero(int pos);


        inline bool colIsInObj(int pos) {
            return colList[pos].objCol;
        }


        inline int conCoefCount() {
            return _nzCount;
        }

        inline int numberOfObjCoef() {
            return _numberOfObjCoef;
        }

        inline bool conHasRange(int pos) {
            return conList[pos]->d_is_range;
        }

        bool conBoundIsZero(int pos);


        inline string conRhs(int pos) {
            return conBound(pos);
        }

        string conRange(int pos);

        inline int colListPos(int pos) {
            return colPos[pos];
        }

    };

}
#endif // CMPLINSTANCE_HH
