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


#ifndef FILEBASE_HH
#define FILEBASE_HH

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <map>

#include "LocationInfo.hh"


using namespace std;


namespace cmpl
{
	class MainData;


// possible values for FileBase::_mode
#define IO_MODE_STD_IO				1					///< output to cout or input from stdin
#define IO_MODE_STD_CERR			2					///< output to cerr (only for output files)
#define IO_MODE_STRING				3					///< output in the string or input from the string FileBase::_ioString
#define IO_MODE_FILE				4					///< output in the file or input from the file named FileBase::_fileName

// values for standard file names ("{}" is replaced by the base file name)
#define IO_FILE_STANDARD_CMPL		"{}.cmpl"			///< cmpl input file
#define IO_FILE_STANDARD_CDAT		"{}.cdat"			///< cmpl data file
#define IO_FILE_STANDARD_XDAT		"{}.xdat"			///< cmpl data file for excel data
#define IO_FILE_STANDARD_OPRE		"{}.precmpl"		///< output file with precompiler result
#define IO_FILE_STANDARD_EXTDATA	"{}.extdata"		///< output file with extern data info
#define IO_FILE_STANDARD_OOPT       "{}.opt"            ///< output file with command line options
#define IO_FILE_STANDARD_MPS		"{}.mps"			///< model output mps or free mps file
#define IO_FILE_STANDARD_OSIL		"{}.osil"			///< model output osil file
#define IO_FILE_STANDARD_MATRIX		"{}.mat"			///< model output  matrix file
#define IO_FILE_STANDARD_ERROR		"{}.err"			///< error file
#define IO_FILE_CMPLMSG     		"{}.cmsg"			///< cmplMsg file
#define IO_FILE_STANDARD_PROTO		"{}.proto"			///< protocol file
#define IO_FILE_STANDARD_SYNTAX		"{}.syntax"			///< syntax info file
#define IO_FILE_SOLUTION_ASCII		"{}.sol"			///< std solution file
#define IO_FILE_SOLUTION_SOLCSV		"{}.csv"		///< csv solution file
#define IO_FILE_SOLUTION_SOLXML     "{}.csol"			///< CmplSolution file
//TODO: other file types


    /**
     * <code>FileBase</code> is the base class for the handling of the several input or output files
     */
    class FileBase
    {

    protected:
        bool _enabled;					///< enable or disable this input or output
        int _mode;						///< mode for input or output
        string _fileName;				///< file name (pseudo name for input from string)
        string _ioString;				///< result string of output or input string (only used for _mode == IO_MODE_STRING)

		LocationInfo _locSrc;			///< location of file name or input string
		MainData *_data;				///< MainData object, only needed for file name replacement "{}"
		map<string,string> *_aliases;	///< aliases for file names / NULL: no aliases set
		void *_extra;					///< pointer for arbitrary data


        /**
         * constructor
         */
        FileBase()										{ setDefaults(); }

		/**
		 * copy constructor
		 */
		FileBase(FileBase& file)						{ _enabled = file._enabled; _mode = file._mode; _fileName = file._fileName; _data = file._data; _locSrc = file._locSrc; _aliases = file._aliases; }


    public:

        /**
         * Returns true|false if the file is an active file
         *
         * @return  true|false
         */
        inline bool enabled()							{ return _enabled; }

        /**
         * Sets the file either as an active file or not
         * @param o
         */
        inline void setEnabled(bool o)					{  _enabled = o; }

        /**
         * Returns the output or input mode of the file
		 * @return  one of IO_MODE_*
         */
        inline int mode()								{ return _mode; }

        /**
         * Sets the output mode of the file
         * @param mode  one of IO_MODE_*
         */
        inline void setMode(int mode)					{ _mode = mode; }

        /**
         * Returns the file name
         * @return  file name
         */
        inline string& fileName()						{ return _fileName; }

		/**
		 * Returns a file name, with replacement of "{}" by the base file name, and with replacement of file aliases
		 * @param src	source file name
         * @return  file name
		 */
		string fileNameReplaced(string& src);

		/**
		 * Returns the file name, with replacement of "{}" by the base file name, and with replacement of file aliases
         * @return  file name
		 */
		inline string fileNameReplaced()				{ return fileNameReplaced(_fileName); }

        /**
         * Sets the file name
         * @param  fname
         */
        inline void setFileName(string& fname)			{ _fileName = fname; }

        /**
         * Returns the a string if IO_MODE_STRING is chosen as format
         * @return  string
         */
        inline string& ioString()						{ return _ioString; }

        /**
         * sets a string for IO_MODE_STRING
         * @param  str
         */
        inline void setIoString(string& str)			{ _ioString = str; }

        /**
         * sets file defaults
         */
        inline void setDefaults()						{ _enabled = false; _mode = 0; _data = NULL; _aliases = NULL; }

		/**
		 * set location of file name or input string
		 */
		inline void setLocSrc(LocationInfo& l)			{ _locSrc = l; }

		/**
		 * set location of file name or input string
		 */
		inline void setLocSrc(PositionInfo& p)			{ _locSrc = LocationInfo(p); }

		/**
		 * get location of file name or input string
		 */
		inline LocationInfo& locSrc()					{ return _locSrc; }

		/**
		 * set data object
		 */
		inline void setData(MainData *d)				{ _data = d; }

		/**
		 * get data object
		 */
		inline MainData *data()							{ return _data; }

		/**
		 * set aliases for file names
		 */
		inline void setAliases(map<string,string> *a)	{ _aliases = a; }

		/**
		 * get aliases for file names
		 */
		inline map<string,string>* aliases()			{ return _aliases; }

		/**
		 * set arbitrary data pointer
		 */
		inline void setExtra(void *p)					{ _extra = p; }

		/**
		 * get arbitrary data pointer
		 */
		inline void *extra()							{ return _extra; }

		/**
		 * get whether the name has an absolute dir path
		 * @param ps			path separator char
		 * @return				true if absolute dir path
		 */
		bool hasNameAbsPath(char ps);

		/**
		 * creates new position for this file
		 * @param name			file name
		 * @param loc			source location
		 * @return				new position object
		 */
		PositionInfo* newPositionInfo(const char *name, LocationInfo *loc);

		/**
		 * creates new position for this file
		 * @param name			file name
		 * @param loc			source location
		 * @return				new position object
		 */
		PositionInfo* newPositionInfo(string name, LocationInfo *loc);

        /**
         * get path separator char
         * @param path          path
         * @param psDef         default path separator char
         * @return              path separator char
         */
        static char getDirSepChar(const char *path, const char psDef = '/');

        /**
         * replace last component of path by another file name
         * @param path          path
         * @param file          other file name
         * @param ps            path separator char
         * @return              replaced file name
         */
        static string replFileInPath(string *path, string *file, char ps);


        static string getTmpPath();

        static string getTmpFileName(const string& startStr, const int maxNr ) ;

        static bool exists(const string& fileName);
    };

    /**
     * <code>FileOutput</code> handles output files
     */
    class FileOutput: public FileBase
    {
	private:
		ostream *_oOut;					///< output stream

	public:
        /**
         * constructor
         */
        FileOutput(): FileBase()							{ _oOut = NULL; }

		/**
		 * copy constructor
		 */
		FileOutput(FileOutput& file): FileBase(file)	{ _oOut = NULL; }

        /**
         * destructor
         */
        ~FileOutput()										{ if (_oOut) close(); }

		/**
		 * set file for output
		 * @param data			data object to derive filenames
		 * @param mode			mode
		 * @param fn			pointer to filename
		 * @param std			pointer to standard filename to use if fn is empty
		 * @param overwrite		if already set, then overwrite it
		 * @return				file was already set
		 */
		bool setFile(MainData *data, int mode, const string *fn, const char *std, bool overwrite = false);

        /**
         * opens the output file
		 * @return		stream for writing to the file
         */
        //ostream* open();
        ostream* open(ios_base::openmode mode = ios_base::out);

        /**
         * flush the output
         */
		inline void flush()				{ if (_oOut) _oOut->flush(); }

        /**
         * close the file
         */
        void close();
	};

    /**
     * <code>FileInput</code> handles input files
     */
    class FileInput: public FileBase
    {
	private:
		istream *_iIn;					///< input stream 
		FILE *_iFp;						///< input file (alternative to <code>_iIn</code>)

	public:
        /**
         * constructor
         */
        FileInput(): FileBase()								{ _iIn = NULL; _iFp = NULL; }

		/**
		 * copy constructor
		 */
        FileInput(FileInput& file): FileBase(file)          { _iIn = NULL; _iFp = NULL; }

        /**
         * destructor
         */
        ~FileInput()										{ if (_iIn || _iFp) close(); }

		/**
		 * set file for input
		 * @param data			data object to derive filenames
		 * @param mode			mode
		 * @param fn			pointer to filename or NULL if output filename is to derive from input filename
		 * @param std			pointer to standard filename to use if fn is empty
		 * @param overwrite		if already set, then overwrite it
		 * @return				file was already set
		 */
		bool setFile(MainData *data, int mode, const string *fn, const char *std, bool overwrite = false);

		/**
		 * set string for input
		 * @param data			data object to derive filenames
		 * @param fn			pseudo filename
		 * @param input			input data
		 * @param overwrite		if already set, then overwrite it
		 * @return				whether if error (filename is already set and not overwrite)
		 */
		bool setInputString(MainData *data, const string *fn, const string *input, bool overwrite = false);

        /**
         * opens the input file
		 * @return		stream for reading from the file
         */
        istream* open();

		/**
		 * open and get the output file or string
		 * @param input			return string with input data, or NULL if input file
		 * @return				file pointer to opened input file, or NULL if input string data			
		 */
		FILE* openFile(string **input);

        /**
         * close the file
         */
        void close();
	};


    /**
     * <code>FileException</code> is thrown if an error ocurred by working with a file
     */
    class FileException : public exception
    {
	private:
		string _what;
		string _filename;
        unsigned _line;

	public:
		/**
		 * constructor
		 * @param msg				exception message
		 * @param filename			name of file
		 * @param orgExc            what() from original exception
		 */
		FileException(const char *msg, const char *filename, const char *orgExc = NULL);

        /**
         * constructor
         * @param msg				exception message
         * @param filename			name of file
         * @param line              line number within the file
         * @param orgExc            what() from original exception
         */
        FileException(const char *msg, const char *filename, unsigned line, const char *orgExc = NULL): FileException(msg, filename, orgExc)   { _line = line; }

        /**
		 * destructor
		 */
        virtual ~FileException() noexcept			{ }

		/**
		 * get exception info
		 */
        virtual const char *what() const noexcept	{ return _what.c_str(); }

		/**
		 * get filename
		 */
        const string& filename() const noexcept		{ return _filename; }

        /**
         * get line number
         */
        unsigned line() const noexcept              { return _line; }
    };
}

#endif // FILEBASE_HH
