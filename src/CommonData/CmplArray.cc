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


#include "../Control/MainControl.hh"
#include "CmplArray.hh"



using namespace std;


namespace cmpl
{

    /*********** CmplArray **********/

    /**
     * constructor: new array with given definition set
     * @param ds			definition set / NULL: use empty set
     * @param def           value for initialization of elements / NULL: use <code>ct</code>
     * @param ct            value type for initialization of elements (alternative to <code>def</code>)
     */
    CmplArray::CmplArray(CmplVal *ds, CmplVal *def, tp_e ct)
    {
        if (ds) {
            _defset.unset();
            ds->toSet(_defset, typeConversionFromSub, NULL);
        }
        else {
            _defset.set(TP_SET_EMPTY);
        }

        _cap = _size = SetBase::cnt(_defset);
        _cnt = 0;
        _array = new CmplVal[_cap];

        CmplVal *a = _array;
        if (def) {
            ct = def->t;
            for (unsigned long i = 0; i < _size; i++, a++)
                a->copyFrom(def, true, false);
        }
        else if (ct != TP_EMPTY) {
            for (unsigned long i = 0; i < _size; i++, a++)
                a->set(ct);
        }

        _objType = -1;
        _allValid = (_size == 0 || ct != TP_EMPTY);
        _hasInvalid = !_allValid;
        _validSet.set(ct == TP_EMPTY ? TP_SET_EMPTY : TP_EMPTY);
    }

	/**
	 * constructor: new array with given elements and definition set <code>1..cnt</code>
	 * @param arr			array with elements (reserved to length <code>cnt</code>)
	 * @param cnt			count of elements in <code>arr</code>
	 * @param allValid		there is no empty element in <code>arr</code> / false: not known
	 * @param hasInvalid	there is at least one empty element in <code>arr</code> / false: not known
	 */
	CmplArray::CmplArray(CmplVal *arr, unsigned long cnt, bool allValid, bool hasInvalid)
	{
		_defset.set(TP_SET_R1_1_UB, (intType)cnt);

		_array = arr;
		_cap = _size = cnt;
        _cnt = (allValid ? cnt : 0);

        _objType = -1;
		_allValid = allValid || (_size == 0);
		_hasInvalid = hasInvalid;
	}

	/**
	 * constructor: new array with given elements and given definition set
	 * @param arr			array with elements (reserved to size of <code>defset</code>)
	 * @param defset		definition set
	 * @param allValid		there is no empty element in <code>arr</code> / false: not known
	 * @param hasInvalid	there is at least one empty element in <code>arr</code> / false: not known
	 */
	CmplArray::CmplArray(CmplVal *arr, CmplVal& defset, bool allValid, bool hasInvalid)
	{
		_defset.copyFrom(defset, true, false);

		_array = arr;
        _cap = _size = SetBase::cnt(_defset);
        _cnt = (allValid ? _size : 0);

        _objType = -1;
		_allValid = allValid || (_size == 0);
		_hasInvalid = hasInvalid;
	}

    /**
     * copy constructor: new array as copy from source array
     * @param src			source array
     */
    CmplArray::CmplArray(CmplArray *src)
    {
        _defset.set(TP_SET_EMPTY);
        _cap = _size = 0;
        _array = NULL;

        copyFrom(src);
    }

    /**
     * constructor: new array as copy from source array, with changed definition set
     * @param src			source array
     * @param defset		new definition set (must have same count as definition set in src)
     * @param uo            sort elements according user order
     */
    CmplArray::CmplArray(CmplArray *src, CmplVal& defset, bool uo): CmplArray(src)
    {
        if (SetBase::cnt(_defset) != SetBase::cnt(defset))
            throw invalid_argument("wrong element count in new definition set");

        if (uo && (SetBase::hasUserOrder(defset) || SetBase::hasUserOrder(_defset))) {
            //TODO: eigentlich tut SetIterator zu viel, nur Mapping kanonische Reihenfolge / Nutzerreihenfolge benoetigt
            SetIterator ss(_defset, SetIterator::iteratorTupleSimple, true);
            SetIterator sd(defset, SetIterator::iteratorTupleSimple, true);

            for (ss.begin(), sd.begin(); ss; ss++, sd++)
                _array[sd.tupleIndex()].copyFrom(src->_array[ss.tupleIndex()]);
        }

        _defset.copyFrom(defset);
    }


	/**
	 * destructor
	 */
	CmplArray::~CmplArray()
	{
		CmplVal *p = _array;
		for (unsigned long i = 0; i < _size; i++, p++)
			p->dispose();

		_defset.dispose();

		delete _array;
	}


    /**
     * fill this from another array
     * @param src           source array
     */
    void CmplArray::copyFrom(CmplArray *src)
    {
        if (_array && _size) {
            CmplVal *p = _array;
            for (unsigned long i = 0; i < _size; i++, p++)
                p->dispUnset();
        }

        _defset.copyFrom(src->_defset);
        _size = src->_size;
        _cnt = src->_cnt;

        if (!_array || _cap < _size) {
            if (_array)
                delete _array;

            _cap = _size;
            _array = new CmplVal[_cap];
        }

        CmplVal *d = _array;
        CmplVal *s = src->_array;
        for (unsigned long i = 0; i < _size; i++, d++, s++)
            d->copyFrom(s, true, false);

        _objType = src->_objType;
        _allValid = src->_allValid;
        _hasInvalid = src->_hasInvalid;
        _validSet.copyFrom(src->_validSet);
    }

    /**
     * clear this array
     * @param defset        new definition set / NULL: use TP_SET_EMPTY
     */
    void CmplArray::clear(CmplVal *defset)
    {
        if (_array && _size) {
            CmplVal *p = _array;
            for (unsigned long i = 0; i < _size; i++, p++)
                p->dispUnset();
        }

        if (defset) {
            _defset.copyFrom(defset);
            _size = SetBase::cnt(_defset);
        }
        else {
            _defset.set(TP_SET_EMPTY);
            _size = 0;
        }

        if (!_array || _cap < _size) {
            if (_array)
                delete _array;

            _cap = _size;
            _array = new CmplVal[_cap];
        }

        _cnt = 0;
        _objType = -1;
        _allValid = (_size == 0);
        _hasInvalid = !_allValid;
        _validSet.set(TP_SET_EMPTY);
    }


    /**
     * compute object type from the elements
     * @param f				if true then compute also if the array already has a known object type
     */
    void CmplArray::compObjType(bool f)
    {
        if (_objType == -1 || f) {
            // set object type
            CmplVal *p = _array;
            unsigned long i;
            unsigned long cnt = 0;
            _objType = 0;

            for (i = 0; i < _size; i++, p++) {
                if (*p) {
                    if (!cnt) {
                        _objType = p->getObjectType();
                    }
                    else if (!p->hasObjectType(_objType)) {
                        _objType = -2;
                        break;
                    }
                    cnt++;
                }
            }

            // if all elements are checked, then set validity info too
            if (i == _size && !_allValid && !_validSet) {
                _cnt = cnt;
                _allValid = (_cnt == _size);
                _hasInvalid = !_allValid;
            }
        }
    }


    /**
     * merge object type
     * @param o				object type to merge to the object type of the array
     */
    void CmplArray::mergeObjType(intType o)
    {
        compObjType(false);

        if (_cnt == 0)
            _objType = o;
        else if (_objType != o && _objType != -2)
            _objType = -2;
    }


    /**
     * get set of valid elements
     * @param vs            return set
     * @return              false if set is unknown
     */
    bool CmplArray::getValidSet(CmplVal& vs)
    {
        if (_allValid) {
            vs.copyFrom(_defset, true, false);
            return true;
        }
        else if (_hasInvalid && _validSet) {
            vs.copyFrom(_validSet, true, false);
            return true;
        }
        else {
            return false;
        }
    }


    /**
     * insert new values in the array
     * @param ds			new definition set for the array
     * @param ins			insert values at this position (must match the difference between old and new definition set)
     * @param src			pointer to values to insert (count of values must match the difference between current count and the count of the new definition set) / NULL: only reserve the place for the values
     */
    void CmplArray::insertValues(CmplVal& ds, unsigned long ins, CmplVal *src)
    {
        unsigned long nc = SetBase::cnt(ds);
        long dc = nc - _size;

        if (nc <= _cap) {
            if (ins < _size)
                memmove(_array + ins + dc, _array + ins, (_size - ins) * sizeof(CmplVal));
        }
        else {
            unsigned long cc = ARR_NEWCAP(_cap);
            if (cc < nc)
                cc = nc;

            CmplVal *arr = new CmplVal[cc];
            if (ins > 0)
                memcpy(arr, _array, ins * sizeof(CmplVal));
            if (ins < _size)
                memcpy(arr + ins + dc, _array + ins, (_size - ins) * sizeof(CmplVal));

            delete _array;

            _cap = cc;
            _array = arr;
        }

        _size = nc;
        _defset.copyFrom(ds, true, true);

        if (src) {
            CmplVal *s = src;
            CmplVal *d = _array + ins;

            for (long i = 0; i < dc; i++, s++, d++) {
                if (*s) {
                    d->moveFrom(s);
                    _cnt++;

                    if (_allValid || _objType >= 0) {
                        if (_validSet)
                            _validSet.dispose();
                        if (_objType >= 0 && !d->hasObjectType(_objType))
                            _objType = -2;
                    }
                }
                else {
                    d->unset();
                    _allValid = false;
                }
            }
        }
        else {
            CmplVal *d = _array + ins;
            for (long i = 0; i < dc; i++, d++)
                d->unset();

            _allValid = false;
            _hasInvalid = true;
        }
    }



    /*********** CmplArrayIterator **********/

	/**
	 * start or restart the iteration
	 * @return              first element in the array in iteration order
	 */
	CmplVal* CmplArrayIterator::begin()
	{
		_iter.begin();
		_curNumber = 0;
		_curIndex = _iter.tupleIndex();

		if (_skipEmpty && _iter && !_arr._array[_curIndex]) {
			iter();
			_curNumber = 0;
		}

		return (_iter ? _arr._array + _curIndex : NULL);
	}


	/**
	 * iterate to the next element in iteration order
	 * @return              next element in the array
	 */
	CmplVal* CmplArrayIterator::iter()
	{
		_iter.iter();
		_curIndex = _iter.tupleIndex();

		if (_skipEmpty)
		{
			while (_iter && !_arr._array[_curIndex]) {
				_iter.iter();
				_curIndex = _iter.tupleIndex();
			}
		}

		_curNumber++;
		return (_iter ? _arr._array + _curIndex : NULL);
	}


	/**
	 * call a function for each element in the array
	 * @param fct			function to call
	 *  @param no				current number of the iteration
	 *  @param ind				index of the current element
	 *  @param tpl				current index tuple
	 *  @param elem				current element in the array
	 */
	void CmplArrayIterator::forEach(void (*fct)(unsigned long no, unsigned long ind, const CmplVal& tpl, CmplVal& elem))
	{
		begin();
		while (_iter) {
			fct(_curNumber, _curIndex, _iter.curTuple(), _arr._array[_curIndex]);
			iter();
		}
	}


    /*********** CmplArrayIteratorRec **********/

    /**
     * start or restart the iteration
     * @return              first element in the array in iteration order
     */
    CmplVal* CmplArrayIteratorRec::begin()
    {
        _curRecTuple.dispUnset();

        if (_subIter) {
            delete _subIter;
            _subIter = NULL;
        }

        CmplVal *r = CmplArrayIterator::begin();
        if (_iter) {
            CmplVal *v = _arr._array + _curIndex;
            if (v->t == TP_ARRAY || v->t == TP_ARRAY_COMP) {
                _subIter = new CmplArrayIteratorRec(*(v->array()), _skipEmpty, _useOrder, _reverse);
                r = _subIter->begin();
            }
        }

        return r;
    }

    /**
     * iterate to the next element in iteration order
     * @return      next element in the array
     */
    CmplVal* CmplArrayIteratorRec::iter()
    {
        _curRecTuple.dispUnset();
        CmplVal *r;

        if (_subIter) {
            r = _subIter->iter();
            if (_subIter->ended()) {
                delete _subIter;
                _subIter = NULL;
            }
            else {
                _curNumber++;
            }
        }

        if (!_subIter) {
            r = CmplArrayIterator::iter();
            if (_iter) {
                CmplVal *v = _arr._array + _curIndex;
                if (v->t == TP_ARRAY || v->t == TP_ARRAY_COMP) {
                    _subIter = new CmplArrayIteratorRec(*(v->array()), _skipEmpty, _useOrder, _reverse);
                    r = _subIter->begin();
                }
            }
        }

        return r;
    }

    /**
     * get index tuple of current element of iteration
     * @return				current index tuple / TP_EMPTY if iteration is ended
     */
    const CmplVal& CmplArrayIteratorRec::curTuple()
    {
        if (!_curRecTuple) {
            if (_subIter) {
                unsigned r = _subIter->rankCurTuple() + Tuple::rank(_iter.curTuple());
                Tuple *tpl = new Tuple(r);
                fillCurRecTuple(tpl, 0);
                _curRecTuple.set(TP_ITUPLE, tpl);
            }
            else {
                _curRecTuple.copyFrom(_iter.curTuple(), true, false);
            }
        }

        return _curRecTuple;
    }

    /**
     * call a function for each element in the array
     * @param fct			function to call
     *  @param no				current number of the iteration
     *  @param ind				index of the current element
     *  @param tpl				current index tuple
     *  @param elem				current element in the array
     */
    void CmplArrayIteratorRec::forEach(void (*fct)(unsigned long no, unsigned long ind, const CmplVal& tpl, CmplVal& elem))
    {
        begin();
        while (_iter) {
            fct(_curNumber, _curIndex, _curRecTuple, *(this->operator *()));
            iter();
        }
    }

    /**
     * recursive fill current tuple value
     * @param tpl           destination tuple
     * @param rf            tuple rank from
     */
    void CmplArrayIteratorRec::fillCurRecTuple(Tuple *tpl, unsigned rf)
    {
        CmplVal *e = tpl->at(rf);
        const CmplVal& s = _iter.curTuple();
        unsigned r;

        if (s.isTuple()) {
            r = s.tuple()->rank();
            CmplVal *p = s.tuple()->at(0);
            for (unsigned i = 0; i < r; i++, e++, p++)
                e->copyFrom(p);
        }
        else {
            r = 1;
            e->copyFrom(s);
        }

        if (_subIter)
            _subIter->fillCurRecTuple(tpl, rf + r);
    }

}

