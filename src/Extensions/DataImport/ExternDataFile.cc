
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

#include <cctype>
#include <unistd.h>

#include "ExternDataFile.hh"
#include "../../Control/MainControl.hh"
#include "../../Control/MainData.hh"
#include "../../CommonData/ExternDataInfo.hh"
#include "../../CommonData/CmplSourceInput.hh"
#include "../../Modules/Precompiler/Precompiler.hh"
#include "../../Modules/Interpreter/Interpreter.hh"
#include "../../Modules/Interpreter/ExecContext.hh"
#include "../../Modules/Interpreter/TupleUtil.hh"
#include "../../Modules/Interpreter/SetUtil.hh"



namespace cmpl
{

    /****** ExternDataFile ****/

    /**
     * write data object to output stream
     * @param si        serialization info
     * @param data      main data object
     */
    void ExternDataFile::serializeTo(MainData::SerializeInfo& si, const MainData *data)
    {
        // write instance variables
        (*si.ostr) << ';' << _extSrcType << ';' << _handlePrio << ';' << (_readAll ? 1 : 0) << ';' << (_assArraySub ? 1 : 0) << endl;
    }

    /**
     * fill data object from input stream
     * @param si        serialization info
     * @param data      main data object
     * @param subCnt    count of sub data objects
     * @param lc		line count per element or 0 if elements are len coded
     * @param rline		remaining section start line
     */
    void ExternDataFile::deserializeFrom(MainData::SerializeInfo& si, const MainData *data, int subCnt, int lc, string &rline)
    {
        // read instance variables
        _extSrcType = MainData::getNextLong(rline, si.pos);
        _handlePrio = MainData::getNextLong(rline, si.pos);
        _readAll = (MainData::getNextLong(rline, si.pos) != 0);
        _assArraySub = (MainData::getNextLong(rline, si.pos) != 0);

        si.pos->addLines();
    }


    /**
     * destructor
     */
    ExternDataFile::~ExternDataFile()
    {
        for (auto x : _files)
            delete x.second;
    }


    /************** handle registered actions **********/

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
    bool ExternDataFile::run(ModuleBase *mod, int step, int id, int ref, int prio, CmdLineOptList::SingleOption *opt, void *par)
    {
        switch (ref) {
            case OPTION_EXT_EXEC_PRECOMP_DATFILE:
                _cdatDefault = (opt->neg() ? NULL : mod->data()->globStrings()->store((*opt)[0]));
                return true;

            case OPTION_EXT_EXEC_PRECOMP_DATA:
            case OPTION_EXT_EXEC_PRECOMP_DATA2:
                {
                    DataImportExtensionPar *dpar = (DataImportExtensionPar *)par;
                    ExternDataFileHandleOpt *edf;
                    dpar->setData((edf = new ExternDataFileHandleOpt((Precompiler *)mod, this, opt, (ref == OPTION_EXT_EXEC_PRECOMP_DATA2))));
                    dpar->setHandlePrio(edf->checkData());
                }
                return true;

            case OPTION_EXT_EXEC_PRECOMP_ASSARRSUB:
                _assArraySub = !(opt->neg());
                break;

            case OPTION_EXT_EXEC_INTERPRET_READALL:
                _readAll = !(opt->neg());
                return true;
        }

        return false;
    }



    /************** import of data **********/

    /**
     * read all symbols from a data file
     * @param fh            file handling object
     * @param inStr         stream for reading data file
     * @param fn            name of data file
     * @param nm            namespace name / -1: no namespace
     * @param loc			location of data header line
     */
    void ExternDataFile::readFileAllSymbols(ExternDataFileHandleOpt *fh, istream *inStr, string& fn, int nm, LocationInfo& loc)
    {
        string sep = StringStore::whiteSpaces() + "[<";
        string l, s;
        streampos p;
        size_t i1, i2;

        while (getline(*inStr, l)) {
            if (!l.empty()) {
                p = inStr->tellg() - (streamoff)(inStr->gcount());

                i1 = l.find_first_not_of(StringStore::whiteSpaces());
                if (i1 != string::npos && l[i1] == '%') {
                    i2 = l.find_first_of(sep, i1);
                    s = (i2 != string::npos ? l.substr(i1 + 1, i2 - i1 - 1) : l.substr(i1));
                    s = StringStore::lTrim(s);

                    insertDataSymbol(fh, nm, s, fn, loc, (long)p);
                }
            }
        }
    }

    /**
     * insert data symbol in info about extern data
     * @param fh            file handling object
     * @param nm            namespace name / -1: no namespace
     * @param sym           symbol name
     * @param fn            file name of cmpl data file
     * @param loc           location of data symbol in input file
     * @param sp            position of symbol in the data file / -1: not known
     * @return              true if symbol is inserted to info
     */
    void ExternDataFile::insertDataSymbol(ExternDataFileHandleOpt *fh, int nm, string& sym, string& fn, LocationInfo& loc, long sp)
    {
        if (fh->insertDataSymbol(nm, sym, fn, loc, sp)) {
            if (nm >= 0)
                fh->_mod->resStream() << fh->data()->globStrings()->at(nm) << '.';
            fh->_mod->resStream() << sym << "; ";
        }
    }

    /**
     * import data values
     * @param mod       module calling the extension
     * @param ei		info about external data to import
     */
    void ExternDataFile::import(ModuleBase *mod, ImportExternDataInfo *ei)
    {
        int filename = ei->externInfo->srcname();
        ExternDataFileImport *ed;

        if (_files.count(filename)) {
            ed = _files[filename];
        }
        else {
            ed = new ExternDataFileImport(mod, this, ei);
            _files[filename] = ed;
        }

        ed->import(ei->result, ei->externInfo);
    }



    /****** ExternDataFileHandleOpt ****/

    /**
     * destructor
     */
    ExternDataFileHandleOpt::~ExternDataFileHandleOpt()
    {
        deleteInfo();
    }

    /**
     * delete info
     */
    void ExternDataFileHandleOpt::deleteInfo()
    {
        if (_filename) {
            delete _filename;
            _filename = NULL;
        }

        if (_namespace) {
            delete _namespace;
            _namespace = NULL;
        }

        for (unsigned i = 0; i < _symbols.size(); i++) {
            if (_symbols[i]) {
                delete _symbols[i];
                _symbols[i] = NULL;
            }
        }
        _symbols.clear();

        if (_errorInfo) {
            delete _errorInfo;
            _errorInfo = NULL;
        }

        for (unsigned i = 0; i < _locations.size(); i++) {
            delete _locations[i];
        }
        _locations.clear();
    }

    /**
     * check command line option for external data
     * @return       priority of handling the given external data import by this extension
     */
    int ExternDataFileHandleOpt::checkData()
    {
        unsigned ocf = 0;

        if (!_optWithColon) {
            // skip first arg if it is the order number (handled already by the precompiler module)
            if (_opt->sepArgStart() == 1 && _opt->size() > 1)
                ocf = 1;

            // check whether first arg is a valid file name
            if (_opt->size() > ocf) {
                // get file name
                string& fn = (*_opt)[ocf];
                FileInput dataFile;
                dataFile.setFile(ctrl()->data(), IO_MODE_FILE, &fn, IO_FILE_STANDARD_CDAT);
                dataFile.setAliases(&(_mod->fileAlias()));

                // get directory name if necessary
                string dataDir;
                bool chgToDataDir = false;
                DataImportBase::getDataDir(_mod, &dataFile, dataDir, chgToDataDir);

                // try to open file
                string ffn = dataDir + dataFile.fileNameReplaced();
                FILE *fp = fopen(ffn.c_str(), "r");
                if (fp) {
                    fclose(fp);

                    int res = checkData(true, ocf);
                    if (!_errorInfo)
                        return res;

                    deleteInfo();
                }
            }
        }

        return checkData(false, ocf);
    }

    /**
     * check command line option for external data
     * @param fnf   true: first arg is used as filename / false: scan also first arg for words
     * @param ocf   start of scanned args
     * @return      priority of handling the given external data import by this extension
     */
    int ExternDataFileHandleOpt::checkData(bool fnf, unsigned ocf)
    {
        unsigned mode = (_optWithColon ? 1 : 0);      // 0: filename/namespace; 1: symbol name; 2: index or type assertion
        unsigned cnt = 0;
        unsigned oc;
        DataImportBase::TxtWord *cont = NULL;

        // scan args
        for (oc = ocf; oc < _opt->size(); oc++) {
            string& inp = (*_opt)[oc];
            size_t p = 0;
            char splitChar = ' ';

            PositionInfo& posArg = _opt->argPos(oc);
            PositionInfo posInArg(POSITION_TYPE_STRING);
            if (!posArg.hasLine()) {
                LocationInfo *loc = new LocationInfo(posArg);
                _locations.push_back(loc);
                posInArg.setInLoc(loc);
                posInArg.set(inp.c_str());
            }
            PositionInfo& posInfo = (posArg.hasLine() ? posArg : posInArg);

            if (fnf) {
                _filename = new DataImportBase::TxtWord(inp, 0, inp.length(), false, posInfo);
                cnt++;
                fnf = false;
                continue;
            }

            while (splitChar) {
                bool bracket = (mode == 2);
                DataImportBase::TxtWord *word = DataImportBase::scanWord(inp, p, (mode==0 && cnt==0), bracket, (mode==0 && cnt==0 ? WHITE_SPACES_AND(":") : NULL), splitChar, posInfo, cont);
                cont = NULL;

                if (word) {
                    if (word->unfinished()) {
                        if (mode == 2) {
                            cont = word;
                            word = NULL;
                        }
                        else {
                            _errorInfo = DataImportBase::newErrorInfo(ERROR_LVL_NORMAL, ctrl()->printBuffer("unfinished word '%s'", word->word.c_str()), word->pos, word->len, _locations);
                            delete word;
                        }
                        break;
                    }
                    else if (mode == 0) {
                        if (cnt == 0)
                            _filename = word;
                        else if (cnt == 1)
                            _namespace = word;
                        else {
                            _errorInfo = DataImportBase::newErrorInfo(ERROR_LVL_NORMAL, ctrl()->printBuffer("':' expected before external symbol names, at '%s'", word->word.c_str()), word->pos, 0, _locations);
                            delete word;
                            break;
                        }
                    }
                    else if (mode == 1) {
                        _symbols.push_back(word);
                        mode = 2;
                    }
                    else {
                        if (!bracket) {
                            _errorInfo = DataImportBase::newErrorInfo(ERROR_LVL_NORMAL, ctrl()->printBuffer("non matching bracket within '%s'", word->word.c_str()), word->pos, word->len, _locations);
                            delete word;
                            break;
                        }
                        else if (word->startChar() == '[' && cnt == 1) {
                            _symbols.push_back(word);
                        }
                        else {
                            if (cnt == 1) {
                               _symbols.push_back(NULL);
                               cnt++;
                            }

                            if (cnt == 2) {
                                _symbols.push_back(word);
                            }
                            else {
                                DataImportBase::TxtWord *w = _symbols.back();
                                word->setPrv(w);
                                _symbols.back() = word;
                            }
                        }
                    }

                    word = NULL;
                    cnt++;
                }

                if (splitChar) {
                    if (!isspace(splitChar)) {
                        if (mode == 0 && splitChar == ':') {
                            mode = 1;
                            cnt = 0;
                        }
                        else if (mode == 2 && splitChar == ',') {
                            mode = 1;
                            cnt = 0;
                            while (_symbols.size() % 3)
                                _symbols.push_back(NULL);
                        }
                        else {
                            PositionInfo pe = posInfo + p;
                            _errorInfo = DataImportBase::newErrorInfo(ERROR_LVL_NORMAL, ctrl()->printBuffer("unexpected '%c'", splitChar), pe, 0, _locations);
                            break;
                        }
                    }
                }
            }

            if (_errorInfo)
                break;
        }

        if (cont) {
            LocationInfo loc = cont->location();
            _errorInfo = DataImportBase::newErrorInfo(ERROR_LVL_NORMAL, (cont->openQt ? "unclosed quote" : "unclosed bracket"), loc, _locations);
            delete cont;
        }
        else if (mode == 0) {
            _allSymbols = true;
        }
        else if (mode == 1 && _symbols.empty() && !_errorInfo) {
            _errorInfo = new DataImportBase::ErrorInfo(ERROR_LVL_WARN, "no symbol names given after ':', no external data will be read", &(_opt->loc(true)));
        }
        else {
            while (_symbols.size() % 3)
                _symbols.push_back(NULL);
        }

        int prio = ((ExternDataFile *)_ext)->handlePrio();
        if (_errorInfo)
            prio = -_errorInfo->level;

        return prio;
    }

    /**
     * run handling of command line option for external data import
     * @return      true if option is handled here
     */
    bool ExternDataFileHandleOpt::run()
    {
        if (_errorInfo) {
            ctrl()->errHandler().error(_errorInfo->level, _errorInfo->msg.c_str(), *(_errorInfo->locInfo));
            if (_errorInfo->level != ERROR_LVL_WARN)
                return false;
        }

        // data file
        string fn, *fnp = NULL;
        if (_filename) {
            fnp = &(_filename->word);
        }
        else if (((ExternDataFile *)_ext)->cdatDefault()) {
            fn = ((ExternDataFile *)_ext)->cdatDefault();
            fnp = &fn;
        }

        FileInput dataFile;
        dataFile.setFile(ctrl()->data(), IO_MODE_FILE, fnp, ((ExternDataFile *)_ext)->cdatStandard());
        dataFile.setAliases(&(_mod->fileAlias()));

        // get directory name if necessary
        string dataDir;
        bool chgToDataDir = false;
        DataImportBase::getDataDir(_mod, &dataFile, dataDir, chgToDataDir);

        // use a fixed location
        LocationInfo& loc = _opt->loc(true);
        PositionInfo pos(&loc, POSITION_TYPE_FIXLOC);
        _mod->outStartPos(pos);

        // start cmpl code
        _mod->resStream() << "{:" << endl;

        // namespace
        int nm = -1;
        if (_namespace) {
            // iterate over namespace parts
            string& nss = _namespace->word;
            size_t p = 0;
            while (p != string::npos) {
                p = nss.find_first_of('.', p + 1);
                nm = data()->globStrings()->storeInd(p == string::npos ? nss : nss.substr(0, p));
                if (!_mod->testHeaderDataNamespaces(nm, true))
                    _mod->resStream() << "\tpublic const container " << data()->globStrings()->at(nm) << ';' << endl;
            }
        }

        if (_allSymbols) {
            // no individual symbols given, use all symbols from data file
            processHeaderDataAllSymbols(dataFile, nm, loc, dataDir, chgToDataDir);
        }

        else if (!_symbols.empty()) {
            // individual symbols
            processHeaderDataGivenSymbols(dataFile, nm, dataDir);
        }

        _mod->resStream() << "};" << endl;

        PositionInfo *curPos = _mod->curPos();
        if (curPos)
            _mod->outStartPos(*curPos);

        return true;
    }

    /**
     * process data header line for all symbols from a data file
     * @param dataFile          data file
     * @param nm                namespace name / -1: no namespace
     * @param loc				location of data header line
     * @param dataDir			path for data file
     * @param chgToDataDir		change dir to dataDir for opening the data file
     */
    void ExternDataFileHandleOpt::processHeaderDataAllSymbols(FileInput &dataFile, int nm, LocationInfo& loc, string& dataDir, bool chgToDataDir)
    {
        string fn = dataDir + dataFile.fileNameReplaced();

#if CHDIR
        string curpath;
        if (chgToDataDir && !dataDir.empty()) {
            curpath = _mod->getCurrentWd();

            PROTO_OUTL("  change to dir '" << dataDir << "'");
            if (chdir(dataDir.c_str()) != 0)
                ctrl()->errHandler().error(ERROR_LVL_FATAL, ctrl()->printBuffer("cannot change to directory '%s'", dataDir.c_str()), loc);
        }
#endif

        _mod->resStream() << "\tpublic const extern: ";

        // read symbols from data file
        try {
            PROTO_OUTL("  open data file '" << dataFile.fileNameReplaced() << "' to read all symbols");

            istream *inStr = dataFile.open();
            ((ExternDataFile *)_ext)->readFileAllSymbols(this, inStr, fn, nm, loc);
            dataFile.close();
        }
        catch (FileException& e) {
            ctrl()->errHandler().error(ERROR_LVL_FATAL, ctrl()->printBuffer("%s: cmpl data file '%s'", e.what(), e.filename().c_str()), loc);
        }
        catch (exception& ex) {
            ctrl()->errHandler().error(ERROR_LVL_FATAL, "cmpl data file error", loc, &ex);
        }

        _mod->resStream() << endl;

#if CHDIR
        if (!curpath.empty()) {
            PROTO_OUTL("  back to dir '" << curpath << "'");
            if (chdir(curpath.c_str()) != 0)
                ctrl()->errHandler().error(ERROR_LVL_FATAL, ctrl()->printBuffer("cannot change to directory '%s'", curpath.c_str()), loc);
        }
#endif
    }

    /**
     * process data header line with individual symbols given
     * @param dataFile          data file
     * @param nm                namespace name / -1: no namespace
     * @param dataDir			path for data file
     */
    void ExternDataFileHandleOpt::processHeaderDataGivenSymbols(FileInput &dataFile, int nm, string& dataDir)
    {
        string fn = dataDir + dataFile.fileNameReplaced();
        const char *nms = (nm >= 0 ? data()->globStrings()->at(nm) : NULL);
        vector<string*> defSym;

        for (unsigned i = 0; i < _symbols.size(); i += 3) {
            // _symbols[i] contains the symbol name, _symbols[i+1] (if existing) the definition set assertion, _symbols[i+2] (if existing) the data type assertion
            DataImportBase::TxtWord *sym = _symbols[i];
            DataImportBase::TxtWord *ind = _symbols[i+1];
            DataImportBase::TxtWord *dtp = _symbols[i+2];

            string& symname = sym->word;
            LocationInfo locS(sym->location());
            if (insertDataSymbol(nm, symname, fn, locS)) {
                // cmpl code for extern symbol declaration
                // use a fixed location
                PositionInfo posS(&locS, POSITION_TYPE_FIXLOC);
                _mod->outStartPos(posS);

                _mod->resStream() << "\tpublic const extern ";
                if (nm >= 0)
                    _mod->resStream() << nms << '.';
                _mod->resStream() << symname << ';' << endl;

                if (ind || dtp) {
                    // cmpl code for assertion
                    // use a fixed location
                    DataImportBase::TxtWord *assFrom = (ind ?: dtp);
                    DataImportBase::TxtWord *assTo = (dtp ?: ind);

                    LocationInfo locA(assFrom->startPos(), assTo->endPos());
                    PositionInfo posA(&locA, POSITION_TYPE_FIXLOC);
                    _mod->outStartPos(posA);

                    bool assArraySub = (ind && ((ExternDataFile *)_ext)->assArraySub());

                    if (dtp || assArraySub) {
                         _mod->resStream() << "\t{: new local const type _t := ";

                         if (dtp && assArraySub) {
                            _mod->resStream() << '(';
                            dtp->outWithPos(_mod);
                            _mod->resStream() << ") || null";
                         }
                         else if (dtp) {
                            dtp->outWithPos(_mod);
                         }
                         else {
                            _mod->resStream() << "numeric || string || null";
                         }

                         _mod->resStream() << ';';
                    }

                    _mod->resStream() << endl << '\t' << "assert ";
                    if (dtp || assArraySub) {
                        _mod->resStream() << "_t " << endl;
                        _mod->outStartPos(posA);
                    }

                    if (nm >= 0)
                        _mod->resStream() << nms << '.';
                    _mod->resStream() << symname;

                    if (ind)
                        ind->outWithPos(_mod, nms, &defSym);

                    _mod->resStream() << ';';
                    if (dtp || assArraySub) {
                        _mod->resStream() << endl;
                        _mod->outStartPos(posA);
                        _mod->resStream() << " };";
                    }

                    _mod->resStream() << endl;
                }

                if (nm >= 0)
                    defSym.push_back(&(sym->word));
            }
        }
    }

    /**
     * insert data symbol in info about extern data
     * @param nm 			namespace name / -1: no namespace
     * @param sym			symbol name
     * @param fn			file name of cmpl data file
     * @param loc			location of data symbol in input file
     * @param sp			position of symbol in the data file / -1: not known
     * @return				true if symbol is inserted to info
     */
    bool ExternDataFileHandleOpt::insertDataSymbol(int nm, string& sym, string& fn, LocationInfo& loc, long sp)
    {
        GET_NEW_DATA(ExternDataInfo, ed);

        bool ret = false;

        string intsym;
        if (nm >= 0) {
            intsym = data()->globStrings()->at(nm);
            intsym += '.';
        }
        intsym += sym;

        try {
            ret = ed->insertExternInfo(data(), intsym.c_str(), sym.c_str(), ((ExternDataFile *)_ext)->extSrcType(), fn.c_str(), sp);
        }
        catch (ExternDataInfoInconsistentException&) {
            ctrl()->errHandler().error(ERROR_LVL_NORMAL, ctrl()->printBuffer("extern data declaration '%s' is inconsistent against a prior declaration for this symbol", intsym.c_str()), loc);
        }

        return ret;
    }



    /****** ExternDataFileImport ****/

    /**
     * constructor
     * @param mod       calling module
     * @param edf       parent object
     * @param ei        info about external data to import
     */
    ExternDataFileImport::ExternDataFileImport(ModuleBase *mod, ExternDataFile *edf, ImportExternDataInfo *ei):
        _mod(mod), _parent(edf), _filename(ei->externInfo->srcname()), _ctx(ei->execContext), _syntaxElem(ei->syntaxElem)
    {
    }


    /**
     * import data values
     * @param res		return values
     * @param ei		info about external data to import
     */
    void ExternDataFileImport::import(CmplVal& res, ExternDataInfo::EIRecord *ei)
    {
        int symname = ei->extsym();

        if (!_values.count(symname)) {
            if (ei->srcpos() >= 0 && !(_parent->readAll())) {
                // read values only for the requested symbol
                try {
                    readValues(false, symname, ei->srcpos());
                }
                catch (...) {
                    // ignore exception and try to proceed with reading values for all symbols from the file
                }
            }

            if (!_values.count(symname)) {
                readValues(true);

                if (!_values.count(symname)) {
                    throw runtime_error(_mod->ctrl()->printBuffer("symbol '%s' not found in external data file '%s'", _mod->data()->globStrings()->at(symname), _mod->data()->globStrings()->at(_filename)));
                }
            }
        }

        res.copyFrom(_values[symname], true, false);
    }


    /**
     * read values for one symbol or for all symbols from cmpl data file
     * @param readAll	read values for all symbols from data file or only for one symbol
     * @param symname	only used if !readAll: name of the symbol
     * @param fpos		only used if !readAll: start position of values of the symbol in the data file
     */
    void ExternDataFileImport::readValues(bool readAll, int symname, long fpos)
    {
        //TODO: Anpassen auf passendes Directory
        //  Ohne Alias: _filename sollte schon im Precompiler auf absoluten Namen geaendert sein, der hier einfach verwendet werden kann
        //  Mit Alias: Alias muss geeignet funktionieren, auch wenn _filename absoluten Pfad enthaelt, aber Alias nur einfachen Dateinamen, muss geprueft werden
        //              Resultierender Dateiname, egal ob mit oder ohne absoluten Pfad, sollte hier einfach verwendbar sein.

        string fn = _mod->data()->globStrings()->at(_filename);
        Interpreter *intMod = dynamic_cast<Interpreter *>(_mod);

        FileInput dataFile;
        dataFile.setFile(_mod->data(), IO_MODE_FILE, &fn, NULL);
        dataFile.setAliases(&(intMod->fileAlias()));
        istream *inStr = dataFile.open();

        CmplValAuto v;
        unsigned line = 0;
        bool rec = false;

        if (!readAll) {
            inStr->seekg(fpos);

            readSymbolValues(v, inStr, line, rec, &symname);
            _values[symname].moveFrom(v, false);
        }
        else {
            while (true) {
                int sym = readSymbolValues(v, inStr, line, rec);
                if (sym == -1)
                    break;

                _values[sym].moveFrom(v, false);
            }
        }
    }


    /**
     * read values for one symbol
     * @param res		return of read values
     * @param inStr		open input stream
     * @param line		current line in the input file
     * @param rec       recover mode, skip file up to next symbol start
     * @param checkSym	check that values for this symbol are read / NULL: any symbol
     * @return          symbol for which values are read / -1: end of file
     */
    int ExternDataFileImport::readSymbolValues(CmplVal& res, istream *inStr, unsigned& line, bool& rec, int *checkSym)
    {
        int sym = -1;

        try {
            // read head line for symbol
            string lstr;
            size_t p = getNextLine(inStr, line, lstr, false, (rec ? '%' : '\0'));
            if (p == string::npos)
                return -1;
            else if (p)
                lstr = lstr.substr(p);

            rec = false;
            ValueMode mode = valueScalar;
            CmplValAuto modePar, valDef;

            sym = parseSymbolHead(mode, modePar, valDef, lstr, line);
            if (checkSym && sym != *checkSym)
                throw FileException("symbol found on given position is not the expected symbol", _mod->data()->globStrings()->at(_filename), line);

            // read values for symbol
            switch (mode) {
                case valueScalar:
                    readValuesScalar(res, inStr, line, lstr);
                    break;

                case valueSet:
                    readValuesSet(res, inStr, line, lstr, (unsigned)(modePar.v.i));
                    break;

                case valueArray:
                    readValuesArray(res, inStr, line, lstr, modePar);
                    break;

                case valueIndices:
                    readValuesIndices(res, inStr, line, lstr, modePar, valDef);
                    break;
            }
        }

        catch (FileException& e) {
            if (!checkSym || sym != *checkSym) {
                _mod->ctrl()->errHandler().error((checkSym ? ERROR_LVL_EASY : ERROR_LVL_NORMAL), _mod->ctrl()->printBuffer("error while reading value of external data symbol '%s': %s, in '%s' line %u", _mod->data()->globStrings()->at(checkSym ? *checkSym : sym), e.what(), e.filename().c_str(), e.line()), ((Interpreter *)_mod)->syntaxElement(_syntaxElem)->loc());
                res.unset();
            }

            if (checkSym)
                throw;  // expception is ignored, and this function is called again with !checkSym

            rec = true;
        }

        return sym;
    }


    /**
     * parse headline for symbol
     * @param mode		return mode of the values for the symbol
     * @param modePar	return parameter for the mode
     * @param valDef	return default value for sparse array
     * @param lstr		line from data file; return remaining part of the line
     * @param line		line number for error messages
     * @return		symbol
     */
    int ExternDataFileImport::parseSymbolHead(ValueMode& mode, CmplVal& modePar, CmplVal& valDef, string& lstr, unsigned line)
    {
        if (lstr[0] != '%') {
            throw FileException("symbol head line starting with '%' expected in cmpl data file", _mod->data()->globStrings()->at(_filename), line);
        }

        size_t p = lstr.find_first_of(WHITE_SPACES_AND("[<"), 1);
        if (p == string::npos || p == 1) {
            throw FileException("missing symbol name in symbol head line in cmpl data file", _mod->data()->globStrings()->at(_filename), line);
        }

        string symName(lstr.substr(1, p - 1));
        int sym = _mod->data()->globStrings()->storeInd(symName);

        p = lstr.find_first_not_of(WHITE_SPACES, p);
        if (p != string::npos) {
            if (lstr[p] == 's') {
                mode = valueSet;
                modePar.set(TP_INT, 1);

                if (lstr.size() >= p + 3 && lstr[p+1] == 'e' && lstr[p+2] == 't') {
                    p = lstr.find_first_not_of(WHITE_SPACES, p + 3);
                    if (p != string::npos && lstr[p] == '[') {
                        size_t pr = lstr.find(']', p + 1);
                        if (pr != string::npos) {
                            string rank(lstr.substr(p + 1, pr - p - 1));
                            p = lstr.find_first_not_of(WHITE_SPACES, pr + 1);

                            pr = 0;
                            if (!readOneVal(modePar, rank, pr, line, true) || modePar.t != TP_INT || pr != string::npos) {
                                throw FileException(_mod->ctrl()->printBuffer("invalid  rank specification '%s' for set in symbol head line in cmpl data file", rank.c_str()), _mod->data()->globStrings()->at(_filename), line);
                            }
                        }
                    }
                }
            }

            else if (lstr[p] == '[') {
                mode = valueArray;
                modePar.set(TP_SET_NULL);
                valDef.unset();

                size_t ps = lstr.find(']', p + 1);
                if (ps != string::npos) {
                    string sets(lstr, p + 1, ps - p - 1);
                    p = lstr.find_first_not_of(WHITE_SPACES, ps + 1);

                    string setName;
                    vector<int> vsets;
                    size_t pn = 0;

                    while (StringStore::iterWords(sets, setName, pn, ",")) {
                        setName = StringStore::lrTrim(setName);

                        int set = _mod->data()->globStrings()->storeInd(setName);
                        if (!_values.count(set)) {
                            throw FileException(_mod->ctrl()->printBuffer("reference to unknown set '%s' in symbol head line in cmpl data file", setName.c_str()), _mod->data()->globStrings()->at(_filename), line);
                        }

                        if (!_values[set].isSet()) {
                            throw FileException(_mod->ctrl()->printBuffer("reference to value '%s' which is no set in symbol head line in cmpl data file", setName.c_str()), _mod->data()->globStrings()->at(_filename), line);
                        }

                        vsets.push_back(set);
                    }

                    if (vsets.empty()) {
                        throw FileException("missing sets for array in head line in cmpl data file", _mod->data()->globStrings()->at(_filename), line);
                    }

                    if (vsets.size() == 1) {
                        modePar.copyFrom(_values[vsets[0]], true, false);
                    }
                    else {
                        Tuple *tpl = new Tuple(vsets.size());
                        CmplValAuto vtpl(TP_TUPLE, tpl);

                        CmplVal *v = tpl->at(0);
                        for (int set : vsets)
                            (v++)->copyFrom(_values[set], true, false);

                        if (!TupleUtil::toIndexTuple(_ctx, modePar, tpl)) {
                            throw FileException("handling of given index set not implemented, in head line in cmpl data file", _mod->data()->globStrings()->at(_filename), line);
                        }
                    }

                    if (p != string::npos && lstr[p] == '=') {
                        p = lstr.find_first_not_of(WHITE_SPACES, p + 1);
                        if (!readOneVal(valDef, lstr, p, line)) {
                            throw FileException("invalid default value in symbol head line in cmpl data file", _mod->data()->globStrings()->at(_filename), line);
                        }
                    }

                    if (p != string::npos && lstr[p] == 'i' && lstr.substr(p, 7) == "indices") {
                        mode = valueIndices;
                        p = lstr.find_first_not_of(WHITE_SPACES, p + 7);
                    }
                }
            }
        }

        if (p == string::npos || lstr[p] != '<') {
            throw FileException("unexpected end of symbol head line in cmpl data file", _mod->data()->globStrings()->at(_filename), line);
        }

        lstr = StringStore::lTrim(lstr.substr(p + 1));
        return sym;
    }


    /**
     * read scalar value from data file
     * @param res		return of read value
     * @param inStr		open input stream
     * @param line		current line in the input file
     * @param lstr		line to process before reading next line from file
     */
    void ExternDataFileImport::readValuesScalar(CmplVal& res, istream *inStr, unsigned& line, string& lstr)
    {
        bool expVal = true;
        size_t pos = 0;

        while (true) {
            if (pos == string::npos || lstr.size() <= pos)
                pos = getNextLine(inStr, line, lstr, true);

            if (expVal) {
                if (!readOneVal(res, lstr, pos, line)) {
                    throw FileException("value expected in cmpl data file", _mod->data()->globStrings()->at(_filename), line);
                }
                expVal = false;
            }
            else if (lstr[pos] == '>') {
                break;
            }
            else {
                throw FileException("'>' expected in cmpl data file", _mod->data()->globStrings()->at(_filename), line);
            }
        }
    }


    /**
     * read set value from data file
     * @param res		return of read value
     * @param inStr		open input stream
     * @param line		current line in the input file
     * @param lstr		line to process before reading next line from file
     * @param rank		rank of set to read
     */
    void ExternDataFileImport::readValuesSet(CmplVal& res, istream *inStr, unsigned& line, string& lstr, unsigned rank)
    {
        size_t pos = 0;
        CmplValAuto v, t;
        CmplVal *vp;
        unsigned r = 0;
        unsigned long dummy;
        bool qt;

        res.set(rank ? TP_SET_EMPTY : TP_SET_NULL);

        Tuple *tpl;
        if (rank > 1) {
            tpl = new Tuple(rank);
            t.set(TP_ITUPLE, tpl);
        }

        while (true) {
            if (pos == string::npos || lstr.size() <= pos)
                pos = getNextLine(inStr, line, lstr, true);

            if (lstr[pos] == '>')
                break;

            if (rank == 0) {
                throw FileException("unexpected value for null tuple set in cmpl data file", _mod->data()->globStrings()->at(_filename), line);
            }

            else if (rank == 1) {
                if (res.t == TP_SET_EMPTY && isdigit(lstr[pos])) {
                    // check for special format for an algorithmic set (maybe with contained whitespaces)
                    size_t pe = lstr.find('>', pos);
                    size_t pn = lstr.find_first_not_of(WHITE_SPACES_AND("0123456789+-.()"), pos);
                    if (pn == string::npos || pn >= pe) {
                        if (testConstructAlgSet(lstr.substr(pos, pe-pos), res, line)) {
                            pos = pe;
                            continue;
                        }
                    }
                }

                qt = (lstr[pos] == '"' || lstr[pos] == '\'');
                if (!readOneVal(t, lstr, pos, line, true)) {
                    throw FileException("invalid value for set in cmpl data file", _mod->data()->globStrings()->at(_filename), line);
                }

                if (!qt && t.t == TP_STR) {
                    // check for special format for an algorithmic set (only without contained whitespaces)
                    CmplVal r;
                    if (testConstructAlgSet(string(t.stringAsCharP(_mod)), r, line))
                        t.moveFrom(r);
                }

                if (t.isSet()) {
                    if (res.t == TP_SET_EMPTY) {
                        res.moveFrom(t, false);
                    }
                    else {
                        SetUtil::addSet(_ctx, &v, _syntaxElem, &res, &t, true, false);
                        res.moveFrom(v, true);
                    }
                }
                else {
                    if (!SetUtil::tupleInSet(_ctx, res, t, dummy)) {
                        SetUtil::addTupleToSet(_ctx, v, res, t, true, false);
                        res.moveFrom(v, true);
                    }
                    else {
                        _mod->ctrl()->errHandler().error(ERROR_LVL_WARN, _mod->ctrl()->printBuffer("non-unique value ignored for set, in '%s' line %u", _mod->data()->globStrings()->at(_filename), line), ((Interpreter *)_mod)->syntaxElement(_syntaxElem)->loc());
                    }
                }
            }

            else {
                vp = tpl->at(r++);
                if (!readOneVal(*vp, lstr, pos, line, true)) {
                    throw FileException("invalid value for set in cmpl data file", _mod->data()->globStrings()->at(_filename), line);
                }

                if (r == rank) {
                    if (!SetUtil::tupleInSet(_ctx, res, t, dummy)) {
                        SetUtil::addTupleToSet(_ctx, v, res, t, true, false);
                        res.moveFrom(v, true);
                    }
                    else {
                        _mod->ctrl()->errHandler().error(ERROR_LVL_WARN, _mod->ctrl()->printBuffer("non-unique value ignored for set, in '%s' line %u", _mod->data()->globStrings()->at(_filename), line), ((Interpreter *)_mod)->syntaxElement(_syntaxElem)->loc());
                    }

                    tpl = new Tuple(rank);
                    t.dispSet(TP_ITUPLE, tpl);
                    r = 0;
                }
            }
        }

        if (r != 0) {
            throw FileException("invalid count of values for set in cmpl data file", _mod->data()->globStrings()->at(_filename), line);
        }

        CmplVal cs;
        if (SetUtil::canonicalSet(cs, res))
            res.moveFrom(cs, true);
    }


    /**
     * test whether string contains a algorithmic set and construct the set
     * @param s         input string
     * @param set       result set
     * @param line      current line in the input file
     * @return          true if algorithmic set is constructed
     */
    bool ExternDataFileImport::testConstructAlgSet(string s, CmplVal& set, unsigned& line)
    {
        size_t ps1 = s.find("..");
        size_t ps2 = s.find('(');
        size_t ps3 = s.find(')');

        if (ps1 != string::npos || (ps2 != string::npos && ps3 != string::npos && ps2 < ps3)) {
            string sf(s, 0, (ps1 != string::npos ? ps1 : ps2));
            string st(s, (ps1 != string::npos ? (ps1 + 2) : (ps3 + 1)));
            string ss;
            if (ps1 == string::npos)
                ss = s.substr(ps2 + 1, ps3 - ps2 - 1);

            CmplValAuto vf, vt, vs;
            size_t pf = 0, pt = 0, ps = 0;
            if (sf.empty() || !readOneVal(vf, sf, pf, line, true) || vf.t != TP_INT || pf != string::npos
                    || st.empty() || !readOneVal(vt, st, pt, line, true) || vt.t != TP_INT || pt != string::npos
                    || (ps1 == string::npos && (ss.empty() || !readOneVal(vs, ss, ps, line, true) || vs.t != TP_INT || vs.v.i == 0 || ps != string::npos))) {
                throw FileException("invalid format for values of algorithmic set in cmpl data file", _mod->data()->globStrings()->at(_filename), line);
            }

            bool no = (ps1 == string::npos && vs.v.i < 0);
            unsigned long sp = (ps1 == string::npos ? (unsigned long)(no ? -vs.v.i : vs.v.i) : 1);
            SetUtil::constructAlg(set, vf.v.i, vt.v.i, sp, no);

            return true;
        }

        return false;
    }


    /**
     * read array of values from data file
     * @param res		return of read value
     * @param inStr		open input stream
     * @param line		current line in the input file
     * @param lstr		line to process before reading next line from file
     * @param defset	definition set for the array
     */
    void ExternDataFileImport::readValuesArray(CmplVal& res, istream *inStr, unsigned& line, string& lstr, CmplVal& defset)
    {
        size_t pos = 0;
        CmplVal *vp;
        CmplArray *arr = new CmplArray(defset);
        res.set(TP_ARRAY, arr);

        SetIterator iter(defset, SetIterator::iteratorTupleSimple, true);
        iter.begin();

        while (true) {
            if (pos == string::npos || lstr.size() <= pos)
                pos = getNextLine(inStr, line, lstr, true);

            if (lstr[pos] == '>')
                break;

            if (iter.ended()) {
                throw FileException("too much values for array in cmpl data file", _mod->data()->globStrings()->at(_filename), line);
            }

            vp = arr->at(iter.tupleIndex());
            if (!readOneVal(*vp, lstr, pos, line)) {
                throw FileException("invalid value in cmpl data file", _mod->data()->globStrings()->at(_filename), line);
            }

            iter++;
        }

        if (!iter.ended()) {
            throw FileException("too less values for array in cmpl data file", _mod->data()->globStrings()->at(_filename), line);
        }
    }


    /**
     * read array of values from data file, with given index tuple per array element
     * @param res		return of read value
     * @param inStr		open input stream
     * @param line		current line in the input file
     * @param lstr		line to process before reading next line from file
     * @param set		set to which index tuples must belong
     * @param def   	default value
     */
    void ExternDataFileImport::readValuesIndices(CmplVal& res, istream *inStr, unsigned& line, string& lstr, CmplVal& set, CmplVal& def)
    {
        CmplArray *arr;
        if (def)
            arr = new CmplArray(set, def);
        else
            arr = new CmplArray();

        res.set(TP_ARRAY, arr);

        size_t pos = 0;
        unsigned rank = SetBase::minRank(set);
        unsigned r = 0;
        unsigned long ind, dummy;

        CmplValAuto v, t;
        Tuple *tpl;
        if (rank > 1) {
            tpl = new Tuple(rank);
            t.set(TP_ITUPLE, tpl);
        }
        else if (rank == 0) {
            t.set(TP_ITUPLE_NULL);
        }

        while (true) {
            if (pos == string::npos || lstr.size() <= pos)
                pos = getNextLine(inStr, line, lstr, true);

            if (lstr[pos] == '>')
                break;

            if (!readOneVal(v, lstr, pos, line, (r < rank))) {
                throw FileException("invalid value in cmpl data file", _mod->data()->globStrings()->at(_filename), line);
            }

            if (r == rank) {
                if (!SetUtil::tupleInSet(_ctx, set, t, ind)) {
                    throw FileException("invalid index tuple for array in cmpl data file", _mod->data()->globStrings()->at(_filename), line);
                }

                if (def) {
                    arr->at(ind)->moveFrom(v, true);
                }
                else {
                    if (!SetUtil::tupleInSet(_ctx, arr->defset(), t, dummy)) {
                        CmplValAuto dsn;
                        ind = SetUtil::addTupleToSet(_ctx, dsn, arr->defset(), t, false, true);
                        arr->insertValues(dsn, ind, &v);
                        v.dispUnset();
                    }
                    else {
                        throw FileException("repeated specification of same index tuple for array in cmpl data file", _mod->data()->globStrings()->at(_filename), line);
                    }
                }

                r = 0;
                if (rank > 1) {
                    tpl = new Tuple(rank);
                    t.dispSet(TP_ITUPLE, tpl);
                }
            }
            else {
                if (rank > 1)
                    tpl->at(r)->moveFrom(v, false);
                else
                    t.moveFrom(v, false);

                r++;
            }
        }

        if (r != 0) {
            throw FileException("invalid count of values for array in cmpl data file", _mod->data()->globStrings()->at(_filename), line);
        }
    }


    /**
     * read next line from data file
     * @param inStr		open input stream
     * @param line		current line in the input file
     * @param lstr		return of read line
     * @param errEof	throw exception if EOF
     * @param skipTo    if given then skip up to line starting with this char
     * @return          position of first non-space char in the line / string::npos if EOF
     */
    size_t ExternDataFileImport::getNextLine(istream *inStr, unsigned& line, string& lstr, bool errEof, char skipTo)
    {
        while (true) {
            line++;
            if (!getline(*inStr, lstr)) {
                if (inStr->eof()) {
                    if (errEof)
                        throw FileException("unexpected end of file in cmpl data file", _mod->data()->globStrings()->at(_filename), line);
                    else
                        return string::npos;
                }
                else {
                    throw FileException("read error in cmpl data file", _mod->data()->globStrings()->at(_filename), line);
                }
            }

            size_t pos = lstr.find_first_not_of(WHITE_SPACES);
            if (pos != string::npos && lstr[pos] != '#' && (!skipTo || lstr[pos] == skipTo))
                return pos;
        }
    }


    /**
     * read one value (TP_INT, TP_REAL or TP_STR) from string
     * @param v         return value
     * @param lstr		source string
     * @param pos		start position, moved forward to next value or string::npos if no one
     * @param line		current line in the input file
     * @param noReal	don't accept value of type TP_REAL
     * @return          true if value is successfully read
     */
    bool ExternDataFileImport::readOneVal(CmplVal& v, const string& lstr, size_t& pos, unsigned line, bool noReal)
    {
        size_t sz = lstr.size();
        if (pos == string::npos || pos >= sz)
            return false;

        size_t p;
        char c = lstr[pos];
        if (isspace(c)) {
            while (isspace(c)) {
                if (++pos == lstr.size())
                    return false;
                c = lstr[pos];
            }
        }

        if (c == '"' || c == '\'') {
            size_t p = lstr.find(c, pos + 1);
            if (p == string::npos)
                return false;

            unsigned s = _mod->data()->globStrings()->storeInd(lstr.substr(pos+1, p-pos-1));
            v.set(TP_STR, (intType)s);

            pos = p + 1;
        }

        else {
            tp_e t = TP_INT;
            p = pos;

            size_t dig = string::npos;
            size_t dp = string::npos;
            size_t e = string::npos;

            while (p < sz && !isspace(c = lstr[p])) {
                if (isdigit(c)) {
                    dig = p;
                }
                else if (c == '>') {
                    break;
                }
                else if (t != TP_STR && (p > pos || (c != '+' && c != '-')))
                {
                    if (c == '.' && dp == string::npos && e == string::npos) {
                        t = TP_REAL;
                        dp = p;
                    }
                    else if ((c == 'e' || c == 'E') && dig != string::npos && e == string::npos) {
                        t = TP_REAL;
                        e = p;
                    }
                    else if (!((c == '+' || c == '-') && e != string::npos && p == e + 1)) {
                        t = TP_STR;
                    }
                }

                p++;
            }

            if (dig == string::npos || (e != string::npos && e > dig))
                t = TP_STR;

            string str = lstr.substr(pos, p - pos);
            if (t == TP_INT) {
                if (p - pos < 5 || ((Interpreter *)_mod)->intOutOfRangeMode() == INT_OPER_OORANGE_IGNORE) {
                    v.set(TP_INT, (intType)STR_TO_CMPLINT(str.c_str()));
                }
                else {
                    realType r = STR_TO_CMPLREAL(str.c_str());

                    if (r >= CMPLINT_MIN && r <= CMPLINT_MAX)
                        v.set(TP_INT, (intType)STR_TO_CMPLINT(str.c_str()));
                    else if (noReal || ((Interpreter *)_mod)->intOutOfRangeMode() == INT_OPER_OORANGE_ERROR)
                        throw FileException("integer value is out of range in cmpl data file", _mod->data()->globStrings()->at(_filename), line);
                    else
                        v.set(TP_REAL, r);
                }
            }
            else if (t == TP_REAL) {
                v.set(TP_REAL, (realType) STR_TO_CMPLREAL(str.c_str()));
            }
            else {
                unsigned s = _mod->data()->globStrings()->storeInd(str);
                v.set(TP_STR, (intType)s);
            }

            pos = p;
        }

        pos = lstr.find_first_not_of(WHITE_SPACES, pos);
        return (!noReal || v.t != TP_REAL);
    }


}


