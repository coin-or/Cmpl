
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


#ifndef MODULEBASE_HH
#define MODULEBASE_HH

#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <chrono>

#include "CmdLineOptList.hh"
#include "ErrorHandler.hh"


using namespace std;


// step of command line option parsing or origin of command line options, ordered by ascending priority
#define CMDLINE_OPT_PARSE_MODCONF		1		// parse command line options from module config file
#define CMDLINE_OPT_PARSE_HEADER		2		// parse command line options from cmpl header
#define CMDLINE_OPT_PARSE_START			3		// parse command line options from start of run() function



namespace cmpl
{
	class MainControl;
	class MainData;


    /**
	 * <code>ModuleBase</code> is the base class for all module classes.
	 */
	class ModuleBase
	{
		friend class MainControl;

	protected:
		const char *_modName;                   ///< module name
		MainControl *_ctrl;						///< main object of execution
		MainData *_data;						///< data object, filled inside the run method

        bool _isSilent;                         ///< suppresses output messages
        bool _deactivated;                      ///< module is deactivated      //TODO: fuer Extensions auch spaeter ausliefern, damit auch ueber cmpl-Header? (dann auch in ExtensionBase geeignet beruecksichtigen)

	private:
        vector<CmdLineOptList::RegOption> _modOptReg;	///< registered command line options for delivery to this module
        map<string, vector<int>> _extOptReg;            ///< registered extension option execution steps (key is option name, value is vector of extension execution steps)

        chrono::system_clock::time_point _startTime;    ///< time of start

#if PROTO
	protected:
		ostream *_protoStream;					///< stream for protocol / NULL: no protocol
#endif

	protected:
		/**
		 * constructor.
		 */
		ModuleBase()							{ }

		/**
		 * destructor.
		 */
		virtual ~ModuleBase()					{ }
		
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
		virtual void run() = 0;

		/**
		 * get name of the module
		 */
		virtual const char *moduleName() const		{ return _modName; }

        /**
         * get whether value access need locks because of concurrent threads
         */
        virtual bool needLocks() const              { return false; }

		/**
		 * get main object
		 */
		inline MainControl *ctrl() const			{ return _ctrl; }

		/**
		 * get data object
		 */
		inline MainData *data() const				{ return _data; }

		/**
		 * get this module
		 */
		inline ModuleBase *modp()					{ return this; }

        /**
         * get start time
         */
        chrono::system_clock::time_point startTime()    { return _startTime; }

        /**
         * get time duration since start of this module
         */
        chrono::milliseconds durStartMod()            { return chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now() - _startTime); }

        /**
         * get time duration since start of the application
         */
        chrono::milliseconds durStartApp();

		/**
		 * module will be only started if no error is occured with a level higher than this in previous modules.
		 * (most modules run only if no errors occured, but a module can overwrite that.)
		 */
		virtual int maxErrorLevelRun() const		{ return ERROR_LVL_WARN; }

	private:
		/**
		 * parse command line options for the module
		 * @param opts			command line options
		 * @param step			step of command line option parsing or origin of command line options (one of <code>CMDLINE_OPT_PARSE*</code>)
		 * @param modNames		relevant names for module specification in command line options
		 */
        void parseCmdLineOpts(CmdLineOptList *opts, int step, vector<const char *> &modNames);

        /**
         * check and parse a single command line option
         * @param opt           command line option
         * @param reg           registration of the command line option for the module
         * @param step          step of command line option parsing or origin of command line options (one of <code>CMDLINE_OPT_PARSE*</code>)
         * @param modspec       level of module specification for the option
         * @param parseOpt      call parsing function for the command line option
         * @return              true if command line option successfully checked
         */
        bool checkParseSingleCmdLineOpt(CmdLineOptList::SingleOption *opt, CmdLineOptList::RegOption *reg, int step, int modspec, bool parseOpt);

    protected:
        /**
         * register handling of command line options for a given extension execution step
         * @param extStep       extension execution step
         * @return              true if there is at least one concerned extension
         */
        bool regExtOption(int extStep);


		/*********** handling of command line options options **********/
		// module classes must overwrite this methods

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
         * execute step for every registered extension for a single command line option
         * @param extStep       extension execution step
         * @param extRef        map from extension identifier to command line option reference key
         * @param prio          priority value of option
         * @param opt           option
         * @return              true if option is used by an extension
         */
        virtual bool execExtOption(int extStep, map<int, int>& extRef, int prio, CmdLineOptList::SingleOption *opt)       { return false; }

#if PROTO
		/**
		 * parse protocol file or trace file option
		 * @param opt			option
		 * @param err			name for error message
		 * @return				output stream or NULL
		 */
		ostream* parseProtoOption(CmdLineOptList::SingleOption *opt, const char *err);
#endif //PROTO

		/**
		 * writes usage info for the module to stream
		 * @param s				stream to write to
		 */
		virtual void usage(ostream& s);

		
		/*********** utility functions for options **********/

	protected:
		/**
		 * parse command line option with file name aliases
		 */
		void parseFileAliases(CmdLineOptList::SingleOption *opt, map<string,string>& fileAlias, map<string,unsigned>& fileAliasPrio);

        /**
         * execute step for every registered extension for a single command line option, for options simply to deliver to all concerned extensions
         * @param extStep       extension execution step
         * @param extRef        map from extension identifier to command line option reference key
         * @param prio          priority value of option
         * @param opt           option
         * @return              true if option is used by an extension
         */
        bool execExtOptionSimple(int extStep, map<int, int>& extRef, int prio, CmdLineOptList::SingleOption *opt);



		/*********** other functions **********/

	public:
        /**
         * writes a Cmpl message
         * @param s         an outstream
         * @param str       the message
         */
        inline void CmplOutput(ostream& s, string str) { if (!_isSilent) s << "CMPL: " << str << endl;}


		/**
		 * flag wether to write a protocol file
		 */
		inline bool isProtocol() {
#if PROTO
			return (_protoStream != NULL);
#else
			return false;
#endif //PROTO
		}

		/**
		 * get stream for protocol output
		 */
		inline ostream *protoStream() {
#if PROTO
			return _protoStream;
#else
			return NULL;
#endif //PROTO
		}
    };
}



/*********** defines for module definitions **********/
// declaration of module construct function, for use in the following two makros. n is the module name
#define MODULE_CLASS_DECL(n)        ModuleBase *constructModule_ ## n(MainControl *ctrl, const char *name)

// register a module, only for use in file "modules.reg". n is the module name, o the order number, d a short module description
#define MODULE_CLASS_REG(o,n,d)     { MODULE_CLASS_DECL(n); const char *__s = _modNames.store(#n); _modules[__s] = (ModOrderFunc){ o, constructModule_ ## n }; _modDescr.push_back(ModDescr(__s, false, d)); }

// define a module, for use in the .cc file of the module. n is the module name, C is the name of the module class
// (if another function is needed for constructing an instance of the module class, then use: MODULE_CLASS_DECL(n) { /* individual function body */ })
#define MODULE_CLASS(n,C)           MODULE_CLASS_DECL(n) { ModuleBase *m = new C(); m->init(ctrl, ctrl->data(), name); return m; }



/*********** defines for command line options **********/
// register an option, for use in regModOptions()
#define REG_CMDL_OPTION(r,o,mi,ma,n,p)			modOptReg.push_back((CmdLineOptList::RegOption) { r, o, mi, ma, n, p })

// values for handling of negated options
#define CMDL_OPTION_NEG_IGNORE			0			// don't deliver negated option
#define CMDL_OPTION_NEG_ERROR			-1			// negated option is an error
#define CMDL_OPTION_NEG_DELIV			1			// deliver negated option with check of minArgs and maxArgs
#define CMDL_OPTION_NEG_NO_ARG			2			// deliver negated option but it cannot have any arg

// construct and deconstruct priority value
#define CMDL_OPTION_MAKE_PRIORITY(p,s,m)		(((p) << 16) + ((s) << 12) + (m))		// priority value from explicit priority p, step s and module specification m
#define CMDL_OPTION_PRIORITY_EXPL(v)			(((v) & 0xff0000) >> 16)				// separate explicit priority from priority value
#define CMDL_OPTION_PRIORITY_STEP(v)			(((v) & 0x00f000) >> 12)				// separate step from priority value
#define CMDL_OPTION_PRIORITY_MODSPEC(v)			((v) & 0x000fff)						// separate module specification from priority value


#endif // MODULEBASE_HH

