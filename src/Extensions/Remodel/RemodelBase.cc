
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


#include <atomic>
#include <thread>

#include "RemodelBase.hh"
#include "RemodelBaseMod.hh"
#include "../../Control/MainControl.hh"
#include "../../Modules/Interpreter/Interpreter.hh"
#include "../../Modules/Interpreter/ValType.hh"
#include "../../CommonData/ValFormula.hh"



namespace cmpl
{
    /**
     * constructor
     * @param mod			module creating this extension object
     */
    RemodelBase::RemodelBase(RemodelBaseMod *mod): ExtensionBase(mod)
    {
        _remodelEnabled = true;
        _maxThreads = mod->_maxThreads;
        _bigM = mod->_bigM;
        _namePref = mod->_namePref;
        _nameSep = mod->_nameSep;
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
    bool RemodelBase::run(ModuleBase *mod, int step, int id, int ref, int prio, CmdLineOptList::SingleOption *opt, void *par)
    {
        switch (ref) {
            case OPTION_EXT_REMODEL_ENABLE:
                _remodelEnabled = !(opt->neg());
                return true;

            case OPTION_EXT_THREADS:
                _maxThreads = (unsigned)(opt->neg() ? 0 : opt->argAsInt(0, mod->ctrl()));
                return true;

            case OPTION_EXT_REMODEL_BIGM:
                _bigM = STR_TO_CMPLREAL((*opt)[0].c_str());
                return true;

            case OPTION_EXT_NAMEPREF:
                _namePref = RemodelBaseMod::parseOptString(mod, opt);
                return true;

            case OPTION_EXT_NAMESEP:
                _nameSep = RemodelBaseMod::parseOptString(mod, opt);
                return true;
        }

        return false;
    }

    /**
	 * run the extension function
	 * @param mod			module calling the extension
	 * @param step			execution step within the module
	 * @param id			additional identificator
	 * @param par			additional parameter
	 */
	void RemodelBase::run(ModuleBase *mod, int step, int id, void *par)
	{
        if (step == EXT_STEP_INTERPRET_REMODEL) {
            PROTO_MOD_OUTL(mod, " extension " << extName() << ": matrix remodeling: " << step << " " << id);
            if (remodelEnabled()) {
                remodelAll((Interpreter *)mod, (OptModel *)par, ((OptModel *)par)->rows().size());
            }
            else {
                PROTO_MOD_OUTL(mod, "   extension " << extName() << " is disabled");
            }
        }

        else {
            mod->ctrl()->errHandler().internalError(mod->ctrl()->printBuffer("invalid execution step for extension %s, called for '%s' %d %d", extName(), mod->moduleName(), step, id));
        }

        PROTO_MOD_OUTL(mod, " extension " << extName() << ": end");
    }


    /**
     * remodel all constraints and objectives
     * @param modp      intepreter module calling the extension
     * @param om		optimization model
     * @param size      count of potential work steps (usually count of matrix rows)
     */
    void RemodelBase::remodelAll(Interpreter *modp, OptModel *om, unsigned long size)
    {
        remodelInit(modp, om);

        unsigned thrMax = _maxThreads;
        if (thrMax > size / 2)
            thrMax = size / 2;

        atomic<unsigned long> rc(0);
        atomic<unsigned long> res(0);

        if (thrMax <= 1) {
            PROTO_MOD_OUTL(modp, " start remodelations (without parallel threads)");

            remodelAllThdr(this, modp, om, size, &rc, &res);
        }
        else {
            PROTO_MOD_OUTL(modp, " start remodelations using " << thrMax << " threads");

            vector<thread> threads;
            for (unsigned i = 0; i < thrMax; i++)
                threads.push_back(thread(RemodelBase::remodelAllThdr, this, modp, om, size, &rc, &res));

            for (thread& th: threads)
                th.join();
        }

        if (res)
            om->modelProp().reset();

        PROTO_MOD_OUTL(modp, " executed " << res << " remodelations");
    }

    /**
     * remodel all constraints and objectives, for running in parallel worker threads
     * @param rme       remodel extension object
     * @param modp      intepreter module calling the extension
     * @param om        optimization model
     * @param rowcnt    row count at start of running this remodel extension
     * @param rc        thread safe row counter
     * @param res       thread safe return of result count
     */
    void RemodelBase::remodelAllThdr(RemodelBase *rme, Interpreter *modp, OptModel *om, unsigned long rowcnt, atomic<unsigned long> *rc, atomic<unsigned long> *res)
    {
        while (true) {
            try {
                unsigned long rowno = rc->fetch_add(1);
                if (rowno >= rowcnt)
                    break;

                void *ws = rme->getWorkStep(modp, om, rowno);
                if (ws) {
                    try {
                        *res += rme->remodelWorkStep(modp, om, ws);
                    }
                    catch (exception ex) {
                        rme->outError(modp, ws, ex.what(), ERROR_LVL_NORMAL);
                        break;
                    }
                }
            }
            catch (...) {
                modp->ctrl()->errHandler().internalError(modp->ctrl()->printBuffer("unknown error within remodel extension %s", rme->extName()));
                break;
            }
        }
    }


    /**
     * return next potential workstep for remodeling (usually pointer to matrix row)
     * @param modp      intepreter module calling the extension
     * @param om		optimization model
     * @param rowno     row count of workstep
     * @return          pointer to workstep / NULL: no workstep
     */
    void *RemodelBase::getWorkStep(Interpreter *modp, OptModel *om, unsigned long rowno)
    {
        ValueTreeRoot& rows = om->rows();
        if (rows[rowno]) {
            OptCon *oc = dynamic_cast<OptCon *>(rows[rowno]);
            if (!oc)
                modp->ctrl()->errHandler().internalError(modp->ctrl()->printBuffer("invalid type of matrix row in remodel extension %s", extName()));

            return oc;
        }

        return NULL;
    }


    /**
     * execute one potential workstep for remodeling
     * @param modp      intepreter module calling the extension
     * @param om        optimization model
     * @param ws        pointer to workstep (already checked for correct type)
     */
    unsigned RemodelBase::remodelWorkStep(Interpreter *modp, OptModel *om, void *ws)
    {
        if (ws)
            return remodelCon(modp, om, (OptCon *)ws);
        else
            return 0;
    }


    /**
     * generate new optimization variable
     * @param modp      intepreter module calling the extension
     * @param om		optimization model
     * @param se        syntax element
     * @param dt        data type for optimization variable
     * @param lb        lower bound for optimization variable / NULL: no lower bound
     * @param ub        upper bound for optimization variable / NULL: no lower bound
     * @param nm        name for new optimization variable / empty or NULL: don't set name
     * @param tpl       index tuple for new optimization variable (only used if nm is given)
     * @return          new optimization variable (has counted reference within OptModel)
     */
    OptVar *RemodelBase::newOptVar(Interpreter *modp, OptModel *om, unsigned se, CmplVal& dt, CmplVal *lb, CmplVal *ub, string *nm, CmplVal *tpl)
    {
        LockGuard<mutex> lck(needLock(), _varMtx);

        // generate optimization variable
        OptVar *v = new OptVar(om, dt, false, se);

        // set type and bounds
        dt.valType()->setForOptVar(v);
        if (lb && *lb)
            v->setLowBound(*lb);
        if (ub && *ub)
            v->setUppBound(*ub);

        // add prefix string
        unsigned nmi = addStorePrefixString(modp, nm, om->cols().size(), false);

        // set name and tuple
        if (nmi) {
            CmplVal cn(TP_STR, (intType)nmi);
            if (tpl && *tpl) {
                v->setInValueTree(&(om->cols()), cn, *tpl);
            }
            else {
                CmplVal tp(TP_ITUPLE_NULL);
                v->setInValueTree(&(om->cols()), cn, tp);
            }
        }

        return v;
    }

    /**
     * generate new optimization constraint
     * @param modp      intepreter module calling the extension
     * @param om		optimization model
     * @param oc        original constraint / NULL: no one
     * @param se        syntax element
     * @param f         formula for new constraint
     * @param nm        name for new constraint / empty: don't set name / NULL: replace oc if given
     * @param tpl       index tuple for new constraint (only used if nm is given)
     * @return          constraint (has counted reference within OptModel)
     */
    OptCon *RemodelBase::newOptCon(Interpreter *modp, OptModel *om, OptCon *oc, unsigned se, CmplVal& f, string *nm, CmplVal *tpl)
    {
        if (nm || !oc) {
            LockGuard<mutex> lck(needLock(), _conMtx);

            OptCon *c = new OptCon(om, f, false, se);

            unsigned nmi = addStorePrefixString(modp, nm, om->rows().size(), true, &f);
            if (nmi) {
                CmplVal cn(TP_STR, (intType)nmi);
                if (tpl && *tpl) {
                    c->setInValueTree(&(om->rows()), cn, *tpl);
                }
                else {
                    CmplVal tp(TP_ITUPLE_NULL);
                    c->setInValueTree(&(om->rows()), cn, tp);
                }
            }
            return c;
        }
        else {
            oc->value().copyFrom(f, true, true);
            return oc;
        }
    }


    /**
     * add prefix string to name and store result name in global string store
     * @param modp      intepreter module calling the extension
     * @param nm        name without prefix / NULL: no one
     * @param cur       current number to use if no name
     * @param lc        true: constraint / false: variable
     * @param f         formula for new constraint: if given then add prefix string only if formula is auto generated
     * @return          index number of result name / 0: no one
     */
    unsigned RemodelBase::addStorePrefixString(Interpreter *modp, string *nm, unsigned long cur, bool lc, CmplVal *f)
    {
        unsigned nmi = 0;

        if (_namePref && f && f->t == TP_FORMULA) {
            ValFormulaCompare *fc = dynamic_cast<ValFormulaCompare *>(f->valFormula());
            if (fc && fc->autogen())
                f = NULL;
        }

        if (_namePref && !f) {
            string n(modp->data()->globStrings()->at(_namePref));
            if (nm && !nm->empty()) {
                if (nm->find(n) == 0)
                    n = *nm;
                else
                    n += *nm;
            }
            else {
                n = n + (lc ? "line" : "var") + to_string(cur);
            }

            nmi = modp->data()->globStrings()->storeInd(n);
        }

        else if (nm && !nm->empty()) {
            nmi = modp->data()->globStrings()->storeInd(*nm);
        }

        return nmi;
    }

    /**
     * get base name and tuple of a source variable or constraint
     * @param modp      intepreter module calling the extension
     * @param src       source variable or constraint
     * @param tpl       return of tuple
     * @param nameSep   add name separator if given
     * @param attach    attach postfix string (return empty if no postfix string)
     * @param pfs       postfix string
     * @return          name
     */
    string RemodelBase::getBaseNameTuple(Interpreter *modp, OptVarConBase *src, CmplVal& tpl, bool nameSep, bool attach, unsigned pfs)
    {
        CmplValAuto nm;
        string name;

        if (src->isInTree()) {
            src->getIndexOfLevel(1, nm);
            src->getIndexOfLevel(2, 0, tpl);

            if (!attach || pfs) {
                if (nm.isScalarString()) {
                    name = nm.stringAsCharP(modp);
                    if (nameSep && _nameSep)
                        name += modp->data()->globStrings()->at(_nameSep);

                    if (attach)
                        name += modp->data()->globStrings()->at(pfs);
                }
            }
        }

        return name;
    }

    /**
     * get name and tuple based on two source variables or constraints
     * @param modp      intepreter module calling the extension
     * @param src1      first source variable or constraint
     * @param src2      second source variable or constraint
     * @param tpl       return of tuple
     * @param nameSep   concat names by name separator if given
     * @return          name
     */
    string RemodelBase::getBaseNameTuple2(Interpreter *modp, OptVarConBase *src1, OptVarConBase *src2, CmplVal& tpl, bool nameSep)
    {
        CmplValAuto tpl1, tpl2;
        string nm1 = getBaseNameTuple(modp, src1, tpl1, false);
        string nm2 = getBaseNameTuple(modp, src2, tpl2, false);

        string name;
        if (!nm1.empty() || !nm2.empty()) {
            if (nm1.empty() || nm2.empty())
                name = (nm1.empty() ? nm2 : nm1);
            else if (nameSep && _nameSep)
                name = nm1 + modp->data()->globStrings()->at(_nameSep) + nm2;
            else
                name = nm1 + nm2;
        }

        if (tpl1.t == TP_ITUPLE_NULL || tpl2.t == TP_ITUPLE_NULL)
            tpl.moveFrom((tpl1.t == TP_ITUPLE_NULL ? tpl2 : tpl1));
        else
            Tuple::constructTuple(tpl, tpl1, tpl2);

        return name;
    }

    /**
     * attach postfix string to name
     * @param modp      intepreter module calling the extension
     * @param base      base name
     * @param nameSep   add name separator if given
     * @param pfs       postfix string
     * @return          resulting name / empty if no postfix string
     */
    string RemodelBase::attachName(Interpreter *modp, string& base, bool nameSep, unsigned pfs)
    {
        string name;

        if (pfs && !base.empty()) {
            name = base;

            if (nameSep && _nameSep)
                name += modp->data()->globStrings()->at(_nameSep);

            name += modp->data()->globStrings()->at(pfs);
        }

        return name;
    }


    /**
     * output error message
     * @param modp      intepreter module calling the extension
     * @param ws        workstep (matrix row)
     * @param msg       error message
     * @param errLvl    error level
     */
    void RemodelBase::outError(Interpreter *modp, void *ws, const char *msg, int errLvl)
    {
        OptCon *oc = (OptCon *)ws;

        ostringstream ostr;
        ostr << "matrix row ";

        if (oc->isInTree())
            oc->recOutName(ostr, modp->data()->globStrings(), '"', true);
        else
            ostr << "id = " << oc->id();

        ostr << ": " << msg;

        const SyntaxElement *sep = modp->syntaxElement(oc->syntaxElem());
        modp->ctrl()->errHandler().error(errLvl, ostr.str().c_str(), sep->loc());
    }
}


