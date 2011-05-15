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
 * here for functions for the control structure of cmpl.
 */

#ifndef CMPL_INTFUNCCTRL_HH
#define CMPL_INTFUNCCTRL_HH

#include "IntFuncBase.hh"


namespace cmpl
{
	/**
	 * The <code>IntFuncLoopCtrl</code> class is used for
	 * start or end of a control structure.
	 */
	class IntFuncLoopCtrl: public IntFuncBase
	{
	public:
		/**
		 * constructor
		 */
		inline IntFuncLoopCtrl(Main *m): IntFuncBase(m) {}

		/**
		 * executes the command represented by this class.
		 * @param vp    pointer to the current command in the intermediary code
		 */
		virtual void exec(val_str *vp);
	};


	/**
	 * The <code>IntFuncProgEnd</code> class is used for
	 * iterating a control structure or testing a condition in a control structure.
	 */
	class IntFuncTestIter: public IntFuncBase
	{
	public:
		/**
		 * constructor
		 */
		inline IntFuncTestIter(Main *m): IntFuncBase(m) {}

		/**
		 * executes the command represented by this class.
		 * @param vp    pointer to the current command in the intermediary code
		 */
		virtual void exec(val_str *vp);
	};


	/**
	 * The <code>IntFuncProgEnd</code> class is used for
	 * testing an alternative condition in a control structure.
	 */
	class IntFuncTestAlter: public IntFuncBase
	{
	public:
		/**
		 * constructor
		 */
		inline IntFuncTestAlter(Main *m): IntFuncBase(m) {}

		/**
		 * executes the command represented by this class.
		 * @param vp    pointer to the current command in the intermediary code
		 */
		virtual void exec(val_str *vp);
	};


	/**
	 * The <code>IntFuncProgEnd</code> class is used for
	 * changing the current position in the intermediary code.
	 */
	class IntFuncGoto: public IntFuncBase
	{
	public:
		/**
		 * constructor
		 */
		inline IntFuncGoto(Main *m): IntFuncBase(m) {}

		/**
		 * executes the command represented by this class.
		 * @param vp    pointer to the current command in the intermediary code
		 */
		virtual void exec(val_str *vp);
	};
}


#endif // !CMPL_INTFUNCCTRL_HH

