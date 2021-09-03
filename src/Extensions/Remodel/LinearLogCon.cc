
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


#include "LinearLogCon.hh"
#include "LinearLogConMod.hh"
#include "../../Control/MainControl.hh"
#include "../../Modules/Precompiler/Precompiler.hh"
#include "../../Modules/Interpreter/Interpreter.hh"
#include "../../Modules/Interpreter/ValType.hh"
#include "../../Modules/Interpreter/OperationBase.hh"
#include "../../CommonData/ValFormula.hh"



namespace cmpl
{
    /**
     * constructor
     * @param mod			module creating this extension object
     */
    LinearLogCon::LinearLogCon(LinearLogConMod *mod): RemodelBase(mod)
    {
        _attachNameConAnd = mod->_attachNameConAnd;
        _attachNameConOr = mod->_attachNameConOr;
        _attachNameVarOr = mod->_attachNameVarOr;
        _attachNameConDltNeg = mod->_attachNameConDltNeg;
        _attachNameVarDltNeg = mod->_attachNameVarDltNeg;
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
    bool LinearLogCon::run(ModuleBase *mod, int step, int id, int ref, int prio, CmdLineOptList::SingleOption *opt, void *par)
    {
        if (RemodelBase::run(mod, step, id, ref, prio, opt, par))
            return true;

        switch (ref) {
            case OPTION_EXT_ATTACHNAMECONAND:
                _attachNameConAnd = RemodelBaseMod::parseOptString(mod, opt);
                return true;

            case OPTION_EXT_ATTACHNAMECONOR:
                _attachNameConOr = RemodelBaseMod::parseOptString(mod, opt);
                return true;

            case OPTION_EXT_ATTACHNAMEVAROR:
                _attachNameVarOr = RemodelBaseMod::parseOptString(mod, opt);
                return true;

            case OPTION_EXT_ATTACHNAMECONDLTNEG:
                _attachNameConDltNeg = RemodelBaseMod::parseOptString(mod, opt);
                return true;

            case OPTION_EXT_ATTACHNAMEVARDLTNEG:
                _attachNameVarDltNeg = RemodelBaseMod::parseOptString(mod, opt);
                return true;
        }

        return false;
    }

	/**
	 * write data object to output stream
	 * @param si        serialization info
	 * @param data      main data object
	 */
	void LinearLogCon::serializeTo(MainData::SerializeInfo& si, const MainData *data)
	{
        // write instance variables
        (*si.ostr) << ';' << _maxThreads << ';' << _bigM << ';' << _namePref << ';' << _nameSep << ';' << _attachNameConAnd << ';' << _attachNameConOr << ';' << _attachNameVarOr << ';' << _attachNameConDltNeg << ';' << _attachNameVarDltNeg << endl;
    }

	/**
	 * fill data object from input stream
	 * @param si        serialization info
	 * @param data      main data object
	 * @param subCnt    count of sub data objects
	 * @param lc		line count per element or 0 if elements are len coded
	 * @param rline		remaining section start line
	 */
	void LinearLogCon::deserializeFrom(MainData::SerializeInfo& si, const MainData *data, int subCnt, int lc, string &rline)
	{
        // read instance variables
        _maxThreads = MainData::getNextLong(rline, si.pos);
        _bigM = MainData::getNextDouble(rline, si.pos);
        _namePref = MainData::getNextLong(rline, si.pos);
        _nameSep = MainData::getNextLong(rline, si.pos);
        _attachNameConAnd = MainData::getNextLong(rline, si.pos);
        _attachNameConOr = MainData::getNextLong(rline, si.pos);
        _attachNameVarOr = MainData::getNextLong(rline, si.pos);
        _attachNameConDltNeg = MainData::getNextLong(rline, si.pos);
        _attachNameVarDltNeg = MainData::getNextLong(rline, si.pos);

        si.pos->addLines();
    }


    /**
     * remodel one constraint and objective
     * @param modp      intepreter module calling the extension
     * @param om		optimization model
     * @param oc        constraint and objective
     * @return          count of executed remodelations
     */
    unsigned LinearLogCon::remodelCon(Interpreter *modp, OptModel *om, OptCon *oc)
    {
        if (!oc->objective()) {
            CmplVal& rv = oc->value();

            if (rv.t == TP_FORMULA && rv.valFormula()->isBool()) {
                ValFormulaCompare *fc = dynamic_cast<ValFormulaCompare *>(rv.valFormula());
                if (!fc || fc->isNeg()) {
                    CmplValAuto tpl;
                    string name = getBaseNameTuple(modp, oc, tpl, true);

                    CmplValAuto res;
                    if (remodelRec(modp, om, oc, rv, false, name, tpl, res)) {
                        if (formulaToCon(modp, om, oc, res, name, tpl, _frstReplOC) || !_frstReplOC) {
                            // delete original constraint
                            oc->removeFromValueTree(&(om->rows()));
                            CmplObjBase::dispose(oc);
                        }
                        return 1;
                    }
                }
            }
        }

        return 0;
    }

    /**
     * generate constraints from the linearization result
     * @param modp      intepreter module calling the extension
     * @param om		optimization model
     * @param oc        original constraint
     * @param frml      formula with linearization result
     * @param name      base name for new constraints
     * @param tpl       base tuple for new constraints
     * @param repl      replace original constraint
     * @return          new value for repl
     */
    bool LinearLogCon::formulaToCon(Interpreter *modp, OptModel *om, OptCon *oc, CmplVal& frml, string& name, CmplVal& tpl, bool repl)
    {
        // check for single compare formula
        ValFormulaCompare *fc = dynamic_cast<ValFormulaCompare *>(frml.valFormula());
        if (fc) {
            newOptCon(modp, om, oc, oc->syntaxElem(), frml, (repl ? NULL : &name), &tpl);
            return false;
        }

        // check for single binary variable (negated or not)
        ValFormulaVar *fv = dynamic_cast<ValFormulaVar *>(frml.valFormula());
        ValFormulaLogCon *fl = dynamic_cast<ValFormulaLogCon *>(frml.valFormula());

        if (fv || (fl && fl->logNeg())) {
            CmplVal rhs(TP_INT, (intType)(fv ? 1 : 0));
            CmplValAuto f(TP_FORMULA, new ValFormulaCompareOp(oc->syntaxElem(), (fv ? &frml : &(fl->formulas()[0])), &rhs, (fv != NULL), (fv == NULL), false, true));
            newOptCon(modp, om, oc, oc->syntaxElem(), f, (repl ? NULL : &name), &tpl);
            return false;
        }

        // formula must be and-connected
        if (fl && !(fl->logOr())) {
            vector<CmplVal>& parts = fl->formulas();

            string nm2;
            CmplValAuto tp2;
            CmplVal *tpit = NULL;

            if (!name.empty()) {
                nm2 = name + modp->data()->globStrings()->at(_attachNameConAnd);
                Tuple::constructTuple(tp2, tpl, CmplVal(TP_ITUPLE_1INT, (intType)0));
                if (tp2.useValP())
                    tpit = tp2.tuple()->at(tp2.tuple()->rank() - 1);
            }

            // new constraint for every part of the formula
            for (unsigned i = 0; i < parts.size(); i++) {
                if (tpit)
                    tpit->v.i++;
                else if (tp2.useInt())
                    tp2.v.i++;

                repl = formulaToCon(modp, om, oc, parts[i], nm2, tp2, repl);
            }
        }

        return repl;
    }

    /**
     * recursive linearization of a logical connected expression
     * @param modp      intepreter module calling the extension
     * @param om		optimization model
     * @param oc        original constraint
     * @param frml      formula with logical connected expression
     * @param oneg      outer negation
     * @param name      base name for new constraint and optimization variables / empty: no name
     * @param tpl       base index tuple for new constraint and optimization variables / empty or TP_NULL: no tuple
     * @param res       return of resulting constraint as formula
     * @return          true if ok / false if error
     */
    bool LinearLogCon::remodelRec(Interpreter *modp, OptModel *om, OptCon *oc, CmplVal& frml, bool oneg, string& name, CmplVal& tpl, CmplVal& res)
    {
        // result value res can only be one of:
        //      1. single compare formula (ValFormulaCompare) without negation
        //      2. single binary variable (ValFormulaVar)
        //      3. negated single binary variable (ValFormulaLogCon, negated, with only one part)
        //      4. not negated and-connected formula (ValFormulaLogCon) of parts of the type 1-3 (but not recursive)

        if (frml.t != TP_FORMULA || !(frml.valFormula()->isBool())) {
            outError(modp, oc, "wrong formula type within logical connected constraint at LinearLogCon::remodelRec()", ERROR_LVL_NORMAL);
            return 0;
        }

        bool neg;
        OptVar *sbv = frml.valFormula()->asSingleBin(neg);
        if (sbv) {
            if (neg == oneg)
                res.set(TP_FORMULA, new ValFormulaVarOp(oc->syntaxElem(), sbv));
            else
                res.set(TP_FORMULA, new ValFormulaLogConOp(oc->syntaxElem(), true, new ValFormulaVarOp(oc->syntaxElem(), sbv)));

            return true;
        }

        ValFormulaLogCon *fl = dynamic_cast<ValFormulaLogCon *>(frml.valFormula());
        if (fl) {
            neg = (fl->logNeg() ? !oneg : oneg);
            vector<CmplVal>& parts = fl->formulas();

            if (!parts.size()) {
                outError(modp, oc, "empty formula part within logical connected constraint at LinearLogCon::remodelRec()", ERROR_LVL_NORMAL);
                return false;
            }

            else if (parts.size() == 1)
                return remodelRec(modp, om, oc, parts[0], neg, name, tpl, res);
            else if (neg == fl->logOr())
                return linearizeAnd(modp, om, oc, parts, neg, name, tpl, res);
            else
                return linearizeOr(modp, om, oc, parts, neg, name, tpl, res);
        }

        ValFormulaCompare *fc = dynamic_cast<ValFormulaCompare *>(frml.valFormula());
        if (fc) {
            neg = (fc->isNeg() ? !oneg : oneg);
            if (!neg) {
                res.copyFrom(frml, true, false);
                return true;
            }
            else {
                return linearizeNeg(modp, om, oc, fc, name, tpl, res);
            }
        }

        ValFormulaVar *fv = dynamic_cast<ValFormulaVar *>(frml.valFormula());
        if (fv) {
            if (!oneg)
                res.copyFrom(frml, true, false);
            else
                res.set(TP_FORMULA, new ValFormulaLogConOp(oc->syntaxElem(), true, fv));

            return true;
        }

        outError(modp, oc, "wrong formula type within logical connected constraint at LinearLogCon::remodelRec()", ERROR_LVL_NORMAL);
        return false;
    }

    /**
     * linearization of parts connected by logical And
     * @param modp      intepreter module calling the extension
     * @param om		optimization model
     * @param oc        original constraint
     * @param parts     parts connected by logical And
     * @param neg       negation of every part
     * @param name      base name for new constraint and optimization variables / empty: no name
     * @param tpl       base index tuple for new constraint and optimization variables / empty or TP_NULL: no tuple
     * @param res       return of resulting constraint as formula
     * @return          true if ok / false if error
     */
    bool LinearLogCon::linearizeAnd(Interpreter *modp, OptModel *om, OptCon *oc, vector<CmplVal>& parts, bool neg, string& name, CmplVal& tpl, CmplVal& res)
    {
        // all parts are connected by logical And: linearize them separatly and construct a new ValFormulaLogCon of the result
        ValFormulaLogCon *fr = new ValFormulaLogConOp(oc->syntaxElem(), false, false);
        res.set(TP_FORMULA, fr);

        string nm2;
        CmplValAuto tp2;
        CmplVal *tpit = NULL;

        if (!name.empty()) {
            nm2 = name + modp->data()->globStrings()->at(_attachNameConAnd);
            Tuple::constructTuple(tp2, tpl, CmplVal(TP_ITUPLE_1INT, (intType)0));
            if (tp2.useValP())
                tpit = tp2.tuple()->at(tp2.tuple()->rank() - 1);
        }

        for (unsigned i = 0; i < parts.size(); i++) {
            if (tpit)
                tpit->v.i++;
            else if (tp2.useInt())
                tp2.v.i++;

            CmplValAuto resp;
            if (!remodelRec(modp, om, oc, parts[i], neg, nm2, tp2, resp))
                return false;

            fr->formulas().push_back(resp);
        }

        return true;
    }

    /**
     * linearization of parts connected by logical Or
     * @param modp      intepreter module calling the extension
     * @param om		optimization model
     * @param oc        original constraint
     * @param parts     parts connected by logical Or
     * @param neg       negation of every part
     * @param name      base name for new constraint and optimization variables / empty: no name
     * @param tpl       base index tuple for new constraint and optimization variables / empty or TP_NULL: no tuple
     * @param res       return of resulting constraint as formula
     * @return          true if ok / false if error
     */
    bool LinearLogCon::linearizeOr(Interpreter *modp, OptModel *om, OptCon *oc, vector<CmplVal>& parts, bool neg, string& name, CmplVal& tpl, CmplVal& res)
    {
        string nm2, nmv;
        CmplValAuto tp2;
        CmplVal *tpit = NULL;

        if (!name.empty()) {
            nm2 = name + modp->data()->globStrings()->at(_attachNameConOr);
            nmv = name + modp->data()->globStrings()->at(_attachNameVarOr);

            Tuple::constructTuple(tp2, tpl, CmplVal(TP_ITUPLE_1INT, (intType)0));
            if (tp2.useValP())
                tpit = tp2.tuple()->at(tp2.tuple()->rank() - 1);
        }

        // generate new binary variable for every part and increase all formulas in the part by it
        CmplValAuto resf;
        for (unsigned i = 0; i < parts.size(); i++) {
            CmplVal& pfrml = parts[i];

            RemodelCacheGuard cg;
            ValFormula *cacheKey = (pfrml.t == TP_FORMULA ? pfrml.valFormula() : NULL);
            OptVar *bv = (cacheKey ? cg.checkPrep(_linOrCache, cacheKey) : NULL);
            CmplValAuto vbv;

            if (bv) {
                vbv.set(TP_FORMULA, new ValFormulaVarOp(oc->syntaxElem(), bv));
            }
            else {
                if (tpit)
                    tpit->v.i++;
                else if (tp2.useInt())
                    tp2.v.i++;

                CmplValAuto resp;
                if (!remodelRec(modp, om, oc, parts[i], neg, nm2, tp2, resp))
                    return false;

                bv = newOptVar(modp, om, oc->syntaxElem(), modp->binTypeVar(), NULL, NULL, &nmv, &tp2);
                vbv.set(TP_FORMULA, new ValFormulaVarOp(oc->syntaxElem(), bv));
                constraintForOrPart(modp, om, oc, resp, vbv, nm2, tp2);

                if (cg.hasKey())
                    cg.insert(_linOrCache, bv);
            }

            if (i == 0) {
                resf.moveFrom(vbv);
            }
            else {
                CmplValAuto p;
                OperationBase::plus(modp->baseExecCtx(), &p, oc->syntaxElem(), &resf, &vbv);
                resf.moveFrom(p);
            }
        }

        // result formula: sum of all generated binary variables must be lesser than their count
        CmplVal cnt(TP_INT, (intType)(parts.size() - 1));
        res.set(TP_FORMULA, new ValFormulaCompareOp(oc->syntaxElem(), &resf, &cnt, false, true, false, true));
        //res.set(TP_FORMULA, new ValFormulaCompareOp(oc->syntaxElem(), &resf, &cnt, true, true, false, true));

        return true;
    }

    /**
     * generate constraint for linearization of a part of logical Or
     * @param modp      intepreter module calling the extension
     * @param om		optimization model
     * @param oc        original constraint
     * @param frml      formula of the part
     * @param vbv       binary variable to include in the formula of the part
     * @param name      name for new constraint / empty: no name
     * @param tpl       index tuple for new constraint
     */
    void LinearLogCon::constraintForOrPart(Interpreter *modp, OptModel *om, OptCon *oc, CmplVal& frml, CmplVal& vbv, string& name, CmplVal& tpl)
    {
        // if frml consists of other formulas connected by logical And then recursiv call for all parts
        ValFormulaLogCon *fl = dynamic_cast<ValFormulaLogCon *>(frml.valFormula());
        if (fl && !(fl->logOr())) {
            string nm2;
            CmplValAuto tp2;
            CmplVal *tpit = NULL;

            if (!name.empty()) {
                nm2 = name + modp->data()->globStrings()->at(_attachNameConAnd);
                Tuple::constructTuple(tp2, tpl, CmplVal(TP_ITUPLE_1INT, (intType)0));
                if (tp2.useValP())
                    tpit = tp2.tuple()->at(tp2.tuple()->rank() - 1);
            }

            vector<CmplVal>& parts = fl->formulas();
            for (unsigned i = 0; i < parts.size(); i++) {
                if (tpit)
                    tpit->v.i++;
                else if (tp2.useInt())
                    tp2.v.i++;

                constraintForOrPart(modp, om, oc, parts[i], vbv, nm2, tp2);
            }
        }

        else
        {
            CmplValAuto lhs, rhs, fact;
            bool ge, eq, ag;

            ValFormulaVar *fv = dynamic_cast<ValFormulaVar *>(frml.valFormula());
            if (fv || (fl && fl->logNeg())) {
                // frml is single binary variable (negated or not)
                ge = (fl == NULL);
                eq = false;
                lhs.copyFrom((fv ? frml : fl->formulas()[0]), true, false);
                rhs.set(TP_INT, (intType)(ge ? 1 : 0));
                fact.set(TP_INT, (intType)(ge ? 1 : -1));
                ag = true;
            }
            else {
                // frml must be compare formula
                ValFormulaCompare *fc = dynamic_cast<ValFormulaCompare *>(frml.valFormula());
                ge = fc->isCompGe();
                eq = (ge && fc->isCompLe());
                fc->getSide(true, lhs);
                fc->getSide(false, rhs);
                fact.set(TP_REAL, (ge ? _bigM : -_bigM));
                ag = fc->autogen();
            }

            CmplValAuto fvbv, nlhs;
            OperationBase::mult(modp->baseExecCtx(), &fvbv, oc->syntaxElem(), &vbv, &fact, true);
            OperationBase::plus(modp->baseExecCtx(), &nlhs, oc->syntaxElem(), &lhs, &fvbv);
            CmplValAuto f(TP_FORMULA, new ValFormulaCompareOp(oc->syntaxElem(), &nlhs, &rhs, ge, !ge, false, ag));

            if (!eq) {
                newOptCon(modp, om, oc, oc->syntaxElem(), f, &name, &tpl);
            }
            else {
                CmplValAuto fvbv2, nlhs2;
                fact.set(TP_REAL, -_bigM);
                OperationBase::mult(modp->baseExecCtx(), &fvbv2, oc->syntaxElem(), &vbv, &fact, true);
                OperationBase::plus(modp->baseExecCtx(), &nlhs2, oc->syntaxElem(), &lhs, &fvbv2);
                CmplValAuto f2(TP_FORMULA, new ValFormulaCompareOp(oc->syntaxElem(), &nlhs2, &rhs, false, true, false, ag));

                string nm2;
                CmplValAuto tp2;
                CmplVal *tpit = NULL;

                if (!name.empty()) {
                    nm2 = name + modp->data()->globStrings()->at(_attachNameConAnd);
                    Tuple::constructTuple(tp2, tpl, CmplVal(TP_ITUPLE_1INT, (intType)1));
                    if (tp2.useValP())
                        tpit = tp2.tuple()->at(tp2.tuple()->rank() - 1);
                }

                newOptCon(modp, om, oc, oc->syntaxElem(), f, &nm2, &tp2);

                if (tpit)
                    tpit->v.i++;
                else if (tp2.useInt())
                    tp2.v.i++;

                newOptCon(modp, om, oc, oc->syntaxElem(), f2, &nm2, &tp2);
            }
        }
    }

    /**
     * linearization of negated comparison formula
     * @param modp      intepreter module calling the extension
     * @param om		optimization model
     * @param oc        original constraint
     * @param fc        comparison formula to negate
     * @param name      base name for new constraint and optimization variables / empty: no name
     * @param tpl       base index tuple for new constraint and optimization variables / empty or TP_NULL: no tuple
     * @param res       return of resulting constraint as formula
     * @return          true if ok / false if error
     */
    bool LinearLogCon::linearizeNeg(Interpreter *modp, OptModel *om, OptCon *oc, ValFormulaCompare *fc, string& name, CmplVal& tpl, CmplVal& res)
    {
        if (fc->isCompGe() && fc->isCompLe()) {
            // linearize as:  !(lhs >= rhs) || !(lhs <= rhs)
            vector<CmplVal> parts;
            parts.push_back(CmplVal(TP_FORMULA, new ValFormulaCompareOp(oc->syntaxElem(), fc, true, false, true, fc->autogen())));
            parts.push_back(CmplVal(TP_FORMULA, new ValFormulaCompareOp(oc->syntaxElem(), fc, false, true, true, fc->autogen())));
            return linearizeOr(modp, om, oc, parts, false, name, tpl, res);
        }

        else {
            bool ge = fc->isCompGe();

            // generate new optimization variable
            string nm;
            if (_attachNameVarDltNeg && !name.empty())
                nm = name + modp->data()->globStrings()->at(_attachNameVarDltNeg);

            CmplVal lb(TP_INT, (intType)0);
            OptVar *delta = newOptVar(modp, om, oc->syntaxElem(), modp->realTypeVar(), &lb, NULL, &nm, &tpl);

            // constraint for delta variable:  bigM*delta >= 1
            CmplVal one(TP_INT, (intType)1);
            CmplValAuto vd(TP_FORMULA, new ValFormulaVarOp(oc->syntaxElem(), delta, _bigM));
            CmplValAuto cd(TP_FORMULA, new ValFormulaCompareOp(oc->syntaxElem(), &vd, &one, true, false, false, true));

            if (_attachNameConDltNeg && !name.empty())
                nm = name + modp->data()->globStrings()->at(_attachNameConDltNeg);

            newOptCon(modp, om, oc, oc->syntaxElem(), cd, &nm, &tpl);

            // increase comparison formula by delta variable:
            //      !(lhs <= rhs)  ->  lhs - delta >= rhs
            //      !(lhs >= rhs)  ->  lhs + delta <= rhs
            CmplValAuto lhs, rhs, nlhs;
            fc->getSide(true, lhs);
            fc->getSide(false, rhs);

            CmplValAuto dt(TP_FORMULA, new ValFormulaVarOp(oc->syntaxElem(), delta, (intType)(ge ? 1 : -1)));
            OperationBase::plus(modp->baseExecCtx(), &nlhs, oc->syntaxElem(), &lhs, &dt);

            res.set(TP_FORMULA, new ValFormulaCompareOp(oc->syntaxElem(), &nlhs, &rhs, !ge, ge, false, fc->autogen()));
            return true;
        }
    }

}


