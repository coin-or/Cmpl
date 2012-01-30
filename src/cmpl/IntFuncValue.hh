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
 * here for functions for fetching and storing values.
 */

#ifndef CMPL_INTFUNCVALUE_HH
#define CMPL_INTFUNCVALUE_HH

#include "IntFuncBase.hh"


namespace cmpl
{
	/**
	 * The <code>IntFuncLitVal</code> class is used for
	 * push a literal value on the value stack.
	 */
	class IntFuncLitVal: public IntFuncBase
	{
	public:
		/**
		 * constructor
		 */
		inline IntFuncLitVal(CmplCore *m): IntFuncBase(m) {}

		/**
		 * executes the command represented by this class.
		 * @param vp    pointer to the current command in the intermediary code
		 */
		virtual void exec(val_str *vp);
	};


	/**
	 * The <code>IntFuncLitValShort</code> class is used for
	 * push an embedded literal short int value on the value stack.
	 */
	class IntFuncLitValShort: public IntFuncBase
	{
	public:
		/**
		 * constructor
		 */
		inline IntFuncLitValShort(CmplCore *m): IntFuncBase(m) {}

		/**
		 * executes the command represented by this class.
		 * @param vp    pointer to the current command in the intermediary code
		 */
		virtual void exec(val_str *vp);
	};


	/**
	 * The <code>IntFuncLitSet</code> class is used for
	 * construct a new set and push it on the value stack.
	 */
	class IntFuncLitSet: public IntFuncBase
	{
	public:
		/**
		 * constructor
		 */
		inline IntFuncLitSet(CmplCore *m): IntFuncBase(m) {}

		/**
		 * executes the command represented by this class.
		 * @param vp    pointer to the current command in the intermediary code
		 */
		virtual void exec(val_str *vp);
	};


	/**
	 * The <code>IntFuncConstructTupel</code> class is used for
	 * construct a new tupel or tupelset by tupel construction and push it on the value stack.
	 */
	class IntFuncConstructTupel: public IntFuncBase
	{
	public:
		/**
		 * constructor
		 */
		inline IntFuncConstructTupel(CmplCore *m): IntFuncBase(m) {}

		/**
		 * executes the command represented by this class.
		 * @param vp    pointer to the current command in the intermediary code
		 */
		virtual void exec(val_str *vp);
	};


	/**
	 * The <code>IntFuncConstructTupelset</code> class is used for
	 * construct a new tupelset from an enumeration of tupels or tupelsets and push it on the value stack.
	 */
	class IntFuncConstructTupelset: public IntFuncBase
	{
	public:
		/**
		 * constructor
		 */
		inline IntFuncConstructTupelset(CmplCore *m): IntFuncBase(m) {}

		/**
		 * executes the command represented by this class.
		 * @param vp    pointer to the current command in the intermediary code
		 */
		virtual void exec(val_str *vp);
	};


	/**
	 * The <code>IntFuncFetchConst</code> class is used for
	 * fetch a value and push it on the value stack.
	 */
	class IntFuncFetchConst: public IntFuncBase
	{
	public:
		/**
		 * constructor
		 */
		inline IntFuncFetchConst(CmplCore *m): IntFuncBase(m) {}

		/**
		 * executes the command represented by this class.
		 * @param vp    pointer to the current command in the intermediary code
		 */
		virtual void exec(val_str *vp);

	protected:
		/**
		 * fetchs one value or values of an array and pushs they on the value stack.
		 * @param vp	pointer to the current command in the intermediary code
		 * @param fl	true: fetch values of an array / false: fetch a single value
		 */
		void ffetch_const(val_str *vp, bool fl);
	};


	/**
	 * The <code>IntFuncFConstFree</code> class is used for
	 * fetch the current value of an array within an implicit loop and push it on the value stack.
	 */
	class IntFuncFConstFree: public IntFuncFetchConst
	{
	public:
		/**
		 * constructor
		 */
		inline IntFuncFConstFree(CmplCore *m): IntFuncFetchConst(m) {}

		/**
		 * executes the command represented by this class.
		 * @param vp    pointer to the current command in the intermediary code
		 */
		virtual void exec(val_str *vp);
	};


	/**
	 * The <code>IntFuncFetchVals</code> class is used for
	 * fetch all values of an array and push they on the value stack.
	 */
	class IntFuncFetchVals: public IntFuncBase
	{
	public:
		/**
		 * constructor
		 */
		inline IntFuncFetchVals(CmplCore *m): IntFuncBase(m) {}

		/**
		 * executes the command represented by this class.
		 * @param vp    pointer to the current command in the intermediary code
		 */
		virtual void exec(val_str *vp);

	protected:
		/**
		 * fetchs all values of an array and push they on the value stack.
		 * @param array		array
		 * @param mi		number of dimensions
		 * @param ii		recursion level
		 * @return			number of found values
		 */
		long ffetch_vals(array_str *array, int mi, int ii);

		/**
		 * pushs a value on the value stack, if it is not empty.
		 * @param up		value
		 * @param tp		type of param <code>up</code>
		 * @return			number of found values (0 or 1)
		 */
		long ffetch_val(union ptr_sym_u up, enum ptrtp tp);
	};


	/**
	 * The <code>IntFuncFetchTmp</code> class is used for
	 * fetch the value of a temporary symbol and push it on the value stack.
	 */
	class IntFuncFetchTmp: public IntFuncBase
	{
	public:
		/**
		 * constructor
		 */
		inline IntFuncFetchTmp(CmplCore *m): IntFuncBase(m) {}

		/**
		 * executes the command represented by this class.
		 * @param vp    pointer to the current command in the intermediary code
		 */
		virtual void exec(val_str *vp);
	};


	/**
	 * The <code>IntFuncStoreTmp</code> class is used for
	 * set the value of a temporary symbol.
	 */
	class IntFuncStoreTmp: public IntFuncBase
	{
	public:
		/**
		 * constructor
		 */
		inline IntFuncStoreTmp(CmplCore *m): IntFuncBase(m) {}

		/**
		 * executes the command represented by this class.
		 * @param vp    pointer to the current command in the intermediary code
		 */
		virtual void exec(val_str *vp);
	};
}


#endif // !CMPL_INTFUNCVALUE_HH

