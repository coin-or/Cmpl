
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


#ifndef DATAIMPORTBASE_HH
#define DATAIMPORTBASE_HH


#include "../../Control/ExtensionBase.hh"
#include "../../CommonData/ExtensionSteps.hh"


using namespace std;



namespace cmpl
{
    class Precompiler;
    class FileInput;
    struct ImportExternDataInfo;


    /**
	 * the <code>DataImportBase</code> class is the base class for extensions
	 * used to import data for cmpl from an external data source.
	 */
	class DataImportBase : public ExtensionBase
	{
    protected:
        int _extSrcType;            ///< type of external data source handled by this extension class
        int _handlePrio;            ///< priority of handling with this extension if more than one extension can handle it

	public:
		/**
		 * constructor
		 * @param mod			module creating this extension object
         * @param est           type of external data source handled by this extension class
         * @param hp            priority of handling with this extension if more than one extension can handle it
		 */
        DataImportBase(ModuleBase *mod, int est, int hp): ExtensionBase(mod), _extSrcType(est), _handlePrio(hp)  { }

        /**
         * get type of external data source handled by this extension class
         */
        int extSrcType()            { return _extSrcType; }

        /**
         * get priority of handling with this extension
         */
        int handlePrio()            { return _handlePrio; }


		/************** overwritten methods of <code>ExtensionBase</code> **********/

	protected:
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
		/**
		 * get count of sub data objects
		 */
		virtual int subCnt()								{ return 0; }
										   
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


        /************** import of data **********/
    protected:
        /**
         * import data values
         * @param mod       module calling the extension
         * @param ei		info about external data to import
         */
        virtual void import(ModuleBase *mod, ImportExternDataInfo *ei) = 0;



        /************** utility types and functions for use in derived classes **********/
    public:
        /**
         * store for info about possible error
         */
        struct ErrorInfo {
            int level;                  ///< error level (one of ERROR_LVL_*)
            string msg;                 ///< error message
            LocationInfo *locInfo;      ///< error location

            /**
             * constructor
             * @param lvl   one of ERROR_LVL_*
             * @param txt   error message
             * @param loc   error location
             */
            ErrorInfo(int lvl, const char *txt, LocationInfo *loc): level(lvl), msg(txt), locInfo(loc)      { }
        };

        /**
         * get new error info object
         * @param lvl       error level (one of ERROR_LVL_*)
         * @param txt       error message
         * @param loc       location info of the error location
         * @param locs      vector to hold location objects
         * @return          new error info object
         */
        static ErrorInfo *newErrorInfo(int lvl, const char *txt, LocationInfo& loc, vector<LocationInfo*>& locs);

        /**
         * get new error info object
         * @param lvl       error level (one of ERROR_LVL_*)
         * @param txt       error message
         * @param pos       position info of the start of error location
         * @param len       length of error location
         * @param locs      vector to hold location objects
         * @return          new error info object
         */
        static ErrorInfo *newErrorInfo(int lvl, const char *txt, PositionInfo& pos, size_t len, vector<LocationInfo*>& locs);

        /**
         * word scanned from a line of text
         */
        struct TxtWord {
            string word;                ///< word
            bool quoted;                ///< word was written with quotes
            size_t len;                 ///< length of the word
            PositionInfo pos;           ///< position info of the start of the word

            string openBr;              ///< open brackets
            bool openQt;                ///< open quote
            TxtWord *prv;               ///< previous word, if this is a continuation of that word / otherwise NULL

            /**
             * constructor: from part of input string
             * @param inp   input string
             * @param st    start position of word in input string
             * @param en    after end position of word in input string
             * @param qt    word is quoted
             * @param pst   position info of start of the input string
             * @param pw    previous word, if this is a continuation of that word / otherwise NULL
             */
            TxtWord(string& inp, size_t st, size_t en, bool qt, PositionInfo& pst, TxtWord *pw = NULL): word(inp, (qt ? st+1 : st), (qt ? (en-st-2) : (en-st))), quoted(qt), len(en-st), pos(pst), openBr(), openQt(false), prv(pw)   { pos.addCols(st); }

            /**
             * constructor: from given word (quoted in the original input string)
             * @param wrd   word
             * @param ln    length of the word
             * @param pst   position info of start of the input string
             * @param st    start position of word in input string
             * @param pw    previous word, if this is a continuation of that word / otherwise NULL
             */
            TxtWord(string& wrd, size_t ln, PositionInfo& pst, size_t st, TxtWord *pw = NULL): word(wrd), quoted(true), len(ln), pos(pst), openBr(), openQt(false), prv(pw)                                                           { pos.addCols(st); }

            /**
             * destructor
             */
            ~TxtWord()                                  { if (prv) delete prv; }

            /**
             * get whether this word is to continue by another word
             */
            inline bool unfinished() const              { return (!openBr.empty() || openQt); }

            /**
             * set word as unfinished by open quote
             */
            inline void setUnfinishedQt()               { openQt = true; }

            /**
             * set word as unfinished by open bracket
             * @param s         string with opened brackets
             */
            inline void setUnfinishedBr(string& s)      { openBr = s; }

            /**
             * set previous word, to mark this as a continuation of that word
             * @param pw        previous word
             */
            void setPrv(TxtWord *pw)                    { if (prv) { prv->setPrv(pw); } else { prv = pw; } }

            /**
             * get start part of word
             */
            string& start()                             { return (prv ? prv->start() : word); }

            /**
             * get first char of word
             */
            char startChar()                            { string& s = start(); return (s.empty() ? '\0' : s[0]); }

            /**
             * get start position of the word
             */
            PositionInfo& startPos()                    { return (prv ? prv->startPos() : pos); }

            /**
             * get start position of the word
             */
            PositionInfo endPos()                       { return (pos + len); }

            /**
             * get location of the word
             */
            LocationInfo location()                     { return LocationInfo(startPos(), endPos()); }

            /**
             * output word with position info to precompiler result stream
             * @param p         precompiler module
             * @param nms       namespace / NULL: don't use namespace here
             * @param defSym    defined symbols to prefix with the namespace
             */
            void outWithPos(Precompiler *p, const char *nms = NULL, vector<string *> *defSym = NULL);

            /**
             * search for unqualified references of a symbol within a word and prefix the symbol reference with a namespace
             * @param wr        word
             * @param s         symbol
             * @param nms       namespace
             */
            static void prefixNamespace(string& wr, string& s, const char *nms);
        };

        /**
         * scan next word from input string
         * @param inp       input string
         * @param p         start position in input string, returns next position
         * @param qt        allow quotes
         * @param br        allow brackets or braces / set from true to false if no matching bracket found
         * @param stc       array of possible stop characters / NULL: all non word characters
         * @param c         return of stop character / '\0' if end of input string
         * @param pst       position info of start of the input string
         * @param cont      continue this word / NULL: start new word
         * @return          scanned word (to delete by the caller) / NULL: no word before the stop character
         */
        static TxtWord *scanWord(string& inp, size_t& p, bool qt, bool &br, const char *stc, char& c, PositionInfo& pst, TxtWord *cont = NULL);

        /**
         * get directory with data file
         * @param p             precompiler module
         * @param dataFile      data file
         * @param dataDir       return of data directory
         * @param chgToDataDir  return whether to change dir to dataDir for opening the data file
         */
        static void getDataDir(Precompiler *p, FileInput *dataFile, string& dataDir, bool& chgToDataDir);
    };

}

#endif // DATAIMPORTBASE_HH

