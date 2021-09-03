
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

#include <cmath>

#include "LinearVarProd.hh"
#include "LinearVarProdMod.hh"
#include "../../Control/MainControl.hh"
#include "../../Modules/Interpreter/Interpreter.hh"
#include "../../Modules/Interpreter/OperationBase.hh"
#include "../../CommonData/ValFormula.hh"



namespace cmpl
{
    /**
     * constructor
     * @param mod			module creating this extension object
     */
    LinearVarProd::LinearVarProd(LinearVarProdMod *mod): RemodelBase(mod)
    {
        _prodLinearLvl = mod->_prodLinearLvl;
        _prodRealWarn = mod->_prodRealWarn;
        _prodDecomp = mod->_prodDecomp;

        _useBigMBound = mod->_useBigMBound;

        _attachNameVarDecomp = mod->_attachNameVarDecomp;
        _attachNameVarNorm = mod->_attachNameVarNorm;

        _hasWarnedProdReal = false;
    }

    /**
     * run the extension function for processing a command line option
     * @param mod			module calling the extension
     * @param step			execution step within the module
     * @param id			additional identificator
     * @param ref           reference number of option registration, should be used for discriminate the options
     * @param prio          priority value of option
     * @param opt           command line option
     * @param par			additional parameter
     * @return              true if option is used by the extension
     */
    bool LinearVarProd::run(ModuleBase *mod, int step, int id, int ref, int prio, CmdLineOptList::SingleOption *opt, void *par)
    {
        if (RemodelBase::run(mod, step, id, ref, prio, opt, par))
            return true;

        switch (ref) {
            case OPTION_EXT_VARPRODLINEARLVL:
                _prodLinearLvl = (opt->neg() ? 0 : (opt->size() == 0 ? 1 : opt->argAsInt(0, mod->ctrl())));
                return true;

            case OPTION_EXT_PRODREALWARNLVL:
                _prodRealWarn = (opt->neg() ? 0 : (opt->size() == 0 ? 1 : opt->argAsInt(0, mod->ctrl())));
                return true;

            case OPTION_EXT_VARPRODDECOMP:
                _prodDecomp = !(opt->neg());
                return true;

            case OPTION_EXT_USEBIGMBOUND:
                _useBigMBound = !(opt->neg());
                return true;

            case OPTION_EXT_ATTACHNAMEVARDECOMP:
                _attachNameVarDecomp = RemodelBaseMod::parseOptString(mod, opt);
                return true;

            case OPTION_EXT_ATTACHNAMEVARNORM:
                _attachNameVarNorm = RemodelBaseMod::parseOptString(mod, opt);
                return true;
        }

        return false;
    }

	/**
	 * write data object to output stream
	 * @param si        serialization info
	 * @param data      main data object
	 */
	void LinearVarProd::serializeTo(MainData::SerializeInfo& si, const MainData *data)
	{
        // write instance variables
        (*si.ostr) << ';' << _maxThreads << endl;   //TODO
    }

	/**
	 * fill data object from input stream
	 * @param si        serialization info
	 * @param data      main data object
	 * @param subCnt    count of sub data objects
	 * @param lc		line count per element or 0 if elements are len coded
	 * @param rline		remaining section start line
	 */
	void LinearVarProd::deserializeFrom(MainData::SerializeInfo& si, const MainData *data, int subCnt, int lc, string &rline)
	{
        // read instance variables
        _maxThreads = MainData::getNextLong(rline, si.pos);
        //TODO
        si.pos->addLines();
    }



    /**
     * remodel one constraint and objective
     * @param modp      intepreter module calling the extension
     * @param om		optimization model
     * @param oc        constraint and objective
     * @return          count of executed remodelations
     */
    unsigned LinearVarProd::remodelCon(Interpreter *modp, OptModel *om, OptCon *oc)
    {
        CmplVal& rv = oc->value();
        if (rv.t == TP_FORMULA && !(rv.valFormula()->linearConstraint()))
            return remodelRecVarProd(modp, om, rv.valFormula());
        else
            return 0;
    }

    /**
     * search recursive for product of optimization variables and linearize and replace all found
     * @param modp      intepreter module calling the extension
     * @param om		optimization model
     * @param f         formula to search in
     * @return          count of executed remodelations
     */
    unsigned LinearVarProd::remodelRecVarProd(Interpreter *modp, OptModel *om, ValFormula *f)
    {
        unsigned cnt = 0;

        //TODO: Nachbehandlung ValFormulaLinearCombOp ueber checkLinear fehlt, wenn ValFormulaLinearCombOp auf der Hauptebene
        //TODO (anderswo): wenn Zielfunktion ohne Richtung, dann nicht als Zielfunktionstyp dargestellt?

        unsigned pc = f->partCount();
        for (unsigned i = 0; i < pc; i++) {
            CmplVal *p = f->getPart(i);
            if (p->t == TP_FORMULA) {
                ValFormula *pf = p->valFormula();

                ValFormulaVarProdOp *vvp = dynamic_cast<ValFormulaVarProdOp *>(pf);
                if (vvp) {
                    // linearize product and replace formula part with new variable containing the product
                    OptVar *prodVar = linearizeVarProds(modp, om, vvp, cnt);
                    if (prodVar) {
                        CmplVal *fact = vvp->getPart(0);
                        ValFormulaVarOp *prodF = new ValFormulaVarOp(vvp->syntaxElem(), prodVar, *fact);
                        p->dispSet(TP_FORMULA, prodF);
                    }
                }

                else {
                    // recursive search
                    ValFormulaLinearCombBase *vcb = dynamic_cast<ValFormulaLinearCombBase *>(pf);
                    if (!vcb || !(vcb->linearConstraint()))
                        cnt += remodelRecVarProd(modp, om, pf);
                }
            }
        }

        ValFormulaLinearCombOp *vco;
        if (cnt && (vco = dynamic_cast<ValFormulaLinearCombOp *>(f)))
            vco->checkLinear(modp->baseExecCtx());

        return cnt;
    }

    /**
     * linearize product of two or more optimization variables
     * @param modp      intepreter module calling the extension
     * @param om        optimization model
     * @param vvp       formula with product of optimization variables
     * @param cnt       count for linearizations
     * @return          new optimization variable representing the product / NULL: don't replace product
     */
    OptVar *LinearVarProd::linearizeVarProds(Interpreter *modp, OptModel *om, ValFormulaVarProdOp *vvp, unsigned& cnt)
    {
        unsigned cntVars = vvp->partCount() - 1;

        if (cntVars < 1) {
            throw invalid_argument("ValFormulaVarProd contains no variable");
        }
        else if (cntVars == 1) {
            return vvp->getPart(1)->optVar();
        }

        else {
            // store variables in multiset container for sorting
            multiset<OptVar *, bool(*)(OptVar *, OptVar *)> vset(compVarLess);

            for (unsigned i = 1; i < vvp->partCount(); i++) {
                OptVar *v = vvp->getPart(i)->optVar();
                vset.insert(v);
            }

            unsigned linSteps = 0;
            if (_prodLinearLvl > 0) {
                // check variable for given bounds
                // (if !_useBigMBound then all variables must have bounds for linearization, otherwise the last may not have bounds, but only when not two real variables)
                unsigned sz = vset.size();
                unsigned i = 0;
                unsigned decomp = 0;
                bool r2 = false;

                for (OptVar *v: vset) {
                    if (i == 0) {
                        if (_prodLinearLvl < 3 && !v->binVar() && (_prodLinearLvl == 1 || !v->intVar()))
                            break;
                        else
                            decomp = 1;
                    }

                    i++;
                    if (i > decomp)
                        break;

                    if (!v->binVar()) {
                        if (!v->lowBound() || !v->uppBound()) {
                            if (!_useBigMBound || r2 || i < sz) {
                                ostringstream ostr;
                                ostr << "optimization variable ";
                                v->recOutName(ostr, modp->data()->globStrings(), true, true);
                                ostr << " is used in variable product, but has no bounds defined";

                                const SyntaxElement *sep = modp->syntaxElement(vvp->syntaxElem());
                                modp->ctrl()->errHandler().error(ERROR_LVL_NORMAL, ostr.str().c_str(), sep->loc());

                                // don't linearize in case of error
                                decomp = i - 1;
                                break;
                            }
                        }

                        if (_prodLinearLvl >= 3 || (_prodLinearLvl == 2 && v->intVar()))
                            decomp = i + 1;

                        if (i + 1 == sz && !v->intVar())
                            r2 = true;
                    }
                    else {
                        decomp = i + 1;
                    }
                }

                linSteps = (decomp <= 1 ? 0 : (decomp >= sz ? (sz - 1) : (decomp - 1)));
            }

            if (!linSteps && vset.size() == 2)
                return NULL;

            // linearization or decomposition of product
            while (true) {
                // linearize or decomposite product of first and second variable
                auto it = vset.begin();
                OptVar *v1 = *(it++);
                OptVar *v2 = *(it++);

                if (!linSteps && vset.size() == 2) {
                    // replace original variable product in vvp by product of v1 and v2
                    vvp->setNewVarProd(v1, v2, true);
                    return NULL;
                }

                OptVar *vp = linearizeVarProd(modp, om, v1, v2, vvp->syntaxElem(), (linSteps > 0));
                cnt++;
                if (linSteps)
                    linSteps--;

                if (vset.size() == 2) {
                    // product is complete
                    return vp;
                }

                // exchange the both factors by the product and continue
                vset.erase(vset.begin(), it);
                vset.insert(vp);
            }
        }
    }

    /**
     * comparison order of optimization variables for products of variables
     * @param v1        first variable
     * @param v2        second variable
     * @return          true if v1 should be used as first factor in a product
     */
    bool LinearVarProd::compVarLess(OptVar *v1, OptVar *v2)
    {
        // prefer binary variable as first factor
        if (v1->binVar() || v2->binVar()) {
            return (!v2->binVar());
        }
        else {
            // prefer integer variable as first factor
            if (v1->intVar() != v2->intVar())  {
                return (v1->intVar());
            }
            else {
                // prefer variable with bound
                bool b1 = (v1->lowBound() && v1->uppBound());
                bool b2 = (v2->lowBound() && v2->uppBound());
                if (!b1 || !b2) {
                    if (b1 || b2)
                        return b1;
                    else
                        return (v1->id() < v2->id());
                }
                else {
                    // prefer variable with lesser bound range as first factor
                    realType r1 = v1->uppBound().numAsReal() - v1->lowBound().numAsReal();
                    realType r2 = v2->uppBound().numAsReal() - v2->lowBound().numAsReal();
                    return (r1 < r2);
                }
            }
        }
    }

    /**
     * linearize or decomposite product of two optimization variables
     * @param modp      intepreter module calling the extension
     * @param om        optimization model
     * @param v1        first optimization variable
     * @param v2        second optimization variable
     * @param se        id of syntax element in the cmpl text creating the product of variables
     * @param lin       linearize product
     * @return          new optimization variable representing the product
     */
    OptVar *LinearVarProd::linearizeVarProd(Interpreter *modp, OptModel *om, OptVar *v1, OptVar *v2, unsigned se, bool lin)
    {
        RemodelCacheGuard cg;
        VarPair v12 { v1->id(), v2->id() };
        unsigned long prodid = cg.checkPrep(_varProdCache, v12);

        if (prodid)
            return (OptVar *)(om->cols().getElem(prodid));

        OptVar *prodvar = checkLinearizeFixedProd(modp, om, v1, v2, se);
        if (!prodvar) {
            if (lin) {
                if (v1->binVar())
                    prodvar = linearizeProdBin(modp, om, v1, v2, se);
                else if (v1->intVar())
                    prodvar = linearizeProdInt(modp, om, v1, v2, se);
                else
                    prodvar = linearizeProdReal(modp, om, v1, v2, se);
            }
            else {
                prodvar = decompositeProd(modp, om, v1, v2, se);
            }
        }

        prodid = prodvar->id();
        cg.insert(_varProdCache, prodid);

        return prodvar;
    }

    /**
     * check if one variable for the product is fixed (or invalid) by its bounds, and make the product then
     * @param modp      intepreter module calling the extension
     * @param om        optimization model
     * @param v1        first optimization variable
     * @param v2        second optimization variable
     * @param se        id of syntax element in the cmpl text creating the product of variables
     * @return          if one variable is fixed, then new optimization variable representing the product, else NULL
     */
    OptVar *LinearVarProd::checkLinearizeFixedProd(Interpreter *modp, OptModel *om, OptVar *v1, OptVar *v2, unsigned se)
    {
        for (int i = 0; i < 2; i++) {
            OptVar *fv = (i == 0 ? v1 : v2);
            CmplVal& lb = fv->lowBound();
            CmplVal& ub = fv->uppBound();

            if (lb && ub) {
                bool fx;
                if (lb.t == TP_INT && ub.t == TP_INT)
                    fx = (lb.v.i >= ub.v.i);
                else
                    fx = (lb.numAsReal() >= ub.numAsReal());

                if (fx) {
                    OptVar *f2 = (i == 0 ? v2 : v1);

                    // create new product variable
                    CmplValAuto tpl;
                    string nmv = getBaseNameTuple2(modp, v1, v2, tpl);

                    OptVar *pv = newOptVar(modp, om, se, f2->dataType(), NULL, NULL, &nmv, &tpl);
                    setBoundsForProduct(modp->baseExecCtx(), pv, lb, lb, v2->lowBound(), v2->uppBound(), se);
                    CmplValAuto pvf(TP_FORMULA, new ValFormulaVarOp(se, pv));

                    // create constraint for product
                    CmplValAuto f2lb1(TP_FORMULA, new ValFormulaVarOp(se, f2, lb));
                    CmplValAuto fc(TP_FORMULA, new ValFormulaCompareOp(se, &f2lb1, &pvf, true, true, false, true));
                    newOptCon(modp, om, NULL, se, fc);

                    return pv;
                }
            }
        }

        return NULL;
    }

    /**
     * decomposite product of two optimization variables by setting a new variable for the product
     * @param modp      intepreter module calling the extension
     * @param om        optimization model
     * @param v1        first optimization variable
     * @param v2        second optimization variable
     * @param se        id of syntax element in the cmpl text creating the product of variables
     * @return          new optimization variable representing the product
     */
    OptVar *LinearVarProd::decompositeProd(Interpreter *modp, OptModel *om, OptVar *v1, OptVar *v2, unsigned se)
    {
        // create new product variable
        CmplValAuto tpl;
        string nmv = getBaseNameTuple2(modp, v1, v2, tpl);

        OptVar *pv = newOptVar(modp, om, se, v2->dataType(), NULL, NULL, &nmv, &tpl);
        setBoundsForProduct(modp->baseExecCtx(), pv, v1->lowBound(), v1->uppBound(), v2->lowBound(), v2->uppBound(), se);
        CmplValAuto pvf(TP_FORMULA, new ValFormulaVarOp(se, pv));

        // product of v1 and v2
        CmplValAuto vvpf(TP_FORMULA, new ValFormulaVarProdOp(se, v1, v2));

        // prod = v1*v2
        CmplValAuto f(TP_FORMULA, new ValFormulaCompareOp(se, &pvf, &vvpf, true, true, false, true));
        newOptCon(modp, om, NULL, se, f);

        return pv;
    }

    /**
     * linearize product of a binary variable and another variable
     * @param modp      intepreter module calling the extension
     * @param om        optimization model
     * @param v1        binary optimization variable
     * @param v2        other optimization variable
     * @param se        id of syntax element in the cmpl text creating the product of variables
     * @return          new optimization variable representing the product
     */
    OptVar *LinearVarProd::linearizeProdBin(Interpreter *modp, OptModel *om, OptVar *v1, OptVar *v2, unsigned se)
    {
        CmplVal& lb = v2->lowBound();
        CmplVal& ub = v2->uppBound();

        // create new product variable
        CmplValAuto tpl;
        string nmv = getBaseNameTuple2(modp, v1, v2, tpl);

        OptVar *pv = newOptVar(modp, om, se, v2->dataType(), &lb, &ub, &nmv, &tpl);
        CmplValAuto pvf(TP_FORMULA, new ValFormulaVarOp(se, pv));

        CmplValAuto v1lb(TP_FORMULA, (lb ? new ValFormulaVarOp(se, v1, lb) : new ValFormulaVarOp(se, v1, -_bigM)));
        CmplValAuto v1ub(TP_FORMULA, (ub ? new ValFormulaVarOp(se, v1, ub) : new ValFormulaVarOp(se, v1, _bigM)));

        realType lbr = lb.numAsReal(-_bigM);
        if (lbr > 0) {
            pv->lowBound().set(TP_INT, 0);
        }
        else if (lbr < 0) {
            // prod >= lb*v1
            CmplValAuto f(TP_FORMULA, new ValFormulaCompareOp(se, &pvf, &v1lb, true, false, false, true));
            newOptCon(modp, om, NULL, se, f);
        }

        realType ubr = ub.numAsReal(_bigM);
        if (ubr < 0) {
            pv->uppBound().set(TP_INT, 0);
        }
        else if (ubr > 0) {
            // prod <= ub*v1
            CmplValAuto f(TP_FORMULA, new ValFormulaCompareOp(se, &pvf, &v1ub, false, true, false, true));
            newOptCon(modp, om, NULL, se, f);
        }

        CmplVal vBigM(TP_REAL, _bigM);

        // prod <= v2 + lb*(v1 - 1)
        CmplValAuto rhs1(TP_FORMULA, new ValFormulaVarOp(se, v2));
        OperationBase::addTo(modp->baseExecCtx(), &rhs1, se, &v1lb);
        OperationBase::addTo(modp->baseExecCtx(), &rhs1, se, (lb ? &lb : &vBigM), (bool)lb);
        CmplValAuto f1(TP_FORMULA, new ValFormulaCompareOp(se, &pvf, &rhs1, false, true, false, true));
        newOptCon(modp, om, NULL, se, f1);

        // prod >= v2 + ub*(v1 - 1)
        CmplValAuto rhs2(TP_FORMULA, new ValFormulaVarOp(se, v2));
        OperationBase::addTo(modp->baseExecCtx(), &rhs2, se, &v1ub);
        OperationBase::addTo(modp->baseExecCtx(), &rhs2, se, (ub ? &ub : &vBigM), true);
        CmplValAuto f2(TP_FORMULA, new ValFormulaCompareOp(se, &pvf, &rhs2, true, false, false, true));
        newOptCon(modp, om, NULL, se, f2);

        return pv;
    }

    /**
     * linearize product of an integer variable and another variable
     * @param modp      intepreter module calling the extension
     * @param om        optimization model
     * @param v1        integer optimization variable
     * @param v2        other optimization variable
     * @param se        id of syntax element in the cmpl text creating the product of variables
     * @return          new optimization variable representing the product
     */
    OptVar *LinearVarProd::linearizeProdInt(Interpreter *modp, OptModel *om, OptVar *v1, OptVar *v2, unsigned se)
    {
        // binary decomposition of v1
        IntVarDecomp& decomp = binaryDecomp(modp, om, v1, se);

        // create new product variable
        CmplValAuto tpl;
        string nmv = getBaseNameTuple2(modp, v1, v2, tpl);

        OptVar *pv = newOptVar(modp, om, se, v2->dataType(), NULL, NULL, &nmv, &tpl);
        setBoundsForProduct(modp->baseExecCtx(), pv, v1->lowBound(), v1->uppBound(), v2->lowBound(), v2->uppBound(), se);
        CmplValAuto pvf(TP_FORMULA, new ValFormulaVarOp(se, pv));

        // product linearization for every binary variable
        CmplValAuto f(TP_FORMULA, new ValFormulaLinearCombOp(se));
        CmplValAuto v2lb1(TP_FORMULA, new ValFormulaVarOp(se, v2, decomp.lb));
        OperationBase::addTo(modp->baseExecCtx(), &f, se, &v2lb1);

        for (unsigned i = 0; i < decomp.size(); i++) {
            OptVar *bv1 = (OptVar *) om->cols().getElem(decomp.bv[i]);
            OptVar *bpr = linearizeProdBin(modp, om, bv1, v2, se);

            CmplValAuto bprf(TP_FORMULA, new ValFormulaVarOp(se, bpr, (intType)decomp.fact(i)));
            OperationBase::addTo(modp->baseExecCtx(), &f, se, &bprf);
        }

        CmplValAuto fc(TP_FORMULA, new ValFormulaCompareOp(se, &f, &pvf, true, true, false, true));
        newOptCon(modp, om, NULL, se, fc);

        return pv;
    }

    /**
     * binary decomposition of an integer variable
     * @param modp      intepreter module calling the extension
     * @param om        optimization model
     * @param v1        integer optimization variable
     * @param se        id of syntax element in the cmpl text creating the product of variables
     * @param bvar      return of newly created binary variables
     * @return          reference to info for decomposition, stored in cache _intBinCache
     */
    LinearVarProd::IntVarDecomp& LinearVarProd::binaryDecomp(Interpreter *modp, OptModel *om, OptVar *v1, unsigned se)
    {
        RemodelCacheGuard cg;
        IntVarDecomp& decomp = cg.checkPrep(_intBinCache, v1->id());

        if (!decomp) {
            decomp.iv = v1->id();
            decomp.lb = v1->lowBound().numAsInt();
            decomp.ub = v1->uppBound().numAsInt();

            if (decomp.ub > decomp.lb) {
                unsigned bcnt = (unsigned) ceil(log2(decomp.ub - decomp.lb + 1));
                decomp.bv.resize(bcnt);

                CmplValAuto tpl;
                string nmv = getBaseNameTuple(modp, v1, tpl, true, true, _attachNameVarDecomp);

                CmplValAuto tp2;
                Tuple::constructTuple(tp2, tpl, CmplVal(TP_ITUPLE_1INT, (intType)0));
                CmplVal *tpit = (tp2.useValP() ? tp2.tuple()->at(tp2.tuple()->rank() - 1) : &tp2);

                CmplValAuto f(TP_FORMULA, new ValFormulaLinearCombOp(se));
                CmplVal lb1v(TP_INT, decomp.lb);
                OperationBase::addTo(modp->baseExecCtx(), &f, se, &lb1v);

                for (unsigned i = 0; i < bcnt; i++) {
                    tpit->v.i = i + 1;
                    OptVar *bv = newOptVar(modp, om, se, modp->binTypeVar(), NULL, NULL, &nmv, &tp2);
                    decomp.bv[i] = bv->id();

                    CmplValAuto bvf(TP_FORMULA, new ValFormulaVarOp(se, bv, (intType)decomp.fact(i)));
                    OperationBase::addTo(modp->baseExecCtx(), &f, se, &bvf);
                }

                CmplValAuto v1f(TP_FORMULA, new ValFormulaVarOp(se, v1));
                CmplValAuto fc(TP_FORMULA, new ValFormulaCompareOp(se, &f, &v1f, true, true, false, true));
                OptCon *oc = newOptCon(modp, om, NULL, se, fc);
                decomp.oc = oc->id();
            }

            cg.insert(_intBinCache, decomp);
        }

        return decomp;
    }

    /**
     * linearize product of two real optimization variables by an approximation
     * @param modp      intepreter module calling the extension
     * @param om        optimization model
     * @param v1        first optimization variable
     * @param v2        second optimization variable
     * @param se        id of syntax element in the cmpl text creating the product of variables
     * @return          new optimization variable representing the product
     */
    OptVar *LinearVarProd::linearizeProdReal(Interpreter *modp, OptModel *om, OptVar *v1, OptVar *v2, unsigned se)
    {
        if (_prodRealWarn && (_prodRealWarn == 1 || !_hasWarnedProdReal)) {
            _hasWarnedProdReal = true;

            ostringstream ostr;
            ostr << "product of real optimization variables ";
            v1->recOutName(ostr, modp->data()->globStrings(), true, true);
            ostr << " and ";
            v2->recOutName(ostr, modp->data()->globStrings(), true, true);
            ostr << " can only be approximated";

            const SyntaxElement *sep = modp->syntaxElement(se);
            modp->ctrl()->errHandler().error(ERROR_LVL_WARN, ostr.str().c_str(), sep->loc());
        }

        // normalization of both factor variables
        OptVar *vn1, *vn2;
        CmplVal val0(TP_INT, (intType)0);
        CmplVal val1(TP_INT, (intType)1);

        CmplVal& lb1 = v1->lowBound();
        CmplVal& ub1 = v1->uppBound();
        CmplVal& lb2 = v2->lowBound();
        CmplVal& ub2 = v2->uppBound();

        for (int i = 0; i < 2; i++) {
            OptVar *vs = (i == 0 ? v1 : v2);

            RemodelCacheGuard cg;
            unsigned long vnid = cg.checkPrep(_realNormCache, vs->id());

            if (!vnid) {
                CmplVal& lb = (i == 0 ? lb1 : lb2);
                CmplVal& ub = (i == 0 ? ub1 : ub2);

                if (!lb || !ub)
                    modp->ctrl()->errHandler().internalError("factor variable has no bounds");

                OptVar *vn;
                if (lb.isNumNull() && ub.isNumOne()) {
                    vn = vs;
                }
                else {
                    // vs = (ub – lb) * vn + lb;
                    CmplValAuto tpl;
                    string nmv = getBaseNameTuple(modp, vs, tpl, true, true, _attachNameVarNorm);
                    vn = newOptVar(modp, om, se, modp->realTypeVar(), &val0, &val1, &nmv, &tpl);

                    CmplValAuto f(TP_FORMULA, new ValFormulaVarOp(se, vn, (ub.numAsReal() - lb.numAsReal())));
                    OperationBase::addTo(modp->baseExecCtx(), &f, se, &lb);

                    CmplValAuto vsf(TP_FORMULA, new ValFormulaVarOp(se, vs));
                    CmplValAuto fc(TP_FORMULA, new ValFormulaCompareOp(se, &f, &vsf, true, true, false, true));
                    newOptCon(modp, om, NULL, se, fc);
                }

                vnid = vn->id();
                cg.insert(_realNormCache, vnid);
            }

            (i == 0 ? vn1 : vn2) = (OptVar *)(om->cols().getElem(vnid));
        }

        // approximation of normalized product: vpn <= vn1; vpn <= vn2; vpn >= vn1 + vn2 - 1
        CmplValAuto tplp;
        string nmvp = getBaseNameTuple2(modp, v1, v2, tplp);

        bool denormprod = (!lb1.isNumNull() || !lb2.isNumNull() || !ub1.isNumOne() || !ub2.isNumOne());
        string nmvpn = (denormprod ? attachName(modp, nmvp, true, _attachNameVarNorm) : nmvp);

        OptVar *vpn = newOptVar(modp, om, se, modp->realTypeVar(), &val0, &val1, &nmvpn, &tplp);
        CmplValAuto vpnf(TP_FORMULA, new ValFormulaVarOp(se, vpn));
        CmplValAuto v1nf(TP_FORMULA, new ValFormulaVarOp(se, vn1));
        CmplValAuto v2nf(TP_FORMULA, new ValFormulaVarOp(se, vn2));

        CmplValAuto fcn1(TP_FORMULA, new ValFormulaCompareOp(se, &vpnf, &v1nf, false, true, false, true));
        newOptCon(modp, om, NULL, se, fcn1);

        CmplValAuto fcn2(TP_FORMULA, new ValFormulaCompareOp(se, &vpnf, &v2nf, false, true, false, true));
        newOptCon(modp, om, NULL, se, fcn2);

        CmplValAuto fn(TP_FORMULA, new ValFormulaLinearCombOp(se));
        OperationBase::addTo(modp->baseExecCtx(), &fn, se, &v1nf);
        OperationBase::addTo(modp->baseExecCtx(), &fn, se, &v2nf);
        OperationBase::addTo(modp->baseExecCtx(), &fn, se, &val1, true);

        CmplValAuto fcn3(TP_FORMULA, new ValFormulaCompareOp(se, &vpnf, &fn, true, false, false, true));
        newOptCon(modp, om, NULL, se, fcn3);

        // de-normalization of normalized product
        OptVar *vp;
        if (denormprod)
        {
            // vp = lb1*v2 + lb2*v1 + (ub1 – lb1)*(ub2 – lb2)*vpn – lb1*lb2;
            vp = newOptVar(modp, om, se, modp->realTypeVar(), NULL, NULL, &nmvp, &tplp);
            setBoundsForProduct(modp->baseExecCtx(), vp, lb1, ub1, lb2, ub2, se);

            CmplValAuto fp(TP_FORMULA, new ValFormulaLinearCombOp(se));

            CmplValAuto v2lb1(TP_FORMULA, new ValFormulaVarOp(se, v2, lb1));
            OperationBase::addTo(modp->baseExecCtx(), &fp, se, &v2lb1);

            CmplValAuto v1lb2(TP_FORMULA, new ValFormulaVarOp(se, v1, lb2));
            OperationBase::addTo(modp->baseExecCtx(), &fp, se, &v1lb2);

            realType ublb = (ub1.numAsReal() - lb1.numAsReal()) * (ub2.numAsReal() - lb2.numAsReal());
            CmplValAuto vpnublb(TP_FORMULA, new ValFormulaVarOp(se, vpn, ublb));
            OperationBase::addTo(modp->baseExecCtx(), &fp, se, &vpnublb);

            CmplVal lblb(TP_REAL, lb1.numAsReal() * lb2.numAsReal());
            OperationBase::addTo(modp->baseExecCtx(), &fp, se, &lblb, true);

            CmplValAuto vpf(TP_FORMULA, new ValFormulaVarOp(se, vp));
            CmplValAuto fcp(TP_FORMULA, new ValFormulaCompareOp(se, &vpf, &fp, true, true, false, true));
            newOptCon(modp, om, NULL, se, fcp);

            // insert also product variable and normalized product variable in cache
            RemodelCacheGuard cge;
            cge.checkPrep(_realNormCache, vp->id());
            unsigned long vpnid = vpn->id();
            cge.insert(_realNormCache, vpnid);
        }
        else {
            vp = vpn;
        }

        return vp;
    }

    /**
     * set lower and upper bound for product variable
     * @param ctx		execution context
     * @param pv        new optimization variable representing the product
     * @param lb1       first lower bound or TP_EMPTY if no such bound
     * @param ub1       first upper bound or TP_EMPTY if no such bound
     * @param lb2       second lower bound or TP_EMPTY if no such bound
     * @param ub2       second upper bound or TP_EMPTY if no such bound
     * @param se        id of syntax element in the cmpl text creating the product of variables
     */
    void LinearVarProd::setBoundsForProduct(ExecContext *ctx, OptVar *pv, CmplVal& lb1, CmplVal& ub1, CmplVal& lb2, CmplVal& ub2, unsigned se)
    {
        if (lb1.numAsReal(-1) >= 0 && lb2.numAsReal(-1) >= 0) {
            OperationBase::multN(ctx, &(pv->lowBound()), se, &lb1, &lb2);

            if (ub1 && ub2)
                OperationBase::multN(ctx, &(pv->uppBound()), se, &ub1, &ub2);
        }
        else {
            bool noLb = ((!lb1 && ub2.numAsReal(1) > 0) || (!lb2 && ub1.numAsReal(1) > 0) || (!ub1 && lb2.numAsReal(-1) < 0) || (!ub2 && lb1.numAsReal(-1) < 0));
            bool noUb = ((!ub1 && ub2.numAsReal(1) > 0) || (!ub2 && ub1.numAsReal(1) > 0) || (!lb1 && lb2.numAsReal(-1) < 0) || (!lb2 && lb1.numAsReal(-1) < 0));

            if (!noLb || !noUb) {
                CmplVal mi(TP_EMPTY);
                CmplVal ma(TP_EMPTY);

                for (int i1 = 0; i1 < 2; i1++) {
                    CmplVal& b1 = (i1 == 0 ? lb1 : ub1);
                    if (b1) {
                        for (int i2 = 0; i2 < 2; i2++) {
                            CmplVal& b2 = (i2 == 0 ? lb2 : ub2);
                            if (b2) {
                                CmplVal p;
                                OperationBase::multN(ctx, &p, se, &b1, &b2);

                                if (!mi || p.numAsReal() < mi.numAsReal())
                                    mi.copyFrom(p);

                                if (!ma || p.numAsReal() > ma.numAsReal())
                                    ma.copyFrom(p);
                            }
                        }
                    }
                }

                if (!noLb)
                    pv->lowBound().copyFrom(mi);
                if (!noUb)
                    pv->uppBound().copyFrom(ma);
            }
        }
    }

}


