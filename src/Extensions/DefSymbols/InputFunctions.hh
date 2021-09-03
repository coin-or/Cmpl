
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


#ifndef INPUTFUNCTIONS_HH
#define INPUTFUNCTIONS_HH


#include "../../Control/ExtensionBase.hh"
#include "../../CommonData/ExtensionSteps.hh"
#include "../../Modules/Interpreter/ValFunction.hh"


using namespace std;



namespace cmpl
{
	class InputFunctionsMod;

    /**
	 * the <code>InputFunctions</code> class is an extension to the compiler module
	 * which defines math functions for cmpl.
	 */
	class InputFunctions : public ExtensionBase
	{
		friend class InputFunctionsMod;

	private:
        vector<unsigned> _namespace;		///< namespace parts of the namespace for the functions
        int _baseId;						///< first registration id


	public:
		/**
		 * constructor
		 * @param mod			module creating this extension object
		 */
        InputFunctions(ModuleBase *mod): ExtensionBase(mod), _namespace()	{ }

        /**
         * constructor
         * @param mod			module creating this extension object
         * @param ns            namespace parts of the namespace for the functions
         */
        InputFunctions(ModuleBase *mod, const vector<unsigned>& ns): ExtensionBase(mod), _namespace(ns)	{ }


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
		virtual const char *extName()				{ return "inputFunctions"; }
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


		/************** extension registration **********/

	private:
		/**
		 * return count of functions to register
		 */
		unsigned countFunc();

	};



	/************** input functions **********/

	/**
	 * base class for internal cmpl functions for input of values
	 */
	class InputFunctionBase : public ValFunctionInternal
	{
	};


	/**
	 * function for reading a value from stdin
	 */
	class InputFunctionReadStdin : public InputFunctionBase
	{
	protected:
        virtual const char *funcName() const override			{ return "readstdin"; }		///< name of the input function
			 
	public:
		/**
		 * calls the cmpl function call operation and store result in execution context, if this value has a special handling for it
		 * @param ctx			execution context
		 * @param arg			pointer to argument value
		 * @return				true
		 */
        virtual bool operCall(ExecContext *ctx, StackValue *arg) override;
	};


	/**
	 * function for reading values from a csv file
	 */
	class InputFunctionReadCSV : public InputFunctionBase
	{
	protected:
        virtual const char *funcName() const override			{ return "readcsv"; }		///< name of the input function
			 
	public:
		/**
		 * calls the cmpl function call operation and store result in execution context, if this value has a special handling for it
		 * @param ctx			execution context
		 * @param arg			pointer to argument value
		 * @return				true
		 */
        virtual bool operCall(ExecContext *ctx, StackValue *arg) override;
	};

}

#endif // INPUTFUNCTIONS_HH

