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
 * here for functions refering to the cmpl file.
 */


#ifndef CMPL_INTFUNCCODE_HH
#define CMPL_INTFUNCCODE_HH

#include "IntFuncBase.hh"


namespace cmpl
{
	/**
	 * The <code>IntFuncProgEnd</code> class is used for
	 * the end of the intermediary code and saving the generated matrix.
	 */
	class IntFuncProgEnd: public IntFuncBase
	{
	public:
		/**
		 * constructor
		 */
		inline IntFuncProgEnd(CmplCore *m): IntFuncBase(m) {}

		/**
		 * executes the command represented by this class.
		 * @param vp    pointer to the current command in the intermediary code
		 */
		virtual void exec(val_str *vp);

	private:
		/**
		 * generates for all problem variables of an array the name for the result matrix,
		 * and inserts the bounds for these variables.
		 * @param array	array of problem variables
		 * @param ii	recursion level
		 * @param gnump	counter for problem variables
		 */
		void fend_names(array_str *array, int ii, int *gnump);

		/**
		 * generates for one problem variable a name for the result matrix,
		 * and inserts the bounds for this variable.
		 * @param vp	info for the problem variable
		 * @param gnump	counter for problem variables
		 */
		void fend_name(var_str *vp, int *gnump);
	};


	/**
	 * The <code>IntFuncSrcFile</code> class is used for
	 * the changing the source cmpl file by include.
	 */
	class IntFuncSrcFile: public IntFuncBase
	{
	public:
		/**
		 * constructor
		 */
		inline IntFuncSrcFile(CmplCore *m): IntFuncBase(m) {}

		/**
		 * executes the command represented by this class.
		 * @param vp    pointer to the current command in the intermediary code
		 */
		virtual void exec(val_str *vp);
	};


	/**
	 * The <code>IntFuncSrcLine</code> class is used for
	 * the changing the line of the source cmpl file.
	 */
	class IntFuncSrcLine: public IntFuncBase
	{
	public:
		/**
		 * constructor
		 */
		inline IntFuncSrcLine(CmplCore *m): IntFuncBase(m) {}

		/**
		 * executes the command represented by this class.
		 * @param vp    pointer to the current command in the intermediary code
		 */
		virtual void exec(val_str *vp);
	};
}


#endif // !CMPL_INTFUNCCODE_HH

