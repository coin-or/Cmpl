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
 * here for functions for terms for coefficients of the result matrix.
 */

#ifndef CMPL_INTFUNCTERM_HH
#define CMPL_INTFUNCTERM_HH

#include "IntFuncBase.hh"

#include "Compiler.hh"		// used for generation of intermediary code for the linearization of products of problem variables
#include "Interpreter.hh"


namespace cmpl
{
	/**
	 * The <code>IntFuncTermBeg</code> class is used for
	 * the start of a new term for a coefficient.
	 */
	class IntFuncTermBeg: public IntFuncBase
	{
	public:
		/**
		 * constructor
		 */
		inline IntFuncTermBeg(CmplCore *m): IntFuncBase(m) {}

		/**
		 * executes the command represented by this class.
		 * @param vp    pointer to the current command in the intermediary code
		 */
		virtual void exec(val_str *vp);
	};


	/**
	 * The <code>IntFuncTermNegFak</code> class is used for
	 * negation of the sign of the term for a coefficient.
	 */
	class IntFuncTermNegFak: public IntFuncBase
	{
	public:
		/**
		 * constructor
		 */
		inline IntFuncTermNegFak(CmplCore *m): IntFuncBase(m) {}

		/**
		 * executes the command represented by this class.
		 * @param vp    pointer to the current command in the intermediary code
		 */
		virtual void exec(val_str *vp);
	};


	/**
	 * The <code>IntFuncTermVar</code> class is used for
	 * a problem variable in a term for a coefficient.
	 */
	class IntFuncTermVar: public IntFuncBase
	{
	public:
		/**
		 * constructor
		 */
		inline IntFuncTermVar(CmplCore *m): IntFuncBase(m) {}

		/**
		 * executes the command represented by this class.
		 * @param vp    pointer to the current command in the intermediary code
		 */
		virtual void exec(val_str *vp);

	protected:
		/**
		 * sets a problem variable for the term
		 * @param i		number of the variable in the symbol table
		 */
		void fterm_var(int i);
	};


	/**
	 * The <code>IntFuncTermVarFree</code> class is used for
	 * a problem variable with a free index in a term for a coefficient.
	 */
	class IntFuncTermVarFree: public IntFuncTermVar
	{
	public:
		/**
		 * constructor
		 */
		inline IntFuncTermVarFree(CmplCore *m): IntFuncTermVar(m) {}

		/**
		 * executes the command represented by this class.
		 * @param vp    pointer to the current command in the intermediary code
		 */
		virtual void exec(val_str *vp);
	};


	/**
	 * The <code>IntFuncTermEnd</code> class is used for
	 * the end of a term for a coefficient, also makes the linearization of products of problem variables.
	 */
	class IntFuncTermEnd: public IntFuncBase
	{
	public:
		/**
		 * constructor
		 */
		inline IntFuncTermEnd(CmplCore *m): IntFuncBase(m) {}

		/**
		 * executes the command represented by this class.
		 * @param vp    pointer to the current command in the intermediary code
		 */
		virtual void exec(val_str *vp);

	private:
		/**
		 * processes a term without a problem variable.
		 */
		void fkum_fonds();

		/**
		 * saves the coefficient for a problem variable in the result matrix.
		 */
		void fkum_vkoeff();

		/**
		 * iterates over a matrix product.
		 */
		void fvprod_iter();

		/**
		 * inserts a new generated problem variable.
		 * @param sym_p		place in the symbol table
		 * @param mi		number of indexes
		 * @param vpp		returns the inserted variable
		 */
		void make_test_var(symbol_str *sym_p, int mi, var_str **vpp);

#ifdef VAR_PROD
	private:
		/**
		 * tests if the factors of a product of problem variables should swaped, then swap.
		 * @return		1: a problem variable is removed from the product because of an error
		 */
		int tv_test_swap_fakt();

		/**
		 * shifts problem variables in the info for the product, after replacing by generated variables.
		 * @param von	start position to shift
		 * @param bis	end position to shift
		 * @param anz	shift width
		 */
		void tv_shift(int von, int bis, int anz);

		/**
		 * tests if for a product already exists a linearization and in this case replace the product by the generated variable.
		 * @return		number of remaining variables in the product
		 */
		int search_rep_var_prod();

		/**
		 * searches an already existing binary dissection for an integer problem variable, or generate a new one.
		 * @param cmplcomp	compiler object for generating intermediary code
		 * @param tvs		resulting binary dissection
		 * @param tvf		info for problem variable to dissect
		 * @return			0: ok / 1: error
		 */
		int search_make_split(Compiler &cmplcomp, struct term_var_str::tv_str *tvs, struct term_var_str::tv_str *tvf);

		/**
		 * searches an already existing normalization for a real problem variable, or generate a new one.
		 * @param cmplcomp	compiler object for generating intermediary code
		 * @param tvn		resulting normalization
		 * @param tvf		info for problem variable to normalize
		 * @return			0: ok / 1: error
		 */
		int search_make_norm(Compiler &cmplcomp, struct term_var_str::tv_str *tvn, struct term_var_str::tv_str *tvf);

		/**
		 * stores the linearization of a product of two problem variables.
		 * @param tvp		linearization of the product
		 * @param tvf1		first problem variable
		 * @param tvf2		second problem variable
		 */
		void ins_var_prod(struct term_var_str::tv_str *tvp, struct term_var_str::tv_str *tvf1, struct term_var_str::tv_str *tvf2);

		/**
		 * replaces a product of problem variables by a new variable, generated the necessary constraints.
		 * @param cmplcomp	compiler object for generating intermediary code
		 * @return			0: ok / 1: error
		 */
		int rep_prod(Compiler &cmplcomp);

		/**
		 * generates a new variable for replacing a product or for a binary dissection.
		 * @param cmplcomp	compiler object for generating intermediary code
		 * @param tvp		resulting variable
		 * @param tvf1		first variable
		 * @param tvf2		second variable for a product or null for a binary dissection
		 * @param sn		number of variable to generate for a binary dissection / -1: make a normalization for a real problem variable
		 * @return			0: ok / 1: error
		 */
		int make_prod_var(Compiler &cmplcomp, struct term_var_str::tv_str *tvp, struct term_var_str::tv_str *tvf1, struct term_var_str::tv_str *tvf2, int sn);

		/**
		 * generates the intermediary code for one constraint for the linearization of a product of problem variables.
		 * @param cmplcomp	compiler object for generating intermediary code
		 * @param tvp		product variable
		 * @param tvf1		first problem variable
		 * @param tvf2		second problem variable or null
		 * @param bv		bound value for the constraint
		 * @param bf		true: bv is an integer value / false: a real value
		 * @param lm		bv is an upper bound (MODE_O) or a lower bound (MODE_U)
		 * @param tmp_num	number of context for a temporary counter for a binary dissection
		 */
		void prod_rest_comp(Compiler &cmplcomp, struct term_var_str::tv_str *tvp, struct term_var_str::tv_str *tvf1, struct term_var_str::tv_str *tvf2, double bv, bool bf, int lm, int tmp_num);
#endif
	};
}


#endif // !CMPL_INTFUNCTERM_HH

