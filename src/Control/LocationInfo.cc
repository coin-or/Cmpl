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

#include "LocationInfo.hh"
#include "MainControl.hh"


namespace cmpl
{
	/**
	 * output location for location definition to stream
	 * @param ostr		output stream
	 * @param inLocNr	if location has inLoc then use this definition number for it
	 * @param outTN		output also type and name
	 */
	void LocationInfo::outForDefLoc(ostream& ostr, int inLocNr, bool outTN) const
	{
		ostr << _begin.line() << ',' << _begin.col() << ',' << _end.line() << ',' << _end.col() << ',';

		if (_begin.isFixLoc() || _begin.inLoc())
			ostr << inLocNr;

		if (outTN) {
			ostr << ',' << type() << ',';
			if (!_begin.isFixLoc())
				ostr << name();
		}
		else {
			ostr << ",,";
		}
	}

	/**
	 * get a location object without inLoc
	 */
	const LocationInfo LocationInfo::withoutInLoc() const
	{
		if (_begin.isFixLoc()) {
			return *(_begin.fixLoc());
		}
		if (!_begin.inLoc() && !_end.inLoc() && !_end.isFixLoc()) {
			return *this;
		}
		else {
			return LocationInfo(_begin.withoutInLoc(), _end.withoutInLoc());
		}
	}


	/**
	 * output position to a stream
	 * @param ostr		output stream
	 * @param pos		position
	 */
	ostream& operator<< (ostream& ostr, const PositionInfo& pos)
	{
		if (pos.type() == POSITION_TYPE_FIXLOC) {
			ostr << *(pos.fixLoc());
		}
		else {
            if (pos.type() == POSITION_TYPE_STRING)
                ostr << '"' << pos.name() << '"';
            else
                ostr << pos.name();

			if (pos.type() == POSITION_TYPE_WORDARR)
				ostr << ":$" << pos.col();
			else {
				if (pos.hasLine() && pos.hasCol())
					ostr << ':' << pos.line() << '.' << pos.col();
				else if (pos.hasLine())
					ostr << ':' << pos.line();
				else if (pos.hasCol())
					ostr << ":C" << pos.col();
			}
		}
		return ostr;
	}


	/**
	 * output location to a stream
	 * @param ostr		output stream
	 * @param loc		location
	 */
	ostream& operator<< (ostream& ostr, const LocationInfo& loc)
	{
		PositionInfo last = loc.end() - 1;

		ostr << loc.begin();
		if (loc.begin() != last) {
			ostr << '-';
			if (loc.begin().name() != last.name() || loc.begin().type() != last.type())
				ostr << last;
			else {
				if (last.hasLine() && loc.begin().line() != last.line())
					ostr << last.line() << '.' << last.col();
				else if (last.type() == POSITION_TYPE_WORDARR)
					ostr << '$' << last.col();
				else
					ostr << last.col();
			}
		}

		if (loc.inLoc())
			ostr << ", called from: " << *(loc.inLoc());

		return ostr;
	}


	/**
	 * parse an int number
	 * @param ctrl				main control object for error handling
	 * @param txt               text with number
	 * @param res				return number
	 * @return 					whether txt is not empty
	 */
	bool PositionInfo::parsePMInt(MainControl *ctrl, const char *txt, int& res)
	{
		res = 0;
		if (*txt == '\0')
			return true;

		string s = txt;
		return parsePMInt(ctrl, s, res);
	}

	/**
	 * parse an int number
	 * @param ctrl				main control object for error handling
	 * @param str               string with number
	 * @param res				return number
	 * @return 					whether str is not empty
	 */
	bool PositionInfo::parsePMInt(MainControl *ctrl, const string& str, int& res)
	{
		res = 0;
		bool empty = false;
		string s = StringStore::lTrim(str);
			
		if (s.empty()) {
			empty = true;
		}
		else {
			try {
				res = stoi(s);
			}
			catch (exception) {
				ctrl->errHandler().error(ERROR_LVL_NORMAL, ctrl->printBuffer("illegal character in position mark ('%s' is not a number)", str.c_str()), *this);
				empty = true;
			}
		}

		return !empty;
	}


	/**
	 * parse Line position mark
	 * @param ctrl				main control object for error handling
	 * @param txt               text with line number
	 */
	void PositionInfo::parseLinePM(MainControl *ctrl, const char *txt)
	{
		int line;
		if (parsePMInt(ctrl, txt, line))
			setLine(line);

		setCol(1);
	}

	/**
	 * parse Col position mark
	 * @param ctrl				main control object for error handling
	 * @param txt               text with col number
	 */
	void PositionInfo::parseColPM(MainControl *ctrl, const char *txt)
	{
		int col;
		if (parsePMInt(ctrl, txt, col))
			setCol(col);
	}

	/**
	 * parse StartPos position mark
	 * @param ctrl				main control object for error handling
	 * @param txt               text with StartPos info
	 * @param names				string store for names
	 * @return					inLoc to set in this / 0: no one
	 */
	int PositionInfo::parseStartPosPM(MainControl *ctrl, const char *txt, StringStore *names)
	{
		string s = txt;

		int inLoc;
		bool isInLoc;
		int tp = type();
		string nm;

		size_t p = s.find(',');
		string t = s.substr(0, p);
		isInLoc = parsePMInt(ctrl, t, inLoc);

		if (p != string::npos) {
			s = s.substr(p+1);
			p = s.find(',');
			t = s.substr(0, p);
			if (! parsePMInt(ctrl, t, tp))
				tp = type();
				
			if (p != string::npos) {
				nm = s.substr(p+1);
			}
		}

		_inLoc = NULL;
		_type = tp;
		_line = _col = (tp==POSITION_TYPE_WORDARR || tp==POSITION_TYPE_DESCR || tp==POSITION_TYPE_FIXLOC ? 0 : 1);

		if (! nm.empty())
			_name = names->store(nm);

		return (isInLoc ? inLoc : 0);
	}

	/**
	 * parse DefLoc position mark
	 * @param ctrl				main control object for error handling
	 * @param txt               text with DefLoc info
	 * @param names				string store for names
	 * @param defNr				return of definition number for location
	 * @param loc				return of parsed location
	 * @return					inLoc to set in loc / 0: no one
	 */
	int PositionInfo::parseDefLocPM(MainControl *ctrl, const char *txt, StringStore *names, int& defNr, LocationInfo& loc)
	{
		string s = txt;

		int nr, inLoc;
		bool isInLoc;
		int tp = type();
		int fl = 1, fc = 1, tl = 1, tc = 1;
		string nm;

		size_t p = s.find(':');
		if (p == string::npos) {
			ctrl->errHandler().error(ERROR_LVL_NORMAL, "definition number for DefLoc missing", *this);
			return 0;
		}

		string t = s.substr(0, p);
		if (!parsePMInt(ctrl, t, nr)) {
			ctrl->errHandler().error(ERROR_LVL_NORMAL, "definition number for DefLoc missing", *this);
			return 0;
		}

		s = s.substr(p+1);
		p = s.find(',');
		t = s.substr(0, p);
		if (!parsePMInt(ctrl, t, fl))
			fl = 1;

		if (p != string::npos) {
			s = s.substr(p+1);
			p = s.find(',');
			t = s.substr(0, p);
			if (!parsePMInt(ctrl, t, fc))
				fc = 1;

			if (p != string::npos) {
				s = s.substr(p+1);
				p = s.find(',');
				t = s.substr(0, p);
				if (!parsePMInt(ctrl, t, tl))
					tl = 1;

				if (p != string::npos) {
					s = s.substr(p+1);
					p = s.find(',');
					t = s.substr(0, p);
					if (!parsePMInt(ctrl, t, tc))
						tc = 1;

					if (p != string::npos) {
						s = s.substr(p+1);
						p = s.find(',');
						t = s.substr(0, p);
						isInLoc = parsePMInt(ctrl, t, inLoc);

						if (p != string::npos) {
							s = s.substr(p+1);
							p = s.find(',');
							t = s.substr(0, p);
							if (! parsePMInt(ctrl, t, tp))
								tp = type();
								
							if (p != string::npos) {
								nm = s.substr(p+1);
							}
						}
					}
				}
			}
		}

		if (nm.empty())
			nm = name();

		loc._begin = PositionInfo(tp, names->store(nm));
		loc._begin._line = fl;
		loc._begin._col = fc;

		loc._end = loc._begin;
		loc._end._line = tl;
		loc._end._col = tc;

		defNr = nr;

		return (isInLoc ? inLoc : 0);
	}
}

