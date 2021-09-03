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


#include "SetBase.hh"
#include "SetIterator.hh"
#include "../Control/MainControl.hh"


using namespace std;


namespace cmpl
{

    /************** SetBase **********/

    /**
     * get count of elements in the set
     * @param v				set to count elements (must be of any finite set type)
     * @return				count of elements
     */
    unsigned long SetBase::cnt(const CmplVal& v)
    {
        const CmplVal& set = SET_VAL_WO_ORDER(v);

        if (set.useValP()) {
            return set.setBase()->cnt();
        }
        else {
            switch (set.t) {
                case TP_SET_R1_0_UB:
                case TP_SET_R1_0_UB_MNF:
                    return (unsigned long)(set.v.i + 1);
                case TP_SET_R1_1_UB:
                case TP_SET_R1_1_UB_MNF:
                    return (unsigned long)(set.v.i);

                case TP_SET_EMPTY:
                    return 0;
                case TP_SET_NULL:
                case TP_SET_1INT:
                case TP_SET_1STR:
                    return 1;

                default:		// no finite set
                    return 1;
            }
        }
    }

    /**
     * @brief set or unset explicit mark as non-free for a set
     * @param v         set
     * @param mnf       true: mark as non-free / false: mark as free
     */
    void SetBase::setMarkNF(CmplVal& v, bool mnf)
    {
        CmplVal& set = SET_VAL_WO_ORDER(v);

        switch (set.t) {
            case TP_SET_FIN:
            case TP_SET_REC_MULT:
            case TP_SET_R1_ENUM:
            case TP_SET_R1_ALG:
            case TP_SET_INF_TPL:
            case TP_SET_INF_SET:
                ((SetBase2 *)(set.setBase()))->_markNF = mnf;
                break;

            case TP_SET_R1_0_UB:
            case TP_SET_R1_0_UB_MNF:
                set.t = (mnf ? TP_SET_R1_0_UB_MNF : TP_SET_R1_0_UB);
                break;
            case TP_SET_R1_1_UB:
            case TP_SET_R1_1_UB_MNF:
                set.t = (mnf ? TP_SET_R1_1_UB_MNF : TP_SET_R1_1_UB);
                break;

            case TP_SET_NULL:
            case TP_ITUPLE_NULL:
                set.t = (mnf ? TP_ITUPLE_NULL : TP_SET_NULL);
                break;

            case TP_SET_1INT:
            case TP_ITUPLE_1INT:
            case TP_INT:
            case TP_BIN:
                set.t = (mnf ? TP_INT : TP_SET_1INT);
                break;
            case TP_SET_1STR:
            case TP_ITUPLE_1STR:
            case TP_STR:
                set.t = (mnf ? TP_STR : TP_SET_1STR);
                break;

            case TP_SET_R1_LB_INF:
            case TP_SET_R1_LB_INF_MNF:
                set.t = (mnf ? TP_SET_R1_LB_INF_MNF : TP_SET_R1_LB_INF);
                break;
            case TP_SET_R1_INF_UB:
            case TP_SET_R1_INF_UB_MNF:
                set.t = (mnf ? TP_SET_R1_INF_UB_MNF : TP_SET_R1_INF_UB);
                break;
            case TP_SET_R1_IINF:
            case TP_SET_R1_IINF_MNF:
                set.t = (mnf ? TP_SET_R1_IINF_MNF : TP_SET_R1_IINF);
                break;

            case TP_SET_R1A:
            case TP_SET_R1A_MNF:
                set.t = (mnf ? TP_SET_R1A_MNF : TP_SET_R1A);
                break;
            case TP_SET_R1A_INT:
            case TP_SET_R1A_INT_MNF:
                set.t = (mnf ? TP_SET_R1A_INT_MNF : TP_SET_R1A_INT);
                break;
            case TP_SET_R1A_STR:
            case TP_SET_R1A_STR_MNF:
                set.t = (mnf ? TP_SET_R1A_STR_MNF : TP_SET_R1A_STR);
                break;

            case TP_SET_ALL:
            case TP_SET_ALL_MNF:
            case TP_TUPLE_EMPTY:
                set.t = (mnf ? TP_SET_ALL_MNF : TP_SET_ALL);
                break;
            case TP_SET_ALL_INT:
            case TP_SET_ALL_INT_MNF:
                set.t = (mnf ? TP_SET_ALL_INT_MNF : TP_SET_ALL_INT);
                break;
            case TP_SET_ALL_STR:
            case TP_SET_ALL_STR_MNF:
                set.t = (mnf ? TP_SET_ALL_STR_MNF : TP_SET_ALL_STR);
                break;

            default:
                break;
        }
    }


    /**
     * get whether set is in canonical representation
     */
    bool SetBase::isCanonical(const CmplVal& v)
    {
        if (v.useValP())
            return v.setBase()->isCanonical();
        else if (v.t == TP_SET_R1_0_UB || v.t == TP_SET_R1_0_UB_MNF || v.t == TP_SET_R1_1_UB || v.t == TP_SET_R1_1_UB_MNF)
            return (v.v.i > (v.t == TP_SET_R1_0_UB || v.t == TP_SET_R1_0_UB_MNF ? 0 : 1));
        else if (v.t == TP_SET_R1_LB_INF || v.t == TP_SET_R1_LB_INF_MNF)
            return (v.v.i != 0);
        else
            return true;
    }



    /************** SetAlg **********/

	/**
	 * write contents of the object to a stream
	 * @param modp			calling module
	 * @param mode			mode for output: 0=direct; 1=part of other value
	 */
	void SetAlg::write(ostream& ostr, ModuleBase *modp, int mode) const
	{
		if (mode == 0)
			ostr << '[';
        if (_markNF)
            ostr << "/set(";

        if (_step == 1)
            ostr << _start << ".." << (_start + _cnt - 1);
        else
            ostr << _start << '(' << _step << ')' << (_start + _step * (_cnt - 1));

        if (_markNF)
            ostr << ')';
        if (mode == 0)
			ostr << ']';
	}


    /**
     * get upper or lower bound of an algorithmic set
     * @param set			set (must be algorithmic set)
     * @param ub			true: get upper bound / false: get lower bound
     * @return				upper or lower bound
     */
    intType SetAlg::bound(const CmplVal& set, bool ub)
    {
        if (ub) {
            if (set.t == TP_SET_R1_ALG) {
                SetAlg *s = set.setAlg();
                return s->_start + ((s->_cnt - 1) * s->_step);
            }
            else {
                return set.v.i;
            }
        }
        else {
            if (set.t == TP_SET_R1_ALG)
                return set.setAlg()->_start;
            else if (set.t == TP_SET_1INT)
                return set.v.i;
            else if (set.t == TP_SET_R1_1_UB || set.t == TP_SET_R1_1_UB_MNF)
                return 1;
            else
                return 0;
        }
    }


    /**
	 * set tuple elements for the tuple on a given index position in the set
	 * @param ind			index number within the set
	 * @param tpl			tuple value for inserting the elements
	 * @param ti			start element number in tpl
     * @param useOrder		use user order of subsets
     * @param reverse		use reverse order
     * @return				index of the tuple within the set
	 */
    unsigned long SetAlg::tupleAtIntern(unsigned long ind, Tuple *tpl, unsigned ti, bool useOrder, bool reverse)
	{
        unsigned long tplInd = (!reverse ? ind : (_cnt - 1 - ind));
		 
		tpl->setRank(ti + 1);
		tpl->at(ti)->set(TP_INT, (intType)(_start + tplInd * _step));

		return tplInd;
	}



	/************** SetEnum **********/

	/**
	 * write contents of the object to a stream
	 * @param modp			calling module
	 * @param mode			mode for output: 0=direct; 1=part of other value
	 */
	void SetEnum::write(ostream& ostr, ModuleBase *modp, int mode) const
	{
        if (_markNF)
            ostr << '/';

        ostr << "set(";

		CmplVal v(TP_STR);
		for (unsigned long i = 0; i < _cnt; i++) {
			if (i)
				ostr << ", ";

            if (i < _cntInt) {
                ostr << _array[i];
			}
			else {
                v.v.i = _array[i];
				v.write(ostr, modp, 1);
			}
		}

		ostr << ')';
	}

	/**
	 * check whether the set is equal to another set
	 * @param s2			other set
	 * @param uo			check user order also
	 * @return				true if both sets are equal
	 */
    bool SetEnum::equal(const SetEnum *s2) const
	{
		if (_cnt != s2->_cnt || _cntInt != s2->_cntInt)
			return false;

		if (!SetIterator::compareArr<intType>(_cnt, _array, s2->_array))
			return false;

		return true;
	}

	/**
	 * set tuple elements for the tuple on a given index position in the set
	 * @param ind			index number within the set
	 * @param tpl			tuple value for inserting the elements
	 * @param ti			start element number in tpl
     * @param useOrder		use user order of subsets
     * @param reverse		use reverse order
     * @return				index of the tuple within the set
	 */
    unsigned long SetEnum::tupleAtIntern(unsigned long ind, Tuple *tpl, unsigned ti, bool useOrder, bool reverse)
	{
		unsigned long tplInd = (!reverse ? ind : (_cnt - 1 - ind));

		tpl->setRank(ti + 1);
		tpl->at(ti)->set((tplInd < _cntInt ? TP_INT : TP_STR), _array[tplInd]);

		return tplInd;
	}



	/************** SetRecMult **********/

	/**
	 * constructor
	 * @param rank			rank of set (count of elements in arr)
	 * @param arr			array with part sets
	 */
	SetRecMult::SetRecMult(unsigned rank, CmplVal *arr)
	{
		_rank = rank;
		_array = arr;

        _cnts = NULL;
        setCnt(true);
	}

	/**
	 * constructor: create set from two other sets
	 * @param s1			first source set (must be one of TP_SET_R1_ENUM, TP_SET_R1_ALG, TP_SET_R1_0_UB, TP_SET_R1_1_UB, TP_SET_1INT, TP_SET_1STR, TP_INT, TP_STR or TP_SET_REC_MULT)
	 * @param s2			second source set (must be one of TP_SET_R1_ENUM, TP_SET_R1_ALG, TP_SET_R1_0_UB, TP_SET_R1_1_UB, TP_SET_1INT, TP_SET_1STR, TP_INT, TP_STR or TP_SET_REC_MULT)
     * @param woOrder       without user order
     */
    SetRecMult::SetRecMult(CmplVal& s1, CmplVal& s2, bool woOrder)
	{
        unsigned r1 = (s1.t == TP_INT || s1.t == TP_STR ? 1 : SetBase::rank(s1));
        unsigned long c1 = (s1.t == TP_INT || s1.t == TP_STR ? 1 : SetBase::cnt(s1));

        unsigned r2 = (s2.t == TP_INT || s2.t == TP_STR ? 1 : SetBase::rank(s2));
        unsigned long c2 = (s2.t == TP_INT || s2.t == TP_STR ? 1 : SetBase::cnt(s2));

		_rank = r1 + r2;
        _cnt = c1 * c2;
        _markNF = false;
        _isCanonical = false;

		_array = new CmplVal[_rank];
		_cnts = new unsigned long[_rank];

        bool uo = (SetBase::hasUserOrder(s1) || SetBase::hasUserOrder(s2));
        _subOrder = (uo && !woOrder);

		unsigned id = 0;
		if (s1.t == TP_SET_REC_MULT) {
			for (unsigned ic = 0; ic < r1; ic++) {
				_cnts[id] = s1.setRecMult()->_cnts[ic];
                CmplVal& v = (uo && woOrder ? SET_VAL_WO_ORDER(s1.setRecMult()->_array[ic]) : s1.setRecMult()->_array[ic]);
                _array[id++].copyFrom(v, true, false);
			}
		}
		else {
			_cnts[id] = c1;
            CmplVal& v = (uo && woOrder ? SET_VAL_WO_ORDER(s1) : s1);
            _array[id++].copyFrom(v, true, false);
		}

		if (s2.t == TP_SET_REC_MULT) {
			for (unsigned ic = 0; ic < r2; ic++) {
				_cnts[id] = s2.setRecMult()->_cnts[ic];
                CmplVal& v = (uo && woOrder ? SET_VAL_WO_ORDER(s2.setRecMult()->_array[ic]) : s2.setRecMult()->_array[ic]);
                _array[id++].copyFrom(v, true, false);
			}
		}
		else {
			_cnts[id] = c2;
            CmplVal& v = (uo && woOrder ? SET_VAL_WO_ORDER(s2) : s2);
            _array[id++].copyFrom(v, true, false);
		}
	}

    /**
     * constructor: create set from two other sets (use instead of previous constructor to handle if s1 or s2 is TP_SET_REC_MULT with direct order!)
     * @param s1			first source set (must be one of TP_SET_R1_ENUM, TP_SET_R1_ALG, TP_SET_R1_0_UB, TP_SET_R1_1_UB, TP_SET_1INT, TP_SET_1STR, TP_INT, TP_STR or TP_SET_REC_MULT)
     * @param s2			second source set (must be one of TP_SET_R1_ENUM, TP_SET_R1_ALG, TP_SET_R1_0_UB, TP_SET_R1_1_UB, TP_SET_1INT, TP_SET_1STR, TP_INT, TP_STR or TP_SET_REC_MULT)
     * @param tp            return of type of result
     * @param woOrder       without user order
     */
    SetBase* SetRecMult::construct(CmplVal& s1, CmplVal& s2, tp_e& tp, bool woOrder)
    {
        unsigned long *oa = NULL;
        if (!woOrder && ((SET_VAL_HAS_ORDER(s1) && s1.setWithOrder()->set().t == TP_SET_REC_MULT) || (SET_VAL_HAS_ORDER(s2) && s2.setWithOrder()->set().t == TP_SET_REC_MULT))) {
            unsigned long cs1 = SetBase::cnt(s1);
            unsigned long cs2 = SetBase::cnt(s2);

            if (cs1 * cs2 > 1) {
                unsigned long *oa1 = SetIterator::copyOrder(s1);
                unsigned long *oa2 = SetIterator::copyOrder(s2);

                oa = new unsigned long[cs1 * cs2];

                unsigned long *ob = oa;
                for (unsigned long i = 0; i < cs1; i++) {
                    for (unsigned long j = 0; j < cs2; j++) {
                        *(ob++) = oa1[i] * cs2 + oa2[j];
                    }
                }

                delete oa1;
                delete oa2;
            }
        }

        SetBase *res;
        if (oa == NULL) {
            res = new SetRecMult(s1, s2, woOrder);
            tp = TP_SET_REC_MULT;
        }
        else {
            SetRecMult *srm = new SetRecMult(s1, s2, true);
            CmplVal v(TP_SET_REC_MULT, srm, false);
            res = new SetWithOrder(v, oa);
            tp = TP_SET_WITH_UO;
        }

        return res;
    }

    /**
     * constructor: create set with only one tuple as element
     * @param tup			element for the set
     * @param tps			start index of used tuple part
     * @param toSet			convert scalar index values to sets
     */
    SetRecMult::SetRecMult(const Tuple *tup, unsigned tps, bool toSet)
    {
        _rank = tup->rank() - tps;
        _cnt = 1;
        _markNF = false;
        _isCanonical = false;

        _array = new CmplVal[_rank];
        _cnts = new unsigned long[_rank];
        _subOrder = false;

        CmplVal *s = _array;
        for (unsigned r = 0; r < _rank; r++, s++) {
            CmplVal *p = tup->at(r + tps);
            if (toSet && p->isScalarIndex())
                p->toSet(*s, typeConversionFromSub, NULL);
            else
                s->copyFrom(p, true, false);

            //TODO: wenn tup kein TP_ITUPLE, sondern selbst Set oder Intervall enthaelt

            _cnts[r] = 1;
        }
    }


    /**
     * set _cnt and other instance variables after changing _array
     * @param rnk       also _rank may be changed
     */
    void SetRecMult::setCnt(bool rnk)
    {
        if (rnk) {
            if (_cnts)
                delete[] _cnts;

            _cnts = new unsigned long[_rank];
        }

        _cnt = 1;
        _markNF = false;
        _isCanonical = false;
        _subOrder = false;

        CmplVal *s = _array;
        for (unsigned r = 0; r < _rank; r++, s++) {
            _cnts[r] = SetBase::cnt(*s);
            _cnt *= _cnts[r];

            if (!_subOrder && SetBase::hasUserOrder(*s))
                _subOrder = true;
        }
    }


    /**
     * push all tuple divisible parts in a vector
     * @param dtp       vector to push parts in
     * @param uo        consider user order
     * @return          true if parts are pushed to vector; false if set is not tuple divisible
     */
    bool SetRecMult::partsToVector(vector<CmplValAuto>& dtp, bool uo) const
    {
        CmplVal *s = _array;
        for (unsigned r = 0; r < _rank; r++, s++)
            dtp.push_back(CmplValAuto(s));

        return true;
    }


	/**
	 * write contents of the object to a stream
	 * @param modp			calling module
	 * @param mode			mode for output: 0=direct; 1=part of other value
	 */
	void SetRecMult::write(ostream& ostr, ModuleBase *modp, int mode) const
	{
        if (_markNF)
            ostr << "/set(";

        if (mode == 0 || _markNF)
			ostr << '[';

		for (unsigned r = 0; r < _rank; r++) {
			if (r)
				ostr << ", ";

			_array[r].write(ostr, modp, 1);
		}

        if (mode == 0 || _markNF)
			ostr << ']';
        if (_markNF)
            ostr << ')';
    }


    /**
     * get whether user order is reverse standard order
     * @param oneNeg    return true if set contains no more than one element
     */
    bool SetRecMult::negOrder(bool oneNeg) const
    {
        if (!oneNeg || _cnt > 1) {
            CmplVal *s;
            unsigned r;

            for (r = 0, s = _array; r < _rank; r++, s++) {
                if (!SetBase::negOrder(*s, oneNeg))
                    return false;
            }
        }

        return true;
    }


    /**
     * get set with all user order removed
     * @param res		value for result
     */
    void SetRecMult::stripOrder(CmplVal& res)
    {
        SetRecMult *s;
        if (_subOrder) {
            CmplVal *arr = new CmplVal[_rank];
            CmplVal *q, *d;
            unsigned r;

            for (r = 0, q = _array, d = arr; r < _rank; r++, d++, q++)
                d->copyFrom(SET_VAL_WO_ORDER(*q), true);

            s = new SetRecMult(_rank, arr);
        }
        else {
            s = this;
        }

        res.set(TP_SET_REC_MULT, s);
    }


    /**
     * get one rank 1 part of the set
     * @param i				index of the part
     * @param woOrder		strip user order from result
     * @return				rank 1 part of the set
     */
    CmplVal *SetRecMult::partSet(unsigned i, bool woOrder) const
    {
        CmplVal *r = _array + i;
        return (woOrder && r->setValHasOrder() ? &(r->setWithOrder()->set()) : r);
    }


    /**
     * set part of the set as another set
     * @param res			store for result set
     * @param i				part from this index
     * @param j				part up to this index
     * @param woOrder		strip user order from result
     */
    void SetRecMult::partSet(CmplVal& res, unsigned i, unsigned j, bool woOrder)
    {
        if (j >= _rank)
            j = _rank - 1;

        if (i > j) {
            res.set(TP_SET_NULL);
        }
        else if (i == j) {
            res.copyFrom(woOrder && _subOrder ? SET_VAL_WO_ORDER(_array[i]) : _array[i]);
        }
        else if (i == 0 && j == _rank - 1 && (!woOrder || !_subOrder)) {
            res.set(TP_SET_REC_MULT, this);
        }
        else {
            unsigned rank = j + 1 - i;
            CmplVal *arr = new CmplVal[rank];

            for (unsigned r = 0; r < rank; r++)
                arr[r].copyFrom(woOrder && _subOrder ? SET_VAL_WO_ORDER(_array[i+r]) : _array[i+r]);

            SetRecMult *srm = new SetRecMult(rank, arr);
            res.set(TP_SET_REC_MULT, srm);
        }
    }


	/**
	 * set tuple elements for the tuple on a given index position in the set
	 * @param ind			index number within the set
	 * @param tpl			tuple value for inserting the elements
	 * @param ti			start element number in tpl
     * @param useOrder		use user order of subsets
     * @param reverse		use reverse order
     * @return				index of the tuple within the set
	 */
    unsigned long SetRecMult::tupleAtIntern(unsigned long ind, Tuple *tpl, unsigned ti, bool useOrder, bool reverse)
	{
		if (reverse)
			ind = _cnt - 1 - ind;

		unsigned long tplInd = 0;
		unsigned long c = _cnt;
		unsigned long i1;

		for (unsigned r = 0; r < _rank; r++) {
			c /= _cnts[r];
			i1 = ind / c;			
			ind = ind % c;

			if (_array[r].useValP()) {
				tplInd += c * _array[r].setBase()->tupleAtIntern(i1, tpl, ti+r, useOrder, false);
			}
			else {
				tpl->setRank(ti + r + 1);
                tplInd += c * SetIterator::tupleAtInternSimple(_array[r], i1, tpl->at(ti+r), false);
			}
		}

		return tplInd;
	}


    /**
     * recursive copy of user order to an index array
     * @param nr			number of part set
     * @param ord			array to copy user order to
     * @param ob			start index within <code>ord</code>, return current index
     * @param ib			start of set index to put in <code>ord</code>
     */
    void SetRecMult::copyOrderRec(unsigned nr, unsigned long *ord, unsigned long& ob, unsigned long ib) const
    {
        const CmplVal& s = _array[nr];
        unsigned long cs = _cnts[nr];

        if (nr == _rank - 1) {
            if (cs > 1)
                SetIterator::copyOrder(s, ord, ob, ib, cs);
            else
                ord[ob++] = ib;
        }
        else {
            unsigned long cu = 1;

            for (unsigned i = nr + 1; i < _rank; i++)
                cu *= _cnts[i];

            SetWithOrder *so = (cs > 1 && s.setValHasOrder() ? s.setWithOrder() : NULL);
            if (so && so->negOrder()) {
                ib += ((cs - 1) * cu);
                for (unsigned long i = 0; i < cs; i++, ib -= cu)
                    copyOrderRec(nr + 1, ord, ob, ib);
            }
            else if (so && so->order()) {
                for (unsigned long i = 0; i < cs; i++)
                    copyOrderRec(nr + 1, ord, ob, ib + so->order()[i]*cu);
            }
            else {
                for (unsigned long i = 0; i < cs; i++, ib += cu)
                    copyOrderRec(nr + 1, ord, ob, ib);
            }
        }
    }



	/************** SetFinite **********/

	/**
	 * constructor
	 * @param bs			set for the first dimension(s)
	 * @param arr			per element of <code>bs</code> the set for the remaining dimensions
	 * @param minR			minimal rank
	 * @param maxR			maximal rank
	 */
    SetFinite::SetFinite(CmplVal& bs, CmplVal *arr, unsigned minR, unsigned maxR)
	{
		_base.copyFrom(bs, true, false);
        _array = arr;

        _baseCnt = SetBase::cnt(_base);
        _subOrder = SetBase::hasUserOrder(_base);

        if (!_array) {
            _array = new CmplVal[_baseCnt];
            for (unsigned long i = 0; i < _baseCnt; i++)
                _array[i].unset();
        }

		_index = NULL;
		_indexUseOrder = NULL;

		_minRank = minR;
		_maxRank = maxR;
        _baseRank = SetBase::rank(_base);

        _baseSplit = 0;
        _modArray = 0;

		_cnt = (_minRank == 0 ? 1 : 0);
        _markNF = false;
        _isCanonical = false;

        CmplVal *p = _array;
		for (unsigned long i = 0; i < _baseCnt; i++, p++) {
			if (*p) {
                _cnt += SetBase::cnt(*p);
                if (!_subOrder && p->useValP() && p->setBase()->hasUserOrder())
                    _subOrder = true;
			}
			else {
				_cnt++;
			}
		}
	}

    /**
     * constructor: from another set
     * @param set			set to construct from, act here as base set (must be a finite set, but not the empty set and not the null tuple set)
     * @param woOrder		strip user order
     * @param splt          value for <code>_baseSplit</code> or 0 if unknown
     * @param addNull		add null tuple to resulting finite set
     * @param rmi			min rank for the result set / 0: not specified
     * @param rma			max rank for the result set / 0: not specified
     */
    SetFinite::SetFinite(const CmplVal& set, bool woOrder, unsigned splt, bool addNull, unsigned rmi, unsigned rma)
    {
        if (set.t == TP_SET_FIN) {
            SetFinite *src = set.setFinite();

            if (addNull && src->_minRank == 0)
                addNull = false;

            if (woOrder) {
                SetBase::stripOrder(src->_base, _base);
                _subOrder = false;
            }
            else {
                _base.copyFrom(src->_base, true, false);
                _subOrder = SetBase::hasUserOrder(_base);
            }

            _baseCnt = src->_baseCnt;
            _baseSplit = src->_baseSplit;
            _modArray = src->_modArray;

            _cnt = src->_cnt + (addNull ? 1 : 0);
            _markNF = false;
            _isCanonical = false;

            _array = new CmplVal[_modArray ?: _baseCnt];
            CmplVal *a = _array, *b = src->_array;
            for (unsigned i = 0; i < (_modArray ?: _baseCnt); i++, a++, b++) {
                if (woOrder) {
                    SetBase::stripOrder(*b, *a);
                }
                else {
                    a->copyFrom(b, true, false);
                    if (!_subOrder && SetBase::hasUserOrder(*a))
                        _subOrder = true;
                }
            }

            _baseRank = src->_baseRank;
            _minRank = (addNull ? 0 : (rmi ?: src->_minRank));
            _maxRank = (rma ?: src->_maxRank);
        }

        else {
            if (woOrder) {
                SetBase::stripOrder(set, _base);
                _subOrder = false;
            }
            else {
                _base.copyFrom(set, true, false);
                _subOrder = SetBase::hasUserOrder(_base);
            }

            _baseCnt = SetBase::cnt(_base);
            _cnt = _baseCnt + (addNull ? 1 : 0);
            _markNF = false;
            _isCanonical = false;

            _baseRank = SetBase::rank(_base);
            _minRank = (addNull ? 0 : (rmi ?: _baseRank));
            _maxRank = (rma ?: _baseRank);

            _array = NULL;
            _baseSplit = (_baseRank == 1 || _minRank == 0 ? _baseRank : 0);
            _modArray = 0;
            if (splt && !_baseSplit)
                setBaseSplit(splt, true);

            _array = new CmplVal[_modArray ?: _baseCnt];
            CmplVal *a = _array;
            for (unsigned i = 0; i < (_modArray ?: _baseCnt); i++, a++)
                a->t = TP_EMPTY;
        }

        _index = NULL;
        _indexUseOrder = NULL;
    }

	/**
	 * destructor
	 */
	SetFinite::~SetFinite()
	{
        for (unsigned i = 0; i < (_modArray ?: _baseCnt); i++)
            _array[i].dispose();

        delete[] _array;

		_base.dispose();

        if (_indexUseOrder)
            delete[] _indexUseOrder;

		if (_index)
            delete[] _index;
	}


    /**
     * get whether the tuple set or recursively one of the subsets contains the null tuple
     */
    bool SetFinite::containNullSub() const
    {
        if (_minRank == 0)
            return true;

        CmplVal *ss = _array;
        for (unsigned i = 0; i < (_modArray ?: _baseCnt); i++, ss++) {
            if (ss->t == TP_SET_FIN && ss->setFinite()->containNullSub())
                return true;
        }

        return false;
    }


    /**
     * push all tuple divisible parts in a vector
     * @param dtp       vector to push parts in
     * @param uo        consider user order
     * @return          true if parts are pushed to vector; false if set is not tuple divisible
     */
    bool SetFinite::partsToVector(vector<CmplValAuto>& dtp, bool uo) const
    {
        if (_minRank == 0 || _baseRank == 1 || _baseSplit == 0 || _baseSplit >= _baseRank || (uo && SetBase::hasDirectUserOrder(_base) && SetBase::hasUserOrder(_base)))
            return false;

        unsigned dfr = _baseRank - _baseSplit;

        // _base can only be SetRecMult
        SetRecMult *srm = SET_VAL_WO_ORDER(_base).setRecMult();
        CmplVal *e = srm->partSet(0);
        for (unsigned r = 0; r < dfr; r++, e++)
            dtp.emplace_back(uo ? *e : SET_VAL_WO_ORDER(*e));

        CmplValAuto bs;
        srm->partSet(bs, dfr, _baseRank - 1, !uo);

        unsigned long bsc = SetBase::cnt(bs);
        CmplVal *arr = new CmplVal[bsc];

        CmplVal *es = _array;
        CmplVal *ed = arr;
        for (unsigned long i = 0; i < bsc; i++, es++, ed++)
            ed->copyFrom(uo ? *es : SET_VAL_WO_ORDER(*es));

        SetFinite *sfn = new SetFinite(bs, arr, _minRank - dfr, _maxRank - dfr);
        sfn->_isCanonical = _isCanonical;

        dtp.emplace_back(TP_SET_FIN, sfn);
        return true;
    }

    /**
     * set min rank and max rank for this set
     * @param ss		set rank after changing only this subset / NULL: check all subsets
     * @param pss		previous state of changed subset / NULL: change of <code>ss</code> is only adding additional elements
     */
    void SetFinite::setRank(CmplVal *ss, CmplVal *pss)
    {
        if (ss) {
            unsigned srmi = (*ss ? SetBase::minRank(*ss) : 0);
            unsigned srma = (*ss ? SetBase::maxRank(*ss) : 0);

            if (pss) {
                unsigned psrmi = (*pss ? SetBase::minRank(*pss) : 0);
                unsigned psrma = (*pss ? SetBase::maxRank(*pss) : 0);

                if ((psrmi + _baseRank == _minRank && srmi > psrmi) || (psrma + _baseRank == _maxRank && srma < psrma))
                    ss = NULL;
                else if (_subOrder && SetBase::hasUserOrder(*pss))
                    ss = NULL;
            }

            if (ss) {
                if (_minRank > _baseRank + srmi)
                    _minRank = _baseRank + srmi;
                if (_maxRank < _baseRank + srma)
                    _maxRank = _baseRank + srma;
                if (!_subOrder && SetBase::hasUserOrder(*ss))
                    _subOrder = true;
                return;
            }
        }

        if (!ss) {
            CmplVal *s = _array;
            _baseRank = SetBase::rank(_base);
            _subOrder = SetBase::hasUserOrder(_base);

            if (_baseCnt == 0) {
                _minRank = (_minRank ? _baseRank : 0);
                _maxRank = _baseRank;
            }
            else {
                _minRank = (_minRank ? _baseRank + (*s ? SetBase::minRank(*s) : 0) : 0);
                _maxRank = _baseRank + (*s ? SetBase::maxRank(*s) : 0);
                if (!_subOrder && SetBase::hasUserOrder(*s))
                    _subOrder = true;

                s++;
                for (unsigned long i = 1; i < (_modArray ?: _baseCnt); i++, s++) {
                    unsigned srmi = (*s ? SetBase::minRank(*s) : 0);
                    unsigned srma = (*s ? SetBase::maxRank(*s) : 0);
                    if (_minRank > _baseRank + srmi)
                        _minRank = _baseRank + srmi;
                    if (_maxRank < _baseRank + srma)
                        _maxRank = _baseRank + srma;

                    if (!_subOrder && SetBase::hasUserOrder(*s))
                        _subOrder = true;
                }
            }
        }
    }


    /**
     * set count of elements new for this set, by counting the elements of the subsets
     */
    void SetFinite::setCount()
    {
        if (_indexUseOrder)
            delete _indexUseOrder;

        if (_index)
            delete _index;

        _index = NULL;
        _indexUseOrder = NULL;

        _baseCnt = SetBase::cnt(_base);
        fillIndex();

        _cnt = (_baseCnt ? _index[_baseCnt - 1] : 0) + (_minRank == 0 ? 1 : 0);
    }


	/**
	 * write contents of the object to a stream
	 * @param modp			calling module
	 * @param mode			mode for output: 0=direct; 1=part of other value
	 */
	void SetFinite::write(ostream& ostr, ModuleBase *modp, int mode) const
	{
        if (_markNF)
            ostr << '/';

		ostr << "set(";

        const CmplVal v(TP_SET_FIN, const_cast<SetFinite *>(this), false);		// CmplVal constructor with inc=false doesn't change this, so removing const is save here
		SetIterator iter(v, SetIterator::iteratorTupleVal, true, false);

		iter.begin();
		if (iter) {
			(*iter).write(ostr, modp, 1);

			for (iter++; iter; iter++) {
				ostr << ", ";
				(*iter).write(ostr, modp, 1);
			}
		}

		ostr << ')';
	}


    /**
     * get whether user order is reverse standard order
     * @param oneNeg    return true if set contains no more than one element
     */
    bool SetFinite::negOrder(bool oneNeg) const
    {
        if (!oneNeg || _cnt > 1) {
            if (_minRank == 0)
                return false;

            if ((_baseCnt > 1 || !oneNeg) && !SetBase::negOrder(_base, false))
                return false;

            CmplVal *s;
            unsigned long i;

            for (i = 0, s = _array; i < (_modArray ?: _baseCnt); i++, s++) {
                if (!SetBase::negOrder(*s, oneNeg))
                    return false;
            }
        }

        return true;
    }


    /**
     * get set with all user order removed
     * @param res		value for result
     */
    void SetFinite::stripOrder(CmplVal& res)
    {
        SetFinite *s;
        if (_subOrder) {
            CmplVal bs;
            SetBase::stripOrder(_base, bs);

            CmplVal *arr = new CmplVal[_modArray ?: _baseCnt];
            CmplVal *q, *d;
            unsigned long i;

            for (i = 0, q = _array, d = arr; i < (_modArray ?: _baseCnt); i++, d++, q++)
                SetBase::stripOrder(*q, *d);

            s = new SetFinite(bs, arr, _minRank, _maxRank);
            s->_modArray = _modArray;
            bs.dispose();
        }
        else {
            s = this;
        }

        res.set(TP_SET_FIN, s);
    }

    /**
     * get base set
     * @param woOrder		strip user order from result
     */
    const CmplVal& SetFinite::baseSet(bool woOrder) const
    {
        return (woOrder ? SET_VAL_WO_ORDER(_base) : _base);
    }

    /**
     * get one subset of the set
     * @param i				index of the subset
     * @param woOrder		strip user order from result
     * @return				subset
     */
    CmplVal& SetFinite::subSet(unsigned long i, bool woOrder) const
    {
        if (_modArray)
            i %= _modArray;

        return (woOrder ? SET_VAL_WO_ORDER(_array[i]) : _array[i]);
    }


    /**
     * set <code>_baseSplit</code> and <code>_modArray</code>
     * @param splt          value for <code>_baseSplit</code>
     * @param modarr        set also <code>_modArray</code>
     */
    void SetFinite::setBaseSplit(unsigned splt, bool modarr)
    {
        _baseSplit = splt;

        if (modarr && splt && splt < _baseRank) {
            // base set can only be SetRecMult
            SetRecMult *srm = baseSet(true).setRecMult();

            unsigned long md = 1;
            for (unsigned r = 1; r <= splt; r++)
                md *= srm->partCnt(_baseRank - r);

            if (_modArray == 0 || md < _modArray) {
                if (_array) {
                    CmplVal *arr = new CmplVal[md];
                    for (unsigned long i = 0; i < md; i++)
                        arr[i].moveFrom(_array[i]);

                    for (unsigned long i = 0; i < (_modArray ?: _baseCnt); i++)
                        _array[i].dispose();

                    delete[] _array;

                    _array = arr;
                }

                _modArray = md;
            }
        }
    }


	/**
	 * set tuple elements for the tuple on a given index position in the set
	 * @param ind			index number within the set
	 * @param tpl			tuple value for inserting the elements
	 * @param ti			start element number in tpl
     * @param useOrder		use user order of subsets
     * @param reverse		use reverse order
     * @return				index of the tuple within the set
	 */
    unsigned long SetFinite::tupleAtIntern(unsigned long ind, Tuple *tpl, unsigned ti, bool useOrder, bool reverse)
	{
		if (reverse)
			ind = _cnt - 1 - ind;

		unsigned long tplIndBase = 0;
		if (_minRank == 0) {
			if (ind == 0) {
				tpl->setRank(ti);
				return 0;
			}

			ind--;
			tplIndBase = 1;
		}

        unsigned long *index = fillIndex();
        unsigned long *indexArr = (useOrder ? fillIndexUseOrder() : index);

        unsigned long i1 = SetIterator::searchArrayPos<unsigned long>(indexArr, _baseCnt, ind);
		unsigned long i2 = (i1==0 ? ind : (ind - indexArr[i1-1]));

		unsigned long ti1;
		if (_base.useValP()) {
            ti1 = _base.setBase()->tupleAtIntern(i1, tpl, ti, useOrder, false);
		}
		else {
			tpl->setRank(ti + 1);
            ti1 = SetIterator::tupleAtInternSimple(_base, i1, tpl->at(ti), false);
		}

		unsigned long ti2 = 0;
        CmplVal* sub = subSet(ti1);
        if (*sub) {
            if (sub->useValP()) {
                ti2 = sub->setBase()->tupleAtIntern(i2, tpl, ti + _baseRank, useOrder, false);
			}
			else {
				tpl->setRank(ti + _baseRank + 1);
                ti2 = SetIterator::tupleAtInternSimple(*sub, i2, tpl->at(ti + _baseRank), false);
			}
		}

        return tplIndBase + (ti1==0 ? 0 : index[ti1-1]) + ti2;
	}

	/**
	 * fills <code>_index</code>
	 */
    unsigned long *SetFinite::fillIndex()
	{
        if (!_index) {
            _index = new unsigned long[_baseCnt];

            CmplVal *vp = _array;
            _index[0] = SetBase::cnt(*vp);

            vp++;
            for (unsigned long i = 1; i < _baseCnt; i++, vp++) {
                if (_modArray && i % _modArray == 0)
                    vp = _array;

                _index[i] = _index[i - 1] + SetBase::cnt(*vp);
            }
        }

        return _index;
	}

	/**
	 * fills <code>_indexUseOrder</code>
	 */
    unsigned long *SetFinite::fillIndexUseOrder()
	{
        if (!_indexUseOrder && SetBase::hasUserOrder(_base)) {
            unsigned long *ord = NULL;
            bool freeOrd = false;

            if (_base.t != TP_SET_WITH_UO) {
                ord = SetIterator::copyOrder(_base);
                freeOrd = true;
            }
            else {
                SetWithOrder *so = _base.setWithOrder();
                if (so->negOrder()) {
                    _indexUseOrder = new unsigned long[_baseCnt];
                    for (unsigned long i = 0; i < _baseCnt; i++)
                        _indexUseOrder[i] = (i==0 ? 0 : _indexUseOrder[i - 1]) + SetBase::cnt(*subSet(_baseCnt - 1 - i));
                }
                else if (so->order()) {
                    ord = so->order();
                }
            }

            if (ord) {
                _indexUseOrder = new unsigned long[_baseCnt];
                for (unsigned long i = 0; i < _baseCnt; i++)
                    _indexUseOrder[i] = (i==0 ? 0 : _indexUseOrder[i - 1]) + SetBase::cnt(*subSet(ord[i]));
            }

            if (freeOrd)
                delete ord;
		}

        return _indexUseOrder ?: fillIndex();
	}



    /************** SetWithOrder **********/

    /**
     * write contents of the object to a stream
     * @param modp			calling module
     * @param mode			mode for output: 0=direct; 1=part of other value
     */
    void SetWithOrder::write(ostream& ostr, ModuleBase *modp, int mode) const
    {
        if (_negOrder && (_set.useInt() || _set.t == TP_SET_R1_ALG)) {
            intType start, step, end;
            bool mnf;
            if (_set.useInt()) {
                start = _set.v.i;
                step = -1;
                end = (_set.t == TP_SET_R1_0_UB || _set.t == TP_SET_R1_0_UB_MNF ? 0 : 1);
                mnf = (_set.t == TP_SET_R1_0_UB_MNF || _set.t == TP_SET_R1_1_UB_MNF);
            }
            else {
                SetAlg *s = _set.setAlg();
                start = (s->start() + s->step() * (s->cnt() - 1));
                step = -(s->step());
                end = s->start();
                mnf = s->markNF();
            }

            if (mode == 0)
                ostr << '[';
            if (mnf)
                ostr << "/set(";
            ostr << start << '(' << step << ')' << end;
            if (mnf)
                ostr << ')';
            if (mode == 0)
                ostr << ']';
        }
        else {
            if (SetBase::markNF(_set))
                ostr << '/';
            ostr << "set(";

            const CmplVal v(TP_SET_WITH_UO, const_cast<SetWithOrder *>(this), false);		// CmplVal constructor with inc=false doesn't change this, so removing const is save here
            SetIterator iter(v, SetIterator::iteratorTupleSimple, true, false);

            iter.begin();
            if (iter) {
                (*iter).write(ostr, modp, 1);

                for (iter++; iter; iter++) {
                    ostr << ", ";
                    (*iter).write(ostr, modp, 1);
                }
            }

            ostr << ')';
        }
    }


    /**
     * check whether the specified order is equal to another set order
     * @param s2			other set
     * @return				true if both sets are equal
     */
    bool SetWithOrder::equalOrder(const SetWithOrder *s2) const
    {
        unsigned long c1 = SetBase::cnt(_set);
        unsigned long c2 = SetBase::cnt(s2->_set);

        if (c1 != c2)
            return false;

        if (_negOrder || s2->_negOrder)
            return (_negOrder && s2->_negOrder);

        if (_order || s2->_order) {
            if (_order && s2->_order)
                return (memcmp(_order, s2->_order, c1 * sizeof(unsigned long)) == 0);
            else
                return false;
        }

        return true;
    }


    /**
     * set tuple elements for the tuple on a given index position in the set
     * @param ind			index number within the set
     * @param tpl			tuple value for inserting the elements
     * @param ti			start element number in tpl
     * @param useOrder		use user order of subsets
     * @param reverse		use reverse order
     * @return				index of the tuple within the set
     */
    unsigned long SetWithOrder::tupleAtIntern(unsigned long ind, Tuple *tpl, unsigned ti, bool useOrder, bool reverse)
    {
        if (useOrder) {
            if (_negOrder) {
                reverse = !reverse;
            }
            else if (_order) {
                ind = _order[!reverse ? ind : (cnt() - 1 - ind)];
                reverse = false;
            }
        }

        if (_set.useValP()) {
            return _set.setBase()->tupleAtIntern(ind, tpl, ti, false, reverse);
        }
        else {
            tpl->setRank(ti + 1);
            return SetIterator::tupleAtInternSimple(_set, ind, tpl->at(ti), reverse);
        }
    }


    /**
     * allocate new index array for user order
     * @param init			initialize array with standard order (if false, then the caller must fill the array before the set can be used!)
     * @return				index array
     */
    unsigned long *SetWithOrder::newOrder(bool init)
    {
        setNegOrder(false);

        unsigned long cnt = SetBase::cnt(_set);
        _order = new unsigned long[cnt];

        if (init) {
            unsigned long *o = _order;
            for (unsigned long i = 0; i < cnt; i++, o++)
                *o = i;
        }

        return _order;
    }

}

