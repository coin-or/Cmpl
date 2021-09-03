
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


#ifndef TUPLEUTIL_HH
#define TUPLEUTIL_HH

#include <vector>

#include "../../CommonData/CmplVal.hh"
#include "../../CommonData/Tuple.hh"


using namespace std;



namespace cmpl
{
	class ExecContext;
	class StackValue;
    class CBAssignInfoComplex;
    class CBAssignInfoBasis;


	/**
	 * class for static utility functions for tuples
	 */
	class TupleUtil
	{
		/****** tuple construction ****/
	public:
		/**
		 * construct a tuple from stack values
		 * @param ctx			execution context
		 * @param res			store for result tuple
         * @param resPush       return whether res is to push to the stack
		 * @param cnt			count of components for tuple on stack
         * @param listTuple		if tuple is an index tuple than keep it on the stack as TP_LIST_TUPLE
		 * @param top			topmost component for tuple on stack / NULL: if cnt==0
		 * @return				stack element to pop to before pushing result tuple / NULL: no element to pop
		 */
        static StackValue *construct(ExecContext *ctx, CmplVal &res, bool &resPush, unsigned cnt, bool listTuple, StackValue *top);

		/**
		 * construct an index tuple from a TP_LIST_TUPLE value on the stack
		 * @param ctx			execution context
		 * @param res			store for result tuple
		 * @param lst			TP_LIST_TUPLE value on stack
		 */
		static void constructFromList(ExecContext *ctx, CmplVal &res, StackValue *lst);

        /**
         * convert tuple src to index tuple or to tuple set
         * @param ctx			execution context
         * @param res			store for result tuple or set
         * @param src			value to convert
         * @param listTuple		accept tuple as list
         */
        static void toIndexTuple(ExecContext *ctx, CmplVal &res, StackValue *src, bool listTuple);

        /**
         * convert tuple tpl to index tuple or to tuple set
         * @param ctx			execution context
         * @param res			store for result tuple or set
         * @param tpl			tuple to convert
         * @return              true if conversion is successful
         */
        static bool toIndexTuple(ExecContext *ctx, CmplVal &res, Tuple *tpl);


        /**
         * convert tuple or value to its canonical tuple form
         * @param ctx			execution context
         * @param res           store for result tuple
         * @param src           source tuple or value
         */
        static void canonicalTuple(ExecContext *ctx, CmplVal& res, CmplVal& src);

        /**
         * check if the tuple only contains simple index values, sets, or elements with type TP_DEF_CB_SYM (for other elements converting in set is tried)
         * @param ctx			execution context
         * @param res           store for result tuple
         * @param src           source tuple
         * @param tcr			transform result tuple to canonical set representation
         * @return              true if tuple only contains simple index values, sets, or elements with type TP_DEF_CB_SYM
         */
        static bool asTupleIndexOrSet(ExecContext *ctx, CmplVal& res, CmplVal& src, bool tcr);

        /**
         * try to match first tuple with a second tuple
         * @param ctx			execution context
         * @param se			syntax element id of operation
         * @param t1            first tuple (can contain TP_DEF_CB_SYM, which can match with any part of the second tuple, also with no part or more than one part)
         * @param t2            second tuple
         * @param ait           if given then for every part of the first tuple is registered in this array how many parts of the second tuple it is assigned in the matching result
         * @return              true if the tuples match
         */
        static bool matchTuple(ExecContext *ctx, unsigned se, CmplVal& t1, CmplVal& t2, unsigned *ait);

    private:
        /**
         * try to match range in first tuple with a range in second tuple
         * @param ctx			execution context
         * @param se			syntax element id of operation
         * @param t1            first tuple (can contain TP_DEF_CB_SYM, which can match with any part of the second tuple, also with no part or more than one part)
         * @param t2            second tuple
         * @param ait           if given then for every part of the first tuple is registered in this array how many parts of the second tuple it is assigned in the matching result
         * @param t1f           considered range in tuple t1 from
         * @param t1t           considered range in tuple t1 to
         * @param t2f           considered range in tuple t2 from
         * @param t2t           considered range in tuple t2 to
         * @return              true if considered range of the tuples match
         */
        static bool matchTupleRec(ExecContext *ctx, unsigned se, Tuple *t1, Tuple *t2, unsigned *ait, unsigned t1f, unsigned t1t, unsigned t2f, unsigned t2t);

        /**
         * inserts match info for tuple parts, with default assignment
         * @param ait           array for registering how many parts of the second tuple are assigned to a part of the first tuple in the matching result
         * @param tf            considered range of the first tuple from
         * @param tt            considered range of the first tuple to
         * @param rrank         remaining rank of second tuple to assign here
         */
        static void setTupleMatchCountDefault(unsigned *ait, unsigned tf, unsigned tt, unsigned rrank);
    };


    /**
     * utility class for matching tupels against a set, for executing a function as described in <code>TupleMatching::Mode</code>
     */
    class TupleMatching
    {
    public:
        enum Mode {
            matchIn,            ///< matching for operator "in" as boolean operator (_src is left operand and _pat is right operand in this operation; user order is never used)
            matchIter,          ///< matching for operator "in" as iteration operator (_src is right operand and _pat is left operand in this operation (contrary to matchIn); user order of _src is used)
            matchReduce,		///< matching for operator "*>" (_src is left operand and _pat is right operand in this operation; user order of _src is used)
            matchIndex,         ///< matching for indexation (_src is definition set of the array and _pat is the indexing tuple in this operation; user order of _pat is used, but if _pat contains only elements without explicit or implicit order, then user order of _src is used)
        };

    private:
        struct ptuples;

    private:
        // source values
        ExecContext *_ctx;          ///< execution context
        Mode _mode;                 ///< matching mode
        CmplVal _src;               ///< source value to match against <code>_pat</code>, must be an index tuple (_mode == matchIn) or a finite tuple set
        CmplVal _pat;               ///< tuple pattern, must be a tuple containing index values, sets (not for _mode == matchIter), or elements with type TP_DEF_CB_SYM (only for _mode == matchIter)
        bool _useOrder;				///< use user order

        // result values
        CBAssignInfoComplex *_assInfo;		///< only for _mode == matchIter: assign info of codeblock symbols for match tuples
        vector<unsigned long> *_resIndex;	///< only for _mode == matchIndex: indexes of matched tupels within _src

        // temporary values used in match()
        unsigned long _srcCnt;      ///< count of elements in <code>_src</code>
        unsigned _srcMaR;           ///< max set rank of <code>_src</code>
        bool _srcUO;                ///< <code>_src</code> is or contains set with user order

        vector<unsigned> _freePos;	///< positions of elements marked as free within <code>_pat</code>
        unsigned *_minRank;			///< array with minimal rank of elements within <code>_pat</code>
        unsigned *_maxRank;			///< array with maximal rank of elements within <code>_pat</code>

        bool _oneRank;				///< true if _minRank == _maxRank for all its entries
        bool _indexTuple;			///< <code>_pat</code> is index tuple (doesn't contain any set)
        bool _infTpl;				///< <code>_pat</code> contains an infinite set
        unsigned long _tplCnt;		///< count of tuple elements in <code>_pat</code> (only if !_infTpl)
        unsigned _tplRank;			///< rank of tuple <code>_pat</code>
        unsigned _patOrder;         ///< only for _mode == matchIndex (otherwise 0): flag whether <code>_pat</code> contains elements with user order (2) or implicit order (1) or only elements without any order (0)

        vector<CmplVal *> _uoSetSort;   ///< only used for _patOrder == 2: vector of part sets within <code>_pat</code> which have user order
        vector<unsigned long *> _uoInvSort; ///< only used for _patOrder == 2: vector of arrays with inverse user order of part sets within <code>_pat</code> which have user order
        vector<ptuples> _uoSort;    ///< only used for _patOrder == 2: part tuples for sorting of result

        unsigned _minRankSum;		///< sum over <code>_minRank</code>
        unsigned _maxRankSum;		///< sum over <code>_maxRank</code>
        unsigned _firstDiffRank;	///< index of first element within <code>_pat</code> with minimal rank unequal to maximal rank
        unsigned _afterDiffRank;	///< index after last element within <code>_pat</code> with minimal rank unequal to maximal rank

    public:
        /**
         * constructor
         * @param ctx			execution context
         * @param mode          matching mode
         * @param src           source tuple (mode == matchIn) or finite set
         * @param pat           tuple pattern to match <code>src</code> against
         * @param uo            use user order
         */
        TupleMatching(ExecContext *ctx, Mode mode, const CmplVal& src, const CmplVal& pat, bool uo);

        /**
         * destructor
         */
        ~TupleMatching()                                { _src.dispose(); _pat.dispose(); releaseResult(); }

        /**
         * try to match _src with the elements of the tuple pattern _pat and make a result set of all matching tuples
         * @param res           store for result set
         * @param tcr			transform result set to canonical set representation
         * @return              true if any matching tuples, then result set is filled
         */
        bool match(CmplVal& res, bool tcr = true);

        /**
         * take assign info result of tuple matching for iteration
         * @return              assign info object for codeblock symbols in iteration
         */
        CBAssignInfoBasis *takeAssignInfo();

        /**
         * get indexes of matched tupels within _src, only for mode matchIndex
         */
        vector<unsigned long> *resIndex()               { return (_mode == matchIndex ? _resIndex : NULL); }

    private:
        /**
         * release result values
         */
        void releaseResult();

        /**
         * prepare <code>_src</code> for matching
         */
        void prepareSource();

        /**
         * prepare <code>_pat</code> for matching, and fills temporary instance variables
         */
        void prepareTuple();

        /**
         * matches single index tuple against <code>_pat</code>
         * @param it            index tuple
         * @param am            return of matching rank per element in <code>_pat</code>
         * @return              true if successfully matched
         */
        bool singleMatch(const CmplVal& it, unsigned *am);

        /**
         * try to match range in first tuple with a range in second tuple
         * @param t1            first tuple (can contain sets and index values)
         * @param t2            second tuple (contains only index values)
         * @param am            for every part of the first tuple is registered in this array how many parts of the second tuple it is assigned in the matching result
         * @param t1f           considered range in tuple t1 from
         * @param t1t           considered range in tuple t1 to
         * @param t2f           considered range in tuple t2 from
         * @param t2t           considered range in tuple t2 to
         * @return              true if considered range of the tuples match
         */
        bool singleMatchRec(Tuple *t1, Tuple *t2, unsigned *am, unsigned t1f, unsigned t1t, unsigned t2f, unsigned t2t);

        /**
         * try to match part of iteration tuple
         * @param set           set or simple index value to match tuple part against
         * @param mr            minimal rank of <code>set</code>
         * @param itpl          iteration tuple
         * @param rf            first element number of regarding part of the iteration tuple
         * @param rc            count of elements in regarding part of the iteration tuple
         * @return              true if iteration tuple part matches
         */
        bool matchPart(CmplVal *set, unsigned mr, Tuple *itpl, unsigned rf, unsigned rc);

        /**
         * reduce given tuple according to positions in <code>_freePos</code>, then add it to result set
         * @param res           result set
         * @param tpl           source tuple
         * @param ranks         rank per element in match tuple <code>tpl</code>
         * @param uo            add to result set with user order
         * @param srcInd        index of tuple in source set
         * @return              true if tuple is added to result set / false if tuple was already in result set
         */
        bool reduceAddTuple(CmplVal& res, const CmplVal& tpl, unsigned *ranks, bool uo, unsigned long srcInd);

        /**
         * add user order from <code>_uoSort</code> to result set
         * @param res           result set
         */
        void addResUO(CmplVal& res);

    private:
        /**
         * part tuples for sorting of the result array
         */
        struct ptuples {
            unsigned long _pos;             ///< position of regarding element in result set
            vector<CmplValAuto> _tp;        ///< part tuples for one matching tuple

            /**
             * constructor
             * @param ctx           execution context
             * @param rtpl          single result tuple
             * @param ranks         rank per element in match tuple
             * @param freePos       positions of elements marked as free within <code>_pat</code>
             * @param uoSetSort     vector of part sets within <code>_pat</code> which have user order
             * @param uoInvSort     vector of arrays with inverse user order for part sets within <code>_pat</code> which have user order
             */
            ptuples(ExecContext *ctx, const CmplVal& rtpl, unsigned *ranks, vector<unsigned>& freePos, vector<CmplVal *>& uoSetSort, vector<unsigned long *>& uoInvSort);

            /**
             * compare two ptuples
             * @param a             first element
             * @param b             second element
             * @return              true if <code>a</code> is less than <code>b</code>
             */
            static bool less(const ptuples& a, const ptuples& b);

            /**
             * compare to simple index values in canonical order
             * @param atp           type of first value (can only be int or string)
             * @param ai            first value
             * @param btp           type of second value (can only be int or string)
             * @param bi            second value
             * @return              -1 if first value is less than second value, 1 if first value is greater than second value, 0 if values are equal
             */
            static int cmp(const tp_e atp, const intType ai, const tp_e btp, const intType bi)    { bool astr = (atp == TP_STR || atp == TP_ITUPLE_1STR); bool bstr = (btp == TP_STR || btp == TP_ITUPLE_1STR); return (astr != bstr ? (astr ? 1 : -1) : (ai == bi ? 0 : (ai < bi ? -1 : 1))); }
        };
    };


    /**
     * exception thrown in <code>TupleMatching</code> if a source value is invalid
     */
    class TupleMatchingValueException : public runtime_error
    {
    private:
        int _invSrc;            ///< true: _src is invalid / false: _pat is invalid

    public:
        /**
         * constructor
         * @param txt       error message
         * @param invSrc    true: _src is invalid / false: _pat is invalid
         */
        TupleMatchingValueException(const string& txt, bool invSrc) noexcept : runtime_error(txt), _invSrc(invSrc)      { }

        /**
         * get whether source or tuple pattern is invalid
         */
        int invSrc() const noexcept                 { return _invSrc; }
    };



    /**
     * utility class for iterating over all index tuples of a finite set or a tuple which may contain finite sets
     */
    class TupleIterator
    {
    private:
        ExecContext *_ctx;          ///< execution context

        CmplValAuto _tpl;           ///< tuple or set to iterate
        unsigned _rank;             ///< rank of tuple

        bool _useOrder;				///< iterate in user order
        bool _reverse;				///< iterate in reverse order

        vector<SetIterator> _sub;   ///< sub iterator per tuple element

        unsigned long _cnt;			///< count of elements
        unsigned long _ind;			///< index number of current element
        unsigned long _tplInd;		///< index of the current element within the set (only used if _useOrder)

        bool _ended;                ///< iteration is ended
        CmplValAuto _cur;   		///< current tuple

    public:
        /**
         * constructor
         * @param tpl			tuple or set to iterate (must contain only finite sets or index values)
         * @param useOrder		iterate in user order
         * @param reverse		iterate in reverse order
         */
        TupleIterator(ExecContext *ctx, const CmplVal& tpl, bool useOrder = false, bool reverse = false): _ctx(ctx), _tpl(tpl), _useOrder(useOrder), _reverse(reverse), _cur(TP_EMPTY)      { iterIntern(true); }

        /**
         * get current element of iteration
         * @return				current index tuple / TP_EMPTY if iteration is ended
         */
        const CmplVal& curTuple() const     { return _cur; }

        /**
         * get current index number of iteration
         * @return				current index number
         */
        unsigned long curIndex() const      { return _ind; }

        /**
         * get index of the current element within the set
         * @return				index of the current element within the set / equal count() if iteration is ended
         */
        unsigned long tupleIndex() const	{ return (_useOrder ? _tplInd : (_reverse && _ind < _cnt ? (_cnt - 1 - _ind) : _ind)); }

        /**
         * start or restart the iteration
         * @return				first tupel of the set
         */
        CmplVal& begin()					{ iterIntern(true); return _cur; }

        /**
         * iterate to the next tuple
         * @return				next tuple of the set
         */
        CmplVal& iter()					{ iterIntern(false); return _cur; }

        /**
         * get value TP_EMPTY as flag that the iteration is ended
         * @return				value TP_EMPTY
         */
        CmplVal end() const				{ return CmplVal(TP_EMPTY); }

        /**
         * returns whether the iteration is ended
         * @return				true if end of iteration is reached (current element is then TP_EMPTY)
         */
        bool ended() const                      { return _ended; }

        /**
         * get current element of iteration
         * @return				current index tuple / TP_EMPTY if iteration is ended
         */
        const CmplVal& operator* () const { return _cur; }

        /**
         * iterate to the next tuple
         */
        void operator++ ()				{ iterIntern(false); }

        /**
         * iterate to the next tuple
         */
        void operator++ (int)			{ iterIntern(false); }

        /**
         * returns whether the iteration is not ended
         * @return				true if iteration is not ended (current element is not TP_EMPTY)
         */
        explicit operator bool() const	{ return !_ended; }

    private:
        /**
         * iterate to the first or next tuple
         * @param start			start or restart the iteration
         */
        void iterIntern(bool start);
    };
}

#endif // TUPLEUTIL_HH

