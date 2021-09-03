
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


#ifndef MAINDATA_HH
#define MAINDATA_HH

#include <string>
#include <list>
#include <vector>
#include <map>
#include <set>
#include <sstream>

#include "StringStore.hh"
#include "LocationInfo.hh"


using namespace std;


namespace cmpl
{
	class MainControl;


	/**
	 * <code>MainData</code> contains data which is common for all modules.
	 */
	class MainData
	{
		friend class MainControl;
		friend class ErrorHandler;

	public:
		/**
		 * info for serialization of data or for reading of serialized data
		 */
		struct SerializeInfo
		{
			ModuleBase *modp;						///< module calling the serialization

			ostream *ostr;							///< output stream to write to (only for serialization of data)

			istream *istr;							///< input stream to read from (only for reading of serialized data)
			PositionInfo *pos;						///< current position in input stream (only for reading of serialized data)

			int version;							///< version number of the serialization format
			unsigned instance;						///< string number for name of cmpl instance which create the serialized data
			bool lineNums;							///< serialized data has line numbers

			int toFromServer;						///< serialization is used to transfer data between client and server (0: no, 1: from client to server, -1: from server to client)
			int serverStep;							///< only for toFromServer!=0: module order step of transfer of data to/from server

			bool ioCmdLineOpts;						///< input/output also of the command line options
			bool ioModules;							///< input/output also the names of the modules to execute
			bool ioExtensions;						///< input/output also extension classes

			int ioDataDefault;						///< default for serialization of data classes: -1: none / 0: call checkSerialize() / 1: all
			set<string> *ioDataClasses;				///< names of data classes to serialize
			set<string> *ioDataNoClasses;			///< names of data classes not to serialize
			set<string> *ioDataModules;				///< names of modules whose data classes to serialize
			set<string> *ioDataNoModules;			///< names of modules whose data classes not to serialize

			unsigned *mapStrings;					///< array with string numbers in resulting global string store (only for reading of serialized data)

            map<const LocationInfo *, unsigned> mapLocsTo;	///< mapping of location pointers to location number for the serialized data
            vector<LocationInfo *> mapLocsFrom;     ///< mapping of location numbers to location pointer from the serialized data

			/**
			 * constructor for serialization of data
			 */
            SerializeInfo(ModuleBase *m, ostream *o, int v, unsigned i, bool ln)			{ modp = m; ostr = o; istr = NULL; pos = NULL; version = v; instance = i; lineNums = ln; toFromServer = 0; ioCmdLineOpts = ioModules = ioExtensions = false; ioDataDefault = 0; ioDataClasses = ioDataNoClasses = ioDataModules = ioDataNoModules = NULL; mapStrings = NULL; }

			/**
			 * constructor for reading of serialized data
			 */
            SerializeInfo(ModuleBase *m, istream *i, PositionInfo *p)						{ modp = m; ostr = NULL; istr = i; pos = p; version = -1; toFromServer = 0; ioCmdLineOpts = ioModules = ioExtensions = false; ioDataDefault = 0; ioDataClasses = ioDataNoClasses = ioDataModules = ioDataNoModules = NULL; mapStrings = NULL; }

			/**
			 * destructor
			 */
            ~SerializeInfo()																{ if (mapStrings) { delete mapStrings; } }

			/**
			 * set serialization info for transfer data between client and server
			 */
			void setIoServer(int tfs, int sst)												{ toFromServer = tfs; serverStep = sst; }

			/**
			 * set serialization info for properties of the main control
			 */
			void setIoControl(bool clo, bool mod, bool ext)									{ ioCmdLineOpts = clo; ioModules = mod; ioExtensions = ext; }

			/**
			 * set serialization info for data classes
			 */
			void setIoData(int dd, set<string> *dc, set<string> *dnc, set<string> *dm, set<string> *dnm)	{ ioDataDefault = dd; ioDataClasses = dc; ioDataNoClasses = dnc; ioDataModules = dm; ioDataNoModules = dnm; }
		};


		/**
		 * abstract base class for special cmpl data classes
		 */
		class DataBase
		{
		protected:
			const char *_modName;					///< name (or alias name) of the module constructing this data object

			/**
			 * constructor
			 * @param m			module name
			 * @param data		main data object
			 */
			DataBase(const char *m, MainData *data);

		public:
            /**
             * destructor
             */
            virtual ~DataBase()							{ }

            /**
			 * get name of the module constructing this data object
			 */
			inline const char *modName() const			{ return _modName; }

			/**
			 * delete all data
			 */
			virtual void cleanUp() = 0;

			/**
			 * get count of sub data objects
			 */
			virtual int subCnt() = 0;

			/**
			 * if sub data is splitted up to different parts for serialization then get count of these parts, otherwise 0
			 */
			virtual int subPartCount() = 0;

			/**
			 * get serialization lines per data element, or 0 if data elements can have multiline contents for which a len coded serialization is necessary
			 */
			virtual int linesPerData()					{ return 1; }

			/**
			 * check if serialization to or from stream should be done for the data object
			 * @param si		serialization info
			 * @param data		main data object
			 * @return			true if serialization should be done
			 */
			virtual bool checkSerialize(SerializeInfo& si, const MainData *data) = 0;

			/**
			 * write data object to output stream
			 * @param si		serialization info
			 * @param data		main data object
			 */
			virtual void serializeTo(SerializeInfo& si, const MainData *data) = 0;

			/**
			 * fill data object from input stream
			 * @param si		serialization info
			 * @param data		main data object
			 * @param subCnt    count of sub data objects
			 * @param lc		line count per element or 0 if elements are len coded
			 * @param rline		remaining section start line
			 */
			virtual void deserializeFrom(SerializeInfo& si, const MainData *data, int subCnt, int lc, string &rline) = 0;
		};


		/************** common **********/

	protected:
		StringStore *_globStrings;					///< global string store for all strings used here in MainData

        //vector<LocationInfo *> *_globLocs;			///< vector of all locations used here in MainData (this vector owns the objects pointing to)
                                                        //TODO: ist der vector wirklich owner der LocationInfo-Objekte? Wenn ja, fehlt Freigabe bei cleanUp()
                                                        //          wer sollte sonst owner sein? (vielleicht SyntaxElement-Objekte, aber wer ist dann owner der uebergeordneten Locations?)
        map<unsigned long, vector<LocationInfo *>> *_globLocs;  ///< all locations used here in MainData, organized by LocationInfo::key()

		const char *_cmplFileBase;					///< base name for deriving default file names, it is the filename without extension of the first cmpl input file

	public:
		/**
		 * constructor
		 */
		MainData();

		/**
		 * destructor
		 */
		~MainData();

		/**
		 * delete all data
		 * @param delData		delete subdata
		 */
		void cleanUp(bool delData);

		/**
		 * delete all data
		 */
		inline void cleanUp()		{ cleanUp(true); }

		/**
		 * fills this as a shallow copy from another data object
		 * @param src		source data object
		 */
		void copyShallowFrom(MainData& src);

		/**
		 * set base name for deriving default file names
		 */
		inline void setFileBaseBase(string s)				{ _cmplFileBase = _globStrings->store(s); }

		/**
		 * returns a filename without extension, from which other file names can be derived 
		 * @return		base filename
		 */
		inline const char *cmplFileBase() const				{ return (_cmplFileBase ?: "{}"); }

		/**
		 * get global string store
		 */
		inline StringStore *globStrings() const				{ return _globStrings; }

		/**
         * get all locations used here in MainData
		 */
        inline map<unsigned long, vector<LocationInfo *>> *globLocs() const		{ return _globLocs; }

		/**
		 * makes an aequivalent persistent location, i.e. a location that refers nothing outside of this
		 * @param loc		given location
		 * @return			aequivalent persistent location
		 */
		LocationInfo persistentLocation(const LocationInfo& loc);

		/**
		 * change position to a persistent position, i.e. a position that refers nothing outside of this
		 * @param pos		given position
		 */
		void persistentPosition(PositionInfo& pos);

		/**
		 * search aequivalent location in _globLocs and insert it if not found
		 * @param loc		given location
		 * @return			aequivalent location in _globLocs
		 */
		LocationInfo* searchInsertInGlobLocs(const LocationInfo *loc);

		/**
		 * search aequivalent location in _globLocs and return number of it or -1 if not found
         * @param si		serialization info (si.mapLocsTo must be filled)
         * @param loc		given location
		 */
        int searchLoc(SerializeInfo &si, const LocationInfo *loc) const;


		/************** executed modules **********/

	public:
		struct ExecutedModule
		{
			const char *module;						///< name of module in which the error occured
			const char *orgMod;						///< original module name if module is an alias name (otherwise NULL)
			const char *instance;					///< name of the cmpl instance which has executed the module
			bool finished;							///< true if execution of the module is finished

			/**
			 * constructor
			 */
			inline ExecutedModule(const char *m, const char *o, const char *i, bool f = false): module(m), orgMod(o), instance(i), finished(f)	{ }
		};

	protected:
		list<ExecutedModule> *_executedModules;		///< list of names of the already executed modules

		/**
		 * add name of the already executed modules
		 * @param m			name of module
		 * @param o			original module name if module is an alias name (otherwise NULL)
		 * @param i			name of the cmpl instance which has executed the module
		 */
		void addExecutedModules(const char *m, const char *o, const char *i);

		/**
		 * set execution of a module as finished
		 * @param m			name of module
		 * @param i			name of the cmpl instance which has executed the module
		 */
		void setExecutedModuleFinished(const char *m, const char *i);

	public:
		/**
		 * get list of names of the already executed modules
		 */
		list<ExecutedModule> *executedModules() const		{ return _executedModules; }



		/************** error handling **********/
	
	public:
		/**
		 * struct for one error entry
		 */
		struct ErrorEntry
		{
			int lvl;								///< error level (one of ERROR_LVL_*)
			bool cont;								///< entry continues the previous entry
			const char *msg;						///< error message
			const char *exc;						///< text of original exception / NULL: no exception
			const char *module;						///< name of module in which the error occured
			const char *step;						///< name of the exec step within the module where the error occured
			LocationInfo *loc;						///< location of the error

			/**
			 * constructor
			 */
			inline ErrorEntry(int l, bool c, const char *m, const char *e, const char *mo, const char *s, LocationInfo *lo): lvl(l), cont(c), msg(m), exc(e), module(mo), step(s), loc(lo)	{ }
		};

	protected:
		list<ErrorEntry> *_errorList;				///< list of all errors and warnings
		//list<LocationInfo*> *_inLocList;			///< list of all locations which are used as inLoc within locations in _errorList
		int _errorLevelMax;							///< max level of occured errors or warnings / 0: no error or warning
		int _errorCnt;								///< error count
		int _warnCnt;								///< warning count

		/**
		 * clear error list
		 */
		void resetErrors();

		/**
		 * add an error entry.
		 * @param lvl		error level (one of ERROR_LVL_*)
		 * @param cont		entry continues the previous entry
		 * @param msg		error message
		 * @param exc		original exception / NULL: no exception
		 * @param modul		name of module in which the error occured
		 * @param step		name of the exec step within the module where the error occured
		 * @param loc		location of the error
		 */
		void addError(int lvl, bool cont, const char *msg, exception *exc, const char *modul, const char *step, const LocationInfo& loc);

		/**
		 * get list of all errors and warnings
		 */
		const list<ErrorEntry> *errorList() const		{ return _errorList; }

		/**
		 * get max level of occured errors or warnings
		 */
		int errorLevelMax() const						{ return _errorLevelMax; }

		/**
		 * set max level of occured errors or warnings
		 */
		void setErrorLevelMax(int l)					{ _errorLevelMax = l; }

		/**
		 * get error count
		 */
		int errorCnt() const							{ return _errorCnt; }

		/**
		 * get warning count
		 */
		int warnCnt() const								{ return _warnCnt; }



		/************** cmpl data **********/

	protected:
		map<string, DataBase *> _data;			///< cmpl generated data

	public:
		/**
		 * set cmpl data
		 * @param n			name of data class
		 * @param d			object with data
		 * @param del		delete previous data object if exists
		 */
		void setData(string n, DataBase *d, bool del = true);

		/**
		 * get cmpl data
		 * @param n			name of data class
		 * @return			data object or NULL if not set
		 */
		DataBase *data(string n);

		/**
		 * return whether cmpl data is set
		 * @param n			name of data class
		 * @return			true if the data object is set
		 */
		bool hasData(string n) const;

		/**
		 * get map with all data objects
		 */
		map<string, DataBase *>& dataMap()				{ return _data; }



		/************** serialization to stream **********/

	public:
		/**
		 * write data object to output stream
		 * @param si		serialization info
		 */
		void serializeTo(SerializeInfo& si);

		/**
		 * get version number of the format of serialization file
		 */
		inline int version() const										{ return 1; }

	private:
		/**
		 * write common data to output stream
		 * @param si		serialization info
		 */
		void serializeCommonTo(SerializeInfo& si);

		/**
		 * write command line options to output stream
		 * @param si		serialization info
		 */
		void serializeCmdLineOptsTo(SerializeInfo& si);

		/**
		 * write modules and extensions to output stream
		 * @param si		serialization info
		 */
		void serializeCodePartsTo(SerializeInfo& si);

		/**
		 * write data from data object to output stream
		 * @param si		serialization info
		 */
		void serializeDataTo(SerializeInfo& si);

		/**
		 * check whether a data object has to be serialized
		 * @param si		serialization info
		 * @param d			data object
		 * @param nc		name of data object
		 * @return			true if data object has to be serialized
		 */
		bool checkDataSerialize(SerializeInfo& si, DataBase *d, const string& nc);


		/************** deserialization from stream **********/

	public:
		/**
		 * fill data object from input stream
		 * @param si		serialization info
		 */
		void deserializeFrom(SerializeInfo& si);

	private:
		/**
		 * read common data from input stream
		 * @param si		serialization info
		 * @param cnt		count of elements within the section
		 */
		void readSectionCommon(SerializeInfo& si, int cnt);

		/**
		 * read data lines of Strings section from input stream
		 * @param si		serialization info
		 * @param cnt		count of elements within the section
		 */
		void readStringsData(SerializeInfo& si, int cnt);

		/**
		 * read data lines of Locations section from input stream
		 * @param si		serialization info
		 * @param cnt		count of elements within the section
		 */
		void readLocationsData(SerializeInfo& si, int cnt);

		/**
		 * read section for MainControl data from input stream
		 * @param si		serialization info
		 * @param cnt		count of elements within the section
		 */
		void readSectionMainControl(SerializeInfo& si, int cnt);

		/**
		 * read common data from input stream
		 * @param si		serialization info
		 * @param cnt		count of elements within the section
		 */
		void readSectionCmdLineOpts(SerializeInfo& si, int cnt);

		/**
		 * read common data from input stream
		 * @param si		serialization info
		 * @param cnt		count of elements within the section
		 */
		void readSectionCodeParts(SerializeInfo& si, int cnt);

		/**
		 * read common data from input stream
		 * @param si		serialization info
		 * @param cnt		count of elements within the section
		 */
		void readSectionData(SerializeInfo& si, int cnt);



		/************** static utility functions for serialization **********/

	public:
		/**
		 * search an element within a list
		 * @param lst		list
		 * @param elem		element to search in the list
		 * @return			index of the element or -1 if not found
		 */
        template <class T, class CT = const T> static int searchList(list<T>& lst, CT& elem)		{ int i=0; for (typename list<T>::iterator it=lst.begin(); it!=lst.end(); it++, i++) { if (*it == elem) return i; } return -1; }

		/**
		 * search an element within a vector
		 * @param lst		vector
		 * @param elem		element to search in the vector
		 * @return			index of the element or -1 if not found
		 */
        template <class T, class CT = const T> static int searchVector(vector<T>& lst, CT& elem)	{ int i=0; for (typename vector<T>::iterator it=lst.begin(); it!=lst.end(); it++, i++) { if (*it == elem) return i; } return -1; }

		/**
		 * search an element within a vector of pointers
		 * @param lst		vector
		 * @param elem		element to search in the vector
		 * @return			index of the element or -1 if not found
		 */
        template <class T, class CT = const T> static int searchVectorPtr(vector<T*>& lst, CT& elem)	{ int i=0; for (typename vector<T*>::iterator it=lst.begin(); it!=lst.end(); it++, i++) { if (**it == elem) return i; } return -1; }

		/**
		 * get an element within a list
		 * @param lst		list
		 * @param elem		index of the element in the list
		 * @return			element
		 */
		//template <typename T> static T& elemList(list<T> lst, int i)		{ typename list<T>::iterator it = lst.begin(); for (int j=0; j<i; j++, it++); return *it; }

		/**
		 * read start line of a section
		 * @param si		serialization info
		 * @param inLevel	level of expected section
		 * @param outLevel	return level of found section
		 * @param cnt		return count of elements within the section
		 * @param lc		return line count per element or 0 if elements are len coded
		 * @param rline		return remaining section start line
		 * @return			name of found section
		 */
		static string readSectionStart(SerializeInfo& si, int inLevel, int& outLevel, int& cnt, int& lc, string &rline);

		/**
		 * skip section with its data
		 * @param si		serialization info
		 * @param level		level of section
		 * @param cnt		count of elements within the section
		 * @param lc		line count per element or 0 if elements are len coded
		 */
		static void skipSection(SerializeInfo& si, int level, int cnt, int lc);

		/**
		 * get int value from string, and set string and position to the next possible value.
		 * @param s         input string, returns string without parsed value
		 * @param pos       position of input string, returns iterated position
		 * @return          parsed value
		 */
		static long getNextLong(string &s, PositionInfo *pos);

        /**
         * get double value from string, and set string and position to the next possible value.
         * @param s			input string, returns string without parsed value
         * @param pos		position of input string, returns iterated position
         * @return			parsed value
         */
        static double getNextDouble(string &s, PositionInfo *pos);

		/**
		 * read len coded data line from serialized data
		 * @param si		serialization info
		 * @param buf		buffer to return data
		 * @param cap		current capacity of buf
		 * @return			length of data
		 */
		static size_t readLenCodedDataLine(SerializeInfo& si, char **buf, size_t& cap);
	};


	/**
	 * <code>MainDataInputException</code> is thrown if an error ocurred while filling the <code>MainData</code> object from a stream
	 */
	class MainDataInputException : public exception
	{
	private:
		string _what;

	public:
		/**
		* constructor
		* @param msg			exception message
		* @param pos			position where the error ocurred
		*/
		MainDataInputException(const char *msg, const PositionInfo &pos)	{ cerr << "Exc" << endl; ostringstream o; o << msg << "; at " << pos; _what = o.str(); cerr << "  " << what() << endl; }

		/**
		* destructor
		*/
        virtual ~MainDataInputException() noexcept	{ }

		/**
		* get exception info
		*/
        virtual const char *what() const noexcept			{ return _what.c_str(); }
	};



	/*********** defines for data definitions **********/
	// declaration of construct function for the data object, for use in the following two makros. n is the name of the data class
	#define DATA_CLASS_DECL(n)		MainData::DataBase *constructData_ ## n(const char *m, MainData *data)

	// register a data class, only for use in file "CommonData/data.reg". n is the name of the data class
	#define DATA_CLASS_REG(n)		else if (dataName.compare(#n) == 0) { DATA_CLASS_DECL(n); dataObj = constructData_ ## n(modName, data); }

	// define a data class, for use in the .cc file of the data class. n is the name of the data class
	#define DATA_CLASS(n)			DATA_CLASS_DECL(n) { n *d = new n(m, data); return d; }


	// check if global data object has certain cmpl data. g is global data object, n is the name of the data class
	#define HAS_DATA(g,n)			((g)->hasData(string(#n)))

	// get cmpl data from global data object. n is the name of the data class, d is the data variable to define
	#define GET_DATA(n,d)			n *d = (n *)(data()->data(string(#n)))

	// set cmpl data to global data object. n is the name of the data class, d is the data object
	#define SET_DATA(n,d)			data()->setData(string(#n), d)

	// get cmpl data from global data object or create new if not exists. n is the name of the data class, d is the data variable to define
	#define GET_NEW_DATA(n,d)		n *d = (n *)(data()->data(string(#n))); if (!d) { d = new n(moduleName(), data()); data()->setData(string(#n), d); }

	// create new cmpl data object and set it to global data object. n is the name of the data class, d is the data variable to define
	#define SET_NEW_DATA(n,d)		n *d = new n(moduleName(), data()); data()->setData(string(#n), d)

}
#endif // MAINDATA_HH

