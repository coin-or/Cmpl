
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


#include "OperationBase.hh"
#include "Interval.hh"
#include "SetUtil.hh"
#include "TupleUtil.hh"
#include "ValFunction.hh"
#include "ValType.hh"
#include "Interpreter.hh"
#include "../../Control/MainControl.hh"
#include "../../CommonData/IntCode.hh"


namespace cmpl
{
    /************** OperationBase **********/

    /**
     * executes operation
     * @param ctx			execution context
     * @param res			store for result value
     * @param tp			mark result as transposed
     * @param se			syntax element id of operation
     * @param op			operation code (one of the minor codes for INTCODE_OPERATION)
     * @param cmpFollow		other compare operation follows immediately as continuation of this compare operation (only used for compare operation)
     * @param ac			count of arguments for the operation
     * @param a1			argument one
     * @param a2			argument two
     * @return				true if operation is handled here
     */
    bool OperationBase::execOper(ExecContext *ctx, CmplVal& res, bool& tp, unsigned se, unsigned short op, bool cmpFollow, unsigned short ac, StackValue *a1, StackValue *a2)
    {
        PROTO_MOD_OUTL(ctx->modp(), "exec operation: " << op << " (" << cmpFollow << "): " << ac);

        CmplVal resDs;
        MatrixMultMode mm = matrixMultNone;
        CmplVal nl(TP_NULL);
        CmplVal *a1s = (a1 ? a1->simpleValueOrEmptyArr(&nl) : NULL);
        CmplVal *a2s = (a2 ? a2->simpleValueOrEmptyArr(&nl) : NULL);
        bool a1cbs = (a1s && a1s->isCbhVal());
        bool a2cbs = (a2s && a2s->isCbhVal());

        if (a1cbs || a2cbs) {
            tp_e a1st = (a1s ? a1s->t : TP_EMPTY);
            tp_e a2st = (a2s ? a2s->t : TP_EMPTY);
            bool cbsEq = (op == ICS_OPER_EQ || op == ICS_OPER_EQ2);
            bool cbsIn = (op == ICS_OPER_IN || op == ICS_OPER_OF);
            res.unset();

            if (cbsEq && (!a1cbs || !a2cbs) && (a1st == TP_DEF_CB_SYM || a1st == TP_DEF_CB_SYM_TUPLE || a2st == TP_DEF_CB_SYM || a2st == TP_DEF_CB_SYM_TUPLE)) {
                bool r = CodeBlockHeaderBase::execCbsOperEq(ctx, (a1cbs ? a1s : a2s), (a1cbs ? a2 : a1), se);
                res.set(TP_CBHEAD_BIN, r);
            }
            else if (cbsIn && !a2cbs && (a1st == TP_DEF_CB_SYM || a1st == TP_DEF_CB_SYM_TUPLE)) {
                CBHeaderIterInfo *r = CodeBlockHeaderBase::execCbsOperIn(ctx, (op == ICS_OPER_IN), a1s, a2, se);
                if (r)
                    res.setP(TP_CBHEAD_ITER, r);
                else
                    res.set(TP_CBHEAD_BIN, false);
            }
            else if (op == ICS_OPER_AND && a1s && a2s) {
                bool b1, b2;
                if (a1s->toBool(b1, typeConversionAll, ctx->modp()) && a2s->toBool(b2, typeConversionAll, ctx->modp()))
                    res.set(TP_CBHEAD_BIN, (b1 && b2));
            }

            if (!res) {
                if (!cbsEq && !cbsIn)
                    ctx->valueError("operation not allowed with codeblock symbol definition", (a1cbs ? a1 : a2)->val(), se);
                else
                    ctx->valueError("second operand cannot be codeblock symbol definition", a2->val(), se);

                res.set(TP_BIN, false);
            }

            return true;
        }

        if (op == ICS_OPER_TRP) {
            if (a1->val().isArray() && !a1->val().array()->isScalar() && SetBase::rank1(a1->val().array()->defset())) {
                tp = true;
                return false;
            }
        }

        if ((ac >= 1 && a1s && a1s->t == TP_NULL) || (ac == 2 && a2s && a2s->t == TP_NULL)) {
            // if one operand in a numerical or logical operation is null, then the other operand determines the result
            if ((ac == 2 && op <= ICS_OPER_NOT) || op <= ICS_OPER_SUB || op == ICS_OPER_NOT) {
                if (a1s && a1s->t == TP_NULL && (ac == 1 || (a2s && a2s->t == TP_NULL))) {
                    res.set(TP_NULL);
                    return true;
                }
                else if (a2s && a2s->t == TP_NULL) {
                    if (op == ICS_OPER_OR && a1s && a1s->t == TP_DATA_TYPE) {
                        ValTypeUserOr::construct(&res, a1s, a2s);
                        return true;
                    }
                    else {
                        res.copyFrom(a1->val());
                        return true;
                    }
                }
                else {
                    if (op == ICS_OPER_ADD || op == ICS_OPER_MUL || op == ICS_OPER_AND || op == ICS_OPER_OR) {
                        res.copyFrom(a2->val());
                        return true;
                    }
                    else {
                        a1->val().dispSet(TP_INT, (intType)(op == ICS_OPER_SUB ? 0 : 1));
                        a1s = &(a1->val());
                    }
                }
            }
        }

        if (!resultSet(ctx, resDs, mm, se, op, ac, a1, a2)) {
            res.set(TP_INT, 0);
            return true;
        }

        PROTO_MOD_OUTL(ctx->modp(), "  result set: " << resDs << " (" << (int)mm << ')');

        CmplArray *arr = NULL;
        if (resDs.t != TP_SET_NULL) {
            arr = new CmplArray(resDs);
            arr->delValidInfo();
            res.set(TP_ARRAY, arr);
        }

        if (ac == 0) {
            if (op == ICS_OPER_MUL || op == ICS_OPER_DIV) {
                res.set(op == ICS_OPER_MUL ? TP_SET_R1A : TP_SET_R1A_MNF);
            }
            else {
                ctx->modp()->ctrl()->errHandler().internalError("illegal operation with no operand");
                res.set(TP_INT, 0);
            }
        }
        else if (ac == 1) {
            if (op == ICS_OPER_TRP) {
                unsigned long c1 = SetBase::cnt(*(resDs.setRecMult()->partSet(0)));
                unsigned long c2 = arr->size() / c1;
                CmplVal *r = arr->at(0);

                for (unsigned long i1 = 0; i1 < c1; i1++) {
                    for (unsigned long i2 = 0; i2 < c2; i2++, r++)
                        r->copyFrom(a1->val().array()->at(i2 * c1 + i1), true, false);
                }
            }
            else if (arr) {
                for (unsigned long i = 0; i < arr->size(); i++)
                    execUnaryOper(ctx, arr->at(i), se, op, a1->val().array()->at(i));
            }
            else {
                execUnaryOper(ctx, &res, se, op, a1s);
            }
        }
        else {
            CmplVal sempty(TP_SET_EMPTY);
            CmplVal* df1 = (a1->val().t == TP_NULL ? &sempty : (a1->val().isArray() && !a1->val().array()->isScalar() ? &(a1->val().array()->defset()) : NULL));
            CmplVal* df2 = (a2->val().t == TP_NULL ? &sempty : (a2->val().isArray() && !a2->val().array()->isScalar() ? &(a2->val().array()->defset()) : NULL));

            if (op == ICS_OPER_OF) {
                df2 = NULL;
                a2s = &(a2->val());
            }

            if (df1 && df2) {
                if (op == ICS_OPER_EQ2 || op == ICS_OPER_NE2) {
                    if (!SetUtil::compareEq(df1, df2, true)) {
                        res.set(TP_BIN, (op != ICS_OPER_EQ2));
                    }
                    else {
                        unsigned long cnt = SetBase::cnt(*df1);
                        res.set(TP_BIN, (op == ICS_OPER_EQ2));
                        for (unsigned long i = 0; i < cnt; i++) {
                            execBinaryOper(ctx, &res, se, op, cmpFollow, a1->val().array()->at(i), a2->val().array()->at(i));
                            if (res.v.i == (op == ICS_OPER_EQ2 ? 0 : 1))
                                return true;
                        }
                    }
                }
                else if (op == ICS_OPER_MUL && mm != matrixMultNone) {
                    execMatrixMult(ctx, &res, mm, se, a1->val().array(), a2->val().array());
                    tp = (mm == matrixMultRowMtx);
                }
                else {
                    for (unsigned long i = 0; i < arr->size(); i++)
                        execBinaryOper(ctx, arr->at(i), se, op, cmpFollow, a1->val().array()->at(i), a2->val().array()->at(i));
                }
            }
            else if (df1 || df2) {
                if (op == ICS_OPER_EQ2 || op == ICS_OPER_NE2) {
                    res.set(TP_BIN, (op != ICS_OPER_EQ2));
                }
                else {
                    // temporary add additional reference to the scalar argument, in order to prevent reusing of its value object within the operation
                    CmplValAuto rsa;
                    if (!df1) {
                        if (a1s->useValP())
                            rsa.copyFrom(a1s);
                    }
                    else if (!df2) {
                        if (a2s->useValP())
                            rsa.copyFrom(a2s);
                    }

                    for (unsigned long i = 0; i < arr->size(); i++)
                        execBinaryOper(ctx, arr->at(i), se, op, cmpFollow, (df1 ? a1->val().array()->at(i) : a1s), (df2 ? a2->val().array()->at(i) : a2s));
                }
            }
            else {
                execBinaryOper(ctx, &res, se, op, cmpFollow, a1s, a2s);
            }
        }

        return true;
    }


    /**
     * computes definition set of the result array of an operation
     * @param ctx			execution context
     * @param res			store for result definition set
     * @param mm			return mode for matrix multiplication
     * @param se			syntax element id of operation
     * @param op			operation code (one of the minor codes for INTCODE_OPERATION)
     * @param ac			count of arguments for the operation
     * @param a1			argument one
     * @param a2			argument two
     * @return				false if error
     */
    bool OperationBase::resultSet(ExecContext *ctx, CmplVal& res, MatrixMultMode& mm, unsigned se, unsigned short op, unsigned short ac, StackValue *a1, StackValue *a2)
    {
        bool ba1 = (ac > 0 && (a1->val().t == TP_NULL || (a1->val().isArray() && !a1->val().array()->isScalar())));
        bool ba2 = (ac > 1 && (a2->val().t == TP_NULL || (a2->val().isArray() && !a2->val().array()->isScalar())));

        // check if arguments are valid
        if (ac == 0) {
            if (op != ICS_OPER_MUL && op != ICS_OPER_DIV) {
                ctx->modp()->ctrl()->errHandler().internalError("illegal operation with no argument");
                return false;
            }
        }
        else if (ac == 1) {
            if (op > ICS_OPER_DIV && op != ICS_OPER_NOT && op != ICS_OPER_TRP) {
                ctx->modp()->ctrl()->errHandler().internalError("illegal binary operation with one argument");
                return false;
            }

            if (ba1 && (op == ICS_OPER_MUL || op == ICS_OPER_DIV)) {
                ctx->valueError("argument for operation cannot be an array", a1->val(), se);
                return false;
            }

            if (op == ICS_OPER_TRP) {
                // transpose for array with rank 1 is handled directly in ExecContext, so here array must have rank 2
                CmplVal *df1 = (ba1 && a1->val().t != TP_NULL ? &(a1->val().array()->defset()) : NULL);
                if (!df1 || df1->t != TP_SET_REC_MULT || df1->setRecMult()->maxRank() != 2) {
                     ctx->valueError("transpose operation can only used for array with rank 1 or rectangular array with rank 2", a1->val(), se);
                     return false;
                }
            }
        }
        else {
            if (op == ICS_OPER_NOT || op == ICS_OPER_TRP) {
                ctx->modp()->ctrl()->errHandler().internalError("illegal unary operation with two arguments");
                return false;
            }

            if ((op == ICS_OPER_DIV || op == ICS_OPER_POW || op == ICS_OPER_OBJTO) && ba2) {
                ctx->valueError("second argument for operation must be a scalar value", a2->val(), se);
                return false;
            }

            if (op == ICS_OPER_IN && (ba1 || ba2)) {
                ctx->valueError("arguments for operation 'in' must be scalar values", (ba1 ? a1 : a2)->val(), se);
                return false;
            }

            if (op == ICS_OPER_OF) {
                ba2 = false;
                if (ba1) {
                    ctx->valueError("first argument for operation 'of' must be a scalar value", a1->val(), se);
                    return false;
                }
            }
        }

        // get result definition set
        if (ba1 && ba2) {
            if (op == ICS_OPER_EQ2 || op == ICS_OPER_NE2) {
                res.set(TP_SET_NULL);
            }
            else {
                CmplVal sempty(TP_SET_EMPTY);
                CmplVal& df1 = (a1->val().t == TP_NULL ? sempty : a1->val().array()->defset());
                CmplVal& df2 = (a2->val().t == TP_NULL ? sempty : a2->val().array()->defset());

                if (op == ICS_OPER_MUL) {
                    if (!resultSetMatrixMult(ctx, res, mm, se, a1, a2, df1, df2))
                        return false;
                }
                else {
                    if (!SetUtil::compareEq(&df1, &df2)) {
                        ctx->valueError("both arguments for operation must be arrays with the same definition set", a2->val(), se);
                        return false;
                    }

                    res.copyFrom(df1, true, false);
                }
            }
        }
        else if (ba1 || ba2) {
            if (op == ICS_OPER_TRP) {
                SetRecMult *tq = a1->val().array()->defset().setRecMult();
                SetRecMult *td = new SetRecMult(*(tq->partSet(1)), *(tq->partSet(0)));
                res.set(TP_SET_REC_MULT, td);
            }
            else {
                CmplVal& vr = (ba1 ? a1 : a2)->val();
                if (vr.t == TP_NULL)
                    res.set(TP_SET_EMPTY);
                else
                    res.copyFrom(vr.array()->defset(), true, false);
            }
        }
        else {
            res.set(TP_SET_NULL);
        }

        return true;
    }

    /**
     * get mode and result definition set for matrix multiplication
     * @param ctx			execution context
     * @param res			store for result definition set
     * @param mm			return mode for matrix multiplication
     * @param se			syntax element id of operation
     * @param a1			argument one
     * @param a2			argument two
     * @param df1			definition set of argument one
     * @param df2			definition set of argument two
     * @return				false if error
     */
    bool OperationBase::resultSetMatrixMult(ExecContext *ctx, CmplVal& res, MatrixMultMode& mm, unsigned se, StackValue *a1, StackValue *a2, CmplVal& df1, CmplVal& df2)
    {
        // definition sets of both arguments must be finite sets either with rank 1 or TP_SET_REC_MULT with rank 2
        unsigned r1 = SetBase::rank(df1);
        unsigned r2 = SetBase::rank(df2);
        if (r1 < 1 || r1 > 2 || (r1 == 1 && !SetBase::rank1(df1)) || (r1 == 2 && df1.t != TP_SET_REC_MULT)) {
            ctx->valueError("definition set of array argument one has wrong rank or set type", a1->val(), se);
            return false;
        }
        if (r2 < 1 || r2 > 2 || (r2 == 1 && !SetBase::rank1(df2)) || (r2 == 2 && df2.t != TP_SET_REC_MULT)) {
            ctx->valueError("definition set of array argument two has wrong rank or set type", a2->val(), se);
            return false;
        }

        if (r1 == 1 && r2 == 1) {
            unsigned long c1 = SetBase::cnt(df1);
            unsigned long c2 = SetBase::cnt(df2);

            if (a1->isTransposed() && !a2->isTransposed()) {
                if (c1 != c2 || !SetUtil::compareEq(&df1, &df2)) {
                    ctx->valueError("indizes of arrays for matrix multiplication don't match", a1->val(), se);
                    return false;
                }

                res.set(TP_SET_NULL);
                mm = matrixMultRowCol;
            }
            else if (!a1->isTransposed() && a2->isTransposed()) {
                SetRecMult *srm = new SetRecMult(df1, df2);
                res.set(TP_SET_REC_MULT, srm);
                mm = matrixMultColRow;
            }
            else {
                ctx->valueError("for multiplication of two arrays with rank 1 exactly one of them must be transposed", a1->val(), se);
                return false;
            }
        }
        else if (r1 == 2 && r2 == 1) {
            if (a2->isTransposed()) {
                ctx->valueError("for multiplication of array with rank 2 times array with rank 1 the second argument must not be transposed", a2->val(), se);
                return false;
            }

            CmplVal *s1a, *s1b;
            s1a = df1.setRecMult()->partSet(0);
            s1b = df1.setRecMult()->partSet(1);

            if (!SetUtil::compareEq(s1b, &df2)) {
                ctx->valueError("indizes of arrays for matrix multiplication don't match", a1->val(), se);
                return false;
            }

            res.copyFrom(s1a, true, false);
            mm = matrixMultMtxCol;
        }
        else if (r1 == 1 && r2 == 2) {
            if (!a1->isTransposed()) {
                ctx->valueError("for multiplication of array with rank 1 times array with rank 2 the first argument must be transposed", a1->val(), se);
                return false;
            }

            CmplVal *s2a, *s2b;
            s2a = df2.setRecMult()->partSet(0);
            s2b = df2.setRecMult()->partSet(1);

            if (!SetUtil::compareEq(&df1, s2a)) {
                ctx->valueError("indizes of arrays for matrix multiplication don't match", a1->val(), se);
                return false;
            }

            res.copyFrom(s2b, true, false);
            mm = matrixMultRowMtx;
        }
        else {	// r1 == 2 && r2 == 2
            CmplVal *s1a, *s1b, *s2a, *s2b;
            s1a = df1.setRecMult()->partSet(0);
            s1b = df1.setRecMult()->partSet(1);
            s2a = df2.setRecMult()->partSet(0);
            s2b = df2.setRecMult()->partSet(1);

            if (!SetUtil::compareEq(s1b, s2a)) {
                ctx->valueError("indizes of arrays for matrix multiplication don't match", a1->val(), se);
                return false;
            }

            SetRecMult *srm = new SetRecMult(*s1a, *s2b);
            res.set(TP_SET_REC_MULT, srm);
            mm = matrixMultMtxMtx;
        }

        return true;
    }


    /**
     * executes unary operation (not for array)
     * @param ctx			execution context
     * @param res			store for result value
     * @param se			syntax element id of operation
     * @param op			operation code (one of the minor codes for INTCODE_OPERATION)
     * @param ac			count of arguments for the operation
     * @param a1			argument (must not be an array)
     */
    void OperationBase::execUnaryOper(ExecContext *ctx, CmplVal *res, unsigned se, unsigned short op, CmplVal *a1)
    {
        CmplVal r;

        switch (op) {
            case ICS_OPER_ADD:
                pos(ctx, res, se, a1);
                break;

            case ICS_OPER_SUB:
                neg(ctx, res, se, a1);
                break;

            case ICS_OPER_MUL:
                setMarkNF(ctx, res, se, a1, false);
                break;

            case ICS_OPER_DIV:
                setMarkNF(ctx, res, se, a1, true);
                break;

            case ICS_OPER_NOT:
                if (a1->t == TP_FORMULA) {
                    if (a1->valFormula()->isBool()) {
                        (dynamic_cast<OperationBase *>(a1->valFormula()))->notF(ctx, res, se);
                    }
                    else {
                        ctx->modp()->ctrl()->errHandler().internalError("argument for logical operation must be a formula with boolean value");
                        res->set(TP_BIN, false);
                    }
                }
                else if (a1->t == TP_OPT_VAR && a1->optVar()->binVar()) {
                    res->set(TP_FORMULA, new ValFormulaLogConOp(se, true, new ValFormulaVarOp(se, a1->optVar())), true);
                }
                else {
                    bool b;
                    if (a1->toBool(b, typeConversionAll, ctx->modp())) {
                        res->set(TP_BIN, b);
                    }
                    else {
                        ctx->valueError("argument cannot be converted to boolean value", *a1, se);
                        res->set(TP_BIN, false);
                    }
                }
                break;

            default:
                ctx->modp()->ctrl()->errHandler().internalError("illegal unary operation");
                res->set(TP_INT, 0);
        }
    }


    /**
     * executes binary operation (not for array)
     * @param ctx			execution context
     * @param res			store for result value
     * @param se			syntax element id of operation
     * @param op			operation code (one of the minor codes for INTCODE_OPERATION)
     * @param cmpFollow		other compare operation follows immediately as continuation of this compare operation (only used for compare operation)
     * @param ac			count of arguments for the operation
     * @param a1			argument one (must not be an array)
     * @param a2			argument two (must not be an array)
     */
    void OperationBase::execBinaryOper(ExecContext *ctx, CmplVal *res, unsigned se, unsigned short op, bool cmpFollow, CmplVal *a1, CmplVal *a2)
    {
        // some operations generally don't support formulas or variables
        if ((a1->t == TP_FORMULA || a1->t == TP_OPT_VAR) && (op == ICS_OPER_IN || op == ICS_OPER_OF || op == ICS_OPER_IRR)) {
            ctx->valueError("formula or optimization variable is not allowed as first argument of the operation", *a1, se);
            res->set(TP_INT, 0);
        }
        else if (((a2->t == TP_FORMULA || a2->t == TP_OPT_VAR) && (op == ICS_OPER_POW || op == ICS_OPER_IN || op == ICS_OPER_OF || op == ICS_OPER_IRR || op == ICS_OPER_OBJTO))) {
            ctx->valueError("formula or optimization variable is not allowed as second argument of the operation", *a2, se);
            res->set(TP_INT, 0);
        }
        else {
            switch (op) {
                case ICS_OPER_ADD:
                    plus(ctx, res, se, a1, a2);
                    break;

                case ICS_OPER_SUB:
                    minus(ctx, res, se, a1, a2);
                    break;

                case ICS_OPER_MUL:
                    mult(ctx, res, se, a1, a2);
                    break;

                case ICS_OPER_DIV:
                    div(ctx, res, se, a1, a2);
                    break;

                case ICS_OPER_POW:
                    pow(ctx, res, se, a1, a2);
                    break;

                case ICS_OPER_AND:
                    logAndOr(ctx, res, se, a1, a2, op);
                    break;

                case ICS_OPER_OR:
                    if (a1->t == TP_DATA_TYPE) {
                        if (!ValTypeUserOr::construct(res, a1, a2)) {
                            ctx->valueError("invalid second argument in construction of user defined union data type", *a2, se);
                            res->copyFrom(a1, true, false);
                        }
                    }
                    else {
                        logAndOr(ctx, res, se, a1, a2, op);
                    }
                    break;

                case ICS_OPER_EQ:
                    comp(ctx, res, se, a1, a2, true, true, false, cmpFollow);
                    break;

                case ICS_OPER_EQ2:
                    res->set(TP_BIN, compEq2(ctx, se, a1, a2));
                    break;

                case ICS_OPER_NE:
                    comp(ctx, res, se, a1, a2, true, true, true, cmpFollow);
                    break;

                case ICS_OPER_NE2:
                    res->set(TP_BIN, !compEq2(ctx, se, a1, a2));
                    break;

                case ICS_OPER_LT:
                    comp(ctx, res, se, a1, a2, true, false, true, cmpFollow);
                    break;

                case ICS_OPER_GT:
                    comp(ctx, res, se, a1, a2, false, true, true, cmpFollow);
                    break;

                case ICS_OPER_LE:
                    comp(ctx, res, se, a1, a2, false, true, false, cmpFollow);
                    break;

                case ICS_OPER_GE:
                    comp(ctx, res, se, a1, a2, true, false, false, cmpFollow);
                    break;

                case ICS_OPER_IN:
                    tplInSet(ctx, res, se, a1, a2, true);
                    break;

                case ICS_OPER_OF:
                    tplInSet(ctx, res, se, a1, a2, false);
                    break;

                case ICS_OPER_IRR:
                    tplMatch(ctx, res, se, a1, a2);
                    break;

                case ICS_OPER_OBJTO:
                    objTo(ctx, res, se, a1, a2);
                    break;

                default:
                    ctx->modp()->ctrl()->errHandler().internalError("illegal binary operation");
                    res->set(TP_INT, 0);
            }
        }
    }

    /**
     * executes matrix multiplication
     * @param ctx			execution context
     * @param res			result array store
     * @param mm			mode for matrix multiplication
     * @param se			syntax element id of operation
     * @param a1			argument one array
     * @param a2			argument two array
     */
    void OperationBase::execMatrixMult(ExecContext *ctx, CmplVal *res, MatrixMultMode mm, unsigned se, CmplArray *a1, CmplArray *a2)
    {
        unsigned long c1, c2, c3;
        unsigned long i1, i2, i3;
        CmplVal *r, *s1, *s2;
        CmplVal acc, acc2, mul;

        switch (mm) {
            case matrixMultRowCol:
                c1 = a1->size();
                for (i1 = 0, s1 = a1->at(0), s2 = a2->at(0); i1 < c1; i1++, s1++, s2++) {
                    mult(ctx, &mul, se, s1, s2);
                    plus(ctx, &acc, se, &acc2, &mul);
                    acc2.moveFrom(acc, true);
                    mul.dispUnset();
                }
                res->moveFrom(acc2, false);
                break;

            case matrixMultColRow:
                c1 = a1->size();
                c2 = a2->size();
                r = res->array()->at(0);

                for (i1 = 0, s1 = a1->at(0); i1 < c1; i1++, s1++) {
                    for (i2 = 0, s2 = a2->at(0); i2 < c2; i2++, s2++) {
                        mult(ctx, r++, se, s1, s2);
                    }
                }
                break;

            case matrixMultMtxCol:
                c2 = a2->size();
                c1 = a1->size() / c2;
                s1 = a1->at(0);

                for (i1 = 0, r = res->array()->at(0); i1 < c1; i1++, r++) {
                    for (i2 = 0, s2 = a2->at(0); i2 < c2; i2++, s2++, s1++) {
                        mult(ctx, &mul, se, s1, s2);
                        plus(ctx, &acc, se, &acc2, &mul);
                        acc2.moveFrom(acc, true);
                        mul.dispUnset();
                    }
                    r->moveFrom(acc2, false);
                }
                break;

            case matrixMultRowMtx:
                c1 = a1->size();
                c2 = a2->size() / c1;

                for (i2 = 0, r = res->array()->at(0); i2 < c2; i2++, r++) {
                    for (i1 = 0, s1 = a1->at(0); i1 < c1; i1++, s1++) {
                        s2 = a2->at(i1 * c2 + i2);
                        mult(ctx, &mul, se, s1, s2);
                        plus(ctx, &acc, se, &acc2, &mul);
                        acc2.moveFrom(acc, true);
                        mul.dispUnset();
                    }
                    r->moveFrom(acc2, false);
                }
                break;

            case matrixMultMtxMtx:
                a1->defset().setRecMult()->partSet(acc, 1, 1);
                c1 = SetBase::cnt(acc);
                c2 = a1->size() / c1;
                c3 = a2->size() / c1;
                acc.dispUnset();

                r = res->array()->at(0);
                for (i2 = 0; i2 < c2; i2++) {
                    for (i3 = 0; i3 < c3; i3++, r++) {
                        for (i1 = 0; i1 < c1; i1++) {
                            s1 = a1->at(i2 * c1 + i1);
                            s2 = a2->at(i1 * c3 + i3);
                            mult(ctx, &mul, se, s1, s2);
                            plus(ctx, &acc, se, &acc2, &mul);
                            acc2.moveFrom(acc, true);
                            mul.dispUnset();
                        }
                        r->moveFrom(acc2, false);
                    }
                }
                break;

            default:
                break;
        }
    }


    /**
     * unary plus of arbitrary value
     * @param ctx			execution context
     * @param res			store for result definition set
     * @param se			syntax element id of operation
     * @param a1			argument one (must not be an array)
     */
    void OperationBase::pos(ExecContext *ctx, CmplVal *res, unsigned se, CmplVal *a1)
    {
        if (!*a1) {
            res->unset();
        }
        else if (a1->t == TP_FORMULA) {
			(dynamic_cast<OperationBase *>(a1->valFormula()))->posF(ctx, res, se);
        }
        else if (a1->t == TP_OPT_VAR) {
            ValFormulaVarOp v(se, a1->optVar());
            v.posF(ctx, res, se);
        }
        else if (a1->isScalarNumber()) {
            posN(ctx, res, se, a1);
        }
        else {
            CmplVal n;
            if (a1->toNumber(n, typeConversionExact, ctx->modp())) {
                posN(ctx, res, se, &n);
            }
            else {
                ctx->valueError("argument is not a number", *a1, se);
            }
        }
    }


    /**
     * negation of arbitrary value
     * @param ctx			execution context
     * @param res			store for result definition set
     * @param se			syntax element id of operation
     * @param a1			argument one (must not be an array)
     */
    void OperationBase::neg(ExecContext *ctx, CmplVal *res, unsigned se, CmplVal *a1)
    {
        if (!*a1) {
            res->unset();
        }
        else if (a1->t == TP_FORMULA) {
			(dynamic_cast<OperationBase *>(a1->valFormula()))->negF(ctx, res, se);
        }
        else if (a1->t == TP_OPT_VAR) {
            ValFormulaVarOp *f = new ValFormulaVarOp(se, a1->optVar());
            f->negF(ctx, res, se);
            if (!f->refCnt())
                delete f;
        }
        else if (a1->isScalarNumber()) {
            negN(ctx, res, se, a1);
        }
        else {
            CmplVal n;
            if (a1->toNumber(n, typeConversionExact, ctx->modp())) {
                negN(ctx, res, se, &n);
            }
            else {
                ctx->valueError("argument is not a number", *a1, se);
            }
        }
    }

    /**
     * negation of number
     * @param ctx			execution context
     * @param res			store for result definition set
     * @param se			syntax element id of operation
     * @param a1			argument one (must be TP_INT, TP_REAL or TP_INFINITE)
     */
    void OperationBase::negN(ExecContext *ctx, CmplVal *res, unsigned se, CmplVal *a1)
    {
        if (a1->useInt()) {
            if (a1->t == TP_INFINITE) {
                res->set(TP_INFINITE, -a1->v.i);
            }
            else {
                res->set(TP_INT, -a1->v.i);
                if (a1->v.i == INT_MIN && ctx->modp()->intOutOfRangeMode() != INT_OPER_OORANGE_IGNORE) {
                    if (ctx->modp()->intOutOfRangeMode() == INT_OPER_OORANGE_CONVERT)
                        res->set(TP_REAL, -((realType)(a1->v.i)));
                    else
                        ctx->valueError("integer result of operation is out of range", *a1, se);
                }
            }
        }
        else {
            res->set(TP_REAL, -a1->v.r);
        }
    }

    /**
     * logical Not for formula
     * @param ctx			execution context
     * @param res			store for result value
     * @param se			syntax element id of operation
     * @param noReuse       don't reuse this even if it has only one reference
     */
    void OperationBase::notF(ExecContext *ctx, CmplVal *res, unsigned se, bool noReuse)
    {
        res->set(TP_FORMULA, new ValFormulaLogConOp(se, true, dynamic_cast<ValFormula *>(this)), true);
    }

    /**
     * addition of two arbitrary values
     * @param ctx			execution context
     * @param res			store for result value
     * @param se			syntax element id of operation
     * @param a1			argument one (must not be an array)
     * @param a2			argument two (must not be an array)
     */
    void OperationBase::plus(ExecContext *ctx, CmplVal *res, unsigned se, CmplVal *a1, CmplVal *a2)
    {
        if (!*a1 || !*a2) {
            res->copyFrom((!*a1 ? a2 : a1), true, false);
        }
        else if (a1->t == TP_FORMULA || a2->t == TP_FORMULA) {
            if (a1->isNumNull(true) || a2->isNumNull(true)) {
                res->copyFrom((a1->isNumNull(true) ? a2 : a1), true, false);
            }
            else {
                // check for use of shortcut for linear combination, for better performance
                if (a1->t == TP_FORMULA && a1->v.vp->refCnt() == 1) {
                    ValFormulaLinearCombOp *lc = dynamic_cast<ValFormulaLinearCombOp *>(a1->valFormula());
                    if (lc && lc->addTerm(ctx, se, a2, false)) {
                        res->copyFrom(a1, true, false);
                        return;
                    }
                }

                unsigned p1 = (a1->t == TP_FORMULA ? (dynamic_cast<OperationBase *>(a1->valFormula()))->formulaOperPrio(ICS_OPER_ADD, true, a2) : 1);
                unsigned p2 = (a2->t == TP_FORMULA ? (dynamic_cast<OperationBase *>(a2->valFormula()))->formulaOperPrio(ICS_OPER_ADD, false, a1) : 1);

                if (p1 == 0) {
                    ctx->valueError("given formula is not allowed as first argument of the operation", *a1, se);
                    res->set(TP_INT, 0);
                    return;
                }
                else if (p2 == 0) {
                    ctx->valueError("given formula is not allowed as second argument of the operation", *a2, se);
                    res->set(TP_INT, 0);
                    return;
                }
                else {
                    if (p1 >= p2)
                        (dynamic_cast<OperationBase *>(a1->valFormula()))->plusF(ctx, res, se, a2);
                    else
                        (dynamic_cast<OperationBase *>(a2->valFormula()))->plusF(ctx, res, se, a1);
                }
            }
        }
        else if (a1->t == TP_OPT_VAR || a2->t == TP_OPT_VAR) {
            if (a1->isNumNull() || a2->isNumNull()) {
                res->copyFrom((a1->isNumNull() ? a2 : a1), true, false);
            }
            else {
                ValFormulaLinearCombOp *f = new ValFormulaLinearCombOp(se, (a1->t == TP_OPT_VAR ? a1 : a2)->optVar());
                f->plusF(ctx, res, se, (a1->t == TP_OPT_VAR ? a2 : a1));
                if (!f->refCnt())
                    delete f;
            }
        }
        else if (a1->isScalarNumber() && a2->isScalarNumber()) {
            plusN(ctx, res, se, a1, a2);
        }
        else {
            if ((a1->isSet() || a1->isTuple() || a1->isInterval()) && (a2->isSet() || a2->isTuple() || a2->isInterval())) {
                // "+" is used for union operation
                if (a1->isInterval() && a2->isInterval()) {
                    if (Interval::merge(*res, *a1, *a2))
                        return;
                }

                SetUtil::addSet(ctx, res, se, a1, a2, false, true);
            }
            else if ((a1->t == TP_STR || a1->t == TP_STRINGP) || (a2->t == TP_STR || a2->t == TP_STRINGP)) {
                // "+" is used for string concat
                CmplVal s1, s2;
                bool cs1, cs2, ds1, ds2;

                cs1 = a1->toString(s1, typeConversionAll, ctx->modp(), ds1);
                cs2 = a2->toString(s2, typeConversionAll, ctx->modp(), ds2);

                if (cs1 && cs2) {
                    const char *cp1 = s1.stringAsCharP(ctx->modp());
                    const char *cp2 = s2.stringAsCharP(ctx->modp());
                    string *rp = new string(cp1);
                    rp->append(cp2);
                    res->set(TP_STRINGP, rp);
                }
                else {
                    res->set(TP_INT, 0);
                    if (!cs1)
                        ctx->valueError("argument one is not convertible to a string", *a1, se);
                    else
                        ctx->valueError("argument two is not convertible to a string", *a1, se);
                }

                if (ds1)
                    s1.dispose();
                if (ds2)
                    s2.dispose();
            }
            else {
                // "+" is used for numerical addition
                CmplVal n1, n2;
                if (!(a1->toNumber(n1, typeConversionExact, ctx->modp()))) {
                    ctx->valueError("argument one is not a number", *a1, se);
                    res->set(TP_INT, 0);
                }
                else if (!(a2->toNumber(n2, typeConversionExact, ctx->modp()))) {
                    ctx->valueError("argument two is not a number", *a2, se);
                    res->set(TP_INT, 0);
                }
                else {
                    plusN(ctx, res, se, &n1, &n2);
                }
            }
        }
    }

    /**
     * addition of numbers
     * @param ctx			execution context
     * @param res			store for result definition set
     * @param se			syntax element id of operation
     * @param a1			argument one (must be TP_INT, TP_REAL or TP_INFINITE)
     * @param a2			argument two (must be TP_INT, TP_REAL or TP_INFINITE)
     */
    void OperationBase::plusN(ExecContext *ctx, CmplVal *res, unsigned se, CmplVal *a1, CmplVal *a2)
    {
        if (a1->t == TP_INT && a2->t == TP_INT && a1->v.i != 0 && a2->v.i != 0 && ctx->modp()->intOutOfRangeMode() != INT_OPER_OORANGE_IGNORE) {
            realType rr = (realType)(a1->v.i) + (realType)(a2->v.i);
            if (rr < INT_MAX && rr > INT_MIN)
                res->set(TP_INT, a1->v.i + a2->v.i);
            else if (ctx->modp()->intOutOfRangeMode() == INT_OPER_OORANGE_CONVERT)
                res->set(TP_REAL, rr);
            else
                ctx->valueError("integer result of operation is out of range", *a2, se);
        }
        else {
            res->copyFrom(a1, true, false);
            res->numAdd(*a2);
        }
    }

    /**
     * minus of two arbitrary values
     * @param ctx			execution context
     * @param res			store for result definition set
     * @param se			syntax element id of operation
     * @param a1			argument one (must not be an array)
     * @param a2			argument two (must not be an array)
     */
    void OperationBase::minus(ExecContext *ctx, CmplVal *res, unsigned se, CmplVal *a1, CmplVal *a2)
    {
        if (!*a1 || !*a2) {
            if (!*a1)
                neg(ctx, res, se, a2);
            else
                res->copyFrom(a1, true, false);
        }
        else if (a1->t == TP_FORMULA || a2->t == TP_FORMULA) {
            if (a2->isNumNull()) {
                res->copyFrom(a1, true, false);
            }
            else if (a1->isNumNull()) {
                neg(ctx, res, se, a2);
            }
            else {
                // check for use of shortcut for linear combination, for better performance
                if (a1->t == TP_FORMULA && a1->v.vp->refCnt() == 1) {
                    ValFormulaLinearCombOp *lc = dynamic_cast<ValFormulaLinearCombOp *>(a1->valFormula());
                    if (lc && lc->addTerm(ctx, se, a2, true)) {
                        res->copyFrom(a1, true, false);
                        return;
                    }
                }

                unsigned p1 = (a1->t == TP_FORMULA ? (dynamic_cast<OperationBase *>(a1->valFormula()))->formulaOperPrio(ICS_OPER_SUB, true, a2) : 1);
                unsigned p2 = (a2->t == TP_FORMULA ? (dynamic_cast<OperationBase *>(a2->valFormula()))->formulaOperPrio(ICS_OPER_SUB, false, a1) : 1);

                if (p1 == 0) {
                    ctx->valueError("given formula is not allowed as first argument of the operation", *a1, se);
                    res->set(TP_INT, 0);
                    return;
                }
                else if (p2 == 0) {
                    ctx->valueError("given formula is not allowed as second argument of the operation", *a2, se);
                    res->set(TP_INT, 0);
                    return;
                }
                else {
                    if (p1 >= p2)
                        (dynamic_cast<OperationBase *>(a1->valFormula()))->minusF(ctx, res, se, a2, false);
                    else
                        (dynamic_cast<OperationBase *>(a2->valFormula()))->minusF(ctx, res, se, a1, true);
                }
            }
        }
        else if (a1->t == TP_OPT_VAR || a2->t == TP_OPT_VAR) {
            if (a2->isNumNull()) {
                res->copyFrom(a1, true, false);
            }
            else if (a1->isNumNull()) {
                neg(ctx, res, se, a2);
            }
            else {
                ValFormulaLinearCombOp *f = new ValFormulaLinearCombOp(se, (a1->t == TP_OPT_VAR ? a1 : a2)->optVar());
                f->minusF(ctx, res, se, (a1->t == TP_OPT_VAR ? a2 : a1), !(a1->t == TP_OPT_VAR));
                if (!f->refCnt())
                    delete f;
            }
        }
        else if (a1->isScalarNumber() && a2->isScalarNumber()) {
            minusN(ctx, res, se, a1, a2);
        }
        else if ((a1->isSet() || a1->isTuple() || a1->isInterval()) && (a2->isSet() || a2->isTuple() || a2->isInterval())) {
            // "-" is used for set difference
            //TODO: set difference
            ctx->valueError("set difference is not implemented yet", *a1, se);
            res->set(TP_SET_EMPTY);
        }
        else {
            CmplVal n1, n2;
            if (!(a1->toNumber(n1, typeConversionExact, ctx->modp()))) {
                ctx->valueError("argument one is not a number", *a1, se);
                res->set(TP_INT, 0);
            }
            else if (!(a2->toNumber(n2, typeConversionExact, ctx->modp()))) {
                ctx->valueError("argument two is not a number", *a2, se);
                res->set(TP_INT, 0);
            }
            else {
                minusN(ctx, res, se, &n1, &n2);
            }
        }
    }

    /**
     * minus of numbers
     * @param ctx			execution context
     * @param res			store for result definition set
     * @param se			syntax element id of operation
     * @param a1			argument one (must be TP_INT, TP_REAL or TP_INFINITE)
     * @param a2			argument two (must be TP_INT, TP_REAL or TP_INFINITE)
     */
    void OperationBase::minusN(ExecContext *ctx, CmplVal *res, unsigned se, CmplVal *a1, CmplVal *a2)
    {
        if (a1->t == TP_INT && a2->t == TP_INT && a1->v.i != 0 && a2->v.i != 0 && ctx->modp()->intOutOfRangeMode() != INT_OPER_OORANGE_IGNORE) {
            realType rr = (realType)(a1->v.i) - (realType)(a2->v.i);
            if (rr < INT_MAX && rr > INT_MIN)
                res->set(TP_INT, a1->v.i - a2->v.i);
            else if (ctx->modp()->intOutOfRangeMode() == INT_OPER_OORANGE_CONVERT)
                res->set(TP_REAL, rr);
            else
                ctx->valueError("integer result of operation is out of range", *a2, se);
        }
        else {
            res->copyFrom(a1, true, false);
            res->numAdd(*a2, true);
        }
    }

    /**
     * add a value to a destination value
     * @param ctx           execution context
     * @param dst           destination value and first argument
     * @param se            syntax element id of operation
     * @param a2            second argument
     * @param neg           subtract second argument
     */
    void OperationBase::addTo(ExecContext *ctx, CmplVal *dst, unsigned se, CmplVal *a2, bool neg)
    {
        CmplVal t;
        if (neg)
            minus(ctx, &t, se, dst, a2);
        else
            plus(ctx, &t, se, dst, a2);

        dst->moveFrom(t, true);
    }

    /**
     * multiplication of two arbitrary values
     * @param ctx			execution context
     * @param res			store for result definition set
     * @param se			syntax element id of operation
     * @param a1			argument one (must not be an array)
     * @param a2			argument two (must not be an array)
     */
    void OperationBase::mult(ExecContext *ctx, CmplVal *res, unsigned se, CmplVal *a1, CmplVal *a2, bool noReuseArg)
    {
        if (a1->isNumOne(true) || a2->isNumOne(true)) {
            res->copyFrom((a1->isNumOne(true) ? a2 : a1), true, false);
        }
        else if (a1->isNumNull() || a2->isNumNull()) {
            res->set(TP_INT, 0);
        }
        else if (a1->t == TP_FORMULA || a2->t == TP_FORMULA) {
            unsigned p1 = (a1->t == TP_FORMULA ? (dynamic_cast<OperationBase *>(a1->valFormula()))->formulaOperPrio(ICS_OPER_MUL, true, a2) : 1);
            unsigned p2 = (a2->t == TP_FORMULA ? (dynamic_cast<OperationBase *>(a2->valFormula()))->formulaOperPrio(ICS_OPER_MUL, false, a1) : 1);

            if (p1 == 0) {
                ctx->valueError("given formula is not allowed as first argument of the operation", *a1, se);
                res->set(TP_INT, 0);
                return;
            }
            else if (p2 == 0) {
                ctx->valueError("given formula is not allowed as second argument of the operation", *a2, se);
                res->set(TP_INT, 0);
                return;
            }
            else {
                if (p1 >= p2)
                    (dynamic_cast<OperationBase *>(a1->valFormula()))->multF(ctx, res, se, a2, noReuseArg);
                else
                    (dynamic_cast<OperationBase *>(a2->valFormula()))->multF(ctx, res, se, a1, noReuseArg);
            }
        }
        else if (a1->t == TP_OPT_VAR || a2->t == TP_OPT_VAR) {
            if (a1->t != TP_OPT_VAR || a2->t != TP_OPT_VAR) {
                CmplVal *vn = (a1->t != TP_OPT_VAR ? a1 : a2);
                intType vi;
                realType vr;

                if (vn->toInt(vi, typeConversionExact, ctx->modp()))
                    res->set(TP_FORMULA, new ValFormulaVarOp(se, (a1->t == TP_OPT_VAR ? a1 : a2)->optVar(), vi));
                else if (vn->toReal(vr, typeConversionExact, ctx->modp()))
                    res->set(TP_FORMULA, new ValFormulaVarOp(se, (a1->t == TP_OPT_VAR ? a1 : a2)->optVar(), vr));
                else {
                    if (a1->t != TP_OPT_VAR)
                        ctx->valueError("argument one is not a number", *a1, se);
                    else
                        ctx->valueError("argument two is not a number", *a2, se);
                    res->set(TP_INT, 0);
                }
            }
            else {
                ValFormulaVarOp *f = new ValFormulaVarOp(se, (a1->t == TP_OPT_VAR ? a1 : a2)->optVar());
                f->multF(ctx, res, se, (a1->t == TP_OPT_VAR ? a2 : a1), noReuseArg);
                if (!f->refCnt())
                    delete f;
            }
        }
        else if (a1->isScalarNumber() && a2->isScalarNumber()) {
            multN(ctx, res, se, a1, a2);
        }
        else if ((a1->isSet() || a1->isTuple() || a1->isInterval()) && (a2->isSet() || a2->isTuple() || a2->isInterval())) {
            // "*" is used for intersect operation
            if (a1->isInterval() && a2->isInterval()) {
                if (Interval::intersect(*res, *a1, *a2))
                    return;
            }

            //TODO: set intersection
            ctx->valueError("set intersection is not implemented yet", *a1, se);
            res->set(TP_SET_EMPTY);
        }
        else {
            CmplVal n1, n2;
            if (!(a1->toNumber(n1, typeConversionExact, ctx->modp()))) {
                ctx->valueError("argument one is not a number", *a1, se);
                res->set(TP_INT, 0);
            }
            else if (!(a2->toNumber(n2, typeConversionExact, ctx->modp()))) {
                ctx->valueError("argument two is not a number", *a2, se);
                res->set(TP_INT, 0);
            }
            else {
                multN(ctx, res, se, &n1, &n2);
            }
        }
    }

    /**
     * multiplication of numbers
     * @param ctx			execution context
     * @param res			store for result definition set
     * @param se			syntax element id of operation
     * @param a1			argument one (must be TP_INT, TP_REAL or TP_INFINITE)
     * @param a2			argument two (must be TP_INT, TP_REAL or TP_INFINITE)
     */
    void OperationBase::multN(ExecContext *ctx, CmplVal *res, unsigned se, CmplVal *a1, CmplVal *a2)
    {
        if (a1->isNumNull() || a2->isNumNull()) {
            res->set(TP_INT, 0);
        }
        else if (a1->isNumOne(true) || a2->isNumOne(true)) {
            res->copyFrom((a1->isNumOne(true) ? a2 : a1));
        }
        else if (a1->t == TP_INFINITE || a2->t == TP_INFINITE) {
            bool s1 = (a1->useInt() ? (a1->v.i >= 0) : (a1->v.r >= 0));
            bool s2 = (a2->useInt() ? (a2->v.i >= 0) : (a2->v.r >= 0));
            res->set(TP_INFINITE, (s1 == s2 ? 1 : -1));
        }
        else {
            realType rr = 0;
            if (a1->t == TP_REAL || a2->t == TP_REAL || ctx->modp()->intOutOfRangeMode() != INT_OPER_OORANGE_IGNORE) {
                realType r1 = (a1->t == TP_REAL ? a1->v.r : (realType)(a1->v.i));
                realType r2 = (a2->t == TP_REAL ? a2->v.r : (realType)(a2->v.i));
                rr = r1 * r2;
                res->set(TP_REAL, rr);
            }

            if (a1->useInt() && a2->useInt()) {
                if (rr < INT_MAX && rr > INT_MIN)
                    res->set(TP_INT, a1->v.i * a2->v.i);
                else if (ctx->modp()->intOutOfRangeMode() == INT_OPER_OORANGE_ERROR)
                    ctx->valueError("integer result of operation is out of range", *a2, se);
            }
        }
    }

    /**
     * multiplication to a destination value
     * @param ctx           execution context
     * @param dst           destination value and first argument
     * @param se            syntax element id of operation
     * @param a2            second argument
     */
    void OperationBase::multTo(ExecContext *ctx, CmplVal *dst, unsigned se, CmplVal *a2)
    {
        CmplVal t;
        mult(ctx, &t, se, dst, a2);
        dst->moveFrom(t, true);
    }


    /**
     * division of two arbitrary values
     * @param ctx			execution context
     * @param res			store for result definition set
     * @param se			syntax element id of operation
     * @param a1			argument one (must not be an array)
     * @param a2			argument two (must not be an array)
     */
    void OperationBase::div(ExecContext *ctx, CmplVal *res, unsigned se, CmplVal *a1, CmplVal *a2)
    {
        if (!*a1 || !*a2) {
            if (!*a1) {
                CmplVal v1(TP_REAL, (realType)1);
                div(ctx, res, se, &v1, a2);
            }
            else {
                res->copyFrom(a1, true, false);
            }
        }
        else if (a1->t == TP_FORMULA || a2->t == TP_FORMULA) {
            unsigned p1 = (a1->t == TP_FORMULA ? (dynamic_cast<OperationBase *>(a1->valFormula()))->formulaOperPrio(ICS_OPER_DIV, true, a2) : 1);
            unsigned p2 = (a2->t == TP_FORMULA ? (dynamic_cast<OperationBase *>(a2->valFormula()))->formulaOperPrio(ICS_OPER_DIV, false, a1) : 1);

            if (p1 == 0) {
                ctx->valueError("given formula is not allowed as first argument of the operation", *a1, se);
                res->set(TP_INT, 0);
                return;
            }
            else if (p2 == 0) {
                ctx->valueError("given formula is not allowed as second argument of the operation", *a2, se);
                res->set(TP_INT, 0);
                return;
            }
            else {
                if (p1 >= p2)
                    (dynamic_cast<OperationBase *>(a1->valFormula()))->divF(ctx, res, se, a2, false);
                else
                    (dynamic_cast<OperationBase *>(a2->valFormula()))->divF(ctx, res, se, a1, true);
            }
        }
        else if (a1->t == TP_OPT_VAR || a2->t == TP_OPT_VAR) {
            //TODO: anders
            /*
            if (a1->t != TP_OPT_VAR || a2->t != TP_OPT_VAR) {
                CmplVal *vn = (a1->t != TP_OPT_VAR ? a1 : a2);
                intType vi;
                realType vr;

                if (vn->toInt(vi, typeConversionExact, ctx->modp()))
                    res->set(TP_FORMULA, new ValFormulaVarOp((a1->t == TP_OPT_VAR ? a1 : a2)->optVar(), vi));
                else if (vn->toReal(vr, typeConversionExact, ctx->modp()))
                    res->set(TP_FORMULA, new ValFormulaVarOp((a1->t == TP_OPT_VAR ? a1 : a2)->optVar(), vr));
                else {
                    if (a1->t != TP_OPT_VAR)
                        ctx->valueError("argument one is not a number", *a1, se);
                    else
                        ctx->valueError("argument two is not a number", *a2, se);
                    res->set(TP_INT, 0);
                }
            }
            else {
                ValFormulaVarOp *f = new ValFormulaVarOp((a1->t == TP_OPT_VAR ? a1 : a2)->optVar());
                f->multF(ctx, res, se, (a1->t == TP_OPT_VAR ? a2 : a1));
                if (!f->refCnt())
                    delete f;
            }
            */
            ctx->internalError("dvision with optimization variable not implemented");
        }
        else if (a1->isScalarNumber() && a2->isScalarNumber()) {
            divN(ctx, res, se, a1, a2);
        }
        else {
            CmplVal n1, n2;
            if (!(a1->toNumber(n1, typeConversionExact, ctx->modp()))) {
                ctx->valueError("argument one is not a number", *a1, se);
                res->set(TP_INT, 0);
            }
            else if (!(a2->toNumber(n2, typeConversionExact, ctx->modp()))) {
                ctx->valueError("argument two is not a number", *a2, se);
                res->set(TP_INT, 0);
            }
            else {
                divN(ctx, res, se, &n1, &n2);
            }
        }
    }

    /**
     * division of numbers
     * @param ctx			execution context
     * @param res			store for result definition set
     * @param se			syntax element id of operation
     * @param a1			argument one (must be TP_INT, TP_REAL or TP_INFINITE)
     * @param a2			argument two (must be TP_INT, TP_REAL or TP_INFINITE)
     */
    void OperationBase::divN(ExecContext *ctx, CmplVal *res, unsigned se, CmplVal *a1, CmplVal *a2)
    {
        if (a1->isNumNull() || a2->isNumOne()) {
            res->copyFrom(a1);
        }
        else if (a2->isNumNull()) {
            res->set(TP_INFINITE, (a1->useInt() ? (a1->v.i >= 0 ? 1 : -1) : (a1->v.r >= 0 ? 1 : -1)));
        }
        else if (a1->t == TP_INFINITE || a2->t == TP_INFINITE) {
            if (a1->t == TP_INFINITE && a2->t == TP_INFINITE)
                res->set(TP_INT, (a1->v.i * a2->v.i));
            else if (a2->t == TP_INFINITE)
                res->set(TP_INT, 0);
            else {
                realType r2 = (a2->t == TP_REAL ? a2->v.r : (realType)(a2->v.i));
                res->set(TP_INFINITE, (r2 >= 0 ? a1->v.i : -a1->v.i));
            }
        }
        else {
            realType r1 = (a1->t == TP_REAL ? a1->v.r : (realType)(a1->v.i));
            realType r2 = (a2->t == TP_REAL ? a2->v.r : (realType)(a2->v.i));
            res->set(TP_REAL, r1 / r2);
        }
    }


    /**
     * call of pow() for two arbitrary values
     * @param ctx			execution context
     * @param res			store for result definition set
     * @param se			syntax element id of operation
     * @param a1			argument one (must not be an array)
     * @param a2			argument two (must not be an array)
     */
    void OperationBase::pow(ExecContext *ctx, CmplVal *res, unsigned se, CmplVal *a1, CmplVal *a2)
    {
        if (a1->isScalarNumber() && a2->isScalarNumber()) {
            if (a2->isNumNull() || a1->isNumOne()) {
                res->set(TP_INT, 1);
            }
            else if (a2->isNumOne()) {
                res->copyFrom(a1);
            }
            else if (a1->t == TP_INFINITE || a2->t == TP_INFINITE) {
                if (a2->t == TP_INFINITE && a2->v.i < 0)
                    res->set(TP_INT, 0);
                else
                    res->copyFrom(a1);
            }
            else {
                realType r1 = (a1->t == TP_REAL ? a1->v.r : (realType)(a1->v.i));
                realType r2 = (a2->t == TP_REAL ? a2->v.r : (realType)(a2->v.i));
                res->set(TP_REAL, ::pow(r1, r2));
            }
        }
        else if ((a1->t == TP_OPT_VAR || a1->t == TP_FORMULA) && a2->isScalarNumber()) {
            intType a2i;
            if (!a2->toInt(a2i, typeConversionValue, ctx->modp()) || a2i < 0) {
                ctx->valueError("argument two must be a non-negative integer number when argument one contains an optimization variable", *a2, se);
                res->set(TP_INT, 0);
            }
            else if (a2i > 10) {
                ctx->valueError("operation not implemented for argument two greater than 10 when argument one contains an optimization variable", *a2, se);
                res->set(TP_INT, 0);
            }
            else if (a2i <= 1) {
                if (a2i == 0)
                    res->set(TP_INT, 1);
                else
                    res->copyFrom(a1);
            }
            else {
                CmplValAuto t1, t2;
                t1.copyFrom(a1);

                for (int i = 1; i < a2i; i++) {
                    mult(ctx, &t2, se, &t1, a1);
                    t1.moveFrom(t2, true);
                }

                res->moveFrom(t1);
            }
        }
        else {
            if (!a1->isScalarNumber())
                ctx->valueError("argument one is not a number", *a1, se);
            else
                ctx->valueError("argument two is not a number", *a2, se);
            res->set(TP_INT, 0);
        }
    }


    /**
     * compare of two arbitrary values
     * @param ctx			execution context
     * @param res			store for result definition set
     * @param se			syntax element id of operation
     * @param a1			argument one
     * @param a2			argument two
     * @param ge			compare for greater equal (together with le: compare for equality)
     * @param le			compare for lesser equal (together with le: compare for equality)
     * @param neg			negate comparison
     * @param cont			another compare operation follows immediately
     */
    void OperationBase::comp(ExecContext *ctx, CmplVal *res, unsigned se, CmplVal *a1, CmplVal *a2, bool ge, bool le, bool neg, bool cont)
    {
        if (a1->t == TP_FORMULA || a2->t == TP_FORMULA) {
            unsigned p1 = (a1->t == TP_FORMULA ? (dynamic_cast<OperationBase *>(a1->valFormula()))->formulaOperPrio(ICS_OPER_EQ, true, a2) : 1);
            unsigned p2 = (a2->t == TP_FORMULA ? (dynamic_cast<OperationBase *>(a2->valFormula()))->formulaOperPrio(ICS_OPER_EQ, false, a1) : 1);

            if (p1 == 0) {
                ctx->valueError("given formula is not allowed as first argument of the operation", *a1, se);
                res->set(TP_BIN, false);
                return;
            }
            else if (p2 == 0) {
                ctx->valueError("given formula is not allowed as second argument of the operation", *a2, se);
                res->set(TP_BIN, false);
                return;
            }
            else {
                if (p1 >= p2)
                    (dynamic_cast<OperationBase *>(a1->valFormula()))->compF(ctx, res, se, a2, ge, le, neg);
                else
                    (dynamic_cast<OperationBase *>(a2->valFormula()))->compF(ctx, res, se, a1, le, ge, neg);
            }
        }
        else if (a1->t == TP_OPT_VAR || a2->t == TP_OPT_VAR) {
            ValFormulaVarOp *f = new ValFormulaVarOp(se, (a1->t == TP_OPT_VAR ? a1 : a2)->optVar());
            if (a1->t == TP_OPT_VAR)
                f->compF(ctx, res, se, a2, ge, le, neg);
            else
                f->compF(ctx, res, se, a1, le, ge, neg);

            if (!f->refCnt())
                delete f;
        }
        else if (a1->isOptRow() || a2->isOptRow()) {
            ctx->valueError("constraint or objective not allowed as argument of compare operation", (a1->isOptRow() ? *a1 : *a2), se);
            res->set(TP_BIN, false);
        }
        else {
            compN(ctx, res, se, a1, a2, ge, le, neg);
        }

        if (cont) {
            ValFormulaCompContinueOp *cc = new ValFormulaCompContinueOp(se, res, a2);
            res->dispSet(TP_FORMULA, cc);
        }
    }

    /**
     * compare of two values (a1 and a2 must not be or contain optimization variables)
     * @param ctx			execution context
     * @param res			store for result definition set
     * @param se			syntax element id of operation
     * @param a1			argument one
     * @param a2			argument two
     * @param ge			compare for greater equal (together with le: compare for equality)
     * @param le			compare for lesser equal (together with le: compare for equality)
     * @param neg			negate comparison
     */
    void OperationBase::compN(ExecContext *ctx, CmplVal *res, unsigned se, CmplVal *a1, CmplVal *a2, bool ge, bool le, bool neg)
    {
        bool bge, ble;
        compNRec(ctx, se, a1, a2, (ge && le), bge, ble);

        if (ge && le)
            res->set(TP_BIN, (neg ? (!bge || !ble) : (bge && ble)));
        else if (ge)
            res->set(TP_BIN, (neg ? (ble && !bge) : bge));
        else if (le)
            res->set(TP_BIN, (neg ? (bge && !ble) : ble));
        else
            res->set(TP_BIN, false);
    }

    /**
     * compare of two values (a1 and a2 must not be or contain optimization variables)
     * @param ctx			execution context
     * @param se			syntax element id of operation
     * @param a1			argument one
     * @param a2			argument two
     * @param eq            compare only for equality
     * @param bge			return if a1 is greater equal a2
     * @param ble			return if a1 is lesser equal a2
     */
    void OperationBase::compNRec(ExecContext *ctx, unsigned se, CmplVal *a1, CmplVal *a2, bool eq, bool& bge, bool& ble)
    {
        if (*a1 == *a2) {
            bge = ble = true;
            return;
        }

        // compare as number
        CmplValAuto n1, n2;
        bool bn1 = a1->toNumber(n1, typeConversionExact, ctx->modp());
        bool bn2 = a2->toNumber(n2, typeConversionExact, ctx->modp());

        if (bn1 || bn2) {
            if (bn1 && bn2) {
                if (n1.t == TP_INFINITE || n2.t == TP_INFINITE) {
                    bge = (n1.t == TP_INFINITE && n1.v.i == 1) || (n2.t == TP_INFINITE && n2.v.i == -1);
                    ble = !bge;
                }
                else if (n1.t == TP_REAL || n2.t == TP_REAL) {
                    realType r1 = (n1.t == TP_REAL ? n1.v.r : (realType)n1.v.i);
                    realType r2 = (n2.t == TP_REAL ? n2.v.r : (realType)n2.v.i);
                    bge = (r1 >= r2);
                    ble = (r1 <= r2);
                }
                else {
                    bge = (n1.v.i >= n2.v.i);
                    ble = (n1.v.i <= n2.v.i);
                }
                return;
            }
            else if (!a1->isTuple() || !a2->isTuple()) {
                ctx->valueError("comparison of values with incompatible types", *a2, se);
                bge = ble = false;
                return;
            }
        }

        // compare as string
        else if (a1->isScalarString() || a2->isScalarString() || a1->t == TP_ITUPLE_1STR || a2->t == TP_ITUPLE_1STR) {
            if ((a1->isScalarString() || a1->t == TP_ITUPLE_1STR) && (a2->isScalarString() || a2->t == TP_ITUPLE_1STR)) {
                if (a1->t == TP_ITUPLE_1STR && a2->t == TP_ITUPLE_1STR) {
                    bge = (a1->v.i >= a2->v.i);
                    ble = (a1->v.i <= a2->v.i);
                }
                if (eq && TP_INDEX_VAL_BASE(a1->t) == TP_STR && TP_INDEX_VAL_BASE(a2->t) == TP_STR) {
                    bge = ble = (a1->v.i == a2->v.i);
                }
                else {
                    const char *s1 = a1->stringAsCharP(ctx->modp());
                    const char *s2 = a2->stringAsCharP(ctx->modp());
                    int ires = strcmp(s1, s2);
                    bge = (ires >= 0);
                    ble = (ires <= 0);
                }
                return;
            }
            else if (!a1->isTuple() || !a2->isTuple()) {
                ctx->valueError("comparison of values with incompatible types", *a2, se);
                bge = ble = false;
                return;
            }
        }

        // compare tuple values
        if (a1->isTuple() && a2->isTuple()) {
            unsigned r1 = Tuple::rank(a1);
            unsigned r2 = Tuple::rank(a2);

            if (!r1 || !r2) {
                bge = (r2 == 0);
                ble = (r1 == 0);
            }
            else if (eq && r1 != r2) {
                bge = ble = false;
            }
            else {
                unsigned rm = (r1 <= r2 ? r1 : r2);
                for (unsigned r = 0; r < rm; r++) {
                    CmplVal *tv1 = (a1->useValP() ? a1->tuple()->at(r) : a1);
                    CmplVal *tv2 = (a2->useValP() ? a2->tuple()->at(r) : a2);

                    if (((tv1->t == TP_INT || tv1->t == TP_ITUPLE_1INT) && (tv2->t == TP_INT || tv2->t == TP_ITUPLE_1INT))
                            || ((tv1->t == TP_STR || tv1->t == TP_ITUPLE_1STR) && (tv2->t == TP_STR || tv2->t == TP_ITUPLE_1STR))) {
                        bge = (tv1->v.i >= tv2->v.i);
                        ble = (tv1->v.i <= tv2->v.i);
                    }
                    else {
                        compNRec(ctx, se, tv1, tv2, eq, bge, ble);
                    }

                    if (!(bge && ble))
                        return;
                }

                if (r1 > r2)
                    ble = false;
                else if (r1 < r2)
                    bge = false;

                return;
            }
        }

        //TODO: fehlt noch: Interval, Sets (auch aus Tupel oder Intervall)
        ctx->internalError("comparison for such value types not implemented");
    }

    /**
     * compare for formula
     * @param ctx			execution context
     * @param res			store for result definition set
     * @param se			syntax element id of operation
     * @param a2			argument two
     * @param ge			compare for greater equal (together with le: compare for equality)
     * @param le			compare for lesser equal (together with le: compare for equality)
     * @param neg			negate comparison
     */
    void OperationBase::compF(ExecContext *ctx, CmplVal *res, unsigned se, CmplVal *a2, bool ge, bool le, bool neg)
    {
        ValFormulaCompareOp *f = new ValFormulaCompareOp(se, dynamic_cast<ValFormula *>(this), a2, ge, le, neg);
        res->set(TP_FORMULA, f);
    }

    /**
     * compare with "==" (or "!=") of two arbitrary values
     * @param ctx			execution context
     * @param se			syntax element id of operation
     * @param a1			argument one
     * @param a2			argument two
     * @return              both arguments are equal
     */
    bool OperationBase::compEq2(ExecContext *ctx, unsigned se, const CmplVal *a1, const CmplVal *a2)
    {
        if (*a1 == *a2)
            return true;

        // compare as number
        CmplValAuto n1, n2;
        bool bn1 = a1->toNumber(n1, typeConversionExact, ctx->modp());
        bool bn2 = a2->toNumber(n2, typeConversionExact, ctx->modp());

        if (bn1 || bn2) {
            if (!bn1 || !bn2)
                return false;

            else if (n1.t == TP_INFINITE || n2.t == TP_INFINITE)
                return false;

            else if (n1.useInt() && n2.useInt())
                return (n1.v.i == n2.v.i);

            else if (n1.t == TP_REAL && n2.t == TP_REAL)
                return (n1.v.r == n2.v.r);

            else if ((n1.t == TP_REAL && n1.v.r != (realType)(intType)(n1.v.r)) || (n2.t == TP_REAL && n2.v.r != (realType)(intType)(n2.v.r)))
                return false;

            else
                return ((n1.t == TP_REAL ? (intType)(n1.v.r) : n1.v.i) == (n2.t == TP_REAL ? (intType)(n2.v.r) : n2.v.i));
        }

        // compare as string
        bn1 = (a1->isScalarString() || a1->t == TP_ITUPLE_1STR);
        bn2 = (a2->isScalarString() || a2->t == TP_ITUPLE_1STR);
        if (bn1 || bn2) {
            if (!bn1 || !bn2)
                return false;

            else if (n1.useInt() && n2.useInt())
                return (n1.v.i == n2.v.i);

            else {
                const char *s1 = a1->stringAsCharP(ctx->modp());
                const char *s2 = a2->stringAsCharP(ctx->modp());
                return (strcmp(s1, s2) == 0);
            }
        }

        //TODO: Intervall, Tupel, Set, Formel/OptVar

        return false;
    }


    /**
     * operation for optimization direction
     * @param ctx			execution context
     * @param res			store for result value
     * @param se			syntax element id of operation
     * @param a1			argument one
     * @param a2			argument two
     */
    void OperationBase::objTo(ExecContext *ctx, CmplVal *res, unsigned se, CmplVal *a1, CmplVal *a2)
    {
        // argument two must be a function object providing optDir()
        int od = (a2->t == TP_FUNCTION ? a2->valFunction()->optDir() : 0);
        if (!od) {
            ctx->valueError("argument two must be one of the functions max or min", *a2, se);
            od = 1;
        }

        ValFormulaObjectiveOp *f = new ValFormulaObjectiveOp(se, a1, (od > 0));
        res->set(TP_FORMULA, f);
    }


    /**
     * logical And or Or operation
     * @param ctx			execution context
     * @param res			store for result value
     * @param se			syntax element id of operation
     * @param a1			argument one
     * @param a2			argument two
     * @param op            connection by logical Or or by logical And
     */
    void OperationBase::logAndOr(ExecContext *ctx, CmplVal *res, unsigned se, CmplVal *a1, CmplVal *a2, unsigned short op)
    {
        if (a1->t == TP_FORMULA || a2->t == TP_FORMULA) {
            unsigned p1 = (a1->t == TP_FORMULA ? (dynamic_cast<OperationBase *>(a1->valFormula()))->formulaOperPrio(op, true, a2) : 1);
            unsigned p2 = (a2->t == TP_FORMULA ? (dynamic_cast<OperationBase *>(a2->valFormula()))->formulaOperPrio(op, false, a1) : 1);

            if (p1 == 0) {
                ctx->valueError("given formula is not allowed as first argument of the operation", *a1, se);
                res->set(TP_BIN, false);
                return;
            }
            else if (p2 == 0) {
                ctx->valueError("given formula is not allowed as second argument of the operation", *a2, se);
                res->set(TP_BIN, false);
                return;
            }
            else {
                if (p1 >= p2)
                    (dynamic_cast<OperationBase *>(a1->valFormula()))->logAndOrF(ctx, res, se, a2, (op == ICS_OPER_OR));
                else
                    (dynamic_cast<OperationBase *>(a2->valFormula()))->logAndOrF(ctx, res, se, a1, (op == ICS_OPER_OR));
            }
        }
        else if ((a1->t == TP_OPT_VAR && a1->optVar()->binVar()) || (a2->t == TP_OPT_VAR && a2->optVar()->binVar())) {
            ValFormulaVarOp *f = new ValFormulaVarOp(se, (a1->t == TP_OPT_VAR && a1->optVar()->binVar() ? a1 : a2)->optVar());
            f->logAndOrF(ctx, res, se, (a1->t == TP_OPT_VAR && a1->optVar()->binVar() ? a2 : a1), (op == ICS_OPER_OR));
            if (!f->refCnt())
                delete f;
        }
        else {
            bool b1, b2;
            bool ib1 = a1->toBool(b1, typeConversionAll, ctx->modp());
            bool ib2 = a2->toBool(b2, typeConversionAll, ctx->modp());

            if (ib1 && ib2) {
                if (op == ICS_OPER_OR)
                    res->set(TP_BIN, (b1 || b2));
                else
                    res->set(TP_BIN, (b1 && b2));
            }
            else {
                ctx->valueError("arguments for logical operation must have a boolean value", (ib1 ? *a2 : *a1), se);
            }
        }
    }

    /**
     * logical And or Or operation
     * @param ctx			execution context
     * @param res			store for result value
     * @param se			syntax element id of operation
     * @param a2			argument two
     * @param logOr         true: connection by logical Or / false: connection by logical And
     */
    void OperationBase::logAndOrF(ExecContext *ctx, CmplVal *res, unsigned se, CmplVal *a2, bool logOr)
    {
        if (a2->t == TP_FORMULA || (a2->t == TP_OPT_VAR && a2->optVar()->binVar())) {
            res->set(TP_FORMULA, new ValFormulaLogConOp(se, dynamic_cast<ValFormula *>(this), (a2->t == TP_FORMULA ? a2->valFormula() : new ValFormulaVarOp(se, a2->optVar())), logOr), true);
        }
        else {
            bool b2;
            if (a2->toBool(b2, typeConversionAll, ctx->modp())) {
                if ((b2 && !logOr) || (!b2 && logOr))
                    res->set(TP_FORMULA, dynamic_cast<ValFormula *>(this), true);
                else
                    res->set(TP_BIN, b2);
            }
            else {
                ctx->valueError("argument must have a boolean value", *a2, se);
                res->set(TP_BIN, false);
            }
        }
    }

    /**
     * check whether tuple is in set
     * @param ctx			execution context
     * @param res			store for result value
     * @param se			syntax element id of operation
     * @param a1			argument one
     * @param a2			argument two
     * @param opIn          operation "in" (true) or "of" (false)
     */
    void OperationBase::tplInSet(ExecContext *ctx, CmplVal *res, unsigned se, CmplVal *a1, CmplVal *a2, bool opIn)
    {
        CmplValAuto set;

        if (opIn && a2->isInterval()) {
            // check whether number is within interval
            if (a1->isScalarNumber()) {
                res->set(TP_BIN, Interval::numInInterval(*a2, *a1));
            }
            else if (a1->isTuple() && Tuple::rank(*a1) == 1) {
                CmplValAuto n;
                if (a1->t == TP_ITUPLE || a1->t == TP_LIST_TUPLE)
                    n.copyFrom(a1->t == TP_ITUPLE ? a1->tuple()->at(0) : &((ctx->stackCur() - a1->v.i)->val()));
                else if (a1->t == TP_ITUPLE_1INT)
                    n.set(TP_INT, a1->v.i);

                res->set(TP_BIN, Interval::numInInterval(*a2, n));
            }
            else {
                res->set(TP_BIN, false);
            }

            return;
        }

        if (!SetUtil::setForInOrOf(ctx, set, *a2, opIn, opIn)) {
            ctx->valueError((opIn ? "second argument must be a set" : "second argument must be an array"), *a2, se);
            res->set(TP_BIN, false);
            return;
        }

        if (opIn && (set.isTuple() || set.t == TP_SET_INF_TPL)) {
            try {
                TupleMatching tm(ctx, TupleMatching::matchIn, *a1, set, true);
                CmplValAuto t;
                res->set(TP_BIN, tm.match(t));
            }
            catch (TupleMatchingValueException& ex) {
                res->set(TP_BIN, false);
                if (ex.invSrc())
                    ctx->valueError("first argument must be an index value or tuple", *a1, se);
                else
                    ctx->valueError("second argument must be a tuple or set", *a2, se);
            }
            return;
        }

        if (set.t == TP_SET_ALL || set.t == TP_SET_EMPTY || a1->t == TP_EMPTY) {
            res->set(TP_BIN, (set.t == TP_SET_ALL && a1->t != TP_EMPTY));
            return;
        }

        CmplValAuto tpl;
        if (a1->isITuple()) {
            tpl.copyFrom(a1);
        }
        else if (a1->isScalarIndex()) {
            tpl.set(TP_INDEX_VAL_TUPLE(a1->t), a1->v.i);
        }
        else if (a1->isScalarValue()) {
            intType i;
            if (a1->toInt(i, typeConversionValue, ctx->modp()))
                tpl.set(TP_ITUPLE_1INT, i);
        }
        else if (a1->isTuple() && a1->useValP()) {
            if (!TupleUtil::toIndexTuple(ctx, tpl, a1->tuple()))
                tpl.dispUnset();
        }

        if (!tpl.isITuple()) {
            ctx->valueError("first argument must be an index value or tuple", *a1, se);
            res->set(TP_BIN, false);
            return;
        }

        unsigned long num;
        bool b = SetUtil::tupleInSet(ctx, set, tpl, num);
        res->set(TP_BIN, b);
    }


    /**
     * execute tuple matching operation "*>"
     * @param ctx			execution context
     * @param res			store for result value
     * @param se			syntax element id of operation
     * @param a1			argument one
     * @param a2			argument two
     */
    void OperationBase::tplMatch(ExecContext *ctx, CmplVal *res, unsigned se, CmplVal *a1, CmplVal *a2)
    {
        try {
            TupleMatching tm(ctx, TupleMatching::matchReduce, *a1, *a2, true);
            tm.match(*res);
        }
        catch (TupleMatchingValueException& ex) {
            res->set(TP_SET_EMPTY);
            if (ex.invSrc())
                ctx->valueError("first argument must be a finite set", *a1, se);
            else
                ctx->valueError("second argument must be a tuple or set", *a2, se);
        }
    }


    /**
     * execute unary operation "/" or "*", for marking a set as non-free or free
     * @param ctx			execution context
     * @param res			store for result value
     * @param se			syntax element id of operation
     * @param a1			argument value
     * @param mnf			true: mark as non-free / false: mark as free
     */
    void OperationBase::setMarkNF(ExecContext *ctx, CmplVal *res, unsigned se, CmplVal *a1, bool mnf)
    {
        if (a1->isTuple() && a1->useValP()) {
            // recursive for all tuple parts
            Tuple *tpl1 = a1->tuple();
            unsigned r = tpl1->rank();

            Tuple *tplr;
            res->set(TP_TUPLE, (tplr = new Tuple(r)));

            for (unsigned i = 0; i < r; i++) {
                setMarkNF(ctx, tplr->at(i), se, tpl1->at(i), mnf);
            }
        }
        else {
            if (a1->t == TP_STRINGP)
                a1->stringPToStr(ctx->modp());

            if (a1->isInterval())
                SetUtil::constructFromInterval(ctx, *res, *a1);
            else
                res->copyFrom(a1);

            if (res->isSet() || res->isScalarIndex() || (res->isTuple() && !res->useValP()))
                SetBase::setMarkNF(*res, mnf);
            else
                ctx->valueError("argument must be a set, tuple or index value", *a1, se);
        }
    }



    /************** ValFormulaVarOp **********/

    /**
     * get priority of calling the binary operation at this formula object
     * @param op			operation code (binary operation)
     * @param fa			this is the first argument of the operation
     * @param oa			other argument of the operation
     * @return				priority level to call execution of the operation at this instead of the other argument (at least 2), or 0 if the formula doesn't implement the operation
     */
    unsigned ValFormulaVarOp::formulaOperPrio(unsigned short op, bool fa, CmplVal *oa)
    {
        if (op == ICS_OPER_AND || op == ICS_OPER_OR)
            return (isBool() ? 2 : 0);
        else
            return 2;
    }


    /**
     * negation of formula
     * @param ctx			execution context
     * @param res			store for result definition set
     * @param se			syntax element id of operation
     */
    void ValFormulaVarOp::negF(ExecContext *ctx, CmplVal *res, unsigned se)
    {
        ValFormulaVarOp *f = (refCnt() > 1 ? new ValFormulaVarOp(this) : this);
        res->set(TP_FORMULA, f, true);

        if (f->_factor) {
            if (f->_factor.useInt())
                f->_factor.v.i = -f->_factor.v.i;
            else
                f->_factor.v.r = -f->_factor.v.r;
        }
        else {
            f->_factor.set(TP_INT, -1);
        }
    }

    /**
     * addition of formulas
     * @param ctx			execution context
     * @param res			store for result definition set
     * @param se			syntax element id of operation
     * @param a2			argument two
     */
    void ValFormulaVarOp::plusF(ExecContext *ctx, CmplVal *res, unsigned se, CmplVal *a2)
    {
        ValFormulaLinearCombOp *f = new ValFormulaLinearCombOp(se, this);
        f->plusF(ctx, res, se, a2);
        if (!f->refCnt())
            delete f;
    }

    /**
     * minus of formulas
     * @param ctx			execution context
     * @param res			store for result definition set
     * @param se			syntax element id of operation
     * @param a2			argument two
     * @param rev			substract this from a2
     */
    void ValFormulaVarOp::minusF(ExecContext *ctx, CmplVal *res, unsigned se, CmplVal *a2, bool rev)
    {
        ValFormulaLinearCombOp *f = new ValFormulaLinearCombOp(se, this);
        f->minusF(ctx, res, se, a2, rev);
        if (!f->refCnt())
            delete f;
    }

    /**
     * multiplication of formulas
     * @param ctx			execution context
     * @param res			store for result definition set
     * @param se			syntax element id of operation
     * @param a2			argument two
     * @param noReuseArg    don't reuse argument even if it has only one reference
     */
    void ValFormulaVarOp::multF(ExecContext *ctx, CmplVal *res, unsigned se, CmplVal *a2, bool noReuseArg)
    {
        if (a2->t == TP_FORMULA || a2->t == TP_OPT_VAR) {
            ValFormulaVarProdOp *f = new ValFormulaVarProdOp(se, this);
            f->multF(ctx, res, se, a2, noReuseArg);
            if (!f->refCnt())
                delete f;
        }
        else {
            CmplVal n2;
            if (!(a2->toNumber(n2, typeConversionExact, ctx->modp()))) {
                ctx->valueError("argument is not a number", *a2, se);
            }
            else {
                ValFormulaVarOp *f = (noReuseArg || refCnt() > 1 ? new ValFormulaVarOp(this) : this);
                res->set(TP_FORMULA, f, true);

                if (_factor)
                    OperationBase::multN(ctx, &(f->_factor), se, &_factor, &n2);
                else
                    f->_factor.copyFrom(n2);
            }
        }
    }



    /************** ValFormulaVarProdOp **********/

    /**
     * removes variables from product and replace by product of the two given variables
     * @param v1        first variable
     * @param v2        second variable
     * @param keepF     true: keep factor unchanged / false: remove factor
     */
    void ValFormulaVarProdOp::setNewVarProd(OptVar *v1, OptVar *v2, bool keepF)
    {
        if (!keepF)
            _factor.dispUnset();

        for (unsigned i = 0; i < _optVars.size(); i++)
            _optVars[i].dispose();

        _optVars.clear();

        _optVars.push_back(CmplVal(TP_OPT_VAR, v1));
        _optVars.push_back(CmplVal(TP_OPT_VAR, v2));
    }

    /**
     * append variable product to terms of ValFormulaLinearComb
     * @param terms     terms of ValFormulaLinearComb
     * @param neg       negate term
     */
    void ValFormulaVarProdOp::pushToLinearCombTerms(vector<CmplVal> &terms, bool neg)
    {
        if (!_factor) {
            if (neg)
                terms.push_back(CmplVal(TP_INT, (intType)-1));
            else
                terms.push_back(CmplVal());

            terms.push_back(CmplVal(TP_FORMULA, this));
        }
        else {
            // split factor
            ValFormulaVarProdOp *f = new ValFormulaVarProdOp(this);
            f->_factor.dispUnset();

            if (neg)
                terms.push_back(_factor.useInt() ? CmplVal(TP_INT, -_factor.v.i) : CmplVal(TP_REAL, -_factor.v.r));
            else
                terms.push_back(CmplVal(_factor));

            terms.push_back(CmplVal(TP_FORMULA, f));
        }
    }

    /**
     * get priority of calling the binary operation at this formula object
     * @param op			operation code (binary operation)
     * @param fa			this is the first argument of the operation
     * @param oa			other argument of the operation
     * @return				priority level to call execution of the operation at this instead of the other argument (at least 2), or 0 if the formula doesn't implement the operation
     */
    unsigned ValFormulaVarProdOp::formulaOperPrio(unsigned short op, bool fa, CmplVal *oa)
    {
        if (op == ICS_OPER_AND || op == ICS_OPER_OR)
            return (isBool() ? 5 : 0);
        else
            return 5;
    }


    /**
     * negation of formula
     * @param ctx			execution context
     * @param res			store for result definition set
     * @param se			syntax element id of operation
     */
    void ValFormulaVarProdOp::negF(ExecContext *ctx, CmplVal *res, unsigned se)
    {
        ValFormulaVarProdOp *f = (refCnt() > 1 ? new ValFormulaVarProdOp(this) : this);
        res->set(TP_FORMULA, f, true);

        if (f->_factor) {
            if (f->_factor.useInt())
                f->_factor.v.i = -f->_factor.v.i;
            else
                f->_factor.v.r = -f->_factor.v.r;
        }
        else {
            f->_factor.set(TP_INT, -1);
        }
    }

    /**
     * addition of formulas
     * @param ctx			execution context
     * @param res			store for result definition set
     * @param se			syntax element id of operation
     * @param a2			argument two
     */
    void ValFormulaVarProdOp::plusF(ExecContext *ctx, CmplVal *res, unsigned se, CmplVal *a2)
    {
        ValFormulaLinearCombOp *f = new ValFormulaLinearCombOp(se, this);
        f->plusF(ctx, res, se, a2);
        if (!f->refCnt())
            delete f;
    }

    /**
     * minus of formulas
     * @param ctx			execution context
     * @param res			store for result definition set
     * @param se			syntax element id of operation
     * @param a2			argument two
     * @param rev			substract this from a2
     */
    void ValFormulaVarProdOp::minusF(ExecContext *ctx, CmplVal *res, unsigned se, CmplVal *a2, bool rev)
    {
        ValFormulaLinearCombOp *f = new ValFormulaLinearCombOp(se, this);
        f->minusF(ctx, res, se, a2, rev);
        if (!f->refCnt())
            delete f;
    }

    /**
     * multiplication of formulas
     * @param ctx			execution context
     * @param res			store for result definition set
     * @param se			syntax element id of operation
     * @param a2			argument two
     * @param noReuseArg    don't reuse argument even if it has only one reference
     */
    void ValFormulaVarProdOp::multF(ExecContext *ctx, CmplVal *res, unsigned se, CmplVal *a2, bool noReuseArg)
    {
        ValFormulaVarProdOp *f = NULL;
        CmplVal n2;

        if (a2->t == TP_FORMULA || a2->t == TP_OPT_VAR || a2->toNumber(n2, typeConversionExact, ctx->modp())) {
            f = (noReuseArg || refCnt() > 1 ? new ValFormulaVarProdOp(this) : this);
            res->set(TP_FORMULA, f, true);

            if (a2->t == TP_OPT_VAR) {
                f->_optVars.push_back(CmplVal(a2));
            }
            else if (a2->t == TP_FORMULA) {
                ValFormulaVar *fv = dynamic_cast<ValFormulaVar *>(a2->v.vp);
                if (fv) {
                    if (fv->termVar(0))
                        f->_optVars.push_back(CmplVal(fv->termVar(0)));

                    if (fv->termFac(0))
                        n2.copyFrom(fv->termFac(0));
                }
                else {
                    ValFormulaVarProdOp *vp = dynamic_cast<ValFormulaVarProdOp *>(a2->v.vp);
                    if (vp) {
                        for (unsigned i = 0; i < vp->_optVars.size(); i++)
                            f->_optVars.push_back(CmplVal(vp->_optVars[i]));

                        if (vp->_factor)
                            n2.copyFrom(vp->_factor);
                    }
                    else {
                        ctx->valueError("formula value not suitable for operation", *a2, se);
                    }
                }
            }

            if (n2) {
                if (_factor)
                    OperationBase::multN(ctx, &(f->_factor), se, &_factor, &n2);
                else
                    f->_factor.copyFrom(n2);
            }
        }
        else {
            ctx->valueError("argument is not a number", *a2, se);
        }
    }



    /************** ValFormulaLinearCombOp **********/

    /**
     * get priority of calling the binary operation at this formula object
     * @param op			operation code (binary operation)
     * @param fa			this is the first argument of the operation
     * @param oa			other argument of the operation
     * @return				priority level to call execution of the operation at this instead of the other argument (at least 2), or 0 if the formula doesn't implement the operation
     */
    unsigned ValFormulaLinearCombOp::formulaOperPrio(unsigned short op, bool fa, CmplVal *oa)
    {
        if (op == ICS_OPER_AND || op == ICS_OPER_OR)
            return 0;
        else
            return 10;
    }


    /**
     * negation of formula
     * @param ctx			execution context
     * @param res			store for result definition set
     * @param se			syntax element id of operation
     */
    void ValFormulaLinearCombOp::negF(ExecContext *ctx, CmplVal *res, unsigned se)
    {
        ValFormulaLinearCombOp *f = (refCnt() > 1 ? new ValFormulaLinearCombOp(this) : this);
        res->set(TP_FORMULA, f, true);

        if (f->_constTerm) {
            if (f->_constTerm.useInt())
                f->_constTerm.v.i = -f->_constTerm.v.i;
            else
                f->_constTerm.v.r = -f->_constTerm.v.r;
        }

        for (unsigned i = 0; i < _terms.size(); i += 2) {
            CmplVal& v = _terms[i];
            if (v) {
                if (v.useInt())
                    v.v.i = -v.v.i;
                else
                    v.v.r = -v.v.r;
            }
            else {
                v.set(TP_INT, -1);
            }
        }
    }

    /**
     * addition of formulas
     * @param ctx			execution context
     * @param res			store for result definition set
     * @param se			syntax element id of operation
     * @param a2			argument two
     */
    void ValFormulaLinearCombOp::plusF(ExecContext *ctx, CmplVal *res, unsigned se, CmplVal *a2)
    {
        plusMinusF(ctx, res, true, se, a2, false);
    }

    /**
     * minus of formulas
     * @param ctx			execution context
     * @param res			store for result definition set
     * @param se			syntax element id of operation
     * @param a2			argument two
     * @param rev			substract this from a2
     */
    void ValFormulaLinearCombOp::minusF(ExecContext *ctx, CmplVal *res, unsigned se, CmplVal *a2, bool rev)
    {
        if (rev) {
            ValFormulaLinearCombOp *f = (refCnt() > 1 ? new ValFormulaLinearCombOp(this) : this);
            CmplValAuto a1;

            f->negF(ctx, &a1, se);
			(dynamic_cast<ValFormulaLinearCombOp *>(a1.valFormula()))->plusF(ctx, res, se, a2);

            if (!f->refCnt())
                delete f;
        }
        else {
            plusMinusF(ctx, res, true, se, a2, true);
        }
    }

    /**
     * add or substract of formulas
     * @param ctx			execution context
     * @param res			store for result definition set
     * @param newres        true: fill res with new result object
     * @param se			syntax element id of operation
     * @param a2			argument two
     * @param minus			add or substract
     */
    void ValFormulaLinearCombOp::plusMinusF(ExecContext *ctx, CmplVal *res, bool newres, unsigned se, CmplVal *a2, bool minus)
    {
        ValFormulaLinearCombOp *f;
        if (newres) {
            f = (refCnt() > 1 ? new ValFormulaLinearCombOp(this) : this);
            res->set(TP_FORMULA, f, true);
        }
        else {
            f = dynamic_cast<ValFormulaLinearCombOp *>(res->v.vp);
        }

        if (!f->addTerm(ctx, se, a2, minus)) {
            if (a2->t == TP_FORMULA)
                ctx->valueError("formula value not suitable for operation", *a2, se);
            else
                ctx->valueError("argument is not a number", *a2, se);
        }
    }

    /**
     * try to add argument two as a term into this
     * @param ctx			execution context
     * @param se			syntax element id of operation
     * @param a2			argument two
     * @param minus			add or substract
     * @return              true if success, false if argument two is not suitable
     */
    bool ValFormulaLinearCombOp::addTerm(ExecContext *ctx, unsigned se, CmplVal *a2, bool minus)
    {
        if (a2->t == TP_OPT_VAR) {
            if (minus)
                _terms.push_back(CmplVal(TP_INT, (intType)-1));
            else
                _terms.push_back(CmplVal());

            _terms.push_back(CmplVal(a2));
        }

        else if (a2->t == TP_FORMULA) {
            ValFormulaLinearCombBase *lc = dynamic_cast<ValFormulaLinearCombBase *>(a2->v.vp);
            if (lc) {
                CmplVal *ct = lc->constTerm();
                if (ct && *ct) {
                    CmplVal n1(_constTerm);
                    if (!n1)
                        n1.set(TP_INT, 0);

                    if (minus)
                        OperationBase::minusN(ctx, &(_constTerm), se, &n1, ct);
                    else
                        OperationBase::plusN(ctx, &(_constTerm), se, &n1, ct);
                }

                for (unsigned i = 0; i < lc->termCnt(); i++) {
                    CmplVal *tf = lc->termFac(i);
                    CmplVal *tv = lc->termVar(i);

                    if (minus)
                        _terms.push_back(tf ? (tf->useInt() ? CmplVal(TP_INT, -tf->v.i) : CmplVal(TP_REAL, -tf->v.r)) : CmplVal(TP_INT, (intType)-1));
                    else
                        _terms.push_back(tf ? CmplVal(tf) : CmplVal());

                    _terms.push_back(CmplVal(tv));
                }

                if (_linear && !(lc->linearConstraint()))
                    _linear = false;
            }
            else {
                ValFormulaVarProdOp *vp = dynamic_cast<ValFormulaVarProdOp *>(a2->v.vp);
                if (vp) {
                    vp->pushToLinearCombTerms(_terms, minus);
                    _linear = false;
                }
                else {
                    return false;
                }
            }
        }

        else {
            CmplVal n2;
            if (!(a2->toNumber(n2, typeConversionExact, ctx->modp()))) {
                return false;
            }
            else {
                CmplVal n1(_constTerm);
                if (!n1)
                    n1.set(TP_INT, 0);

                if (minus)
                    OperationBase::minusN(ctx, &_constTerm, se, &n1, &n2);
                else
                    OperationBase::plusN(ctx, &_constTerm, se, &n1, &n2);
            }
        }

        return true;
    }

    /**
     * multiplication of formulas
     * @param ctx			execution context
     * @param res			store for result definition set
     * @param se			syntax element id of operation
     * @param a2			argument two
     * @param noReuseArg    don't reuse argument even if it has only one reference
     */
    void ValFormulaLinearCombOp::multF(ExecContext *ctx, CmplVal *res, unsigned se, CmplVal *a2, bool noReuseArg)
    {
        if (a2->t == TP_FORMULA || a2->t == TP_OPT_VAR) {
            ValFormulaLinearCombOp *lc = (a2->t == TP_FORMULA ? dynamic_cast<ValFormulaLinearCombOp *>(a2->v.vp) : NULL);
            ValFormulaLinearCombOp *f;

            if (lc) {
                if (!(lc->_constTerm.isNumNull(true))) {
                    multWithN(ctx, res, se, &(lc->_constTerm), true);
                    f = dynamic_cast<ValFormulaLinearCombOp *>(res->v.vp);
                }
                else {
                    f = new ValFormulaLinearCombOp(se);
                    res->set(TP_FORMULA, f, true);
                }

                if (!_constTerm.isNumNull(true)) {
                    // f->_constTerm = this->_constTerm * lc->_constTerm is computed here another time, so delete the already computed value
                    if (f->_constTerm)
                        f->_constTerm.dispose();

                    CmplValAuto t1;
                    OperationBase::mult(ctx, &t1, se, a2, &_constTerm, true);
                    f->plusMinusF(ctx, res, false, se, &t1, false);
                }

                if (lc->_terms.size() > 0 && _terms.size() > 0) {
                    for (unsigned i = 0; i < lc->_terms.size(); i += 2) {
                        multTermsVarAdd(ctx, se, f->_terms, &(lc->_terms[i+1]), &(lc->_terms[i]));
                    }

                    f->_linear = false;
                }
            }

            else if (a2->t == TP_OPT_VAR || dynamic_cast<ValFormulaVar *>(a2->v.vp) || dynamic_cast<ValFormulaVarProd *>(a2->v.vp)) {
                f = new ValFormulaLinearCombOp(se);
                res->set(TP_FORMULA, f, true);

                if (!_constTerm.isNumNull(true)) {
                    CmplValAuto t1;
                    OperationBase::mult(ctx, &t1, se, a2, &_constTerm, true);
                    f->plusMinusF(ctx, res, false, se, &t1, false);
                }

                if (_terms.size() > 0) {
                    multTermsVarAdd(ctx, se, f->_terms, a2, NULL);
                    f->_linear = false;
                }
            }

            else {
                ctx->valueError("formula value not suitable for operation", *a2, se);
            }

            // OK wenn ValFormulaVar oder ValFormulaVarProd:
            //      Neues leeres Ergebnis initialisieren
            //      Wenn _constTerm und _constTerm != 0:
            //          Neuen Term in Ergebnis aufnehmen, mit a2 * _constTerm (Mult auf a2 aufrufen und dann als Term einfuegen)
            //      Funktion fuer Multiplikation mit Var aufrufen:
            //          Fuer jeden Term in this:
            //              Neuen Term in Ergebnis aufnehmen, mit a2 * Faktor * Restterm (Mult auf a2 aufrufen und dann als Term einfuegen)
            // OK Wenn ValFormulaLinearCombOp:
            //      Wenn a2->_constTerm und a2->_constTerm != 0:
            //          Neues Ergebnis initialisieren, mit this * a2->_constTerm (Mult fuer ValFormulaLinearCombOp * Zahl aufrufen)
            //      Sonst:
            //          Neues leeres Ergebnis initialisieren
            //      Fuer jeden Term in a2:
            //          Teilergebnis ermitteln fuer this * a2->term(i) (ueber obige Funktion)
            //          Terme aus dem Teilergebnis ins Ergebnis einfuegen (Teilergebnis gar nicht materialisieren, sondern Terme direkt durch obige Funktion einfuegen)

            // Benoetigte Funktionen:
            //  OK     multWithN: Multipliziert Zahl, Code wie bisher hier unten
            //      multVar:
            //          Parameter:
            //              Ergebnisterme: vector, in den Ergebnisterme der Multiplikation this * Var zu stellen sind
            //              Var: entweder TP_OPT_VAR oder ValFormulaVar oder ValFormulaVarProd
            //              Faktor: zusaetzlicher Faktor (Zahl)

            // OK (in multN und anderswo): Multiplikation mit 0 speziell behandeln, ergibt Zahl 0 (und nie Formel)
            //          (so dass Faktor in ValFormulaVar oder ValFormulaVarProd oder in Term von ValFormulaLinearComb nie 0 sein kann)
        }
        else {
            CmplVal n2;
            if (a2->toNumber(n2, typeConversionExact, ctx->modp()))
                multWithN(ctx, res, se, a2, noReuseArg);
            else
                ctx->valueError("argument is not a number", *a2, se);
        }
    }

    /**
     * multiplication with a number
     * @param ctx			execution context
     * @param res			store for result definition set
     * @param se			syntax element id of operation
     * @param a2			argument two (must be TP_INT, TP_REAL or TP_INFINITE)
     * @param noReuseArg    don't reuse argument even if it has only one reference
     */
    void ValFormulaLinearCombOp::multWithN(ExecContext *ctx, CmplVal *res, unsigned se, CmplVal *a2, bool noReuseArg)
    {
        ValFormulaLinearCombOp *f = (noReuseArg || refCnt() > 1 ? new ValFormulaLinearCombOp(this) : this);
        res->set(TP_FORMULA, f, true);

        if (_constTerm)
            OperationBase::multN(ctx, &(f->_constTerm), se, &_constTerm, a2);

        for (unsigned i = 0; i < _terms.size(); i += 2) {
            CmplVal& tf = f->_terms[i];
            if (tf)
                OperationBase::multN(ctx, &tf, se, &(_terms[i]), a2);
            else
                tf.copyFrom(a2);
        }
    }

    /**
     * multiplication of the terms in this with optimization variables and appending resulting terms
     * @param ctx			execution context
     * @param se			syntax element id of operation
     * @param resTerms		append resulting terms to this vector
     * @param v2			optimization variable (must be TP_OPT_VAR or TP_FORMULA containing ValFormulaVar or ValFormulaVarProd)
     * @param f2            additional numeric factor (must be TP_INT or TP_REAL) or NULL if no such factor
     */
    void ValFormulaLinearCombOp::multTermsVarAdd(ExecContext *ctx, unsigned se, vector<CmplVal> &resTerms, CmplVal *v2, CmplVal *f2)
    {
        for (unsigned i = 0; i < _terms.size(); i += 2) {
            CmplValAuto t1;
            OperationBase::mult(ctx, &t1, se, v2, &_terms[i+1], true);

            CmplVal fact(_terms[i]);
            if (f2 && *f2)
                OperationBase::multN(ctx, &fact, se, &fact, f2);

            if (!fact.isNumOne(true)) {
                CmplValAuto t2;
                t2.moveFrom(t1);
                OperationBase::mult(ctx, &t1, se, &t2, &fact);
            }

            if (!t1.isNumNull()) {
                ValFormulaVarProdOp *vp;
                if (t1.t == TP_FORMULA && (vp = dynamic_cast<ValFormulaVarProdOp *>(t1.v.vp))) {
                    vp->pushToLinearCombTerms(resTerms);
                }
                else {
                    ctx->internalError("unsupported type for product of variables");
                }
            }
        }
    }


    /**
     * check whether formula is linear and set flag for it, change also terms from ValFormulaVar to TP_OPT_VAR
     */
    void ValFormulaLinearCombOp::checkLinear(ExecContext *ctx)
    {
        // if a term is ValFormulaVar, then split it in factor and TP_OPT_VAR
        for (unsigned i = 1; i < _terms.size(); i += 2) {
            CmplVal& v = _terms[i];
            if (v.t == TP_FORMULA) {
                ValFormulaVar *vfv = dynamic_cast<ValFormulaVar *>(v.valFormula());
                if (vfv) {
                    CmplVal *fact = vfv->termFac(0);
                    if (fact && *fact) {
                        CmplVal& ofact = _terms[i-1];
                        if (!ofact)
                            ofact.copyFrom(fact);
                        else
                            OperationBase::multTo(ctx, &ofact, vfv->syntaxElem(), fact);
                    }

                    CmplValAuto ov(vfv->termVar(0));
                    v.moveFrom(ov, true);
                }
            }
        }

        // this is linear if now all terms are TP_OPT_VAR
        for (unsigned i = 1; i < _terms.size(); i += 2) {
            CmplVal& v = _terms[i];
            if (v.t != TP_OPT_VAR) {
                _linear = false;
                return;
            }
        }

        _linear = true;
    }

    /**
     * get factor and variable of a term within a formula
     * @param frm           formula
     * @param i             number of term
     * @param tfac          return of factor within the term (or TP_EMPTY if factor is 1)
     * @param tvar          return of variable (can also be product of variables) within the term
     * @param tlin          return whether term is linear (false if tvar contains a product of variables)
     * @return              true if result is returned / false if arguments are not suitable
     */
    bool ValFormulaLinearCombOp::getTermFacVar(ValFormula *frm, unsigned i, CmplVal& tfac, CmplVal& tvar, bool& tlin)
    {
        ValFormulaLinearComb *flc = dynamic_cast<ValFormulaLinearComb *>(frm);
        if (flc) {
            if (i < 0 || i >= flc->termCnt())
                return false;

            tfac.copyFrom(flc->getPart(2*i + 1));
            tvar.copyFrom(flc->getPart(2*i + 2));
            tlin = (tvar.t == TP_OPT_VAR);
            return true;
        }

        ValFormulaVar *fv = dynamic_cast<ValFormulaVar *>(frm);
        if (fv) {
            if (i != 0)
                return false;

            tfac.copyFrom(fv->getPart(0));
            tvar.copyFrom(fv->getPart(1));
            tlin = true;
            return true;
        }

        ValFormulaVarProd *fvp = dynamic_cast<ValFormulaVarProd *>(frm);
        if (fvp) {
            if (i != 0)
                return false;

            tfac.copyFrom(fv->getPart(0));

            ValFormulaVarProdOp *nfvp;
            tvar.dispSet(TP_FORMULA, nfvp = new ValFormulaVarProdOp(fvp));
            nfvp->getPart(0)->unset();

            tlin = false;
            return true;
        }

        return false;
    }

    /**
     * add a new term to this linear combination
     * @param tfac          factor for term
     * @param tvar          variable (can also be product of variables) within the term
     * @param neg           negate factor
     */
    void ValFormulaLinearCombOp::addTermFacVar(CmplVal& tfac, CmplVal& tvar, bool neg)
    {
        if (tfac && !tfac.isScalarNumber())
            throw invalid_argument("factor for term must be a scalar number");

        ValFormulaVarProd *fvp = (tvar.t == TP_FORMULA ? dynamic_cast<ValFormulaVarProd *>(tvar.valFormula()) : NULL);
        if ((tvar.t != TP_OPT_VAR && !fvp) || (fvp && !fvp->getPart(0)->isNumOne(true)))
            throw invalid_argument("variable for term must be a variable or product of variables");

        if (!neg) {
            _terms.push_back(tfac);
        }
        else {
            CmplVal tfacn(TP_INT, (intType)0);
            if (tfac)
                tfacn.numAdd(tfac, true);
            else
                tfacn.set(TP_INT, (intType)-1);

            _terms.push_back(tfacn);
        }

        _terms.push_back(tvar);

        if (_linear && fvp)
            _linear = false;
    }



    /************** ValFormulaCompareOp **********/

    /**
     * check whether this formula has its canonical form
     * @param chg   change this to canonical form if it is necessary and possible
     * @return      return whether formula has now a canonical form
     */
    bool ValFormulaCompareOp::canonicalForm(bool chg)
    {
        // canonical form is: <linear combination> relation <scalar number>
        // check for canonical form
        bool cnf = false;
        if (_rightSide.isScalarNumber() && _leftSide.t == TP_FORMULA) {
            ValFormulaLinearCombBase *lcb = dynamic_cast<ValFormulaLinearCombBase *>(_leftSide.valFormula());
            if (lcb) {
                CmplVal *constTerm = lcb->constTerm();
                if (!constTerm || !*constTerm)
                    cnf = true;
            }
        }

        // change to canonical form
        if (!cnf && chg) {
            checkSwapSides();

            if (_leftSide.t != TP_FORMULA || (!_rightSide.isScalarNumber() && _rightSide.t != TP_FORMULA))
                return false;

            ValFormula *lf = _leftSide.valFormula();
            ValFormula *rf = (_rightSide.t == TP_FORMULA ? _rightSide.valFormula() : NULL);

            // left side: change to ValFormulaLinearComb if necessary
            ValFormulaLinearCombOp *llc = dynamic_cast<ValFormulaLinearCombOp *>(lf);
            if (!llc) {
                CmplValAuto tfac, tvar;
                bool tlin;
                if (!ValFormulaLinearCombOp::getTermFacVar(lf, 0, tfac, tvar, tlin))
                    return false;

                _leftSide.dispSet(TP_FORMULA, llc = new ValFormulaLinearCombOp(lf->syntaxElem()));
                llc->addTermFacVar(tfac, tvar);
            }

            // right side: move all but a scalar value to left side
            if (rf) {
                CmplValAuto scal;
                CmplValAuto tfac, tvar;
                bool tlin;

                ValFormulaLinearCombOp *rlc = dynamic_cast<ValFormulaLinearCombOp *>(rf);
                if (rlc) {
                    for (unsigned i = 0; i < rlc->termCnt(); i++) {
                        ValFormulaLinearCombOp::getTermFacVar(rlc, 0, tfac, tvar, tlin);
                        llc->addTermFacVar(tfac, tvar, true);
                    }

                    scal.copyFrom(rlc->constTerm());
                    if (!scal)
                         scal.set(TP_INT, 0);
                }
                else {
                    if (!ValFormulaLinearCombOp::getTermFacVar(rf, 0, tfac, tvar, tlin))
                        return false;

                    llc->addTermFacVar(tfac, tvar, true);
                    scal.set(TP_INT, 0);
                }

                _rightSide.copyFrom(scal);
            }

            // move a scalar value from left side to right side
            CmplVal *lct = llc->constTerm();
            if (*lct) {
                _rightSide.numAdd(*lct, true);
                lct->dispUnset();
            }

            // if the left side has a canonical form then change it to it
            llc->canonicalForm(true);
        }

        return cnf;
    }


    /**
     * swap sides to ensure that a simple value stands on the right side
     */
    void ValFormulaCompareOp::checkSwapSides()
    {
        if (_leftSide.isScalarNumber()) {
            CmplVal s;
            s.moveFrom(_leftSide);
            _leftSide.moveFrom(_rightSide);
            _rightSide.moveFrom(s);

            swap(_compGe, _compLe);
        }

        if (_leftSide.t == TP_OPT_VAR) {
            CmplVal f(TP_FORMULA, new ValFormulaVarOp(syntaxElem(), _leftSide.optVar()));
            _leftSide.moveFrom(f, true);
        }

        if (_rightSide.t == TP_OPT_VAR) {
            CmplVal f(TP_FORMULA, new ValFormulaVarOp(syntaxElem(), _rightSide.optVar()));
            _rightSide.moveFrom(f, true);
        }
    }


    /**
     * logical Not for formula
     * @param ctx			execution context
     * @param res			store for result value
     * @param se			syntax element id of operation
     * @param noReuse       don't reuse this even if it has only one reference
     */
    void ValFormulaCompareOp::notF(ExecContext *ctx, CmplVal *res, unsigned se, bool noReuse)
    {
        ValFormulaCompareOp *f = (noReuse || refCnt() > 1 ? new ValFormulaCompareOp(this) : this);
        res->set(TP_FORMULA, f, true);
        f->_compNeg = (_compNeg ? false : true);
    }

    /**
     * get priority of calling the binary operation at this formula object
     * @param op			operation code (binary operation)
     * @param fa			this is the first argument of the operation
     * @param oa			other argument of the operation
     * @return				priority level to call execution of the operation at this instead of the other argument (at least 2), or 0 if the formula doesn't implement the operation
     */
    unsigned ValFormulaCompareOp::formulaOperPrio(unsigned short op, bool fa, CmplVal *oa)
    {
        if (op != ICS_OPER_AND && op != ICS_OPER_OR && op != ICS_OPER_OBJTO)
            return 0;
        else
            return 20;
    }



    /************** ValFormulaCompContinueOp **********/

    /**
     * get priority of calling the binary operation at this formula object
     * @param op			operation code (binary operation)
     * @param fa			this is the first argument of the operation
     * @param oa			other argument of the operation
     * @return				priority level to call execution of the operation at this instead of the other argument (at least 2), or 0 if the formula doesn't implement the operation
     */
    unsigned ValFormulaCompContinueOp::formulaOperPrio(unsigned short op, bool fa, CmplVal *oa)
    {
        if (op != ICS_OPER_EQ && op != ICS_OPER_NE && op != ICS_OPER_LT && op != ICS_OPER_GT && op != ICS_OPER_LE && op != ICS_OPER_GE)
            return 0;
        else
            return 99;
    }

    /**
     * compare for formula
     * @param ctx			execution context
     * @param res			store for result definition set
     * @param se			syntax element id of operation
     * @param a2			argument two
     * @param ge			compare for greater equal (together with le: compare for equality)
     * @param le			compare for lesser equal (together with le: compare for equality)
     * @param neg			negate comparison
     */
    void ValFormulaCompContinueOp::compF(ExecContext *ctx, CmplVal *res, unsigned se, CmplVal *a2, bool ge, bool le, bool neg)
    {
        if (_cmpRes.t == TP_BIN && _cmpRes.v.i == 0) {
            res->set(TP_BIN, false);
        }
        else if (_cmpRes.t == TP_BIN || (_cmpRes.t == TP_FORMULA && _cmpRes.valFormula()->isBool())) {
            // execute the compare operation and connect its result with the previous compare result in _cmpRes by a logical And operation
            CmplValAuto rc;
            OperationBase::comp(ctx, &rc, se, &_rightSide, a2, ge, le, neg, false);

            if (_cmpRes.t == TP_BIN || (rc.t == TP_BIN && rc.v.i == 0)) {
                res->moveFrom(rc);
            }
            else if (rc.t == TP_BIN) {
                res->copyFrom(_cmpRes, true, false);
            }
            else {
                OperationBase::logAndOr(ctx, res, se, &_cmpRes, &rc, ICS_OPER_AND);
            }
        }
        else {
            ctx->modp()->ctrl()->errHandler().internalError("wrong value type in continued comparison");
            res->set(TP_BIN, false);
        }
    }



    /************** ValFormulaObjectiveOp **********/

    /**
     * get priority of calling the binary operation at this formula object
     * @param op			operation code (binary operation)
     * @param fa			this is the first argument of the operation
     * @param oa			other argument of the operation
     * @return				priority level to call execution of the operation at this instead of the other argument (at least 2), or 0 if the formula doesn't implement the operation
     */
    unsigned ValFormulaObjectiveOp::formulaOperPrio(unsigned short op, bool fa, CmplVal *oa)
    {
        // no possible operation
        return 0;
    }



    /************** ValFormulaLogConOp **********/

    /**
     * logical Not for formula
     * @param ctx			execution context
     * @param res			store for result value
     * @param se			syntax element id of operation
     * @param noReuse       don't reuse this even if it has only one reference
     */
    void ValFormulaLogConOp::notF(ExecContext *ctx, CmplVal *res, unsigned se, bool noReuse)
    {
        ValFormulaLogConOp *f = (noReuse || refCnt() > 1 ? new ValFormulaLogConOp(this) : this);
        res->set(TP_FORMULA, f, true);
        f->_logNeg = (_logNeg ? false : true);
    }

    /**
     * logical And or Or for formula
     * @param ctx			execution context
     * @param res			store for result value
     * @param se			syntax element id of operation
     * @param a2			argument two
     * @param logOr			logical Or or logical And
     */
    void ValFormulaLogConOp::logAndOrF(ExecContext *ctx, CmplVal *res, unsigned se, CmplVal *a2, bool logOr)
    {
        if (_logNeg || _logOr != logOr || a2->t != TP_FORMULA) {
            // construct new logical expression containing this and a2
            OperationBase::logAndOrF(ctx, res, se, a2, logOr);
        }
        else {
            // add a2 to the formulas in this (or copy of this)
            ValFormulaLogConOp *f = (refCnt() > 1 ? new ValFormulaLogConOp(this) : this);
            res->set(TP_FORMULA, f, true);

            ValFormulaLogCon *lc = dynamic_cast<ValFormulaLogCon *>(a2->v.vp);
            if (lc && !lc->logNeg() && lc->logOr() == logOr) {
                // add all parts of a2 to result
                for (unsigned i = 0; i < lc->formulas().size(); i++) {
                    f->formulas().push_back(CmplVal(TP_FORMULA, lc->formulas()[i].valFormula(), true));
                }
            }
            else {
                // add whole a2 to result
                f->formulas().push_back(CmplVal(TP_FORMULA, a2->valFormula(), true));
            }
        }
    }

    /**
     * get priority of calling the binary operation at this formula object
     * @param op			operation code (binary operation)
     * @param fa			this is the first argument of the operation
     * @param oa			other argument of the operation
     * @return				priority level to call execution of the operation at this instead of the other argument (at least 2), or 0 if the formula doesn't implement the operation
     */
    unsigned ValFormulaLogConOp::formulaOperPrio(unsigned short op, bool fa, CmplVal *oa)
    {
        if (op != ICS_OPER_AND && op != ICS_OPER_OR)
            return 0;
        else if (!_logNeg && ((op == ICS_OPER_OR && _logOr) || (op == ICS_OPER_AND && !_logOr)))
            return 45;
        else
            return 40;
    }



    /************** ValFormulaLogConOp **********/

    /**
     * insert next conditional part of the formula
     * @param pc        condition which must be true
     * @param ncs       conditions which must be false
     * @param v         value
     */
    void ValFormulaCondOp::insPart(CmplValAuto& pc, vector<CmplValAuto>& ncs, CmplValAuto& v)
    {
        if (_parts.empty() || _binary)
            _binary = (v.t == TP_BIN || v.t == TP_INT || (v.t == TP_FORMULA && v.valFormula()->isBool()) || (v.t == TP_OPT_VAR && v.optVar()->binVar()));

        if (_parts.empty() || _numeric)
            _numeric = (v.isScalarNumber() || v.t == TP_OPT_VAR || (v.t == TP_FORMULA && v.valFormula()->isNumeric()));

        if (_parts.empty() || _optCon)
            _optCon = (v.t == TP_FORMULA && v.valFormula()->canOptRow(false));

        if (_parts.empty() || _optObj)
            _optObj = (v.t == TP_FORMULA && v.valFormula()->canOptRow(true));

        if (v.t == TP_OPT_VAR) {
            OptVar *ov = v.optVar();
            CmplValAuto fv(TP_FORMULA, new ValFormulaVarOp(ov->syntaxElem(), ov));
            _parts.emplace_back(TP_FORMULA, new Part(pc, ncs, fv, syntaxElem()));
        }
        else {
            _parts.emplace_back(TP_FORMULA, new Part(pc, ncs, v, syntaxElem()));
        }
    }

    /**
     * merge last parts of regular constructed condition formula, because its values are equal (this is not checked here!)
     * @param cnt       count of merges
     */
    void ValFormulaCondOp::mergeLast(unsigned cnt)
    {
        while (cnt && _parts.size() >= 2) {
            _parts.pop_back();
            dynamic_cast<Part *>(_parts.back().valFormula())->_posCond.dispUnset();
            cnt--;
        }
    }

    /**
     * get priority of calling the binary operation at this formula object
     * @param op			operation code (binary operation)
     * @param fa			this is the first argument of the operation
     * @param oa			other argument of the operation
     * @return				priority level to call execution of the operation at this instead of the other argument (at least 2), or 0 if the formula doesn't implement the operation
     */
    unsigned ValFormulaCondOp::formulaOperPrio(unsigned short op, bool fa, CmplVal *oa)
    {
        if (op == ICS_OPER_AND || op == ICS_OPER_OR) {
            if (_binary)
                return 50;
            else
                return 0;
        }
        else {
            if (_numeric)
                return 50;
            else
                return 0;
        }
    }

    /**
     * negation of formula
     * @param ctx			execution context
     * @param res			store for result definition set
     * @param se			syntax element id of operation
     */
    void ValFormulaCondOp::negF(ExecContext *ctx, CmplVal *res, unsigned se)
    {
        if (!_numeric) {
            CmplValAuto v(TP_FORMULA, dynamic_cast<ValFormula *>(this));
            ctx->valueError("argument is not a number", v, se);
        }
        else if (_numericVar) {
            OperationBase::neg(ctx, res, se, &_numericVar);
        }
        else {
            ValFormulaCondOp *f = (refCnt() > 1 ? new ValFormulaCondOp(this) : this);
            res->set(TP_FORMULA, f);

            CmplValAuto pr;
            for (CmplVal& vp: f->_parts) {
                Part *p = dynamic_cast<Part *>(vp.valFormula());
                if (p->_val) {
                    OperationBase::neg(ctx, &pr, se, &(p->_val));
                    p->_val.moveFrom(pr, true);
                }
            }
        }
    }

    /**
     * addition of formulas
     * @param ctx			execution context
     * @param res			store for result definition set
     * @param se			syntax element id of operation
     * @param a2			argument two
     */
    void ValFormulaCondOp::plusF(ExecContext *ctx, CmplVal *res, unsigned se, CmplVal *a2)
    {
        ValFormulaCondOp *a2cond;
        bool err;
        if (checkUseNumericVar(ctx, se, a2, true, true, true, a2cond, err))
            OperationBase::plus(ctx, res, se, &_numericVar, (a2cond ? &(a2cond->_numericVar) : a2));
        else if (!err)
            execOperForParts(ctx, res, se, ICS_OPER_ADD, false, a2, a2cond);
    }

    /**
     * minus of formulas
     * @param ctx			execution context
     * @param res			store for result definition set
     * @param se			syntax element id of operation
     * @param a2			argument two
     * @param rev			substract this from a2
     */
    void ValFormulaCondOp::minusF(ExecContext *ctx, CmplVal *res, unsigned se, CmplVal *a2, bool rev)
    {
        ValFormulaCondOp *a2cond;
        bool err;
        if (checkUseNumericVar(ctx, se, a2, true, true, true, a2cond, err)) {
            if (!rev)
                OperationBase::minus(ctx, res, se, &_numericVar, (a2cond ? &(a2cond->_numericVar) : a2));
            else
                OperationBase::minus(ctx, res, se, (a2cond ? &(a2cond->_numericVar) : a2), &_numericVar);
        }
        else if (!err) {
            execOperForParts(ctx, res, se, ICS_OPER_SUB, rev, a2, a2cond);
        }
    }

    /**
     * multiplication of formulas
     * @param ctx			execution context
     * @param res			store for result definition set
     * @param se			syntax element id of operation
     * @param a2			argument two
     * @param noReuseArg    don't reuse argument even if it has only one reference
     */
    void ValFormulaCondOp::multF(ExecContext *ctx, CmplVal *res, unsigned se, CmplVal *a2, bool noReuseArg)
    {
        ValFormulaCondOp *a2cond;
        bool err;
        if (checkUseNumericVar(ctx, se, a2, false, false, false, a2cond, err))
            OperationBase::mult(ctx, res, se, &_numericVar, (a2cond ? &(a2cond->_numericVar) : a2), noReuseArg);
        else if (!err)
            execOperForParts(ctx, res, se, ICS_OPER_MUL, false, a2, a2cond, noReuseArg);
    }

    /**
     * compare for formula
     * @param ctx			execution context
     * @param res			store for result definition set
     * @param se			syntax element id of operation
     * @param a2			argument two
     * @param ge			compare for greater equal (together with le: compare for equality)
     * @param le			compare for lesser equal (together with le: compare for equality)
     * @param neg			negate comparison
     */
    void ValFormulaCondOp::compF(ExecContext *ctx, CmplVal *res, unsigned se, CmplVal *a2, bool ge, bool le, bool neg)
    {
        ValFormulaCondOp *a2cond;
        bool err;
        if (checkUseNumericVar(ctx, se, a2, true, false, true, a2cond, err)) {
            OperationBase::comp(ctx, res, se, &_numericVar, (a2cond ? &(a2cond->_numericVar) : a2), ge, le, neg, false);
        }
        else if (!err) {
            unsigned short op = (!neg ? (ge && le ? ICS_OPER_EQ : (ge ? ICS_OPER_GE : ICS_OPER_LE)) : (ge && le ? ICS_OPER_NE : (ge ? ICS_OPER_LT : ICS_OPER_GT)));
            execOperForParts(ctx, res, se, op, false, a2, a2cond);
        }
    }

    /**
     * logical Not for formula
     * @param ctx			execution context
     * @param res			store for result value
     * @param se			syntax element id of operation
     * @param noReuse       don't reuse this even if it has only one reference
     */
    void ValFormulaCondOp::notF(ExecContext *ctx, CmplVal *res, unsigned se, bool noReuse)
    {
        CmplValAuto t;
        if (convertToFormulaLogCon(ctx, &t, se, true))
            OperationBase::execUnaryOper(ctx, res, se, ICS_OPER_NOT, &t);
    }

    /**
     * logical And or Or for formula
     * @param ctx			execution context
     * @param res			store for result value
     * @param se			syntax element id of operation
     * @param a2			argument two
     * @param logOr			logical Or or logical And
     */
    void ValFormulaCondOp::logAndOrF(ExecContext *ctx, CmplVal *res, unsigned se, CmplVal *a2, bool logOr)
    {
        CmplValAuto t1, t2;
        if (convertToFormulaLogCon(ctx, &t1, se, true)) {
            ValFormulaCondOp *f2 = (a2->t == TP_FORMULA ? dynamic_cast<ValFormulaCondOp *>(a2->valFormula()) : NULL);
            if (f2) {
                if (!f2->convertToFormulaLogCon(ctx, &t2, se, true))
                    return;
            }
            else {
                t2.copyFrom(a2);
            }

            OperationBase::execBinaryOper(ctx, res, se, (logOr ? ICS_OPER_OR : ICS_OPER_AND), false, &t1, &t2);
        }
    }

    /**
     * check whether an operation must be performed on replacement variables
     * @param ctx			execution context
     * @param se			syntax element id of operation
     * @param a2			argument two
     * @param usevar        replacement variables must be used if one of the arguments already has one
     * @param complete      replacement variables must be used if one of the arguments is a non-complete conditional value
     * @param formula       replacement variables must be used if the second argument is not a scalar number
     * @param a2cond        return of second argument if it is a conditional value
     * @param err           return whether the arguments are invalid
     * @return              true if the operation must be performed on replacement variables
     */
    bool ValFormulaCondOp::checkUseNumericVar(ExecContext *ctx, unsigned se, CmplVal *a2, bool usevar, bool complete, bool formula, ValFormulaCondOp *&a2cond, bool &err)
    {
        a2cond = (a2->t == TP_FORMULA ? dynamic_cast<ValFormulaCondOp *>(a2->valFormula()) : NULL);

        // arguments must be numeric
        err = (!_numeric || !(a2cond ? a2cond->_numeric : (a2->t == TP_FORMULA ? a2->valFormula()->isNumeric() : (a2->isScalarNumber() || a2->t == TP_OPT_VAR))));
        if (err) {
            if (!_numeric) {
                CmplValAuto ev(TP_FORMULA, this);
                ctx->valueError("argument for operation must be numeric", ev, se);
            }
            else {
                ctx->valueError("argument for operation must be numeric", *a2, se);
            }
            return false;
        }

        bool eqcond;
        if (a2cond) {
            eqcond = (_parts.size() == a2cond->_parts.size());
            if (eqcond) {
                for (unsigned i = 0; i < _parts.size(); i++) {
                    Part *p1 = dynamic_cast<Part *>(_parts[i].valFormula());
                    Part *p2 = dynamic_cast<Part *>(a2cond->_parts[i].valFormula());
                    if (!p1->eqCond(*p2)) {
                        eqcond = false;
                        break;
                    }
                }
            }
        }
        else {
            eqcond = false;
        }

        bool res =
            (a2cond && !eqcond) ||
            (usevar && (_numericVar || (a2cond && a2cond->_numericVar))) ||
            (complete && !eqcond && (!_complete || (a2cond && !(a2cond->_complete)))) ||
            (formula && !a2cond && !a2->isScalarNumber());

        if (res) {
            checkCreateNumericVar(ctx, se);
            if (a2cond)
                a2cond->checkCreateNumericVar(ctx, se);
        }

        return res;
    }

    /**
     * test whether _numericVar already exists, and if not then create it
     * @param ctx           execution context
     * @param se            syntax element id of operation
     * @param nv            if not NULL then return here copy of _numericVar
     * @return              true if _numericVar is new created
     */
    bool ValFormulaCondOp::checkCreateNumericVar(ExecContext *ctx, unsigned se, CmplVal *nv)
    {
        bool res = false;

        if (!_numericVar) {
            CmplValAuto lb, ub;
            getBounds(lb, ub, false);

            CmplVal& dt = ctx->modp()->realTypeVar();
            OptVar *v = new OptVar(ctx->modp()->getResModel(), dt, false, se);
            _numericVar.set(TP_FORMULA, new ValFormulaVarOp(se, v));

            dt.valType()->setForOptVar(v);
            if (lb)
                v->setLowBound(lb);
            if (ub)
                v->setUppBound(ub);

            CmplValAuto cv(TP_FORMULA, this);
            v->setAddProp(new AddPropOptVarConValCond(cv));

            res = true;
        }

        if (nv)
            nv->copyFrom(_numericVar);

        return res;
    }

    /**
     * fix the replacement variable by a conditional compare formula
     * @param ctx           execution context
     * @param res           store for result value
     * @param se            syntax element id
     * @return              replacement variable
     */
    OptVar *ValFormulaCondOp::fixNumericVar(ExecContext *ctx, CmplVal *res, unsigned se)
    {
        if (_numericVar) {
            execOperForParts(ctx, res, se, ICS_OPER_EQ, false, &_numericVar, NULL);
            CmplVal *nv = _numericVar.valFormula()->getPart(1);
            return nv->optVar();
        }
        else {
            return NULL;
        }
    }

    /**
     * convert this conditional value (must be binary) to a logical formula
     * @param ctx           execution context
     * @param res           store for result value
     * @param se            syntax element id of operation
     * @param err           output error message if not successful
     * @return              true if conversion is successful
     */
    bool ValFormulaCondOp::convertToFormulaLogCon(ExecContext *ctx, CmplVal *res, unsigned se, bool err)
    {
        if (!_binary) {
            CmplValAuto ev(TP_FORMULA, this);
            if (err)
                ctx->valueError("argument for operation must be boolean", ev, se);
            return false;
        }

        if (!_binaryFormula) {
            // every part has the logical formula: val || !pC || nC1 || nC2 ...
            // and the formulas for all parts are connected by &&
            ValFormulaLogConOp *rf = new ValFormulaLogConOp(syntaxElem(), false, (_parts.size() == 1));
            _binaryFormula.set(TP_FORMULA, rf);

            for (unsigned i = 0; i < _parts.size(); i++) {
                const ValFormulaCond::Part *p = dynamic_cast<Part *>(_parts[i].valFormula());

                ValFormulaLogConOp *pf = (_parts.size() == 1 ? rf : new ValFormulaLogConOp(syntaxElem(), false, true));
                CmplValAuto pv(TP_FORMULA, pf);

                ValFormulaCondOp *sub = (p->_val.t == TP_FORMULA ? dynamic_cast<ValFormulaCondOp *>(p->_val.valFormula()) : NULL);
                if (sub) {
                    CmplValAuto subv;
                    if (sub->convertToFormulaLogCon(ctx, &subv, se, err))
                        pf->formulas().emplace_back(subv);
                    else
                        return false;
                }
                else if (p->_val.t == TP_FORMULA && p->_val.valFormula()->isBool()) {
                    pf->formulas().emplace_back(p->_val);
                }
                else {
                    bool bv;
                    if (!p->_val.toBool(bv, typeConversionAll, ctx->modp())) {
                        CmplValAuto ev(p->_val);
                        if (err)
                            ctx->valueError("all parts of conditional value argument must be boolean", ev, se);
                        return false;
                    }

                    if (bv)
                        continue;
                }

                if (p->_posCond) {
                    ValFormulaLogConOp *nf = new ValFormulaLogConOp(syntaxElem(), true, false);
                    pf->formulas().emplace_back(TP_FORMULA, nf);
                    nf->formulas().emplace_back(p->_posCond);
                }

                for (unsigned j = 0; j < p->_negConds.size(); j++) {
                    pf->formulas().emplace_back(p->_negConds[j]);
                }

                if (_parts.size() > 1)
                    rf->formulas().emplace_back(pv);
            }

            if (rf->formulas().size() == 0)
                _binaryFormula.dispSet(TP_BIN, true);
        }

        res->copyFrom(_binaryFormula);
        return true;
    }

    /**
     * check this for simple form ("{ cond: optvar = scal1; }" or "{ cond: optvar = scal1; |: optvar = scal2; }")
     * @param cond          return of condition
     * @param optvar        return of optimization variable
     * @param scal1         return of first scalar value
     * @param scal2         return of second scalar value or TP_EMPTY if no one
     * @return              true if simple form
     */
    bool ValFormulaCondOp::checkSimpleForm(CmplVal *cond, CmplVal *optvar, CmplVal *scal1, CmplVal *scal2)
    {
        if (!_binary || _parts.size() < 1 || _parts.size() > 2)
            return false;

        const Part *p1 = dynamic_cast<Part *>(_parts[0].valFormula());
        if (p1->_posCond.t != TP_FORMULA || p1->_negConds.size() != 0 || p1->_val.t != TP_FORMULA)
            return false;

        ValFormulaCompare *c1 = dynamic_cast<ValFormulaCompare *>(p1->_val.valFormula());
        if (!c1 || !c1->isCompGe() || !c1->isCompLe() || c1->isNeg())
            return false;

        ValFormulaCompare *c2 = NULL;
        if (_parts.size() > 1) {
            const Part *p2 = dynamic_cast<Part *>(_parts[1].valFormula());
            if (p2->_posCond || p2->_negConds.size() != 1 || p2->_val.t != TP_FORMULA)
                return false;

            c2 = dynamic_cast<ValFormulaCompare *>(p2->_val.valFormula());
            if (!c2 || !c2->isCompGe() || !c2->isCompLe() || c2->isNeg())
                return false;
        }

        CmplVal *p1a = c1->getPart(0);
        CmplVal *p1b = c1->getPart(1);
        CmplVal *p2a = (c2 ? c2->getPart(0) : NULL);
        CmplVal *p2b = (c2 ? c2->getPart(1) : NULL);
        if ((!p1a->isScalarNumber() && !p1b->isScalarNumber()) || (c2 && !p2a->isScalarNumber() && !p2b->isScalarNumber()))
            return false;

        CmplVal *vf1 = (!p1a->isScalarNumber() ? p1a : p1b);
        CmplVal *vf2 = (c2 ? (!p2a->isScalarNumber() ? p2a : p2b) : NULL);

        CmplVal *v1 = NULL;
        if (vf1->t == TP_OPT_VAR) {
            v1 = vf1;
        }
        else if (vf1->t == TP_FORMULA) {
            ValFormulaVar *vv = dynamic_cast<ValFormulaVar *>(vf1->valFormula());
            if (vv) {
                if (vv->getPart(0)->isNumOne(true) && vv->getPart(1)->t == TP_OPT_VAR)
                    v1 = vv->getPart(1);
            }
        }
        if (!v1)
            return false;

        if (c2) {
            CmplVal *v2 = NULL;
            if (vf2->t == TP_OPT_VAR) {
                v2 = vf2;
            }
            else if (vf2->t == TP_FORMULA) {
                ValFormulaVar *vv = dynamic_cast<ValFormulaVar *>(vf2->valFormula());
                if (vv) {
                    if (vv->getPart(0)->isNumOne(true) && vv->getPart(1)->t == TP_OPT_VAR)
                        v2 = vv->getPart(1);
                }
            }
            if (!v2 || v2->optVar() != v1->optVar())
                return false;
        }

        cond->copyFrom(p1->_posCond);
        optvar->copyFrom(v1);
        scal1->copyFrom(p1a->isScalarNumber() ? p1a : p1b);
        if (c2)
            scal2->copyFrom(p1a->isScalarNumber() ? p2a : p2b);
        else
            scal2->unset();

        return true;
    }


    /**
     * execute binary operation for every part of the conditional value
     * @param ctx			execution context
     * @param res			store for result value
     * @param se			syntax element id of operation
     * @param op			operation code (one of the minor codes for INTCODE_OPERATION)
     * @param rev           swap operands
     * @param a2            second operand
     * @param a2cond        if given, then use parts of this conditional value instead of a2
     * @param noReuseArg    don't reuse argument even if it has only one reference
     */
    void ValFormulaCondOp::execOperForParts(ExecContext *ctx, CmplVal *res, unsigned se, unsigned short op, bool rev, CmplVal *a2, ValFormulaCondOp *a2cond, bool noReuseArg)
    {
        ValFormulaCondOp *f = (noReuseArg || refCnt() > 1 || _numericVar || _binaryFormula ? new ValFormulaCondOp(this) : this);
        res->set(TP_FORMULA, f, true);

        f->_numericVar.dispUnset();
        f->_binaryFormula.dispUnset();
        f->_optCon = f->_optObj = (f->_parts.size() > 0);

        f->_numeric = (op >= ICS_OPER_ADD && op <= ICS_OPER_POW);
        f->_binary = (op >= ICS_OPER_AND && op <= ICS_OPER_GE);

        CmplValAuto pr;
        for (unsigned i = 0; i < f->_parts.size(); i++) {
            Part *p1 = dynamic_cast<Part *>(_parts[i].valFormula());
            Part *p2 = (a2cond ? dynamic_cast<Part *>(a2cond->_parts[i].valFormula()) : NULL);
            Part *pf = dynamic_cast<Part *>(f->_parts[i].valFormula());

            CmplVal *pa1 = &(p1->_val);
            CmplVal *pa2 = (p2 ? &(p2->_val) : a2);

            if (rev)
                OperationBase::execBinaryOper(ctx, &pr, se, op, false, pa2, pa1);
            else
                OperationBase::execBinaryOper(ctx, &pr, se, op, false, pa1, pa2);

            if (f->_optCon && (pr.t != TP_FORMULA || !(pr.valFormula()->canOptRow(false))))
                f->_optCon = false;
            if (f->_optObj && (pr.t != TP_FORMULA || !(pr.valFormula()->canOptRow(true))))
                f->_optObj = false;

            pf->_val.moveFrom(pr, true);
        }
    }
}

