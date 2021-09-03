
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


#include "LinearConstantRow.hh"
#include "LinearConstantRowMod.hh"
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
    LinearConstantRow::LinearConstantRow(LinearConstantRowMod *mod): RemodelBase(mod)
    {
        _delConstRow = mod->_delConstRow;
        _falseAsError = mod->_falseAsError;
        _attachNameVar = mod->_attachNameVar;
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
    bool LinearConstantRow::run(ModuleBase *mod, int step, int id, int ref, int prio, CmdLineOptList::SingleOption *opt, void *par)
    {
        if (RemodelBase::run(mod, step, id, ref, prio, opt, par))
            return true;

        switch (ref) {
            case OPTION_EXT_DELCONSTROW:
                _delConstRow = !opt->neg();
                return true;

            case OPTION_EXT_FALSEASERROR:
                _falseAsError = !opt->neg();
                return true;

            case OPTION_EXT_ATTACHNAMEVAR:
                _attachNameVar = RemodelBaseMod::parseOptString(mod, opt);
                return true;
        }

        return false;
    }

    /**
	 * write data object to output stream
	 * @param si        serialization info
	 * @param data      main data object
	 */
	void LinearConstantRow::serializeTo(MainData::SerializeInfo& si, const MainData *data)
	{
        // write instance variables
        (*si.ostr) << ';' << _maxThreads << ';' << _namePref << ';' << _nameSep << ';' << _attachNameVar << ';' << (_delConstRow ? 1 : 0) << ';' << (_falseAsError ? 1 : 0) << endl;
    }

	/**
	 * fill data object from input stream
	 * @param si        serialization info
	 * @param data      main data object
	 * @param subCnt    count of sub data objects
	 * @param lc		line count per element or 0 if elements are len coded
	 * @param rline		remaining section start line
	 */
	void LinearConstantRow::deserializeFrom(MainData::SerializeInfo& si, const MainData *data, int subCnt, int lc, string &rline)
	{
        // read instance variables
        _maxThreads = MainData::getNextLong(rline, si.pos);
        _namePref = MainData::getNextLong(rline, si.pos);
        _nameSep = MainData::getNextLong(rline, si.pos);
        _attachNameVar = MainData::getNextLong(rline, si.pos);
        _delConstRow = (MainData::getNextLong(rline, si.pos) != 0);
        _falseAsError = (MainData::getNextLong(rline, si.pos) != 0);

        si.pos->addLines();
    }


    /**
     * remodel one constraint and objective
     * @param modp      intepreter module calling the extension
     * @param om		optimization model
     * @param oc        constraint and objective
     * @return          count of executed remodelations
     */
    unsigned LinearConstantRow::remodelCon(Interpreter *modp, OptModel *om, OptCon *oc)
    {
        CmplVal& rv = oc->value();

        if (rv.isEmpty()) {
            // delete empty matrix row
            oc->removeFromValueTree(&(om->rows()));
            CmplObjBase::dispose(oc);
            return 1;
        }
        else if (oc->objective()) {
            ValFormulaObjective *fo = (rv.t == TP_FORMULA ? dynamic_cast<ValFormulaObjective *>(rv.valFormula()) : NULL);
            if (rv.isScalarNumber()) {
                linearizeObj(modp, om, oc, rv);
                return 1;
            }
            else if (fo && fo->formula().isScalarNumber()) {
                linearizeObj(modp, om, oc, fo->formula());
                return 1;
            }
        }
        else {
            if (rv.t == TP_BIN) {
                linearizeCon(modp, om, oc, (rv.v.i != 0));
                return 1;
            }
        }

        return 0;
    }

    /**
     * linearize objective with constant value
     * @param modp      intepreter module calling the extension
     * @param om		optimization model
     * @param oc        matrix row
     * @param rv        constant number
     */
    void LinearConstantRow::linearizeObj(Interpreter *modp, OptModel *om, OptCon *oc, CmplVal& rv)
    {
        if (_delConstRow) {
            oc->removeFromValueTree(&(om->rows()));
            CmplObjBase::dispose(oc);
        }
        else if (rv.t != TP_INFINITE) {
            // generate new optimization variable with rv as lower and upper bound
            CmplVal bound;
            if (rv.t == TP_REAL)
                bound.set(TP_REAL, rv.v.r);
            else
                bound.set(TP_INT, rv.v.i);

            CmplValAuto ovf;
            newConstOptVar(modp, om, oc, bound, ovf);

            // use only generated optimization variable for the matrix row
            oc->value().moveFrom(ovf, true);
        }
        else {
            outError(modp, oc, "objective has infinite constant value", ERROR_LVL_NORMAL);
        }
    }

    /**
     * linearize constraint with constant value of fulfilling
     * @param modp      intepreter module calling the extension
     * @param om		optimization model
     * @param oc        matrix row
     * @param cf        flag whether constraint is fulfilled
     */
    void LinearConstantRow::linearizeCon(Interpreter *modp, OptModel *om, OptCon *oc, bool cf)
    {
        if (!cf && _falseAsError) {
            outError(modp, oc, "constant constraint not fulfilled, optimization problem cannot have a valid solution", (_delConstRow ? ERROR_LVL_NORMAL : ERROR_LVL_WARN));
        }

        if (_delConstRow && (cf || _falseAsError)) {
            oc->removeFromValueTree(&(om->rows()));
            CmplObjBase::dispose(oc);
        }
        else {
            // generate new optimization variable with 1 as lower and upper bound
            CmplVal bound(TP_INT, (intType)1);
            CmplValAuto ovf;
            newConstOptVar(modp, om, oc, bound, ovf);

            // use constraint: generated optimization variable = (cf ? 1 : 0)
            CmplVal rhs(TP_INT, (intType)(cf ? 1 : 0));
            oc->value().dispSet(TP_FORMULA, new ValFormulaCompareOp(oc->syntaxElem(), &ovf, &rhs, true, true, false), true);
        }
    }

    /**
     * generate new optimization variable with constant value
     * @param modp      intepreter module calling the extension
     * @param om		optimization model
     * @param oc        matrix row
     * @param bound     lower and upper bound for optimization variable
     * @param res       value to store result formula of type ValFormulaVar
     */
    void LinearConstantRow::newConstOptVar(Interpreter *modp, OptModel *om, OptCon *oc, CmplVal& bound, CmplVal& res)
    {
        // name and tuple
        CmplValAuto tpl;
        string nm = getBaseNameTuple(modp, oc, tpl, true, true, _attachNameVar);

        // generate optimization variable
        CmplVal& dt = modp->realTypeVar();
        OptVar *v = newOptVar(modp, om, oc->syntaxElem(), dt, &bound, &bound, &nm, &tpl);

        res.set(TP_FORMULA, new ValFormulaVarOp(oc->syntaxElem(), v));
    }

}


