
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


#include "DataImportBase.hh"
#include "../../Control/MainControl.hh"
#include "../../Modules/Precompiler/Precompiler.hh"
#include "../../Modules/Interpreter/Interpreter.hh"



namespace cmpl
{
	/**
	 * run the extension function
	 * @param mod			module calling the extension
	 * @param step			execution step within the module
	 * @param id			additional identificator
	 * @param par			additional parameter
	 */
	void DataImportBase::run(ModuleBase *mod, int step, int id, void *par)
	{
        if (step == EXT_STEP_INTERPRET_EXTDATA_IMPORT) {
            PROTO_MOD_OUTL(mod, " extension " << extName() << ": import data: " << step << " " << id);
            import(mod, (ImportExternDataInfo *)par);
        }

        else {
            mod->ctrl()->errHandler().internalError(mod->ctrl()->printBuffer("invalid execution step for extension %s, called for '%s' %d %d", extName(), mod->moduleName(), step, id));
        }

        PROTO_MOD_OUTL(mod, " extension " << extName() << ": end");
    }

	/**
	 * write data object to output stream
	 * @param si        serialization info
	 * @param data      main data object
	 */
	void DataImportBase::serializeTo(MainData::SerializeInfo& si, const MainData *data)
	{
        // write instance variables
        (*si.ostr) << ';' << _extSrcType << ';' << _handlePrio << endl;
    }

	/**
	 * fill data object from input stream
	 * @param si        serialization info
	 * @param data      main data object
	 * @param subCnt    count of sub data objects
	 * @param lc		line count per element or 0 if elements are len coded
	 * @param rline		remaining section start line
	 */
	void DataImportBase::deserializeFrom(MainData::SerializeInfo& si, const MainData *data, int subCnt, int lc, string &rline)
	{
        // read instance variables
        _extSrcType = MainData::getNextLong(rline, si.pos);
        _handlePrio = MainData::getNextLong(rline, si.pos);

        si.pos->addLines();
    }



    /**
     * get new error info object
     * @param lvl       error level (one of ERROR_LVL_*)
     * @param txt       error message
     * @param loc       location info of the error location
     * @param locs      vector to hold location objects
     * @return          new error info object
     */
    DataImportBase::ErrorInfo *DataImportBase::newErrorInfo(int lvl, const char *txt, LocationInfo& loc, vector<LocationInfo*>& locs)
    {
        LocationInfo *lc = new LocationInfo(loc);
        locs.push_back(lc);

        DataImportBase::ErrorInfo *ei = new DataImportBase::ErrorInfo(lvl, txt, lc);
        return ei;
    }

    /**
     * get new error info object
     * @param lvl       error level (one of ERROR_LVL_*)
     * @param txt       error message
     * @param pos       position info of the start of error location
     * @param len       length of error location
     * @param locs      vector to hold location objects
     * @return          new error info object
     */
    DataImportBase::ErrorInfo *DataImportBase::newErrorInfo(int lvl, const char *txt, PositionInfo& pos, size_t len, vector<LocationInfo*>& locs)
    {
        LocationInfo *lc;
        if (len) {
            PositionInfo pe = pos + len;
            lc = new LocationInfo(pos, pe);
        }
        else {
            lc = new LocationInfo(pos);
        }

        locs.push_back(lc);

        DataImportBase::ErrorInfo *ei = new DataImportBase::ErrorInfo(lvl, txt, lc);
        return ei;
    }


    /**
     * output word with position to precompiler result stream
     * @param p         precompiler module
     * @param nms       namespace / NULL: don't use namespace here
     * @param defSym    defined symbols to prefix with the namespace
     */
    void DataImportBase::TxtWord::outWithPos(Precompiler *p, const char *nms, vector<string *> *defSym)
    {
        if (prv)
            prv->outWithPos(p, nms, defSym);

        LocationInfo locS(pos, pos + len);
        PositionInfo posS(&locS, POSITION_TYPE_FIXLOC);

        p->resStream() << endl;
        p->outStartPos(posS);

        if (nms && defSym && defSym->size() > 0) {
            // prefix referenced symbols in word with namespace
            string wr(word);
            for (string *s : *defSym)
                prefixNamespace(wr, *s, nms);

            p->resStream() << wr;
        }
        else {
            p->resStream() << word;
        }
    }


    /**
     * search for unqualified references of a symbol within a word and prefix the symbol reference with a namespace
     * @param wr        word
     * @param s         symbol
     * @param nms       namespace
     */
    void DataImportBase::TxtWord::prefixNamespace(string& wr, string& s, const char *nms)
    {
        if (nms) {
            size_t p = wr.find(s);
            if (p != string::npos) {
                bool b;
                if (p > 0) {
                    char c = wr[p-1];
                    b = !(isalnum(c) || isdigit(c) || c == '_' || c == '.');
                }
                else
                    b = true;

                bool e;
                if (p + s.size() < wr.size()) {
                    char c = wr[p + s.size()];
                    e = !(isalnum(c) || isdigit(c) || c == '_');
                }
                else
                    e = true;

                if (b && e) {
                    wr = wr.substr(0, p) + nms + '.' + wr.substr(p);
                }
            }
        }
    }


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
    DataImportBase::TxtWord *DataImportBase::scanWord(string& inp, size_t& p, bool qt, bool& br, const char *stc, char& c, PositionInfo& pst, DataImportBase::TxtWord *cont)
    {
        c = 0;
        if (p == string::npos || p > inp.size()) {
            p = string::npos;
            return cont;
        }

        size_t ps = inp.find_first_not_of(WHITE_SPACES, p);
        if (ps == string::npos) {
            p = string::npos;
            return cont;
        }

        DataImportBase::TxtWord *res = NULL;
        size_t pwe = 0;
        string openBr;
        bool openQt = false;
        bool quoted = (cont ? cont->openQt : false);


        if ((br && (inp[ps] == '(' || inp[ps] == '[' || inp[ps] == '{')) || (cont && !(cont->openBr.empty()))) {
            // word within brackets
            bool brError = false;
            bool inQuotes = quoted;
            string brackets;
            if (cont)
                brackets = cont->openBr;

            for (p = ps; p < inp.size(); p++) {
                if (!inQuotes) {
                    if (inp[p] == '(' || inp[p] == '[' || inp[p] == '{') {
                        brackets.push_back(inp[p]);
                    }
                    else if (inp[p] == ')' || inp[p] == ']' || inp[p] == '}') {
                        char sbr = (inp[p] == ')' ? '(' : (inp[p] == ']' ? '[' : '{'));
                        if (sbr == brackets.back()) {
                            brackets.pop_back();
                            if (brackets.empty())
                                break;
                        }
                        else {
                            // non matching bracket
                            brError = true;
                            break;
                        }
                    }
                    else if (inp[p] == '"') {
                        inQuotes = true;
                    }
                }
                else if (inp[p] == '"') {
                    if (p < inp.size()-1 && inp[p+1] == '"')
                        p++;
                    else
                        inQuotes = false;
                }
            }

            if (p < inp.size())
                p++;

            pwe = p;

            if (brError) {
                // non matching bracket
                br = false;
            }
            else {
                c = ' ';
                openBr = brackets;
                openQt = inQuotes;
            }
        }

        else if (quoted || (qt && inp[ps] == '"')) {
            // quoted word
            p = ps;
            if (!quoted) {
                quoted = true;
                p++;
            }

            p = inp.find('"', p);

            if (p == string::npos || p == inp.size() - 1) {
                openQt = (p == string::npos);
                pwe = (openQt ? inp.size() : p) + 1;
                p = string::npos;
            }
            else {
                p++;
                if (inp[p] != '"') {
                    openQt = false;
                    pwe = p;
                }
                else {
                    string resWord(inp, ps+1, p-ps-1);
                    openQt = true;

                    for (p++; p < inp.size(); p++) {
                        if (inp[p] == '"') {
                            p++;
                            if (p == inp.size() || inp[p] != '"') {
                                openQt = false;
                                break;
                            }
                        }

                        resWord.push_back(inp[p]);
                    }

                    res = new TxtWord(resWord, p-ps, pst, ps, cont);
                    if (openQt)
                        res->setUnfinishedQt();
                }

                c = ' ';
            }
        }

        else {
            // word without quotes or brackets
            if (stc) {
                p = inp.find_first_of(stc, ps);
            }
            else {
                for (p = ps; p < inp.size(); p++) {
                    if (!isalnum(inp[p]) && inp[p] != '_' && inp[p] != '.')
                        break;
                }
            }

            if (p > ps) {
                pwe = (p == string::npos || p > inp.size() ? inp.size() : p);
                if (p < inp.size())
                    c = ' ';
            }
            else if (p < inp.size()) {
                c = inp[p++];
            }
        }


        if (pwe && !res) {
            res = new DataImportBase::TxtWord(inp, ps, pwe, quoted, pst, cont);
            if (openQt)
                res->setUnfinishedQt();
            if (!openBr.empty())
                res->setUnfinishedBr(openBr);
        }

        if (p >= inp.size())
            p = string::npos;

        return (res ?: cont);
    }


    /**
     * get directory with data file
     * @param p             precompiler module
     * @param dataFile      data file
     * @param dataDir       return of data directory
     * @param chgToDataDir  return whether to change dir to dataDir for opening the data file
     */
    void DataImportBase::getDataDir(Precompiler *p, FileInput *dataFile, string& dataDir, bool& chgToDataDir)
    {
        dataDir.clear();
        chgToDataDir = false;

#if CHDIR
        p->includeInit();

        if (!(dataFile->hasNameAbsPath(p->dirSepChar())))
        {
            if (p->InInclFiles()) {
                // within an included file the working directory is already changed, use it
                dataDir = p->getCurrentWd();
            }
            else {
                // in a main level file within another directory use the directory part of filename
                if (p->curPos() && p->curPos()->isFile()) {
                    string mfn = p->curPos()->name();
                    p->splitPathName(mfn, dataDir);
                    chgToDataDir = true;
                }
            }

            if (!dataDir.empty() && dataDir[dataDir.size()-1] != p->dirSepChar())
                dataDir += p->dirSepChar();
        }
#endif
    }
}


