
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


#include <sstream>

#include "ExecContext.hh"
#include "Interpreter.hh"
#include "ValueStore.hh"
#include "Interval.hh"
#include "TupleUtil.hh"
#include "SetUtil.hh"
#include "ValFunction.hh"
#include "ValType.hh"
#include "ValContainer.hh"
#include "ValSpecial.hh"
#include "OperationBase.hh"
#include "../../Control/MainControl.hh"
#include "../../CommonData/IntCode.hh"



namespace cmpl
{

	/*********** ExecContext **********/

	/**
	 * constructor
	 * @param mod			interpreter module
	 * @param prv			parent execution context / NULL: this is the root execution context
     * @param fct           execution context is a function context
     * @param fctArg		only for function context: pointer to function argument
	 */
    ExecContext::ExecContext(Interpreter *mod, ExecContext *prv, bool fct, StackValue *fctArg)
	{
		_modp = mod;
		_parent = prv;
		_level = (prv ? prv->_level + 1 : 0);
        _fctContext = fct;
        _needLock = (prv ? prv->_needLock : false);

        _localSymbols = NULL;
        _localSymbolCap = 0;
        _localSymbolCreateTo = 0;

		_stackTop = 0;
		_stackCap = ARR_NEWCAP(0);
		_stack = NULL;
		ARR_REALLOC(_stack, StackValue, _stackTop, _stackCap);

        _fctArg = (fct ? fctArg : (prv ? prv->_fctArg : NULL));
        _fctThis = (fct && prv ? prv->_callThis : (prv ? prv->_fctThis : NULL));
        _callThis = NULL;

        _cbContextTop = 0;
        _cbContextCreateTop = 0;
        _cbContext = NULL;

        if (_fctArg) {
            _cbContextCap = ARR_NEWCAP(0);
            ARR_REALLOC(_cbContext, CBContextNmPref, _cbContextTop, _cbContextCap);
        }
        else {
            _cbContextCap = 0;
        }

        _nmPrefCmd = (!fct && prv ? prv->_nmPrefCmd : -1);
        _nmPrefStart.cbContext = NULL;
        if (fct && prv && (prv->_nmPrefCmd >= 0 || prv->_cbContextTop)) {
            _nmPrefStart.nmPref = (prv->_nmPrefCmd >= 0 ? prv->_nmPrefCmd : prv->_cbContext[prv->_cbContextTop - 1].nmPref);
            _nmPrefStart.nmPrefSet = (prv->_nmPrefCmd >= 0 ? 1 : prv->_cbContext[prv->_cbContextTop - 1].nmPrefSet);
        }
        else if (prv) {
            _nmPrefStart.nmPref = prv->_nmPrefStart.nmPref;
            _nmPrefStart.nmPrefSet = prv->_nmPrefStart.nmPrefSet;
        }
        else {
            _nmPrefStart.nmPref = -1;
            _nmPrefStart.nmPrefSet = 0;
        }

        _topIterVal.init();
        _iterVals = NULL;
        _iterValsCap = 0;
        _iterValsTop = 0;

        _curVarCondMap = (prv ? prv->_curVarCondMap : NULL);

        _assRhsNo = -1;
        _assDataType = NULL;

        _cancel = false;
	}

	/**
	 * destructor
	 */
	ExecContext::~ExecContext()
	{
        if (_localSymbols) {
            // only for error handling: uninitialize symbols
            for (unsigned i = _localSymbolCreateTo; i < _localSymbolCap; i++)
                _localSymbols[i].unset();

            delete[] _localSymbols;
        }

        if (_stack) {
            // only for error handling: empty stack
            if (_stackTop > 0)
                stackPopTo(_stack);

            delete[] _stack;
        }

        if (_cbContext) {
            // only for error handling: delete codeblock contexts
            for (unsigned i = _cbContextCreateTop; i < _cbContextTop; i++) {
                if (_cbContext[i].cbContext)
                    delete _cbContext[i].cbContext;
            }

            delete[] _cbContext;
        }

        if (_iterVals) {
            for (unsigned i = 0; i < _iterValsCap; i++)
                _iterVals[i].dispose();

            delete[] _iterVals;
        }

        _topIterVal.dispose();

        while (_curVarCondMap && _curVarCondMap->_execContext == this) {
            VarCondMapping *vcm = _curVarCondMap;
            _curVarCondMap = vcm->_parent;
            delete vcm;
        }

        _opRes.dispose();
	}


    /**
     * creates new execution context object for iteration execution within own thread
     * @param lsdt          count of initialized local symbols in <code>prv</code>
     * @return              new execution context
     */
    ExecContext *ExecContext::newAsIterCopy(unsigned lsdt)
    {
        ExecContext *res = new ExecContext(_modp, this, false);
        res->_needLock = true;

        // local symbol values
        if (_localSymbolCap) {
            res->_localSymbolCap = _localSymbolCap;
            res->_localSymbols = new SymbolValue[_localSymbolCap];

            if (lsdt) {
                res->_localSymbolCreateTo = lsdt;

                // copy local symbols values without increment reference counter, because the symbols up to <code>lsdt</code> are never uninitialized within the the new execution context
                memcpy(res->_localSymbols, _localSymbols, sizeof(SymbolValue) * lsdt);
            }
        }

        // codeblock contexts
        if (_cbContextCap) {
            res->_cbContextCap = _cbContextCap;
            res->_cbContextTop = res->_cbContextCreateTop = _cbContextTop;

            ARR_REALLOC(res->_cbContext, CBContextNmPref, 0, res->_cbContextCap);
            memcpy(res->_cbContext, _cbContext, sizeof(CBContextNmPref) * res->_cbContextTop);
        }

        return res;
    }

    /**
     * reinitialize execution context object for reuse in another thread for iteration execution
     */
    void ExecContext::reinitIterCopy()
    {
        _cancel = false;

        if (_stackTop)
            internalError("invalid computation stack state in execution context for iteration");

        if (_cbContextTop != _cbContextCreateTop)
            internalError("invalid codeblock context stack state in execution context for iteration");

        if (_localSymbolCap > _localSymbolCreateTo && !_localSymbols[_localSymbolCreateTo].isUninitialized())
            internalError("invalid local symbol array state in execution context for iteration");
    }

    /**
     * release execution context object created for iteration execution within own thread
     */
    void ExecContext::disposeIterCopy()
    {
        //TODO?
    }


    /**
     * get start of the array with intermediary code
     */
    IntCode::IcElem *ExecContext::codeBase() const
    {
        return _modp->_code;
    }


	/**
	 * main function, runs the main loop for execution of the intermediary code
     * @param startPos      start position within the intermediary code
     * @param userFct       execution of a user defined function
     * @param res           pointer for storing result (NULL if not used)
     */
    void ExecContext::run(unsigned startPos, bool userFct, CmplVal *res)
	{
        PROTO_MOD_OUTL(_modp, "start main loop in execution context (level " << _level << ") at code pos " << startPos);

        IntCode::IcElem *curCommand = _modp->_code + startPos;
		IntCode::IcElem *cMax = _modp->_code + _modp->_codeCnt;
		IntCode::IcElem *ncd;

        // check for execution of single codeblock header or single codeblock body
        bool cbHeader = false, cbBody = false;
        unsigned cbLvl = 0;
        unsigned long stackTopStart = _stackTop;
        unsigned cbTopStart = _cbContextTop;
        bool cbCancel = false, endCtx = false;

        if (curCommand->tp == IntCode::icTypeCommand && (curCommand->v.c.major == INTCODE_CODEBLOCK || curCommand->v.c.major == INTCODE_CB_HEADER)) {
            if (curCommand->v.c.major == INTCODE_CODEBLOCK && curCommand->v.c.minor == ICS_CB_PART_BODY)
                cbBody = true;
            else if (curCommand->v.c.major == INTCODE_CB_HEADER && curCommand->v.c.minor == ICS_CBHEADER_START)
                cbHeader = true;
        }

        // check for execution of user defined function or of whole code
        if (userFct || startPos == 0) {
            PROTO_MOD_OUT_IC(_modp, (unsigned)(curCommand - _modp->_code), curCommand);
            if (curCommand->tp != IntCode::icTypeCommand || curCommand->v.c.major != INTCODE_FUNCTION || curCommand->v.c.minor != ICS_FUNC_START || curCommand->v.c.cnt != 0)
                internalError((startPos ? "invalid function start within intermediary code" : "invalid start of intermediary code"));

            _localSymbolCap = curCommand->v.c.par;
			_localSymbols = new SymbolValue[_localSymbolCap];

			//TODO: init fuer lokale Symbole; sonst?
			// (lokale Symbole können hier mangels Info nicht weiter initialisiert werden, erst beim ersten Fetch darauf (darin steht dann auch die eindeutige Symbolnummer))

            curCommand++;
		}

		// execution main loop
        while (curCommand < cMax) {
            _curCommand = curCommand;
#if PROTO
			if (_modp->isProtocol()) {
                curCommand->protoOut(_modp, (unsigned)(curCommand - codeBase()));
                for (unsigned i = 1; i <= curCommand->v.c.cnt; i++)
                    curCommand[i].protoOut(_modp, (unsigned)(curCommand + i - codeBase()));
			}
#endif

            if (_cancel && !cbCancel) {
                LockGuard<recursive_mutex> lck(_needLock, _cancelMtx);

                if ((cbHeader || cbBody) && cbTopStart > _cancelCbLvl) {
                    stackPopTo(_stack + _stackTop, true);
                    if (cbHeader)
                        res->set(TP_BIN, false);
                    else
                        res->set(TP_BLANK);

                    if (_cbContextTop > cbTopStart)
                        popCBContext(curCommand->se, _cbContextTop - cbTopStart);

                    if (_cancelCont && cbTopStart == _cancelCbLvl + 1)
                        _cancel = false;

                    PROTO_MOD_OUTL(_modp, "leave execution context (level " << _level << ") for single codeblock header or body after cancel signal at code pos " << (unsigned)(curCommand - _modp->_code) << endl << "  -> result: " << res);
                    return;
                }
                else {
                    cbCancel = true;
                }
            }

            if (curCommand->tp != IntCode::icTypeCommand)
				internalError("invalid element in intermediary code");

            switch (curCommand->v.c.major) {
				case INTCODE_NOP:
                    ncd = curCommand + (curCommand->v.c.cnt + 1);
					break;

				case INTCODE_LIT_VAL:
                    ncd = execCodeLitVal(curCommand);
					break;

				case INTCODE_FETCH:
                    ncd = execCodeFetch(curCommand);
					break;

				case INTCODE_FETCH_SPECIAL:
                    ncd = execCodeFetchSpecial(curCommand);
					break;

				case INTCODE_ASSIGN:
                    ncd = execCodeAssign(curCommand);
					break;

				case INTCODE_OPERATION:
                    ncd = execCodeOperation(curCommand);
					break;

				case INTCODE_CONSTRUCT:
                    ncd = execCodeConstruct(curCommand);
					break;

				case INTCODE_LINENAMEPREF:
                    ncd = execCodeLineNamePref(curCommand);
                    break;

				case INTCODE_FUNCTION:
                    ncd = execCodeFunction(curCommand);
					if (!ncd) {
                        if (res) {
                            StackValue *sv = stackCur();
                            res->copyFrom(sv->val(), true, false);
                            pop();
                        }
                        PROTO_MOD_OUTL(_modp, "leave execution context (level " << _level << ") at code pos " << (unsigned)(curCommand - _modp->_code));
                        return;
					}
					break;

				case INTCODE_CODEBLOCK:
                    ncd = execCodeCodeblock(curCommand, cbBody, cbLvl, cbCancel, endCtx);
                    if (endCtx) {
                        if (_stackTop > stackTopStart) {
                            StackValue *sv = stackCur();
                            res->copyFrom(sv->val(), true, false);
                            pop();
                        }
                        else {
                            res->unset();
                        }
                        PROTO_MOD_OUTL(_modp, "leave execution context (level " << _level << ") for single codeblock body at code pos " << (unsigned)(curCommand - _modp->_code) << endl << "  -> body result: " << res);
                        return;
                    }
                    break;

                case INTCODE_CB_HEADER:
                    ncd = execCodeCBHeader(curCommand, cbHeader);
                    if (cbHeader && !cbLvl && curCommand->v.c.minor == ICS_CBHEADER_END) {
                        StackValue *sv = stackCur();
                        res->copyFrom(sv->val(), true, false);
                        pop();
                        PROTO_MOD_OUTL(_modp, "leave execution context (level " << _level << ") for single codeblock header at code pos " << (unsigned)(curCommand - _modp->_code) << endl << "  -> header result: " << res);
                        return;
                    }
                    break;

				case INTCODE_CB_CONTROL:
                    ncd = execCodeCBControl(curCommand, (cbHeader || cbBody), cbLvl, cbTopStart);
                    if (!ncd) {
                        stackPopTo(_stack + _stackTop, true);
                        if (cbHeader)
                            res->set(TP_BIN, false);
                        else
                            res->moveFrom(_opRes);

                        PROTO_MOD_OUTL(_modp, "leave execution context (level " << _level << ") for single codeblock header or body after control command at code pos " << (unsigned)(curCommand - _modp->_code) << endl << "  -> result: " << res);
                        return;
                    }
					break;

				default:
					internalError("invalid command in intermediary code");
			}

            curCommand = ncd;
		}

		_modp->ctrl()->errHandler().internalError(_modp->ctrl()->printBuffer("unexpected end of intermediary code, at code pos %u", _modp->_codeCnt));
	}


	/**
	 * executes intermediary code command for literal value
	 * @param cd		intermediary code command
	 * @return			next intermediary code position / NULL if you have to leave this execution context after the command
	 */
	IntCode::IcElem* ExecContext::execCodeLitVal(IntCode::IcElem *cd)
	{
		switch (cd->v.c.minor) {
			case ICS_LIT_INT:
				pushInt((cd->v.c.cnt ? cd[1].v.i : cd->v.c.par), cd->se);
				break;

			case ICS_LIT_STR:
				pushString((cd->v.c.cnt ? cd[1].v.i : cd->v.c.par), cd->se);
				break;

			case ICS_LIT_REAL:
				pushReal(cd[1].v.r, cd->se);
				break;

			case ICS_LIT_BLANK:
				pushBlank(cd->se);
				break;
		}

		return cd + (cd->v.c.cnt + 1);
	}

	/**
	 * executes intermediary code command for fetch a value
	 * @param cd		intermediary code command
	 * @return			next intermediary code position / NULL if you have to leave this execution context after the command
	 */
	IntCode::IcElem* ExecContext::execCodeFetch(IntCode::IcElem *cd)
	{
		unsigned int fetchId = cd[1].v.n.n2;

		SymbolValue *symval;
        CmplValAuto valCont(TP_EMPTY);
        bool needLockSym, needLockVS;

		if (cd->v.c.par & ICPAR_FETCH_SUBSYM) {
            CmplVal *base = stackCur()->simpleValue();
            fetchId = 0;

            if (!base || base->t != TP_CONTAINER) {
                valueError("left operand of subsymbol operator is not a container value", stackCur());
                symval = NULL;
            }
            else {
                ValContainer *cont = base->valContainer();
                {
                    needLockSym = _needLock;
                    LockGuard<mutex> lckC(_needLock, cont->accMtx());
                    symval = cont->getInsertSymbol(this, cd);
                }

                // distinct whether previous fetch was ICPAR_FETCH_NMSPACE (then addval of left operand is TP_STR)
                CmplVal& pav = stackCur()->_addVal;
                if (pav.t == TP_STR) {
                    if (cd->v.c.par & ICPAR_FETCH_NMSPACE) {
                        // concat symbol names for full namespace name
                        const char *nms = modp()->ctrl()->printBuffer("%s.%s", modp()->data()->globStrings()->at(pav.v.i), modp()->symbolInfo(symval->defId())->name());
                        valCont.set(TP_STR, (intType)(modp()->data()->globStrings()->storeInd(nms)));
                    }
                    else {
                        valCont.copyFrom(pav, true, false);
                    }
                }
                else if (!(cd->v.c.par & ICPAR_FETCH_LVALUE)) {
                    // use container value as addval of result
                    valCont.copyFrom(base, true, false);
                }
            }

            pop();
		}
        else if (cd->v.c.minor == ICS_FETCH_GLOBAL) {
            symval = _modp->_globSymbols + fetchId;
            needLockSym = _needLock;
        }
        else {
            symval = _localSymbols + fetchId;
            if (!symval->defId())
                symval->init((*_modp->_symbolInfo)[cd[1].v.n.n1], false, false);

            needLockSym = (_needLock && fetchId < _localSymbolCreateTo && !symval->readOnly());
		}

		StackValue *sv = pushPre(cd->se);
        if (cd->v.c.minor == ICS_FETCH_DEF_CB) {
            // code block symbol definition, use special value
            sv->_val.set(TP_DEF_CB_SYM, (intType) fetchId);
        }
        else if (cd->v.c.par & ICPAR_FETCH_LVALUE) {
			// lvalue, fetch as symbol value
            sv->_val.setP(TP_SYMBOL_VAL, symval);
		}
        else if (!symval) {
            // error, already reported
            sv->_val.set(TP_INT, 0);
        }
        else {
            //TODO: Pruefung auf Mapping
            VarCondMapVS *map = (_curVarCondMap ? checkGetMappedVS(symval): NULL);

            if (cd->v.c.par & ICPAR_FETCH_INCDEC) {
                intType i;
                {
                    LockGuard<mutex> lckS(needLockSym, symval->accMtx());

                    if (symval->readOnly() || !(symval->hasValueStore()) || map) {
                        CmplVal v;
                        v.setP(TP_SYMBOL_VAL, symval);
                        valueError((symval->readOnly() ? "constant symbol used for increment/decrement" : (!(symval->hasValueStore()) ? "uninitialized symbol used for increment/decrement" : "inconsistent mix of assignments within a codeblock part with conditions over optimization variables")), v, cd->se);
                        i = 0;
                    }
                    else {
                        needLockVS = (_needLock && (fetchId < _localSymbolCreateTo || symval->valueStore()->values()->refCnt() > 1));
                        LockGuard<mutex> lckV(needLockVS, symval->valueStore()->accMtx());


                        i = symval->valueStore()->fetchInc(this, (bool)(cd[2].v.n.n1), (cd[2].v.n.n2 ? 1 : -1), cd->se);
                    }
                }

                sv->_val.set(TP_INT, i);
                sv->_syntaxElem = cd[2].se;
            }
            else {
                LockGuard<mutex> lckS(needLockSym, symval->accMtx());
                ValueStore *vs = (map ? map->getDstVS() : (symval->hasValueStore() ? symval->valueStore() : NULL));

                needLockVS = (_needLock && vs && !vs->isConst());
                LockGuard<mutex> lckV(needLockVS, (vs ? &(vs->accMtx()) : NULL));

                // if simple value then fetch value directly to stack, else fetch as array
                CmplVal *v = (vs ? vs->simpleValue() : symval->simpleValue());
                if (v)
                    sv->_val.copyFrom(*v, true, false);
                else if (vs)
                    sv->_val.set(TP_ARRAY, vs->values(), true);
                else
                    sv->_val.set(TP_NULL);		// value for symbol is not set
            }
        }

        if (valCont) {
            sv->_addVal.moveFrom(valCont);
        }
        else if (cd->v.c.par & ICPAR_FETCH_NMSPACE) {
            const char *symNameStr = modp()->symbolInfo(symval->defId())->name();
            sv->_addVal.set(TP_STR, (intType)(modp()->data()->globStrings()->search(symNameStr)));
        }

		return cd + (cd->v.c.cnt + 1);
	}

    /**
     * executes intermediary code command for fetch of a pseudo symbol
     * @param cd		intermediary code command
     * @return			next intermediary code position / NULL if you have to leave this execution context after the command
     */
    IntCode::IcElem* ExecContext::execCodeFetchSpecial(IntCode::IcElem *cd)
    {
        bool lvalue = cd->v.c.par & ICPAR_FETCH_LVALUE;
        bool subsym = cd->v.c.par & ICPAR_FETCH_SUBSYM;

        if (cd->v.c.par & ICPAR_FETCH_INCDEC) {
            execError("increment/decrement not allowed with pseudo symbol", cd->se);
        }

        CmplVal *base = NULL;
        bool doPop = false;

        if (subsym) {
            base = stackCur()->simpleValue();

            if (!base) {
                valueError("invalid use of subsymbol operator", stackCur());

                CmplValAuto dv(TP_NULL);
                if (lvalue)
                    dv.set(TP_SPECIALSYM, new ValSpecialDummy(this, cd->se, lvalue, NULL));

                pop();
                pushVal(dv, cd->se);
                return cd + (cd->v.c.cnt + 1);
            }

            doPop = true;
        }

        CmplValAuto res(TP_NULL);
        if (lvalue) {
            res.set(TP_SPECIALSYM, ValSpecialBase::create(this, cd->v.c.minor, cd->se, lvalue, base));
        }
        else {
            // handle $arg and $this here for better performance
            if (cd->v.c.minor == ICS_SPECIAL_ARG || cd->v.c.minor == ICS_SPECIAL_THIS) {
                if (cd->v.c.minor == ICS_SPECIAL_ARG) {
                    if (_fctArg)
                        res.copyFrom(_fctArg->val(), true, false);
                    else
                        execError("pseudosymbol '$arg' cannot be used outside of a function", cd->se);
                }
                else {
                    if (_fctThis)
                        res.set(TP_CONTAINER, _fctThis);
                    else
                        execError("no '$this' container available", cd->se);
                }
            }
            else {
                CmplValAuto v(TP_SPECIALSYM, ValSpecialBase::create(this, cd->v.c.minor, cd->se, lvalue, base));
                ValSpecialBase *sp = v.valSpecial();

                if (sp->hasIndexedHandling())
                    res.moveFrom(v);
                else
                    sp->get(res);
            }
        }

        if (doPop)
            pop();

        pushVal(res, cd->se);
        return cd + (cd->v.c.cnt + 1);
    }

	/**
	 * executes intermediary code command for assign a value
	 * @param cd		intermediary code command
	 * @return			next intermediary code position / NULL if you have to leave this execution context after the command
	 */
	IntCode::IcElem* ExecContext::execCodeAssign(IntCode::IcElem *cd)
	{
		// get params
		unsigned cntLhs = cd->v.c.par & ICPAR_ASSIGN_CNTLHS;
		bool assignConst = cd->v.c.par & ICPAR_ASSIGN_CONST;
        unsigned assignOrdered = (cd->v.c.par & ICPAR_ASSIGN_ORDERED ? 2 : (_modp->orderedIter() ? 1 : 0));
        bool assignInitial = cd->v.c.par & ICPAR_ASSIGN_INITIAL;    //TODO
        bool assignNocond = cd->v.c.par & ICPAR_ASSIGN_NOCOND;
		bool setResName = cd->v.c.par & ICPAR_ASSIGN_RESNAME;
        PROTO_MOD_OUTL(_modp, "  cntLhs " << cntLhs << "; assignConst " << assignConst << "; assignInitial " << assignInitial << "; assignNocond " << assignNocond << "; assignOrdered " << assignOrdered << "; setResName " << setResName);

		// get object type and data type
        _assObjType = -1;
        _assDataType = NULL;
        _assSyntaxElem = cd->se;
        _assStartVolaRhs = 0;
        _assNextRhs = false;
        _assOrdered = (assignOrdered != 0);

		if (cd->v.c.cnt > 0) {
            bool locAssType = cd->v.c.par & ICPAR_ASSIGN_OBJTYPE_LOCAL;
			unsigned i = cd[1].v.n.n1;
            if (i || locAssType) {
                SymbolValue *symval = (locAssType ? (_localSymbols + i) : (_modp->_globSymbols + i));
				CmplVal *v = symval->simpleValue();
				if (!v || v->t != TP_OBJECT_TYPE)
					internalError("given object type in assignment is not an object type");
                _assObjType = v->v.i;
			}

            locAssType = cd->v.c.par & ICPAR_ASSIGN_DATATYPE_LOCAL;
			i = cd[1].v.n.n2;
            if (i || locAssType) {
                SymbolValue *symval = (locAssType ? (_localSymbols + i) : (_modp->_globSymbols + i));
				CmplVal *v = symval->simpleValue();
				if (!v || v->t != TP_DATA_TYPE)
					internalError("given data type in assignment is not a data type");
                _assDataType = v->valType();
			}

			if (cd->v.c.cnt > 1)
                _assStartVolaRhs = cd[2].v.n.n1;
		}

        if (assignOrdered == 2 && !_assStartVolaRhs)
            assignOrdered = 1;

        unsigned svLhsNo;
        char op = '\0';

        _assRhsNo = _stackTop - 1;
        PROTO_MOD_OUTL(_modp,  "  rhs = " << assRhs()->val());
		
		if (cd->v.c.minor == ICS_ASSIGN_RHSONLY) {
            if (assRhs()->val().t == TP_NULL || (_assObjType != VAL_OBJECT_TYPE_CON && _assObjType != VAL_OBJECT_TYPE_OBJ)) {
                if (_assObjType == VAL_OBJECT_TYPE_VAR && assRhs()->val().t != TP_NULL) {
                    CmplValAuto t;
                    if (!checkContainerConvSpecial(t, assRhs()->val(), TP_OPT_VAR) || t.t != TP_NULL)
                        valueError("expression without assignment cannot be used with modificator 'var'", assRhs());
                }
			}
            else if (setResName && (_assObjType == VAL_OBJECT_TYPE_CON || _assObjType == VAL_OBJECT_TYPE_OBJ)) {
                // convert right hand side to constraint or objective
                StackValue *sv = getCurAssignRhs(_assSyntaxElem);
                if (curNmPref() > 0) {
                    CmplVal *ssv = sv->simpleValue();
                    ValueStore::setValInValueTree(this, (ssv ? ssv : &(sv->val())), _assSyntaxElem, _modp->getResModel());
                }
			}
            stackPopTo(assRhs());
        }
		else {
            switch (cd->v.c.minor) {
                case ICS_ASSIGN_LHSONLY:
                    assignInitial = true;
                    // no break

                case ICS_ASSIGN_EXTERN:
                    if (_assObjType == VAL_OBJECT_TYPE_OBJ || _assObjType == VAL_OBJECT_TYPE_CON)
                        valueError("alone left hand side cannot be used with modificator 'con' or 'obj'", assRhs());
                    // no break

                case ICS_ASSIGN_ASSERT:
                    svLhsNo = _assRhsNo;
                    _assRhsNo = -1;
                    break;

                case ICS_ASSIGN_REF:
                    if (!assRhs()->lvalue())
                        valueError("internal error: lvalue expected on stack, but not given", assRhs(), ERROR_LVL_FATAL);
                    if (assRhs()->hasIndex())
                        valueError("right hand side of ref assignment cannot have indexation", assRhs());
                    if (_curVarCondMap && !assignNocond)
                        valueError("ref assignment not possible within codeblock with condition over optimization variables", assRhs());
                    _assObjType = -1;
                    _assDataType = NULL;
                    break;

                case ICS_ASSIGN_ADD:
                    op = '+';
                    break;
                case ICS_ASSIGN_SUB:
                    op = '-';
                    break;
                case ICS_ASSIGN_MUL:
                    op = '*';
                    break;
                case ICS_ASSIGN_DIV:
                    op = '/';
                    break;
            }

            if (_assRhsNo >= 0) {
                // if right hand side is a list then convert it to an array or tuple
                if (assRhs()->isList()) {
                    replaceListOnStack(assRhs());
                    _assRhsNo = _stackTop - 1;
                }
                //TODO: Wenn RHS Typ TP_SPECIALSYM, dann darin get aufrufen und auf stattdessen auf den Stack
                //          oder vielleicht diese Umwandlung mit in replaceListOnStack()?

                svLhsNo = stackPrev(assRhs()) - _stack;
			}

            for (unsigned iLhs = 1; iLhs < cntLhs; iLhs++)
                svLhsNo = stackPrev(_stack + svLhsNo) - _stack;

            unsigned svLhsBaseNo = svLhsNo;
            for (unsigned iLhs = 0; iLhs < cntLhs; iLhs++, svLhsNo++) {
                if (!_stack[svLhsNo].lvalue())
                    valueError("internal error: lvalue expected on stack, but not given", _stack + svLhsNo);
                else if (_stack[svLhsNo].val().t == TP_SYMBOL_VAL) {
                    SymbolValue *sym = (SymbolValue *)(_stack[svLhsNo].val().v.p);
                    if (sym) {
                        bool needLockSym = (_needLock && (sym->global() || (!sym->global() && (sym - _localSymbols) < _localSymbolCreateTo)));
                        LockGuard<mutex> lckS(needLockSym, sym->accMtx());

                        VarCondMapVS *map = NULL;
                        if (_curVarCondMap) {
                            bool ncerr;
                            map = checkGetMappedVS(sym, (cd->v.c.minor != ICS_ASSIGN_ASSERT && !assignConst && !assignNocond), assignNocond, ncerr);
                            if (ncerr || (map && assignConst)) {
                                valueError("inconsistent mix of conditional and non-conditional assignments within a codeblock part with conditions over optimization variables", _stack + svLhsNo);
                                map = NULL;
                            }
                        }
                        //TODO: Verwendung map in den folgenden Funktionsaufrufen bei der Ausfuehrung der Zuweisungsfunktionalitaet

                        if (cd->v.c.minor == ICS_ASSIGN_EXTERN)
                            _stack[svLhsNo].importExtern(this, assignConst, cd->se);
                        else if (cd->v.c.minor == ICS_ASSIGN_ASSERT)
                            _stack[svLhsNo].checkAssert(this, assignConst, _assDataType, _assObjType, cd->se);
                        else
                            _stack[svLhsNo].doAssign(this, op, (cd->v.c.minor == ICS_ASSIGN_REF), assignConst, assignInitial, assignOrdered, setResName, map);
                    }
                }
                else {  // _stack[svLhsNo].val().t == TP_SPECIALSYM
                    if (cd->v.c.minor == ICS_ASSIGN_EXTERN || cd->v.c.minor == ICS_ASSIGN_ASSERT || cd->v.c.minor == ICS_ASSIGN_REF || assignConst || setResName)
                        valueError("invalid assign modificator for assign to pseudo symbol", _stack + svLhsNo);
                    else
                        _stack[svLhsNo].doAssignSpecial(this, op, assignInitial, assignOrdered);
                }
			}

            stackPopTo(_stack + svLhsBaseNo);
		}

		return cd + (cd->v.c.cnt + 1);
	}


	/**
	 * executes intermediary code command for execution of an operation
	 * @param cd		intermediary code command
	 * @return			next intermediary code position / NULL if you have to leave this execution context after the command
	 */
	IntCode::IcElem* ExecContext::execCodeOperation(IntCode::IcElem *cd)
	{
        intType svNo1, svNo2;
        intType sveNo = -1;
        CmplVal *v;
        intType i;
        unsigned short ac;
        bool pushRes = true;
        bool transp = false;

		switch (cd->v.c.minor) {
			case ICS_OPER_FUNC:
				// function call
                svNo1 = stackCur() - _stack;
                sveNo = svNo2 = stackPrev(_stack + svNo1) - _stack;
                v = _stack[svNo2].simpleValue();

                if (!v)
                    valueError("function call for a not function-like value", _stack + svNo2);

                if (v->t == TP_OBJECT_TYPE) {
                    //Umwandlung/Konstruktor
                    //TODO
                }
                else if (v->t == TP_FUNCTION || v->t == TP_DATA_TYPE) {
                    ValContainer *pt = _callThis;
                    _callThis = (_stack[svNo2]._addVal.t == TP_CONTAINER ? _stack[svNo2]._addVal.valContainer() : NULL);
                    if (!((ValFunctionBase *)(v->v.vp))->operCall(this, _stack + svNo1))
                        valueError("internal error: unhandled function call", _stack + svNo2, ERROR_LVL_FATAL);
                    _callThis = pt;
                }
                else if (v->toInt(i, typeConversionExact, modp())) {
                    // construct algorithmic set: first pseudo function call for expression like "1(1)(n)"
                    SetUtil::constructAlg(this, _opRes, _stack + svNo2, _stack + svNo1, NULL);
                }
                else if (SET_TP_WO_ORDER(*v) == TP_SET_R1_ALG && ((SetAlg *)(SET_VAL_WO_ORDER(*v).v.vp))->incomplete()) {
                    // construct algorithmic set: second pseudo function call for expression like "1(1)(n)"
                    SetUtil::constructAlg(this, _opRes, v->setBase(), _stack + svNo1);
                }
                else {
                    valueError("internal error: unhandled function call", _stack + svNo2, ERROR_LVL_FATAL);
                }
                break;

            case ICS_OPER_INDEX:
                // indexation operation
                svNo1 = stackCur() - _stack;
                sveNo = svNo2 = stackPrev(_stack + svNo1) - _stack;

                if (!_stack[svNo2].indexation(this, _stack + svNo1)) {
                    pushRes = false;
                    sveNo = svNo1;
                }
                break;

            case ICS_OPER_ARRCAST:
                // array cast operation
                svNo1 = stackCur() - _stack;
                sveNo = svNo2 = stackPrev(_stack + svNo1) - _stack;
                _stack[svNo1].arraycast(this, _stack + svNo2);
                break;

            default:
                // replace lists by arrays
                ac = cd->v.c.par & ICPAR_OPER_CNT;
                if (ac > 0) {
                    if (ac == 1) {
                        sveNo = svNo1 = replaceListOnStack(stackCur()) - _stack;
                        svNo2 = -1;
                    }
                    else {
                        svNo2 = replaceListOnStack(stackCur()) - _stack;
                        sveNo = svNo1 = stackPrev(_stack + svNo2) - _stack;
                        if (_stack[svNo1].isList()) {
                            CmplVal rl;
                            rl.setP(TP_REF_LIST, _stack + svNo1);
                            pushVal(rl, _stack[svNo1].syntaxElem());
                            svNo1 = replaceListOnStack(stackCur()) - _stack;
                        }
                    }
                }
                else {
                    sveNo = svNo1 = svNo2 = -1;
                }

                // execute operation
                if (!OperationBase::execOper(this, _opRes, transp, cd->se, cd->v.c.minor, (bool)(cd->v.c.par & ICPAR_OPER_CMP_FOLLOW), ac, (svNo1 >= 0 ? _stack + svNo1 : NULL), (svNo2 >= 0 ? _stack + svNo2 : NULL))) {
                    //TODO: sonstige Operation, insbesondere vielleicht in Codeblockheader (oder vielleicht doch gar nichts hier?)
                    // (fuer Transpose bei eindimensionalem Arrays hier weiter nichts zu tun, schon durch transp erledigt)
                    pushRes = false;
                    if (transp)
                        sveNo = -1;
                }
                break;
		}

        if (sveNo >= 0)
            stackPopTo(_stack + sveNo);
        if (pushRes)
            pushOpResult(cd->se);

        if (transp)
            stackCur()->_transpose = true;

		return cd + (cd->v.c.cnt + 1);
	}


	/**
	 * executes intermediary code command for execution of a value construction
	 * @param cd		intermediary code command
	 * @return			next intermediary code position / NULL if you have to leave this execution context after the command
	 */
	IntCode::IcElem* ExecContext::execCodeConstruct(IntCode::IcElem *cd)
	{
		StackValue *sv1, *sv2, *sv3;
		StackValue *sve = NULL;
		unsigned long cnt = (cd->v.c.cnt ? (unsigned long)(cd[1].v.i) : cd->v.c.par);
		bool pushRes = true;

		switch (cd->v.c.minor) {
			case ICS_CONSTRUCT_ARRAY:
            case ICS_CONSTRUCT_ARRAY_IND:
            case ICS_CONSTRUCT_ARRAY_WOI:
                if (cnt == 0) {
					_opRes.set(TP_NULL);
                }
                if (cnt == 1 && (cd->v.c.minor != ICS_CONSTRUCT_ARRAY_IND || stackCur()->val().t == TP_BLANK)) {
                    if (stackCur()->val().t != TP_BLANK) {
                        pushRes = false;
                    }
                    else {
                        sve = stackCur();
                        _opRes.set(TP_NULL);
                    }
                }
                else
                {
                    sve = StackValue::constructList(this, _opRes, cnt, stackCur(), (cd->v.c.minor == ICS_CONSTRUCT_ARRAY_WOI), cd->se);
                }
				break;

            case ICS_CONSTRUCT_TUPLE:
                {
                    // keep index tuple as list on stack if next command is indexation operation, for better performance
                    IntCode::IcElem *nc = cd + cd->v.c.cnt + 1;
                    bool listTuple = (nc->v.c.major == INTCODE_OPERATION && nc->v.c.minor == ICS_OPER_INDEX);
                    sv1 = (cnt == 0 ? NULL : stackCur());
                    sve = TupleUtil::construct(this, _opRes, pushRes, cnt, listTuple, sv1);
                }
				break;

			case ICS_CONSTRUCT_IV_WOLB:
				if (cnt == 0) {
					Interval::construct(this, _opRes, NULL, NULL);
				}
				else {
					sv1 = sve = stackCur();
					Interval::construct(this, _opRes, NULL, sv1);
				}
				break;

			case ICS_CONSTRUCT_IV_LB:
				sv1 = stackCur();
				if (cnt == 1) {
					sve = sv1;
					Interval::construct(this, _opRes, sv1, NULL);
				}
				else {
					sv2 = sve = stackPrev(sv1);
					Interval::construct(this, _opRes, sv2, sv1);
				}
				break;

			case ICS_CONSTRUCT_SET:
				sv1 = stackCur();
				sv2 = stackPrev(sv1);
				sv3 = sve = stackPrev(sv2);
				SetUtil::constructAlg(this, _opRes, sv3, sv2, sv1);
				break;
		}

		if (sve)
			stackPopTo(sve);
		if (pushRes)
			pushOpResult(cd->se);

		return cd + (cd->v.c.cnt + 1);
	}


    /**
     * executes intermediary code command for name prefix for naming of rows and cols in the result matrix
     * @param cd		intermediary code command
     * @return			next intermediary code position / NULL if you have to leave this execution context after the command
     */
    IntCode::IcElem* ExecContext::execCodeLineNamePref(IntCode::IcElem *cd)
    {
        switch (cd->v.c.minor) {
            case ICS_LNP_EXTEND:
                {
                    bool cmd;
                    intType curpref = curNmPref(cmd);
                    if (cmd) {
                        internalError("multiple setting of lineNamePref for current statement");
                    }
                    else {
                        intType addpref = (cd->v.c.cnt == 0 ? (intType) cd->v.c.par : cd[1].v.i);
                        if (curpref >= 0) {
                            string s(_modp->data()->globStrings()->at(curpref));
                            if (_modp->nmPrefSep(false))
                                s.append(_modp->data()->globStrings()->at(_modp->nmPrefSep(false)));
                            s.append(_modp->data()->globStrings()->at(addpref));
                            _nmPrefCmd = (intType) _modp->data()->globStrings()->storeInd(s);
                        }
                        else {
                            _nmPrefCmd = addpref;
                        }
                    }
                }
                break;

            case ICS_LNP_RESET:
                _nmPrefCmd = -1;
                break;

            default:
                internalError("invalid lineNamePref subcommand in intermediary code");
        }

        return cd + (cd->v.c.cnt + 1);
    }


    /**
	 * executes intermediary code command for functions and jumps
	 * @param cd		intermediary code command
	 * @return			next intermediary code position / NULL if you have to leave this execution context after the command
	 */
	IntCode::IcElem* ExecContext::execCodeFunction(IntCode::IcElem *cd)
	{
		switch (cd->v.c.minor) {
			case ICS_FUNC_START:
				internalError("unexpected function start in intermediary code");
				break;

			case ICS_FUNC_END:
				if (!_fctContext)
					internalError("unexpected function end in intermediary code");
				return NULL;

			case ICS_FUNC_STORE:
                // construct user function object and push it on the stack
                _opRes.set(TP_FUNCTION, new ValFunctionUser(cd[1].v.n.n1));
                pushOpResult(cd->se);
				return _modp->_code + cd[1].v.n.n2;

			case ICS_FUNC_GOTO:
				return _modp->_code + cd[1].v.n.n1;

			default:
				internalError("invalid function subcommand in intermediary code");
		}

		return cd + (cd->v.c.cnt + 1);
	}


    /**
     * executes intermediary code command for code blocks
     * @param cd		intermediary code command
     * @param cbBody    execution for a single codeblock body
     * @param cbLvl     level of nested codeblocks
     * @param cbCancel  cancelling codeblock at level _cancelCbLvl; skip new codeblock if new one starts
     * @param endCtx    return whether execution in this execution context is ended
     * @return			next intermediary code position / NULL if you have to leave this execution context after the command
     */
    IntCode::IcElem* ExecContext::execCodeCodeblock(IntCode::IcElem *cd, bool cbBody, unsigned& cbLvl, bool& cbCancel, bool &endCtx)
    {
        if (cbBody && !cbLvl && cd->v.c.minor != ICS_CB_BLOCK_START) {
            endCtx = (cd->v.c.minor == ICS_CB_PART_END);
            return cd + (cd->v.c.cnt + 1);
        }

        CodeBlockContext *cb;
        bool toBlockEnd = false, directBlockEnd = false;
        endCtx = false;

        switch (cd->v.c.minor) {
            case ICS_CB_BLOCK_START:
                if (cbCancel) {
                    // dummy result TP_NULL for skipped codeblock; continue execution after codeblock end
                    pushNull(cd->se);
                    cd = codeBase() + cd[1].v.n.n1;
                    break;
                }

                cbLvl++;
                cb = CodeBlockContext::newContextStart(this, cd);
                pushCBContext(cb, cd->se);
                break;

            case ICS_CB_PART_START:
                cb = getCBContext();
                if (cb) {
                    cb->startPart(cd);
                }
                break;

            case ICS_CB_PART_BODY:
                cb = getCBContext();
                if (cb) {
                    cb->execPartBody(cd);
                    return codeBase() + cd[1].v.n.n2;
                }
                break;

            case ICS_CB_PART_END:
                if (cbCancel) {
                    toBlockEnd = true;
                }
                else {
                    cb = getCBContext();
                    if (cb) {
                        toBlockEnd = cb->endPart();
                    }
                    else {
                        if (cd->v.c.par & ICPAR_BLOCK_STMNT) {
                            // result value of code block without result expression is "null"
                            pushNull(cd->se);
                        }

                        uninitCBSymbols(cd[2].v.n.n2, cd[2].v.n.n1, true);

                        // directly proceed with ICS_CB_BLOCK_END
                        directBlockEnd = true;
                    }
                }

                // go to end of code block
                if (!directBlockEnd) {
                    if (!toBlockEnd)
                        break;
                    else
                        return codeBase() + cd[1].v.n.n1;
                }

                // directly proceed with ICS_CB_BLOCK_END
                cd = codeBase() + cd[1].v.n.n1;
#if PROTO
                if (_modp->isProtocol()) {
                    cd->protoOut(_modp, (unsigned)(cd - codeBase()));
                    for (unsigned i = 1; i <= cd->v.c.cnt; i++)
                        cd[i].protoOut(_modp, (unsigned)(cd + i - codeBase()));
                }
#endif
                // *NO* break

            case ICS_CB_BLOCK_END:
                if (cbCancel) {
                    LockGuard<recursive_mutex> lck(_needLock, _cancelMtx);
                    if (_cbContextTop == _cancelCbLvl + 1) {
                        _cancel = false;
                        cbCancel = false;

                        cb = getCBContext();
                        if (!cb)
                            internalError("inconsistency in codeblock cancelling");

                        IntCode::IcElem *repStart = cb->checkStartRepeat();
                        if (repStart)
                            return repStart + (repStart->v.c.cnt + 1);;
                    }
                }

                cbLvl--;
                cb = getCBContext();
                if (cb) {
                    CmplVal& res = cb->cbResult();
                    if (res.t == TP_BLANK)
                        pushNull(cd->se);
                    else
                        pushVal(res, cd->se, true);
                }
                else {
                    // result value of code block without executed part is "null"
                    if (!toBlockEnd && !directBlockEnd)
                        pushNull(cd->se);
                }

                popCBContext(cd->se);
                break;

            default:
                internalError("invalid function subcommand in intermediary code");
        }

        return cd + (cd->v.c.cnt + 1);
    }


    /**
     * executes intermediary code command for code block headers
     * @param cd		intermediary code command
     * @param cbHeader  execution for a single codeblock header
     * @return			next intermediary code position / NULL if you have to leave this execution context after the command
     */
    IntCode::IcElem* ExecContext::execCodeCBHeader(IntCode::IcElem *cd, bool cbHeader)
    {
        CodeBlockContext *cb = getCBContext();
        StackValue *sv;
        CmplVal *v;
        CmplValAuto cbhRes;
        bool b = false;

        switch (cd->v.c.minor) {
            case ICS_CBHEADER_START:
                if (!cbHeader && cb && (cd->v.c.par & ICPAR_CBHP_HCNT) && (((cd->v.c.par & ICPAR_CBHP_PHCBS) || (cb->ordered() && (cd->v.c.par & ICPAR_CBHP_NCSYM))))) {
                    // don't execute header now, but store it in codeblock context
                    cb->storeHeader(cd);
                    cd = codeBase() + cd[1].v.n.n1;
                }
                break;

            case ICS_CBHEADER_END:
                sv = stackCur();
                v = sv->simpleValue();

                if (v && (v->t == TP_CBHEAD_ITER || (v->t == TP_FORMULA && v->valFormula()->isBool()) || (v->t == TP_OPT_VAR && v->optVar()->binVar()) || v->toBool(b, typeConversionAll, _modp))) {
                    bool nextPart = false;

                    if (v->t == TP_CBHEAD_ITER) {
                        CBHeaderIterInfo *cbhIter = (CBHeaderIterInfo *)(v->v.p);
                        v->unset();

                        if (SetBase::empty(cbhIter->iterSet())) {
                            nextPart = true;
                            cbhRes.set(TP_BIN, false);
                            delete cbhIter;
                        }
                        else if (cb && !cb->hasVarCondition()) {
                            if (cbHeader)
                                cbhRes.setP(TP_CBHEAD_ITER, cbhIter);
                            else
                                cb->storeHeader(cbhIter, cd);
                        }
                        else {
                            delete cbhIter;
                            if (!cb)
                                internalError("missing codeblock context for iteration");
                            else
                                valueError("mix of iterations and conditions over optimization variables is not allowed in same codeblock", sv);
                        }
                    }
                    else if (v->t == TP_FORMULA || v->t == TP_OPT_VAR) {
                        if (cbHeader) {
                            cbhRes.copyFrom(v);
                        }
                        else {
                            if (!cb)
                                cb = setCBContext(cd);

                            cb->addCurVarCondition(v, sv->syntaxElem());
                        }
                    }
                    else if (b) {
                        if (cbHeader) {
                            cbhRes.set(TP_BIN, true);
                        }
                        else if (cb && (cd->v.c.par & ICPAR_CBHP_HCNT) == 0) {
                            cb->storeHeader(NULL, cd);
                        }
                    }
                    else {
                        nextPart = true;
                        cbhRes.set(TP_BIN, false);
                    }

                    if (!cbHeader && nextPart) {
                        // go to next part or to end of code block, get address from first argument of this command
                        pop();
                        return codeBase() + cd[1].v.n.n1;
                    }
                }
                else {
                    valueError("codeblock header must be a boolean expression", sv);
                    cbhRes.set(TP_BIN, false);
                }

                pop();
                if (cbHeader) {
                    pushVal(cbhRes, cd->se);
                }
                break;

            default:
                internalError("invalid function subcommand in intermediary code");
        }

        return cd + (cd->v.c.cnt + 1);
    }


    /**
     * executes intermediary code command for code blocks
     * @param cd		intermediary code command
     * @param cbHB      execution for a single codeblock header or body
     * @param cbLvl     level of nested codeblocks
     * @param cbTS      start level of nested codeblocks
     * @return			next intermediary code position / NULL if you have to leave this execution context after the command
     */
    IntCode::IcElem* ExecContext::execCodeCBControl(IntCode::IcElem *cd, bool cbHB, unsigned cbLvl, unsigned cbTS)
    {
        IntCode::IcElem *res = cd + (cd->v.c.cnt + 1);

        unsigned diffLvl = cd->v.c.par & ICPAR_CBC_CNTLVL;
        CodeBlockContext *cbDst = getCBContext(diffLvl);

        bool hasLhs = cd->v.c.par & ICPAR_CBC_HASLHS;
        bool lhsTuple = cd->v.c.par & ICPAR_CBC_LHSTUPLE;
        bool rhsValue = cd->v.c.par & ICPAR_CBC_RHSVALUE;

        if (!cbDst || (diffLvl && !hasLhs) || (!diffLvl && (!cbHB || cbLvl))) {
            internalError("inconsistency in codeblock context objects while executing control command");
            return cd + (cd->v.c.cnt + 1);
        }

        StackValue *svRhs = NULL, *svTpl = NULL, *sve = NULL;
        if (rhsValue) {
            svRhs = sve = stackCur();
            if (lhsTuple)
                svTpl = sve = stackPrev(svRhs);
        }
        else if (lhsTuple) {
            svTpl = sve = stackCur();
        }

        _opRes.set(TP_BLANK);

        switch (cd->v.c.minor) {
            case ICS_CBC_CONTINUE:
                if (diffLvl) {
                    cbDst->controlCommand(this, cd->v.c.minor, _cbContextTop - 1, svTpl, svRhs);
                    res = codeBase() + cd[1].v.n.n1;
                }
                else if (hasLhs) {
                    cbDst->assignResult(svRhs, svTpl);
                }
                else {
                    if (svRhs)
                        _opRes.copyFrom(svRhs->val(), true, false);
                }
                break;

            case ICS_CBC_BREAK:
            case ICS_CBC_REPEAT:
                cbDst->controlCommand(this, cd->v.c.minor, _cbContextTop - 1, svTpl, svRhs);
                res = codeBase() + cd[1].v.n.n2;
                break;

            default:
                internalError("invalid function subcommand in intermediary code");
        }

        if (sve)
            stackPopTo(sve);

        if (cbHB && !cbLvl)
            return NULL;
        else
            return res;
    }



	/**
	 * prepare stack to push a value
	 * @param se			syntax element in the cmpl text creating this stack value
	 * @return				stack element to take up the value to push
	 */
	StackValue* ExecContext::pushPre(unsigned se)
	{
		if (_stackTop == _stackCap) {
			_stackCap = ARR_NEWCAP(_stackCap);
			ARR_REALLOC(_stack, StackValue, _stackTop, _stackCap);
		}

		StackValue *sv = _stack + _stackTop++;
		PROTO_MOD_OUTL(_modp, "pushPre: " << _stackTop << " (" << se << ')');

		sv->init(se);
		return sv;
	}

	/**
	 * pointer to previous element on the stack relative to a given element
	 * @param sv		given base element
	 * @param err		true: if sv is already the first element then error / false: then return NULL
	 * @return			element before sv
	 */
    StackValue* ExecContext::stackPrev(StackValue *sv, bool err)
	{
		PROTO_MOD_OUTL(_modp, "stackPrev: " << (sv - _stack));

		if (sv > _stack) {
			if (!sv->_val.isList() || sv->_val.t == TP_REF_LIST) {
                StackValue *r = sv - 1;
                PROTO_MOD_OUTL(_modp, "  -> " << r->val());
                return r;
			}

            unsigned long cnt = stackListTotalCnt(sv);
            if ((unsigned long)(sv - _stack) >= cnt) {
                StackValue *r = sv - cnt;
                PROTO_MOD_OUTL(_modp, "  -> " << r->val());
                return r;
			}
		}

		if (err)
			internalError("stack underflow");

		return NULL;
	}

	/**
	 * pointer to previous element on the stack relative to a given element (static version, no check for underflow possible)
	 * @param sv		given base element
	 * @return			element before sv
	 */
	StackValue* ExecContext::stackPrevStatic(StackValue *sv)
	{
		if (!sv->_val.isList() || sv->_val.t == TP_REF_LIST)
			return sv - 1;
		else
            return sv - stackListTotalCnt(sv);
	}

    /**
     * get total count of stack values for a list
     * @param sv		list header element
     * @return			total count of elements on the stack for the list
     */
    unsigned long ExecContext::stackListTotalCnt(StackValue *sv)
    {
        unsigned long cnt;
        if (sv->_addVal.t == TP_INT) {
            cnt = sv->_addVal.v.i;
        }
        else {
            if (sv->_val.t == TP_LIST_SIMPLE || sv->_val.t == TP_LIST_SPARSE || sv->_val.t == TP_LIST_TUPLE) {
                cnt = sv->_val.v.i;
            }
            else {
                unsigned long c = 1, sz = sv->_val.v.i;
                StackValue *a = sv;

                cnt = 0;
                while (sz) {
                    a -= c;
                    if (!a->_val.isList() || a->_val.t == TP_REF_LIST)
                        c = 1;
                    else
                        c = stackListTotalCnt(a);

                    cnt += c;
                    sz--;
                }
            }

            sv->_addVal.set(TP_INT, (intType)cnt);
        }

        return cnt + 1;
    }

    /**
	 * discards the value on top of stack
	 */
	void ExecContext::pop()
	{
		if (_stackTop == 0)
			internalError("stack underflow");

		StackValue *sv = _stack + --_stackTop;

		if (sv->_val.isList() && sv->_val.t != TP_REF_LIST) {
			// direct list on stack, discard all values of the list
            unsigned long cnt = stackListTotalCnt(sv) - 1;
            if (_stackTop < cnt)
				internalError("stack underflow");

			_stackTop -= cnt;

			sv->unsetValue();
			for (unsigned long i = 0; i < cnt; i++, sv--)
				sv->unsetValue();
		}
		else {
			sv->unsetValue();
		}

		PROTO_MOD_OUTL(_modp, "pop: " << _stackTop);
	}

	/**
	 * discards value on the stack up to given element
	 * @param sv		given base element
	 * @param incl		discard also element sv
	 */
	void ExecContext::stackPopTo(StackValue *sv, bool incl)
	{
		// discard elements over sv
		StackValue *top = _stack + _stackTop;
        if (sv == top && incl)
            return;
        if (sv >= top)
            internalError("invalid stack popTo");

        for (StackValue *s = sv + 1; s < top; s++)
			s->unsetValue();

		_stackTop = (sv - _stack) + 1;
		PROTO_MOD_OUTL(_modp, "popTo: " << _stackTop);

		// discard element sv
		if (incl)
			pop();
	}

    /**
     * replace a list value on the stack by an array or tuple
     * @param sv		list header
     * @return 			result value
     */
    StackValue *ExecContext::replaceListOnStack(StackValue *sv)
    {
        if (sv->isList()) {
            if (sv != stackCur())
                valueError("internal error: list value must be the topmost stack element to replace it", sv);

            CmplVal a;
            unsigned se = sv->syntaxElem();
            StackValue *lb;

            if (sv->val().t == TP_LIST_TUPLE) {
                TupleUtil::constructFromList(this, a, sv);
                lb = sv - (sv->val().v.i + 1);
            }
            else {
                lb = StackValue::arrayFromList(this, a, sv, false);
            }

            stackPopTo(lb);
            pushVal(a, se, true);
            return stackCur();
        }
        else {
            return sv;
        }
    }


    /**
     * get from stack value (which can be array or list) the first scalar values
     * @param sv            source stack value
     * @param maxCnt        max count of scalar values to fetch
     * @param args          array for returning fetched scalar values
     * @param se            array for returning syntax element numbers of fetched scalar values
     * @return              count of found values (can be greater than maxCnt if more values are found) / 0: found values are not scalar
     */
    unsigned ExecContext::getFirstSimpleValues(StackValue *sv, unsigned maxCnt, CmplVal *args, unsigned *se)
    {
        CmplVal& v = sv->val();
        if (v.isArray()) {
            CmplArrayIterator iter(*(v.array()));
            unsigned i = 0;

            for (iter.begin(); iter; iter++, i++) {
                if (i == maxCnt)
                    break;

                const CmplVal& t = iter.curTuple();
                if ((t.t != TP_INT && t.t != TP_ITUPLE_1INT) || t.v.i != (i + 1) || (*iter)->isArray())
                    return 0;

                args[i].copyFrom(*iter, true, false);
                se[i] = sv->syntaxElem();
            }

            return i;
        }

        else if (v.isList()) {
            StackValue *lst = sv;
            bool rf = (v.t == TP_REF_LIST);

            if (rf) {
                while (lst->val().t == TP_REF_LIST)
                    lst = (StackValue *)(lst->val().v.p);
            }

            unsigned long cnt = lst->val().v.i;
            StackValue *lb;
            if (lst->val().t != TP_LIST_SIMPLE && lst->val().t != TP_LIST_SPARSE) {
                lb = lst - 1;
                for (unsigned long i = 0; i < cnt; i++, lb--) {
                    if (lb->val().isList() || lb->val().isArray())
                        return 0;
                }
            }

            lb = lst - cnt;
            unsigned ic = (cnt < maxCnt ? (unsigned)cnt : maxCnt);

            for (unsigned i = 0; i < ic; i++, lb++) {
                args[i].copyFrom(lb->val(), true, false);
                se[i] = (rf ? sv->syntaxElem() : lb->syntaxElem());
            }

            return (cnt <= maxCnt ? (unsigned)cnt : (maxCnt + 1));
        }

        else {
            if (maxCnt) {
                args[0].copyFrom(v, true, false);
                se[0] = sv->syntaxElem();
            }
            return 1;
        }

        //TODO
        return 0;
    }



    /****** code block handling ****/

    /**
     * push an object to the code block context start
     * @param cb            code block context object or NULL if no context is needed
     * @param se			syntax element in the cmpl text creating this code block context object
     */
    void ExecContext::pushCBContext(CodeBlockContext *cb, unsigned se)
    {
        if (_cbContextTop == _cbContextCap) {
            _cbContextCap = ARR_NEWCAP(_cbContextCap);
            ARR_REALLOC(_cbContext, CBContextNmPref, _cbContextTop, _cbContextCap);
        }

        CBContextNmPref& curCB = _cbContext[_cbContextTop++];
        curCB.cbContext = cb;

        if (_nmPrefCmd >= 0) {
            curCB.nmPref = _nmPrefCmd;
            curCB.nmPrefSet = 1;
            _nmPrefCmd = -1;
        }
        else {
            curCB.nmPref = (_cbContextTop == 1 ? _nmPrefStart.nmPref : _cbContext[_cbContextTop - 2].nmPref);
            curCB.nmPrefSet = 0;
        }

        if (cb)
            cb->setNmPrefStart(curCB.nmPref, curCB.nmPrefSet);

        PROTO_MOD_OUTL(_modp, "pushCBContext: " << _cbContextTop << " (" << se << ')');
    }

    /**
     * replace empty top element on code block context stack by a new code block context object
     * @param cd        intermediary code command (must be INTCODE_CB_HEADER/ICS_CBHEADER_END)
     * @return          code block context object
     */
    CodeBlockContext *ExecContext::setCBContext(IntCode::IcElem *cd)
    {
        if (!_cbContextTop)
            internalError("missing code block context object stack element");

         CBContextNmPref& curCB = _cbContext[_cbContextTop - 1];
        if (!curCB.cbContext)
            curCB.cbContext = CodeBlockContext::newContextAdd(this, cd);

        return curCB.cbContext;
    }

    /**
     * get next upper existing code block context object on the code block stack
     * @param lvl       start position in the code block stack
     * @return          code block context object or NULL if no such context object exists
     */
    CodeBlockContext *ExecContext::getNextUpperCBContext(unsigned lvl)
    {
        unsigned i = lvl + 1;
        if (i >= _cbContextTop)
            return NULL;

        CBContextNmPref *res = _cbContext + i;
        if (res->cbContext)
            return res->cbContext;

        for (i++, res++; i < _cbContextTop; i++, res++) {
            if (res->cbContext)
                return res->cbContext;
        }

        return NULL;
    }

    /**
     * pop and discard code block context objects
     * @param se			syntax element in the cmpl text creating this code block context object
     * @param cnt           count of code block context objects to discard
     */
    void ExecContext::popCBContext(unsigned se, unsigned cnt)
    {
        while (cnt && _cbContextTop) {
            cnt--;
            _cbContextTop--;

            if (_cbContext[_cbContextTop].cbContext) {
                delete _cbContext[_cbContextTop].cbContext;
            }

            PROTO_MOD_OUTL(_modp, "popCBContext: " << (_cbContextTop+1) << " (" << se << ')');
        }

        //TODO
    }


    /**
     * set value for codeblock symbol
     * @param n         number of codeblock symbol within array _localSymbols
     * @param va        value as CmplVal
     * @param vb        value as StackValue (only used if no <code>va</code>)
     */
    void ExecContext::setCBSymbolVal(unsigned n, const CmplVal *va, StackValue *vb)
    {
        SymbolValue *sym = _localSymbols + n;

        if (!sym->isUninitialized())
            internalError("double initialization for codeblock symbol");

        if (va) {
            sym->setSimpleConstVal(*va);
        }
        else if (vb) {
            CmplVal *vs = vb->simpleValue();
            if (vs) {
                sym->setSimpleConstVal(*vs);
            }
            else {
                ValueStore *store = sym->valueStore(true);
                store->setValue(this, vb, true);
                sym->setReadOnly();
            }
        }
    }

    /**
     * uninitialize local symbols
     * @param n         number of first symbol within array _localSymbols
     * @param to        max number of symbol + 1
     * @param delId		unset also SymbolValue::_defId
     */
    void ExecContext::uninitCBSymbols(unsigned n, unsigned to, bool delId)
    {
        for (SymbolValue *sym = _localSymbols + n; n < to; n++, sym++) {
            if (!sym->isUninitialized()) {
                sym->unset(delId, true);
            }
        }
    }


    /**
     * set and propagate cancel flag
     * @param cmd		control command (one of ICS_CBC_BREAK, ICS_CBC_CONTINUE, ICS_CBC_REPEAT)
     * @param dstLvl	nesting codeblock level of destination codeblock of the control command
     */
    void ExecContext::setCancel(int cmd, unsigned dstLvl)
    {
        LockGuard<recursive_mutex> lck(true, _cancelMtx);
        if (!_cancel || dstLvl < _cancelCbLvl || (dstLvl == _cancelCbLvl && _cancelCont && cmd != ICS_CBC_CONTINUE)) {
            _cancel = true;
            _cancelCont = (cmd == ICS_CBC_CONTINUE);
            _cancelCbLvl = dstLvl;

            for (unsigned n = max(dstLvl + 1, _cbContextCreateTop); n < _cbContextTop; n++) {
                if (_cbContext[n].cbContext)
                    _cbContext[n].cbContext->setCancel(cmd, dstLvl);
            }
        }
    }


    /**
     * set top part of current iteration tuple
     * @param iter      iteration to which the tuple part belongs
     * @param val       part of tuple (must be scalar index value or index tuple)
     */
    void ExecContext::setIterTplPart(CodeBlockIteration *iter, const CmplVal& val)
    {
        if (!_topIterVal.empty()) {
            if (_iterValsTop == _iterValsCap) {
                _iterValsCap = ARR_NEWCAP(_iterValsCap);
                ARR_REALLOC(_iterVals, IterTuplePart, _iterValsTop, _iterValsCap);
                for (unsigned i = _iterValsTop; i < _iterValsCap; i++)
                    _iterVals[i].init();
            }

            IterTuplePart& tp = _iterVals[_iterValsTop++];
            tp.tplPart.moveFrom(_topIterVal.tplPart);
            tp.iter = _topIterVal.iter;
        }

        _topIterVal.tplPart.copyFrom(val);
        _topIterVal.iter = iter;
    }

    /**
     * unset top part of current iteration tuple
     * @param iter      iteration to which the tuple part belongs
     */
    void ExecContext::delIterTplPart(CodeBlockIteration *iter)
    {
        bool found = false;
        if (!_topIterVal.empty()) {
            found = (_topIterVal.iter == iter);
            _topIterVal.dispose();
        }

        while (!found && _iterValsTop > 0) {
            IterTuplePart& tp = _iterVals[--_iterValsTop];
            found = (tp.iter == iter);
            tp.dispose();
        }
    }

    /**
     * get all parts of current iteration tuple
     * @param parts     vector for return of the parts
     * @return          count of found tuple parts
     */
    unsigned ExecContext::getIterTplParts(vector<CmplValAuto>& parts)
    {
        unsigned res = (_parent ? _parent->getIterTplParts(parts) : 0);
        return res + getIterTplPartsThis(parts, 0);
    }

    /**
     * get parts of current iteration tuple, from start of given iteration
     * @param parts     vector for return of the parts
     * @param start     iteration
     * @return          count of found tuple parts
     */
    unsigned ExecContext::getIterTplParts(vector<CmplValAuto>& parts, CodeBlockIteration *start)
    {
        unsigned res = 0;

        if (!_topIterVal.empty() && _topIterVal.iter == start) {
            res = getIterTplPartsThis(parts, _iterValsTop);
        }

        else if (_iterValsTop) {
            int i;
            for (i = _iterValsTop - 1; i >= 0; i--) {
                IterTuplePart *tp = _iterVals + i;
                if (!tp->empty() && tp->iter == start)
                    break;
            }

            if (i >= 0)
                res = getIterTplPartsThis(parts, i);
        }

        if (!res) {
            if (_parent) {
                res = _parent->getIterTplParts(parts, start);
                if (res)
                    res += getIterTplPartsThis(parts, 0);
            }
        }

        return res;
    }

    /**
     * get all parts of current iteration tuple which are stored in this execution context
     * @param parts     vector for return of the parts
     * @param start     start index within _iterVals
     * @return          count of found tuple parts
     */
    unsigned ExecContext::getIterTplPartsThis(vector<CmplValAuto>& parts, unsigned start)
    {
        unsigned res = 0;

        if (start < _iterValsTop) {
            IterTuplePart *tp = _iterVals + start;
            for (unsigned i = start; i < _iterValsTop; i++, tp++) {
                if (!tp->empty()) {
                    parts.emplace_back();
                    parts.back().copyFrom(tp->tplPart);
                    res++;
                }
            }
        }

        if (!_topIterVal.empty()) {
            parts.emplace_back();
            parts.back().copyFrom(_topIterVal.tplPart);
            res++;
        }

        return res;
    }

    /**
     * get parts of current iteration tuple, only from start of the topmost iteration
     * @param parts     vector for return of the parts
     * @return          count of found tuple parts
     */
    unsigned ExecContext::getIterTplPartsIter(vector<CmplValAuto>& parts)
    {
        int i;
        for (i = _cbContextTop - 1; i >= 0; i--) {
            CodeBlockContext *cb = _cbContext[i].cbContext;
            if (cb && cb->_startIter) {
                return (cb->_startIterObj ? getIterTplParts(parts, cb->_startIterObj) : 0);
            }
        }

        return 0;
    }

    /**
     * get parts of current iteration tuple, only from start of the topmost change of name prefix with $curDestName
     * @param parts     vector for return of the parts
     * @return          count of found tuple parts
     */
    unsigned ExecContext::getIterTplPartsNmPref(vector<CmplValAuto>& parts)
    {
        bool found = false;
        CodeBlockContext *cbStart = NULL;
        ExecContext *ctx = this;

        while (!found && ctx) {
            for (int i = ctx->_cbContextTop - 1; i >= 0; i--) {
                CBContextNmPref& curCB = ctx->_cbContext[i];
                if (curCB.nmPrefSet == 2) {
                    found = true;
                    break;
                }

                CodeBlockContext *cb = curCB.cbContext;
                if (cb && cb->_startIter && cb->_startIterObj)
                    cbStart = cb;
            }

            if (!found)
                ctx = ctx->_parent;
        }

        if (found)
            return (cbStart && cbStart->_startIterObj ? getIterTplParts(parts, cbStart->_startIterObj) : 0);
        else
            return getIterTplParts(parts);
    }


    /**
     * start mapping for codeblock part with conditions over optimization variables
     * @param cb        codeblock context
     * @param cbp       number of current codeblock part
     * @param varCond   condition formula ver optimization variables (must be TP_FORMULA, TP_OPT_VAR or TP_BIN with value true)
     * @param lsf       number of first symbol within array _localSymbols that doesn't need mapping
     * @param init      init new mapping object (if false then there must be a current mapping object matching the codeblock)
     */
    void ExecContext::varCondMapStartPart(CodeBlockContext *cb, int cbp, CmplVal& varCond, unsigned lsf, bool init)
    {
        VarCondMapping *vcm;
        if (init) {
            vcm = new VarCondMapping(this, cb, lsf);
            _curVarCondMap = vcm;
        }
        else {
            vcm = VarCondMapping::check(this, cb, -1);
        }

        vcm->startPart(cbp, varCond);
    }

    /**
     * end mapping for codeblock part with conditions over optimization variables
     * @param cb        codeblock context (must match the current mapping object)
     * @param cbp       number of current codeblock part (must match the current mapping object)
     */
    void ExecContext::varCondMapEndPart(CodeBlockContext *cb, int cbp)
    {
        VarCondMapping *vcm = VarCondMapping::check(this, cb, cbp);
        vcm->endPart();
    }

    /**
     * set codeblock result using mapping for codeblock part with conditions over optimization variables
     * @param cb        codeblock context (must match the current mapping object)
     * @param cbp       number of current codeblock part (must match the current mapping object)
     * @param val       codeblock result value of current codeblock part
     * @param ovr       override all existing results
     * @param se		syntax element id of source value
     */
    void ExecContext::varCondMapSetCBResult(CodeBlockContext *cb, int cbp, CmplVal& val, bool ovr, unsigned se)
    {
        VarCondMapping *vcm = VarCondMapping::check(this, cb, cbp);
        vcm->setMappedCBResult(val, ovr, se);
    }

    /**
     * merge mapped values for conditions over optimization variables at the end of codeblock, and remove mapping object
     * @param cb        codeblock context (must match the current mapping object)
     * @param res       return of merged codeblock result
     */
    void ExecContext::varCondMapMerge(CodeBlockContext *cb, CmplVal& res)
    {
        VarCondMapping *vcm = VarCondMapping::check(this, cb, -1);
        vcm->mergeAll(res);

        _curVarCondMap = vcm->_parent;
        delete vcm;

        //TODO anderswo: varCondMapMerge() muss bei jeder regulaeren Beendigung des Codeblocks gerufen werden; abraeumen des MappingInfos auch bei jeder irregulaeren Beendigung
        //                  insbesondere auch Beendigung ueber Codeblock-Control-Kommandos beruecksichtigen
    }


    /**
     * check if mapping is needed and given, and get the mapped value store
     * @param sym       mapping for the value store of this symbol
     * @param nw        if mapping is needed but doesn't exist, then create a new mapped value store
     * @param nocond    nocond modificator is given for assignment
     * @param ncerr     return whether error of mixing conditional and non-conditional assignments
     * @return          mapping info for the value store / NULL: no mapping
     */
    VarCondMapVS *ExecContext::checkGetMappedVS(SymbolValue *sym, bool nw, bool nocond, bool& ncerr)
    {
        ncerr = false;
        if (!_curVarCondMap)
            return NULL;

        int locSymNo = (!sym->global() ? (sym - _localSymbols) : -1);
        if (locSymNo >= 0 && (unsigned)locSymNo >= _curVarCondMap->_unmappedLSFrom)
            return NULL;

        bool needLockSym = (_needLock && (locSymNo < 0 || (unsigned)locSymNo < _localSymbolCreateTo));
        LockGuard<mutex> lckS(needLockSym, sym->accMtx());

        ValueStore *vs = sym->valueStore();
        if (!vs) {
            if ((nw || nocond) && sym->isUninitialized())
                vs = sym->valueStore(true);
            else
                return NULL;
        }

        return _curVarCondMap->getMappedVSRec(0, this, locSymNo, vs, nw, nocond, ncerr);
    }


    /****** assignment ****/

    /**
     * get right hand side value for the current assignment
     * @param se		id of syntax element for use as default
     * @return 			right hand side value on the stack
     */
    StackValue *ExecContext::getCurAssignRhs(unsigned se)
    {
        if (_assNextRhs) {
            if (_assStartVolaRhs) {
                //TODO:
                //	Code ab _assStartVolaRhs erneut ausfuehren bis zur aktuellen Codeposition (moeglichst direkt in diesem ExecContext)
                //	dadurch wird RHS neu oben auf dem Stack hinterlassen, darauf _assRhsNo setzen
                stackPopTo(assRhs(), false);	//nur vorlaeufig
                valueError("volatile right hand side value not implemented", assRhs());
                //stackPopTo(assRhs(), true);
                //run(_assStartVolaRhs, _curCommand);
                //_assRhsNo = _stackTop - 1;
            }
            else {
                if (_assRhsNo >= 0)
                    stackPopTo(assRhs(), false);
                else
                    pop();
            }
        }
        else if (_assRhsNo < 0) {
            // push default value to stack
            if (_assObjType == VAL_OBJECT_TYPE_CON || _assObjType == VAL_OBJECT_TYPE_OBJ) {
                valueError("no constraint or objective given within 'con' or 'obj'", stackCur());
                pushEmpty(se);
                _assRhsNo = _stackTop - 1;
            }
            else if (_assObjType == VAL_OBJECT_TYPE_VAR) {
                if (_assDataType) {
                    pushPre(se);
                    _assRhsNo = _stackTop - 1;
                    assRhs()->val().set(TP_DATA_TYPE, _assDataType);
                    _assDataType = NULL;
                }
            }
            else {
                pushPre(se);
                _assRhsNo = _stackTop - 1;
                if (_assDataType && _assDataType->baseType() != TP_CONTAINER) {
                    _assDataType->defValCopy(assRhs()->val());
                    _assDataType = NULL;
                }
            }
        }

        if (_assRhsNo >= 0 && assRhs()->isList()) {
            replaceListOnStack(assRhs());
            _assRhsNo = _stackTop - 1;
        }

        StackValue *res = assRhs();
        bool volRes = false;

        // convert to object type
        if (_assObjType != -1) {
            res = pushPre(_assRhsNo >= 0 ? assRhs()->syntaxElem() : se);
            ObjectTypeUtil::convertTo(this, res->val(), _assObjType, assRhs(), _assSyntaxElem, res->syntaxElem(), _assOrdered);

            if (_assObjType != VAL_OBJECT_TYPE_PAR)
                volRes = true;
        }

        // convert to data type
        if (_assDataType) {
            unsigned svNo = res - _stack;
            res = pushPre(res->syntaxElem());
            _assDataType->convertTo(this, res->val(), _stack + svNo);
        }

        if (_assObjType == VAL_OBJECT_TYPE_VAR) {
            res->fixDataTypeVar();
        }

        if (volRes || _assStartVolaRhs)
            _assNextRhs = true;

        return res;
    }


    /****** utility functions ****/

    /**
     * check if source value is a container with a defined special convert function, and call this function
     * @param dst       store for result value of the called function
     * @param src       source value
     * @param tp        type of convert
     * @return          true if special convert function is found and called
     */
    bool ExecContext::checkContainerConvSpecial(CmplVal& dst, const CmplVal& src, tp_e tp)
    {
        if (src.t != TP_CONTAINER)
            return false;

        // get symbol number of searched element
        unsigned symIdSub;
        switch (tp) {
            case TP_OPT_VAR: symIdSub = modp()->symIdSubAsVar(); break;
            case TP_OPT_CON: symIdSub = modp()->symIdSubAsCon(); break;
            case TP_OPT_OBJ: symIdSub = modp()->symIdSubAsObj(); break;
            case TP_STR:     symIdSub = modp()->symIdSubAsString(); break;
            default:
                return false;
        }

        // check for function
        ValContainer *cont = src.valContainer();
        SymbolValue *sy = cont->getSimple(symIdSub, (_curCommand ? _curCommand - codeBase() : NULL));

        if (!sy)
            return false;

        CmplVal *vf = sy->simpleValue();
        if (!vf || vf->t != TP_FUNCTION)
            return false;

        // call function, with empty argument value, and with the container as $this
        CmplValAuto tmp;
        tmp.moveFrom(_opRes);

        StackValue arg;
        arg.init(_curCommand ? _curCommand->se : 0);
        arg.val().set(TP_NULL);

        ValContainer *pt = setCallThis(cont);
        vf->valFunction()->operCall(this, &arg);
        setCallThis(pt);

        dst.moveFrom(_opRes);
        _opRes.moveFrom(tmp);

        return true;
    }


	/****** error handling ****/

	/**
	 * internal error
	 * @param msg		error message
	 */		
	void ExecContext::internalError(const char *msg)
	{
		ostringstream ostr;
		ostr << msg << ", at code pos ";

#if PROTO
		_curCommand->protoOut(_modp, (unsigned)(_curCommand - _modp->_code), &ostr);
#else
		ostr << (unsigned)(_curCommand - _modp->_code);
#endif

		_modp->ctrl()->errHandler().internalError(ostr.str().c_str());
	}

    /**
     * other execution error
     * @param msg		error message
     * @param se		syntax element id of source value
     * @param level		one of ERROR_LVL_*
     */
    void ExecContext::execError(const char *msg, unsigned se, int level)
    {
        const SyntaxElement *sep = _modp->syntaxElement(se);

        ostringstream ostr;
        ostr << msg;

        const char *stxt = sep->text();
        if (stxt)
            ostr << ", at '" << stxt << "'";

#if PROTO
        if (_modp->isProtocol()) {
            *(_modp->protoStream()) << "error: " << msg << ", at code pos ";
            _curCommand->protoOut(_modp, (unsigned)(_curCommand - _modp->_code));
        }
#endif

        _modp->ctrl()->errHandler().error(level, ostr.str().c_str(), sep->loc());
    }

    /**
     * value is not suited for the current intermediary code command
     * @param msg		error message
     * @param val		value
     * @param se		syntax element id of source value
     * @param level		one of ERROR_LVL_*
     */
    void ExecContext::valueError(const char *msg, CmplVal& val, unsigned se, int level)
	{
        const SyntaxElement *sep = _modp->syntaxElement(se);

		ostringstream ostr;
		ostr << msg << " '";
        val.write(ostr, _modp);		//TODO: spezielle Werte anders ausgeben (z.B. SymbolValue)
		ostr << "'";

        const char *stxt = sep->text();
		if (stxt)
			ostr << ", at '" << stxt << "'";

#if PROTO
		if (_modp->isProtocol()) {
			*(_modp->protoStream()) << "error: " << msg << ", at code pos ";
			_curCommand->protoOut(_modp, (unsigned)(_curCommand - _modp->_code));
		}
#endif

        _modp->ctrl()->errHandler().error(level, ostr.str().c_str(), sep->loc());
	}

    /**
     * @param tp		type of failing assertion
     * @param ase		syntax element id of assert command
     * @param vse       syntax element id of failing value / 0: in assert command itself
     * @param level		one of ERROR_LVL_*
     */
    void ExecContext::assertionError(const char *tp, unsigned ase, unsigned vse, int level)
    {
        const char *msg1 = _modp->ctrl()->printBuffer("assertion about %s failed", tp);
        const SyntaxElement *asep = _modp->syntaxElement(ase);

        if (vse) {
            const SyntaxElement *vsep = _modp->syntaxElement(vse);
            _modp->ctrl()->errHandler().error(level, msg1, vsep->loc(), "for given assertion", asep->loc());
        }
        else {
            _modp->ctrl()->errHandler().error(level, msg1, asep->loc());
        }
    }



    /*********** VarCondMapping **********/

    /**
     * constructor
     * @param ctx       execution context
     * @param cb        codeblock context of the codeblock this mapping is created for
     * @param lsf       number of first symbol within the execution context that doesn't need mapping
     */
    VarCondMapping::VarCondMapping(ExecContext *ctx, CodeBlockContext *cb, unsigned lsf)
    {
        _execContext = ctx;
        _parent = ctx->curVarCondMap();

        _cbContext = cb;
        _unmappedLSFrom = lsf;
        _partCnt = cb->partCnt();
        _curCBPart = -1;

        _partVarCond.resize(_partCnt);
        for (unsigned i = 0; i < _partCnt; i++)
            _partVarCond[i].unset();

        _prvPartConds.resize(_partCnt + 1);
        for (unsigned i = 0; i <= _partCnt; i++)
            _prvPartConds[i] = NULL;

        _trueCondPart = -1;
        _mapCBRes = NULL;
    }

    /**
     * destructor
     */
    VarCondMapping::~VarCondMapping()
    {
        for (unsigned i = 0; i <= _partCnt; i++) {
            if (_prvPartConds[i])
                delete _prvPartConds[i];
        }

        if (_mapCBRes)
            delete _mapCBRes;

        for (auto ma : _mapVS) {
            VarCondMapVS *cma = ma.second;
            if (cma)
                delete cma;
        }
    }

    /**
     * check and get current mapping object within the execution context
     * @param ctx       execution context
     * @param cb        codeblock context of the codeblock this mapping is created for
     * @param cbp       number of current codeblock part / -1: no one
     * @return          pointer to current mapping object within the execution context
     */
    VarCondMapping *VarCondMapping::check(ExecContext *ctx, CodeBlockContext *cb, int cbp)
    {
        VarCondMapping *vcm = ctx->curVarCondMap();

        if (!vcm)
            ctx->internalError("missing mapping info for execution within a codeblock part with conditions over optimization variables");

        if (vcm->_execContext != ctx || vcm->_cbContext != cb || vcm->_curCBPart != cbp)
            ctx->internalError("inconsistent mapping info for execution within a codeblock part with conditions over optimization variables");

        return vcm;
    }


    /**
     * start of execution for one codeblock part
     * @param cbp       number of current codeblock part
     * @param varCond   condition formula ver optimization variables (must be TP_FORMULA, TP_OPT_VAR or TP_BIN with value true)
     */
    void VarCondMapping::startPart(int cbp, CmplVal& varCond)
    {
        if (_trueCondPart != -1 || cbp < 0 || (unsigned)cbp >= _partVarCond.size() || _partVarCond[cbp].t != TP_EMPTY)
            _execContext->internalError("inconsistent start of codeblock part in mapping info for execution within a codeblock part with conditions over optimization variables");

        _curCBPart = cbp;
        if (varCond.t == TP_OPT_VAR) {
            OptVar *ov = varCond.optVar();
            _partVarCond[cbp].dispSet(TP_FORMULA, new ValFormulaVarOp(ov->syntaxElem(), ov));
        }
        else {
            _partVarCond[cbp].copyFrom(varCond);
        }

        if (varCond.t == TP_BIN)
            _trueCondPart = cbp;

        int pbp = cbp - 1;
        while (pbp >= 0 && !_partVarCond[pbp])
            pbp--;

        if (pbp >= 0) {
            _prvPartConds[cbp] = new vector<CmplValAuto>(*(_prvPartConds[pbp]));
            _prvPartConds[cbp]->emplace_back(_partVarCond[pbp]);
        }
        else {
            _prvPartConds[cbp] = new vector<CmplValAuto>();
        }
    }

    /**
     * end of execution for one codeblock part
     */
    void VarCondMapping::endPart()
    {
        _curCBPart = -1;
        _curFullCond.dispUnset();
    }


    /**
     * get mapping info for an value store
     * @param lvl       recursion level
     * @param ctxOrg    original execution context
     * @param locSymNo  symbol no of local symbol within ctxOrg / -1: no local symbol
     * @param srcVS     source value store of mapping
     * @param nw        if mapping doesn't exist, then insert new mapping
     * @param nocond    nocond modificator is given for assignment
     * @param ncerr     return whether error of mixing conditional and non-conditional assignments
     * @return          mapping info for the value store / NULL: no mapping
     */
    VarCondMapVS *VarCondMapping::getMappedVSRec(unsigned lvl, ExecContext *ctxOrg, int locSymNo, ValueStore *srcVS, bool nw, bool nocond, bool& ncerr)
    {
        //TODO: Sperren so ausreichend und richtig?
        //  (Sperre fuer Symbol schon durch Aufrufer gesetzt; Sperre fuer ValueStore hier gesetzt, bevor daraus kopiert wird)

        if (nocond) {
            if (!_nocond.count(srcVS)) {
                _nocond.insert(srcVS);
                ncerr = _mapVS.count(srcVS);

                if (!ncerr && _parent && (locSymNo < 0 || ((!_execContext->_fctContext || _execContext == _parent->_execContext) && (unsigned)locSymNo < _parent->_unmappedLSFrom)))
                    _parent->getMappedVSRec(lvl+1, ctxOrg, locSymNo, srcVS, nw, nocond, ncerr);
            }

            return NULL;
        }
        else {
            ncerr = _nocond.count(srcVS);
            if (ncerr)
                return NULL;
        }

        VarCondMapVS *map = (_mapVS.count(srcVS) ? _mapVS[srcVS] : NULL);
        if (map && map->getDstVS())
            return map;

        VarCondMapVS *parmap = NULL;
        if (_parent && (locSymNo < 0 || ((!_execContext->_fctContext || _execContext == _parent->_execContext) && (unsigned)locSymNo < _parent->_unmappedLSFrom)))
            parmap = _parent->getMappedVSRec(lvl+1, ctxOrg, locSymNo, srcVS, nw, nocond, ncerr);

        if (!nw || _curCBPart < 0)
            return parmap;

        if (!map) {
            ValueStore *vs = (parmap ? parmap->getDstVS() : srcVS);
            LockGuard<mutex> lckVS(_execContext->needLock(), vs->accMtx());
            _mapVS[srcVS] = map = new VarCondMapVS(this, vs, parmap);
        }

        map->setCopyVS();
        return map;
    }

    /**
     * set codeblock result
     * @param val       codeblock result value of current codeblock part
     * @param ovr       override all existing results
     * @param se		syntax element id of source value
     */
    void VarCondMapping::setMappedCBResult(CmplVal& val, bool ovr, unsigned se)
    {
        if (!_mapCBRes)
            _mapCBRes = new vector<CmplValAuto>(_partCnt);

        CmplValAuto& v = (*_mapCBRes)[_curCBPart];

        if (val.t != TP_ARRAY && (v.t == TP_EMPTY || v.t == TP_BLANK || (ovr && v.t != TP_VALUESTORE))) {
            v.copyFrom(val, true, true);
        }
        else {
            if (v.t != TP_VALUESTORE) {
                CmplVal cbn(TP_VALUESTORE, new ValueStore(v));
                v.moveFrom(cbn, true);
            }

            v.valueStore()->setValue(_execContext, NULL, ovr, NULL, &val, se);
        }
    }


    /**
     * merge symbol values and codeblock result
     * @param res       return of merged codeblock result
     */
    void VarCondMapping::mergeAll(CmplVal& res)
    {
        if (_trueCondPart < 0) {
            int pbp = _partCnt - 1;
            while (pbp >= 0 && !_partVarCond[pbp])
                pbp--;

            if (pbp >= 0) {
                _prvPartConds[_partCnt] = new vector<CmplValAuto>(*(_prvPartConds[pbp]));
                _prvPartConds[_partCnt]->emplace_back(_partVarCond[pbp]);
            }
            else {
                _prvPartConds[_partCnt] = new vector<CmplValAuto>();
            }
        }

        // merge symbol values
        for (auto kvp : _mapVS) {
            CmplValAuto arr;
            VarCondMapVS *map = kvp.second;
            ValueStore *dst = map->_srcVS.valueStore();

            LockGuard<mutex> lckVS(_execContext->needLock(), dst->accMtx());
            if (mergeArray(arr, (map->chgInfo()->fullChg() ? NULL : dst), map->_dstMapVS, map->chgInfo()))
                dst->setValue(_execContext, NULL, true, NULL, &arr, _cbContext->syntaxElem());
        }

        // merge codeblock result
        if (_mapCBRes) {
            if (!mergeArray(res, NULL, *_mapCBRes, NULL))
                res.set(TP_NULL);
        }

        //TODO: Behandlung Elemente fuer Ergebnismatrix
    }

    /**
     * merge all values of an array
     * @param res       return of merged result (array of values or scalar value) or value store to set result values in
     * @param dstVS     value store to set result values in (alternative to res) / NULL: no such alternative
     * @param map       values or value store per codeblock part
     * @param chg       original values and change info / NULL: no original values
     * @return          true if result is returned in res
     */
    bool VarCondMapping::mergeArray(CmplVal& res, ValueStore *dstVS, vector<CmplValAuto>& map, VSChangeInfo *chg)
    {
        vector<CmplValAuto> pvals((_trueCondPart < 0 ? _partCnt + 1 : _partCnt));
        unsigned ptc = (_trueCondPart < 0 ? _partCnt : _trueCondPart);

        unsigned pfrom = 0, pend = 0;
        for (unsigned p = 0; p < _partVarCond.size(); p++) {
            if (_partVarCond[p]) {
                if (!pend)
                    pfrom = p;
                pend = p + 1;
            }
        }

        if (!pend)
            return false;

        if (!chg && _trueCondPart < 0)
            pvals[_partCnt].set(TP_NULL);

        unsigned pend2 = pend;
        if (_trueCondPart > 0) {
            pend2 = _trueCondPart - 1;
            while (pend2 > pfrom && !_partVarCond[pend2])
                pend2--;

            pend2++;
        }

        // check if definition set of destination array is directly given
        CmplValAuto dds(TP_EMPTY);
        CmplVal ntpls(TP_SET_NULL);
        CmplVal vnull(TP_NULL);

        if (!chg || chg->fullChg()) {
            // definition set of destination array is directly given if all source definition sets are equal
            CmplVal& vp = map[pfrom];
            dds.copyFrom(vp.t == TP_VALUESTORE ? vp.valueStore()->values()->defset() : (vp.t == TP_ARRAY ? vp.array()->defset() : ntpls));

            for (unsigned p = pfrom + 1; p < pend; p++) {
                CmplVal& vp2 = map[p];
                if (vp2) {
                    CmplVal& ds2 = (vp2.t == TP_VALUESTORE ? vp2.valueStore()->values()->defset() : (vp2.t == TP_ARRAY ? vp2.array()->defset() : ntpls));
                    if (ds2 != dds) {
                        dds.dispUnset();
                        break;
                    }
                }
            }

            if (dds && _trueCondPart < 0 && chg && chg->baseVS()->values()) {
                CmplVal& ds2 = chg->baseVS()->values()->defset();
                if (ds2 != dds)
                    dds.dispUnset();
            }
        }
        else if (!chg->defsetChg()) {
            // definition set of destination array is directly given if all elements in the source array are changed
            bool allChg;
            bool hasChg = chg->hasChgElem(allChg);

            if (!hasChg)
                return false;

            if (allChg)
                dds.copyFrom(chg->baseVS()->values()->defset());
        }

        // execute merging
        if (dds) {
            if (dds.t == TP_SET_NULL) {
                for (unsigned p = pfrom; p < pend; p++) {
                    CmplVal& vp = map[p];
                    if (vp)
                        pvals[p].copyFrom((vp.t == TP_VALUESTORE ? vp.valueStore()->values()->at(0) : (vp.t == TP_ARRAY ? vp.array()->at(0) : &vp)));
                }

                if (chg && _trueCondPart < 0)
                    pvals[_partCnt].copyFrom((chg->baseVS()->values() ? chg->baseVS()->values()->at(0) : &vnull));

                mergeVal(res, pvals, pfrom, pend2, ptc);
            }

            else {
                CmplArray *arr = new CmplArray(dds);
                res.set(TP_ARRAY, arr);

                unsigned long sz = arr->size();
                for (unsigned long i = 0; i < sz; i++) {
                    for (unsigned p = pfrom; p < pend; p++) {
                        CmplVal& vp = map[p];
                        if (vp)
                            pvals[p].copyFrom((vp.t == TP_VALUESTORE ? vp.valueStore()->values()->at(i) : (vp.t == TP_ARRAY ? vp.array()->at(i) : &vnull)));
                    }

                    if (chg && _trueCondPart < 0)
                        pvals[_partCnt].copyFrom((chg->baseVS()->values() ? chg->baseVS()->values()->at(i) : &vnull));

                    mergeVal(*(arr->at(i)), pvals, pfrom, pend2, ptc);
                }
            }

            return true;
        }

        else {
            ValueStore *dt = NULL;
            CmplValAuto dtv;

            if (dstVS) {
                dt = dstVS;
            }
            else {
                dt = new ValueStore();
                dtv.set(TP_VALUESTORE, dt);
            }

            CmplVal *iset;
            bool useChgFlags, useInd;
            bool useSrcArr = (chg && chg->baseVS()->values() && (!chg->fullChg() || _trueCondPart < 0));
            unsigned long n;

            if (useSrcArr) {
                iset = &(chg->baseVS()->values()->defset());
                useChgFlags = !chg->fullChg();
                useInd = (!chg->fullChg() && !chg->defsetChg());
            }
            else {
                CmplVal& vp2 = map[pfrom];
                iset = (vp2.t == TP_VALUESTORE ? &(vp2.valueStore()->values()->defset()) : (vp2.t == TP_ARRAY ? &(vp2.array()->defset()) : &ntpls));
                useChgFlags = false;
                useInd = false;
            }

            // use tuples from all part sets for the result, beginning with source array or first part
            bool fset = true;
            unsigned pnxt = (useSrcArr ? pfrom : (pfrom + 1));
            while (iset) {
                SetIterator iter(*iset);
                for (iter.begin(); iter; iter++) {
                    // get value from every part for the tuple
                    unsigned long i = iter.curIndex();
                    const CmplVal& curtp = iter.curTuple();

                    if (fset) {
                        if (useChgFlags && !chg->chgInd(i))
                            continue;

                        for (unsigned p = pfrom; p < pend; p++) {
                            CmplVal& vp = map[p];
                            if (vp) {
                                CmplArray *a = (vp.t == TP_VALUESTORE ? vp.valueStore()->values() : (vp.t == TP_ARRAY ? vp.array() : NULL));
                                if (!a)
                                    pvals[p].copyFrom(curtp.t == TP_ITUPLE_NULL ? vp : vnull);
                                if (useInd || (!useSrcArr && p == pfrom))
                                    pvals[p].copyFrom(a->at(i));
                                else
                                    pvals[p].copyFrom((SetUtil::tupleInSet(_execContext, a->defset(), curtp, n) ? a->at(n) : &vnull));
                            }
                        }
                        if (ptc >= pend) {
                            CmplArray *a = chg->baseVS()->values();
                            if (useInd || useSrcArr || !a)
                                pvals[ptc].copyFrom((a ? a->at(i) : &vnull));
                            else
                                pvals[ptc].copyFrom((SetUtil::tupleInSet(_execContext, a->defset(), curtp, n) ? a->at(n) : &vnull));
                        }
                    }
                    else {
                        if (SetUtil::tupleInSet(_execContext, dt->values()->defset(), curtp, n))
                            continue;

                        for (unsigned p = pnxt-1; p < pend; p++) {
                            CmplVal& vp = map[p];
                            if (vp) {
                                CmplArray *a = (vp.t == TP_VALUESTORE ? vp.valueStore()->values() : (vp.t == TP_ARRAY ? vp.array() : NULL));
                                if (!a)
                                    pvals[p].copyFrom(curtp.t == TP_ITUPLE_NULL ? vp : vnull);
                                if (p < pnxt)
                                    pvals[p].copyFrom(a->at(i));
                                else
                                    pvals[p].copyFrom((SetUtil::tupleInSet(_execContext, a->defset(), curtp, n) ? a->at(n) : &vnull));
                            }
                        }
                    }

                    // store merged value in result value store
                    CmplValAuto dv;
                    mergeVal(dv, pvals, pfrom, pend2, ptc);
                    if (dv.t != TP_NULL)
                        dt->setSingleValue(_execContext, &curtp, 0, dv, _cbContext->syntaxElem());
                }

                if ((!chg || chg->fullChg() || chg->defsetChg()) && pnxt < pend) {
                    // go to next part to use tuples from
                    while (!(map[pnxt]))
                        pnxt++;

                    for (unsigned p = pfrom; p < pnxt; p++)
                        pvals[p].dispSet((map[p] ? TP_BLANK : TP_EMPTY));
                    if (useSrcArr && fset)
                        pvals[ptc].dispSet(TP_BLANK);

                    CmplVal& vp2 = map[pnxt++];
                    iset = (vp2.t == TP_VALUESTORE ? &(vp2.valueStore()->values()->defset()) : (vp2.t == TP_ARRAY ? &(vp2.array()->defset()) : &ntpls));
                    fset = false;
                }
                else {
                    iset = NULL;
                }
            }

            if (!dstVS)
                res.set(TP_ARRAY, dt->values());
        }

        return !dstVS;

        // Test ob Ziel-Definitionsset direkt gegeben:
        //  wenn kein chgInfo:
        //      potentielles Ziel-Definitionsset ist das des ersten verwendeten Teils
        //      Vergleich mit den Sets aller uebrigen verwendeten Teile (einfacher cmplVal-Vergleich (bei Objekt nur auf Objektidentitaet))
        //      wenn alle gleich: Ziel-Definitionsset gegeben
        //  wenn chgInfo und fullChg:
        //      potentielles Ziel-Definitionsset ist das des ersten verwendeten Teils
        //      Vergleich mit den Sets aller uebrigen verwendeten Teile (einfacher cmplVal-Vergleich (bei Objekt nur auf Objektidentitaet))
        //          wenn _trueCondPart < 0 dann auch Vergleich mit Ausgangsarray
        //      wenn alle gleich: Ziel-Definitionsset gegeben
        //  wenn chgInfo und !fullChg:
        //      Nur moeglich wenn !_defsetAdd && !_defsetDel
        //      Pruefen ueber _chgFlags:
        //          gar kein Element geaendert (sollte nie vorkommen): Ergebnis ist TP_NULL, weiter nichts zu tun
        //          alle Elemente geaendert: Ziel-Definitionsset gegeben aus Ausgangsarray
        //          sonst: nicht gegeben

        // Wenn Ziel-Definitionsset direkt gegeben:
        //      Ergebnisarray mit dem Definitionsset anlegen
        //      Pro Index durchgehen (Tuple ist unwichtig, keine Iteration darueber notwendig):
        //          Quellwerte aus allen Teilen holen (abhaengig von _trueCondPart auch aus Ausgangsarray); ueberall einfach mit selbem Index
        //          Eigentliches Merge aufrufen, Ergebnis daraus im Zielarray unter dem Index
        //      abweichend wenn Definitionsset Nulltupleset:
        //          kein Ergebnisarray, sondern einzelnen Ergebniswert direkt verwenden
        // Sonst wenn chgInfo und !fullChg:
        //      Wenn !_defsetAdd && !_defsetDel:
        //          Leeres Ergebnisarray anlegen
        //          Set-Iteration ueber Definitionsset des Ausgangsarrays:
        //              Pruefen ob Tupel einzubeziehen: ueber Indexnummer in _chgFlags, wenn ja:
        //                  Quellwerte aus allen Teilen holen (abhaengig von _trueCondPart auch aus Ausgangsarray); ueberall einfach mit selbem Index
        //                  Eigentliches Merge aufrufen
        //                  Ergebnis daraus dem Ergebnisarray hinzufuegen, unter dem aktuellen Tupel der Iteration
        //      Sonst:
        //          Leeres Ergebnisarray anlegen
        //          Set-Iteration ueber Definitionsset des Ausgangsarrays analog oben; aber bei Bestimmung der Quellwerte unterscheiden:
        //              wenn Set des Teilarrays gleich dem des Ausgangsarrays (einfacher cmplVal-Vergleich): wie oben einfach mit selbem Index
        //                  sonst muss Index jeweils ueber Tupelsuche bestimmt werden
        //              wenn _trueCondPart >= 0, dann kann Tupel in saemtlichen Teilarrays fehlen, dann nicht in Ergebnisarray
        //          Wenn _defsetAdd, danach alle Teilarrays durchgehen:
        //              Vergleich mit Ausgangsarray (einfacher cmplVal-Vergleich), nur wenn ungleich:
        //                  Set-Iteration ueber das Teilarray, Tupel im bisherigen Ergebnisarray suchen, nur wenn noch nicht enthalten:
        //                      Quellwerte aus nachfolgenden Teilarrays ueber Tupelsuche holen (im Ausgangsarray und davorliegenden Teilarrays jedenfalls nicht enhalten)
        //                      wie oben eigentliches Merge
        // Sonst (kein chgInfo oder fullChg):
        //          Wie im vorherigen Fall, aber wenn _trueCondPart >= 0, dann ist das erste Teilarray das erste durchzugehende Array (anstelle des Ausgangsarrays)
        //              und keine Pruefung ueber _chgFlags, sondern immer alle Arrayelemente einbeziehen
    }


    /**
     * merge one value
     * @param res       return of merged result
     * @param pvals     source value per codeblock part
     * @param pfrom     first used codeblock part
     * @param pend      after last used codeblock part
     * @param ptc       part in pvals with condition TP_TRUE
     */
    void VarCondMapping::mergeVal(CmplVal& res, vector<CmplValAuto>& pvals, unsigned pfrom, unsigned pend, unsigned ptc)
    {
        CmplValAuto& tcv = pvals[ptc];
        unsigned cnteq = 0;
        ValFormulaCondOp *resv = NULL;
        OptCon *oc = (tcv.isOptRow() ? tcv.optCon() : NULL);
        bool complete = !tcv.isEmpty();

        for (unsigned p = pfrom; p < pend; p++) {
            if (_partVarCond[p]) {
                CmplValAuto& pv = pvals[p];

                if ((!pv.isEmpty() && pv != tcv) || (pv.isEmpty() && !tcv.isEmpty())) {
                    cnteq = 0;
                    if (!checkType(res, pv, oc))
                        return;
                    if (oc)
                        continue;

                    if (!resv) {
                        if (!checkType(res, tcv, oc))
                            return;

                        resv = new ValFormulaCondOp(_cbContext->syntaxElem());
                        res.set(TP_FORMULA, resv);

                        if (p > pfrom && !tcv.isEmpty()) {
                            for (unsigned pp = pfrom; pp < p; pp++) {
                                if (_partVarCond[pp])
                                    resv->insPart(_partVarCond[pp], *(_prvPartConds[pp]), tcv);
                            }
                        }
                    }
                }
                else {
                    cnteq++;
                }

                if (!pv.isEmpty()) {
                    if (resv)
                        resv->insPart(_partVarCond[p], *(_prvPartConds[p]), pv);
                }
                else {
                    if (complete)
                        complete = false;
                }
            }
        }

        if (resv) {
            if (!tcv.isEmpty()) {
                CmplValAuto emp(TP_EMPTY);
                resv->insPart(emp, *(_prvPartConds[ptc]), tcv);

                if (cnteq)
                    resv->mergeLast(cnteq);
            }

            resv->setComplete(complete);
        }
        else if (oc) {
            res.dispSet((oc->objective() ? TP_OPT_OBJ : TP_OPT_CON), oc);
        }
        else {
            if (!tcv.isEmpty())
                res.copyFrom(tcv);
            else
                res.dispSet(TP_NULL);
        }

        //  Wert pvals[ptc] holen als Vergleichswert
        //  Werte in pvals durchgehen, soweit betroffen:
        //      Wenn Wert ungleich Vergleichswert:
        //          Pruefung ob sowohl aktueller Wert nummerisch (auch TP_BIN, auch Formel) oder TP_NULL (sonst Fehler; Vergleichswert als Ersatzergebnis)
        //          Wenn erster solcher Wert:
        //              Auch pruefen ob Vergleichswert nummerisch (auch TP_BIN, auch Formel) oder TP_NULL (sonst Fehler; Vergleichswert als Ersatzergebnis)
        //              ValFormulaCondOp als Ergebniswert bereitstellen
        //              Wenn nicht bei pfrom und Wert nicht TP_NULL:
        //                  Alle Teile vor dem aktuellen durchgehen und eintragen (ueberall selber Wert)
        //          Wert fuer aktuellen Teil eintragen (soweit nicht TP_NULL)
        //          Kennzeichen merken fuer Wert ungleich Vergleichswert
        //  Wenn gar kein Wert ungleich Vergleichswert (ValFormulaCondOp nicht bereitgestellt):
        //      Vergleichswert als Ergebnis kopieren
        //  Sonst wenn Kennzeichen fuer letzten ungleichen Wert nicht pend-1:
        //      Gleiche Werte hinten in ValFormulaCondOp zusammenfassen

        // Eigentlicher Merge:
        //  erhaelt: pro Teil den dortigen Wert bzw. TP_NULL wenn nicht enthalten
        //              (in mindestens 1 uebergebenem Teil existiert ein Wert, sonst gar nicht erst im Zieldefinitionsset)
        //      Teile durchgehen und pruefen (jeweils pfrom - pend-1, und zusaetzlich ptc):
        //          alle gleich (und auch kein TP_NULL): Wert ist direkt Ergebniswert
        //          sonst:
        //                  (c1: v1), (!c1 && c2: v2), (!c1 && !c2 ... && cn: vn), (!c1 && !c2 ... && !cn: tv)
        //              Teile, deren Wert TP_NULL ist, werden weggelassen
        //              Wenn von hinten beginnend Teile gleich sind (und nicht TP_NULL), dann zusammenfassen:
        //                      z.B.: vn==tv -> (!c1 && !c2 ... && !cn-1: tv)
        //              Alle c muessen binaer (Formel mit Binaerkennzeichen) sein
        //              Alle v muessen numerisch (auch TP_BIN) oder Formel oder Restriktion sein
        //              Ergebnis ist Bedingungsformel
        //              Ergebnisbedingungsformel hat Kennzeichen dass binaer, wenn alle Teilwerte v1..vn,tv binaer sind

        //  aber abweichend fuer Objekttyp obj/con:
        //          es darf nur einen solchen Wert geben (ausser Duplikate), sonst muss alles leer sein
        //          dann diesen einen Wert unbedingt setzen (unabhaengig davon aus welchem Bedingungsteil)
        //          sonst Fehler
        //          (obj ist tatsaechlich immer Fehler, aber der wurde bereits bei der Generierung gemeldet)

    }

    /**
     * check whether value has valid type for merge
     * @param res       set this to replacement value if type is invalid
     * @param v         value to check
     * @param oc        optimization constraint or objective if found in values / NULL: no one
     * @return          true if value has valid type
     */
    bool VarCondMapping::checkType(CmplVal& res, CmplVal& v, OptCon *&oc)
    {
        if (oc) {
            if (v.isEmpty()) {
                return true;
            }
            else if (!v.isOptRow() || v.optCon() != oc) {
                _execContext->valueError("constraint must be unique in conditional expression", v, _cbContext->syntaxElem());
                res.dispSet((oc->objective() ? TP_OPT_OBJ : TP_OPT_CON), oc);
                return false;
            }
        }
        else if (v.isOptRow()) {
            oc = v.optCon();
        }
        else if (!v.isScalarNumber() && v.t != TP_FORMULA && v.t != TP_OPT_VAR && !v.isEmpty()) {
            _execContext->valueError("value has not a valid type for a conditional expression", v, _cbContext->syntaxElem());
            res.copyFrom(v);
            return false;
        }

        return true;
    }


    /**
     * get full condition (including from parent) for current codeblock part
     */
    CmplValAuto &VarCondMapping::getFullCond()
    {
        if (!_curFullCond && _curCBPart >= 0) {
            _curFullCond.copyFrom(_partVarCond[_curCBPart]);
            unsigned se = _cbContext->syntaxElem();

            vector<CmplValAuto> *ncs = _prvPartConds[_curCBPart];
            if (ncs) {
                for (CmplVal& nc : *ncs) {
                    CmplValAuto tn, tc;
                    OperationBase::execUnaryOper(_execContext, &tn, se, ICS_OPER_NOT, &nc);
                    OperationBase::execBinaryOper(_execContext, &tc, se, ICS_OPER_AND, false, &_curFullCond, &tn);
                    _curFullCond.moveFrom(tc, true);
                }
            }

            if (_parent) {
                CmplVal& pfc = _parent->getFullCond();
                CmplValAuto tc;
                OperationBase::execBinaryOper(_execContext, &tc, se, ICS_OPER_AND, false, &pfc, &_curFullCond);
                _curFullCond.moveFrom(tc, true);
            }
        }

        return _curFullCond;
    }
}

