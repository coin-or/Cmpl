
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


#include "LinearConditional.hh"
#include "LinearConditionalMod.hh"
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
    LinearConditional::LinearConditional(LinearConditionalMod *mod): LinearLogCon(mod)
    {
        _nameCondVar  = mod->_nameCondVar;
        _nameCondCon  = mod->_nameCondCon;
        _nameInd = 0;
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
    bool LinearConditional::run(ModuleBase *mod, int step, int id, int ref, int prio, CmdLineOptList::SingleOption *opt, void *par)
    {
        if (LinearLogCon::run(mod, step, id, ref, prio, opt, par))
            return true;

        switch (ref) {
            case OPTION_EXT_NAMECONDVAR:
                _nameCondVar = RemodelBaseMod::parseOptString(mod, opt);
                return true;

            case OPTION_EXT_NAMECONDCON:
                _nameCondCon = RemodelBaseMod::parseOptString(mod, opt);
                return true;
        }

        return false;
    }

	/**
	 * write data object to output stream
	 * @param si        serialization info
	 * @param data      main data object
	 */
	void LinearConditional::serializeTo(MainData::SerializeInfo& si, const MainData *data)
	{
        // write instance variables
        (*si.ostr) << ';' << _maxThreads << ';' << _bigM << ';' << _namePref << ';' << _nameSep << ';' << _nameCondVar << ';' << _nameCondCon
                    << ';' << _attachNameConAnd << ';' << _attachNameConOr << ';' << _attachNameVarOr << ';' << _attachNameConDltNeg << ';' << _attachNameVarDltNeg << endl;
    }

	/**
	 * fill data object from input stream
	 * @param si        serialization info
	 * @param data      main data object
	 * @param subCnt    count of sub data objects
	 * @param lc		line count per element or 0 if elements are len coded
	 * @param rline		remaining section start line
	 */
	void LinearConditional::deserializeFrom(MainData::SerializeInfo& si, const MainData *data, int subCnt, int lc, string &rline)
	{
        // read instance variables
        _maxThreads = MainData::getNextLong(rline, si.pos);
        _bigM = MainData::getNextDouble(rline, si.pos);
        _namePref = MainData::getNextLong(rline, si.pos);
        _nameSep = MainData::getNextLong(rline, si.pos);
        _nameCondVar = MainData::getNextLong(rline, si.pos);
        _nameCondCon = MainData::getNextLong(rline, si.pos);
        _attachNameConAnd = MainData::getNextLong(rline, si.pos);
        _attachNameConOr = MainData::getNextLong(rline, si.pos);
        _attachNameVarOr = MainData::getNextLong(rline, si.pos);
        _attachNameConDltNeg = MainData::getNextLong(rline, si.pos);
        _attachNameVarDltNeg = MainData::getNextLong(rline, si.pos);

        si.pos->addLines();
    }


    /**
     * remodel all constraints and objectives
     * @param modp      intepreter module calling the extension
     * @param om		optimization model
     * @param size      count of potential work steps (usually count of matrix rows)
     */
    void LinearConditional::remodelAll(Interpreter *modp, OptModel *om, unsigned long size)
    {
        if (om->modelProp().hasCondDepVal())
            RemodelBase::remodelAll(modp, om, size);
    }

    /**
     * remodel one constraint and objective
     * @param modp      intepreter module calling the extension
     * @param om		optimization model
     * @param oc        constraint and objective
     * @return          count of executed remodelations
     */
    unsigned LinearConditional::remodelCon(Interpreter *modp, OptModel *om, OptCon *oc)
    {
        //TODO: anderswo (in LinearLogCon):
        //  Schranken beruecksichtigen:
        //      wenn Gleichung an einer Schranke in eine Oder-Verknuepfung eingehend, dann in Ungleichung umwandeln (da an Schranke, bleibt die Semantik dabei gleich; aber Linearisierung wird einfacher)

        // Constraint:
        //      c1) Pruefen ob Formel darin ValFormulaCond, wenn ja:
        //          - Umwandeln Formel in ValFormulaLogCon: ueber ValFormulaCondOp::convertToFormulaLogCon()
        //          + Linearisieren der Restriktion
        //      c2) Alle Variablen in der Formel darin durchgehen, und pruefen ob darin _addProp gesetzt, mit Typ AddPropOptVarConValCond, fuer jede solche Variable:
        //          Pruefen ob Variable schon behandelt (in AddPropOptVarConBase::_linearized, Pruefung und Setzen geeignet threadsicher schuetzen), wenn nein:
        //              Hinzufuegen neue Restriktion, mit: ValFormulaCond (aus AddPropOptVarConBase::_srcVal) = Variable;
        //                      (abhaengig von Aufrufparametern: a) ohne Namen; oder b) neuer Name mit Hochzaehlen, und diesen Namen ebenfalls fuer die Variable setzen)
        //              Compare-Op in allen Teilen ausfuehren (hier unabhaengig davon, dass natuerlich schon _numericVar zugeordnet)
        //              - Umwandeln ueber ValFormulaCondOp::convertToFormulaLogCon() wie in c1) (oder neue Restriktion laeuft einfach spaeter ueber c1))
        //              + Linearisieren der neuen Restriktion wie in c1) (oder neue Restriktion laeuft einfach spaeter ueber c1))
        // Objective:
        //      o1) Pruefen ob Formel darin ValFormulaCond, oder ValFormulaObjective und darin ValFormulaCond, wenn ja:
        //              Existenz _numericVar sicherstellen (ueber ValFormulaCondOp::checkCreateNumericVar())
        //              Verwendung dieser _numericVar anstelle von ValFormulaCond in der Formel
        //              dann wie c2) fuer die Variable _numericVar (bzw. einfach wie ueblich auch o2) ausfuehren)
        //      o2) genau wie c2)

        unsigned res = 0;
        CmplVal& rv = oc->value();

        if (rv.t == TP_FORMULA) {
            ValFormula *rf = rv.valFormula();

            if (!oc->objective()) {
                ValFormulaCondOp *fcond = dynamic_cast<ValFormulaCondOp *>(rf);
                if (fcond) {
                    bool deloc;
                    if (linearizeConditionalCond(modp, om, oc, fcond, NULL, deloc)) {
                        res++;
                        if (deloc) {
                            oc->removeFromValueTree(&(om->rows()));
                            CmplObjBase::dispose(oc);
                            return res;
                        }
                    }
                    else {
                        outError(modp, oc, "wrong formula type within conditional constraint at LinearConditional::remodelCon()", ERROR_LVL_NORMAL);
                    }
                }
            }

            else {
                ValFormulaObjective *fobj = dynamic_cast<ValFormulaObjective *>(rf);
                if (fobj) {
                    CmplVal *ofv = fobj->getPart(0);
                    if (ofv->t == TP_FORMULA) {
                        ValFormulaCondOp *fcond = dynamic_cast<ValFormulaCondOp *>(ofv->valFormula());
                        if (fcond) {
                            // replace objective by a new decision variable, which is fixed by a conditional constraint
                            CmplValAuto nv;
                            fcond->checkCreateNumericVar(modp->baseExecCtx(), oc->syntaxElem(), &nv);

                            CmplValAuto no(TP_FORMULA, new ValFormulaObjectiveOp(fobj->syntaxElem(), &nv, fobj->maximize()));
                            rv.moveFrom(no, true);
                        }
                    }
                }
            }

            // search for decision variables in the formula of the constraint, which must be fixed by a conditional constraint
            OptVar *ov;
            stack<pair<ValFormula *, unsigned>> iter;
            while ((ov = OptVar::iterInFormula(rv, iter))) {
                if (ov->addProp()) {
                    AddPropOptVarConValCond *apvc = dynamic_cast<AddPropOptVarConValCond *>(ov->addProp());
                    if (apvc && checkSetLinearizedVarConValCond(apvc)) {
                        if (addConForValCond(modp, om, apvc, ov->syntaxElem()))
                            res++;
                        else
                            outError(modp, oc, "error for variable which must be fixed by a conditional constraint at LinearConditional::remodelCon()", ERROR_LVL_NORMAL);
                    }
                }
            }
        }
		
        return res;
    }

    /**
     * check if variable for conditional value is not linearized yet, and set flag for it
     * @param apvc      additional property for an optimization variable which is used as replacement for a conditional value
     * @return          true if not linearized yet
     */
    bool LinearConditional::checkSetLinearizedVarConValCond(AddPropOptVarConValCond *apvc)
    {
        LockGuard<mutex> lck(needLock(), _chkLinMtx);
        if (apvc->linearized()) {
            return false;
        }
        else {
            apvc->setLinearized();
            return true;
        }
    }

    /**
     * add constraint for variable used for a conditional value
     * @param modp      intepreter module calling the extension
     * @param om        optimization model
     * @param apvc      additional property for an optimization variable which is used as replacement for a conditional value
     * @param se        id of syntax element in the cmpl text creating the product of variables
     * @return          success: true / error: false
     */
    bool LinearConditional::addConForValCond(Interpreter *modp, OptModel *om, AddPropOptVarConValCond *apvc, unsigned se)
    {
        ValFormulaCondOp *vc = dynamic_cast<ValFormulaCondOp *>(apvc->srcVal().valFormula());
        if (!vc || !vc->isNumeric())
            return false;

        // conditional constraint for fixing the variable
        CmplValAuto cf;
        OptVar *nv = vc->fixNumericVar(modp->baseExecCtx(), &cf, se);

        ValFormulaCondOp *fcond = dynamic_cast<ValFormulaCondOp *>(cf.valFormula());
        if (!fcond)
            return false;

        // linearization
        CmplValAuto lf;
        bool d;
        if (!linearizeConditionalCond(modp, om, NULL, fcond, &lf, d))
            return false;

        if (lf) {
            string nmc;
            CmplVal ind;
            if (_nameCondCon || _nameCondVar) {
                if (_nameCondCon)
                    nmc = modp->data()->globStrings()->at(_nameCondCon);

                unsigned i = ++_nameInd;
                ind.set(TP_INT, (intType)i);
            }

            newOptCon(modp, om, NULL, se, lf, (_nameCondCon ? &nmc : NULL), (_nameCondCon ? &ind : NULL));
            if (_nameCondVar) {
                CmplVal nmv(TP_STR, (intType)_nameCondVar);
                nv->setInValueTree(&(om->cols()), nmv, ind);
            }
        }

        return true;
    }

    /**
     * linearization of a conditional constraint
     * @param modp      intepreter module calling the extension
     * @param om		optimization model
     * @param oc        conditional constraint / NULL: generate new constraint
     * @param fcond     conditional formula of the constraint
     * @param res       return of resulting formula for constraint / NULL: set resulting formula within oc
     * @param deloc     return whether oc must be removed
     * @return          true if ok / false if error
     */
    bool LinearConditional::linearizeConditionalCond(Interpreter *modp, OptModel *om, OptCon *oc, ValFormulaCondOp *fcond, CmplVal *res, bool& deloc)
    {
        CmplValAuto nlc;
        deloc = false;

        // check for simple form
        CmplValAuto cond, optvar, scal1, scal2;
        if (fcond->checkSimpleForm(&cond, &optvar, &scal1, &scal2)) {
            // linearize condition to binary variable
            bool alsoNeg = (bool)scal2;
            int signScal1 = scal1.numCmp(0);
            if (!alsoNeg) {
                CmplValAuto lb, ub;
                optvar.optVar()->getBounds(lb, ub);
                if (signScal1 >= 0)
                    alsoNeg = (!lb || lb.numCmp(0) != 0 || !ub || ub.numCmp(scal1) != 0);
                else
                    alsoNeg = (!lb || lb.numCmp(scal1) != 0 || !ub || ub.numCmp(0) != 0);
            }

            CmplValAuto bvar1, bvar2;
            bool negbvar;
            if (linearizeCondToBin(modp, om, oc, cond, false, alsoNeg, false, bvar1, bvar2, negbvar) < 0)
                return false;

            // linearization of the constraint
            CmplValAuto nlhs(TP_FORMULA, new ValFormulaVarOp(fcond->syntaxElem(), optvar.optVar()));

            if (bvar1.t == TP_BIN) {
                // cond || scal2 || alsoneg:    optvar = (cond ? scal1 : (scal2 ? scal2 : 0))
                CmplVal valnull(TP_INT, (intType)0);
                CmplVal& v = ((bvar1.v.i == 0) == negbvar ? scal1 : (scal2 || !alsoNeg ? scal2 : valnull));
                if (v)
                    nlc.set(TP_FORMULA, new ValFormulaCompareOp(fcond->syntaxElem(), &nlhs, &v, true, true, false, true));
            }
            else {
                // !scal2 && !alsoNeg && scal1 >= 0:    optvar >= scal1 * bvar
                // !scal2 && !alsoNeg && scal1 < 0:     optvar <= scal1 * bvar
                // !scal2 && alsoNeg:                   optvar = scal1 * bvar
                // scal2:                               optvar = (scal1 - scal2) * bvar + scal2;
                CmplValAuto fact(scal1);
                if (scal2)
                    fact.numAdd(scal2, true);

                CmplValAuto nrhs(TP_FORMULA, new ValFormulaVarOp(fcond->syntaxElem(), bvar1.optVar(), fact));
                if (bvar2) {
                    CmplValAuto v(TP_FORMULA, new ValFormulaVarOp(fcond->syntaxElem(), bvar2.optVar(), fact));
                    OperationBase::addTo(modp->baseExecCtx(), &nrhs, fcond->syntaxElem(), &v);
                }

                if (negbvar) {
                    CmplValAuto t;
                    t.moveFrom(nrhs);
                    OperationBase::minus(modp->baseExecCtx(), &nrhs, fcond->syntaxElem(), &fact, &t);
                }

                if (scal2)
                    OperationBase::addTo(modp->baseExecCtx(), &nrhs, fcond->syntaxElem(), &scal2);

                nlc.set(TP_FORMULA, new ValFormulaCompareOp(fcond->syntaxElem(), &nlhs, &nrhs, (scal2 || alsoNeg || signScal1 >= 0), (scal2 || alsoNeg || signScal1 < 0), false, true));
            }
        }

        else {
            for (unsigned i = 0; i < fcond->partCount(); i++) {
                ValFormulaCond::Part *fcpp = dynamic_cast<ValFormulaCond::Part *>(fcond->getPart(i)->valFormula());
                bool lastp = (i == fcond->partCount() - 1);

                OrConnectedBinVars conds;
                bool skip = false;

                for (unsigned j = 1; j < fcpp->partCount(); j++) {
                    CmplVal *condp = fcpp->getPart(j);
                    if (condp && *condp) {
                        ValFormula *condf = condp->valFormula();
                        bool neg;
                        OptVar *bv = condf->asSingleBin(neg);
                        if (bv) {
                            conds.add(bv, ((j == 1 && neg) || (j > 1 && !neg) ? 1 : 0));
                        }
                        else {
                            CmplValAuto bvar1, bvar2;
                            int lr = linearizeCondToBin(modp, om, oc, *condp, false, (!lastp || j > 1), false, bvar1, bvar2, neg);
                            if (lr < 0) {
                                return false;
                            }
                            else if (lr == 0) {
                                bool fixres = (neg == (bvar1.v.i == 0));
                                if (j == 1 && fixres) {
                                    // skip following parts
                                    i = fcond->partCount();
                                }
                                else if ((j == 1 && !fixres) || (j > 1 && fixres)) {
                                    // skip current part
                                    skip = true;
                                    break;
                                }
                            }
                            else {
                                conds.add(bvar1.optVar(), ((j == 1 && neg) || (j > 1 && !neg) ? 1 : 0));
                                if (lr == 2)
                                    conds.add(bvar2.optVar(), -1);
                            }
                        }
                    }
                }

                if (!skip) {
                    CmplValAuto pres;
                    if (!linearizeFormulaOrBins(modp, om, oc, fcpp->getPart(0), conds, pres))
                        return false;

                    if (pres) {
                        if (!nlc) {
                            nlc.moveFrom(pres);
                        }
                        else {
                            CmplValAuto t;
                            OperationBase::execBinaryOper(modp->baseExecCtx(), &t, fcond->syntaxElem(), ICS_OPER_AND, false, &nlc, &pres);
                            nlc.moveFrom(t, true);
                        }
                    }
                }
            }

            //TODO
            //  Testen ob alle Bedingungen (_posCond und _negConds) bereits durch einzelne Binaervariable (direkt, negiert oder mit "var <op> 0/1" -> das ist asSingleBin()) ausgedrueckt sind
            //      -> Iteration ueber alle Bedingungen (mit Kennzeichen ob pos/neg und ob letzter Teil) (ueber getPart().getPart())
            //          pruefen ob asSingleBin(), wenn nein:
            //              linearizeCondToBin() fuer die Teilbedingung aufrufen
            //              in kopierter Formel Teilbedingung durch neue Binaervariable ersetzen:
            //                  wenn fixierter Wert:
            //                      posCond && true -> posCond auf TP_EMPTY; alle parts dahinter entfernen
            //                      posCond && false -> part entfernen
            //                      negCond && true -> part entfernen (sollte eigentlich nie auftreten, weil part schon durch vorheriges posCond && true entfernt wurde
            //                      negCond && false -> aus negConds entfernen
            //                  wenn 2 Binaervariablen:
            //                      beide Binaervariablen fuer eine Bedingung merken
            //          pro Part (hier? oder besser in LinearLogCon?):
            //              wenn value Vergleichsformel ohne Negation:
            //                  Wenn nur 1 Bedingung (auch falls dafuer 2 Binaervariablen): Linearisierung wie in fk1.cmpl beschrieben
            //                  Wenn mehrere Bedingungen: Zusammenfassung aller Bedingungen zu einer neuen Binaervariablen, dann Linearisierung wie in fk1.cmpl beschrieben
            //              sonst:
            //                  auch value mit linearizeCondToBin() in Binaervariable wandeln (negiert ohne lin2)
            //                  Erfuellungsrestriktion ueber Binaervariablen der Bedingungen und von value
            //          wenn mehrere Parts dann mit && verknuepft als FormulaLogCon in die Restriktion (trivialles Auseinandernehmen verbleibt dann fuer LinearLogCon)

            //      Teile der Bedingungsformel durchgehen "{ r1: c1; | r2: c2; … | rn: cn; }"
            //          Einzelbedingungen im Teil durchgehen; jeweils durch Binärvariable ausdrücken (mit Cache)
            //              Part::_posCond: wenn gesetzt; als Binaervariable, wenn nichtletzter Teil dann lin2
            //              jede Part::_negConds: als Binaervariable (immer mit lin2) (sollte immer aus Cache bedient werden)
            //              Part::_val: ist hier immer Formel mit booleschem Wert: ebenfalls als Binaervariable (negiert ohne lin2?) (wirklich? oder Linearisierung in LinearLogCon ueberlassen?)
            //          logisch verknüpfte Formel aus Binärvariablen und Teilrestriktion aufbauen
            //              "(!r1 || c1) && (!r2 || r1 || c2) && (!rn || r1 || r2 … || cn);"
            //      Für neue logisch verknüpfte Formel Linearisierung aufrufen, Ergebnis davon ist Ergebnis hier
            //      Ergebnis setzen
        }

        if (res)
            res->moveFrom(nlc, true);
        else if (oc && nlc)
            oc->value().moveFrom(nlc, true);
        else if (oc)
            deloc = true;

        return true;
    }

}


