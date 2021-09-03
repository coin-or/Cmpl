
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


#ifndef OPTMATRIX_HH
#define OPTMATRIX_HH

#include <unordered_map>
#include <list>
#include <stack>

#include "../Control/MainData.hh"
#include "CmplVal.hh"
#include "ValueTree.hh"


using namespace std;



namespace cmpl
{
    class OptModel;
    class LinearModel;
    class ValFormula;


    /**
     * base class for additional properties of an optimization matrix element, especially for linearization
     */
    class AddPropOptVarConBase
    {
    protected:
        CmplValAuto _srcVal;                ///< source value (typically TP_FORMULA) which has caused the optimization matrix element
        bool _linearized;                   ///< necessary linearization is already done

    protected:
        /**
         * constructor
         * @param src       source value
         */
        AddPropOptVarConBase(CmplVal& src): _srcVal(src), _linearized(false)        { }

    public:
        /**
         * destructor
         */
        virtual ~AddPropOptVarConBase()                                             { }

        /**
         * get name of this additional property type
         */
        virtual const char *name() = 0;

        /**
         * get source value (typically TP_FORMULA) which has caused the optimization matrix element
         */
        CmplVal& srcVal()                       { return _srcVal; }

        /**
         * get whether necessary linearization is already done
         */
        bool linearized()                       { return _linearized; }

        /**
         * set whether necessary linearization is already done
         */
        void setLinearized(bool lin = true)     { _linearized = lin; }


        //TODO
        //  (insbesondere virtual abstract Funktionen fuer Modelleigenschaften, fuer Linearisierung)
    };

    /**
     * additional property for an optimization variable which is used as replacement for a conditional value
     */
    class AddPropOptVarConValCond : public AddPropOptVarConBase
    {
    public:
        /**
         * constructor
         * @param src       source value, must be TP_FORMULA with ValFormulaCond
         */
        AddPropOptVarConValCond(CmplVal& src): AddPropOptVarConBase(src)            { }

        /**
         * destructor
         */
        virtual ~AddPropOptVarConValCond()                                          { }

        /**
         * get name of this additional property type
         */
        const char *name() override                  { return "ConValCond"; }

        //TODO
    };


	/**
	 * base class for optimization matrix elements
	 */
	class OptVarConBase : public CmplObjBase, public ValueTreeElem
	{
	protected:
        unsigned _syntaxElem;               ///< id of syntax element in the cmpl text creating this matrix element
        CmplValAuto _names;                 ///< if set then TP_ARRAY of additional names usable for linearisation

        AddPropOptVarConBase *_addProp;     ///< if set then additional properties, especially for linearization (owning pointer)

        //TODO

    public:
        /**
         * destructor
         */
        virtual ~OptVarConBase()                    { if (_addProp) delete _addProp; }

        /**
         * get id of syntax element in the cmpl text creating this matrix element
         */
        unsigned syntaxElem() const                 { return _syntaxElem; }

        /**
         * get TP_ARRAY of additional names usable for linearisation
         */
        CmplVal& names()                            { return _names; }

        /**
         * get additional properties, especially for linearization
         */
        AddPropOptVarConBase *addProp() const       { return _addProp; }

        /**
         * set additional properties, especially for linearization
         */
        void setAddProp(AddPropOptVarConBase *a)    { _addProp = a; }

        /**
         * get lower and upper bound of the possible value range of this variable or constraint
         * @param lb        return of lower bound (TP_INT, TP_REAL, TP_INFINITE, or TP_EMPTY when unknown)
         * @param ub        return of upper bound (TP_INT, TP_REAL, TP_INFINITE, or TP_EMPTY when unknown)
         */
        virtual void getBounds(CmplVal& lb, CmplVal& ub) const  { }

        //TODO
	};


	/**
	 * class for one optimization variable (column in the optimization matrix)
	 */
	class OptVar : public OptVarConBase
	{
	protected:
		CmplVal _dataType;								///< data type
        bool _hasDataType;								///< data type is fixed (when false, then _dataType is the default data type for variables)

        bool _intVar;									///< integer optimization variable
        CmplVal _lowBound;								///< lower bound (can only be TP_EMPTY, TP_INT, TP_REAL)
        CmplVal _uppBound;								///< upper bound (can only be TP_EMPTY, TP_INT, TP_REAL)

        int _usedByCon;                                 ///< mark variable as used by at least one constraint: 0: not used, 1: used, -1: used only in constraint deleted by remodelation

    public:
        /**
         * constructor
         * @param om		result matrix
         * @param tp		data type
         * @param defTp		data type tp is default type
         * @param se		id of syntax element in the cmpl text creating this matrix element
         */
        OptVar(OptModel *om, CmplVal& tp, bool defTp, unsigned se);

        /**
         * destructor
         */
        virtual ~OptVar()                       { _dataType.dispose(); _lowBound.dispose(); _uppBound.dispose(); }

        /**
         * write contents of the object to a stream
         * @param modp			calling module
         * @param mode			mode for output: 0=direct; 1=part of other value
         */
        virtual void write(ostream& ostr, ModuleBase *modp, int mode = 0) const override     { ostr << "<var#" << _id; if (mode == 0) { ostr << ": " << (_hasDataType ? "" : "(def.type) "); _dataType.write(ostr, modp, 1); } ostr << '>'; }

        /**
         * set data type as fixed
         */
        inline void fixDataType()					{ _hasDataType = true; }

        /**
         * get data type
         */
        inline CmplVal& dataType()					{ return _dataType; }

        /**
         * get whether this is an integer optimization variable
         */
        inline bool intVar()						{ return _intVar; }

        /**
         * get whether this is a binary optimization variable
         */
        inline bool binVar()                        { return (_intVar && _uppBound.t == TP_INT && _uppBound.v.i == 1 && _lowBound.t == TP_INT && _lowBound.v.i == 0); }

        /**
         * set whether this is an integer optimization variable
         */
        inline void setIntVar(bool iv)				{ _intVar = iv; }

        /**
         * get lower bound (can only be TP_EMPTY, TP_INT or TP_REAL)
         */
        inline CmplVal& lowBound()					{ return _lowBound; }

        /**
         * set lower bound (must be TP_EMPTY, TP_INT or TP_REAL)
         */
        inline void setLowBound(CmplVal& lb)		{ _lowBound.copyFrom(lb); }

        /**
         * get upper bound (can only be TP_EMPTY, TP_INT or TP_REAL)
         */
        inline CmplVal& uppBound()					{ return _uppBound; }

        /**
         * set upper bound (must be TP_EMPTY, TP_INT or TP_REAL)
         */
        inline void setUppBound(CmplVal& lb)		{ _uppBound.copyFrom(lb); }

        /**
         * get lower and upper bound of the possible value range of this variable or constraint
         * @param lb        return of lower bound (TP_INT, TP_REAL, TP_INFINITE, or TP_EMPTY when unknown)
         * @param ub        return of upper bound (TP_INT, TP_REAL, TP_INFINITE, or TP_EMPTY when unknown)
         */
        void getBounds(CmplVal& lb, CmplVal& ub) const override;

        /**
         * get whether variable is used by at least one constraint: 0: not used, 1: used, -1: used only in constraint deleted by remodelation
         */
        inline int usedByCon()                     { return _usedByCon; }

        /**
         * set whether variable is used by at least one constraint: 0: not used, 1: used, -1: used only in constraint deleted by remodelation
         */
        inline void setUsedByCon(int i)            { _usedByCon = i; }

        /**
         * iterates over all optimization variables found in formula or constraint
         * @param fv        formula or constraint (only used in initializating call)
         * @param stat      current internal iteration status, empty in initializating call
         * @return          first (in initializating call) or next optimization variable, or NULL if iteration ended
         */
        static OptVar *iterInFormula(CmplVal& fv, stack<pair<ValFormula *, unsigned>>& stat);
	};


	/**
	 * class for one optimization constraint or objective (row in the optimization matrix)
	 */
	class OptCon : public OptVarConBase
	{
	protected:
        CmplVal _formula;								///< formula for constraint or objective, should be TP_FORMULA
        bool _objective;								///< objective or constraint
		
	public:
        /**
         * constructor
         * @param om		result matrix
         * @param f			formula for constraint or objective
         * @param obj		true: objective / false: constraint
         * @param se		id of syntax element in the cmpl text creating this matrix element
         */
        OptCon(OptModel *om, CmplVal& f, bool obj, unsigned se);

        /**
         * destructor
         */
        virtual ~OptCon()                       { _formula.dispose(); }

        /**
         * write contents of the object to a stream
         * @param modp			calling module
         * @param mode			mode for output: 0=direct; 1=part of other value
         */
        virtual void write(ostream& ostr, ModuleBase *modp, int mode = 0) const override     { ostr << (_objective ? "<obj#" : "<con#") << _id << ": "; _formula.write(ostr, modp, 1); ostr << '>'; }


        /**
		 * get whether this is a linear constraint or objective
		 */
        bool linearConstraint();
		
        /**
         * get formula for constraint or objective
         */
        ValFormula *formula()							{ return (_formula.t == TP_FORMULA ? _formula.valFormula() : NULL); }

        /**
         * get value of the constraint or objective
         */
        CmplVal& value()                                { return _formula; }

        /**
         * get whether objective or constraint
         */
        bool objective()                                { return _objective; }

        /**
         * get lower and upper bound of the possible value range of this variable or constraint
         * @param lb        return of lower bound (TP_INT, TP_REAL, TP_INFINITE, or TP_EMPTY when unknown)
         * @param ub        return of upper bound (TP_INT, TP_REAL, TP_INFINITE, or TP_EMPTY when unknown)
         */
        void getBounds(CmplVal& lb, CmplVal& ub) const override;

        //TODO
	};


    /**
     * class for linear model by columns or by rows, filled from <code>OptModel</code> object before output
     */
    class LinearModel
    {
        friend class OptModel;

    public:
        /**
         * coefficient for one row or column
         */
        struct Coefficient
        {
            unsigned long idRC;							///< identy number of row or column
            intType iCoeff;								///< integer coefficient / 0: if real number coefficient is used
            realType rCoeff;							///< real number coefficient

            /**
             * constructor
             */
            inline Coefficient()													{ idRC = 0; iCoeff = 0; rCoeff = 0.0; }

            /**
             * constructor
             */
            inline Coefficient(unsigned long id)									{ idRC = id; iCoeff = 0; rCoeff = 0.0; }

            /**
             * constructor
             */
            inline Coefficient(unsigned long id, intType i, bool neg = false)		{ idRC = id; iCoeff = (neg ? -i : i); rCoeff = 0.0; }

            /**
             * constructor
             */
            inline Coefficient(unsigned long id, realType r, bool neg = false)		{ idRC = id; iCoeff = 0; rCoeff = (neg ? -r : r); }

            /**
             * constructor
             */
            inline Coefficient(unsigned long id, CmplVal& v, bool neg = false)		{ idRC = id; iCoeff = (!v ? (neg ? -1 : 1) : (v.t == TP_INT || v.t == TP_BIN ? (neg ? -v.v.i : v.v.i) : 0)); rCoeff = (v.t == TP_REAL ? (neg ? -v.v.r : v.v.r) : 0.0); }

            /**
             * constructor: RHS for using in additional pseudo constraint when variable is not used in at least one constraint
             * @param ov    optimization variable
             */
            Coefficient(OptVar *ov);

            /**
             * initialize coefficient
             * @param id	identy number of row or column
             */
            inline void init(unsigned long id)										{ idRC = id; iCoeff = 0; rCoeff = 0.0; }

            /**
             * negate this coefficient
             */
            inline void negate()													{ iCoeff = -iCoeff; rCoeff = -rCoeff; }

            /**
             * add another value to the coefficient
             * @param v			other value
             * @param neg		negate value v
             */
            void add(CmplVal& v, bool neg = false);

            /**
             * add another value to the coefficient
             * @param v			other value
             */
            void add(Coefficient& v);

            /**
             * multiply another value to the coefficient
             * @param v         other value
             */
            void mult(CmplVal& v);

            /**
             * check if this coefficient is a numeric 0
             * @return			true if numberic 0
             */
            inline bool isNumNull() const											{ return (iCoeff == 0 && rCoeff == 0); }

            /**
             * get coefficient value as string suitable for output
             * @param rf		format spezifier for real number output or NULL for standard format
             * @param ms		max length of output string
             * @return 			coefficient value as string
             */
            string outString(const char *rf, unsigned ms) const;
        };

    protected:
        OptModel *_om;									///< sourve optimization model for this linear model
        bool _byCol;									///< linear model is filled by column (as needed for instance for MPS format) / false: filled by row

        vector<Coefficient> *_coeffs;					///< array of vectors with the coefficients (size of array is number of columns (if _byCol) or number of rows (if !_byCol))
        Coefficient *_rhs;								///< array of right hand sides
        char *_mode;									///< array of mode per row (size of array is number of rows) (valid are: 'N', 'L', 'G', 'E': like in MPS; or '+': maximize, or '-': minimize)

        /**
         * fill _coeffs, _rhs and _mode from sourve optimization model _om
         */
        virtual void fill();

        /**
         * constructor
         */
        LinearModel(OptModel *om, bool byCol, bool fil = true): _om(om), _byCol(byCol), _coeffs(NULL), _rhs(NULL), _mode(NULL)		{ if (fil) fill(); }

    public:
        /**
         * destructor
         */
        virtual ~LinearModel()          { if (_coeffs) delete[] _coeffs; if (_rhs) delete[] _rhs; if (_mode) delete[] _mode; }


        /**
         * get whether linear model is filled by column (as needed for instance for MPS format) / false: filled by row
         */
        bool byCol() const										{ return _byCol; }

        /**
         * get array of vectors with the coefficients (size of array is number of columns (if _byCol) or number of rows (if !_byCol))
         */
        const vector<Coefficient> *coeffs() const				{ return _coeffs; }

        /**
         * get array of right hand sides (size of array is number of rows)
         */
        const Coefficient *rhs() const							{ return _rhs; }

        /**
         * get array of mode per row (size of array is number of rows) (valid are: 'N', 'L', 'G', 'E': like in MPS; or '+': maximize, or '-': minimize)
         */
        const char *mode() const								{ return _mode; }

        /**
         * model is quadratic
         */
        virtual bool quadratic() const                          { return false; }
    };


    /**
     * class for quadratic model, as extension to linear model
     */
    class QLinearModel : public LinearModel
    {
        friend class OptModel;

    public:
        /**
         * coefficient for product of two optimization variables
         */
        struct CoefficientVarProd : public Coefficient
        {
            unsigned long idC2;							///< identy number of second column

            /**
             * constructor
             */
            inline CoefficientVarProd(unsigned long id1, unsigned long id2, CmplVal& v, bool neg = false): Coefficient(id1, v, neg), idC2(id2)		{ }
        };

        /**
         * list of coefficient for products belonging to one row
         */
        struct CoefficientVarProdList
        {
            unsigned long idR;                          ///< identy number of row
            list<CoefficientVarProd> cvp;               ///< coefficients for products of the row

            /**
             * constructor
             */
            CoefficientVarProdList(): idR(0)            { }
        };

    protected:
        list<CoefficientVarProdList> _prods;            ///< list for all rows with products of optimization variables

        /**
         * fill _coeffs, _rhs and _mode from sourve optimization model _om
         */
        void fill() override;

        /**
         * constructor
         */
        QLinearModel(OptModel *om, bool byCol, bool fil = true): LinearModel(om, byCol, false)      { if (fil) fill(); }

    public:
        /**
         * model is quadratic
         */
        bool quadratic() const override             { return true; }

        /**
         * get list for all rows with products of optimization variables
         */
        list<CoefficientVarProdList>& prods()       { return _prods; }
    };


    /**
     * <code>NonLinearModelException</code> is thrown if tried to fill <code>LinearModel</code> for an <code>OptModel</code> object containing non linear constraints.
     * if a quadratic model is allowed, then thrown if the model ist also no quadratic model.
     */
    class NonLinearModelException : public exception        //TODO: besser von runtime_error ableiten?
    {
    private:
        unsigned long _idRow;							///< identy number of non linear row

    public:
        /**
         * constructor
         * @param r					identy number of non linear row
         */
        NonLinearModelException(unsigned long r): _idRow(r)		{ }

        /**
         * destructor
         */
        virtual ~NonLinearModelException() noexcept				{ }

        /**
         * get exception info
         */
        virtual const char *what() const noexcept				{ return "non-linear constraint in the model"; }

        /**
         * get identy number of non linear row
         */
        unsigned long idRow() noexcept							{ return _idRow; }
    };



	/**
     * The <code>OptModel</code> class holds the columns (variables) and rows (constraints and objectives) for the optimization. It is generated by the Interpreter module.
     */
    class OptModel : public MainData::DataBase
	{
    public:
        /**
         * type of the optimization model (save extensions)
         */
        enum ModelType {
            modelTypeUnknown,           ///< model type not yet computed

            modelTypeLP,                ///< continuos linear
            modelTypeMIP,               ///< mixed integer linear
            modelTypeQP,                ///< continuos quadratic
            modelTypeMIQP,              ///< mixed integer quadratic

            //TODO: non-linear types
            modelTypeNL,                ///< other non-linear type
        };

        /**
         * properties of the optimization model
         */
        struct Properties {
            bool init = false;                  ///< properties are initialized
            int vartypes = 0;                   ///< type of variables in model: 0:only continuos / 1:also binary / 2:also integer other than binary
            int conditions = 0;                 ///< logical conditions or operations between optimization constraints: 0:no / -1:only trivial (i.e. logical And) / 1:other
            int varprodInt = 0;                 ///< products of optimization variables with one integer operand: 0:no / 1:only with one binary / 2:also with integer other than binary
            int varprodReal = 0;                ///< products of optimization variables with both continuos variables: 0:no / 1:yes
            // TODO: other non-linear properties

            // properties from extension modules (for each such property: 0:extension not used / <0:used but already linearized / >0:used and handling still necessary
            int sos = 0;                        ///< SOS or SOS2

            /**
             * reset all properties
             */
            void reset()            { init = false; vartypes = 0; conditions = 0; varprodInt = 0; varprodReal = 0; sos = 0; }

            /**
             * get whether model is linear
             */
            bool linear()           { return (init && !conditions && !varprodInt && !varprodReal && sos <= 0); }

            /**
             * get whether model has products of optimization variables
             */
            bool hasVarProd()       { return (init && (varprodInt || varprodReal)); }

            /**
             * get whether model has sos
             */
            bool hasSos()           { return (sos > 0); }

            /**
             * get whether model has conditions
             */
            bool hasConditions()    { return conditions; }

            /**
             * get model type (type with least linearizations)
             */
            ModelType modelType()   { return (!init ? modelTypeUnknown : (vartypes > 0 || conditions > 0 || varprodInt > 0 || sos > 0 ? (varprodInt || varprodReal ? modelTypeMIQP : modelTypeMIP) : (varprodReal ? modelTypeQP : modelTypeLP))); }
        };



	private:
		ValueTreeRoot _cols;							///< optimization variables (elements are OptVar)
		ValueTreeRoot _rows;							///< optimization constraints and objectives (elements are OptCon)

        Properties _prop;                               ///< properties of optimization model

        LinearModel *_modCol;							///< linear model per column; only filled from _cols and _rows if needed for output
        LinearModel *_modRow;							///< linear model per row; only filled from _cols and _rows if needed for output

        unsigned long _objIdx;
        string _objName;
        string _objSense;

        //bool _linearModelChecked;                       ///< to prevent to repeat the test for linearity
        //bool _isLinearModel;                            ///< indicator whether the model is linear

        //bool _isInteger;                                ///< indicator whether the model is integer

        bool _exportOnly;                               ///< indicator for export mode (true) or solving mode (false)


        //TODO

	public:
		/**
		 * constructor
         * @param m         module name
         * @param data      main data object
         */
        OptModel(const char *m, MainData *data): MainData::DataBase(m, data), _cols(TP_OPT_VAR), _rows(TP_OPT_CON), _modCol(NULL), _modRow(NULL)
              { _prop.reset(); _exportOnly=false; }

        /**
         * destructor
         */
        inline ~OptModel()										{ cleanUp(); }


        /************** get model **********/
    public:
        /**
         * get optimization variables
         */
        ValueTreeRoot& cols()									{ return _cols; }

        /**
         * get optimization constraints and objectives
         */
        ValueTreeRoot& rows()									{ return _rows; }

        /**
         * get linear model per column
         * @param qp            allow also quadratic model (returned as QLinearModel)
         */
        LinearModel *getColModel(bool qp = false)               { if (!_modCol) { _modCol = (qp && _prop.hasVarProd() ? new QLinearModel(this, true) : new LinearModel(this, true)); } return _modCol; }

        /**
         * get linear model per row
         * @param qp            allow also quadratic model (returned as QLinearModel)
         */
        LinearModel *getRowModel(bool qp = false)				{ if (!_modRow) { _modRow = (qp && _prop.hasVarProd() ? new QLinearModel(this, false) : new LinearModel(this, false)); } return _modRow; }


        /**
         * set model properties
         * @param modp          calling module
         */
        void setModelProperties(ModuleBase *modp);

        /**
         * get model properties
         */
        Properties& modelProp()                                 { return _prop; }

        /**
         * gets indicator whether the model is linear
         * * @return        true if model is linear
         */
        bool isLinearModel()                                    { return _prop.linear(); }

        /**
         * sets  whether the model is integer or not
         * @param isInt				true for integer
         */
        //inline void setIsInteger(bool isInt) {_isInteger=isInt;}

        /**
         * gets indicator whether the model is integer
         * * @return        true if model is integer
         */
        inline bool isInteger()                                 { return (_prop.vartypes > 0); }

        /**
         * sets whether the model runs in export or solving mode
         * @param exp		true for export mode
         */
        inline void setExportOnly(bool exp)                     { _exportOnly = exp; }

        /**
         * gets indicator whether the model runs in export or solving mode
         * * @return        true for export mode
         */
        inline bool exportOnly()                                { return _exportOnly; }

        /**
         * sets row idx for the objective function
         * @param idx		row idx
         */
        inline void setObjIdx(unsigned long idx)                { _objIdx = idx; }

        /**
         * gets row idx for the objective function
         */
        inline unsigned long objIdx()                           { return _objIdx; }


        /**
         * sets name for the objective function
         * @param name		obj name
         */
        inline void setObjName(string name)                     { _objName = name; }

        /**
         * gets the name for the objective function
         */
        inline string objName()                                 { return _objName; }

        /**
         * sets sense for the objective function
         * @param name		obj name
         */
        inline void setObjSense(string sense)                   { _objSense = sense; }

        /**
         * gets the sense for the objective function
         */
        inline string objSense()                                { return _objSense; }


    private:
        /**
         * get array of names for usage on formatted outputs of the columns or rows of the model
         * @param col				true for column names or false for row names
         * @param sstore			stored string values
         * @param prefStd			prefix for standard name if the element have no name in the value tree / NULL if no standard name
         * @param prefSubs			prefix for substitute name, used if a regular name not exists or is not unique
         * @param quote				quote char for strings, 0 if no one
         * @param rplSp				replacement char for space, 0 for removing spaces, space char for leaving spaces unchanged
         * @param mLen				max length for result name
         * @return					array of the names
         */
        string *outNamesArray(bool col, StringStore *sstore, const char *prefStd, const char *prefSubs, char quote, char rplSp, unsigned mLen);

    public:
        /**
         * get array of column names for usage on formatted outputs
         * @param sstore			stored string values
         * @param quote				quote char for strings, 0 if no one
         * @param rplSp				replacement char for space, 0 for removing spaces, space char for leaving spaces unchanged
         * @param mLen				max length for result name
         * @return					array of the names
         */
        string *colNames(StringStore *sstore, char quote, char rplSp, unsigned mLen)		{ return outNamesArray(true, sstore, NULL, "_c", quote, rplSp, mLen); }

        /**
         * get array of column names for usage on formatted outputs
         * @param sstore			stored string values
         * @param quote				quote char for strings, 0 if no one
         * @param rplSp				replacement char for space, 0 for removing spaces, space char for leaving spaces unchanged
         * @param mLen				max length for result name
         * @return					array of the names
         */
        string *rowNames(StringStore *sstore, char quote, char rplSp, unsigned mLen)		{ return outNamesArray(false, sstore, "line_", "_r", quote, rplSp, mLen); }


        /************** implementation for MainData::DataBase methods **********/
    public:
        /**
         * delete all data
         */
        virtual void cleanUp();

        /**
         * get count of sub data objects
         */
        virtual int subCnt()																		{ return _cols.size() + _rows.size(); }

        /**
         * if sub data is splitted up to different parts for serialization then get count of these parts, otherwise 0
         */
        virtual int subPartCount()																	{ return 2; }

        /**
         * check if serialization to or from stream should be done for the data object
         * @param si		serialization info
         * @param data		main data object
         * @return			true if serialization should be done
         */
        virtual bool checkSerialize(MainData::SerializeInfo& si, const MainData *data);

        /**
         * write data object to output stream
         * @param si		serialization info
         * @param data		main data object
         */
        virtual void serializeTo(MainData::SerializeInfo& si, const MainData *data);

        /**
         * fill data object from input stream
         * @param si		serialization info
         * @param data		main data object
         * @param pos		position of input string, returns iterated position
         * @param subCnt	count of sub data objects
         * @param lc		line count per element or 0 if elements are len coded
         * @param rline		remaining section start line
         */
        virtual void deserializeFrom(MainData::SerializeInfo& si, const MainData *data, int subCnt, int lc, string &rline);
	};
}

#endif // OPTMATRIX_HH

