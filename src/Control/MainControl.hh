
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


#ifndef MAINCONTROL_HH
#define MAINCONTROL_HH

#define CMPL_VERSION		"2.0.0"

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <stdarg.h>
#include <limits.h>

#include "CmdLineOptList.hh"
#include "ErrorHandler.hh"
#include "ThreadHandler.hh"
#include "FileBase.hh"
#include "ModuleBase.hh"
#include "ExtensionBase.hh"
#include "StringStore.hh"
#include "MainData.hh"


using namespace std;


namespace cmpl
{
	class MainData;
	class ModulesConf;


	/**
	 * <code>MainControl</code> controls the execution of the cmpl modules.
	 */
	class MainControl : public ModuleBase
	{
	private:
		friend class ModulesConf;
		friend class ModuleBase;
		friend class CmdLineOptList;

		/**
		 * struct for a module to run
		 */
		struct Module {
			const char *name;						///< module name (allocated in ModulesConf)
			vector<const char *> modNames;			///< names for module specification in command line options (strings allocated in ModulesConf)
			const char *alias;						///< module alias (allocated in ModulesConf) / NULL: no alias
			int order;								///< order number for the module
			bool executed;							///< module was already executed

			// constructor
			Module(const char *n, const char *a, int o, vector<const char *> *modN)
																{ name = n; alias = a; order = o; executed = false;
																  if (modN) { modNames = *modN; } modNames.push_back(a ?: n); }
		};

		CmdLineOptList *_startOpts;					///< command line options given in the run() function
		CmdLineOptList _modConfOpts;				///< command line options from module config file
		CmdLineOptList _headerOpts;					///< command line options from cmpl header

		StringStore _cmdLineOptNames;				///< string store for option names from command line options
		map<const char *, map<const char *, int>> _optUsed;	///< using of option from command line options: first key is option name, second key is module name, value is the highest priority the option is used with

		vector<const char *> _modNames;				///< names for this module specified in command line options
		set<const char *> _modNameSet;				///< names of inserted modules
		set<const char *> _modAggrSet;				///< names of inserted module aggregations

		string _modConfigFile;						///< filename of module config file
		LocationInfo _modConfLoc;					///< location of filename for module config file
		ModulesConf *_modConf;						///< config of module names (from _startOpts)

		map<int, vector<Module *>> _modules;		///< lists of modules per order number
		map<int, vector<Module *>>::iterator _itO;	///< iterator for current element in _modules
		unsigned _indModule;						///< current index of element of current list in _modules

		map<ModuleBase*, Module*> _runningMod;		///< current running modules (only one module at time)

		string _instanceName;						///< name of this cmpl instance

		FileOutput _errorFile;						///< output error file
        ErrorHandler _errHandler;					///< error handler

        FileOutput _cmplMsgFile;					///< cmplMsg error file

        FileOutput *_optOutFile;                    ///< file for output of command line options
        bool _optWarnUnused;                        ///< output warning if a command line option ist not used
        set<string> _optMarkUsed;                   ///< mark this command line options as used

        bool _isSilentStart;                        ///< suppresses output of start message

#if PROTO
		StringStore _protoFileNames;				///< string store for names of protocol files
		map<int, FileOutput *> _protoFiles;			///< protocol output files per number of filename in _protoFileNames
#endif //PROTO

		bool _printVersion;							///< print version and exit
		bool _printUsage;							///< print usage and exit
        bool _usageAll;                             ///< print usage for MainControl and all modules
        string _usageModule;						///< print usage for this module / empty: for MainControl (only used if !_usageAll)


		/****** main execution ****/

	public:
		/**
		 * initialize modul, to call immediately after the constructor.
         * @param ctrl          main object
		 * @param data			data object
		 * @param name          module name (or alias)
		 */
		virtual void init(MainControl *ctrl, MainData *data, const char *name);

		/**
		 * destructor
		 */
		~MainControl();

		/**
		 * run execution of modules
		 * @param opts			command line options
		 * @return				exit code
		 */
		int run(CmdLineOptList *opts);

		/**
		 * get name of the module
		 */
		virtual const char *moduleName() const			{ return "_MainControl_"; }



		/****** virtual functions from ModuleBase ****/

	private:
		/**
		 * run module function
		 * @param data          object for input data and output data
		 */
		virtual void run();

		/**
		 * register command line options for delivery to this module
		 * @param modOptReg		vector to register in
		 */
		virtual void regModOptions(vector<CmdLineOptList::RegOption> &modOptReg);

        /**
         * get names of command line options which are not to write to a command line option file
         * @return              set with names of these command line options
         */
        set<string> noWriteOptions();

		/**
		 * parse single option from command line options, this function is called for every delivered option
		 * @param ref           reference number of option registration, should be used for discriminate the options
		 * @param prio          priority value of option
		 * @param opt           option
		 * @return              true if option is used by the module
		 */
		virtual bool parseOption(int ref, int prio, CmdLineOptList::SingleOption *opt);

		/**
		 * parse command line options for the module
		 * @param opts          command line options
		 * @param step          parse step (one of <code>CALLARG_PARSE_*</code>)
		 * @param modNames      relevant names for module specification in command line options
		 */
		virtual void parseCmdLineOpts(CmdLineOptList *opts, int step, vector<const char *> &modNames);

		/**
		 * writes usage info for the module to stream
		 * @param s             stream to write to
		 */
		virtual void usage(ostream& s);

        /**
         * writes usage info for a given module to stream
         * @param s             stream to write to
         * @param modConf       object for construction and configuration of module
         * @param mod           module name
         */
        void usageModule(ostream& s, ModulesConf& modConf, const char *mod);

		/**
		 * writes version info to stream
		 * @param s             stream to write to
		 */
		void version(ostream& s);



		/****** functions to run a module from another module ****/

	public:
		/**
		 * run directly another module with special options.
		 * @param modName		name of module to start
		 * @param opts			pseudo command line options to deliver the module arbitrary parameters (the module doesn't get the normal command line options)
		 * (if data is to deliver to or from the called module then use the global data object.)
		 */
		void runModuleFunc(const char *modName, CmdLineOptList *opts);

		/**
		 * add another module for execution, if it not contained already
		 * @param modName		module name
		 * @param order			order number for execution
		 * @param opts			additional command line options, should be restricted by name to the module / NULL: no additional options
		 * @param alias			module alias / NULL: no alias
		 * @param modNames      vector with module names for use of command line options / NULL: use only module name
		 * @return				module new inserted
		 */
		bool addModule(const char *modName, int order, CmdLineOptList *opts = NULL, const char *alias = NULL, vector<const char *> *modNames = NULL);


		/****** functions for module information and handling ****/

	public:
		/**
		 * get current running modules (only one module at time)
		 */
		map<ModuleBase*, Module*>& runningModules()				{ return _runningMod; }

		/**
		 * get order number for a running module
		 * @param mod			running module
		 * @return				order number / -1: no module
		 */
		int getOrderForRunningModule(ModuleBase *mod)			{ if (_runningMod.count(mod) > 0) return _runningMod[mod]->order; else return -1; }

		/**
		 * delete all modules still not executed within a range of order numbers
		 * @param orderFrom		start order number for range
		 * @param orderTo		end order number for range
		 * @return				count of deleted modules
		 */
		int delModules(int orderFrom, int orderTo);

		/**
		 * insert used strings for modules in the data object
		 */
		void insertModuleStrings(MainData *data);

		/**
		 * get count of elements for serialization of the modules
		 */
		inline int subCntModules()									{ return 2; }

		/**
		 * write data for serialization of the modules to output stream
		 * @param si		serialization info
		 * @param data      main data object
		 */
		void serializeModulesTo(MainData::SerializeInfo& si, const MainData *data);

		/**
		 * read modules from serialization stream
		 * @param si		serialization info
		 * @param data      main data object
		 * @param cnt		count of elements within the section
		 */
		void deserializeModulesFrom(MainData::SerializeInfo& si, const MainData *data, int cnt);

		/**
		 * get name of this cmpl instance
		 */
		inline const char *instanceName()							{ return _instanceName.c_str(); }

        /**
         * get full name of executed binary / NULL when not set
         */
        inline string* binFullName()                                { return (_startOpts && !(_startOpts->binFullName().empty()) ? &(_startOpts->binFullName()) : NULL); }


		/****** functions for error handling and protocol output ****/

	public:
		/**
		 * get error handler
		 */
		inline ErrorHandler& errHandler()      						{ return _errHandler; }

#if PROTO
		/**
		 * gets the output stream for a protocol output
		 * @param temp          temporary output file object
		 * @return              output stream
		 */
		ostream *initProto(FileOutput& temp);
#endif //PROTO


		/****** functions for formatted output ****/

	private:
		char *_printBuffer;					///< global buffer for formatted output
		int _printBufferCap;				///< capacity of buffer _printBuffer

	public:
		/**
		 * prints with vsnprintf to the global print buffer
		 */
		const char *printBuffer(const char *format, ...);



		/****** functions for command line options ****/

	private:
		/**
		 * gets the priority of an used option used for a module
		 * @param n				module name
		 * @param o				option name
		 * @return				used with this priority / -1: not used yet
		 */
		int getOptionPrio(const char *n, const char *o);

		/**
		 * sets an option as used by a module with a priority
		 * @param n				module name
		 * @param o				option name
		 * @param prio			used with this priority
		 */
		void setOptionPrio(const char *n, const char *o, int prio);

		/**
		 * parse special command line options only for MainControl
		 * @param opts          command line options
		 * @param vh            search for options for print version or usage
         * @param io            search for option for command line option file
		 * @param fbn           search for option for file base name
		 * @param prt			search for option for protocol output
		 * @param mcf           search for option for module config file
		 * @param mod           search for options for module names
		 */
        void parseControlOpts(CmdLineOptList *opts, bool vh, bool io, bool fbn, bool prt, bool mcf, bool mod);

        /**
         * read command line options from file
         * @param fn            file name
         * @param pos           position info for file name
         * @param nloc          location info of option with file name
         */
        void readFromOptFile(string& fn, PositionInfo& npos, LocationInfo& nloc);

	public:
		/**
		 * append command line options
		 * @param opts			command line options
		 * @param step			origin of command line options (one of <code>CMDLINE_OPT_PARSE*</code>)
		 * @param curMod		use this command line options also for current running module 
		 * @param ctrlMod		use this command line options also for main control, especially for modules names
		 */
		void appendCmdLineOpts(CmdLineOptList *opts, int step, bool curMod, bool ctrlMod);

		/**
		 * insert used strings and locations for command line options in the data object
		 */
		void insertCmdLineOptsStringsAndLocs(MainData *data);

		/**
		 * get count of elements for serialization of the command line options
		 */
		inline int subCntCmdLineOpts()							{ return 3; }

		/**
		 * write data for serialization of the command line options to output stream
		 * @param si		serialization info
		 * @param data      main data object
		 */
		void serializeCmdLineOptsTo(MainData::SerializeInfo& si, const MainData *data);

		/**
		 * read command line options from serialization stream
		 * @param si		serialization info
		 * @param data      main data object
		 * @param cnt		count of elements within the section
		 */
		void deserializeCmdLineOptsFrom(MainData::SerializeInfo& si, const MainData *data, int cnt);

    private:
        /**
         * write command line options to file
         * @param file      file to write
         */
        void writeOptFile(FileOutput& file);


		/****** functions for extensions ****/

	private:
        map<int, map<int, ExtensionBase *>> _regExtensionsA;                    ///< registered extension objects not for a certain module: first key is execution step, second key is identificator
        map<const char *, map<int, map<int, ExtensionBase *>>> _regExtensionsB;	///< registered extension objects for a certain module: first key is module name, second key is execution step, third key is identificator
		int _nextExtensionId;													///< next usable extension identificator

        map<int, vector<CmdLineOptList::RegOptionExt>> _regExtOptions;          ///< registered command line options for delivery to extensions: key is execution step


		/**
		 * run all registered extensions for a module after running the module
		 * @param mod			module calling the extension
		 */
		void moduleEndExtensions(ModuleBase *mod);

		/**
		 * delete extension objects in _regExtensions
		 */
		void deleteExtensions();

		/**
		 * count the registrations for an extension object
		 * @param ext			searched extension
		 * @return				count of registrations
		 */
		unsigned countExtension(ExtensionBase *ext);
		
        /**
         * get registered extension object
         * @param mod			name of module calling the extension
         * @param step			execution step within the module
         * @param id			identificator
         * @return              extension object or NULL if no one is registered
         */
        ExtensionBase *getExtension(const char *n, int step, int id);

	public:
		/**
		 * run all registered extensions for a module and execution step
		 * @param mod			module calling the extension
		 * @param step			execution step within the module
		 * @param par			additional parameter
		 * @return				count of executed extensions
		 */
		unsigned runExtension(ModuleBase *mod, int step, void *par = NULL);

		/**
		 * run registered extension for a given identificator
		 * @param mod			module calling the extension
		 * @param step			execution step within the module
		 * @param id			identificator
		 * @param par			additional parameter
		 * @return				count of executed extensions
		 */
		unsigned runExtension(ModuleBase *mod, int step, int id, void *par = NULL);

        /**
         * run registered extension for handling of a command line option
         * @param mod			module calling the extension
         * @param step			execution step within the module
         * @param id			identificator
         * @param ref			reference number of option registration, should be used for discriminate the options
         * @param prio			priority value of option
         * @param opt			option
         * @param par			additional parameter
         * @return				true if option is used by the extension
         */
        bool runExtension(ModuleBase *mod, int step, int id, int ref, int prio, CmdLineOptList::SingleOption *opt, void *par = NULL);

        /**
         * register command line option for extension
         * @param n				module name
         * @param step          execution step within the module
         * @param ext			extension object
         * @param ref           reference number for command line option
         * @param opt           option
         * @param mi            minimal count of args for the option
         * @param ma            maximal count of args for the option / -1: unbound
         * @param neg           negated option: 0: don't deliver; -1: throw error; 1: deliver with check of minArgs and maxArgs; 2: deliver but cannot have any arg
         * @param hp            deliver option only if priority is higher / false: deliver independent of priority
         * @param id            identificator of the extension
         * @param mod           extension module name
         */
        void regCmdOptExtension(const char *n, int step, ExtensionBase *ext, int ref, const char *opt, int mi, int ma, int neg, bool hp, int id, const char *mod);

        /**
         * get registration of command line option for an extension step
         * @param step          execution step within the module
         * @return              registration for the extension step
         */
        vector<CmdLineOptList::RegOptionExt> *getRegCmdOptExtension(int step);

		/**
		 * register extension using a new unique identificator
		 * @param n				module name
		 * @param step			execution step within the module
		 * @param ext			extension object
		 * @param cnt			count of needed identificators
		 * @param regAll		register for all identificators or only for the first
		 * @return				identificator
		 */
		int registerExtension(const char *n, int step, ExtensionBase *ext, unsigned cnt = 1, bool regAll = false);

		/**
		 * register extension using a given identificator
		 * @param n				module name
		 * @param step			execution step within the module
		 * @param id			identificator
		 * @param ext			extension object
		 * @param cnt			if greater 1 then register extension with this count of identificators
		 * @param repl			replace existing extension registration
		 * @return				identificator / != id if registration exists and no replacing
		 */
		int registerExtension(const char *n, int step, int id, ExtensionBase *ext, unsigned cnt = 1, bool repl = true);

        /**
         * get map with registered extension objects not for a certain module
         */
        inline map<int, map<int, ExtensionBase *>>& regExtensionsA()                            { return _regExtensionsA; }

		/**
         * get map with registered extension objects for a certain module
		 */
        inline map<const char *, map<int, map<int, ExtensionBase *>>>& regExtensionsB()			{ return _regExtensionsB; }

		/**
		 * insert used strings and locations for extensions in the data object
		 */
		void insertExtensionStringsAndLocs(MainData *data);

		/**
		 * get count of elements for serialization of the extensions
		 */
		int subCntExtensions();

		/**
		 * write data for serialization of the extensions to output stream
		 * @param si		serialization info
		 * @param data      main data object
		 */
		void serializeExtensionsTo(MainData::SerializeInfo& si, const MainData *data);

		/**
		 * read extensions serialization stream
		 * @param si		serialization info
		 * @param data      main data object
		 * @param cnt		count of elements within the section
		 */
		void deserializeExtensionsFrom(MainData::SerializeInfo& si, const MainData *data, int cnt);


		/****** internal functions ****/

	private:
		/**
		 * get default name of module config file
		 */
		const char *modConfigFileDefault()			{ return ".modules"; }

		/**
		 * add a module to <code>_modules</code>, if it not contained already
		 * @param n				module name
		 * @param a				module alias / NULL: no alias
		 * @param o				order number
		 * @param opts			command line options from module configuration
		 * @param modNames      vector with rekursive module/module aggregation names / NULL: use only module name
		 * @return				module new inserted
		 */
		bool addModule(const char *n, const char *a, int o, CmdLineOptList& opts, vector<const char *> *modNames);
	
		/**
		 * add a module aggregation, if it not contained already
		 * @param n				module aggregation name
		 * @param opts			command line options from module configuration
		 * @param modNames      vector with rekursive module/module aggregation names / NULL: use new vector
		 * @return				module aggregation new inserted
		 */
		bool addModuleAggr(const char *n, CmdLineOptList& opts, vector<const char *> *modNames);

		/**
		 * output a warning about an unused command line option
		 * @param opt			unused command line option
		 */
		void outWarnUnusedCmdLineOpt(CmdLineOptList::SingleOption *opt);
	};
}



/****** macro definition for error handling ****/
#define ERRHANDLER              modp()->ctrl()->errHandler()

/****** macro definitions for output to the protocol file ****/
#if PROTO
#define PROTO_OUT(a)            { if (modp()) { ostream *__p = modp()->protoStream(); if (__p) { LockGlobalGuard(true, LockGlobalGuard::coutLock); *__p << a; __p->flush(); } } }
#define PROTO_OUTL(a)           { if (modp()) { ostream *__p = modp()->protoStream(); if (__p) { LockGlobalGuard(true, LockGlobalGuard::coutLock); *__p << a << endl; } } }
#define PROTO_MOD_OUT(m,a)      { if (m->modp()) { ostream *__p = m->modp()->protoStream(); if (__p) { LockGlobalGuard(true, LockGlobalGuard::coutLock); *__p << a; __p->flush(); } } }
#define PROTO_MOD_OUTL(m,a)     { if (m->modp()) { ostream *__p = m->modp()->protoStream(); if (__p) { LockGlobalGuard(true, LockGlobalGuard::coutLock); *__p << a << endl; } } }
#else
#define PROTO_OUT(a)
#define PROTO_OUTL(a)
#define PROTO_MOD_OUT(m,a)
#define PROTO_MOD_OUTL(m,a)
#endif


/****** macro definitions for memory allocation ****/
#define ARRAY_CAP_MIN           30          /* minimum capacity for array */
#define ARRAY_CAP_GROW          0.1         /* relative growth for array */
#define ARR_NEWCAP(oc)          (ARRAY_CAP_MIN >= ARRAY_CAP_GROW * oc ? (unsigned long)(oc + ARRAY_CAP_MIN) : (unsigned long)(oc * (1 + ARRAY_CAP_GROW)))
#define ARR_REALLOC(p,t,l,c)    { t *__x = p; p = new t[c]; if(__x) { memcpy(p, __x, sizeof(t)*l); delete __x; } }


#endif // MAINCONTROL_HH

