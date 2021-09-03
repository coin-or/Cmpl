
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


#include "BoundRow.hh"
#include "BoundRowMod.hh"
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
    BoundRow::BoundRow(BoundRowMod *mod): RemodelBase(mod)
    {
        _delAllBoundRow = mod->_delAllBoundRow;
        _delUnnamedBoundRow = mod->_delUnnamedBoundRow;
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
    bool BoundRow::run(ModuleBase *mod, int step, int id, int ref, int prio, CmdLineOptList::SingleOption *opt, void *par)
    {
        if (RemodelBase::run(mod, step, id, ref, prio, opt, par))
            return true;

        switch (ref) {
            case OPTION_EXT_DELALLBOUNDROW:
                _delAllBoundRow = !opt->neg();
                return true;

            case OPTION_EXT_DELUNMBOUNDROW:
                _delUnnamedBoundRow = !opt->neg();
                return true;
        }

        return false;
    }

    /**
	 * write data object to output stream
	 * @param si        serialization info
	 * @param data      main data object
	 */
	void BoundRow::serializeTo(MainData::SerializeInfo& si, const MainData *data)
	{
        // write instance variables
        (*si.ostr) << ';' << _maxThreads << ';' << (_delAllBoundRow ? 1 : 0) << ';' << (_delUnnamedBoundRow ? 1 : 0) << endl;
    }

	/**
	 * fill data object from input stream
	 * @param si        serialization info
	 * @param data      main data object
	 * @param subCnt    count of sub data objects
	 * @param lc		line count per element or 0 if elements are len coded
	 * @param rline		remaining section start line
	 */
	void BoundRow::deserializeFrom(MainData::SerializeInfo& si, const MainData *data, int subCnt, int lc, string &rline)
	{
        // read instance variables
        _maxThreads = MainData::getNextLong(rline, si.pos);
        _delAllBoundRow = (MainData::getNextLong(rline, si.pos) != 0);
        _delUnnamedBoundRow = (MainData::getNextLong(rline, si.pos) != 0);

        si.pos->addLines();
    }


    /**
     * remodel one constraint and objective
     * @param modp      intepreter module calling the extension
     * @param om		optimization model
     * @param oc        constraint and objective
     * @return          count of executed remodelations
     */
    unsigned BoundRow::remodelCon(Interpreter *modp, OptModel *om, OptCon *oc)
    {
        CmplVal& rv = oc->value();
        unsigned mod = 0;
        bool del = false;

        if (rv.t == TP_FORMULA) {
            ValFormulaCompare *fc = dynamic_cast<ValFormulaCompare *>(rv.valFormula());
            if (fc) {
                mod = checkRemodelBound(modp, fc, del);
            }
            else {
                ValFormulaLogCon *fl = dynamic_cast<ValFormulaLogCon *>(rv.valFormula());
                if (fl && !fl->logNeg() && !fl->logOr())
                    mod = checkRemodelBound(modp, fl, del);
            }

            if (del) {
                if (_delAllBoundRow || (_delUnnamedBoundRow && !oc->isInTree())) {
                     oc->removeFromValueTree(&(om->rows()));
                     CmplObjBase::dispose(oc);
                }
            }
        }

        return mod;
    }


    /**
     * check a logical-and formula whether its parts can be remodeled as bounds, and make this remodeling
     * @param modp      intepreter module calling the extension
     * @param fl        not negated logical-and formula
     * @param del       return if the whole formula can be deleted after remodeling
     * @return          count of executed remodelations
     */
    unsigned BoundRow::checkRemodelBound(Interpreter *modp, ValFormulaLogCon *fl, bool& del)
    {
        vector<CmplVal>& fls = fl->formulas();
        unsigned mod = 0;
        bool adel = true;

        for (CmplVal v: fls) {
            bool sdel = false;
            if (v.t == TP_FORMULA) {
                ValFormulaCompare *fc = dynamic_cast<ValFormulaCompare *>(v.valFormula());
                if (fc)
                    mod = checkRemodelBound(modp, fc, sdel);
            }

            if (!sdel)
                adel = false;
        }

        del = (mod ? adel : false);
        return mod;
    }

    /**
     * check a compare formula whether it can be remodeled as a bound, and make this remodeling
     * @param modp      intepreter module calling the extension
     * @param fc        compare formula
     * @param del       return if the formula can be deleted after remodeling
     * @return          count of executed remodelations
     */
    unsigned BoundRow::checkRemodelBound(Interpreter *modp, ValFormulaCompare *fc, bool& del)
    {
        CmplVal *lhs = fc->getPart(0);
        CmplVal *rhs = fc->getPart(1);
        unsigned mod = 0;

        if (lhs && rhs) {
            CmplVal *nhs = (lhs->isScalarNumber() ? lhs : (rhs->isScalarNumber() ? rhs : NULL));
            if (nhs) {
                CmplVal *fhs = (nhs == lhs ? rhs : lhs);
                OptVar *ov = (fhs->t == TP_OPT_VAR ? fhs->optVar() : NULL);
                ValFormulaLinearCombBase *lc = (fhs->t == TP_FORMULA ? dynamic_cast<ValFormulaLinearCombBase *>(fhs->valFormula()) : NULL);
                if (ov || (lc && lc->linearConstraint() && lc->termCnt() == 1)) {
                    bool le = fc->isCompLe();
                    bool ge = fc->isCompGe();
                    bool neg = fc->isNeg();

                    if ((le || ge) && (!le || !ge || !neg)) {
                        if (nhs == lhs)
                            swap(le, ge);

                        CmplVal bnd(nhs);
                        if (lc) {
                            CmplVal *cnst = lc->constTerm();
                            if (cnst && !cnst->isNumNull(true))
                                OperationBase::addTo(modp->baseExecCtx(), &bnd, fc->syntaxElem(), cnst, true);

                            CmplVal *fact = lc->termFac(0);
                            if (fact && !fact->isNumOne(true)) {
                                if (fact->isNumNull() || fact->t == TP_INFINITE)
                                    return 0;

                                CmplVal div(bnd);
                                OperationBase::divN(modp->baseExecCtx(), &bnd, fc->syntaxElem(), &div, fact);
                            }

                            CmplVal *vov = lc->termVar(0);
                            if (!vov || vov->t != TP_OPT_VAR)
                                return 0;

                            ov = vov->optVar();
                        }

                        if (checkSetBound(modp, ov, &bnd, le, ge, neg, del)) {
                            mod = 1;
                        }
                        else {
                            ostringstream ostr;
                            ostr << "optimization variable ";
                            ov->recOutName(ostr, modp->data()->globStrings(), true, true);
                            ostr << " has no valid range left";

                            const SyntaxElement *sep = modp->syntaxElement(fc->syntaxElem());
                            modp->ctrl()->errHandler().error(ERROR_LVL_WARN, ostr.str().c_str(), sep->loc());
                        }
                    }
                }
            }
        }

        return mod;
    }


    /**
     * check and set bound for an optimization variable
     * @param modp      intepreter module calling the extension
     * @param ov        optimization variable
     * @param bnd       bound value
     * @param le        variable must be lesser equal bound
     * @param ge        variable must be greater equal bound
     * @param neg       condition is negated (never with: le && ge)
     * @param del       return if the formula can be deleted after remodeling
     * @return          false if no valid range is left for the variable
     */
    bool BoundRow::checkSetBound(Interpreter *modp, OptVar *ov, CmplVal *bnd, bool le, bool ge, bool neg, bool& del)
    {
        // only check and wait for pseudo cache, but never insert anything
        RemodelCacheGuard cg;
        (void) cg.checkPrep(_accessGuardCache, ov->id());

        bool invalid = false;

        if (le && ge) {
            checkSetSingleBound(ov, bnd, true, invalid);
            checkSetSingleBound(ov, bnd, false, invalid);
            if (!invalid)
                del = true;
        }

        else if ((le && !neg) || (ge && neg)) {
            int sb = checkSetSingleBound(ov, bnd, true, invalid);
            if (!invalid && (!neg || sb == -1))
                del = true;
        }

        else if ((ge && !neg) || (le && neg)) {
            int sb = checkSetSingleBound(ov, bnd, false, invalid);
            if (!invalid && (!neg || sb == -1))
                del = true;
        }

        if (del)
            ov->setUsedByCon(-1);

        return !invalid;
    }


    /**
     * check and set lower or upper bound for an optimization variable
     * @param ov        optimization variable
     * @param bnd       bound value
     * @param upp       true: upper bound / false: lower bound
     * @param invalid   return whether no valid range is left for the variable
     * @return          1: bound new set / 0: bound has already value bnd / -1: otherwise
     */
    int BoundRow::checkSetSingleBound(OptVar *ov, CmplVal *bnd, bool upp, bool& invalid)
    {
        if (bnd->t == TP_INFINITE) {
            if ((upp && bnd->v.i < 0) || (!upp && bnd->v.i > 0))
                invalid = true;

            return -1;
        }

        CmplVal& lb = ov->lowBound();
        CmplVal& ub = ov->uppBound();

        if ((upp && lb && bnd->numCmp(lb) == -1) || (!upp && ub && bnd->numCmp(ub) == 1)) {
            invalid = true;
            return -1;
        }

        int cb;
        if (upp)
            cb = (ub ? bnd->numCmp(ub) : -1);
        else
            cb = (lb ? bnd->numCmp(lb) : 1);

        if (cb == (upp ? -1 : 1)) {
            (upp ? ub : lb).copyFrom(bnd);
            return 1;
        }
        else {
            return (cb == 0 ? 0 : -1);
        }
    }
}


