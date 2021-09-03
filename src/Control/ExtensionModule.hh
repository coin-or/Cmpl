
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


#ifndef EXTENSIONMODULE_HH
#define EXTENSIONMODULE_HH

#include <string>
#include <vector>

#include "ModuleBase.hh"


namespace cmpl
{
	class ExtensionBase;

    /**
	 * <code>ExtensionBase</code> is the base class for all extension classes.
	 */
	class ExtensionModule: public ModuleBase
	{
	protected:
		vector<string> _registerMods;		///< names of modules to register this extension for

	protected:
		/**
		 * register command line options options for delivery to this module
		 * @param modOptReg		vector to register in
		 */
		virtual void regModOptions(vector<CmdLineOptList::RegOption> &modOptReg);

        /**
         * register command line options for the extension
         * @param ext           extension object
         * @param id			extension identificator
         */
        virtual void regExtOptions(ExtensionBase *ext, int id)      { }

		/**
		 * parse single option from command line options, this function is called for every delivered option
		 * @param ref			reference number of option registration, should be used for discriminate the options
		 * @param prio			priority value of option
		 * @param opt			option
		 * @return				true if option is used by the module
		 */
		virtual bool parseOption(int ref, int prio, CmdLineOptList::SingleOption *opt);

		/**
		 * writes usage info for the module to stream
		 * @param s				stream to write to
		 */
		virtual void usage(ostream& s);

	public:
		/**
		 * create the extension object from serialization
		 * @param si        serialization info
		 * @param data      main data object
		 * @param subCnt    count of sub data objects
		 * @param lc        line count per element or 0 if elements are len coded
		 * @param rline     remaining section start line
		 */
		virtual ExtensionBase *createFromSerialization(MainData::SerializeInfo& si, const MainData *data, int subCnt, int lc, string &rline) = 0;
	};
}


/*********** defines for command line options **********/
// register an option, for use in regModOptions()
#define REG_CMDL_OPTION_EXT(r,o,mi,ma,n,p,id,mod,stp,ext)			ctrl()->regCmdOptExtension(mod, stp, ext, r, o, mi, ma, n, p, id, moduleName())


#endif // EXTENSIONMODULE_HH

