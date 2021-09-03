
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


#ifndef SETITERATOR_HH
#define SETITERATOR_HH


#include "CmplVal.hh"
#include "Tuple.hh"
#include "SetBase.hh"


using namespace std;


namespace cmpl
{
	/**
	 * Utility class for iterating over a finite index tuple set
	 */
	class SetIterator
	{
		friend class SetRecMult;
		friend class SetFinite;
        friend class SetWithOrder;

	public:
		enum ReturnMode {
			iteratorTupleSimple,		///< if iterating a simple rank 1 set, then return scalar values instead of tuples; never store position in the set in the resulting tuple
			iteratorTupleSimpleWithSet,	///< if iterating a simple rank 1 set, then return scalar values instead of tuples; and for rank 2 and greater store position in the set in the resulting tuple object
			iteratorTupleVal,			///< return ever a tuple, but for rank 0 and 1 as special value instead of tuple object; never store position in the set in the resulting tuple
			iteratorTupleValWithSet,	///< return ever a tuple, but for rank 0 and 1 as special value instead of tuple object; and for rank 2 and greater store position in the set in the resulting tuple object
			iteratorTupleObjWithSet,	///< return ever a tuple object, even for rank 0 and 1; and store in every resulting tuple object the position in the set
		};

	private:
		CmplVal _set;				///< set to iterate
		ReturnMode _rtm;			///< return mode for tuples
		bool _useOrder;				///< iterate in user order
		bool _reverse;				///< iterate in reverse order
		unsigned long _cnt;			///< count of elements in <code>_set</code>

		bool _start;				///< start or restart the iteration
		unsigned long _ind;			///< index number of current element
		unsigned long _indBase;		///< index number of current element in SetFinite::_base (only used for iteration over <code>SetFinite</code>)
		unsigned long *_multInd;	///< indizes for single sets within <code>SetRecMult</code>

		unsigned long _tplInd;		///< index of the current element within the set (only used if _useOrder)
		unsigned long _tplIndBase;	///< index of the current element within the set SetFinite::_base (only used if _useOrder for iteration over <code>SetFinite</code>)
		unsigned long *_tplMultInd;	///< indizes of the current element within the sets <code>SetRecMult</code> (only used if _useOrder)

        unsigned long *_directOrder;///< array with direct order (if not null, then use it even with !_useOrder) (not allocated here)

		CmplVal _cur;				///< current tuple
		Tuple *_curTpl;				///< pointer to current tuple object itself (in subiterator tuple object from base iterator)
		bool _curNull;				///< current iteration is the null tuple (only used for iteration over <code>SetFinite</code>)

		SetIterator *_sub;			///< current subiterator for this iterator (only used for iteration over <code>SetFinite</code>)
		SetIterator *_parent;		///< this is a subiterator within the given parent iterator / NULL: this is a base iterator
		unsigned _tupInd;			///< this iterator fills the result tuple from this index (only for subiterator, otherwise 0)



		/************** functions for iteration **********/

	public:

        /**
         * empty constructor
         */
        SetIterator()                                                                                                           { CmplVal se(TP_SET_EMPTY); initIntern(se, iteratorTupleSimple, false, false); }

        /**
		 * constructor
		 * @param set			set to iterate (must be of any finite set type)
		 * @param rtm			return mode for tuples
		 * @param useOrder		iterate in user order
		 * @param reverse		iterate in reverse order
		 */
        SetIterator(const CmplVal& set, ReturnMode rtm = iteratorTupleSimple, bool useOrder = false, bool reverse = false)      { initIntern(set, rtm, useOrder, reverse); }

        /**
         * destructor
         */
        ~SetIterator()                                                                                                          { dispose(); }

        /**
         * initialize set iterator
         * @param set			set to iterate (must be of any finite set type)
         * @param rtm			return mode for tuples
         * @param useOrder		iterate in user order
         * @param reverse		iterate in reverse order
         */
        void init(const CmplVal& set, ReturnMode rtm = iteratorTupleSimple, bool useOrder = false, bool reverse = false)        { dispose(); initIntern(set, rtm, useOrder, reverse); }

    private:
        /**
         * initialize set iterator
         * @param set			set to iterate (must be of any finite set type)
         * @param rtm			return mode for tuples
         * @param useOrder		iterate in user order
         * @param reverse		iterate in reverse order
         */
        void initIntern(const CmplVal& set, ReturnMode rtm, bool useOrder, bool reverse);

        /**
         * dispose set iterator
         */
        void dispose();


    public:
		/**
		 * get count of elements in the set
		 * @return				count of elements
		 */
		inline unsigned long count() const		{ return _cnt; }

		/**
		 * get current element of iteration
		 * @return				current index tuple / TP_EMPTY if iteration is ended
		 */
        inline const CmplVal& curTuple() const	{ return _cur; }

		/**
		 * get current index number of iteration
		 * @return				current index number / equal count() if iteration is ended
		 */
		inline unsigned long curIndex() const	{ return _ind; }

		/**
		 * get index of the current element within the set
		 * @return				index of the current element within the set / equal count() if iteration is ended
		 */
        inline unsigned long tupleIndex() const	{ return ((_useOrder || _directOrder) ? _tplInd : (_reverse && _ind < _cnt ? (_cnt - 1 - _ind) : _ind)); }

		/**
		 * start or restart the iteration
		 * @return				first tupel of the set
		 */
		inline CmplVal& begin()					{ iterIntern(true); return _cur; }

		/**
		 * iterate to the next tuple
		 * @return				next tuple of the set
		 */
		inline CmplVal& iter()					{ iterIntern(false); return _cur; }

		/**
		 * get value TP_EMPTY as flag that the iteration is ended
		 * @return				value TP_EMPTY
		 */
		inline CmplVal end() const				{ return CmplVal(TP_EMPTY); }

		/**
		 * returns whether the iteration is ended
		 * @return				true if end of iteration is reached (current element is then TP_EMPTY)
		 */
		inline bool ended() const				{ return (_ind >= _cnt); }

		/**
		 * get current element of iteration
		 * @return				current index tuple / TP_EMPTY if iteration is ended
		 */
		const inline CmplVal& operator* () const { return _cur; }

		/**
		 * iterate to the next tuple
		 */
		inline void operator++ ()				{ iterIntern(false); }
		
		/**
		 * iterate to the next tuple
		 */
		inline void operator++ (int)			{ iterIntern(false); }
		
		/**
		 * returns whether the iteration is not ended
		 * @return				true if iteration is not ended (current element is not TP_EMPTY)
		 */
		inline explicit operator bool() const	{ return (_ind < _cnt); }


		/**
		 * call a function for each tuple in the set
		 * @param fct           function to call
		 * 	@param no				current number of the iteration
		 * 	@param tplInd			index of the current tuple within the set
		 * 	@param cur				current tuple
		 */
		void forEach(void (*fct)(unsigned long no, unsigned long tplInd, CmplVal& cur));


		/**
		 * get tuple at a given index position in a set
         * @param res           return of result tuple (if set is a simple rank 1 set, then as scalar value instead of tuple) / TP_EMPTY: ind is out of range
		 * @param set			set (must be of any finite set type)
		 * @param ind			index number of tuple
		 * @param rtm			return mode for tuples
		 * @param useOrder		use user order
		 * @param reverse		use reverse order
		 */
        static void at(CmplVal &res, const CmplVal &set, unsigned long ind, ReturnMode rtm = iteratorTupleSimple, bool useOrder = false, bool reverse = false);

		/**
		 * check two c-arrays for equality
		 * @param cnt			count of elements in either array
		 * @param arr1			first array
		 * @param arr2			second array
		 * @return				true if both arrays contains the same elements
		 */
		template <class T> static bool compareArr(unsigned long cnt, T *arr1, T *arr2)		{ for (unsigned long i = 0; i < cnt; i++, arr1++, arr2++) { if (*arr1 != *arr2) return false; } return true; }

        /**
         * check whether a scalar index value is an element of a set
         * @param set			set (must not be TP_SET_INF_TPL or TP_SET_INF_SET)
         * @param ind 			index value, must be int or string
         * @return 				true if ind is an element in set
         */
        static bool isScalarIn(const CmplVal& set, const CmplVal& ind);

        /**
         * check whether a scalar index value is an element of a set, and returns the index number of the element in the set if finite
         * @param set			set
         * @param ind			index value, must be int or string
         * @param num			return of the index number of ind within set (only if set is a finite set)
         * @return 				true if ind is an element in set
         */
        static bool isScalarIn(const CmplVal& set, const CmplVal& ind, unsigned long& num);

        /**
         * copy user order of a set to an index array
         * @param set			source set (must be a finite set)
         * @param add			allocate additional elements more than length of <code>set</code>
         * @param initAdd		initialize additional elements
         * @param ins           only used if initAdd: insert additional elements for this tuple position
         * @return 				new array with copied user order
         */
        static unsigned long *copyOrder(const CmplVal &set, unsigned long add, bool initAdd, unsigned long ins);

        /**
         * copy user order of a set to an index array
         * @param set			source set (must be a finite set)
         * @return 				new array with copied user order
         */
        inline static unsigned long *copyOrder(const CmplVal &set)             { return copyOrder(set, 0, false, 0); }

        /**
         * copy user order of a set to an index array
         * @param set			source set (must be a finite set)
         * @param ord			array to copy user order to
         * @param ob			start index within <code>ord</code>, return current index
         * @param ib			start of set index to put in <code>ord</code>
         * @param cs			length of source set / 0: not known
         */
        static void copyOrder(const CmplVal &set, unsigned long *ord, unsigned long& ob, unsigned long ib, unsigned long cs = 0);

	private:
		/**
		 * set tuple element for the tuple on a given index position in a set
		 * @param set			set (must be of any finite set type with rank 1) (must not be TP_SET_FIN or TP_SET_REC_MULT)
		 * @param ind			index number within the set
		 * @param elem			tuple element to insert value
		 * @param reverse		use reverse order
		 * @return				index of the tuple within the set
		 */
        static unsigned long tupleAtInternSimple(const CmplVal &set, unsigned long ind, CmplVal *elem, bool reverse);

    public:
		/**
		 * search a value within a sorted array
		 * @param array			array to search in
		 * @param cnt			length of array
		 * @param k				value to search
		 * @return				position p in the array, with (p==0 ? 0 : array[p-1]) <= k < array[p]
         */
        template<class t> static unsigned long searchArrayPos(t *array, unsigned long cnt, t k);


		/************** internal functions for iteration **********/

	private:

		/**
		 * iterate to the first or next tuple
		 * @param start			start or restart the iteration
		 */
		void iterIntern(bool start);

		/**
		 * set a new tuple value as current tuple if neccessary, only called for base iterator
		 * @param sv		set all tuple values if a new tuple object
		 * @return			current tuple is now a new tuple object
		 */
		bool checkSetNewTuple(bool sv);

		/**
		 * set all tuple element values
		 * @param sub		also set tuple values for subiterator
		 */
		void setTupleValsAll(bool sub);

		/**
		 * set one tuple element value
         * @param vsa		set from which value to set (must have rank 1)
		 * @param ind		index number of element in the set vs
		 * @param tc		element number in the tuple, based on _tupInd
		 * @param rev		reverse order
		 */
        void setTupleValOne(CmplVal& vsa, unsigned long ind, unsigned tc, bool rev);

		/**
		 * set iterator to first tuple
		 * @param sv		set tuple values
		 * @param fn		iterate to next after null tuple
		 * @return			false if no next tuple
		 */
		bool iterInternStart(bool sv, bool fn);

		/**
		 * set iterator to next tuple
		 * @param sv		set tuple values
		 * @return			false if no next tuple
		 */
		bool iterInternNext(bool sv);

		/**
		 * get a new subiterator or reuse the existing one, only called when _set.t == TP_SET_FIN
		 * @param svp		set value to subiterate
		 */
		void checkGetSubIterator(CmplVal *svp);

		/**
		 * get tuple index for a simple set that is used as part of another set
		 * @param set		simple set
		 * @param ind		iteration index within set
		 * @param rev		reverse iteration
		 * @return			tuple index
		 */
        static unsigned long tplIndForUserOrderOfSimpleSet(CmplVal& set, unsigned long ind, bool rev);
	};


	
	/************** implementation of template function here in the header file **********/

	/**
	 * search a value within a sorted array
	 * @param array			array to search in
	 * @param cnt			length of array
	 * @param k				value to search
     * @return				position p in the array, with (p==0 ? MinValue(t) : array[p-1]) <= k < array[p]
	 */
    template<class t> unsigned long SetIterator::searchArrayPos(t *array, unsigned long cnt, t k)
	{
		unsigned long lb = 0;
		unsigned long ub = cnt;
		unsigned long m;

		while (lb < ub) {
			m = (ub - lb) / 2 + lb;

			if (k >= array[m]) {
				if (k == array[m])
					return m+1;
				else
					lb = m + 1;
			}
			else {
				if (m == 0 || k >= array[m-1])
					return m;
				else
					ub = m - 1;
			}
		}

		return lb;
	}

}

#endif // SETITERATOR_HH


