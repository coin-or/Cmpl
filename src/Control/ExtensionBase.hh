
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


#ifndef EXTENSIONBASE_HH
#define EXTENSIONBASE_HH

#include "ModuleBase.hh"
#include "MainData.hh"


namespace cmpl
{
    /**
	 * <code>ExtensionBase</code> is the base class for all extension classes.
	 */
	class ExtensionBase : public MainData::DataBase
	{
		friend class MainControl;

	public:
		/**
		 * constructor
		 * @param mod			module creating this extension object
		 */
		ExtensionBase(ModuleBase *mod) : MainData::DataBase(mod->moduleName(), mod->data())		{ }

		/**
		 * destructor
		 */
		virtual ~ExtensionBase()	{ }

	protected:
		/**
		 * run the extension function
		 * @param mod			module calling the extension
		 * @param step			execution step within the module
		 * @param id			additional identificator
		 * @param par			additional parameter
		 */
		virtual void run(ModuleBase *mod, int step, int id, void *par) = 0;

        /**
         * run the extension function for processing a command line option
         * @param mod			module calling the extension
         * @param step			execution step within the module
         * @param id			additional identificator
         * @param ref           reference number of option registration, should be used for discriminate the options
         * @param prio          priority value of option
         * @param opt           command line option
         * @param par			additional parameter
         * @return              true if option is used by the extension
         */
        virtual bool run(ModuleBase *mod, int step, int id, int ref, int prio, CmdLineOptList::SingleOption *opt, void *par)    { return false; }

		/**
		 * called after running the module, before deleting the module object
		 * @param mod			module calling the extension
		 * @param id			additional identificator
		 */
		virtual void moduleEnd(ModuleBase *mod, int id) 	{ }

	public:
#if PROTO
		/**
		 * get name of the extension, only used for protocol output
		 */
		virtual const char *extName() = 0;
#endif //PROTO
		

		/**
		 * delete all data
		 */
		virtual void cleanUp()		{ }

		/**
		 * if sub data is splitted up to different parts for serialization then get count of these parts, otherwise 0
		 */
		virtual int subPartCount()	{ return 0; }

		/**
		 * insert strings and locations used from this in the data object
		 */
		virtual void insertStringsAndLocs(MainData *data)	{ }

		/**
		 * check if serialization to or from stream should be done for the data object
		 * @param si		serialization info
		 * @param data		main data object
		 * @return			true if serialization should be done
		 */
		virtual bool checkSerialize(MainData::SerializeInfo& si, const MainData *data)		{ return true; }
	};
}

#endif // EXTENSIONBASE_HH

