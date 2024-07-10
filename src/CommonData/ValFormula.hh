
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


#ifndef VALFORMULA_HH
#define VALFORMULA_HH


#include "CmplVal.hh"
#include "OptModel.hh"


using namespace std;



namespace cmpl
{
	/**
	 * base class for formulas, especially for use in constraints
	 */
	class ValFormula : public CmplObjBase
	{
    private:
        unsigned _syntaxElem;               ///< id of syntax element in the cmpl text creating this formula value

    protected:
        /**
         * constructor
         * @param se        id of syntax element in the cmpl text creating this formula value
         */
        ValFormula(unsigned se): _syntaxElem(se)    { }

    public:
        /**
         * get id of syntax element in the cmpl text creating this matrix element
         */
        unsigned syntaxElem() const                 { return _syntaxElem; }

        /**
         * get whether this formula is an expression with numeric value
         */
        virtual bool isNumeric() const              { return true; }

        /**
         * get whether this formula is an expression with boolean value
         */
        virtual bool isBool() const                 { return false; }

        /**
         * return whether this formula can be expressed by a single binary variable (only implemented for formulas with isBool() == true)
         * @param neg           return if the binary variable negated
         * @return              binary variable / NULL: other formula
         */
        virtual OptVar* asSingleBin(bool& neg)      { return NULL; }

        /**
         * get count of direct parts of the formula
         */
        virtual unsigned partCount() const          { return 0; }

        /**
         * get direct part
         * @param i     number of part
         * @return
         */
        virtual CmplVal *getPart(unsigned i)        { return NULL; }

        /**
         * check whether this formula has its canonical form
         * @param chg   change this to canonical form if it is necessary and possible
         * @return      return whether formula has now a canonical form
         */
        virtual bool canonicalForm(bool chg) = 0;       // pure virtual to make an abstract class

        /**
         * check whether this is equal to second formula (formulas should be in canonical form)
         * @param f2    second formula
         * @return
         */
        virtual bool equals(ValFormula *f2)         { return (this == f2); }

        /**
         * get hash for formula
         */
        virtual size_t formulahash()                { return hash<unsigned long>{}((unsigned long)this); }

    public:
        /************** functions for model properties **********/

        /**
         * get whether this formula is suitable for optimization row (linear or non-linear)
         * @param obj			true: as objective / false: as constraint
         * @return
         */
        virtual bool canOptRow(bool obj) const		{ return false; }

        /**
         * get whether this formula is suitable for linear constraint or objective
         */
        virtual bool linearConstraint() const   	{ return false; }

        /**
         * get lower and upper bound of the possible value range of this formula
         * @param lb        return of lower bound (TP_INT, TP_REAL, TP_INFINITE, or TP_EMPTY when unknown)
         * @param ub        return of upper bound (TP_INT, TP_REAL, TP_INFINITE, or TP_EMPTY when unknown)
         * @param con       bounds for the constraint using this formula
         */
        virtual void getBounds(CmplVal& lb, CmplVal& ub, bool con) const                  { lb.dispUnset(); ub.dispUnset(); }

        /**
         * get lower and upper bound of the possible value range for a value
         * @param v         value (can be formula, optimization variable or constraint, or scalar number)
         * @param lb        return of lower bound (TP_INT, TP_REAL, TP_INFINITE, or TP_EMPTY when unknown)
         * @param ub        return of upper bound (TP_INT, TP_REAL, TP_INFINITE, or TP_EMPTY when unknown)
         * @param con       bounds for the constraint using this formula
         */
        static void getBounds(const CmplVal &v, CmplVal& lb, CmplVal& ub, bool con);

        /**
         * set model properties from this constraint
         * @param prop          properties of optimization model
         */
        virtual void setModelProperties(OptModel::Properties& prop) const                   { }


        /************** functions for filling linear model **********/

        /**
         * fills coefficients from this constraint for linear or quadratic model per column
         * @param row			identity number of this row
         * @param coeffs		array to fill with vector of coefficients per column
         * @param vpl           list to fill with products of variables / NULL: no quadratic rows allowed
         */
        virtual void fillCoeffInLinearModelCol(unsigned long row, vector<LinearModel::Coefficient> *coeffs, QLinearModel::CoefficientVarProdList *vpl)					{ throw NonLinearModelException(row); }

        /**
         * fills coefficients from this constraint for linear or quadratic model per row
         * @param row			identity number of this row
         * @param coeffs		vector to fill of coefficients for this row
         * @param vpl           list to fill with products of variables / NULL: no quadratic rows allowed
         */
        virtual void fillCoeffInLinearModelRow(unsigned long row, vector<LinearModel::Coefficient> *coeffs, QLinearModel::CoefficientVarProdList *vpl)					{ throw NonLinearModelException(row); }

        /**
         * fills mode and right hand side of this constraint for linear model
         * @param row			identity number of this row
         * @param mode			mode to fill (valid are: 'N', 'L', 'G', 'E': like in MPS; or '+': maximize, or '-': minimize)
         * @param rhs			right hand side to fill
         * @param qp            quadratic rows allowed
         */
        virtual void fillModeRhsInLinearModel(unsigned long row, char *mode, LinearModel::Coefficient *rhs, bool qp)                                                    { throw NonLinearModelException(row); }

        /**
         * fills coefficients from this constraint for linear or quadratic model per column
         * @param row			identity number of this row
         * @param coeffs		array to fill with vector of coefficients per column
         * @param lhs			this formula is left hand side of comparison
         * @param vpl           list to fill with products of variables / NULL: no quadratic rows allowed
         */
        virtual void fillCoeffInLinearModelColIntern(unsigned long row, vector<LinearModel::Coefficient> *coeffs, bool lhs, QLinearModel::CoefficientVarProdList *vpl)  { throw NonLinearModelException(row); }

        /**
         * fills coefficients from this constraint for linear or quadratic model per row
         * @param row			identity number of this row
         * @param coeffs		vector to fill of coefficients for this row
         * @param colMap		map number of column to index in vector coeffs
         * @param lhs			this formula is left hand side of comparison
         * @param vpl           list to fill with products of variables / NULL: no quadratic rows allowed
         */
        virtual void fillCoeffInLinearModelRowIntern(unsigned long row, vector<LinearModel::Coefficient> *coeffs, map<unsigned long, unsigned long>& colMap, bool lhs, QLinearModel::CoefficientVarProdList *vpl)		{ throw NonLinearModelException(row); }

        /**
         * fills right hand side of this constraint for linear model
         * @param row			identity number of this row
         * @param rhs			right hand side to fill
         * @param lhs			this formula is left hand side of comparison
         * @param qp            quadratic rows allowed
         */
        virtual void fillRhsInLinearModelIntern(unsigned long row, LinearModel::Coefficient *rhs, bool lhs, bool qp)					{ throw NonLinearModelException(row); }


        //TODO: andere virtuelle Funktionen fuer Zugriff, soweit ausserhalb Interpreter-Modul benoetigt
	};



    /**
     * base class for formula for linear combination of optimization variables
     */
    class ValFormulaLinearCombBase : public ValFormula
    {
    protected:
        /**
         * constructor
         * @param se        id of syntax element in the cmpl text creating this formula value
         */
        ValFormulaLinearCombBase(unsigned se): ValFormula(se)   { }

    public:
        /**
         * get whether this formula is suitable for linear constraint or objective
         */
        bool linearConstraint() const override 		{ return true; }

        /**
         * get whether this formula is suitable for optimization row (linear or non-linear)
         * @param obj			true: as objective / false: as constraint
         * @return
         */
        bool canOptRow(bool obj) const override		{ return true; }

        /**
         * get count of terms
         */
        virtual unsigned termCnt() const			{ return 0; }

        /**
         * get factor of a term
         * @param i		number of term
         * @param ae    return factor even if empty
         */
        virtual CmplVal *termFac(unsigned i, bool ae = false)    { return NULL; }

        /**
         * get factor of a term
         * @param i		optimization variable of term
         */
        virtual CmplVal *termVar(unsigned i)        { return NULL; }

        /**
         * get constant term
         */
        virtual CmplVal *constTerm()    			{ return NULL; }


        /************** filling linear model **********/
    public:
        /**
         * fills coefficients from this constraint for linear or quadratic model per column
         * @param row			identity number of this row
         * @param coeffs		array to fill with vector of coefficients per column
         * @param vpl           list to fill with products of variables / NULL: no quadratic rows allowed
         */
        void fillCoeffInLinearModelCol(unsigned long row, vector<LinearModel::Coefficient> *coeffs, QLinearModel::CoefficientVarProdList *vpl) override		{ fillCoeffInLinearModelColIntern(row, coeffs, true, vpl); }

        /**
         * fills coefficients from this constraint for linear or quadratic model per row
         * @param row			identity number of this row
         * @param coeffs		vector to fill of coefficients for this row
         * @param vpl           list to fill with products of variables / NULL: no quadratic rows allowed
         */
        void fillCoeffInLinearModelRow(unsigned long row, vector<LinearModel::Coefficient> *coeffs, QLinearModel::CoefficientVarProdList *vpl)	override		{ map<unsigned long, unsigned long> colMap; fillCoeffInLinearModelRowIntern(row, coeffs, colMap, true, vpl); }

        /**
         * fills mode and right hand side of this constraint for linear model
         * @param row			identity number of this row
         * @param mode			mode to fill (valid are: 'N', 'L', 'G', 'E': like in MPS; or '+': maximize, or '-': minimize)
         * @param rhs			right hand side to fill
         * @param qp            quadratic rows allowed
         */
        void fillModeRhsInLinearModel(unsigned long row, char *mode, LinearModel::Coefficient *rhs, bool qp) override           { *mode = 'N'; fillRhsInLinearModelIntern(row, rhs, true, qp); }
    };


    /**
     * class for formula of single optimization variable with optional factor
     */
    class ValFormulaVar : public ValFormulaLinearCombBase
    {
    protected:
        CmplVal _factor;					///< factor, can only be TP_REAL or TP_INT (or TP_EMPTY if no factor)
        CmplVal _optVar;					///< optimization variable, must be TP_OPT_VAR

    public:
        /**
         * copy constructor
         * @param f			source formula
         */
        inline ValFormulaVar(ValFormulaVar *f): ValFormulaLinearCombBase(f->syntaxElem()), _factor(f->_factor), _optVar(f->_optVar)						{ }

        /**
         * constructor
         * @param se        id of syntax element in the cmpl text creating this formula value
         * @param ov		optimization variable
         */
        inline ValFormulaVar(unsigned se, OptVar *ov): ValFormulaLinearCombBase(se), _factor(), _optVar(TP_OPT_VAR, ov, true)							{ }

        /**
         * constructor
         * @param se        id of syntax element in the cmpl text creating this formula value
         * @param ov		optimization variable
         * @param f			factor
         */
        inline ValFormulaVar(unsigned se, OptVar *ov, CmplVal& f): ValFormulaLinearCombBase(se), _factor(f), _optVar(TP_OPT_VAR, ov, true)              { }

        /**
         * constructor
         * @param se        id of syntax element in the cmpl text creating this formula value
         * @param ov		optimization variable
         * @param r			factor
         */
        inline ValFormulaVar(unsigned se, OptVar *ov, realType r): ValFormulaLinearCombBase(se), _factor(TP_REAL, r), _optVar(TP_OPT_VAR, ov, true)		{ }

        /**
         * constructor
         * @param se        id of syntax element in the cmpl text creating this formula value
         * @param ov		optimization variable
         * @param i			factor
         */
        inline ValFormulaVar(unsigned se, OptVar *ov, intType i): ValFormulaLinearCombBase(se), _factor(TP_INT, i), _optVar(TP_OPT_VAR, ov, true)       { }

        /**
         * destructor
         */
        virtual ~ValFormulaVar()		{ _factor.dispose(); _optVar.dispose(); }


        /**
         * get whether this formula is an expression with boolean value
         */
        bool isBool() const override;

        /**
         * return whether this formula can be expressed by a single binary variable (only implemented for formulas with isBool() == true)
         * @param neg           return if the binary variable negated
         * @return              binary variable / NULL: other formula
         */
        OptVar* asSingleBin(bool& neg) override;

        /**
         * get lower and upper bound of the possible value range of this formula
         * @param lb        return of lower bound (TP_INT, TP_REAL, TP_INFINITE, or TP_EMPTY when unknown)
         * @param ub        return of upper bound (TP_INT, TP_REAL, TP_INFINITE, or TP_EMPTY when unknown)
         * @param con       bounds for the constraint using this formula
         */
        void getBounds(CmplVal& lb, CmplVal& ub, bool con) const override;

        /**
         * set model properties from this constraint
         * @param prop          properties of optimization model
         */
        void setModelProperties(OptModel::Properties& prop) const override;

        /**
         * get count of direct parts of the formula
         */
        unsigned partCount() const override             { return 2; }

        /**
         * get direct part
         * @param i     number of part
         * @return
         */
        CmplVal *getPart(unsigned i) override           { return (i == 0 ? &_factor : (i == 1 ? &_optVar : NULL)); }

        /**
         * get count of terms
         */
        unsigned termCnt() const override               { return 1; }

        /**
         * get factor of a term
         * @param i		number of term
         * @param ae    return factor even if empty
         * @return
         */
        CmplVal *termFac(unsigned i, bool ae = false) override   { return (i == 0 && (_factor || ae) ? &_factor : NULL); }

        /**
         * get factor of a term
         * @param i		optimization variable of term
         * @return
         */
        CmplVal *termVar(unsigned i) override           { return (i == 0 ? &_optVar : NULL); }

        /**
         * check whether this is equal to second formula (formulas should be in canonical form)
         * @param f2    second formula
         * @return
         */
        bool equals(ValFormula *f2) override;

        /**
         * get hash for formula
         */
        size_t formulahash() override                   { return (hash<unsigned long>{}((unsigned long)(_optVar.optVar()->id())) ^ (hash<unsigned long>{}((unsigned long)(_factor.v.i)) << 16)); }

        /**
         * write contents of the object to a stream
         * @param modp			calling module
         * @param mode			mode for output: 0=direct; 1=part of other value
         */
        void write(ostream& ostr, ModuleBase *modp, int mode = 0) const override     { ostr << "<f-v: "; if (_factor) { _factor.write(ostr, modp, 1); ostr << '*'; } _optVar.write(ostr, modp, 1); ostr << '>'; }


        /************** filling linear model **********/
    public:
        /**
         * fills coefficients from this constraint for linear or quadratic model per column
         * @param row			identity number of this row
         * @param coeffs		array to fill with vector of coefficients per column
         * @param lhs			this formula is left hand side of comparison
         * @param vpl           list to fill with products of variables / NULL: no quadratic rows allowed
         */
        void fillCoeffInLinearModelColIntern(unsigned long row, vector<LinearModel::Coefficient> *coeffs, bool lhs, QLinearModel::CoefficientVarProdList *vpl) override;

        /**
         * fills coefficients from this constraint for linear or quadratic model per row
         * @param row			identity number of this row
         * @param coeffs		vector to fill of coefficients for this row
         * @param colMap		map number of column to index in vector coeffs
         * @param lhs			this formula is left hand side of comparison
         * @param vpl           list to fill with products of variables / NULL: no quadratic rows allowed
         */
        void fillCoeffInLinearModelRowIntern(unsigned long row, vector<LinearModel::Coefficient> *coeffs, map<unsigned long, unsigned long>& colMap, bool lhs, QLinearModel::CoefficientVarProdList *vpl) override;

        /**
         * fills right hand side of this constraint for linear model
         * @param row			identity number of this row
         * @param rhs			right hand side to fill
         * @param lhs			this formula is left hand side of comparison
         * @param qp            quadratic rows allowed
         */
        void fillRhsInLinearModelIntern(unsigned long row, LinearModel::Coefficient *rhs, bool lhs, bool qp) override							{ /* leave rhs unchanged, nothing to do */ }
    };


    /**
     * class for formula of product of two or more single optimization variables, and with an optional constant factor
     */
    class ValFormulaVarProd : public ValFormula
    {
    protected:
        CmplVal _factor;					///< factor, can only be TP_REAL or TP_INT (or TP_EMPTY if no factor)
        vector<CmplVal> _optVars;			///< optimization variables, must be TP_OPT_VAR

    public:
        /**
         * copy constructor
         * @param f			source formula
         */
        inline ValFormulaVarProd(ValFormulaVarProd *f): ValFormula(f->syntaxElem()), _factor(f->_factor), _optVars(f->_optVars)					{ }

        /**
         * constructor
         * @param se        id of syntax element in the cmpl text creating this formula value
         * @param f			formula for variable
         */
        inline ValFormulaVarProd(unsigned se, ValFormulaVar *f): ValFormula(se)      { if(f->termFac(0)) _factor.copyFrom(f->termFac(0)); _optVars.push_back(CmplVal(f->termVar(0))); }

        /**
         * constructor
         * @param se        id of syntax element in the cmpl text creating this formula value
         */
        inline ValFormulaVarProd(unsigned se): ValFormula(se), _factor()             { }

        /**
         * destructor
         */
        virtual ~ValFormulaVarProd()		{ _factor.dispose(); for (unsigned i = 0; i < _optVars.size(); i++) { _optVars[i].dispose(); }; }

        /**
         * get count of direct parts of the formula
         */
        unsigned partCount() const override             { return (_optVars.size() + 1); }

        /**
         * get direct part
         * @param i     number of part
         * @return
         */
        CmplVal *getPart(unsigned i) override           { return (i == 0 ? &_factor : (i <= _optVars.size() ? &(_optVars[i-1]) : NULL)); }

        /**
         * get lower and upper bound of the possible value range of this formula
         * @param lb        return of lower bound (TP_INT, TP_REAL, TP_INFINITE, or TP_EMPTY when unknown)
         * @param ub        return of upper bound (TP_INT, TP_REAL, TP_INFINITE, or TP_EMPTY when unknown)
         * @param con       bounds for the constraint using this formula
         */
        void getBounds(CmplVal& lb, CmplVal& ub, bool con) const override;

        /**
         * set model properties from this constraint
         * @param prop          properties of optimization model
         */
        void setModelProperties(OptModel::Properties& prop) const override;

        /**
         * check whether this is equal to second formula (formulas should be in canonical form)
         * @param f2    second formula
         * @return
         */
        bool equals(ValFormula *f2) override;

        /**
         * get hash for formula
         */
        size_t formulahash() override                   { return (hash<unsigned long>{}((unsigned long)(_optVars[0].optVar()->id())) ^ (_optVars.size() > 1 ? hash<unsigned long>{}((unsigned long)(_optVars[1].optVar()->id())) << 8 : 0) ^ (hash<unsigned long>{}((unsigned long)(_factor.v.i + _optVars.size())) << 16)); }

        /**
         * write contents of the object to a stream
         * @param modp			calling module
         * @param mode			mode for output: 0=direct; 1=part of other value
         */
        void write(ostream& ostr, ModuleBase *modp, int mode = 0) const override    { ostr << "<f-vp: "; if (_factor) { _factor.write(ostr, modp, 1); } for (unsigned i = 0; i < _optVars.size(); i++) { ostr << '*'; _optVars[i].write(ostr, modp, 1); } ostr << '>'; }


        /************** filling linear model **********/
    public:
        /**
         * fills coefficients from this constraint for linear or quadratic model per column
         * @param row			identity number of this row
         * @param coeffs		array to fill with vector of coefficients per column
         * @param lhs			this formula is left hand side of comparison
         * @param vpl           list to fill with products of variables / NULL: no quadratic rows allowed
         */
        void fillCoeffInLinearModelColIntern(unsigned long row, vector<LinearModel::Coefficient> *coeffs, bool lhs, QLinearModel::CoefficientVarProdList *vpl) override;

        /**
         * fills coefficients from this constraint for linear or quadratic model per row
         * @param row			identity number of this row
         * @param coeffs		vector to fill of coefficients for this row
         * @param colMap		map number of column to index in vector coeffs
         * @param lhs			this formula is left hand side of comparison
         * @param vpl           list to fill with products of variables / NULL: no quadratic rows allowed
         */
        void fillCoeffInLinearModelRowIntern(unsigned long row, vector<LinearModel::Coefficient> *coeffs, map<unsigned long, unsigned long>& colMap, bool lhs, QLinearModel::CoefficientVarProdList *vpl) override;

        /**
         * fills right hand side of this constraint for linear model
         * @param row			identity number of this row
         * @param rhs			right hand side to fill
         * @param lhs			this formula is left hand side of comparison
         * @param qp            quadratic rows allowed
         */
        void fillRhsInLinearModelIntern(unsigned long row, LinearModel::Coefficient *rhs, bool lhs, bool qp) override							{ /* leave rhs unchanged, nothing to do */ }

        /**
         * fill product of two optimization variables in list
         * @param vpl           list to fill in
         * @param row           identity number of row within the optimization model
         * @param fact          additional factor / NULL: no one
         * @param neg           negate coefficient
         */
        void fillVarProdList(QLinearModel::CoefficientVarProdList *vpl, unsigned long row, CmplVal *fact, bool neg);
    };


    /**
     * formula for linear combination of optimization variables
     */
    class ValFormulaLinearComb : public ValFormulaLinearCombBase
    {
    protected:
        bool _linear;                       ///< flag whether this formula is suitable for linear constraint or objective
        CmplVal _constTerm;					///< term without variable, can only be TP_REAL or TP_INT (or TP_EMPTY if no such term exists)
        vector<CmplVal> _terms;				///< every term has two elements in this vector, first coefficient (must be TP_REAL or TP_INT), second variable (must be TP_OPT_VAR if _linear)

    public:
        /**
         * empty constructor
         * @param se        id of syntax element in the cmpl text creating this formula value
         */
        inline ValFormulaLinearComb(unsigned se): ValFormulaLinearCombBase(se), _linear(true), _constTerm(), _terms()                       { }

        /**
         * copy constructor
         * @param f			source formula
         */
        inline ValFormulaLinearComb(ValFormulaLinearComb *f): ValFormulaLinearCombBase(f->syntaxElem()), _linear(f->_linear), _constTerm(f->_constTerm), _terms(f->_terms)		{ }

        /**
         * constructor
         * @param se        id of syntax element in the cmpl text creating this formula value
         * @param f			formula for variable
         */
        inline ValFormulaLinearComb(unsigned se, ValFormulaVar *f): ValFormulaLinearCombBase(se), _linear(true), _constTerm(), _terms()		{ _terms.push_back(f->termFac(0) ? CmplVal(f->termFac(0)) : CmplVal()); _terms.push_back(CmplVal(f->termVar(0))); }

        /**
         * constructor
         * @param se        id of syntax element in the cmpl text creating this formula value
         * @param ov		optimization variable
         */
        inline ValFormulaLinearComb(unsigned se, OptVar *ov): ValFormulaLinearCombBase(se), _linear(true), _constTerm(), _terms()			{ _terms.push_back(CmplVal()); _terms.push_back(CmplVal(TP_OPT_VAR, ov)); }

        /**
         * destructor
         */
        virtual ~ValFormulaLinearComb()					{ _constTerm.dispose(); for (unsigned i = 0; i < _terms.size(); i++) { _terms[i].dispose(); } }


        /**
         * get whether this formula is suitable for linear constraint or objective
         */
        bool linearConstraint() const override          { return _linear; }

        /**
         * get lower and upper bound of the possible value range of this formula
         * @param lb        return of lower bound (TP_INT, TP_REAL, TP_INFINITE, or TP_EMPTY when unknown)
         * @param ub        return of upper bound (TP_INT, TP_REAL, TP_INFINITE, or TP_EMPTY when unknown)
         * @param con       bounds for the constraint using this formula
         */
        void getBounds(CmplVal& lb, CmplVal& ub, bool con) const override;

        /**
         * set model properties from this constraint
         * @param prop          properties of optimization model
         */
        void setModelProperties(OptModel::Properties& prop) const override;

        /**
         * get count of direct parts of the formula
         */
        unsigned partCount() const override             { return (_terms.size() + 1); }

        /**
         * get direct part
         * @param i     number of part
         * @return
         */
        CmplVal *getPart(unsigned i) override           { return (i == 0 ? &_constTerm : (i <= _terms.size() ? &(_terms[i-1]) : NULL)); }

        /**
         * get count of terms
         */
        unsigned termCnt() const override               { return _terms.size() / 2; }

        /**
         * get factor of a term
         * @param i		number of term
         * @param ae    return factor even if empty
         * @return
         */
        CmplVal *termFac(unsigned i, bool ae = false) override   { unsigned j = 2*i; return (j < _terms.size() && (_terms[j] || ae) ? &(_terms[j]) : NULL); }

        /**
         * get factor of a term
         * @param i		optimization variable of term
         */
        CmplVal *termVar(unsigned i) override           { unsigned j = 2*i + 1; return (j < _terms.size() ? &(_terms[j]) : NULL); }

        /**
         * get constant term
         */
        CmplVal *constTerm() override                   { return &_constTerm; }

        /**
         * return whether this formula can be expressed by a single binary variable (only implemented for formulas with isBool() == true)
         * @param neg           return if the binary variable negated
         * @return              binary variable / NULL: other formula
         */
        OptVar* asSingleBin(bool& neg) override;

        /**
         * check whether this is equal to second formula (formulas should be in canonical form)
         * @param f2    second formula
         * @return
         */
        bool equals(ValFormula *f2) override;

        /**
         * get hash for formula
         */
        size_t formulahash() override                   { return (hash<unsigned long>{}((unsigned long)(_terms[1].optVar()->id())) ^ (hash<unsigned long>{}((unsigned long)(_terms[_terms.size()-1].optVar()->id())) << 8) ^ (hash<unsigned long>{}((unsigned long)(_terms[0].v.i + _constTerm.v.i + _terms.size())) << 16)); }

        /**
         * write contents of the object to a stream
         * @param modp			calling module
         * @param mode			mode for output: 0=direct; 1=part of other value
         */
        void write(ostream& ostr, ModuleBase *modp, int mode = 0) const override     { ostr << "<f-lc: "; if (_constTerm) { _constTerm.write(ostr, modp, 1); } for (unsigned i = 0; i < _terms.size(); i += 2) { ostr << " + "; if (_terms[i]) { _terms[i].write(ostr, modp, 1); ostr << '*'; } _terms[i+1].write(ostr, modp, 1); } ostr << '>'; }


        /************** filling linear model **********/
    public:
        /**
         * fills coefficients from this constraint for linear or quadratic model per column
         * @param row			identity number of this row
         * @param coeffs		array to fill with vector of coefficients per column
         * @param lhs			this formula is left hand side of comparison
         * @param vpl           list to fill with products of variables / NULL: no quadratic rows allowed
         */
        void fillCoeffInLinearModelColIntern(unsigned long row, vector<LinearModel::Coefficient> *coeffs, bool lhs, QLinearModel::CoefficientVarProdList *vpl) override;

        /**
         * fills coefficients from this constraint for linear or quadratic model per row
         * @param row			identity number of this row
         * @param coeffs		vector to fill of coefficients for this row
         * @param colMap		map number of column to index in vector coeffs
         * @param lhs			this formula is left hand side of comparison
         * @param vpl           list to fill with products of variables / NULL: no quadratic rows allowed
         */
        void fillCoeffInLinearModelRowIntern(unsigned long row, vector<LinearModel::Coefficient> *coeffs, map<unsigned long, unsigned long>& colMap, bool lhs, QLinearModel::CoefficientVarProdList *vpl) override;

        /**
         * fills right hand side of this constraint for linear model
         * @param row			identity number of this row
         * @param rhs			right hand side to fill
         * @param lhs			this formula is left hand side of comparison
         * @param qp            quadratic rows allowed
         */
        void fillRhsInLinearModelIntern(unsigned long row, LinearModel::Coefficient *rhs, bool lhs, bool qp) override;

    private:
        /**
         * check if this row is quadratic, with no other non-linearities
         */
        bool checkQP();
    };


    /**
     * formula for compare operation
     */
    class ValFormulaCompare : public ValFormula
    {
    protected:
        CmplVal _leftSide;					///< left side of compare, must be TP_FORMULA
        CmplVal _rightSide;					///< right side of compare, can be TP_FORMULA or TP_REAL or TP_INT
        bool _compGe;						///< check for left side greater or equal right side
        bool _compLe;						///< check for left side lesser or equal right side
        bool _compNeg;						///< negate comparison

        bool _autogen;                      ///< automatically generated within linearization

    public:
        /**
         * copy constructor
         * @param f			source formula
         */
        inline ValFormulaCompare(ValFormulaCompare *f): ValFormula(f->syntaxElem()), _leftSide(f->_leftSide), _rightSide(f->_rightSide), _compGe(f->_compGe), _compLe(f->_compLe), _compNeg(f->_compNeg), _autogen(f->_autogen)             { }

        /**
         * constructor
         * @param f			source formula
         */
        inline ValFormulaCompare(unsigned se, ValFormulaCompare *f, bool ge, bool le, bool neg, bool ag = false): ValFormula(se), _leftSide(f->_leftSide), _rightSide(f->_rightSide), _compGe(ge), _compLe(le), _compNeg(neg), _autogen(ag) { }

        /**
         * constructor
         */
        inline ValFormulaCompare(unsigned se, CmplVal *ls, CmplVal *rs, bool ge, bool le, bool neg, bool ag = false): ValFormula(se), _leftSide(ls), _rightSide(rs), _compGe(ge), _compLe(le), _compNeg(neg), _autogen(ag)                  { }

        /**
         * constructor
         */
        inline ValFormulaCompare(unsigned se, ValFormula *ls, CmplVal *rs, bool ge, bool le, bool neg, bool ag = false): ValFormula(se), _leftSide(TP_FORMULA, ls), _rightSide(rs), _compGe(ge), _compLe(le), _compNeg(neg), _autogen(ag)   { }

        /**
         * destructor
         */
        virtual ~ValFormulaCompare()				{ _leftSide.dispose(); _rightSide.dispose(); }

        /**
         * get whether this formula is suitable for linear constraint or objective
         */
        bool linearConstraint()	const override		{ return (!_compNeg && (_leftSide.t == TP_REAL || _leftSide.t == TP_INT || (_leftSide.t == TP_FORMULA && _leftSide.valFormula()->linearConstraint())) && (_rightSide.t == TP_REAL || _rightSide.t == TP_INT || (_rightSide.t == TP_FORMULA && _rightSide.valFormula()->linearConstraint())) && (_leftSide.t == TP_FORMULA || _rightSide.t == TP_FORMULA)); }

        /**
         * get lower and upper bound of the possible value range of this formula (as if used as constraint)
         * @param lb        return of lower bound (TP_INT, TP_REAL, TP_INFINITE, or TP_EMPTY when unknown)
         * @param ub        return of upper bound (TP_INT, TP_REAL, TP_INFINITE, or TP_EMPTY when unknown)
         * @param con       bounds for the constraint using this formula
         */
        void getBounds(CmplVal& lb, CmplVal& ub, bool con) const override;

        /**
         * set model properties from this constraint
         * @param prop          properties of optimization model
         */
        void setModelProperties(OptModel::Properties& prop) const override;

        /**
         * get whether this formula is an expression with numeric value
         */
        bool isNumeric() const override             { return false; }

        /**
         * get whether this formula is an expression with boolean value
         */
        bool isBool() const override                { return true; }

        /**
         * return whether this formula can be expressed by a single binary variable (only implemented for formulas with isBool() == true)
         * @param neg           return if the binary variable negated
         * @return              binary variable / NULL: other formula
         */
        OptVar* asSingleBin(bool& neg) override;

        /**
         * get whether this formula is suitable for optimization row (linear or non-linear)
         * @param obj			true: as objective / false: as constraint
         * @return
         */
        bool canOptRow(bool obj) const override		{ return !obj; }

        /**
         * get count of direct parts of the formula
         */
        unsigned partCount() const override         { return 2; }

        /**
         * get direct part
         * @param i     number of part
         * @return
         */
        CmplVal *getPart(unsigned i) override       { return (i == 0 ? &_leftSide : (i == 1 ? &_rightSide : NULL)); }

        /**
         * check whether this is equal to second formula (formulas should be in canonical form)
         * @param f2    second formula
         * @return
         */
        bool equals(ValFormula *f2) override;

        /**
         * get hash for formula
         */
        size_t formulahash() override                   { return (hash<unsigned long>{}((unsigned long)(_compLe ? 1 : 0) + (_compGe ? 2 : 0) + (_compNeg ? 4 : 0)) ^ ((_leftSide.t == TP_FORMULA ? _leftSide.valFormula()->formulahash() : hash<unsigned long>{}((unsigned long)_leftSide.v.i)) << 4) ^ ((_rightSide.t == TP_FORMULA ? _rightSide.valFormula()->formulahash() : hash<unsigned long>{}((unsigned long)_rightSide.v.i)) << 8)); }

        /**
         * write contents of the object to a stream
         * @param modp			calling module
         * @param mode			mode for output: 0=direct; 1=part of other value
         */
        void write(ostream& ostr, ModuleBase *modp, int mode = 0) const override    { ostr << "<f-cmp: "; _leftSide.write(ostr, modp, 1); ostr << (_compNeg ? (_compGe ? (_compLe ? " <> " : " < ") : " > ") : (_compGe ? (_compLe ? " = " : " >= ") : " <= ")); _rightSide.write(ostr, modp, 1); ostr << '>'; }


        /************** filling linear model **********/
    public:
        /**
         * fills coefficients from this constraint for linear or quadratic model per column
         * @param row			identity number of this row
         * @param coeffs		array to fill with vector of coefficients per column
         * @param vpl           list to fill with products of variables / NULL: no quadratic rows allowed
         */
        void fillCoeffInLinearModelCol(unsigned long row, vector<LinearModel::Coefficient> *coeffs, QLinearModel::CoefficientVarProdList *vpl) override;

        /**
         * fills coefficients from this constraint for linear or quadratic model per row
         * @param row			identity number of this row
         * @param coeffs		vector to fill of coefficients for this row
         * @param vpl           list to fill with products of variables / NULL: no quadratic rows allowed
         */
        void fillCoeffInLinearModelRow(unsigned long row, vector<LinearModel::Coefficient> *coeffs, QLinearModel::CoefficientVarProdList *vpl) override;

        /**
         * fills mode and right hand side of this constraint for linear model
         * @param row			identity number of this row
         * @param mode			mode to fill (valid are: 'N', 'L', 'G', 'E': like in MPS; or '+': maximize, or '-': minimize)
         * @param rhs			right hand side to fill
         * @param qp            quadratic rows allowed
         */
        void fillModeRhsInLinearModel(unsigned long row, char *mode, LinearModel::Coefficient *rhs, bool qp) override;

    public:
        /**
         * get left or right side of compare
         * @param left          true: left side / false: right side
         * @param res           return of result value
         */
        inline void getSide(bool left, CmplVal& res)      { res.copyFrom((left ? _leftSide : _rightSide), true, false); }

        /**
         * get whether compare is greater or equal
         */
        inline bool isCompGe() const                { return _compGe; }

        /**
         * get whether compare is lesser or equal
         */
        inline bool isCompLe() const                { return _compLe; }

        /**
         * get whether compare is negated
         */
        inline bool isNeg() const                   { return _compNeg; }

        /**
         * get whether automatically generated within linearization
         */
        inline bool autogen() const                 { return _autogen; }
    };


    /**
     * formula with operation for optimization direction
     */
    class ValFormulaObjective : public ValFormula
    {
    protected:
        CmplVal _formula;					///< formula to minimize or to maximize, should be TP_FORMULA
        bool _maximize;						///< true: maximize; false: minimize

    public:
        /**
         * constructor
         * @param se        id of syntax element in the cmpl text creating this formula value
         * @param f         formula to minimize or to maximize, should be TP_FORMULA
         * @param ma        true: maximize; false: minimize
         */
        ValFormulaObjective(unsigned se, CmplVal *f, bool ma): ValFormula(se), _formula(f), _maximize(ma)   { }

        /**
         * destructor
         */
        virtual ~ValFormulaObjective()				{ _formula.dispose(); }

        /**
         * get whether this formula is suitable for linear constraint or objective
         */
        bool linearConstraint() const override		{ return (_formula.t == TP_FORMULA && _formula.valFormula()->linearConstraint()); }

        /**
         * get lower and upper bound of the possible value range of this formula
         * @param lb        return of lower bound (TP_INT, TP_REAL, TP_INFINITE, or TP_EMPTY when unknown)
         * @param ub        return of upper bound (TP_INT, TP_REAL, TP_INFINITE, or TP_EMPTY when unknown)
         * @param con       bounds for the constraint using this formula
         */
        void getBounds(CmplVal& lb, CmplVal& ub, bool con) const override;

        /**
         * set model properties from this constraint
         * @param prop          properties of optimization model
         */
        void setModelProperties(OptModel::Properties& prop) const override       { if (_formula.t == TP_FORMULA) { _formula.valFormula()->setModelProperties(prop); } }

        /**
         * get whether this formula is suitable for optimization row (linear or non-linear)
         * @param obj			true: as objective / false: as constraint
         * @return
         */
        bool canOptRow(bool obj) const override		{ return obj; }

        /**
         * get count of direct parts of the formula
         */
        unsigned partCount() const override         { return 1; }

        /**
         * get direct part
         * @param i     number of part
         * @return
         */
        CmplVal *getPart(unsigned i) override       { return (i == 0 ? &_formula : NULL); }

        /**
         * get formula to minimize or to maximize
         */
        CmplVal& formula()                          { return _formula; }

        /**
         * get true if formula is to maximize or false if it is to minimize
         */
        bool maximize()                             { return _maximize; }

        /**
         * check whether this is equal to second formula (formulas should be in canonical form)
         * @param f2    second formula
         * @return
         */
        bool equals(ValFormula *f2) override;

        /**
         * get hash for formula
         */
        size_t formulahash() override                   { return (hash<unsigned long>{}((unsigned long)(_maximize ? 1 : 0)) ^ ((_formula.t == TP_FORMULA ? _formula.valFormula()->formulahash() : hash<unsigned long>{}((unsigned long)_formula.v.i)) << 1)); }

        /**
         * write contents of the object to a stream
         * @param modp			calling module
         * @param mode			mode for output: 0=direct; 1=part of other value
         */
        void write(ostream& ostr, ModuleBase *modp, int mode = 0) const override    { ostr << "<f-obj: "; _formula.write(ostr, modp, 1); ostr << " -> " << (_maximize ? "max" : "min") << '>'; }


        /************** filling linear model **********/
    public:
        /**
         * fills coefficients from this constraint for linear or quadratic model per column
         * @param row			identity number of this row
         * @param coeffs		array to fill with vector of coefficients per column
         * @param vpl           list to fill with products of variables / NULL: no quadratic rows allowed
         */
        void fillCoeffInLinearModelCol(unsigned long row, vector<LinearModel::Coefficient> *coeffs, QLinearModel::CoefficientVarProdList *vpl) override;

        /**
         * fills coefficients from this constraint for linear or quadratic model per row
         * @param row			identity number of this row
         * @param coeffs		vector to fill of coefficients for this row
         * @param vpl           list to fill with products of variables / NULL: no quadratic rows allowed
         */
        void fillCoeffInLinearModelRow(unsigned long row, vector<LinearModel::Coefficient> *coeffs, QLinearModel::CoefficientVarProdList *vpl) override;

        /**
         * fills mode and right hand side of this constraint for linear model
         * @param row			identity number of this row
         * @param mode			mode to fill (valid are: 'N', 'L', 'G', 'E': like in MPS; or '+': maximize, or '-': minimize)
         * @param rhs			right hand side to fill
         * @param qp            quadratic rows allowed
         */
        void fillModeRhsInLinearModel(unsigned long row, char *mode, LinearModel::Coefficient *rhs, bool qp) override;
    };


    /**
     * formula of values connected by logical operations
     */
    class ValFormulaLogCon : public ValFormula
    {
    protected:
        bool _logNeg;                       ///< logical negation of the whole expression
        bool _logOr;                        ///< true: connection by logical Or / false: connection by logical And
        vector<CmplVal> _formulas;			///< connected formulas

    public:
        /**
         * copy constructor
         * @param f			source formula
         */
        inline ValFormulaLogCon(ValFormulaLogCon *f): ValFormula(f->syntaxElem()), _logNeg(f->_logNeg), _logOr(f->_logOr), _formulas(f->_formulas)		{ }

        /**
         * constructor
         * @param se        id of syntax element in the cmpl text creating this formula value
         * @param neg       logical negation
         * @param logOr		true: connect formula by logical Or / false: by logical And
         */
        inline ValFormulaLogCon(unsigned se, bool neg, bool logOr): ValFormula(se), _logNeg(neg), _logOr(logOr), _formulas()         { }

        /**
         * constructor
         * @param se        id of syntax element in the cmpl text creating this formula value
         * @param neg       logical negation
         * @param f			source formula
         */
        inline ValFormulaLogCon(unsigned se, bool neg, ValFormula *f): ValFormula(se), _logNeg(neg), _logOr(false), _formulas()		{ _formulas.push_back(CmplVal(TP_FORMULA, f, true)); }

        /**
         * constructor
         * @param se        id of syntax element in the cmpl text creating this formula value
         * @param f1		first source formula
         * @param f2		second source formula
         * @param logOr		true: connect formula by logical Or / false: by logical And
         */
        inline ValFormulaLogCon(unsigned se, ValFormula *f1, ValFormula *f2, bool logOr): ValFormula(se), _logNeg(false), _logOr(logOr), _formulas()		{ _formulas.push_back(CmplVal(TP_FORMULA, f1, true)); _formulas.push_back(CmplVal(TP_FORMULA, f2, true)); }

        /**
         * destructor
         */
        virtual ~ValFormulaLogCon()                 { for (unsigned i = 0; i < _formulas.size(); i++) { _formulas[i].dispose(); } }

        /**
         * get whether logical negation of the whole expression
         */
        inline bool logNeg()                        { return _logNeg; }

        /**
         * get whether connection by logical Or or by logical And
         */
        inline bool logOr()                         { return _logOr; }

        /**
         * get connected formulas
         */
        inline vector<CmplVal>& formulas()          { return _formulas; }

        /**
         * get whether this formula is suitable for optimization row (linear or non-linear)
         * @param obj			true: as objective / false: as constraint
         * @return
         */
        bool canOptRow(bool obj) const override		{ return !obj; }

        /**
         * get lower and upper bound of the possible value range of this formula
         * @param lb        return of lower bound (TP_INT, TP_REAL, TP_INFINITE, or TP_EMPTY when unknown)
         * @param ub        return of upper bound (TP_INT, TP_REAL, TP_INFINITE, or TP_EMPTY when unknown)
         * @param con       bounds for the constraint using this formula
         */
        void getBounds(CmplVal& lb, CmplVal& ub, bool con) const override;

        /**
         * set model properties from this constraint
         * @param prop          properties of optimization model
         */
        void setModelProperties(OptModel::Properties& prop) const override;

        /**
         * get whether this formula is an expression with numeric value
         */
        bool isNumeric() const override             { return false; }

        /**
         * get whether this formula is an expression with boolean value
         */
        bool isBool() const override                { return true; }

        /**
         * return whether this formula can be expressed by a single binary variable (only implemented for formulas with isBool() == true)
         * @param neg           return if the binary variable negated
         * @return              binary variable / NULL: other formula
         */
        OptVar* asSingleBin(bool& neg) override;

        /**
         * get count of direct parts of the formula
         */
        unsigned partCount() const override         { return _formulas.size(); }

        /**
         * get direct part
         * @param i     number of part
         * @return
         */
        CmplVal *getPart(unsigned i) override       { return (i < _formulas.size() ? &(_formulas[i]) : NULL); }

        /**
         * check whether this is equal to second formula (formulas should be in canonical form)
         * @param f2    second formula
         * @return
         */
        bool equals(ValFormula *f2) override;

        /**
         * get hash for formula
         */
        size_t formulahash() override                   { return (hash<unsigned long>{}((unsigned long)(_logNeg ? 1 : 0) + (_logOr ? 2 : 0) + (_formulas.size() << 2)) ^ ((_formulas[0].t == TP_FORMULA ? _formulas[0].valFormula()->formulahash() : hash<unsigned long>{}((unsigned long)_formulas[0].v.i)) << 4) ^ ((_formulas[_formulas.size()-1].t == TP_FORMULA ? _formulas[_formulas.size()-1].valFormula()->formulahash() : hash<unsigned long>{}((unsigned long)_formulas[_formulas.size()-1].v.i)) << 8)); }

        /**
         * write contents of the object to a stream
         * @param modp			calling module
         * @param mode			mode for output: 0=direct; 1=part of other value
         */
        void write(ostream& ostr, ModuleBase *modp, int mode = 0) const override;
    };


    class ValFormulaCondOp;

    /**
     * formula of a conditional value
     */
    class ValFormulaCond : public ValFormula
    {
    public:
        /**
         * part of ValFormulaCond, must not used as independent formula
         */
        class Part : public ValFormula
        {
            friend class ValFormulaCond;
            friend class ValFormulaCondOp;

        private:
            CmplValAuto _posCond;                   ///< condition which must be true for this part of formula (can only be TP_FORMULA (but not ValFormulaCond) or TP_EMPTY)
            vector<CmplValAuto> _negConds;          ///< conditions which must be false for this part of formula (can only be TP_FORMULA (but not ValFormulaCond))

            CmplValAuto _val;                       ///< value of this part (value of the formula, if the conditions match) (can be a numeric type or TP_FORMULA (also ValFormulaCond))

        public:
            /**
             * constructor
             */
            Part(CmplValAuto& pc, vector<CmplValAuto>& ncs, CmplValAuto& v, unsigned se): ValFormula(se), _posCond(pc), _negConds(ncs), _val(v)  { }

            /**
             * check whether conditions are the same as in another part
             */
            bool eqCond(Part& p2);

            /**
             * get count of direct parts of the formula
             */
            unsigned partCount() const override         { return 2 + _negConds.size(); }

            /**
             * get direct part
             * @param i     number of part
             * @return
             */
            CmplVal *getPart(unsigned i) override       { return (i == 0 ? &_val : (i == 1 ? &_posCond : (i < _negConds.size() + 2 ? &(_negConds.at(i - 2)) : NULL))); }

            /**
             * check whether this formula has its canonical form
             * @param chg   change this to canonical form if it is necessary and possible
             * @return      return whether formula has now a canonical form
             */
            bool canonicalForm(bool chg) override       { return false; }       // canonical form not implemented for this type of formula

            /**
             * check whether this is equal to second formula (formulas should be in canonical form)
             * @param f2    second formula
             * @return
             */
            bool equals(ValFormula *f2) override;

            /**
             * get hash for formula
             */
            size_t formulahash() override                   { return (hash<unsigned long>{}((unsigned long)_negConds.size()) ^ ((_posCond.t == TP_FORMULA ? _posCond.valFormula()->formulahash() : hash<unsigned long>{}((unsigned long)_posCond.v.i)) << 4) ^ ((_val.t == TP_FORMULA ? _val.valFormula()->formulahash() : hash<unsigned long>{}((unsigned long)_val.v.i)) << 8)); }
        };

    protected:
        vector<CmplValAuto> _parts;                 ///< parts (all parts must be TP_FORMULA of type ValFormulaCond::Part) (there must be not more than one part with matching conditions)
        bool _complete;                             ///< the condition of one part is fulfilled in any case

        bool _binary;                               ///< flag whether all parts of this formula can be used as boolean value
        bool _numeric;                              ///< flag whether all parts of this formula have a numeric value

        bool _optCon;                               ///< flag whether this formula is suitable as constraint
        bool _optObj;                               ///< flag whether this formula is suitable as objective

        CmplValAuto _numericVar;                    ///< optimization variable as proxy for the numeric value (can only be TP_FORMULA ValFormulaVar or TP_EMPTY) (can only be used if _numeric)
        CmplValAuto _binaryFormula;                 ///< boolean formula that represents the value (can only be TP_FORMULA ValFormulaLogCon or TP_EMPTY) (can only be used if _binary)

        //TODO:
        //  _numericVar:
        //      allgemein optVar-Objekt erweitern um Zusatzwert (AddPropOptVarConValCond) (Quellwert, wenn Variable als Ersatz für diesen Wert dient)
        //      wenn _numericVar gebildet wird: ValFormulaCond-Objekt dort eintragen (erzeugt zirkulären Verweis, sollte aber nicht stören? (da Variablen sowieso bis zum Programmende existieren))
        //      Bei Bestimmung Modelleigenschaften:
        //          nach Variablen mit solchem Zusatzwert suchen, und je nach Art des Werts eine entsprechende Modelleigenschaft setzen
        //          (auch nach constraints mit bestimmten Wertart suchen, ebenfalls innerhalb ValFormulaObjective)

        //  ValFormulaCond als Teil anderer Formeln
        //      Bei objTo-Operation wird erster Operand direkt verwendet, so dass ValFormulaCond Bestandteil von ValFormulaObjective wird
        //      Alle anderen Formeln werden durch Operationen erzeugt, die formulaOperPrio() verwenden, dadurch kann dort nie ValFormulaCond Bestandteil sein
        //          (denn die Operation wird in ValFormulaCond ausgeführt, wenn ein Operand ValFormulaCond ist, so dass Ergebnis dann auch direkt ValFormulaCond ist)

        //  bei Linearisierung (separate Extension):
        //      Durchgehen alle Modellzeilen und darin alle verwendeten Variablen
        //          a) constraint hat selbst Formel ValFormulaCond:
        //                  umwandeln in _binaryFormula, die ersetzt die constraint
        //          b) objective hat entweder selbst Formel ValFormulaCond, oder ValFormulaObjective, deren erster Teil ValFormulaCond ist:
        //                  noetigenfalls noch _numericVar erzeugen, und ValFormulaCond in objective damit ersetzen
        //                  dann weiter mit c)
        //          c) constraint/objective enthält Variable, die Zusatzwert vom Typ AddPropOptVarConValCond eingetragen hat
        //                  nur soweit die Variable noch nicht behandelt wurde (! AddPropOptVarConValCond::_linearized)
        //                  für Zusatzwert ausführen wie Vergleichsoperation EQ mit _numericVar als zweitem Operanden (dann natürlich _numericVar nicht als Ersatz für ersten Operanden verwenden)
        //                  ergibt neues ValFormulaCond, dass in _binaryFormula umwandeln
        //                  diese _binaryFormula als neue constraint dem Modell hinzufügen
        //                  merken dass Variable abgearbeitet wurde (AddPropOptVarConValCond::_linearized = true)
        //      (eigentliche Linearisierung damit noch nicht erledigt, entstandene constraints müssen dann noch von LinearLogCon linearisiert werden)
        //          (wie muss das mit der Namensvergabe sein, insbesondere ob _ davor oder nicht?)

    public:
        /**
         * copy constructor
         * @param f			source formula
         */
        inline ValFormulaCond(ValFormulaCond *f): ValFormula(f->syntaxElem()), _parts(f->_parts), _complete(f->_complete), _binary(f->_binary), _numeric(f->_numeric), _optCon(f->_optCon), _optObj(f->_optObj), _numericVar(f->_numericVar), _binaryFormula(f->_binaryFormula)		{ }

        /**
         * constructor
         * @param se        id of syntax element in the cmpl text creating this formula value
         */
        inline ValFormulaCond(unsigned se): ValFormula(se), _parts(), _complete(false), _binary(false), _numeric(false), _optCon(false), _optObj(false)         { }


        /**
         * get whether this formula is suitable for optimization row (linear or non-linear)
         * @param obj			true: as objective / false: as constraint
         * @return
         */
        bool canOptRow(bool obj) const override		{ return (obj ? _optObj : _optCon); }

        /**
         * get lower and upper bound of the possible value range of this formula
         * @param lb        return of lower bound (TP_INT, TP_REAL, TP_INFINITE, or TP_EMPTY when unknown)
         * @param ub        return of upper bound (TP_INT, TP_REAL, TP_INFINITE, or TP_EMPTY when unknown)
         * @param con       bounds for the constraint using this formula
         */
        void getBounds(CmplVal& lb, CmplVal& ub, bool con) const override;

        /**
         * set model properties from this constraint
         * @param prop          properties of optimization model
         */
        void setModelProperties(OptModel::Properties& prop) const override;

        /**
         * get whether this formula is an expression with numeric value
         */
        bool isNumeric() const override             { return _numeric; }

        /**
         * get whether this formula is an expression with boolean value
         */
        bool isBool() const override                { return _binary; }

        /**
         * return whether this formula can be expressed by a single binary variable (only implemented for formulas with isBool() == true)
         * @param neg           return if the binary variable negated
         * @return              binary variable / NULL: other formula
         */
        OptVar* asSingleBin(bool& neg) override     { return NULL; }

        /**
         * get count of direct parts of the formula
         */
        unsigned partCount() const override         { return _parts.size(); }

        /**
         * get direct part
         * @param i     number of part
         * @return
         */
        CmplVal *getPart(unsigned i) override       { return (i < _parts.size() ? &(_parts[i]) : NULL); }

        /**
         * check whether this is equal to second formula (formulas should be in canonical form)
         * @param f2    second formula
         * @return
         */
        bool equals(ValFormula *f2) override;

        /**
         * get hash for formula
         */
        size_t formulahash() override                   { size_t res = 0; for (unsigned i = 0; i < _parts.size(); i++) { res ^= (_parts[i].valFormula()->formulahash() << i); } return res; }

        /**
         * write contents of the object to a stream
         * @param modp			calling module
         * @param mode			mode for output: 0=direct; 1=part of other value
         */
        void write(ostream& ostr, ModuleBase *modp, int mode = 0) const override;
    };
}

#endif // VALFORMULA_HH

