
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


#ifndef TUPLE_HH
#define TUPLE_HH


#include <stdexcept>
#include <vector>
#include "CmplVal.hh"


using namespace std;



#define TUPLE_MIN_CAPACITY		5		///< minimal capacity for every tuple, to avoid double memory allocation

#define TUPLE_CHECK_INDEX				///< if this is defined then check index for tuple elements, else skip the check for better performance


namespace cmpl
{
	class SetIterator;

	/**
	 * The <code>Tuple</code> class represents a tuple of a certain count of arbitrary cmpl values
	 */
	class Tuple : public CmplObjBase
	{
		friend class SetIterator;
	
	public:
		enum TupleType {
			tupleIndexOnly = 0,		///< tuple use only int or string elements (tuple is TP_ITUPLE)
			tupleSetFinite = 1,		///< tuple can be converted to a finite index tuple set
			tupleSetInf = 2,		///< tuple can be converted to an infinite index tuple set
			tupleElse = 3,			///< other tuple (not an index tuple and cannot be converted to an index tuple set)
			tupleUnknown = 4,		///< tuple type is not known
		};

	private:
		unsigned _rank;				///< count of elements in the tuple
		unsigned _cap;				///< capacity for elements in the tuple

		TupleType _tupleType;		///< type of this tuple (but the correct use is not checked!)

		CmplVal *_array;			///< array with elements of the tuple
		CmplVal _arrMin[TUPLE_MIN_CAPACITY];		///< embedded array for elements, to avoid a second memory allocation in the most cases

		CmplVal _iterSet;			///< tuple is part of iteration of this set / TP_EMPTY: not set
		unsigned long _setInd;		///< index of this tuple within <code>_iterSet</code>

	private:
#ifdef TUPLE_CHECK_INDEX
		/**
		 * check if index is valid for tuple element, otherwise throw exception
		 * @param i				index of element (must be between 0 and _rank)
		 */
        inline void checkIndex(unsigned i) const		{
            if (i >= _rank)
            {
                throw out_of_range("tuple element index");
            } }
#else
		/**
		 * empty dummy function (don't check index)
		 */
        inline void checkIndex(unsigned i) const		{ /*no check*/ }
#endif

	public:
		/**
		 * constructor
		 * @param rank			initial rank of the tuple
		 * @param cap			capacity for the tuple / 0: same as rank
		 * @param ttp			tuple type
		 */
		Tuple(unsigned rank, unsigned cap = 0, TupleType ttp = tupleUnknown);

		/**
		 * destructor
		 */
		~Tuple();

		/**
		 * get rank
		 */
		inline unsigned rank() const						{ return _rank; }

		/**
		 * get rank
		 * @param tp			value, must be a tuple
		 */
        static inline unsigned rank(const CmplVal& tp)		{ return (tp.isTuple() ? (tp.useValP() ? tp.tuple()->rank() : (tp.rank0() ? 0 : 1)) : 1); }

		/**
	 	 * set rank
		 * @param rank		new rank for the tuple
		 * @param disp		dispose values of removed elements (not used for marked index tuples)
		 */
		void setRank(unsigned rank, bool disp = true);

		/**
		 * get capacity
		 */
		inline unsigned cap() const							{ return _cap; }

        /**
         * count tuple parts with given type
         * @param t         type
         * @return          count of tuple parts with given type
         */
        unsigned countPartType(tp_e t) const;

		/**
		 * element of tuple
		 * @param i				index of element (must be between 0 and _rank)
		 */
        inline CmplVal *at(unsigned i) const				{ checkIndex(i); return _array + i; }

        /**
         * element of tuple
         * @param tp			tuple value
         * @param i				index of element
         * @return 				element of tuple or value itself
         */
        static inline const CmplVal *at(const CmplVal& tp, unsigned i)	{ return (tp.isTuple() && tp.useValP() ? tp.tuple()->at(i) : (!i ? &tp : NULL)); }

		/**
		 * element of tuple
		 * @param i				index of element (must be between 0 and _rank)
		 */
		inline CmplVal& operator[] (unsigned i)				{ checkIndex(i); return _array[i]; }

        /**
         * get part of this tuple
         * @param res           return result
         * @param f             first element of part
         * @param r             rank of part
         */
        void partTuple(CmplVal& res, unsigned f, unsigned r) const;

        /**
         * construct new tuple by concatenation of two tuples
         * @param res           return result
         * @param fi            first source tuple
         * @param sc            second source tuple
         */
        static void constructTuple(CmplVal& res, const CmplVal& fi, const CmplVal& sc);

        /**
         * construct new tuple by concatenation of tuples
         * @param res           return result
         * @param src           source tuples
         */
        static void constructTuple(CmplVal& res, const vector<CmplValAuto>& src);

		/**
		 * get type of this tuple
		 */
		TupleType tupleType() const							{ return _tupleType; }

		/**
		 * get if this tuple is marked as used only as index tuple
		 */
		bool onlyIndex() const								{ return (_tupleType == tupleIndexOnly); }

		/**
		 * check whether the tuple is an index tuple
		 * @return				true if it is an index tuple (all values int or string)
		 */
		bool isIndexTuple()	const							{ if (_tupleType == tupleUnknown) { { CmplVal *vp = _array; for (unsigned i=0; i<_rank; i++, vp++) { if (!vp->isScalarIndex()) return false; } } return true; } else { return onlyIndex(); } }


		/**
		 * get whether this tuple has information about the iteration set it comes from
		 */
		bool hasIterSet() const								{ return (_iterSet.t != TP_EMPTY); }

		/**
		 * get iteration set and index within this set
		 * @param set			return iteration set
		 * @return				index of the tuple within the set
		 */
        unsigned long getIterSetIndex(CmplVal& set)			{ set.copyFrom(_iterSet, true, false); return _setInd; }

        /**
         * concat all current tuples of given set iterators to this tuple (must have enough capacity)
         * @param vsi           vector of set iterators
         * @param ssi           start copy of tuple parts with this set iterator
         */
        void concatIter(vector<SetIterator>& vsi, unsigned ssi = 0);

		/**
		 * write contents of the object to a stream
		 * @param modp			calling module
		 * @param mode			mode for output: 0=direct; 1=part of other value
		 */
        virtual void write(ostream& ostr, ModuleBase *modp, int mode = 0) const override;


		/**
		 * change an index tuple value to an equivalent special value if possible
		 * @param v				value to change to special value
		 * @param disp			dispose cmpl value object
		 * @return				value is changed
		 */
		static bool toSpecialValTuple(CmplVal& v, bool disp);


	private:
		/**
		 * constructor with iteration set, only called from SetIterator
		 * @param set			iteration set
		 * @param rank			initial rank of the tuple
		 * @param cap			capacity for the tuple / 0: same as rank
		 */
        Tuple(const CmplVal& set, unsigned rank, unsigned cap = 0);

		/**
		 * set index within iteration set, only called from SetIterator
		 * @param ind			index of tuple within the set
		 */
		void setIterInd(unsigned long ind)					{ _setInd = ind; }
	};

}

#endif // TUPLE_HH

