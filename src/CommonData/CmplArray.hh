
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


#ifndef CMPLARRAY_HH
#define CMPLARRAY_HH

#include <mutex>

#include "CmplVal.hh"
#include "SetBase.hh"
#include "SetIterator.hh"


using namespace std;



namespace cmpl
{
    class CmplArrayIterator;
    class CmplArrayIteratorRec;


	/**
	 * The <code>CmplArray</code> class represents an array of cmpl values, defined over a definition set
	 */
	class CmplArray : public CmplObjBase
	{
		friend class CmplArrayIterator;
        friend class CmplArrayIteratorRec;

	protected:
		unsigned long _cap;			///< capacity for elements in the array
		unsigned long _size;		///< count of elements in the array
		unsigned long _cnt;			///< count of non-empty elements in the array (if known)

		CmplVal *_array;			///< array with elements (reserved to length <code>_cap</code>; used to length <code>_size</code>)
		CmplVal _defset;			///< definition set for array indizes (count of elements in this set equals to <code>_size</code>) / TP_EMPTY: use implicit definition set <code>1.._size</code> TODO: doch nicht, muss immer gesetzt werden!
										// <code>_defset</code> must be TP_EMPTY if at least one value in <code>_array</code> is itself an array	//TODO: eher array darin doch gar nicht erlauben

        intType _objType;			///< object type of all non-empty elements in <code>_array</code> / -1: not known / -2: mixed

		bool _allValid;				///< there is no empty element in <code>_array</code> / false: not known
		bool _hasInvalid;			///< there is at least one empty element in <code>_array</code> / false: not known
		CmplVal _validSet;			///< definition set only of the non empty elements in <code>_array</code> / TP_EMPTY: equal to <code>_defset</code> (if _allValid==true) or not computed yet (if _allValid==false)
		//TODO: Setzen/Löschen _allValid/_hasInvalid/_validSet
		//TODO: Verwendung/Setzen _allValid/_hasInvalid für CmplArrayIterator

        mutex _accMtx;              ///< mutex for access lock

    private:
        /**
         * constructor: new array with given definition set
         * @param ds			definition set / NULL: use empty set
         * @param def           value for initialization of elements / NULL: use <code>ct</code>
         * @param ct            value type for initialization of elements (alternative to <code>def</code>)
         */
        CmplArray(CmplVal *ds, CmplVal *def, tp_e ct);

	public:
        /**
         * constructor: new array with empty definition set
         */
        CmplArray(): CmplArray(NULL, NULL, TP_EMPTY)                                { }

        /**
         * constructor: new array with given definition set
         * @param ds			definition set
         * @param ct            value type for initialization of elements
         */
        CmplArray(CmplVal& ds, tp_e ct = TP_EMPTY): CmplArray(&ds, NULL, ct)        { }

        /**
         * constructor: new array with given definition set
         * @param ds			definition set
         * @param def           value for initialization of elements
         */
        CmplArray(CmplVal& ds, CmplVal& def): CmplArray(&ds, &def, TP_EMPTY)        { }

		/**
		 * constructor: new array with given elements and definition set <code>1..cnt</code>
		 * @param arr			array with elements (reserved to length <code>cnt</code>)
		 * @param cnt			count of elements in <code>arr</code>
		 * @param allValid		there is no empty element in <code>arr</code> / false: not known
		 * @param hasInvalid	there is at least one empty element in <code>arr</code> / false: not known
		 */
		CmplArray(CmplVal *arr, unsigned long cnt, bool allValid = false, bool hasInvalid = false);

		/**
		 * constructor: new array with given elements and given definition set
		 * @param arr			array with elements (reserved to size of <code>defset</code>)
		 * @param defset		definition set
		 * @param allValid		there is no empty element in <code>arr</code> / false: not known
		 * @param hasInvalid	there is at least one empty element in <code>arr</code> / false: not known
		 */
		CmplArray(CmplVal *arr, CmplVal& defset, bool allValid = false, bool hasInvalid = false);

        /**
         * copy constructor: new array as copy from source array
         * @param src			source array
         */
        CmplArray(CmplArray *src);

        /**
         * constructor: new array as copy from source array, with changed definition set
         * @param src			source array
         * @param defset		new definition set (must have same count as definition set in src)
         * @param uo            sort elements according user order
         */
        CmplArray(CmplArray *src, CmplVal& defset, bool uo);

        /**
		 * destructor
		 */
		~CmplArray();


        /**
         * fill this from another array
         * @param src           source array
         */
        void copyFrom(CmplArray *src);

        /**
         * clear this array
         * @param defset        new definition set / NULL: use TP_SET_EMPTY
         */
        void clear(CmplVal *defset = NULL);


        /**
         * get mutex for access lock
         */
        inline mutex& accMtx()                               { return _accMtx; }

		/**
		 * get size
		 */
		inline unsigned long size() const					{ return _size; }

		/**
		 * get count of non-empty elements
		 */
        inline unsigned long cnt() const					{ return (_allValid ? _size : _cnt); }

		/**
		 * get capacity
		 */
		inline unsigned long cap() const					{ return _cap; }

		/**
		 * get definition set
		 */
        inline CmplVal& defset()        					{ return _defset; }

		/**
		 * get rank of array
		 */
        inline unsigned rank() const						{ return SetBase::rank(_defset); }

        /**
         * get whether array has user order
         */
        inline bool hasUserOrder() const                    { return SetBase::hasUserOrder(_defset); }

		/**
		 * check if it is an empty array
		 */
		inline bool isEmpty() const							{ return (_size == 0); }

		/**
		 * check if it is a scalar value
		 */
        inline bool isScalar() const                        { return (_defset.t == TP_SET_NULL); }


		/**
		 * get element at a given position
		 * @param i				index of element (must be between 0 and _size-1)
		 */
		inline CmplVal *at(unsigned long i)					{ return _array + i; }

		/**
		 * get element at a given position
		 * @param i				index of element (must be between 0 and _size-1)
		 */
		inline CmplVal& operator[] (unsigned long i)		{ return _array[i]; }

		/**
		 * get pointer to simple (scalar) value or NULL if array doesn't contain only a simple value
		 * @return				pointer to value or NULL
		 */
        inline CmplVal *simpleValue()       				{ return (isScalar() ? _array : NULL); }


        /**
         * get whether there is a known object type of all non-empty elements
        */
        inline bool hasObjType()							{ return (_objType >= 0); }

        /**
         * compute object type from the elements
         * @param f				if true then compute also if the array already has a known object type
         */
        void compObjType(bool f = false);

        /**
         * get object type of all
         */
        inline intType objType(bool c = false)				{ if (c && _objType == -1) { compObjType(); } return _objType; }

        /**
         * set object type for the array
         * @param o				object type to set
         */
        inline void setObjType(intType o)					{ _objType = o; }

        /**
         * reset object type for the array
         * @param mix			true: set to mixed object type / false: set to unknown object type
         */
        inline void delObjType(bool mix = false)			{ _objType = (mix ? -2 : -1); }

        /**
         * merge object type
         * @param o				object type to merge to the object type of the array
         */
        void mergeObjType(intType o);

        /**
         * merge object type
         * @param v				merge object type of this value
         */
        inline void mergeObjType(CmplVal &v)				{ mergeObjType(v.getObjectType()); }


		/**
		 * gets whether the array has info about validility of elements
		 */
		inline bool hasValidInfo() const					{ return (_allValid || _hasInvalid); }

		/**
		 * gets whether there is no empty element in the array / false: not known
		 */
		inline bool allValid() const						{ return _allValid; }

		/**
		 * gets whether there is at least one empty element in the array / false: not known
		 */
		inline bool hasInvalid() const						{ return _hasInvalid; }

		/**
		 * delete info about validility of elements
		 */
		inline void delValidInfo()							{ _allValid = _hasInvalid = false; _validSet.dispUnset(); }

		/**
		 * set info about validility of elements
		 * @param av			true: all elements valid / false: at least one element invalid
		 */
        inline void setValidInfo(bool av)					{ _allValid = av; _hasInvalid = !av; _validSet.dispUnset(); if (av) { _cnt = _size; } }

		/**
		 * set info about validility of elements
		 * @param st			type for set of valid elements (TP_EMPTY: all elements are valid; or set type using int arg)
		 * @param i				int arg for set (if set type uses such an arg)
		 */
        inline void setValidInfo(tp_e st, intType i = 0)	{ _validSet.dispSet(st, i); if (st != TP_EMPTY && SetBase::cnt(_validSet) < SetBase::cnt(_defset)) { _allValid = false; _hasInvalid = true; _cnt = SetBase::cnt(_validSet); } else { _validSet.dispUnset(); _allValid = true; _hasInvalid = false; _cnt = _size; } }

		/**
		 * set info about validility of elements
		 * @param st			type for set of valid elements (set type using pointer arg)
		 * @param set			set of valid elements
		 */
        inline void setValidInfo(tp_e st, SetFinite *set)	{ _validSet.dispSet(st, set); if (SetBase::cnt(_validSet) < SetBase::cnt(_defset)) { _allValid = false; _hasInvalid = true; _cnt = SetBase::cnt(_validSet); } else { _validSet.dispUnset(); _allValid = true; _hasInvalid = false; _cnt = _size; } }

        /**
         * set info about validility of elements
         * @param set			set of valid elements
         */
        inline void setValidInfo(CmplVal& set)              { if (set.useValP()) setValidInfo(set.t, set.setFinite()); else if(set.useInt()) setValidInfo(set.t, set.v.i); else setValidInfo(set.t); }

        /**
         * copy info about validility from other array
         * @param src			source array
         */
        inline void copyValidInfo(CmplArray *src)			{ _allValid = src->_allValid; _hasInvalid = src->_hasInvalid; _validSet.copyFrom(src->_validSet); _cnt = (_allValid ? _size : src->_cnt); }

        /**
         * get set of valid elements
         * @param vs            return set
         * @return              false if set is unknown
         */
        bool getValidSet(CmplVal& vs);

        /**
         * insert new values in the array
         * @param ds			new definition set for the array
         * @param ins			insert values at this position (must match the difference between old and new definition set)
         * @param src			pointer to values to insert (count of values must match the difference between current count and the count of the new definition set) / NULL: only reserve the place for the values
         */
        void insertValues(CmplVal& ds, unsigned long ins, CmplVal *src = NULL);
    };


	/**
	 * Utility class for iterating over a cmpl array
	 */
	class CmplArrayIterator
	{
    protected:
		CmplArray& _arr;			///< array to iterate
		SetIterator _iter;			///< iterator for the definition set
		bool _skipEmpty;			///< skip empty elements

		unsigned long _curNumber;	///< current number in the iteration
		unsigned long _curIndex;	///< index of the current element within the array

	public:
		/**
		 * constructor
		 * arr			array to iterate
		 * skipEmpty	skip empty elements
		 * useOrder		iterate in user order
		 * reverse		iterate in reverse order
		 */
		CmplArrayIterator(CmplArray& arr, bool skipEmpty = false, bool useOrder = false, bool reverse = false):
                                                            _arr(arr), _iter(arr._defset, SetIterator::iteratorTupleSimple, useOrder, reverse), _skipEmpty(skipEmpty), _curNumber(0) { }

		/**
		 * get count of elements to iterate over
		 * @return              count of elements
		 */
		inline unsigned long count() const      			{ return (_skipEmpty ? _arr._cnt : _arr._size); }

		/**
		 * start or restart the iteration
		 * @return              first element in the array in iteration order
		 */
		CmplVal* begin();

		/**
		 * iterate to the next element in iteration order
		 * @return              next element in the array
		 */
		CmplVal* iter();

		/**
		 * iterate to the next element in iteration order
		 */
		inline void operator++ ()               { iter(); }

		/**
		 * iterate to the next element in iteration order
		 */
		inline void operator++ (int)            { iter(); }

		/**
		 * returns whether the iteration is ended
		 * @return				true if end of iteration is reached
		 */
		inline bool ended() const               { return _iter.ended(); }

		/**
		* returns whether the iteration is not ended
		* @return              true if iteration is not ended
		*/
		inline explicit operator bool() const   { return !_iter.ended(); }


		/**
		 * get current number within the iteration
		 * @return              current number
		 */
		inline unsigned long curNumber() const	{ return _curNumber; }

		/**
		 * get index of the current element within the array
		 * @return              index of the current element within the array
		 */
		inline unsigned long curIndex() const	{ return _iter.tupleIndex(); }

		/**
		 * get current element of iteration
		 * @return				current element in the array / TP_EMPTY if iteration is ended
		 */
		inline CmplVal cur() const				{ return (_iter ? _arr._array[curIndex()] : CmplVal(TP_EMPTY));  }

		/**
		 * get current element of iteration
		 * @return				current element in the array / NULL if iteration is ended
		 */
		inline CmplVal* operator* () const		{ return (_iter ? _arr._array + _curIndex : NULL); }

		/**
		 * get index tuple of current element of iteration
		 * @return				current index tuple / TP_EMPTY if iteration is ended
		 */
        inline const CmplVal& curTuple()        { return _iter.curTuple(); }


		/**
		 * call a function for each element in the array
		 * @param fct			function to call
         * @param no			current number of the iteration
         * @param ind			index of the current element
         * @param tpl			current index tuple
         * @param elem			current element in the array
		 */
		void forEach(void (*fct)(unsigned long no, unsigned long ind, const CmplVal& tpl, CmplVal& elem));
	};


    /**
     * Utility class for iterating over a cmpl array, but in difference to its base class with recursive iteration over contained arrays.
     * For use with arrays of type TP_ARRAY_COMP.
     */
    class CmplArrayIteratorRec: public CmplArrayIterator
    {
    private:
        CmplArrayIteratorRec *_subIter;
        CmplVal _curRecTuple;

        bool _useOrder;
        bool _reverse;

    public:
        /**
         * constructor
         * arr			array to iterate
         * skipEmpty	skip empty elements
         * useOrder		iterate in user order
         * reverse		iterate in reverse order
         */
        inline CmplArrayIteratorRec(CmplArray& arr, bool skipEmpty = false, bool useOrder = false, bool reverse = false):
                    CmplArrayIterator(arr, skipEmpty, useOrder, reverse), _subIter(NULL), _useOrder(useOrder), _reverse(reverse)	{ }

        /**
         * destructor
         */
        inline ~CmplArrayIteratorRec()	{ _curRecTuple.dispose(); if (_subIter) delete _subIter; }

        /**
         * get count of elements to iterate over
         * @return              count of elements
         */
        inline unsigned long count() const = delete;    // not implemented for CmplArrayIteratorRec

        /**
         * start or restart the iteration
         * @return      first element in the array in iteration order
         */
        CmplVal* begin();

        /**
         * iterate to the next element in iteration order
         * @return      next element in the array
         */
        CmplVal* iter();

        /**
         * iterate to the next element in iteration order
         */
        inline void operator++ ()               { iter(); }

        /**
         * iterate to the next element in iteration order
         */
        inline void operator++ (int)            { iter(); }

        /**
         * get index of the current element within the array
         * @return              index of the current element within the array
         */
        inline unsigned long curIndex() const = delete;     // not possible for CmplArrayIteratorRec

        /**
         * get current element of iteration
         * @return				current element in the array / TP_EMPTY if iteration is ended
         */
        inline CmplVal cur() const				{ return (_subIter ? _subIter->cur() : CmplArrayIterator::cur()); }

        /**
         * get current element of iteration
         * @return				current element in the array / NULL if iteration is ended
         */
        inline CmplVal* operator* () const		{ return (_subIter ? _subIter->operator *() : CmplArrayIterator::operator *()); }

        /**
         * get index tuple of current element of iteration
         * @return				current index tuple / TP_EMPTY if iteration is ended
         */
        const CmplVal& curTuple();

        /**
         * call a function for each element in the array
         * @param fct			function to call
         * @param no			current number of the iteration
         * @param ind			index of the current element
         * @param tpl			current index tuple
         * @param elem			current element in the array
         */
        void forEach(void (*fct)(unsigned long no, unsigned long ind, const CmplVal& tpl, CmplVal& elem));

    private:
        /**
         * get tuple rank for current iteration tuple
         */
        inline unsigned rankCurTuple()          { return (Tuple::rank(_iter.curTuple()) + (_subIter ? _subIter->rankCurTuple() : 0)); }

        /**
         * recursive fill current tuple value
         * @param tpl           destination tuple
         * @param rf            tuple rank from
         */
        void fillCurRecTuple(Tuple *tpl, unsigned rf);
    };

}

#endif // CMPLARRAY_HH

