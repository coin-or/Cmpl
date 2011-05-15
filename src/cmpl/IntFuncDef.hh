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
		inline IntFuncDefBase(Main *m): IntFuncBase(m) {}

	protected:
		/**
		 * definition of a parameter or variable,
		 * builds the data structure to represent a multidimensional array.
		 * @param sym_p		data structure for the symbol
		 * @param c_i		number of dimensions
		 * @param arr_p		current array, null at the start of recursion
		 * @param ii		recursion level
		 * @param mod		definition of parameter or variable
		 * @param tp_cd		explicit given type for parameter or TPP_EMPTY for none
		 */
		void fdef_vekts(symbol_str *sym_p, int c_i, array_str *arr_p, int ii, int mod, enum etp tp_cd);

		/**
		 * definition of a single parameter or variable.
		 * @param sym_p		data structure for the symbol
		 * @param up		pointer to the place for the parameter or variable
		 * @param mod		definition of parameter or variable
		 * @param tp_cd		explicit given type for parameter or TPP_EMPTY for none
		 */
		void fdef_ins_val(symbol_str *sym_p, union ptr_sym_u *up, int mod, enum etp tp_cd);

		/**
		 * definition of a single parameter.
		 * @param up		pointer to the place for the parameter
		 * @param tp_cd		explicit given type for parameter or TPP_EMPTY for none
		 */
		void fdef_const_val(union ptr_sym_u *up, enum etp tp_cd);

		/**
		 * definition of a set.
		 * @param up		pointer to the place for the parameter
		 * @param tp_cd		explicit given type for parameter or TPP_EMPTY for none
		 */
		void fdef_set_val(union ptr_sym_u *up, enum etp tp_cd);

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
		inline IntFuncDefConst(Main *m): IntFuncDefBase(m) {}

		/**
		 * executes the command represented by this class.
		 * @param vp    pointer to the current command in the intermediary code
		 */
		virtual void exec(val_str *vp);
	};


	/**
	 * The <code>IntFuncProgEnd</code> class is used for
	 * the definition of a problem variable or an array of variables.
	 */
	class IntFuncDefVar: public IntFuncDefBase
	{
	public:
		/**
		 * constructor
		 */
		inline IntFuncDefVar(Main *m): IntFuncDefBase(m) {}

		/**
		 * executes the command represented by this class.
		 * @param vp    pointer to the current command in the intermediary code
		 */
		virtual void exec(val_str *vp);
	};


	/**
	 * The <code>IntFuncProgEnd</code> class is used for
	 * reset the bounds of problem variables.
	 */
	class IntFuncResetVarB: public IntFuncBase
	{
	public:
		/**
		 * constructor
		 */
		inline IntFuncResetVarB(Main *m): IntFuncBase(m) {}

		/**
		 * executes the command represented by this class.
		 * @param vp    pointer to the current command in the intermediary code
		 */
		virtual void exec(val_str *vp);
	};


	/**
	 * The <code>IntFuncProgEnd</code> class is used for
	 * definition of a lower bound for variables.
	 */
	class IntFuncDefDnVar: public IntFuncBase
	{
	public:
		/**
		 * constructor
		 */
		inline IntFuncDefDnVar(Main *m): IntFuncBase(m) {}

		/**
		 * executes the command represented by this class.
		 * @param vp    pointer to the current command in the intermediary code
		 */
		virtual void exec(val_str *vp);
	};


	/**
	 * The <code>IntFuncProgEnd</code> class is used for
	 * definition of an upper bound for variables.
	 */
	class IntFuncDefUpVar: public IntFuncBase
	{
	public:
		/**
		 * constructor
		 */
		inline IntFuncDefUpVar(Main *m): IntFuncBase(m) {}

		/**
		 * executes the command represented by this class.
		 * @param vp    pointer to the current command in the intermediary code
		 */
		virtual void exec(val_str *vp);
	};


	/**
	 * The <code>IntFuncProgEnd</code> class is used for
	 * provide variables with a type and with bounds.
	 */
	class IntFuncDefVarType: public IntFuncBase
	{
	public:
		/**
		 * constructor
		 */
		inline IntFuncDefVarType(Main *m): IntFuncBase(m) {}

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
}


#endif // !CMPL_INTFUNCDEF_HH

