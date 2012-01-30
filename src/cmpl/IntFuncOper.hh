/***********************************************************************
 *  This code is part of CMPL
 *
 *  Copyright (C) 2007, 2008, 2009, 2010 Thomas Schleiff - Halle(Saale), 
 *  Germany and Mike Steglich - Technical University of Applied Sciences
 *  Wildau, Germany 
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


/**
 * classes for executing functions of the intermediary code,
 * here for functions for operations and built-in functions.
 */

#ifndef CMPL_INTFUNCOPER_HH
#define CMPL_INTFUNCOPER_HH

#include "IntFuncBase.hh"


namespace cmpl
{
	/**
	 * The <code>IntFuncNumOp</code> class is used for
	 * a numerical operation or some other operations.
	 */
	class IntFuncNumOp: public IntFuncBase
	{
	public:
		/**
		 * constructor
		 */
		inline IntFuncNumOp(CmplCore *m): IntFuncBase(m) {}

		/**
		 * executes the command represented by this class.
		 * @param vp    pointer to the current command in the intermediary code
		 */
		virtual void exec(val_str *vp);
	
	protected:
		/**
		 * concats two strings, the result is pushed at the value stack.
		 * @param v1    first string value
		 * @param v2	second string value
		 */
		void fstrcat(val_str *v1, val_str *v2);

		/**
		 * executes a set operation, the result is pushed at the value stack.
		 * @param v1	first set value, also determines the order of elements in the result set
		 * @param v2	second set value
		 */
		void fset_op(int op, val_str v1, val_str v2);

		/**
		 * gives a set with the same elements like an input set, but in the canonical order.
		 * @param vi	input set
		 * @param vo	returns the result set
		 */
		void order_set(val_str *vi, val_str *vo);
	};


	/**
	 * The <code>IntFuncSetOp</code> class is used for
	 * set operations.
	 */
	class IntFuncSetOp: public IntFuncNumOp
	{
	public:
		/**
		 * constructor
		 */
		inline IntFuncSetOp(CmplCore *m): IntFuncNumOp(m) {}

		/**
		 * executes the command represented by this class.
		 * @param vp    pointer to the current command in the intermediary code
		 */
		virtual void exec(val_str *vp);
	};


	/**
	 * The <code>IntFuncLogOp</code> class is used for
	 * boolean operations.
	 */
	class IntFuncLogOp: public IntFuncBase
	{
	public:
		/**
		 * constructor
		 */
		inline IntFuncLogOp(CmplCore *m): IntFuncBase(m) {}

		/**
		 * executes the command represented by this class.
		 * @param vp    pointer to the current command in the intermediary code
		 */
		virtual void exec(val_str *vp);
	};


	/**
	 * The <code>IntFuncCmpOp</code> class is used for
	 * compare operations.
	 */
	class IntFuncCmpOp: public IntFuncBase
	{
	public:
		/**
		 * constructor
		 */
		inline IntFuncCmpOp(CmplCore *m): IntFuncBase(m) {}

		/**
		 * executes the command represented by this class.
		 * @param vp    pointer to the current command in the intermediary code
		 */
		virtual void exec(val_str *vp);

	private:
		/**
		 * compares two int values.
		 * @param op	compare operation (EQ, EQ2, GE, LE)
		 * @param v1	first value
		 * @param v2	second value
		 * @return		compare result
		 */
		bool fcmp_int(int op, int v1, int v2);

		/**
		 * compares two double values.
		 * @param op	compare operation (EQ, EQ2, GE, LE)
		 * @param v1	first value
		 * @param v2	second value
		 * @return		compare result
		 */
		bool fcmp_dbl(int op, double v1, double v2);

		/**
		 * compares two string values.
		 * @param op	compare operation (EQ, EQ2, GE, LE)
		 * @param v1	first value
		 * @param v2	second value
		 * @return		compare result
		 */
		bool fcmp_str(int op, char *v1, char *v2);

		/**
		 * compares two set values.
		 * @param op	compare operation (EQ, EQ2, GE, LE, GT, LT)
		 * @param set1	first value
		 * @param set2	second value
		 * @return		compare result
		 */
		bool fcmp_set(int op, val_str *set1, val_str *set2);
	};


	/**
	 * The <code>IntFuncCountOp</code> class is used for
	 * a count operation.
	 */
	class IntFuncCountOp: public IntFuncBase
	{
	public:
		/**
		 * constructor
		 */
		inline IntFuncCountOp(CmplCore *m): IntFuncBase(m) {}

		/**
		 * executes the command represented by this class.
		 * @param vp    pointer to the current command in the intermediary code
		 */
		virtual void exec(val_str *vp);
	};


	/**
	 * The <code>IntFuncConvOp</code> class is used for
	 * a convert operation.
	 */
	class IntFuncConvOp: public IntFuncBase
	{
	public:
		/**
		 * constructor
		 */
		inline IntFuncConvOp(CmplCore *m): IntFuncBase(m) {}

		/**
		 * executes the command represented by this class.
		 * @param vp    pointer to the current command in the intermediary code
		 */
		virtual void exec(val_str *vp);
	};


	/**
	 * The <code>IntFuncTypeStr</code> class is used for
	 * giving a type string for an expression or a problem variable.
	 */
	class IntFuncTypeStr: public IntFuncBase
	{
	public:
		/**
		 * constructor
		 */
		inline IntFuncTypeStr(CmplCore *m): IntFuncBase(m) {}

		/**
		 * executes the command represented by this class.
		 * @param vp    pointer to the current command in the intermediary code
		 */
		virtual void exec(val_str *vp);
	};


	/**
	 * The <code>IntFuncMathFunc</code> class is used for
	 * a mathematical built-in function.
	 */
	class IntFuncMathFunc: public IntFuncBase
	{
	public:
		/**
		 * constructor
		 */
		inline IntFuncMathFunc(CmplCore *m): IntFuncBase(m) {}

		/**
		 * executes the command represented by this class.
		 * @param vp    pointer to the current command in the intermediary code
		 */
		virtual void exec(val_str *vp);

	private:
		/**
		 * output of an error message for a mathematical built-in function.
		 * @param func	name of the function
		 */
		void f_math_func_error(const char *func);
	};


	/**
	 * The <code>IntFuncAggrOp</code> class is used for
	 * an aggregate built-in function (max or min).
	 */
	class IntFuncAggrOp: public IntFuncBase
	{
	public:
		/**
		 * constructor
		 */
		inline IntFuncAggrOp(CmplCore *m): IntFuncBase(m) {}

		/**
		 * executes the command represented by this class.
		 * @param vp    pointer to the current command in the intermediary code
		 */
		virtual void exec(val_str *vp);
	};


	/**
	 * The <code>IntFuncUtilOp</code> class is used for
	 * other functions (at time only len for strings and sets).
	 */
	class IntFuncUtilOp: public IntFuncBase
	{
	public:
		/**
		 * constructor
		 */
		inline IntFuncUtilOp(CmplCore *m): IntFuncBase(m) {}

		/**
		 * executes the command represented by this class.
		 * @param vp    pointer to the current command in the intermediary code
		 */
		virtual void exec(val_str *vp);
	};


	/**
	 * The <code>IntFuncDefFunc</code> class is used for
	 * counting how much elements of an array are defined.
	 */
	class IntFuncDefFunc: public IntFuncBase
	{
	public:
		/**
		 * constructor
		 */
		inline IntFuncDefFunc(CmplCore *m): IntFuncBase(m) {}

		/**
		 * executes the command represented by this class.
		 * @param vp    pointer to the current command in the intermediary code
		 */
		virtual void exec(val_str *vp);

	private:
		/**
		 * counts how much elements in an array are defined.
		 * @param array	array with elements to count
		 * @param mi	number of dimensions in the start array of the recursion
		 * @param ii	current level of recursion
		 * @return		number of defined elements
		 */
		long ff_def_count(array_str *array, int mi, int ii);
	};


	/**
	 * The <code>IntFuncDefsetFunc</code> class is used for
	 * giving the definition set for the first free index of an array.
	 */
	class IntFuncDefsetFunc: public IntFuncBase
	{
	public:
		/**
		 * constructor
		 */
		inline IntFuncDefsetFunc(CmplCore *m): IntFuncBase(m) {}

		/**
		 * executes the command represented by this class.
		 * @param vp    pointer to the current command in the intermediary code
		 */
		virtual void exec(val_str *vp);
	};


	/**
	 * The <code>IntFuncDefsetExtnum</code> class is used for
	 * giving the lowest or highest numerical value of the definition set for the first free index of an array.
	 */
	class IntFuncDefsetExtnum: public IntFuncBase
	{
	public:
		/**
		 * constructor
		 */
		inline IntFuncDefsetExtnum(CmplCore *m): IntFuncBase(m) {}

		/**
		 * executes the command represented by this class.
		 * @param vp    pointer to the current command in the intermediary code
		 */
		virtual void exec(val_str *vp);

	private:
		/**
		 * gives the lowest or highest numerical value of the intersection between the definition set of an array and a second set.
		 * @param op		search lowest or highest value (MIN_VAL or MAX_VAL)
		 * @param array		definition set of this array is used
		 * @param set		second set
		 * @param is_res	returns if a result is found
		 * @return			result value
		 */
		long defset_get_extnum(int op, array_str *array, val_str *set, bool *is_res);
	};
}


#endif // !CMPL_INTFUNCOPER_HH

