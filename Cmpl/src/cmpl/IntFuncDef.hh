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
 * here for functions for definition of parameters and variables.
 */

#ifndef CMPL_INTFUNCDEF_HH
#define CMPL_INTFUNCDEF_HH

#include "IntFuncBase.hh"


namespace cmpl
{
	/**
	 * The <code>IntFuncDefBase</code> class is the base class
	 * for the classes for definition of parameters and variables.
	 */
	class IntFuncDefBase: public IntFuncBase
	{
	protected:
		/**
		 * constructor
		 */
		inline IntFuncDefBase(CmplCore *m): IntFuncBase(m) {}

	protected:
		/**
		 * definition of a parameter or variable,
		 * builds the data structure to represent a multidimensional array.
		 * @param sym_p		data structure for the symbol
		 * @param c_i		number of dimensions	//TODO
		 * @param mod		definition of parameter or variable
		 * @param tp_cd		explicit given type for parameter or TPP_EMPTY for none
		 * @param dim_rhs	rank of right hand side expression / -1 if mod is DEF_VAR
		 * @param indices	not only the values but also the indices come from the stack (can only be used with dim_rhs == 1)
		 * @param vpall		use this value for all / NULL: get the values from stack
		 */
		void fdef_val(symbol_str *sym_p, int c_i, int mod, enum etp tp_cd, int dim_rhs, bool indices, val_str *vpall);

		/**
		 * definition of a parameter or variable,
		 * builds the data structure to represent a multidimensional array (not for tupelsets).
		 * @param sym_p		data structure for the symbol
		 * @param c_i		number of dimensions
		 * @param arr_p		current array, null at the start of recursion
		 * @param ii		recursion level
		 * @param mod		definition of parameter or variable
		 * @param tp_cd		explicit given type for parameter or TPP_EMPTY for none
		 * @param vpall		use this value for all / NULL: get the values from stack
		 */
		void fdef_vekts(symbol_str *sym_p, int c_i, array_str *arr_p, int ii, int mod, enum etp tp_cd, val_str *vpall);

		/**
		 * definition of an array of parameters or variables defined over a tupelset.
		 * @param sym_p		data structure for the symbol
		 * @param mod		definition of parameter or variable
		 * @param tp_cd		explicit given type for parameter or TPP_EMPTY for none
		 * @param tps		tupelset with defining tupels of the array
		 * @param indices	not only the values but also the indices come from the stack (can only be used with dim_rhs == 1)
		 * @param vpall		use this value for all / NULL: get the values from stack
		 */
		void fdef_arr_tps(symbol_str *sym_p, int mod, enum etp tp_cd, Tupelset *tps, bool indices, val_str *vpall);

		/**
		 * definition of a single parameter or variable.
		 * @param sym_p		data structure for the symbol
		 * @param up		pointer to the place for the parameter or variable
		 * @param mod		definition of parameter or variable
		 * @param tp_cd		explicit given type for parameter or TPP_EMPTY for none
		 * @param vp		use this value / NULL: get the value from stack
		 */
		void fdef_ins_val(symbol_str *sym_p, union ptr_sym_u *up, int mod, enum etp tp_cd, val_str *vp);

		/**
		 * definition of a single parameter.
		 * @param up		pointer to the place for the parameter
		 * @param tp_cd		explicit given type for parameter or TPP_EMPTY for none
		 * @param vp		use this value / NULL: get the value from stack
		 */
		void fdef_const_val(union ptr_sym_u *up, enum etp tp_cd, val_str *vp);

		/**
		 * definition of a set.
		 * @param up		pointer to the place for the parameter
		 * @param tp_cd		explicit given type for parameter or TPP_EMPTY for none
		 * @param vp		use this value / NULL: get the value from stack
		 */
		void fdef_set_val(union ptr_sym_u *up, enum etp tp_cd, val_str *vp);

		/**
		 * definition of a tupel or tupelset.
		 * @param up		pointer to the place for the parameter
		 * @param tp_cd		explicit given type for parameter or TPP_EMPTY for none (not used at time)
		 * @param tps		true: tupelset / false: tupel
		 * @param rank		rank of the tupel or tupelset
		 * @param vp		use this value / NULL: get the value from stack
		 */
		void fdef_tupel_val(union ptr_sym_u *up, enum etp tp_cd, bool tps, int rank, val_str *vp);

		/**
		 * definition of a cmpl proplem.
		 * @param up		pointer to the place for the parameter
		 * @param tp_cd		explicit given type for parameter or TPP_EMPTY for none
		 * @param sub_symtab	subsymbol table for the cmpl proplem
		 * @param vp		use this value / NULL: get the value from stack
		 */
		void fdef_problem_val(union ptr_sym_u *up, enum etp tp_cd, SymbolTab *sub_symtab, val_str *vp);

		/**
		 * definition of a variable.
		 * @param up		pointer to the place for the variable
		 */
		void fdef_var(union ptr_sym_u *up);	
	};


	/**
	 * The <code>IntFuncDefConst</code> class is used for
	 * the definition of a parameter or a parameter array.
	 */
	class IntFuncDefConst: public IntFuncDefBase
	{
	public:
		/**
		 * constructor
		 */
		inline IntFuncDefConst(CmplCore *m): IntFuncDefBase(m) {}

		/**
		 * executes the command represented by this class.
		 * @param vp    pointer to the current command in the intermediary code
		 */
		virtual void exec(val_str *vp);
		
	private:
		val_str *searchStackList(val_str *vp, int dim);
	};


	/**
	 * The <code>IntFuncDefVar</code> class is used for
	 * the definition of a problem variable or an array of variables.
	 */
	class IntFuncDefVar: public IntFuncDefBase
	{
	public:
		/**
		 * constructor
		 */
		inline IntFuncDefVar(CmplCore *m): IntFuncDefBase(m) {}

		/**
		 * executes the command represented by this class.
		 * @param vp    pointer to the current command in the intermediary code
		 */
		virtual void exec(val_str *vp);
	};


	/**
	 * The <code>IntFuncResetVarB</code> class is used for
	 * reset the bounds of problem variables.
	 */
	class IntFuncResetVarB: public IntFuncBase
	{
	public:
		/**
		 * constructor
		 */
		inline IntFuncResetVarB(CmplCore *m): IntFuncBase(m) {}

		/**
		 * executes the command represented by this class.
		 * @param vp    pointer to the current command in the intermediary code
		 */
		virtual void exec(val_str *vp);
	};


	/**
	 * The <code>IntFuncDefDnVar</code> class is used for
	 * definition of a lower bound for variables.
	 */
	class IntFuncDefDnVar: public IntFuncBase
	{
	public:
		/**
		 * constructor
		 */
		inline IntFuncDefDnVar(CmplCore *m): IntFuncBase(m) {}

		/**
		 * executes the command represented by this class.
		 * @param vp    pointer to the current command in the intermediary code
		 */
		virtual void exec(val_str *vp);
	};


	/**
	 * The <code>IntFuncDefUpVar</code> class is used for
	 * definition of an upper bound for variables.
	 */
	class IntFuncDefUpVar: public IntFuncBase
	{
	public:
		/**
		 * constructor
		 */
		inline IntFuncDefUpVar(CmplCore *m): IntFuncBase(m) {}

		/**
		 * executes the command represented by this class.
		 * @param vp    pointer to the current command in the intermediary code
		 */
		virtual void exec(val_str *vp);
	};


	/**
	 * The <code>IntFuncDefVarType</code> class is used for
	 * provide variables with a type and with bounds.
	 */
	class IntFuncDefVarType: public IntFuncBase
	{
	public:
		/**
		 * constructor
		 */
		inline IntFuncDefVarType(CmplCore *m): IntFuncBase(m) {}

		/**
		 * executes the command represented by this class.
		 * @param vp    pointer to the current command in the intermediary code
		 */
		virtual void exec(val_str *vp);

	private:
		/**
		 * provides an array of variables with a type and with bounds.
		 * @param array_p	array of variables
		 * @param ii		recursion level
		 * @param vtype		type for the variables
		 */
		void fdef_vartype(array_str *array_p, int ii, enum etp vtype);

		/**
		 * provides a single variable with a type and with bounds.
		 * @param vp		info for the variable
		 * @param vtype		type for the variables
		 */
		void set_vartype_bounds(var_str *vp, enum etp vtype);
	};

	/**
	 * The <code>IntFuncInitCmplData</code> class is used to
	 * initialize the values for symbols defined in a cmplData file.
	 */
	class IntFuncInitCmplData: public IntFuncDefBase
	{
	public:
		/**
		 * constructor
		 */
		inline IntFuncInitCmplData(CmplCore *m): IntFuncDefBase(m) {}

		/**
		 * executes the command represented by this class.
		 * @param vp    pointer to the current command in the intermediary code
		 */
		virtual void exec(val_str *vp);

		/**
		 * initializes the values for one symbol from a cmplData file.
		 * called from the cmplData object.
		 * the caller must store the values on the stack before calling this function.
		 * @param symnr			symbol number of the symbol to initialize
		 * @param sets			array of the symbol numbers of the sets or tupelsets for the array to define, len of array is MAX_ANZ_IND
		 * @param count			count of values stored on the stack for the symbol, including the default value if given
		 * @param settp			type of set, only used if the symbol is a set
		 * @param modeIndices	the symbol use the mode "indices" in the cmplData file
		 * @param withDefault	a default value is used for the symbol values in the cmplData file
		 */
		void insertCmplDataValues(int symnr, int *sets, int count, enum etp settp, bool modeIndices, bool withDefault);

	private:
		const char *doInsertCmplDataValues(symbol_str *s_p, int count, enum etp settp, bool modeIndices, bool withDefault, int *errcnt);

		Tupelset *tupelsetForSetArray(int *sets);
	};
}


#endif // !CMPL_INTFUNCDEF_HH

