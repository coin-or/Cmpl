
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


#include "SetInfinite.hh"
#include "SetUtil.hh"
#include "TupleUtil.hh"
#include "ExecContext.hh"
#include "../../CommonData/IntCode.hh"


namespace cmpl
{

	/****** SetInfinite ****/



    /****** SetInfiniteTpl ****/

    /**
     * constructor
     * @param pc			count of part sets
     * @param arr			array with part sets
     */
    SetInfiniteTpl::SetInfiniteTpl(unsigned pc, CmplVal *arr)
    {
        _partCnt = pc;
        _parts = arr;

        _markNF = false;
        _isCanonical = true;
        _subOrder = false;

        _minRank = 0;
        _maxRank = 0;

        CmplVal *s = arr;
        for (unsigned r = 0; r < pc; r++, s++) {
            if (_isCanonical && !SetBase::isCanonical(*s))
                _isCanonical = false;

            if (!_subOrder && SetBase::hasUserOrder(*s))
                _subOrder = true;

            _minRank += SetBase::minRank(s);
            if (_maxRank < UINT_MAX) {
                unsigned rr = SetBase::maxRank(s);
                if (rr == UINT_MAX || _maxRank + rr >= UINT_MAX || _maxRank + rr < _maxRank)
                    _maxRank = UINT_MAX;
                else
                    _maxRank += rr;
            }
        }
    }

    /**
     * constructor
     * @param parts			vector with part sets
     * @param tcr			transform part sets to canonical set representation
     * @param uo            keep user order
     */
    SetInfiniteTpl::SetInfiniteTpl(vector<CmplValAuto>& parts, bool tcr, bool uo)
    {
        _partCnt = parts.size();
        _parts = new CmplVal[_partCnt];

        _markNF = false;
        _isCanonical = true;
        _subOrder = false;

        _minRank = 0;
        _maxRank = 0;

        CmplVal *s = _parts;
        for (unsigned r = 0; r < _partCnt; r++, s++) {
            CmplValAuto& p = parts[r];

            if (p.t == TP_SET_WITH_UO && uo) {
                s->copyFrom(p, true, false);
                _subOrder = true;
            }
            else {
                s->copyFrom(SET_VAL_WO_ORDER(p), true, false);
            }

            if (!SetBase::isCanonical(*s)) {
                if (tcr) {
                    CmplValAuto v;
                    if (SetUtil::canonicalSet(v, *s, !uo))
                        s->moveFrom(v);
                }
                else {
                    _isCanonical = false;
                }
            }

            _minRank += SetBase::minRank(s);
            if (_maxRank < UINT_MAX) {
                unsigned rr = SetBase::maxRank(s);
                if (rr == UINT_MAX || _maxRank + rr >= UINT_MAX || _maxRank + rr < _maxRank)
                    _maxRank = UINT_MAX;
                else
                    _maxRank += rr;
            }
        }
    }


    /**
     * get max bound for tuple part count this set is divisible in
     */
    unsigned SetInfiniteTpl::maxTupleParts() const
    {
        if (_isCanonical) {
            return _partCnt;
        }
        else {
            vector<CmplValAuto> dtp;
            partsToVector(dtp, true);
            return dtp.size();
        }
    }


    /**
     * push all tuple divisible parts in a vector
     * @param dtp       vector to push parts in
     * @param uo        consider user order
     * @return          true if parts are pushed to vector; false if set is not tuple divisible
     */
    bool SetInfiniteTpl::partsToVector(vector<CmplValAuto>& dtp, bool uo) const
    {
        if (_isCanonical && (uo || !_subOrder)) {
            CmplVal *p = _parts;
            for (unsigned i = 0; i < _partCnt; i++, p++)
                dtp.emplace_back(p);
        }
        else {
            CmplVal *p = _parts;
            for (unsigned i = 0; i < _partCnt; i++, p++) {
                if (p->isSet() && p->useValP() && !SetBase::rank1(*p))
                    SetBase::partsToVector(*p, dtp, uo);
                else
                    dtp.emplace_back(p);
            }
        }

        return true;
    }


    /**
     * get set with all user order removed
     * @param res		value for result
     */
    void SetInfiniteTpl::stripOrder(CmplVal& res)
    {
        SetInfiniteTpl *s;
        if (_subOrder) {
            CmplVal *arr = new CmplVal[_partCnt];
            CmplVal *q, *d;
            unsigned r;

            for (r = 0, q = _parts, d = arr; r < _partCnt; r++, d++, q++)
                d->copyFrom(SET_VAL_WO_ORDER(*q), true);

            s = new SetInfiniteTpl(_partCnt, arr);
        }
        else {
            s = this;
        }

        res.set(TP_SET_INF_TPL, s);
    }


    /**
     * convert this to the aequivalent set in canonical representation
     * @param res			return of result set
     * @param woOrder       strip order from set
     * @return				true if result set is computed; false if this is already in canonical representation
     */
    bool SetInfiniteTpl::canonicalSet(CmplVal &res, bool woOrder)
    {
        if (_isCanonical)
            return false;

        //TODO
        // - all subsets must be canonical
        // - if no subset is infinite, then change to finite set
        // - no one of the subsets is further divisible by partsToVector (unter Beruecksichtigung Reihenfolge, ausser wenn mit woOrder)
        // - enthaelt keine Sets, die fuer Tuplematching irrelevant sind (z.B. TP_SET_NULL, oder z.B. TP_SET_ALL_INT, der direkt auf TP_SET_ALL_INT oder TP_SET_ALL folgt)
        return false;   //TODO
    }


    /**
     * check if tuple or a part of a tuple is element within a set
     * @param ctx			execution context
     * @param tup			tuple to search (can also be TP_LIST_TUPLE, then it must be the topmost element on the value stack of the execution context)
     * @param tps			start index of used tuple part
     * @param tpc			count of indizes of used tuple part / -1: up to the end of tuple
     * @return 				true if tuple is element of the set
     */
    bool SetInfiniteTpl::tupleInSet(ExecContext *ctx, const CmplVal &tup, unsigned tps, int tpc) const
    {
        unsigned trf = (tup.t == TP_LIST_TUPLE ? (unsigned)tup.v.i : Tuple::rank(tup));
        unsigned tr = (trf > tps ? trf - tps : 0);
        if (tpc >= 0 && tr > (unsigned)tpc)
            tr = tpc;

        if (tr == 0)
            return (_partCnt == 0 || _minRank == 0);

        if (tr < _minRank || tr > _maxRank)
            return false;

        if (_partCnt == 1) {
            unsigned long dummy;
            return (SetUtil::tupleInSet(ctx, _parts[0], tup, dummy, tps, tr, true) == tr);
        }

        // check by tuple matching
        bool ntpl = (tr < trf || tup.t == TP_LIST_TUPLE);
        CmplValAuto tpl;

        if (ntpl) {
            if (tup.t == TP_ITUPLE) {
                tup.tuple()->partTuple(tpl, tps, tr);
            }
            else {
                if (tr == 1) {
                    if (tup.t == TP_LIST_TUPLE) {
                        const CmplVal& sc = (ctx->stackCur() - tup.v.i + tps)->val();
                        tpl.copyFrom(sc);
                        if (tpl.useInt())
                            tpl.t = TP_INDEX_VAL_TUPLE(tpl.t);
                    }
                    else if (tup.useInt() && tps == 0) {
                        tpl.t = TP_INDEX_VAL_TUPLE(tup.t);
                        tpl.v.i = tup.v.i;
                    }
                }
                else if (tup.t == TP_LIST_TUPLE) {
                    tpl.set(TP_ITUPLE, new Tuple(tr));
                    Tuple& t = *(tpl.tuple());

                    for (unsigned r = 0; r < tr; r++) {
                        const CmplVal& sc = (ctx->stackCur() - tup.v.i + tps + r)->val();
                        t[r].copyFrom(sc);
                    }
                }
            }
        }

        CmplVal s(TP_SET_INF_TPL, const_cast<SetInfiniteTpl *>(this), false);		// CmplVal constructor with inc=false doesn't change this, so removing const is save here
        TupleMatching tm(ctx, TupleMatching::matchIn, s, (ntpl ? tpl : tup), false);

        CmplValAuto res;
        return tm.match(res);
    }


    /**
     * write contents of the object to a stream
     * @param modp			calling module
     * @param mode			mode for output: 0=direct; 1=part of other value
     */
    void SetInfiniteTpl::write(ostream& ostr, ModuleBase *modp, int mode) const
    {
        if (mode == 0)
            ostr << '[';

        for (unsigned r = 0; r < _partCnt; r++) {
            if (r)
                ostr << ", ";

            _parts[r].write(ostr, modp, 1);
        }

        if (mode == 0)
            ostr << ']';
    }



    /****** SetInfiniteSet ****/

    /**
     * constructor
     * @param pc			count of part sets
     * @param arr			array with part sets
     * @param op            constructing operation
     */
    SetInfiniteSet::SetInfiniteSet(unsigned pc, CmplVal *arr, unsigned short op)
    {
        _partCnt = pc;
        _parts = arr;
        _constructOp = op;

        _markNF = false;
        _isCanonical = true;

        CmplVal *s = arr;
        for (unsigned r = 0; r < pc; r++, s++) {
            if (SetBase::hasUserOrder(*s)) {
                CmplVal s2;
                SetBase::stripOrder(*s, s2);
                s->moveFrom(s2, true);
            }

            if (_isCanonical && !SetBase::isCanonical(*s))
                _isCanonical = false;
        }
    }

#if 0
    /**
     * get minimal rank of tuples within the tuple set
     */
    unsigned SetInfiniteSet::minRank() const
    {
        CmplVal *p = _parts;
        unsigned r = SetBase::minRank(*p);

        p++;
        if (r == 0) {
            return (_constructOp == ICS_OPER_SUB && SetBase::minRank(*p) == 0 ? 1 : 0);
        }
        else {
            if (_constructOp == ICS_OPER_ADD) {
                for (unsigned i = 1; i < _partCnt; i++, p++)
                    r = min(SetBase::minRank(*p), r);
            }
            else if (_constructOp == ICS_OPER_MUL) {
                for (unsigned i = 1; i < _partCnt; i++, p++)
                    r = max(r, SetBase::minRank(*p));
            }
        }

        return r;
    }


    /**
     * get maximal rank of tuples within the tuple set
     * @param ub        upper bound for return value
     */
    unsigned SetInfiniteSet::maxRank(unsigned ub) const
    {
        CmplVal *p = _parts;
        unsigned r = SetBase::maxRank(*p, ub);
        if (r == ub && _constructOp != ICS_OPER_MUL)
            return ub;

        p++;
        if (_constructOp == ICS_OPER_ADD) {
            for (unsigned i = 1; i < _partCnt; i++, p++) {
                unsigned rr = SetBase::maxRank(*p, ub);
                if (rr == ub)
                    return ub;

                if (rr > r)
                    r = rr;
            }
        }
        else if (_constructOp == ICS_OPER_MUL) {
            unsigned rr = SetBase::maxRank(*p, ub);
            if (rr < r)
                r = rr;
        }

        return r;
    }
#endif


    /**
     * convert this to the aequivalent set in canonical representation
     * @param res			return of result set
     * @param woOrder       strip order from set (ignored because this has never a considered user order)
     * @return				true if result set is computed; false if this is already in canonical representation
     */
    bool SetInfiniteSet::canonicalSet(CmplVal &res, bool woOrder)
    {
        if (_isCanonical)
            return false;

        //TODO
        // - all subsets must be canonical
        // - if no subset is infinite, then change to finite set
        // - contains no more than one finite direct subset; contains no direct subsets TP_SET_INF_SET with the same constructing operation
        //      wenn _constructOp == ICS_OPER_MUL, darf gar kein endlicher Set enthalten sein; bei _constructOp == ICS_OPER_SUB darf der erste Set nicht endlich sein
        // - enthaelt keine trivial ueberfluessigen Sets/Operationen
        return false;   //TODO
    }


    /**
     * check if tuple or a part of a tuple is element within a set
     * @param ctx			execution context
     * @param tup			tuple to search (can also be TP_LIST_TUPLE, then it must be the topmost element on the value stack of the execution context)
     * @param tps			start index of used tuple part
     * @param tpc			count of indizes of used tuple part / -1: up to the end of tuple
     * @return 				true if tuple is element of the set
     */
    bool SetInfiniteSet::tupleInSet(ExecContext *ctx, const CmplVal &tup, unsigned tps, int tpc) const
    {
        if (!_partCnt)
            return false;

        unsigned tr = (tup.t == TP_LIST_TUPLE ? (unsigned)tup.v.i : Tuple::rank(tup));
        tr = (tr > tps ? tr - tps : 0);
        if (tpc >= 0 && tr > (unsigned)tpc)
            tr = tpc;

        if (tr < _minRank || tr > _maxRank)
            return false;

        if (tr == 0 && _minRank == 0)
            return true;

        unsigned long dummy;
        bool f = (SetUtil::tupleInSet(ctx, _parts[0], tup, dummy, tps, tr, true) == tr);
        if ((f && _constructOp == ICS_OPER_ADD) || (!f && _constructOp != ICS_OPER_ADD) || _partCnt == 1)
            return f;

        if (_constructOp == ICS_OPER_SUB)
            return (SetUtil::tupleInSet(ctx, _parts[1], tup, dummy, tps, tr, true) != tr);

        for (unsigned r = 1; r < _partCnt; r++) {
            f = (SetUtil::tupleInSet(ctx, _parts[r], tup, dummy, tps, tr, true) == tr);
            if ((f && _constructOp == ICS_OPER_ADD) || (!f && _constructOp != ICS_OPER_ADD))
                return f;
        }

        return f;
    }


    /**
     * write contents of the object to a stream
     * @param modp			calling module
     * @param mode			mode for output: 0=direct; 1=part of other value
     */
    void SetInfiniteSet::write(ostream& ostr, ModuleBase *modp, int mode) const
    {
        ostr << "set(";

        for (unsigned r = 0; r < _partCnt; r++) {
            if (r)
                ostr << (_constructOp == ICS_OPER_ADD ? " + " : (_constructOp == ICS_OPER_MUL ? " * " : " - "));

            _parts[r].write(ostr, modp, 0);
        }

        ostr << ')';
    }

}

