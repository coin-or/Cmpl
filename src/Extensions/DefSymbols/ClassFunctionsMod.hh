
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


#ifndef CLASSFUNCTIONSMOD_HH
#define CLASSFUNCTIONSMOD_HH


#include "../../Control/ExtensionModule.hh"


using namespace std;



namespace cmpl
{
    /**
	 * <code>ClassFunctionsMod</code> is the extension module class for extension <code>ClassFunctions</code>
	 * which defines a container symbol "class" and functions within it.
	 */
	class ClassFunctionsMod : public ExtensionModule
	{

        /************** command line options **********/

    private:
        vector<unsigned> _namespace;				///< namespace parts of the namespace for the functions


		/************** overwritten methods of <code>ModuleBase</code> **********/

	public:
        /**
         * initialize modul, to call immediately after the constructor.
         * @param ctrl			main object
         * @param data			data object
         * @param name			module name (or alias)
         */
        virtual void init(MainControl *ctrl, MainData *data, const char *name);

        /**
		 * run module function
		 */
		virtual void run();

    protected:
        /**
         * register command line options options for delivery to this module
         * @param modOptReg		vector to register in
         */
        virtual void regModOptions(vector<CmdLineOptList::RegOption> &modOptReg);

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


		/************** overwritten methods of <code>ExtensionModule</code> **********/

	public:
		/**
		 * create the extension object from serialization
		 * @param si        serialization info
		 * @param data      main data object
		 * @param subCnt    count of sub data objects
		 * @param lc		line count per element or 0 if elements are len coded
		 * @param rline		remaining section start line
		 */
		virtual ExtensionBase *createFromSerialization(MainData::SerializeInfo& si, const MainData *data, int subCnt, int lc, string &rline);
	};
}

#endif // CLASSFUNCTIONSMOD_HH

