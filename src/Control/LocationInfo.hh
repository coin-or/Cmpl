
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


#ifndef LOCATIONINFO_HH
#define LOCATIONINFO_HH

#include <string>
#include <string.h>
#include <iostream>


using namespace std;


// Possible types of positions
#define POSITION_TYPE_FILE		0				// position in a file
#define POSITION_TYPE_STREAM	1				// position in an input stream
#define POSITION_TYPE_STRING	2				// position in a string
#define POSITION_TYPE_WORDARR	3				// position in an array of words
#define POSITION_TYPE_DESCR		4				// position is only a description

#define POSITION_TYPE_FIXLOC	5				// position is a fixed location


namespace cmpl
{
	class MainControl;
	class StringStore;
	class LocationInfo;

	/**
	 * <code>PositionInfo</code> represents a position in a file or similar,
	 * mostly for use in error handling.
	 */
	class PositionInfo
	{
	private:
		/**
		 * location in wich this position is embedded.
		 * e.g. if this is a position in an included file,
		 * then _inLoc holds the location of the include.
		 * also holds the fixed location if _type == POSITION_TYPE_FIXLOC.
		 */
		const LocationInfo *_inLoc;

		/**
		 * type of position, one of POSITION_TYPE_*
		 */
		int _type;

		/**
		 * filename or similar description, dependent on _type:
		 * POSITION_TYPE_FILE			file name, can be used to open the file
		 * POSITION_TYPE_STREAM			description or name of stream
		 * POSITION_TYPE_STRING			contains the string itself instead of a name
		 * POSITION_TYPE_WORDARR		name of the array of words
		 * POSITION_TYPE_DESCR			description of position
		 * POSITION_TYPE_FIXLOC			not used
		 * (the value of _name should be stored in a StringStore to enforce equal strings to have equal char pointers)
		 */
		const char *_name;

		/**
		 * line number of the position, dependent on _type:
		 * POSITION_TYPE_FILE, POSITION_TYPE_STREAM, POSITION_TYPE_STRING	line number, starts from 1
		 * POSITION_TYPE_WORDARR, POSITION_TYPE_DESCR, POSITION_TYPE_FIXLOC		not used
		 */
		unsigned _line;

		/**
		 * column number of the position, dependent on _type:
		 * POSITION_TYPE_FILE, POSITION_TYPE_STREAM, POSITION_TYPE_STRING	column number, starts from 1
		 * POSITION_TYPE_WORDARR		index of the word in the array, starts from 0
		 * POSITION_TYPE_DESCR, POSITION_TYPE_FIXLOC			not used
		 */
		unsigned _col;


	public:
		/**
		 * constructor
		 */
		PositionInfo(int t, const char *n)			{ _inLoc = NULL; _type = t; _name = n; _line = _col = (t==POSITION_TYPE_WORDARR || t==POSITION_TYPE_DESCR || t==POSITION_TYPE_FIXLOC ? 0 : 1); }

		/**
		 * constructor
		 */
		PositionInfo(int t)							{ _inLoc = NULL; _type = t; _name = "()"; _line = _col = (t==POSITION_TYPE_WORDARR || t==POSITION_TYPE_DESCR || t==POSITION_TYPE_FIXLOC ? 0 : 1); }

		/**
		 * constructor
		 */
		PositionInfo()								{ _inLoc = NULL; _type = POSITION_TYPE_DESCR; _name = "()"; _line = _col = 0; }

		/**
		 * constructor
		 */
		PositionInfo(const LocationInfo *l, int t, const char *n)	{ _inLoc = l; _type = t; _name = n; _line = _col = (t==POSITION_TYPE_WORDARR || t==POSITION_TYPE_DESCR || t==POSITION_TYPE_FIXLOC ? 0 : 1); }

		/**
		 * constructor
		 */
		PositionInfo(const LocationInfo *l, int t)					{ _inLoc = l; _type = t; _name = "()"; _line = _col = (t==POSITION_TYPE_WORDARR || t==POSITION_TYPE_DESCR || t==POSITION_TYPE_FIXLOC ? 0 : 1); }

		/**
		 * constructor
		 */
		PositionInfo(const LocationInfo *l)							{ _inLoc = l; _type = POSITION_TYPE_FIXLOC; _name = "()"; _line = _col = 0; }


		/**
		 * get type of position
		 */
		inline int type() const						{ return _type; }

		/**
		 * get whether position belongs to a file (you can use _name for opening a file)
		 */
		inline bool isFile() const					{ return (_type == POSITION_TYPE_FILE); }

		/**
		 * get whether position belongs to a file (you can use _name to get the content of the string)
		 */
		inline bool isString() const				{ return (_type == POSITION_TYPE_STRING); }

		/**
		 * get whether _name contains only a descriptive name
		 */
		inline bool isDescr() const					{ return (_type == POSITION_TYPE_STREAM || _type == POSITION_TYPE_WORDARR || _type == POSITION_TYPE_DESCR); }

		/**
		 * get whether position is a fixed location
		 */
		inline bool isFixLoc() const				{ return (_type == POSITION_TYPE_FIXLOC); }

		/**
		 * get whether position has a name
		 */
		inline bool hasName() const					{ return (_type != POSITION_TYPE_FIXLOC); }

		/**
		 * get whether position has a line number or similar
		 */
		inline bool hasLine() const					{ return (_type != POSITION_TYPE_WORDARR && _type != POSITION_TYPE_DESCR && _type != POSITION_TYPE_FIXLOC); }

		/**
		 * get whether position has a column number or similar
		 */
		inline bool hasCol() const					{ return (_type != POSITION_TYPE_DESCR && _type != POSITION_TYPE_FIXLOC); }

		/**
		 * get whether the end position for a string can derived from the start position by counting the chars in the string
		 */
		inline bool useCharCount() const			{ return (_type != POSITION_TYPE_WORDARR && _type != POSITION_TYPE_DESCR && _type != POSITION_TYPE_FIXLOC); }

		/**
		 * get whether the end position for a string can derived from the start position by counting the words in the string
		 */
		inline bool useWordCount() const			{ return (_type == POSITION_TYPE_WORDARR && _type != POSITION_TYPE_FIXLOC); }


		/**
		 * get location in with this position is embedded, or the fixed location representing this position
		 */
		inline const LocationInfo* inLoc() const	{ return (_type != POSITION_TYPE_FIXLOC ? _inLoc : NULL); }

		/**
		 * get the fixed location representing this position
		 */
		inline const LocationInfo* fixLoc() const	{ return (_type == POSITION_TYPE_FIXLOC ? _inLoc : NULL); }

		/*
		 * get name
		 */
		inline const char* name() const				{ return _name; }

		/*
		 * get line number
		 */
		inline unsigned line() const				{ return _line; }

		/*
		 * get column number
		 */
		inline unsigned col() const					{ return _col; }


		/**
		 * set name
		 */
		inline void set(const char *n)				{ _name = n; _line = _col = (_type==POSITION_TYPE_WORDARR || _type == POSITION_TYPE_DESCR || _type == POSITION_TYPE_FIXLOC ? 0 : 1); }

		/**
		 * set name, line and column number
		 */
		inline void set(const char *n, unsigned l, unsigned c=1)	{ _name = n; _line = (_type==POSITION_TYPE_WORDARR || _type == POSITION_TYPE_DESCR || _type == POSITION_TYPE_FIXLOC ? 0 : l); _col = (_type == POSITION_TYPE_DESCR || _type == POSITION_TYPE_FIXLOC ? 0 : (_type==POSITION_TYPE_WORDARR ? l : c)); }

		/**
		 * set line and column number
		 */
		inline void set(unsigned l, unsigned c=1)	{ _line = (_type==POSITION_TYPE_WORDARR || _type == POSITION_TYPE_DESCR || _type == POSITION_TYPE_FIXLOC ? 0 : l); _col = (_type == POSITION_TYPE_DESCR || _type == POSITION_TYPE_FIXLOC ? 0 : (_type==POSITION_TYPE_WORDARR ? l : c)); }


		/**
		 * set name only
		 */
		inline void setName(const char *n)			{ _name = n; }

		/**
		 * set line only
		 */
		inline void setLine(unsigned l)				{ _line = l; }

		/**
		 * set column only
		 */
		inline void setCol(unsigned c)				{ _col = c; }


		/**
		 * set location in with this position is embedded, or the fixed location representing this position
		 */
		inline void setInLoc(const LocationInfo *loc)	{ _inLoc = loc; }


		/**
		 * advance line
		 * @param l		count of lines
		 */
		inline void addLines(int l=1)				{ if (hasLine()) { _line = max(1u, _line + l); if (hasCol()) { _col = 1; } } }

		/**
		 * advance columns
		 * @param c		count of columns
		 */
		inline void addCols(int c=1)				{ if (hasCol()) { _col = max(1u, _col + c); } }


		/**
		 * advance columns
		 * @param c		count of columns
		 */
		inline const PositionInfo& operator+= (const int c)
		{
			addCols(c);
			return *this;
		}

		/**
		 * advance columns
		 * @param c		count of columns
		 */
		inline const PositionInfo& operator-= (const int c)
		{
			addCols(-c);
			return *this;
		}

		/**
		 * advance columns to new position
		 * @param c		count of columns
		 */
		inline const PositionInfo operator+ (const int c) const
		{
			PositionInfo res = *this;
			return res += c;
		}

		/**
		 * advance columns to new position
		 * @param c		count of columns
		 */
		inline const PositionInfo operator- (const int c) const
		{
			PositionInfo res = *this;
			return res -= c;
		}

		/**
		 * get a location object without inLoc
		 */
		inline const PositionInfo withoutInLoc() const
		{
			if (_inLoc) {
				PositionInfo res = *this;
				res._inLoc = NULL;
				return res;
			}
			else {
				return *this;
			}
		}

		friend bool operator== (const PositionInfo& p1, const PositionInfo& p2);
		friend bool operator!= (const PositionInfo& p1, const PositionInfo& p2);
		friend bool operator< (const PositionInfo& p1, const PositionInfo& p2);

		friend ostream& operator<< (ostream& ostr, const PositionInfo& pos);



		/************** utility functions for position marks **********/

		/**
		 * parse an int number
		 * @param ctrl				main control object for error handling
		 * @param txt               text with number
		 * @param res				return number
		 * @return 					whether txt is not empty
		 */
		bool parsePMInt(MainControl *ctrl, const char *txt, int& res);

		/**
		 * parse an int number
		 * @param ctrl				main control object for error handling
		 * @param str               string with number
		 * @param res				return number
		 * @return 					whether str is not empty
		 */
		bool parsePMInt(MainControl *ctrl, const string& str, int& res);

		/**
		 * parse Line position mark
		 * @param ctrl				main control object for error handling
		 * @param txt               text with line number
		 */
		void parseLinePM(MainControl *ctrl, const char *txt);

		/**
		 * parse Col position mark
		 * @param ctrl				main control object for error handling
		 * @param txt               text with col number
		 */
		void parseColPM(MainControl *ctrl, const char *txt);

		/**
		 * parse StartPos position mark
		 * @param ctrl				main control object for error handling
		 * @param txt               text with StartPos info
		 * @param names				string store for names
		 * @return					inLoc to set in this / 0: no one
		 */
		int parseStartPosPM(MainControl *ctrl, const char *txt, StringStore *names);

		/**
		 * parse DefLoc position mark
		 * @param ctrl				main control object for error handling
		 * @param txt               text with DefLoc info
		 * @param names				string store for names
		 * @param defNr				return of definition number for location
		 * @param loc				return of parsed location
		 * @return					inLoc to set in loc / 0: no one
		 */
		int parseDefLocPM(MainControl *ctrl, const char *txt, StringStore *names, int& defNr, LocationInfo& loc);
	};



	/**
	 * <code>LocationInfo</code> represents a location in a file or similar,
	 * mostly for use in error handling.
	 */
	class LocationInfo
	{
		friend class PositionInfo;

	private:
		PositionInfo _begin;		///< starting position
		PositionInfo _end;			///< end position (first position after location)

	public:
		/**
		 * constructor
		 */
		LocationInfo() : _begin(), _end()																	{ }

		/**
		 * constructor
		 */
		LocationInfo(int t) : _begin(t), _end(t)															{ }

		/**
		 * constructor
		 */
		LocationInfo(const PositionInfo& b) : _begin(b), _end(b+1)											{ }

		/**
		 * constructor
		 */
		LocationInfo(const PositionInfo& b, const PositionInfo& e) : _begin(b), _end(e)						{ }


		/**
		 * get type of location
		 */
		inline int type() const						{ return _begin.type(); }

		/**
		 * get whether location belongs to a file (you can use _name for opening a file)
		 */
		inline bool isFile() const					{ return _begin.isFile(); }

		/**
		 * get whether location belongs to a file (you can use _name to get the content of the string)
		 */
		inline bool isString() const				{ return _begin.isString(); }

		/**
		 * get whether name contains only a descriptive name
		 */
		inline bool isDescr() const					{ return _begin.isDescr(); }

		/**
		 * get whether location has a line number or similar
		 */
		inline bool hasLine() const					{ return _begin.hasLine(); }

		/**
		 * get whether location has a column number or similar
		 */
		inline bool hasCol() const					{ return _begin.hasCol(); }

		/**
		 * get location in with this location is embedded
		 */
		inline const LocationInfo* inLoc() const	{ return _begin.inLoc(); }

		/**
		 * get the fixed location representing this position
		 */
		inline const LocationInfo* fixLoc() const	{ return _begin.fixLoc(); }

        /**
		 * get name
		 */
		inline const char* name() const				{ return _begin.name(); }

        /**
         * get a key value that is equal for equal locations (but also different locations can have the same key value)
         */
        inline unsigned long key() const            { return (_begin.isFixLoc() ? _begin.fixLoc()->key() : ((_begin.type() & 0x0fUL) << 28) | (((_begin.name() ? _begin.name()[0] : 0) & 0x0fUL) << 24) | ((_begin.line() & 0xffUL) << 16) | ((_begin.col() & 0xffUL) << 8) | ((_end.line() & 0x0fUL) << 4) | ((_end.col() & 0x0fUL) << 0));  }

		/**
		 * get begin position of location
		 */
		inline const PositionInfo& begin() const	{ return _begin; }

		/**
		 * get end position of location
		 */
		inline const PositionInfo& end() const		{ return _end; }


		/**
		 * set begin and end position of location joining two locations
		 */
		inline void setFrom(const LocationInfo& b, const LocationInfo& e)	{ _begin = b._begin; _end = e._end; }

		/**
		 * set begin and end position of location from the end position of another location
		 */
		inline void setFrom(const LocationInfo& e)							{ _begin = _end = e._end; }

		/**
		 * set begin and end position of location from two positions
		 */
		inline void setFrom(const PositionInfo& b, const PositionInfo& e)	{ _begin = b; _end = e; }


		/**
		 * reset initial location to final location
		 */
		inline void step()							{ _begin = _end; }

		/**
		 * extend the current location to c next columns
		 */
		inline void columns(int c = 1)				{ _end.addCols(c); }

		/**
		 * extend the current location to c next lines
		 */
		inline void lines(int c = 1)				{ _end.addLines(c); }


		/**
		 * extends location up to an end position
		 * @param e		end position
		 */
		inline const LocationInfo operator+= (const PositionInfo& e)
		{
			_end = e;
			return *this;
		}

		/**
		 * join two locations.
		 * @param b		start location
		 * @param e		end location
		 */
		inline const LocationInfo operator+ (const LocationInfo& e) const
		{
			LocationInfo res = *this;
			res._end = e._end;
			return res;
		}

		/**
		 * output location for location definition to stream
		 * @param ostr		output stream
		 * @param inLocNr	if location has inLoc then use this definition number for it
		 * @param outTN		output also type and name
		 */
		void outForDefLoc(ostream& ostr, int inLocNr, bool outTN) const;

		/**
		 * get a location object without inLoc
		 */
		const LocationInfo withoutInLoc() const;


		friend bool operator== (const LocationInfo& l1, const LocationInfo& l2);
		friend bool operator!= (const LocationInfo& l1, const LocationInfo& l2);
		friend bool operator< (const LocationInfo& l1, const LocationInfo& l2);

		friend ostream& operator<< (ostream& ostr, const LocationInfo& loc);
	};


	/**
	 * Compare two positions
	 */
	inline bool operator== (const PositionInfo& p1, const PositionInfo& p2)
	{
		return (p1._type == p2._type && strcmp(p1._name, p2._name) == 0 && p1._line == p2._line && p1._col == p2._col && (p1._inLoc == p2._inLoc || (p1._inLoc && p2._inLoc && *(p1._inLoc) == *(p2._inLoc))));
	}

	/**
	 * Compare two positions
	 */
	inline bool operator!= (const PositionInfo& p1, const PositionInfo& p2)
	{
		return !(p1 == p2);
	}

	/**
	 * less-operator for positions to allow sorting
	 */
	inline bool operator< (const PositionInfo& p1, const PositionInfo& p2)
	{
		return (p1._type < p2._type || (p1._type == p2._type &&
			(strcmp(p1._name, p2._name) < 0 || (strcmp(p1._name, p2._name) == 0 &&
			(p1._line < p2._line || (p1._line == p2._line &&
			(p1._col < p2._col || (p1._col == p2._col &&
			((!(p1._inLoc) && p2._inLoc) || (p1._inLoc && p2._inLoc && *(p1._inLoc) < *(p2._inLoc)))))))))));
	}

	/**
	 * Compare two locations
	 */
	inline bool operator== (const LocationInfo& l1, const LocationInfo& l2)
	{
		return (l1._begin == l2._begin && l1._end == l2._end);
	}

	/**
	 * Compare two locations
	 */
	inline bool operator!= (const LocationInfo& l1, const LocationInfo& l2)
	{
		return !(l1 == l2);
	}

	/**
	 * less-operator for locations to allow sorting
	 */
	inline bool operator< (const LocationInfo& l1, const LocationInfo& l2)
	{
		return (l1._begin < l2._begin || (l1._begin == l2._begin && l1._end < l2._end));
	}
}

#endif // LOCATIONINFO_HH

