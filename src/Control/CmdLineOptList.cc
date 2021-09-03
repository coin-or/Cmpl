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


#include "CmdLineOptList.hh"
#include "MainControl.hh"
#include "ModulesConf.hh"
#include "MainData.hh"


namespace cmpl
{
	/***********************************************************************
	 *  Usage example:
	 *
	 *  {
	 *  	cmpl::CmdLineOptList *cl = cmpl::Start::getNewEmptyCmdLineOptList();
	 *  	cmpl::PositionInfo p1(...), p2(...);
	 *		cl->addOption("i", p1)->arg("abc.cmpl", p2);
	 *
	 * 		...
	 *
	 * 		cmpl::Start::dispose(cl);
	 *	}
	 ***********************************************************************/


	/* **************** SingleOption ************* */

	/**
	 * set option name
	 * @param n			option name, can be "[module.][no-]option[!...][=arg1,...]"
	 * @param pos		start position of the option, only for error messages
	 */
	void CmdLineOptList::SingleOption::setOpt(string& n, PositionInfo& pos)
	{
		size_t p;
		int pe = (pos.useCharCount() ? n.size() : (pos.useWordCount() ? 1 : 0));
		int pc = pe;
		string o = (!n.empty() && n[0] == '-' ? n.substr(1) : n);

		// test module specification
		if ((p = o.find(".")) != string::npos) {
			_module = o.substr(0, p);
			o = o.substr(p+1);
		}
		else {
			_module.clear();
		}

		// test negation
		_neg = false;
		if (o.size() > 3 && o.substr(0, 3) == "no-") {
			_neg = true;
			o = o.substr(3);
		}

		// test arguments
		if ((p = o.find("=")) != string::npos) {
			if (pos.useCharCount())
				pc -= (o.size() - p);

			PositionInfo posA = (pos.useCharCount() ? pos + (pc + 1) : pos);

			_opt = o.substr(0, p);
			string a = o.substr(p+1);

			_sepArgStart = 1;

			while ((p = a.find(",")) != string::npos) {
				string b = a.substr(0, p);
				arg(b, posA);

				a = a.substr(p+1);
				_sepArgStart++;

				if (pos.useCharCount())
					posA += (p + 1);
			}

			arg(a, posA);
		}
		else {
			_opt = o;
		}
		
		// test explicit priority
		_prio = 0;
		while (_opt.size() > 1 && _opt[_opt.size() - 1] == '!') {
			_prio++;
			_opt.erase(_opt.size() - 1);
		}

		// set location
		_locName = LocationInfo(pos, pos+pc);
		_locOpt = (pc==pe ? _locName : LocationInfo(pos, pos+pe));
	}


	/**
	 * check whether this option is equal to another option
	 * @param o			other option
	 * @return			true if options are equal
	 */
	bool CmdLineOptList::SingleOption::isEqual(CmdLineOptList::SingleOption& o)
	{
		// compare all instance variables, but not _useCount
		if (_opt != o._opt || _module != o._module
				|| _neg != o._neg || _prio != o._prio || _sepArgStart != o._sepArgStart
				|| _args.size() != o._args.size()
				|| _locName != o._locName || _locOpt != o._locOpt)
			return false;

		if (_args.size() > 0) {
			vector<string>::iterator it = _args.begin();
			vector<string>::iterator it2 = o._args.begin();
			for (; it != _args.end(); it++, it2++) {
				if (*it != *it2)
					return false;
			}
		}

		return true;
	}


	/**
	 * add an arg for an option
	 * @param a			argument (can also contain spaces)
	 * @param pos		start position of the arg, only for error messages
	 * @return			this
	 */
	CmdLineOptList::SingleOption* CmdLineOptList::SingleOption::arg(string& a, PositionInfo& pos)
	{
		_args.push_back(a);
		_argPos.push_back(pos);

		//_locOpt = _locOpt + LocationInfo(pos + (pos.useCharCount() ? a.size() : (pos.useWordCount() ? 1 : 0)));
		_locOpt += (pos + (pos.useCharCount() ? a.size() : (pos.useWordCount() ? 1 : 0)));
		return this;
	}

	/**
	 * get an argument as int number, error if arg is not an int.
	 * @param i			index
	 */
	int CmdLineOptList::SingleOption::argAsInt(size_t i, MainControl *ctrl)
	{
		int res = 1;

		try {
			res = stoi(_args[i]);
		}
		catch (exception) {
			ctrl->errHandler().error(ERROR_LVL_NORMAL, ctrl->printBuffer("argument '%s' for option '%s' is not an int number", _args[i].c_str(), _opt.c_str()), _locOpt);
		}	

		return res;
	}

	/**
	 * test whether option is valid for names in <code>modNames</code>
	 * @param modNames  relevant names for module specification in command line options
	 * @return          option is valid with this level of module specification / -1: not valid for the module
	 */
	int CmdLineOptList::SingleOption::testModName(vector<const char *> &modNames)
	{
		if (_module.empty())
			return 0;

		int lvl = 1;
		for (vector<const char *>::iterator it = modNames.begin(); it != modNames.end(); it++, lvl++) {
			if (_module == *it)
				return lvl;
		}

		return -1;
	}



	/* **************** CmdLineOptList ************* */

	/**
	 * clean up this object
	 */
	void CmdLineOptList::cleanUp()
	{
		for (size_t i = 0; i < _opts.size(); i++)
			delete _opts[i];
		_opts.clear();

		if (_modConf) {
			delete _modConf;
			_modConf = NULL;
		}

        _binFullName.clear();
	}

	/**
	 * fill this with command line options as supplied from main()
	 */
	void CmdLineOptList::fillFromMain(int argc, char *argv[])
	{
		PositionInfo pos(POSITION_TYPE_WORDARR, "command line");
		SingleOption *sa = NULL;

        fillBinFullName(argc > 0 ? argv[0] : NULL);

		for (int i = 1; i < argc; i++) {
			string arg = argv[i];
			pos.set(i);

			if (arg[0] == '-')
				sa = addOption(arg, pos, true);
			else if (sa)
				sa->arg(arg, pos);
            else {
                string o = "i";
                sa = addOption(o, pos, true)->arg(arg, pos);
            }
		}
	}

    /**
     * fill full name of executed binary
     * @param arg       name of executed binary as given in first command line argument of main function / NULL: no such
     */
    void CmdLineOptList::fillBinFullName(char *arg)
    {
        if (arg)
            _binFullName = arg;

        // use enviroment CMPLHOME instead, if given and arg doesn't contain a path
        char *cmplBin = getenv("CMPLHOME");
        if (cmplBin && (!arg || _binFullName.find_first_of("/\\") == string::npos)) {
            _binFullName = cmplBin;
            if (StringStore::contains(cmplBin,"/")) {
                _binFullName+="bin/cmpl";
            } else {
                _binFullName+="bin\\cmpl.exe";
            }
        }
    }

	/**
	 * add a new option to this
	 * @param opt		option string, the option can have several args separated by spaces
	 * @param pos		start position of option, only for error messages
	 * @param single	<code>opt</code> contains only of one single option keyword without args
	 * @return			info object for the option
	 */
	CmdLineOptList::SingleOption* CmdLineOptList::addOption(string& opt, PositionInfo& pos, bool single)
	{
		if (opt.empty())
			return NULL;

		// iterate over words in opt
		size_t wn = 0;
		string word;
		bool quote;
		SingleOption *sa = NULL;
		PositionInfo posA = pos;

		while ((single && !sa) || (!single && StringStore::iterWords(opt, word, wn, quote))) {
			if (single)
				word = opt;
			else if (pos.useCharCount())
				posA = pos + ((wn == string::npos ? opt.size() : wn) - word.size());

			if (!sa || (!quote && word.size() >= 2 && word[0] == '-')) {
				// start of new option
				sa = new CmdLineOptList::SingleOption(this);
				sa->setOpt(word, posA);

				_opts.push_back(sa);
			}
			else {
				// arg for the option
				sa->arg(word, posA);
			}

			if (pos.useWordCount())
				posA += 1;
		}

		return sa;
	}

	/**
	 * delete an option from this, including its args
	 * @param optInfo	info object for the option
	 * @return			true if the option is found and deleted
	 */
	bool CmdLineOptList::delOption(CmdLineOptList::SingleOption* optInfo)
	{
		if (optInfo && optInfo->_cmdLineOptList == this) {
			for (size_t i = 0; i < _opts.size(); i++) {
				if (_opts[i] == optInfo) {
					_opts.erase(_opts.begin() + i);
					delete optInfo;
					return true;
				}
			}
		}

		return false;
	}


	/**
	 * append command line options from another CmdLineOptList object
	 * @param src      source of command line options to append
	 */
	void CmdLineOptList::append(CmdLineOptList& src)
	{
		for (size_t i = 0; i < src._opts.size(); i++) {
			CmdLineOptList::SingleOption *sa = new CmdLineOptList::SingleOption(*(src._opts[i]));
			sa->_cmdLineOptList = this;
			_opts.push_back(sa);
		}
	}
			 

	/**
	 * iterate to the first or next relevant option
     * @param modNames  relevant names for module specification in command line options / NULL: don't check module name
	 * @param iter      current iteration index, start with -1
	 * @param modspec	return level of module specification
	 * @return          next option or NULL if no one is left
	 */
    CmdLineOptList::SingleOption* CmdLineOptList::iterOptions(vector<const char *> *modNames, int &iter, int &modspec)
	{
		if (iter < 0)
			iter = -1;

		for (iter++; iter < (int)_opts.size(); iter++) {
            if (!modNames || (modspec = _opts[iter]->testModName(*modNames)) >= 0)
				return _opts[iter];
		}

		return NULL;
	}


	/**
	 * check if all options in _opts are used by modules
	 * @param ctrl      main object
     * @param mark      mark this command line options as used
     * @param warn      output warning if a command line option ist not used
     */
    void CmdLineOptList::checkCmdLineOptUsed(MainControl *ctrl, set<string>& mark, bool warn)
	{
        for (size_t i = 0; i < _opts.size(); i++) {
			if (_opts[i]->_useCount == 0) {
                if (mark.count(_opts[i]->opt()))
                    _opts[i]->markUsed();
                else if (warn)
                    ctrl->outWarnUnusedCmdLineOpt(_opts[i]);
			}
		}
	}

    /**
     * write all options to a command line option file
     * @param ostr      stream for writing
     * @param tp        type code (1: command line / 2: cmpl header)
     * @param nowrite   don't write this options
     * @param check     check if this option is written
     * @return          whether checked option is written
     */
    bool CmdLineOptList::writeToOptFile(ostream *ostr, int tp, set<string> &nowrite, const char *check)
    {
        bool found = false;
        for (size_t i = 0; i < _opts.size(); i++) {
            SingleOption *opt = _opts[i];
            if (check && opt->opt() == check)
                found = true;

            if (!nowrite.count(opt->opt()))
                writeToOptFile(ostr, tp, opt);
        }

        return found;
    }

    /**
     * write single option to a command line option file
     * @param ostr      stream for writing
     * @param tp        type code (1: command line / 2: cmpl header)
     * @param opt       option to write
     */
    void CmdLineOptList::writeToOptFile(ostream *ostr, int tp, SingleOption *opt)
    {
        *ostr << opt->_opt << ';' << opt->_module << ';' << tp << ';';

        const PositionInfo& p1 = opt->_locName.begin();
        const PositionInfo& p2 = opt->_locOpt.end();
        if (p1.type() == POSITION_TYPE_WORDARR && p2.hasCol())
            *ostr << p1.col() << ';' << (p2.col() - 1) << ';';
        else if ((p1.type() == POSITION_TYPE_FILE || p1.type() == POSITION_TYPE_STREAM) && p2.hasCol())
            *ostr << StringStore::quoteString(p1.name()) << ',' << p1.line() << ',' << p1.col() << ';' << p2.col() << ';';
        else
            *ostr << ";;";

        *ostr << (opt->_neg ? 1 : 0) << ';' << opt->_prio << ';' << opt->_sepArgStart << ';' << opt->_useCount << ';' << opt->_args.size();

        vector<string>::iterator its = opt->_args.begin();
        vector<PositionInfo>::iterator itl = opt->_argPos.begin();
        for (; its != opt->_args.end(); its++, itl++) {
            *ostr << ';' << StringStore::quoteString(*its) << ';';
            if (itl->hasCol())
                *ostr << itl->col();
        }

        *ostr << endl;
    }

    /**
     * write single pseudo option to a command line option file
     * @param ostr      stream for writing
     * @param tp        type code (0: pseudo option)
     * @param name      name option to write
     * @param arg       argument for option / NULL: no argument
     * @param fixloc    string for fixed location of option / NULL: no location
     */
    void CmdLineOptList::writeToOptFile(ostream *ostr, int tp, const char *name, const char *arg, const char *fixloc)
    {
        *ostr << name << ';' << ';' << tp << ';';

        if (fixloc)
            *ostr << StringStore::quoteString(fixloc);
        *ostr << ";;";

        *ostr << "0;0;0;0;";
        if (arg)
            *ostr << "1;" << StringStore::quoteString(arg) << ';';
        else
            *ostr << "0";

        *ostr << endl;
    }


    /**
     * write all options from a command line option file
     * @param ctrl		main object
     * @param istr      stream for reading
     * @param fpos      position info of command line option file
     * @param opts1     command line option list for type code 1
     * @param opts2     command line option list for type code 2
     * @param resname   if a option with this name and type code 0 is found then return its first parameter
     * @return          found parameter value according resname or empty string
     */
    string CmdLineOptList::readFromOptFile(MainControl *ctrl, istream *istr, PositionInfo& fpos, CmdLineOptList& opts1, CmdLineOptList& opts2, const char *resname)
    {
        string res;
        string l, word;
        bool quote;
        const char *sep = ";";

        int line = 0;
        while (getline(*istr, l)) {
            fpos.set(++line);

            if (!l.empty() && !isspace(l[0]) && l[0] != '#') {
                int nr = 1;
                size_t pos = 0;
                fpos.setCol(pos + 1);

                SingleOption opt(NULL);
                int tp, cnt = -1;
                string oloc1;

                try {
                    while (StringStore::iterWords(l, word, pos, quote, '"', sep, false, true, true)) {
                        switch (nr) {
                            case 1: opt._opt = word; break;
                            case 2: opt._module = word; break;
                            case 3: tp = stoi(word); break;
                            case 4: oloc1 = word; break;
                            case 5: parseLocFromOptFile(ctrl, opt, tp, true, &oloc1, &word); break;
                            case 6: opt._neg = (stoi(word) != 0); break;
                            case 7: opt._prio = stoi(word); break;
                            case 8: opt._sepArgStart = stoi(word); break;
                            case 9: opt._useCount = stoi(word); break;
                            case 10: cnt = stoi(word); break;
                            case 11: opt._args.push_back(StringStore::unquoteString(word)); break;
                            case 12: parseLocFromOptFile(ctrl, opt, tp, false, &word); cnt--; nr -= 2; break;
                        }

                        nr++;
                        fpos.setCol(pos + 1);
                    }
                }
                catch (...) {
                    throw FileException("invalid format", fpos.name());
                }
                if (cnt) {
                    throw FileException("wrong count of arguments", fpos.name());
                }

                if (tp == 0 && resname && opt._opt == resname) {
                    res = opt._args[0];
                }
                else {
                    CmdLineOptList& optlst = (tp >= 2 ? opts2 : opts1);
                    optlst._opts.push_back(new SingleOption(&opt, &optlst));
                }
            }
        }

        return res;
    }

    /**
     * parse location information for one option from a command line option file
     * @param ctrl		main object
     * @param opt       option
     * @param tp        type code (1: command line / 2: cmpl header / 0: internal)
     * @param ol        true: location for option, using locs1 and locs2 / false: position for argument of option, using only locs1
     * @param locs1     first location info string
     * @param locs2     second location info string
     */
    void CmdLineOptList::parseLocFromOptFile(MainControl *ctrl, SingleOption& opt, int tp, bool ol, string *locs1, string *locs2)
    {
        if (!locs1 || locs1->empty() || StringStore::lrTrim(*locs1).empty())
            locs1 = NULL;
        if (!locs2 || locs2->empty() || StringStore::lrTrim(*locs2).empty())
            locs2 = NULL;

        if (ol) {
            if (locs1) {
                int nr = 0;
                size_t pos = 0;
                const char *sep = ",";
                string word, s1, s2, s3;
                bool quote, q1;

                while (StringStore::iterWords(*locs1, word, pos, quote, '"', sep, false, true, false)) {
                    nr++;
                    switch (nr) {
                        case 1: s1 = word; q1 = quote; break;
                        case 2: s2 = word; break;
                        case 3: s3 = word; break;
                    }
                }

                if (nr == 3) {
                    PositionInfo p(POSITION_TYPE_FILE, ctrl->data()->globStrings()->store(StringStore::unquoteString(s1)));
                    p.set(stoi(s2), stoi(s3));

                    opt._locName.setFrom(p, p);
                    opt._locName.columns(opt._opt.size());

                    opt._locOpt.setFrom(p, p);
                    if (locs2)
                        opt._locOpt.columns(stoi(*locs2) + 1 - stoi(s2));
                    else
                        opt._locOpt.columns(opt._opt.size());
                }

                else if (nr == 1 && q1) {
                    PositionInfo p(POSITION_TYPE_DESCR, ctrl->data()->globStrings()->store(StringStore::unquoteString(s1)));
                    opt._locName.setFrom(p, p);
                    opt._locOpt.setFrom(p, p);
                }

                else if (nr == 1 && tp <= 1) {
                    PositionInfo p(POSITION_TYPE_WORDARR, "command line");
                    p.set(stoi(s1));

                    opt._locName.setFrom(p, p);
                    opt._locName.columns(1);

                    opt._locOpt.setFrom(p, p);
                    if (locs2)
                        opt._locOpt.columns(stoi(*locs2) + 1 - stoi(s1));
                    else
                        opt._locOpt.columns(1);
                }
            }
        }

        else {
            const PositionInfo& opos = opt._locOpt.begin();
            if (opos.hasCol() && locs1) {
                PositionInfo apos = opos;
                apos.setCol(stoi(*locs1));
                opt._argPos.push_back(apos);
            }
            else {
                opt._argPos.emplace_back();
            }
        }
    }



	/****** functions for serialization ****/

	/**
	 * insert strings and locations used from this in the data object
	 */
	void CmdLineOptList::insertStringsAndLocs(MainData *data)
	{
		StringStore *s = data->globStrings();

		for (vector<SingleOption *>::iterator it = _opts.begin(); it != _opts.end(); it++) {
			SingleOption *opt = *it;

			s->store(opt->_opt);
			s->store(opt->_module);

			opt->_locName = data->persistentLocation(opt->_locName);
			data->searchInsertInGlobLocs(&(opt->_locName));

			opt->_locOpt = data->persistentLocation(opt->_locOpt);
			data->searchInsertInGlobLocs(&(opt->_locOpt));

			for (vector<string>::iterator its = opt->_args.begin(); its != opt->_args.end(); its++) {
				s->store(*its);
			}

			for (vector<PositionInfo>::iterator itl = opt->_argPos.begin(); itl != opt->_argPos.end(); itl++) {
				data->persistentPosition(*itl);
				LocationInfo loc(*itl);
				data->searchInsertInGlobLocs(&loc);
			}
		}
	}

	/**
	 * write data object to output stream
	 * @param si		serialization info
	 * @param data		main data object
	 */
	void CmdLineOptList::serializeTo(MainData::SerializeInfo& si, const MainData *data)
	{
		StringStore *s = data->globStrings();
		int i = 0;

		for (vector<SingleOption *>::iterator it = _opts.begin(); it != _opts.end(); it++) {
			SingleOption *opt = *it;

			if (si.lineNums)
				(*si.ostr) << i++ << '#';

			(*si.ostr) << s->search(opt->_opt) << ';' << s->search(opt->_module) << ';' << 
                data->searchLoc(si, &(opt->_locName)) << ';' << data->searchLoc(si, &(opt->_locOpt)) << ';' <<
				(opt->_neg ? 1 : 0) << ';' << opt->_prio << ';' << opt->_sepArgStart << ';' << opt->_useCount << ';' <<
				opt->_args.size();

			vector<string>::iterator its = opt->_args.begin();
			vector<PositionInfo>::iterator itl = opt->_argPos.begin();
			for (; its != opt->_args.end(); its++, itl++) {
				LocationInfo loc(*itl);
                (*si.ostr) << ';' << s->search(*its) << ';' << data->searchLoc(si, &loc);
			}

			(*si.ostr) << endl;
		}
	}

	/**
	 * fill data object from input stream
	 * @param si		serialization info
	 * @param data		main data object
	 * @param subCnt    count of sub data objects
	 */
	void CmdLineOptList::deserializeFrom(MainData::SerializeInfo& si, const MainData *data, int subCnt)
	{
		string line;

		for (int i = 0; i < subCnt; i++) {
			// read option line from serialization stream
			getline(*si.istr, line);
			if (!si.istr->good())
				throw MainDataInputException("unexpected end of file or read error", *si.pos);

			if (si.lineNums)
				MainData::getNextLong(line, si.pos);

			SingleOption *opt = new SingleOption(this);
			opt->_opt = data->globStrings()->at(si.mapStrings[MainData::getNextLong(line, si.pos)]);
			opt->_module = data->globStrings()->at(si.mapStrings[MainData::getNextLong(line, si.pos)]);
            opt->_locName = *(si.mapLocsFrom[MainData::getNextLong(line, si.pos)]);
            opt->_locOpt = *(si.mapLocsFrom[MainData::getNextLong(line, si.pos)]);
			opt->_neg = (MainData::getNextLong(line, si.pos) != 0);
			opt->_prio = MainData::getNextLong(line, si.pos);
			opt->_sepArgStart = MainData::getNextLong(line, si.pos);
			opt->_useCount = MainData::getNextLong(line, si.pos);

			unsigned ac = MainData::getNextLong(line, si.pos);
			if (ac > 0) {
				opt->_args.reserve(ac);
				opt->_argPos.reserve(ac);

				for (unsigned a = 0; a < ac; a++) {
					opt->_args.push_back(string(data->globStrings()->at(si.mapStrings[MainData::getNextLong(line, si.pos)])));
                    opt->_argPos.push_back(si.mapLocsFrom[MainData::getNextLong(line, si.pos)]->begin());
				}
			}

			// check if option already exists
			SingleOption *o = NULL;
			for (vector<SingleOption *>::iterator it = _opts.begin(); it != _opts.end(); it++) {
				if ((*it)->isEqual(*opt)) {
					o = *it;
					break;
				}
			}

			if (o) {
				// option already exists
				if (o->_useCount < opt->_useCount)
					o->_useCount = opt->_useCount;

				delete opt;
			}
			else {
				// insert option
				_opts.push_back(opt);
			}

			si.pos->addLines();
		}
	}

}

