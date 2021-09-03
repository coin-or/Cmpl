
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


#ifndef DATAIMPORTBASEMOD_HH
#define DATAIMPORTBASEMOD_HH


#include "../../Control/ExtensionModule.hh"


using namespace std;



namespace cmpl
{
	class DataImportBase;

	
    /**
	 * <code>DataImportBaseMod</code> is the base class for extensions
	 * used to import data for cmpl from an external data soruce.
	 */
	class DataImportBaseMod : public ExtensionModule
	{
    protected:
        int _extSrcType;            ///< type of external data source (must be unique over all used external data source extensions)
        int _handlePrio;            ///< priority of handling with this extension if more than one extension can handle it

		bool _regPrecompiler;		///< register the extension for precompiler module
		bool _regInterpreter;		///< register the extension for interpreter module

    protected:
		/**
		 * create object for execution of the extension
		 */
		virtual DataImportBase *createExecObj() = 0;

		/**
		 * get default type of external data source
		 */
		virtual int defaultSrcType() = 0;

        /**
         * initialize modul, to call immediately after the constructor.
         * @param ctrl			main object
         * @param data			data object
         * @param name			module name (or alias)
         */
        virtual void init(MainControl *ctrl, MainData *data, const char *name);

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


		/************** overwritten methods of <code>ModuleBase</code> **********/

	public:
		/**
		 * run module function
		 */
		virtual void run();


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

#endif // DATAIMPORTBASEMOD_HH

