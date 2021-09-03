
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


#ifndef START_HH
#define START_HH

namespace cmpl
{
	class MainControl;
	class MainData;
	class CmdLineOptList;

	/**
	 * <code>Start</code> contains static functions to start execution of cmpl if used as standalone program
	 */
	class Start
	{
	public:
		/**
		 * gets a new <code>CmdLineOptList</code> object
		 */
		static CmdLineOptList *getNewEmptyCmdLineOptList();

		/**
		 * free an <code>CmdLineOptList</code> object
		 * @param args		CmdLineOptList to free, pointer is after that no longer usable
		 */
		static void dispose(CmdLineOptList *args);


		/**
		 * gets a new <code>MainData</code> object
		 */
		static MainData *getNewEmptyMainData();

		/**
		 * free an <code>MainData</code> object
		 * @param args		MainData to free, pointer is after that no longer usable
		 */
		static void dispose(MainData *data);


		/**
		 * run cmpl
		 * @param opts      	command line options
		 * @param data          object for input data and output data
		 * @return              exit code
		 */
		static int run(CmdLineOptList *opts, MainData *data);
	};
}
#endif // START_HH

