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


#include "SetIterator.hh"
#include "../Control/MainControl.hh"


using namespace std;


namespace cmpl
{

	/************** SetIterator **********/

	/**
     * initialize set iterator
	 * @param set			set to iterate (must be of any finite set type)
	 * @param rtm			return mode for tuples
	 * @param useOrder		iterate in user order
	 * @param reverse		iterate in reverse order
	 */
    void SetIterator::initIntern(const CmplVal& set, SetIterator::ReturnMode rtm, bool useOrder, bool reverse)
	{
        if (!set.isSet() || !SetBase::finite(set))
			throw invalid_argument("no finite set for SetIterator");

        _set.copyFrom(set, true, false);
        _rtm = rtm;
        _useOrder = useOrder;
        _reverse = reverse;

        _cnt = SetBase::cnt(_set);
		_start = true;
		_ind = 0;
		_tplInd = 0;

		_cur.unset();
		_curTpl = NULL;
		_curNull = false;

		_sub = NULL;
		_parent = NULL;
		_tupInd = 0;

		_multInd = NULL;
		_tplMultInd = NULL;
        _directOrder = NULL;

        if (_set.t == TP_SET_WITH_UO) {
            SetWithOrder *so = _set.setWithOrder();
            if (_useOrder) {
                if (so->negOrder())
                    _reverse = !_reverse;
                else if (so->order())
                    _directOrder = so->order();
            }

            _useOrder = false;
            _set.copyFrom(so->set(), true, true);
        }
        else if (_useOrder && (!_set.useValP() || !_set.setBase()->hasUserOrder())) {
            _useOrder = false;
        }

		if (_set.t == TP_SET_REC_MULT) {
			_multInd = new unsigned long[_set.setRecMult()->_rank];
			if (_useOrder)
				_tplMultInd = new unsigned long[_set.setRecMult()->_rank];
		}
		else if (_set.t == TP_SET_FIN && _set.setFinite()->_base.t == TP_SET_REC_MULT) {
			_multInd = new unsigned long[_set.setFinite()->_baseRank];
			if (_useOrder)
				_tplMultInd = new unsigned long[_set.setFinite()->_baseRank];
		}
	}


    /**
     * dispose set iterator
     */
    void SetIterator::dispose()
    {
        _set.dispose();

        if (_sub) {
			delete _sub;
            _sub = NULL;
        }

        if (_multInd) {
			delete _multInd;
            _multInd = NULL;
        }

        if (_tplMultInd) {
			delete _tplMultInd;
            _tplMultInd = NULL;
        }

        if (!_parent && _curTpl && _curTpl->decRef()) {
			delete _curTpl;
            _curTpl = NULL;
        }
	}


	/**
	 * get tuple at a given index position in a set
     * @param res           return of result tuple (if set is a simple rank 1 set, then as scalar value instead of tuple) / TP_EMPTY: ind is out of range
     * @param set			set (must be of any finite set type)
	 * @param ind			index number of tuple
	 * @param rtm			return mode for tuples
	 * @param useOrder		use user order
	 * @param reverse		use reverse order
	 * @return				tuple (if set is a simple rank 1 set, then as scalar value instead of tuple) / TP_EMPTY: ind is out of range
	 */
    void SetIterator::at(CmplVal& res, const CmplVal& set, unsigned long ind, SetIterator::ReturnMode rtm, bool useOrder, bool reverse)
	{
        const CmplVal& setv = SET_VAL_WO_ORDER(set);
        unsigned long cnt = SetBase::cnt(setv);
		unsigned long tplInd;

        if (ind >= cnt) {
            res.unset();
            return;
        }

        bool useSubOrder = false;
        if (useOrder) {
            if (set.setValHasOrder()) {
                SetWithOrder *so = set.setWithOrder();
                if (so->negOrder())
                    ind = cnt - 1 - ind;
                else if (so->order())
                    ind = so->order()[ind];
            }
            else if (setv.useValP() && setv.setBase()->hasSubUserOrder()) {
                useSubOrder = true;
            }
        }

        switch (setv.t) {
            case TP_SET_FIN:
                if (rtm == iteratorTupleSimpleWithSet || rtm == iteratorTupleValWithSet || rtm == iteratorTupleObjWithSet)
                    res.set(TP_ITUPLE, new Tuple(setv, 0, setv.setFinite()->_maxRank));
                else
                    res.set(TP_ITUPLE, new Tuple(0, setv.setFinite()->_maxRank, Tuple::tupleIndexOnly));

                tplInd = setv.setFinite()->tupleAtIntern(ind, res.tuple(), 0, useSubOrder, reverse);
                if (rtm != iteratorTupleObjWithSet)
                    Tuple::toSpecialValTuple(res, true);
                else if (rtm == iteratorTupleSimpleWithSet || rtm == iteratorTupleValWithSet || rtm == iteratorTupleObjWithSet)
                    res.tuple()->setIterInd(tplInd);

                break;

            case TP_SET_REC_MULT:
                if (rtm == iteratorTupleSimpleWithSet || rtm == iteratorTupleValWithSet || rtm == iteratorTupleObjWithSet)
                    res.set(TP_ITUPLE, new Tuple(setv, 0, setv.setRecMult()->_rank));
                else
                    res.set(TP_ITUPLE, new Tuple(0, setv.setRecMult()->_rank, Tuple::tupleIndexOnly));

                tplInd = setv.setRecMult()->tupleAtIntern(ind, res.tuple(), 0, useSubOrder, reverse);
                if (rtm != iteratorTupleObjWithSet)
                    Tuple::toSpecialValTuple(res, true);
                else if (rtm == iteratorTupleSimpleWithSet || rtm == iteratorTupleValWithSet || rtm == iteratorTupleObjWithSet)
                    res.tuple()->setIterInd(tplInd);

                break;

            case TP_SET_NULL:
                if (rtm == iteratorTupleObjWithSet)
                    res.set(TP_ITUPLE, new Tuple(setv, 0, 0));
                else
                    res.set(TP_ITUPLE_NULL);
                break;

            default:
                if (rtm == iteratorTupleObjWithSet) {
                    res.set(TP_ITUPLE, new Tuple(setv, 1, 1));
                    res.tuple()->setIterInd(tupleAtInternSimple(setv, ind, res.tuple()->at(0), reverse));
                }
                else {
                    tupleAtInternSimple(setv, ind, &res, reverse);
                    if (rtm != iteratorTupleSimple && rtm != iteratorTupleSimpleWithSet)
                        res.t = (res.t == TP_INT ? TP_ITUPLE_1INT : TP_ITUPLE_1STR);
                }
                break;
		}
	}



    /**
     * check whether a scalar index value is an element of a set
     * @param set			set (must not be TP_SET_INF_TPL or TP_SET_INF_SET)
     * @param ind 			index value, must be int or string
     * @return 				true if ind is an element in set
     */
    bool SetIterator::isScalarIn(const CmplVal& set, const CmplVal& ind)
    {
        const CmplVal& setv = SET_VAL_WO_ORDER(set);

        if (setv.isSetFin() || setv.isScalarIndex()) {
            unsigned long n;
            return isScalarIn(setv, ind, n);
        }

        if (ind.isScalarIndex()) {
            switch (setv.t) {
                case TP_SET_R1_LB_INF:
                case TP_SET_R1_LB_INF_MNF:
                    return (ind.isScalarInt() && ind.v.i >= setv.v.i);

                case TP_SET_R1_INF_UB:
                case TP_SET_R1_INF_UB_MNF:
                    return (ind.isScalarInt() && ind.v.i <= setv.v.i);

                case TP_SET_R1_IINF:
                case TP_SET_R1_IINF_MNF:
                    return (ind.isScalarInt());

                case TP_SET_R1A:
                case TP_SET_R1A_MNF:
                    return true;

                case TP_SET_R1A_INT:
                case TP_SET_R1A_INT_MNF:
                    return (ind.isScalarInt() && ind.v.i >= 0);

                case TP_SET_R1A_STR:
                case TP_SET_R1A_STR_MNF:
                    return (ind.t == TP_STR);

                case TP_SET_ALL:
                case TP_SET_ALL_MNF:
                    return true;

                case TP_SET_ALL_INT:
                case TP_SET_ALL_INT_MNF:
                    return (ind.isScalarInt() && ind.v.i >= 0);

                case TP_SET_ALL_STR:
                case TP_SET_ALL_STR_MNF:
                    return (ind.t == TP_STR);

                case TP_SET_INF_TPL:
                case TP_SET_INF_SET:
                    // cannot handled here
                    throw invalid_argument("no suitable infinite set in SetIterator::isScalarIn()");

                default:
                    if (!setv.isSetInf())
                        throw invalid_argument("no suitable set in SetIterator::isScalarIn()");
            }
        }
        else {
            throw invalid_argument("no suitable index value in SetIterator::isScalarIn()");
        }

        return false;
    }


    /**
     * check whether a scalar index value is an element of a set, and returns the index number of the element in the set if finite
     * @param set			set
     * @param ind			index value, must be int or string
     * @param num			return of the index number of ind within set (only if set is a finite set)
     * @return 				true if ind is an element in set
     */
    bool SetIterator::isScalarIn(const CmplVal& set, const CmplVal& ind, unsigned long& num)
    {
        const CmplVal& setv = SET_VAL_WO_ORDER(set);

        if (setv.isSetInf()) {
            return isScalarIn(setv, ind);
        }

        if (ind.isScalarIndex())
        {
            intType i = ind.v.i;
            switch (setv.t) {
                case TP_SET_R1_ALG:
                    i -= setv.setAlg()->start();
                    if (i >= 0 && ind.isScalarInt()) {
                        unsigned long stp = setv.setAlg()->step();
                        if (stp == 1) {
                            if (i < (intType)(setv.setAlg()->cnt())) {
                                num = i;
                                return true;
                            }
                        }
                        else {
                            unsigned long end = stp * (setv.setAlg()->cnt() - 1);
                            if (i <= (intType)end && i % stp == 0) {
                                num = i / stp;
                                return true;
                            }
                        }
                    }
                    break;

                case TP_SET_R1_ENUM:
                    if (ind.isScalarInt()) {
                        if (setv.setEnum()->_cntInt > 0) {
                            unsigned long p = searchArrayPos<intType>(setv.setEnum()->_array, setv.setEnum()->_cntInt, i);
                            if (p > 0 && p <= setv.setEnum()->_cntInt && setv.setEnum()->_array[p-1] == i) {
                                num = p-1;
                                return true;
                            }
                        }
                    }
                    else if (ind.t == TP_STR) {
                        unsigned long cntStr = setv.setEnum()->cnt() - setv.setEnum()->_cntInt;
                        if (cntStr > 0) {
                            intType *arr = setv.setEnum()->_array + setv.setEnum()->_cntInt;
                            unsigned long p = searchArrayPos<intType>(arr, cntStr, i);
                            if (p > 0 && p <= cntStr && arr[p-1] == i) {
                                num = p-1 + setv.setEnum()->_cntInt;
                                return true;
                            }
                        }
                    }
                    break;

                case TP_SET_REC_MULT:
                    if (setv.setRecMult()->maxRank() == 1) {
                        return isScalarIn(setv.setRecMult()->partSet(0), ind, num);
                    }
                    break;

                case TP_SET_FIN:
                    if (setv.setFinite()->minRank() <= 1 && setv.setFinite()->maxRank() >= 1 && setv.setFinite()->_baseRank == 1) {
                        unsigned long nb;
                        if (isScalarIn(setv.setFinite()->_base, ind, nb)) {
                            const CmplVal *ss = setv.setFinite()->subSet(nb);
                            if (ss->t == TP_EMPTY || SetBase::minRank(*ss) == 0) {
                                num = setv.setFinite()->indSubStart(nb);
                                return true;
                            }
                        }
                    }
                    break;

                case TP_SET_R1_0_UB:
                case TP_SET_R1_0_UB_MNF:
                    if (i >= 0 && i <= setv.v.i && ind.isScalarInt()) {
                        num = i;
                        return true;
                    }
                    break;

                case TP_SET_R1_1_UB:
                case TP_SET_R1_1_UB_MNF:
                    if (i >= 1 && i <= setv.v.i && ind.isScalarInt()) {
                        num = i - 1;
                        return true;
                    }
                    break;

                case TP_BIN:
                case TP_INT:
                case TP_SET_1INT:
                    if (setv.v.i == i && ind.isScalarInt()) {
                        num = 0;
                        return true;
                    }
                    break;

                case TP_STR:
                case TP_SET_1STR:
                    if (setv.v.i == i && ind.t == TP_STR) {
                        num = 0;
                        return true;
                    }
                    break;

                default:
                    if (!setv.isSetFin())
                        throw invalid_argument("no suitable finite set in SetIterator::isScalarIn()");
            }
        }
        else
        {
            throw invalid_argument("no suitable index value in SetIterator::isScalarIn()");
        }

        return false;
    }


    /**
     * copy user order of a set to an index array
     * @param set			source set (must be a finite set)
     * @param add			allocate additional elements more than length of <code>set</code>
     * @param initAdd		initialize additional elements
     * @param ins           only used if initAdd: insert additional elements for this tuple position
     * @return 				new array with copied user order
     */
    unsigned long *SetIterator::copyOrder(const CmplVal &set, unsigned long add, bool initAdd, unsigned long ins)
    {
        unsigned long cs = SetBase::cnt(set);
        unsigned long *ord = new unsigned long[cs + add];

        unsigned long b = 0;
        copyOrder(set, ord, b, (initAdd && ins==0 ? add : 0));

        if (add && initAdd) {
            unsigned long i, ob, ib;

            if (ins && ins < cs) {
                for (i = 0; i < cs; i++) {
                    if (ord[i] >= ins)
                        ord[i] += add;
                }
                ib = ins;
            }
            else {
                ib = (ins==0 ? 0 : cs);
            }

            for (i = 0, ob = cs; i < add; i++, ob++, ib++)
                ord[ob] = ib;
        }

        return ord;
    }

    /**
     * copy user order of a set to an index array
     * @param set			source set (must be a finite set)
     * @param ord			array to copy user order to
     * @param ob			start index within <code>ord</code>, return current index
     * @param ib			start of set index to put in <code>ord</code>
     * @param cs			length of source set / 0: not known
     */
    void SetIterator::copyOrder(const CmplVal &set, unsigned long *ord, unsigned long& ob, unsigned long ib, unsigned long cs)
    {
        unsigned long i;

        if (!cs)
            cs = SetBase::cnt(set);

        if (set.setValHasOrder()) {
            SetWithOrder *so = set.setWithOrder();

            if (so->negOrder()) {
                for (i = 0; i < cs; i++)
                    ord[ob++] = ib + cs - i - 1;
            }
            else if (so->order()) {
                memcpy(ord + ob, so->order(), sizeof(unsigned long) * cs);
                if (ib) {
                    for (i = 0; i < cs; i++)
                        ord[ob++] += ib;
                }
                else {
                    ob += cs;
                }
            }
            else {
                for (i = 0; i < cs; i++)
                    ord[ob++] = ib++;
            }
        }
        else {
            if ((set.t == TP_SET_REC_MULT || set.t == TP_SET_FIN) && set.setBase()->hasSubUserOrder()) {
                if (set.t == TP_SET_REC_MULT) {
                    set.setRecMult()->copyOrderRec(0, ord, ob, ib);
                }
                else {
                    SetFinite *sf = set.setFinite();

                    unsigned long *ordb = NULL;
                    if (SetBase::hasUserOrder(sf->_base)) {
                        ordb = new unsigned long[sf->_baseCnt];
                        i = 0;
                        copyOrder(sf->_base, ordb, i, 0, sf->_baseCnt);
                    }

                    if (sf->_minRank == 0)
                        ord[ob++] = ib;

                    for (i = 0; i < sf->_baseCnt; i++) {
                        CmplVal& ss = *(sf->subSet(ordb ? ordb[i] : i));
                        unsigned long ibs = ib + sf->indSubStart(ordb ? ordb[i] : i);

                        if (ss)
                            copyOrder(ss, ord, ob, ibs);
                        else
                            ord[ob++] = ibs;
                    }

                    if (ordb)
                        delete[] ordb;
                }
            }
            else {
                for (i = 0; i < cs; i++)
                    ord[ob++] = ib++;
            }
        }

        return;
    }


    /**
	 * set tuple element for the tuple on a given index position in a set
	 * @param set			set (must be of any finite set type with rank 1) (must not be TP_SET_FIN or TP_SET_REC_MULT)
	 * @param ind			index number within the set
	 * @param elem			tuple element to insert value
	 * @param reverse		use reverse order
	 * @return				index of the tuple within the set
	 */
    unsigned long SetIterator::tupleAtInternSimple(const CmplVal& set, unsigned long ind, CmplVal *elem, bool reverse)
	{
		unsigned long tplInd;
        const CmplVal& setv = SET_VAL_WO_ORDER(set);

        switch (setv.t) {
			case TP_SET_R1_ENUM:
				{
                    SetEnum *setEnum = setv.setEnum();
					tplInd = (!reverse ? ind : (setEnum->cnt() - ind - 1));
					elem->set((tplInd < setEnum->_cntInt ? TP_INT : TP_STR), setEnum->_array[tplInd]);
				}
				break;

			case TP_SET_R1_ALG:
				{
                    SetAlg *setAlg = setv.setAlg();
                    tplInd = (!reverse ? ind : (setAlg->cnt() - 1 - ind));
					elem->set(TP_INT, (intType)(setAlg->_start + tplInd * setAlg->_step));
				}
				break;

			case TP_SET_R1_0_UB:
			case TP_SET_R1_0_UB_MNF:
                tplInd = (reverse ? setv.v.i - ind : ind);
				elem->set(TP_INT, (intType)tplInd);
				break;

			case TP_SET_R1_1_UB:
			case TP_SET_R1_1_UB_MNF:
                tplInd = (reverse ? setv.v.i - 1 - ind : ind);
				elem->set(TP_INT, (intType)(tplInd + 1));
				break;

			case TP_SET_1INT:
			case TP_SET_1STR:
				tplInd = 0;
                elem->set((setv.t==TP_SET_1INT ? TP_INT : TP_STR), setv.v.i);
				break;

            case TP_INT:
            case TP_STR:
                tplInd = 0;
                elem->set(setv.t, setv.v.i);
                break;

			default:		// no finite set
				throw invalid_argument("no suitable finite set in SetIterator::tupleAtInternSimple()");
		}

		return tplInd;
	}


	/**
	 * iterate to the first or next tuple, only called for base iterator
	 * @param start			start or restart the iteration
	 */
	void SetIterator::iterIntern(bool start)
	{
		_start = start;
        if (_start)
            _ind = 0;
        else
            _ind++;

        if (ended()) {
            _ind = _tplInd = _cnt;
            _cur.unset();
            return;
        }

        _tplInd = (_reverse ? _cnt - 1 - _ind : _ind);
        if (_directOrder)
            _tplInd = _directOrder[_tplInd];

		tp_e tpVal1Int, tpVal1Str;
		if (_rtm == iteratorTupleObjWithSet) {
			tpVal1Int = TP_ITUPLE;
			tpVal1Str = TP_ITUPLE;
		}
		else if (_set.t != TP_SET_FIN && _set.t != TP_SET_REC_MULT && (_rtm == iteratorTupleSimple || _rtm == iteratorTupleSimpleWithSet)) {
			tpVal1Int = TP_INT;
			tpVal1Str = TP_STR;
		}
		else {
			tpVal1Int = TP_ITUPLE_1INT;
			tpVal1Str = TP_ITUPLE_1STR;
		}

		if (_start && (_set.t == TP_SET_FIN || _set.t == TP_SET_REC_MULT || _rtm == iteratorTupleObjWithSet)) {
			checkSetNewTuple(false);
			if (_set.t != TP_SET_FIN)
                _curTpl->setRank(SetBase::rank(_set));
		}

		switch (_set.t) {
			case TP_SET_FIN:
			case TP_SET_REC_MULT:
				{
					if (_start) {
						iterInternStart(true, false);
					}
					else {
						bool ntp = checkSetNewTuple(false);
						iterInternNext(!ntp);
						if (ntp)
							setTupleValsAll(true);
					}

					if (_rtm == iteratorTupleSimpleWithSet || _rtm == iteratorTupleValWithSet || _rtm == iteratorTupleObjWithSet)
						_curTpl->setIterInd(_tplInd);
					if (_rtm != iteratorTupleObjWithSet)
						Tuple::toSpecialValTuple(_cur, false);
				}
				break;

			case TP_SET_R1_ENUM:
				{
					SetEnum *setEnum = _set.setEnum();
					if (_rtm == iteratorTupleObjWithSet) {
						if (!_start)
							checkSetNewTuple(false);

						_curTpl->at(0)->set((_tplInd < setEnum->_cntInt ? TP_INT : TP_STR), setEnum->_array[_tplInd]);
						_curTpl->setIterInd(_tplInd);
					}
					else {
						_cur.set((_tplInd < setEnum->_cntInt ? tpVal1Int : tpVal1Str), setEnum->_array[_tplInd]);
					}
				}
				break;

			case TP_SET_R1_ALG:
				{
					SetAlg *setAlg = _set.setAlg();
					if (_rtm == iteratorTupleObjWithSet) {
						if (!_start)
							checkSetNewTuple(false);

						_curTpl->at(0)->set(TP_INT, (intType)(setAlg->_start + _tplInd * setAlg->_step));
						_curTpl->setIterInd(_tplInd);
					}
					else {
                        if (_start)
                            _cur.set(tpVal1Int);
                        _cur.v.i = (intType)(setAlg->_start + _tplInd * setAlg->_step);
					}
				}
				break;

			case TP_SET_R1_0_UB:
			case TP_SET_R1_0_UB_MNF:
			case TP_SET_R1_1_UB:
			case TP_SET_R1_1_UB_MNF:
				if (_rtm == iteratorTupleObjWithSet) {
					if (!_start)
						checkSetNewTuple(false);

					_curTpl->at(0)->set(TP_INT, (intType)((_set.t==TP_SET_R1_1_UB || _set.t==TP_SET_R1_1_UB_MNF ? 1 : 0) + _tplInd));
					_curTpl->setIterInd(_tplInd);
				}
				else
				{
                    if (_start)
                        _cur.set(tpVal1Int, (intType)((_set.t==TP_SET_R1_1_UB || _set.t==TP_SET_R1_1_UB_MNF ? 1 : 0) + _tplInd - 1));

                    if (_directOrder)
                        _cur.v.i = (intType)((_set.t==TP_SET_R1_1_UB || _set.t==TP_SET_R1_1_UB_MNF ? 1 : 0) + _tplInd);
                    else if (!_reverse)
                        _cur.v.i++;
                    else
                        _cur.v.i--;
                }
				break;

			case TP_SET_NULL:
				if (_rtm != iteratorTupleObjWithSet)
					_cur.set(TP_ITUPLE_NULL);
				break;

			case TP_SET_1INT:
			case TP_SET_1STR:
				if (_rtm != iteratorTupleObjWithSet)
					_cur.set((_set.t==TP_SET_1INT ? tpVal1Int : tpVal1Str), _set.v.i);
				else
					_curTpl->at(0)->set((_set.t==TP_SET_1INT ? TP_INT : TP_STR), _set.v.i);
				break;

			default:		// no finite set
				throw invalid_argument("no finite set in SetIterator::iterIntern()");
		}

		_start = false;
	}


	/**
	 * set a new tuple value as current tuple if neccessary, only called for base iterator
	 * @param sv		set all tuple values if a new tuple object
	 * @return			current tuple is now a new tuple object
	 */
	bool SetIterator::checkSetNewTuple(bool sv)
	{
		if (_cur.useValP() && _curTpl->refCnt() == 1)
			return false;

		bool ntp = false;
		if (_cur.useValP()) {
			_curTpl->decRef();
			ntp = true;
		}
		else if (!_curTpl)
			ntp = true;

		if (ntp) {
			if (_rtm == iteratorTupleSimpleWithSet || _rtm == iteratorTupleValWithSet || _rtm == iteratorTupleObjWithSet)
                _curTpl = new Tuple(_set, 0, SetBase::rank(_set));
			else
                _curTpl = new Tuple(0, SetBase::rank(_set), Tuple::tupleIndexOnly);

			SetIterator *sb = _sub;
			while (sb) {
				sb->_curTpl = _curTpl;
				sb = sb->_sub;
			}
		}

		_cur.set(TP_ITUPLE, _curTpl);

		if (sv)
			setTupleValsAll(true);

		return ntp;
	}


    /**
	 * set all tuple element values
	 * @param sub		also set tuple values for subiterator
	 */
	void SetIterator::setTupleValsAll(bool sub)
	{
        if (_directOrder || _useOrder) {
            //TODO: wenn !_directOrder dann doch wegen Performance nicht ueber tupleAtIntern
            //		stattdessen folgende Faelle unterscheiden (hier bzw. in den weiteren Funktionen):
            //			_set.t == TP_SET_FIN && _set.setFinite()->_base hat keine user order:
            //					hier gar keine Sonderbehandlung, user order wirkt sich dann nur in rekursiv untergeordneten Aufrufen aus
            //			_set.t == TP_SET_FIN && _set.setFinite()->_base hat direkte user order:
            //					_base entsprechend der direkten user order iterieren
            //					Verwendung von SetFinite::_indexUseOrder dafuer
            //			sonst:
            //					in _set (bzw. bei TP_SET_FIN _base) einzelne Teilsets einzeln iterieren, jeweils dabei gegebenenfalls Reihenfolge beachten
            //					Verwendung von this->_tplMultInd dafuer
            _tplInd = _set.setBase()->tupleAtIntern((_directOrder ? _directOrder[_ind] : _ind), _curTpl, _tupInd, _useOrder, _reverse);
		}
		else {
            if (_set.t == TP_SET_FIN && _curNull) {
				_tplInd = 0;
				_curTpl->setRank(_tupInd);
			}
			else {
                CmplVal& vsa = (_set.t == TP_SET_FIN ? _set.setFinite()->_base : _set);
                CmplVal& vs = SET_VAL_WO_ORDER(vsa);
                unsigned long tplInd = _indBase; // (_useOrder ? 0 : _indBase);

				if (vs.t == TP_SET_REC_MULT) {
					unsigned rank = vs.setRecMult()->_rank;
					_curTpl->setRank(_tupInd + rank);

                    for (unsigned i = 0; i < rank; i++) {
                        //if (!_useOrder)
                            setTupleValOne(vs.setRecMult()->_array[i], _multInd[i], i, _reverse);
                        //else {
                        //	setTupleValOne(vs.setRecMult()->_array[i], _tplMultInd[i], i, false);
                        //	tplInd = tplInd * vs.setRecMult()->_cnts[i] + _tplMultInd[i];
                        //}
                    }
				}
				else {
					_curTpl->setRank(_tupInd + 1);

					tplInd = (_set.t == TP_SET_FIN ? _indBase : _ind);
                    //if (_useOrder && _set.useValP() && _set.setBase()->hasUserOrder()) {
                    //	if (_set.t == TP_SET_FIN)
                    //		tplInd = _tplIndBase;
                    //	else if (_set.t == TP_SET_R1_ALG && _reverse != _set.setAlg()->_negOrder)
                    //		tplInd = _cnt - 1 - _ind;
                    //	else if (_set.t == TP_SET_R1_ENUM)
                    //      tplInd = _set.setEnum()->_order[_reverse ? _cnt - 1 - _ind : _ind];
                    //}

                    setTupleValOne(vs, tplInd, 0, (_reverse /*&& !_useOrder*/));
				}
			
                if (sub && _sub && _set.t == TP_SET_FIN && *(_set.setFinite()->subSet(tplInd)))
					_sub->setTupleValsAll(true);

                //if (_useOrder) {
                //	if (_set.t == TP_SET_FIN)
                //		_tplInd = (_set.setFinite()->_minRank == 0 ? 1 : 0) + (tplInd == 0 ? 0 : _set.setFinite()->_index[tplInd-1]) + (_sub ? _sub->tupleIndex() : 0);
                //	else
                //		_tplInd = tplInd;
                //}
            }
		}
	}

	/**
	 * set one tuple element value
     * @param vsa		set from which value to set (must have rank 1)
	 * @param ind		index number of element in the set vs
	 * @param tc		element number in the tuple, based on _tupInd
	 * @param rev		reverse order
	 */
    void SetIterator::setTupleValOne(CmplVal& vsa, unsigned long ind, unsigned tc, bool rev)
	{
        CmplVal& vs = SET_VAL_WO_ORDER(vsa);

		switch (vs.t) {
			case TP_SET_R1_ENUM:
				{
					SetEnum *setEnum = vs.setEnum();
					unsigned long indEnum = (!rev ? ind : (setEnum->cnt() - ind - 1));

					_curTpl->at(_tupInd + tc)->set((indEnum < setEnum->_cntInt ? TP_INT : TP_STR), setEnum->_array[indEnum]);
				}
				break;

			case TP_SET_R1_ALG:
				{
					SetAlg *setAlg = vs.setAlg();
					intType ival = setAlg->_start + (!rev ? ind : (setAlg->cnt() - 1 - ind)) * setAlg->_step;
					_curTpl->at(_tupInd + tc)->set(TP_INT, ival);
				}
				break;

			case TP_SET_R1_0_UB:
			case TP_SET_R1_0_UB_MNF:
				_curTpl->at(_tupInd + tc)->set(TP_INT, (intType)(rev ? vs.v.i - ind : ind));
				break;

			case TP_SET_R1_1_UB:
			case TP_SET_R1_1_UB_MNF:
				_curTpl->at(_tupInd + tc)->set(TP_INT, (intType)(rev ? vs.v.i - ind : (ind + 1)));
				break;

			case TP_SET_1INT:
			case TP_SET_1STR:
            case TP_INT:
            case TP_STR:
                _curTpl->at(_tupInd + tc)->set((vs.t==TP_SET_1INT || vs.t==TP_INT ? TP_INT : TP_STR), vs.v.i);
				break;

			default:		// no finite set
				throw invalid_argument("no suitable finite set in SetIterator::setTupleValOne()");
				break;
		}
	}


	/**
	 * set iterator to first tuple
	 * @param sv		set tuple values
	 * @param fn		iterate to next after null tuple
	 * @return			false if no next tuple
	 */
	bool SetIterator::iterInternStart(bool sv, bool fn)
	{
		if (_parent)
			_ind = (fn ? 1 : 0);

        if (_directOrder || _useOrder) {
			if (sv)
				setTupleValsAll(false);
		}
		else {
			if (_set.t == TP_SET_FIN && !fn && _set.setFinite()->_minRank == 0 && (!_reverse || _set.setFinite()->cnt() == 1)) {
				_curNull = true;
				_tplInd = 0;
				if (sv)
					setTupleValsAll(false);
			}
			else {
				if (fn && _set.setFinite()->cnt() == 1)
					return false;
				
				_indBase = _tplIndBase = 0;
				_curNull = false;

                CmplVal& vsa = (_set.t == TP_SET_FIN ? _set.setFinite()->_base : _set);
                CmplVal& vs = SET_VAL_WO_ORDER(vsa);

				if (vs.t == TP_SET_REC_MULT) {
					unsigned rank = vs.setRecMult()->_rank;

					for (unsigned i = 0; i < rank; i++) {
						_multInd[i] = 0;

                        //if (_useOrder) {
                        //	_tplMultInd[i] = tplIndForUserOrderOfSimpleSet(vs.setRecMult()->_array[i], 0, _reverse);
                        //	_tplIndBase = _tplIndBase * vs.setRecMult()->_cnts[i] + _tplMultInd[i];
                        //}
					}
				}
                //else if (_useOrder) {
                //	_tplIndBase = tplIndForUserOrderOfSimpleSet(vs, 0, _reverse);
                //}

				if (sv)
					setTupleValsAll(false);

				if (_set.t == TP_SET_FIN) {
                    CmplVal *svp = _set.setFinite()->subSet(_indBase); // (_useOrder ? _tplIndBase : _indBase);
					if (*svp) {
						checkGetSubIterator(svp);
						_sub->iterInternStart(sv, false);
					}

                    //if (_useOrder)
                    //	_tplInd = (_set.setFinite()->_minRank == 0 ? 1 : 0) + (_tplIndBase == 0 ? 0 : _set.setFinite()->_index[_tplIndBase-1]) + (*svp ? _sub->tupleIndex() : 0);
				}
                //else if (_useOrder)
                //	_tplInd = _tplIndBase;
			}
		}

		return true;
	}

	/**
	 * set iterator to next tuple
	 * @param sv		set tuple values
	 * @return			false if no next tuple
	 */
	bool SetIterator::iterInternNext(bool sv)
	{
		if (_parent) {
			if (++_ind >= _cnt) {
				_tplInd = _cnt;
				return false;
			}
		}

        if (_directOrder || _useOrder) {
			if (sv)
				setTupleValsAll(false);
		}

		else {
			if (_set.t == TP_SET_FIN) {
				if (_curNull)
					return iterInternStart(sv, true);

				if (_reverse && _set.setFinite()->_minRank == 0 && _ind == _cnt - 1) {
					_curNull = true;
					_tplInd = 0;
					if (sv)
						setTupleValsAll(false);

					return true;
				}

                if (*(_set.setFinite()->subSet(/*_useOrder ? _tplIndBase :*/ _indBase))) {
					if (_sub->iterInternNext(sv)) {
                        //if (_useOrder)
                        //	_tplInd = (_set.setFinite()->_minRank == 0 ? 1 : 0) + (_tplIndBase == 0 ? 0 : _set.setFinite()->_index[_tplIndBase-1]) + _sub->tupleIndex();

						return true;
					}
				}
			}

            CmplVal& vsa = (_set.t == TP_SET_FIN ? _set.setFinite()->_base : _set);
            CmplVal& vs = SET_VAL_WO_ORDER(vsa);

            if (_set.t == TP_SET_FIN)
                _indBase++;

			if (vs.t == TP_SET_REC_MULT) {
				unsigned rank = vs.setRecMult()->_rank;
				if (sv)
					_curTpl->setRank(_tupInd + rank);

                //unsigned long tplInd = (_set.t == TP_SET_FIN ? _tplIndBase : _tplInd);
                //unsigned long tplIndFakt = 1;

				for (unsigned r = rank - 1; ; r--) {
					_multInd[r]++;
					bool restart = (_multInd[r] >= vs.setRecMult()->_cnts[r]);

					if (restart)
						_multInd[r] = 0;

                    //if (_useOrder) {
                    //	unsigned long tplMultIndPrv = _tplMultInd[r];
                    //	_tplMultInd[r] = tplIndForUserOrderOfSimpleSet(vs.setRecMult()->_array[r], _multInd[r], _reverse);

                    //	tplInd += (_tplMultInd[r] - tplMultIndPrv) * tplIndFakt;
                    //	tplIndFakt *= vs.setRecMult()->_cnts[r];
						
                    //	if (sv)
                    //		setTupleValOne(vs.setRecMult()->_array[r], _tplMultInd[r], r, false);
                    //}
                    //else {
						if (sv)
                            setTupleValOne(vs.setRecMult()->_array[r], _multInd[r], r, _reverse);
                    //}

					if (!restart || !r)
						break;
				}

                //if (_useOrder) {
                //	if (_set.t == TP_SET_FIN)
                //		_tplIndBase = tplInd;
                //	else
                //		_tplInd = tplInd;
                //}
			}
			else {
				unsigned long ind = (_set.t == TP_SET_FIN ? _indBase : _ind);
                //unsigned long tplInd = ind;

                //if (_useOrder) {
                //	tplInd = tplIndForUserOrderOfSimpleSet(vs, ind, _reverse);

                //	if (sv) {
                //		_curTpl->setRank(_tupInd + 1);
                //		setTupleValOne(vs, tplInd, 0, false);
                //	}

                //	if (_set.t == TP_SET_FIN)
                //		_tplIndBase = tplInd;
                //	else
                //		_tplInd = tplInd;
                //}
                //else {
					if (sv) {
						_curTpl->setRank(_tupInd + 1);
                        setTupleValOne(vs, ind, 0, _reverse);
					}
                //}
			}

			if (_set.t == TP_SET_FIN) {
                CmplVal *svp = _set.setFinite()->subSet(_indBase); //(_useOrder ? _tplIndBase : _indBase);
				if (*svp) {
					checkGetSubIterator(svp);
					_sub->iterInternStart(sv, false);
				}

                //if (_useOrder)
                //	_tplInd = (_set.setFinite()->_minRank == 0 ? 1 : 0) + (_tplIndBase == 0 ? 0 : _set.setFinite()->_index[_tplIndBase-1]) + (*svp ? _sub->tupleIndex() : 0);
			}
		}

		return true;
	}


	/**
	 * get a new subiterator or reuse the existing one, only called when _set.t == TP_SET_FIN
	 * @param svp		set value to subiterate
	 */
	void SetIterator::checkGetSubIterator(CmplVal *svp)
	{
		if (_sub) {
			if (_sub->_set == *svp) {
				_sub->_curTpl = _curTpl;
				_sub->_start = true;
				_sub->_ind = 0;
				return;
			}

			delete _sub;
		}

		_sub = new SetIterator(*svp, _rtm, _useOrder, _reverse);

		_sub->_curTpl = _curTpl;
		_sub->_parent = this;
		_sub->_tupInd = _tupInd + _set.setFinite()->_baseRank;
	}


	/**
	 * get tuple index for a simple set that is used as part of another set
	 * @param set		simple set
	 * @param ind		iteration index within set
	 * @param rev		reverse iteration
	 * @return			tuple index
	 */
	unsigned long SetIterator::tplIndForUserOrderOfSimpleSet(CmplVal& set, unsigned long ind, bool rev)
	{
        unsigned long *order = NULL;

        if (SET_VAL_HAS_ORDER(set)) {
            SetWithOrder *so = set.setWithOrder();
            if (so->negOrder())
                rev = !rev;
            else if (so->order())
                order = so->order();
        }

        if (rev)
            ind = SetBase::cnt(set) - 1 - ind;

        if (order)
            ind = order[ind];

        return ind;
	}


    //TODO
    //-----
    /**
	 * call a function for each tuple in the set
	 * @param fct			function to call
     * @param no			current number of the iteration
     * @param tplInd		index of the current tuple within the set
     * @param cur			current tuple
	 */
	void SetIterator::forEach(void (*fct)(unsigned long no, unsigned long tplInd, CmplVal& cur))
	{
		_ind = _tplInd = 0;
		if (_cnt == 0)
			return;

		tp_e tpVal1Int, tpVal1Str;
		if (_rtm == iteratorTupleObjWithSet) {
			tpVal1Int = TP_ITUPLE;
			tpVal1Str = TP_ITUPLE;
		}
		else if (_set.t != TP_SET_FIN && _set.t != TP_SET_REC_MULT && (_rtm == iteratorTupleSimple || _rtm == iteratorTupleSimpleWithSet)) {
			tpVal1Int = TP_INT;
			tpVal1Str = TP_STR;
		}
		else {
			tpVal1Int = TP_ITUPLE_1INT;
			tpVal1Str = TP_ITUPLE_1STR;
		}

		if (_set.t == TP_SET_FIN || _set.t == TP_SET_REC_MULT || _rtm == iteratorTupleObjWithSet) {
			checkSetNewTuple(false);
			if (_set.t != TP_SET_FIN && _set.t != TP_SET_REC_MULT)
                _curTpl->setRank(SetBase::rank(_set));
		}

		switch (_set.t) {
			case TP_SET_FIN:
			case TP_SET_REC_MULT:
				{
					bool ntp;
					iterInternStart(true, false);

					for (; _ind < _cnt; _ind++) {
						if (_rtm != iteratorTupleObjWithSet)
							Tuple::toSpecialValTuple(_cur, false);

						fct(_ind, _tplInd, _cur);

						ntp = checkSetNewTuple(false);
						iterInternNext(!ntp);
						if (ntp)
							setTupleValsAll(true);
					}
				}
				break;

			case TP_SET_R1_ENUM:
				{
					SetEnum *setEnum = _set.setEnum();

                    for (; _ind < _cnt; _ind++) {
						_tplInd = (_reverse ? _cnt - 1 - _ind : _ind);
                        if (_directOrder)
                            _tplInd = _directOrder[_tplInd];

						if (_rtm == iteratorTupleObjWithSet) {
							checkSetNewTuple(false);
							_curTpl->at(0)->set((_tplInd < setEnum->_cntInt ? TP_INT : TP_STR), setEnum->_array[_tplInd]);
						}
						else {
							_cur.set((_tplInd < setEnum->_cntInt ? tpVal1Int : tpVal1Str), setEnum->_array[_tplInd]);
						}

						fct(_ind, _tplInd, _cur);
					}
				}
				break;

			case TP_SET_R1_ALG:
				{
					SetAlg *setAlg = _set.setAlg();
                    intType i;

                    for (; _ind < _cnt; _ind++) {
                        _tplInd = (_reverse ? _cnt - 1 - _ind : _ind);
                        if (_directOrder)
                            _tplInd = _directOrder[_tplInd];

                        i = setAlg->_start + _tplInd * setAlg->_step;

                        if (_rtm == iteratorTupleObjWithSet) {
                            checkSetNewTuple(false);
                            _curTpl->at(0)->set(TP_INT, i);
                        }
                        else {
                            _cur.set(tpVal1Int, i);
                        }

                        fct(_ind, _tplInd, _cur);
                    }
				}
				break;

			case TP_SET_R1_0_UB:
			case TP_SET_R1_0_UB_MNF:
			case TP_SET_R1_1_UB:
			case TP_SET_R1_1_UB_MNF:
                if (_rtm == iteratorTupleObjWithSet || _directOrder) {
                    intType start = (intType)(_set.t==TP_SET_R1_1_UB || _set.t==TP_SET_R1_1_UB_MNF ? 1 : 0);
                    for (; _ind < _cnt; _ind++) {
                        _tplInd = (_reverse ? _cnt - 1 - _ind : _ind);
                        if (_directOrder)
                            _tplInd = _directOrder[_tplInd];

                        if (_rtm == iteratorTupleObjWithSet) {
                            checkSetNewTuple(false);
                            _curTpl->at(0)->set(TP_INT, (intType)(start + _tplInd));
                        }
                        else {
                            _cur.set(tpVal1Int, (intType)(start + _tplInd));
                        }

                        fct(_ind, _tplInd, _cur);
                    }
				}
				else
				{
					if (!_reverse) {
						_cur.set(tpVal1Int, (intType)(_set.t==TP_SET_R1_1_UB || _set.t==TP_SET_R1_1_UB_MNF ? 1 : 0));
						for (; _ind < _cnt; _ind++, _cur.v.i++)
							fct(_ind, _ind, _cur);
					}
					else {
						_cur.set(tpVal1Int, (intType)(_set.t==TP_SET_R1_1_UB || _set.t==TP_SET_R1_1_UB_MNF ? _cnt : (_cnt - 1)));
						for (; _ind < _cnt; _ind++, _cur.v.i--)
							fct(_ind, _cnt-1-_ind, _cur);
					}
				}
				break;

			case TP_SET_NULL:
				if (_rtm != iteratorTupleObjWithSet)
					_cur.set(TP_ITUPLE_NULL);

				fct(0, 0, _cur);
				break;

			case TP_SET_1INT:
			case TP_SET_1STR:
				if (_rtm == iteratorTupleObjWithSet)
					_curTpl->at(0)->set(TP_INT, _set.v.i);
				else
					_cur.set((_set.t==TP_SET_1INT ? tpVal1Int : tpVal1Str), _set.v.i);

				fct(0, 0, _cur);
				break;

			default:		// no finite set
				throw invalid_argument("no finite set in SetIterator::forEach()");
		}

		_ind = _tplInd = _cnt;
	}

}

