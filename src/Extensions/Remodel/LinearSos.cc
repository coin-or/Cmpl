
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


#include "LinearSos.hh"
#include "LinearSosMod.hh"
#include "../../Modules/Compiler/CompilerContext.hh"
#include "../../Modules/Interpreter/Interpreter.hh"
#include "../../Modules/Interpreter/ValType.hh"
#include "../../Modules/Interpreter/OperationBase.hh"
#include "../../Modules/Interpreter/ValContainer.hh"
#include "../../Modules/Interpreter/SetUtil.hh"
#include "../../CommonData/ValFormula.hh"


#define M(n,c,s,p)    { (c>=0 ? comp->insertExtensionSymbol(n,true,false,specialSymbolNo,p,c) : comp->insertExtensionSymbol(n,false,true,(s ? specialSymbolNamespace : specialSymbolKnownVal),p,0)); }


namespace cmpl
{
    /****** LinearSos::SOSStore ****/

    /**
     * add optimization variable (or parameter) to SOS
     * @param ctx			execution context
     * @param src			source value
     * @param se			syntax element id of source value
     */
    void LinearSos::SOSStore::addSOSVar(ExecContext *ctx, CmplVal& src, unsigned se)
    {
        if (src.t == TP_OPT_VAR) {
            vars.push_back(src.optVar()->id());
        }
        else if (src.t == TP_EMPTY || src.isScalarNumber()) {
            bool isZero = src.isNumNull(true);

            if (!isZero)
                cntConst++;

            if (!sos2) {
                if (!isZero && cntConst > 1 && !(noSolution))
                    noSolution = true;
            }
            else {
                posConst.push_back(vars.size());
                vars.push_back(isZero ? 0 : 1);

                if (!isZero && cntConst > 1 && !(noSolution)) {
                    if (cntConst > 2 || (cntConst == 2 && (posConst[posConst.size() - 2] != vars.size() - 2 || vars[vars.size() - 2] != 1)))
                        noSolution = true;
                }
            }
        }
        else if (src.t != TP_NULL) {
            ctx->valueError("invalid argument value for SOS", src, se);
        }
    }

    /**
     * write info for sos to stream
     * @param ostr          stream
     * @param modp          interpreter module
     */
    void LinearSos::SOSStore::write(ostream& ostr, Interpreter *modp)
    {
        ostr << (sos2 ? "SOS2 " : "SOS ") << (name ? modp->data()->globStrings()->at(name) : "(not named)") << ' ' << vars.size() << " elements";
        if (noSolution)
            ostr << " (invalid by constant non zero elements)";
    }


    /****** LinearSos::OutModelExtData ****/

    /**
     * get data for one SOS
     * @param info          info for special extension data
     * @param row           data row number, must be lesser than info.rows
     * @param vars          return of identy numbers of the optimization variables in the SOS
     * @param name          name for SOS / 0: no name given
     * @return              identity number of SOS/SOS2
     */
   // unsigned LinearSos::OutModelExtData::getData(Info& info, unsigned row, vector<unsigned long>* vars, unsigned& name)
    unsigned LinearSos::OutModelExtData::getData(Info& info, unsigned row, vector<unsigned long>& vars, unsigned& name)
    {
        if (row < 0 || row >= _rows.size())
            return 0;

        unsigned rsos = _rows[row];
        SOSStore *sos = &(_ext->_storeSos[rsos]);

        if (!_sos2 || sos->posConst.size() == 0) {
            //vars = &(sos->vars);
            vars = sos->vars;
        }
        else {
            if (sos->vars2.size() == 0 && sos->posConst.size() < sos->vars.size()) {
                unsigned indPosConst = 0;
                unsigned nextPosConst = sos->posConst[indPosConst];

                for (unsigned i = 0; i < sos->vars.size(); i++) {
                    if (i == nextPosConst)
                        nextPosConst = (++indPosConst < sos->posConst.size() ? sos->posConst[indPosConst] : sos->vars.size());
                    else
                        sos->vars2.push_back(sos->vars[i]);
                }
            }

            //vars = &(sos->vars2);
            vars = sos->vars2;
        }

        name = sos->name;
        return rsos;
    }



    /****** LinearSos ****/

    /**
     * constructor
     * @param mod			module creating this extension object
     */
    LinearSos::LinearSos(LinearSosMod *mod): RemodelBase(mod)
    {
        _namespace = mod->_namespace;

        _useForSos1 = (mod->_useForSos == LinearSosMod::sosTypeOnlySos1 || mod->_useForSos == LinearSosMod::sosTypeBoth);
        _useForSos2 = (mod->_useForSos == LinearSosMod::sosTypeOnlySos2 || mod->_useForSos == LinearSosMod::sosTypeBoth);

        _idForSos1 = 0;
        _idForSos2 = 0;

        _sosNative = LinearSosMod::sosTypeNone;
        _sos2NativeOptPrio = 0;

        _warnInvalid = mod->_warnInvalid;

        _attachNameVarSos = mod->_attachNameVarSos;
        _attachNameVarSosNeg = mod->_attachNameVarSosNeg;
        _attachNameConSos = mod->_attachNameConSos;

        _attachNameVarSos2 = mod->_attachNameVarSos2;
        _attachNameConSos2 = mod->_attachNameConSos2;
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
    bool LinearSos::run(ModuleBase *mod, int step, int id, int ref, int prio, CmdLineOptList::SingleOption *opt, void *par)
    {
        if (RemodelBase::run(mod, step, id, ref, prio, opt, par))
            return true;

        switch (ref) {
            case OPTION_EXT_SOSNATIVE:
                if (prio >= _sos2NativeOptPrio) {
                    prio = _sos2NativeOptPrio;
                    if (!opt->neg())
                        _sosNative = LinearSosMod::sosTypeBoth;
                    else
                        _sosNative = LinearSosMod::sosTypeNone;
                }
                else {
                    if (!opt->neg())
                        _sosNative = (_sosNative == LinearSosMod::sosTypeNone || _sosNative == LinearSosMod::sosTypeOnlySos1 ? LinearSosMod::sosTypeOnlySos1 : LinearSosMod::sosTypeBoth);
                    else
                        _sosNative = (_sosNative == LinearSosMod::sosTypeNone || _sosNative == LinearSosMod::sosTypeOnlySos1 ? LinearSosMod::sosTypeNone : LinearSosMod::sosTypeOnlySos2);
                }
                return true;

            case OPTION_EXT_SOS2NATIVE:
                if (prio >= _sos2NativeOptPrio) {
                    prio = _sos2NativeOptPrio;
                    if (!opt->neg())
                        _sosNative = (_sosNative == LinearSosMod::sosTypeNone || _sosNative == LinearSosMod::sosTypeOnlySos2 ? LinearSosMod::sosTypeOnlySos2 : LinearSosMod::sosTypeBoth);
                    else
                        _sosNative = (_sosNative == LinearSosMod::sosTypeNone || _sosNative == LinearSosMod::sosTypeOnlySos2 ? LinearSosMod::sosTypeNone : LinearSosMod::sosTypeOnlySos1);
                }
                return true;

            case OPTION_EXT_WARNINVALID:
                _warnInvalid = !opt->neg();
                return true;

            case OPTION_EXT_ATTACHNAMEVARSOS:
                _attachNameVarSos = RemodelBaseMod::parseOptString(mod, opt);
                return true;

            case OPTION_EXT_ATTACHNAMEVARSOSNEG:
                _attachNameVarSosNeg = RemodelBaseMod::parseOptString(mod, opt);
                return true;

            case OPTION_EXT_ATTACHNAMECONSOS:
                _attachNameConSos = RemodelBaseMod::parseOptString(mod, opt);
                return true;

            case OPTION_EXT_ATTACHNAMEVARSOS2:
                _attachNameVarSos2 = RemodelBaseMod::parseOptString(mod, opt);
                return true;

            case OPTION_EXT_ATTACHNAMECONSOS2:
                _attachNameConSos2 = RemodelBaseMod::parseOptString(mod, opt);
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
    void LinearSos::run(ModuleBase *mod, int step, int id, void *par)
    {
        if (step == EXT_STEP_COMPILE_PREDEF_SYMBOLS) {
            PROTO_MOD_OUTL(mod, " extension linearSos: define symbols: " << step << " " << id);

            CompilerContext *comp = (CompilerContext *)par;
            int cont = -1;
            unsigned offset = 0;
            _baseId = id;

            // define namespace symbols
            if (!_namespace.empty()) {
                for (unsigned i = 0; i < _namespace.size(); i++) {
                    const char *nsp = mod->data()->globStrings()->at(_namespace[i]);
                    M ( nsp, cont, true, _baseId + offset );
                    cont = _baseId + offset++;
                }
            }

            // define function symbols
            if (_useForSos1) {
                _idForSos1 = _baseId + offset++;
                M ( "sos1", cont, false, _idForSos1 )
            }

            if (_useForSos2) {
                _idForSos2 = _baseId + offset++;
                M ( "sos2", cont, false, _idForSos2 )
            }

            // define plain subsymbols, usable for container objects representing the SOS
            bool nw;
            _defIdSubOrd = comp->insertExtensionPlainSubSymbol("ord", true, nw)->defId();
            _defIdSubName = comp->insertExtensionPlainSubSymbol("name", true, nw)->defId();
            _defIdSubAdd = comp->insertExtensionPlainSubSymbol("add", true, nw)->defId();
            _defIdSubVarArr = comp->insertExtensionPlainSubSymbol("*vararray", true, nw)->defId();
        }

        else if (step == EXT_STEP_INTERPRET_PREDEF_SYMBOLS_INIT) {
            PROTO_MOD_OUTL(mod, " extension linearSos: initialize symbol values: " << step << " " << id);

            SymbolValue *sv = (SymbolValue *)par;
            if (_useForSos1 && id == _idForSos1)
                sv->setSimpleConstVal(TP_FUNCTION, new SosFunction(this, 1));
            else if (_useForSos1 && id == _idForSos2)
                sv->setSimpleConstVal(TP_FUNCTION, new SosFunction(this, 2));
            else if (id >= _baseId && id < _baseId + (int)_namespace.size())
                sv->setSimpleConstVal(TP_CONTAINER, new ValContainer(true));
            else
                mod->ctrl()->errHandler().internalError(mod->ctrl()->printBuffer("unknown identificator for extension linearSos, called for '%s' %d %d", mod->moduleName(), step, id));
        }

        else if (step == EXT_STEP_INTERPRET_INIT_FINAL) {
            // Initialize template container object
            if (!_templContainer) {
                ValContainer *vc = new ValContainer(true);
                _templContainer.set(TP_CONTAINER, vc);

                SymbolValue *sy = vc->insertPredef((Interpreter *)mod, _defIdSubOrd);
                sy->setSimpleConstVal(TP_EMPTY);

                sy = vc->insertPredef((Interpreter *)mod, _defIdSubName);
                sy->setSimpleConstVal(TP_FUNCTION, new SosContFunctionName(this));

                sy = vc->insertPredef((Interpreter *)mod, _defIdSubAdd);
                sy->setSimpleConstVal(TP_FUNCTION, new SosContFunctionAdd(this));

                sy = vc->insertPredef((Interpreter *)mod, _defIdSubVarArr);
                sy->setSimpleConstVal(TP_NULL);

                sy = vc->insertPredef((Interpreter *)mod, ((Interpreter *)mod)->symIdSubAsVar());
                sy->setSimpleConstVal(TP_FUNCTION, new SosContFunctionAsVar(this));

                sy = vc->insertPredef((Interpreter *)mod, ((Interpreter *)mod)->symIdSubAsString());
                sy->setSimpleConstVal(TP_FUNCTION, new SosContFunctionAsString(this));
            }
        }

        else if (step == EXT_STEP_INTERPRET_REMODEL) {
            PROTO_MOD_OUTL(mod, " extension " << extName() << ": matrix remodeling: " << step << " " << id);

            _linForSos1 = (_sosNative != LinearSosMod::sosTypeOnlySos1 && _sosNative != LinearSosMod::sosTypeBoth);
            _linForSos2 = (_sosNative != LinearSosMod::sosTypeOnlySos2 && _sosNative != LinearSosMod::sosTypeBoth);

            if (!_storeSos.empty() && remodelEnabled())
                remodelAll((Interpreter *)mod, (OptModel *)par, _storeSos.size());

            initTransferInfo((Interpreter *)mod);
        }

        else if (step == EXT_STEP_INTERPRET_MODELPROP) {
            OptModel::Properties *prop = (OptModel::Properties *)par;
            prop->sos = getModelProp();
        }

        else if (step == EXT_STEP_OUTMODEL_SPECIAL_INFO) {
            if (!_storeSos.empty())
                fillTransferInfo(mod, (list<OutModelExtDataBase::Info> *)par);
        }

        else {
            mod->ctrl()->errHandler().internalError(mod->ctrl()->printBuffer("invalid execution step for extension linearSos, called for '%s' %d %d", mod->moduleName(), step, id));
        }

        PROTO_MOD_OUTL(mod, " extension " << extName() << ": end");
    }

	/**
	 * write data object to output stream
	 * @param si        serialization info
	 * @param data      main data object
	 */
	void LinearSos::serializeTo(MainData::SerializeInfo& si, const MainData *data)
	{
        // write instance variables
		//TODO
        (*si.ostr) << ';' << _maxThreads << ';' << _namePref << ';' << _nameSep << endl;
    }

	/**
	 * fill data object from input stream
	 * @param si        serialization info
	 * @param data      main data object
	 * @param subCnt    count of sub data objects
	 * @param lc		line count per element or 0 if elements are len coded
	 * @param rline		remaining section start line
	 */
	void LinearSos::deserializeFrom(MainData::SerializeInfo& si, const MainData *data, int subCnt, int lc, string &rline)
	{
        // read instance variables
		//TODO
        _maxThreads = MainData::getNextLong(rline, si.pos);
        _namePref = MainData::getNextLong(rline, si.pos);
        _nameSep = MainData::getNextLong(rline, si.pos);

        si.pos->addLines();
    }


    /**
     * get sos store belonging to sos container object
     * @param modp      intepreter module
     * @param cont      container object
     * @return          pointer to sos store
     */
    LinearSos::SOSStore *LinearSos::getSOSStore(Interpreter *modp, ValContainer *cont)
    {
        if (!cont) {
            modp->ctrl()->errHandler().internalError("$this object is not set within call for SOS");
            return NULL;
        }

        SymbolValue *sy = cont->getSimple(_defIdSubOrd);
        if (!sy) {
            modp->ctrl()->errHandler().internalError("$this object for SOS has no property 'ord'");
            return NULL;
        }

        CmplVal& ord = sy->simpleConstVal();
        if (ord.t != TP_INT || ord.v.i < 0) {
            modp->ctrl()->errHandler().internalError("$this object for SOS has invalid property 'ord'");
            return NULL;
        }

        if (ord.v.i < (intType)_storeSos.size())
            return &(_storeSos[ord.v.i]);
        else
            return NULL;
    }

    /**
     * get the array of variables stored in the sos container object, and delete it in the container object
     * @param modp      intepreter module
     * @param cont      container object
     * @param dst       store the array here / NULL: only delete the array
     */
    void LinearSos::getDelSOSVarArray(Interpreter *modp, ValContainer *cont, CmplVal *dst)
    {
        if (!cont) {
            modp->ctrl()->errHandler().internalError("$this object is not set within call for SOS");
            return;
        }

        SymbolValue *sy = cont->getSimple(_defIdSubVarArr);
        if (!sy) {
            modp->ctrl()->errHandler().internalError("$this object for SOS doesn't contain an array of variables");
            return;
        }

        if (dst) {
            CmplVal& va = sy->simpleConstVal();
            if (va.t == TP_ARRAY)
                dst->moveFrom(va);
            else
                dst->set(TP_NULL);
        }

        sy->setSimpleConstVal(TP_NULL);
    }


    /**
     * return next potential workstep for remodeling
     * @param modp      intepreter module calling the extension
     * @param om        optimization model
     * @param rowno     row count of workstep
     * @return          pointer to workstep / NULL: no workstep
     */
    void *LinearSos::getWorkStep(Interpreter *modp, OptModel *om, unsigned long rowno)
    {
        if (rowno < _storeSos.size()) {
            SOSStore *ws = &(_storeSos[rowno]);
            if ((!(ws->sos2) && (_linForSos1 || ws->cntConst)) || (ws->sos2 && (_linForSos2 || !(ws->posConst.empty()))))
                return ws;
        }

        return NULL;
    }


    /**
     * execute one potential workstep for remodeling
     * @param modp      intepreter module calling the extension
     * @param om        optimization model
     * @param ws        pointer to workstep (already checked for correct type)
     */
    unsigned LinearSos::remodelWorkStep(Interpreter *modp, OptModel *om, void *ws)
    {
        if (ws)
            return remodelSOS(modp, om, (SOSStore *)ws);
        else
            return 0;
    }


    /**
     * output error message
     * @param modp      intepreter module calling the extension
     * @param ws        workstep
     * @param msg       error message
     * @param errLvl    error level
     */
    void LinearSos::outError(Interpreter *modp, void *ws, const char *msg, int errLvl)
    {
        SOSStore *sos = (SOSStore *)ws;

        ostringstream ostr;
        ostr << (sos->sos2 ? "sos2: " : "sos: ") << msg;

        const SyntaxElement *sep = modp->syntaxElement(sos->syntaxElem);
        modp->ctrl()->errHandler().error(errLvl, ostr.str().c_str(), sep->loc());
    }


    /**
     * remodel one SOS or SOS2
     * @param modp      intepreter module calling the extension
     * @param om		optimization model
     * @param sos       SOS
     * @return          count of executed remodelations
     */
    unsigned LinearSos::remodelSOS(Interpreter *modp, OptModel *om, SOSStore *sos)
    {
        if ((!sos->sos2 && sos->vars.size() + sos->cntConst <= 1) || (sos->sos2 && sos->vars.size() <= 2)) {
            // no additional variables or constraints necessary
            sos->linearized = true;
            return 0;
        }

        if (sos->noSolution || sos->cntConst >= (sos->sos2 ? 2 : 1)) {
            // linearize degenerated SOS
            linearizeZeroSOS(modp, om, sos, sos->noSolution);
        }
        else {
            // linearization for SOS
            vector<SOSLinearizeInfo> linInfo(sos->vars.size());
            OptVar *vf = linearizeSOSCount(modp, om, sos, linInfo);

            // additional linearization for SOS2
            if (sos->sos2 && vf)
                linearizeSOS2Order(modp, om, sos, linInfo, vf);
        }

        sos->linearized = true;
        return 1;
    }


    /**
     * linearize restriction about count of non zero values within SOS or SOS2
     * @param modp      intepreter module calling the extension
     * @param om		optimization model
     * @param sos       SOS
     * @param linInfo   info object for components of SOS or SOS2
     * @return          first variable in SOS (only used for name)
     */
    OptVar* LinearSos::linearizeSOSCount(Interpreter *modp, OptModel *om, SOSStore *sos, vector<SOSLinearizeInfo>& linInfo)
    {
        ValueTreeRoot& cols = om->cols();
        OptVar *fv = NULL;
        int maxSOSCount = (sos->sos2 ? 2 : 1);

        // generate binary variables for every component variable
        unsigned indPosConst = 0;
        unsigned nextPosConst = (sos->sos2 && indPosConst < sos->posConst.size() ? sos->posConst[indPosConst] : sos->vars.size());

        for (unsigned i = 0; i < sos->vars.size(); i++) {
            unsigned long vid = sos->vars[i];
            SOSLinearizeInfo& info = linInfo[i];

            if (i == nextPosConst) {
                info.isVar = false;
                info.varUB = vid;

                if (info.varUB)
                    maxSOSCount--;

                nextPosConst = (++indPosConst < sos->posConst.size() ? sos->posConst[indPosConst] : sos->vars.size());
            }

            else {
                OptVar *v = (OptVar*)(cols.getElem(vid));
                if (v->binVar()) {
                    // use source variable
                    info.varUB = vid + 1;
                }
                else {
                    // create variable and constraint for upper bound and for lower bound
                    info.varUB = createSOSPartVar(modp, om, v, sos->syntaxElem, true);
                    info.varLB = createSOSPartVar(modp, om, v, sos->syntaxElem, false);
                }

                if (!fv)
                    fv = v;
            }
        }

        // new constraint for max count of non zero elements
        CmplValAuto lc;
        for (const SOSLinearizeInfo& info: linInfo) {
            if (info.isVar) {
                if (info.varUB) {
                    CmplValAuto v(TP_OPT_VAR, (OptVar*)(cols.getElem(info.varUB - 1)));
                    OperationBase::addTo(modp->baseExecCtx(), &lc, sos->syntaxElem, &v);
                }

                if (info.varLB) {
                    CmplValAuto v(TP_OPT_VAR, (OptVar*)(cols.getElem(info.varLB - 1)));
                    OperationBase::addTo(modp->baseExecCtx(), &lc, sos->syntaxElem, &v);
                }
            }
        }

        CmplVal rhs(TP_INT, (intType)maxSOSCount);
        CmplValAuto f(TP_FORMULA, new ValFormulaCompareOp(sos->syntaxElem, &lc, &rhs, false, true, false, true));

        CmplValAuto tpl;
        string nms = (sos->name ? modp->data()->globStrings()->at(sos->name) : getBaseNameTuple(modp, fv, tpl, true, true, _attachNameConSos));

        newOptCon(modp, om, NULL, sos->syntaxElem, f, &nms, &tpl);

        return fv;
    }


    /**
     * create variable and constraint for check if source variable has a value greater or lesser than zero
     * @param modp      intepreter module calling the extension
     * @param om        optimization model
     * @param src       source variable
     * @param se        id of syntax element in the cmpl text of SOS
     * @param upp       true: check for greater than zero / false: check for lesser than zero
     * @return          identy number of created binary variable + 1 / 0: source variable cannot have such a value
     */
    unsigned long LinearSos::createSOSPartVar(Interpreter *modp, OptModel *om, OptVar *src, unsigned se, bool upp)
    {
        // get and check bound
        realType rbnd;
        CmplVal& bnd = (upp ? src->uppBound() : src->lowBound());
        if (bnd.t == TP_INFINITE)
            rbnd = bnd.v.i * _bigM;
        else if (bnd.isScalarNumber())
            rbnd = (bnd.useInt() ? (realType)bnd.v.i : bnd.v.r);
        else
            rbnd = (upp ? _bigM : -_bigM);

        if ((upp && rbnd <= 0) || (!upp && rbnd >= 0))
            return 0;

        // use cache for mapping original variable to new binary variable
        RemodelCacheGuard cg;
        auto& cache = (upp ? _gt0Cache : _lt0Cache);
        unsigned long bvid = cg.checkPrep(cache, src->id());

        if (!bvid) {
            // create new binary variable
            CmplValAuto tpl;
            string srcName = getBaseNameTuple(modp, src, tpl, true);
            string nmv = attachName(modp, srcName, false, (upp ? _attachNameVarSos : _attachNameVarSosNeg));

            OptVar *bv = newOptVar(modp, om, se, modp->binTypeVar(), NULL, NULL, &nmv, &tpl);

            // create new constraint
            //  upp:    rbnd*bv + -1*src >= 0
            //  !upp:   rbnd*bv + -1*src <= 0
            CmplValAuto t1(TP_FORMULA, new ValFormulaVarOp(se, bv, rbnd));
            CmplValAuto t2(TP_FORMULA, new ValFormulaVarOp(se, src, (intType)-1));

            CmplValAuto lc;
            OperationBase::plus(modp->baseExecCtx(), &lc, src->syntaxElem(), &t1, &t2);

            CmplVal rhs(TP_INT, (intType)0);
            CmplValAuto f(TP_FORMULA, new ValFormulaCompareOp(se, &lc, &rhs, upp, !upp, false, true));
            newOptCon(modp, om, NULL, src->syntaxElem(), f);

            bvid = bv->id() + 1;
            if (cg.hasKey())
                cg.insert(cache, bvid);
        }

        return bvid;
    }


    /**
     * linearize restriction order of non zero values within SOS2
     * @param modp      intepreter module calling the extension
     * @param om		optimization model
     * @param sos       SOS
     * @param linInfo   info object for components of SOS2
     * @param fv        first variable in SOS2 (only used for name)
     */
    void LinearSos::linearizeSOS2Order(Interpreter *modp, OptModel *om, SOSStore *sos, vector<SOSLinearizeInfo>& linInfo, OptVar *fv)
    {
        CmplValAuto sos2;
        ValueTreeRoot& cols = om->cols();
        SOSLinearizeInfo *prv = NULL;

        for (unsigned i = 0; i < sos->vars.size(); i++) {
            SOSLinearizeInfo& info = linInfo[i];
            unsigned long vid = sos->vars[i];
            OptVar *src = (OptVar*)(info.isVar ? cols.getElem(vid) : NULL);

            // formula for positive difference to previous element
            CmplValAuto lc;

            if (info.isVar) {
                if (info.varUB) {
                    CmplValAuto v(TP_OPT_VAR, (OptVar*)(cols.getElem(info.varUB - 1)));
                    OperationBase::addTo(modp->baseExecCtx(), &lc, sos->syntaxElem, &v);
                }

                if (info.varLB) {
                    CmplValAuto v(TP_OPT_VAR, (OptVar*)(cols.getElem(info.varLB - 1)));
                    OperationBase::addTo(modp->baseExecCtx(), &lc, sos->syntaxElem, &v);
                }
            }
            else if (info.varUB) {
                CmplVal v(TP_INT, (intType)1);
                OperationBase::addTo(modp->baseExecCtx(), &lc, sos->syntaxElem, &v);
            }

            if (prv) {
                if (prv->isVar) {
                    if (prv->varUB) {
                        CmplValAuto v(TP_OPT_VAR, (OptVar*)(cols.getElem(prv->varUB - 1)));
                        OperationBase::addTo(modp->baseExecCtx(), &lc, sos->syntaxElem, &v, true);
                    }

                    if (prv->varLB) {
                        CmplValAuto v(TP_OPT_VAR, (OptVar*)(cols.getElem(prv->varLB - 1)));
                        OperationBase::addTo(modp->baseExecCtx(), &lc, sos->syntaxElem, &v, true);
                    }
                }
                else if (prv->varUB) {
                    CmplVal v(TP_INT, (intType)1);
                    OperationBase::addTo(modp->baseExecCtx(), &lc, sos->syntaxElem, &v, true);
                }
            }

            CmplValAuto tpl;
            string nmv;
            if (src)
                nmv = getBaseNameTuple(modp, src, tpl, true, true, _attachNameVarSos2);

            OptVar *bv = newOptVar(modp, om, sos->syntaxElem, modp->binTypeVar(), NULL, NULL, &nmv, &tpl);
            info.varOrder = bv->id() + 1;

            CmplValAuto vbv(TP_FORMULA, new ValFormulaVarOp(sos->syntaxElem, bv));
            CmplValAuto f(TP_FORMULA, new ValFormulaCompareOp(sos->syntaxElem, &lc, &vbv, false, true, false, true));
            newOptCon(modp, om, NULL, sos->syntaxElem, f);

            OperationBase::addTo(modp->baseExecCtx(), &sos2, sos->syntaxElem, &vbv);
            prv = &info;
        }

        // constraint for only one positive difference in SOS2
        CmplVal rhs(TP_INT, (intType)1);
        CmplValAuto f(TP_FORMULA, new ValFormulaCompareOp(sos->syntaxElem, &sos2, &rhs, false, true, false, true));

        CmplValAuto tpl;
        string nms;
        if (sos->name || sos->name2) {
            nms = modp->data()->globStrings()->at(sos->name2 ?: sos->name);
            if (!sos->name2)
                nms = attachName(modp, nms, true, _attachNameConSos2);
        }
        else {
            nms = getBaseNameTuple(modp, fv, tpl, true, true, _attachNameConSos2);
        }

        newOptCon(modp, om, NULL, sos->syntaxElem, f, &nms, &tpl);
    }


    /**
     * linearize degenerated SOS by fixing all variables to zero
     * @param modp      intepreter module calling the extension
     * @param om        optimization model
     * @param sos       SOS
     * @param addNoSol  add constraint for no valid solution
     */
    void LinearSos::linearizeZeroSOS(Interpreter *modp, OptModel *om, SOSStore *sos, bool addNoSol)
    {
        ValueTreeRoot& cols = om->cols();
        OptVar *fv = NULL;

        // fix every component variable to zero
        unsigned indPosConst = 0;
        unsigned nextPosConst = (sos->sos2 && indPosConst < sos->posConst.size() ? sos->posConst[indPosConst] : sos->vars.size());

        for (unsigned i = 0; i < sos->vars.size(); i++) {
            unsigned long vid = sos->vars[i];

            if (i == nextPosConst) {
                nextPosConst = (++indPosConst < sos->posConst.size() ? sos->posConst[indPosConst] : sos->vars.size());
            }

            else {
                OptVar *v = (OptVar*)(cols.getElem(vid));
                if (!fv)
                    fv = v;

                CmplValAuto vbv(TP_FORMULA, new ValFormulaVarOp(sos->syntaxElem, v));
                CmplVal rhs(TP_INT, (intType)0);
                CmplValAuto f(TP_FORMULA, new ValFormulaCompareOp(sos->syntaxElem, &vbv, &rhs, true, true, false, true));

                newOptCon(modp, om, NULL, sos->syntaxElem, f);
            }
        }

        if (addNoSol) {
            // generate new optimization variable with 0 as lower and upper bound
            CmplVal bound(TP_INT, (intType)0);
            CmplValAuto ovf;

            CmplVal& dt = modp->realTypeVar();
            OptVar *v = newOptVar(modp, om, sos->syntaxElem, dt, &bound, &bound);

            // add constraint for fixing the new variable to -1 for no valid solution
            CmplValAuto vbv(TP_FORMULA, new ValFormulaVarOp(sos->syntaxElem, v));
            CmplVal rhs(TP_INT, (intType)-1);
            CmplValAuto f(TP_FORMULA, new ValFormulaCompareOp(sos->syntaxElem, &vbv, &rhs, true, true, false, true));

            CmplValAuto tplc;
            string nmc;
            if (sos->name || fv)
                nmc = (sos->name ? modp->data()->globStrings()->at(sos->name) : getBaseNameTuple(modp, fv, tplc, true, true, _attachNameConSos));

            newOptCon(modp, om, NULL, sos->syntaxElem, f, &nmc, &tplc);
        }
    }


    /**
     * get value for model properties
     * @return          0:extension not used / <0:used but already linearized / >0:used and handling still necessary
     */
    int LinearSos::getModelProp()
    {
        int res = 0;

        for (unsigned i = 0; i < _storeSos.size(); i++) {
            const SOSStore& sos = _storeSos[i];
            if (sos.vars.size()) {
                if (!res || (res == -1 && !sos.linearized))
                    res = (sos.linearized ? -1 : 1);
            }
        }

        return res;
    }


    /**
     * initialize info for data transfer to model output
     * @param modp      intepreter module calling the extension
     */
    void LinearSos::initTransferInfo(Interpreter *modp)
    {
        // init _transfer
        for (unsigned s2 = 0; s2 <= 1; s2++) {
            for (unsigned li = 0; li <= 1; li++) {
                OutModelExtData& t = _transfer[OutModelExtData::num((bool)s2, (bool)li)];
                t._ext = this;
                t._sos2 = (bool)s2;
                t._linearized = (bool)li;
                t._rows.clear();
                t._fLoc = NULL;
            }
        }

        // fill info from SOS store
        ValueTreeRoot& cols = modp->getResModel()->cols();
        for (unsigned i = 0; i < _storeSos.size(); i++) {
            const SOSStore& sos = _storeSos[i];
            if (sos.vars.size()) {
                OutModelExtData& t = _transfer[OutModelExtData::num((bool)sos.sos2, (bool)sos.linearized)];
                t._rows.push_back(i);

                if (!t._fLoc)
                    t._fLoc = &(modp->syntaxElement(_storeSos[i].syntaxElem)->loc());

                if (!sos.linearized) {
                    // mark SOS vars as used
                    for (unsigned long vid : sos.vars) {
                        OptVar *v = (OptVar*)(cols.getElem(vid));
                        v->setUsedByCon(1);
                    }
                }
            }
        }
    }

    /**
     * fill info for data transfer to model output
     * @param mod       module calling the extension
     * @param lst       list to fill info in
     */
    void LinearSos::fillTransferInfo(ModuleBase *mod, list<OutModelExtDataBase::Info> *lst)
    {
        // fill info in lst
        for (OutModelExtData& t: _transfer) {
            if (t._rows.size()) {
                //int key = (t._sos2 ? OutModelExtDataSOS1::key : OutModelExtDataSOS2::key);

                int key = (t._sos2 ? OutModelExtDataSOS2Key : OutModelExtDataSOS1Key);

                string errMsg;
                if (!t._linearized)
                    errMsg = string("definition of ") + (t._sos2 ? "SOS2" : "SOS") + (" (change configuration or command line options to use linearization for SOS)");

                lst->emplace_back(key, &t, t._rows.size(), t._linearized, (t._linearized ? 0 : ERROR_LVL_NORMAL), errMsg, t._fLoc);
            }
        }
    }



    /****** SosFunction ****/

    /**
     * calls the cmpl function call operation and store result in execution context, if this value has a special handling for it
     * @param ctx			execution context
     * @param arg			pointer to argument value
     * @return				true
     */
    bool SosFunction::operCall(ExecContext *ctx, StackValue *arg)
    {
        // create SOS
        CmplValAuto vda;
        unsigned indexSOS = createSOS(ctx, arg, vda);
        CmplVal vi(TP_INT, (intType)indexSOS);

        // create result container
        ValContainer *templ = _ext->_templContainer.valContainer();
        templ->copy(ctx, ctx->opResult(), false);

        ValContainer *vc = ctx->opResult().valContainer();
        SymbolValue *sy = vc->getSimple(_ext->_defIdSubOrd);
        sy->setSimpleConstVal(vi);

        if (vda) {
            sy = vc->getSimple(_ext->_defIdSubVarArr);
            sy->setSimpleConstVal(vda);
        }

        return true;
    }

    /**
     * create new SOS
     * @param ctx			execution context
     * @param arg			pointer to argument value
     * @param vds           return of array of new defined variables
     * @return				index number of the SOS in LinearSos::_storeSos
     */
    unsigned SosFunction::createSOS(ExecContext *ctx, StackValue *arg, CmplVal &vda)
    {
        // because internal a LinearSos::SOSStore-pointer to an element in vector is used here, the whole function must be protected
        LockGuard<mutex>(true, _ext->_accMtx);

        unsigned indexSOS = _ext->_storeSos.size();

        // distinguish by first argument between call for existing variables and for creating new variables
        unsigned iter = 0;
        CmplVal *as1 = arg->simpleValue();
        StackValue *a1 = (as1 ? NULL : arg->iterArray(ctx, iter));

        if (as1 || a1) {
            if (a1)
                as1 = (a1 ? a1->simpleValue() : NULL);

            if (a1 && as1 && (as1->isTuple() || as1->isSet())) {
                // creating new array of variables, as1 is definition set, second argument must be a type
                StackValue *a2 = arg->iterArray(ctx, iter);
                StackValue *a3 = arg->iterArray(ctx, iter);
                CmplVal *as2 = (a2 ? a2->simpleValue() : NULL);

                if (a3 || !as2 || as2->t != TP_DATA_TYPE) {
                    if (a3)
                        ctx->valueError("too much arguments for function", a3);
                    else
                        ctx->valueError("second argument must be a data type", (a2 ?: arg));
                }
                else {
                    CmplValAuto resDs;
                    if (SetUtil::convertToSetOrTuple(ctx, resDs, *as1, typeConversionExact, false) && resDs.isSetFin()) {
                        if (resDs.t != TP_SET_NULL) {
                            // create new array and set it as return value
                            CmplArray *arr = new CmplArray(resDs);
                            vda.set(TP_ARRAY, arr);

                            CmplVal sos;
                            if (arr->hasUserOrder()) {
                                CmplArrayIterator iter(*arr, false, true, false);
                                for (iter.begin(); iter; iter++)
                                    createSOSVar(ctx, sos, *iter, as2, arg->syntaxElem());
                            }
                            else {
                                CmplVal *s = arr->at(0);
                                for (unsigned long i = 0; i < arr->size(); i++, s++)
                                    createSOSVar(ctx, sos, s, as2, arg->syntaxElem());
                            }

                            // all array elements are valid values now
                            arr->setValidInfo(true);
                        }
                    }
                    else {
                        ctx->valueError("first argument must be a finite set", a1);
                    }
                }
            }

            else if (arg->val().t != TP_NULL) {
                // argument must be an array of existing variables (or numeric parameters)
                CmplVal sos;
                callForArrayAggr(ctx, sos, arg);

                LinearSos::SOSStore *s = (sos.t == TP_VOIDP ? (LinearSos::SOSStore *)(sos.v.p) : NULL);
                if (s && s->noSolution && _ext->_warnInvalid) {
                    if (_sosType == 1)
                        ctx->valueError("SOS cannot have a valid solution because of contained non zero parameters", arg, ERROR_LVL_WARN);
                    else
                        ctx->valueError("SOS2 cannot have a valid solution because of contained non zero parameters", arg, ERROR_LVL_WARN);
                }
            }
        }

        if (indexSOS == _ext->_storeSos.size()) {
            // no SOS created, make empty SOS as substitute
            _ext->createSOSStore((_sosType == 2), arg->syntaxElem());
        }

        return indexSOS;
    }


    /**
     * create new optimization variable within an array and add the variable to SOS
     * @param ctx           execution context
     * @param sosVal        SOS store
     * @param dst           array element for the new variable
     * @param tp            data type of the variable
     * @param se            syntax element id of source value
     */
    void SosFunction::createSOSVar(ExecContext *ctx, CmplVal& sosVal, CmplVal *dst, CmplVal *tp, unsigned se)
    {
        // create variable
        ObjectTypeUtil::convertTo(ctx, *dst, VAL_OBJECT_TYPE_VAR, tp, se, se);

        // add variable to SOS
        addSOSVar(ctx, sosVal, *dst, se);
    }


    /**
     * add optimization variable (or parameter) to SOS
     * @param ctx			execution context
     * @param sos           SOS store
     * @param src			source value
     * @param se			syntax element id of source value
     */
    void SosFunction::addSOSVar(ExecContext *ctx, CmplVal& sosVal, CmplVal& src, unsigned se)
    {
        // initialize new sos store if necessary
        LinearSos::SOSStore *sos;
        if (sosVal.isEmpty()) {
            sos = _ext->createSOSStore((_sosType == 2), se);
            sosVal.t = TP_VOIDP;
            sosVal.v.p = sos;
        }
        else {
            sos = (LinearSos::SOSStore *)(sosVal.v.p);
        }

        // add variable
        sos->addSOSVar(ctx, src, se);
    }


    /****** SosContFunctionBase ****/

    /**
     * get sos store for the $this container object
     * @param ctx           execution context
     * @param setRes        set container object also as result value
     * @return              pointer to sos store
     */
    LinearSos::SOSStore *SosContFunctionBase::getSOSForThis(ExecContext *ctx, bool setRes)
    {
        ValContainer *vc = ctx->getCallThis();
        LinearSos::SOSStore *res = _ext->getSOSStore(ctx->modp(), vc);

        if (setRes && res)
            ctx->opResult().set(TP_CONTAINER, vc);

        return res;
    }


    /****** SosContFunctionName ****/

    /**
     * calls the cmpl function call operation and store result in execution context, if this value has a special handling for it
     * @param ctx			execution context
     * @param arg			pointer to argument value
     * @return				true
     */
    bool SosContFunctionName::operCall(ExecContext *ctx, StackValue *arg)
    {
        // get argument values (1 or 2 string values)
        unsigned nm1 = 0, nm2 = 0;
        unsigned iter = 0;

        CmplVal *as1 = arg->simpleValue();
        StackValue *a1 = (as1 ? NULL : arg->iterArray(ctx, iter));

        if (as1 || a1) {
            if (a1)
                as1 = (a1 ? a1->simpleValue() : NULL);

            if (as1) {
                CmplVal s1;
                bool disp;

                as1->toString(s1, typeConversionValue, ctx->modp(), disp);
                s1.stringPToStr(ctx->modp(), disp);
                nm1 = s1.v.i;

                if (a1) {
                    StackValue *a2 = arg->iterArray(ctx, iter);
                    if (a2) {
                        CmplVal *as2 = (a2 ? a2->simpleValue() : NULL);
                        if (as2) {
                            CmplVal s2;
                            as2->toString(s2, typeConversionValue, ctx->modp(), disp);
                            s2.stringPToStr(ctx->modp(), disp);
                            nm2 = s2.v.i;
                        }
                    }
                }
            }
        }

        // because internal a LinearSos::SOSStore-pointer to an element in vector is used here, the remaining function must be protected
        LockGuard<mutex>(true, _ext->_accMtx);
        LinearSos::SOSStore *sos = getSOSForThis(ctx, true);

        sos->name = nm1;
        sos->name2 = nm2;

        return true;
    }


    /****** SosContFunctionAdd ****/

    /**
     * calls the cmpl function call operation and store result in execution context, if this value has a special handling for it
     * @param ctx			execution context
     * @param arg			pointer to argument value
     * @return				true
     */
    bool SosContFunctionAdd::operCall(ExecContext *ctx, StackValue *arg)
    {
        // because internal a LinearSos::SOSStore-pointer to an element in vector is used here, the whole function must be protected
        LockGuard<mutex>(true, _ext->_accMtx);

        CmplVal sos;
        callForArrayAggr(ctx, sos, arg);

        getDelVarArrayForThis(ctx, NULL);
        return true;
    }

    /**
     * calls the function for a simple source value (no array or list)
     * @param ctx			execution context
     * @param sosVal		store for result value
     * @param src			source value
     * @param aggr          called for aggregating elements of an array or a list
     * @param se			syntax element id of source value
     * @param info          info object for use by the caller
     * @return              only used if aggr: true if result is final
     */
    bool SosContFunctionAdd::operCallSimple(ExecContext *ctx, CmplVal& sosVal, CmplVal& src, bool aggr, unsigned se, void *info)
    {
        // get sos store if necessary
        LinearSos::SOSStore *sos;
        if (sosVal.isEmpty()) {
            sos = getSOSForThis(ctx, true);
            sosVal.t = TP_VOIDP;
            sosVal.v.p = sos;
        }
        else {
            sos = (LinearSos::SOSStore *)(sosVal.v.p);
        }

        // add variable
        sos->addSOSVar(ctx, src, se);
        return false;
    }


    /****** SosContFunctionAsVar ****/

    /**
     * calls the cmpl function call operation and store result in execution context, if this value has a special handling for it
     * @param ctx			execution context
     * @param arg			pointer to argument value
     * @return				true
     */
    bool SosContFunctionAsVar::operCall(ExecContext *ctx, StackValue *arg)
    {
        getDelVarArrayForThis(ctx, &(ctx->opResult()));
        return true;
    }


    /****** SosContFunctionAsString ****/

    /**
     * calls the cmpl function call operation and store result in execution context, if this value has a special handling for it
     * @param ctx			execution context
     * @param arg			pointer to argument value
     * @return				true
     */
    bool SosContFunctionAsString::operCall(ExecContext *ctx, StackValue *arg)
    {
        // because internal a LinearSos::SOSStore-pointer to an element in vector is used here, the whole function must be protected
        LockGuard<mutex>(true, _ext->_accMtx);
        LinearSos::SOSStore *sos = getSOSForThis(ctx, false);

        ostringstream o;
        sos->write(o, ctx->modp());

        CmplVal& res = ctx->opResult();
        res.t = TP_STRINGP;
        res.v.s = new string(o.str());

        return true;
    }

}


