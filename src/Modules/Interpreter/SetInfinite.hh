
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


#ifndef SETINFINITE_HH
#define SETINFINITE_HH

#include "../../CommonData/CmplVal.hh"
#include "../../CommonData/SetBase.hh"


using namespace std;



//Durch unendliche Sets betroffene Funktionen:
// A: Explizite Umwandlung (Funktion "set(...)"):
//      bool ValTypeSet::tryConvertSimpleTo(ExecContext *ctx, CmplVal& res, CmplVal& src, TypeConversionLevel tcl) const
//          bool SetUtil::convertToSetOrTuple(ExecContext *ctx, CmplVal& res, CmplVal& src, TypeConversionLevel tcl, bool toTuple)
// B: TupleMatching
//  B1: matchIn
//          (links: nie Set, sondern einfacher Wert oder Indextupel)
//          (rechts: umgewandelt in Set mit SetUtil::convertToSetOrTuple() -> stattdessen moeglichst auf Tuple direkt ausfuehren)
//          + Aufruf muss noch implementiert werden, fuer Operation "in" (bisher nur fuer finite Set und nicht ueber TupleMatching)
//  B2: matchIter
//          (links: nie Set, sondern CB-Symbol oder Indextupel mit CB-Symbol)
//          (rechts: immer finite Set)
//          + Ausfuehrung mit Indextupel, das teilweise CB-Symbol enthaelt, funktioniert anscheinend noch nicht
//  B3: matchReduce
//          (links: immer finite Set)
//          rechts: Tupel, dass unendliche Sets enthalten kann
//              Umwandlung in Tupel notwendig, Aufspaltung Sets mit "partsToVector()"
//                  + TupleUtil::canonicalTuple() (muss auch unendliche Sets innerhalb des Tupels passend beruecksichtigen, bzw. Set in Tuple wandeln)
//                      (wird in TupleMatching::prepareTuple() fuer Set schon gerufen; und fuer Tuple vorher in TupleUtil::asTupleIndexOrSet())
//          + Ist Implementierung TupleMatching fertig, einschliesslich Reihenfolgen?
//          + Aufruf muss noch implementiert werden, fuer Operation "*>"
//  B4: matchIndex
//          (links: immer finiter Set (Definitionsset Array))
//          rechts: wie in B3
//          + Ausfuehrung bei Set-haltigem Tuple? (fuer rechte Seite (Array), fuer rechte Seite (Liste), fuer linke Seite (Zuweisung))
//          -> Erhaltung Reihenfolge aus rechtem Operand (indizierendes Tupel) fehlt (wenn ueber Iteration ueber Definitionsset, also wenn Tupel unendlich oder groesser als Definitionsset)
// C: Set-Operationen
//      wenn Operand Tuple, dann in Set umwandeln (wie in A.)
//  C1: + / - / *
//      u.a.: SetUtil::addSet()
//  C2: Vergleiche


// . bool TupleUtil::asTupleIndexOrSet(ExecContext *ctx, CmplVal& res, CmplVal& src)
//      . bool SetUtil::convertToSetOrTuple(ExecContext *ctx, CmplVal& res, CmplVal& src, TypeConversionLevel tcl, bool toTuple)
//          + bool TupleUtil::toIndexTuple(ExecContext *ctx, CmplVal& res, Tuple *tpl)
// . void TupleUtil::toIndexTuple(ExecContext *ctx, CmplVal &res, StackValue *src, bool listTuple)
//          + bool TupleUtil::toIndexTuple(ExecContext *ctx, CmplVal& res, Tuple *tpl)

// + void OperationBase::tplInSet(ExecContext *ctx, CmplVal *res, unsigned se, CmplVal *a1, CmplVal *a2, bool opIn)
// + unsigned SetUtil::tupleInSet(ExecContext *ctx, const CmplVal &set, const CmplVal &tup, unsigned long& num, unsigned tps, int tpc)
//          -> Erweiterung fuer unendliche Sets:
//                  kann fuer SetInfiniteTpl ueber TupleMatching laufen; aber sonst nicht (weil sonst unendliche Rekursion)



namespace cmpl
{
    class ExecContext;


	/**
     * abstract base class for infinite sets.
     * infinite sets are constructed recursively by other sets.
	 */
    class SetInfinite : public SetBase2
	{
    protected:
        unsigned int _partCnt;				///< count of part sets (must be at least 2)
        CmplVal *_parts;                    ///< array of part sets, length is _partCnt (elements can be simple index values or sets (finite and infinite))

        unsigned _minRank;                  ///< minimal rank of tuples within the tuple set
        unsigned _maxRank;                  ///< maximal rank of tuples within the tuple set (UINT_MAX if unbounded)

    public:
        /**
         * destructor
         */
        ~SetInfinite()                                  { for (unsigned i = 0; i < _partCnt; i++) { _parts[i].dispose(); } delete[] _parts; }

    public:
        /**
         * get whether the set contains a finite count of elements
         */
        bool finite() const override                    { return false; }

        /**
         * get count of part sets
         * @return				count of part sets
         */
        unsigned int partCnt() const                    { return _partCnt; }

        /**
         * get one part of the set
         * @param i				index of the part
         * @return				part of the set
         */
        CmplVal *partSet(unsigned i) const              { return _parts + i; }

        /**
         * get minimal rank of tuples within the tuple set
         */
        unsigned minRank() const override       		{ return _minRank; }

        /**
         * get maximal rank of tuples within the tuple set
         * @param ub        upper bound for return value
         */
        unsigned maxRank(unsigned ub = UINT_MAX) const override    { return min(_maxRank, ub); }

        /**
         * get whether all tupels in the tuple set have the same rank
         */
        bool oneRank() const override                   { return (_maxRank != UINT_MAX && _minRank == _maxRank); }

        /**
         * set tuple elements for the tuple on a given index position in the set: not usable for infinite set, do nothing
         * @param ind			index number within the set
         * @param tpl			tuple value for inserting the elements
         * @param ti			start element number in tpl
         * @param useOrder		use user order of subsets
         * @param reverse		use reverse order
         * @return				index of the tuple within the set
         */
        unsigned long tupleAtIntern(unsigned long ind, Tuple *tpl, unsigned ti, bool useOrder, bool reverse) override    { return 0; }

        /**
         * convert this to the aequivalent set in canonical representation
         * @param res			return of result set
         * @param woOrder       strip order from set
         * @return				true if result set is computed; false if this is already in canonical representation
         */
        virtual bool canonicalSet(CmplVal &res, bool woOrder) = 0;

        /**
         * check if tuple or a part of a tuple is element within a set
         * @param ctx			execution context
         * @param tup			tuple to search (can also be TP_LIST_TUPLE, then it must be the topmost element on the value stack of the execution context)
         * @param tps			start index of used tuple part
         * @param tpc			count of indizes of used tuple part / -1: up to the end of tuple
         * @return 				true if tuple is element of the set
         */
        virtual bool tupleInSet(ExecContext *ctx, const CmplVal &tup, unsigned tps = 0, int tpc = -1) const = 0;
    };


    /**
     * infinite set constructed as tuple from other sets (e.g. "[set1, set2]").
     * parts can be simple index values or sets (finite or infinite, but not SetInfiniteTpl).
     * must have at least two parts, and at least one part must be another infinite set.
     */
    class SetInfiniteTpl : public SetInfinite
    {
    protected:
        bool _subOrder;                     ///< user order on a finite subset

        // _parts koennen auch TP_SET_REC_MULT bzw. TP_SET_FIN mit TP_SET_REC_MULT als baseSet sein
        //     In solch einem Fall ist this nicht kanonisch (ausser wenn durch Reihenfolgen diese Sets nicht teilbar sind); bei Umwandlung in kanonisch sind sie aufzuteilen.
        //     Ist this kanonisch, dann sind keine weiteren Aufteilungen moeglich, maxTupleParts() und partsToVector() koennen direkt _partCnt und _parts verwenden.

    public:
        /**
         * constructor
         * @param pc			count of part sets
         * @param arr			array with part sets
         */
        SetInfiniteTpl(unsigned pc, CmplVal *arr);

        /**
         * constructor
         * @param parts			vector with part sets
         * @param tcr			transform part sets to canonical set representation
         * @param uo            keep user order
         */
        SetInfiniteTpl(vector<CmplValAuto>& parts, bool tcr, bool uo);

        /**
         * get max bound for tuple part count this set is divisible in
         */
        unsigned maxTupleParts() const;

        /**
         * push all tuple divisible parts in a vector
         * @param dtp       vector to push parts in
         * @param uo        consider user order
         * @return          true if parts are pushed to vector; false if set is not tuple divisible
         */
        bool partsToVector(vector<CmplValAuto> &dtp, bool uo) const override;

        /**
         * get whether some subset of this has an user order
         * (that user order on subsets is ignored, if there is a direct user order)
         */
        bool hasSubUserOrder() const override		{ return _subOrder; }

        /**
         * get whether the tuple set has an user order of its elements (direct order or on subsets)
         */
        bool hasUserOrder() const override			{ return _subOrder; }

        /**
         * get set with all user order removed
         * @param res		value for result
         */
        void stripOrder(CmplVal& res) override;

        /**
         * convert this to the aequivalent set in canonical representation
         * @param res			return of result set
         * @param woOrder       strip order from set
         * @return				true if result set is computed; false if this is already in canonical representation
         */
        bool canonicalSet(CmplVal &res, bool woOrder) override;

        /**
         * check if tuple or a part of a tuple is element within a set
         * @param ctx			execution context
         * @param tup			tuple to search (can also be TP_LIST_TUPLE, then it must be the topmost element on the value stack of the execution context)
         * @param tps			start index of used tuple part
         * @param tpc			count of indizes of used tuple part / -1: up to the end of tuple
         * @return 				true if tuple is element of the set
         */
        bool tupleInSet(ExecContext *ctx, const CmplVal &tup, unsigned tps = 0, int tpc = -1) const override;

        /**
         * write contents of the object to a stream
         * @param modp			calling module
         * @param mode			mode for output: 0=direct; 1=part of other value
         */
        void write(ostream& ostr, ModuleBase *modp, int mode = 0) const override;
    };


    /**
     * infinite set constructed by a set operation from other sets (e.g. "set(set1, set2)" or "set1 * set2").
     * all parts must be other sets (finite or infinite, also another SetInfiniteSet).
     * must have at least two parts, and at least one part must be another infinite set.
     * it must have exactly two parts if constructing operation is ICS_OPER_SUB.
     * can never have a user order on subsets.
     */
    class SetInfiniteSet : public SetInfinite
    {
    private:
        unsigned short _constructOp;		///< constructing operation, can only be ICS_OPER_ADD, ICS_OPER_MUL or ICS_OPER_SUB

    public:
        /**
         * constructor
         * @param pc			count of part sets
         * @param arr			array with part sets
         * @param op            constructing operation
         */
        SetInfiniteSet(unsigned pc, CmplVal *arr, unsigned short op);


        /**
         * convert this to the aequivalent set in canonical representation
         * @param res			return of result set
         * @param woOrder       strip order from set (ignored because this has never a considered user order)
         * @return				true if result set is computed; false if this is already in canonical representation
         */
        bool canonicalSet(CmplVal &res, bool woOrder) override;

        /**
         * check if tuple or a part of a tuple is element within a set
         * @param ctx			execution context
         * @param tup			tuple to search (can also be TP_LIST_TUPLE, then it must be the topmost element on the value stack of the execution context)
         * @param tps			start index of used tuple part
         * @param tpc			count of indizes of used tuple part / -1: up to the end of tuple
         * @return 				true if tuple is element of the set
         */
        bool tupleInSet(ExecContext *ctx, const CmplVal &tup, unsigned tps = 0, int tpc = -1) const override;

        /**
         * write contents of the object to a stream
         * @param modp			calling module
         * @param mode			mode for output: 0=direct; 1=part of other value
         */
        void write(ostream& ostr, ModuleBase *modp, int mode = 0) const override;
    };

}

#endif // SETINFINITE_HH

