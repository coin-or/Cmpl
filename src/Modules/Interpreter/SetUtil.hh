
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


#ifndef SETUTIL_HH
#define SETUTIL_HH

#include <map>

#include "../../CommonData/CmplVal.hh"
#include "../../CommonData/Tuple.hh"
#include "../../CommonData/SetBase.hh"


using namespace std;



namespace cmpl
{
	class Interpreter;
	class ExecContext;
	class StackValue;


	/**
	 * class for static utility functions for tuple sets
	 */
	class SetUtil
    {

        /************** set construction **********/

	public:
		/**
		 * construct tuple set of rank 1 from an interval
		 * @param ctx			execution context
		 * @param res			store for result set
		 * @param iv			interval to construct set from
		 */
        static void constructFromInterval(ExecContext *ctx, CmplVal &res, const CmplVal &iv);

		/**
		 * construct algorithmic tuple set of rank 1
		 * @param ctx			execution context
		 * @param res			store for result set
		 * @param start			first value
		 * @param step			step to next value (must not be 0)
		 * @param end			last value / NULL: construct incomplete set without last value
		 */
		static void constructAlg(ExecContext *ctx, CmplVal &res, StackValue *start, StackValue *step, StackValue *end);

		/**
		 * complete construction of an algorithmic tuple set of rank 1
		 * @param ctx			execution context
		 * @param res			store for result set
		 * @param sb			set to complete (must be an incomplete set without last value)
		 * @param end			last value
		 */
        static void constructAlg(ExecContext *ctx, CmplVal &res, SetBase *sb, StackValue *end);

		/**
		 * construction of an algorithmic tuple set of rank 1
		 * @param res			store for result set
		 * @param st			start value
		 * @param en			end value
		 * @param sp			step value (must be greater or equal 1)
		 * @param no			reverse order
		 */
		static void constructAlg(CmplVal &res, intType st, intType en, unsigned long sp, bool no);


        /************** set comparison **********/

		/**
		 * compare two sets for complete equality and for an equal base (not for infinite sets)
         * @param s1a			first set
         * @param s2a			second set
		 * @param eqr			check equal base up to this rank, return up to which rank the bases are equal
		 * @param cc			check also for complete equality
		 * @param uo			check user order
		 * @return				true if both sets are complete equal (if !cc then return ever false)
		 */
        static bool compareEqRank(CmplVal *s1a, CmplVal *s2a, unsigned& eqr, bool cc, bool uo);

        /**
         * compare two sets for complete equality
         * @param s1			first set
         * @param s2			second set
         * @param uo			check user order
         * @return				true if both sets are complete equal
         */
        static bool compareEq(CmplVal *s1, CmplVal *s2, bool uo = true)			{ unsigned eqr = 0; return compareEqRank(s1, s2, eqr, true, uo); }

        /**
         * test whether first set is a subset of second set
         * @param ctx			execution context
         * @param s1            first set
         * @param s2            second set
         * @return              true if first set is a subset of second set
         */
        static bool isSubSet(ExecContext *ctx, CmplVal *s1, CmplVal *s2);


        /************** get parts of sets **********/

        /**
		 * get base set from a given set (not for infinite sets)
		 * @param res			store for result set
		 * @param set			set for getting the base from
		 * @param br			rank for base set
		 */
		static void getBaseSet(CmplVal &res, CmplVal& set, unsigned br);

		/**
		 * get first remaining subset of a set after removing a set base, and check whether all other subsets would be equal (not for infinite sets)
		 * @param res			store for result subset
		 * @param s				source set
		 * @param br			rank of the base to remove
		 * @param uo			check user order in subsets
		 * @return				true if the other subsets would be equal to the first one
		 */
		static bool subSetFirst(CmplVal *res, CmplVal& s, unsigned br, bool uo);

		/**
		 * get the i-th remaining subset of a set after removing a set base (not for infinite sets)
		 * @param res			store for result subset
		 * @param s				source set
		 * @param br			rank of the base to remove
		 * @param i				index number of subset
		 */
		static void subSetN(CmplVal *res, CmplVal& s, unsigned br, unsigned long i);

		/**
		 * fill the array of subsets for <code>SetFinite</code>, and move part of subsets to the base set if possible.
		 * @param bs			base set, changed by this function if eqr > 0
		 * @param eqc			true if all subsets are equal
		 * @param eqr			rank of subset part to move to the base set
         * @param rev			reverse order of subsets in <code>subs</code>
		 * @param subs			subsets, key must be numbered from 1, if a searched key is not contained the corresponding subset is treated as null
		 * @param nNull			index of null element, for this number no subset is searched in subs / 0 if no null element
		 */
        static CmplVal *arrayForFinSet(CmplVal& bs, bool eqc, unsigned eqr, bool rev, map<unsigned long, CmplVal>& subs, unsigned long nNull);


        /************** search in sets **********/

        /**
         * search a tuple or a part of a tuple within a set
         * @param ctx			execution context
         * @param seta			set to search the tuple within
         * @param tup			tuple to search (can also be TP_LIST_TUPLE, then it must be the topmost element on the value stack of the execution context)
         * @param num			return of the index number of the tuple within the set (only if set is finite)
         * @param tps			start index of used tuple part
         * @param tpc			max count of indizes of used tuple part (searching for the longest tuple part not longer than tpc) / -1: up to the end of tuple (searching only for the full remaining tuple)
         * @param fulltp        search only for tuple length tpc
         * @return 				count of indizes of found tuple part + 1
         */
        static unsigned tupleInSet(ExecContext *ctx, const CmplVal &seta, const CmplVal &tup, unsigned long& num, unsigned tps = 0, int tpc = -1, bool fulltp = false);

        /**
         * get set for operation "in" or "of"
         * @param ctx           execution context
         * @param set           return result set
         * @param src           source value (right operand in operation "in" or "of")
         * @param opIn          operation "in" (true) or "of" (false)
         * @param toTuple       allow tuple as result
         * @return              source value is appropriate
         */
        static bool setForInOrOf(ExecContext *ctx, CmplVal& set, CmplVal& src, bool opIn, bool toTuple = false);

        /**
         * get set of valid elements for array
         * @param ctx           execution context
         * @param vs            return set
         * @param arr           source array
         */
        static void getArrayValidSet(ExecContext *ctx, CmplVal& vs, CmplArray *arr);


        /************** add tuple to set **********/

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
        static void addSet(ExecContext *ctx, CmplVal *res, unsigned se, CmplVal *a1, CmplVal *a2, bool ord, bool tcr);

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
        static unsigned long addTupleToSet(ExecContext *ctx, CmplVal &res, const CmplVal &seta, const CmplVal &tup, bool ord, bool tcr, unsigned tps = 0);

    private:
        /**
         * create a new SetFinite as copy of another set, with reduced base and/or added base element
         * @param ctx			execution context
         * @param set			source set (must be a finite set)
         * @param br			max base rank for result set
         * @param tpa			element to add to base set: must have rank 1 and must not be already part of the base set / NULL: don't add element
         * @param num			return of the index number of the added element (not changed if !tpa)
         * @return				result set
         */
        static SetFinite *copyAsFinSet(ExecContext *ctx, const CmplVal &set, unsigned br, const CmplVal *tpa, unsigned long& num);

    public:
        /**
         * convert given set to the aequivalent set in canonical representation
         * @param res			return of result set
         * @param seta			source set (must be a finite set)
         * @param woOrder       strip order from set
         * @return				true if result set is computed; false if source set is already in canonical representation
         */
        static bool canonicalSet(CmplVal &res, CmplVal &seta, bool woOrder = false);

        /**
         * check if split of base set of SetFinite is possible, set <code>_baseSplit</code> within SetFinite </code>
         * @param set			source set (must be TP_SET_FIN)
         * @param uo            consider user order
         * @param modarr        set also <code>_modArray</code> within SetFinite
         * @param force         check split also if set is already in canonical representation
         * @return              true if split is possible
         */
        static bool checkBaseSplitFinSet(CmplVal &set, bool uo, bool modarr = false, bool force = false);

    private:
        /**
         * set user order in canonical representation for a set
         * @param res           result set
         * @param set           source set (without user order), if empty then use set from <code>setOrder</code>
         * @param setOrder      original user order for the set / NULL: no original user order exists
         * @param orderArr      if not NULL then use this user order instead of the one from <code>setOrder</code> (if given then <code>set</code> must not be empty) (if given then this function will consume or delete this array)
         * @return              true if new result set / false if source set is already equal to result
         */
        static bool canonicalOrder(CmplVal &res, const CmplVal &set, const SetWithOrder *setOrder, unsigned long *orderArr);

        /**
         * set order for set, if necessary then recursive for subsets
         * @param res           result set
         * @param set           source set (without user order)
         * @param orderArr      array with user order
         * @return              true if new result set / false if source set is already equal to result
         */
        static bool setOrderArrRec(CmplVal &res, const CmplVal &set, const unsigned long *orderArr);

        /**
         * set reverse order for set, if necessary then recursive for subsets
         * @param res           result set
         * @param seta          source set
         */
        static void setOrderNegRec(CmplVal &res, const CmplVal &seta);


        /************** utility functions **********/

    public:
        /**
         * try to convert a value to a set or an index tuple
         * @param ctx			execution context
         * @param res			return of result value (with type TP_SET_* or TP_ITUPLE*)
         * @param src           source value
         * @param tcl           conversion level
         * @param toTuple       if false then convert only to set but not to tuple
         * @return              true if conversion is successful
         */
        static bool convertToSetOrTuple(ExecContext *ctx, CmplVal& res, CmplVal &src, TypeConversionLevel tcl, bool toTuple = true);

        /**
         * return new user order array for merging user orders of the source sets of a tuple
         * @param ctx			execution context
         * @param src           vector of source values (must be finite sets, intervals, or single index values)
         * @return              new array with resulting user order
         */
        static unsigned long* tupleMergeOrder(ExecContext *ctx, const vector<const CmplVal *>& src);

        /**
         * recursive fill array for merged user order
         * @param dst           next position in destination array, incremented by the function
         * @param ind           current index in argument arrays
         * @param sCnt          array with count of elements per source set
         * @param sMultCnt      array with count of elements multiplied over all source sets with above <code>ind</code>, or 0 if no further recursion
         * @param sOrd          array of user order arrays per source set
         * @param ofs           current offset for destination user order
         */
        static void tupleMergeOrderRek(unsigned long* &dst, unsigned ind, vector<unsigned long>& sCnt, vector<unsigned long>& sMultCnt, vector<unsigned long *>& sOrd, unsigned long ofs);

        /**
         * convert tuple tpl to set
         * @param ctx           execution context
         * @param res           return of result value
         * @param tpl           tuple (all parts must be scalar index values or non-empty sets or intervals, and not be infinite)
         * @param tcr			transform result set to canonical set representation
         * @param uo            keep user order
         */
        static void tupleToSetFin(ExecContext *ctx, CmplVal& res, Tuple *tpl, bool tcr, bool uo = true);

        /**
         * convert tuple tpl to set
         * @param ctx           execution context
         * @param res           return of result value
         * @param tpl           tuple (all parts must be scalar index values or non-empty sets or intervals, and at least one part should be infinite)
         * @param tcr			transform result set to canonical set representation
         * @param uo            keep user order
         */
        static void tupleToSetInf(ExecContext *ctx, CmplVal& res, Tuple *tpl, bool tcr, bool uo = true);

        /**
         * get lower and upper bound of integer elements within a rank 1 set
         * @param low           return of lower bound (can be only TP_INT or TP_INFINITE)
         * @param upp           return of upper bound (can be only TP_INT or TP_INFINITE)
         * @param src           source set
         * @return              false if set is not a rank 1 set
         */
        static bool rank1IntBounds(CmplVal& low, CmplVal& upp, const CmplVal& src);
    };


	/**
	 * class for constructing of a finite set
	 */
	class SetConstructHelper
	{
	private:
		Interpreter *_modp;				///< interpreter module

		tp_e _mode;						///< current mode for set in construction (if subsets exists then for the base set with rank 1), can be:
											// TP_SET_EMPTY: no element (start of construction) or only the null tuple
											// TP_SET_1INT / TP_SET_1STR: one element
											// TP_SET_R1_ALG: algorithmic set
											// TP_SET_R1_ENUM: enumeration set
        bool _rev;						///< if true then the elements are added from the last to the first
        unsigned _tpBaseInd;			///< use tupel part from this element index (for recursive set construction)

        unsigned long _cnt;				///< count of elements
        unsigned _minRank;				///< minimal rank of elements
        unsigned _maxRank;				///< maximal rank of elements

        intType _algStart;				///< start element in mode TP_SET_R1_ALG (also the only element in TP_SET_1INT / TP_SET_1STR)
        intType _algDiff;				///< difference between two adjacent elements in mode TP_SET_R1_ALG
        unsigned long _nNull;			///< index number of the null tuple if exists in set (count from 1; 0: no null tuple)

        map<intType, unsigned long> _enumInt;	///< int elements for mode TP_SET_R1_ENUM (value of map counts from 1)
        map<intType, unsigned long> _enumStr;	///< string elements for mode TP_SET_R1_ENUM (value of map counts from 1)

        map<unsigned long, CmplVal> _subSet;				///< subset per number of base element (if subset are directly given) (key of map counts from 1)

        //map<unsigned long, SetConstructHelper *> _subCtx;	///< subset per number of base element (if subset is to construct) (key of map counts from 1)
            // Offenbar nie verwendet, kann wohl ganz weg

	public:
		/**
		 * constructor
		 * @param modp			interpreter module
		 * @param rev			add elements in reverse order
		 */
		SetConstructHelper(Interpreter *modp, bool rev): _modp(modp), _mode(TP_SET_EMPTY), _rev(rev), _tpBaseInd(0), _cnt(0), _minRank(1), _maxRank(1), _nNull(0)		{ }

		/**
		 * destructor
		 */
		~SetConstructHelper();

		/**
		 * add element for the set
		 * @param v			element to add
         * @return			true if element is added, false if element already exists in the set
		 */
        bool add(CmplVal &v);

        /**
         * add int element for the set
         * @param i			element to add
         * @return			true if element is added, false if element already exists in the set
         */
        bool add(intType i);

		/**
		 * add a base element and the subset for it
		 * @param i			base element
		 * @param isInt		base element is int or is string
		 * @param subSet	subset for the base element
		 * @return			true if base element and subset are added, false if base element already exists in the set or subset is empty
		 */
		bool addBaseAndSubset(intType i, bool isInt, CmplVal& subSet);

		/**
		 * construct resulting set
		 * @param res		store for result set
		 */
		void construct(CmplVal &res);

        /**
         * construct resulting set with no index from base elements
         * @param ctx		execution context
         * @param res		store for result set
         * @param se		syntax element id of source values
         * @param arr       reorder elements in this array according result set
         * @return          true if success, false if indizes from subsets are not unique
         */
        bool constructWoi(ExecContext *ctx, CmplVal &res, unsigned se, CmplVal *arr = NULL);

    private:
        /**
         * add int element for the set
         * @param i			element to add
         * @return			true if element is added, false if element already exists in the set
         */
        bool addInt(intType i);

		/**
		 * add string element for the set
		 * @param i			element to add
		 * @return			true if element is added, false if element already exists in the set
		 */
		bool addStr(intType i);

		/**
		 * add null tuple for the set
		 * @return			true if element is added, false if element already exists in the set
		 */
		bool addNull();

		/**
		 * change mode to TP_SET_R1_ENUM
		 * (current mode must be one of TP_SET_1INT, TP_SET_1STR or TP_SET_R1_ALG)
		 */
		void changeToModeEnum();

		/**
		 * construct resulting base set
		 * @param bs		store for result set
		 */
		void constructBase(CmplVal &bs);
	};

}

#endif // SETUTIL_HH

