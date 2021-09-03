/***********************************************************************
 *  This code is part of CMPL
 *
 *  Copyright (C) 2007, 2008, 2009, 2010 Thomas Schleiff - Halle(Saale), 
 *  Germany and Mike Steglich - Technical University of Applied Sciences
 *  Wildau, Germany 
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


#ifndef STRINGSTORE_HH
#define STRINGSTORE_HH

#include <string.h>
#include <string>
#include <map>

#include <algorithm>
#include <vector>


using namespace std;


namespace cmpl
{
	class ModuleBase;
	class ModulesConf;
	class MainControl;


	/**
	 * <code>StringStoreStringNotFoundException</code> is thrown if a string is not found in the string store
	 */
	class StringStoreStringNotFoundException : public exception
	{
	public:
		/**
		 * get exception info
		 */
		virtual const char *what() const throw()          { return "string not found in the string store"; }
	};


	/**
	 * class for a store for strings.
	 * strings in it are referencable by their number or an permanent char pointer.
	 */
	class StringStore
	{
	private:
		ModuleBase *_modp;					///< module to which this string store belongs
		string _name;						///< name of string store, only used for protocol output

		const char **_strings_ind;   		///< array of pointers to the strings; the index in this array is the number of the saved string
		const char **_strings_sort;  		///< array of pointers to the same strings, but sorted in ascending alphabetic order
		unsigned *_index;              		///< array of the indexes of the strings in <code>strings_ind</code> in the same order like in <code>strings_sort</code>
		unsigned _cap;                 		///< capacity of the arrays <code>strings_ind</code>, <code>strings_sort</code> and <code>index</code>
		unsigned _len;                 		///< last used element + 1 in the arrays <code>strings_ind</code>, <code>strings_sort</code> and <code>index</code>

	public:
		/**
		 * constructor (after it setModp() must be called before this object can be used)
         * @param addEmpty  add empty string as first string to the string store
		 */
        StringStore(bool addEmpty = false);

		/**
		 * sets module to which this string store belongs
		 * @param modp		module
		 * @param name		name for string store
		 */
		inline void setModp(ModuleBase *modp, string name)		{ _modp = modp; _name = name; }

		/**
		 * destructor
		 */
		~StringStore();


		/**
		 * initialize new string store
		 */
        void init();

		/**
		 * remove all strings from the string table
		 */
		void clear();


		/**
		 * get module to which this string store belongs
		 */
		inline ModuleBase *modp()								{ return _modp; }

		/**
		 * get count of strings in this string store
		 */
		inline unsigned size()									{ return _len; }

		/**
		 * gives the string with the number i.
		 * @param i     reference number of the string
		 * @return      pointer to the string
		 */
		inline const char *operator[](unsigned i) 				{ return _strings_ind[i]; }

		/**
		 * gives the string with the number i.
		 * @param i     reference number of the string
		 * @return      pointer to the string
		 */
		inline const char *at(unsigned i) 						{ return _strings_ind[i]; }
		

		/**
		 * search and insert a string to the string table.
		 * @param str           input string
		 * @param free          free original string (or don't copy it when inserting, so it is freed in the destructor)
		 * @param insert		insert if not found
		 * @param found         return if string <code>str</code> is found (true) or new inserted (false)
		 * @return              reference number of the string
		 */
		unsigned searchInsert(const char *str, bool free, bool insert, bool &found);

		/**
		 * search a string in the string store, throw exception if not found.
		 * @param str			string to search
		 * @return				reference number of the string
		 */
		unsigned search(const char *str)						{ bool found; unsigned i = searchInsert(str, false, false, found); if (!found) throw StringStoreStringNotFoundException(); return i; }

		/**
		 * search a string in the string store, throw exception if not found.
		 * @param str			string to search
		 * @return				reference number of the string
		 */
		unsigned search(const string &str)						{ return search(str.c_str()); }

		/**
		 * store a string in the string table, and return its number.
		 * @param str			input string
		 * @return				reference number of the string
		 */
		inline unsigned storeInd(const char *str)				{ bool found; return searchInsert(str, false, true, found); }

		/**
		 * store a string in the string table, and return its number.
		 * @param str			input string
		 * @return				reference number of the string
		 */
		inline unsigned storeInd(const string &str)				{ return storeInd(str.c_str()); }

		/**
		 * store a string in the string table, and return a permanent pointer on it.
		 * @param str			input string
		 * @return				pointer to string
		 */
		inline const char *store(const char *str)				{ if (str) { unsigned i = storeInd(str); return _strings_ind[i]; } else { return str; } }

		/**
		 * store a string in the string table, and return a permanent pointer on it.
		 * @param str			input string
		 * @return				pointer to string
		 */
		inline const char *store(const string &str)				{ unsigned i = storeInd(str); return _strings_ind[i]; }

		/**
		 * merge contents of a second string store to this.
		 * @param ss2			second string store
		 * @param mr			map to insert mapping for the reference numbers of merged strings
		 * @param mc			map to insert mapping for the char pointers of merged strings
		 * @return				count of strings inserted in this
		 */
		unsigned merge(StringStore& ss2, map<unsigned, unsigned>& mr, map<const char *, const char *>& mc);


		/*********** static string utility functions ********************/

        // white space characters
        #define WHITE_SPACES            " \t\f\v\n\r"
        // white space characters and other chars (a must be a literal string)
        #define WHITE_SPACES_AND(a)     (WHITE_SPACES a)

		/**
		 * returns string with standard whitespace chars
		 */
        static const string& whiteSpaces()						{ static const string ws = WHITE_SPACES; return ws; }

		/**
		 * returns a left trimmed string
		 * @param str		string to trim
		 * @param ws		space chars
		 * @return			left trimmed string
		 */
        static string lTrim(const string& str, const char *ws = WHITE_SPACES)		{ size_t p = str.find_first_not_of(ws); if (p==string::npos) return ""; else return str.substr(p); }

		/**
		 * returns a right trimmed string
		 * @param str		string to trim
		 * @param ws		space chars
		 * @return			right trimmed string
		 */
        static string rTrim(const string& str, const char *ws = WHITE_SPACES)		{ size_t p = str.find_last_not_of(ws); if (p==string::npos) return ""; else return str.substr(0, p+1); }

		/**
		 * returns a left and right trimmed string
		 * @param str		string to trim
		 * @param ws		space chars
		 * @return			left and right trimmed string
		 */
        static string lrTrim(const string& str, const char *ws = WHITE_SPACES)		{ size_t p = str.find_first_not_of(ws); if (p==string::npos) return ""; else return str.substr(p, str.find_last_not_of(ws)-p+1); }

        /**
         * returns string in upper cases
         * @param str		string to transform to upper case
         * @return			string in upper case
         */
        static string upperCase(const string& str)      {  string tmpString=str; transform(tmpString.begin(), tmpString.end(), tmpString.begin(), ::toupper); return tmpString;   }

        /**
         * returns string in lower cases
         * @param str		string to transform to lower case
         * @return			string in lower case
         */
        static string lowerCase(const string& str) {       string tmpString=str;    transform(tmpString.begin(), tmpString.end(), tmpString.begin(), ::tolower);  return tmpString;    }


        static string replaceAll(const string& str, const string& targ, const string& repl);


        /**
         * returns whether a string starts with an particular string
         * @param str		string to check
         * @param start     start string
         * @return			true or false
         */
        static bool startsWith(const string& str, const string&  start)      { return  ( str.substr(0, start.length() ) == start );        }

        static bool stringEndsWith(string str, string end) ;

        static bool contains(const string& str, const string& str1) {   return  ( str.find(str1) != string::npos );  }

        static void getOptionValue(const string& str, string& key, string& val);

        static void split(const string& str, vector<string>& strVec, const char *sep = WHITE_SPACES) ;

        static bool toInt(const string& str, int& t);

        static bool toLong(const string& str, long& t );

        static bool toDouble(const string& str, double& t );

        static void  cleanDoubleQuotes(string &str);

        /**
        * @brief returns the name of a model
        *
        * @param probName
        * @return model name w/o extension
        */
        static string modelName(const string &probName);


        /**
         * encodes xml expressions in a string
         * @return			encoded string
         */
        static  string encodeXml(string &str) ;


		/**
		 * iterates over words in a string, on each call the next word is given back
		 * @param str		string to iterate over
		 * @param word		return of next word
		 * @param pos		current position in string, start iteration with 0
         * @param sep		separating chars
         * @param msep      multiple consecutive separating chars count as one separator
         * @param esc       use escape with \ for embedded quotes (false: use doubled quote char)
		 * @return			false if no more word found
		 */
        static bool iterWords(const string& str, string& word, size_t& pos, const char *sep = WHITE_SPACES, bool msep = true, bool esc = false)		{ bool quote; return iterWords(str, word, pos, quote, '\0', sep, msep, esc); }

		/**
		 * iterates over words in a string, on each call the next word is given back, with regarding of quotes
		 * @param str		string to iterate over
		 * @param word		return of next word
		 * @param pos		current position in string, start iteration with 0
		 * @param quote		return if the word was quoted
		 * @param qc		quote char / '\0' no quote char
         * @param sep		separating chars
         * @param msep      multiple consecutive separating chars count as one separator
         * @param esc       use escape with \ for embedded quotes (false: use doubled quote char)
         * @param keepq     keep quotes and escapes in returned word
         * @return			false if no more word found
		 */
        static bool iterWords(const string& str, string& word, size_t& pos, bool& quote, char qc = '"', const char *sep = WHITE_SPACES, bool msep = true, bool esc = false, bool keepq = false);

        /**
         * return a string suitable as name
         * @param str		source string
         * @param rplSp		replacement char for space, 0 for removing spaces, space char for leaving spaces unchanged
         * @param mLen		max length for result name
         * @return
         */
        static string asName(const string& str, char rplSp, unsigned mLen);

        /**
         * @brief surround a string with quotes, and escape embedded quote chars and new line chars
         * @param str       source string
         * @param qc		quote char
         * @return          quoted string
         */
        static string quoteString(const string& str, char qc = '"')     { return quoteString(str.c_str(), qc); }

        /**
         * @brief surround a string with quotes, and escape embedded quote chars and new line chars
         * @param str       source string
         * @param qc		quote char
         * @return          quoted string
         */
        static string quoteString(const char *str, char qc = '"');

        /**
         * remove quotes from string, and unescape embedded quote chars and new line chars
         * @param str       source string
         * @param qc		quote char
         * @return          unquoted string
         */
        static string unquoteString(const string& str, char qc = '"')   { return unquoteString(str.c_str(), qc); }

        /**
         * remove quotes from string, and unescape embedded quote chars and new line chars
         * @param str       source string
         * @param qc		quote char
         * @return          unquoted string
         */
        static string unquoteString(const char *str, char qc = '"');
    };
}

#endif // !STRINGSTORE_HH

