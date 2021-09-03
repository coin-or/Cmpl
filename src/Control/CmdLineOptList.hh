
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


#ifndef CMDLINEOPTLIST_HH
#define CMDLINEOPTLIST_HH

#include <string>
#include <vector>
#include <cstdlib>

#include "LocationInfo.hh"
#include "MainData.hh"


using namespace std;


namespace cmpl
{
	class MainControl;
	class ModulesConf;
	class MainData;


	/**
	 * <code>CmdLineOptList</code> represents the command line options for cmpl
	 */
	class CmdLineOptList
	{
		friend class MainControl;
		friend class ModuleBase;


	public:
		/**
		 * <code>SingleOption</code> contains a single option out of the command line options
		 */
		class SingleOption
		{
			friend class CmdLineOptList;
			friend class ModulesConf;

		private:
			CmdLineOptList *_cmdLineOptList;	///< <code>CmdLineOptList</code> to which this option belongs
			string _opt;						///< option name (without leading '-')
			string _module;						///< module name for which this option is valid / empty: for all modules

			LocationInfo _locName;				///< location of the option name
			LocationInfo _locOpt;				///< location of option including arguments

			bool _neg;							///< negated option
			unsigned _prio;						///< priority of option

			vector<string> _args;				///< argument strings
			vector<PositionInfo> _argPos;		///< start position of every element in _args (vector has same size as _args)
			unsigned _sepArgStart;				///< index of first arg in <code>_args</code> which is separated by a space

			int _useCount;						///< count of usings of this option by modules


			/**
			 * constructor
			 */
			SingleOption(CmdLineOptList *cmdLineOptList)				{ _cmdLineOptList = cmdLineOptList; _neg = false; _prio = 0; _sepArgStart = 0; _useCount = 0; }

        public:
            /**
             * copy constructor for another CmdLineOptList
             * @param s         source single option
             * @param l         CmdLineOptList object
             */
            SingleOption(SingleOption *s, CmdLineOptList *l):
                _cmdLineOptList(l), _opt(s->_opt), _module(s->_module), _locName(s->_locName), _locOpt(s->_locOpt), _neg(s->_neg), _prio(s->_prio),
                _args(s->_args), _argPos(s->_argPos), _sepArgStart(s->_sepArgStart), _useCount(s->_useCount)            { }

        private:
			/**
			 * set option name
			 * @param n			option name, can be "[module.][no-]option[!...][=arg1,...]"
			 * @param pos		start position of the option, only for error messages
			 */
			void setOpt(string& n, PositionInfo& pos);

			/**
			 * check whether this option is equal to another option
			 * @param o			other option
			 * @return			true if options are equal
			 */
			bool isEqual(SingleOption& o);


		public:
			/**
			 * add an arg for an option
			 * @param a			argument (can also contain spaces)
			 * @param info		start position of the arg, only for error messages
			 * @return			this
			 */
			SingleOption* arg(string& a, PositionInfo& pos);


		public:
			/**
			 * get option name
			 */
			inline string& opt()								{ return _opt; }

			/**
			 * get whether negated option
			 */
			inline bool neg()									{ return _neg; }

			/**
			 * get explicit priority of option
			 */
			inline unsigned prio()								{ return _prio; }

			/**
			 * get location of option
			 * @param wa		location with arguments / false: location of option name only
			 */
			inline LocationInfo& loc(bool wa)					{ return (wa ? _locOpt : _locName); }

			/**
			 * get position of an argument of the option
			 * @param i			index
			 */
			inline PositionInfo& argPos(size_t i)				{ return _argPos[i]; }

			/**
			 * mark the option as used
			 * @param n			name of using module
			 */
			inline void markUsed()								{ _useCount++; }

            /**
             * get count of usings of this option by modules
             */
            inline int useCount()								{ return _useCount; }

			/**
			 * get size of argument vector
			 */
			inline size_t size()								{ return _args.size(); }

			/**
			 * get an argument string from _args
			 * @param i			index
			 */
			inline string& operator[] (size_t i)				{ return _args[i]; }

			/**
			 * get an argument as int number, throw error if arg is not an int.
			 * @param i			index
			 */
			int argAsInt(size_t i, MainControl *ctrl);

			/**
			 * get index of first arg which is separated by a space
			 */
			inline unsigned sepArgStart()						{ return _sepArgStart; }

			/**
			 * test whether option is valid for names in <code>modNames</code>
			 * @param modNames	relevant names for module specification in command line options
	 		 * @return          option is valid with this level of module specification / -1: not valid for the module
			 */
			int testModName(vector<const char *> &modNames);

            /**
             * test whether option is valid for given module name
             * @param modName   relevant name for module specification in command line options
             * @return          option is valid with this level of module specification / -1: not valid for the module
             */
            int testModName(const char *modName)                { return (_module.empty() ? 0 : (_module == modName ? 1 : -1));	}
        };


	public:
		/**
		 * <code>RegOption</code> is used to register that an option is to deliver to a modul
		 */
		struct RegOption
		{
			int ref;					///< reference number
			const char *opt;			///< option
			int minArgs;				///< minimal count of args for the option
			int maxArgs;				///< maximal count of args for the option / -1: unbound
			int allowNeg;				///< negated option: 0: don't deliver; -1: throw error; 1: deliver with check of minArgs and maxArgs; 2: deliver but cannot have any arg
			bool handlePrio;			///< deliver option only if priority is higher / false: deliver independent of priority
		};

        /**
         * extended registration for command line option for an extension
         */
        struct RegOptionExt: public RegOption
        {
            int extID;                  ///< identificator key of the extension
            const char *module;         ///< name of extension module the registration belongs to

            /**
             * constructor
             */
            RegOptionExt(int r, const char *o, int mi, int ma, int an, bool hp, int ei, const char *mod)    { ref = r; opt = o; minArgs = mi; maxArgs = ma; allowNeg = an; handlePrio = hp; extID = ei; module = mod; }
        };


		/****** member of CmdLineOptList ****/
		
	private:
		vector<SingleOption *> _opts;	///< options
		ModulesConf *_modConf;			///< config of module names, stored here for persistence
        string _binFullName;            ///< full name of executed binary / empty when not set or not applicable here


	public:
		/**
		 * constructor for a new empty argument list
		 */
        CmdLineOptList(): _opts(), _modConf(NULL), _binFullName()                       { }

        /**
         * constructor for a new empty argument list
         * @param bfn       full name of executed binary
         */
        CmdLineOptList(const char *bfn): _opts(), _modConf(NULL), _binFullName(bfn)		{ }

		/**
		 * destructor
		 */
		~CmdLineOptList()					{ cleanUp(); }

		/**
		 * clean up this object
		 */
		void cleanUp();

		/**
		 * fill this with command line options as supplied from main()
		 */
		void fillFromMain(int argc, char *argv[]);

        /**
         * fill full name of executed binary
         * @param arg       name of executed binary as given in first command line argument of main function / NULL: no such
         */
        void fillBinFullName(char *arg = NULL);

		/**
		 * add a new option to this
		 * @param opt		option string, the option can have several args separated by spaces
		 * @param pos		start position of the option, only for error messages
		 * @param single	<code>opt</code> contains only of one single option keyword without args
		 * @return			info object for the option
		 */
		SingleOption* addOption(string& opt, PositionInfo& pos, bool single = false);

		/**
		 * delete an option from this, including its args
		 * @param optInfo	info object for the option
		 * @return			true if the option is found and deleted
		 */
		bool delOption(SingleOption* optInfo);

		/**
		 * append command line options from another CmdLineOptList object
		 * @param src		source of command line options to append
		 */
		void append(CmdLineOptList& src);

        /**
         * full name of executed binary / empty when not set or not applicable here
         */
        string& binFullName()               { if (_binFullName.empty()) fillBinFullName(); return _binFullName; }


	private:
		/**
		 * iterate to the first or next relevant option
         * @param modNames	relevant names for module specification in command line options / NULL: don't check module name
		 * @param iter		current iteration index, start with -1
		 * @param modspec	return level of module specification
		 * @return			next option or NULL if no one is left
		 */
        SingleOption* iterOptions(vector<const char *> *modNames, int &iter, int &modspec);

		/**
		 * check if all options in _opts are used by modules
		 * @param ctrl		main object
         * @param mark      mark this command line options as used
         * @param warn      output warning if a command line option ist not used
         */
        void checkCmdLineOptUsed(MainControl *ctrl, set<string> &mark, bool warn);

        /**
         * write all options to a command line option file
         * @param ostr      stream for writing
         * @param tp        type code (1: command line / 2: cmpl header)
         * @param nowrite   don't write this options
         * @param check     check if this option is written
         * @return          whether checked option is written
         */
        bool writeToOptFile(ostream *ostr, int tp, set<string> &nowrite, const char *check);

        /**
         * write single option to a command line option file
         * @param ostr      stream for writing
         * @param tp        type code (1: command line / 2: cmpl header)
         * @param opt       option to write
         */
        static void writeToOptFile(ostream *ostr, int tp, SingleOption *opt);

        /**
         * write single pseudo option to a command line option file
         * @param ostr      stream for writing
         * @param tp        type code (0: pseudo option)
         * @param name      name option to write
         * @param arg       argument for option / NULL: no argument
         * @param fixloc    string for fixed location of option / NULL: no location
         */
        static void writeToOptFile(ostream *ostr, int tp, const char *name, const char *arg, const char *fixloc);

        /**
         * write all options from a command line option file
         * @param ctrl		main object
         * @param istr      stream for reading
         * @param fpos      position info of command line option file
         * @param opts1     command line option list for type code 1
         * @param opts2     command line option list for type code 2
         * @param resname   if a option with this name and type code 0 is found then return its first parameter
         * @return          found parameter value according resname or empty string
         */
        static string readFromOptFile(MainControl *ctrl, istream *istr, PositionInfo &fpos, CmdLineOptList &opts1, CmdLineOptList &opts2, const char *resname);

        /**
         * parse location information for one option from a command line option file
         * @param ctrl		main object
         * @param opt       option
         * @param tp        type code (1: command line / 2: cmpl header / 0: internal)
         * @param ol        true: location for option, using locs1 and locs2 / false: position for argument of option, using only locs1
         * @param locs1     first location info string
         * @param locs2     second location info string
         */
        static void parseLocFromOptFile(MainControl *ctrl, SingleOption& opt, int tp, bool ol, string *locs1, string *locs2 = NULL);


		/****** functions for serialization ****/

	public:
		/**
		 * insert strings and locations used from this in the data object
		 */
		void insertStringsAndLocs(MainData *data);

		/**
		 * get count of sub data objects
		 */
		inline int subCnt()						{ return _opts.size(); }

		/**
		 * write data object to output stream
		 * @param si		serialization info
		 * @param data		main data object
		 */
		void serializeTo(MainData::SerializeInfo& si, const MainData *data);

		/**
		 * fill data object from input stream
		 * @param si		serialization info
		 * @param data		main data object
		 * @param subCnt    count of sub data objects
		 */
		void deserializeFrom(MainData::SerializeInfo& si, const MainData *data, int subCnt);
	};
}
#endif // CMDLINEOPTLIST_HH
