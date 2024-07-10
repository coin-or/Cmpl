
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


#ifndef EXTERNDATAFILE_HH
#define EXTERNDATAFILE_HH


#include "DataImportBase.hh"
#include "../../CommonData/ExternDataInfo.hh"
#include "../../CommonData/CmplVal.hh"
#include "../../Modules/Precompiler/Precompiler.hh"


using namespace std;



namespace cmpl
{
    class ExternDataFileHandleOpt;
    class ExternDataFileImport;
    class ExecContext;
    struct ImportExternDataInfo;


    /*********** command line options delivered to the extension by run() **********/

    #define OPTION_EXT_EXEC_PRECOMP_DATFILE     101
    #define OPTION_EXT_EXEC_PRECOMP_ASSARRSUB   102

    #define OPTION_EXT_EXEC_PRECOMP_DATA        105
    #define OPTION_EXT_EXEC_PRECOMP_DATA2       106

    #define OPTION_EXT_EXEC_INTERPRET_READALL   111



    /**
	 * the <code>ExternDataFile</code> class is an extension to the interpreter module
	 * for importing data from a cmpl data file.
	 */
	class ExternDataFile : public DataImportBase
	{
    protected:
        friend class ExternDataFileHandleOpt;

        const char *_cdatDefault;   ///< default name for cmpl data file
        bool _readAll;              ///< read values for all symbols from a data file at once
        bool _assArraySub;          ///< accept for a given array assertion a definition set that is a subset of the given set / false: check for equality

    public:
		/**
		 * constructor
		 * @param mod			module creating this extension object
         * @param est           type of external data source handled by this extension class
         * @param hp            priority of handling with this extension if more than one extension can handle it
         * @param ra            read values for all symbols from a data file at once
         */
        ExternDataFile(ModuleBase *mod, int est, int hp): DataImportBase(mod, est, hp), _cdatDefault(NULL), _readAll(false), _assArraySub(true)  { }

        /**
         * get default name for cmpl data file
         */
        inline const char *cdatDefault()                   { return _cdatDefault; }

        /**
         * get standard name for cmpl data file
         */
        virtual const char *cdatStandard()                 { return IO_FILE_STANDARD_CDAT; }

        /**
         * get whether read values for all symbols from a data file at once
         */
        inline bool readAll()                              { return _readAll; }

        /**
         * get whether accept for a given array assertion a definition set that is a subset of the given set
         */
        inline bool assArraySub()                          { return _assArraySub; }

#if PROTO
		/**
		 * get name of the extension, only used for protocol output
		 */
		virtual const char *extName()				{ return "externDataFile"; }
#endif //PROTO

    public:
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



        /************** handle registered actions **********/
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


        /************** import of data **********/

	protected:
        map<int, ExternDataFileImport*>	_files;     ///< objects for import and caching of the external data, key denotes the data file

        /**
         * destructor
         */
        virtual ~ExternDataFile();

        /**
         * read all symbols from a data file
         * @param fh            file handling object
         * @param inStr         stream for reading data file
         * @param fn            name of data file
         * @param nm            namespace name / -1: no namespace
         * @param loc			location of data header line
         */
        virtual void readFileAllSymbols(ExternDataFileHandleOpt *fh, istream *inStr, string& fn, int nm, LocationInfo& loc);

        /**
         * insert data symbol in info about extern data
         * @param fh            file handling object
         * @param nm            namespace name / -1: no namespace
         * @param sym           symbol name
         * @param fn            file name of cmpl data file
         * @param loc           location of data symbol in input file
         * @param sp            position of symbol in the data file / -1: not known
         * @return              true if symbol is inserted to info
         */
        void insertDataSymbol(ExternDataFileHandleOpt *fh, int nm, string& sym, string& fn, LocationInfo& loc, long sp);

        /**
         * import data values
         * @param mod           module calling the extension
         * @param ei            info about external data to import
         */
        virtual void import(ModuleBase *mod, ImportExternDataInfo *ei);
	};


    /**
     * class for handling of command line option for external data
     */
    class ExternDataFileHandleOpt: public DataImportExtensionPar::InternalDataBase
    {
    protected:
        friend class ExternDataFile;

        DataImportBase::TxtWord *_filename;         ///< filename of data file / NULL: not specified
        DataImportBase::TxtWord *_namespace;        ///< namespace name / NULL: not specified

        bool _optWithColon;                         ///< command line option has implied colon
        bool _allSymbols;                           ///< import all symbols defined in the data file
        vector<DataImportBase::TxtWord*> _symbols;  ///< vector of info for external data symbols, each symbol with three entries (the second and third can be NULL)

        DataImportBase::ErrorInfo *_errorInfo;      ///< info about possible error / NULL: no error
        vector<LocationInfo*> _locations;           ///< locations used here
        
    public:
        /**
         * constructor
         * @param mod       module calling the extension
         * @param ext       extension
         * @param opt       command line option for external data
         * @param owc       command line option has implied colon
         */
        ExternDataFileHandleOpt(Precompiler *mod, ExtensionBase *ext, CmdLineOptList::SingleOption *opt, bool owc):
            DataImportExtensionPar::InternalDataBase(mod, ext, opt), _filename(NULL), _namespace(NULL), _optWithColon(owc), _allSymbols(false), _errorInfo(NULL)    { }

        /**
         * destructor
         */
        virtual ~ExternDataFileHandleOpt();

        /**
         * check command line option for external data
         * @return       priority of handling the given external data import by this extension
         */
        int checkData();

        /**
         * check command line option for external data
         * @param fnf   true: first arg is used as filename / false: scan also first arg for words
         * @param ocf   start of scanned args
         * @return       priority of handling the given external data import by this extension
         */
        int checkData(bool fnf, unsigned ocf);

        /**
         * run handling of command line option for external data import
         * @return      true if option is handled here
         */
        virtual bool run();

    private:
        /**
         * delete info
         */
        void deleteInfo();

        /**
         * get extension object
         */
        ExternDataFile *getExt()            { return (ExternDataFile *)_ext; }

        /**
         * process data header line for all symbols from a data file
         * @param dataFile          data file
         * @param nm                namespace name / -1: no namespace
         * @param loc				location of data header line
         * @param dataDir			path for data file
         * @param chgToDataDir		change dir to dataDir for opening the data file
         */
        void processHeaderDataAllSymbols(FileInput &dataFile, int nm, LocationInfo& loc, string& dataDir, bool chgToDataDir);

        /**
         * process data header line with individual symbols given
         * @param dataFile          data file
         * @param nm                namespace name / -1: no namespace
         * @param dataDir			path for data file
         */
        void processHeaderDataGivenSymbols(FileInput &dataFile, int nm, string& dataDir);

        /**
         * insert data symbol in info about extern data
         * @param nm 			namespace name / -1: no namespace
         * @param sym			symbol name
         * @param fn			file name of cmpl data file
         * @param loc			location of data symbol in input file
         * @param sp			position of symbol in the data file / -1: not known
         * @return				true if symbol is inserted to info
         */
        bool insertDataSymbol(int nm, string& sym, string& fn, LocationInfo& loc, long sp = -1);
    };


    /**
     * class for import and caching of external data values, read from a cmpl data file
     */
    class ExternDataFileImport
    {
        friend class ExternDataFile;

    private:
        /**
         * mode for reading values for one symbol
         */
        enum ValueMode {
            valueScalar,			///< scalar value
            valueSet,               ///< set (rank is given as mode parameter)
            valueArray,             ///< array of values (definition set is given as mode parameter)
            valueIndices,			///< array of values with given index tuple per element (set for allowed index tuples is given as mode parameter)
        };

    private:
        ModuleBase *_mod;               ///< calling module
        ExternDataFile *_parent;        ///< parent object

        int _filename;                  ///< name of the cmpl data file
        ExecContext *_ctx;              ///< calling execution context
        unsigned _syntaxElem;           ///< id of syntax element

        map<int, CmplVal> _values;		///< already read values: key denotes the symbol name

    private:
        /**
         * constructor
         * @param mod       calling module
         * @param edf       parent object
         * @param ei		info about external data to import
         */
        ExternDataFileImport(ModuleBase *mod, ExternDataFile *edf, ImportExternDataInfo *ei);

        /**
         * destructor
         */
        ~ExternDataFileImport()     { for (auto x : _values) { x.second.dispose(); } }

        /**
         * import data values
         * @param res		return values
         * @param ei		info about external data to import
         */
        void import(CmplVal& res, ExternDataInfo::EIRecord *ei);

        /**
         * read values for one symbol or for all symbols from cmpl data file
         * @param readAll	read values for all symbols from data file or only for one symbol
         * @param symname	only used if !readAll: name of the symbol
         * @param fpos		only used if !readAll: start position of values of the symbol in the data file
         */
        void readValues(bool readAll, int symname = 0, long fpos = 0);

        /**
         * read values for one symbol
         * @param res		return of read values
         * @param inStr		open input stream
         * @param line		current line in the input file
         * @param rec       recover mode, skip file up to next symbol start
         * @param checkSym	check that values for this symbol are read / NULL: any symbol
         * @return          symbol for which values are read / -1: end of file
         */
        int readSymbolValues(CmplVal& res, istream *inStr, unsigned& line, bool &rec, int *checkSym = NULL);

        /**
         * parse headline for symbol
         * @param mode		return mode of the values for the symbol
         * @param modePar	return parameter for the mode
         * @param valDef	return default value for sparse array
         * @param lstr		line from data file; return remaining part of the line
         * @param line		line number for error messages
         * @return          symbol
         */
        int parseSymbolHead(ValueMode& mode, CmplVal& modePar, CmplVal& valDef, string& lstr, unsigned line);

        /**
         * read scalar value from data file
         * @param res		return of read value
         * @param inStr		open input stream
         * @param line		current line in the input file
         * @param lstr		line to process before reading next line from file
         */
        void readValuesScalar(CmplVal& res, istream *inStr, unsigned& line, string& lstr);

        /**
         * read set value from data file
         * @param res		return of read value
         * @param inStr		open input stream
         * @param line		current line in the input file
         * @param lstr		line to process before reading next line from file
         * @param rank		rank of set to read
         */
        void readValuesSet(CmplVal& res, istream *inStr, unsigned& line, string& lstr, unsigned rank);

        /**
         * test whether string contains a algorithmic set and construct the set
         * @param s         input string
         * @param set       result set
         * @param line      current line in the input file
         * @return          true if algorithmic set is constructed
         */
        bool testConstructAlgSet(string s, CmplVal& set, unsigned& line);

        /**
         * read array of values from data file
         * @param res		return of read value
         * @param inStr		open input stream
         * @param line		current line in the input file
         * @param lstr		line to process before reading next line from file
         * @param defset	definition set for the array
         */
        void readValuesArray(CmplVal& res, istream *inStr, unsigned& line, string& lstr, CmplVal& defset);

        /**
         * read array of values from data file, with given index tuple per array element
         * @param res		return of read value
         * @param inStr		open input stream
         * @param line		current line in the input file
         * @param lstr		line to process before reading next line from file
         * @param set		set to which index tuples must belong
         * @param def       default value
         */
        void readValuesIndices(CmplVal& res, istream *inStr, unsigned& line, string& lstr, CmplVal& set, CmplVal& def);

        /**
         * read next line from data file
         * @param inStr		open input stream
         * @param line		current line in the input file
         * @param lstr		return of read line
         * @param errEof	throw exception if EOF
         * @param skipTo    if given then skip up to line starting with this char
         * @return          position of first non-space char in the line / string::npos if EOF
         */
        size_t getNextLine(istream *inStr, unsigned& line, string& lstr, bool errEof, char skipTo = '\0');

        /**
         * read one value (TP_INT, TP_REAL or TP_STR) from string
         * @param v         return value
         * @param lstr		source string
         * @param pos		start position, moved forward to next value or string::npos if no one
         * @param line		current line in the input file
         * @param noReal	don't accept value of type TP_REAL
         * @return          true if value is successfully read
         */
        bool readOneVal(CmplVal& v, const string& lstr, size_t& pos, unsigned line, bool noReal = false);

    };
}

#endif // EXTERNDATAFILE_HH

