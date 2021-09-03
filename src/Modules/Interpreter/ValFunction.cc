
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


#include "ValFunction.hh"
#include "Interpreter.hh"
#include "Interval.hh"
#include "TupleUtil.hh"
#include "SetUtil.hh"
#include "OperationBase.hh"
#include "../../Control/MainControl.hh"



namespace cmpl
{

    /****** ValFunctionBase ****/

    /**
     * calls function for every single element of an array or a list (result is array over the same definition set as argument value)
     * @param ctx			execution context
     * @param res           return of result value
     * @param arg			pointer to argument value
     * @param info          info object for use by the caller
     */
    void ValFunctionBase::callForArrayElements(ExecContext *ctx, CmplVal& res, StackValue *arg, void *info)
    {
        if (arg->isList())
            arg = ctx->replaceListOnStack(arg);

        CmplVal *s = arg->simpleValue();
        if (s) {
            operCallSimple(ctx, res, *s, false, arg->syntaxElem(), info);
        }
        else if (arg->val().t == TP_ARRAY) {
            CmplArray *arr = new CmplArray(arg->val().array()->defset());
            arr->copyValidInfo(arg->val().array());

            CmplVal *r = arr->at(0);
            s = arg->val().array()->at(0);

            for (unsigned long i = 0; i < arr->size(); i++, r++, s++)
                operCallSimple(ctx, *r, *s, false, arg->syntaxElem(), info);

            res.set(TP_ARRAY, arr);
        }
        else {
            ctx->valueError(ctx->modp()->ctrl()->printBuffer("function '%s' not implemented for value type", funcName()), arg);
            res.set(TP_NULL);
        }
    }


    /**
     * recursive call for aggregating the elements of an array or a list
     * @param ctx			execution context
     * @param res           return of result value
     * @param arg			pointer to argument value
     * @param info          info object for use by the caller
     */
    void ValFunctionBase::callForArrayAggrRek(ExecContext *ctx, CmplVal &res, StackValue *arg, void *info)
    {
        if (arg->isList()) {
            if (arg->val().t == TP_LIST_TUPLE) {
                CmplValAuto tp;
                TupleUtil::constructFromList(ctx, tp, arg);
                operCallSimple(ctx, res, tp, false, arg->syntaxElem(), info);
            }
            else {
                while (arg->val().t == TP_REF_LIST)
                    arg = (StackValue *)(arg->val().v.p);

                unsigned long cnt = ExecContext::stackListTotalCnt(arg) - 1;
                for (unsigned long i = cnt; i > 0; i--) {
                    StackValue *sv = arg - i;
                    CmplVal *s = sv->simpleValue();
                    if (s && !s->isList()) {
                        operCallSimple(ctx, res, *s, true, arg->syntaxElem(), info);
                    }
                    else if (sv->val().t == TP_ARRAY || sv->val().t == TP_REF_LIST) {
                        callForArrayAggrRek(ctx, res, sv, info);
                    }
                }
            }
        }
        else {
            CmplVal *s = arg->simpleValue();
            if (s) {
                operCallSimple(ctx, res, *s, false, arg->syntaxElem(), info);
            }
            else if (arg->val().t == TP_ARRAY) {
                CmplArray *arr = arg->val().array();
                if (arr->hasUserOrder()) {
                    CmplArrayIterator iter(*arr, false, true, false);
                    for (iter.begin(); iter; iter++) {
                        if (operCallSimple(ctx, res, **iter, true, arg->syntaxElem(), info))
                            break;
                    }
                }
                else {
                    s = arr->at(0);
                    for (unsigned long i = 0; i < arr->size(); i++, s++) {
                        if (operCallSimple(ctx, res, *s, true, arg->syntaxElem(), info))
                            break;
                    }
                }
            }
            else {
                ctx->valueError(ctx->modp()->ctrl()->printBuffer("function '%s' not implemented for value type", funcName()), arg);
            }
        }
    }

    /**
     * set a value within a result array
     * @param ctx           execution context
     * @param res           result array in which the value is to set (is not necessary an array in input)
     * @param tp1           first tuple part for the value
     * @param tp2           second tuple part for the value
     * @param val           value
     * @param se            syntax element id
     */
    void ValFunctionBase::setInResultArray(ExecContext *ctx, CmplVal &res, const CmplVal &tp1, const CmplVal &tp2, const CmplVal &val, unsigned se)
    {
        if (val.t == TP_NULL || (val.isArray() && val.array()->size() == 0))
            return;

        CmplValAuto tp;
        if (!tp1 || tp1.t == TP_ITUPLE_NULL)
            tp.copyFrom(tp2);
        else if (!tp2 || tp2.t == TP_ITUPLE_NULL)
            tp.copyFrom(tp1);
        else
            Tuple::constructTuple(tp, tp1, tp2);

        if (val.isArray()) {
            CmplArray *ra = val.array();
            CmplArrayIterator iter(*ra);
            for (iter.begin(); iter; iter++) {
                CmplVal *rv = *iter;
                setInResultArray(ctx, res, tp, iter.curTuple(), *rv, se);
            }
        }

        else {
            if (res.t == TP_NULL || !res.isArray()) {
                if (res.t == TP_NULL) {
                    res.set(TP_ARRAY, new CmplArray());
                }
                else {
                    CmplVal ds(TP_SET_NULL);
                    CmplVal nr(TP_ARRAY, new CmplArray(ds, res));
                    res.moveFrom(nr, true);
                }
            }

            CmplArray *arr = res.array();
            unsigned long ind;
            if (SetUtil::tupleInSet(ctx, arr->defset(), tp, ind)) {
                arr->at(ind)->copyFrom(val);
            }
            else {
                CmplValAuto nds, v;
                ind = SetUtil::addTupleToSet(ctx, nds, arr->defset(), tp, false, true);
                v.copyFrom(val);
                arr->insertValues(nds, ind, &v);
            }
        }
    }



    /****** ValFunctionUser ****/

    /**
     * calls the cmpl function call operation and store result in execution context, if this value has a special handling for it
     * @param ctx			execution context
     * @param arg			pointer to argument value
     * @return				true if this has handled the operation
     */
    bool ValFunctionUser::operCall(ExecContext *ctx, StackValue *arg)
    {
        // function argument
        StackValue *fctArg = arg;
        if (arg->isList() && arg->val().t != TP_REF_LIST)
        {
            CmplVal rl;
            rl.setP(TP_REF_LIST, arg);
            ctx->pushVal(rl, arg->syntaxElem());
            fctArg = ctx->stackCur();
        }

        // run function in own execution context
        ExecContext fctExec(ctx->modp(), ctx, true, fctArg);

        CmplVal res;
        fctExec.run(_startPos, true, &res);

        ctx->opResult().moveFrom(res);
        return true;
    }



	/****** ValFunctionInternSum ****/

    /**
     * calls the function for a simple source value (no array or list)
     * @param ctx			execution context
     * @param res			store for result value
     * @param src			source value
     * @param aggr          called for aggregating elements of an array or a list
     * @param se			syntax element id of source value
     * @param info          info object for use by the caller
     * @return              only used if aggr: true if result is final
     */
    bool ValFunctionInternSum::operCallSimple(ExecContext *ctx, CmplVal& res, CmplVal& src, bool aggr, unsigned se, void *info)
    {
        if (!res || res.t == TP_NULL) {
            res.copyFrom(src, true, false);
        }
        else {
            CmplValAuto a1;
            a1.moveFrom(res, false);
            OperationBase::plus(ctx, &res, se, &a1, &src);
        }

        return false;
    }


	/****** ValFunctionInternMax ****/

    /**
     * calls the function for a simple source value (no array or list)
     * @param ctx			execution context
     * @param res			store for result value
     * @param src			source value
     * @param aggr          called for aggregating elements of an array or a list
     * @param se			syntax element id of source value
     * @param info          info object for use by the caller
     * @return              only used if aggr: true if result is final
     */
    bool ValFunctionInternMax::operCallSimple(ExecContext *ctx, CmplVal& res, CmplVal& src, bool aggr, unsigned se, void *info)
    {
        if (src && src.t != TP_NULL) {
            CmplVal srcv;
            if (src.isScalarNumber()) {
                srcv.copyFrom(src);
            }
            else if (src.isInterval()) {
                Interval::uppBoundVal(srcv, src);
            }
            else if (src.isSet() && SET_VAL_WO_ORDER(src).rank1()) {
                CmplVal d;
                SetUtil::rank1IntBounds(d, srcv, src);
            }

            if ((!res || res.t == TP_NULL) && srcv.isScalarNumber()) {
                res.copyFrom(srcv, true, false);
            }
            else if (srcv.isScalarNumber() && res.isScalarNumber()) {
                if (res.t == TP_INFINITE || srcv.t == TP_INFINITE) {
                    if ((res.t == TP_INFINITE && res.v.i > 0) || (srcv.t == TP_INFINITE && srcv.v.i < 0)) {
                        return (res.t == TP_INFINITE && res.v.i > 0);
                    }
                    else if ((srcv.t == TP_INFINITE && srcv.v.i > 0) || (res.t == TP_INFINITE && res.v.i < 0)) {
                        res.copyFrom(srcv);
                        return (srcv.t == TP_INFINITE && srcv.v.i > 0);
                    }
                }
                else if (srcv.isScalarInt() && res.isScalarInt()) {
                    if (srcv.v.i > res.v.i)
                        res.copyFrom(srcv);
                }
                else {
                    if (srcv.numAsReal() > res.numAsReal())
                        res.copyFrom(srcv);
                }

            }
            else {
                ctx->valueError(ctx->modp()->ctrl()->printBuffer("invalid argument for function '%s'", funcName()), src, se);
                return true;
            }
        }

        return false;
    }


	/****** ValFunctionInternMin ****/

    /**
     * calls the function for a simple source value (no array or list)
     * @param ctx			execution context
     * @param res			store for result value
     * @param src			source value
     * @param aggr          called for aggregating elements of an array or a list
     * @param se			syntax element id of source value
     * @param info          info object for use by the caller
     * @return              only used if aggr: true if result is final
     */
    bool ValFunctionInternMin::operCallSimple(ExecContext *ctx, CmplVal& res, CmplVal& src, bool aggr, unsigned se, void *info)
    {
        if (src && src.t != TP_NULL) {
            CmplVal srcv;
            if (src.isScalarNumber()) {
                srcv.copyFrom(src);
            }
            else if (src.isInterval()) {
                Interval::lowBoundVal(srcv, src);
            }
            else if (src.isSet() && SET_VAL_WO_ORDER(src).rank1()) {
                CmplVal d;
                SetUtil::rank1IntBounds(srcv, d, src);
            }

            if ((!res || res.t == TP_NULL) && srcv.isScalarNumber()) {
                res.copyFrom(srcv, true, false);
            }
            else if (srcv.isScalarNumber() && res.isScalarNumber()) {
                if (res.t == TP_INFINITE || srcv.t == TP_INFINITE) {
                    if ((res.t == TP_INFINITE && res.v.i < 0) || (srcv.t == TP_INFINITE && srcv.v.i > 0)) {
                        return (res.t == TP_INFINITE && res.v.i < 0);
                    }
                    else if ((srcv.t == TP_INFINITE && srcv.v.i < 0) || (res.t == TP_INFINITE && res.v.i > 0)) {
                        res.copyFrom(srcv);
                        return (srcv.t == TP_INFINITE && srcv.v.i < 0);
                    }
                }
                else if (srcv.isScalarInt() && res.isScalarInt()) {
                    if (srcv.v.i < res.v.i)
                        res.copyFrom(srcv);
                }
                else {
                    if (srcv.numAsReal() < res.numAsReal())
                        res.copyFrom(srcv);
                }

            }
            else {
                ctx->valueError(ctx->modp()->ctrl()->printBuffer("invalid argument for function '%s'", funcName()), src, se);
                return true;
            }
        }

        return false;
    }


    /****** ValFunctionInternAnd ****/

    /**
     * calls the cmpl function call operation and store result in execution context, if this value has a special handling for it
     * @param ctx			execution context
     * @param arg			pointer to argument value
     * @return				true
     */
    bool ValFunctionInternAnd::operCall(ExecContext *ctx, StackValue *arg)
    {
        CmplVal ea(TP_NULL);
        CmplVal *v = arg->simpleValueOrEmptyArr(&ea);

        if (v && v->t == TP_NULL)
            ctx->opResult().set(TP_BIN, true);
        else
            callForArrayAggr(ctx, ctx->opResult(), arg);

        return true;
    }

    /**
     * calls the function for a simple source value (no array or list)
     * @param ctx			execution context
     * @param res			store for result value
     * @param src			source value
     * @param aggr          called for aggregating elements of an array or a list
     * @param se			syntax element id of source value
     * @param info          info object for use by the caller
     * @return              only used if aggr: true if result is final
     */
    bool ValFunctionInternAnd::operCallSimple(ExecContext *ctx, CmplVal& res, CmplVal& src, bool aggr, unsigned se, void *info)
    {
        if (src && src.t != TP_NULL) {
            if (!res || res.t == TP_NULL) {
                bool sb;
                if ((src.t == TP_FORMULA && src.valFormula()->isBool()) || (src.t == TP_OPT_VAR && src.optVar()->binVar())) {
                    res.copyFrom(src);
                }
                else if (src.t != TP_FORMULA && src.t != TP_OPT_VAR && src.toBool(sb, typeConversionAll, ctx->modp())) {
                    res.set(TP_BIN, sb);
                }
                else {
                    ctx->valueError("argument must have a boolean value", src, se);
                    res.set(TP_BIN, false);
                }
            }
            else {
                CmplValAuto r;
                OperationBase::execBinaryOper(ctx, &r, se, ICS_OPER_AND, false, &res, &src);
                res.moveFrom(r, true);
            }
        }

        return (res && res.t == TP_BIN && res.v.i == 0);
    }


    /****** ValFunctionInternOr ****/

    /**
     * calls the cmpl function call operation and store result in execution context, if this value has a special handling for it
     * @param ctx			execution context
     * @param arg			pointer to argument value
     * @return				true
     */
    bool ValFunctionInternOr::operCall(ExecContext *ctx, StackValue *arg)
    {
        CmplVal ea(TP_NULL);
        CmplVal *v = arg->simpleValueOrEmptyArr(&ea);

        if (v && v->t == TP_NULL)
            ctx->opResult().set(TP_BIN, false);
        else
            callForArrayAggr(ctx, ctx->opResult(), arg);

        return true;
    }

    /**
     * calls the function for a simple source value (no array or list)
     * @param ctx			execution context
     * @param res			store for result value
     * @param src			source value
     * @param aggr          called for aggregating elements of an array or a list
     * @param se			syntax element id of source value
     * @param info          info object for use by the caller
     * @return              only used if aggr: true if result is final
     */
    bool ValFunctionInternOr::operCallSimple(ExecContext *ctx, CmplVal& res, CmplVal& src, bool aggr, unsigned se, void *info)
    {
        if (src && src.t != TP_NULL) {
            if (!res || res.t == TP_NULL) {
                bool sb;
                if ((src.t == TP_FORMULA && src.valFormula()->isBool()) || (src.t == TP_OPT_VAR && src.optVar()->binVar()) || src.t == TP_DATA_TYPE) {
                    res.copyFrom(src);
                }
                else if (src.t != TP_FORMULA && src.t != TP_OPT_VAR && src.t != TP_DATA_TYPE && src.toBool(sb, typeConversionAll, ctx->modp())) {
                    res.set(TP_BIN, sb);
                }
                else {
                    ctx->valueError("argument must have a boolean value", src, se);
                    res.set(TP_BIN, true);
                }
            }
            else {
                CmplValAuto r;
                OperationBase::execBinaryOper(ctx, &r, se, ICS_OPER_OR, false, &res, &src);
                res.moveFrom(r, true);
            }
        }

        return (res && res.t == TP_BIN && res.v.i == 1);
    }


	/****** ValFunctionInternCount ****/

	/**
	 * calls the cmpl function call operation and store result in execution context, if this value has a special handling for it
	 * @param ctx			execution context
	 * @param arg			pointer to argument value
	 * @return				true
	 */
	bool ValFunctionInternCount::operCall(ExecContext *ctx, StackValue *arg)
	{
        //TODO: Für besser Performance Liste direkt durchgehen, statt erst durch Array zu ersetzen
        if (arg->isList())
            arg = ctx->replaceListOnStack(arg);

        CmplVal *va = arg->simpleValue();
        if (va) {
            ctx->opResult().set(TP_INT, (intType)(va->isEmpty() ? 0 : 1));
        }
        else if (arg->val().t == TP_ARRAY) {
            CmplArray *arr = arg->val().array();
            if (arr->allValid()) {
                ctx->opResult().set(TP_INT, (intType)(arg->val().array()->size()));
            }
            else {
                CmplValAuto vs;
                SetUtil::getArrayValidSet(ctx, vs, arr);
                ctx->opResult().set(TP_INT, (intType)(SetBase::cnt(vs)));
            }
        }
        else {
            ctx->valueError(ctx->modp()->ctrl()->printBuffer("function '%s' not implemented for value type", funcName()), arg);
            ctx->opResult().set(TP_INT, (intType)0);
        }

        return true;
	}


	/****** ValFunctionInternDef ****/

	/**
	 * calls the cmpl function call operation and store result in execution context, if this value has a special handling for it
	 * @param ctx			execution context
	 * @param arg			pointer to argument value
	 * @return				true
	 */
	bool ValFunctionInternDef::operCall(ExecContext *ctx, StackValue *arg)
	{
        //TODO: Für besser Performance Liste direkt durchgehen, statt erst durch Array zu ersetzen
        if (arg->isList())
            arg = ctx->replaceListOnStack(arg);

        if (arg->simpleValue()) {
            ctx->opResult().set(TP_INT, (intType)1);
        }
        else if (arg->val().t == TP_ARRAY) {
            ctx->opResult().set(TP_INT, (intType)(arg->val().array()->size()));
        }
        else {
            ctx->valueError(ctx->modp()->ctrl()->printBuffer("function '%s' not implemented for value type", funcName()), arg);
            ctx->opResult().set(TP_INT, (intType)0);
        }

        return true;
	}


	/****** ValFunctionInternValid ****/

	/**
	 * calls the cmpl function call operation and store result in execution context, if this value has a special handling for it
	 * @param ctx			execution context
	 * @param arg			pointer to argument value
	 * @return				true
	 */
	bool ValFunctionInternValid::operCall(ExecContext *ctx, StackValue *arg)
	{
        //TODO: Für besser Performance Liste direkt durchgehen, statt erst durch Array zu ersetzen
        if (arg->isList())
            arg = ctx->replaceListOnStack(arg);

        CmplVal *va = arg->simpleValue();
        if (va) {
            ctx->opResult().set(TP_BIN, !(va->isEmpty()));
        }
        else if (arg->val().t == TP_ARRAY) {
            CmplArray *arr = arg->val().array();
            if (arr->hasValidInfo()) {
                ctx->opResult().set(TP_BIN, arr->allValid());
            }
            else {
                CmplValAuto vs;
                SetUtil::getArrayValidSet(ctx, vs, arr);
                ctx->opResult().set(TP_BIN, (SetBase::cnt(vs) == arr->size()));
            }
        }
        else {
            ctx->valueError(ctx->modp()->ctrl()->printBuffer("function '%s' not implemented for value type", funcName()), arg);
            ctx->opResult().set(TP_BIN, false);
        }

        return true;
	}


	/****** ValFunctionInternDim ****/

	/**
	 * calls the cmpl function call operation and store result in execution context, if this value has a special handling for it
	 * @param ctx			execution context
	 * @param arg			pointer to argument value
	 * @return				true
	 */
	bool ValFunctionInternDim::operCall(ExecContext *ctx, StackValue *arg)
	{
        if (arg->isList())
            arg = ctx->replaceListOnStack(arg);

        if (arg->simpleValue()) {
            ctx->opResult().set(TP_INT, (intType)0);
        }
        else if (arg->val().t == TP_ARRAY) {
            // get last tuple of definition set, use first element of this tuple
            CmplVal& ds = arg->val().array()->defset();
            if (SetBase::cnt(ds) == 0) {
                ctx->opResult().set(TP_INT, (intType)0);
            }
            else {
                CmplValAuto lt;
                SetIterator::at(lt, ds, 0, SetIterator::iteratorTupleSimple, false, true);
                if (lt.isITuple())
                    ctx->opResult().copyFrom(Tuple::at(lt, 0), true, false);
                else
                    ctx->opResult().copyFrom(lt, true, false);
            }
        }
        else {
            ctx->valueError(ctx->modp()->ctrl()->printBuffer("function '%s' not implemented for value type", funcName()), arg);
            ctx->opResult().set(TP_INT, (intType)0);
        }
		
		return true;
	}


	/****** ValFunctionInternDefset ****/

	/**
	 * calls the cmpl function call operation and store result in execution context, if this value has a special handling for it
	 * @param ctx			execution context
	 * @param arg			pointer to argument value
	 * @return				true
	 */
	bool ValFunctionInternDefset::operCall(ExecContext *ctx, StackValue *arg)
	{
        if (arg->isList())
            arg = ctx->replaceListOnStack(arg);

        if (arg->simpleValue()) {
            ctx->opResult().set(TP_SET_NULL);
        }
        else if (arg->val().isArray()) {
            CmplArray *arr = arg->val().array();
            if (arg->val().t == TP_ARRAY_COMP)
                arr = StackValue::arrayFromArrayComp(ctx, arr);     //TODO: Bestimmung volles Array hier nicht notwendig, defSet reicht

            ctx->opResult().copyFrom(arr->defset(), true, false);
        }
        else {
            ctx->valueError(ctx->modp()->ctrl()->printBuffer("function '%s' not implemented for value type", funcName()), arg);
            ctx->opResult().set(TP_NULL);
        }
		
		return true;
	}


	/****** ValFunctionInternValidset ****/

	/**
	 * calls the cmpl function call operation and store result in execution context, if this value has a special handling for it
	 * @param ctx			execution context
	 * @param arg			pointer to argument value
	 * @return				true
	 */
	bool ValFunctionInternValidset::operCall(ExecContext *ctx, StackValue *arg)
	{
        if (arg->isList())
            arg = ctx->replaceListOnStack(arg);

        if (arg->simpleValue()) {
            ctx->opResult().set(arg->simpleValue()->t != TP_EMPTY ? TP_SET_NULL : TP_SET_EMPTY);
        }
        else if (arg->val().isArray()) {
            CmplArray *arr = arg->val().array();
            if (arg->val().t == TP_ARRAY_COMP)
                arr = StackValue::arrayFromArrayComp(ctx, arr);        //TODO: Bestimmung volles Array hier nicht notwendig, validSet reicht

            SetUtil::getArrayValidSet(ctx, ctx->opResult(), arr);
        }
        else {
            ctx->valueError(ctx->modp()->ctrl()->printBuffer("function '%s' not implemented for value type", funcName()), arg);
            ctx->opResult().set(TP_NULL);
        }

        return true;
    }


	/****** ValFunctionInternLen ****/

    /**
     * calls the function for a simple source value (no array or list)
     * @param ctx			execution context
     * @param res			store for result value
     * @param src			source value
     * @param aggr          called for aggregating elements of an array or a list
     * @param se			syntax element id of source value
     * @param info          info object for use by the caller
     * @return              only used if aggr: true if result is final
     */
    bool ValFunctionInternLen::operCallSimple(ExecContext *ctx, CmplVal& res, CmplVal& src, bool aggr, unsigned se, void *info)
	{
        if (src.isSet()) {
            if (src.isSetFin())
                res.set(TP_INT, (intType)SetBase::cnt(src));
            else
                res.set(TP_INFINITE, 1);
        }
        else if (src.isTuple()) {
            // tuple is handled as a set with one element
            res.set(TP_INT, 1);
        }
        else if (src.isInterval() && Interval::isSet(src)) {
            // if interval can used as a set, then give its len
            if (Interval::isEmpty(src))
                res.set(TP_INT, 0);
            else if (Interval::isSetFin(src))
                res.set(TP_INT, 1 + Interval::uppBoundInt(src) - Interval::lowBoundInt(src));
            else
                res.set(TP_INFINITE, 1);
        }
        else if (src.isScalarString()) {
            // function used for string len
            if (src.t == TP_STRINGP)
                res.set(TP_INT, (intType)(src.v.s->size()));
            else
                res.set(TP_INT, (intType)(strlen(src.stringAsCharP(ctx->modp()))));
        }
        else {
            ctx->valueError(ctx->modp()->ctrl()->printBuffer("invalid argument for function '%s'", funcName()), src, se);
            res.set(TP_INT, 0);
            return true;
        }

        return false;
	}


	/****** ValFunctionInternRank ****/

    /**
     * calls the function for a simple source value (no array or list)
     * @param ctx			execution context
     * @param res			store for result value
     * @param src			source value
     * @param aggr          called for aggregating elements of an array or a list
     * @param se			syntax element id of source value
     * @param info          info object for use by the caller
     * @return              only used if aggr: true if result is final
     */
    bool ValFunctionInternRank::operCallSimple(ExecContext *ctx, CmplVal& res, CmplVal& src, bool aggr, unsigned se, void *info)
    {
        if (src.isSet()) {
            unsigned l = SetBase::minRank(src);
            unsigned u = SetBase::maxRank(src, UINT_MAX);

            if (l == u) {
                res.set(TP_INT, (intType)l);
            }
            else {
                CmplVal lb(TP_INT, (intType)l);
                CmplVal ub;
                if (u == UINT_MAX)
                    ub.set(TP_INFINITE, 1);
                else
                    ub.set(TP_INT, (intType)u);

                Interval::construct(res, lb, ub);
            }
        }
        else if (src.isTuple()) {
            res.set(TP_INT, (intType)(Tuple::rank(src)));
        }
        else if (!src.isSpecial()) {
            res.set(TP_INT, 1);
        }
        else {
            ctx->valueError(ctx->modp()->ctrl()->printBuffer("invalid argument for function '%s'", funcName()), src, se);
            res.set(TP_INT, 0);
            return true;
        }

        return false;
	}


    /****** ValFunctionInternArray ****/

    /**
     * calls the cmpl function call operation and store result in execution context, if this value has a special handling for it
     * @param ctx			execution context
     * @param arg			pointer to argument value
     * @return				true
     */
    bool ValFunctionInternArray::operCall(ExecContext *ctx, StackValue *arg)
    {
        CmplVal& res = ctx->opResult();
        res.set(TP_NULL);

        if (arg->isList())
            arg = ctx->replaceListOnStack(arg);

        CmplVal *s = arg->simpleValue();
        if (s) {
            operCallSimple(ctx, res, *s, false, arg->syntaxElem());
        }
        else if (arg->val().t == TP_ARRAY) {
            CmplVal nulltp(TP_ITUPLE_NULL);
            CmplArray *arrsrc = arg->val().array();

            s = arrsrc->at(0);
            SetIterator iter(arrsrc->defset());
            iter.begin();

            for (unsigned long i = 0; i < arrsrc->size(); i++, s++, iter++) {
                CmplValAuto r;
                operCallSimple(ctx, r, *s, false, arg->syntaxElem());

                if (r.t != TP_NULL) {
                    const CmplVal& tp = *iter;
                    if (res.t == TP_NULL && tp.t == TP_ITUPLE_NULL)
                        res.moveFrom(r);
                    else
                        setInResultArray(ctx, res, tp, nulltp, r, arg->syntaxElem());
                }
            }

            if (res.isArray()) {
                CmplValAuto vs;
                SetUtil::getArrayValidSet(ctx, vs, res.array());
            }
        }
        else {
            ctx->valueError(ctx->modp()->ctrl()->printBuffer("function '%s' not implemented for value type", funcName()), arg);
            res.set(TP_NULL);
        }

        return true;
    }

    /**
     * calls the function for a simple source value (no array or list)
     * @param ctx			execution context
     * @param res			store for result value
     * @param src			source value
     * @param aggr          called for aggregating elements of an array or a list
     * @param se			syntax element id of source value
     * @param info          info object for use by the caller
     * @return              only used if aggr: true if result is final
     */
    bool ValFunctionInternArray::operCallSimple(ExecContext *ctx, CmplVal& res, CmplVal& src, bool aggr, unsigned se, void *info)
    {
        if (src.isSet() || src.isInterval()) {
            if (src.isSetInf() || (src.isInterval() && Interval::isInf(src))) {
                ctx->valueError(ctx->modp()->ctrl()->printBuffer("function '%s' cannot be used for infinite set or interval", funcName()), src, se);
                res.set(TP_NULL);
                return true;
            }

            CmplValAuto ss;
            if (src.isInterval())
                SetUtil::constructFromInterval(ctx, ss, src);
            else
                ss.copyFrom(src);

            unsigned long sz = SetBase::cnt(ss);
            CmplVal ds((sz == 0 ? TP_SET_EMPTY : (sz == 1 ? TP_SET_1INT : TP_SET_R1_1_UB)), (intType)sz);
            CmplArray *arr = new CmplArray(ds);
            res.set(TP_ARRAY, arr);

            if (sz) {
                CmplVal *ra = arr->at(0);
                SetIterator iter(ss, SetIterator::iteratorTupleVal, true, false);

                for (iter.begin(); iter; iter++, ra++)
                    ra->copyFrom(iter.curTuple());

                arr->setValidInfo(true);
            }
        }

        else if (src.isTuple()) {
            unsigned sz = Tuple::rank(src);
            bool av = true;

            CmplVal ds((sz == 0 ? TP_SET_EMPTY : (sz == 1 ? TP_SET_1INT : TP_SET_R1_1_UB)), (intType)sz);
            CmplArray *arr = new CmplArray(ds);
            res.set(TP_ARRAY, arr);

            if (sz) {
                CmplVal *ra = arr->at(0);
                for (unsigned i = 0; i < sz; i++, ra++) {
                    const CmplVal *v = Tuple::at(src, i);
                    ra->copyFrom(v);
                    if (v->t == TP_EMPTY)
                        av = false;
                }

                if (av) {
                    arr->setValidInfo(true);
                }
                else {
                    CmplValAuto vs;
                    arr->delValidInfo();
                    SetUtil::getArrayValidSet(ctx, vs, arr);
                }
            }
        }

        else {
            res.copyFrom(src);
        }

        return true;
    }


	/****** ValFunctionInternEcho ****/

    /**
     * calls the cmpl function call operation and store result in execution context, if this value has a special handling for it
     * @param ctx			execution context
     * @param arg			pointer to argument value
     * @return				true
     */
    bool ValFunctionInternEcho::operCall(ExecContext *ctx, StackValue *arg)
    {
        LockGlobalGuard(true, LockGlobalGuard::coutLock);

        if (ctx->modp()->echoDuration())
            cout << ctx->modp()->durStartApp().count() / 1000. << ": ";

        callForArrayAggr(ctx, ctx->opResult(), arg);
        cout << endl;

        ctx->opResult().set(TP_NULL);
        return true;
    }

    /**
     * calls the function for a simple source value (no array or list)
     * @param ctx			execution context
     * @param res			store for result value
     * @param src			source value
     * @param aggr          called for aggregating elements of an array or a list
     * @param se			syntax element id of source value
     * @param info          info object for use by the caller
     * @return              only used if aggr: true if result is final
     */
    bool ValFunctionInternEcho::operCallSimple(ExecContext *ctx, CmplVal& res, CmplVal& src, bool aggr, unsigned se, void *info)
    {
        if (!res || res.t == TP_NULL)
            res.set(TP_INT, (intType)0);
        else
            cout << ' ';

        CmplValAuto t;
        if (ctx->checkContainerConvSpecial(t, src, TP_STR))
            t.write(cout, ctx->modp());
        else
            src.write(cout, ctx->modp());

        return false;
    }

//	/**
//	 * calls the cmpl function call operation and store result in execution context, if this value has a special handling for it
//	 * @param ctx			execution context
//	 * @param arg			pointer to argument value
//	 * @return				true
//	 */
//    bool ValFunctionInternEcho::operCall(ExecContext *ctx, StackValue *arg)
//    {
//        CmplVal *v = arg->simpleValue();
//        if (v && !v->isList()) {
//            v->write(cout, ctx->modp());
//            cout << endl;
//        }
//        else if (arg->isList()) {
//            CmplVal &r = arg->val();
//            if (r.t == TP_LIST_TUPLE) {
//                CmplVal o;
//                TupleUtil::constructFromList(ctx, o, arg);
//                o.write(cout, ctx->modp());
//                cout << endl;
//                o.dispose();
//            }
//            else {
//                while (arg->val().t == TP_REF_LIST)
//                    arg = (StackValue *)(arg->val().v.p);

//                unsigned long cnt = ExecContext::stackListTotalCnt(arg) - 1;
//                for (unsigned long i = cnt; i > 0; i--) {
//                    CmplVal *a = arg[-i].simpleValue();
//                    if (a && !a->isList()) {
//                        a->write(cout, ctx->modp());
//                        cout << ' ';
//                    }
//                    else if (arg[-i].val().t == TP_ARRAY) {
//                        echoArraySimple(ctx, arg[-i].val().array());
//                    }
//                }
//                cout << endl;
//            }
//        }
//        else if (arg->val().t == TP_ARRAY) {
//            echoArraySimple(ctx, arg->val().array());
//            cout << endl;
//        }
//        else {
//            //TODO
//            ctx->valueError(ctx->modp()->ctrl()->printBuffer("function '%s' not implemented for value type", funcName()), arg);
//        }

//        ctx->opResult().set(TP_NULL);
//        return true;
//    }

//    /**
//     * simple array output
//     * @param ctx			execution context
//     * @param arr			array
//     */
//    void ValFunctionInternEcho::echoArraySimple(ExecContext *ctx, CmplArray *arr)
//    {
//        CmplArrayIterator iter(*arr, true, true);
//        for (iter.begin(); iter; iter++) {
//            CmplVal *v = *iter;
//            v->write(cout, ctx->modp());
//            cout << ' ';
//        }
//    }


	/****** ValFunctionInternError ****/

	/**
	 * calls the cmpl function call operation and store result in execution context, if this value has a special handling for it
	 * @param ctx			execution context
	 * @param arg			pointer to argument value
	 * @return				true
	 */
	bool ValFunctionInternError::operCall(ExecContext *ctx, StackValue *arg)
	{
		ctx->valueError("user error", arg, ERROR_LVL_FATAL);

		ctx->opResult().set(TP_NULL);
		return true;
	}


	/****** ValFunctionInternFormat ****/

	/**
	 * calls the cmpl function call operation and store result in execution context, if this value has a special handling for it
	 * @param ctx			execution context
	 * @param arg			pointer to argument value
	 * @return				true
	 */
	bool ValFunctionInternFormat::operCall(ExecContext *ctx, StackValue *arg)
	{
        string *rp = new string();
        ctx->opResult().set(TP_STRINGP, rp);

        Info inf(rp);
        callForArrayAggr(ctx, ctx->opResult(), arg, &inf);

        if (inf.formattp != TP_EMPTY || inf.nxtformat < strlen(inf.fullformat)) {
            ctx->valueError(ctx->modp()->ctrl()->printBuffer("too less arguments for function '%s'", funcName()), arg);
            rp->append(inf.formatbuf);
            if (inf.fullformat)
                rp->append(inf.fullformat + inf.nxtformat);
        }

        return true;
    }

    /**
     * calls the function for a simple source value (no array or list)
     * @param ctx			execution context
     * @param res			store for result value
     * @param src			source value
     * @param aggr          called for aggregating elements of an array or a list
     * @param se			syntax element id of source value
     * @param info          info object for use by the caller
     * @return              only used if aggr: true if result is final
     */
    bool ValFunctionInternFormat::operCallSimple(ExecContext *ctx, CmplVal& res, CmplVal& src, bool aggr, unsigned se, void *info)
    {
        Info *inf = (Info *)info;

        bool start = !inf->fullformat;
        if (start) {
            if (src.t != TP_STR && src.t != TP_STRINGP) {
                ctx->valueError(ctx->modp()->ctrl()->printBuffer("first argument in function '%s' must be a string", funcName()), src, se);
                return true;
            }

            inf->fullformat = src.stringAsCharP(ctx->modp());
            inf->formatse = se;
            inf->nxtformat = 0;

            ctx->opResult().set(TP_STRINGP, inf->res);
        }

        if (inf->formattp == TP_EMPTY) {
            if (!extractNextFormat(ctx, inf))
                return true;
        }

        if (!start) {
            if (inf->readadd < inf->cntadd) {
                intType i;
                if (!src.toInt(i, typeConversionValue, ctx->modp())) {
                    ctx->valueError(ctx->modp()->ctrl()->printBuffer("argument for width or precision for format specifier in function '%s' must be an int", funcName()), src, se);
                    i = 1;
                }

                if (inf->readadd == 0)
                    inf->add1 = i;
                else
                    inf->add2 = i;

                inf->readadd++;
            }
            else {
                // write argument value using current format
                writeValue(ctx, src, se, inf);

                if (!extractNextFormat(ctx, inf))
                    return true;
            }
        }

        return false;
	}

    /**
     * get next format specifier and set info for it
     * @param ctx           execution context
     * @param inf           info object
     * @return              false if error or end of format
     */
    bool ValFunctionInternFormat::extractNextFormat(ExecContext *ctx, Info *inf)
    {
        inf->formattp = TP_EMPTY;
        inf->cntadd = inf->readadd = 0;

        // search for start of next format specifier
        const char *ff = inf->fullformat;
        unsigned p = inf->nxtformat;

        for (; ff[p]; p++) {
            if (ff[p] == '%') {
                if (ff[p + 1] == '%')
                    p++;
                else if (ff[p + 1] != '\0')
                    break;
            }
        }

        if (p > inf->nxtformat) {
            inf->res->append(ff + inf->nxtformat, p - inf->nxtformat);
            inf->nxtformat = p;
        }

        if (!ff[p])
            return false;

        // search for end of next format specifier
        unsigned int i = 0;
        for (; ff[p] && i < 100; p++) {
            inf->formatbuf[i++] = ff[p];

            if (strchr("diouxXfFeEgGaAcspn", ff[p]))
                break;

            if (ff[p] == '*')
                inf->cntadd++;
        }

        inf->formatbuf[i] = '\0';

        if (!ff[p] || i >= 100 || inf->cntadd > 2) {
            CmplValAuto f(TP_STRINGP, new string(ff + inf->nxtformat));
            ctx->valueError(ctx->modp()->ctrl()->printBuffer("invalid format specifier in function '%s'", funcName()), f, inf->formatse);

            inf->res->append(ff + inf->nxtformat);
            inf->nxtformat = strlen(inf->fullformat);
            return false;
        }

        // remove type length from specifier
        char tpc = inf->formatbuf[i - 1];
        char ln1 = (i >= 2 ? inf->formatbuf[i - 2] : '\0');
        char ln2 = (i >= 3 ? inf->formatbuf[i - 3] : '\0');

        if (isalpha(ln1)) {
            unsigned cl = 1;
            if ((ln1 == 'h' || ln1 == 'l') && ln2 == ln1)
                cl = 2;

            inf->formatbuf[i - 1 - cl] = tpc;
            inf->formatbuf[i - cl] = '\0';

            i -= cl;
        }

        // type of format specifier
        const char *lenspec = NULL;
        if (strchr("diouxX", tpc)) {
            inf->formattp = TP_INT;
            lenspec = CMPLINT_STDLENSPEC;
        }
        else if (strchr("fFeEgGaA", tpc)) {
            inf->formattp = TP_REAL;
            lenspec = CMPLREAL_STDLENSPEC;
        }
        else if (tpc == 's' || tpc == 'c') {
            inf->formattp = TP_STR;
            inf->strc = (tpc == 'c');
        }
        else {
            CmplValAuto f(TP_STRINGP, new string(ff + inf->nxtformat));
            ctx->valueError(ctx->modp()->ctrl()->printBuffer("invalid format specifier type in function '%s'", funcName()), f, inf->formatse);

            inf->res->append(ff + inf->nxtformat);
            inf->nxtformat = strlen(inf->fullformat);
            return false;
        }

        // insert standard type length before specifier
        if (lenspec && *lenspec) {
            if (!lenspec[1]) {
                inf->formatbuf[i - 1] = lenspec[0];
                inf->formatbuf[i] = tpc;
                inf->formatbuf[i + 1] = '\0';
            }
            else {
                inf->formatbuf[i - 1] = lenspec[0];
                inf->formatbuf[i] = lenspec[1];
                inf->formatbuf[i + 1] = tpc;
                inf->formatbuf[i + 2] = '\0';
            }
        }

        inf->nxtformat = p + 1;
        return true;
    }

    /**
     * write value to result string using current format
     * @param ctx           execution context
     * @param val           value
     * @param se            syntax element id of the value
     * @param inf           info object
     */
    void ValFunctionInternFormat::writeValue(ExecContext *ctx, CmplVal& val, unsigned se, Info *inf)
    {
        // convert value to type of format specifier
        realType valr;
        intType vali;
        const char *vals;
        bool conv;
        CmplVal strv;
        bool strdisp = false;

        if (inf->formattp == TP_INT) {
            conv = val.toInt(vali, typeConversionAll, ctx->modp());
        }
        else if (inf->formattp == TP_REAL) {
            conv = val.toReal(valr, typeConversionAll, ctx->modp());
        }
        else {
            conv = val.toString(strv, typeConversionAll, ctx->modp(), strdisp);
            vals = strv.stringAsCharP(ctx->modp());
        }

        if (!conv) {
            ctx->valueError(ctx->modp()->ctrl()->printBuffer("invalid type of argument for format specifier '%s' in function '%s'", inf->formatbuf, funcName()), val, se);
            valr = 0;
            vali = 0;
            vals = "";
        }

        // format value using printf
        if (printfValue(inf, valr, vali, vals)) {
            inf->res->append(inf->buf);
        }
        else {
            CmplValAuto f(TP_STRINGP, new string(inf->formatbuf));
            ctx->valueError(ctx->modp()->ctrl()->printBuffer("invalid format specifier in function '%s'", funcName()), f, inf->formatse);
            inf->res->append(inf->formatbuf);
        }

        inf->buf[0] = '\0';

        if (strdisp)
            strv.dispose();

        inf->formatbuf[0] = '\0';
        inf->formattp = TP_EMPTY;
    }

    /**
     * printf the value to buffer
     * @param inf           info object
     * @param valr          value if real
     * @param vali          value if int
     * @param vals          value if string
     * @return              false if error
     */
    bool ValFunctionInternFormat::printfValue(Info *inf, realType valr, intType vali, const char *vals)
    {
        unsigned sc = (inf->formattp == TP_INT ? 10 : (inf->formattp == TP_REAL ? 20 : (inf->strc ? 40 : 30))) + inf->cntadd;

        if (!inf->buf) {
            inf->buflen = 50;
            inf->buf = new char[inf->buflen];
        }

        int res;
        while (true) {
            switch (sc) {
                case 10: res = snprintf(inf->buf, inf->buflen, inf->formatbuf, vali); break;
                case 11: res = snprintf(inf->buf, inf->buflen, inf->formatbuf, inf->add1, vali); break;
                case 12: res = snprintf(inf->buf, inf->buflen, inf->formatbuf, inf->add1, inf->add2, vali); break;

                case 20: res = snprintf(inf->buf, inf->buflen, inf->formatbuf, valr); break;
                case 21: res = snprintf(inf->buf, inf->buflen, inf->formatbuf, inf->add1, valr); break;
                case 22: res = snprintf(inf->buf, inf->buflen, inf->formatbuf, inf->add1, inf->add2, valr); break;

                case 30: res = snprintf(inf->buf, inf->buflen, inf->formatbuf, vals); break;
                case 31: res = snprintf(inf->buf, inf->buflen, inf->formatbuf, inf->add1, vals); break;
                case 32: res = snprintf(inf->buf, inf->buflen, inf->formatbuf, inf->add1, inf->add2, vals); break;

                case 40: res = snprintf(inf->buf, inf->buflen, inf->formatbuf, *vals); break;
                case 41: res = snprintf(inf->buf, inf->buflen, inf->formatbuf, inf->add1, *vals); break;
                case 42: res = snprintf(inf->buf, inf->buflen, inf->formatbuf, inf->add1, inf->add2, *vals); break;
            }

            if (res < 0)
                return false;
            else if ((unsigned)res < inf->buflen)
                break;

            inf->buflen = res + 1;
            delete inf->buf;
            inf->buf = new char[inf->buflen];
        }

        return true;
    }



    /****** ValFunctionDivInteger ****/

	/**
	 * calls the cmpl function call operation and store result in execution context, if this value has a special handling for it
	 * @param ctx			execution context
	 * @param arg			pointer to argument value
	 * @return				true
	 */
    bool ValFunctionDivInteger::operCall(ExecContext *ctx, StackValue *arg)
	{
        CmplValAuto args[2];
        unsigned se[2];
        intType res = 1;

        if (ctx->getFirstSimpleValues(arg, 2, args, se) != 2) {
            ctx->valueError(ctx->modp()->ctrl()->printBuffer("wrong count of arguments for function '%s'", funcName()), arg);
        }
        else {
            intType i1, i2;
            if (!args[0].toInt(i1, typeConversionValue, ctx->modp())) {
                ctx->valueError(ctx->modp()->ctrl()->printBuffer("for function '%s' first argument must be an integer value", funcName()), args[0], se[0]);
            }
            else if (!args[1].toInt(i2, typeConversionValue, ctx->modp())) {
                ctx->valueError(ctx->modp()->ctrl()->printBuffer("for function '%s' second argument must be an integer value", funcName()), args[1], se[1]);
            }
            else if (i2 == 0) {
                ctx->valueError(ctx->modp()->ctrl()->printBuffer("for function '%s' second argument must not be 0", funcName()), args[1], se[1]);
            }
            else {
                res = execDiv(i1, i2);
            }
        }
		
        ctx->opResult().set(TP_INT, res);
		return true;
	}



	/****** ValFunctionInternSrand ****/

	/**
	 * calls the cmpl function call operation and store result in execution context, if this value has a special handling for it
	 * @param ctx			execution context
	 * @param arg			pointer to argument value
	 * @return				true
	 */
    bool ValFunctionInternSrand::operCall(ExecContext *ctx, StackValue *arg)
    {
        CmplVal *v = arg->simpleValue();
        intType i;
        if (v && !v->isList()) {
            if (v->toInt(i, typeConversionExact, ctx->modp())) {
                  srand( i );
                  ctx->opResult().set(TP_INT, i);
            } else {
                ctx->valueError(ctx->modp()->ctrl()->printBuffer("wrong argument for function %s", funcName()), arg);
                ctx->opResult().set(TP_NULL);
            }

        } else {
            ctx->valueError(ctx->modp()->ctrl()->printBuffer("wrong <array|list|symbol> argument for function %s", funcName()), arg);
            ctx->opResult().set(TP_NULL);
        }


        return true;
    }


	/****** ValFunctionInternRand ****/

	/**
	 * calls the cmpl function call operation and store result in execution context, if this value has a special handling for it
	 * @param ctx			execution context
	 * @param arg			pointer to argument value
	 * @return				true
	 */
    bool ValFunctionInternRand::operCall(ExecContext *ctx, StackValue *arg)
    {
        CmplVal *v = arg->simpleValue();
        intType i,res;
        if (v && !v->isList()) {
            if (v->toInt(i, typeConversionExact, ctx->modp())) {
                if (i<=RAND_MAX) {
                  res=rand()%i;
                  ctx->opResult().set(TP_INT, res);
                } else {
                    ctx->valueError(ctx->modp()->ctrl()->printBuffer("wrong argument for function %s", funcName()), arg);
                    ctx->opResult().set(TP_NULL);
                }
            } else {
                ctx->valueError(ctx->modp()->ctrl()->printBuffer("wrong argument for function %s", funcName()), arg);
                ctx->opResult().set(TP_NULL);
            }

        } else {
            ctx->valueError(ctx->modp()->ctrl()->printBuffer("wrong <array|list|symbol> argument for function %s", funcName()), arg);
            ctx->opResult().set(TP_NULL);
        }

        return true;
    }

}

