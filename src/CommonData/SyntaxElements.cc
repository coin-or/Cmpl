
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


#include "SyntaxElements.hh"


namespace cmpl
{
	// defines data class
	DATA_CLASS( SyntaxElements );


	/**
	 * writes the symbol info as an XML entry
	 * @param ostr              stream to write to
	 * @param indent            string for indentation of element
	 */
	void SymbolInfo::writeXML(ostream& ostr, const string &indent)
	{
		ostr << indent << "<symbol name='" << _name << "' symid='" << _defId << "' globid='" << _globId << "' predef='" << _predef << "' defloc='" << _loc
            << "' sub='" << (_subSymbol ? 1 : 0) << "' predefcont='" << _predefCont << "' const='" << (_readOnly ? 1 : 0) << "' scope='" << _scopeType << "' spec='" << _special << "' />" << endl;
	}


	/**
	 * creates a new element
	 * @param id			unique id number for the syntax element
	 * @param tp			type of the element
	 * @param text			text
	 * @param alttext		alternative text
	 * @param symId			unique symbol definition number
	 * @param locBegin      begin location
	 * @param locEnd        end location
	 * @return              new element
	 */
	SyntaxElement* SyntaxElement::newEntry(unsigned id, SyntaxElementType tp, const char *text, const char *alttext, unsigned symId, LocationInfo *locBegin, LocationInfo *locEnd)
	{
		SyntaxElement *res = new SyntaxElement(id);

		res->_type = tp;
		res->_text = text;
		res->_alttext = alttext;
		res->_symId = symId;

		if (locBegin == locEnd) {
			res->_loc = locBegin->withoutInLoc();
		}
		else {
			LocationInfo loc = *locBegin + *locEnd;
			res->_loc = loc.withoutInLoc();
		}

		res->_next = NULL;
		res->_child = NULL;

		return res;
	}

	/**
	 * fills a map of syntax elements by id
	 * @param mp				map to fill
	 */
	void SyntaxElement::fillMap(map<unsigned, const SyntaxElement *> *mp)
	{
        SyntaxElement *s = this;
        while (s) {
            (*mp)[s->_id] = s;

            if (s->_child)
                s->_child->fillMap(mp);


            s = s->_next;
        }
	}

	/**
	 * Writes the element as an XML entry
	 * @param ostr  			stream to write to
	 * @param indent            string for indentation of element
	 * @param childIndentAdd    additional indentation for child elements
	 */
	void SyntaxElement::writeXML(ostream& ostr, const string &indent, int childIndentAdd)
	{
		// names of element types in the order of enum SyntaxElementType
        const char **elemTypeNames = new const char*[32] {
			"Code",
			"Directive",
			"Modificator",
			"LitString",
			"LitInt",
			"LitDouble",
			"Statement",
			"Assignment",
			"AssignLhs",
			"AssignRhs",
			"Codeblock",
			"BlockPart",
			"BlockHeader",
			"BlockBody",
			"UnaryOp",
			"BinaryOp",
			"CompareOp",
			"SubSymbolOp",
			"Indexation",
			"FuncCall",
			"ArrayCast",
			"Array",
			"Tupel",
			"Set",
			"Interval",
			"Function",
			"LineNamePrefix",
			"Symbol",
			"SymbolIncDec",
			"Empty",
			"Error",
		};

		writeXML(ostr, elemTypeNames, indent, childIndentAdd);
	}

	/**
	 * Writes the element as an XML entry
	 * @param ostr              stream to write to
	 * @param etnm              element type names
	 * @param indent            string for indentation of element
	 * @param childIndentAdd    additional indentation for child elements
	 */
	void SyntaxElement::writeXML(ostream& ostr, const char **etnm, const string &indent, int childIndentAdd)
	{
        SyntaxElement *se = this;
        while (se) {
            ostr << indent << "<" << etnm[se->_type] << " id='" << se->_id << "'";

            if (se->_text)
                ostr << " text='" << se->_text << "'";
            if (se->_alttext)
                ostr << " alttext='" << se->_alttext << "'";

            if (se->_symId != 0)
                ostr << " symid='" << se->_symId << "'";

            ostr << " loc='" << se->_loc << "'";

            if (se->_child)
            {
                ostr << ">" << endl;

                const string childIndent(indent.length() + childIndentAdd, ' ');
                se->_child->writeXML(ostr, etnm, childIndent, childIndentAdd);

                ostr << indent << "</" << etnm[se->_type] << ">" << endl;
            }
            else
            {
                ostr << " />" << endl;
            }

            se = se->_next;
        }
	}


	/**
	 * insert strings and locations used from this in the data object
	 * @param data		main data object
	 */
	void SyntaxElement::insertStringsAndLocs(MainData *data)
	{
		StringStore *gs = data->globStrings();

        SyntaxElement *se = this;
        while (se) {
            if (se->_text)
                gs->store(se->_text);

            if (se->_alttext)
                gs->store(se->_alttext);

            se->_loc = data->persistentLocation(se->_loc);
            data->searchInsertInGlobLocs(&(se->_loc));

            if (se->_child)
                se->_child->insertStringsAndLocs(data);

            se = se->_next;
        }
	}


	/**
	 * write element to output stream
	 * @param si				serialization info
	 * @param data				main data object
	 * @param cur				current number
	 * @param prv				number of previous element / -1: current element is root element
	 * @param child				current element is child of previous element
	 */
	void SyntaxElement::serializeTo(MainData::SerializeInfo& si, const MainData *data, int& cur, int prv, bool child)
	{
		if (si.lineNums)
			(*si.ostr) << cur << '#';

		int cre = cur++;
		(*si.ostr) << prv << ';' << (child ? 1 : 0) << ';' << _id << ';' << (int)_type << ';' << (_text ? (int)(data->globStrings()->search(_text)) : -1) << ';' << (_alttext ? (int)(data->globStrings()->search(_alttext)) : -1) << ';'
            << _symId << ';' << data->searchLoc(si, &_loc) << endl;

		if (_child)
			_child->serializeTo(si, data, cur, cre, true);

		if (_next)
			_next->serializeTo(si, data, cur, cre, false);
	}



	/**
	 * set root element
	 * @param r			root of syntax elements
	 * @param s			mapping for defined symbols
	 * @param data		main data object
	 */
	void SyntaxElements::setRoot(SyntaxElement *r, map<unsigned, SymbolInfo *> *s, MainData *data)
	{
		cleanUp();
		_root = r;
		_cnt = (r ? r->count() : 0);
		_symbols = s;

        insertStringsAndLocs(data);
	}

	/**
	 * insert strings and locations used from this in the data object
	 * @param data		main data object
	 */
	void SyntaxElements::insertStringsAndLocs(MainData *data)
	{
		StringStore *gs = data->globStrings();

		// symbols
		for (map<unsigned, SymbolInfo *>::iterator it = _symbols->begin(); it != _symbols->end(); it++) {
			SymbolInfo *s = it->second;

			gs->store(s->_name);
			
			s->_loc = data->persistentLocation(s->_loc);
			data->searchInsertInGlobLocs(&(s->_loc));
		}

		// Elements
		_root->insertStringsAndLocs(data);
	}


	/**
	 * Writes syntax elements as XML entries
	 * @param ostr				stream to write to
	 * @param childIndentAdd	additional indentation for child elements
	 */
	void SyntaxElements::writeXML(ostream& ostr, int childIndentAdd)
	{
		ostr << "<cmpl>" << endl;
		const string indent(childIndentAdd, ' ');

		// symbol list
		if (_symbols) {
			ostr << indent << "<symbolList>" << endl;
			const string indent2 = indent + indent;
			for (map<unsigned, SymbolInfo *>::iterator it = _symbols->begin(); it != _symbols->end(); it++)
				it->second->writeXML(ostr, indent2);
			ostr << indent << "</symbolList>" << endl;
		}
			
		// syntax structure
		if (_root)
			_root->writeXML(ostr, indent, childIndentAdd);

		ostr << "</cmpl>" << endl;
	}


	/**
	 * delete all data
	 */
	void SyntaxElements::cleanUp()
	{
		if (_root) {
			delete _root;
			_root = NULL;
			_cnt = 0;
		}

		if (_symbols) {
			for (map<unsigned, SymbolInfo *>::iterator it = _symbols->begin(); it != _symbols->end(); it++)
				delete it->second;

			delete _symbols;
			_symbols = NULL;
		}
	}


	/**
	 * check if serialization to or from stream should be done for the data object
	 * @param si		serialization info
	 * @param data		main data object
	 * @return			true if serialization should be done
	 */
	bool SyntaxElements::checkSerialize(MainData::SerializeInfo& si, const MainData *data)
	{
		return (si.toFromServer >= 1);
	}

	/**
	 * write data object to output stream
	 * @param si		serialization info
	 * @param data		main data object
	 */
	void SyntaxElements::serializeTo(MainData::SerializeInfo& si, const MainData *data)
	{
		// Symbols
		(*si.ostr) << "*Symbols:" << _symbols->size() << ";1" << endl;

		int i = 0;
		for (map<unsigned, SymbolInfo *>::iterator it = _symbols->begin(); it != _symbols->end(); it++, i++) {
			if (si.lineNums)
				(*si.ostr) << i << '#';

			SymbolInfo *s = it->second;
            (*si.ostr) << it->first << ';' << data->globStrings()->search(s->_name) << ';' << s->_defId << ';' << s->_globId << ';' << data->searchLoc(si, &(s->_loc)) << ';'
                << s->_predef << ';' << (s->_subSymbol ? 1 : 0) << ';'<< s->_predefCont << ';'  << (s->_readOnly ? 1 : 0) << ';' << s->_scopeType << ';' << s->_special << endl;
		}

		// Elements
		(*si.ostr) << "*Elements:" << (_root ? _root->count() : 0) << ";1" << endl;
		int cur = 0;
		_root->serializeTo(si, data, cur, -1, false);
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
	void SyntaxElements::deserializeFrom(MainData::SerializeInfo& si, const MainData *data, int subCnt, int lc, string &rline)
	{
		string subName, rln, line;
		int outLevel, i;

		si.pos->addLines();

		// Symbols
		subName = MainData::readSectionStart(si, 1, outLevel, subCnt, lc, rln);
		if (subName != "Symbols")
			throw MainDataInputException("unexpected section, expecting 'Symbols'", *si.pos);

		_symbols = new map<unsigned, SymbolInfo *>();
		SymbolInfo *sy;

		si.pos->addLines();
		for (i = 0; i < subCnt; i++) {
			getline(*si.istr, line);
			if (!si.istr->good())
				throw MainDataInputException("unexpected end of file or read error", *si.pos);

			if (si.lineNums)
				MainData::getNextLong(line, si.pos);
			
			sy = new SymbolInfo();
            unsigned symkey = (unsigned) MainData::getNextLong(line, si.pos);
			sy->_name = data->globStrings()->at(si.mapStrings[MainData::getNextLong(line, si.pos)]);
			sy->_defId = MainData::getNextLong(line, si.pos);
			sy->_globId = MainData::getNextLong(line, si.pos);
            sy->_loc = *(si.mapLocsFrom[MainData::getNextLong(line, si.pos)]);
			sy->_predef = MainData::getNextLong(line, si.pos);
			sy->_subSymbol = (MainData::getNextLong(line, si.pos) != 0);
            sy->_predefCont = MainData::getNextLong(line, si.pos);
			sy->_readOnly = (MainData::getNextLong(line, si.pos) != 0);
			sy->_scopeType = MainData::getNextLong(line, si.pos);
			sy->_special = MainData::getNextLong(line, si.pos);

            (*_symbols)[symkey] = sy;

			si.pos->addLines();
		}

		// Elements
		subName = MainData::readSectionStart(si, 1, outLevel, subCnt, lc, rln);
		if (subName != "Elements")
			throw MainDataInputException("unexpected section, expecting 'Elements'", *si.pos);

		SyntaxElement **ses = new SyntaxElement*[subCnt];
		SyntaxElement *se;
		int prv;
		bool child;
        unsigned id;
        int tx;

		si.pos->addLines();
		for (i = 0; i < subCnt; i++) {
			getline(*si.istr, line);
			if (!si.istr->good())
				throw MainDataInputException("unexpected end of file or read error", *si.pos);

			if (si.lineNums)
				MainData::getNextLong(line, si.pos);
			
			prv = MainData::getNextLong(line, si.pos);
			child = (MainData::getNextLong(line, si.pos) != 0);
			id = MainData::getNextLong(line, si.pos);

			se = new SyntaxElement(id);
			se->_type = (SyntaxElementType) MainData::getNextLong(line, si.pos);

			tx = MainData::getNextLong(line, si.pos);
			se->_text = (tx >= 0 ? data->globStrings()->at(si.mapStrings[tx]) : NULL);

			tx = MainData::getNextLong(line, si.pos);
			se->_alttext = (tx >= 0 ? data->globStrings()->at(si.mapStrings[tx]) : NULL);

			se->_symId = MainData::getNextLong(line, si.pos);
            se->_loc = *(si.mapLocsFrom[MainData::getNextLong(line, si.pos)]);

			ses[i] = se;

			if (prv == -1)
				_root = se;
			else if (child)
				ses[prv]->_child = se;
			else
				ses[prv]->_next = se;

			si.pos->addLines();
		}

		delete ses;
	}
}

