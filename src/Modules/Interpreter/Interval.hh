
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


#ifndef INTERVAL_HH
#define INTERVAL_HH

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
	 * the <code>Interval</code> class represents an interval of int or real numbers.
	 * an object of this class is only used if a lower bound and an upper bound exists,
	 * otherwise special types TP_INTERVAL_* are used for the interval value.
	 */
	class Interval : public CmplObjBase
	{
	private:
		CmplVal _lowBound;				///< lower bound (can only be TP_REAL or TP_INT)
		CmplVal _uppBound;				///< upper bound (can only be TP_REAL or TP_INT)

	public:
		/**
		 * constructor
		 * @param lb			lower bound (must be TP_REAL or TP_INT)
		 * @param ub			upper bound (must be TP_REAL or TP_INT)
		 */
        Interval(CmplVal& lb, CmplVal& ub): _lowBound(lb), _uppBound(ub)			{ }

		/**
		 * get lower bound
		 */
		CmplVal& lowBound()				{ return _lowBound; }

		/**
		 * get upper bound
		 */
		CmplVal& uppBound()				{ return _uppBound; }


		/**
		 * write contents of the object to a stream
		 * @param modp			calling module
		 * @param mode			mode for output: 0=direct; 1=part of other value
		 */
        virtual void write(ostream& ostr, ModuleBase *modp, int mode = 0) const override     { _lowBound.write(ostr, modp, 1); ostr << ".."; _uppBound.write(ostr, modp, 1); }


		/****** static info function for interval values ****/

	public:
		/**
		 * get whether the interval v is an empty interval
		 */
        static bool isEmpty(const CmplVal &v);

		/**
		 * get whether the interval v has a lower bound
		 */
        inline static bool hasLowBound(const CmplVal &v)						{ return (v.boundLow() || v.boundLU()); }

		/**
		 * get whether the interval v has an upper bound
		 */
        inline static bool hasUppBound(const CmplVal &v)						{ return (v.boundUpp() || v.boundLU()); }

        /**
         * get whether the interval v has no bound
         */
        inline static bool hasNoBound(const CmplVal &v)                         { return (!v.boundLow() && !v.boundUpp() && !v.boundLU()); }

        /**
         * get whether the interval is infinite (not bounded on both sides)
         */
        inline static bool isInf(const CmplVal &v)                              { return (!v.boundLU()); }

		/**
		 * get whether the lower bound of interval v is a real number (only if hasLowBound(v))
		 */
        inline static bool isLowBoundReal(const CmplVal &v)                     { return (hasLowBound(v) && (v.useReal() || (v.t == TP_INTERVAL && v.interval()->_lowBound.t == TP_REAL))); }

		/**
		 * get whether the upper bound of interval v is a real number (only if hasUppBound(v))
		 */
        inline static bool isUppBoundReal(const CmplVal &v)                     { return (hasUppBound(v) && (v.useReal() || (v.t == TP_INTERVAL && v.interval()->_uppBound.t == TP_REAL))); }

		/**
		 * get lower bound of interval v as a real number (only if hasLowBound(v), or TP_INTERVAL_EMPTY) (for TP_INTERVAL_EMPTY return 0)
		 */
        static realType lowBoundReal(const CmplVal &v);

		/**
		 * get upper bound of interval v as a real number (only if hasUppBound(v), or TP_INTERVAL_EMPTY) (for TP_INTERVAL_EMPTY return -1)
		 */
        static realType uppBoundReal(const CmplVal &v);

		/**
		 * get lower bound of interval v as an int number (only if hasLowBound(v), or TP_INTERVAL_EMPTY) (for TP_INTERVAL_EMPTY return 0)
		 */
        static intType lowBoundInt(const CmplVal &v);

		/**
		 * get upper bound of interval v as an int number (only if hasUppBound(v), or TP_INTERVAL_EMPTY) (for TP_INTERVAL_EMPTY return -1)
		 */
        static intType uppBoundInt(const CmplVal &v);

        /**
         * get lower bound of interval v and set r to it (can be TP_INT, TP_REAL or TP_INFINITE)
         */
        static void lowBoundVal(CmplVal &r, const CmplVal &v);

        /**
         * get upper bound of interval v and set r to it (can be TP_INT, TP_REAL or TP_INFINITE)
         */
        static void uppBoundVal(CmplVal &r, const CmplVal &v);

        /**
         * get whether a numerical value n is within an interval v
         */
        static bool numInInterval(const CmplVal &v, const CmplVal &n);

        /**
         * get whether the interval is aequivalent to a set
         */
        static bool isSet(const CmplVal &v)					{ return (v.isInterval() && (v.useNothing() || v.useInt() || (v.t == TP_INTERVAL && !isLowBoundReal(v) && !isUppBoundReal(v)))); }

        /**
         * get whether the interval is aequivalent to a finite set
         */
        static bool isSetFin(const CmplVal &v)				{ return (v.isInterval() && v.t != TP_INTERVAL_EMPTY && v.boundLU() && (v.useInt() || (v.t == TP_INTERVAL && !isLowBoundReal(v) && !isUppBoundReal(v)))); }

        /**
         * get whether the interval is aequivalent to an infinite set
         */
        static bool isSetInf(const CmplVal &v)				{ return (v.isInterval() && v.t != TP_INTERVAL_EMPTY && !v.boundLU() && (v.useNothing() || v.useInt() || (v.t == TP_INTERVAL && !isLowBoundReal(v) && !isUppBoundReal(v)))); }


		/****** interval construction ****/

		/**
		 * construct an interval from stack values
		 * @param ctx			execution context
		 * @param res			store for result interval
		 * @param lb			value for lower bound / NULL: no lower bound
		 * @param ub			value for upper bound / NULL: no upper bound
		 */
		static void construct(ExecContext *ctx, CmplVal &res, StackValue *lb, StackValue *ub);

		/**
		 * construct an interval from values
		 * @param res			store for result interval
		 * @param lb			value for lower bound (must be TP_INT, TP_REAL or TP_INFINITE) / TP_EMPTY: no lower bound
		 * @param ub			value for upper bound (must be TP_INT, TP_REAL or TP_INFINITE) / TP_EMPTY: no upper bound
		 */
		static void construct(CmplVal &res, CmplVal& lb, CmplVal& ub);


        /****** interval operations ****/

        /**
         * merge two intervals to a result interval
         * @param res			store for result interval
         * @param a1			first source interval (must be an interval)
         * @param a2			second source interval (must be an interval)
         * @param mgap          merge even when there is a gap between the two source intervals
         * @return              true if result interval is created / false if not possible
         */
        static bool merge(CmplVal &res, const CmplVal &a1, const CmplVal &a2, bool mgap = false);

        /**
         * intersect two intervals to a result interval
         * @param res			store for result interval
         * @param a1			first source interval (must be an interval)
         * @param a2			second source interval (must be an interval)
         * @return              true if result interval is created / false if not possible
         */
        static bool intersect(CmplVal &res, const CmplVal &a1, const CmplVal &a2);
    };
}

#endif // INTERVAL_HH

