
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
            FormulaCacheKey cacheKey(pfrml);
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


    /**
     * linearization of formula or-connected with binary variables
     * @param modp      intepreter module calling the extension
     * @param om		optimization model
     * @param oc        original constraint / NULL: no one
     * @param frm       formula / TP_BIN / TP_EMPTY / NULL: use formula from oc
     * @param bins      or-connected binary variables
     * @param res       return of resulting formula
     * @return          true if ok / false if error
     */
    bool LinearLogCon::linearizeFormulaOrBins(Interpreter *modp, OptModel *om, OptCon *oc, CmplVal *frm, OrConnectedBinVars &bins, CmplVal& res)
    {
        if ((!oc && !frm) || (frm && frm->t != TP_FORMULA && frm->t != TP_BIN && frm->t != TP_EMPTY))
            return false;

        ValFormula *ff = (frm ? (frm->t == TP_FORMULA ? frm->valFormula() : NULL) : oc->formula());
        if (!ff && (!frm || frm->t == TP_FORMULA))
            return false;

        unsigned se = (oc ? oc->syntaxElem() : (ff ? ff->syntaxElem() : 0));

        if (ff) {
            bool fc = (ff ? ff->canonicalForm(true) : true);
            ValFormulaCompareOp *fcmp = dynamic_cast<ValFormulaCompareOp *>(ff);

            if (bins.empty()) {
                res.dispSet(TP_FORMULA, ff);
            }

            else if (fcmp && fc && !fcmp->isNeg()) {
                CmplValAuto bvar;
                bool bneg;

                if (bins.size() == 1 || (bins.size() == 2 && bins[1].second == -1)) {
                    // only one condition in bins, use it directly
                    bneg = (bins[0].second == 0);

                    bvar.set(TP_FORMULA, new ValFormulaVarOp(se, bins[0].first));
                    if (bins.size() == 2) {
                        CmplValAuto bvar2(TP_FORMULA, new ValFormulaVarOp(se, bins[1].first));
                        OperationBase::addTo(modp->baseExecCtx(), &bvar, se, &bvar2);
                    }
                }
                else {
                    bool bp = true;
                    for(unsigned i = 0; i < bins.size(); i++) {
                        if (bins[i].second == 0) {
                            bp = false;
                            break;
                        }
                    }

                    if (bp) {
                        // all conditions in bins are non negated, use it directly
                        bneg = false;

                        bvar.set(TP_FORMULA, new ValFormulaVarOp(se, bins[0].first));
                        for(unsigned i = 1; i < bins.size(); i++) {
                            CmplValAuto bvar2(TP_FORMULA, new ValFormulaVarOp(se, bins[i].first));
                            OperationBase::addTo(modp->baseExecCtx(), &bvar, se, &bvar2);
                        }
                    }
                    else {
                        // make a new binary variable for the or-connected conditions
                        RemodelCacheGuard cg;
                        unsigned long nbvarid = cg.checkPrep(_orBinsCache, bins);
                        if (nbvarid) {
                            bvar.dispSet(TP_OPT_VAR, (OptVar *)(om->cols().getElem(nbvarid)));
                        }
                        else {
                            // bvar_new <= sum(bins)            (but for negated bins[i] use 1-bins[i] within the sum)
                            bvar.set(TP_INT, (intType)0);
                            intType nn = 0;
                            for(unsigned i = 0; i < bins.size(); i++) {
                                CmplValAuto b(TP_FORMULA, new ValFormulaVarOp(se, bins[i].first));
                                bool neg = (bins[i].second == 0 || (bins[i].second == -1 && bins[i-1].second == 0));
                                OperationBase::addTo(modp->baseExecCtx(), &bvar, se, &b, neg);

                                if (bins[i].second == 0)
                                    nn++;
                            }

                            if (nn > 0) {
                                CmplVal nnv(TP_INT, nn);
                                OperationBase::addTo(modp->baseExecCtx(), &bvar, se, &nnv);
                            }

                            CmplVal val0(TP_INT, (intType)0);
                            CmplVal val1(TP_INT, (intType)1);
                            OptVar *nfbvar = newOptVar(modp, om, se, modp->binTypeVar(), &val0, &val1);
                            CmplValAuto nbvar(TP_FORMULA, new ValFormulaVarOp(se, nfbvar));

                            CmplValAuto nc(TP_FORMULA, new ValFormulaCompareOp(se, &nbvar, &bvar, false, true, false, true));
                            newOptCon(modp, om, NULL, se, nc);

                            nbvarid = nfbvar->id();
                            cg.insert(_orBinsCache, nbvarid);

                            bvar.moveFrom(nbvar, true);
                        }
                    }
                }

                CmplValAuto lb, ub;
                fcmp->getBounds(lb, ub, true);

                CmplValAuto lhs, rhs;
                fcmp->getSide(true, lhs);
                fcmp->getSide(false, rhs);

                if (bneg) {
                    // check for neccessary bounds
                    if ((!fcmp->isCompGe() || (lb && lb.numCmp(-_bigM) > 0)) && (!fcmp->isCompLe() || (ub && ub.numCmp(_bigM) < 0))) {
                        if (fcmp->isCompGe()) {
                            // lhs - (rhs + lb)*bvar >= -lb     (if lb > 0 then 0 can be used instead)
                            CmplValAuto nrhs(TP_INT, (intType)0);
                            if (lb.numCmp(0) < 0)
                                nrhs.numAdd(lb, true);

                            CmplValAuto fact(nrhs);
                            fact.numAdd(rhs, true);

                            CmplValAuto fbvar, nlhs;
                            OperationBase::mult(modp->baseExecCtx(), &fbvar, se, &fact, &bvar, true);
                            OperationBase::plus(modp->baseExecCtx(), &nlhs, se, &lhs, &fbvar);

                            res.dispSet(TP_FORMULA, new ValFormulaCompareOp(se, &nlhs, &nrhs, true, false, false));
                        }

                        if (fcmp->isCompLe()) {
                            // lhs - (rhs - ub)*bvar <= ub      (if ub < 0 then 0 can be used instead)
                            CmplValAuto nrhs(TP_INT, (intType)0);
                            if (ub.numCmp(0) > 0)
                                nrhs.numAdd(ub, false);

                            CmplValAuto fact(nrhs);
                            fact.numAdd(rhs, true);

                            CmplValAuto fbvar, nlhs;
                            OperationBase::mult(modp->baseExecCtx(), &fbvar, se, &fact, &bvar, true);
                            OperationBase::plus(modp->baseExecCtx(), &nlhs, se, &lhs, &fbvar);

                            if (!fcmp->isCompGe()) {
                                res.dispSet(TP_FORMULA, new ValFormulaCompareOp(se, &nlhs, &nrhs, false, true, false));
                            }
                            else {
                                CmplValAuto res2(TP_FORMULA, new ValFormulaCompareOp(se, &nlhs, &nrhs, false, true, false));
                                CmplValAuto res1;
                                res1.moveFrom(res);

                                OperationBase::execBinaryOper(modp->baseExecCtx(), &res, se, ICS_OPER_AND, false, &res1, &res2);
                            }
                        }
                    }
                    else {
                        // make a new binary variable as negation
                        RemodelCacheGuard cg;
                        unsigned long nbvarid = cg.checkPrep(_orBinsCache, bins);
                        if (nbvarid) {
                            bvar.dispSet(TP_OPT_VAR, (OptVar *)(om->cols().getElem(nbvarid)));
                        }
                        else {
                            // bvar_new = 1 - bvar
                            CmplVal val0(TP_INT, (intType)0);
                            CmplVal val1(TP_INT, (intType)1);
                            OptVar *nfbvar = newOptVar(modp, om, se, modp->binTypeVar(), &val0, &val1);

                            CmplValAuto nbvar(TP_FORMULA, new ValFormulaVarOp(se, nfbvar));
                            OperationBase::addTo(modp->baseExecCtx(), &nbvar, se, &bvar);

                            CmplValAuto nc(TP_FORMULA, new ValFormulaCompareOp(se, &nbvar, &val1, true, true, false, true));
                            newOptCon(modp, om, NULL, se, nc);

                            nbvarid = nfbvar->id();
                            cg.insert(_orBinsCache, nbvarid);

                            bvar.dispSet(TP_OPT_VAR, nfbvar);
                        }

                        bneg = false;
                    }
                }

                if (!bneg) {
                    if (fcmp->isCompGe()) {
                        // lhs + BigM*bvar >= rhs   (if lb is known then rhs-lb can be used instead of BigM)
                        CmplValAuto fact;
                        if (lb && lb.t != TP_INFINITE) {
                            fact.copyFrom(rhs);
                            if (lb.numCmp(0) < 0)
                                OperationBase::addTo(modp->baseExecCtx(), &fact, se, &lb, true);
                        }
                        else {
                            fact.set(TP_REAL, _bigM);
                        }

                        CmplValAuto fbvar, nlhs;
                        OperationBase::mult(modp->baseExecCtx(), &fbvar, se, &fact, &bvar, true);
                        OperationBase::plus(modp->baseExecCtx(), &nlhs, se, &lhs, &fbvar);

                        res.dispSet(TP_FORMULA, new ValFormulaCompareOp(se, &nlhs, &rhs, true, false, false));
                    }

                    if (fcmp->isCompLe()) {
                        // lhs - BigM*bvar <= rhs   (if ub is known then ub-rhs can be used instead of BigM)
                        CmplValAuto fact;
                        if (ub && ub.t != TP_INFINITE) {
                            if (rhs.numCmp(0) < 0)
                                fact.copyFrom(rhs);
                            else
                                fact.set(TP_INT, (intType)0);

                            OperationBase::addTo(modp->baseExecCtx(), &fact, se, &ub, true);
                        }
                        else {
                            fact.set(TP_REAL, -_bigM);
                        }

                        CmplValAuto fbvar, nlhs;
                        OperationBase::mult(modp->baseExecCtx(), &fbvar, se, &fact, &bvar, true);
                        OperationBase::plus(modp->baseExecCtx(), &nlhs, se, &lhs, &fbvar);

                        if (!fcmp->isCompGe()) {
                            res.dispSet(TP_FORMULA, new ValFormulaCompareOp(se, &nlhs, &rhs, false, true, false));
                        }
                        else {
                            CmplValAuto res2(TP_FORMULA, new ValFormulaCompareOp(se, &nlhs, &rhs, false, true, false));
                            CmplValAuto res1;
                            res1.moveFrom(res);

                            OperationBase::execBinaryOper(modp->baseExecCtx(), &res, se, ICS_OPER_AND, false, &res1, &res2);
                        }
                    }
                }

                return true;
            }
        }

        // express also fullfilment of frm by a binary variable
        CmplValAuto frmbv;
        bool frmneg = false;

        if (frm->t == TP_FORMULA) {
            CmplValAuto rv1, rv2;
            bool rneg;

            int i = linearizeCondToBin(modp, om, NULL, *frm, true, false, false, rv1, rv2, rneg);
            frmneg = !rneg;

            if (i < 0) {
                return false;
            }
            else if (i == 0) {
                frmbv.copyFrom(rv1);
            }
            else {
                frmbv.set(TP_FORMULA, new ValFormulaVarOp(se, rv1.optVar()));
                if (i > 1)
                    OperationBase::addTo(modp->baseExecCtx(), &frmbv, se, &rv2);
            }
        }
        else {
            frmbv.copyFrom(frm);
        }

        if (frmbv.t == TP_BIN && (frmbv.v.i == 0) != frmneg)
            frmbv.unset();

        if (!frmbv && bins.empty()) {
            frmbv.set(TP_BIN, (intType)0);
            frmneg = false;
        }

        if (frmbv.t != TP_BIN) {
            // or-connect frmbv and all variables from bins
            // frmbv + sum(bins) >= 1
            CmplValAuto lhs(TP_INT, (intType)0);
            intType nn = 0;

            if (frmbv) {
                if (!frmneg) {
                    lhs.copyFrom(frmbv);
                }
                else {
                    OperationBase::addTo(modp->baseExecCtx(), &lhs, se, &frmbv, true);
                    nn = 1;
                }
            }

            unsigned long nbvarid = 0;
            {
                // use from cache if already inserted there, but don't insert anything in the cache
                RemodelCacheGuard cg;
                nbvarid = cg.checkPrep(_orBinsCache, bins);
            }

            if (nbvarid) {
                CmplValAuto bvs(TP_FORMULA, new ValFormulaVarOp(se, (OptVar *)(om->cols().getElem(nbvarid))));
                OperationBase::addTo(modp->baseExecCtx(), &lhs, se, &bvs);
            }
            else {
                for(unsigned i = 0; i < bins.size(); i++) {
                    CmplValAuto b(TP_FORMULA, new ValFormulaVarOp(se, bins[i].first));
                    bool neg = (bins[i].second == 0 || (bins[i].second == -1 && bins[i-1].second == 0));
                    OperationBase::addTo(modp->baseExecCtx(), &lhs, se, &b, neg);

                    if (bins[i].second == 0)
                        nn++;
                }
            }

            if (nn > 0) {
                CmplVal nnv(TP_INT, nn);
                OperationBase::addTo(modp->baseExecCtx(), &lhs, se, &nnv);
            }

            CmplVal rhs(TP_INT, (intType)1);
            res.dispSet(TP_FORMULA, new ValFormulaCompareOp(se, &lhs, &rhs, true, false, false, true));
        }

        if (frmbv.t == TP_BIN) {
            if (!frmneg)
                res.copyFrom(frmbv);
            else
                res.dispSet(TP_BIN, (intType)(frmbv.v.i == 0 ? 1 : 0));
        }

        return true;
    }


    /**
     * linearization of a formula to a new binary optimization variable, so that the fulfillment of the formula is expressed by the value of the binary variable
     * @param modp      intepreter module calling the extension
     * @param om		optimization model
     * @param oc        original constraint / NULL: no one
     * @param frm       formula
     * @param oneg      outer negation
     * @param alsoNeg   also non-fulfillment of the formula must be expressed by the value of the binary variable
     * @param fixVar    true: if fulfillment of the formula is already known then generate a fixed binary optimization variable / false: then return as TP_BIN
     * @param res       resulting binary variable (TP_OPT_VAR) (can also be TP_BIN if !fixVar)
     * @param res2      only used if two binary variables are needed, and the sum of it express the fulfillment of the formula, then second resulting binary variable
     * @param negres    if fulfillment of the formula is expressed by value 0 of the binary variable, then true is returned here
     * @return          count of resulting binary variables (0 - 2) / -1: not linearized
     */
    int LinearLogCon::linearizeCondToBin(Interpreter *modp, OptModel *om, OptCon *oc, CmplVal& frm, bool oneg, bool alsoNeg, bool fixVar, CmplVal& res, CmplVal& res2, bool &negres)
    {
        // check for special cases
        if ((frm.t == TP_BIN && !fixVar) || (frm.t == TP_OPT_VAR && frm.optVar()->binVar())) {
            res.copyFrom(frm);
            negres = oneg;
            return (frm.t == TP_OPT_VAR ? 1 : 0);
        }
        else if ((frm.t != TP_FORMULA && frm.t != TP_BIN) || (frm.t == TP_FORMULA && !frm.valFormula()->isBool())) {
            throw invalid_argument("formula has not a binary value");
        }
        else if (frm.t == TP_FORMULA) {
            bool sbneg = false;
            OptVar *sbov = frm.valFormula()->asSingleBin(sbneg);
            if (sbov) {
                res.set(TP_OPT_VAR, sbov);
                negres = (sbneg != oneg);
                return 1;
            }
        }

        // fixed binary variable for given binary value
        if (frm.t == TP_BIN) {
            CondToBinCacheKey fbkey(true, true);
            RemodelCacheGuard cg;
            CondToBinCacheVal& fbval = cg.checkPrep(_condToBinCache, fbkey);

            if (fbval)
                return fbval.getRes(modp, om, this, (oc ? oc->syntaxElem() : 0), (oneg ? frm.v.i != 0 : frm.v.i == 0), fixVar, res, res2, negres);

            CmplVal fbbnd(TP_INT, (intType)1);
            OptVar *fbvar = newOptVar(modp, om, (oc ? oc->syntaxElem() : 0), modp->binTypeVar(), &fbbnd, &fbbnd);

            CmplValAuto bv1(TP_OPT_VAR, fbvar);
            CmplValAuto bv2;

            CondToBinCacheVal fbvalres(bv1, bv2, 1, true);
            cg.insert(_condToBinCache, fbvalres);

            return fbvalres.getRes(modp, om, this, (oc ? oc->syntaxElem() : 0), (oneg ? frm.v.i != 0 : frm.v.i == 0), fixVar, res, res2, negres);
        }

        // frm is TP_FORMULA
        frm.valFormula()->canonicalForm(true);
        unsigned se = (oc ? oc->syntaxElem() : frm.valFormula()->syntaxElem());

        ValFormulaCompareOp *frmcmp = dynamic_cast<ValFormulaCompareOp *>(frm.valFormula());
        ValFormulaLogConOp *frmlog = dynamic_cast<ValFormulaLogConOp *>(frm.valFormula());
        if (!frmcmp && !frmlog)
            throw invalid_argument("formula has invalid type");

        // use cache: check cache entries in fixed order to prevent deadlocks (first non-negated, second negated)
        bool frmneg = (frmcmp ? frmcmp->isNeg() : frmlog->logNeg());
        ValFormula *frmcg1, *frmcg2;
        CmplValAuto frmcgval;

        if (frmcmp)
            frmcmp->notF(modp->baseExecCtx(), &frmcgval, frmcmp->syntaxElem(), true);
        else
            frmlog->notF(modp->baseExecCtx(), &frmcgval, frmlog->syntaxElem(), true);

        if (!frmneg) {
            frmcg1 = ((ValFormula *)frmcmp ?: frmlog);
            frmcg2 = frmcgval.valFormula();
        }
        else {
            frmcg1 = frmcgval.valFormula();
            frmcg2 = ((ValFormula *)frmcmp ?: frmlog);
        }

        CondToBinCacheKey cgkey1(frmcg1, true);
        RemodelCacheGuard cg1;
        CondToBinCacheVal& fbval1 = cg1.checkPrep(_condToBinCache, cgkey1);
        if (fbval1)
            return fbval1.getRes(modp, om, this, se, (frmneg != oneg), fixVar, res, res2, negres);

        CondToBinCacheKey cgkey2(frmcg2, true);
        RemodelCacheGuard cg2;
        CondToBinCacheVal& fbval2 = cg2.checkPrep(_condToBinCache, cgkey2);
        if (fbval2)
            return fbval2.getRes(modp, om, this, se, (frmneg == oneg), fixVar, res, res2, negres);

        CondToBinCacheKey cgkey1f(frmcg1, false);
        RemodelCacheGuard cg1f;
        CondToBinCacheVal& fbval1f = cg1f.checkPrep(_condToBinCache, cgkey1f);

        CondToBinCacheKey cgkey2f(frmcg2, false);
        RemodelCacheGuard cg2f;
        CondToBinCacheVal& fbval2f = cg2f.checkPrep(_condToBinCache, cgkey2f);

        bool lin1 = true;
        bool lin2 = alsoNeg;
        unsigned binval = 0;
        bool resfix = false;
        CmplValAuto bvar1, bvar2;

        if (fbval1f || fbval2f) {
            if ((fbval1f && fbval2f) || (fbval1f ? fbval1f : fbval2f).fixed)
                throw logic_error("CondToBinCache invalid");

            if (!alsoNeg && ((fbval1f && frmneg == oneg) || (fbval2f && frmneg != oneg))) {
                int r = (fbval1f ? fbval1f : fbval2f).getRes(modp, om, this, se, false, fixVar, res, res2, negres);
                (fbval1f ? cg2f : cg1f).release(_condToBinCache);
                cg2.release(_condToBinCache);
                cg1.release(_condToBinCache);
                return r;
            }

            // use binary variable from cache and generate constraints only for non-fulfillment
            bvar1.copyFrom((fbval1f ? fbval1f : fbval2f).var1);
            bvar2.copyFrom((fbval1f ? fbval1f : fbval2f).var2);
            alsoNeg = true;
            lin1 = false;
            lin2 = true;
        }

        // if outer negation then linearize negated formula
        ValFormulaCompareOp *frmlincmp = frmcmp;
        ValFormulaLogConOp *frmlinlog = frmlog;
        if (oneg) {
            frmlincmp = dynamic_cast<ValFormulaCompareOp *>(frmcgval.valFormula());
            frmlinlog = dynamic_cast<ValFormulaLogConOp *>(frmcgval.valFormula());
        }

        // linearization
        if (frmlincmp) {
            alsoNeg = linearizeCmpToBin(modp, om, se, frmlincmp, lin1, lin2, bvar1, bvar2, binval, resfix);
        }
        else {
            alsoNeg = linearizeLogToBin(modp, om, se, frmlinlog, lin1, lin2, bvar1, bvar2, binval, resfix);
        }

        // handle fixed result (result is then expressed in binval only)
        if (resfix) {
            alsoNeg = true;
            if (fixVar) {
                CmplVal fbbnd(TP_INT, (intType)1);
                OptVar *fbvar = newOptVar(modp, om, (oc ? oc->syntaxElem() : 0), modp->binTypeVar(), &fbbnd, &fbbnd);
                bvar1.dispSet(TP_OPT_VAR, fbvar);
            }
        }

        // insert result into cache
        CondToBinCacheVal fbvalres(bvar1, bvar2, binval, resfix);
        if (alsoNeg) {
            CondToBinCacheVal fbvalresneg(bvar1, bvar2, (binval == 0 ? 1 : 0), resfix);

            cg2f.release(_condToBinCache, false);
            cg1f.release(_condToBinCache, false);
            (frmneg != oneg ? cg2 : cg1).insert(_condToBinCache, fbvalres);
            (frmneg != oneg ? cg1 : cg2).insert(_condToBinCache, fbvalresneg);
        }
        else {
            (frmneg != oneg ? cg2f : cg1f).insert(_condToBinCache, fbvalres);
            (frmneg != oneg ? cg1f : cg2f).release(_condToBinCache);
            cg2.release(_condToBinCache);
            cg1.release(_condToBinCache);
        }

        return fbvalres.getRes(modp, om, this, se, false, fixVar, res, res2, negres);

        /*
         * Mögliche Ergebnisse:
         *      a) trivial: fix bool
         *          - ohne Linearisierung
         *          - neue fixierte bin Variable
         *      b) eine neue Binärvariable
         *      c) zwei neue Binärvariable (deren Summe ist drückt Formelerfüllung aus)
         *  jeweils:
         *      Erfüllung ausgedrückt durch 1
         *      oder durch 0
         *
         * TODO:
         *      frm auf Gültigkeit prüfen:
         *          TP_BIN  -> trivial; Ergebnis a), fertig
         *          TP_OPT_VAR  -> nur erlaubt, wenn Datentyp bin; frm stellt direkt das Ergebnis dar, fertig
         *          sonst nur Formel erlaubt, die booleschen Ausdruck darstellt
         *              muss entweder Vergleichsformel oder Logische Verknüpfung von Vergleichsformeln sein
         *      Suche von frm im Cache, wenn gefunden:
         *          Ergebnis noch nicht da (weil gerade durch anderen Thread linearisiert): Warten bis Ergebnis da ist
         *          im Cache bisher !alsoNeg, hier aber alsoNeg verlangt:
         *              Ergebnisvariablen aus Cache verwenden
         *              Weitere Linearisierung ausführen, aber nur noch für die Neg-Seite
         *          im Cache schon so vollständig wie hier verlangt:
         *              Ergebnisvariablen aus dem Cache zurück, fertig
         *          im Cache geeignet einfügen, dass dieser Thread die Linearisierung ausführt
         *      Eigentliche Linearisierung:
         *          Wenn Formel logische Verknüpfung:
         *              Eventuelle Negation eine Ebene nach unten verschieben
         *              Rekursive Aufrufe für die nächstuntere Ebene
         *              Ergebnisse der rekursiven Aufrufe zu Ergebnis zusammensetzen, zu neuer Ergebnisvariable
         *          Sonst:
         *              Formel nötigenfalls noch in Normalform bringen: LinKomb <Relation> Zahlenwert
         *              Schranken von LinKomb holen
         *              Fälle unterscheiden (wie in Dokumentation beschrieben):
         *                  Jeweilige Linearisierung
         *      Ergebnis im Cache eintragen
         *
         * Cache:
         *      Key: Formel + Beidseit-Kennzeichen (also ob auch Nichterfüllung)
         *      Wert: Variable (+ zweite Variable) + Erfüllungswert (also ob 0 oder 1)
         * z.B. wenn Formel frm, neue Variable vb, und Erfüllung durch 1 ausgedrückt:
         *      !alsoNeg: 1 Eintrag:
         *          frm + false  -> vb + <leer> + 1
         *          (aber wenn implizit doch alsoNeg, dann doch so wie bei alsoNeg)
         *      alsoNeg: 2 Einträge:
         *          frm + true  -> vb + <leer> + 1
         *          !frm + true  -> vb + <leer> + 0
         * Darstellung Cache-Teile:
         *      Key:
         *          (erstmal?): ValFormula* + bool + bool      Hash: ((size_t)(ValFormula*)) + (size_t)(1.bool?2:0) + (size_t)(2.bool?1:0)
         *              1.bool: Darf nur gesetzt sein, wenn ValFormula* == NULL; dann: Wert aus direkt als Formel angegebenem TP_BIN
         *              2.bool: Beidseit-Kennzeichen
         *          (später vielleicht): Hash aus Formel bestimmen, so dass gleiche Formeln (Variablen + Koeffizienten + Relation + Operatoren gleich) gleichen Hash
         *                                  Vergleichsfunktion, dass gleiche Formeln gleich
         *      Value:
         *          OptVar* + OptVar* + unsigned                to bool -> (unsigned != 0)
         *                      (beide OptVar* können in gesetztem Eintrag NULL sein, wenn Formel triviales Ergebnis und !fixVar)
         *                      (unsigned kann nur 0 sein (für leeren Eintrag), oder 1 oder 2 (für gesetzten Eintrag, Erfüllungswert für Variable ist unsigned-1))
         * Generell für Cache:
         *      Prüfung auf Eintrag:
         *          Kein Eintrag -> Eintrag anlegen, als in Berechnung (ist damit reserviert)
         *          Fertiger Eintrag -> Ergebnis aus Eintrag verwenden
         *          Reservierter Eintrag -> Warten bis Eintrag seinen Status ändert, dann erneut prüfen (neue Prüfung kann wieder eine der 3 Möglichkeiten ergeben) (macht RemodelCacheGuard sowieso)
         *      Fertigstellen Eintrag:
         *          Ergebnis für Eintrag da -> In Eintrag eintragen; Eintrag als fertig kennzeichnen; Signal dass Statusänderung (macht RemodelCacheGuard sowieso)
         *          Kein Ergebnis für Eintrag -> Eintrag löschen (Reservierung aufheben); Signal dass Statusänderung (macht RemodelCacheGuard sowieso)
         * Bei Prüfung Cache:
         *      auf "frm + true" und auf "!frm + true" prüfen; jeweils nötigenfalls diesen Eintrag als in Berechnung reservieren
         *          dabei Reihenfolge: entsprechend ob "frm" selbst eine negierte Formel ist (direkt eine Negation enthält):
         *              frm ist nicht negiert: 1. frm, 2. !frm
         *              frm ist negiert:       1. !frm, 2. frm
         *              (dadurch ist sichergestellt, dass Threads dieselbe Reihenfolge der Prüfung verwenden, egal ob die Ausgangsformel negiert ist oder nicht)
         *          Wenn erste Prüfung einen schon vorhandenen Eintrag ergibt (nötigenfalls nach Warten):
         *              Ergebnis aus Eintrag verwenden, fertig (gar keine zweite Prüfung)
         *          andernfalls (erste Prüfung hat Eintrag neu reserviert):
         *              auch zweite Prüfung muss keinen Eintrag finden (also Eintrag neu reservieren); sonst Fehler in Cache-Verwaltung
         *      auf "frm + false" und auf "!frm + false" prüfen (Reservierung wäre nicht nötig, schadet aber auch nicht)
         *          (Einfach in dieser Reihenfolge, Einträge können sowieso nicht reserviert sein (weil in Berechnung immer auch "frm + true" und "!frm + true" reserviert wären))
         *          Einträge können nicht beide vorhanden sein; sonst Fehler in Cache-Verwaltung oder Berechnung (weil sonst Einträge "frm + true" und "!frm + true" schon berechnet sein müssten)
         *          Wenn "frm + false" vorhanden und !alsoNeg:
         *              Ergebnis aus Eintrag verwenden; reservierte Einträge "frm + true" und "!frm + true" ohne Ergebnis freigeben; Signal dass Cache geändert; fertig
         *          Wenn "frm + false" vorhanden und alsoNeg:
         *              Variablen aus Eintrag verwenden, nur non-fulfillment muss noch linearisiert werden
         *          Wenn "!frm + false" vorhanden
         *              Variablen aus Eintrag verwenden, noch non-fulfillment für !frm linearisieren; falls !alsoNeg gilt implizit doch alsoNeg
         * Generell bei gefundenem Ergebnis im Cache:
         *      wenn in diesem Ergebnis gar keine Variable:
         *          Formel hat dann direktes Ergebnis TP_BIN, und wurde bisher mit !fixVar linearisiert
         *          Wenn hier !fixVar:
         *              Ergebnis aus Eintrag als TP_BIN zurück
         *          Sonst:
         *              Neue Binärvariable, auf Ergebnis aus Eintrag fixieren
         *              implizit gilt alsoNeg (entsprechend Einträge bei Abschluss für Cache mit neuer Binärvariable neu setzen)
         *              (Linearisierung für frm damit fertig)
         * Bei Abschluss für Cache:
         *      wenn alsoNeg (egal ob im Aufruf vorgegeben oder implizit gesetzt):
         *          Einträge "frm + false" und auf "!frm + false" ohne Ergebnis freigeben
         *          Beide Einträge "frm + true" und auf "!frm + true" als fertig setzen
         *      sonst:
         *          Eintrag "frm + false" als fertig setzen
         *          Eintrag "!frm + false" ohne Ergebnis freigeben
         *          Beide Einträge "frm + true" und auf "!frm + true" ohne Ergebnis freigeben
         *      Signal dass Cache geändert (macht RemodelCacheGuard sowieso)
         */

    }


    /**
     * check whether this is equal to p2
     * @param p2
     * @return
     */
    bool LinearLogCon::CondToBinCacheKey::operator ==(const CondToBinCacheKey& p2) const
    {
        return ((frm == p2.frm || (frm.t == TP_FORMULA && p2.frm.t == TP_FORMULA && frm.valFormula()->equals(p2.frm.valFormula()))) && alsoNeg == p2.alsoNeg);
    }

    /**
     * get hash for a CondToBinCacheKey
     * @param p
     * @return
     */
    size_t LinearLogCon::CondToBinCacheKeyHash::operator() (CondToBinCacheKey const& p) const noexcept
    {
        return ((p.alsoNeg ? 1 : 0) ^ (p.frm.valFormula()->formulahash() << 1));
    }

    /**
     * set result for LinearLogCon::linearizeCondToBin() from this
     * @param modp      intepreter module calling the extension
     * @param om		optimization model
     * @param llc       calling object
     * @param se        syntax element
     * @param neg       set negation of this as result
     * @param fixVar    true: if fulfillment of the formula is already known then generate a fixed binary optimization variable / false: then return as TP_BIN
     * @param res       resulting binary variable (TP_OPT_VAR) (can also be TP_BIN if !fixVar)
     * @param res2      only used if two binary variables are needed, and the sum of it express the fulfillment of the formula, then second resulting binary variable
     * @param negres    if fulfillment of the formula is expressed by value 0 of the binary variable, then true is returned here
     * @return          count of resulting binary variables (0 - 2)
     */
    int LinearLogCon::CondToBinCacheVal::getRes(Interpreter *modp, OptModel *om, LinearLogCon *llc, unsigned se, bool neg, bool fixVar, CmplVal &res, CmplVal &res2, bool &negres) const
    {
        if (!used)
            throw logic_error("use of empty element of CondToBinCache");

        if (fixed) {
            if (!fixVar) {
                res.dispSet(TP_BIN, (binval == (neg ? 0 : 1)));
                res2.dispUnset();
                negres = false;
                return 0;
            }
            else if (!var1) {
                CmplVal fbbnd(TP_INT, (intType)1);
                res.dispSet(TP_OPT_VAR, llc->newOptVar(modp, om, se, modp->binTypeVar(), &fbbnd, &fbbnd));
                res2.dispUnset();
                negres = (binval == (neg ? 1 : 0));
                return 1;
            }
        }

        res.copyFrom(var1);
        res2.copyFrom(var2);
        negres = (binval == (neg ? 1 : 0));

        return (var1 ? (var2 ? 2 : 1) : 0);
    }

    /**
     * linearization of a compare formula to a new binary optimization variable, so that the fulfillment of the formula is expressed by the value of the binary variable
     * @param modp      intepreter module calling the extension
     * @param om		optimization model
     * @param se        syntax element
     * @param frm       formula
     * @param lin1      generate constraints and variables to express fulfillment of the formula
     * @param lin2      generate additional constraints to express non-fulfillment of the formula
     * @param bvar1     resulting binary variable (TP_OPT_VAR)
     * @param bvar2     only used if two binary variables are needed, and the sum of it express the fulfillment of the formula, then second resulting binary variable
     * @param binval    return of the value of the binary variable, which expresses fulfillment of the formula (only 0 or 1)
     * @param resfix    return whether the formula has a fixed result (then bvar1 and bvar2 are not used)
     * @return          whether non-fulfillment of the formula is also expressed by the resulting binary variable
     */
    bool LinearLogCon::linearizeCmpToBin(Interpreter *modp, OptModel *om, unsigned se, ValFormulaCompareOp *frm, bool lin1, bool lin2, CmplVal& bvar1, CmplVal& bvar2, unsigned& binval, bool& resfix)
    {
        // formula must be in canonical form (that is: <linear combination> relation <scalar number>)
        if (!frm->canonicalForm(true))
            throw invalid_argument("formula has invalid type");

        // get parts of formula
        CmplVal *lhsv = frm->getPart(0);
        ValFormula *lhs = lhsv->valFormula();
        CmplVal *rhsv = frm->getPart(1);

        ValFormulaLinearCombBase *lhslk = dynamic_cast<ValFormulaLinearCombBase *>(lhs);
        if (!lhslk || !rhsv->isScalarNumber())
            throw invalid_argument("formula has invalid type");

        bool relGe = frm->isCompGe();
        bool relLe = frm->isCompLe();
        bool relNeg = frm->isNeg();

        CmplVal lb, ub;
        lhs->getBounds(lb, ub, true);

        int cmpLb = (lb ? lb.numCmp(*rhsv) : -1);
        int cmpUb = (ub ? ub.numCmp(*rhsv) : 1);

        // check for fixed result
        if (lb && ub && lb.numCmp(ub) == 0) {
            if (cmpLb == 0)
                binval = (relNeg ? 0 : 1);
            else if (cmpLb < 0)
                binval = (((relLe && !relGe && !relNeg) || (relGe && relNeg)) ? 1 : 0);
            else
                binval = (((relGe && !relLe && !relNeg) || (relLe && relNeg)) ? 1 : 0);

            bvar1.dispUnset();
            bvar2.dispUnset();
            resfix = true;
            return true;
        }
        else if ((cmpLb >= 0 && relGe && !relLe && relNeg) || (cmpLb > 0 && (relLe && !relNeg))
                 || (cmpUb <= 0 && relLe && !relGe && relNeg) || (cmpUb < 0 && relGe && !relNeg)) {
            binval = 0;
            bvar1.dispUnset();
            bvar2.dispUnset();
            resfix = true;
            return true;
        }
        else if ((cmpLb >= 0 && relGe && !relLe && !relNeg) || (cmpLb > 0 && relLe && relNeg)
                 || (cmpUb <= 0 && relLe && !relGe && !relNeg) || (cmpUb < 0 && relGe && relNeg)) {
            binval = 1;
            bvar1.dispUnset();
            bvar2.dispUnset();
            resfix = true;
            return true;
        }

        resfix = false;

        // bounds for new variables
        CmplVal valnull(TP_INT, (intType)0);
        CmplVal valone(TP_INT, (intType)1);

        CmplVal ubrng(TP_INFINITE, (intType)1);
        if (ub && ub.t != TP_INFINITE && lb && lb.t != TP_INFINITE) {
            ubrng.copyFrom(ub);
            ubrng.numAdd(lb, true);
        }

        CmplVal bigrng(TP_REAL, _bigM);
        if (ubrng.t != TP_INFINITE)
            bigrng.copyFrom(ubrng);

        // linearize case: rhs == lb(lhs) || rhs == ub(lhs)
        if (cmpLb == 0 || cmpUb == 0) {
            if (!lin2 && !relNeg)
                lin2 = true;

            if (lin1) {
                bvar1.dispSet(TP_OPT_VAR, newOptVar(modp, om, se, modp->binTypeVar(), &valnull, &valone));
                bvar2.dispUnset();
            }

            CmplValAuto vardiff;
            if (lin2 && ((cmpLb == 0 && !lb.isNumNull()) || (cmpUb == 0 && !ub.isNumNull()) || lhslk->termCnt() != 1))
                vardiff.set(TP_OPT_VAR, newOptVar(modp, om, se, modp->realTypeVar(), &valnull, &ubrng));

            binval = (relNeg ? 1 : 0);

            // (cmpLb == 0 ? (lhs - lb(lhs)) : (ub(lhs) - lhs))
            CmplValAuto nlhsb;
            if (cmpLb == 0)
                OperationBase::minus(modp->baseExecCtx(), &nlhsb, se, lhsv, &lb);
            else
                OperationBase::minus(modp->baseExecCtx(), &nlhsb, se, &ub, lhsv);

            CmplValAuto fvardiff;
            if (vardiff)
                fvardiff.set(TP_FORMULA, new ValFormulaVarOp(se, vardiff.optVar()));

            CmplValAuto fbvar1(TP_FORMULA, new ValFormulaVarOp(se, bvar1.optVar()));

            if (lin1) {
                // (vardiff ? vd : (cmpLb == 0 ? (lhs - lb(lhs)) : (ub(lhs) - lhs))) <= bigrng * bvar
                CmplVal *nlhs = (vardiff ? &fvardiff : &nlhsb);
                CmplValAuto nrhs(TP_FORMULA, new ValFormulaVarOp(se, bvar1.optVar(), bigrng));

                CmplValAuto nc(TP_FORMULA, new ValFormulaCompareOp(se, nlhs, &nrhs, false, true, false, true));
                newOptCon(modp, om, NULL, se, nc);
            }

            if (lin2) {
                if (vardiff) {
                    // (cmpLb == 0 ? (lhs - lb(lhs)) : (ub(lhs) - lhs)) = vardiff;
                    CmplValAuto nc1(TP_FORMULA, new ValFormulaCompareOp(se, &nlhsb, &fvardiff, true, true, false, true));
                    newOptCon(modp, om, NULL, se, nc1);

                    // bigM * vardiff >= bvar;
                    CmplValAuto nlhs(TP_FORMULA, new ValFormulaVarOp(se, vardiff.optVar(), _bigM));
                    CmplValAuto nc2(TP_FORMULA, new ValFormulaCompareOp(se, &nlhs, &fbvar1, true, false, false, true));
                    newOptCon(modp, om, NULL, se, nc2);
                }
                else {
                    // (cmpLb == 0 ? bigM : -bigM) * lhs >= bvar;
                    CmplValAuto nlhs(TP_REAL, (cmpLb == 0 ? _bigM : -_bigM));
                    OperationBase::multTo(modp->baseExecCtx(), &nlhs, se, lhsv);

                    CmplValAuto nc(TP_FORMULA, new ValFormulaCompareOp(se, &nlhs, &fbvar1, true, false, false, true));
                    newOptCon(modp, om, NULL, se, nc);
                }
            }
        }

        // linearize case: lb(lhs) < rhs < ub(lhs)
        else {
            if (!lin2 && (!relNeg || (relLe && relGe)))
                lin2 = true;

            if (lin1) {
                bvar1.dispSet(TP_OPT_VAR, newOptVar(modp, om, se, modp->binTypeVar(), &valnull, &valone));
                if (relLe && relGe)
                    bvar2.dispSet(TP_OPT_VAR, newOptVar(modp, om, se, modp->binTypeVar(), &valnull, &valone));
                else
                    bvar2.dispUnset();
            }

            binval = (relNeg ? 1 : 0);

            // rhs - lb(lhs); or bigM if lb unknown
            CmplValAuto rhs_lb(rhsv);
            if (lb && lb.t != TP_INFINITE)
                rhs_lb.numAdd(lb, true);
            else
                rhs_lb.set(TP_REAL, _bigM);

            // ub(lhs) – rhs; or bigM if ub unknown
            CmplValAuto ub_rhs(ub);
            if (ub && ub.t != TP_INFINITE)
                ub_rhs.numAdd(rhsv, true);
            else
                ub_rhs.set(TP_REAL, _bigM);

            for (unsigned side = (relLe ? 0 : 1); side <= (relGe ? 1 : 0); side++)
            {
                CmplVal& bvar = (side == 1 && bvar2 ? bvar2 : bvar1);

                // side 0: lhs - rhs
                // side 1: rhs - lhs
                CmplValAuto lhsrhs;
                OperationBase::minus(modp->baseExecCtx(), &lhsrhs, se, (side == 0 ? lhsv : rhsv), (side == 0 ? rhsv : lhsv));

                if (lin1) {
                    // side 0: lhs – rhs <= (ub(lhs) – rhs) * bvar
                    // side 1: rhs – lhs <= (rhs - lb(lhs)) * bvar
                    CmplValAuto facbvar(TP_FORMULA, new ValFormulaVarOp(se, bvar.optVar(), (side == 0 ? ub_rhs : rhs_lb)));
                    CmplValAuto nc(TP_FORMULA, new ValFormulaCompareOp(se, &lhsrhs, &facbvar, false, true, false, true));
                    newOptCon(modp, om, NULL, se, nc);
                }

                if (lin2) {
                    CmplValAuto vardiff(TP_OPT_VAR, newOptVar(modp, om, se, modp->realTypeVar(), &valnull, (side == 0 ? &ub_rhs : &rhs_lb)));

                    if ((side == 0 && (!lb || lb.t == TP_INFINITE)) || (side == 1 && (!ub || ub.t == TP_INFINITE))) {
                        // bnvar = 1 - bvar
                        CmplValAuto bnvar(TP_OPT_VAR, newOptVar(modp, om, se, modp->realTypeVar(), &valnull, &valone));
                        CmplValAuto bnlhs(TP_FORMULA, new ValFormulaVarOp(se, bnvar.optVar()));
                        CmplValAuto bnrhs;
                        OperationBase::minus(modp->baseExecCtx(), &bnrhs, se, &valone, &bvar);
                        CmplValAuto ncbn(TP_FORMULA, new ValFormulaCompareOp(se, &bnlhs, &bnrhs, true, true, false, true));
                        newOptCon(modp, om, NULL, se, ncbn);

                        // side 0: lhs – rhs + bigM * bnvar = vardiff
                        // side 1: rhs – lhs + bigM * bnvar = vardiff
                        CmplValAuto facbnvar(TP_FORMULA, new ValFormulaVarOp(se, bnvar.optVar(), _bigM));
                        CmplValAuto nlhs;
                        OperationBase::plus(modp->baseExecCtx(), &nlhs, se, &lhsrhs, &facbnvar);
                        CmplValAuto nrhs(TP_FORMULA, new ValFormulaVarOp(se, vardiff.optVar()));
                        CmplValAuto nc(TP_FORMULA, new ValFormulaCompareOp(se, &nlhs, &nrhs, true, true, false, true));
                        newOptCon(modp, om, NULL, se, nc);
                    }
                    else {
                        // side 0: lhs – rhs + (rhs + 1 – lb(lhs)) * (1 - bvar) = vardiff
                        // side 1: rhs – lhs + (ub(lhs) + 1 - rhs) * (1 - bvar) = vardiff
                        CmplValAuto fact(side == 0 ? rhs_lb : ub_rhs);
                        fact.numAdd(valone);
                        CmplValAuto facbvar(TP_FORMULA, new ValFormulaVarOp(se, bvar.optVar(), fact));
                        CmplValAuto nlhs;
                        OperationBase::minus(modp->baseExecCtx(), &nlhs, se, &lhsrhs, &facbvar);
                        OperationBase::addTo(modp->baseExecCtx(), &nlhs, se, &fact);
                        CmplValAuto nrhs(TP_FORMULA, new ValFormulaVarOp(se, vardiff.optVar()));
                        CmplValAuto nc(TP_FORMULA, new ValFormulaCompareOp(se, &nlhs, &nrhs, true, true, false, true));
                        newOptCon(modp, om, NULL, se, nc);
                    }

                    // bigM * vardiff >= 1
                    CmplValAuto facvardiff(TP_FORMULA, new ValFormulaVarOp(se, vardiff.optVar(), _bigM));
                    CmplValAuto nc(TP_FORMULA, new ValFormulaCompareOp(se, &facvardiff, &valone, true, false, false, true));
                    newOptCon(modp, om, NULL, se, nc);
                }
            }
        }

        return lin2;
    }

    /**
     * linearization of a logical connected formula to a new binary optimization variable, so that the fulfillment of the formula is expressed by the value of the binary variable
     * @param modp      intepreter module calling the extension
     * @param om		optimization model
     * @param se        syntax element
     * @param frm       formula
     * @param lin1      generate constraints and variables to express fulfillment of the formula
     * @param lin2      generate additional constraints to express non-fulfillment of the formula
     * @param bvar1     resulting binary variable (TP_OPT_VAR)
     * @param bvar2     only used if two binary variables are needed, and the sum of it express the fulfillment of the formula, then second resulting binary variable
     * @param binval    return of the value of the binary variable, which expresses fulfillment of the formula (only 0 or 1)
     * @param resfix    return whether the formula has a fixed result (then bvar1 and bvar2 are not used)
     * @return          whether non-fulfillment of the formula is also expressed by the resulting binary variable
     */
    bool LinearLogCon::linearizeLogToBin(Interpreter *modp, OptModel *om, unsigned se, ValFormulaLogConOp *frm, bool lin1, bool lin2, CmplVal& bvar1, CmplVal& bvar2, unsigned& binval, bool& resfix)
    {
        //TODO: vielleicht Formel vor Suche im Cache in eine Art Normalform bringen? (aber nur sinnvoll, wenn nicht Objektidentitaet; sonst unguenstig)

        bool neg = frm->logNeg();
        vector<CmplVal>& parts = frm->formulas();
        if (!parts.size())
            throw invalid_argument("empty formula part within logical connected constraint at LinearLogCon::linearizeLogToBin()");

        bool frmOr = (neg ? !(frm->logOr()) : frm->logOr());
        intType cnt = 0;
        CmplValAuto valnull(TP_INT, (intType)0);
        CmplValAuto valone(TP_INT, (intType)1);
        CmplValAuto frmsum(valnull);
        CmplValAuto br1a, br2a;
        bool negresa;

        for (unsigned i = 0; i < parts.size(); i++)
        {
            CmplValAuto br1, br2;
            bool negres;
            int sr = linearizeCondToBin(modp, om, NULL, parts[i], neg, lin2, false, br1, br2, negres);
            if (sr == -1)
                return -1;

            if (sr == 0) {
                if ((frmOr && negres == (br1.v.i == 0)) || (!frmOr && negres != (br1.v.i == 0))) {
                    // fixed value determines result
                    bvar1.dispUnset();
                    bvar2.dispUnset();
                    binval = (negres == (br1.v.i == 0) ? 1 : 0);
                    resfix = true;
                    return true;
                }
            }
            else {
                if (cnt == 0) {
                    br1a.copyFrom(br1);
                    br2a.copyFrom(br2);
                    negresa = negres;
                }

                cnt++;
                if (negres)
                    OperationBase::addTo(modp->baseExecCtx(), &frmsum, se, &valone);

                OperationBase::addTo(modp->baseExecCtx(), &frmsum, se, &br1, negres);
                if (sr == 2)
                    OperationBase::addTo(modp->baseExecCtx(), &frmsum, se, &br2, negres);
            }
        }

        if (cnt == 0) {
            // result is fixed value
            bvar1.dispUnset();
            bvar2.dispUnset();
            binval = (frmOr ? 0 : 1);
            resfix = true;
            lin2 = true;
        }

        else if (cnt == 1) {
            // directly use result from recursive call
            bvar1.moveFrom(br1a, true);
            bvar2.moveFrom(br2a, true);
            binval = (negresa ? 0 : 1);
            resfix = false;
        }

        else {
            CmplValAuto bvres(TP_OPT_VAR, newOptVar(modp, om, se, modp->binTypeVar(), &valnull, &valone));
            if (frmOr) {
                if (lin1) {
                    // bv1 + bv2 + … + bvn <= cnt * bvres;
                    CmplValAuto rhs(TP_FORMULA, new ValFormulaVarOp(se, bvres.optVar(), cnt));
                    CmplValAuto nc(TP_FORMULA, new ValFormulaCompareOp(se, &frmsum, &rhs, false, true, false, true));
                    newOptCon(modp, om, NULL, se, nc);
                }

                if (lin2) {
                    // bv1 + bv2 + … + bvn >= bvres;
                    CmplValAuto rhs(TP_FORMULA, new ValFormulaVarOp(se, bvres.optVar()));
                    CmplValAuto nc(TP_FORMULA, new ValFormulaCompareOp(se, &frmsum, &rhs, true, false, false, true));
                    newOptCon(modp, om, NULL, se, nc);
                }
            }
            else {
                if (lin1) {
                    // bv1 + bv2 + … + bvn - cnt + 1 <= bvres;
                    CmplValAuto lhs(TP_INT, 1 - cnt);
                    OperationBase::addTo(modp->baseExecCtx(), &lhs, se, &frmsum);

                    CmplValAuto rhs(TP_FORMULA, new ValFormulaVarOp(se, bvres.optVar()));
                    CmplValAuto nc(TP_FORMULA, new ValFormulaCompareOp(se, &lhs, &rhs, false, true, false, true));
                    newOptCon(modp, om, NULL, se, nc);
                }

                if (lin2) {
                    // bv1 + bv2 + … + bvn >= cnt * bvres;
                    CmplValAuto rhs(TP_FORMULA, new ValFormulaVarOp(se, bvres.optVar(), cnt));
                    CmplValAuto nc(TP_FORMULA, new ValFormulaCompareOp(se, &frmsum, &rhs, true, false, false, true));
                    newOptCon(modp, om, NULL, se, nc);
                }
            }

            bvar1.moveFrom(bvres, true);
            bvar2.dispUnset();

            binval = 1;
            resfix = false;
        }

        return lin2;
    }

}


