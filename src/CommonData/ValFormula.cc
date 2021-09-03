
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


#include "ValFormula.hh"


namespace cmpl
{

    /************** ValFormula **********/

    /**
     * get lower and upper bound of the possible value range for a value
     * @param v         value (can be formula, optimization variable or constraint, or scalar number)
     * @param lb        return of lower bound (TP_INT, TP_REAL, TP_INFINITE, or TP_EMPTY when unknown)
     * @param ub        return of upper bound (TP_INT, TP_REAL, TP_INFINITE, or TP_EMPTY when unknown)
     * @param con       bounds for the constraint using this formula
     */
    void ValFormula::getBounds(const CmplVal &v, CmplVal& lb, CmplVal& ub, bool con)
    {
        if (v.t == TP_FORMULA) {
            v.valFormula()->getBounds(lb, ub, con);
        }
        else if (v.isOptRC()) {
            v.optRC()->getBounds(lb, ub);
        }
        else if (v.isScalarNumber()) {
            if (con) {
                lb.dispSet(TP_INT, 0);
                ub.dispSet(TP_INT, 0);
            }
            else {
                lb.copyFrom(v);
                ub.copyFrom(v);
            }
        }
        else {
            lb.dispUnset();
            ub.dispUnset();
        }
    }



    /************** ValFormulaVar **********/

    /**
     * get whether this formula is an expression with boolean value
     */
    bool ValFormulaVar::isBool() const
    {
        // boolean expression if optimization variable is binary without factor
        return (_optVar.t == TP_OPT_VAR && _factor.isNumOne(true) && _optVar.optVar()->binVar());
    }

    /**
     * return whether this formula can be expressed by a single binary variable (only implemented for formulas with isBool() == true)
     * @param neg           return if the binary variable negated
     * @return              binary variable / NULL: other formula
     */
    OptVar* ValFormulaVar::asSingleBin(bool& neg)
    {
        return (isBool() ? _optVar.optVar() : NULL);
    }

    /**
     * get lower and upper bound of the possible value range of this formula
     * @param lb        return of lower bound (TP_INT, TP_REAL, TP_INFINITE, or TP_EMPTY when unknown)
     * @param ub        return of upper bound (TP_INT, TP_REAL, TP_INFINITE, or TP_EMPTY when unknown)
     * @param con       bounds for the constraint using this formula
     */
    void ValFormulaVar::getBounds(CmplVal& lb, CmplVal& ub, bool con) const
    {
        if (_optVar.t == TP_OPT_VAR) {
            _optVar.optVar()->getBounds(lb, ub);

            if (_factor.isScalarNumber() && !_factor.isNumOne()) {
                lb.numMult(_factor);
                ub.numMult(_factor);

                if (_factor.numCmp(0) < 0) {
                    CmplVal t;
                    t.moveFrom(lb);
                    lb.moveFrom(ub);
                    ub.moveFrom(t);
                }
            }
        }
        else {
            lb.dispUnset();
            ub.dispUnset();
        }
    }

    /**
     * set model properties from this constraint
     * @param prop          properties of optimization model
     */
    void ValFormulaVar::setModelProperties(OptModel::Properties& prop) const
    {
        if (_optVar.t == TP_OPT_VAR) {
            OptVar *v = _optVar.optVar();
            if (v->binVar() && prop.vartypes == 0)
                prop.vartypes = 1;
            else if (v->intVar() && !v->binVar())
                prop.vartypes = 2;
        }
    }

    /**
     * fills coefficients from this constraint for linear or quadratic model per column
     * @param row			identity number of this row
     * @param coeffs		array to fill with vector of coefficients per column
     * @param lhs			this formula is left hand side of comparison
     * @param vpl           list to fill with products of variables / NULL: no quadratic rows allowed
     */
    void ValFormulaVar::fillCoeffInLinearModelColIntern(unsigned long row, vector<LinearModel::Coefficient> *coeffs, bool lhs, QLinearModel::CoefficientVarProdList *vpl)
    {
        if (_optVar.t == TP_OPT_VAR) {
            if (!_factor.isNumNull()) {
                unsigned long n = _optVar.optVar()->id();
                vector<LinearModel::Coefficient> *cp = coeffs + n - 1;

                if (cp->size() && cp->back().idRC == row)
                    cp->back().add(_factor, !lhs);
                else
                    cp->push_back(LinearModel::Coefficient(row, _factor, !lhs));

                _optVar.optVar()->setUsedByCon(1);
            }
        }
    }


    /**
     * fills coefficients from this constraint for linear or quadratic model per row
     * @param row			identity number of this row
     * @param coeffs		vector to fill of coefficients for this row
     * @param colMap		map number of column to index in vector coeffs
     * @param lhs			this formula is left hand side of comparison
     * @param vpl           list to fill with products of variables / NULL: no quadratic rows allowed
     */
    void ValFormulaVar::fillCoeffInLinearModelRowIntern(unsigned long row, vector<LinearModel::Coefficient> *coeffs, map<unsigned long, unsigned long>& colMap, bool lhs, QLinearModel::CoefficientVarProdList *vpl)
    {
        if (_optVar.t == TP_OPT_VAR) {
            if (!_factor.isNumNull()) {
                unsigned long n = _optVar.optVar()->id();

                if (colMap.count(n))
                    coeffs->at(colMap[n]).add(_factor, !lhs);
                else {
                    colMap[n] = coeffs->size();
                    coeffs->push_back(LinearModel::Coefficient(n, _factor, !lhs));
                }

                _optVar.optVar()->setUsedByCon(1);
            }
        }
    }



    /************** ValFormulaVarProd **********/

    /**
     * get lower and upper bound of the possible value range of this formula
     * @param lb        return of lower bound (TP_INT, TP_REAL, TP_INFINITE, or TP_EMPTY when unknown)
     * @param ub        return of upper bound (TP_INT, TP_REAL, TP_INFINITE, or TP_EMPTY when unknown)
     * @param con       bounds for the constraint using this formula
     */
    void ValFormulaVarProd::getBounds(CmplVal& lb, CmplVal& ub, bool con) const
    {
        bool res = false;
        if (_optVars.size()) {
            _optVars[0].optVar()->getBounds(lb, ub);
            if (lb && ub) {
                res = true;
                for (unsigned i = 1; i < _optVars.size(); i++) {
                    CmplValAuto vlb, vub;
                    _optVars[i].optVar()->getBounds(vlb, vub);
                    if (!vlb || !vub) {
                        res = false;
                        break;
                    }

                    CmplValAuto v1, v2;
                    v1.copyFrom(lb);
                    v1.numMult(vlb);
                    v2.copyFrom(ub);
                    v2.numMult(vub);

                    if (lb.numCmp(0) >= 0 && vlb.numCmp(0) >= 0) {
                        lb.moveFrom(v1, true);
                        ub.moveFrom(v2, true);
                    }
                    else {
                        CmplValAuto v3, v4;
                        v3.copyFrom(lb);
                        v3.numMult(vub);
                        v4.copyFrom(ub);
                        v4.numMult(vlb);

                        lb.copyFrom(v1);
                        ub.copyFrom(v2);

                        if (lb.numCmp(v2) > 0)
                            lb.copyFrom(v2);
                        if (lb.numCmp(v3) > 0)
                            lb.copyFrom(v3);
                        if (lb.numCmp(v4) > 0)
                            lb.copyFrom(v4);

                        if (ub.numCmp(v1) < 0)
                            ub.copyFrom(v1);
                        if (ub.numCmp(v3) < 0)
                            ub.copyFrom(v3);
                        if (ub.numCmp(v4) < 0)
                            ub.copyFrom(v4);
                    }
                }

                if (res && _factor.isScalarNumber() && !_factor.isNumOne()) {
                    lb.numMult(_factor);
                    ub.numMult(_factor);

                    if (_factor.numCmp(0) < 0) {
                        CmplVal t;
                        t.moveFrom(lb);
                        lb.moveFrom(ub);
                        ub.moveFrom(t);
                    }
                }
            }
        }

        if (!res) {
            lb.dispUnset();
            ub.dispUnset();
        }
    }

    /**
     * set model properties from this constraint
     * @param prop          properties of optimization model
     */
    void ValFormulaVarProd::setModelProperties(OptModel::Properties& prop) const
    {
        unsigned cr = 0, ci = 0, cb = 0;
        for (unsigned i = 0; i < _optVars.size(); i++) {
            OptVar *v = _optVars[i].optVar();
            if (v->binVar())
                cb++;
            else if (v->intVar())
                ci++;
            else
                cr++;
        }

        if ((cb && prop.vartypes == 0) || (ci && prop.vartypes < 2))
            prop.vartypes = (ci ? 2 : 1);

        if (_optVars.size() > 1) {
            if (cr >= 2 && !prop.varprodReal)
                prop.varprodReal = 1;

            if ((cb || ci) && prop.varprodInt < 2) {
                if (ci && cr + ci >= 2)
                    prop.varprodInt = 2;
                else if (prop.varprodInt == 0)
                    prop.varprodInt = 1;
            }
        }
    }


    /**
     * fills coefficients from this constraint for linear or quadratic model per column
     * @param row			identity number of this row
     * @param coeffs		array to fill with vector of coefficients per column
     * @param lhs			this formula is left hand side of comparison
     * @param vpl           list to fill with products of variables / NULL: no quadratic rows allowed
     */
    void ValFormulaVarProd::fillCoeffInLinearModelColIntern(unsigned long row, vector<LinearModel::Coefficient> *coeffs, bool lhs, QLinearModel::CoefficientVarProdList *vpl)
    {
        if (vpl)
            fillVarProdList(vpl, row, NULL, !lhs);
        else
            throw NonLinearModelException(row);
    }

    /**
     * fills coefficients from this constraint for linear or quadratic model per row
     * @param row			identity number of this row
     * @param coeffs		vector to fill of coefficients for this row
     * @param colMap		map number of column to index in vector coeffs
     * @param lhs			this formula is left hand side of comparison
     * @param vpl           list to fill with products of variables / NULL: no quadratic rows allowed
     */
    void ValFormulaVarProd::fillCoeffInLinearModelRowIntern(unsigned long row, vector<LinearModel::Coefficient> *coeffs, map<unsigned long, unsigned long>& colMap, bool lhs, QLinearModel::CoefficientVarProdList *vpl)
    {
        if (vpl)
            fillVarProdList(vpl, row, NULL, !lhs);
        else
            throw NonLinearModelException(row);
    }

    /**
     * fill product of two optimization variables in list
     * @param vpl           list to fill in
     * @param row           identity number of row within the optimization model
     * @param fact          additional factor / NULL: no one
     * @param neg           negate coefficient
     */
    void ValFormulaVarProd::fillVarProdList(QLinearModel::CoefficientVarProdList *vpl, unsigned long row, CmplVal *fact, bool neg)
    {
        if (_optVars.size() != 2)
            throw NonLinearModelException(row);

        if (!vpl->idR)
            vpl->idR = row;

        vpl->cvp.emplace_back(_optVars[0].optVar()->id(), _optVars[1].optVar()->id(), _factor, neg);
        if (fact && !(fact->isNumOne(true)))
            vpl->cvp.back().mult(*fact);

        _optVars[0].optVar()->setUsedByCon(1);
        _optVars[1].optVar()->setUsedByCon(1);
    }



    /************** ValFormulaLinearComb **********/

    /**
     * get lower and upper bound of the possible value range of this formula
     * @param lb        return of lower bound (TP_INT, TP_REAL, TP_INFINITE, or TP_EMPTY when unknown)
     * @param ub        return of upper bound (TP_INT, TP_REAL, TP_INFINITE, or TP_EMPTY when unknown)
     * @param con       bounds for the constraint using this formula
     */
    void ValFormulaLinearComb::getBounds(CmplVal& lb, CmplVal& ub, bool con) const
    {
        if (!con && _constTerm) {
            lb.copyFrom(_constTerm);
            ub.copyFrom(_constTerm);
        }
        else {
            lb.dispSet(TP_INT, 0);
            ub.dispSet(TP_INT, 0);
        }

        for (unsigned i = 1; i < _terms.size(); i += 2) {
            const CmplVal& factor = _terms[i-1];
            if (!factor.isNumNull()) {
                CmplValAuto tlb, tub;
                ValFormula::getBounds(_terms[i], tlb, tub, con);
                if (!tlb || !tub)
                    break;

                if (!factor.isNumOne()) {
                    tlb.numMult(factor);
                    tub.numMult(factor);

                    if (factor.numCmp(0) < 0) {
                        CmplVal t;
                        t.moveFrom(tlb);
                        tlb.moveFrom(tub);
                        tub.moveFrom(t);
                    }
                }

                lb.numAdd(tlb);
                ub.numAdd(tub);

                if (lb.t == TP_INFINITE && lb.v.i < 0 && ub.t == TP_INFINITE && ub.v.i > 0)
                    break;
            }
        }
    }

    /**
     * set model properties from this constraint
     * @param prop          properties of optimization model
     */
    void ValFormulaLinearComb::setModelProperties(OptModel::Properties& prop) const
    {
        for (unsigned i = 1; i < _terms.size(); i += 2) {
            if (!_terms[i-1].isNumNull()) {
                if (_terms[i].t == TP_OPT_VAR) {
                    if (prop.vartypes < 2) {
                        OptVar *v = _terms[i].optVar();
                        if (v->binVar() && prop.vartypes == 0)
                            prop.vartypes = 1;
                        else if (v->intVar() && !v->binVar())
                            prop.vartypes = 2;
                    }
                }
                else if (_terms[i].t == TP_FORMULA) {
                    ValFormula *f = _terms[i].valFormula();
                    f->setModelProperties(prop);
                }
            }
        }
    }


    /**
     * return whether this formula can be expressed by a single binary variable (only implemented for formulas with isBool() == true)
     * @param neg           return if the binary variable negated
     * @return              binary variable / NULL: other formula
     */
    OptVar* ValFormulaLinearComb::asSingleBin(bool& neg)
    {
        if (_linear && _terms.size() == 2 && _terms[0].isNumOne() && _constTerm.isNumNull(true)) {
            CmplVal& v = _terms[1];
            if (v.t == TP_OPT_VAR && v.optVar()->binVar()) {
                neg = false;
                return v.optVar();
            }
        }

        return NULL;
    }

    /**
     * fills coefficients from this constraint for linear or quadratic model per column
     * @param row			identity number of this row
     * @param coeffs		array to fill with vector of coefficients per column
     * @param lhs			this formula is left hand side of comparison
     * @param vpl           list to fill with products of variables / NULL: no quadratic rows allowed
     */
    void ValFormulaLinearComb::fillCoeffInLinearModelColIntern(unsigned long row, vector<LinearModel::Coefficient> *coeffs, bool lhs, QLinearModel::CoefficientVarProdList *vpl)
    {
        bool qp = (_linear ? false : (vpl ? checkQP() : false));
        if (!_linear && !qp)
            throw NonLinearModelException(row);

        for (unsigned i = 0; i < _terms.size(); i += 2) {
            if (!_terms[i].isNumNull()) {
                if (_terms[i+1].t == TP_OPT_VAR) {
                    unsigned long n = _terms[i+1].optVar()->id();
                    vector<LinearModel::Coefficient> *cp = coeffs + n - 1;

                    if (cp->size() && cp->back().idRC == row)
                        cp->back().add(_terms[i], !lhs);
                    else
                        cp->push_back(LinearModel::Coefficient(row, _terms[i], !lhs));

                    _terms[i+1].optVar()->setUsedByCon(1);
                }
                else if (qp) {
                    ValFormulaVarProd *vp = dynamic_cast<ValFormulaVarProd *>(_terms[i+1].valFormula());
                    vp->fillVarProdList(vpl, row, &(_terms[i]), !lhs);
                }
            }
        }
    }


    /**
     * fills coefficients from this constraint for linear or quadratic model per row
     * @param row			identity number of this row
     * @param coeffs		vector to fill of coefficients for this row
     * @param colMap		map number of column to index in vector coeffs
     * @param lhs			this formula is left hand side of comparison
     * @param vpl           list to fill with products of variables / NULL: no quadratic rows allowed
     */
    void ValFormulaLinearComb::fillCoeffInLinearModelRowIntern(unsigned long row, vector<LinearModel::Coefficient> *coeffs, map<unsigned long, unsigned long>& colMap, bool lhs, QLinearModel::CoefficientVarProdList *vpl)
    {
        bool qp = (_linear ? false : (vpl ? checkQP() : false));
        if (!_linear && !qp)
            throw NonLinearModelException(row);

        for (unsigned i = 0; i < _terms.size(); i += 2) {
            if (!_terms[i].isNumNull()) {
                if (_terms[i+1].t == TP_OPT_VAR) {
                    unsigned long n = _terms[i+1].optVar()->id();

                    if (colMap.count(n))
                        coeffs->at(colMap[n]).add(_terms[i], !lhs);
                    else {
                        colMap[n] = coeffs->size();
                        coeffs->push_back(LinearModel::Coefficient(n, _terms[i], !lhs));
                    }

                    _terms[i+1].optVar()->setUsedByCon(1);
                }
                else if (qp) {
                    ValFormulaVarProd *vp = dynamic_cast<ValFormulaVarProd *>(_terms[i+1].valFormula());
                    vp->fillVarProdList(vpl, row, &(_terms[i]), !lhs);
                }
            }
        }
    }


    /**
     * fills right hand side of this constraint for linear model
     * @param row			identity number of this row
     * @param rhs			right hand side to fill (valid are only TP_INT and TP_REAL)
     * @param lhs			this formula is left hand side of comparison
     * @param qp            quadratic rows allowed
     */
    void ValFormulaLinearComb::fillRhsInLinearModelIntern(unsigned long row, LinearModel::Coefficient *rhs, bool lhs, bool qp)
    {
        if (!qp && !_linear)
            throw NonLinearModelException(row);

        if (!_constTerm.isNumNull(true)) {
            rhs->add(_constTerm, lhs);
        }
    }


    /**
     * check if this row is quadratic, with no other non-linearities
     */
    bool ValFormulaLinearComb::checkQP()
    {
        bool qp = false;

        for (unsigned i = 1; i < _terms.size(); i += 2) {
            if (!_terms[i-1].isNumNull() && _terms[i].t == TP_FORMULA) {
                ValFormulaVarProd *vp = dynamic_cast<ValFormulaVarProd *>(_terms[i].valFormula());
                if (vp) {
                    // only products of two variables are allowed for quadratic problem
                    if (vp->partCount() == 3)
                        qp = true;
                    else
                        return false;
                }
            }
        }

        return qp;
    }



    /************** ValFormulaCompare **********/

    /**
     * get lower and upper bound of the possible value range of this formula (as if used as constraint)
     * @param lb        return of lower bound (TP_INT, TP_REAL, TP_INFINITE, or TP_EMPTY when unknown)
     * @param ub        return of upper bound (TP_INT, TP_REAL, TP_INFINITE, or TP_EMPTY when unknown)
     * @param con       bounds for the constraint using this formula
     */
    void ValFormulaCompare::getBounds(CmplVal& lb, CmplVal& ub, bool con) const
    {
        if (con) {
            ValFormula::getBounds(_leftSide, lb, ub, true);

            if (!_rightSide.isScalarNumber()) {
                CmplValAuto rb1, rb2;
                ValFormula::getBounds(_rightSide, rb1, rb2, con);
                if (rb1 && rb2) {
                    lb.numAdd(rb2, true);
                    ub.numAdd(rb1, true);
                }
                else {
                    lb.dispUnset();
                    ub.dispUnset();
                }
            }
        }

        else {
            lb.dispSet(TP_INT, 0);
            ub.dispSet(TP_INT, 1);
        }
    }

    /**
     * set model properties from this constraint
     * @param prop          properties of optimization model
     */
    void ValFormulaCompare::setModelProperties(OptModel::Properties& prop) const
    {
        if (_leftSide.t == TP_FORMULA)
            _leftSide.valFormula()->setModelProperties(prop);

        if (_rightSide.t == TP_FORMULA)
            _rightSide.valFormula()->setModelProperties(prop);
    }


    /**
     * return whether this formula can be expressed by a single binary variable (only implemented for formulas with isBool() == true)
     * @param neg           return if the binary variable negated
     * @return              binary variable / NULL: other formula
     */
    OptVar* ValFormulaCompare::asSingleBin(bool& neg)
    {
        if (_leftSide.t == TP_FORMULA && _rightSide.isScalarNumber()) {
            bool ineg;
            OptVar *sbv = _leftSide.valFormula()->asSingleBin(ineg);
            if (sbv && !ineg) {
                if (_rightSide.isNumOne()) {
                    if (_compGe) {
                        neg = _compNeg;
                        return sbv;
                    }
                }

                else if (_rightSide.isNumNull()) {
                    if (_compLe) {
                        neg = !_compNeg;
                        return sbv;
                    }
                }
            }
        }

        return NULL;
    }

    /**
     * swap sides to ensure that a simple value stands on the right side
     */
    void ValFormulaCompare::checkSwapSides()
    {
        if (_leftSide.isScalarNumber()) {
            CmplVal s;
            s.moveFrom(_leftSide);
            _leftSide.moveFrom(_rightSide);
            _rightSide.moveFrom(s);

            swap(_compGe, _compLe);
        }

        if (_leftSide.t == TP_OPT_VAR) {
            CmplVal f(TP_FORMULA, new ValFormulaVar(syntaxElem(), _leftSide.optVar()));
            _leftSide.moveFrom(f, true);
        }

        if (_rightSide.t == TP_OPT_VAR) {
            CmplVal f(TP_FORMULA, new ValFormulaVar(syntaxElem(), _rightSide.optVar()));
            _rightSide.moveFrom(f, true);
        }
    }

    /**
     * fills coefficients from this constraint for linear or quadratic model per column
     * @param row			identity number of this row
     * @param coeffs		array to fill with vector of coefficients per column
     * @param vpl           list to fill with products of variables / NULL: no quadratic rows allowed
     */
    void ValFormulaCompare::fillCoeffInLinearModelCol(unsigned long row, vector<LinearModel::Coefficient> *coeffs, QLinearModel::CoefficientVarProdList *vpl)
    {
        if (_leftSide.t == TP_FORMULA) {
            ValFormula *f = _leftSide.valFormula();
            f->fillCoeffInLinearModelColIntern(row, coeffs, true, vpl);
        }

        if (_rightSide.t == TP_FORMULA) {
            ValFormula *f = _rightSide.valFormula();
            f->fillCoeffInLinearModelColIntern(row, coeffs, false, vpl);
        }
    }


    /**
     * fills coefficients from this constraint for linear or quadratic model per row
     * @param row			identity number of this row
     * @param coeffs		vector to fill of coefficients for this row
     * @param vpl           list to fill with products of variables / NULL: no quadratic rows allowed
     */
    void ValFormulaCompare::fillCoeffInLinearModelRow(unsigned long row, vector<LinearModel::Coefficient> *coeffs, QLinearModel::CoefficientVarProdList *vpl)
    {
        map<unsigned long, unsigned long> colMap;

        if (_leftSide.t == TP_FORMULA) {
            ValFormula *f = _leftSide.valFormula();
            f->fillCoeffInLinearModelRowIntern(row, coeffs, colMap, true, vpl);
        }

        if (_rightSide.t == TP_FORMULA) {
            ValFormula *f = _rightSide.valFormula();
            f->fillCoeffInLinearModelRowIntern(row, coeffs, colMap, false, vpl);
        }
    }


    /**
     * fills mode and right hand side of this constraint for linear model
     * @param row			identity number of this row
     * @param mode			mode to fill (valid are: 'N', 'L', 'G', 'E': like in MPS; or '+': maximize, or '-': minimize)
     * @param rhs			right hand side to fill
     * @param qp            quadratic rows allowed
     */
    void ValFormulaCompare::fillModeRhsInLinearModel(unsigned long row, char *mode, LinearModel::Coefficient *rhs, bool qp)
    {
        if (!qp && !linearConstraint())
            throw NonLinearModelException(row);

        if (_rightSide.t == TP_FORMULA) {
            ValFormula *f = _rightSide.valFormula();
            f->fillRhsInLinearModelIntern(row, rhs, false, qp);
        }
        else if (_rightSide.isScalarNumber()) {
            rhs->add(_rightSide);
        }

        if (_leftSide.t == TP_FORMULA) {
            ValFormula *f = _leftSide.valFormula();
            f->fillRhsInLinearModelIntern(row, rhs, true, qp);
        }
        else if (_leftSide.isScalarNumber()) {
            rhs->add(_leftSide, true);
        }

        *mode = (_compGe ? (_compLe ? 'E' : 'G') : (_compLe ? 'L' : 'N'));
    }



    /************** ValFormulaObjective **********/

    /**
     * constructor
     * @param f         formula to minimize or to maximize, should be TP_FORMULA
     * @param ma        true: maximize; false: minimize
     */
    ValFormulaObjective::ValFormulaObjective(unsigned se, CmplVal *f, bool ma): ValFormula(se)
    {
        if (f->t == TP_OPT_VAR)
            _formula.set(TP_FORMULA, new ValFormulaVar(se, f->optVar()));
        else
            _formula.copyFrom(f, true, false);

        _maximize = ma;
    }

    /**
     * get lower and upper bound of the possible value range of this formula
     * @param lb        return of lower bound (TP_INT, TP_REAL, TP_INFINITE, or TP_EMPTY when unknown)
     * @param ub        return of upper bound (TP_INT, TP_REAL, TP_INFINITE, or TP_EMPTY when unknown)
     * @param con       bounds for the constraint using this formula
     */
    void ValFormulaObjective::getBounds(CmplVal& lb, CmplVal& ub, bool con) const
    {
        ValFormula::getBounds(_formula, lb, ub, con);
    }

    /**
     * fills coefficients from this constraint for linear or quadratic model per column
     * @param row			identity number of this row
     * @param coeffs		array to fill with vector of coefficients per column
     * @param vpl           list to fill with products of variables / NULL: no quadratic rows allowed
     */
    void ValFormulaObjective::fillCoeffInLinearModelCol(unsigned long row, vector<LinearModel::Coefficient> *coeffs, QLinearModel::CoefficientVarProdList *vpl)
    {
        if (_formula.t == TP_FORMULA) {
            ValFormula *f = _formula.valFormula();
            f->fillCoeffInLinearModelColIntern(row, coeffs, true, vpl);
        }
    }


    /**
     * fills coefficients from this constraint for linear or quadratic model per row
     * @param row			identity number of this row
     * @param coeffs		vector to fill of coefficients for this row
     * @param vpl           list to fill with products of variables / NULL: no quadratic rows allowed
     */
    void ValFormulaObjective::fillCoeffInLinearModelRow(unsigned long row, vector<LinearModel::Coefficient> *coeffs, QLinearModel::CoefficientVarProdList *vpl)
    {
        map<unsigned long, unsigned long> colMap;

        if (_formula.t == TP_FORMULA) {
            ValFormula *f = _formula.valFormula();
            f->fillCoeffInLinearModelRowIntern(row, coeffs, colMap, true, vpl);
        }
    }


    /**
     * fills mode and right hand side of this constraint for linear model
     * @param row			identity number of this row
     * @param mode			mode to fill (valid are: 'N', 'L', 'G', 'E': like in MPS; or '+': maximize, or '-': minimize)
     * @param rhs			right hand side to fill
     * @param qp            quadratic rows allowed
     */
    void ValFormulaObjective::fillModeRhsInLinearModel(unsigned long row, char *mode, LinearModel::Coefficient *rhs, bool qp)
    {
        if (!qp && !linearConstraint())
            throw NonLinearModelException(row);

        if (_formula.t == TP_FORMULA) {
            ValFormula *f = _formula.valFormula();
            f->fillRhsInLinearModelIntern(row, rhs, true, qp);
        }

        *mode = (_maximize ? '+' : '-');
    }



    /************** ValFormulaLogCon **********/

    /**
     * get lower and upper bound of the possible value range of this formula
     * @param lb        return of lower bound (TP_INT, TP_REAL, TP_INFINITE, or TP_EMPTY when unknown)
     * @param ub        return of upper bound (TP_INT, TP_REAL, TP_INFINITE, or TP_EMPTY when unknown)
     * @param con       bounds for the constraint using this formula
     */
    void ValFormulaLogCon::getBounds(CmplVal& lb, CmplVal& ub, bool con) const
    {
        if (con) {
            //TODO: gibt es hier irgendwie sinnvolle Werte?
            // zumindest Fall beidseitig beschraenkter Restriktion gesondert beruecksichtigen (der hier als 2 Und-verknuepfte Vergleichsformeln erscheint)
            lb.dispUnset();
            ub.dispUnset();
        }
        else {
            lb.dispSet(TP_INT, 0);
            ub.dispSet(TP_INT, 1);
        }
    }

    /**
     * set model properties from this constraint
     * @param prop          properties of optimization model
     */
    void ValFormulaLogCon::setModelProperties(OptModel::Properties& prop) const
    {
        int pcp = prop.conditions;

        for (unsigned i = 0; i < _formulas.size(); i++) {
            _formulas[i].valFormula()->setModelProperties(prop);
        }

        if (_logNeg || _logOr || (!pcp && prop.conditions == -1))
            prop.conditions = 1;
        else if (!prop.conditions)
            prop.conditions = -1;
    }

    /**
     * return whether this formula can be expressed by a single binary variable (only implemented for formulas with isBool() == true)
     * @param neg           return if the binary variable negated
     * @return              binary variable / NULL: other formula
     */
    OptVar* ValFormulaLogCon::asSingleBin(bool& neg)
    {
        if (_formulas.size() == 1) {
            CmplVal& f = _formulas[0];
            if (f.t == TP_FORMULA) {
                ValFormula *frml = f.valFormula();
                if (frml->isBool()) {
                    bool ineg;
                    OptVar *sbv = frml->asSingleBin(ineg);
                    if (sbv) {
                        neg = (ineg != _logNeg);
                        return sbv;
                    }
                }
            }
        }

        return NULL;
    }

    /**
     * write contents of the object to a stream
     * @param modp			calling module
     * @param mode			mode for output: 0=direct; 1=part of other value
     */
    void ValFormulaLogCon::write(ostream& ostr, ModuleBase *modp, int mode) const
    {
        ostr << "<f-log: ";

        if (_logNeg)
            ostr << "!(";

        for (unsigned i = 0; i < _formulas.size(); i++) {
            if (i > 0)
                ostr << (_logOr ? " || " : " && ");
            _formulas[i].write(ostr, modp, 1);
        }

        if (_logNeg)
            ostr << ")";

        ostr << '>';
    }



    /************** ValFormulaCond **********/

    /**
     * check whether conditions are the same as in another part
     */
    bool ValFormulaCond::Part::eqCond(Part& p2)
    {
        //TODO: inhaltlich vergleichen, nicht nur auf Objektidentitaet

        if (_posCond != p2._posCond)
            return false;

        if (_negConds.size() != p2._negConds.size())
            return false;

        for (unsigned i = 0; i < _negConds.size(); i++) {
            if (_negConds[i] != p2._negConds[i])
                return false;
        }

        return true;
    }

    /**
     * get lower and upper bound of the possible value range of this formula
     * @param lb        return of lower bound (TP_INT, TP_REAL, TP_INFINITE, or TP_EMPTY when unknown)
     * @param ub        return of upper bound (TP_INT, TP_REAL, TP_INFINITE, or TP_EMPTY when unknown)
     * @param con       bounds for the constraint using this formula
     */
    void ValFormulaCond::getBounds(CmplVal& lb, CmplVal& ub, bool con) const
    {
        lb.dispUnset();
        ub.dispUnset();

        for (unsigned i = 0; i < _parts.size(); i++) {
            CmplValAuto plb, pub;
            const Part *p = dynamic_cast<Part *>(_parts[i].valFormula());
            const CmplVal& pv = p->_val;

            if (pv.t == TP_FORMULA) {
                pv.valFormula()->getBounds(plb, pub, con);
            }
            else if (pv.isScalarNumber()) {
                if (con) {
                    plb.dispSet(TP_INT, 0);
                    pub.dispSet(TP_INT, 0);
                }
                else {
                    plb.copyFrom(pv);
                    pub.copyFrom(pv);
                }
            }

            if (!lb || (plb && plb.numCmp(lb) < 0))
                lb.moveFrom(plb, true);
            if (!ub || (pub && pub.numCmp(ub) > 0))
                ub.moveFrom(pub, true);
        }

        if (!_complete && lb && ub) {
            // extend range to 0
            if (lb.numCmp(0) >= 0)
                lb.dispSet(TP_INT, 0);
            else if (ub.numCmp(0) <= 0)
                ub.dispSet(TP_INT, 0);
        }
    }

    /**
     * set model properties from this constraint
     * @param prop          properties of optimization model
     */
    void ValFormulaCond::setModelProperties(OptModel::Properties& prop) const
    {
        if (prop.conditions <= 0)
            prop.conditions = 1;

        //TODO: wenn Teil keine Formel, sondern Einzelvariable ist, dann entsprechend behandeln
        //          (vielleicht am besten direkt in OptVar auch passende Methode)
        for (unsigned i = 0; i < _parts.size(); i++) {
            const Part *p = dynamic_cast<Part *>(_parts[i].valFormula());

            if (p->_posCond.t == TP_FORMULA)
                p->_posCond.valFormula()->setModelProperties(prop);

            for (unsigned n = 0; n < p->_negConds.size(); n++) {
                const CmplVal& nc = p->_negConds[n];
                if (nc.t == TP_FORMULA)
                    nc.valFormula()->setModelProperties(prop);
            }

            if (p->_val.t == TP_FORMULA)
                p->_val.valFormula()->setModelProperties(prop);
        }
    }

    /**
     * write contents of the object to a stream
     * @param modp			calling module
     * @param mode			mode for output: 0=direct; 1=part of other value
     */
    void ValFormulaCond::write(ostream& ostr, ModuleBase *modp, int mode) const
    {
        ostr << "<f-cond: ";

        for (unsigned i = 0; i < _parts.size(); i++) {
            if (i > 0)
                ostr << ", ";

            const Part *p = dynamic_cast<Part *>(_parts[i].valFormula());
            ostr << '(';

            if (p->_posCond)
                p->_posCond.write(ostr, modp, 1);

            for (unsigned n = 0; n < p->_negConds.size(); n++) {
                if (n > 0 || p->_posCond)
                    ostr << " && ";

                ostr << '!';
                p->_negConds[n].write(ostr, modp, 1);
            }

            ostr << " -> ";
            p->_val.write(ostr, modp, 1);

            ostr << ')';
        }

        ostr << '>';
    }

}

