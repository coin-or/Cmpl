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
 * here for functions for input and output.
 */

#ifndef CMPL_INTFUNCENV_HH
#define CMPL_INTFUNCENV_HH

#include "IntFuncBase.hh"


namespace cmpl
{
	/**
	 * The <code>IntFuncReadCSV</code> class is used for reading
	 * a one or two dimensional array of numeric values from a csv file.
	 */
	class IntFuncReadCSV: public IntFuncBase
	{
	public:
		/**
		 * constructor
		 */
		inline IntFuncReadCSV(Main *m): IntFuncBase(m) {}

		/**
		 * executes the command represented by this class.
		 * @param vp	pointer to the current command in the intermediary code
		 */
		virtual void exec(val_str *vp);
	};


	/**
	 * The <code>IntFuncReadStdin</code> class is used for reading
	 * a single value from stdin.
	 */
	class IntFuncReadStdin: public IntFuncBase
	{
	public:
		/**
		 * constructor
		 */
		inline IntFuncReadStdin(Main *m): IntFuncBase(m) {}

		/**
		 * executes the command represented by this class.
		 * @param vp	pointer to the current command in the intermediary code
		 */
		virtual void exec(val_str *vp);
	};


	/**
	 * The <code>IntFuncOutputMsg</code> class is used for output
	 * a user generated message or error message.
	 */
	class IntFuncOutputMsg: public IntFuncBase
	{
	public:
		/**
		 * constructor
		 */
		inline IntFuncOutputMsg(Main *m): IntFuncBase(m) {}

		/**
		 * executes the command represented by this class.
		 * @param vp	pointer to the current command in the intermediary code
		 */
		virtual void exec(val_str *vp);
	};
}


#endif // !CMPL_INTFUNCENV_HH

