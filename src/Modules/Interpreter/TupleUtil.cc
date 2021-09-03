
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


#include "TupleUtil.hh"
#include "SetUtil.hh"
#include "SetInfinite.hh"
#include "Interval.hh"
#include "Interpreter.hh"
#include "ExecContext.hh"
#include "CodeBlockContext.hh"
#include "OperationBase.hh"
#include "../../Control/MainControl.hh"


namespace cmpl
{

	/****** TupleUtil ****/

	/**
	 * construct a tuple from stack values
	 * @param ctx			execution context
	 * @param res			store for result tuple
     * @param pushRes       return whether res is to push to the stack
	 * @param cnt			count of components for tuple on stack
     * @param listTuple		if tuple is an index tuple than keep it on the stack as TP_LIST_TUPLE
	 * @param top			topmost component for tuple on stack / NULL: if cnt==0
	 * @return				stack element to pop to before pushing result tuple / NULL: no element to pop
	 */
    StackValue *TupleUtil::construct(ExecContext *ctx, CmplVal &res, bool &resPush, unsigned cnt, bool listTuple, StackValue *top)
	{
        resPush = true;

		// if no component then return empty tuple
		if (cnt == 0) {
			res.set(TP_TUPLE_EMPTY);
			return NULL;
		}

		CmplVal *v;
        CmplVal nl(TP_NULL);

		if (cnt == 1) {
            v = top->simpleValueOrEmptyArr(&nl);

			// check for empty tuple (with one blank component) or null tuple
			if (v && (v->t == TP_BLANK || v->t == TP_NULL)) {
				res.set((v->t == TP_BLANK ? TP_TUPLE_EMPTY : TP_ITUPLE_NULL));
				return top;
			}

			// check if the single component is already a tuple, then let stack unchanged
            if ((top->val().t == TP_LIST_TUPLE && listTuple) || (v && v->isTuple())) {
				res.unset();
                resPush = false;
				return NULL;
			}

			// check if simple index tuple
			if (v && v->isScalarIndex()) {
				top->ifStringPChangeToStr(ctx);
				res.set((v->t == TP_STR ? TP_ITUPLE_1STR : TP_ITUPLE_1INT), v->v.i);
				return top;
			}
		}

		// discard trailing nulls
		StackValue *ptop = NULL;
        while (cnt && top->simpleValueOrEmptyArr(&nl) && top->simpleValueOrEmptyArr(&nl)->t == TP_NULL) {
			ptop = top;
			top = ctx->stackPrev(ptop);
			cnt--;
		}

		// count nulls, blanks and result tuple rank, check tuple type
		unsigned i, cnn = 0, cbl = 0, rtr = 0;
        bool indTuple = true;
        bool noset = false, infset = false, cbs = false;
		StackValue *sv, *bot = ptop;
		Tuple::TupleType ttp;

		for (i = 0, sv = top; i < cnt; i++, bot = sv, sv = ctx->stackPrev(sv, false)) {
            v = sv->simpleValueOrEmptyArr(&nl);
			if (v) {
				PROTO_MOD_OUTL(ctx->modp(), "  value for tuple: " << *v);
				if (v->t == TP_BLANK) {
					cbl++;
					rtr++;
					indTuple = false;
					infset = true;
				}
				else if (v->t == TP_NULL) {
					cnn++;
					listTuple = false;
				}
                else if (v->isTuple() || v->t == TP_DEF_CB_SYM_TUPLE) {
                    rtr += Tuple::rank(*v);
					listTuple = false;
					if (!v->isITuple()) {
						indTuple = false;

						if (v->t == TP_TUPLE_EMPTY)
							infset = true;
						else {
							ttp = v->tuple()->tupleType();
							if (ttp == Tuple::tupleSetInf)
								infset = true;
							else if (ttp >= Tuple::tupleElse)
								noset = true;

                            if (v->t == TP_DEF_CB_SYM_TUPLE)
                                cbs = true;
						}
					}
				}
				else {
					rtr++;
					if (v->isScalarIndex()) {
						sv->ifStringPChangeToStr(ctx);
					}
                    else if (v->t == TP_DEF_CB_SYM) {
                        cbs = true;
                    }
                    else {
						indTuple = false;
						if (v->isInterval() || v->isSet() || v->t == TP_DATA_TYPE) {
                            if (!(v->cntElemFin()) || v->t == TP_DATA_TYPE)
								infset = true;
						}
						else {
							noset = true;
						}
					}
				}
			}
			else if (sv->val().t == TP_LIST_TUPLE) {
				PROTO_MOD_OUTL(ctx->modp(), "  value for tuple: tuple list rank " << sv->val().v.i);
				rtr += sv->val().v.i;
				listTuple = false;
			}
			else {
				ctx->valueError("components of tuple must be scalar values or other tuples", sv);
				cnn++;
			}
		}

		// check for null tuple or empty tuple
		if (rtr == 0 || cbl == cnt) {
			res.set((rtr == 0 ? TP_ITUPLE_NULL : TP_TUPLE_EMPTY));
			return bot;
		}

		// check if simple index tuple (first non-null is the simple value for tuple)
        if (rtr == 1 && indTuple && !cbs) {
			for (i = 0, sv = top; i < cnt; i++, sv = ctx->stackPrev(sv, false)) {
                v = sv->simpleValue();
                if (v && v->t != TP_NULL && (v->isScalarIndex() || (v->isTuple() && Tuple::rank(*v) == 1))) {
					if (v->isScalarIndex())
						res.set((v->t == TP_STR ? TP_ITUPLE_1STR : TP_ITUPLE_1INT), v->v.i);
					else
						res.copyFrom(*v);

					return bot;
				}
			}
		}

		// check if tuple can remain as list on stack
        if (indTuple && listTuple && !cbs) {
			res.set(TP_LIST_TUPLE, (intType)cnt);
			return ptop;
		}

		// construct tuple object
		ttp = (indTuple ? Tuple::tupleIndexOnly : (noset ? Tuple::tupleElse : (infset ? Tuple::tupleSetInf : Tuple::tupleSetFinite)));
		Tuple *tpl = new Tuple(rtr, 0, ttp);
		unsigned i2;

        for (i = 0, sv = top; i < cnt; i++, sv = ctx->stackPrev(sv, false)) {
            v = sv->simpleValueOrEmptyArr(&nl);
			if (v) {
				if (v->t != TP_NULL) {
                    if (v->isTuple() || v->t == TP_DEF_CB_SYM_TUPLE) {
						if (v->useValP()) {
							Tuple *tpl2 = v->tuple();
							for (i2 = tpl2->rank(); i2 > 0; ) {
								tpl->at(--rtr)->copyFrom(*(tpl2->at(--i2)));
								PROTO_MOD_OUTL(ctx->modp(), "  set tuple elem #" << rtr << " to " << *(tpl2->at(i2)));
							}
						}
						else if (Tuple::rank(*v) != 0) {
							if (v->t == TP_TUPLE_EMPTY) {
								tpl->at(--rtr)->set(TP_BLANK);
								PROTO_MOD_OUTL(ctx->modp(), "  set tuple elem #" << rtr << " to <blank>");
							}
							else {
								tpl->at(--rtr)->set((v->t == TP_ITUPLE_1INT ? TP_INT : TP_STR), v->v.i);
								PROTO_MOD_OUTL(ctx->modp(), "  set tuple elem #" << rtr << " to " << v->v.i);
							}
						}
					}
					else {
						tpl->at(--rtr)->copyFrom(*v);
						PROTO_MOD_OUTL(ctx->modp(), "  set tuple elem #" << rtr << " to " << *v);
					}
				}
			}
			else if (sv->val().t == TP_LIST_TUPLE) {
				StackValue *lt = sv - 1;
				for (i2 = sv->val().v.i; i2 > 0; lt--) {
					tpl->at(--rtr)->copyFrom(lt->val());
					PROTO_MOD_OUTL(ctx->modp(), "  set tuple elem #" << rtr << " to " << lt->val());
				}
			}
		}

        res.set((cbs ? TP_DEF_CB_SYM_TUPLE : (indTuple ? TP_ITUPLE : TP_TUPLE)), tpl);
		return bot;
	}


	/**
	 * construct an index tuple from a TP_LIST_TUPLE value on the stack
	 * @param ctx			execution context
	 * @param res			store for result tuple
	 * @param lst			TP_LIST_TUPLE value on stack
	 */
	void TupleUtil::constructFromList(ExecContext *ctx, CmplVal &res, StackValue *lst)
	{
		if (lst->val().t == TP_LIST_TUPLE) {
			unsigned rank = lst->val().v.i;
			StackValue *lt = lst - 1;

			if (rank > 1) {
				Tuple *tpl = new Tuple(rank, 0, Tuple::tupleIndexOnly);
				res.set(TP_ITUPLE, tpl);

				for (unsigned i = rank; i > 0; i--, lt--)
					tpl->at(i-1)->copyFrom(lt->val());
			}
			else {
				if (rank == 0)
					res.set(TP_ITUPLE_NULL);
				else
					res.set((lt->val().t == TP_INT ? TP_ITUPLE_1INT : TP_ITUPLE_1STR), lt->val().v.i);
			}
		}
		else {
			// if value on stack is already an index tuple then use it
			CmplVal *v = lst->simpleValue();
			if (v && v->isITuple()) {
				res.copyFrom(*v);
			}
			else {
				ctx->valueError("value must be an index tuple", lst);
				res.set(TP_ITUPLE_NULL);
			}
		}
	}


    /**
     * convert tuple src to index tuple or to tuple set
     * @param ctx			execution context
     * @param res			store for result tuple or set
     * @param src			value to convert
     * @param listTuple		accept tuple as list
     */
    void TupleUtil::toIndexTuple(ExecContext *ctx, CmplVal &res, StackValue *src, bool listTuple)
    {
        CmplVal *v = (src->val().t == TP_LIST_TUPLE ? &(src->val()) : src->simpleValue());
        if (!v) {
            ctx->valueError("value not valid for indexation", src);
            res.set(TP_SET_ALL);
        }
        else if (v->isITuple() || v->isSet()) {
            res.copyFrom(v, true, false);
        }
        else if (v->t == TP_LIST_TUPLE) {
            if (listTuple)
                res.setP(TP_REF_LIST, src);
            else
                constructFromList(ctx, res, src);
        }
        else if (v->t == TP_TUPLE_EMPTY) {
            res.set(TP_SET_ALL);
        }
        else if (v->isScalarIndex()) {
            res.set((v->t == TP_STR ? TP_ITUPLE_1STR : TP_ITUPLE_1INT), v->v.i);
        }
        else if (v->t == TP_TUPLE) {
            if (!toIndexTuple(ctx, res, v->tuple())) {
                ctx->valueError("handling of tuple value not implemented", src);
                res.set(TP_SET_ALL);
            }
        }
        else {
            ctx->valueError("value not valid for indexation", src);
            res.set(TP_SET_ALL);
        }
    }

    /**
     * convert tuple tpl to index tuple or to tuple set
     * @param ctx			execution context
     * @param res			store for result tuple or set
     * @param tpl			tuple to convert
     * @return              true if conversion is successful
     */
    bool TupleUtil::toIndexTuple(ExecContext *ctx, CmplVal& res, Tuple *tpl)
    {
        // check type of result tuple or set
        tp_e tp = TP_ITUPLE_NULL;
        unsigned r = tpl->rank();
        CmplVal *e = tpl->at(0);

        for (unsigned i = 0; i < r; i++, e++) {
            if (e->t == TP_SET_EMPTY || tp == TP_SET_EMPTY) {
                tp = TP_SET_EMPTY;
            }
            else if (e->isScalarIndex()) {
                if (tp == TP_ITUPLE_NULL)
                    tp = TP_ITUPLE;
            }
            else if (e->t == TP_BLANK || e->t == TP_SET_ALL) {
                if (tp == TP_ITUPLE_NULL)
                    tp = TP_SET_ALL;
                else if (tp != TP_SET_ALL) {
                    tp = TP_SET_INF_TPL;
                }
            }
            else if (e->isInterval()) {
                if (Interval::isEmpty(*e))
                    tp = TP_SET_EMPTY;
                else if (!Interval::isSet(*e))
                    return false;
                else if (Interval::isSetInf(*e) || tp == TP_SET_ALL)
                    tp = TP_SET_INF_TPL;
                else if (tp != TP_SET_INF_TPL)
                    tp = TP_SET_FIN;
            }
            else if (e->isSet()) {
                if (tp == TP_SET_ALL)
                    tp = TP_SET_INF_TPL;
                else if (tp != TP_SET_INF_TPL)
                    tp = (e->isSetInf() ? TP_SET_INF_TPL : TP_SET_FIN);
            }
            else {
                return false;
            }
        }

        // set result value
        if (tp == TP_ITUPLE_NULL || tp == TP_SET_EMPTY || tp == TP_SET_ALL) {
            res.set(tp);
        }
        else if (tp == TP_ITUPLE) {
            if (r == 1) {
                e = tpl->at(0);
                res.set((e->t == TP_STR ? TP_ITUPLE_1STR : TP_ITUPLE_1INT), e->v.i);
            }
            else {
                Tuple *nt = new Tuple(r, 0, Tuple::tupleIndexOnly);
                res.set(TP_ITUPLE, nt);

                for (unsigned i = 0; i < r; i++)
                    nt->at(i)->copyFrom(tpl->at(i), true, false);
            }
        }
        else {
            if (tp == TP_SET_INF_TPL)
                SetUtil::tupleToSetInf(ctx, res, tpl, false);
            else
                SetUtil::tupleToSetFin(ctx, res, tpl, false);
        }

        return true;
    }


    /**
     * convert tuple or value to its canonical tuple form
     * @param ctx			execution context
     * @param res           store for result tuple
     * @param src           source tuple or value
     */
    void TupleUtil::canonicalTuple(ExecContext *ctx, CmplVal& res, CmplVal& src)
    {
        if (src.isTuple()) {
            bool conv = (src.useValP() && !src.isITuple());
            Tuple *tpl;
            CmplVal *p;
            unsigned r, maxRank;

            if (conv) {
                tpl = src.tuple();
                r = maxRank = tpl->rank();
                conv = false;

                p = tpl->at(0);
                for (unsigned i = 0; i < r; i++, p++) {
                    if (p->t == TP_NULL || p->t == TP_SET_NULL || (p->isSet() && !SetBase::rank1(*p))) {
                        conv = true;
                        if (SetBase::finite(*p))
                            maxRank += SetBase::maxRank(*p) - 1;
                        else if (p->t == TP_SET_INF_TPL)
                            maxRank += p->setInfiniteTpl()->maxTupleParts() - 1;
                        else if (p->t == TP_NULL || p->t == TP_SET_NULL)
                            maxRank -= 1;
                    }
                }
            }

            if (conv) {
                vector<CmplValAuto> dtp;
                dtp.reserve(maxRank);

                p = tpl->at(0);
                for (unsigned i = 0; i < r; i++, p++) {
                    if (p->t != TP_NULL && p->t != TP_SET_NULL) {
                        if (p->isSet() && !SetBase::rank1(*p)) {
                            if (!p->useValP() || p->setBase()->isCanonical()) {
                                SetBase::partsToVector(*p, dtp, true);
                            }
                            else {
                                CmplValAuto pc;
                                bool b = SetUtil::canonicalSet(pc, *p);
                                SetBase::partsToVector((b ? pc : *p), dtp, true);
                            }
                        }
                        else {
                            dtp.emplace_back(p);
                        }
                    }
                }

                r = dtp.size();
                if (r == 0) {
                    res.set(TP_ITUPLE_NULL);
                }
                else if (r == 1 && dtp[0].isScalarIndex()) {
                    res.set(TP_INDEX_VAL_TUPLE(dtp[0].t), dtp[0].v.i);
                }
                else {
                    Tuple *dest = new Tuple(r);
                    res.set(TP_TUPLE, dest);

                    p = dest->at(0);
                    for (unsigned i = 0; i < r; i++, p++)
                        p->moveFrom(dtp[i], false);
                }
            }
            else {
                res.copyFrom(src, true, false);
            }
        }
        else {
            if (src.isScalarIndex()) {
                src.stringPToStr(ctx->modp());
                res.set(TP_INDEX_VAL_TUPLE(src.t), src.v.i);
            }
            else if (src.t == TP_NULL || src.t == TP_SET_NULL) {
                res.set(TP_ITUPLE_NULL);
            }
            else {
                Tuple *tpl = new Tuple(1);
                tpl->at(0)->copyFrom(src, true, false);
                res.set(TP_TUPLE, tpl);

                if (src.isSet() && !SetBase::rank1(src)) {
                    CmplVal v;
                    canonicalTuple(ctx, v, res);
                    res.moveFrom(v, true);
                }
            }
        }
    }


    /**
     * check if the tuple only contains simple index values, sets, or elements with type TP_DEF_CB_SYM (for other elements converting in set is tried)
     * @param ctx			execution context
     * @param res           store for result tuple
     * @param src           source tuple
     * @param tcr			transform result tuple to canonical set representation
     * @return              true if tuple only contains simple index values, sets, or elements with type TP_DEF_CB_SYM
     */
    bool TupleUtil::asTupleIndexOrSet(ExecContext *ctx, CmplVal& res, CmplVal& src, bool tcr)
    {
        bool b = true;

        if (src.isTuple()) {
            if (src.isITuple()) {
                res.copyFrom(src, true, false);
            }
            else if (src.t == TP_TUPLE_EMPTY) {
                res.set(TP_SET_ALL);
            }
            else {
                Tuple *tpl = src.tuple();
                unsigned r = tpl->rank();
                CmplValAuto rr(TP_EMPTY);

                CmplVal *p = tpl->at(0);
                for (unsigned i = 0; i < r; i++, p++) {
                    if (p->isScalarIndex() || p->isSet() || p->t == TP_DEF_CB_SYM) {
                        if (rr)
                            rr.tuple()->at(i)->copyFrom(p, true, false);
                    }
                    else {
                        CmplValAuto s;
                        if (SetUtil::convertToSetOrTuple(ctx, s, *p, typeConversionExact)) {
                            if (!rr) {
                                rr.set(TP_TUPLE, new Tuple(r));
                                for (unsigned j = 0; j < i; j++)
                                    rr.tuple()->at(j)->copyFrom(tpl->at(j), true, false);
                            }

                            // because a tuple cannot contain directly other tuples, the only tuple value s can have here is a simple scalar index value tuple
                            if (s.t == TP_ITUPLE_1INT || s.t == TP_ITUPLE_1STR)
                                s.t = (s.t == TP_ITUPLE_1INT ? TP_INT : TP_STR);

                            rr.tuple()->at(i)->moveFrom(s, false);
                        }
                        else {
                            b = false;
                            break;
                        }
                    }
                }

                if (b) {
                    if (tcr)
                        canonicalTuple(ctx, res, (rr ?: src));
                    else if (rr)
                        res.moveFrom(rr);
                    else
                        res.copyFrom(src);
                }
            }
        }
        else if (src.isScalarIndex()) {
            src.stringPToStr(ctx->modp());
            res.set(TP_INDEX_VAL_TUPLE(src.t), src.v.i);
        }
        else {
            b = SetUtil::convertToSetOrTuple(ctx, res, src, typeConversionExact);
        }

        return b;
    }


    /**
     * try to match first tuple with a second tuple
     * @param ctx			execution context
     * @param se			syntax element id of operation
     * @param t1            first tuple (can contain TP_DEF_CB_SYM, which can match with any part of the second tuple, also with no part or more than one part)
     * @param t2            second tuple
     * @param ait           if given then for every part of the first tuple is registered in this array how many parts of the second tuple it is assigned in the matching result
     * @return              true if the tuples match
     */
    bool TupleUtil::matchTuple(ExecContext *ctx, unsigned se, CmplVal& t1, CmplVal& t2, unsigned *ait)
    {
        unsigned r1 = Tuple::rank(t1);
        unsigned r2 = Tuple::rank(t2);

        if (!r1) {
            return (!r2);
        }

        else if (r1 == 1) {
            bool b = false;

            if (t1.useValP() && t1.tuple()->at(0)->t == TP_DEF_CB_SYM) {
                b = true;
                if (ait)
                    ait[0] = r2;
            }
            else if (r2 == 1) {
                const CmplVal *v1 = Tuple::at(t1, 0);
                const CmplVal *v2 = Tuple::at(t2, 0);
                b = OperationBase::compEq2(ctx, se, v1, v2);

                if (b && ait)
                    ait[0] = 1;
            }

            return b;
        }

        else {
            // because t1 has at least rank 2 it is useValP()
            Tuple *t1p = t1.tuple();
            CmplVal *p1 = t1p->at(0);
            unsigned i1;
            unsigned cntCbs = 0;
            unsigned firstCbs, lastCbs;

            for (i1 = 0; i1 < r1; i1++, p1++) {
                if (p1->t == TP_DEF_CB_SYM) {
                    lastCbs = i1;
                    if (!cntCbs)
                        firstCbs = i1;
                    cntCbs++;
                }
            }

            if (cntCbs == r1) {
                if (ait)
                    setTupleMatchCountDefault(ait, 0, r1, r2);
                return true;
            }
            else if (r1 - cntCbs > r2) {
                return false;
            }
            else if (cntCbs == 0) {
                if (r1 != r2)
                    return false;

                // because t2 has at least rank 2 it is useValP()
                CmplVal *p2 = t2.tuple()->at(0);
                for (i1 = 0, p1 = t1p->at(0); i1 < firstCbs; i1++, p1++, p2++) {
                    if (!OperationBase::compEq2(ctx, se, p1, p2))
                        return false;
                }

                if (ait)
                    setTupleMatchCountDefault(ait, 0, r1, r2);
                return true;
            }
            else if (r2 == 1) {
                bool b = false;

                for (i1 = 0, p1 = t1p->at(0); i1 < r1; i1++, p1++) {
                    // t1 contains only one element not TP_DEF_CB_SYM here
                    if (p1->t != TP_DEF_CB_SYM) {
                        const CmplVal *p2 = Tuple::at(t2, 0);
                        b = OperationBase::compEq2(ctx, se, p1, p2);
                        break;
                    }
                }

                if (b && ait) {
                    for (unsigned a = 0; a < r1; a++)
                        ait[a] = (a == i1 ? 1 : 0);
                }

                return b;
            }

            else {
                // because t2 has at least rank 2 it is useValP()
                Tuple *t2p = t2.tuple();
                CmplVal *p2;

                if (firstCbs > 0) {
                    for (i1 = 0, p1 = t1p->at(0), p2 = t2p->at(0); i1 < firstCbs; i1++, p1++, p2++) {
                        if (!OperationBase::compEq2(ctx, se, p1, p2))
                            return false;
                        else if (ait)
                            ait[i1] = 1;
                    }
                }

                if (lastCbs < r1 - 1) {
                    for (i1 = r1 - 1, p1 = t1p->at(r1-1), p2 = t2p->at(r2-1); i1 > lastCbs; i1--, p1--, p2--) {
                        if (!OperationBase::compEq2(ctx, se, p1, p2))
                            return false;
                        else if (ait)
                            ait[i1] = 1;
                    }
                }

                if (lastCbs - firstCbs + 1 == cntCbs) {
                    if (ait)
                        setTupleMatchCountDefault(ait, firstCbs, lastCbs+1, r2 - (r1 - cntCbs));
                    return true;
                }

                return matchTupleRec(ctx, se, t1p, t2p, ait, firstCbs, lastCbs+1, firstCbs, r2 - (r1 - lastCbs - 1));
            }
        }
    }


    /**
     * try to match range in first tuple with a range in second tuple
     * @param ctx			execution context
     * @param se			syntax element id of operation
     * @param t1            first tuple (can contain TP_DEF_CB_SYM, which can match with any part of the second tuple, also with no part or more than one part)
     * @param t2            second tuple
     * @param ait           if given then for every part of the first tuple is registered in this array how many parts of the second tuple it is assigned in the matching result
     * @param t1f           considered range in tuple t1 from
     * @param t1t           considered range in tuple t1 to
     * @param t2f           considered range in tuple t2 from
     * @param t2t           considered range in tuple t2 to
     * @return              true if considered range of the tuples match
     */
    bool TupleUtil::matchTupleRec(ExecContext *ctx, unsigned se, Tuple *t1, Tuple *t2, unsigned *ait, unsigned t1f, unsigned t1t, unsigned t2f, unsigned t2t)
    {
        if (t1f >= t1t) {
            // no more elements to consider in t1
            return (t2f >= t2t);
        }

        else if (t2f >= t2t) {
            // no more elements to consider in t2
            CmplVal *p1 = t1->at(t1f);
            unsigned i1;
            for (i1 = t1f; i1 < t1t && p1->t == TP_DEF_CB_SYM; i1++, p1++) ;

            if (i1 == t1t) {
                if (ait)
                    setTupleMatchCountDefault(ait, t1f, t1t, 0);
                return true;
            }

            return false;
        }

        else {
            CmplVal *p1 = t1->at(t1f);
            CmplVal *p2 = t2->at(t2f);
            unsigned i1, i2;

            if (p1->t != TP_DEF_CB_SYM) {
                for (i1 = t1f, i2 = t2f; i1 < t1t && i2 < t2t && p1->t != TP_DEF_CB_SYM; i1++, i2++, p1++, p2++) {
                    if (!OperationBase::compEq2(ctx, se, p1, p2))
                        return false;
                    else if (ait)
                        ait[i1] = 1;
                }

                if (i1 == t1t)
                    return (i2 == t2t);
                else
                    return matchTupleRec(ctx, se, t1, t2, ait, i1, t1t, i2, t2t);
            }

            else {
                for (i1 = t1f + 1, p1++; i1 < t1t && p1->t == TP_DEF_CB_SYM; i1++, p1++) ;
                if (i1 >= t1t) {
                    if (ait)
                        setTupleMatchCountDefault(ait, t1f, t1t, t2t - t2f);
                    return true;
                }

                unsigned i1b = i1 + 1;
                CmplVal *p1b = p1 + 1;
                unsigned cncbs = 0;
                bool hcbs = false;
                for (; i1b < t1t; i1b++, p1b++) {
                    if (p1b->t == TP_DEF_CB_SYM)
                        hcbs = true;
                    else
                        cncbs++;
                }

                unsigned t2tb = t2t - cncbs;
                unsigned t2fb = (hcbs ? t2f : t2tb);

                for (i2 = t2fb, p2 = t2->at(t2fb); i2 < t2tb; i2++, p2++) {
                    if (OperationBase::compEq2(ctx, se, p1, p2) && matchTupleRec(ctx, se, t1, t2, ait, i1+1, t1t, i2+1, t2t)) {
                        if (ait) {
                            setTupleMatchCountDefault(ait, t1f, i1, i2 - t2f);
                            ait[i1] = 1;
                        }
                        return true;
                    }
                }

                return false;
            }
        }
    }


    /**
     * inserts match info for tuple parts, with default assignment
     * @param ait           array for registering how many parts of the second tuple are assigned to a part of the first tuple in the matching result
     * @param tf            considered range of the first tuple from
     * @param tt            considered range of the first tuple to
     * @param rrank         remaining rank of second tuple to assign here
     */
    void TupleUtil::setTupleMatchCountDefault(unsigned *ait, unsigned tf, unsigned tt, unsigned rrank)
    {
        // every one part of the first tuple is matched to one part of the second tuple if remain any, else to zero parts of the second tuple.
        // but the last considered part of the first tuple match all remaining parts of the second tuple
        unsigned *dp = ait + tf;
        for (unsigned i = tf; i < tt - 1; i++, dp++) {
            if (rrank) {
                *dp = 1;
                rrank--;
            }
            else {
                *dp = 0;
            }
        }

        *dp = rrank;
    }



    /****** TupleMatching ****/

    /**
     * constructor
     * @param ctx			execution context
     * @param mode          matching mode
     * @param src           source tuple (mode == matchIn) or finite set
     * @param pat           tuple pattern to match <code>src</code> against
     * @param uo            use user order
     */
    TupleMatching::TupleMatching(ExecContext *ctx, Mode mode, const CmplVal& src, const CmplVal& pat, bool uo):
        _ctx(ctx), _mode(mode), _useOrder(uo), _assInfo(NULL), _resIndex(NULL), _minRank(NULL), _maxRank(NULL)
    {
        CmplValAuto t(src);
        if (!TupleUtil::asTupleIndexOrSet(ctx, _src, t, false)) {
            throw TupleMatchingValueException("value contains inappropriate values for tuple matching", true);
        }

        t.copyFrom(pat);
        if (!TupleUtil::asTupleIndexOrSet(ctx, _pat, t, true)) {
            throw TupleMatchingValueException("value contains inappropriate values for tuple matching", false);
        }
    }


    /**
     * try to match _src with the elements of the tuple pattern _pat and make a result set of all matching tuples
     * @param res           store for result set
     * @param tcr			transform result set to canonical set representation
     * @return              true if any matching tuples, then result set is filled
     */
    bool TupleMatching::match(CmplVal& res, bool tcr)
    {
        bool b = false;
        unsigned long ind;

        releaseResult();
        res.set(TP_SET_EMPTY);

        CmplValAuto patOrg(TP_EMPTY);
        if (_mode == matchIter)
            patOrg.copyFrom(_pat);

        prepareSource();
        if (_srcCnt)
            prepareTuple();

        if (_mode == matchIter && !_freePos.empty())
            _assInfo = new CBAssignInfoComplex(patOrg, &_freePos);
        else if (_mode == matchIndex)
            _resIndex = new vector<unsigned long>();


        if (!_pat || !_srcCnt) {
            if (_mode == matchIndex && _freePos.empty())
                throw TupleMatchingValueException("array doesn't contain element with given index tuple", false);
        }

        else if (_indexTuple) {
            if (!_src.isSet()) {
                CmplValAuto t;
                if (!SetUtil::convertToSetOrTuple(_ctx, t, _src, typeConversionExact, false))
                    throw TupleMatchingValueException("value must be a tuple or set", true);
                _src.moveFrom(t, true);
            }

            b = SetUtil::tupleInSet(_ctx, _src, _pat, ind);
            if (b) {
                reduceAddTuple(res, _pat, _minRank, false, ind);
            }
            else if (_mode == matchIndex && _freePos.empty()) {
                    throw TupleMatchingValueException("array doesn't contain element with given index tuple", false);
            }
        }

        else if (0 && _oneRank && !_infTpl && _tplCnt < _srcCnt && (!_useOrder || !_srcUO || (_mode != matchIter && _mode != matchReduce)))
        {
            if (!_src.isSet()) {
                CmplValAuto t;
                if (!SetUtil::convertToSetOrTuple(_ctx, t, _src, typeConversionExact, false))
                    throw TupleMatchingValueException("value must be a tuple or set", true);
                _src.moveFrom(t, true);
            }

            bool uo = (_useOrder && _patOrder != 1 && _mode != matchIn);
            TupleIterator it(_ctx, _pat, uo);
            _patOrder = 0;

            for (it.begin(); it; it++) {
                if (SetUtil::tupleInSet(_ctx, _src, *it, ind)) {
                    b = true;
                    reduceAddTuple(res, *it, _minRank, uo, ind);

                    if (_freePos.empty())
                        break;
                }
            }
        }

        else
        {
            unique_ptr<unsigned[]> amp{new unsigned[_tplRank]};
            unsigned *am = amp.get();

            bool uo = (_useOrder && !_patOrder && _mode != matchIn);
            TupleIterator it(_ctx, _src, uo);

            for (it.begin(); it; it++) {
                if (singleMatch(*it, am)) {
                    b = true;
                    reduceAddTuple(res, *it, am, uo, it.tupleIndex());

                    if (_freePos.empty())
                        break;
                }
            }

            if (b && _uoSort.size() > 1)
                addResUO(res);
        }

        if (b && tcr) {
            CmplVal t;
            if (SetUtil::canonicalSet(t, res))
                res.moveFrom(t, true);
        }

        return b;
    }


    /**
     * take assign info result of tuple matching for iteration
     * @return              assign info object for codeblock symbols in iteration
     */
    CBAssignInfoBasis *TupleMatching::takeAssignInfo()
    {
        CBAssignInfoBasis *res = NULL;

        if (_assInfo) {
            CBAssignInfoTuple *ait = _assInfo->asAssignInfoTuple(0);
            if (ait) {
                res = ait;
                DELETE_UNSET(_assInfo);
            }
            else {
                res = _assInfo;
                _assInfo = NULL;
            }
        }

        return res;
    }


    /**
     * release result values
     */
    void TupleMatching::releaseResult()
    {
        _freePos.clear();
        DELETE_UNSET(_minRank);
        DELETE_UNSET(_maxRank);
        DELETE_UNSET(_assInfo);
        DELETE_UNSET(_resIndex);

        if (_uoSetSort.size()) {
            for (unsigned i = 0; i < _uoInvSort.size(); i++) {
                if (_uoInvSort[i])
                    delete[] _uoInvSort[i];
            }

            _uoSetSort.clear();
            _uoInvSort.clear();
        }
    }


    /**
     * prepare <code>_src</code> for matching
     */
    void TupleMatching::prepareSource()
    {
        _srcCnt = 1;
        _srcMaR = 0;
        _srcUO = false;

        if (!_src.isSet() && !_src.isTuple()) {
            CmplValAuto t;
            if (!SetUtil::convertToSetOrTuple(_ctx, t, _src, typeConversionExact, true))
                throw TupleMatchingValueException("value must be a tuple or set", true);
            _src.moveFrom(t, true);
        }

        if (_src.isITuple()) {
            _srcMaR = Tuple::rank(_src);
        }
        else {
            if (_src.isTuple() && _src.useValP()) {
                Tuple *tpl = _src.tuple();
                unsigned r = tpl->rank();

                CmplVal *p = tpl->at(0);
                for (unsigned i = 0; i < r; i++, p++) {
                    if (p->isSetFin()) {
                        _srcCnt *= SetBase::cnt(*p);
                        _srcMaR += SetBase::maxRank(*p);
                        _srcUO = _srcUO || SetBase::hasUserOrder(*p);
                    }
                    else if (p->isScalarIndex()) {
                        _srcMaR++;
                    }
                    else {
                        throw TupleMatchingValueException("value must be a finite set", true);
                    }
                }
            }
            else if (_src.isSetFin()) {
                _srcCnt = SetBase::cnt(_src);
                _srcMaR = SetBase::maxRank(_src);
                _srcUO = SetBase::hasUserOrder(_src);
            }
            else {
                throw TupleMatchingValueException("value must be a finite set", true);
            }

            if (_mode == matchIn && _srcCnt != 1)
                throw TupleMatchingValueException("value must be a single index tuple", true);
        }
    }


    /**
     * prepare <code>_pat</code> for matching, and fills temporary instance variables
     */
    void TupleMatching::prepareTuple()
    {
        // if set is given instead of tuple then convert it to canonical tuple
        if (_pat.isSet()) {
            bool sr1 = SetBase::rank1(_pat);
            Tuple *tpl = new Tuple(1);

            tpl->at(0)->moveFrom(_pat, false);
            _pat.set(TP_TUPLE, tpl);

            if (!sr1) {
                CmplVal t;
                TupleUtil::canonicalTuple(_ctx, t, _pat);
                _pat.moveFrom(t, true);
            }
        }

        // check elements of tuple
        unsigned r = Tuple::rank(_pat);
        unsigned miR, maR;

        _oneRank = true;
        _indexTuple = true;
        _infTpl = false;
        _tplCnt = 1;
        _minRankSum = 0;
        _patOrder = 0;

        if (r) {
            _minRank = new unsigned[r];
            _maxRank = new unsigned[r];

            if (_pat.useValP()) {
                Tuple *ts = _pat.tuple();
                Tuple *tr = NULL;
                CmplValAuto n, nt;

                CmplVal *e = ts->at(0);
                for (unsigned i = 0; i < r; i++, e++) {
                    if (e->t == TP_DEF_CB_SYM) {
                        if (_mode != matchIter)
                            throw TupleMatchingValueException("tuple value contains inappropriate values for tuple matching", false);

                        // replace TP_DEF_CB_SYM by TP_SET_ALL
                        n.set(TP_SET_ALL);
                        _freePos.push_back(i);
                    }
                    else if (e->isSet()) {
                        if (e->t == TP_SET_1INT || e->t == TP_SET_1STR)
                            n.set(TP_INDEX_VAL_BASE(e->t), e->v.i);
                        else if (_mode == matchIter)
                            throw TupleMatchingValueException("tuple value contains inappropriate values for tuple matching", false);

                        if (e->t == TP_SET_EMPTY) {
                            _infTpl = false;
                            _tplCnt = 0;
                            break;
                        }
                        else if ((_mode == matchReduce || _mode == matchIndex) && !SetBase::markNF(e)) {
                            _freePos.push_back(i);

                            if (_mode == matchIndex && _patOrder < 2 && ((e->isSetFin() && SetBase::cnt(e) > 1) || e->t == TP_SET_R1_LB_INF || e->t == TP_SET_R1A_INT || e->t == TP_SET_ALL_INT))
                                _patOrder = (SetBase::hasUserOrder(e) ? 2 : 1);
                        }
                    }

                    CmplVal& vr = (n ?: *e);
                    if (vr.isSet()) {
                        miR = SetBase::minRank(vr);
                        maR = SetBase::maxRank(vr, _srcMaR);
                        _indexTuple = false;

                        if (!_infTpl) {
                            if (vr.isSetInf())
                                _infTpl = true;
                            else
                                _tplCnt *= SetBase::cnt(vr);
                        }
                    }
                    else {
                        miR = maR = 1;
                    }

                    _minRank[i] = miR;
                    _maxRank[i] = maR;
                    _minRankSum += miR;
                    if (miR != maR)
                        _oneRank = false;

                    if (n) {
                        if (!tr) {
                            tr = new Tuple(r);
                            nt.set(TP_TUPLE, tr);

                            for (unsigned j = 0; j < i; j++)
                                tr->at(j)->copyFrom(ts->at(j), true, false);
                        }
                        tr->at(i)->moveFrom(n, false);
                    }
                    else if (tr) {
                        tr->at(i)->copyFrom(e, true, false);
                    }
                }

                if (nt)
                    _pat.moveFrom(nt, true);
            }
            else {
                // <code>_pat</code> can only be a rank 1 index tuple
                _minRank[0] = 1;
                _maxRank[0] = 1;
                _minRankSum = 1;
            }
        }

        if ((!_infTpl && !_tplCnt) || _minRankSum > _srcMaR) {
            _pat.dispUnset();
            _freePos.clear();
            DELETE_UNSET(_minRank);
            DELETE_UNSET(_maxRank);
        }
        else {
            if (_oneRank) {
                DELETE_UNSET(_maxRank);
                _maxRankSum = _minRankSum;
                _firstDiffRank = _afterDiffRank = 0;
            }
            else {
                if (_minRankSum == _srcMaR) {
                    _oneRank = true;
                    DELETE_UNSET(_maxRank);
                    _maxRankSum = _minRankSum;
                    _firstDiffRank = _afterDiffRank = 0;
                }
                else {
                    unsigned dr = _srcMaR - _minRankSum;
                    _maxRankSum = 0;
                    _afterDiffRank = 0;

                    for (unsigned i = 0; i < r; i++) {
                        if (_maxRank[i] != _minRank[i]) {
                            if (_maxRank[i] > _minRank[i] + dr)
                                _maxRank[i] = _minRank[i] + dr;

                            if (!_afterDiffRank)
                                _firstDiffRank = i;
                            _afterDiffRank = i + 1;
                        }

                        _maxRankSum += _maxRank[i];
                    }
                }
            }

            if (_patOrder == 2 && _useOrder) {
                // fill _uoInvSort with arrays of inverse user order
                CmplVal *e = _pat.tuple()->at(0);
                for (unsigned i = 0; i < r; i++, e++) {

                    if (e->isSetFin() && SetBase::hasUserOrder(e)) {
                        _uoSetSort.push_back(e);

                        unsigned long cs = SetBase::cnt(e);
                        unsigned long *ord = SetIterator::copyOrder(e);

                        unsigned long *inv;
                        _uoInvSort.push_back((inv = new unsigned long[cs]));

                        for (unsigned long n = 0; n < cs; n++)
                            inv[ord[n]] = n;
                    }
                    else {
                        _uoSetSort.push_back(NULL);
                        _uoInvSort.push_back(NULL);
                    }
                }
            }
        }

        _tplRank = r;
    }


    /**
     * matches single index tuple against <code>_tpl</code>
     * @param it            index tuple
     * @param am            return of matching rank per element in <code>_tpl</code>
     * @return              true if successfully matched
     */
    bool TupleMatching::singleMatch(const CmplVal& it, unsigned *am)
    {
        unsigned long dummy;

        unsigned itr = Tuple::rank(it);
        if (itr < _minRankSum || itr > _maxRankSum)
            return false;

        if (!itr) {
            unsigned *amp = am;
            for (unsigned r = 0; r < _tplRank; r++, amp++)
                *amp = 0;

            return true;
        }

        else if (_tplRank == 1)
        {
            *am = itr;
            if (!_pat.useValP()) {
                // _maxRankSum can only be 1 here, so itr must be also 1
                return (it.v.i == _pat.v.i && it.t == TP_INDEX_VAL_BASE(_pat.t));
            }
            else {
                CmplVal *v = _pat.tuple()->at(0);
                if (v->isSet())
                    return SetUtil::tupleInSet(_ctx, v, it, dummy);
                else
                    return (it.v.i == v->v.i && it.t == v->t);
            }
        }

        else if (itr == 1) {
            bool b = false;
            unsigned i;

            for (i = 0; i < _tplRank; i++) {
                if (_minRank[i] == 1 || (!_minRankSum && _maxRank[i])) {
                    CmplVal *v = _pat.tuple()->at(i);
                    if (v->isSet())
                        b = SetUtil::tupleInSet(_ctx, v, it, dummy);
                    else
                        b = (it.v.i == v->v.i && TP_INDEX_VAL_BASE(it.t) == v->t);

                    if (b || _minRank[i] == 1)
                        break;
                }
            }

            if (b) {
                unsigned *amp = am;
                for (unsigned r = 0; r < _tplRank; r++, amp++)
                    *amp = (r == i ? 1 : 0);
            }

            return b;
        }

        // (<code>it</code> and <code>_tpl</code> are tupels with useValP() here, because cases with 0 or 1 elements are already handled)
        else
        {
            // match parts before first element with diffRank
            Tuple *mtpl = _pat.tuple();
            Tuple *itpl = it.tuple();

            unsigned ft = (_oneRank || itr == _minRankSum ? _tplRank : _firstDiffRank);

            unsigned i, r, miR;
            CmplVal *v = mtpl->at(0);
            unsigned *amp = am;

            for (i = 0, r = 0; i < ft; i++, v++, amp++) {
                miR = _minRank[i];
                if (miR && !matchPart(v, miR, itpl, r, miR))
                    return false;

                *amp = miR;
                r += miR;
            }

            if (ft == _tplRank)
                return true;

            unsigned fdr = r;
            unsigned rr = itr - r;

            // match parts after last element with diffRank
            if (_afterDiffRank < _tplRank) {
                v = mtpl->at(_tplRank - 1);
                amp = am + (_tplRank - 1);
                for (i = _tplRank - 1, r = itr; i >= _afterDiffRank; i--, v--, amp--)
                {
                    miR = _minRank[i];
                    r -= miR;
                    rr -= miR;

                    if (miR && !matchPart(v, miR, itpl, r, miR))
                        return false;

                    *amp = miR;
                }
            }

            if (_firstDiffRank + 1 == _afterDiffRank) {
                // only one element with diffRank exists, so it must take the full remaining rank
                am[_firstDiffRank] = rr;
                return matchPart(mtpl->at(_firstDiffRank), _minRank[_firstDiffRank], itpl, fdr, rr);
            }
            else {
                // recursive search for a match
                return singleMatchRec(mtpl, itpl, am, _firstDiffRank, _afterDiffRank, fdr, fdr + rr);
            }
        }
    }


    /**
     * try to match range in first tuple with a range in second tuple
     * @param t1            first tuple (can contain sets and index values)
     * @param t2            second tuple (contains only index values)
     * @param am            for every part of the first tuple is registered in this array how many parts of the second tuple it is assigned in the matching result
     * @param t1f           considered range in tuple t1 from
     * @param t1t           considered range in tuple t1 to
     * @param t2f           considered range in tuple t2 from
     * @param t2t           considered range in tuple t2 to
     * @return              true if considered range of the tuples match
     */
    bool TupleMatching::singleMatchRec(Tuple *t1, Tuple *t2, unsigned *am, unsigned t1f, unsigned t1t, unsigned t2f, unsigned t2t)
    {
        if (t1f >= t1t) {
            // no more elements to consider in t1
            return (t2f >= t2t);
        }

        else if (t2f >= t2t) {
            // no more elements to consider in t2
            CmplVal *p1 = t1->at(t1f);
            unsigned i1;

            for (i1 = t1f; i1 < t1t && _minRank[i1] == 0; i1++, p1++)
                am[i1] = 0;

            return (i1 == t1t);
        }

        else {
            CmplVal *p1 = t1->at(t1f);
            unsigned i1, i2;
            unsigned miR, maR;

            if (_minRank[t1f] == _maxRank[t1f]) {
                for (i1 = t1f, i2 = t2f; i1 < t1t && i2 < t2t && _minRank[i1] == _maxRank[i1]; i1++, p1++) {
                    miR = _minRank[i1];
                    if (!matchPart(p1, miR, t2, i2, miR))
                        return false;

                    i2 += miR;
                    am[i1] = miR;
                }

                if (i1 == t1t)
                    return (i2 == t2t);
                else
                    return singleMatchRec(t1, t2, am, i1, t1t, i2, t2t);
            }

            else {
                // determine corrected range for minimal and maximal rank for element at t1f
                unsigned miRt = 0, maRt = 0;
                for (i1 = t1f + 1; i1 < t1t; i1++) {
                    miRt += _minRank[i1];
                    maRt += _maxRank[i1];
                }

                miR = max(_minRank[t1f], (t2t - t2f > maRt ? t2t - t2f - maRt : 0));
                maR = min(_maxRank[t1f], (t2t - t2f > miRt ? t2t - t2f - miRt : 0));

                if (miR > maR)
                    return false;

                // check for matching rang 1 part
                if (miR <= 1 && maR) {
                    if (matchPart(p1, miR, t2, t2f, 1) && singleMatchRec(t1, t2, am, t1f + 1, t1t, t2f + 1, t2t)) {
                        am[t1f] = 1;
                        return true;
                    }
                }

                // check for matching rang 0 part
                if (miR == 0) {
                    if (singleMatchRec(t1, t2, am, t1f + 1, t1t, t2f, t2t)) {
                        am[t1f] = 0;
                        return true;
                    }
                }

                // check for matching parts with rang from 2
                if (maR >= 2) {
                    for (unsigned r2 = 2; r2 <= maR; r2++) {
                        if (matchPart(p1, miR, t2, t2f, r2) && singleMatchRec(t1, t2, am, t1f + 1, t1t, t2f + r2, t2t)) {
                            am[t1f] = r2;
                            return true;
                        }
                    }
                }

                return false;
            }
        }
    }


    /**
     * try to match part of iteration tuple
     * @param set           set or simple index value to match tuple part against
     * @param mr            minimal rank of <code>set</code>
     * @param itpl          iteration tuple
     * @param rf            first element number of regarding part of the iteration tuple
     * @param rc            count of elements in regarding part of the iteration tuple
     * @return              true if iteration tuple part matches
     */
    bool TupleMatching::matchPart(CmplVal *set, unsigned mr, Tuple *itpl, unsigned rf, unsigned rc)
    {
        CmplVal *p;

        if (set->isSet()) {
            if (mr > rc) {
                return false;
            }
            else if (rc == 0) {
                return (mr == 0);
            }
            else {
                unsigned long dummy;

                if (rc == 1) {
                    p = itpl->at(rf);
                    return SetUtil::tupleInSet(_ctx, set, p, dummy);
                }
                else {
                    CmplVal vtpl(TP_TUPLE, itpl, false);
                    unsigned t = SetUtil::tupleInSet(_ctx, set, vtpl, dummy, rf, rc);
                    return (t == rc + 1);
                }
            }
        }
        else if (rc == 1) {
            p = itpl->at(rf);
            return (set->v.i == p->v.i && set->t == p->t);
        }
        else {
            return false;
        }
    }


    /**
     * reduce given tuple according to positions in <code>_freePos</code>, then add it to result set
     * @param res           result set
     * @param tpl           source tuple
     * @param ranks         rank per element in match tuple <code>tpl</code>
     * @param uo            add to result set with user order
     * @param srcInd        index of tuple in source set
     * @return              true if tuple is added to result set / false if tuple was already in result set
     */
    bool TupleMatching::reduceAddTuple(CmplVal& res, const CmplVal& tpl, unsigned *ranks, bool uo, unsigned long srcInd)
    {
        CmplValAuto redTpl;
        if (_freePos.empty()) {
            redTpl.set(TP_ITUPLE_NULL);
        }
        else {
            unsigned rr = 0;
            for (unsigned fp : _freePos)
                rr += ranks[fp];

            if (rr == Tuple::rank(tpl)) {
                redTpl.copyFrom(tpl, true, false);
            }
            else if (rr == 0) {
                redTpl.set(TP_ITUPLE_NULL);
            }
            else if (rr == 1) {
                unsigned r = 0;
                for (unsigned fp : _freePos) {
                    if (ranks[fp]) {
                        for (unsigned i = 0; i < fp; i++)
                            r += ranks[i];

                        const CmplVal *v = Tuple::at(tpl, r);
                        redTpl.set(TP_INDEX_VAL_TUPLE(v->t), v->v.i);
                        break;
                    }
                }
            }
            else {
                Tuple *t = new Tuple(rr);
                redTpl.set(TP_ITUPLE, t);

                unsigned te = 0;
                unsigned r = 0, il = 0;

                for (unsigned fp : _freePos) {
                    if (ranks[fp]) {
                        for (unsigned i = il; i < fp; i++)
                            r += ranks[i];

                        for (unsigned i = 0; i < ranks[fp]; i++)
                            t->at(te++)->copyFrom(tpl.tuple()->at(r++), true, false);

                        il = fp + 1;
                    }
                }
            }
        }

        unsigned long pos;
        bool b = SetUtil::tupleInSet(_ctx, res, redTpl, pos);
        if (!b) {
            CmplVal t;
            pos = SetUtil::addTupleToSet(_ctx, t, res, redTpl, uo, false);
            res.moveFrom(t, true);

            if (_assInfo) {
                unsigned *ai = new unsigned[_freePos.size()];
                unsigned i = 0;
                for (unsigned fp : _freePos)
                    ai[i++] = ranks[fp];

                _assInfo->addInfo(pos, ai);
            }

            if (_resIndex) {
                _resIndex->insert(_resIndex->begin() + pos, srcInd);
            }

            if (!uo && _uoSetSort.size()) {
                _uoSort.emplace(_uoSort.begin() + pos, _ctx, redTpl, ranks, _freePos, _uoSetSort, _uoInvSort);
            }
        }

        return b;
    }


    /**
     * add user order from <code>_uoSort</code> to result set
     * @param res           result set
     */
    void TupleMatching::addResUO(CmplVal& res)
    {
        // sort tuples
        unsigned long cnt = _uoSort.size();

        for (unsigned long n = 0; n < cnt; n++)
            _uoSort[n]._pos = n;

        sort(_uoSort.begin(), _uoSort.end(), ptuples::less);

        // check sort order
        bool uo = false, rev = false;
        if (_uoSort[0]._pos == 0) {
            for (unsigned long n = 1; n < cnt; n++) {
                if (_uoSort[n]._pos != n) {
                    uo = true;
                    break;
                }
            }
        }
        else {
            uo = true;
            rev = true;
            for (unsigned long n = 0; n < cnt; n++) {
                if (_uoSort[n]._pos != cnt - 1 - n) {
                    rev = false;
                    break;
                }
            }
        }

        // add order to result set
        if (uo) {
            unsigned long *o = NULL;
            if (!rev) {
                o = new unsigned long[cnt];
                for (unsigned long n = 0; n < cnt; n++)
                    o[n] = _uoSort[n]._pos;
            }

            CmplVal t(TP_SET_WITH_UO, (rev ? new SetWithOrder(res, true) : new SetWithOrder(res, o)));
            res.moveFrom(t, true);
        }
    }



    /****** TupleMatching::ptuples ****/

    /**
     * constructor
     * @param ctx           execution context
     * @param rtpl          single result tuple
     * @param ranks         rank per element in match tuple
     * @param freePos       positions of elements marked as free within <code>_pat</code>
     * @param uoSetSort     vector of part sets within <code>_pat</code> which have user order
     * @param uoSetPat      vector of arrays with inverse user order for part sets within <code>_pat</code> which have user order
     */
    TupleMatching::ptuples::ptuples(ExecContext *ctx, const CmplVal& rtpl, unsigned *ranks, vector<unsigned>& freePos, vector<CmplVal *>& uoSetSort, vector<unsigned long *>& uoInvSort)
    {
        unsigned rs = 0;
        for (unsigned fp : freePos) {

            _tp.emplace_back(TP_ITUPLE_NULL);
            unsigned r = ranks[fp];

            if (r) {
                CmplValAuto& t = _tp.back();
                if (rtpl.useValP())
                    rtpl.tuple()->partTuple(t, rs, r);
                else
                    t.copyFrom(rtpl);

                rs += r;

                if (uoSetSort[fp]) {
                    // replace tuple t by sort number in user order
                    unsigned long n;
                    SetUtil::tupleInSet(ctx, *(uoSetSort[fp]), t, n);
                    unsigned long i = uoInvSort[fp][n];

                    t.dispSet(TP_INT, (intType)i);
                }
            }
        }
    }


    /**
     * compare two ptuples
     * @param a             first element
     * @param b             second element
     * @return              true if <code>a</code> is less than <code>b</code>
     */
    bool TupleMatching::ptuples::less(const ptuples& a, const ptuples& b)
    {
        const vector<CmplValAuto>& va = a._tp;
        const vector<CmplValAuto>& vb = b._tp;
        unsigned ca = va.size();
        unsigned cb = vb.size();

        for (unsigned i = 0; i < ca && i < cb; i++) {
            const CmplValAuto& ta = va[i];
            const CmplValAuto& tb = vb[i];

            if (ta.useInt() && tb.useInt()) {
                int cc = cmp(ta.t, ta.v.i, tb.t, tb.v.i);
                if (cc)
                    return (cc < 0);
            }

            unsigned ra = Tuple::rank(ta);
            unsigned rb = Tuple::rank(tb);
            for (unsigned r = 0; r < ra && r < rb; r++) {
                const CmplVal *ia = Tuple::at(ta, r);
                const CmplVal *ib = Tuple::at(tb, r);

                int cc = cmp(ia->t, ia->v.i, ib->t, ib->v.i);
                if (cc)
                    return (cc < 0);
            }

            if (ra != rb)
                return (ra < rb);
        }

        return (ca < cb);
    }



    /****** TupleIterator ****/

    /**
     * iterate to the first or next tuple
     * @param start			start or restart the iteration
     */
    void TupleIterator::iterIntern(bool start)
    {
        if (start) {
            _sub.clear();
            _ind = 0;
            _tplInd = 0;
            _cnt = 0;
            _ended = true;
            _cur.dispUnset();

            if (_tpl.isITuple()) {
                _cnt = 1;
                _ended = false;
                _cur.copyFrom(_tpl);
            }
            else {
                if (_tpl.isTuple())
                    _rank = Tuple::rank(_tpl);
                else
                    _rank = 1;

                if (_rank) {
                    _sub.resize(_rank);

                    _cnt = 1;
                    _ended = false;

                    unsigned mr = _rank;
                    for (unsigned r = 0; r < _rank; r++) {
                        const CmplVal *e = (_tpl.isTuple() ? Tuple::at(_tpl, r) : &_tpl);
                        if (e->isSet()) {
                            _sub[r].init(e, SetIterator::iteratorTupleVal, _useOrder, _reverse);
                            mr += SetBase::maxRank(e);
                        }
                        else {
                            CmplValAuto s;
                            if (e->isInterval())
                                SetUtil::constructFromInterval(_ctx, s, *e);
                            else if (e->isScalarIndex())
                                s.set(TP_INDEX_VAL_SET(e->t), e->v.i);
                            else if (e->t == TP_ITUPLE_NULL)
                                s.set(TP_SET_NULL);

                            _sub[r].init(s, SetIterator::iteratorTupleVal, _useOrder, _reverse);
                        }

                        _sub[r].begin();
                        _cnt *= _sub[r].count();

                        if (_sub[r].ended()) {
                            _ended = true;
                            break;
                        }
                    }

                    if (!_ended) {
                        if (_rank == 1) {
                            _cur.copyFrom(_sub[0].curTuple());
                        }
                        else {
                            _cur.set(TP_ITUPLE, new Tuple(0, mr, Tuple::tupleIndexOnly));
                            _cur.tuple()->concatIter(_sub, 0);
                        }
                    }
                }
            }
        }

        else if (!_ended) {
            _ind++;

            if (_tpl.isITuple()) {
                _cur.dispUnset();
                _ended = true;
            }
            else if (_rank == 1) {
                _sub[0]++;
                _cur.copyFrom(_sub[0].curTuple());
                _ended = _sub[0].ended();
            }
            else {
                unsigned r = _rank - 1;
                while (true) {
                    _sub[r]++;
                    if (_sub[r] || r == 0)
                        break;

                    _sub[r].begin();
                    r--;
                }

                if (_sub[0]) {
                    _cur.tuple()->concatIter(_sub, r);
                }
                else {
                    _cur.dispUnset();
                    _ended = true;
                }
            }
        }

        if (_useOrder && !_ended && _rank) {
            // set _tplInd
            _tplInd = _sub[_rank - 1].tupleIndex();

            if (_rank > 1) {
                unsigned long c = _sub[_rank - 1].count();
                for (unsigned r = _rank - 2; ; r--) {
                    _tplInd += _sub[r].tupleIndex() * c;
                    c *= _sub[r].count();

                    if (!r)
                        break;
                }
            }
        }
    }

}

