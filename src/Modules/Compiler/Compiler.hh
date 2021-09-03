
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


#ifndef COMPILER_HH
#define COMPILER_HH


#include "../../Control/ModuleBase.hh"
#include "../../Control/FileBase.hh"


using namespace std;



namespace cmpl
{
    /**
	 * the <code>Compiler</code> class starts the parser
	 */
	class Compiler : public ModuleBase
	{

		/************** command line options **********/

	private:
		FileInput *_inputFile;					///< input file, only used if input not is taken from _data, else NULL
		FileInput *_externFile;					///< extern data info file, only used if input not is taken from _data, else NULL

		FileOutput *_syntaxXmlFile;				///< output file for syntax structure output as XML
		bool _checkOnly;							///< only syntax check, no generating of intermediary code

		bool _allowNewPublic;						///< allow new-definition when it permanently discards the previous symbol
		bool _allowNewConst;						///< allow new-definition when the previous symbol is const-defined

		bool _compatLineNamePref;					///< allow undefined name as line name prefix for compatibility
		bool _compatCBSymbols;						///< allow undefined name as code block symbol for compatibility
		bool _compatCBHAssign;						///< allow assign operator in code block header instead of equal compare operator

		bool _warnCBInsertEOS;						///< output warning if an end-of-statement token is inserted after a codeblock

        short _intOutOfRangeMode;					///< mode for handling literal integer values out of integer range (one of INT_OPER_OORANGE_*)

#if PROTO
		bool _traceScanning;						///< write scanner trace output to stderr
		bool _traceParsing;							///< write parser trace output to stderr
        FileOutput *_outputCode;					///< output file for protocol output of generated intermediary code
#endif


    public:
		/**
		 * destructor
		 */
		~Compiler();

#if PROTO
		/**
		 * get whether write scanner trace output to stderr
		 */
		bool traceScanning()			{ return _traceScanning; }

		/**
		 * get whether write parser trace output to stderr
		 */
		bool traceParsing()				{ return _traceParsing; }
#endif

		/**
		 * get whether to make only syntax check, no generating of intermediary code
		 */
		bool checkOnly()				{ return _checkOnly; }

		/**
		 * get whether to allow new-definition when it permanently discards the previous symbol
		 */
		bool allowNewPublic()			{ return _allowNewPublic; }

		/**
		 * get whether to allow new-definition when the previous symbol is const-defined
		 */
		bool allowNewConst()			{ return _allowNewConst; }

		/**
		 * get whether to allow undefined name as line name prefix for compatibility
		 */
		bool compatLineNamePref()		{ return _compatLineNamePref; }

		/**
		 * get whether to allow undefined name as code block symbol for compatibility
		 */
		bool compatCBSymbols()			{ return _compatCBSymbols; }

		/**
		 * get whether to allow assign operator in code block header for compatibility
		 */
		bool compatCBHAssign()			{ return _compatCBHAssign; }

		/**
		 * get whether to output warning if an end-of-statement token is inserted after a codeblock
		 */
		bool warnCBInsertEOS()			{ return _warnCBInsertEOS; }

        /**
         * get mode for handling literal integer values out of integer range (one of INT_OPER_OORANGE_*)
         */
        short intOutOfRangeMode()		{ return _intOutOfRangeMode; }


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



		/****** error output ****/

	public:
		/**
		 * output error message and possible original exception; disable code generation if error.
		 * @param level			one of ERROR_LVL_*
		 * @param msg			error message
		 * @param loc			error location
		 * @param orgExc		original exception or NULL
		 * @param fatal			flag whether to throw FatalException / -1: according to level
		 */
        void error(int level, const char *msg, const LocationInfo &loc, exception *orgExc = NULL, int fatal = -1);

		/**
		 * output error message and possible original exception; disable code generation if error.
		 * @param level			one of ERROR_LVL_*
		 * @param msg			error message
		 * @param pos			error position
		 * @param orgExc		original exception or NULL
		 * @param fatal			flag whether to throw FatalException / -1: according to level
		 */
        void error(int level, const char *msg, const PositionInfo &pos, exception *orgExc = NULL, int fatal = -1);

		/**
		 * output two joined error messages; disable code generation if error.
		 * @param level			one of ERROR_LVL_*
		 * @param msg1			error message #1
		 * @param loc1			error location #1
		 * @param msg2			error message #2
		 * @param loc2			error location #2
		 */
		void error(int level, const char *msg1, LocationInfo& loc1, const char *msg2, LocationInfo& loc2);

		/**
		 * output internal fatal error without location information; disable code generation if error.
		 * @param msg			error message
		 * @param orgExc		original exception or NULL
		 */
		void internalError(const char *msg, exception *orgExc = NULL);



		/************** input and output of data **********/

	private:
		/**
		 * read input file to data object
		 * @param file			file to read
		 */
		void readInputFile(FileInput& file);

		/**
		 * read extern data info file to data object
		 * @param file			file to read
		 */
		void readExternFile(FileInput& file);

		/**
		 * write syntax structure info file as xml
		 * @param file			file to write
		 */
		void writeSyntaxXmlFile(FileOutput& file);

		/**
		 * protocol output of generated intermediary code
		 * @param file			file to write
		 */
		void writeCodeProtocol(FileOutput& file);
	};
}

#endif // COMPILER_HH

