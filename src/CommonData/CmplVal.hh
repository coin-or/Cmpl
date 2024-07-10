
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


#ifndef CMPLVAL_HH
#define CMPLVAL_HH

#include <stdlib.h>
#include <limits.h>
#include <math.h>
#include <errno.h>

#include <string>
#include <ostream>
#include <atomic>


using namespace std;


namespace cmpl
{

	/************** types for numeric cmpl base values **********/

	/**
	 * type for integer values in cmpl
	 */
	typedef long intType;

	#define STR_TO_CMPLINT(a)		atol(a)
	#define STR_TO_CMPLINT2(a,ce)	strtol(a,ce,10)
	#define CMPLINT_MAX				LONG_MAX
	#define CMPLINT_MIN				LONG_MIN
    #define CMPLINT_STDFORMAT		"%ld"
    #define CMPLINT_STDLENSPEC      "l";

	/**
	 * type for real values in cmpl
	 */
	typedef double realType;

	#define STR_TO_CMPLREAL(a)		atof(a)
	#define STR_TO_CMPLREAL2(a,ce)	strtod(a,ce)
	#define CMPLREAL_MAX			DBL_MAX
	#define CMPLREAL_MIN			DBL_MIN
    #define CMPLREAL_STDFORMAT		"%f"
    #define CMPLREAL_STDLENSPEC     "";

	#ifdef INFINITY
	#define CMPLREAL_INFPOS			INFINITY
	#define CMPLREAL_INFNEG			-INFINITY
	#else
	#define CMPLREAL_INFPOS			HUGE_VAL
	#define CMPLREAL_INFNEG			-HUGE_VAL
	#endif


	class ModuleBase;

	/************** base class for cmpl value objects **********/

	/**
	 * base class for memory handling by reference count.
	 */
	class CmplObjBase
	{
	private:
        atomic<unsigned> _refCnt;			///< reference counter

	protected:
		/**
		 * constructor
		 */
		inline CmplObjBase(): _refCnt(0)			{ }

	public:
		/**
		 * destructor
		 */
		inline virtual ~CmplObjBase()				{ }

		/**
		 * get reference counter
		 */
        inline unsigned refCnt() volatile			{ return _refCnt.load(); }

		/**
		 * increment reference counter
		 */
        inline void incRef() volatile				{ _refCnt++; }

		/**
		 * decrement reference counter
		 * @return				true if reference counter is 0 after decrement
		 */
        inline bool decRef() volatile				{ if (_refCnt.load()) return (_refCnt-- <= 1); else return true; }
                //TODO: so nicht threadsicher!!!

        /**
         * decrement reference counter and delete object if reference counter is 0 after decrement
         * @param v             pointer to object to dispose; is set to NULL if the object was deleted
         * @return              true if object was deleted
         */
        template<class t> inline static bool dispose(t *&v) { if (v && v->decRef()) { delete v; v = NULL; return true; } else return false; }

        /**
         * set pointer to another value, dispose previous value
         * @param v             pointer to object to set
         * @param n             new value for <code>v</code>
         */
        template<class t> inline static void dispSet(t *&v, t *n)     { dispose(v); if (n) { n->incRef(); v = n; } }

		/**
		 * write contents of the object to a stream
		 * @param modp			calling module
		 * @param mode			mode for output: 0=direct; 1=part of other value
		 */
		virtual void write(ostream& ostr, ModuleBase *modp, int mode = 0) const		{ ostr << "<unknown object>"; }
	};



	/************** class declarations for cmpl value objects (all these classes are derived from <code>CmplObjBase</code>) **********/

	class Interval;					///< interval with lower and upper bound					(defined and only used in module <code>Interpreter</code>)

    class Tuple;					///< tuple												(common use for indexing result elements)

    class SetBase;					///< base class for tuple sets							(common use for sets)
    class SetAlg;					///< class for algorithmic set							(common use for sets)
    class SetEnum;					///< class for enumeration set							(common use for sets)
    class SetRecMult;				///< class for finite rectangle index tuple set			(common use for sets)
    class SetFinite;				///< class for arbitrary finite index tuple set			(common use for sets)
    class SetWithOrder;				///< extension of a set with an user order				(common use for sets)
    class SetInfinite;				///< base class for arbitrary infinite index tuple set		(defined and only used in module <code>Interpreter</code>)
    class SetInfiniteTpl;			///< class for tuple constructed infinite index tuple set	(defined and only used in module <code>Interpreter</code>)
    class SetInfiniteSet;			///< class for set constructed infinite index tuple set		(defined and only used in module <code>Interpreter</code>)

	class ValFormula;				///< formula value										(common use as part of optimization constraints and objectives)
	class ValFunction;				///< function handle										(defined and only used in module <code>Interpreter</code>)
	class ValContainer;				///< container value										(defined and only used in module <code>Interpreter</code>) TODO: oder doch allgemein für Datenimport/-export???
	class ValType;					///< data type value										(defined and only used in module <code>Interpreter</code>)
    class ValSpecialBase;           ///< pseudo symbol handling value                           (defined and only used in module <code>Interpreter</code>)

	class OptVar;					///< optimization variable								(common use for result elements)
	class OptCon;					///< optimization constraint or objective				(common use for result elements)
    class OptVarConBase;			///< optimization result object (variable/constraint)	(common use for result elements)

	class CmplArray;				///< array of other values								(common use for result elements)
    class ValueStore;               ///< store for values within an array                       (defined and only used in module <code>Interpreter</code>)


	/************** type for generic cmpl values **********/

	/**
	 * union for value of an arbitrary data type.
	 */
	union tp_u {
		realType r;								///< real number
		intType i;								///< integer number or index for a stored string

		string *s;								///< string (not stored in string store)
		CmplObjBase *vp;						///< pointer to cmpl value object
		void *p;								///< other pointer
	};

	/**
	 * type codes, to determine which data type is in a <code>tp_u</code>.
	 */
	enum tp_e {
		// empty values
		TP_EMPTY			= 0x000008,		///< value is not set																	(tp_u not used)
		TP_NULL				= 0x000009,		///< null value																			(tp_u not used)
		TP_BLANK			= 0x00000a,		///< literal blank value (only used in array or tuple construction)						(tp_u not used)

		// numeric values
		TP_REAL				= 0x041000,		///< real value																			tp_u.r
		TP_INFINITE			= 0x042000,		///< infinite real value																tp_u.i (only used for sign)
		TP_INT				= 0x0c2000,		///< int value																			tp_u.i
        TP_BIN				= 0x0c200b,		///< binary value																		tp_u.i (only 0 or 1)

		// string values
		TP_STR				= 0x082000,		///< stored string																		tp_u.i
		TP_STRINGP			= 0x084000,		///< string																				tp_u.s

		// interval values
		TP_INTERVAL_EMPTY	= 0x200908,		///< empty interval (no value contained)												(tp_u not used)
		TP_INTERVAL			= 0x208917,		///< interval with lower and upper bound												tp_u.interval
		TP_INTERVAL_ALL		= 0x200910,		///< unbounded interval																	(tp_u not used)
		TP_INTERVAL_0_INF	= 0x200915,		///< interval from 0 without upper bound												(tp_u not used)
		TP_INTERVAL_1_INF	= 0x210915,		///< interval from 1 without upper bound												(tp_u not used)
		TP_INTERVAL_LB_R	= 0x2019d5,		///< interval only with lower bound (bound is real value)								tp_u.r
		TP_INTERVAL_LB_I	= 0x2029f5,		///< interval only with lower bound (bound is integer value)							tp_u.i
		TP_INTERVAL_UB_R	= 0x2019d6,		///< interval only with upper bound (bound is real value)								tp_u.r
		TP_INTERVAL_UB_I	= 0x2029f6,		///< interval only with upper bound (bound is integer value)							tp_u.i
		TP_INTERVAL_0_UB_R	= 0x2019db,		///< interval between 0 and upper bound (bound is real value)							tp_u.r
		TP_INTERVAL_0_UB_I	= 0x2029fb,		///< interval between 0 and upper bound (bound is integer value)						tp_u.i
		TP_INTERVAL_1_UB_R	= 0x2119df,		///< interval between 1 and upper bound (bound is real value)							tp_u.r
		TP_INTERVAL_1_UB_I	= 0x2129ff,		///< interval between 1 and upper bound (bound is integer value)						tp_u.i

		// tuple values
		TP_TUPLE			= 0x28811b,		///< arbitrary tuple																	tp_u.tuple
		TP_TUPLE_EMPTY		= 0x280808,		///< empty tuple ("[]" in cmpl)															(tp_u not used)
		TP_ITUPLE			= 0x2c812b,		///< index tuple (only with integer or string components)								tp_u.tuple
		TP_ITUPLE_NULL		= 0x2c08c8,		///< null tuple																			(tp_u not used)
		TP_ITUPLE_1INT		= 0x2c29f9,		///< index tuple with only one integer component										tp_u.i
		TP_ITUPLE_1STR		= 0x2c29e9,		///< index tuple with only one string component											tp_u.i

		// set values
        TP_SET_FIN			= 0x308127,		///< arbitrary finite tuple set															tp_u.setBase (setFinite)
        TP_SET_REC_MULT		= 0x318127,		///< finite rectangle tuple set of certain rank											tp_u.setBase (setRecMult)
        TP_SET_R1_ENUM		= 0x308927,		///< enumeration tuple set of rank 1													tp_u.setBase (setEnum)
        TP_SET_R1_ALG		= 0x3089f7,		///< algorithmic tuple set of rank 1													tp_u.setBase (setAlg)
        TP_SET_R1_0_UB		= 0x3029fb,		///< algorithmic tuple set of rank 1 between 0 and upper bound							tp_u.i
        TP_SET_R1_0_UB_MNF	= 0x3429fb,		///< algorithmic tuple set of rank 1 between 0 and upper bound, marked as non-free		tp_u.i
		TP_SET_R1_1_UB		= 0x3029ff,		///< algorithmic tuple set of rank 1 between 1 and upper bound							tp_u.i
		TP_SET_R1_1_UB_MNF	= 0x3429ff,		///< algorithmic tuple set of rank 1 between 1 and upper bound, marked as non-free		tp_u.i
		TP_SET_EMPTY		= 0x300908,		///< empty tuple set																	(tp_u not used)
		TP_SET_NULL			= 0x3008c9,		///< tuple set with only the null tuple as element										(tp_u not used)
		TP_SET_1INT			= 0x3029f9,		///< tuple set with only one tuple with one integer component							tp_u.i
		TP_SET_1STR			= 0x3029e9,		///< tuple set with only one tuple with one string component							tp_u.i

        TP_SET_INF_TPL		= 0x388120,		///< arbitrary infinite tuple set, tuple constructed									tp_u.setBase (SetInfiniteTpl)
        TP_SET_INF_SET		= 0x398120,		///< arbitrary infinite tuple set, set constructed                                      tp_u.setBase (SetInfiniteSet)

        TP_SET_R1_LB_INF	= 0x3829f5,		///< infinite tuple set of rank 1 with all integers from a lower bound					tp_u.i
		TP_SET_R1_LB_INF_MNF= 0x3c29f5,		///< infinite tuple set of rank 1 with all integers from a lower bound, marked as non-free		tp_u.i
		TP_SET_R1_INF_UB	= 0x3829f6,		///< infinite tuple set of rank 1 with all integers up to an upper bound				tp_u.i
		TP_SET_R1_INF_UB_MNF= 0x3c29f6,		///< infinite tuple set of rank 1 with all integers up to an upper bound, marked as non-free	tp_u.i
        TP_SET_R1_IINF   	= 0x3809f0,		///< infinite tuple set of rank 1 with all integers (including negative)				(tp_u not used)
        TP_SET_R1_IINF_MNF  = 0x3c09f0,		///< infinite tuple set of rank 1 with all integers (including negative), marked as non-free	(tp_u not used)

        TP_SET_R1A			= 0x390920,		///< tuple set of all tuples of rank 1													(tp_u not used)
        TP_SET_R1A_MNF		= 0x3d0920,		///< tuple set of all tuples of rank 1, marked as non-free								(tp_u not used)
        TP_SET_R1A_INT		= 0x3909f0,		///< tuple set of all tuples of rank 1 only with non-neg integer components				(tp_u not used)
        TP_SET_R1A_INT_MNF	= 0x3d09f0,		///< tuple set of all tuples of rank 1 only with non-neg integer components, marked as non-free	(tp_u not used)
        TP_SET_R1A_STR		= 0x3909e0,		///< tuple set of all tuples of rank 1 only with string components						(tp_u not used)
        TP_SET_R1A_STR_MNF	= 0x3d09e0,		///< tuple set of all tuples of rank 1 only with string components, marked as non-free	(tp_u not used)

        TP_SET_ALL			= 0x380220,		///< tuple set of all tuples of all ranks												(tp_u not used)
        TP_SET_ALL_MNF		= 0x3c0220,		///< tuple set of all tuples of all ranks, marked as non-free							(tp_u not used)
        TP_SET_ALL_INT		= 0x3802f0,		///< tuple set of all tuples of all ranks only with non-neg integer components			(tp_u not used)
        TP_SET_ALL_INT_MNF	= 0x3c02f0,		///< tuple set of all tuples of all ranks only with non-neg integer components, marked as non-free	(tp_u not used)
        TP_SET_ALL_STR		= 0x3802e0,		///< tuple set of all tuples of all ranks only with string components					(tp_u not used)
        TP_SET_ALL_STR_MNF	= 0x3c02e0,		///< tuple set of all tuples of all ranks only with string components, marked as non-free	(tp_u not used)

        TP_SET_WITH_UO		= 0x328127,		///< value for extending a tuple set by an user defined order							tp_u.setBase (setWithOrder)

		// other parameter values
		TP_FORMULA			= 0x408000,		///< formula																			tp_u.valFormula
        //TP_FORMULA_EMPTY	= 0x400000,		///< empty formula																		(tp_u not used)
        TP_FUNCTION			= 0x488000,		///< function object                        											tp_u.valFunction
		TP_CONTAINER		= 0x498000,		///< container object																	tp_u.valContainer
        TP_SPECIALSYM       = 0x4a8000,     ///< pseudo symbol handling value                                                       tp_u.ValSpecialBase
		TP_DATA_TYPE		= 0x4c8000,		///< cmpl data type object																tp_u.valType
		TP_DATA_BASETYPE	= 0x4c2000,		///< cmpl base data type																tp_u.i (used for tp_e)
		TP_OBJECT_TYPE		= 0x4d2000,		///< cmpl object type																	tp_u.i (must be one of VAL_OBJECT_TYPE_*)

		// variables and constraints
		TP_OPT_VAR			= 0x808000,		///< variable for optimization (column in result matrix)								tp_u.optVar
		TP_OPT_CON			= 0xc08000,		///< constraint for optimization (line in result matrix)								tp_u.optCon
        TP_OPT_OBJ			= 0xc48000,		///< objective for optimization (line in result matrix)									tp_u.optCon

		// values only used in special cases in the module Interpreter
        TP_ARRAY			= 0xf08000,		///< array of other simple values														tp_u.array
        TP_ARRAY_COMP       = 0xf48000,     ///< array of other values, can contain null elements and other arrays                  tp_u.array

		TP_LIST_SIMPLE		= 0xe02000,		///< list of values on a stack, elements are no other arrays or lists					tp_u.i (used for count of stack entries belonging to the list)
		TP_LIST_SPARSE		= 0xe12000,		///< like a simple list, but contain null elements										tp_u.i (used for count of stack entries belonging to the list)
		TP_LIST_TUPLE		= 0xe22000,		///< tuple represented by a list of integer or string values on a stack					tp_u.i (used for count of stack entries belonging to the list)
        TP_LIST_COMP		= 0xe42000,		///< list of values on a stack, elements may be also other arrays or lists				tp_u.i (used for count of stack entries directly belonging to the list)
        TP_LIST_COMP_SPARSE	= 0xe52000,		///< like TP_LIST_COMP, but contain null elements										tp_u.i (used for count of stack entries directly belonging to the list)
		TP_REF_LIST			= 0xe5c000,		///< reference to a list value on a stack												tp_u.p (used for pointer to the stack element with the actual list)

        TP_SYMBOL_VAL		= 0xf8c000,		///< value for a symbol	lvalue															tp_u.p (used for pointer to the symbol value entry)
        TP_VALUESTORE       = 0xf98000,     ///< store for values within an array                                                   tp_u.valueStore

        TP_DEF_CB_SYM       = 0xe82000,     ///< value for definition of code block symbol                                          tp_u.i (used for index number within current local symbol table)
        TP_DEF_CB_SYM_TUPLE = 0xe88010,     ///< value for a tuple with at least one TP_DEF_CB_SYM as element                       tp_u.tuple
        TP_CBHEAD_BIN       = 0xe92000,     ///< binary value resulting from definition of code block symbol                        tp_u.i (only 0 or 1)
        TP_CBHEAD_ITER      = 0xe9c000,     ///< iteration info resulting from definition of code block symbol                      tp_u.p (used for pointer to CBHeaderIterInfo)
        //TP_CB_CONTROL       = 0xeac000,     ///< value resulting from codeblock control command                                     tp_u.p (used for pointer to ???TODO)

        TP_VALTREE_FLAT		= 0xfcc000,		///< value tree root for flat access													tp_u.p (used for pointer to ValueTreeRoot)
		TP_VALTREE_IND		= 0xfdc000,		///< value tree root for access by index tupel											tp_u.p (used for pointer to ValueTreeRoot)

        TP_VOIDP            = 0xffc000,     ///< arbitrary pointer                                                                  tp_u.p
	};

	/************** defines for checking type codes <code>tp_e</code> **********/

	// type of value (bit 0 - 5)
	// (bit 6 - 7 are used for distinction between otherwise equal type codes)
    #define TP_IS_EMPTY(t)                      ((((unsigned)t) & 0xfc0000) == 0x000000) 		// oooo oo			///< empty value

	#define TP_IS_SCALAR_REAL(t)				((((unsigned)t) & 0xfc0000) == 0x040000) 		// oooo o1			///< real number
	#define TP_IS_SCALAR_INT(t)					((((unsigned)t) & 0xfc0000) == 0x0c0000)		// oooo 11			///< integer number
	#define TP_IS_SCALAR_STRING(t)				((((unsigned)t) & 0xfc0000) == 0x080000)		// oooo 1o			///< string value
	#define TP_IS_SCALAR_INDEX(t)				((((unsigned)t) & 0xf80000) == 0x080000)		// oooo 1.			///< index value (int or string)
	#define TP_IS_SCALAR_NUMBER(t)				((((unsigned)t) & 0xf40000) == 0x040000)		// oooo .1			///< numeric value (real or int)
	#define TP_IS_SCALAR_VALUE(t)				((((unsigned)t) & 0xf00000) == 0x000000)		// oooo ..			///< scalar value (number or string)

	#define TP_IS_INTERVAL(t)					((((unsigned)t) & 0xfc0000) == 0x200000)		// oo1o oo			///< interval
	#define TP_IS_ATUPLE(t)						((((unsigned)t) & 0xfc0000) == 0x280000)		// oo1o 1o			///< arbitrary tuple
	#define TP_IS_ITUPLE(t)						((((unsigned)t) & 0xfc0000) == 0x2c0000)		// oo1o 11			///< index tuple
	#define TP_IS_TUPLE(t)						((((unsigned)t) & 0xf80000) == 0x280000)		// oo1o 1.			///< tuple
	#define TP_IS_SET_FIN(t)					((((unsigned)t) & 0xf80000) == 0x300000)		// oo11 o.			///< finite tuple set
	#define TP_IS_SET_INF(t)					((((unsigned)t) & 0xf80000) == 0x380000)		// oo11 1.			///< infinite tuple set
	#define TP_IS_SET_MNF(t)					((((unsigned)t) & 0xf40000) == 0x340000)		// oo11 .1			///< tuple set marked as non-free
	#define TP_IS_SET(t)						((((unsigned)t) & 0xf00000) == 0x300000)		// oo11 ..			///< tuple set
	#define TP_IS_COMPOSITE(t)					((((unsigned)t) & 0xe00000) == 0x200000)		// oo1. ..			///< composite value (interval, set or tuple)

	#define TP_IS_OPT_COL(t)					((((unsigned)t) & 0xf00000) == 0x800000)		// 1ooo ..			///< column for result optimization matrix (optimization variable)
	#define TP_IS_OPT_ROW(t)					((((unsigned)t) & 0xf00000) == 0xc00000)		// 11oo ..			///< row for result optimization matrix (constraint or objective)
	#define TP_IS_OPT_RC(t)						((((unsigned)t) & 0xb00000) == 0x800000)		// 1.oo .. 			///< row or column for result optimization matrix

	#define TP_IS_ARRAY(t)						((((unsigned)t) & 0xf80000) == 0xf00000)		// 1111 0.			///< array of other values
	#define TP_IS_LIST(t)						((((unsigned)t) & 0xf80000) == 0xe00000)		// 1110	0.			///< list of values on a stack
    #define TP_IS_CBH_VAL(t)                    ((((unsigned)t) & 0xfc0000) == 0xe80000)        // 1110 10          ///< value with codeblock symbol definition
	#define TP_IS_SPECIAL(t)					((((unsigned)t) & 0xe00000) == 0xe00000)		// 111....			///< special values used in module Interpreter

	// used portion of tp_u (bit 8 - 11)
	#define TP_USE_NOTHING(t)					((((unsigned)t) & 0x00f000) == 0x000000)		// oooo				///< tp_u not used
	#define TP_USE_REAL(t)						((((unsigned)t) & 0x00f000) == 0x001000)		// ooo1				///< use tp_u.r
	#define TP_USE_INT(t)						((((unsigned)t) & 0x00f000) == 0x002000)		// oo1o				///< use tp_u.i
	#define TP_USE_STRINGP(t)					((((unsigned)t) & 0x00f000) == 0x004000)		// o1oo				///< use tp_u.s
	#define TP_USE_VALP(t)						((((unsigned)t) & 0x00c000) == 0x008000)		// 1o..				///< use pointer to class derived from CmplValBase
	#define TP_USE_VOIDP(t)						((((unsigned)t) & 0x00f000) == 0x00c000)		// 11oo				///< use tp_u.p
    #define TP_USE_PTR(t)						((((unsigned)t) & 0x00c000) != 0)				// 1... or .1..     ///< use pointer within tp_u

	// rank of value (only for TP_IS_COMPOSITE) (bit 12 - 15)
	#define TP_RANK_UNKNOWN(t)					((((unsigned)t) & 0x000e00) == 0x000000)		// ooo.				///< rank is not specified by type
	#define TP_RANK_ALL(t)						((((unsigned)t) & 0x000f00) == 0x000200)		// oo1o				///< elements of all ranks contained	
	#define TP_RANK_0(t)						((((unsigned)t) & 0x000f00) == 0x000800)		// 1ooo				///< only the element with rank 0 is contained
	#define TP_RANK_1(t)						((((unsigned)t) & 0x000f00) == 0x000900)		// 1oo1				///< only elements with rank 1 are contained

	// type of elements (only for TP_IS_COMPOSITE) (bit 16 - 19)
	#define TP_ELEM_NOTHING(t)					((((unsigned)t) & 0x0000f0) == 0x000000)		// oooo				///< no element contained
	#define TP_ELEM_UNKNOWN(t)					((((unsigned)t) & 0x0000f0) == 0x000010)		// ooo1				///< type of contained elements is not specified by type
	#define TP_ELEM_NULL(t)						((((unsigned)t) & 0x0000f0) == 0x0000c0)		// 11oo				///< only the null element is contained
	#define TP_ELEM_REAL(t)						((((unsigned)t) & 0x0000f0) == 0x0000d0)		// 11o1				///< elements are real numbers
	#define TP_ELEM_INT(t)						((((unsigned)t) & 0x0000f0) == 0x0000f0)		// 1111				///< elements are integer numbers
	#define TP_ELEM_STRING(t)					((((unsigned)t) & 0x0000f0) == 0x0000e0)		// 111o				///< elements are strings
	#define TP_ELEM_INDEX(t)					((((unsigned)t) & 0x0000f0) == 0x000020)		// oo1o				///< elements are index values (int or string)
	#define TP_ELEM_IS_INDEX(t)					((((unsigned)t) & 0x000020) == 0x000020)		// ..1.				///< elements are a type of index values

	// type of bounds (only for TP_IS_COMPOSITE) (bit 20 - 23)
	#define TP_BOUND_NOTHING(t)					((((unsigned)t) & 0x00000f) == 0x000000)		// oooo				///< no bound	
	#define TP_BOUND_UNKNOWN(t)					((((unsigned)t) & 0x00000f) == 0x000001)		// ooo1				///< existence of bounds are not specified by type
	#define TP_BOUND_LOW(t)						((((unsigned)t) & 0x00000f) == 0x000005)		// o1o1				///< lower bound exists, but no upper bound
	#define TP_BOUND_UPP(t)						((((unsigned)t) & 0x00000f) == 0x000006)		// o11o				///< upper bound exists, but no lower bound
	#define TP_BOUND_LU(t)						((((unsigned)t) & 0x000003) == 0x000003)		// ..11				///< lower and upper bound exists
	#define TP_BOUND_0_UPP(t)					((((unsigned)t) & 0x00000f) == 0x00000b)		// 1o11				///< contains values between 0 and an upper bound
	#define TP_BOUND_1_UPP(t)					((((unsigned)t) & 0x00000f) == 0x00000f)		// 1111				///< contains values between 1 and an upper bound
	#define TP_BOUND_EMPTY(t)					((((unsigned)t) & 0x00000f) == 0x000008)		// 1ooo				///< no value exists between bounds
	#define TP_BOUND_ONE(t)						((((unsigned)t) & 0x00000f) == 0x000009)		// 1oo1				///< exactly one value exists between bounds

	#define TP_BOUND_HAS_LOW(t)					(((((unsigned)t) & 0x00000f) != 0x000000) && ((((unsigned)t) & 0x00000f) != 0x000006))		///< lower bound exists
	#define TP_BOUND_HAS_UPP(t)					(((((unsigned)t) & 0x00000f) != 0x000000) && ((((unsigned)t) & 0x00000f) != 0x000005))		///< upper bound exists

	// count of elements (only for TP_IS_COMPOSITE)
	#define TP_CNT_ELEM_0(t)					(TP_BOUND_EMPTY(t))													///< no element
	#define TP_CNT_ELEM_1(t)					(TP_BOUND_ONE(t))													///< exactly one element
	#define TP_CNT_ELEM_INF(t)					((TP_IS_SET(t) && TP_BOUND_NOTHING(t)) || (TP_IS_INTERVAL(t) && t!=TP_INTERVAL && (!TP_BOUND_HAS_LOW(t) || !TP_BOUND_HAS_UPP(t) || TP_USE_REAL(t)))) 			///< infinite count of elements
	#define TP_CNT_ELEM_FIN(t)					(TP_IS_TUPLE(t) || (TP_IS_SET(t) && !TP_BOUND_NOTHING(t)) || (TP_IS_INTERVAL(t) && (t==TP_INTERVAL_EMPTY || t==TP_INTERVAL_0_UB_I || t==TP_INTERVAL_1_UB_I)))	///< finite count of elements

    // conversion between index value, tuple with one component and set with one element
    #define TP_INDEX_VAL_BASE(t)                (((((unsigned)t) & 0x00f0f0) == 0x0020f0 || ((unsigned)t) == TP_INT) ? TP_INT : (((((unsigned)t) & 0x00f0f0) == 0x0020e0 || ((unsigned)t) == TP_STR) ? TP_STR : TP_EMPTY))                      ///< base index value type if given type has int or string elements and use tp_u.i
    #define TP_INDEX_VAL_TUPLE(t)               (((((unsigned)t) & 0x00f0f0) == 0x0020f0 || ((unsigned)t) == TP_INT) ? TP_ITUPLE_1INT : (((((unsigned)t) & 0x00f0f0) == 0x0020e0 || ((unsigned)t) == TP_STR) ? TP_ITUPLE_1STR : TP_EMPTY))      ///< index tuple type with one component if given type has int or string elements and use tp_u.i
    #define TP_INDEX_VAL_SET(t)                 (((((unsigned)t) & 0x00f0f0) == 0x0020f0 || ((unsigned)t) == TP_INT) ? TP_SET_1INT : (((((unsigned)t) & 0x00f0f0) == 0x0020e0 || ((unsigned)t) == TP_STR) ? TP_SET_1STR : TP_EMPTY))            ///< set type with one element if given type has int or string elements and use tp_u.i

	// values for TP_OBJECT_TYPE
	#define VAL_OBJECT_TYPE_PAR					0x000000			///< parameter
	#define VAL_OBJECT_TYPE_VAR					0x800000			///< optimization variable
	#define VAL_OBJECT_TYPE_CON					0xc00000			///< optimization constraint
	#define VAL_OBJECT_TYPE_OBJ					0xc40000			///< optimization objective

    #define TP_GET_OBJECT_TYPE(t)				((((unsigned)(t)) & 0xb00000) == 0x800000 ? (((unsigned)(t)) & 0xfc0000) : VAL_OBJECT_TYPE_PAR)			///< get object type for type code t
    #define TP_HAS_OBJECT_TYPE(t,o)				(TP_GET_OBJECT_TYPE(t) == (unsigned)o)								///< check whether type code t is for a value of object type o

    // user defined order for tuple set
    #define SET_VAL_HAS_ORDER(x)				((x).t == TP_SET_WITH_UO)												///< check whether value contains a set with user order
    #define SET_VAL_WO_ORDER(x)					(SET_VAL_HAS_ORDER(x) ? (((SetWithOrder*)((x).v.vp))->set()) : (x))		///< get set value without user order (but set may contain user order on subsets)
    #define SET_TP_WO_ORDER(x)					(SET_VAL_WO_ORDER(x).t)													///< get type of set value without user order


	/**
	 * type conversion levels for cmpl values
	 */
	enum TypeConversionLevel {
		typeConversionNoConv = 1,					///< only exact match of type, no actual conversion
		typeConversionIdent = 2,					///< converse only between types with identical values (eg. between TP_INT and TP_ITUPLE_1INT)
		typeConversionFromSub = 3,					///< converse only from subtype to superior type (eg. TP_BIN to TP_INT)
		typeConversionExact = 4,					///< converse only if the new type can represent all values of the original type (eg. TP_INT to TP_REAL)
		typeConversionValue = 5,					///< converse only if the new type can represent the actual value to converse (eg. TP_REAL to TP_INT, if the real value has no decimals)
		typeConversionLoseAcc = 6,					///< converse also if the value lose accuracy by the conversion (e.g TP_REAL to TP_INT, also if the real value has decimals)
		typeConversionAll = 7,						///< converse also from string or from set with one element
	};


	/**
	 * type for an arbitrary cmpl value.
	 */
	class CmplVal
	{
	public:
		tp_e t;			///< type
		tp_u v;			///< value

		// constructors
		inline CmplVal()											{ t = TP_EMPTY; }
		inline CmplVal(tp_e ct)										{ t = ct; }
		inline CmplVal(tp_e ct, realType r)							{ t = ct; v.r = r; }
		inline CmplVal(tp_e ct, intType i)							{ t = ct; v.i = i; }
		inline CmplVal(tp_e ct, string *s)							{ t = ct; v.s = s; }
        inline CmplVal(tp_e ct, CmplObjBase *p, bool inc=true)		{ t = ct; v.vp = p; if (inc) p->incRef(); }

		// set values
		inline void unset()											{ t = TP_EMPTY; }
		inline void set(tp_e ct)									{ t = ct; }
		inline void set(tp_e ct, realType r)						{ t = ct; v.r = r; }
		inline void set(tp_e ct, intType i)							{ t = ct; v.i = i; }
        inline void set(tp_e ct, int i)                             { t = ct; v.i = i; }
        inline void set(tp_e ct, bool b)                            { t = ct; v.i = (b ? 1 : 0); }
        inline void set(tp_e ct, string *s)							{ t = ct; v.s = s; }
		inline void set(tp_e ct, CmplObjBase *p, bool inc=true)		{ t = ct; v.vp = p; if (inc) p->incRef(); }
        inline void setP(tp_e ct, void *p)							{ t = ct; v.p = p; }

		inline void incRef()										{ if (useValP()) v.vp->incRef(); }

		// checking type codes
        inline bool isEmpty() const                 				{ return TP_IS_EMPTY(t); }
        inline bool isScalarReal() const							{ return TP_IS_SCALAR_REAL(t); }
		inline bool isScalarInt() const								{ return TP_IS_SCALAR_INT(t); }
		inline bool isScalarString() const							{ return TP_IS_SCALAR_STRING(t); }
		inline bool isScalarIndex() const							{ return TP_IS_SCALAR_INDEX(t); }
		inline bool isScalarNumber() const							{ return TP_IS_SCALAR_NUMBER(t); }
		inline bool isScalarValue() const							{ return TP_IS_SCALAR_VALUE(t); }
		inline bool isInterval() const								{ return TP_IS_INTERVAL(t); }
		inline bool isATuple() const								{ return TP_IS_ATUPLE(t); }
		inline bool isITuple() const								{ return TP_IS_ITUPLE(t); }
        inline bool isTuple() const									{ return TP_IS_TUPLE(t) || t == TP_DEF_CB_SYM_TUPLE; }
		inline bool isSetFin() const								{ return TP_IS_SET_FIN(t); }
		inline bool isSetInf() const								{ return TP_IS_SET_INF(t); }
		inline bool isSetMnf() const								{ return TP_IS_SET_MNF(t); }
		inline bool isSet() const									{ return TP_IS_SET(t); }
		inline bool isComposite() const								{ return TP_IS_COMPOSITE(t); }
		inline bool isOptCol() const								{ return TP_IS_OPT_COL(t); }
		inline bool isOptRow() const								{ return TP_IS_OPT_ROW(t); }
		inline bool isOptRC() const									{ return TP_IS_OPT_RC(t); }
		inline bool isArray() const									{ return TP_IS_ARRAY(t); }
		inline bool isList() const									{ return TP_IS_LIST(t); }
        inline bool isCbhVal() const                                { return TP_IS_CBH_VAL(t); }
		inline bool isSpecial() const								{ return TP_IS_SPECIAL(t); }

		inline bool useNothing() const								{ return TP_USE_NOTHING(t); }
		inline bool useReal() const									{ return TP_USE_REAL(t); }
		inline bool useInt() const									{ return TP_USE_INT(t); }
		inline bool useStringP() const								{ return TP_USE_STRINGP(t); }
		inline bool useValP() const									{ return TP_USE_VALP(t); }
		inline bool useVoidP() const								{ return TP_USE_VOIDP(t); }
		inline bool usePtr() const									{ return TP_USE_PTR(t); }

		inline bool rankUnknown() const								{ return TP_RANK_UNKNOWN(t); }
		inline bool rankAll() const									{ return TP_RANK_ALL(t); }
        //inline bool rankInt() const									{ return TP_RANK_INT(t); }
		inline bool rank0() const									{ return TP_RANK_0(t); }
		inline bool rank1() const									{ return TP_RANK_1(t); }

		inline bool elemNothing() const								{ return TP_ELEM_NOTHING(t); }
		inline bool elemUnknown() const								{ return TP_ELEM_UNKNOWN(t); }
		inline bool elemNull() const								{ return TP_ELEM_NULL(t); }
		inline bool elemReal() const								{ return TP_ELEM_REAL(t); }
		inline bool elemInt() const									{ return TP_ELEM_INT(t); }
		inline bool elemString() const								{ return TP_ELEM_STRING(t); }
		inline bool elemIndex() const								{ return TP_ELEM_INDEX(t); }
		inline bool elemIsIndex() const								{ return TP_ELEM_IS_INDEX(t); }

		inline bool boundNothing() const							{ return TP_BOUND_NOTHING(t); }
		inline bool boundUnknown() const							{ return TP_BOUND_UNKNOWN(t); }
		inline bool boundLow() const								{ return TP_BOUND_LOW(t); }
		inline bool boundUpp() const								{ return TP_BOUND_UPP(t); }
		inline bool boundLU() const									{ return TP_BOUND_LU(t); }
		inline bool bound0Upp() const								{ return TP_BOUND_0_UPP(t); }
		inline bool bound1Upp() const								{ return TP_BOUND_1_UPP(t); }
		inline bool boundEmpty() const								{ return TP_BOUND_EMPTY(t); }
		inline bool boundOne() const								{ return TP_BOUND_ONE(t); }
		inline bool boundHasLow() const								{ return TP_BOUND_HAS_LOW(t); }
		inline bool boundHasUpp() const								{ return TP_BOUND_HAS_UPP(t); }
		
		inline bool cntElem0() const								{ return TP_CNT_ELEM_0(t); }
		inline bool cntElem1() const								{ return TP_CNT_ELEM_1(t); }
		inline bool cntElemInf() const								{ return TP_CNT_ELEM_INF(t); }
		inline bool cntElemFin() const								{ return TP_CNT_ELEM_FIN(t); }

        inline intType getObjectType() const						{ return TP_GET_OBJECT_TYPE(t); }
        inline bool hasObjectType(int o) const						{ return TP_HAS_OBJECT_TYPE(t, o); }

        inline bool setValHasOrder() const							{ return (t == TP_SET_WITH_UO); }

		// typed alternatives to v.vp
		inline Interval *interval() const							{ return (Interval *) v.vp; }					///< interval with lower and upper bound
		inline Tuple *tuple() const									{ return (Tuple *) v.vp; }						///< tuple
        inline SetBase *setBase() const								{ return (SetBase *) v.vp; }					///< base class for tuple sets (with user order if existing)
        inline SetAlg *setAlg() const								{ return (SetAlg *) v.vp; }						///< class for algorithmic set
        inline SetEnum *setEnum() const								{ return (SetEnum *) v.vp; }					///< class for enumeration set
        inline SetRecMult *setRecMult() const						{ return (SetRecMult *) v.vp; }					///< class for finite rectangle index tuple set
        inline SetFinite *setFinite() const							{ return (SetFinite *) v.vp; }					///< class for arbitrary finite index tuple set
        inline SetInfinite *setInfinite() const						{ return (SetInfinite *) v.vp; }				///< base class for arbitrary infinite index tuple set
        inline SetInfiniteTpl *setInfiniteTpl() const				{ return (SetInfiniteTpl *) v.vp; }				///< class for arbitrary tuple constructed infinite index tuple set
        inline SetInfiniteSet *setInfiniteSet() const				{ return (SetInfiniteSet *) v.vp; }				///< class for arbitrary set constructed infinite index tuple set
        inline SetWithOrder *setWithOrder() const					{ return (SetWithOrder *) v.vp; }				///< extension of a set with an user order
        inline ValFormula *valFormula() const						{ return (ValFormula *) v.vp; }					///< formula value
		inline ValFunction *valFunction() const						{ return (ValFunction *) v.vp; }				///< function handle
		inline ValContainer *valContainer() const					{ return (ValContainer *) v.vp; }				///< container value
        inline ValSpecialBase *valSpecial() const					{ return (ValSpecialBase *) v.vp; }				///< pseudo symbol handling value
        inline ValType *valType() const								{ return (ValType *) v.vp; }					///< data type value
		inline OptVar *optVar() const								{ return (OptVar *) v.vp; }						///< optimization variable
		inline OptCon *optCon() const								{ return (OptCon *) v.vp; }						///< optimization constraint or objective
        inline OptVarConBase *optRC() const							{ return (OptVarConBase *) v.vp; }				///< optimization row or col (variable or constraint)
		inline CmplArray *array() const								{ return (CmplArray *) v.vp; }					///< array of other values
        inline ValueStore *valueStore() const						{ return (ValueStore *) v.vp; }					///< store for values within an array

		// dispose value (not if useVoidP())
		inline void dispose(bool dec=true)							{ if (dec && usePtr()) { if (useValP() && v.vp->decRef()) { delete v.vp; v.vp = 0; } else if (useStringP()) { delete v.s; v.s = 0; } } unset(); }

		// dispose and set value
		inline void dispUnset()										{ dispose(true); }
		inline void dispSet(tp_e ct)								{ if (usePtr()) dispose(true); t = ct; }
		inline void dispSet(tp_e ct, realType r)					{ if (usePtr()) dispose(true); t = ct; v.r = r; }
		inline void dispSet(tp_e ct, intType i)						{ if (usePtr()) dispose(true); t = ct; v.i = i; }
        inline void dispSet(tp_e ct, int i)                         { if (usePtr()) dispose(true); t = ct; v.i = i; }
        inline void dispSet(tp_e ct, bool b)                        { if (usePtr()) dispose(true); t = ct; v.i = (b ? 1 : 0); }
        inline void dispSet(tp_e ct, string *s)						{ if (usePtr()) dispose(true); t = ct; v.s = s; }
		inline void dispSet(tp_e ct, CmplObjBase *p, bool inc=true)	{ if (usePtr()) dispose(true); t = ct; v.vp = p; if (inc) p->incRef(); }

		// output value
		void write(ostream& ostr, ModuleBase *modp, int mode = 0) const;
		friend ostream& operator<< (ostream& ostr, const CmplVal& v);

		// compare operations
		inline bool operator== (const CmplVal& v2) const			{ return (t==v2.t && (useNothing() || (useReal() && v.r==v2.v.r) || (useInt() && v.i==v2.v.i) || (useStringP() && *v.s==*v2.v.s) || (useValP() && v.vp==v2.v.vp) || (useVoidP() && v.p==v2.v.p))); }
		inline bool operator!= (const CmplVal& v2) const			{ return !(operator==(v2)); }

		inline explicit operator bool() const						{ return (t != TP_EMPTY); }

		/**
		 * copy from another value
		 * @param v2		value to copy
		 * @param inc		increment reference counter for value object
		 * @param disp		dispose this before copy
		 */
		inline void copyFrom(const CmplVal& v2, bool inc=true, bool disp=true)
																	{ if (disp) { dispose(); } t = v2.t; if (!useNothing()) { if (useInt()) { v.i = v2.v.i; } else if (useValP()) { v.vp = v2.v.vp; if (inc) v.vp->incRef(); } else if (useReal()) { v.r = v2.v.r; } else if (useStringP()) { v.s = (inc ? new string(*v2.v.s) : v2.v.s); } else if (useVoidP()) { v.p = v2.v.p; } } }
		/**
		 * copy from another value
		 * @param v2		value to copy
		 * @param inc		increment reference counter for value object
		 * @param disp		dispose this before copy
		 */
		inline void copyFrom(const CmplVal *v2, bool inc=true, bool disp=true)
																	{ if (disp) { dispose(); } t = v2->t; if (!useNothing()) { if (useInt()) { v.i = v2->v.i; } else if (useValP()) { v.vp = v2->v.vp; if (inc) v.vp->incRef(); } else if (useReal()) { v.r = v2->v.r; } else if (useStringP()) { v.s = (inc ? new string(*v2->v.s) : v2->v.s); } else if (useVoidP()) { v.p = v2->v.p; } } }

        /**
         * copy constructor
         * @param v			source value
         */
        inline CmplVal(const CmplVal& v)							{ copyFrom(v, true, false); }

        /**
         * copy constructor
         * @param v			source value
         */
        inline CmplVal(const CmplVal *v)							{ copyFrom(v, true, false); }

        /**
         * assign operator
         * @param v			source value
         */
        inline CmplVal& operator= (const CmplVal& v)				{ copyFrom(v, true, true); return *this; }

        /**
		 * move another value to this
		 * @param v2		value to move
		 * @param disp		dispose this before move
		 */
		inline void moveFrom(CmplVal& v2, bool disp=false)			{ if (disp) { dispose(); } t = v2.t; if (!useNothing()) { if (useInt()) { v.i = v2.v.i; } else if (useValP()) { v.vp = v2.v.vp; } else if (useReal()) { v.r = v2.v.r; } else if (useStringP()) { v.s = v2.v.s; } else if (useVoidP()) { v.p = v2.v.p; } } v2.t = TP_EMPTY; }

		/**
		 * move another value to this
		 * @param v2		value to move
		 * @param disp		dispose this before move
		 */
		inline void moveFrom(CmplVal *v2, bool disp=false)			{ if (disp) { dispose(); } t = v2->t; if (!useNothing()) { if (useInt()) { v.i = v2->v.i; } else if (useValP()) { v.vp = v2->v.vp; } else if (useReal()) { v.r = v2->v.r; } else if (useStringP()) { v.s = v2->v.s; } else if (useVoidP()) { v.p = v2->v.p; } } v2->t = TP_EMPTY; }

        /**
         * move constructor
         * @param v			source value
         */
        inline CmplVal(CmplVal&& v)                                 { moveFrom(v, false); }


		// converse to primitive types
		/**
		 * try to get this value as a real value
		 * @param r			return of real value
		 * @param tcl		conversion level
		 * @param modp		calling module (if NULL, then no conversion from stored string)
		 * @return			true if conversion is successful
		 */
        bool toReal(realType& r, TypeConversionLevel tcl, ModuleBase *modp) const;

		/**
		 * try to get this value as an int value
		 * @param r			return of int value
		 * @param tcl		conversion level
		 * @param modp		calling module (if NULL, then no conversion from stored string)
		 * @return			true if conversion is successful
		 */
        bool toInt(intType& r, TypeConversionLevel tcl, ModuleBase *modp) const;

		/**
		 * try to get this value as a boolean value
		 * @param r			return of boolean value
		 * @param tcl		conversion level
		 * @param modp		calling module (if NULL, then no conversion from stored string)
		 * @return			true if conversion is successful
		 */
        bool toBool(bool& r, TypeConversionLevel tcl, ModuleBase *modp) const;

		/**
		 * try to get this value as a number
		 * @param r			return of number (gets type TP_INT, TP_REAL or TP_INFINITE) 
		 * @param tcl		conversion level
		 * @param modp		calling module (if NULL, then no conversion from stored string)
		 * @return			true if conversion is successful
		 */
        bool toNumber(CmplVal& r, TypeConversionLevel tcl, ModuleBase *modp) const;

        /**
         * check if this value is a numeric 0
         * @param emp		also TP_EMPTY count as 0
         * @return			true if numberic 0
         */
        inline bool isNumNull(bool emp = false) const      					{ return (((t == TP_INT || t == TP_BIN) && v.i == 0) || (t == TP_REAL && v.r == 0) || (emp && t == TP_EMPTY)); }

        /**
         * check if this value is a numeric 1
         * @param emp		also TP_EMPTY count as 1
         * @return			true if numberic 1
         */
        inline bool isNumOne(bool emp = false) const						{ return (((t == TP_INT || t == TP_BIN) && v.i == 1) || (t == TP_REAL && v.r == 1) || (emp && t == TP_EMPTY)); }

        /**
         * return numeric value as real number
         * @return          numeric value or default if other value
         * @param df        default value
         */
        inline realType numAsReal(realType df = 0) const                    { return (t == TP_INT || t == TP_BIN ? (realType)v.i : (t == TP_REAL ? v.r : df)); }

        /**
         * return numeric value as integer number
         * @return          numeric value or default if other value
         * @param df        default value
         */
        inline intType numAsInt(intType df = 0) const                       { return (t == TP_INT || t == TP_BIN ? v.i : (t == TP_REAL ? (intType)v.r : df)); }

        /**
         * add another numeric value to this numeric value (this must be isScalarNumber())
         * @param v2		value to add (must be isScalarNumber())
         * @param neg		substract value v2
         */
        void numAdd(const CmplVal& v2, bool neg = false);

        /**
         * multiply another numeric value to this numeric value (this must be isScalarNumber())
         * @param v2		value to multiply (must be isScalarNumber())
         */
        void numMult(const CmplVal &v2);

        /**
         * compare numerical values
         * @param v1        first value (must be TP_INT, TP_BIN, TP_REAL or TP_INFINITE)
         * @param v2        second value (must be TP_INT, TP_BIN, TP_REAL or TP_INFINITE)
         * @return          -1 if first value lesser second value / 0 if both are equal / 1 if first value greater second value
         */
        static int numCmp(const CmplVal& v1, const CmplVal& v2);

        /**
         * compare numerical values, this is first value (must be TP_INT, TP_BIN, TP_REAL or TP_INFINITE)
         * @param v2        second value (must be TP_INT, TP_BIN, TP_REAL or TP_INFINITE)
         * @return          -1 if first value lesser second value / 0 if both are equal / 1 if first value greater second value
         */
        int numCmp(const CmplVal& v2) const                            { return numCmp(*this, v2); }

        /**
         * compare numerical values, this is first value (must be TP_INT, TP_BIN, TP_REAL or TP_INFINITE)
         * @param i2        second value
         * @return          -1 if first value lesser second value / 0 if both are equal / 1 if first value greater second value
         */
        int numCmp(int i2) const                                        { CmplVal v2(TP_INT, (intType)i2); return numCmp(*this, v2); }

        /**
         * compare numerical values, this is first value (must be TP_INT, TP_BIN, TP_REAL or TP_INFINITE)
         * @param r2        second value
         * @return          -1 if first value lesser second value / 0 if both are equal / 1 if first value greater second value
         */
        int numCmp(realType r2) const                                   { CmplVal v2(TP_REAL, r2); return numCmp(*this, v2); }

		/**
		 * try to get this value as a string value
		 * @param r			return of string value (gets type TP_STR or TP_STRINGP)
		 * @param tcl		conversion level
		 * @param modp		calling module
		 * @param disp		return true, if the caller has to dispose r after use
		 * @return			true if conversion is successful
		 */
        bool toString(CmplVal& r, TypeConversionLevel tcl, ModuleBase *modp, bool &disp) const;

		/**
		 * return string value as char pointer
		 * @param modp		calling module
		 * @return			char pointer to string / NULL: no string
		 */
        const char *stringAsCharP(ModuleBase *modp) const;

		/**
		 * if this is TP_STRINGP, then change it to TP_STR
         * @param modp		calling module
         * @param disp		free pointer to string
         */
        void stringPToStr(ModuleBase *modp, bool disp = true);

        /**
         * get numeric value as string suitable for output, this must be TP_REAL, TP_INT or TP_BIN
         * @param rf		format spezifier for real number output or NULL for standard format
         * @param ms		max length of output string
         * @return 			numeric value as string
         */
        string outStringNum(const char *rf, unsigned ms) const;

        /**
         * try to get this value as an index value
         * @param r			return of index value (gets type TP_INT, TP_STR or TP_ITUPLE)
         * @param tcl		conversion level
         * @param modp		calling module (if NULL, then no conversion from stored string)
         * @return			true if conversion is successful
         */
        bool toIndexVal(CmplVal& r, TypeConversionLevel tcl, ModuleBase *modp);

        /**
         * try to get this value as a set
         * @param r			return of set (gets type TP_SET_*)
         * @param tcl		conversion level
         * @param modp		calling module (if NULL, then no conversion from stored string)
         * @return			true if conversion is successful
         */
        bool toSet(CmplVal& r, TypeConversionLevel tcl, ModuleBase *modp);


        // base type
        /**
         * get base data type for type
         * @param tp        given type
         * @return          base type
         */
        static tp_e getBaseType(tp_e tp);

        /**
         * get base data type of value
         * @return          base type
         */
        tp_e getBaseType()                              { return getBaseType(t); }
    };


    /**
     * like <code>CmplVal</code> but with disposing destructor
     */
    class CmplValAuto: public CmplVal
    {
    public:
        /**
         * constructor: as in base class
         */
        template<class ... Args> CmplValAuto(Args ... args): CmplVal(args ...)    { }

        /**
         * destructor
         */
        ~CmplValAuto()          { dispose(); }
    };


    /**
     * <code>OutputValueToLongException</code> is thrown if the output of a value needs more chars than possible
     */
    class OutputValueToLongException : public exception     //TODO: besser von range_error ableiten? oder stattdessen range_error direkt verwenden
    {
        /**
         * get exception info
         */
        virtual const char *what() const throw()				{ return "value is too long in its output representation"; }
    };



    /**
     * if a pointer variable is set, then delete content and set pointer to NULL
     */
    #define DELETE_UNSET(p)             if (p) { delete p; p = NULL; }
    #define DELETE_ARR_UNSET(p)         if (p) { delete[] p; p = NULL; }

}

#endif // CMPLVAL_HH

