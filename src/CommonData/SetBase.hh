
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


#ifndef SETBASE_HH
#define SETBASE_HH

#include <vector>

#include "CmplVal.hh"
#include "Tuple.hh"


using namespace std;


namespace cmpl
{
	class SetRecMult;
	class SetFinite;
    class SetWithOrder;
    class SetIterator;

	class ModuleBase;


	/**
     * Abstract base class for all tuple set classes
	 */
	class SetBase : public CmplObjBase
	{
        friend class SetIterator;
        friend class SetRecMult;
        friend class SetFinite;
        friend class SetWithOrder;

    protected:
        bool _isCanonical;              ///< set is in canonical representation

    public:
        /**
         * get set, without user defined order
         */
        virtual SetBase *woOrder()			{ return this; }

		/**
		 * get whether the set contains a finite count of elements
		 */
		virtual bool finite() const			{ return true; }

        /**
         * get whether the set is finite and can only hold elements of rank 1 (then set can used as subset in SetRecMult)
         */
        bool finiteRank1()                  { return (finite() && rank1()); }

        /**
		 * get count of elements (only for finite sets)
		 */
        virtual unsigned long cnt() const = 0;

        /**
          * get whether this set is explicitly marked as non-free
          */
        virtual bool markNF() const = 0;

        /**
		 * get whether the set is empty
		 */
        inline bool empty() const			{ return (finite() && cnt() == 0); }

		/**
		 * get minimal rank of tuples within the tuple set
		 */
		virtual unsigned minRank() const	{ return 1; }

		/**
		 * get maximal rank of tuples within the tuple set
         * @param ub        upper bound for return value
		 */
        virtual unsigned maxRank(unsigned ub = UINT_MAX) const	{ return (ub ? 1 : 0); }

		/**
		 * get whether all tupels in the tuple set have the same rank
		 */
		virtual bool oneRank() const		{ return true; }

        /**
         * get whether all tupels in the tuple set have rank 1
         */
        bool rank1() const                  { return (minRank() == 1 && oneRank()); }

		/**
		 * get whether the tuple set contains the null tuple
		 */
        inline bool containNull() const		{ return (minRank() == 0); }

        /**
         * push all tuple divisible parts in a vector
         * @param dtp       vector to push parts in
         * @param uo        consider user order
         * @return          true if parts are pushed to vector; false if set is not tuple divisible
         */
        virtual bool partsToVector(vector<CmplValAuto>& dtp, bool uo) const      { return false; }

        /**
         * get whether the tuple set has an user order of its elements specified direct here
         * (if there is a direct user order, then any user order on subsets is ignored)
		 */
        virtual bool hasDirectUserOrder() const	{ return false; }

        /**
         * get whether some subset of this has an user order
         * (that user order on subsets is ignored, if there is a direct user order)
         */
        virtual bool hasSubUserOrder() const	{ return false; }

        /**
         * get whether the tuple set has an user order of its elements (direct order or on subsets)
         */
        virtual bool hasUserOrder() const		{ return false; }

        /**
         * get whether user order is reverse standard order
         * @param oneNeg    return true if set contains no more than one element
         */
        virtual bool negOrder(bool oneNeg) const		{ return (oneNeg && cnt() <= 1); }

        /**
         * get set with all user order removed
         * @param res		value for result
         */
        virtual void stripOrder(CmplVal& res) = 0;

        /**
         * get whether set is in canonical representation
         */
        bool isCanonical() const                { return _isCanonical; }

	protected:
		/**
		 * set tuple elements for the tuple on a given index position in the set
		 * @param ind			index number within the set
		 * @param tpl			tuple value for inserting the elements
	 	 * @param ti			start element number in tpl
         * @param useOrder		use user order of subsets
         * @param reverse		use reverse order
         * @return				index of the tuple within the set
		 */
        virtual unsigned long tupleAtIntern(unsigned long ind, Tuple *tpl, unsigned ti, bool useOrder, bool reverse) = 0;


        /************** static info functions for sets **********/

    public:
        /**
         * get whether the set contains a finite count of elements
         */
        static bool finite(const CmplVal& v)					{ return (v.useValP() ? v.setBase()->finite() : v.isSetFin()); }

        /**
         * get whether the set is finite and can only hold elements of rank 1 (then set can used as subset in SetRecMult)
         */
        static bool finiteRank1(const CmplVal& v)               { return (v.useValP() ? v.setBase()->finiteRank1() : (v.isSetFin() && v.rank1())); }

        /**
         * get count of elements (only for finite sets)
         */
        static unsigned long cnt(const CmplVal& v);

        /**
          * get whether this set is explicitly marked as non-free
          */
        static inline bool markNF(const CmplVal& v)				{ return (v.useValP() ? v.setBase()->markNF() : v.isSetMnf()); }

        /**
         * @brief set or unset explicit mark as non-free for a set
         * @param v         set
         * @param mnf       true: mark as non-free / false: mark as free
         */
        static void setMarkNF(CmplVal& v, bool mnf);

        /**
         * get whether the set is empty
         */
        static inline bool empty(const CmplVal& v)				{ return (v.t == TP_SET_EMPTY || (v.useValP() && v.setBase()->empty())); }

        /**
         * get minimal rank of tuples within the tuple set
         */
        static inline unsigned minRank(const CmplVal& v)		{ return (v.useValP() ? v.setBase()->minRank() : (!v || v.t == TP_SET_NULL || v.rankAll() ? 0 : 1)); }

        /**
         * get maximal rank of tuples within the tuple set
         * @param ub        upper bound for return value
         */
        static inline unsigned maxRank(const CmplVal& v, unsigned ub = UINT_MAX)		{ return (v.useValP() ? v.setBase()->maxRank(ub) : (!v || !ub || v.t == TP_SET_NULL ? 0 : (v.rankAll() ? ub : 1))); }

        /**
         * get whether all tupels in the tuple set have the same rank
         */
        static inline bool oneRank(const CmplVal& v)			{ return (v.t == TP_SET_FIN || v.t == TP_SET_WITH_UO || v.t == TP_SET_INF_TPL || v.t == TP_SET_INF_SET ? v.setBase()->oneRank() : !v.rankAll()); }

        /**
         * get rank of a set, if the set contains tuples of different rank, then max rank
         */
        static inline unsigned rank(const CmplVal& v)			{ return maxRank(v); }

        /**
         * get whether all tupels in the tuple set have rank 1
         */
        static inline bool rank1(const CmplVal& v)              { return (v.useValP() ? v.setBase()->rank1() : v.rank1()); }

        /**
         * get whether the tuple set contains the null tuple
         */
        static inline bool containNull(const CmplVal& v)		{ return (minRank(v) == 0); }

        /**
         * push all tuple divisible parts of a set value in a vector, or push value itself if it isn't tuple divisible
         * @param v         set value
         * @param dtp       vector to push parts in
         * @param uo        consider user order
         */
        static inline void partsToVector(const CmplVal& v, vector<CmplValAuto>& dtp, bool uo)       { if (!v.useValP() || !v.setBase()->partsToVector(dtp, uo)) { dtp.emplace_back(v); } }

        /**
         * get whether the tuple set has an user order of its elements specified direct here
         * (if there is a direct user order, then any user order on subsets is ignored)
         */
        static inline bool hasDirectUserOrder(const CmplVal& v)	{ return (v.setValHasOrder() && v.setBase()->hasDirectUserOrder()); }

        /**
         * get whether some subset of this has an user order
         * (that user order on subsets is ignored, if there is a direct user order)
         */
        static inline bool hasSubUserOrder(const CmplVal& v)	{ return (v.useValP() && v.setBase()->hasSubUserOrder()); }

        /**
         * get whether the tuple set has an user order of its elements (direct order or on subsets)
         */
        static inline bool hasUserOrder(const CmplVal& v)		{ return (v.useValP() && v.setBase()->hasUserOrder()); }

        /**
         * get whether user order is reverse standard order
         * @param oneNeg    return true if set contains no more than one element
         */
        static inline bool negOrder(const CmplVal& v, bool oneNeg)		{ return (v.useValP() ? v.setBase()->negOrder(oneNeg) : (oneNeg && cnt(v) <= 1)); }

        /**
         * get set with all user order removed
         * @param res		value for result
         */
        static inline void stripOrder(const CmplVal&v, CmplVal& res)	{ if (v.t == TP_SET_WITH_UO || v.t == TP_SET_REC_MULT || v.t == TP_SET_FIN || v.t == TP_SET_INF_TPL) v.setBase()->stripOrder(res); else res.copyFrom(v, true, false); }

        /**
         * get whether set is in canonical representation
         */
        static bool isCanonical(const CmplVal& v);

        /**
         * sets whether the set is in canonical representation
         */
        static inline void setCanonical(CmplVal& v, bool c)            { if (v.useValP()) { v.setBase()->_isCanonical = c; } }
    };


    /**
     * Abstract base class for common properties of set classes
     */
    class SetBase2 : public SetBase
    {
        friend class SetBase;

    protected:
        unsigned long _cnt;		///< count of elements
        bool _markNF;			///< this set is explicitly marked as non-free

    public:
        /**
         * get count of elements (only for finite sets)
         */
        virtual unsigned long cnt() const		{ return _cnt; }

        /**
          * get whether this set is explicitly marked as non-free
          */
        virtual bool markNF() const				{ return _markNF; }
    };


	/**
	 * The <code>SetAlg</code> class represents a finite algorithmic index tuple set of rank 1
	 */
    class SetAlg : public SetBase2
	{
		friend class SetIterator;
		friend class SetRecMult;
		friend class SetFinite;

	protected:
		intType _start;			///< start value (lowest value)
        unsigned long _step;	///< step to next value (greater or equal 1)

		bool _incomplete;		///< incomplete set after the first pseudo function call for cmpl expression like "1(1)(n)" (then is _cnt == 0)

	public:
		/**
		 * constructor
		 */
        SetAlg(intType st, unsigned long sp, unsigned long cnt, bool mnf = false)			{ _start = st; _step = sp; _cnt = cnt; _markNF = mnf; _isCanonical = false; _incomplete = false; }

		/**
		 * constructor for incomplete set
		 */
        SetAlg(intType st, unsigned long sp)												{ _start = st; _step = sp; _cnt = 0; _markNF = false; _isCanonical = false; _incomplete = true; }

		/**
		 * get start value (lowest value)
		 */
		inline intType start()						{ return _start; }

		/**
		 * get step to next value (greater or equal 1)
		 */
		inline unsigned long step()					{ return _step; }

        /**
         * get set with all user order removed
         * @param res		value for result
         */
        virtual void stripOrder(CmplVal& res)		{ res.set(TP_SET_R1_ALG, this); }

		/**
		 * write contents of the object to a stream
		 * @param modp			calling module
		 * @param mode			mode for output: 0=direct; 1=part of other value
		 */
        virtual void write(ostream& ostr, ModuleBase *modp, int mode = 0) const override;

        /**
		 * get whether incomplete set after the first pseudo function call
		 */
		inline bool incomplete() const				{ return _incomplete; }

		/**
		 * check whether the set is equal to another set
		 * @param s2			other set
		 * @return				true if both sets are equal
		 */
        inline bool equal(const SetAlg *s2) const		{ return (_cnt == s2->_cnt && _start == s2->_start && _step == s2->_step); }

        /**
         * get upper or lower bound of an algorithmic set
         * @param set			set (must be algorithmic set)
         * @param ub			true: get upper bound / false: get lower bound
         * @return				upper or lower bound
         */
        static intType bound(const CmplVal& set, bool ub);

        /**
         * get step from one value to the next
         * @param set			set (must be an algorithmic set)
         * @return				step
         */
        static inline intType step(const CmplVal& set)			{ return (set.t == TP_SET_R1_ALG ? set.setAlg()->_step : (intType)1); }

	protected:
		/**
		 * set tuple elements for the tuple on a given index position in the set
		 * @param ind			index number within the set
		 * @param tpl			tuple value for inserting the elements
	 	 * @param ti			start element number in tpl
         * @param useOrder		use user order of subsets
         * @param reverse		use reverse order
         * @return				index of the tuple within the set
		 */
        virtual unsigned long tupleAtIntern(unsigned long ind, Tuple *tpl, unsigned ti, bool useOrder, bool reverse);
	};


	/**
	 * The <code>SetEnum</code> class represents a finite enumeration index tuple set of rank 1
	 */
    class SetEnum : public SetBase2
	{
		friend class SetIterator;
		friend class SetRecMult;
		friend class SetFinite;

	protected:
        intType *_array;		///< array with the elements of the set, in canonical order (length is <code>_cnt</code>)
		unsigned long _cntInt;	///< count of integer elements (thereafter start the string elements in <code>_array</code>)

	public:
		/**
		 * constructor
		 */
        SetEnum(unsigned long cnt, unsigned long ci, intType *arr, bool mnf = false)	{ _cnt = cnt; _cntInt = ci; _array = arr; _markNF = mnf; _isCanonical = false; }

		/**
		 * destructor
		 */
        ~SetEnum()									{ delete _array; }

	public:
        /**
         * get set with all user order removed
         * @param res		value for result
         */
        virtual void stripOrder(CmplVal& res)		{ res.set(TP_SET_R1_ENUM, this); }

        /**
		 * write contents of the object to a stream
		 * @param modp			calling module
		 * @param mode			mode for output: 0=direct; 1=part of other value
		 */
        virtual void write(ostream& ostr, ModuleBase *modp, int mode = 0) const override;

		/**
		 * check whether the set is equal to another set
		 * @param s2			other set
		 * @return				true if both sets are equal
		 */
        bool equal(const SetEnum *s2) const;

        /**
         * get array with the elements of the set, in canonical order
         */
        intType *array() const						{ return _array; }

        /**
         * get count of integer elements
         */
        unsigned long cntInt() const				{ return _cntInt; }

	protected:
		/**
		 * set tuple elements for the tuple on a given index position in the set
		 * @param ind			index number within the set
		 * @param tpl			tuple value for inserting the elements
	 	 * @param ti			start element number in tpl
         * @param useOrder		use user order of subsets
         * @param reverse		use reverse order
         * @return				index of the tuple within the set
		 */
        virtual unsigned long tupleAtIntern(unsigned long ind, Tuple *tpl, unsigned ti, bool useOrder, bool reverse);
	};


	/**
	 * The <code>SetRecMult</code> class represents a finite rectangle index tuple set of a certain rank greater 1
	 */
    class SetRecMult : public SetBase2
	{
		friend class SetIterator;
		friend class SetFinite;

	protected:
        unsigned _rank;			///< rank of tuple set (length of <code>_array</code>) (greater 1)

        CmplVal *_array;		///< sets of rank 1 for the different dimensions (can be only of the following types: TP_SET_R1_ENUM, TP_SET_R1_ALG, TP_SET_R1_0_UB, TP_SET_R1_1_UB, TP_SET_1INT, TP_SET_1STR, TP_INT, TP_STR)
		unsigned long *_cnts;	///< counts of sets in _array (same length as <code>_array</code>)

        bool _subOrder;			///< user order on a subset

	public:
		/**
		 * constructor
		 * @param rank			rank of set (count of elements in arr)
		 * @param arr			array with part sets
		 */
		SetRecMult(unsigned rank, CmplVal *arr);

		/**
         * constructor: create set from two other sets (not usable if s1 or s2 is TP_SET_REC_MULT with direct order!)
		 * @param s1			first source set (must be one of TP_SET_R1_ENUM, TP_SET_R1_ALG, TP_SET_R1_0_UB, TP_SET_R1_1_UB, TP_SET_1INT, TP_SET_1STR, TP_INT, TP_STR or TP_SET_REC_MULT)
		 * @param s2			second source set (must be one of TP_SET_R1_ENUM, TP_SET_R1_ALG, TP_SET_R1_0_UB, TP_SET_R1_1_UB, TP_SET_1INT, TP_SET_1STR, TP_INT, TP_STR or TP_SET_REC_MULT)
         * @param woOrder       without user order
		 */
        SetRecMult(CmplVal& s1, CmplVal& s2, bool woOrder = false);

        /**
         * constructor: create set from two other sets (use instead of previous constructor to handle if s1 or s2 is TP_SET_REC_MULT with direct order!)
         * @param s1			first source set (must be one of TP_SET_R1_ENUM, TP_SET_R1_ALG, TP_SET_R1_0_UB, TP_SET_R1_1_UB, TP_SET_1INT, TP_SET_1STR, TP_INT, TP_STR or TP_SET_REC_MULT)
         * @param s2			second source set (must be one of TP_SET_R1_ENUM, TP_SET_R1_ALG, TP_SET_R1_0_UB, TP_SET_R1_1_UB, TP_SET_1INT, TP_SET_1STR, TP_INT, TP_STR or TP_SET_REC_MULT)
         * @param tp            return of type of result
         * @param woOrder       without user order
         */
        static SetBase* construct(CmplVal& s1, CmplVal& s2, tp_e &tp, bool woOrder = false);

        /**
         * constructor: create set with only one tuple as element
         * @param tup			element for the set
         * @param tps			start index of used tuple part
         * @param toSet			convert scalar index values to sets
         */
        SetRecMult(const Tuple *tup, unsigned tps = 0, bool toSet = false);

		/**
		 * destructor
		 */
        ~SetRecMult()								{ for (unsigned i = 0; i < _rank; i++) { _array[i].dispose(); } delete[] _array; delete[] _cnts; }

	public:
        /**
         * set _cnt and other instance variables after changing _array
         * @param rnk       also _rank may be changed
         */
        void setCnt(bool rnk);

		/**
		 * get minimal rank of tuples within the tuple set
		 */
		virtual unsigned minRank() const			{ return _rank; }

		/**
		 * get maximal rank of tuples within the tuple set
         * @param ub        upper bound for return value
         */
        virtual unsigned maxRank(unsigned ub = UINT_MAX) const			{ return min(_rank, ub); }

        /**
         * push all tuple divisible parts in a vector
         * @param dtp       vector to push parts in
         * @param uo        consider user order
         * @return          true if parts are pushed to vector; false if set is not tuple divisible
         */
        virtual bool partsToVector(vector<CmplValAuto> &dtp, bool uo) const;

		/**
		 * write contents of the object to a stream
		 * @param modp			calling module
		 * @param mode			mode for output: 0=direct; 1=part of other value
		 */
        virtual void write(ostream& ostr, ModuleBase *modp, int mode = 0) const override;

        /**
         * get whether some subset of this has an user order
         * (that user order on subsets is ignored, if there is a direct user order)
         */
        virtual bool hasSubUserOrder() const		{ return _subOrder; }

        /**
         * get whether the tuple set has an user order of its elements (direct order or on subsets)
         */
        virtual bool hasUserOrder() const			{ return _subOrder; }

        /**
         * get whether user order is reverse standard order
         * @param oneNeg    return true if set contains no more than one element
         */
        virtual bool negOrder(bool oneNeg) const;

        /**
         * get set with all user order removed
         * @param res		value for result
         */
        virtual void stripOrder(CmplVal& res);

        /**
         * get one rank 1 part of the set
         * @param i				index of the part
         * @return				rank 1 part of the set
         */
        inline CmplVal *partSet(unsigned i) const		{ return _array + i; }

        /**
		 * get one rank 1 part of the set
		 * @param i				index of the part
         * @param woOrder		strip user order from result
		 * @return				rank 1 part of the set
		 */
        CmplVal *partSet(unsigned i, bool woOrder) const;

		/**
		 * set part of the set as another set
		 * @param res			store for result set
		 * @param i				part from this index
		 * @param j				part up to this index
         * @param woOrder		strip user order from result
         */
        void partSet(CmplVal& res, unsigned i, unsigned j, bool woOrder = false);

        /**
         * get count of part set
         * @param i             index of the part
         * @return              count of elements of the part
         */
        inline unsigned long partCnt(unsigned i) const          { return _cnts[i]; }

	protected:
		/**
		 * set tuple elements for the tuple on a given index position in the set
		 * @param ind			index number within the set
		 * @param tpl			tuple value for inserting the elements
	 	 * @param ti			start element number in tpl
         * @param useOrder		use user order of subsets
         * @param reverse		use reverse order
         * @return				index of the tuple within the set
		 */
        virtual unsigned long tupleAtIntern(unsigned long ind, Tuple *tpl, unsigned ti, bool useOrder, bool reverse);

    private:
        /**
         * recursive copy of user order to an index array
         * @param nr			number of part set
         * @param ord			array to copy user order to
         * @param ob			start index within <code>ord</code>, return current index
         * @param ib			start of set index to put in <code>ord</code>
         */
        void copyOrderRec(unsigned nr, unsigned long *ord, unsigned long& ob, unsigned long ib) const;
	};


	/**
	 * The <code>SetFinite</code> class represents an arbitrary finite index tuple set
	 */
    class SetFinite : public SetBase2
	{
		friend class SetIterator;

	protected:
		CmplVal _base;			///< set for the first dimension(s) (TP_SET_R1_ENUM, TP_SET_R1_ALG, TP_SET_R1_0_UB, TP_SET_R1_1_UB, TP_SET_1INT, TP_SET_1STR, TP_INT, TP_STR or TP_SET_REC_MULT)
        unsigned long _baseCnt;	///< count of set <code>_base</code> (length of arrays <code>_array</code>, <code>_index</code> and <code>_indexUseOrder</code>)

        CmplVal *_array;		///< per element of <code>_base</code> the set for the remaining dimensions (length is count of elements of set <code>_base</code>) (elements can be of any finite set type, or TP_EMPTY if no remaining dimension)
		unsigned long *_index;	///< cumulated count of elements in subsets in <code>_array</code> / NULL: not computed yet

        unsigned long *_indexUseOrder;	///< only used if _base has an user order: like <code>_index</code>, but in user order of _base / NULL: not computed yet
        bool _subOrder;			///< user order on a subset (_base or _array)

		unsigned _minRank;		///< minimal rank in this tuple set. if 0, then the set contains in addition to the elements coded in <code>_base</code> and <code>_array</code> also the null tuple
		unsigned _maxRank;		///< maximal rank in this tuple set
		unsigned _baseRank;		///< rank of set <code>_base</code>

        unsigned _baseSplit;    ///< only for <code>_baseRank</code> greater than 1: rank of remaining upper part of set <code>_base</code> after the split of a maximal lower part (greater or equal _baseRank: no splitable part / 0: not known)
        unsigned long _modArray;///< only if a splitable lower part of set <code>_base</code>: <code>_array</code> has this length, do all indexation i by <code>_array[i % _modArray]</code> / 0: not so

	public:
		/**
		 * constructor
		 * @param bs			set for the first dimension(s)
		 * @param arr			per element of <code>bs</code> the set for the remaining dimensions
		 * @param minR			minimal rank
		 * @param maxR			maximal rank
		 */
        SetFinite(CmplVal& bs, CmplVal *arr, unsigned minR, unsigned maxR);

        /**
         * constructor: from another set
         * @param set			set to construct from, act here as base set (must be a finite set, but not the empty set and not the null tuple set)
         * @param woOrder		strip user order
         * @param splt          value for <code>_baseSplit</code> or 0 if unknown
         * @param addNull		add null tuple to resulting finite set (append if <code>ord</code>)
         * @param rmi			min rank for the result set / 0: not specified
         * @param rma			max rank for the result set / 0: not specified
         */
        SetFinite(const CmplVal &set, bool woOrder, unsigned splt, bool addNull, unsigned rmi, unsigned rma);

		/**
		 * destructor
		 */
		~SetFinite();

	public:
		/**
		 * get minimal rank of tuples within the tuple set
		 */
        virtual unsigned minRank() const		{ return _minRank; }

		/**
		 * get maximal rank of tuples within the tuple set
         * @param ub        upper bound for return value
         */
        virtual unsigned maxRank(unsigned ub = UINT_MAX) const		{ return min(_maxRank, ub); }

        /**
         * get rank of set <code>_base</code>
         */
        unsigned baseRank() const				{ return _baseRank; }

        /**
         * get rank of remaining upper part of set <code>_base</code> after the split of a maximal lower part / 0: not known
         * @param uo        no split if base set has direct user order
         */
        unsigned baseSplit(bool uo = false) const		{ return (_baseRank == 1 || _baseSplit >= _baseRank || (uo && SetBase::hasDirectUserOrder(_base) && SetBase::hasUserOrder(_base)) ? _baseRank : _baseSplit); }

		/**
		 * get whether all tupels in the tuple set have the same rank
		 */
        virtual bool oneRank() const			{ return (_minRank == _maxRank); }

		/**
		 * get whether the tuple set contains the null tuple
		 */
        virtual bool containNull() const		{ return (_minRank == 0); }

        /**
         * get whether the tuple set or recursively one of the subsets contains the null tuple
         */
        bool containNullSub() const;

        /**
         * push all tuple divisible parts in a vector
         * @param dtp       vector to push parts in
         * @param uo        consider user order
         * @return          true if parts are pushed to vector; false if set is not tuple divisible
         */
        virtual bool partsToVector(vector<CmplValAuto> &dtp, bool uo) const;

        /**
         * set min rank and max rank for this set
         * @param ss		set rank after changing only this subset / NULL: check all subsets
         * @param pss		previous state of changed subset / NULL: change of <code>ss</code> is only adding additional elements
         */
        void setRank(CmplVal *ss = NULL, CmplVal *pss = NULL);

        /**
         * set count of elements new for this set, by counting the elements of the subsets
         */
        void setCount();

		/**
		 * write contents of the object to a stream
		 * @param modp			calling module
		 * @param mode			mode for output: 0=direct; 1=part of other value
		 */
        virtual void write(ostream& ostr, ModuleBase *modp, int mode = 0) const override;

        /**
         * get whether some subset of this has an user order
         * (that user order on subsets is ignored, if there is a direct user order)
         */
        virtual bool hasSubUserOrder() const		{ return _subOrder; }

        /**
         * get whether the tuple set has an user order of its elements (direct order or on subsets)
         */
        virtual bool hasUserOrder() const			{ return _subOrder; }

        /**
         * get whether user order is reverse standard order
         * @param oneNeg    return true if set contains no more than one element
         */
        virtual bool negOrder(bool oneNeg) const;

        /**
         * get set with all user order removed
         * @param res		value for result
         */
        virtual void stripOrder(CmplVal& res);

        /**
		 * get base set
		 */
        inline CmplVal *baseSet()                       { return &_base; }

        /**
         * get base set
         * @param woOrder		strip user order from result
         */
        const CmplVal &baseSet(bool woOrder) const;

		/**
		 * get count of elements in base set (also number of subsets)
         * @param md            return <code>_modArray</code> if set
		 */
        inline unsigned long baseCnt(bool md = false) const			{ return (md && _modArray ? _modArray : _baseCnt); }

		/**
		 * get one subset of the set
		 * @param i				index of the subset
		 * @return				subset
		 */
        inline CmplVal *subSet(unsigned long i) const	{ return _array + (_modArray ? (i % _modArray) : i); }

        /**
         * get one subset of the set
         * @param i				index of the subset
         * @param woOrder		strip user order from result
         * @return				subset
         */
        CmplVal& subSet(unsigned long i, bool woOrder) const;

        /**
         * set <code>_baseSplit</code> and <code>_modArray</code>
         * @param splt          value for <code>_baseSplit</code>
         * @param modarr        set also <code>_modArray</code>
         */
        void setBaseSplit(unsigned splt, bool modarr = false);

    protected:
		/**
		 * set tuple elements for the tuple on a given index position in the set
		 * @param ind			index number within the set
		 * @param tpl			tuple value for inserting the elements
	 	 * @param ti			start element number in tpl
         * @param useOrder		use user order of subsets
         * @param reverse		use reverse order
         * @return				index of the tuple within the set
		 */
        virtual unsigned long tupleAtIntern(unsigned long ind, Tuple *tpl, unsigned ti, bool useOrder, bool reverse);

		/**
		 * fills <code>_index</code>
		 */
        unsigned long *fillIndex();

    public:
        /**
         * get _index, fill it if neccessary
         */
        unsigned long *index()									{ return _index ?: fillIndex(); }

    protected:
        /**
		 * fills <code>_indexUseOrder</code>
		 */
        unsigned long *fillIndexUseOrder();

        /**
         * get _indexUseOrder, fill it if neccessary
         */
        unsigned long *indexUseOrder()							{ return _indexUseOrder ?: fillIndexUseOrder(); }

    public:
        /**
         * get start index of subset elements of the base element with given index
         * @param i				index of the base element
         * @return 				start index of subset elements of the base element
         */
        inline unsigned long indSubStart(unsigned long i)		{ if (!_index) { fillIndex(); } return (i ? _index[i-1] : 0) + (_minRank == 0 ? 1 : 0); }
	};


    /**
     * class to extend a set by an user defined order
     */
    class SetWithOrder : public SetBase
    {
    protected:
        CmplVal _set;			///< set

        bool _negOrder;			///< if true, then the user order is the reverse standard order
        unsigned long *_order;	///< indizes of the set elements in user order (only used if !_negOrder)
            // if !_negOrder && !_order: then the user order is equal to the standard order, and this object only masks user orders in subsets

    public:
        /**
         * constructor: order equal to the standard order
         * @param s			set to extend by the user order
         */
        inline SetWithOrder(const CmplVal& s)                         { _set.copyFrom(s, true, false); _negOrder = false; _order = NULL; _isCanonical = false; }

        /**
         * constructor: reverse order
         * @param s			set to extend by the user order
         * @param negOrder	use reverse standard order
         */
        inline SetWithOrder(const CmplVal& s, bool negOrder)          { _set.copyFrom(s, true, false); _negOrder = negOrder; _order = NULL; _isCanonical = false; }

        /**
         * constructor: order by array of indizes
         * @param s			set to extend by the user order
         * @param order		array of indizes
         */
        inline SetWithOrder(const CmplVal& s, unsigned long *order)   { _set.copyFrom(s, true, false); _negOrder = false; _order = order; _isCanonical = false; }

        /**
          * destructor
          */
        inline ~SetWithOrder()				{ _set.dispose(); if (_order) delete[] _order; }

        /**
         * get contained set value
         */
        inline const CmplVal& set() const	{ return _set; }

        /**
         * get contained set value
         */
        inline CmplVal& set()               { return _set; }

        /**
         * get set, without user defined order
         */
        virtual SetBase *woOrder() const	{ return (_set.useValP() ? _set.setBase() : NULL); }

        /**
         * get count of elements (only for finite sets)
         */
        virtual unsigned long cnt() const	{ return SetBase::cnt(_set); }

        /**
         * get whether this set is explicitly marked as non-free
         */
        virtual bool markNF() const			{ return SetBase::markNF(_set); }

        /**
         * get minimal rank of tuples within the tuple set
         */
        virtual unsigned minRank() const	{ return SetBase::minRank(_set); }

        /**
         * get maximal rank of tuples within the tuple set
         * @param ub        upper bound for return value
         */
        virtual unsigned maxRank(unsigned ub = UINT_MAX) const	{ return SetBase::maxRank(_set, ub); }

        /**
         * get whether all tupels in the tuple set have the same rank
         */
        virtual bool oneRank() const		{ return SetBase::oneRank(_set); }

        /**
         * push all tuple divisible parts in a vector
         * @param dtp       vector to push parts in
         * @param uo        consider user order
         * @return          true if parts are pushed to vector; false if set is not tuple divisible
         */
        virtual bool partsToVector(vector<CmplValAuto>& dtp, bool uo) const     { return (!_set.useValP() || (uo && hasUserOrder()) ? false : _set.setBase()->partsToVector(dtp, false)); }

        /**
         * get whether the tuple set has an user order of its elements specified direct here
         * (if there is a direct user order, then any user order on subsets is ignored)
         */
        virtual bool hasDirectUserOrder() const		{ return true; }

        /**
         * get whether some subset of this has an user order
         * (that user order on subsets is ignored, if there is a direct user order)
         */
        virtual bool hasSubUserOrder() const		{ return (_set.useValP() && _set.setBase()->hasUserOrder()); }

        /**
         * get whether the tuple set has an effective user order of its elements (direct order or on subsets)
         */
        virtual bool hasUserOrder() const			{ return (_negOrder || _order); }

        /**
         * get whether user order is reverse standard order
         * @param oneNeg    return true if set contains no more than one element
         */
        virtual bool negOrder(bool oneNeg) const    { return (_negOrder || (oneNeg && cnt() <= 1)); }

        /**
         * get whether user order is reverse standard order
         */
        bool negOrder() const                       { return _negOrder; }

        /**
         * get set with all user order removed
         * @param res		value for result
         */
        virtual void stripOrder(CmplVal& res)		{ SetBase::stripOrder(_set, res); }

        /**
         * write contents of the object to a stream
         * @param modp			calling module
         * @param mode			mode for output: 0=direct; 1=part of other value
         */
        virtual void write(ostream& ostr, ModuleBase *modp, int mode = 0) const override;

        /**
         * check whether the specified order is equal to another set order
         * @param s2			other set
         * @return				true if both sets are equal
         */
        bool equalOrder(const SetWithOrder *s2) const;

    protected:
        /**
         * set tuple elements for the tuple on a given index position in the set
         * @param ind			index number within the set
         * @param tpl			tuple value for inserting the elements
         * @param ti			start element number in tpl
         * @param useOrder		use user order of subsets
         * @param reverse		use reverse order
         * @return				index of the tuple within the set
         */
        virtual unsigned long tupleAtIntern(unsigned long ind, Tuple *tpl, unsigned ti, bool useOrder, bool reverse);

    public:
        /**
         * get indizes of set elements in user order / NULL: no user order specified or user order is reverse standard order
         */
        inline unsigned long *order() const			{ return _order; }

        /**
         * set or delete reverse standard order
         */
        inline void setNegOrder(bool o)				{ _negOrder = o; if (_order) { delete[] _order; _order = NULL; } }

        /**
         * set user order by index array
         * @param o				array with direct user order, length must match tuple count
         */
        inline void setOrder(unsigned long *o)		{ setNegOrder(false); _order = o; }

        /**
         * allocate new index array for user order
         * @param init			initialize array with standard order (if false, then the caller must fill the array before the set can be used!)
         * @return				index array
         */
        unsigned long *newOrder(bool init);
    };

}

#endif // SETBASE_HH

