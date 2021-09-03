
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


#ifndef SOLVERSELECT_HH
#define SOLVERSELECT_HH

#include <map>
#include <array>

#include "../../Control/ExtensionBase.hh"
#include "../../CommonData/ExtensionSteps.hh"


using namespace std;


namespace cmpl
{
    class Interpreter;
    class OptModel;
    class SolverSelectMod;


    /*********** command line options delivered to the extension by run() **********/

    #define OPTION_EXT_EXEC_SOLVERSEL_CONFIG_FILE           101
    #define OPTION_EXT_EXEC_SOLVERSEL_SOLVERNAME            201



    /**
	 * the <code>SolverSelect</code> class is an extension to the interpreter module
	 * for selecting a solver and appropriate options depending on the optimization model properties.
	 */
	class SolverSelect : public ExtensionBase
	{
    private:
        /**
         * extensions within the optimization model
         */
        enum ModelExtension {
            modelExtCondition,          ///< logical conditions or operations between optimization constraints
            modelExtConditionTriv,      ///< trivial logical operations between optimization constraints (i.e. only logical And)
            modelExtIntVarProd,         ///< products of variables, all at least with one integer operand
            modelExtIntVarProdBin,      ///< products of variables, all at least with one binary operand
            modelExtSOS,                ///< SOS or SOS2
            //TODO: other extensions

            modelExt_Size               ///< count of possible model extensions
        };

    private:
        unsigned _configFile;					///< filename of solver select config file / 0: no config file (nothing to do)
        LocationInfo _configFileLoc;			///< location of filename for config file

        string _solver;                         ///< explicit selected solver / empty: no one
        string _section;                        ///< name of section to read

        map<string, bool> _cache;               ///< cache for already evaluated expressions

        map<string, int> _modelTypes;           ///< Names of model types, value is OptModel::ModelType
        map<string, int> _modelExt;             ///< Names of model extensions, value is ModelExtension

        const string _kwlast = "last";          ///< key word "last"
        const string _kwelse = "else";          ///< key word "else"

        int _omType;                            ///< type of optimization model (save extensions) (type is OptModel::ModelType)
        array<int, modelExt_Size> _omExt;       ///< model extensions: 1: extension itself is present / -1: subset of the extension is present / otherwise 0


	public:
		/**
		 * constructor
		 * @param mod			module creating this extension object
		 */
        SolverSelect(SolverSelectMod *mod);


		/************** overwritten methods of <code>ExtensionBase</code> **********/

	protected:
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
       virtual bool run(ModuleBase *mod, int step, int id, int ref, int prio, CmdLineOptList::SingleOption *opt, void *par);

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
		virtual const char *extName()				{ return "solverSelect"; }
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
        virtual void deserializeFrom(MainData::SerializeInfo& si, const MainData *data, int subCnt, int lc, string& rline);

    private:
        /**
         * execute solver selection by reading config file
         * @param modp      intepreter module calling the extension
         * @param om        optimization model
         */
        void exec(Interpreter *modp, OptModel *om);

        /**
         * execute solver selection using open config file
         * @param modp      intepreter module calling the extension
         * @param om        optimization model
         * @param conf      input stream for reading open config file
         */
        void exec(Interpreter *modp, OptModel *om, istream *conf);

        /**
         * execute for a line within the solver section of the config file
         * @param modp      intepreter module calling the extension
         * @param om        optimization model
         * @param pos       position within config file
         * @param line      line from config file
         * @param begin     start of non-whitespace within <code>line</code>
         * @return          true: break execution of current config file section
         */
        bool execLine(Interpreter *modp, OptModel *om, PositionInfo& pos, string line, size_t begin);

        /**
         * evaluate logical expression about boolean conditions about optimization model
         * @param modp      intepreter module calling the extension
         * @param om        optimization model
         * @param pos       position within config file
         * @param line      line from config file
         * @param begin     start of expression within <code>line</code>
         * @param end       after end of expression within <code>line</code>
         */
        bool evalBoolExpr(Interpreter *modp, OptModel *om, PositionInfo& pos, string line, size_t begin, size_t end);

        /**
         * evaluate single boolean condition about optimization model
         * @param modp      intepreter module calling the extension
         * @param om        optimization model
         * @param pos       position within config file
         * @param line      line from config file
         * @param begin     start of condition name within <code>line</code>
         * @param end       after end of condition name within <code>line</code>
         */
        bool evalCondition(Interpreter *modp, OptModel *om, PositionInfo& pos, string line, size_t begin, size_t end);

        /**
         * evaluate condition about model type with specified model extensions
         * @param modp      intepreter module calling the extension
         * @param om        optimization model
         * @param pos       position within config file
         * @param line      line from config file
         * @param begin     start of names of model extensions within <code>line</code>
         * @param end       after end of names of model extensions within <code>line</code>
         * @param mtp       given model type (is OptModel::ModelType)
         */
        bool evalModelTypeAndExt(Interpreter *modp, OptModel *om, PositionInfo& pos, string line, size_t begin, size_t end, int mtp);

        /**
         * search matching closing bracket for given opening bracket
         * @param modp      intepreter module calling the extension
         * @param pos       position within config file
         * @param line      line from config file
         * @param begin     position of opening bracket within <code>line</code>
         * @param end       end of search range within <code>line</code>
         * @return          position of closing bracket
         */
        size_t findMatchBracket(Interpreter *modp, PositionInfo& pos, string line, size_t begin, size_t end);

        /**
         * add command line options
         * @param modp      intepreter module calling the extension
         * @param opts      command line options to add
         * @param lc        position within config file
         */
        void addCmdLineOpts(Interpreter *modp, string& opts, PositionInfo& pos);


        /**
         * init model type and extensions
         * @param om        optimization model
         */
        void initModelType(OptModel *om);

        /**
         * get model extensions if model is considered to have a given type
         * @param om        optimization model
         * @param itp       given model type (OptModel::ModelType)
         * @param ext       return of model extensions / NULL: check if model type is compatible without any extension
         * @return          false if given type is incompatible with model
         */
        bool getModelExtForType(OptModel *om, int itp, array<int, modelExt_Size> *ext);
	};
}

#endif // SOLVERSELECT_HH

