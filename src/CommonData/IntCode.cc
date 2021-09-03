
/***********************************************************************
 *  This code is part of CMPL
 *
 *  Copyright (C) 2007, 2008, 2009, 2010, 2011 Thomas Schleiff - Halle(Saale),
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


#include "IntCode.hh"
#include "../Control/MainControl.hh"


namespace cmpl
{
	// defines data class
	DATA_CLASS( IntCode );


#if PROTO
	// names of intermediary code commands, only used for protocol output
	// must match defines for INTCODE_* in IntCode.hh
	const char *commandNames[][30] = {
		{ "NOP" },
		{ "LIT_VAL",			"INT", "STR", "REAL", "BLANK" },
        { "FETCH",				"GLOBAL", "LOCAL", "DEF_CB" },
        { "FETCH_SPECIAL",		"ARG", "THIS", "CURTUPEL", "CURFULLTUPLE", "CURDESTNAME", "CURDESTTUPEL", "VARBYNAME", "VARBYNO", "CONBYNAME", "CONBYNO", "OBJECTTYPE", "DATATYPE", "TYPEBASE", "TYPEPAR", "ISREADONLY", "DESTNAME", "DESTTUPEL", "DESTFULLNAME", "FULLFORMULA", "FORMULAARRAY" },
		{ "ASSIGN",				"NORMAL", "REF", "ADD", "SUB", "MUL", "DIV", "LHSONLY", "RHSONLY", "EXTERN", "ASSERT" },
		{ "OPERATION",			"ADD", "SUB", "MUL", "DIV", "EXP", "AND", "OR", "NOT", "EQ", "EQ2", "NE", "NE2", "LT", "GT", "LE", "GE", "IN", "OF", "IRR", "TRP", "OBJTO", "INDEX", "FUNC", "ARRCAST" },
		{ "CONSTRUCT",			"ARRAY", "ARRAY_IND", "ARRAY_WOI", "TUPEL", "IV_WOLB", "IV_LB", "SET" },
		{ "LINENAMEPREF",		"EXTEND", "RESET" },
		{ "FUNCTION",			"START", "END", "STORE", "GOTO" },
		{ "CODEBLOCK",			"BLOCK_START", "BLOCK_END", "PART_START", "PART_BODY", "PART_END" },
        { "CB_HEADER",          "HEADER_START", "HEADER_END" },
		{ "CB_CONTROL",			"BREAK", "CONTINUE", "REPEAT" },
	};

	/**
	 * write code element to protocol
	 * @param m		module
	 * @param i     address of code element within the code
	 * @param ostr	output to this stream / NULL: to protocol stream
	 */
	void IntCode::IcElem::protoOut(ModuleBase *m, unsigned i, ostream *ostr)
	{
		if (!ostr) {
			if (m->modp())
				ostr = m->modp()->protoStream();

			if (!ostr)
				return;
		}

		*ostr << " @ " << i << " (" << se << "): ";

		switch (tp) {
			case icTypeCommand:
                if (v.c.minor > 0 && v.c.minor < 30) {
					*ostr << commandNames[v.c.major][0] << " - " << commandNames[v.c.major][v.c.minor] << ": " << v.c.par << ", " << v.c.cnt << endl;
				}
                else if (v.c.minor) {
					*ostr << commandNames[v.c.major][0] << " - " << v.c.minor << ": " << v.c.par << ", " << v.c.cnt << endl;
				}
				else {
					*ostr << commandNames[v.c.major][0] << ": " << v.c.par << ", " << v.c.cnt << endl;
				}
				break;

			case icTypeInt:
				*ostr << "    (argInt) " << v.i << endl;
				break;

			case icTypeReal:
				*ostr << "    (argReal) " << v.r << endl;
				break;

			case icTypeNo2:
				*ostr << "    (argNo2) " << v.n.n1 << ", " << v.n.n2 << endl;
				break;

			default:
				*ostr << "(unknown)" << endl;
				break;
		}
	}
#endif


	/**
	* delete all data
	*/
	void IntCode::cleanUp()
	{
		if (_code) {
			delete _code;
			_code = NULL;
			_cnt = 0;
		}
	}


	/**
	 * check if serialization to or from stream should be done for the data object
	 * @param si		serialization info
	 * @param data		main data object
	 * @return			true if serialization should be done
	 */
	bool IntCode::checkSerialize(MainData::SerializeInfo& si, const MainData *data)
	{
		return (si.toFromServer >= 1);
	}

	/**
	 * write data object to output stream
	 * @param si		serialization info
	 * @param data		main data object
	 */
	void IntCode::serializeTo(MainData::SerializeInfo& si, const MainData *data)
	{
		(*si.ostr) << endl;
		
		IcElem *e = _code;
		for (unsigned i = 0; i < _cnt; i++, e++) {
			if (si.lineNums)
				(*si.ostr) << i << '#';

			(*si.ostr) << (int)(e->tp) << ';' << e->se << ';';

			switch (e->tp) {
				case icTypeCommand:
					(*si.ostr) << e->v.c.major << ';' << e->v.c.minor << ';' << e->v.c.par << ';' << e->v.c.cnt << endl;
					break;

				case icTypeInt:
					(*si.ostr) << e->v.i << endl;
					break;

				case icTypeReal:
					(*si.ostr) << e->v.r << endl;
					break;

				case icTypeNo2:
					(*si.ostr) << e->v.n.n1 << ';' << e->v.n.n2 << endl;
					break;

				default:
					(*si.ostr) << endl;
					break;
			}
		}
	}

	/**
	 * fill data object from input stream
	 * @param si		serialization info
	 * @param data		main data object
	 * @param pos		position of input string, returns iterated position
	 * @param subCnt	count of sub data objects
	 * @param lc		line count per element or 0 if elements are len coded
	 * @param rline		remaining section start line
	 */
	void IntCode::deserializeFrom(MainData::SerializeInfo& si, const MainData *data, int subCnt, int lc, string &rline)
	{
		cleanUp();
		_code = new IcElem[subCnt];
		_cnt = subCnt;

		IcElem *e = _code;
		string line;
		bool isString = false;

		si.pos->addLines();

		for (unsigned i = 0; i < _cnt; i++, e++) {
			getline(*si.istr, line);
			if (!si.istr->good())
				throw MainDataInputException("unexpected end of file or read error", *si.pos);

			if (si.lineNums)
				MainData::getNextLong(line, si.pos);

			e->tp = (IcType) MainData::getNextLong(line, si.pos);
			e->se = MainData::getNextLong(line, si.pos);

			switch (e->tp) {
				case icTypeCommand:
					e->v.c.major = (unsigned short) MainData::getNextLong(line, si.pos);
					e->v.c.minor = (unsigned short) MainData::getNextLong(line, si.pos);
					e->v.c.par = (unsigned short) MainData::getNextLong(line, si.pos);
					e->v.c.cnt = (unsigned short) MainData::getNextLong(line, si.pos);

					if (e->v.c.major == INTCODE_LIT_VAL && e->v.c.minor == ICS_LIT_STR) {
						if (e->v.c.cnt == 0)
							e->v.c.par = si.mapStrings[e->v.c.par];
						else
							isString = true;
					}

					break;

				case icTypeInt:
					e->v.i = STR_TO_CMPLINT(line.c_str());
					if (isString) {
						e->v.i = si.mapStrings[e->v.i];
						isString = false;
					}
					break;

				case icTypeReal:
					e->v.r = STR_TO_CMPLREAL(line.c_str());
					break;

				case icTypeNo2:
					e->v.n.n1 = MainData::getNextLong(line, si.pos);
					e->v.n.n2 = MainData::getNextLong(line, si.pos);
					break;
			}
		}
	}

}

