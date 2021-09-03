
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


#ifndef COMPATSYMBOLS_HH
#define COMPATSYMBOLS_HH


#include "../../Control/ExtensionBase.hh"
#include "../../CommonData/ExtensionSteps.hh"


using namespace std;



namespace cmpl
{
    /**
	 * the <code>CompatSymbols</code> class is an extension to the compiler module
	 * which defines compatibility symbols for cmpl.
	 */
	class CompatSymbols : public ExtensionBase
	{

	public:
		/**
		 * constructor
		 * @param mod			module creating this extension object
		 */
		CompatSymbols(ModuleBase *mod): ExtensionBase(mod)	{ }


		/************** overwritten methods of <code>ExtensionBase</code> **********/

	protected:
		/**
		 * run the extension function
		 * @param mod			module calling the extension
		 * @param step			execution step within the module
		 * @param id			additional identificator
		 * @param par			additional parameter
		 */
		virtual void run(ModuleBase *mod, int step, int id, void *par);

		/**
		 * called after running the module, before deleting the module object
		 * @param mod			module calling the extension
		 * @param id			additional identificator
		 */
		virtual void moduleEnd(ModuleBase *mod, int id)		{ }

	public:
#if PROTO
		/**
		 * get name of the extension, only used for protocol output
		 */
		virtual const char *extName()				{ return "compatSymbols"; }
#endif //PROTO

		/**
		 * get count of sub data objects
		 */
		virtual int subCnt()						{ return 0; }

		/**
		 * write data object to output stream
		 * @param si        serialization info
		 * @param data      main data object
		 */
		virtual void serializeTo(MainData::SerializeInfo& si, const MainData *data);

		/**
		 * fill data object from input stream
		 * @param si        serialization info
		 * @param data      main data object
		 * @param subCnt    count of sub data objects
		 * @param lc		line count per element or 0 if elements are len coded
		 * @param rline		remaining section start line
		 */
		virtual void deserializeFrom(MainData::SerializeInfo& si, const MainData *data, int subCnt, int lc, string &rline);

	};
}

#endif // COMPATSYMBOLS_HH

