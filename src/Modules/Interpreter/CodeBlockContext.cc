
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


#include "CodeBlockContext.hh"
#include "ExecContext.hh"
#include "OperationBase.hh"
#include "Interpreter.hh"
#include "SetUtil.hh"
#include "TupleUtil.hh"
#include "../../CommonData/CmplArray.hh"
#include "../../CommonData/ValFormula.hh"
#include "../../Control/MainControl.hh"


namespace cmpl
{

	/*********** CodeBlockContext **********/

    /**
     * destructor
     */
    CodeBlockContext::~CodeBlockContext()
    {
        if (_startIter)
            delete _startIter;

        if (!_headers.empty()) {
            for (CodeBlockHeaderBase *&cbh : _headers)
                delete cbh;
            _headers.clear();
        }

        const_cast<CmplVal&>(_cbResult).dispose();
    }


    /**
     * create new code block context object if needed at code block start
     * @param ctx       execution context
     * @param cd        intermediary code command (here INTCODE_CODEBLOCK/ICS_CB_BLOCK_START)
     * @return          pointer to new code block context or NULL if no context is needed
     */
    CodeBlockContext *CodeBlockContext::newContextStart(ExecContext *ctx, IntCode::IcElem *cd)
    {
        // no context is needed if the codeblock doesn't use iteration and no control command belongs to the codeblock
        if ((cd->v.c.par & (ICPAR_BLOCK_OP_IN | ICPAR_BLOCK_CTRL)) == 0)
            return NULL;

        return new CodeBlockContext(ctx, cd, ctx->curCBLevel());
    }

    /**
     * create new code block context object when needed whitin the execution of the codeblock
     * @param ctx       execution context
     * @param hd        intermediary code command (here INTCODE_CB_HEADER/ICS_CBHEADER_END)
     * @return          pointer to new code block context
     */
    CodeBlockContext *CodeBlockContext::newContextAdd(ExecContext *ctx, IntCode::IcElem *hd)
    {
        CodeBlockContext *cb = new CodeBlockContext(ctx, ctx->codeBase() + hd[2].v.n.n1, ctx->curCBLevel() - 1);
        cb->startPart(ctx->codeBase() + hd[2].v.n.n2);
        return cb;
    }


    /**
     * stores a codeblock header for later execution
     * @param cd        intermediary code command for codeblock header start
     */
    void CodeBlockContext::storeHeader(IntCode::IcElem *cd)
    {
        CodeBlockHeaderExec *cbh = new CodeBlockHeaderExec(_execContext, cd);
        _headers.push_back(cbh);
    }

    /**
     * stores a codeblock header with iteration
     * @param cbhIter   iteration info / NULL: insert pseudo iteration if neccessary
     * @param cd        intermediary code command for codeblock header start
     */
    void CodeBlockContext::storeHeader(CBHeaderIterInfo *cbhIter, IntCode::IcElem *cd)
    {
        if (cbhIter) {
            if (!_startIter) {
                _startIter = cbhIter;
            }
            else if (_headers.empty() && _startIter->merge(cbhIter, ordered())) {
                delete cbhIter;
            }
            else {
                CodeBlockHeaderIter *cbh = new CodeBlockHeaderIter(_execContext, cd, cbhIter);
                _headers.push_back(cbh);
            }
        }
        else if (!_startIter && _headers.empty()) {
            unsigned short par = _execContext->codeBase()[_curBodyCode].v.c.par;
            if (par & ICPAR_BLOCK_OP_IN)
                _startIter = new CBHeaderIterInfo();
        }
    }


    /**
     * get whether ordered execution for current part of this codeblock
     */
    bool CodeBlockContext::ordered() const
    {
        return ((_curPartPar & ICPAR_BLOCK_ORDERED) || _execContext->modp()->orderedIter());
    }


    /**
     * add a conditions over optimization variable for the current codeblock part
     * @param v             value with the condition formula (must be TP_FORMULA, TP_OPT_VAR or TP_BIN with value true)
     * @param se            syntax element of v
     */
    void CodeBlockContext::addCurVarCondition(CmplVal *v, unsigned se)
    {
        if (!hasVarCondition() && (_icStart->v.c.par & (ICPAR_BLOCK_OP_IN | ICPAR_BLOCK_REPEAT))) {
            if (_icStart->v.c.par & ICPAR_BLOCK_OP_IN)
                _execContext->valueError("mix of iterations and conditions over optimization variables is not allowed in same codeblock", *v, se);
            else
                _execContext->valueError("use of 'repeat' and conditions over optimization variables is not allowed in same codeblock", *v, se);

            return;
        }

        if ((v->t != TP_FORMULA && v->t != TP_OPT_VAR && v->t != TP_BIN) || (v->t == TP_BIN && v->v.i == 0))
            _execContext->internalError("wrong value type for condition over optimization variables");
        if ((v->t == TP_FORMULA && !(v->valFormula()->isBool())) || (v->t == TP_OPT_VAR && !v->optVar()->binVar()))
            _execContext->internalError("condition over optimization variables must be a boolean expression");

        if (_curVarCondition.t != TP_FORMULA && _curVarCondition.t != TP_OPT_VAR) {
            _curVarCondition.copyFrom(v);
        }
        else if (v->t == TP_FORMULA || v->t == TP_OPT_VAR) {
            CmplValAuto r;
            OperationBase::execBinaryOper(_execContext, &r, se, ICS_OPER_AND, false, &_curVarCondition, v);
            _curVarCondition.moveFrom(r, true);
        }
    }

    /**
     * get codeblock result value
     */
    CmplVal& CodeBlockContext::cbResult()
    {
        // function only called after all threads started in this codeblock are ended, so discarding volatile is safe
        CmplVal& cbRes = const_cast<CmplVal&>(_cbResult);

        if (_hasVarCond)
            _execContext->varCondMapMerge(this, cbRes);

        if (cbRes.t == TP_VALUESTORE) {
            CmplArray *a = cbRes.valueStore()->values();
            CmplVal v(TP_ARRAY, a);
            cbRes.moveFrom(v, true);
        }

        return cbRes;
    }

    /**
     * start execution of a part of the codeblock
     * @param cd            intermediary code command
     */
    void CodeBlockContext::startPart(IntCode::IcElem *cd)
    {
        // init part
        _curPartPar = cd->v.c.par;
        _curPart = _curPartPar & ICPAR_BLOCK_CNTPARTS;
        _lastPart = (_curPart + 1 == (int)partCnt());

        IntCode::IcElem *partEnd = _execContext->codeBase() + cd[1].v.n.n2;
        _localSymTo = partEnd[2].v.n.n1;
        _localSymFrom = partEnd[2].v.n.n2;

        if (_curVarCondition)
            _curVarCondition.dispUnset();

        _curBodyCode = 0;
        if (_startIter) {
            delete _startIter;
            _startIter = NULL;
        }

        if (!_headers.empty()) {
            for (CodeBlockHeaderBase *&cbh : _headers)
                delete cbh;
            _headers.clear();
        }

        if (_hasVarCond && (_curPartPar & (ICPAR_BLOCK_OP_IN | ICPAR_BLOCK_REPEAT)))
            _execContext->internalError("use of iteration or 'repeat' together with conditions over optimization variables");
    }


    /**
     * execute the body of a codeblock part
     * @param cd            intermediary code command
     */
    void CodeBlockContext::execPartBody(IntCode::IcElem *cd)
    {
        if (_cancel)
            return;

        _curBodyCode = cd - _execContext->codeBase();

        if (hasVarCondition()) {
            if (!_startIter) {
                if (!_curVarCondition)
                    _curVarCondition.set(TP_BIN, true);

                _execContext->varCondMapStartPart(this, _curPart, _curVarCondition, _localSymFrom, !_hasVarCond);

                if (!_hasVarCond)
                    _hasVarCond = true;
            }
            else {
                _execContext->internalError("mix of iterations and conditions over optimization variables");
                _curVarCondition.dispUnset();
            }
        }

        if (_startIter) {
            CodeBlockIteration cbIter(_execContext, this, _startIter, -1);
            _startIterObj = &cbIter;
            CmplArray *res = cbIter.run();
            _startIterObj = NULL;

            if (res) {
                //TODO: kuenftig erstmal als TP_ARRAY_COMP, erst spaeter bei Bedarf umwandeln
                //CmplVal v(TP_ARRAY_COMP, res);
                //  (vorlaeufig wird schon hier umgewandelt, damit andere Stellen noch nicht an TP_ARRAY_COMP angepasst werden muessen)
                CmplValAuto v(TP_ARRAY, StackValue::arrayFromArrayComp(_execContext, res));
                assignResult(v, false, _execContext->codeBase()[_curBodyCode].se);
            }
        }
        else {
            CmplValAuto res;
            _execContext->run(_curBodyCode, false, &res);
            _execPart = _curPart;

            if (res)
                assignResult(res, true, _execContext->codeBase()[_curBodyCode].se);

            if (hasVarCondition())
                _execContext->varCondMapEndPart(this, _curPart);
        }

        _execContext->uninitCBSymbols(_localSymFrom, _localSymTo, true);
    }


    /**
     * end of codeblock part
     * @return              true if codeblock completed (jump to codeblock end) or false if proceeding with next part
     */
    bool CodeBlockContext::endPart()
    {
        return (_execPart == _curPart && _curVarCondition.t != TP_FORMULA && _curVarCondition.t != TP_OPT_VAR);
    }


    /**
     * set result value and cancel flag for codeblock control command; call this function in the destination codeblock of the control command
     * @param srcCtx	source execution context
     * @param cmd		control command (one of ICS_CBC_BREAK, ICS_CBC_CONTINUE, ICS_CBC_REPEAT)
     * @param srcLvl	nesting codeblock level of source codeblock
     * @param lhs		index tuple on left hand side of the control command / NULL: no left hand side / TP_EMPTY: left hand side without index tuple
     * @param rhs		right hand side of the control command / NULL: no right hand side
     */
    void CodeBlockContext::controlCommand(ExecContext *srcCtx, int cmd, unsigned srcLvl, StackValue *lhs, StackValue *rhs)
    {
        LockGuard<recursive_mutex> lck(srcCtx->needLock(), _cancelMtx);
        if (!_cancel) {
            _ctrlCmd = cmd;
            if (rhs)
                assignResult(rhs, lhs);

            if (cmd == ICS_CBC_CONTINUE) {
                if (_lvl < srcLvl) {
                    _execContext->setCancel(cmd, _lvl);
                    CodeBlockContext *cbc = srcCtx->getNextUpperCBContext(_lvl);
                    if (cbc)
                        cbc->setCancel(cmd, _lvl);
                }
            }
            else {
                _cancel = true;
                _execContext->setCancel(cmd, _lvl);
                if (_startIterObj)
                    _startIterObj->setCancel(cmd, _lvl);
            }
        }
    }


    /**
     * set and propagate cancel flag in nesting codeblocks within the destination codeblock
     * @param cmd		control command (one of ICS_CBC_BREAK, ICS_CBC_CONTINUE, ICS_CBC_REPEAT)
     * @param dstLvl	nesting codeblock level of destination codeblock of the control command (must be lesser than <code>_lvl</code>)
     */
    void CodeBlockContext::setCancel(int cmd, unsigned dstLvl)
    {
        LockGuard<recursive_mutex> lck(true, _cancelMtx);
        if (!_cancel) {
            const_cast<CmplVal&>(_cbResult).dispUnset();

            _cancel = true;
            if (_startIterObj)
                _startIterObj->setCancel(cmd, dstLvl);
        }
    }


    /**
     * assign value to codeblock result
     * @param val		value to assign
     * @param tpl		index tupel for value
     */
    void CodeBlockContext::assignResult(StackValue *val, StackValue *tpl)
    {
        if (!_cancel && val) {
            if (tpl && tpl->val()) {
                CmplValAuto ind;
                TupleUtil::toIndexTuple(_execContext, ind, tpl, false);

                if (ind == TP_SET_ALL)
                {
                    assignResult(val->val(), false, val->syntaxElem());
                }
                else
                {
                    StackValue *cval = StackValue::castArray(_execContext, val, ind, tpl->syntaxElem());
                    assignResult(cval->val(), false, val->syntaxElem());
                    _execContext->pop();
                }
            }
            else {
                assignResult(val->val(), true, val->syntaxElem());
            }
        }
    }

    /**
     * assign value to codeblock result
     * @param val		value to assign
     * @param ovr		override all existing results
     * @param se		syntax element id of source value
     */
    void CodeBlockContext::assignResult(CmplVal& val, bool ovr, unsigned se)
    {
        if (!_cancel && val.t != TP_BLANK) {
            if (_hasVarCond && _curPart >= 0) {
                _execContext->varCondMapSetCBResult(this, _curPart, val, ovr, se);
            }
            else {
                // function is only called either without threading or within lock, so discarding volatile is safe
                CmplVal& cbRes = const_cast<CmplVal&>(_cbResult);

                if (cbRes.t == TP_BLANK || cbRes.t == TP_EMPTY) {
                    cbRes.copyFrom(val, true, false);
                }
                else {
                    if (ovr && cbRes.t != TP_VALUESTORE) {
                        cbRes.copyFrom(val, true, true);
                    }
                    else {
                        if (cbRes.t != TP_VALUESTORE) {
                            CmplVal cbn(TP_VALUESTORE, new ValueStore(cbRes));
                            cbRes.moveFrom(cbn, true);
                        }

                        cbRes.valueStore()->setValue(_execContext, NULL, ovr, NULL, &val, se);
                    }
                }
            }
        }
    }


    /**
     * check whether codeblock is subject of a repeat command, if yes then init codeblock for restart
     * @return          start command of codeblock in the intermediary code / 0: if no repeat command
     */
    IntCode::IcElem* CodeBlockContext::checkStartRepeat()
    {
        LockGuard<recursive_mutex> lck(true, _cancelMtx);
        if (_cancel && _ctrlCmd == ICS_CBC_REPEAT) {
            _execPart = _curPart = -1;
            _curBodyCode = 0;

            if (_startIter) {
                delete _startIter;
                _startIter = NULL;
            }

            if (!_headers.empty()) {
                for (CodeBlockHeaderBase *&cbh : _headers)
                    delete cbh;
                _headers.clear();
            }

            _startIterObj = NULL;
            _cancel = false;
            _ctrlCmd = 0;

            if (hasVarCondition() || !_icStart) {
                if (hasVarCondition())
                    _execContext->internalError("execution of repeat within codeblock with conditions over optimization variables");
                else
                    _execContext->internalError("execution of repeat within codeblock without known start code address");

                _curVarCondition.dispUnset();
                return 0;
            }

            return _icStart;
        }

        return 0;
    }



    /*********** CodeBlockHeaderBase **********/

    /**
     * get syntax element number of the codeblock header
     * @param ctx           execution context
     */
    unsigned CodeBlockHeaderBase::syntaxElem(ExecContext *ctx) const
    {
        return ctx->codeBase()[_code].se;
    }

    /**
     * executes equality operation with codeblock symbol definition
     * @param ctx			execution context
     * @param cbs           value for codeblock symbol definition, must be TP_DEF_CB_SYM or TP_DEF_CB_SYM_TUPLE
     * @param a2            other operand
     * @param se			syntax element id of operation
     * @return              result value
     */
    bool CodeBlockHeaderBase::execCbsOperEq(ExecContext *ctx, CmplVal *cbs, StackValue *a2, unsigned se)
    {
        bool b = false;

        if (cbs->t == TP_DEF_CB_SYM) {
            ctx->setCBSymbolVal(cbs->v.i, NULL, a2);
            b = true;
        }
        else if (cbs->t == TP_DEF_CB_SYM_TUPLE) {
            unsigned cntCbs = cbs->tuple()->countPartType(TP_DEF_CB_SYM);
            CmplValAuto ca2;
            TupleUtil::canonicalTuple(ctx, ca2, a2->val());

            if (cntCbs == cbs->tuple()->rank()) {
                CBAssignInfoTuple ai(cbs->tuple());
                ai.setCBSymbolValues(ctx, ca2, 0);
                b = true;
            }
            else
            {
                CmplValAuto ccbs;
                TupleUtil::canonicalTuple(ctx, ccbs, *cbs);

                unsigned r = ccbs.tuple()->rank();
                unique_ptr<unsigned> at(new unsigned[r]);
                unique_ptr<unsigned> atp(new unsigned[cntCbs]);

                b = TupleUtil::matchTuple(ctx, se, ccbs, ca2, at.get());
                if (b) {
                    CmplValAuto v;
                    unsigned tpr, a2i = 0;
                    CmplVal *p = ccbs.tuple()->at(0);

                    for (unsigned i = 0; i < r; i++, p++) {
                        tpr = at.get()[i];
                        if (p->t == TP_DEF_CB_SYM) {
                            ca2.tuple()->partTuple(v, a2i, tpr);
                            ctx->setCBSymbolVal(p->v.i, &v);
                            v.dispose();
                        }
                        a2i += tpr;
                    }
                }
            }
        }

        return b;
    }

    /**
     * executes "in" or "of" operation with codeblock symbol definition
     * @param ctx			execution context
     * @param opIn          true if "in" operation, false if "of" operation
     * @param cbs           value for codeblock symbol definition, must be TP_DEF_CB_SYM or TP_DEF_CB_SYM_TUPLE
     * @param a2            other operand
     * @param se			syntax element id of operation
     * @return              result value
     */
    CBHeaderIterInfo* CodeBlockHeaderBase::execCbsOperIn(ExecContext *ctx, bool opIn, CmplVal *cbs, StackValue *a2, unsigned se)
    {
        CBAssignInfoBasis *ai = NULL;
        CmplValAuto set;
        bool tpm = (cbs->t != TP_DEF_CB_SYM && cbs->tuple()->countPartType(TP_DEF_CB_SYM) != cbs->tuple()->rank());

        if (!SetUtil::setForInOrOf(ctx, set, a2->val(), opIn, tpm)) {
            ctx->valueError("right operand for 'in' as iteration codeblock header must be a set", a2);
            set.set(TP_SET_EMPTY);
        }

        if (set.isSetInf()) {
            ctx->valueError("right operand for 'in' as iteration codeblock header must be a finite set", a2);
            set.set(TP_SET_EMPTY);
        }

        if (tpm) {
            try {
                TupleMatching tm(ctx, TupleMatching::matchIter, set, *cbs, true);

                CmplValAuto its;
                if (tm.match(its)) {
                    set.moveFrom(its, true);
                    ai = tm.takeAssignInfo();
                }
            }
            catch (TupleMatchingValueException& ex) {
                if (ex.invSrc())
                    ctx->valueError("right operand for 'in' as iteration codeblock header must be a finite set", a2);
                else
                    ctx->valueError("tuple on left side in iteration codeblock header must be an index tuple", *cbs, se);
            }
        }
        else if (SetBase::cnt(set)) {
            if (cbs->t == TP_DEF_CB_SYM)
                ai = new CBAssignInfoSingle(cbs->v.i);
            else
                ai = new CBAssignInfoTuple(cbs->tuple());
        }

        if (ai)
            return new CBHeaderIterInfo(set, ai);
        else
            return NULL;
    }


    /*********** CodeBlockHeaderExec **********/

    /**
     * constructor
     * @param ctx       execution context
     * @param cd        intermediary code command for codeblock header start
     */
    CodeBlockHeaderExec::CodeBlockHeaderExec(ExecContext *ctx, IntCode::IcElem *cd)
    {
        _headNum = (cd->v.c.par & ICPAR_CBHP_HCNT);
        _code = cd - ctx->codeBase();
    }


    /**
     * get resulting value from codeblock header
     * @param ctx           execution context
     * @param res           store for result value (can only be one of TP_BIN, TP_CBHEAD_ITER, TP_FORMULA)
     */
    void CodeBlockHeaderExec::getResult(ExecContext *ctx, CmplVal& res)
    {
        ctx->run(_code, false, &res);
    }


    /*********** CodeBlockHeaderIter **********/

    /**
     * constructor
     * @param ctx       execution context
     * @param cd        intermediary code command for codeblock header start
     * @param cbhIter   iteration info
     */
    CodeBlockHeaderIter::CodeBlockHeaderIter(ExecContext *ctx, IntCode::IcElem *cd, CBHeaderIterInfo *cbhIter)
    {
        _headNum = (cd->v.c.par & ICPAR_CBHP_HCNT);
        _code = cd - ctx->codeBase();
        _iterInfo = cbhIter;
    }

    /**
     * destructor
     */
    CodeBlockHeaderIter::~CodeBlockHeaderIter()
    {
        if (_iterInfo)
            delete _iterInfo;
    }



    /*********** CBAssignInfoEmpty **********/

    /**
     * get this transformed to CBAssignInfoTuple
     * @param r         rank of iteration set this assignment info is used for
     * @return          CBAssignInfoTuple object (can be this)
     */
    CBAssignInfoTuple *CBAssignInfoEmpty::asAssignInfoTuple(unsigned r)
    {
        return (r == 0 ? new CBAssignInfoTuple(NULL) : NULL);
    }


    /*********** CBAssignInfoSingle **********/

    /**
     * set the values for codeblock symbols within the execution context
     * @param ctx		execution context
     * @param curIter	current tuple within the iteration
     * @param curInd	current index number within the iteration
     * @return          symbol number (fetch id) of last set symbol + 1
     */
    unsigned CBAssignInfoSingle::setCBSymbolValues(ExecContext *ctx, const CmplVal &curIter, unsigned long curInd)
    {
        ctx->setCBSymbolVal(_cbFetchID, &curIter);
        return _cbFetchID + 1;
    }

    /**
     * get this transformed to CBAssignInfoTuple
     * @param r         rank of iteration set this assignment info is used for
     * @return          CBAssignInfoTuple object (can be this)
     */
    CBAssignInfoTuple *CBAssignInfoSingle::asAssignInfoTuple(unsigned r)
    {
        Tuple tpl(1);
        tpl[0].set(TP_DEF_CB_SYM, (intType)_cbFetchID);
        return new CBAssignInfoTuple(&tpl, r);
    }


    /*********** CBAssignInfoTuple **********/

    /**
     * constructor
     * @param tpl		tuple with fetch ids of the codeblock symbols within the execution context
     */
    CBAssignInfoTuple::CBAssignInfoTuple(Tuple *tpl, unsigned r)
    {
        _cnt = (tpl ? tpl->rank() : 0);
        _cbFetchIDs = new unsigned[_cnt];
        _tpLen = new unsigned[_cnt];

        for (unsigned i = 0; i < _cnt; i++) {
            _cbFetchIDs[i] = tpl->at(i)->v.i;
            _tpLen[i] = (!r || (i < r && i + 1 < _cnt) ? 1 : (i < r ? r - i : 0));
        }
    }

    /**
     * constructor: merge two assignment info objects
     * @param ai1		assignment info object #1
     * @param ai2		assignment info object #2
     */
    CBAssignInfoTuple::CBAssignInfoTuple(CBAssignInfoTuple *ai1, CBAssignInfoTuple *ai2)
    {
        _cnt = ai1->_cnt + ai2->_cnt;
        _cbFetchIDs = new unsigned[_cnt];
        _tpLen = new unsigned[_cnt];

        unsigned i, j = 0;
        for (i = 0; i < ai1->_cnt; i++, j++) {
            _cbFetchIDs[j] = ai1->_cbFetchIDs[i];
            _tpLen[j] = ai1->_tpLen[i];
        }
        for (i = 0; i < ai2->_cnt; i++, j++) {
            _cbFetchIDs[j] = ai2->_cbFetchIDs[i];
            _tpLen[j] = ai2->_tpLen[i];
        }
    }

    /**
     * set the values for codeblock symbols within the execution context
     * @param ctx		execution context
     * @param curIter	current tuple within the iteration
     * @param curInd	current index number within the iteration
     * @return          symbol number (fetch id) of last set symbol + 1
     */
    unsigned CBAssignInfoTuple::setCBSymbolValues(ExecContext *ctx, const CmplVal &curIter, unsigned long curInd)
    {
        unsigned tps = 0;
        unsigned r = (curIter.isTuple() ? Tuple::rank(curIter) : 1);
        CmplValAuto v;

        for (unsigned i = 0; i < _cnt; i++) {
            unsigned tpe = (i == _cnt - 1 ? r : min(tps + _tpLen[i], r));

            if (tpe <= tps) {
                v.set(TP_ITUPLE_NULL);
            }
            else if (tpe == tps + 1) {
                v.copyFrom(Tuple::at(curIter, tps), true, false);
                if (v.isITuple())
                    v.t = TP_INDEX_VAL_BASE(v.t);
            }
            else if (curIter.isTuple()) {
                curIter.tuple()->partTuple(v, tps, tpe - tps);
            }
            else {
                v.copyFrom(curIter, true, false);
            }

            ctx->setCBSymbolVal(_cbFetchIDs[i], &v);
            v.dispose();
            tps = tpe;
        }

        return (_cnt ? _cbFetchIDs[_cnt-1] + 1 : 0);
    }

    /**
     * get number of the first codeblock symbol set by this assignment info
     * @param ctx		execution context
     * @return		symbol number (fetch id)
     */
    unsigned CBAssignInfoTuple::numCBSymbolsFrom(ExecContext *ctx) const
    {
        if (_cnt == 0) {
            ctx->internalError("wrong use of CBAssignInfoEmpty");
            return 0;
        }
        else {
            return _cbFetchIDs[0];
        }
    }

    /**
     * get this transformed to CBAssignInfoTuple
     * @param r         rank of iteration set this assignment info is used for
     * @return          CBAssignInfoTuple object (can be this)
     */
    CBAssignInfoTuple *CBAssignInfoTuple::asAssignInfoTuple(unsigned r)
    {
        unsigned s = 0;
        for (unsigned i = 0; i < _cnt; i++)
            s += _tpLen[i];

        return (s == r ? this : NULL);
    }



    /*********** CBAssignInfoComplex **********/

    /**
     * constructor
     * @param tpl		tuple with fetch ids of the codeblock symbols within the execution context
     * @param pos       if given then use only these positions of tuple
     */
    CBAssignInfoComplex::CBAssignInfoComplex(CmplVal& tpl, vector<unsigned> *pos)
    {
        _cnt = (pos ? pos->size() : Tuple::rank(tpl));
        _cbFetchIDs = new unsigned[_cnt];
        _tpLen = NULL;

        for (unsigned i = 0; i < _cnt; i++) {
            _cbFetchIDs[i] = Tuple::at(tpl, (pos ? pos->at(i) : i))->v.i;
        }

        _curInfo = 0;
    }

    /**
     * destructor
     */
    CBAssignInfoComplex::~CBAssignInfoComplex()
    {
        DELETE_UNSET(_cbFetchIDs);

        for (oneInfo& oi : _info)
            delete[] oi._rank;

        _info.clear();
    }

    /**
     * add single assignment info
     * @param pos       position of element the assign info is for
     * @param ranks     array with ranks to assign
     */
    void CBAssignInfoComplex::addInfo(unsigned long pos, unsigned *ranks)
    {
        if (_info.empty() || pos > _info.back()._end) {
            _info.emplace_back(pos, pos + 1, ranks);
            ranks = NULL;
        }
        else {
            unsigned i;
            unsigned long e;

            for (i = _info.size(); i > 0; i--) {
                oneInfo& oi = _info[i-1];
                if (pos >= oi._start) {
                    if (oi.ranksEq(_cnt, ranks)) {
                        e = ++(oi._end);
                        break;
                    }
                    else if (pos > oi._start) {
                        e = oi._end + 1;
                        oi._end = pos;

                        _info.insert(_info.begin() + (i++), oneInfo(pos, pos + 1, ranks));
                        ranks = NULL;

                        if (e > pos + 1) {
                            unsigned *r2 = new unsigned[_cnt];
                            for (unsigned j = 0; j < _cnt; j++)
                                r2[j] = oi._rank[j];

                            _info.insert(_info.begin() + (i++), oneInfo(pos + 1, e, r2));
                        }

                        break;
                    }
                }
            }

            if (i == 0) {
                e = max(pos + 1, _info.front()._start);
                _info.insert(_info.begin(), oneInfo(pos, e, ranks));
                ranks = NULL;
                i = 1;
            }

            for (; i < _info.size(); i++) {
                oneInfo& oi = _info[i];
                unsigned long d = e - oi._start;
                oi._start += d;
                oi._end += d;
                e = oi._end;
            }
        }

        if (ranks)
            delete[] ranks;
    }

    /**
     * set the values for codeblock symbols within the execution context
     * @param ctx		execution context
     * @param curIter	current tuple within the iteration
     * @param curInd	current index number within the iteration
     * @return          symbol number (fetch id) of last set symbol + 1
     */
    unsigned CBAssignInfoComplex::setCBSymbolValues(ExecContext *ctx, const CmplVal &curIter, unsigned long curInd)
    {
        // check if current info object is matching
        oneInfo *oi = &(_info[_curInfo]);

        if (curInd < oi->_start || (oi->_end && oi->_end <= curInd)) {
            _curInfo = (curInd < oi->_start ? 0 : (_curInfo + 1));

            while (_info[_curInfo]._end && _info[_curInfo]._end <= curInd)
                _curInfo++;

            oi = &(_info[_curInfo]);
        }

        // set values for codeblock symbols
        unsigned t = 0;
        unsigned *rp = oi->_rank;
        unsigned *fp = _cbFetchIDs;
        CmplValAuto v;

        for (unsigned i = 0; i < _cnt; i++, rp++, fp++) {
            if (*rp == 0 || t >= Tuple::rank(curIter)) {
                v.set(TP_ITUPLE_NULL);
            }
            else if (*rp == 1) {
                v.copyFrom(Tuple::at(curIter, t), true, false);
                t++;
            }
            else {
                curIter.tuple()->partTuple(v, t, *rp);
                t += *rp;
            }

            ctx->setCBSymbolVal(_cbFetchIDs[i], &v);
            v.dispose();
        }

        return (_cnt ? _cbFetchIDs[_cnt-1] + 1 : 0);
    }


    /**
     * get this transformed to CBAssignInfoTuple
     * @param r         (not used here)
     * @return          CBAssignInfoTuple object / NULL: not convertible
     */
    CBAssignInfoTuple *CBAssignInfoComplex::asAssignInfoTuple(unsigned r)
    {
        if (_info.size() != 1)
            return NULL;

        oneInfo& oi = _info[0];
        unsigned *cbFetchIDs = new unsigned[_cnt];
        unsigned *tpLen = new unsigned[_cnt];

        for (unsigned i = 0; i < _cnt; i++) {
            cbFetchIDs[i] = _cbFetchIDs[i];
            tpLen[i] = oi._rank[i];
        }

        return new CBAssignInfoTuple(_cnt, cbFetchIDs, tpLen);
    }



    /*********** CBHeaderIterInfo **********/

    /**
     * check if another iteration info can be merged to this and merge it
     * @param cbhIter       other iteration info
     * @param ordered       for ordered iteration
     * @return              true if cbhIter is merged to this
     */
    bool CBHeaderIterInfo::merge(CBHeaderIterInfo *cbhIter, bool ordered)
    {
        if (SetBase::oneRank(_set) && SetBase::oneRank(cbhIter->_set) && (!ordered || (!SetBase::hasDirectUserOrder(_set) && !SetBase::hasDirectUserOrder(cbhIter->_set)))) {
            CmplVal& s1 = SET_VAL_WO_ORDER(_set);
            CmplVal& s2 = SET_VAL_WO_ORDER(cbhIter->_set);

            if (s1.t != TP_SET_FIN && s2.t != TP_SET_FIN) {
                unsigned r1 = SetBase::rank(s1);
                unsigned r2 = SetBase::rank(s2);
                CBAssignInfoTuple *ai1 = _assignInfo->asAssignInfoTuple(r1);
                CBAssignInfoTuple *ai2 = cbhIter->_assignInfo->asAssignInfoTuple(r2);

                if (ai1 && ai2) {
                    SetRecMult *srm = new SetRecMult(s1, s2, !ordered);
                    _set.dispSet(TP_SET_REC_MULT, srm);

                    CBAssignInfoTuple *ain = new CBAssignInfoTuple(ai1, ai2);
                    if (ai1 != _assignInfo)
                        delete ai1;
                    if (ai2 != cbhIter->_assignInfo)
                        delete ai2;

                    delete _assignInfo;
                    _assignInfo = ain;

                    return true;
                }
            }
        }

        return false;
    }



    /*********** CodeBlockIteration **********/

    /**
     * destructor
     */
    CodeBlockIteration::~CodeBlockIteration()
    {
        if (_result)
            delete _result;

        // don't delete _assignInfo here, because it is deleted with the CBHeaderIterInfo object which created this CodeBlockIteration object
        _assignInfo = NULL;
        _set.dispose();

        if (_excModule)
            delete _excModule;
        if (_excFatal)
            delete _excFatal;
    }


    /**
     * execute iteration for a code block part
     * @return		result array / NULL: no result
     */
    CmplArray *CodeBlockIteration::run()
    {
        try {
            if (_cbContext->_cancel)
                return NULL;

            _nextHead = _headNum + 1;
            if (_nextHead >= (int)(_cbContext->_headers.size()))
                _nextHead = -1;

            _result = NULL;

            // only iterate if set is not empty
            unsigned long cnt = SetBase::cnt(_set);
            if (cnt > 0) {
                ThreadHandler& thdh = _execCtx->modp()->threadHandler();
                bool async = (cnt > 1 && !_cbContext->ordered() && thdh.hasThreading());

                if (async) {
                    LockGuard<mutex> lck(true, _thdInfoMtx);
                    _thdInfoSize = min((unsigned long)(thdh.maxThreads()), cnt);
                    _thdInfo = new CBThreadInfo[_thdInfoSize];

                    // decrement count of running worker threads, because this thread acts now as control thread
                    thdh.decThread();
                }

                // iterate over the set
                SetIterator iter(_set, SetIterator::iteratorTupleSimple, _cbContext->ordered(), false);
                for (iter.begin(); iter; iter++) {
                    if (_cbContext->_cancel)
                        break;

                    if (async) {
                        CBThreadInfo::start(this, iter);
                    }
                    else {
                        execIter(NULL, _execCtx, *iter, iter.curIndex());
                    }
                }

                if (async) {
                    // wait for all started worker threads
                    CBThreadInfo::joinAll(this);

                    LockGuard<mutex> lck(true, _thdInfoMtx);
                    delete[] _thdInfo;
                    _thdInfo = NULL;
                }
            }
        }
        catch (exception& ex) {
            if (_thdInfo) {
                LockGuard<mutex> lck(true, _thdInfoMtx);
                delete[] _thdInfo;
                _thdInfo = NULL;
            }
            throw;
        }

        // all threads are ended, so discarding volatile is safe
        CmplArray *r = const_cast<CmplArray *>(_result);
        _result = NULL;

        // if execution is cancelled then discard result
        if (_cbContext->_cancel || _excModule || _excFatal)
            CmplObjBase::dispose(r);

        // rethrow exception
        if (_excModule || _excFatal) {
            if (_excFatal) {
                FatalException *e = const_cast<FatalException *>(_excFatal);
                throw FatalException(*e);
            }
            else {
                ModuleException *e = const_cast<ModuleException *>(_excModule);
                throw ModuleException(*e);
            }
        }

        return r;
    }


    /**
     * execution for one iteration step
     * @param thd               thread info object / NULL: no threading
     * @param ctx               execution context
     * @param curIter           current element of iteration
     * @param curInd            current index number of iteration
     * @return                  symbol number (fetch id) of first uninitialized local symbol
     */
    void CodeBlockIteration::execIter(CBThreadInfo *thd, ExecContext *ctx, const CmplVal& curIter, unsigned long curInd)
    {
        _assignInfo->setCBSymbolValues(ctx, curIter, curInd);
        ctx->setIterTplPart(this, curIter);

        execElem(thd, ctx, _nextHead, curInd);

        // reset execution context
        ctx->delIterTplPart(this);
        unsigned cbsFrom = (_headNum == -1 ? _cbContext->_localSymFrom : _assignInfo->numCBSymbolsFrom(ctx));
        ctx->uninitCBSymbols(cbsFrom, _cbContext->_localSymTo, false);

        ctx->setCurNmPrefCB(_cbContext);
    }


    /**
     * execution for one iteration element
     * @param thd               thread info object / NULL: no threading
     * @param ctx               execution context
     * @param nextHead          number of next header in the codeblock context / -1: no next header
     * @param curInd            current index number of iteration
     */
    void CodeBlockIteration::execElem(CBThreadInfo *thd, ExecContext *ctx, int nextHead, unsigned long curInd)
    {
        if (_cbContext->_cancel)
            return;

        if (nextHead >= 0) {
            // recursive call for next header
            CodeBlockHeaderBase *head = _cbContext->_headers[nextHead];
            int nextNextHead = (nextHead + 1 < (int)(_cbContext->_headers.size()) ? nextHead + 1 : -1);

            CmplVal cbhRes;
            head->getResult(ctx, cbhRes);

            if (cbhRes.t == TP_BIN) {
                if (cbhRes.v.i)
                    execElem(thd, ctx, nextNextHead, curInd);
            }
            else if (cbhRes.t == TP_FORMULA) {
                if (_cbContext->_lastPart) {
                    //TODO? -> hier immer Fehler wegen Mix mit Iteration?
                    //  oder Ausfuehrung auch hier, wenn spaet ausgewerteter Header auch ohne Iteration?
                    //      dann: Bedingung cbhRes im Codeblockkontext hinzufuegen

                    // rekursiv aufrufen
                    execElem(thd, ctx, nextNextHead, curInd);

                    // effektive Bedingung im Codeblockkontext wieder auf vorherigen Wert zurück
                    //  -> Ruecksetzen sollte nie notwendig sein, weil keine Iteration?
                }
                else {
                    //TODO: Fehlerbehandlung anpassen, Mix mit Iteration generell verboten
                    ctx->valueError("formula with optimization variable not allowed in codeblock header together with an iteration header if another codeblock part follows", cbhRes, head->syntaxElem(ctx));
                }
            }
            else if (cbhRes.t == TP_CBHEAD_ITER) {
                CodeBlockIteration nextIter(ctx, _cbContext, (CBHeaderIterInfo *)(cbhRes.v.p), nextHead);

                if (thd)
                    thd->setNextIter(&nextIter);
                else
                    setNextIter(&nextIter);

                CmplArray *r = nextIter.run();
                if (r)
                    setResult(curInd, r);

                if (thd)
                    thd->setNextIter(NULL);
                else
                    setNextIter(NULL);
            }
            else {
                ctx->internalError("wrong result type for codeblock header");
            }
        }
        else {
            // execution of the codeblock body
            CmplVal cbbRes;
            ctx->run(_cbContext->_curBodyCode, false, &cbbRes);
            _cbContext->_execPart = _cbContext->_curPart;

            if (!cbbRes.isEmpty())
                setResult(curInd, cbbRes);
        }
    }


    /**
     * set info for next iteration object
     * @param ni                next iteration object
     */
    void CodeBlockIteration::setNextIter(CodeBlockIteration *ni)
    {
        LockGuard<mutex> lck(true, _thdInfoMtx);
        _nextIter = ni;
    }


    /**
     * set and propagate cancel flag
     * @param cmd		control command (one of ICS_CBC_BREAK, ICS_CBC_CONTINUE, ICS_CBC_REPEAT)
     * @param dstLvl	nesting codeblock level of destination codeblock of the control command
     */
    void CodeBlockIteration::setCancel(int cmd, unsigned dstLvl)
    {
        LockGuard<mutex> lck(true, _thdInfoMtx);

        if (_thdInfo) {
            CBThreadInfo::setCancel(this, cmd, dstLvl);
        }
        else if (_nextIter) {
            _nextIter->setCancel(cmd, dstLvl);
        }
    }


    /**
     * set exception thrown by separate thread
     * @param ex                exception
     */
    void CodeBlockIteration::setException(ModuleException& ex)
    {
        LockGuard<mutex>(true, _thdInfoMtx);
        if (!_excModule && !_excFatal)
            _excModule = new ModuleException(ex);

        try {
            _cbContext->setCancel(ICS_CBC_BREAK, 0);
        }
        catch (...) { }
    }

    /**
     * set exception thrown by separate thread
     * @param ex                exception
     */
    void CodeBlockIteration::setException(FatalException& ex)
    {
        LockGuard<mutex>(true, _thdInfoMtx);
        if (!_excFatal)
            _excFatal = new FatalException(ex);

        try {
            _cbContext->setCancel(ICS_CBC_BREAK, 0);
        }
        catch (...) { }
    }

    /**
     * set exception thrown by separate thread
     * @param ex                exception / NULL: unknown object thrown
     */
    void CodeBlockIteration::setException(exception *ex)
    {
        try {
            if (ex)
                _cbContext->_execContext->modp()->ctrl()->errHandler().internalError(ex->what());
            else
                _cbContext->_execContext->modp()->ctrl()->errHandler().internalError("unknown value thrown by separate thread");
        }
        catch (FatalException& exf)
        {
            setException(exf);
        }
    }



    /*********** CBThreadInfo **********/

    /**
     * destructor
     */
    CBThreadInfo::~CBThreadInfo()
    {
        if (_thread) {
            try {
                _thread->join();
                delete _thread;
            } catch (...) { }
        }

        if (_nextIterMtx)
            delete _nextIterMtx;

        if (_execCtx) {
            _execCtx->disposeIterCopy();
            delete _execCtx;
        }

        _curIter.dispose();
    }


    /**
     * searches unused entry in array <code>cbIter->_thdInfo</code> and starts a new thread
     * @param cbIter        object for iteration in a codeblock
     * @param iter          iterator object, iterated to the current iteration step
     */
    void CBThreadInfo::start(CodeBlockIteration *cbIter, SetIterator& iter)
    {
        CBThreadInfo *thdInfo = NULL;
        bool decAfter = false;
        ThreadHandler& thdh = cbIter->_execCtx->modp()->threadHandler();

        try {
            while (!thdInfo) {
                if (cbIter->_cbContext->_cancel || cbIter->_excModule || cbIter->_excFatal)
                    return;

                // increment the count of worker threads (wait, if max count of worker threads is reached)
                if (!thdh.waitIncThread(&(cbIter->_cbContext->_cancel)))
                    return;

                // search for info object not currently in use
                unsigned i;
                CBThreadInfo *t = cbIter->_thdInfo;
                for (i = 0; i < cbIter->_thdInfoSize; i++, t++) {
                    if (!t->_used) {
                        thdInfo = t;
                        break;
                    }
                }

                if (thdInfo)
                    break;

                // don't start a new thread now here, but elsewhere in the tree of threads
                thdh.decThread();
                this_thread::yield();
            }

            // even if the info object is no longer in use the thread may not be ended; wait for the end
            thdInfo->join();

            // decrement count of worker threads after execution only when not in the last iteration step
            unsigned long curInd = iter.curIndex();
            decAfter = (curInd < iter.count() - 1);

            PROTO_MOD_OUTL(cbIter->_execCtx->modp(), "start new thread for iteration " << cbIter->_cbContext->_lvl << "." << cbIter->_headNum << "." << curInd << ", using thread #" << (thdInfo - cbIter->_thdInfo));
            thdInfo->_used = true;
            thdInfo->_curIter.copyFrom(*iter, true, true);
            thdInfo->_thread = new thread(CBThreadInfo::execThread, thdInfo, cbIter, curInd, decAfter);
        }
        catch (...) {
            if (thdInfo) {
                thdInfo->_used = false;
                thdInfo->_curIter.dispUnset();

                if (thdInfo->_thread) {
                    try {
                        thdInfo->_thread->join();
                    } catch (...) { }

                    delete thdInfo->_thread;
                }

                thdInfo->_thread = NULL;
            }

            if (decAfter)
                thdh.decThread();

            throw;
        }
    }

    /**
     * main function for a new thread, executed by the thread <code>thdInfo->_thread</code>
     * @param thdInfo       info object for threading
     * @param cbIter        object for iteration in a codeblock
     * @param curInd        current index number of iteration
     * @param decAfter      decrement count of worker threads at the end of this function
     */
    void CBThreadInfo::execThread(CBThreadInfo *thdInfo, CodeBlockIteration *cbIter, unsigned long curInd, bool decAfter)
    {
        try {
            PROTO_MOD_OUTL(cbIter->_execCtx->modp(), "thread #" << (thdInfo - cbIter->_thdInfo) << " started, for iteration " << cbIter->_cbContext->_lvl << "." << cbIter->_headNum << "." << curInd);

            if (!cbIter->_cbContext->_cancel) {
                // use existing execution context for this thread, otherwise initialize a new one
                if (!thdInfo->_execCtx) {
                    unsigned lsdt = (cbIter->_headNum == -1 ? cbIter->_cbContext->_localSymFrom : cbIter->_assignInfo->numCBSymbolsFrom(cbIter->_execCtx));
                    thdInfo->_execCtx = cbIter->_execCtx->newAsIterCopy(lsdt);
                }
                else {
                    thdInfo->_execCtx->reinitIterCopy();
                }

                // execution of the iteration step
                cbIter->execIter(thdInfo, thdInfo->_execCtx, thdInfo->_curIter, curInd);
            }

            PROTO_MOD_OUTL(cbIter->_execCtx->modp(), "thread #" << (thdInfo - cbIter->_thdInfo) << " ended, for iteration " << cbIter->_cbContext->_lvl << "." << cbIter->_headNum << "." << curInd);
        }
        catch (ModuleException& ex) {
            cbIter->setException(ex);
            PROTO_MOD_OUTL(cbIter->_execCtx->modp(), "error (module exception) in thread #" << (thdInfo - cbIter->_thdInfo) << " for iteration " << cbIter->_cbContext->_lvl << "." << cbIter->_headNum << "." << curInd);
        }
        catch (FatalException& ex) {
            cbIter->setException(ex);
            PROTO_MOD_OUTL(cbIter->_execCtx->modp(), "error (module exception) in thread #" << (thdInfo - cbIter->_thdInfo) << " for iteration " << cbIter->_cbContext->_lvl << "." << cbIter->_headNum << "." << curInd);
        }
        catch (exception& ex) {
            cbIter->setException(&ex);
            PROTO_MOD_OUTL(cbIter->_execCtx->modp(), "error (other execption) in thread #" << (thdInfo - cbIter->_thdInfo) << " for iteration " << cbIter->_cbContext->_lvl << "." << cbIter->_headNum << "." << curInd);
        }
        catch (...) {
            cbIter->setException(NULL);
            PROTO_MOD_OUTL(cbIter->_execCtx->modp(), "error (unknown) in thread #" << (thdInfo - cbIter->_thdInfo) << " for iteration " << cbIter->_cbContext->_lvl << "." << cbIter->_headNum << "." << curInd);
        }

        thdInfo->_used = false;
        thdInfo->_curIter.dispUnset();
        if (decAfter)
            cbIter->_execCtx->modp()->threadHandler().decThread();
    }

    /**
     * wait for ending of the started thread
     */
    void CBThreadInfo::join()
    {
        if (_thread) {
            try {
                _thread->join();
                delete _thread;
            }
            catch (...) { }
        }

        _thread = NULL;
        _used = false;
        _curIter.dispUnset();
    }

    /**
     * wait for ending of all threads started for a codeblock iteration
     * @param cbIter        object for iteration in a codeblock
     */
    void CBThreadInfo::joinAll(CodeBlockIteration *cbIter)
    {
        CBThreadInfo *t = cbIter->_thdInfo;
        for (unsigned i = 0; i < cbIter->_thdInfoSize; i++, t++)
            t->join();
    }

    /**
     * set info for next iteration object
     * @param ni            next iteration object
     */
    void CBThreadInfo::setNextIter(CodeBlockIteration *ni)
    {
        if (!_nextIterMtx)
            _nextIterMtx = new mutex();

        LockGuard<mutex> lck(true, *_nextIterMtx);
        _nextIter = ni;
    }

    /**
     * set and propagate cancel flag
     * @param cbIter        object for iteration in a codeblock
     * @param cmd           control command (one of ICS_CBC_BREAK, ICS_CBC_CONTINUE, ICS_CBC_REPEAT)
     * @param dstLvl        nesting codeblock level of destination codeblock of the control command
     */
    void CBThreadInfo::setCancel(CodeBlockIteration *cbIter, int cmd, unsigned dstLvl)
    {
        CBThreadInfo *t = cbIter->_thdInfo;
        for (unsigned i = 0; i < cbIter->_thdInfoSize; i++, t++) {
            if (t->_execCtx)
                t->_execCtx->setCancel(cmd, dstLvl);

            if (t->_nextIter) {
                // if _nextIter is set, then also _nextIterMtx exists
                LockGuard<mutex> lck(true, *(t->_nextIterMtx));
                if (t->_nextIter) {
                    // discarding volatile is safe because in lock
                    const_cast<CodeBlockIteration *>(t->_nextIter)->setCancel(cmd, dstLvl);
                }
            }
        }
    }

}

