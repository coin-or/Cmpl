
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


#include "SolverSelect.hh"
#include "SolverSelectMod.hh"
#include "../../Control/MainControl.hh"
#include "../../Modules/Interpreter/Interpreter.hh"



namespace cmpl
{
    /**
     * constructor
     * @param mod			module creating this extension object
     */
    SolverSelect::SolverSelect(SolverSelectMod *mod): ExtensionBase(mod)
    {
        _configFile = mod->data()->globStrings()->storeInd(SolverSelectMod::defaultConfigFile());
        _configFileLoc = LocationInfo(PositionInfo(POSITION_TYPE_DESCR, "default for solver selection config file"));

        _modelTypes = {
            { "LP",         OptModel::modelTypeLP },
            { "MIP",        OptModel::modelTypeMIP },
            { "QP",         OptModel::modelTypeQP },
            { "MIQP",       OptModel::modelTypeMIQP },
            { "NL",         OptModel::modelTypeNL },
        };

        _modelExt = {
            { "...",        -1 },
            { "..",         -1 },
            { "cond",       modelExtCondition },
            { "trivcond",   modelExtConditionTriv },
            { "intprod",    modelExtIntVarProd },
            { "binprod",    modelExtIntVarProdBin },
            { "sos",        modelExtSOS },
        };
    }

    /**
     * run the extension function for processing a command line option
     * @param mod			module calling the extension
     * @param step			execution step within the module
     * @param id			additional identificator
     * @param ref           reference number of option registration, should be used for discriminate the options
     * @param prio          priority value of option
     * @param opt           command line option
     * @param par			additional parameter
     * @return              true if option is used by the extension
     */
    bool SolverSelect::run(ModuleBase *mod, int step, int id, int ref, int prio, CmdLineOptList::SingleOption *opt, void *par)
    {
        switch (ref) {
            case OPTION_EXT_EXEC_SOLVERSEL_CONFIG_FILE:
                _configFile = 0;
                if (!opt->neg() && opt->size() == 1) {
                    _configFile = mod->data()->globStrings()->storeInd((*opt)[0]);
                    _configFileLoc = opt->loc(true);
                }
                return true;

            case OPTION_EXT_EXEC_SOLVERSEL_SOLVERNAME:
                _solver.clear();
                if (!opt->neg() && opt->size() == 1) {
                    _solver = (*opt)[0];
                }
                return true;
        }

        return false;
    }

    /**
	 * run the extension function
	 * @param mod			module calling the extension
	 * @param step			execution step within the module
	 * @param id			additional identificator
	 * @param par			additional parameter
	 */
	void SolverSelect::run(ModuleBase *mod, int step, int id, void *par)
	{
		if (step == EXT_STEP_INTERPRET_SOLVERSEL) {
			PROTO_MOD_OUTL(mod, " extension SolverSelect: " << step << " " << id);

            if (_configFile) {
                exec((Interpreter *)mod, (OptModel *)par);
            }
            else {
                PROTO_MOD_OUTL(mod, " no config file -> no solver selection");
            }
		}
		else {
			mod->ctrl()->errHandler().internalError(mod->ctrl()->printBuffer("invalid execution step for extension SolverSelect, called for '%s' %d %d", mod->moduleName(), step, id));
		}

		PROTO_MOD_OUTL(mod, " extension SolverSelect: end");
	}

	/**
	 * write data object to output stream
	 * @param si        serialization info
	 * @param data      main data object
	 */
	void SolverSelect::serializeTo(MainData::SerializeInfo& si, const MainData *data)
	{
		// extension has no own data
		//TODO: config file name
		(*si.ostr) << endl;
	}

	/**
	 * fill data object from input stream
	 * @param si        serialization info
	 * @param data      main data object
	 * @param subCnt    count of sub data objects
	 * @param lc		line count per element or 0 if elements are len coded
	 * @param rline		remaining section start line
	 */
	void SolverSelect::deserializeFrom(MainData::SerializeInfo& si, const MainData *data, int subCnt, int lc, string &rline)
	{
		// extension has no own data
		//TODO: config file name
		si.pos->addLines();
	}


    /**
     * execute solver selection by reading config file
     * @param modp      intepreter module calling the extension
     * @param om        optimization model
     */
    void SolverSelect::exec(Interpreter *modp, OptModel *om)
    {
        string filename = modp->data()->globStrings()->at(_configFile);

        try {
            FileInput inFile;
            istream *inStr = NULL;
            char dirSepChar = (modp->ctrl()->binFullName() ? FileBase::getDirSepChar(modp->ctrl()->binFullName()->c_str()) : '\0');
            bool secTry = (dirSepChar && filename.find(dirSepChar) == string::npos);

            // first try: search filename in working directory
            try {
                inFile.setFile(modp->data(), IO_MODE_FILE, &filename, NULL);
                inStr = inFile.open();
            }
            catch (FileException& e) {
                if (!secTry)
                    throw;
            }

            // second try: search filename in the directory of the executing binary
            if (!inStr) {
                try {
                    inFile.close();
                }
                catch(...) {}

                string file2(FileBase::replFileInPath(modp->ctrl()->binFullName(), &filename, dirSepChar));
                inFile.setFile(modp->data(), IO_MODE_FILE, &file2, NULL, true);
                inStr = inFile.open();
            }

            // init model type and extensions
            PROTO_MOD_OUTL(modp, "  init model type and extensions");
            initModelType(om);

            // execute using open config file
            PROTO_MOD_OUTL(modp, "  read config file: " << inFile.fileName());
            _section = StringStore::upperCase(_solver);
            exec(modp, om, inStr);

            inFile.close();
        }
        catch (FileException& e) {
            modp->ctrl()->errHandler().error(ERROR_LVL_NORMAL, modp->ctrl()->printBuffer("%s: solver selection config file '%s'", e.what(), filename.c_str()), _configFileLoc);
        }
    }

    /**
     * execute solver selection using open config file
     * @param modp      intepreter module calling the extension
     * @param om        optimization model
     * @param conf      input stream for reading open config file
     */
    void SolverSelect::exec(Interpreter *modp, OptModel *om, istream *conf)
    {
        // read config file
        string line;
        int lno = 0;
        bool inSect = false;
        size_t nms = _section.size();
        PositionInfo pos(&_configFileLoc, POSITION_TYPE_FILE, modp->data()->globStrings()->at(_configFile));

        PROTO_MOD_OUTL(modp, "  search for initial section [" << _section << "]");

        _cache[_kwlast] = false;
        _cache[_kwelse] = false;

        while (getline(*conf, line)) {
            size_t begin = line.find_first_not_of(StringStore::whiteSpaces());
            lno++;

            // skip empty lines and comment lines (starting with "#" or "//")
            if (begin != string::npos && line[begin] != '#' && (line.size() < begin + 2 || line[begin] != '/' || line[begin+1] != '/')) {
                if (line[begin] == '[') {
                    inSect = (line.size() > begin+nms+1 && line[begin+nms+1] == ']' && (nms == 0 || StringStore::upperCase(line.substr(begin+1, nms)) == _section));
                    if (inSect) {
                        PROTO_MOD_OUTL(modp, "  line " << lno << ": start of section [" << _section << "]");
                    }
                }
                else if (inSect) {
                    pos.set(lno);
                    if (execLine(modp, om, pos, line, begin)) {
                        inSect = false;
                        nms = _section.size();
                    }
                }
            }
        }
    }

    /**
     * execute for a line within the solver section of the config file
     * @param modp      intepreter module calling the extension
     * @param om        optimization model
     * @param pos       position within config file
     * @param line      line from config file
     * @param begin     start of non-whitespace within <code>line</code>
     * @return          true: break execution of current config file section
     */
    bool SolverSelect::execLine(Interpreter *modp, OptModel *om, PositionInfo& pos, string line, size_t begin)
    {
        // separate condition
        size_t i = line.find(':', begin);
        if (i == string::npos) {
            LocationInfo lce(pos);
            modp->ctrl()->errHandler().error(ERROR_LVL_EASY, "wrong line format in solver selection config file", lce);
            return true;
        }

        // eval condition
        bool cond;
        if (i == begin) {
            cond = true;
        }
        else {
            size_t end = line.find_last_not_of(StringStore::whiteSpaces(), i - 1) + 1;
            cond = evalBoolExpr(modp, om, pos, line, begin, end);
        }

        PROTO_MOD_OUTL(modp, "   line " << pos.line() << " (" << line.substr(0, i) << ") -> " << (cond ? "true" : "false"));

        _cache[_kwlast] = cond;
        _cache[_kwelse] = !cond;

        if (cond) {
            PositionInfo pp = pos + (i + 1);
            string action = line.substr(i + 1);
            i = action.find_first_not_of(StringStore::whiteSpaces());
            if (i != string::npos) {
                if (i > 0) {
                    pp += i;
                    action = action.substr(i);
                }

                PROTO_MOD_OUTL(modp, "    action: " << action);

                // check type of action
                size_t w = action.find_first_of(StringStore::whiteSpaces());
                if ((w == 5 && StringStore::startsWith(action, "ERROR")) || w == 7 && StringStore::startsWith(action, "WARNING")) {
                    // display error or warning
                    LocationInfo lce(pos);
                    string msg = StringStore::lTrim(action.substr(w + 1));
                    modp->ctrl()->errHandler().error((w == 5 ? ERROR_LVL_NORMAL : ERROR_LVL_WARN), msg.c_str(), lce);
                }
                else if (w == 4 && StringStore::startsWith(action, "GOTO")) {
                    _section = StringStore::upperCase(StringStore::lrTrim(action.substr(w + 1)));
                    return true;
                }
                else {
                    // add command line options
                    addCmdLineOpts(modp, action, pp);
                }
            }
        }

        return false;
    }

    /**
     * evaluate logical expression about boolean conditions about optimization model
     * @param modp      intepreter module calling the extension
     * @param om        optimization model
     * @param pos       position within config file
     * @param line      line from config file
     * @param begin     start of expression within <code>line</code>
     * @param end       after end of expression within <code>line</code>
     */
    bool SolverSelect::evalBoolExpr(Interpreter *modp, OptModel *om, PositionInfo& pos, string line, size_t begin, size_t end)
    {
        while (true)
        {
            if (begin == string::npos || end == string::npos || begin >= end) {
                LocationInfo lc(pos + begin);
                modp->ctrl()->errHandler().error(ERROR_LVL_NORMAL, "empty condition part in solver selection config file", lc);
                return false;
            }

            // handle logical "or": find next "or" not enclosed in brackets
            size_t bOr = line.find("||", begin);
            if (bOr != string::npos && bOr < end) {
                size_t bBr = line.find('(', begin);

                while (bBr != string::npos && bBr < bOr) {
                    size_t eBr = findMatchBracket(modp, pos, line, bBr, end);
                    bOr = line.find("||", eBr + 1);

                    if (bOr == string::npos || bOr >= end)
                        break;

                    bBr = line.find('(', eBr + 1);
                }

                if (bOr != string::npos && bOr < end) {
                    bool sub = evalBoolExpr(modp, om, pos, line, begin, line.find_last_not_of(StringStore::whiteSpaces(), bOr - 1) + 1);
                    if (sub)
                        return true;

                    begin = line.find_first_not_of(StringStore::whiteSpaces(), bOr + 2);
                    continue;
                }
            }

            // handle logical "not"
            bool neg = false;
            if (line[begin] == '!') {
                neg = true;
                begin = line.find_first_not_of(StringStore::whiteSpaces(), begin + 1);
                if (begin == string::npos || begin >= end)
                    continue;
            }

            bool sub = false;
            size_t bAnd = string::npos;

            // handle bracket
            if (line[begin] == '(') {
                size_t eBr = findMatchBracket(modp, pos, line, begin, end);
                sub = evalBoolExpr(modp, om, pos, line, line.find_first_not_of(StringStore::whiteSpaces(), begin + 1), line.find_last_not_of(StringStore::whiteSpaces(), eBr - 1) + 1);
                bAnd = line.find("&&", eBr + 1);
            }

            else {
                bAnd = line.find("&&", begin);
                sub = evalCondition(modp, om, pos, line, begin, (bAnd != string::npos && bAnd < end ? line.find_last_not_of(StringStore::whiteSpaces(), bAnd - 1) + 1 : end));
            }

            if (neg)
                sub = !sub;

            if (!sub || bAnd == string::npos || bAnd >= end)
                return sub;

            begin = line.find_first_not_of(StringStore::whiteSpaces(), bAnd + 2);
        }

        // (never reached)
    }

    /**
     * evaluate single boolean condition about optimization model
     * @param modp      intepreter module calling the extension
     * @param om        optimization model
     * @param pos       position within config file
     * @param line      line from config file
     * @param begin     start of condition name within <code>line</code>
     * @param end       after end of condition name within <code>line</code>
     */
    bool SolverSelect::evalCondition(Interpreter *modp, OptModel *om, PositionInfo& pos, string line, size_t begin, size_t end)
    {
        string cond = line.substr(begin, end - begin);

        map<string, bool>::iterator it = _cache.find(cond);
        if (it != _cache.end())
            return it->second;

        bool res = false;

        size_t br = cond.find('[');
        bool hasBr = (br != string::npos && cond[end-begin-1] == ']');
        string base = (hasBr ? StringStore::rTrim(cond.substr(0, br)) : cond);

        map<string, int>::iterator itMt = _modelTypes.find(base);
        if (itMt != _modelTypes.end()) {
            OptModel::ModelType mtp = (OptModel::ModelType)(itMt->second);
            if (!hasBr)
                res = getModelExtForType(om, mtp, NULL);
            else
                res = evalModelTypeAndExt(modp, om, pos, line, begin + br + 1, end - 1, mtp);
        }

        else {
            // TODO: other types of conditions


            LocationInfo lc(pos + begin, pos + end);
            modp->ctrl()->errHandler().error(ERROR_LVL_NORMAL, "unknown condition name in solver selection config file", lc);
        }

        _cache[cond] = res;
        return res;
    }

    /**
     * evaluate condition about model type with specified model extensions
     * @param modp      intepreter module calling the extension
     * @param om        optimization model
     * @param pos       position within config file
     * @param line      line from config file
     * @param begin     start of names of model extensions within <code>line</code>
     * @param end       after end of names of model extensions within <code>line</code>
     * @param mtp       given model type (is OptModel::ModelType)
     */
    bool SolverSelect::evalModelTypeAndExt(Interpreter *modp, OptModel *om, PositionInfo& pos, string line, size_t begin, size_t end, int mtp)
    {
        array<int, modelExt_Size> extConf;
        extConf.fill(0);

        // parse extension names
        size_t wb = begin;
        for (size_t i = begin; i <= end; i++) {
            if (i == end || line[i] == ',') {
                size_t we = line.find_last_not_of(StringStore::whiteSpaces(), i - 1) + 1;
                if (we > wb) {
                    int ev = -1;
                    if (line[we-1] == '!') {
                        ev = 1;
                        we = line.find_last_not_of(StringStore::whiteSpaces(), we - 2) + 1;
                    }

                    string ne = line.substr(wb, we - wb);
                    map<string, int>::iterator it = _modelExt.find(ne);

                    if (it != _modelExt.end()) {
                        if (it->second >= 0)
                            extConf[it->second] = ev;
                        else
                            extConf.fill(ev);
                    }
                    else {
                        LocationInfo lc(pos + wb, pos + we);
                        modp->ctrl()->errHandler().error(ERROR_LVL_NORMAL, "unknown model extension name in solver selection config file", lc);
                        return false;
                    }
                }

                if (i < end) {
                    wb = line.find_first_not_of(StringStore::whiteSpaces(), i + 1);
                    i = (wb != string::npos ? wb : end);
                }
            }
        }

        if (extConf[modelExtCondition] && !extConf[modelExtConditionTriv])
            extConf[modelExtConditionTriv] = -1;

        if (extConf[modelExtIntVarProd] && !extConf[modelExtIntVarProdBin])
            extConf[modelExtIntVarProdBin] = -1;

        // check model extensions
        array<int, modelExt_Size> extMod;
        if (!getModelExtForType(om, mtp, &extMod))
            return false;

        for (int i = 0; i < modelExt_Size; i++) {
            if ((extMod[i] == 1 && extConf[i] == 0) || (extMod[i] <= 0 && extConf[i] == 1))
                return false;
        }

        return true;
    }



    /**
     * search matching closing bracket for given opening bracket
     * @param modp      intepreter module calling the extension
     * @param pos       position within config file
     * @param line      line from config file
     * @param begin     position of opening bracket within <code>line</code>
     * @param end       end of search range within <code>line</code>
     * @return          position of closing bracket
     */
    size_t SolverSelect::findMatchBracket(Interpreter *modp, PositionInfo& pos, string line, size_t begin, size_t end)
    {
        char open = line[begin];
        char close = (open == '(' ? ')' : (open == '[' ? ']' : (open == '{' ? '}' : '\0')));
        if (!close) {
            LocationInfo lc(pos + begin);
            modp->ctrl()->errHandler().error(ERROR_LVL_NORMAL, "no opening bracket in condition expression in solver selection config file", lc);
            return end;
        }

        int lvl = 1;
        for (size_t i = begin + 1; i < end; i++) {
            char c = line[i];

            if (c == close) {
                lvl--;
                if (!lvl)
                    return i;
            }
            else if (c == open) {
                lvl++;
            }
        }

        LocationInfo lc(pos + begin, pos + end);
        modp->ctrl()->errHandler().error(ERROR_LVL_NORMAL, "no matching closing bracket in condition expression in solver selection config file", lc);
        return end;
    }


    /**
     * add command line options
     * @param modp      intepreter module calling the extension
     * @param opts      command line options to add
     * @param pos       position info within config file
     */
    void SolverSelect::addCmdLineOpts(Interpreter *modp, string& opts, PositionInfo &pos)
    {
        // construct command line option list
        CmdLineOptList cmdOptList;
        cmdOptList.addOption(opts, pos);

        // store command line option list, and use it also for this module
        modp->ctrl()->appendCmdLineOpts(&cmdOptList, CMDLINE_OPT_PARSE_HEADER, true, true);
    }


    /**
     * init model type and extensions
     * @param om        optimization model
     */
    void SolverSelect::initModelType(OptModel *om)
    {
        OptModel::Properties& prop = om->modelProp();
        if (prop.vartypes == 0)
            _omType = (prop.varprodReal ? OptModel::modelTypeQP : OptModel::modelTypeLP);
        else
            _omType = (prop.varprodReal ? OptModel::modelTypeMIQP : OptModel::modelTypeMIP);

        _omExt.fill(0);
        if (prop.conditions) {
            _omExt[modelExtCondition] = (prop.conditions == -1 ? -1 : 1);
            _omExt[modelExtConditionTriv] = (prop.conditions == -1 ? 1 : 0);
        }
        if (prop.varprodInt) {
            _omExt[modelExtIntVarProd] = (prop.varprodInt == 1 ? -1 : 1);
            _omExt[modelExtIntVarProdBin] = (prop.varprodInt == 1 ? 1 : 0);
        }
        if (prop.sos) {
            _omExt[modelExtSOS] = (prop.sos == 1 ? 1 : 0);
        }
    }

    /**
     * get model extensions if model is considered to have a given type
     * @param om        optimization model
     * @param itp       given model type (OptModel::ModelType)
     * @param ext       return of model extensions / NULL: check if model type is compatible without any extension
     * @return          false if given type is incompatible with model
     */
    bool SolverSelect::getModelExtForType(OptModel *om, int itp, array<int, modelExt_Size> *ext)
    {
        if (itp == _omType && ext) {
            *ext = _omExt;
            return true;
        }

        if (ext)
            ext->fill(0);

        OptModel::ModelType tp = (OptModel::ModelType)itp;
        OptModel::Properties& prop = om->modelProp();

        switch (tp) {
            case OptModel::modelTypeUnknown:
                break;

            case OptModel::modelTypeLP:
                if (_omType == OptModel::modelTypeLP && !ext) {
                    if (prop.conditions <= 0 && prop.sos <= 0)
                        return true;
                }
                break;

            case OptModel::modelTypeMIP:
                if ((_omType == OptModel::modelTypeLP || _omType == OptModel::modelTypeMIP) && !ext) {
                    if (_omType == OptModel::modelTypeMIP || prop.conditions > 0 || prop.sos > 0)
                        return true;
                }
                break;

            case OptModel::modelTypeQP:
                if (_omType == OptModel::modelTypeQP && !ext) {
                    if (prop.conditions <= 0 && prop.sos <= 0)
                        return true;
                }
                break;

            case OptModel::modelTypeMIQP:
                if (_omType == OptModel::modelTypeQP || _omType == OptModel::modelTypeMIQP || prop.varprodInt > 0) {
                    if (_omType == OptModel::modelTypeMIP || _omType == OptModel::modelTypeMIQP || (!ext && (prop.conditions > 0 || prop.sos > 0)))
                        return true;
                }
                break;

            case OptModel::modelTypeNL:
                return false;   // no other non-linear model type implemented yet
        }

        return false;
    }
}


