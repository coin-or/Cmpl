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
 * here for functions for generating lines in the result matrix.
 */

#ifndef CMPL_INTFUNCLINE_HH
#define CMPL_INTFUNCLINE_HH

#include "IntFuncBase.hh"


namespace cmpl
{
	/**
	 * The <code>IntFuncLineBase</code> class is the base class
	 * for the classes for generating lines in the result matrix.
	 */
	class IntFuncLineBase: public IntFuncBase
	{
	public:
		/**
		 * constructor
		 */
		inline IntFuncLineBase(CmplCore *m): IntFuncBase(m) {}

	protected:
		/**
		 * executes a function for all problem variables.
		 * @param func		function to execute
		 */ 
		void forall_var(int (cmpl::IntFuncLineBase::* func)(var_str *));

		/**
		 * executes a function for the problem variables in an array.
		 * @param array		array with problem variables
		 * @param func		function to execute
		 * @return			if <code>func</code> searches a specific variable then 1 if found, otherwise 0.
		 */
		int fforall_var(array_str *array, int (cmpl::IntFuncLineBase::* func)(var_str *));

		/**
		 * sets all coefficients for a problem variable in the current line of the result matrix to 0;
		 * this function is used as param <code>func</code> in <code>forall_var</code>.
		 * @param vp		problem variable
		 * @return			ever 0
		 */
		//int fdef_linebeg(var_str *vp);
		
		/**
		 * saves the coefficients for a problem variable in the current line of the result matrix;
		 * this function is used as param <code>func</code> in <code>forall_var</code>.
		 * @param vp		problem variable
		 * @return			ever 0
		 */
		//int flend_var(var_str *vp);

		/**
		 * marks the problem variable if it has a non zero coefficient in the current line of the result matrix;
		 * this function is used as param <code>func</code> in <code>forall_var</code>.
		 * @param vp		problem variable
		 * @return			ever 0
		 */
		//int ftouch_not0(var_str *vp);

		/**
		 * tests if the problem variable has a specific definition number;
		 * this function is used as param <code>func</code> in <code>forall_var</code>.
		 * @param vp		problem variable
		 * @return			1 if the specific definition number found, otherwise 0
		 */
		int ffvarp_defn(var_str *vp);

		/**
		 * compare two int numbers, for use in qsort()
		 */
		static int cmp_int(const void *p1, const void *p2);

		/**
		 * sets all coefficients for all problem variables in the current line of the result matrix to 0,
		 * for starting a new current line of the result matrix.
		 */
		void fline_beg();
		
		/**
		 * saves all coefficients for all problem variables in the current line of the result matrix,
		 * for ending the current line of the result matrix.
		 * @return			1 if a new current line has to start within an implicit loop, otherwise 0
		 */
		int fline_end();

		/**
		 * tests if the current line is necessary for the result matrix, or if it can changed to a bound.
		 * @return			1 if the current line is necessary, otherwise 0
		 */
		int ftest_dest();

		/**
		 * sets the lower bound for a problem variable.
		 * @param vp		problem variable
		 * @param bval		lower bound
		 * @param f			true if bval is treated as integer, false if bval is double
		 */
		void fset_dn_varb(var_str *vp, double bval, bool f);

		/**
		 * sets the upper bound for a problem variable.
		 * @param vp		problem variable
		 * @param bval		upper bound
		 * @param f			true if bval is treated as integer, false if bval is double
		 */
		void fset_up_varb(var_str *vp, double bval, bool f);

		/**
		 * joins two lines for the result matrix, in which all coefficients are proportional.
		 * @param lp		first line
		 * @param lp1		second line
		 * @param fakt		proportional factor
		 */
		int fset_lineb(dest_str *lp, dest_str *lp1, double fakt);

		/**
		 * procession for a line with lower and upper bound.
		 * @param lp		line
		 * @param uv		lower bound
		 * @param ov		upper bound
		 * @param nu		param <code>uv</code> contains a lower bound
		 * @param no		param <code>ov</code> contains an upper bound
		 */
		int make_lrange(dest_str *lp, val_str uv, val_str ov, bool nu, bool no);

		/**
		 * searches the problem variable with a given definition number.
		 * @param num		definition number
		 * @return			problem variable
		 */
		var_str *fvarp_defn(int num);
	};


	/**
	 * The <code>IntFuncLineBeg</code> class is used for
	 * start a new current line for the result matrix.
	 */
	class IntFuncLineBeg: public IntFuncLineBase
	{
	public:
		/**
		 * constructor
		 */
		inline IntFuncLineBeg(CmplCore *m): IntFuncLineBase(m) {}

		/**
		 * executes the command represented by this class.
		 * @param vp    pointer to the current command in the intermediary code
		 */
		virtual void exec(val_str *vp);
	};


	/**
	 * The <code>IntFuncLineMod</code> class is used for
	 * setting the mode (O, U, G, N) for current line for the result matrix.
	 */
	class IntFuncLineMod: public IntFuncLineBase
	{
	public:
		/**
		 * constructor
		 */
		inline IntFuncLineMod(CmplCore *m): IntFuncLineBase(m) {}

		/**
		 * executes the command represented by this class.
		 * @param vp    pointer to the current command in the intermediary code
		 */
		virtual void exec(val_str *vp);
	};


	/**
	 * The <code>IntFuncLineObjTo</code> class is used for
	 * mark the current line for the result matrix as objective to maximise or minimise.
	 */
	class IntFuncLineObjTo: public IntFuncLineBase
	{
	public:
		/**
		 * constructor
		 */
		inline IntFuncLineObjTo(CmplCore *m): IntFuncLineBase(m) {}

		/**
		 * executes the command represented by this class.
		 * @param vp    pointer to the current command in the intermediary code
		 */
		virtual void exec(val_str *vp);
	};


	/**
	 * The <code>IntFuncLineEnd</code> class is used for
	 * end the current line for the result matrix and save the coefficients of the line.
	 */
	class IntFuncLineEnd: public IntFuncLineBase
	{
	public:
		/**
		 * constructor
		 */
		inline IntFuncLineEnd(CmplCore *m): IntFuncLineBase(m) {}

		/**
		 * executes the command represented by this class.
		 * @param vp    pointer to the current command in the intermediary code
		 */
		virtual void exec(val_str *vp);
	};


	/**
	 * The <code>IntFuncVarLinBeg</code> class is used for
	 * start the linearisation of a product of problem variables
	 */
	class IntFuncVarLinBeg: public IntFuncBase
	{
	public:
		/**
		 * constructor
		 */
		inline IntFuncVarLinBeg(CmplCore *m): IntFuncBase(m) {}

		/**
		 * executes the command represented by this class.
		 * @param vp    pointer to the current command in the intermediary code
		 */
		virtual void exec(val_str *vp);
	};
}



#endif // !CMPL_INTFUNCLINE_HH

