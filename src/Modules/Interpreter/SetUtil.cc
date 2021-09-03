
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


#include <math.h>
#include <map>
#include <set>

#include "SetUtil.hh"
#include "SetInfinite.hh"
#include "TupleUtil.hh"
#include "Interval.hh"
#include "Interpreter.hh"
#include "ExecContext.hh"
#include "../../Control/MainControl.hh"


namespace cmpl
{

	/****** SetUtil ****/

	/**
	 * construct tuple set of rank 1 from an interval
	 * @param ctx			execution context
	 * @param res			store for result set
	 * @param iv			interval to construct set from (must not have real number bounds)
	 */
    void SetUtil::constructFromInterval(ExecContext *ctx, CmplVal &res, const CmplVal& iv)
	{
		if (Interval::isEmpty(iv)) {
			res.set(TP_SET_EMPTY);
		}
		else if (Interval::hasLowBound(iv)) {
            intType lb = Interval::lowBoundInt(iv);

			if (Interval::hasUppBound(iv)) {
                intType ub = Interval::uppBoundInt(iv);

				if (lb == 0)
					res.set(TP_SET_R1_0_UB, ub);
				else if (lb == 1)
					res.set(TP_SET_R1_1_UB, ub);
				else
					res.set(TP_SET_R1_ALG, new SetAlg(lb, 1, (ub + 1 - lb), false));
			}
			else {
                if (lb == 0)
                    res.set(TP_SET_R1A_INT);
                else
                    res.set(TP_SET_R1_LB_INF, lb);
			}
		}
		else {
			if (Interval::hasUppBound(iv)) {
                intType ub = Interval::uppBoundInt(iv);
				res.set(TP_SET_R1_INF_UB, ub);
			}
			else {
                res.set(TP_SET_R1_IINF);
			}
		}
	}

	/**
	 * construct algorithmic tuple set of rank 1
	 * @param ctx			execution context
	 * @param res			store for result set
	 * @param start			first value
	 * @param step			step to next value (must not be 0)
	 * @param end			last value / NULL: construct incomplete set without last value
	 */
	void SetUtil::constructAlg(ExecContext *ctx, CmplVal &res, StackValue *start, StackValue *step, StackValue *end)
	{
		intType st, en, sp = 1;
		bool no = false;

		CmplVal *v = start->simpleValue();
		if (!v || !v->toInt(st, typeConversionExact, ctx->modp()))
			ctx->valueError("start value for algorithmic set must be a scalar integer value", start);

		if (step) {
			v = step->simpleValue();
			if (!v || !v->toInt(sp, typeConversionExact, ctx->modp()))
				ctx->valueError("step value for algorithmic set must be a scalar integer value", step);
			if (sp == 0)
				ctx->valueError("step value for algorithmic must not be 0", step);

			if (sp < 0) {
				sp = -sp;
				no = true;
			}
		}

		if (end) {
			v = end->simpleValue();
			if (!v || !v->toInt(en, typeConversionExact, ctx->modp()))
				ctx->valueError("end value for algorithmic set must be a scalar integer value", end);

			constructAlg(res, st, en, sp, no);
		}
        else {
            res.set(TP_SET_R1_ALG, new SetAlg(st, (unsigned long)sp));
            if (no) {
                SetWithOrder *so = new SetWithOrder(res, true);
                res.dispSet(TP_SET_WITH_UO, so);
            }
		}
	}

	/**
	 * complete construction of an algorithmic tuple set of rank 1
	 * @param ctx			execution context
	 * @param res			store for result set
	 * @param sb			set to complete (must be an incomplete set without last value)
	 * @param end			last value
	 */
    void SetUtil::constructAlg(ExecContext *ctx, CmplVal &res, SetBase *sb, StackValue *end)
	{
		intType en;
		CmplVal *v = end->simpleValue();
		if (!v || !v->toInt(en, typeConversionExact, ctx->modp()))
			ctx->valueError("end value for algorithmic set must be a scalar integer value", end);

        SetAlg *sa;
        bool no;
        if (sb->hasUserOrder()) {
            // user order can be here only reverse order
            no = true;
            sa = ((SetWithOrder *)sb)->set().setAlg();
        }
        else {
            sa = (SetAlg *)sb;
        }

        constructAlg(res, sa->start(), en, sa->step(), no);
	}

	/**
	 * construction of an algorithmic tuple set of rank 1
	 * @param res			store for result set
	 * @param st			start value
	 * @param en			end value
	 * @param sp			step value (must be greater or equal 1)
	 * @param no			reverse order
	 */
	void SetUtil::constructAlg(CmplVal &res, intType st, intType en, unsigned long sp, bool no)
	{
		if (st == en) {
			res.set(TP_SET_1INT, st);
		}
		else {
			if (no) {
				intType t = st;
				st = en;
				en = t;

                if (sp > 1 && (en - st) % sp != 0)
                    st += (en - st) % sp;
			}

			if (st > en) {
				res.set(TP_SET_EMPTY);
                no = false;
			}
            else if (sp == 1) {
                if (st == 0 || st == 1)
					res.set((st == 0 ? TP_SET_R1_0_UB : TP_SET_R1_1_UB), en);
				else
                    res.set(TP_SET_R1_ALG, new SetAlg(st, 1u, (unsigned long)(en - st + 1)));
			}
			else {
                res.set(TP_SET_R1_ALG, new SetAlg(st, (unsigned long)sp, (unsigned long)((en - st) / sp + 1)));
			}

            if (no) {
                SetWithOrder *so = new SetWithOrder(res, true);
                res.dispSet(TP_SET_WITH_UO, so);
            }
        }
	}


	/**
     * compare two sets for complete equality and for an equal base (not for infinite sets, only for sets in canonical representation)
     * @param s1a			first set
     * @param s2a			second set
	 * @param eqr			check equal base up to this rank, return up to which rank the bases are equal
	 * @param cc			check also for complete equality
	 * @param uo			check user order
	 * @return				true if both sets are complete equal (if !cc then return ever false)
	 */
    bool SetUtil::compareEqRank(CmplVal *s1a, CmplVal *s2a, unsigned& eqr, bool cc, bool uo)
	{
        if (*s1a == *s2a)
            return cc;

        CmplVal *s1 = &SET_VAL_WO_ORDER(*s1a);
        CmplVal *s2 = &SET_VAL_WO_ORDER(*s2a);

        if (!uo) {
            if (*s1 == *s2)
                return cc;
        }
        else {
            bool uod1 = SET_VAL_HAS_ORDER(*s1a);
            bool uod2 = SET_VAL_HAS_ORDER(*s2a);
            if (uod1 || uod2) {
                uo = false;
                if (!uod1 || !uod2 || !s1a->setWithOrder()->equalOrder(s2a->setWithOrder())) {
                    eqr = 0;
                    return false;
                }
            }
        }

		if (s1->t == s2->t && (!*s1 || s1->isSetFin() || s1->isScalarIndex()) && (eqr || cc)) {
            if (!*s1 || s1->useNothing() || s1->useInt()) {
                if (s1->useInt() && s1->v.i != s2->v.i) {
                    eqr = 0;
                    return false;
                }
                else {
                    eqr = (s1 && s1->rank1() && eqr ? 1 : 0);
                    return cc;
                }
			}

			unsigned mr1 = s1->setBase()->maxRank();
			if (eqr > mr1)
				eqr = mr1;

			if (s1->v.vp == s2->v.vp || s1->rank1()) {
                if (s1->v.vp == s2->v.vp || (s1->t == TP_SET_R1_ALG && s1->setAlg()->equal(s2->setAlg())) || (s1->t == TP_SET_R1_ENUM && s1->setEnum()->equal(s2->setEnum())))
					return cc;
			}
			else {
				SetBase *sb1 = s1->setBase();
				SetBase *sb2 = s2->setBase();

				unsigned mr2 = sb2->maxRank();
				if (mr1 != mr2) {
					cc = false;
					if (eqr > mr2)
						eqr = mr2;
				}

				if (sb1->cnt() != sb2->cnt())
					cc = false;

				if (sb1->containNull() || sb2->containNull()) {
					eqr = 0;
					if (sb2->containNull() != sb2->containNull())
						cc = false;
				}

				if (eqr || cc) {
					if (s1->t == TP_SET_REC_MULT) {
						unsigned eqrr = 1;
						unsigned eqrm = (cc ? mr1 : eqr);

						for (unsigned i = 0; i < eqrm; i++) {
							if (!compareEqRank(((SetRecMult *)sb1)->partSet(i), ((SetRecMult *)sb2)->partSet(i), eqrr, true, uo)) {
								cc = false;
								if (eqr > i)
									eqr = i;
								break;
							}
						}
					}
					else { //if (s1->t == TP_SET_FIN)
						if (eqr) {
							cc = compareEqRank(((SetFinite *)sb1)->baseSet(), ((SetFinite *)sb2)->baseSet(), eqr, cc, uo);
						}

						if (cc) {
                            unsigned long bcnt1 = SetBase::cnt(*(((SetFinite *)sb1)->baseSet()));
                            unsigned long bcnt2 = SetBase::cnt(*(((SetFinite *)sb2)->baseSet()));

							if (bcnt1 != bcnt2) {
								cc = false;
							}
							else {
								unsigned eqrr = 0;
								for (unsigned long i = 0; i < bcnt1 && cc; i++)
									cc = compareEqRank(((SetFinite *)sb1)->subSet(i), ((SetFinite *)sb2)->subSet(i), eqrr, cc, uo);
							}
						}
					}
				}

				return cc;
			}
		}

		eqr = 0;
		return false;
	}


    /**
     * test whether first set is a subset of second set
     * @param ctx			execution context
     * @param s1            first set
     * @param s2            second set
     * @return              true if first set is a subset of second set
     */
    bool SetUtil::isSubSet(ExecContext *ctx, CmplVal *s1, CmplVal *s2)
    {
        if (s2->t == TP_SET_ALL || s2->t == TP_SET_ALL_MNF)
            return true;

        //TODO: erstmal hier einfach ueber Iteration ueber s1 realisiert
        //  fuer unendlichen Set s1 erstmal immer einfach false
        if (!SetBase::finite(*s1))
            return false;

        if (SetBase::finite(*s2) && SetBase::cnt(*s1) > SetBase::cnt(*s2))
            return false;

        if (SetBase::minRank(*s1) < SetBase::minRank(*s2) || SetBase::maxRank(*s1) > SetBase::maxRank(*s2))
            return false;

        SetIterator iter(*s1);
        unsigned long num;

        for (iter.begin(); iter; iter++) {
            const CmplVal& tp = *iter;
            if (!tupleInSet(ctx, *s2, tp, num))
                return false;
        }

        return true;
    }


	/**
	 * get base set from a given set (not for infinite sets)
	 * @param res			store for result set
	 * @param set			set for getting the base from
	 * @param br			rank for base set
	 */
	void SetUtil::getBaseSet(CmplVal &res, CmplVal& set, unsigned br)
	{
		if (!br) {
			res.set(TP_SET_NULL);
		}
        else if (!set.useValP() || !SetBase::finite(set) || SetBase::finiteRank1(set)) {
			res.copyFrom(set);
		}
		else {
			CmplVal *sb = &set;
			if (set.t == TP_SET_FIN) {
				sb = set.setFinite()->baseSet();
				if (sb->t != TP_SET_REC_MULT) {
					res.copyFrom(*sb);
					return;
				}
			}

			// sb->t == TP_SET_REC_MULT
			sb->setRecMult()->partSet(res, 0, br-1);
		}
	}

	/**
	 * get first remaining subset of a set after removing a set base, and check whether all other subsets would be equal (not for infinite sets)
	 * @param res			store for result subset
	 * @param s				source set
	 * @param br			rank of the base to remove
	 * @param uo			check user order in subsets
	 * @return				true if the other subsets would be equal to the first one
	 */
	bool SetUtil::subSetFirst(CmplVal *res, CmplVal& s, unsigned br, bool uo)
	{
		if (!br) {
			res->copyFrom(s, true, false);
			return true;
		}
		else {
			subSetN(res, s, br, 0);
			if (s.t != TP_SET_FIN)
				return true;

			SetFinite *sf = s.setFinite();
			CmplVal *sb = sf->baseSet();

			unsigned long stp = 1;
			if (sb->t == TP_SET_REC_MULT && br < sb->setRecMult()->maxRank()) {
				SetRecMult *srm = sb->setRecMult();
				for (unsigned i = br; i < srm->maxRank(); i++)
                    stp *= SetBase::cnt(*(srm->partSet(i)));
			}

			unsigned long mj = sf->baseCnt() / stp;
			unsigned eqr = 0;
			bool eq = true;

			for (unsigned long i = 0; eq && i < stp; i++) {
				for (unsigned long j = 1; j < mj; j++) {
					if (!compareEqRank(sf->subSet(i), sf->subSet(i + j*stp), eqr, true, uo)) {
						eq = false;
						break;
					}
				}
			}

			return eq;
		}
	}

	/**
	 * get the i-th remaining subset of a set after removing a set base (not for infinite sets)
	 * @param res			store for result subset
	 * @param s				source set
	 * @param br			rank of the base to remove
	 * @param i				index number of subset
	 */
	void SetUtil::subSetN(CmplVal *res, CmplVal& s, unsigned br, unsigned long i)
	{
		if (!br) {
			res->copyFrom(s, true, false);
		}
		else if (s.t == TP_SET_FIN) {
			SetFinite *sf = s.setFinite();
			CmplVal *sb = sf->baseSet();

			if (sb->t != TP_SET_REC_MULT || br >= sb->setRecMult()->maxRank()) {
				// full base set of s is removed, subset remains directly as result set
				res->copyFrom(sf->subSet(i), true, false);
			}
			else {
				// only part of base set of s is removed, construct new result set from remaining base and its subsets
                CmplValAuto nsb;
				sb->setRecMult()->partSet(nsb, br, sb->setRecMult()->maxRank() - 1);

                unsigned bRank = SetBase::rank(nsb);
                unsigned bCnt = SetBase::cnt(nsb);
				unsigned sMinR, sMaxR;

				CmplVal *subs = new CmplVal[bCnt];
				CmplVal *sub = subs;
				unsigned eqr = sf->maxRank();
				bool eqc = true;

				for (unsigned long j = 0; j < bCnt; j++, sub++) {
					sub->copyFrom(sf->subSet(i*bCnt + j), true, false);

                    if (j == 0 || sMinR > SetBase::minRank(*sub))
                        sMinR = SetBase::minRank(*sub);

                    if (j == 0 || sMaxR > SetBase::maxRank(*sub))
                        sMaxR = SetBase::maxRank(*sub);

					if (j && (eqc || eqr))
						eqc = compareEqRank(subs, sub, eqr, eqc, true);
				}

				if (eqc) {
					sub = subs + 1;
					for (unsigned long j = 1; j < bCnt; j++, sub++)
						sub->copyFrom(subs);

					if (!*subs) {
						// no real subsets, subset array is not needed
						delete subs;
						res->moveFrom(nsb, false);
						return;
					}
				}

				if (!eqr) {
					// construct result set here from the given subsets
                    SetFinite *nsf = new SetFinite(nsb, subs, bRank+sMinR, bRank+sMaxR);
					res->set(TP_SET_FIN, nsf);
				}
				else {
					// part of subsets must move to a new base for the result set, use function arrayForFinSet() for it
					map<unsigned long, CmplVal> msubs;
					sub = subs;
					for (unsigned long j = 0; j < bCnt; j++, sub++)
						msubs[j+1].moveFrom(sub, false);

                    CmplVal *arr = arrayForFinSet(nsb, eqc, eqr, false, msubs, 0);
					if (!arr) {
						res->copyFrom(nsb, true, false);			
					}
					else {
                        SetFinite *nsf = new SetFinite(nsb, arr, bRank+sMinR, bRank+sMaxR);
						res->set(TP_SET_FIN, nsf);
					}

					delete subs;
					for (unsigned long j = 1; j <= bCnt; j++)
						msubs[j+1].dispose();
				}
			}
		}
		else {
			res->unset();
			if (s.t == TP_SET_REC_MULT && br < s.setRecMult()->maxRank())
				s.setRecMult()->partSet(*res, br, s.setRecMult()->maxRank() - 1);
		}
	}

	/**
	 * fill the array of subsets for <code>SetFinite</code>, and move part of subsets to the base set if possible.
	 * @param bs			base set, changed by this function if eqr > 0
	 * @param eqc			true if all subsets are equal
     * @param eqr			rank of subset part to move to the base set
     * @param rev			reverse order of subsets in <code>subs</code>
	 * @param subs			subsets, key must be numbered from 1, if a searched key is not contained the corresponding subset is treated as null
	 * @param nNull			index of null element, for this number no subset is searched in subs / 0 if no null element
	 */
    CmplVal *SetUtil::arrayForFinSet(CmplVal& bs, bool eqc, unsigned eqr, bool rev, map<unsigned long, CmplVal>& subs, unsigned long nNull)
	{
        unsigned long cnt = SetBase::cnt(bs);
		unsigned long subsMax = cnt + (nNull ? 1 : 0);

        CmplValAuto fSub;
		if (subs.size() > 0)
			fSub.copyFrom(subs.begin()->second);
		
		CmplVal *arr, *p;
		bool allEmpty = false;

		if (eqr == 0) {
			// use base and subsets as given
			arr = new CmplVal[cnt];

			if (eqc) {
				// all subsets are equal, use the first subset for all elements
				if (fSub)
					arr->copyFrom(fSub, true, false);
				else {
					arr->set(TP_EMPTY);
					allEmpty = true;
				}

				p = arr + 1;
				for (unsigned long i = 1; i < cnt; i++, p++)
					p->copyFrom(arr, true, false);
			}
			else {
				// copy subsets to element array
                p = (rev ? arr + cnt - 1 : arr);
				for (unsigned long i = 1; i <= subsMax; i++) {
					if (i != nNull) {
						if (subs.count(i)) {
							CmplVal& ss = subs[i];
							if (ss.t == TP_SET_NULL)
								p->set(TP_EMPTY);
							else
								p->copyFrom(ss, true, false);
						}
						else {
							p->set(TP_EMPTY);
                        }

                        if (rev)
                            p--;
                        else
                            p++;
					}
				}
			}
		}
		else {
			// make resulting base set
            CmplValAuto bs1, bs2;
			bs1.moveFrom(bs, false);
			getBaseSet(bs2, fSub, eqr);
            unsigned long cnt2 = SetBase::cnt(bs2);

            tp_e srmt;
            SetBase *srm = SetRecMult::construct(bs1, bs2, srmt);
            bs.set(srmt, srm);

			arr = new CmplVal[srm->cnt()];
            p = arr;

			if (eqc) {
                // all subsets are equal, use the first subset for all elements
				if (SetUtil::subSetFirst(p++, fSub, eqr, true)) {
                    if (*arr) {
						for (unsigned long j = 1; j < cnt2; j++, p++)
                            p->copyFrom(arr, true, false);
					}
					else {
						allEmpty = true;
						for (unsigned long j = 1; j < cnt2; j++, p++)
							p->set(TP_EMPTY);
					}
				}
				else {
					for (unsigned long j = 1; j < cnt2; j++, p++)
						subSetN(p, fSub, eqr, j);
				}

				for (unsigned long i = 1; i < cnt; i++) {
					for (unsigned long j = 0; j < cnt2; j++, p++) {
						if (allEmpty)
							p->set(TP_EMPTY);
						else
							p->copyFrom(p-cnt2, true, false);
					}
				}
			}
			else {
                for (unsigned long i = (rev ? subsMax : 1); (rev ? i >= 1 : i <= subsMax); i += (rev ? -1 : 1)) {
					if (i != nNull) {
						if (subs.count(i)) {
							CmplVal& ss = subs[i];

                            if (subSetFirst(p++, ss, eqr, true)) {
                                for (unsigned long j = 1; j < cnt2; j++, p++)
                                    p->copyFrom(p-1, true, false);
							}
							else {
                                for (unsigned long j = 1; j < cnt2; j++, p++)
									subSetN(p, ss, eqr, j);
							}
						}
					}
				}
			}
		}

		// if no real subset exists, then subset array is not needed
		if (allEmpty && !nNull) {
			delete arr;
			arr = NULL;
		}

		return arr;
	}


    /**
     * search a tuple or a part of a tuple within a set
     * @param ctx			execution context
     * @param seta			set to search the tuple within
     * @param tup			tuple to search (can also be TP_LIST_TUPLE, then it must be the topmost element on the value stack of the execution context)
     * @param num			return of the index number of the tuple within the set (only if set is a finite set)
     * @param tps			start index of used tuple part
     * @param tpc			max count of indizes of used tuple part (searching for the longest tuple part not longer than tpc) / -1: up to the end of tuple (searching only for the full remaining tuple)
     * @param fulltp        search only for tuple length tpc
     * @return 				count of indizes of found tuple part + 1
     */
    unsigned SetUtil::tupleInSet(ExecContext *ctx, const CmplVal &seta, const CmplVal &tup, unsigned long& num, unsigned tps, int tpc, bool fulltp)
    {
        const CmplVal& set = SET_VAL_WO_ORDER(seta);

        if (tpc < 0)
            fulltp = true;

        unsigned tr = (tup.t == TP_LIST_TUPLE ? (unsigned)tup.v.i : Tuple::rank(tup));
        tr = (tr > tps ? tr - tps : 0);
        if (tpc >= 0 && tr > (unsigned)tpc)
            tr = tpc;

        if (set.t == TP_SET_EMPTY || set.t == TP_INTERVAL_EMPTY)
            return 0;

        if (!set.isSet()) {
            if (set.isInterval() && !Interval::isSet(set)) {
                if (tr < 1 || (fulltp && tr > 1))
                    return 0;

                if (tup.t == TP_ITUPLE || tup.t == TP_LIST_TUPLE) {
                    const CmplVal *scp = (tup.t == TP_ITUPLE ? tup.tuple()->at(tps) : &((ctx->stackCur() - tup.v.i + tps)->val()));
                    return (Interval::numInInterval(set, *scp) ? 2 : 0);
                }
                else {
                    CmplVal sc;
                    sc.v.i = tup.v.i;
                    sc.t = (tup.t == TP_ITUPLE_1INT || tup.t == TP_BIN ? TP_INT : (tup.t == TP_ITUPLE_1STR ? TP_STR : tup.t));
                    return (Interval::numInInterval(set, sc) ? 2 : 0);
                }
            }

            else {
                CmplValAuto s1, s2;
                s1.copyFrom(set);
                if (SetUtil::convertToSetOrTuple(ctx, s2, s1, typeConversionExact, false))
                    return tupleInSet(ctx, s2, tup, num, tps, tpc, fulltp);
                else
                    return 0;
            }
        }

        // tuple rank and set rank
        unsigned srl, sru;
        if (SetBase::rank1(set)) {
            srl = sru = 1;
        }
        else {
            srl = SetBase::minRank(set);
            sru = (set.t == TP_SET_FIN || set.isSetInf() ? SetBase::maxRank(set) : srl);
        }

        if (tr < srl || (fulltp && tr > sru))
            return 0;

        // check for null tuple
        if (tr == 0 || sru == 0) {
            if (srl == 0 && (tr == 0 || !fulltp)) {
                num = 0;
                return 1;
            }
            return 0;
        }

        if (set.isSetFin() || ((tr == 1 || sru == 1) && !set.useValP()))
        {
            // check for scalar value
            if (tr == 1 || sru == 1) {
                if (tr == 1 || !fulltp) {
                    if (tup.t == TP_ITUPLE || tup.t == TP_LIST_TUPLE) {
                        const CmplVal *scp = (tup.t == TP_ITUPLE ? tup.tuple()->at(tps) : &((ctx->stackCur() - tup.v.i + tps)->val()));
                        if (SetIterator::isScalarIn(set, *scp, num))
                            return 2;
                    }
                    else {
                        CmplVal sc;
                        sc.v.i = tup.v.i;
                        sc.t = (tup.t == TP_ITUPLE_1INT || tup.t == TP_BIN ? TP_INT : (tup.t == TP_ITUPLE_1STR ? TP_STR : tup.t));
                        if (SetIterator::isScalarIn(set, sc, num))
                            return 2;
                    }
                }

                return (!fulltp && srl == 0 ? 1 : 0);
            }

            // set can be here only TP_SET_REC_MULT or TP_SET_FIN, because it is a finite set with max rank greater than 1
            if (set.t == TP_SET_REC_MULT) {
                // find tuple part in set parts
                SetRecMult *srm = set.setRecMult();
                unsigned long nr = 0;
                unsigned long np;

                for (unsigned r = 0; r < sru; r++) {
                    const CmplVal *ss = srm->partSet(r);

                    if (tup.t == TP_ITUPLE || tup.t == TP_LIST_TUPLE) {
                        const CmplVal *scp = (tup.t == TP_ITUPLE ? tup.tuple()->at(tps + r) : &((ctx->stackCur() - tup.v.i + tps + r)->val()));
                        if (!SetIterator::isScalarIn(*ss, *scp, np))
                            return 0;
                    }
                    else {
                        return 0;
                    }

                    if (r == 0)
                        nr = np;
                    else
                        nr = nr * SetBase::cnt(*ss) + np;
                }

                num = nr;
                return sru + 1;
            }
            else if (set.t == TP_SET_FIN) {
                // recursive calls first to find beginning tuple part in the base set, then remaining tuple part in the subset
                SetFinite *sf = set.setFinite();
                unsigned long nb;
                unsigned tpcb = sf->baseRank();

                if (tupleInSet(ctx, *(sf->baseSet()), tup, nb, tps, tpcb, true) != tpcb + 1)
                    return (!fulltp && srl == 0 ? 1 : 0);

                CmplVal *ss = sf->subSet(nb);
                if (tr == tpcb) {
                    if (ss->t == TP_EMPTY || (ss->t == TP_SET_FIN && ss->setFinite()->minRank() == 0)) {
                        num = sf->indSubStart(nb);
                        return tr + 1;
                    }
                    return 0;
                }

                unsigned long ns;
                unsigned sstp = tupleInSet(ctx, *ss, tup, ns, tps + tpcb, tr - tpcb, fulltp);
                if (sstp) {
                    num = sf->indSubStart(nb) + ns;
                    return tpcb + sstp;
                }

                return 0;
            }
        }

        else {
            if (set.useValP()) {
                // infinite set TP_SET_INF_TPL or TP_SET_INF_SET
                SetInfinite *inf = set.setInfinite();
                unsigned r = tr;
                for (; r > 0; r--) {
                    if (inf->tupleInSet(ctx, tup, tps, r))
                        return r + 1;
                    else if (fulltp)
                        return 0;
                }

                return (srl == 0 ? 1 : 0);
            }

            else {
                // infinite set of arbitrary rank TP_SET_ALL*
                if (set.t == TP_SET_ALL || set.t == TP_SET_ALL_MNF)
                    return tr + 1;

                bool intElem = set.elemInt();
                unsigned r = 0;
                if (tup.t == TP_ITUPLE || tup.t == TP_LIST_TUPLE) {
                    for (; r < tr; r++) {
                        const CmplVal *scp = (tup.t == TP_ITUPLE ? tup.tuple()->at(tps + r) : &((ctx->stackCur() - tup.v.i + tps + r)->val()));
                        if (!scp->isScalarIndex() || scp->isScalarInt() != intElem)
                            break;
                    }
                }

                return (r == tr || !fulltp ? r + 1 : 0);
            }
        }

        return 0;
    }


    /**
     * get set for operation "in" or "of"
     * @param ctx           execution context
     * @param set           return result set
     * @param src           source value (right operand in operation "in" or "of")
     * @param opIn          operation "in" (true) or "of" (false)
     * @param toTuple       allow tuple as result
     * @return              source value is appropriate
     */
    bool SetUtil::setForInOrOf(ExecContext *ctx, CmplVal& set, CmplVal& src, bool opIn, bool toTuple)
    {
        if (opIn) {
            if (src.isArray() || src.isList())
                return false;

            if (src.isSet() || (toTuple && src.isTuple()))
                set.copyFrom(src);
            else if (!SetUtil::convertToSetOrTuple(ctx, set, src, typeConversionValue, toTuple))
                return false;

            if (toTuple && set.isTuple()) {
                if (set.t == TP_TUPLE_EMPTY || set.t == TP_ITUPLE_NULL) {
                    set.set((set.t == TP_TUPLE_EMPTY ? TP_SET_ALL : TP_SET_NULL));
                }
                else if (Tuple::rank(set) == 1) {
                    CmplValAuto t;
                    t.copyFrom(Tuple::at(set, 0));
                    if (!SetUtil::convertToSetOrTuple(ctx, set, t, typeConversionValue, false))
                        return false;
                }
            }
        }
        else {
            if (src.isArray()) {
                //TODO: wenn TP_ARRAY_COMP, dann erst in TP_ARRAY wandeln
                getArrayValidSet(ctx, set, src.array());
            }
            else if (src.isList()) {
                //TODO: Set aus Liste ermitteln, z.B. durch Wandlung in Array
                set.set(TP_SET_EMPTY);//TODO
            }
            else {
                set.set(src ? TP_SET_NULL : TP_SET_EMPTY);
            }
        }

        return true;
    }


    /**
     * get set of valid elements for array
     * @param ctx           execution context
     * @param vs            return set
     * @param arr           source array
     */
    void SetUtil::getArrayValidSet(ExecContext *ctx, CmplVal& vs, CmplArray *arr)
    {
        LockGuard<mutex>(true, arr->accMtx());

        if (arr->getValidSet(vs))
            return;

        unsigned long cnt = 0;
        CmplVal *p = arr->at(0);

        for (unsigned long i = 0; i < arr->size(); i++, p++) {
            if (*p)
                cnt++;
        }

        if (cnt == arr->size()) {
            arr->setValidInfo(true);
            vs.copyFrom(arr->defset(), true, false);
        }
        else if (cnt == 0) {
            arr->setValidInfo(TP_SET_EMPTY);
            vs.set(TP_SET_EMPTY);
        }
        else {
            vs.set(TP_SET_EMPTY);
            CmplValAuto vsn;

            CmplArrayIterator iter(*arr, true);
            for (iter.begin(); iter; iter++) {
                addTupleToSet(ctx, vsn, vs, iter.curTuple(), false, false);
                vs.moveFrom(vsn, true);
            }

            if (canonicalSet(vsn, vs))
                vs.moveFrom(vsn, true);

            arr->setValidInfo(vs);
        }
    }


    /**
     * addition of two set or tuple values
     * @param ctx			execution context
     * @param res			return of result set
     * @param se			syntax element id of operation
     * @param a1			argument one value
     * @param a2			argument two value
     * @param ord			true: append argument two value user order / false: no user order for result set
     * @param tcr			transform result set to canonical set representation
     */
    void SetUtil::addSet(ExecContext *ctx, CmplVal *res, unsigned se, CmplVal *a1, CmplVal *a2, bool ord, bool tcr)
    {
        //TODO
        // aufzurufen fuer:
        //  Operator +: Aufrufen Summierung: ohne Reihenfolge, mit Kanonisieren; nur wenn beide Operanden Set/Tupel(auch Nicht-Indextupel!)/Intervall sind; erster Operand darf auch leer sein
        //  Funktion set(): Start mit leerem Set, für jeden Operanden Aufruf Summierung mit Reihenfolge und ohne Kanonisieren, aber für letztes Element mit Kanonisierung


        CmplValAuto a1s, a2s;
        bool cv1 = convertToSetOrTuple(ctx, a1s, *a1, typeConversionValue);
        bool cv2 = convertToSetOrTuple(ctx, a2s, *a2, typeConversionValue);

        if (!cv1 || !cv2) {
            ctx->valueError("illegal operand type for set union operation", (!cv1 ? a1s : a2s), se);
            res->set(TP_SET_EMPTY);
        }

        else if (a1s.t == TP_SET_ALL || a1s.t == TP_SET_ALL_MNF || a2s.t == TP_SET_ALL || a2s.t == TP_SET_ALL_MNF) {
            res->set(TP_SET_ALL);
        }

        else if (a1s.isSetInf() || a2s.isSetInf()) {
            //TODO: infinite sets
            ctx->valueError("set union operation not implemented for infinite sets", (a1s.isSetInf() ? a1s : a2s), se);
            res->set(TP_SET_ALL);
        }

        else {
            unsigned long a1c = (a1s.isSet() ? SetBase::cnt(a1s) : 1);
            unsigned long a2c = (a2s.isSet() ? SetBase::cnt(a2s) : 1);

            // swap arguments if no user order and the second argument has more elements
            if (!ord && (a2c > a1c || (a1s.isITuple() && a2s.isSet()))) {
                CmplVal swp;
                swp.moveFrom(a1s);
                a1s.moveFrom(a2s);
                a2s.moveFrom(swp);

                unsigned long c = a1c;
                a1c = a2c;
                a2c = c;
            }

            if (a1c == 0 || a2c == 0) {
                // handle with one empty argument
                if (a1c == 0 && a2c == 0)
                    res->set(TP_SET_EMPTY);
                else if ((a1c == 0 && a2s.isITuple()) || (a2c == 0 && a1s.isITuple()))
                    (a1c == 0 ? a2s : a1s).toSet(*res, typeConversionExact, ctx->modp());
                else if (a1c == 0 && !ord)
                    SetBase::stripOrder(a2s, *res);
                else
                    res->copyFrom((a1c == 0 ? a2s : a1s), true, false);
            }
            else {
                // if first argument is tuple then convert to set
                if (a1s.isITuple()) {
                    CmplVal ss;
                    a1s.toSet(ss, typeConversionExact, ctx->modp());
                    a1s.moveFrom(ss, true);
                }

                unsigned long num;
                if (a2s.isITuple()) {
                    if (tupleInSet(ctx, a1s, a2s, num))
                        res->copyFrom(a1s);
                    else
                        addTupleToSet(ctx, *res, a1s, a2s, ord, tcr);
                }
                else {
                    unsigned long added = 0;

                    SetIterator it(a2s, SetIterator::iteratorTupleVal, ord);
                    for (it.begin(); it; it++) {
                        if (!tupleInSet(ctx, a1s, *it, num)) {
                            if (added)
                                a1s.moveFrom(res, true);

                            addTupleToSet(ctx, *res, a1s, *it, ord, false);
                            added++;
                        }
                    }

                    if (!added) {
                        res->copyFrom(a1s);
                    }
                    else if (tcr) {
                        CmplVal rc;
                        if (canonicalSet(rc, *res, false))
                            res->moveFrom(rc, true);
                    }
                }
            }
        }
    }


    /**
     * add one tuple to a set
     * @param ctx			execution context
     * @param res			return of result set
     * @param seta			source set (must be a finite set)
     * @param tup			tuple to add to the set, must not be part of source set (must have type TP_ITUPLE*)
     * @param ord			true: append tuple in user order / false: no user order for result set
     * @param tcr			transform result set to canonical set representation
     * @param tps			start index of used tuple part
     * @return				index of inserted tuple in standard order
     */
    unsigned long SetUtil::addTupleToSet(ExecContext *ctx, CmplVal &res, const CmplVal &seta, const CmplVal &tup, bool ord, bool tcr, unsigned tps)
    {
        const CmplVal &set = SET_VAL_WO_ORDER(seta);
        unsigned long ins;
        unsigned long cs = SetBase::cnt(set);
        unsigned rs = SetBase::rank(set);
        unsigned rt = Tuple::rank(tup) - tps;

        if (cs == 0 || rs == 0) {
            if (rt == 0) {
                res.set(TP_SET_NULL);
            }
            else if (rt == 1) {
                const CmplVal *tpp = (tup.useInt() ? &tup : tup.tuple()->at(tps));
                res.set((tpp->t == TP_INT || tpp->t == TP_ITUPLE_1INT ? TP_SET_1INT : TP_SET_1STR), tpp->v.i);
            }
            else {
                res.set(TP_SET_REC_MULT, new SetRecMult(tup.tuple(), tps, true));
            }

            if (cs == 0) {
                ins = 0;
            }
            else {
                SetFinite *fs = new SetFinite(res, false, 0, true, 0, 0);
                res.dispose();
                res.set(TP_SET_FIN, fs);
                ins = 1;
            }
        }

        else if (rt == 0) {
            res.set(TP_SET_FIN, new SetFinite(set, true, 0, true, 0, 0));
            ins = 0;
        }

        else if (rs == 1 && rt == 1 && set.t != TP_SET_FIN) {
            CmplVal vt;
            if (tup.t == TP_ITUPLE)
                vt.copyFrom(tup.tuple()->at(tps), false, false);
            else
                vt.set((tup.t == TP_ITUPLE_1INT || tup.t == TP_INT ? TP_INT : TP_STR), tup.v.i);

            bool asSetAlg = (set.t != TP_SET_R1_ENUM && set.t != TP_SET_1STR && set.t != TP_STR && vt.t == TP_INT);
            if (asSetAlg) {
                // new set is algorithmic set of rank 1
                if (set.t == TP_SET_1INT || set.t == TP_ITUPLE_1INT || set.t == TP_INT) {
                    if (set.v.i < vt.v.i) {
                        constructAlg(res, set.v.i, vt.v.i, vt.v.i - set.v.i, false);
                        ins = 1;
                    }
                    else {
                        constructAlg(res, vt.v.i, set.v.i, set.v.i - vt.v.i, false);
                        ins = 0;
                    }
                }
                else {
                    intType ub = SetAlg::bound(set, true);
                    intType lb = SetAlg::bound(set, false);
                    intType sp = SetAlg::step(set);

                    if (vt.v.i == ub + sp) {
                        constructAlg(res, lb, vt.v.i, sp, false);
                        ins = cs;
                    }
                    else if (vt.v.i == lb - sp) {
                        constructAlg(res, vt.v.i, ub, sp, false);
                        ins = 0;
                    }
                    else {
                        asSetAlg = false;
                    }
                }
            }

            if (!asSetAlg) {
                // new set is enumeration set of rank 1
                intType *arr = new intType[cs + 1];
                unsigned long eci;

                if (set.t == TP_SET_R1_ENUM) {
                    SetEnum *se = set.setEnum();
                    if (vt.t == TP_INT)
                        ins = SetIterator::searchArrayPos<intType>(se->array(), se->cntInt(), vt.v.i);
                    else
                        ins = se->cntInt() + SetIterator::searchArrayPos<intType>(se->array() + se->cntInt(), cs - se->cntInt(), vt.v.i);

                    if (ins > 0)
                        memcpy(arr, se->array(), ins * sizeof(intType));
                    if (ins < cs)
                        memcpy(arr + ins + 1, se->array() + ins, (cs - ins) * sizeof(intType));

                    arr[ins] = vt.v.i;
                    eci = se->cntInt() + (vt.t == TP_INT ? 1 : 0);
                }
                else if (set.t == TP_SET_1STR) {
                    ins = (vt.v.i < set.v.i || vt.t == TP_INT ? 0 : 1);
                    arr[1-ins] = set.v.i;
                    arr[ins] = vt.v.i;
                    eci = (vt.t == TP_INT ? 1 : 0);
                }
                else {
                    intType ub = SetAlg::bound(set, true);
                    intType lb = SetAlg::bound(set, false);
                    intType sp = SetAlg::step(set);

                    if (vt.t != TP_INT || vt.v.i < lb || vt.v.i > ub) {
                        ins = (vt.t != TP_INT || vt.v.i > ub ? cs : 0);
                        arr[ins] = vt.v.i;

                        intType *a = (ins ? arr : (arr + 1));
                        for (intType v = lb; v <= ub; v += sp, a++)
                            *a = v;
                    }
                    else {
                        intType *a = arr;
                        unsigned long i = 0;
                        bool f = false;

                        for (intType v = lb; v <= ub; v += sp, a++, i++) {
                            if (!f && v > vt.v.i) {
                                *a = vt.v.i;
                                ins = i;
                                a++;
                            }

                            *a = v;
                        }
                    }
                    eci = cs + (vt.t == TP_INT ? 1 : 0);
                }

                res.set(TP_SET_R1_ENUM, new SetEnum(cs + 1, eci, arr));
            }
        }
        else {
            // search tuple prefix within base set
            const CmplVal& bsa = (set.t == TP_SET_FIN ? *(set.setFinite()->baseSet()) : set);
            const CmplVal& bs = SET_VAL_WO_ORDER(bsa);
            unsigned br = (set.t == TP_SET_FIN ? SetBase::rank(bs) : rs);

            unsigned long bnum = 0;
            unsigned bsr = (br < rt ? br : rt);
            unsigned r;

            for (r = 0; r < bsr; r++) {
                const CmplVal *ss = (bs.t == TP_SET_REC_MULT ? bs.setRecMult()->partSet(r, true) : &bs);
                unsigned long np;
                if (!tupleInSet(ctx, *ss, tup, np, tps + r, 1))
                    break;

                bnum = (r == 0 ? 0 : (bnum * SetBase::cnt(*ss))) + np;
            }

            // make new set as copy of set, as SetFinite with reduced base
            const CmplVal *tpa = (r == 0 ? (tup.t == TP_ITUPLE ? tup.tuple()->at(tps) : &tup) : NULL);
            SetFinite *fset = copyAsFinSet(ctx, set, (r==0 ? 1 : r), tpa, bnum);
            ins = fset->indSubStart(bnum);

            // append remaining tuple part to subset
            CmplVal *fss = fset->subSet(bnum);
            CmplVal nss;
            if (*fss) {
                ins += addTupleToSet(ctx, nss, *fss, tup, false, false, tps + (r==0 ? 1 : r));
            }
            else if (rt > (r==0 ? 1 : r)) {
                CmplVal ps(r == 0 ? TP_SET_EMPTY : TP_SET_NULL);
                ins += addTupleToSet(ctx, nss, ps, tup, false, false, tps + (r==0 ? 1 : r));
            }
            else {
                nss.unset();
            }

            fss->moveFrom(nss, true);
            fset->setCount();
            fset->setRank(fss);

            res.set(TP_SET_FIN, fset);
        }

        // handle user order
        if (ord && cs) {
            SetWithOrder *so = NULL;

            if (cs == 1 && ins == 0)
                so = new SetWithOrder(res, true);
            else if (cs > 1 && (SET_VAL_HAS_ORDER(seta) || ins < cs))
                so = new SetWithOrder(res, SetIterator::copyOrder(seta, 1, true, ins));

            if (so)
                res.dispSet(TP_SET_WITH_UO, so, true);
        }

        // simplify result set
        if (tcr) {
            CmplVal sres;
            if (canonicalSet(sres, res, !ord))
                res.moveFrom(sres, true);
            SetBase::setCanonical(res, true);
        }

        return ins;
    }


    /**
     * create a new SetFinite as copy of another set, with reduced base and/or added base element
     * @param ctx			execution context
     * @param set			source set (must be a finite set)
     * @param br			max base rank for result set
     * @param tpa			element to add to base set: must have rank 1 and must not be already part of the base set / NULL: don't add element
     * @param num			return of the index number of the added element (not changed if !tpa)
     * @return				result set
     */
    SetFinite *SetUtil::copyAsFinSet(ExecContext *ctx, const CmplVal &set, unsigned br, const CmplVal *tpa, unsigned long& num)
    {
        SetFinite *res;
        if (set.t == TP_SET_FIN && set.setFinite()->baseRank() == br && !tpa) {
            // result is direct copy of set
            res = new SetFinite(set, true, 0, false, 0, 0);
        }
        else {
            const CmplVal& bsp = (set.t == TP_SET_FIN ? set.setFinite()->baseSet(true) : set);
            unsigned bspr = SetBase::rank(bsp);

            // make new base set for SetFinite
            CmplVal bsn;
            if (br < bspr)
                bsp.setRecMult()->partSet(bsn, 0, br - 1, true);		// bsp must be TP_SET_REC_MULT because its rank bspr is greater than 1
            else
                bsn.copyFrom(bsp, true, false);

            if (bsn.isScalarIndex())
                bsn.t = TP_INDEX_VAL_SET(bsn.t);

            if (tpa) {
                CmplVal bsnn;
                CmplVal tpav(tpa->t == TP_INT ? TP_ITUPLE_1INT : (tpa->t == TP_STR ? TP_ITUPLE_1STR : tpa->t), tpa->v.i);
                num = addTupleToSet(ctx, bsnn, bsn, tpav, false, true);
                bsn.copyFrom(bsnn, true, true);
            }

            // create new SetFinite
            unsigned long bsnc = SetBase::cnt(bsn);
            res = new SetFinite(bsn, false, 0, (set.t == TP_SET_FIN && set.setFinite()->minRank() == 0), SetBase::minRank(set), SetBase::maxRank(set));
            bsn.dispose();

            // insert subsets
            CmplVal ssb;
            if (br < bspr)
                bsp.setRecMult()->partSet(ssb, br, bspr - 1, true);

            unsigned long si = 0;
            for (unsigned long i = 0; i < bsnc; i++) {
                if (i != num || !tpa) {
                    if (set.t == TP_SET_FIN) {
                        if (ssb) {
                            SetFinite *ss = new SetFinite(ssb, false, 0, false, 0, 0);
                            res->subSet(i)->set(TP_SET_FIN, ss);
                            for (unsigned long j = 0; j < ss->baseCnt(); j++)
                                ss->subSet(j)->copyFrom(set.setFinite()->subSet(si++), true, false);

                            ss->setCount();
                            ss->setRank();
                        }
                        else {
                            res->subSet(i)->copyFrom(set.setFinite()->subSet(si++), true, false);
                        }
                    }
                    else {
                        if (ssb) {
                            res->subSet(i)->copyFrom(ssb, true, false);
                        }
                    }
                }
            }

            ssb.dispose();
        }

        return res;
    }


    /**
     * convert given set to the aequivalent set in canonical representation
     * @param res			return of result set
     * @param seta			source set (must be a finite set)
     * @param woOrder       strip order from set
     * @return				true if result set is computed; false if source set is already in canonical representation
     */
    bool SetUtil::canonicalSet(CmplVal &res, CmplVal &seta, bool woOrder)
    {
        //TODO:
        //  wenn Set bereits kanonisch ist, aber noch nicht so gekennzeichnet, dann kann Kennzeichen hier wegen const nicht gesetzt werden

        // the canonical representation guarantees that two equal sets are represented in an identical format.
        // a set is in canonical representation when:
        //  - Subsets:
        //      if the sets contains subsets, then all its subsets are in canonical representation
        //      if the set type is TP_SET_FIN then its base set has the highest possible rank
        //  - Type: the set uses the first possible type in the following order:
        //      TP_SET_EMPTY, TP_SET_NULL
        //      TP_SET_1INT, TP_SET_1STR
        //      TP_SET_R1_0_UB/TP_SET_R1_0_UB_MNF, TP_SET_R1_1_UB/TP_SET_R1_1_UB_MNF
        //      TP_SET_R1_ALG
        //      TP_SET_R1_ENUM
        //      TP_SET_REC_MULT
        //      TP_SET_FIN
        //  - User order:
        //      the set or its subsets have no ineffective user orders (no user order equal standard order, and no user order hidden by another user order)
        //      if the user order can be expressed by user orders of subsets, then the set itself must not have an direct user order
        //      if a user order can be expressed as reverse standard order, then the user order must not use an order array

        if (SetBase::isCanonical(seta)) {
            if (!SET_VAL_HAS_ORDER(seta) || !woOrder)
                return false;
            else {
                SetBase::stripOrder(seta, res);
                return true;
            }
        }

        const CmplVal& set = SET_VAL_WO_ORDER(seta);
        bool woSubOrder = (woOrder || SET_VAL_HAS_ORDER(seta));
        bool dirOrder = false;
        res.unset();

        // handle empty set and null tuple set
        if (SetBase::cnt(set) == 0 || SetBase::maxRank(set) == 0) {
            tp_e t = (SetBase::cnt(set) == 0 ? TP_SET_EMPTY : (SetBase::markNF(set) ? TP_ITUPLE_NULL : TP_SET_NULL));
            if (seta.t == t)
                return false;

            res.set(t);
            return true;
        }

        // handle infinite sets
        if (!SetBase::finite(set)) {
            if (set.useValP()) {
                return set.setInfinite()->canonicalSet(res, woOrder);
            }
            else if ((set.t == TP_SET_R1_LB_INF || set.t == TP_SET_R1_LB_INF_MNF) && set.v.i == 0) {
                res.set((set.t == TP_SET_R1_LB_INF ? TP_SET_R1A_INT : TP_SET_R1A_INT_MNF));
                return true;
            }

            return false;
        }

        if (!SET_VAL_HAS_ORDER(seta) || !SetBase::isCanonical(set)) {
            // Subsets
            if (set.t == TP_SET_FIN || set.t == TP_SET_REC_MULT) {
                CmplVal nss, *nssp, *ssp;
                bool nssb;
                CmplVal *arr = NULL;
                map<SetBase *, CmplVal *> oi;   // for check and keep object identities

                if (set.t == TP_SET_REC_MULT) {
                    SetRecMult *srm = set.setRecMult();
                    for (unsigned r = 0; r < srm->maxRank(); r++) {
                        ssp = srm->partSet(r);
                        nssp = NULL;
                        nssb = false;

                        if (ssp->useValP() && oi.count(ssp->setBase()))
                            nssp = oi[ssp->setBase()];
                        else
                            nssb = canonicalSet(nss, *ssp, woSubOrder);

                        if (nssp || nssb) {
                            if (!arr) {
                                arr = new CmplVal[srm->maxRank()];
                                for (unsigned r2 = 0; r2 < r; r2++)
                                    arr[r2].copyFrom(srm->partSet(r2, woSubOrder), true, false);
                            }
                            if (nssp)
                                arr[r].copyFrom(nssp, true, false);
                            else
                                arr[r].moveFrom(nss);
                        }
                        else if (arr) {
                            arr[r].copyFrom(ssp, true, false);
                        }


                        if (ssp->useValP() && !oi.count(ssp->setBase()))
                            oi[ssp->setBase()] = (nssb ? (arr + r) : NULL);
                    }

                    if (arr) {
                        res.set(TP_SET_REC_MULT, new SetRecMult(srm->maxRank(), arr));
                        for (unsigned r = 0; r < srm->maxRank(); r++)
                            SetBase::setCanonical(arr[r], true);
                    }
                }

                else if (set.t == TP_SET_FIN) {
                    SetFinite *sf = set.setFinite();
                    SetFinite *sfn = NULL;

                    ssp = sf->baseSet();
                    if (canonicalSet(nss, *ssp, woSubOrder)) {
                        arr = new CmplVal[sf->baseCnt()];
                        sfn = new SetFinite(nss, arr, sf->minRank(), sf->maxRank());
                    }

                    for (unsigned long i = 0; i < sf->baseCnt(); i++) {
                        ssp = sf->subSet(i);
                        nssp = NULL;
                        nssb = false;

                        if (ssp->useValP() && oi.count(ssp->setBase()))
                            nssp = oi[ssp->setBase()];
                        else
                            nssb = canonicalSet(nss, *ssp, woSubOrder);

                        if (nssp || nssb) {
                            if (!arr) {
                                arr = new CmplVal[sf->baseCnt()];
                                sfn = new SetFinite(*(sf->baseSet()), arr, sf->minRank(), sf->maxRank());
                                for (unsigned long i2 = 0; i2 < i; i2++)
                                    arr[i2].copyFrom(sf->subSet(i2, woSubOrder), true, false);
                            }
                            if (nssp)
                                arr[i].copyFrom(nssp, true, false);
                            else
                                arr[i].moveFrom(nss);
                        }
                        else if (arr) {
                            arr[i].copyFrom(ssp, true, false);
                        }

                        if (ssp->useValP() && !oi.count(ssp->setBase()))
                            oi[ssp->setBase()] = (nssb ? (arr + i) : NULL);
                    }

                    if (sfn) {
                        sfn->setCount();
                        res.set(TP_SET_FIN, sfn);
                        SetBase::setCanonical(*(sfn->baseSet()), true);
                        for (unsigned long i = 0; i < sf->baseCnt(); i++)
                            SetBase::setCanonical(arr[i], true);

                        checkBaseSplitFinSet(res, false);
                    }
                }
            }

            // type of set
            if ((res ? res.t : set.t) == TP_SET_FIN) {
                SetFinite *sf = (res ? res.setFinite() : set.setFinite());

                if (sf->baseCnt() == 0) {
                    if (sf->minRank() == 0)
                        res.dispSet(TP_SET_NULL);
                    else
                        res.dispSet(TP_SET_EMPTY);
                }
                else if (sf->baseCnt() == 1 && sf->subSet(0)->t != TP_SET_FIN) {
                    CmplVal *ss = sf->subSet(0);
                    SetRecMult *sb = ((ss->t != TP_EMPTY && ss->t != TP_SET_NULL) ? new SetRecMult(*(sf->baseSet()), *ss, true) : NULL);

                    if (sf->minRank() > 0) {
                        if (sb)
                            res.dispSet(TP_SET_REC_MULT, sb);
                        else
                            res.copyFrom(sf->baseSet(), true, true);
                    }
                    else if (sb || ss->t == TP_SET_NULL) {
                        SetFinite *nsf;
                        if (sb) {
                            CmplVal v(TP_SET_REC_MULT, sb, false);
                            nsf = new SetFinite(v, true, 0, true, 0, 0);
                        }
                        else {
                            nsf = new SetFinite(*(sf->baseSet()), true, 0, true, 0, 0);
                        }
                        res.dispSet(TP_SET_FIN, nsf);
                    }
                }
                else {
                    CmplVal *fss = sf->subSet(0);
                    unsigned eqr = (*fss ? (fss->t == TP_SET_FIN ? (fss->setFinite()->minRank() ? fss->setFinite()->baseRank() : 0) : SetBase::minRank(*fss)) : 0);
                    bool subOrd = SetBase::hasUserOrder(*fss) || SetBase::hasUserOrder(*(sf->baseSet()));
                    std::set<SetBase *> oi;
                    if (fss->useValP())
                        oi.insert(fss->setBase());

                    for (unsigned long i = 1; eqr && i < sf->baseCnt(); i++) {
                        CmplVal *ss = sf->subSet(i);
                        if (ss->useValP()) {
                            if (oi.count(ss->setBase()))
                                continue;
                            else
                                oi.insert(ss->setBase());
                        }

                        if (!subOrd && SetBase::hasUserOrder(*ss))
                            subOrd = true;

                        compareEqRank(fss, ss, eqr, false, false);
                    }

                    if (eqr) {
                        // make new finite set with greater base set
                        dirOrder = subOrd && !woOrder;

                        map<unsigned long, CmplVal> subs;
                        for (unsigned long i = 0; i < sf->baseCnt(); i++) {
                            CmplVal& v = SET_VAL_WO_ORDER(*(sf->subSet(i)));
                            subs[i+1].copyFrom(v, false, false);
                        }

                        CmplValAuto bs;
                        bs.copyFrom(sf->baseSet(), true, false);
                        CmplVal *arr = arrayForFinSet(bs, false, eqr, false, subs, 0);

                        // if all subsets are empty and null tuple not contained, then the base set itself is the result
                        if (sf->minRank() > 0 && (arr->t == TP_EMPTY || arr->t == TP_NULL)) {
                            unsigned long i, bsc = SetBase::cnt(bs);
                            CmplVal *ap = arr + 1;
                            for (i = 1; i < bsc; i++, ap++) {
                                if (ap->t != TP_EMPTY && ap->t != TP_NULL)
                                    break;
                            }

                            if (i == bsc) {
                                delete arr;
                                arr = NULL;
                            }

                        }

                        if (!arr) {
                            res.copyFrom(bs, true, true);
                        }
                        else {
                            SetFinite *nsf = new SetFinite(bs, arr, sf->minRank(), sf->maxRank());
                            res.dispSet(TP_SET_FIN, nsf);
                        }
                    }
                }
            }

            if ((res ? res.t : set.t) == TP_SET_REC_MULT) {
                SetRecMult *srm = (res ? res.setRecMult() : set.setRecMult());
                if (srm->maxRank() <= 1) {
                    if (srm->maxRank() == 1)
                        res.copyFrom(srm->partSet(0), true, true);
                    else
                        res.dispSet(TP_SET_NULL);
                }
            }

            if ((res ? res.t : set.t) == TP_SET_R1_ENUM) {
                SetEnum *se = (res ? res.setEnum() : set.setEnum());
                if (se->cnt() <= 1 || se->cnt() == se->cntInt()) {
                    if (se->cnt() == 0) {
                        res.dispSet(TP_SET_EMPTY);
                    }
                    else if (se->cnt() == 1) {
                        res.dispSet((se->cntInt() ? TP_SET_1INT : TP_SET_1STR), *(se->array()));
                    }
                    else {
                        intType st = *(se->array());
                        intType sp = se->array()[1] - st;
                        unsigned long i;

                        for (i = 2; i < se->cnt(); i++) {
                            if (se->array()[i] != st + (intType)i * sp)
                                break;
                        }

                        if (i == se->cnt()) {
                            if (sp == 1 && (st == 0 || st == 1))
                                res.dispSet((st ? TP_SET_R1_1_UB : TP_SET_R1_0_UB), (intType) se->cnt() + st - 1);
                            else
                                res.dispSet(TP_SET_R1_ALG, new SetAlg(st, sp, se->cnt()));
                        }
                    }
                }
            }

            if ((res ? res.t : set.t) == TP_SET_R1_ALG) {
                SetAlg *sa = (res ? res.setAlg() : set.setAlg());
                if (sa->cnt() <= 1 || (sa->step() == 1 && (sa->start() == 0 || sa->start() == 1))) {
                    if (sa->cnt() == 0) {
                        res.dispSet(TP_SET_EMPTY);
                    }
                    else if (sa->cnt() == 1) {
                        res.dispSet(TP_SET_1INT, sa->start());
                    }
                    else {
                        intType st = sa->start();
                        res.dispSet((st ? TP_SET_R1_1_UB : TP_SET_R1_0_UB), (intType) sa->cnt() + st - 1);
                    }
                }
            }

            if (!TP_USE_VALP((res ? res.t : set.t))) {
                unsigned long cnt = SetBase::cnt(set);
                if (cnt <= 1) {
                    tp_e tp = (res ? res.t : set.t);
                    if (cnt == 0 && tp != TP_SET_EMPTY) {
                        res.dispSet(TP_SET_EMPTY);
                    }
                    else if (cnt == 1 && tp != TP_SET_1INT && tp != TP_SET_1STR) {
                        if (tp == TP_SET_R1_1_UB || tp == TP_SET_R1_1_UB_MNF)
                            res.dispSet(TP_SET_1INT, (intType)1);
                        else if (tp == TP_SET_R1_0_UB || tp == TP_SET_R1_0_UB_MNF)
                            res.dispSet(TP_SET_1INT, (intType)0);
                    }
                }
            }

            // set marked as non-free
            if (res && SetBase::markNF(set)) {
                SetBase::setMarkNF(res, true);
            }
        }

        // user order
        if (SET_VAL_HAS_ORDER(seta) || dirOrder) {
            unsigned long cnt = SetBase::cnt(set);
            if (!woOrder && cnt <= 1)
                woOrder = true;

            if (woOrder) {
                // return set without user order
                const CmplVal& so = (res ? res : set);
                if (so.useValP() && so.setBase()->hasUserOrder()) {
                    CmplVal nres;
                    so.setBase()->stripOrder(nres);
                    res.moveFrom(nres, true);
                }
                else if (!res) {
                    res.copyFrom(set, true, false);
                }
            }

            else if (res || SET_VAL_HAS_ORDER(seta)) {
                unsigned long *ord = NULL;
                if (dirOrder && res && !SET_VAL_HAS_ORDER(seta))
                    ord = SetIterator::copyOrder(set);

                CmplVal ores;
                if (canonicalOrder(ores, res, (SET_VAL_HAS_ORDER(seta) ? seta.setWithOrder() : NULL), ord))
                    res.moveFrom(ores, true);
            }
        }

        // result set is canonical
        CmplVal& rset = res ?: seta;
        if (!SetBase::isCanonical(rset))
            SetBase::setCanonical(rset, true);

        CmplVal &rseto = SET_VAL_WO_ORDER(rset);
        if (rseto.t == TP_SET_FIN && rseto.setFinite()->baseSplit() == 0)
            checkBaseSplitFinSet(rseto, false);

        return (bool)res;
    }


    /**
     * check if split of base set of SetFinite is possible, set <code>_baseSplit</code> within SetFinite </code>
     * @param set			source set (must be TP_SET_FIN)
     * @param uo            consider user order
     * @param modarr        set also <code>_modArray</code> within SetFinite
     * @param force         check split also if already known
     * @return              true if split is possible
     */
    bool SetUtil::checkBaseSplitFinSet(CmplVal &set, bool uo, bool modarr, bool force)
    {
        CmplVal &es = SET_VAL_WO_ORDER(set);
        if (es.t != TP_SET_FIN)
            return false;

        SetFinite *sf = es.setFinite();
        if (sf->baseRank() > 1 && sf->minRank() > 1 && (force || sf->baseSplit() == 0)) {
            // base set can only be SetRecMult
            SetRecMult *srm = sf->baseSet(true).setRecMult();
            unsigned long bc = sf->baseCnt();

            // all subsets must be canonical
            if (!sf->isCanonical()) {
                CmplValAuto nss;
                for (unsigned long i = 0; i < bc; i++) {
                    CmplVal *ss = sf->subSet(i);
                    if (canonicalSet(nss, *ss, false))
                        ss->moveFrom(nss, true);
                }
            }

            // check subsets for equality neccessary for a split before rank r
            unsigned long md = 1;
            unsigned r = sf->baseRank() - 1;
            for (; r > 0; r--) {
                md *= srm->partCnt(r);

                bool ne = false;
                for (unsigned long i = 0; !ne && i < md; i++) {
                    CmplVal *s1 = sf->subSet(i);
                    for (unsigned long j = i + md; j < bc; j += md) {
                        CmplVal *s2 = sf->subSet(j);
                        if (!compareEq(s1, s2, true)) {
                            ne = true;
                            break;
                        }
                    }
                }

                if (!ne)
                    break;
            }

            sf->setBaseSplit(sf->baseRank() - r, modarr);
        }

        unsigned splt = sf->baseSplit(uo);
        return (splt && splt < sf->baseRank());
    }


    /**
     * set user order in canonical representation for a set
     * @param res           result set
     * @param set           source set (without user order), if empty then use set from <code>setOrder</code>
     * @param setOrder      original user order for the set / NULL: no original user order exists
     * @param orderArr      if not NULL then use this user order instead of the one from <code>setOrder</code> (if given then <code>set</code> must not be empty) (if given then this function will consume or delete this array)
     * @return              true if new result set / false if source set is already equal to result
     */
    bool SetUtil::canonicalOrder(CmplVal& res, const CmplVal &set, const SetWithOrder *setOrder, unsigned long *orderArr)
    {
        res.unset();
        bool hasOrderArr = (orderArr != NULL);
        const CmplVal& st = (set || !setOrder ? set : setOrder->set());
        unsigned long cnt = (st ? SetBase::cnt(st) : 0);

        // if set hasn't at least two elements then no user order needed
        if (cnt <= 1) {
            if (hasOrderArr)
                delete orderArr;

            if (!set && st && SetBase::hasUserOrder(st))
                SetBase::stripOrder(st, res);

            return (bool)res;
        }

        // if no orderArr is given then use order from setOrder
        if (!hasOrderArr) {
            if (!setOrder) {
                return false;
            }
            else if (!setOrder->hasUserOrder()) {
                if (!set && SetBase::hasUserOrder(st))
                    SetBase::stripOrder(st, res);
                return (bool)res;
            }
            else if (setOrder->negOrder()) {
                if (set || (st.t != TP_SET_FIN && st.t != TP_SET_REC_MULT) || (st.t == TP_SET_FIN && st.setFinite()->minRank() == 0))
                    setOrderNegRec(res, st);
                return (bool)res;
            }
            else {
                orderArr = setOrder->order();
            }
        }

        // check if user order is equal to standard order
        unsigned long i;
        unsigned long *ap;
        for (i = 0, ap = orderArr; i < cnt; i++, ap++) {
            if (*ap != i)
                break;
        }
        if (i == cnt) {
            if (hasOrderArr)
                delete orderArr;

            if (!set && SetBase::hasUserOrder(st))
                SetBase::stripOrder(st, res);

            return (bool)res;
        }

        // check if user order is equal to reverse standard order
        for (i = 0, ap = orderArr + cnt - 1; i < cnt; i++, ap--) {
            if (*ap != i)
                break;
        }
        if (i == cnt) {
            if (hasOrderArr)
                delete orderArr;

            setOrderNegRec(res, st);
            return true;
        }

        // check for moving user order to subsets
        if (st.t == TP_SET_REC_MULT || (st.t == TP_SET_FIN && (st.setFinite()->minRank() || orderArr[0] == 0))) {
            setOrderArrRec(res, st, orderArr);
            if (res || !set) {
                if (hasOrderArr)
                    delete orderArr;
                return (bool)res;
            }
        }

        if (set) {
            // new direct user order
            unsigned long *uo;
            if (hasOrderArr) {
                uo = orderArr;
            }
            else {
                uo = new unsigned long[cnt];
                memcpy(uo, orderArr, sizeof(unsigned long) * cnt);
            }

            SetWithOrder *so = new SetWithOrder(set, uo);
            res.set(TP_SET_WITH_UO, so, true);
            return true;
        }
        else {
            // user order for source set remains unchanged
            if (hasOrderArr)
                delete orderArr;

            return false;
        }
    }


    /**
     * set order for set, if necessary then recursive for subsets
     * @param res           result set
     * @param set           source set (without user order)
     * @param orderArr      array with user order
     * @return              true if new result set / false if source set is already equal to result
     */
    bool SetUtil::setOrderArrRec(CmplVal& res, const CmplVal &set, const unsigned long *orderArr)
    {
        bool suc = false;

        if (set.t == TP_SET_REC_MULT) {
            SetRecMult *srm = set.setRecMult();
            unsigned r = srm->maxRank();

            unsigned long **apuo = new unsigned long *[r];
            for (unsigned i = 0; i < r; i++)
                apuo[i] = NULL;

            unsigned long cnt = srm->cnt();
            unsigned long stp;

            suc = true;
            for (unsigned i = 0; i < r - 1; i++) {
                unsigned long pc = srm->partCnt(i);
                stp = cnt / pc;

                // the user order of the first stp elements must be repeated in the all following blocks of stp elements, compared with modulo stp
                for (unsigned long j = stp; j < cnt; j++) {
                    if (orderArr[j] % stp != orderArr[j % stp] % stp) {
                        suc = false;
                        break;
                    }
                }

                if (!suc)
                    break;

                apuo[i] = new unsigned long[pc];
                unsigned long k = 0;
                for (unsigned long j = 0; j < pc; j++, k += stp)
                    apuo[i][j] = (orderArr[k] % cnt) / stp;

                // for the next part set only the first stp elements of user order must be checked
                cnt = stp;
            }

            if (suc) {
                // get user order for the last part set
                unsigned i = r - 1;
                apuo[i] = new unsigned long[cnt];
                for (unsigned long j = 0; j < cnt; j++)
                    apuo[i][j] = orderArr[j] % cnt;

                // create new set with transferred user order
                CmplVal *arr = new CmplVal[r];
                for (unsigned i = 0; i < r; i++)
                {
                    const CmplVal *ps = srm->partSet(i, true);
                    if (ps->isSet() && srm->partCnt(i) > 1) {
                        if (!canonicalOrder(arr[i], *ps, NULL, apuo[i]))
                            arr[i].copyFrom(ps, true, false);
                    }
                    else {
                        arr[i].copyFrom(ps, true, false);
                        if (apuo[i])
                            delete apuo[i];
                    }
                }

                res.set(TP_SET_REC_MULT, new SetRecMult(r, arr));
            }
            else {
                for (unsigned i = 0; i < r; i++) {
                    if (apuo[i])
                        delete apuo[i];
                }
            }

            delete apuo;
        }

        else if (set.t == TP_SET_FIN && (set.setFinite()->minRank() || orderArr[0] == 0)) {
            // user order can be transferred to subsets, if all elements of every single subset follow each other directly in the user order
            SetFinite *sf = set.setFinite();
            unsigned long bc = sf->baseCnt();
            unsigned long *ind = sf->index();
            unsigned long *asi = new unsigned long[bc];
            unsigned long *abuo = new unsigned long[bc];
            unsigned mr = (sf->minRank() ? 0 : 1);
            unsigned long bi = 0;

            suc = true;
            for (unsigned long i = mr; suc && bi < bc; bi++) {
                unsigned long buo = SetIterator::searchArrayPos(ind, bc, orderArr[i] - mr);
                unsigned long bf = (buo ? ind[buo-1] : 0) + mr;

                abuo[bi] = buo;
                asi[buo] = i++;

                // check if following elements belong to the same subset
                CmplVal *ss = sf->subSet(buo);
                if (*ss && ss->isSet()) {
                    unsigned long bt = ind[buo] + mr;

                    for (unsigned long j = bf + 1; j < bt; j++, i++) {
                        if (orderArr[i] < bf || orderArr[i] >= bt) {
                            suc = false;
                            break;
                        }
                    }
                }
            }

            if (suc) {
                // create new set with user order transferred to subsets
                const CmplVal& bs = sf->baseSet(true);
                CmplValAuto nbs;
                if (!canonicalOrder(nbs, bs, NULL, abuo))
                    nbs.copyFrom(bs, true, false);

                CmplVal *arr = new CmplVal[bc];
                for (unsigned long i = 0; i < bc; i++) {
                    const CmplVal& ss = sf->subSet(i, true);
                    unsigned long sc;

                    if (ss && ss.isSet() && (sc = SetBase::cnt(ss)) > 1) {
                        unsigned long *asuo = new unsigned long[sc];
                        unsigned long bf = (i ? ind[i-1] : 0) + mr;
                        unsigned long k = asi[i];

                        for (unsigned long j = 0; j < sc; j++, k++)
                            asuo[j] = orderArr[k] - bf;

                        if (!canonicalOrder(arr[i], ss, NULL, asuo))
                            arr[i].copyFrom(ss, true, false);
                    }
                    else {
                        arr[i].copyFrom(ss, true, false);
                    }
                }

                res.set(TP_SET_FIN, new SetFinite(nbs, arr, sf->minRank(), sf->maxRank()));
            }

            delete asi;
            if (!suc)
                delete abuo;
        }

        return suc;
    }


    /**
     * set reverse order for set, if necessary then recursive for subsets
     * @param res           result set
     * @param seta          source set
     */
    void SetUtil::setOrderNegRec(CmplVal& res, const CmplVal &seta)
    {
        const CmplVal& set = SET_VAL_WO_ORDER(seta);

        if (SetBase::cnt(set) <= 1) {
            res.copyFrom(set, true, false);
        }
        else if (set.t == TP_SET_REC_MULT) {
            SetRecMult *srm = set.setRecMult();
            unsigned r = srm->maxRank();
            CmplVal *arr = new CmplVal[r];

            for (unsigned i = 0; i < r; i++)
                setOrderNegRec(arr[i], srm->partSet(i));

            res.set(TP_SET_REC_MULT, new SetRecMult(r, arr));
        }
        else if (set.t == TP_SET_FIN && set.setFinite()->minRank()) {
            SetFinite *sf = set.setFinite();
            unsigned long bc = sf->baseCnt();
            CmplVal *arr = new CmplVal[bc];

            CmplValAuto bs;
            setOrderNegRec(bs, *(sf->baseSet()));

            for (unsigned long i = 0; i < bc; i++)
                setOrderNegRec(arr[i], sf->subSet(i));

            res.set(TP_SET_FIN, new SetFinite(bs, arr, sf->minRank(), sf->maxRank()));
        }
        else if (SET_VAL_HAS_ORDER(seta) && seta.setWithOrder()->negOrder()) {
            res.copyFrom(seta, true, false);
        }
        else {
            res.set(TP_SET_WITH_UO, new SetWithOrder(set, true));
        }
    }


    /**
     * try to convert a value to a set or an index tuple
     * @param ctx			execution context
     * @param res			return of result value (with type TP_SET_* or TP_ITUPLE*)
     * @param src           source value
     * @param tcl           conversion level
     * @param toTuple       if false then convert only to set but not to tuple
     * @return              true if conversion is successful
     */
    bool SetUtil::convertToSetOrTuple(ExecContext *ctx, CmplVal& res, CmplVal& src, TypeConversionLevel tcl, bool toTuple)
    {
        if (src.isSet() || (toTuple && src.isITuple())) {
            res.copyFrom(src, true, false);
            return true;
        }

        else if (src.t == TP_EMPTY || src.t == TP_NULL || src.t == TP_BLANK) {
            if (tcl < typeConversionExact)
                return false;

            res.set((src.t == TP_EMPTY ? TP_SET_EMPTY : (src.t == TP_NULL ? TP_SET_NULL : TP_SET_ALL)));
            return true;
        }

        else if (src.isScalarValue()) {
            CmplValAuto rr;
            if (!src.toIndexVal(rr, tcl, ctx->modp()))
                return false;

            if (rr.useInt()) {
                if (toTuple)
                    res.set(TP_INDEX_VAL_TUPLE(rr.t), rr.v.i);
                else
                    res.set(TP_INDEX_VAL_SET(rr.t), rr.v.i);
            }
            else if (rr.isTuple() && !toTuple) {
                CmplVal es(TP_SET_EMPTY);
                addTupleToSet(ctx, res, es, rr, false, true);
            }
            else {
                res.moveFrom(rr, false);
            }

            return true;
        }

        else if (src.isInterval()) {
            constructFromInterval(ctx, res, src);
            return true;
        }

        else if (src.isTuple()) {
            if (src.t == TP_TUPLE_EMPTY) {
                res.set(TP_SET_ALL);
                return true;
            }
            else {
                if (src.isITuple())
                    res.copyFrom(src, true, false);
                else if (!TupleUtil::toIndexTuple(ctx, res, src.tuple()))
                    return false;

                if (res.isSet() || (res.isITuple() && toTuple)) {
                    return true;
                }
                else if (res.isITuple()) {
                    CmplVal t, es(TP_SET_EMPTY);
                    addTupleToSet(ctx, t, es, res, false, true);
                    res.moveFrom(t, true);
                    return true;
                }
            }
        }

        return false;
    }


    /**
     * return new user order array for merging user orders of the source sets of a tuple
     * @param ctx			execution context
     * @param src           vector of source values (must be finite sets, intervals, or single index values)
     * @return              new array with resulting user order
     */
    unsigned long* SetUtil::tupleMergeOrder(ExecContext *ctx, const vector<const CmplVal *>& src)
    {
        // fill data for tupleMergeOrderRek()
        unsigned c = src.size();
        vector<unsigned long> sCnt(c);
        vector<unsigned long> sMultCnt(c);
        vector<unsigned long*> sOrd(c);

        unsigned i;
        for (i = 0; i < c; i++) {
            const CmplVal *s = src[i];

            if (s->isSet()) {
                sCnt[i] = SetBase::cnt(s);
                sOrd[i] = SetIterator::copyOrder(s);
            }
            else if (s->isInterval()) {
                CmplValAuto t;
                SetUtil::constructFromInterval(ctx, t, *s);
                sCnt[i] = SetBase::cnt(t);
                sOrd[i] = SetIterator::copyOrder(t);
            }
            else {
                sCnt[i] = 1;
                sOrd[i] = new unsigned long[1];
                *(sOrd[i]) = 0;
            }
        }

        i = c - 1;
        sMultCnt[i] = 0;
        unsigned long cm = sCnt[i];

        while (i > 0) {
            sMultCnt[--i] = cm;
            cm *= sCnt[i];
        }

        // allocate and fill result array
        unsigned long *res = new unsigned long[cm];

        unsigned long *dst = res;
        tupleMergeOrderRek(dst, 0, sCnt, sMultCnt, sOrd, 0);

        // release temporary arrays
        for (i = 0; i < c; i++)
            delete[] sOrd[i];

        return res;
    }

    /**
     * recursive fill array for merged user order
     * @param dst           next position in destination array, incremented by the function
     * @param ind           current index in argument arrays
     * @param sCnt          array with count of elements per source set
     * @param sMultCnt      array with count of elements multiplied over all source sets with above <code>ind</code>, or 0 if no further recursion
     * @param sOrd          array of user order arrays per source set
     * @param ofs           current offset for destination user order
     */
    void SetUtil::tupleMergeOrderRek(unsigned long* &dst, unsigned ind, vector<unsigned long>& sCnt, vector<unsigned long>& sMultCnt, vector<unsigned long *>& sOrd, unsigned long ofs)
    {
        unsigned long c = sCnt[ind];
        unsigned long cm = sMultCnt[ind];
        unsigned long *o = sOrd[ind];

        if (cm == 0) {
            for (unsigned long i = 0; i < c; i++, o++)
                *(dst++) = ofs + *o;
        }
        else {
            for (unsigned long i = 0; i < c; i++, o++)
                tupleMergeOrderRek(dst, ind + 1, sCnt, sMultCnt, sOrd, *o * cm + ofs);
        }
    }


    /**
     * convert tuple tpl to set
     * @param ctx           execution context
     * @param res           return of result value
     * @param tpl           tuple (all parts must be scalar index values or non-empty sets or intervals, and not be infinite)
     * @param tcr			transform result set to canonical set representation
     * @param uo            keep user order
     */
    void SetUtil::tupleToSetFin(ExecContext *ctx, CmplVal& res, Tuple *tpl, bool tcr, bool uo)
    {
        unsigned tr = tpl->rank();
        CmplVal *e = tpl->at(0);

        if (tr == 0) {
            res.set(TP_SET_NULL);
        }

        else if (tr == 1) {
            if (e->isScalarIndex())
                res.set((e->t == TP_STR ? TP_SET_1STR : TP_SET_1INT), e->v.i);
            else if (e->isSet())
                res.copyFrom(e, true, false);
            else if (e->isInterval())
                SetUtil::constructFromInterval(ctx, res, *e);
        }

        else {
            // check for mode of result set
            tp_e mode = TP_SET_REC_MULT;
            unsigned resRank = 0;
            unsigned baseRank = 0;
            unsigned rmRank = 0;
            unsigned baseInd = tr;
            bool baseSplit = false;
            bool uoRecMult = false;
            bool uoSetFin = false;
            bool uoAllRev = true;

            for (unsigned r = 0; r < tr; r++, e++) {
                CmplVal *ewo = &(SET_VAL_WO_ORDER(*e));
                SetWithOrder *so = (uo && e->t == TP_SET_WITH_UO ? e->setWithOrder() : NULL);

                if (ewo->isSetInf() || (ewo->isInterval() && Interval::isSetInf(*ewo))) {
                    mode = TP_SET_INF_TPL;
                }

                else if (ewo->t == TP_SET_EMPTY || (ewo->isInterval() && Interval::isEmpty(*ewo))) {
                    mode = TP_SET_EMPTY;
                    break;
                }

                else if (mode == TP_SET_REC_MULT && ewo->t == TP_SET_FIN) {
                    mode = (ewo->setFinite()->containNullSub() ? TP_SET_NULL : TP_SET_FIN);
                    baseInd = r;
                    baseSplit = (mode == TP_SET_FIN);
                    rmRank = resRank;
                    baseRank = resRank + (baseSplit ? ewo->setFinite()->baseRank() : 0);
                    resRank += ewo->setFinite()->maxRank();

                    if (uo) {
                        if (so)
                            uoSetFin = true;

                        CmplVal *bss = ewo->setFinite()->baseSet();
                        if (bss->t == TP_SET_WITH_UO && ewo->setFinite()->baseRank() > 1)
                            uoRecMult = true;

                        if (uoAllRev && !SetBase::negOrder(*bss, true))
                            uoAllRev = false;
                    }
                }

                else if (ewo->isSet()) {
                    unsigned rr = SetBase::maxRank(*ewo);
                    resRank += rr;

                    if (uo) {
                        if (so && rr > 1)
                            uoRecMult = true;

                        if (uoAllRev && !SetBase::negOrder(*e, true))
                            uoAllRev = false;
                    }
                }

                else if (!ewo->rank0()) {
                    resRank++;

                    if (uo && uoAllRev && ewo->isInterval())
                        uoAllRev = false;
                }
            }

            // construct result set
            if (mode == TP_SET_REC_MULT || mode == TP_SET_FIN) {
                unsigned rr = (mode == TP_SET_FIN ? baseRank : resRank);
                if (rr == 0) {
                    res.set(TP_SET_NULL);
                    return;
                }

                // construct result set (if mode TP_SET_REC_MULT) or base set for result set as TP_SET_FIN
                CmplValAuto nsv;
                e = tpl->at(0);
                unsigned i = 0;

                bool uoSep = uo && !uoRecMult && !uoSetFin;
                bool uoNsv = uo && !uoAllRev && (uoRecMult || uoSetFin);
                vector<const CmplVal *> uoSrc;

                while (e->t == TP_ITUPLE_NULL) {
                    e++;
                    i++;
                }

                if (baseInd == i + 1 && !baseSplit) {
                    nsv.copyFrom(e, true, false);
                }
                else if (baseInd == i && baseSplit) {
                    nsv.copyFrom(SET_VAL_WO_ORDER(*e).setFinite()->baseSet(), true, false);
                }
                else {
                    CmplVal *arr = new CmplVal[rr];
                    nsv.set(TP_SET_REC_MULT, new SetRecMult(rr, arr));

                    unsigned di = 0;
                    for (; i < baseInd + (baseSplit ? 1 : 0); i++, e++, di++) {
                        CmplVal *ewo = &(SET_VAL_WO_ORDER(*e));

                        if (i == baseInd) {
                            e = ewo->setFinite()->baseSet();
                            ewo = &(SET_VAL_WO_ORDER(*e));
                        }

                        if (ewo->isScalarIndex() || (ewo->isSet() && SetBase::finiteRank1(*ewo))) {
                            arr[di].copyFrom((uoSep ? e : ewo), true, false);
                            if (uoNsv)
                                uoSrc.push_back(e);
                        }
                        else if (ewo->isInterval()) {
                            SetUtil::constructFromInterval(ctx, arr[di], *ewo);
                            if (uoNsv)
                                uoSrc.push_back(arr+di);
                        }
                        else if (ewo->t == TP_SET_REC_MULT) {
                            SetRecMult *srm = ewo->setRecMult();
                            for (unsigned j = 0; j < srm->maxRank(); j++, di++) {
                                CmplVal *srmps = srm->partSet(j);
                                arr[di].copyFrom((uoSep ? srmps : &(SET_VAL_WO_ORDER(*srmps))), true, false);
                            }
                            di--;
                            if (uoNsv)
                                uoSrc.push_back(e);
                        }
                    }

                    nsv.setRecMult()->setCnt(false);

                    if (uo && uoRecMult && !uoSetFin) {
                        // add user order to result set
                        SetWithOrder *so = new SetWithOrder(nsv);
                        CmplValAuto sov(TP_SET_WITH_UO, so);

                        if (uoAllRev)
                            so->setNegOrder(true);
                        else
                            so->setOrder(tupleMergeOrder(ctx, uoSrc));

                        nsv.moveFrom(sov);
                    }
                }

                if (mode == TP_SET_REC_MULT && rr == resRank) {
                    res.moveFrom(nsv);
                }
                else {
                    res.set(TP_SET_FIN, new SetFinite(nsv, false, (rmRank ? baseRank - rmRank : baseRank), false, 0, 0));

                    SetFinite *sfdst = res.setFinite();
                    SetFinite *sfsrc = SET_VAL_WO_ORDER(*(tpl->at(baseInd))).setFinite();
                    unsigned long bcdst = sfdst->baseCnt(true);
                    unsigned long bcsrc = sfsrc->baseCnt();

                    if (baseInd == tr - 1) {
                        // use subsets from source SetFinite at baseInd for resulting SetFinite
                        for (unsigned long i = 0; i < bcdst; i++)
                            sfdst->subSet(i)->copyFrom(sfsrc->subSet(i % bcsrc));
                    }
                    else {
                        // make subsets by recursive call
                        unsigned long i;
                        Tuple tplSub(tr - baseInd);

                        for (unsigned r = baseInd + 1; r < tr; r++)
                            tplSub.at(r - baseInd)->copyFrom(tpl->at(r));

                        for (i = 0; i < bcsrc; i++) {
                            CmplVal *sss = sfsrc->subSet(i);
                            if (*sss)
                                tplSub.at(0)->copyFrom(sss);
                            else
                                tplSub.at(0)->dispSet(TP_ITUPLE_NULL);

                            tupleToSetFin(ctx, *(sfdst->subSet(i)), &tplSub, tcr, (uo && !uoSetFin));

                            if (sfdst->subSet(i)->rank0())
                                sfdst->subSet(i)->set(TP_SET_EMPTY);
                        }

                        for (; i < bcdst; i++)
                            sfdst->subSet(i)->copyFrom(sfdst->subSet(i - bcsrc));
                    }

                    sfdst->setCount();
                    sfdst->setRank();

                    if (uo && uoSetFin) {
                        // add user order to result set
                        SetWithOrder *so = new SetWithOrder(res);
                        CmplValAuto sov(TP_SET_WITH_UO, so);

                        if (uoAllRev) {
                            so->setNegOrder(true);
                        }
                        else {
                            uoSrc.push_back(tpl->at(baseInd));
                            for (unsigned r = baseInd + 1; r < tr; r++) {
                                CmplVal *v = tpl->at(r);
                                if (v->t != TP_ITUPLE_NULL)
                                    uoSrc.push_back(v);
                            }

                            so->setOrder(tupleMergeOrder(ctx, uoSrc));
                        }

                        res.moveFrom(sov);
                    }
                }

                // Zur Reihenfolge: bei Aufbau Sets untergeordnete Reihenfolgen ueber nur 1 Rang einfach uebernehmen
                //          wenn irgendwo Reihenfolge ueber mehr als 1 Rang, dann pruefen, ob ueberall direkt reverse Reihenfolge
                //          wenn ueberall direkt reverse Reihenfolge, dies als Ergebnisreihenfolge eintragen
                //          sonst neue Aufzaehlungs-Ergebnisreihenfolge, ueber Teilsets fuellen
                //  (im rekursiven Aufruf gar keine Reihenfolgen beruecksichtigen; Aufrufparameter dafuer
                //      doch beruecksichtigen, wenn baseSet nur Rang1-Reihenfolgen; dann Reihenfolge aus rekursivem Aufruf einfach fuer Subset beibehalten und keine uebergeordnete Reihenfolge
                // Unterscheidung wenn uo:
                //      In Source-Sets fuer neuen Base-Set (bzw Gesamtergebnisset wenn TP_SET_REC_MULT):
                //          a: gar keine Reihenfolge oder nur Rang-1 Reihenfolgen
                //              f3 || (!f1 && !f2)
                //                  -> keine Reihenfolge fuer Gesamtergebnis, uo rekursiv weitergeben
                //          b: Reihenfolge fuer Gesamt-Source-TP_SET_FIN, in dem der Base-Split passiert (direkte Reihenfolge)
                //              else f2
                //                  -> Ergebnisset braucht eigene direkte Reihenfolge, nachtraeglich damit umgeben; uo braucht nicht rekursiv weitergegeben werden (oder doch, fuer Aufbau Gesamtreihenfolge daraus?)
                //          c: ueberall reverse Reihenfolge (ausser fuer einelementige Sets)
                //              else f4
                //                  -> reverse Reihenfolge ueber neues Base-Set (bzw Gesamtergebnisset wenn TP_SET_REC_MULT), uo rekursiv weitergeben
                //          c: sonst
                //              else
                //                  -> neuer Base-Set (bzw Gesamtergebnisset wenn TP_SET_REC_MULT) braucht direkte Reihenfolge, fuer Subsets uo rekursiv weitergeben
                //      dazu Flags:
                //          f1      uoRecMult   direkte Reihenfolge bei TP_SET_REC_MULT (bis baseInd)
                //          f2      uoSetFin    direkte Reihenfolge bei TP_SET_FIN (an baseInd)
                //          // f3      alle Sets bisher ohne Reihenfolge (bis baseInd)
                //          f4      uoAllRev    alle Sets bisher reverse Reihenfolge (ausser 1-elementige) (bei TP_SET_REC_MULT entweder direkt oder alle untergeordneten Sets) (bis baseInd)
                // (Sollte so fertig implementiert sein!)

                if (tcr) {
                    CmplValAuto cs;
                    if (canonicalSet(cs, res))
                        res.moveFrom(cs, true);
                }
            }

            else if (mode == TP_SET_NULL) {
                // Konstruktion (einschliesslich Reihenfolge) durch volle Iteration ueber alle Teilsets
                //          und Hinzufuegen Tuple zu Ergebnisset soweit darin noch nicht enthalten
                //          (Iteration notwendig, weil Tuple-artiges Zusammenfuegen dazu fuehren koennte, dass Elemente faelschlich doppelt enthalten sind)

                res.set(TP_SET_EMPTY);

                CmplValAuto tup(TP_TUPLE, tpl, true);
                TupleIterator it(ctx, tup, uo);

                CmplValAuto t, tt;
                for (it.begin(); it; it++) {
                    tt.set((*it).t, (*it).v.vp, false);
                    addSet(ctx, &t, 0, &res, &tt, uo, false);
                    res.moveFrom(t, true);
                }

                /*
                // init iteration for all source sets
                vector<SetIterator> vsi(tr);
                CmplVal *e = tpl->at(0);
                for (unsigned r = 0; r < tr; r++, e++) {
                    if (e->isSet()) {
                        vsi[r].init(e, SetIterator::iteratorTupleVal, uo);
                    }
                    else {
                        CmplValAuto s;
                        if (e->isInterval())
                            SetUtil::constructFromInterval(ctx, s, *e);
                        else if (e->isScalarIndex())
                            s.set(TP_INDEX_VAL_SET(e->t), e->v.i);
                        else if (e->t == TP_ITUPLE_NULL)
                            s.set(TP_SET_NULL);

                        vsi[r].init(s, SetIterator::iteratorTupleVal, uo);
                    }

                    vsi[r].begin();
                }

                // iterate over all source sets
                Tuple *tpIt;
                CmplValAuto vTpIt(TP_ITUPLE, tpIt = new Tuple(0, resRank, Tuple::tupleIndexOnly));
                unsigned r = 0;

                while (vsi[0]) {
                    tpIt->concatIter(vsi, r);

                    CmplVal t;
                    addSet(ctx, &t, 0, &res, &vTpIt, uo, false);
                    res.moveFrom(t, true);

                    // iterate to next tuple
                    r = tr - 1;
                    while (true) {
                        vsi[r]++;
                        if (vsi[r] || r == 0)
                            break;

                        vsi[r].begin();
                        r--;
                    }
                }
                */

                if (tcr) {
                    if (canonicalSet(t, res))
                        res.moveFrom(t, true);
                }
            }

            else {
                if (mode == TP_SET_EMPTY)
                    res.set(TP_SET_EMPTY);
                else
                    tupleToSetInf(ctx, res, tpl, tcr);
            }
        }
    }


    /**
     * convert tuple tpl to set
     * @param ctx           execution context
     * @param res           return of result value
     * @param tpl           tuple (all parts must be scalar index values or non-empty sets or intervals, and at least one part should be infinite)
     * @param tcr			transform result set to canonical set representation
     * @param uo            keep user order
     */
    void SetUtil::tupleToSetInf(ExecContext *ctx, CmplVal& res, Tuple *tpl, bool tcr, bool uo)
    {
        vector<CmplValAuto> parts;

        bool inf = false;
        bool emp = false;
        tp_e prvSetAll = TP_BLANK;

        unsigned tr = tpl->rank();
        CmplVal *e = tpl->at(0);

        for (unsigned r = 0; r < tr; r++, e++) {
            CmplVal *ewo = &(SET_VAL_WO_ORDER(*e));

            if (ewo->rank0()) {
                ewo = NULL;
            }

            else if (ewo->isSetInf() || ewo->t == TP_BLANK || (ewo->isInterval() && Interval::isSetInf(*ewo))) {
                inf = true;
                if (ewo->t == TP_SET_ALL || ewo->t == TP_SET_ALL_MNF || ewo->t == TP_SET_ALL_INT || ewo->t == TP_SET_ALL_INT_MNF || ewo->t == TP_SET_ALL_STR || ewo->t == TP_SET_ALL_STR_MNF || ewo->t == TP_BLANK) {
                    if (prvSetAll == TP_BLANK) {
                        prvSetAll = (ewo->t != TP_BLANK ? ewo->t : TP_SET_ALL);
                    }
                    else if (prvSetAll == TP_SET_ALL || prvSetAll == TP_SET_ALL_MNF ||
                             ((prvSetAll == TP_SET_ALL_INT || prvSetAll == TP_SET_ALL_INT_MNF) && (ewo->t == TP_SET_ALL_INT || ewo->t == TP_SET_ALL_INT_MNF)) ||
                             ((prvSetAll == TP_SET_ALL_STR || prvSetAll == TP_SET_ALL_STR_MNF) && (ewo->t == TP_SET_ALL_STR || ewo->t == TP_SET_ALL_STR_MNF))) {
                        ewo = NULL;
                    }
                }
                else if (ewo->isSetInf()) {
                    if (ewo->t == TP_SET_INF_TPL) {
                        SetInfiniteTpl *sit = ewo->setInfiniteTpl();
                        if (sit->isCanonical() || !tcr) {
                            for (unsigned p = 0; p < sit->partCnt(); p++)
                                parts.emplace_back(sit->partSet(p));
                        }
                        else {
                            SetBase::partsToVector(ewo, parts, uo);
                        }
                        ewo = NULL;
                    }
                }
                else {
                    CmplValAuto v;
                    constructFromInterval(ctx, v, *ewo);
                    parts.push_back(v);
                    ewo = NULL;
                }
            }

            else if (ewo->t == TP_SET_EMPTY || (ewo->isInterval() && Interval::isEmpty(*ewo))) {
                emp = true;
                break;
            }

            else {
                if (ewo->isInterval()) {
                    CmplValAuto v;
                    constructFromInterval(ctx, v, *ewo);
                    parts.push_back(v);
                    ewo = NULL;
                }
                else if (ewo->isSet() && ewo->useValP() && tcr) {
                    if (ewo->t == TP_SET_FIN && ewo->setFinite()->baseSplit(uo) == 0)
                        checkBaseSplitFinSet(*ewo, uo);

                    SetBase::partsToVector((uo ? e : ewo), parts, uo);
                    ewo = NULL;
                }

                prvSetAll = TP_BLANK;
            }

            if (ewo) {
                if (ewo->t != TP_BLANK)
                    parts.emplace_back(uo ? e : ewo);
                else
                    parts.emplace_back(TP_SET_ALL);
            }
        }

        if (parts.size() == 0)
            res.set(TP_SET_NULL);
        else if (emp)
            res.set(TP_SET_EMPTY);
        else if (!inf)
            tupleToSetFin(ctx, res, tpl, tcr, uo);
        else if (parts.size() == 1)
            res.moveFrom(parts[0]);
        else
            res.set(TP_SET_INF_TPL, new SetInfiniteTpl(parts, tcr, uo));
    }


    /**
     * get lower and upper bound of integer elements within a rank 1 set
     * @param low           return of lower bound (can be only TP_INT or TP_INFINITE)
     * @param upp           return of upper bound (can be only TP_INT or TP_INFINITE)
     * @param src           source set
     * @return              false if set is not a rank 1 set
     */
    bool SetUtil::rank1IntBounds(CmplVal& low, CmplVal& upp, const CmplVal& src)
    {
        const CmplVal& set = SET_VAL_WO_ORDER(src);

        switch (set.t) {
            case TP_SET_R1_0_UB:
            case TP_SET_R1_0_UB_MNF:
                low.dispSet(TP_INT, 0);
                upp.dispSet(TP_INT, set.v.i);
                return true;

            case TP_SET_R1_1_UB:
            case TP_SET_R1_1_UB_MNF:
                low.dispSet(TP_INT, 1);
                upp.dispSet(TP_INT, set.v.i);
                return true;

            case TP_SET_EMPTY:
                low.dispSet(TP_INFINITE, 1);
                upp.dispSet(TP_INFINITE, -1);
                return true;

            case TP_SET_1INT:
                low.dispSet(TP_INT, set.v.i);
                upp.dispSet(TP_INT, set.v.i);
                return true;

            case TP_SET_R1_ALG:
                low.dispSet(TP_INT, SetAlg::bound(set, false));
                upp.dispSet(TP_INT, SetAlg::bound(set, true));
                return true;

            case TP_SET_R1_ENUM:
                {
                    SetEnum *se = set.setEnum();
                    if (se->cntInt()) {
                        low.dispSet(TP_INT, se->array()[0]);
                        upp.dispSet(TP_INT, se->array()[se->cntInt() - 1]);
                    }
                    else {
                        low.dispSet(TP_INFINITE, 1);
                        upp.dispSet(TP_INFINITE, -1);
                    }
                }
                return true;

            case TP_SET_R1_LB_INF:
            case TP_SET_R1_LB_INF_MNF:
                low.dispSet(TP_INT, set.v.i);
                upp.dispSet(TP_INFINITE, 1);
                return true;

            case TP_SET_R1_INF_UB:
            case TP_SET_R1_INF_UB_MNF:
                low.dispSet(TP_INFINITE, -1);
                upp.dispSet(TP_INT, set.v.i);
                return true;

            case TP_SET_R1_IINF:
            case TP_SET_R1_IINF_MNF:
                low.dispSet(TP_INFINITE, -1);
                upp.dispSet(TP_INFINITE, 1);
                return true;

            case TP_SET_R1A:
            case TP_SET_R1A_MNF:
                low.dispSet(TP_INFINITE, -1);
                upp.dispSet(TP_INFINITE, 1);
                return true;

            case TP_SET_R1A_INT:
            case TP_SET_R1A_INT_MNF:
                low.dispSet(TP_INT, 0);
                upp.dispSet(TP_INFINITE, 1);
                return true;

            default:
                return false;
        }
    }



	/****** SetConstructHelper ****/

	/**
	 * destructor
	 */
	SetConstructHelper::~SetConstructHelper()
	{
		for (map<unsigned long, CmplVal>::iterator it = _subSet.begin(); it != _subSet.end(); it++)
			it->second.dispose();

//		for (map<unsigned long, SetConstructHelper *>::iterator it = _subCtx.begin(); it != _subCtx.end(); it++) {
//			if (it->second)
//				delete it->second;
//		}
	}


	/**
	 * add element for the set
	 * @param v			element to add
     * @return			true if element is added, false if element already exists in the set
	 */
    bool SetConstructHelper::add(CmplVal &v)
	{
        bool added = false;

        if (v.t == TP_INT || v.t == TP_BIN || v.t == TP_ITUPLE_1INT || v.t == TP_SET_1INT)
            added = addInt(v.v.i);
        else if (v.t == TP_STR || v.t == TP_ITUPLE_1STR || v.t == TP_SET_1STR)
            added = addStr(v.v.i);
        else if (v.t == TP_STRINGP)
            added = addStr(_modp->data()->globStrings()->storeInd(v.v.s->c_str()));
        else if (v.t == TP_SET_NULL || v.t == TP_ITUPLE_NULL)
            added = addNull();
		else { // TODO: Tuple/Set
			// wenn unendlicher Set -> hier nicht zu behandeln; für Set-Union-Operation vorsehen
			// wenn einfacher Set (Rang 1): hier direkt Elemente ohne Iteration über SetIterator (aber wenn Nutzerreihenfolge, dann doch Iteration darüber?)
			// wenn anderer Set über Tupel iterieren und hier Tupel einfügen (oder effektiveres Vorgehen?)
			// wenn Nicht-Index-Tuple: in Index-Tuple bzw. Set wandeln
			// wenn Index-Tuple (Rang mindestens 2): berücksichtigen ab Element _tpBaseInd,
			//  erstes Element (also bei _tpBaseInd mit addInt() / addStr() für Basisset dazu, übrige dann weiter rekursiv mit _tpBaseInd = _tpBaseInd + 1
		}
		//TODO: alles andere (außer vielleicht TB_BLANK, das ignoriert wird) ist ein Fehler
		//TODO

        if (added) {
            _cnt++;

            if (_minRank > 1)
                _minRank = 1;
        }

        return added;
	}

    /**
     * add int element for the set
     * @param i			element to add
     * @return			true if element is added, false if element already exists in the set
     */
    bool SetConstructHelper::add(intType i)
    {
        bool added = addInt(i);
        if (added) {
            _cnt++;

            if (_minRank > 1)
                _minRank = 1;
        }

        return added;
    }

    /**
     * add a base element and the subset for it
     * @param i			base element
     * @param isInt		base element is int or is string
     * @param subSet	subset for the base element
     * @return			true if base element and subset are added, false if base element already exists in the set or subset is empty
     */
    bool SetConstructHelper::addBaseAndSubset(intType i, bool isInt, CmplVal& subSet)
    {
        if (subSet.t == TP_SET_EMPTY)
            return false;

        bool added = (isInt ? addInt(i) : addStr(i));
        if (added) {
            _cnt++;
            if (subSet && subSet.t != TP_SET_NULL) {
                _subSet[_cnt].copyFrom(subSet, true, false);

                unsigned r = SetBase::rank(subSet);
                if (_cnt == 1)
                    _minRank = _maxRank = r + 1;
                else if (_maxRank <= r)
                    _maxRank = r + 1;
            }
        }

        return added;
    }


    /**
	 * add int element for the set
	 * @param i			element to add
	 * @return			true if element is added, false if element already exists in the set
	 */
	bool SetConstructHelper::addInt(intType i)
	{
		switch (_mode) {
			case TP_SET_EMPTY:
				_algStart = i;
				_mode = TP_SET_1INT;
				return true;

			case TP_SET_1INT:
				if (i == _algStart)
					return false;

				_algDiff = i - _algStart;
				_mode = TP_SET_R1_ALG;
				return true;

			case TP_SET_1STR:
				changeToModeEnum();
				return addInt(i);

			case TP_SET_R1_ALG:
				// check whether element continues the algorithmic set
				if (i == _algStart + (intType)_cnt * _algDiff) 
					return true;

				// check whether element is already contained in the set
				if (_algDiff > 0) {
					if (i >= _algStart && i < _algStart + (intType)_cnt * _algDiff && (i - _algStart) % _algDiff == 0)
						return false;
				}
				else {
					if (i <= _algStart && i > _algStart + (intType)_cnt * _algDiff && (_algStart - i) % -_algDiff == 0)
						return false;
				}

				changeToModeEnum();
				return addInt(i);

			case TP_SET_R1_ENUM:
				if (_enumInt.count(i))
					return false;

				_enumInt[i] = _cnt + 1;
				return true;

			default:
				// illegal mode
				return false;
		}
	}

	/**
	 * add string element for the set
	 * @param i			element to add
	 * @return			true if element is added, false if element already exists in the set
	 */
	bool SetConstructHelper::addStr(intType i)
	{
		switch (_mode) {
			case TP_SET_EMPTY:
				_algStart = i;
				_mode = TP_SET_1STR;
				return true;

			case TP_SET_1INT:
			case TP_SET_1STR:
			case TP_SET_R1_ALG:
				changeToModeEnum();
				return addStr(i);

			case TP_SET_R1_ENUM:
				if (_enumStr.count(i))
					return false;

				_enumStr[i] = _cnt + 1;
				return true;

			default:
				// illegal mode
				return false;
		}
	}

	/**
	 * add null tuple for the set
	 * @return			true if element is added, false if element already exists in the set
	 */
	bool SetConstructHelper::addNull()
	{
		if (_nNull)
			return false;

		_nNull = _cnt + 1;
		_minRank = 0;
		return true;
	}


	/**
	 * change mode to TP_SET_R1_ENUM
	 * (current mode must be one of TP_SET_1INT, TP_SET_1STR or TP_SET_R1_ALG)
	 */
	void SetConstructHelper::changeToModeEnum()
	{
		if (_mode == TP_SET_1INT) {
			_enumInt[_algStart] = (_nNull == 1 ? 2 : 1);
		}
		else if (_mode == TP_SET_1STR) {
			_enumStr[_algStart] = (_nNull == 1 ? 2 : 1);
		}
		else { // if (_mode == TP_SET_R1_ALG)
			intType v = _algStart;
			for (unsigned long i = 1; i <= _cnt; i++, v += _algDiff) {
				if (i == _nNull)
					i++;

				_enumInt[v] = i;
			}
		}

		_mode = TP_SET_R1_ENUM;
	}


	/**
	 * construct resulting set
	 * @param res		store for result set
	 */
	void SetConstructHelper::construct(CmplVal &res)
	{
		// SetFinite is needed if the set has subsets or if it contains the null tuple and other elements
        if (_subSet.size() > 0 /* || _subCtx.size() > 0 */ || (_nNull && _mode != TP_SET_EMPTY))
		{
            CmplValAuto bs;
			constructBase(bs);

            //TODO: _minRank

//			// construct subsets
//			if (_subCtx.size() > 0) {
//				for (map<unsigned long, SetConstructHelper *>::iterator it = _subCtx.begin(); it != _subCtx.end(); it++) {
//					if (_subSet.count(it->first)) {
//						//TODO: Situation generell verbieten? Oder hier dann Sets geeignet zusammenfassen (den vorhandenen Set _subSet[it->first] und den zu konstruierenden it->second->construct())?
//					}
//					else if (it->second) {
//						it->second->construct(_subSet[it->first]);
//                        unsigned r = SetBase::rank(_subSet[it->first]);
//						if (_maxRank <= r)
//							_maxRank = r + 1;

//						delete it->second;
//						it->second = NULL;
//					}
//				}
//			}

			// compare subsets
			unsigned eqr = 0;
			bool cc = true;

			if (_subSet.size() > 0) {
				if (_subSet.size() < (_cnt - (_nNull ? 1 : 0))) {
					cc = false;
				}
				else {
					eqr = _maxRank - 1;
					map<unsigned long, CmplVal>::iterator itf = _subSet.begin();
					map<unsigned long, CmplVal>::iterator it = itf;

					for (it++; it != _subSet.end() && (eqr || cc); it++) {
						cc = SetUtil::compareEqRank(&(itf->second), &(it->second), eqr, cc, true);
					}
				}
			}

			// create result set
			//TODO: user order (vorerst wird hier jede Nutzerreihenfolge einfach ignoriert)
            CmplVal *arr = SetUtil::arrayForFinSet(bs, cc, eqr, _rev, _subSet, _nNull);
            if (!arr && _minRank) {
				res.copyFrom(bs, true, false);			
			}
			else {
                SetFinite *sf = new SetFinite(bs, arr, _minRank, _maxRank);
				res.set(TP_SET_FIN, sf);
			}
		}
		else {
			if (_nNull)
				res.set(TP_SET_NULL);
			else
				constructBase(res);
		}
	}

    /**
     * construct resulting set with no index from base elements
     * @param ctx		execution context
     * @param res		store for result set
     * @param se		syntax element id of source values
     * @param arr       reorder elements in this array according result set
     * @return          true if success, false if indizes from subsets are not unique
     */
    bool SetConstructHelper::constructWoi(ExecContext *ctx, CmplVal &res, unsigned se, CmplVal *arr)
    {
        if (_nNull) {
            return false;
        }

        else if (_mode == TP_SET_EMPTY) {
            res.set(TP_SET_EMPTY);
        }

        else if (_subSet.size() < _cnt - 1) {
            return false;
        }

        else if (_subSet.empty()) {
            res.set(TP_SET_NULL);
        }

        else {
            CmplValAuto r(TP_SET_EMPTY);
            CmplVal n(TP_SET_NULL);
            unsigned long rc = 0;

            for (unsigned long i = (_rev ? _cnt : 1); i >= 1 && i <= _cnt; i += (_rev ? -1 : 1))
            {
                CmplVal& s = (_subSet.count(i) ? _subSet[i] : n);

                if (r.t == TP_SET_EMPTY) {
                    r.copyFrom(s);
                    rc = SetBase::cnt(r);
                }
                else {
                    CmplValAuto rn;
                    SetUtil::addSet(ctx, &rn, se, &r, &s, true, false);
                    r.moveFrom(rn);

                    rc += SetBase::cnt(s);
                    if (rc > SetBase::cnt(r))
                        return false;
                }
            }

            if (!SetUtil::canonicalSet(res, r))
                res.moveFrom(r);

            if (arr && SetBase::hasUserOrder(res)) {
                // arr umsortieren
                //  Ausgangssituation:
                //      Elemente in arr sind nach den Subsets sortiert:
                //              zuerst alle des ersten Subsets (wenn _rev das mit Index _cnt), dann alle des naechsten Subsets, usw.
                //      Innerhalb jedes Subsets stehen die Elemente in arr dagegen schon in kanonischer Reihenfolge
                //  Zielreihenfolge:
                //      Alle Elemente in kanonischer Reihenfolge des Ergebnissets res
                //  Vorgehen:
                //      Durchgehen der Subsets (wie oben)
                //          Elemente des Subsets stehen fortlaufend kanonisch in einem Bereich in arr: start .. (start + cnt(subset) - 1)
                //          Also Subset in kanonischer Reihenfolge iterieren, damit Quellindex einfach fortlaufend aufsteigend
                //              Aktuelles Subset-Element in res suchen, in kanonischer Reihenfolge, Position ist Zielindex
                //              Wenn Zielindex != Quellindex: Quellelement in temporaerem Mapping zu Zielindex speichern
                //      Alle im Mapping gespeicherte Elemente nach arr zum Zielindex

                map<unsigned long, CmplVal> tm;
                CmplVal *src = arr;

                for (unsigned long i = (_rev ? _cnt : 1); i >= 1 && i <= _cnt; i += (_rev ? -1 : 1)) {
                    if (_subSet.count(i)) {
                        SetIterator iter(_subSet[i]);
                        for (iter.begin(); iter; iter++) {
                            unsigned long di;
                            if (!SetUtil::tupleInSet(ctx, res, *iter, di))
                                throw invalid_argument("inserted element not found in set");

                            if (di != (unsigned long)(src - arr))
                                tm[di].moveFrom(src);

                            src++;
                        }
                    }
                    else {
                        if (src != arr)
                            tm[0].moveFrom(src);
                        src++;
                    }
                }

                for (auto i : tm)
                    arr[i.first].moveFrom(i.second);
            }
        }

        return true;
    }

	/**
	 * construct resulting base set
	 * @param bs		store for result set
	 */
	void SetConstructHelper::constructBase(CmplVal &bs)
	{
		switch (_mode) {
			case TP_SET_EMPTY:
				bs.set(_mode);
				break;

			case TP_SET_1INT:
			case TP_SET_1STR:
				bs.set(_mode, _algStart);
				break;

			case TP_SET_R1_ALG:
				{
					intType st = (_rev ? (_algStart + (_cnt - 1) * _algDiff) : _algStart);
					unsigned long sp = (_rev ? -_algDiff : _algDiff);

					if (sp == 1 && (st == 0 || st == 1)) {
						if (st == 0)
							bs.set(TP_SET_R1_0_UB, (intType)(_cnt - 1));
						else
							bs.set(TP_SET_R1_1_UB, (intType)_cnt);
					}
					else {
						SetAlg *s;
						if (sp > 0)
							s = new SetAlg(st, sp, _cnt, false);
						else
							s = new SetAlg((st + (_cnt - 1) * sp), -sp, _cnt, true);

						bs.set(TP_SET_R1_ALG, s);
					}
				}
				break;

			case TP_SET_R1_ENUM:
				{
					// copy elements in the set
					intType *arr = new intType[_cnt];
					intType *a = arr;
					bool uo = false;
					unsigned long w = (_rev ? _cnt+1 : 0);
					int li;

					for (li = 0; li < 2; li++) {
						map<intType, unsigned long>& mp = (li == 0 ? _enumInt : _enumStr);
						for (map<intType, unsigned long>::iterator it = mp.begin(); it != mp.end(); it++) {
							*(a++) = it->first;
							if (!uo) {
								if ((_rev && it->second > w) || (!_rev && it->second < w))
									uo = true;
								else
									w = it->second;
							}
						}
					}

					// handle user order of the set
					unsigned long *ord = NULL;
					if (uo) {
						w = 0;
						map<unsigned long, unsigned long> mo;
						for (li = 0; li < 2; li++) {
							map<intType, unsigned long>& mp = (li == 0 ? _enumInt : _enumStr);
							for (map<intType, unsigned long>::iterator it = mp.begin(); it != mp.end(); it++)
								mo[_rev ? _cnt - it->second : it->second] = w++;
						}

						ord = new unsigned long[_cnt];
						unsigned long *op = ord;
						for (map<unsigned long, unsigned long>::iterator it = mo.begin(); it != mo.end(); it++)
							*(op++) = it->second;
					}

					SetEnum *s = new SetEnum(_cnt, _enumInt.size(), arr, ord);
					bs.set(TP_SET_R1_ENUM, s);
				}
				break;

			default:
				// illegal value
				break;
		}		
	}

}

