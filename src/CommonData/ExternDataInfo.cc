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


#include "ExternDataInfo.hh"
#include "../Control/ModulesConf.hh"

using namespace std;


namespace cmpl
{
	// defines data class
    DATA_CLASS( ExternDataInfo )



    /**
     * destructor
     */
    ExternDataInfo::~ExternDataInfo()
    {
        cleanUp();
    }

    /**
	 * delete all data
	 */
	void ExternDataInfo::cleanUp()
	{
        for (map<int, EIRecord*>::iterator it = _info.begin(); it != _info.end(); it++) {
            EIRecord *r = it->second;
            delete r;
        }

        _info.clear();
	}

	/**
	 * get count of sub data objects
	 */
	int ExternDataInfo::subCnt()
	{
        return _info.size();
	}

	/**
	 * check if serialization to or from stream should be done for the data object
	 * @param si		serialization info
	 * @param data		main data object
	 * @return			true if serialization should be done
	 */
	bool ExternDataInfo::checkSerialize(MainData::SerializeInfo& si, const MainData *data)
	{
		return (!si.toFromServer || (si.toFromServer == 1 && (si.serverStep < MODULE_CONF_ORDER_CMPL_PARSE || HAS_DATA(data, IntCode))));
	}

	/**
	 * write data object to output stream
	 * @param si		serialization info
	 * @param data		main data object
	 */
	void ExternDataInfo::serializeTo(MainData::SerializeInfo& si, const MainData *data)
	{
		int i = 0;
		(*si.ostr) << endl;

        for (map<int, EIRecord*>::iterator it = _info.begin(); it != _info.end(); it++) {
            if (si.lineNums)
                (*si.ostr) << i++ << '#';

            EIRecord *r = it->second;
            (*si.ostr) << r->_intsym << ';' << r->_extsym << ';' << r->_srctype << ';' << r->_srcname << ';' << (r->_srcfile ? 1 : 0) << ';' << (r->_optval ? 1 : 0) << ';' << r->_addstr1 << ';' << r->_addstr2 << ';' << r->_srcpos << endl;
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
	void ExternDataInfo::deserializeFrom(MainData::SerializeInfo& si, const MainData *data, int subCnt, int lc, string &rline)
	{
		string line;
        int ns, es, st, sn, fn, ov, a1, a2;
		long sp;

		si.pos->addLines();

		for (int i = 0; i < subCnt; i++) {
			getline(*si.istr, line);
			if (!si.istr->good())
				throw MainDataInputException("unexpected end of file or read error", *si.pos);

			if (si.lineNums)
				MainData::getNextLong(line, si.pos);
			
            ns = si.mapStrings[MainData::getNextLong(line, si.pos)];
            es = si.mapStrings[MainData::getNextLong(line, si.pos)];
			st = MainData::getNextLong(line, si.pos);
			sn = si.mapStrings[MainData::getNextLong(line, si.pos)];
            fn = MainData::getNextLong(line, si.pos);
            ov = MainData::getNextLong(line, si.pos);
            a1 = MainData::getNextLong(line, si.pos);
            a2 = MainData::getNextLong(line, si.pos);
			sp = MainData::getNextLong(line, si.pos);

            if (a1 != -1)
                a1 = si.mapStrings[a1];
            if (a2 != -1)
                a2 = si.mapStrings[a2];

            _info[ns] = new EIRecord(ns, es, st, sn, (fn!=0), (ov!=0), a1, a2, sp);

			si.pos->addLines();
		}
	}


    /**
     * get info about an extern data symbol
     * @param intsym	symbol name including namespace (as index in global string store)
     * @return			info about an extern data symbol / NULL: not found
     */
    ExternDataInfo::EIRecord *ExternDataInfo::getExternInfo(int intsym)
    {
        if (_info.count(intsym) == 0)
            return NULL;
        else
            return _info[intsym];
    }

	/**
	 * get info about an extern data symbol
	 * @param data		main data object
     * @param nmssym	symbol name including namespace
     * @param tp		return type of extern data source
	 * @param sname		return name of extern data source (as index in global string store)
	 * @param sp		return position in extern data source if known / -1: not known
	 * @return			true if symbol is found
	 */
    bool ExternDataInfo::getExternInfo(MainData *data, const char *nmssym, int& tp, int& sname, long& sp)
	{
        int intsym = data->globStrings()->storeInd(nmssym);
        EIRecord *ei = getExternInfo(intsym);
        if (!ei)
			return false;

        tp = ei->_srctype;
        sname = ei->_srcname;
        sp = ei->_srcpos;

		return true;
	}

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
    bool ExternDataInfo::getExternInfo(MainData *data, const char *nmssym, int& tp, int& sname, bool& file, bool& optval, int& a1, int& a2, long& sp)
    {
        int intsym = data->globStrings()->storeInd(nmssym);
        EIRecord *ei = getExternInfo(intsym);
        if (!ei)
            return false;

        tp = ei->_srctype;
        sname = ei->_srcname;
        file = ei->_srcfile;
        optval = ei->_optval;
        a1 = ei->_addstr1;
        a2 = ei->_addstr2;
        sp = ei->_srcpos;

        return true;
    }


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
    bool ExternDataInfo::insertExternInfo(MainData *data, const char *intsym, const char *extsym, int tp, const char *sname, bool file, bool optval, const char *as1, const char *as2, long sp)
    {
        int ns, es, sn, a1, a2;
        ns = data->globStrings()->storeInd(intsym);
        es = data->globStrings()->storeInd(extsym);
		sn = data->globStrings()->storeInd(sname);
        a1 = (as1 ? data->globStrings()->storeInd(as1) : -1);
        a2 = (as2 ? data->globStrings()->storeInd(as2) : -1);

        bool ins = (_info.count(ns) == 0);
		if (ins) {
            _info[ns] = new EIRecord(ns, es, tp, sn, file, optval, a1, a2, sp);
		}
		else {
            EIRecord* r = _info[ns];
            if (es != r->_extsym || tp != r->_srctype || sn != r->_srcname || file != r->_srcfile || optval != r->_optval || a1 != r->_addstr1 || a2 != r->_addstr2 || (sp != -1 && r->_srcpos != -1 && sp != r->_srcpos))
				throw ExternDataInfoInconsistentException();

            if (r->_srcpos == -1 && sp != -1)
                r->_srcpos = sp;
		}

		return ins;
	}


	/**
	 * write to a stream for output
	 * @param ostr		output stream
	 * @param data		main data object
	 */
	void ExternDataInfo::writeTo(ostream& ostr, MainData *data)
	{
        for (map<int, EIRecord*>::iterator it = _info.begin(); it != _info.end(); it++) {
            EIRecord *r = it->second;

            ostr << data->globStrings()->at(r->_intsym) << ';' << data->globStrings()->at(r->_extsym) << ';' << r->_srctype << ';'
                 << StringStore::quoteString(data->globStrings()->at(r->_srcname)) << ';'
                 << (r->_srcfile ? 1 : 0) << ';' << (r->_optval ? 1 : 0) << ';';

            if (r->_addstr1 >= 0)
                ostr << StringStore::quoteString(data->globStrings()->at(r->_addstr1)) << ';';
            else
                ostr << ';';

            if (r->_addstr2 >= 0)
                ostr << StringStore::quoteString(data->globStrings()->at(r->_addstr2)) << ';';
            else
                ostr << ';';

            ostr << r->_srcpos << endl;
		}
	}


	/**
	 * read from a stream for input
	 * @param istr		input stream
	 * @param data		main data object
	 */
	void ExternDataInfo::readFrom(istream& istr, MainData *data)
	{
		cleanUp();

		string line, word;
        string ns, es, sname, as1, as2;
        int tp, file, optval;
		long sp;
		int nr;
		size_t pos;
		bool quote;
        const char *sep = ";";

		while (getline(istr, line)) {
			if (!line.empty()) {
                pos = 0;
				nr = 1;
                while (StringStore::iterWords(line, word, pos, quote, '"', sep, false, true)) {
					switch (nr) {
                        case 1: ns = word; break;
                        case 2: es = word; break;
						case 3: tp = stoi(word); break;
						case 4: sname = word; break;
                        case 5: file = stoi(word); break;
                        case 6: optval = stoi(word); break;
                        case 7: as1 = word; break;
                        case 8: as2 = word; break;
                        case 9: sp = stol(word); break;
					}

					nr++;
				}

                insertExternInfo(data, ns.c_str(), es.c_str(), tp, sname.c_str(), (file != 0), (optval != 0), as1.c_str(), as2.c_str(), sp);
            }
		}
	}


	/**
     * get set with names of data files
	 * @param data      main data object
	 * @param filenames set to fill file names in
	 */
    void ExternDataInfo::getDataFileNames(MainData *data, set<const char *> &filenames)
	{
		StringStore *ss = data->globStrings();

        for (map<int, EIRecord*>::iterator it = _info.begin(); it != _info.end(); it++) {
            EIRecord *r = it->second;
            if (r->_srcfile)
                filenames.insert(ss->at(r->_srcname));
		}
	}

}

