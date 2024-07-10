
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


#ifndef MODULESCONF_HH
#define MODULESCONF_HH

#include <string>
#include <unordered_map>
#include <vector>

#include "StringStore.hh"
#include "CmdLineOptList.hh"


// values for ModulesConf::ModOrderFunc::order
#define MODULE_CONF_ORDER_PREP			10			// preparation of data including precompile
#define MODULE_CONF_ORDER_TO_SERVER		20			// optionally transfer of execution to cmpl server
#define MODULE_CONF_ORDER_CMPL_PARSE	30			// generate intermediary code from the cmpl file
#define MODULE_CONF_ORDER_CMPL_EXEC		40			// execute the intermediary code
#define MODULE_CONF_ORDER_CMPL_LIN		50			// linearization and others
#define MODULE_CONF_ORDER_OUTM			60			// write the generated matrix
#define MODULE_CONF_ORDER_OPT			70			// run the optimization
#define MODULE_CONF_ORDER_RESULT		80			// result preparation
#define MODULE_CONF_ORDER_FROM_SERVER	90			// optionally transfer of execution back from cmpl server

#define MODULE_CONF_ORDER_OUTRES		100			// result output
#define MODULE_CONF_ORDER_MATRIX		101			// generated matrix output

#define MODULE_CONF_ORDER_SOLVERCBC		200			// solver cbc
#define MODULE_CONF_ORDER_SOLVERGLPK	201 		// solver glpk
#define MODULE_CONF_ORDER_SOLVERCPLEX	202 		// solver cplex
#define MODULE_CONF_ORDER_SOLVERGUROBI	203 		// solver gurobi
#define MODULE_CONF_ORDER_SOLVERSCIP    204         // solver scip
#define MODULE_CONF_ORDER_SOLVERHIGHS    205         // solver scip

#define MODULE_CONF_ORDER_SOLUTIONASCII 300         // ascii solution report
#define MODULE_CONF_ORDER_SOLUTIONCSV   301         // csv solution report
#define MODULE_CONF_ORDER_SOLUTIONXML   302         // CmplSolution report


using namespace std;


namespace cmpl
{
	class MainControl;
	class ModuleBase;


    /**
	 * <code>ModulesConf</code> handles construction and configuration of module classes.
	 */
	class ModulesConf
	{
		friend class MainControl;
		friend class CmdLineOptList;

	private:
		/**
		 * struct for order number and constructing function of a module
		 */
		struct ModOrderFunc {
			int order;												///< order number
			ModuleBase* (*func)(MainControl*, const char*);			///< constructing function for module
		};

		/**
		 * struct for configuration info for a module
		 */
		struct ModConfInfo {
			string rawValue;										///< unparsed string, other fields only filled after parsing this string
			PositionInfo pos;										///< position of rawValue
			int mode;												///< 0: only rawValue and file position filled; 1: also module filled; 2: all fields filled

			bool modAggr;											///< module is a module aggregation
			const char *module;										///< module name
			const char *alias;										///< alias name or NULL if no alias
			int order;												///< order number / -1: normal order
			CmdLineOptList opts;									///< option list

			/**
			 * constructor
			 */
			ModConfInfo(string& s, PositionInfo& p): rawValue(s), pos(p), mode(0) { }
		};

		/**
		 * struct for description of modules and module aggregations, only for usage info
		 */
		struct ModDescr {
			const char *name;										///< name of module or module aggregation
			bool modAggr;											///< module is a module aggregation
			string descr;											///< description

			/**
			 * constructor
			 */
			ModDescr(const char *n, bool a, string d): name(n), modAggr(a), descr(d) { }
		};


		MainControl *_ctrl;													///< main execution object
		string *_filename;													///< name of configuration file
		LocationInfo _locConf;												///< location of name of configuration file

		StringStore _modNames;												///< store for names and aliases of modules and module aggregations
		unordered_map<const char *, ModOrderFunc> _modules;					///< constructing function of module per module name
		unordered_map<const char *, vector<ModConfInfo *>> _modAggr;		///< module aggregations per name
		vector<ModDescr> _modDescr;											///< description of modules and module aggregations

		
		/**
		 * constructor.
		 * @param ctrl		main object
		 * @param file		filename of the module configuration file
		 * @param loc		location of name of configuration file
		 */
		ModulesConf(MainControl *ctrl, string *file, LocationInfo &loc)				{ setCtrl(ctrl); initModuleNames(); readModuleConf(file, loc); }

		/**
		 * destructor
		 */
		~ModulesConf();

		/**
		 * sets main object
		 * @param ctrl		main object
		 */
		inline void setCtrl(MainControl *ctrl)										{ _ctrl = ctrl; _modNames.setModp((ModuleBase *)ctrl, "potential module names"); }

		/**
		 * fills <code>_modules</code>
		 */
		void initModuleNames();

		/**
		 * read module configuration file
		 * @param file		filename of the module configuration file
		 * @param loc		location of name of configuration file
		 */
		void readModuleConf(string *file, LocationInfo &loc);

		/**
		 * get the name of used module configuration file
		 */
		inline string *filename()													{ return _filename; }

		/**
		 * get main object
		 */
		inline MainControl *ctrl()					{ return _ctrl; }

		/**
		 * get module to which this belongs
		 */
		ModuleBase *modp();

		/**
		 * search a module name and give back the stored name, throw exception if not found.
		 * @param name          module name to search
		 * @return              module name as stored string
		 */
		const char *testModuleName(const char *name);

		/**
		 * constructs a new object of the module class referenced by a name
		 * @param name		name of the module
		 * @param alias		alias name for the module (can be the same as <code>name</code>)
		 * @return			new object of the module class
		 */
		ModuleBase *constructModule(const char *name, const char *alias)			{ return (_modules.find(name)->second.func)(_ctrl, alias); }

		/**
		 * parse module name from command line options, and fill ctrl->_modules
		 * @param name		module name from command line options, can also be "alias=modulName[,orderNo]"
		 * @param err		throw error if <code>name</code> is no valid module name
		 * @param pos		position of module name, should have type POSITION_TYPE_FIXLOC or POSITION_TYPE_DESCR
		 * @return			<code>name</code> contains a module name
		 */
		bool parseCmdLineOptModule(string& name, bool err, PositionInfo& pos);

		/**
		 * parse module name from command line options, and fill ctrl->_modules
		 * @param name		possible module name from command line options
		 * @param err		throw error if <code>name</code> is no valid module name
		 * @param opt		command line options for module
		 * @return			<code>name</code> contains a module name
		 */
		bool parseCmdLineOptModule(string& name, bool err, CmdLineOptList::SingleOption *opt);

		/**
		 * test whether <code>opt</code> contains a module name, parse it and fill ctrl->_modules 
		 * @param info      module info
		 * @param err		show error if <code>info.module</code> is no valid module name
		 * @param modNames	vector with rekursive module/module aggregation names / NULL: no such vector
		 * @return			<code>info.module</code> contains a module name
		 */
		bool testParseCmdLineOptModule(ModConfInfo& info, bool err, vector<const char *> *modNames = NULL);

		/**
		 * insert recursive modules for a module aggregation
		 * @param mod       name of module aggregation
		 * @param modNames  vector with rekursive module/module aggregation names / NULL: no such vector
		 */
		void insertRekModules(const char *mod, vector<const char *> *modNames);

		/**
		 * fill field <code>module</code> in ModConfInfo from <code>ModConfInfo::rawValue</code> if necessary.
		 * @param info      module info
		 */
		void fillModConfInfoName(ModConfInfo& info);

		/**
		 * fill all fields in ModConfInfo from <code>ModConfInfo::rawValue</code> if necessary.
		 * @param info      module info
		 * @param aggr      info contains a module aggregation
		 */
		void fillModConfInfoAll(ModConfInfo& info, bool aggr);

		/**
		 * fill <code>ModConfInfo::opts</code> from string with command line options
		 * @param info      module info
		 * @param opt       string with command line options
		 * @param pos		start position of string <code>opt</code>
		 */
		void parseCmdLineOptList(ModConfInfo& info, string opt, PositionInfo& pos);

	};


	/**
	 * <code>ModuleNotFoundException</code> is thrown if there is no module for a given name
	 */
	class ModuleNotFoundException : public exception
	{
	public:
		/**
		 * get exception info
		 */
		virtual const char *what() const throw()          { return "no such module"; }
	};
}

#endif // MODULESCONF_HH

