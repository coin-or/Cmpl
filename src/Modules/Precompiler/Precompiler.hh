
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


#ifndef PRECOMPILER_HH
#define PRECOMPILER_HH

#include <string>
#include <vector>
#include <map>
#include <stack>
#include <set>

#include "../../Control/ModuleBase.hh"
#include "../../Control/FileBase.hh"


using namespace std;



namespace cmpl
{
    class ExtensionBase;


    /**
	 * <code>Precompiler</code> reads the cmpl input files and processes cmpl header commands.
	 * The result is an input string for the <code>Compiler</code> module.
	 */
	class Precompiler : public ModuleBase
	{

		/************** command line options **********/

	private:
		map<int, vector<FileInput *>> _inputFiles;				///< cmpl input (files and strings), per order number
        string _cmdlDataPseudoFile;                             ///< const string used as pseudo file name to mark a FileInput object as command line option for a data import extension

		map<string, string> _fileAlias;							///< file name aliases
		map<string, unsigned> _fileAliasPrio;					///< priority of file name aliases (same keys as in _fileAlias)

		FileOutput _outFile;									///< output result also to this file (beside of storing it in MainData)
		FileOutput _outExtInfo;									///< output extern data info also to this file (beside of storing it in MainData)

		int _startDefLocNr;										///< start for numbers of location definition

#if PROTO
		bool _traceScanning;									///< write scanner trace output to stderr
#endif


	public:
		/**
		 * destructor
		 */
		~Precompiler();

        /**
         * get file name aliases
         */
        inline map<string, string>& fileAlias()         { return _fileAlias; }


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
         * execute step for every registered extension for a single command line option
         * @param extStep       extension execution step
         * @param extRef        map from extension identifier to command line option reference key
         * @param prio          priority value of option
         * @param opt           option
         * @return              true if option is used by an extension
         */
        virtual bool execExtOption(int extStep, map<int, int>& extRef, int prio, CmdLineOptList::SingleOption *opt);

        /**
         * writes usage info for the module to stream
         * @param s				stream to write to
         */
        virtual void usage(ostream& s);


		/************** mode for cmpl header **********/
	private:
		enum HeaderMode {
			headerMode_No,											///< not within a cmpl header line
			headerMode_Init, 										///< start of cmpl header line
			headerMode_Include,										///< include
			headerMode_Arg,											///< arg
			headerMode_Other,										///< command line option
            headerMode_Error,										///< after error in cmpl header line
            headerMode_Data,										///< data import (handled by appropriate extension)
        };

        /**
         * info object used for headerMode_Data
         */
        struct HeaderModeDataInfo {
            CmdLineOptList::SingleOption *opt;                      ///< command line option
            map<int, int> extRef;                                   ///< map from extension identifier to command line option reference key

            /**
             * constructor
             * @param o         command line option
             * @param m         map from extension identifier to command line option reference key
             */
            HeaderModeDataInfo(CmdLineOptList::SingleOption *o, const map<int, int>& m): opt(new CmdLineOptList::SingleOption(o, NULL)), extRef(m)     { }

            /**
             * destructor
             */
            ~HeaderModeDataInfo()           { delete opt; }
        };


		/************** command line option handling **********/
	private:
        /**
         * handle external data option by delivering it to all potentially concerned extensions
         * @param extRef        map from extension identifier to command line option reference key
         * @param opt           option
         * @return              true if option is used by an extension
         */
        bool execExtOptionData(map<int, int>& extRef, CmdLineOptList::SingleOption *opt);

		/**
		 * parse command line option with input args
		 * @param opt           option
		 * @param isFile        arg is input file
		 * @param hm	        header mode
         * @param extRef        only for hm==headerMode_Data: map from extension identifier to command line option reference key
		 */
        void parseInput(CmdLineOptList::SingleOption *opt, bool isFile, HeaderMode hm, map<int, int> *extRef = NULL);


		/************** scanning **********/
	private:
		void *_scanner;												///< scanner context object
		ostringstream *_resStream;									///< stream to write precompiler result to

		map<int, size_t> _inputFilesCur;							///< next current element of cmpl inputs (files and strings), per order number

        FileInput *_curFile;                                        ///< current input
        FileInput *_dummyFile;                                      ///< dummy file object only for name replacements
		
		/**
		 * executes the scanner
		 */
		void doScan();

	public:
		/**
		 * get stream to write precompiler result to
		 */
		inline ostringstream& resStream()							{ return *_resStream; }

		/**
		 * go to the next input
		 * @param buf       return a string input or NULL if the next input is no string input
		 * @return          opened input file or NULL if the next input is no file
		 */
		FILE *iterInput(const char **buf);

		/**
		 * search for next not consumed input
		 * @param inp		map with inputs
		 * @param cnt		map with current input counters
		 * @param key		return key of found input
		 * @param cur		return current counter of found input
		 * @return			input found / false: all inputs are consumed
		 */
		bool searchNextInput(map<int, vector<FileInput *>>& inp, map<int, size_t>& cnt, int& key, size_t& cur);



		/************** cmpl header **********/
	private:
		HeaderMode _headerMode;										///< mode of cmpl header line

		/**
		 * word within a cmpl header line
		 */
		struct HeaderWord {
			string txt;												///< word
			bool quoted;											///< word was written with quotes
			size_t len;												///< length of word as written
			PositionInfo pos;										///< start position
		};

		HeaderWord *_curHeaderWord;									///< current scanned word within the cmpl header line
		vector<HeaderWord *> _headerWords;							///< words to process

		set<int> _headerDataNamespaces;								///< already defined namespaces by header data commands

		HeaderWord *_includeWord;									///< word to process if _headerMode == headerMode_Include
		LocationInfo _includeLoc;									///< location with current include

	public:
		/**
		 * start cmpl header line
		 */
		void headerStart();

		/**
		 * end cmpl header line
		 * @param eof		end by eof or by eol
		 * @return			execute include operation now
		 */
		bool headerEnd(bool eof);

		/**
		 * add word to scanned cmpl header text
		 * @param txt		word
		 * @param quoted	word is quoted
		 * @param pos		start position of word
		 */
		void headerWord(const char *txt, bool quoted, PositionInfo *pos);

        /**
         * separating spaces scanned in cmpl header text
         * @param cnt       count of separating spaces
         */
        void headerSep(size_t cnt);

//		/**
//		 * add character to scanned cmpl header text
//		 * @param chr		character
//		 * @param spaces	count of leading spaces before the character
//		 */
//		void headerChar(char chr, size_t spaces);

		/**
		 * get whether in header line
		 */
		inline bool isHeaderMode()					{ return (_headerMode != headerMode_No); }

		/**
		 * output error message about wrong c comment in header: line feed
		 */
		void outErrorCCommentLf();

		/**
		 * output error message about wrong c comment: end without start
		 */
		void outErrorCCommentEWS();

        /**
         * test whether namespace is already defined by header data commands
         * @param nm        namespace name
         * @param ins       insert namespace as defined
         * @return          true if namespace is already defined
         */
        bool testHeaderDataNamespaces(int nm, bool ins)       { if (_headerDataNamespaces.count(nm)) { return true; } if (ins) { _headerDataNamespaces.insert(nm); } return false; }

	private:
		/**
		 * process header line with command line options
		 * @param argLine	header line with mode headerMode_Arg or headerMode_Other
		 */
		void processHeaderArg(bool argLine);

		/**
		 * frees header words in _headerWords
		 */
		void freeHeaderWords();



		/************** includes **********/
	private:
        stack<FileInput *> _inclFiles;                              ///< stack of included files
		bool _initInclude;											///< include is initialized

#if CHDIR
		stack<string> _inclDirName;									///< stack of path names of includes
		string _dirStartName;										///< path name of the starting directory
		char _dirSepChar;											///< path separator char
#endif

    public:
		/**
		 * initializes the possibility for including files
		 */
		void includeInit();

        /**
         * get whether within included file
         */
        bool InInclFiles() const                { return !_inclFiles.empty(); }

#if CHDIR
        /**
         * get path separator char
         */
        char dirSepChar() const                 { return _dirSepChar; }

		/**
		 * gets the current directory name
		 * @return		current directory name
		 */
		const string getCurrentWd();

		/**
		 * splits a file name which can contain directory name to the file name and the directory name
		 * @param filename				file name with path
		 * @param path					return the directory name
		 * @return						file name without directory name
		 */
		string splitPathName(string &filename, string &path);
#endif

	public:
		/**
		 * opens the included file and stores the current file to the include stack
		 * @param eof               file within the include lies is already finished
		 * @return                  opened include file
		 */
		FILE *openIncludeFile(bool eof);

		/**
		 * close the included file
		 */
		void closeIncludeFile();


		/************** location handling **********/
	private:
		int _curDefLocNr;											///< next current number for location definition
		map<int, LocationInfo *> _defLoc;							///< defined locations

		vector<PositionInfo *> _curPos;								///< stack with current positions
		PositionInfo *_startCommentPos;								///< start position of current c comment

	public:
		/**
		 * get current position object
		 */
		inline PositionInfo *curPos()								{ return (_curPos.empty() ? NULL : _curPos.back()); }

		/**
		 * write location definition to result stream, or find number of already defined location
		 * @param loc				location
		 * @param outTN             output also type and name
		 * @param defNr				definition number of location / 0: use a new one
		 * @return					definition number of location
		 */
		int outDefLoc(const LocationInfo *loc, bool outTN, int defNr = 0);

		/**
		 * write start position to result stream
		 * @param pos               position object
		 */
		void outStartPos(const PositionInfo& pos);

		/**
		 * write start position to result stream
		 * @param pos               position object
		 * @param locNr             definition number of location within the position lies / 0: no one
		 */
		void outStartPos(const PositionInfo& pos, int locNr);

		/**
		 * stores current position as start position of a c comment
		 */
		void storeCommentStartPos();

		/**
		 * output to result stream after a c comment
		 * @param eol				after comment follows end of line
		 */
		void outPositionAfterComment(bool eol);

		/**
		 * set InLoc in position
		 * @param posOrg			original position for error message only
		 * @param pos               position to set inLoc within
		 * @param inLoc             inLoc number as parsed from position mark
		 * @return                  inLoc for output / 0: no one
		 */
		int setInLocPM(PositionInfo& posOrg, PositionInfo& pos, int inLoc);

		/**
		 * parse DefLoc position mark
		 * @param txt				text with DefLoc info
		 */
		void parseDefLocPM(const char *txt);

		/**
		 * parse StartPos position mark
		 * @param txt				text with StartPos info
		 */
		void parseStartPosPM(const char *txt);

		/**
		 * parse Line position mark
		 * @param txt				text with line number
		 */
		void parseLinePM(const char *txt);

		/**
		 * parse Col position mark
		 * @param txt				text with col number
		 */
		void parseColPM(const char *txt);

		/**
		 * output error message about wrong position mark
		 * @param txt				text with error
		 */
		void outErrorPositionMark(const char *txt);
	};


    /**
     * class for the additional parameter for extensions for handling of external data import.
     * objects of this class exists only while executing Precompiler::execExtOptionData().
     */
    class DataImportExtensionPar
    {
    public:
        /**
         * abstract base class for internal data used by the extension
         */
        class InternalDataBase
        {
        protected:
            Precompiler *_mod;                      ///< calling module
            ExtensionBase *_ext;                    ///< extension
            CmdLineOptList::SingleOption *_opt;     ///< command line option for external data import

        public:
            /**
             * constructor
             * @param mod       module calling the extension
             * @param ext       extension
             * @param opt       command line option for external data
             */
            InternalDataBase(Precompiler *mod, ExtensionBase *ext, CmdLineOptList::SingleOption *opt): _mod(mod), _ext(ext), _opt(opt)      { }

            /**
             * get calling module
             */
            inline ModuleBase *modp() const                                     { return _mod; }

            /**
             * get name of the calling module
             */
            inline const char *moduleName() const                               { return _mod->moduleName(); }

            /**
             * get main control object
             */
            inline MainControl *ctrl() const                                    { return _mod->ctrl(); }

            /**
             * get main data object
             */
            inline MainData *data() const                                       { return _mod->data(); }

            /**
             * destructor
             */
            virtual ~InternalDataBase()                                         { }

            /**
             * run handling of command line option for external data import
             * @return      true if option is handled here
             */
            virtual bool run() = 0;
        };

    private:
        InternalDataBase *_data;                    ///< internal data
        int _handlePrio;                            ///< priority of handling the given external data import by this extension

    public:
        /**
         * constructor
         */
        DataImportExtensionPar(): _data(NULL), _handlePrio(0)       { }

        /**
         * destructor
         */
        ~DataImportExtensionPar()                   { if (_data) delete _data; }

        /**
         * get priority of handling the given external data import by this extension
         */
        int handlePrio()                            { return _handlePrio; }

        /**
         * run handling of the given external data import by this extension
         * @return          true if really handled
         */
        bool runForData()                           { if (_data) return _data->run(); else throw logic_error("call of DataImportExtensionPar::runForData() for uninitialized object"); }

        /**
         * set internal data object
         */
        void setData(InternalDataBase *d)           { { if (_data) delete _data; } _data = d; }

        /**
         * set priority of handling the given external data import by this extension
         */
        void setHandlePrio(int hp)                  { _handlePrio = hp; }
    };
}

/* function prototype for the scanner */
#define YY_DECL			int precmpllex ( void* yyscanner, cmpl::Precompiler* precomp )
YY_DECL;


#endif // PRECOMPILER_HH

