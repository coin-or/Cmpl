
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


#include "ModulesConf.hh"
#include "MainControl.hh"


namespace cmpl
{
	/* **************** ModulesConf ************* */

	/**
	 * destructor
	 */
	ModulesConf::~ModulesConf()
	{
		// free _modAggr
		for (unordered_map<const char *, vector<ModConfInfo *>>::iterator it = _modAggr.begin(); it != _modAggr.end(); it++) {
			vector<ModConfInfo *> &m = it->second;

			for (size_t i = 0; i < m.size(); i++)
				delete m[i];
		}
	}


	/**
	 * fills <code>_modules</code>
	 */
	void ModulesConf::initModuleNames()
	{
		#include "../modules.reg"
	}


	/**
	 * read module configuration file
	 * @param file      filename of the module configuration file
	 * @param loc		location of name of configuration file
	 */
	void ModulesConf::readModuleConf(string *file, LocationInfo &loc)
	{
		_filename = file;
		_locConf = loc;

		try {

			FileInput inFile;
            istream *inStr = NULL;
            char dirSepChar = (_ctrl->binFullName() ? FileBase::getDirSepChar(_ctrl->binFullName()->c_str()) : '\0');
            bool secTry = (dirSepChar && _filename->find(dirSepChar) == string::npos);

            // first try: search _filename in working directory
            try {
                inFile.setFile(_ctrl->_data, IO_MODE_FILE, _filename, NULL);
                inStr = inFile.open();
            }
            catch (FileException& e) {
                if (!secTry)
                    throw;
            }

            // second try: search _filename in the directory of the executing binary
            if (!inStr) {
                try {
                    inFile.close();
                }
                catch(...) {}

                string file2(FileBase::replFileInPath(_ctrl->binFullName(), _filename, dirSepChar));
                inFile.setFile(_ctrl->_data, IO_MODE_FILE, &file2, NULL, true);
                inStr = inFile.open();
                *file = file2;
            }

			string l, s;
			const char *n;
			vector<ModConfInfo *> *ma = NULL;
			int col, line = 0;
			size_t i;
			PositionInfo pos(&_locConf, POSITION_TYPE_FILE, _filename->c_str());

			while (getline(*inStr, l)) {
				line++;
				s = StringStore::lTrim(l);

				// skip empty lines and comment lines (starting with "#" or "//")
				if (!s.empty() && s[0] != '#' && (s.size() < 2 || s[0] != '/' || s[1] != '/')) {
					col = l.size() - s.size();

					if (col == 0) {
						// line starts in first column: contains first new module aggregation name and second the module aggregation description
						i = s.find_first_of(StringStore::whiteSpaces());
						n = _modNames.store(StringStore::rTrim(s.substr(0, i)));
						ma = &_modAggr[n];
						
						if (i != string::npos)
							_modDescr.push_back(ModDescr(n, true, StringStore::lrTrim(s.substr(i))));
					}
					else {
						pos.set(line, col+1);
						if (!ma) {
							LocationInfo lc(pos);
							_ctrl->errHandler().error(ERROR_LVL_EASY, "missing module aggregation start", lc);
						}

						ma->push_back(new ModConfInfo(s, pos));
					}
				}
			}

			inFile.close();
		}
		catch (FileException& e) {
            _ctrl->errHandler().error(ERROR_LVL_NORMAL, _ctrl->printBuffer("%s: module config file '%s'", e.what(), _filename->c_str()), loc);
		}
	}

	/**
	 * get module to which this belongs
	 */
	ModuleBase* ModulesConf::modp()
	{
		return _ctrl;
	}

	/**
	 * search a module name and give back the stored name, throw exception if not found.
	 * @param name          module name to search
	 * @return              module name as stored string
	 */
	const char* ModulesConf::testModuleName(const char *name)
	{
		bool found;
		unsigned i = _modNames.searchInsert(name, false, false, found);

		if (!found)
			throw ModuleNotFoundException();

		return _modNames[i];
	}

	/**
	 * parse module name from command line options, and fill ctrl->_modules
	 * @param name      module name from command line options, can also be "alias=modulName[,orderNo]"
	 * @param err       throw error if <code>name</code> is no valid module name
	 * @param pos		position of module name, should have type POSITION_TYPE_FIXLOC or POSITION_TYPE_DESCR
	 * @return          <code>name</code> contains a module name
	 */
	bool ModulesConf::parseCmdLineOptModule(string& name, bool err, PositionInfo& pos)
	{
		ModConfInfo info(name, pos);
		return testParseCmdLineOptModule(info, err);
	}

	/**
	 * parse module name from command line options, and fill ctrl->_modules
	 * @param name      possible module name from command line options
	 * @param err       throw error if <code>name</code> is no valid module name
	 * @param opt       command line options for module
	 * @return          <code>name</code> contains a module name
	 */
	bool ModulesConf::parseCmdLineOptModule(string& name, bool err, CmdLineOptList::SingleOption *opt)
	{
		PositionInfo pos(&(opt->loc(true)), POSITION_TYPE_FIXLOC);

		if (opt->size() > 0) {
			string s = name;
			for (size_t i=0; i < opt->size(); i++) {
				s += (i==0 ? '=' : ',');
				s += (*opt)[i];
			}

			return parseCmdLineOptModule(s, err, pos);
		}
		else {
			return parseCmdLineOptModule(name, err, pos);
		}
	}


	/**
	 * test whether <code>opt</code> contains a module name, parse it and fill ctrl->_modules
	 * @param info		module info
	 * @param err       show error if <code>info.module</code> is no valid module name
	 * @param modNames	vector with rekursive module/module aggregation names / NULL: no such vector
	 * @return          <code>info.module</code> contains a module name
	 */
	bool ModulesConf::testParseCmdLineOptModule(ModConfInfo& info, bool err, vector<const char *> *modNames)
	{
		fillModConfInfoName(info);

		// test whether a module
		if (_modules.count(info.module) == 1) {
			PROTO_OUTL("insert module: " << info.module);
			fillModConfInfoAll(info, false);

			int o = (info.order == -1 ? _modules[info.module].order : info.order);
			_ctrl->addModule(info.module, info.alias, o, info.opts, modNames);

			return true;
		}

		// test whether a module aggregation
		else if (_modAggr.count(info.module) == 1) {
			PROTO_OUTL("insert module aggregation: " << info.module);
			fillModConfInfoAll(info, true);

			_ctrl->addModuleAggr(info.module, info.opts, modNames);

			return true;
		}

		else if (err) {
			LocationInfo loc = (info.pos.useCharCount() ? LocationInfo(info.pos, info.pos + strlen(info.module)) : LocationInfo(info.pos));
			_ctrl->errHandler().error(ERROR_LVL_WARN, _ctrl->printBuffer("'%s' is not a known module name, ignored", info.module), loc);
		}

		return false;
	}

	/**
	 * insert recursive modules for a module aggregation
	 * @param mod		name of module aggregation
	 * @param modNames	vector with rekursive module/module aggregation names / NULL: no such vector
	 */
	void ModulesConf::insertRekModules(const char *mod, vector<const char *> *modNames)
	{
		vector<ModConfInfo *> &mods = _modAggr[mod];

		for (size_t i = 0; i < mods.size(); i++) {
			testParseCmdLineOptModule(*(mods[i]), true, modNames);
		}
	}

	/**
	 * fill field <code>module</code> in ModConfInfo from <code>ModConfInfo::rawValue</code> if necessary.
	 * @param info      module info
	 */
	void ModulesConf::fillModConfInfoName(ModConfInfo& info)
	{
		if (info.mode >= 1)
			return;

		size_t p;
		string s;
		if ((p = info.rawValue.find("=")) != string::npos)
			s = info.rawValue.substr(0, p);
		else if ((p = info.rawValue.find(" ")) != string::npos || (p = info.rawValue.find("\t")) != string::npos)
			s = info.rawValue.substr(0, p);
		else
			s = info.rawValue;

		info.module = _modNames.store(s);
		info.mode = 1;
	}

	/**
	 * fill all fields in ModConfInfo from <code>ModConfInfo::rawValue</code> if necessary.
	 * @param info      module info
	 * @param aggr		info contains a module aggregation
	 */
	void ModulesConf::fillModConfInfoAll(ModConfInfo& info, bool aggr)
	{
		if (info.mode >= 2)
			return;

		if (info.mode == 0)
			fillModConfInfoName(info);

		size_t p, p2;
		string m;
		string opt;
		int optPosCnt = 0;

		info.modAggr = aggr;

		if ((p = info.rawValue.find_first_of(StringStore::whiteSpaces())) != string::npos) {
			m = info.rawValue.substr(0, p);
			opt = info.rawValue.substr(p+1);
			optPosCnt = p + 1; 
		}
		else {
			m = info.rawValue;
		}

		// parse alias and order
		if ((p = m.find("=")) != string::npos) {
			string b = m.substr(p+1);

			if ((p2 = b.find(",")) != string::npos) {
				try {
					info.order = stoi(b.substr(p2+1));
				}
				catch (exception) {
					LocationInfo loc = (info.pos.useCharCount() ? LocationInfo(info.pos + (p+1), info.pos + (p+1+p2)) : LocationInfo(info.pos));
					_ctrl->errHandler().error(ERROR_LVL_EASY, _ctrl->printBuffer("order number '%s' is not an int", b.substr(p2+1).c_str()), loc);
				}

                b = b.substr(0, p2);
			}
			else {
				info.order = -1;
			}

			info.alias = _modNames.store(b);
		}
		else {
			info.alias = NULL;
			info.order = -1;
		}

		// set command line options
		if (!opt.empty()) {
			PositionInfo optPos = info.pos + (info.pos.useCharCount() ? optPosCnt : (info.pos.useWordCount() ? 1 : 0));
			parseCmdLineOptList(info, opt, optPos);
		}

		info.mode = 2;
	}

	/**
	 * fill <code>ModConfInfo::opts</code> from string with command line options
	 * @param info		module info
	 * @param opt		string with command line options
	 * @param pos		start position of string <code>opt</code>
	 */
	void ModulesConf::parseCmdLineOptList(ModConfInfo& info, string opt, PositionInfo& pos)
	{
		// iterate over words in opts
		size_t p = 0;
		string word;
		bool quote;
		PositionInfo posA = pos;
		CmdLineOptList::SingleOption *sa = NULL;

		while (StringStore::iterWords(opt, word, p, quote)) {
			if (pos.useCharCount())
				posA = pos + ((p == string::npos ? opt.size() : p) - word.size());

			if (word.size() >= 2 && word[0] == '-') {
				// start of new option
				string o = info.alias ?: info.module;
				o += '.';
				o += word.substr(1);
				sa = info.opts.addOption(o, posA, true);
			}
			else if (sa) {
				sa->arg(word, posA);
			}
			else {
				LocationInfo loc = (pos.useCharCount() ? LocationInfo(posA, posA + word.size()) : LocationInfo(posA));
				_ctrl->errHandler().error(ERROR_LVL_NORMAL, _ctrl->printBuffer("option string starts with non-option '%s'", word.c_str()), loc);
			}

			if (pos.useWordCount())
				posA += 1;
		}
	}

}

