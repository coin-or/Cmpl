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


#ifndef EXTERNDATAINFO_HH
#define EXTERNDATAINFO_HH

#include <string>
#include <map>
#include <set>
#include "../Control/MainData.hh"

using namespace std;


namespace cmpl
{
	/**
	 * <code>ExternDataInfo</code> contains information about extern data sources and symbols
	 */
	class ExternDataInfo : public MainData::DataBase
	{
    public:
		struct EIRecord
		{
            friend class ExternDataInfo;

        private:
            int _intsym;						///< index of cmpl symbol name (including namespace) in global string store
            int _extsym;						///< index of identification name in global string store of the symbol within the extern data source
            int _srctype;						///< type of extern data source
            int _srcname;						///< name of extern data source in global string store, e.g. file name
            bool _srcfile;                      ///< true if _srcname is a filename
            bool _optval;                       ///< true if extern data source can contain values with optimization variables
            int _addstr1;                       ///< additional info string #1, for arbitrary use / -1: not used
            int _addstr2;                       ///< additional info string #2, for arbitrary use / -1: not used
            long _srcpos;						///< position in extern data source if known / -1: not known

			/**
			 * constructor
			 */
            EIRecord(int ns, int e, int t, int sn, bool fn, bool ov, int a1, int a2, long p)		{ _intsym = ns; _extsym = e; _srctype = t; _srcname = sn; _srcfile = fn; _optval = ov; _addstr1 = a1, _addstr2 = a2; _srcpos = p; }

        public:
            /**
             * constructor
             */
            EIRecord()                                                                              { _intsym = _extsym = _srctype = _srcname = -1; _srcfile = false; _optval = false; _addstr1 = -1; _addstr2 = -1; _srcpos = -1; }

            /**
             * get index of cmpl symbol name (including namespace) in global string store
             */
            int intsym()           { return _intsym; }

            /**
             * get index of identification name in global string store of the symbol within the extern data source
             */
            int extsym()           { return _extsym; }

            /**
             * get type of extern data source
             */
            int srctype()           { return _srctype; }

            /**
             * get name of extern data source in global string store, e.g. file name
             */
            int srcname()           { return _srcname; }

            /**
             * get whether _srcname is a filename
             */
            bool srcfile()          { return _srcfile; }

            /**
             * get whether extern data source can contain values with optimization variables
             */
            bool optval()           { return _optval; }

            /**
             * get additional info string #1
             */
            int addstr1()           { return _addstr1; }

            /**
             * get additional info string #2
             */
            int addstr2()           { return _addstr2; }

            /**
             * get position in extern data source if known / -1: not known
             */
            int srcpos()           { return _srcpos; }
        };

        map<int, EIRecord*> _info;              ///< list of extern data source infos (key of map: EIRecord::intsym)


		/************** implementation for MainData::DataBase methods **********/

	public:
		/**
		 * constructor
		 * @param m         module name
		 * @param data      main data object
		 */
        ExternDataInfo(const char *m, MainData *data): MainData::DataBase(m, data)				{ }

        /**
         * destructor
         */
        virtual ~ExternDataInfo();

		/**
		 * delete all data
		 */
		virtual void cleanUp();

		/**
		 * get count of sub data objects
		 */
		virtual int subCnt();

		/**
		 * if sub data is splitted up to different parts for serialization then get count of these parts, otherwise 0
		 */
		virtual int subPartCount()								{ return 0; }

		/**
		 * check if serialization to or from stream should be done for the data object
		 * @param si		serialization info
		 * @param data		main data object
		 * @return			true if serialization should be done
		 */
		virtual bool checkSerialize(MainData::SerializeInfo& si, const MainData *data);

		/**
		 * write data object to output stream
		 * @param si		serialization info
		 * @param data		main data object
		 */
		virtual void serializeTo(MainData::SerializeInfo& si, const MainData *data);

		/**
		 * fill data object from input stream
		 * @param si		serialization info
		 * @param data		main data object
		 * @param pos		position of input string, returns iterated position
		 * @param subCnt	count of sub data objects
		 * @param lc		line count per element or 0 if elements are len coded
		 * @param rline		remaining section start line
		 */
		virtual void deserializeFrom(MainData::SerializeInfo& si, const MainData *data, int subCnt, int lc, string &rline);



		/************** input or output for info **********/

	public:
        /**
         * get info about an extern data symbol
         * @param nms		namespace (as index in global string store) / -1: no namespace
         * @param sym		symbol name (as index in global string store)
         * @return			info about an extern data symbol / NULL: not found
         */
        EIRecord *getExternInfo(int intsym);

        /**
		 * get info about an extern data symbol
		 * @param data		main data object
         * @param nmssym	symbol name including namespace
		 * @param tp		return type of extern data source
		 * @param sname		return name of extern data source (as index in global string store)
		 * @param sp		return position in extern data source if known / -1: not known
		 * @return			true if symbol is found
		 */
        bool getExternInfo(MainData *data, const char *nmssym, int& tp, int& sname, long& sp);

        /**
         * get info about an extern data symbol
         * @param data		main data object
         * @param nmssym	symbol name including namespace
         * @param tp		return type of extern data source
         * @param sname		return name of extern data source (as index in global string store)
         * @param file      return whether sname is a file name
         * @param optval    return whether extern data source can contain values with optimization variables
         * @param a1        return of additional info string #1 (as index in global string store) / -1: not used
         * @param a2        return of additional info string #2 (as index in global string store) / -1: not used
         * @param sp		return position in extern data source if known / -1: not known
         * @return			true if symbol is found
         */
        bool getExternInfo(MainData *data, const char *nmssym, int& tp, int& sname, bool &file, bool &optval, int& a1, int& a2, long& sp);

        /**
         * insert new info record about an extern data symbol
         * @param data		main data object
         * @param intsym	symbol name including namespace
         * @param extsym	symbol name within extern data source
         * @param tp		type of extern data source
         * @param sname		name of extern data source
         * @param file      true if sname is a file name
         * @param optval    true if extern data source can contain values with optimization variables
         * @param as1       additional info string #1 / NULL: not used
         * @param as2       additional info string #2 / NULL: not used
         * @param sp		position in extern data source or -1
         * @return			true if inserted / false if already contained / exception if contained with other source
         */
        bool insertExternInfo(MainData *data, const char *intsym, const char *extsym, int tp, const char *sname, bool file, bool optval = false, const char *as1 = NULL, const char *as2 = NULL, long sp = -1);

		/**
		 * write to a stream for output
		 * @param ostr		output stream
		 * @param data		main data object
		 */
		void writeTo(ostream& ostr, MainData *data);

		/**
		 * read from a stream for input
		 * @param istr		input stream
		 * @param data		main data object
		 */
		void readFrom(istream& istr, MainData *data);

		/**
         * get set with names of data files
		 * @param data		main data object
		 * @param filenames	set to fill file names in
		 */
        void getDataFileNames(MainData *data, set<const char *> &filenames);
	};


	/**
	 * <code>ExternDataInfoInconsistentException</code> is thrown if inconsistent information about an extern data symbol should be inserted
	 */
    class ExternDataInfoInconsistentException : public exception        //TODO: besser von runtime_error ableiten? oder direkt verwenden?
	{
		virtual const char *what() throw()		{ return "inconsistent info"; }
	};

}

#endif // EXTERNDATAINFO_HH

