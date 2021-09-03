
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

#include <ctype.h>
#include <unistd.h>

#include "Precompiler.hh"
#include "../../Control/MainControl.hh"
#include "../../Control/MainData.hh"
#include "../../CommonData/ExternDataInfo.hh"
#include "../../CommonData/CmplSourceInput.hh"
#include "../../CommonData/ExtensionSteps.hh"


namespace cmpl
{
	/*********** module definition **********/

	// defines the module "precompile" that is implemented in class "Precompiler".
	// to register this module you have to insert "MODULE_CLASS_REG(1, precompile)" to file "modules.reg"
	MODULE_CLASS( precompile, Precompiler )



	/*********** initialization **********/

	/**
	 * destructor
	 */
	Precompiler::~Precompiler()
	{
		for (map<int, vector<FileInput *>>::iterator it = _inputFiles.begin(); it != _inputFiles.end(); it++) {
			vector<FileInput *> &fl = it->second;
			for (vector<FileInput *>::iterator ifl = fl.begin(); ifl != fl.end(); ifl++) {
                FileInput *f = *ifl;

                if (f->fileName() == _cmdlDataPseudoFile && f->extra())
                    delete (HeaderModeDataInfo *)(f->extra());

                delete f;
			}

			fl.clear();
		}

		freeHeaderWords();

		delete _dummyFile;

		if (_resStream)
			delete _resStream;

		if (_startCommentPos)
			delete _startCommentPos;

		_curPos.clear();

		for (map<int, LocationInfo*>::iterator it = _defLoc.begin(); it != _defLoc.end(); it++)
			delete it->second;
		_defLoc.clear();
	}


	/**
	 * initialize modul, to call immediately after the constructor.
	 * @param ctrl			main object
	 * @param data			data object
	 * @param name			module name (or alias)
	 */
	void Precompiler::init(MainControl *ctrl, MainData *data, const char *name)
	{
		ModuleBase::init(ctrl, data, name);

		_startDefLocNr = 1000;
		_traceScanning = false;

        _cmdlDataPseudoFile = " <:?data?:> ";

		_dummyFile = new FileInput();
		_dummyFile->setData(_data);
		_dummyFile->setAliases(&_fileAlias);

		_resStream = NULL;
		_headerMode = headerMode_No;
		_curHeaderWord = NULL;

		_initInclude = false;
		_startCommentPos = NULL;
	}


	/*********** handling of command line options **********/

#define OPTION_PRECOMP_INFILE			10
#define OPTION_PRECOMP_INSTRING			11
#define OPTION_PRECOMP_HEADARG			13
#define OPTION_PRECOMP_HEADINCL			14
#define OPTION_PRECOMP_FILEALIAS		21
#define OPTION_PRECOMP_STARTDEFLOC		30
#define OPTION_PRECOMP_OUTFILE			35
#define OPTION_PRECOMP_OUTEXTINFO		36
#define OPTION_PRECOMP_TRACESCANNER		40

	/**
	 * register command line options options for delivery to this module
	 * @param modOptReg		vector to register in
	 */
	void Precompiler::regModOptions(vector<CmdLineOptList::RegOption> &modOptReg)
	{
		ModuleBase::regModOptions(modOptReg);

		REG_CMDL_OPTION( OPTION_PRECOMP_INFILE, "i", 0, -1, CMDL_OPTION_NEG_ERROR, false );
		REG_CMDL_OPTION( OPTION_PRECOMP_INSTRING, "cmpl", 0, -1, CMDL_OPTION_NEG_ERROR, false );

		REG_CMDL_OPTION( OPTION_PRECOMP_HEADARG, "arg", 1, 2, CMDL_OPTION_NEG_ERROR, false );
		REG_CMDL_OPTION( OPTION_PRECOMP_HEADINCL, "include", 1, 2, CMDL_OPTION_NEG_ERROR, false );

		REG_CMDL_OPTION( OPTION_PRECOMP_FILEALIAS, "filealias", 0, -1, CMDL_OPTION_NEG_ERROR, false );

		REG_CMDL_OPTION( OPTION_PRECOMP_STARTDEFLOC, "startdefloc", 1, 1, CMDL_OPTION_NEG_ERROR, true );

		REG_CMDL_OPTION( OPTION_PRECOMP_OUTFILE, "o-pre", 0, 1, CMDL_OPTION_NEG_NO_ARG, true );
		REG_CMDL_OPTION( OPTION_PRECOMP_OUTEXTINFO, "o-extern", 0, 1, CMDL_OPTION_NEG_NO_ARG, true );

#if PROTO
		REG_CMDL_OPTION( OPTION_PRECOMP_TRACESCANNER, "trace-scanner", 0, 0, CMDL_OPTION_NEG_DELIV, true );
#endif

        regExtOption(EXT_CMDLOPT_PRECOMP_SIMPLE);
        regExtOption(EXT_CMDLOPT_PRECOMP_PROC_DATA);
    }

	/**
	 * parse single option from command line options, this function is called for every delivered option
	 * @param ref			reference number of option registration, should be used for discriminate the options
	 * @param prio			priority value of option
	 * @param opt			option
	 * @return				true if option is used by the module
	 */
	bool Precompiler::parseOption(int ref, int prio, CmdLineOptList::SingleOption *opt)
	{
		HeaderMode hm;

		if (ModuleBase::parseOption(ref, prio, opt))
			return true;

		switch (ref) {
			case OPTION_PRECOMP_INFILE:
			case OPTION_PRECOMP_INSTRING:
			case OPTION_PRECOMP_HEADARG:
			case OPTION_PRECOMP_HEADINCL:
                hm = (ref==OPTION_PRECOMP_HEADARG ? headerMode_Arg : (ref==OPTION_PRECOMP_HEADINCL ? headerMode_Include : headerMode_No));
                parseInput(opt, (ref==OPTION_PRECOMP_INFILE), hm);
				return true;

			case OPTION_PRECOMP_FILEALIAS:
				parseFileAliases(opt, _fileAlias, _fileAliasPrio);
				return true;

			case OPTION_PRECOMP_STARTDEFLOC:
				_startDefLocNr = opt->argAsInt(0, _ctrl);
				return true;

			case OPTION_PRECOMP_OUTFILE:
				if (opt->neg())
					_outFile.setEnabled(false);
				else {
					_outFile.setFile(_data, IO_MODE_FILE, (opt->size() > 0 ? &((*opt)[0]) : NULL), IO_FILE_STANDARD_OPRE, true);
					if (opt->size() > 0)
						_outFile.setLocSrc(opt->argPos(0));
					else
						_outFile.setLocSrc(opt->loc(true));
				}
				return true;

			case OPTION_PRECOMP_OUTEXTINFO:
				if (opt->neg())
					_outExtInfo.setEnabled(false);
				else {
					_outExtInfo.setFile(_data, IO_MODE_FILE, (opt->size() > 0 ? &((*opt)[0]) : NULL), IO_FILE_STANDARD_EXTDATA, true);
					if (opt->size() > 0)
						_outExtInfo.setLocSrc(opt->argPos(0));
					else
						_outExtInfo.setLocSrc(opt->loc(true));
				}
				return true;

#if PROTO
			case OPTION_PRECOMP_TRACESCANNER:
				_traceScanning = !opt->neg();
				return true;
#endif

		}

		return false;
	}

    /**
     * execute step for every registered extension for a single command line option
     * @param extStep       extension execution step
     * @param extRef        map from extension identifier to command line option reference key
     * @param prio          priority value of option
     * @param opt           option
     * @return              true if option is used by an extension
     */
    bool Precompiler::execExtOption(int extStep, map<int, int>& extRef, int prio, CmdLineOptList::SingleOption *opt)
    {
        switch (extStep) {
            case EXT_CMDLOPT_PRECOMP_SIMPLE:
                return execExtOptionSimple(extStep, extRef, prio, opt);

            case EXT_CMDLOPT_PRECOMP_PROC_DATA:
                if (extRef.empty())
                    return false;

                // if scanning is already running and not cmpl header "%arg" then immediate execution, else store for execution in input order
                if (_resStream && _headerMode != headerMode_Arg) {
                    return execExtOptionData(extRef, opt);
                }
                else {
                    parseInput(opt, false, headerMode_Data, &extRef);
                    return true;
                }
        }

        return false;
    }


    /**
     * handle external data option by delivering it to all potentially concerned extensions
     * @param extRef        map from extension identifier to command line option reference key
     * @param opt           option
     * @return              true if option is used by an extension
     */
    bool Precompiler::execExtOptionData(map<int, int>& extRef, CmdLineOptList::SingleOption *opt)
    {
        if (extRef.empty())
            return false;

        // call every extension for checking the data
        vector<DataImportExtensionPar> parVect(extRef.size());
        unsigned i = 0, iHandlePrio = 0;
        int maxHandlePrio;

        for (map<int, int>::iterator it = extRef.begin(); it != extRef.end(); it++, i++) {
            DataImportExtensionPar& par = parVect[i];
            ctrl()->runExtension(this, EXT_CMDLOPT_PRECOMP_PROC_DATA, it->first, it->second, 0, opt, &par);

            if (i == 0 || maxHandlePrio < par.handlePrio()) {
                iHandlePrio = i;
                maxHandlePrio = par.handlePrio();
            }
        }

        // handle external data import with the extension with the highest priority
        return parVect[iHandlePrio].runForData();
    }


	/**
	 * parse command line option with input args
	 * @param opt			option
	 * @param isFile		arg is input file
	 * @param hm			header mode
     * @param extRef        only for hm==headerMode_Data: map from extension identifier to command line option reference key
     */
    void Precompiler::parseInput(CmdLineOptList::SingleOption *opt, bool isFile, HeaderMode hm, map<int, int>* extRef)
	{
		int order = (hm == headerMode_No ? 1 : 0);
		size_t start = 0;
		if (opt->sepArgStart() == 1 && opt->size() > 1) {
			// first arg is the order number
			order = opt->argAsInt(0, _ctrl);
			start = 1;
		}

        if ((hm != headerMode_No && hm != headerMode_Data) && opt->size() - start != 1) {
			_ctrl->errHandler().error(ERROR_LVL_NORMAL, _ctrl->printBuffer("wrong argument count for option '%s'", opt->opt().c_str()), opt->loc(true));
			return;
		}

		vector<FileInput *> &v = _inputFiles[order];

        if (hm != headerMode_Data) {
            for (size_t i = start; i < opt->size(); i++) {
                FileInput *inp = new FileInput();

                if (isFile)
                    inp->setFile(_data, IO_MODE_FILE, &((*opt)[i]), (const char *)NULL);
                else
                    inp->setInputString(_data, NULL, &((*opt)[i]));

                inp->setLocSrc(opt->argPos(i));
                inp->setAliases(&_fileAlias);
                inp->setExtra((void *) hm);

                v.push_back(inp);
            }
        }
        else {
            // set special pseudo file name as mark for command line option for a data import extension
            FileInput *inp = new FileInput();
            inp->setInputString(_data, &_cmdlDataPseudoFile, NULL);

            inp->setLocSrc(opt->loc(true));
            inp->setAliases(&_fileAlias);
            inp->setExtra(new HeaderModeDataInfo(opt, *extRef));

            v.push_back(inp);
        }
	}

	/**
	 * writes usage info for the module to stream
	 * @param s				stream to write to
	 */
	void Precompiler::usage(ostream& s)
	{
		ModuleBase::usage(s);

		s << "  -i <file1> ...                cmpl input files (you can also specify it as first arg before any option in the command line)" << endl;
		s << "  -cmpl <string1> ...           cmpl input strings" << endl;
		s << "  -arg <string>                 string with command line options, mainly for use in cmpl header" << endl;
		s << "  -include <file>               file to include, mainly for use in cmpl header" << endl;
		s << "  -filealias <from1>=<to1> ...  defines aliases for input file names and data file names" << endl;
		s << "  -startdefloc <no>             start number for location definitions, defaults to 1000" << endl;
		s << "  -o-pre [<file>]               output precompiler result to <file>" << endl;
		s << "  -o-extern [<file>]            output extern data info to <file>" << endl;
#if PROTO
		s << "  -trace-scanner                output precompiler scanner trace to stderr" << endl;
#endif
	}

	/**
	 * run module function
	 */
	void Precompiler::run()
	{
		_ctrl->errHandler().setExecStep("run");

		PROTO_OUTL("Start run precompile module " << moduleName());

		_resStream = new ostringstream();	
		_curFile = NULL;

		doScan();

		// store in data object
		SET_NEW_DATA(CmplSourceInput, resSource);
		string s = _resStream->str();
		resSource->setContents(s);

		delete _resStream;
		_resStream = NULL;

		// file output
		if (_outFile.enabled()) {
			try {
				ostream *os = _outFile.open();
				*os << resSource->contents() << endl;
				_outFile.close();
			}
			catch (FileException& e) {
				_ctrl->errHandler().error(ERROR_LVL_FATAL, _ctrl->printBuffer("%s: write precompiler result file '%s'", e.what(), e.filename().c_str()), _outFile.locSrc());
			}
			catch (exception& ex) {
				_ctrl->errHandler().error(ERROR_LVL_FATAL, "file error at precompiler result output", _outFile.locSrc(), &ex);
			}
		}

		if (_outExtInfo.enabled()) {
			try {
				ostream *os = _outExtInfo.open();
				GET_DATA(ExternDataInfo, ed);
				if (ed)
					ed->writeTo(*os, _data);
				_outExtInfo.close();
			}
			catch (FileException& e) {
				_ctrl->errHandler().error(ERROR_LVL_FATAL, _ctrl->printBuffer("%s: write extern data info file '%s'", e.what(), e.filename().c_str()), _outExtInfo.locSrc());
			}
			catch (exception& ex) {
				_ctrl->errHandler().error(ERROR_LVL_FATAL, "file error at extern data info output", _outExtInfo.locSrc(), &ex);
			}
		}

		PROTO_OUTL("End run precompile module " << moduleName());
	}

	
	/**
	 * go to the next input
	 * @param buf       return a string input or NULL if the next input is no string input
	 * @return          opened input file or NULL if the next input is no file
	 */
	FILE *Precompiler::iterInput(const char **buf)
	{
		// close the previous input
		if (_curFile) {
			_curFile->close();

			if (_curPos.size() != 1)
				_ctrl->errHandler().internalError("position stack of precompiler inputs has wrong size");

			delete curPos();
			_curPos.pop_back();
		}

		// find next input
		int inpKey;
		size_t inpCur;
		FileInput *f;
		string fn;

		while (fn.empty()) {
            if (!searchNextInput(_inputFiles, _inputFilesCur, inpKey, inpCur)) {
				// all inputs are consumed, return NULL
				*buf = NULL;
				return NULL;
			}

			PROTO_OUTL("iterInput: " << inpKey << " " << inpCur);

			// iterate to next input
			f = _inputFiles[inpKey][inpCur];
			_inputFilesCur[inpKey] = inpCur + 1;

            // special data input is handled here
            if (f->fileName() == _cmdlDataPseudoFile) {
                HeaderModeDataInfo *hmd = (HeaderModeDataInfo *) f->extra();
                if (!execExtOptionData(hmd->extRef, hmd->opt))
                    _ctrl->errHandler().error(ERROR_LVL_WARN, "no extension found for handling external data", hmd->opt->loc(true));

                fn.clear();
            }
            else {
                // if filename is empty then skip the file
                fn = f->fileNameReplaced();
            }
		}

		resStream() << endl;

        LocationInfo& inLoc = f->locSrc();
		int locNr = outDefLoc(&inLoc, true); 
		HeaderMode hm = (HeaderMode)(long)(f->extra());

		PROTO_OUTL("  iterTo: '" << fn << "' (" << hm << ") from: " << inLoc);

		// open the input
		string *s;
		FILE *fp = NULL;
		try {
			fp = f->openFile(&s);
		}
		catch (FileException& e) {
			_ctrl->errHandler().error(ERROR_LVL_FATAL, _ctrl->printBuffer("%s: cmpl input file '%s'", e.what(), e.filename().c_str()), _includeLoc);
		}
		catch (exception& ex) {
			_ctrl->errHandler().error(ERROR_LVL_FATAL, "cmpl input file error", _includeLoc, &ex);
		}

		if (s) {
			*buf = s->c_str();
			PROTO_OUTL(*buf);
		}

		// set current position
		if (!_curPos.empty())
			_ctrl->errHandler().internalError("position stack of precompiler inputs is not empty");

		PositionInfo *pos = f->newPositionInfo(fn, &(f->locSrc()));
		outStartPos(*pos, locNr);
		_curPos.push_back(pos);

		_curFile = f;
		_headerMode = hm;
		return fp;
	}

	/**
	 * search for next not consumed input
	 * @param inp		map with inputs
	 * @param cnt		map with current input counters
	 * @param key		return key of found input
	 * @param cur		return current counter of found input
	 * @return			input found / false: all inputs are consumed
	 */
	bool Precompiler::searchNextInput(map<int, vector<FileInput *>>& inp, map<int, size_t>& cnt, int& key, size_t& cur)
	{
		map<int, vector<FileInput *>>::iterator itInp;
		map<int, size_t>::iterator itCnt;

		for (itInp = inp.begin(); itInp != inp.end(); itInp++) {
			key = itInp->first;

			itCnt = cnt.find(key);
			if (itCnt != cnt.end())
				cur = itCnt->second;
			else
				cnt[key] = cur = 0;

			if (cur < itInp->second.size())
				return true;
		}

		return false;
	}


	/**
	 * start cmpl header line
	 */
	void Precompiler::headerStart()
	{
		freeHeaderWords();
		_headerMode = headerMode_Init;
	}

	/**
	 * end cmpl header line
	 * @param eof		end by eof or by eol
	 * @return			execute include operation now
	 */
	bool Precompiler::headerEnd(bool eof)
	{
		bool res = false;

		if (_headerMode != headerMode_No) {
			if (_headerMode == headerMode_Include) {
				// include must have one word
				if ((_curHeaderWord && !(_headerWords.empty())) || (!_curHeaderWord && _headerWords.size() != 1)) {
					PositionInfo pos = *(curPos());
					pos.setCol(1);
					LocationInfo loc(pos, *(curPos())+1);

					if (!_curHeaderWord && _headerWords.empty())
						_ctrl->errHandler().error(ERROR_LVL_NORMAL, "missing argument file for include", loc);
					else
						_ctrl->errHandler().error(ERROR_LVL_NORMAL, "too many argument files for include", loc);

					freeHeaderWords();
				}
				else {
					if (_curHeaderWord) {
						_includeWord = _curHeaderWord;
						_curHeaderWord = NULL;
					}
					else {
						_includeWord = _headerWords[0];
						_headerWords.clear();
					}

					// initialize include
					if (!_initInclude)
						includeInit();

					// get filename
					string fn(_includeWord->txt);

#if CHDIR
					// change path separator char, to allow using of '/' in the cmpl file
					if (_dirSepChar != '/') {
                        unsigned i;
						while ((i = fn.find('/')) != string::npos)
							fn.replace(i, 1, 1, _dirSepChar);
					}
#endif

					PROTO_OUTL("prepare include for file: " << fn);
					
					// file name replacement
					fn = _dummyFile->fileNameReplaced(fn);
					if (fn.empty()) {
						PROTO_OUTL("  skip file");
						delete _includeWord;
						_includeWord = NULL;
					}
					else {
						_includeWord->txt = fn;
					}

					if (_includeWord) {
						_includeLoc = LocationInfo(_includeWord->pos, _includeWord->pos + _includeWord->len);
						res = true;
					}
				}
			}
			else {
                if (_curHeaderWord)
                    headerSep(0);

                if (_headerMode == headerMode_Arg)
					processHeaderArg(true);
				else if (_headerMode == headerMode_Other)
					processHeaderArg(false);

				freeHeaderWords();
			}

			_headerMode = headerMode_No;
		}

		return res;
	}

	/**
	 * add word to scanned cmpl header text
	 * @param txt		word
	 * @param quoted	word is quoted
	 * @param pos		start position of word
	 */
	void Precompiler::headerWord(const char *txt, bool quoted, PositionInfo *pos)
	{
		if (_headerMode == headerMode_Error)
			return;

        if (_curHeaderWord) {
            if (quoted || _curHeaderWord->quoted) {
                _headerWords.push_back(_curHeaderWord);
                _curHeaderWord = NULL;
            }
            else {
                _curHeaderWord->txt += txt;
                _curHeaderWord->len += strlen(txt);
            }
        }

		if (!_curHeaderWord) {
			_curHeaderWord = new HeaderWord();

			if (quoted) {
				// strip quotes and replace double inner quote chars by a single one
				string s = txt;
				s = s.substr(1, s.size() - 2);

				size_t p = 0;
				while ((p = s.find("\"\"", p)) != string::npos) {
					s = s.replace(p, 2, "\"");
					p++;
				}

				_curHeaderWord->txt = s;
			}
			else {
				_curHeaderWord->txt = txt;
			}

			_curHeaderWord->quoted = quoted;
			_curHeaderWord->len = strlen(txt);
			_curHeaderWord->pos = *pos;
		}
	}

    /**
     * separating spaces scanned in cmpl header text
     * @param cnt       count of separating spaces
     */
    void Precompiler::headerSep(size_t cnt)
    {
        if (_headerMode == headerMode_Init) {
            if (!_curHeaderWord) {
                _ctrl->errHandler().error(ERROR_LVL_NORMAL, "mode missing for cmpl header line", *curPos());
                _headerMode = headerMode_Error;
                return;
            }

            if (_curHeaderWord->txt == "include")
                _headerMode = headerMode_Include;
            else if (_curHeaderWord->txt == "arg")
                _headerMode = headerMode_Arg;
            else
                _headerMode = headerMode_Other;

            if (_headerMode == headerMode_Other)
                _headerWords.push_back(_curHeaderWord);
            else
                delete _curHeaderWord;
        }

        else if (_curHeaderWord) {
            _headerWords.push_back(_curHeaderWord);
        }

        _curHeaderWord = NULL;
    }

#if 0
	/**
	 * add character to scanned cmpl header text
	 * @param chr		character
	 * @param spaces	count of leading spaces before the character
	 */
	void Precompiler::headerChar(char chr, size_t spaces)
	{
		bool isSpace = (chr == ' ');
		char buf[2];

		PositionInfo pos = *curPos();
		if (!isSpace && spaces > 0)
			pos += spaces;

		if (_headerMode == headerMode_Error || _headerMode == headerMode_No)
			return;

		if (!_curHeaderWord && isSpace)
			return;

		if (_headerMode == headerMode_Init) {
			if (!_curHeaderWord) {
				_ctrl->errHandler().error(ERROR_LVL_NORMAL, "mode missing for cmpl header line", pos);
				_headerMode = headerMode_Error;
				return;
			}

			if (_curHeaderWord->txt == "include")
				_headerMode = headerMode_Include;
			else if (_curHeaderWord->txt == "arg")
				_headerMode = headerMode_Arg;
			else
				_headerMode = headerMode_Other;

			if (_headerMode == headerMode_Other)
				_headerWords.push_back(_curHeaderWord);
			else
				delete _curHeaderWord;

			_curHeaderWord = NULL;

            if (!isSpace) {
				_ctrl->errHandler().error(ERROR_LVL_NORMAL, _ctrl->printBuffer("illegal character '%c' in cmpl header line", chr), pos);
				_headerMode = headerMode_Error;
				return;
			}
		}

        if (isSpace || spaces) {
            if (_curHeaderWord) {
                _headerWords.push_back(_curHeaderWord);
                _curHeaderWord = NULL;
            }

            if (!isSpace) {
                buf[0] = chr;
                buf[1] = '\0';
                pos.addCols(spaces);
                headerWord(buf, false, &pos);
            }
        }
        else {
            buf[0] = chr;
            buf[1] = '\0';
            headerWord(buf, false, &pos);
        }

		return;
	}
#endif

	/**
	 * output error message about wrong c comment in header: line feed
	 */
	void Precompiler::outErrorCCommentLf()
	{
		_ctrl->errHandler().error(ERROR_LVL_NORMAL, "comment within cmpl header cannot contain line feed", *(curPos()));
	}

	/**
	 * output error message about wrong c comment: end without start
	 */
	void Precompiler::outErrorCCommentEWS()
	{
		_ctrl->errHandler().error(ERROR_LVL_NORMAL, "end of comment without start", *(curPos()));
	}


	/**
	 * process header line with command line options
	 * @param argLine		header line with mode headerMode_Arg or headerMode_Other
	 */
	void Precompiler::processHeaderArg(bool argLine)
	{
		if (_headerWords.empty())
			return;

		// construct command line option list
		CmdLineOptList cmdOptList;
		CmdLineOptList::SingleOption *sa = NULL;

		for (size_t i = 0; i < _headerWords.size(); i++) {
			HeaderWord *hw = _headerWords[i];
			PositionInfo pos = hw->pos + (hw->quoted ? 1 : 0);
			_data->persistentPosition(pos);

			if (!sa || (argLine && !hw->quoted && hw->txt.size() >= 2 && hw->txt[0] == '-'))
				sa = cmdOptList.addOption(hw->txt, pos, true);
			else
				sa->arg(hw->txt, pos);
		}

		// store command line option list, and use it also for this module
		_ctrl->appendCmdLineOpts(&cmdOptList, CMDLINE_OPT_PARSE_HEADER, true, true);
	}

	/**
	 * frees header words in _headerWords
	 */
	void Precompiler::freeHeaderWords()
	{
		if (_curHeaderWord) {
			delete _curHeaderWord;
			_curHeaderWord = NULL;
		}

		if (_includeWord) {
			delete _includeWord;
			_includeWord = NULL;
		}

		if (_headerWords.size() > 0) {
	        for (vector<HeaderWord *>::iterator it = _headerWords.begin(); it != _headerWords.end(); it++)
				delete *it;

			_headerWords.clear();
		}
	}


	/**
	 * initializes the possibility for including files
	 */
	void Precompiler::includeInit()
	{
        if (!_initInclude) {
#if CHDIR
            // store current directory
            _dirStartName = getCurrentWd();

            // get path separator; it can be '/' or '\', ahead of it can be a drive letter
            _dirSepChar = '/';
            if (!_dirStartName.empty()) {
                for (int i = 0; i < 10 && _dirStartName[i] != '\0'; i++) {
                    if (_dirStartName[i] == '/' || _dirStartName[i] == '\\') {
                        _dirSepChar = _dirStartName[i];
                        break;
                    }
                }
            }

            PROTO_OUTL("include init:");
            PROTO_OUTL("  current start path: " << (_dirStartName.empty() ? "(n.a.)" : _dirStartName.c_str()));
            PROTO_OUTL("  path separator char: " << _dirSepChar);
#endif
        }

		_initInclude = true;
	}

#if CHDIR
	/**
	 * gets the current directory name
	 * @return      current directory name
	 */
	const string Precompiler::getCurrentWd()
	{
		int bufsize = 200;

		while (bufsize < 10000) {
			char *buf = new char[bufsize];

			if (getcwd(buf, bufsize-1)) {
				const string s(buf);
				delete buf;
				return s;
			}

			delete buf;
			bufsize *= 2;
		}

		_ctrl->errHandler().internalError("current directory name too long");

		const string s;
		return s;
	}

	/**
	 * splits a file name which can contain directory name to the file name and the directory name
	 * @param filename         		file name with path
	 * @param path                  return the directory name
	 * @return                      file name without directory name
	 */
	string Precompiler::splitPathName(string &filename, string &path)
	{
		// search last occurence of separator
		size_t i = filename.find_last_of(_dirSepChar);
		if (i == string::npos) {
			// no path
			path.clear();
			return filename;
		}
		else {
			path = filename.substr(0, i+1);
			return filename.substr(i+1);
		}
	}
#endif


	/**
	 * opens the included file and stores the current file to the include stack
	 * @param eof				file within the include lies is already finished
	 * @return          		opened include file
	 */
	FILE *Precompiler::openIncludeFile(bool eof)
	{
#if CHDIR
		// if include in a main level file within another directory, then go to this other directory now
		string curpath = getCurrentWd();
		bool chgDir = false;

		if (_inclFiles.empty() && curPos() && curPos()->isFile()) {
			string mfn = curPos()->name();
			string mpath;
			splitPathName(mfn, mpath);

			if (!mpath.empty()) {
				chgDir = true;
				PROTO_OUTL("  change to dir '" << mpath << "'");
				if (chdir(mpath.c_str()) != 0)
					_ctrl->errHandler().error(ERROR_LVL_FATAL, _ctrl->printBuffer("cannot change to directory '%s'", mpath.c_str()), _includeLoc);
			}
		}
#endif

		// get filename
		string filename(_includeWord->txt);
		PROTO_OUTL("open include file: " << filename);

		FileInput *f = new FileInput();
		f->setFile(_data, IO_MODE_FILE, &filename, (const char *)NULL);
		f->setLocSrc(_includeLoc);
		f->setExtra((void *)eof);

		resStream() << endl;
		int locNr = outDefLoc(&_includeLoc, false); 

		// open the include file
		string *s;
		FILE *fp;

		try {
			fp = f->openFile(&s);
		}
		catch (FileException& e) {
			_ctrl->errHandler().error(ERROR_LVL_FATAL, _ctrl->printBuffer("%s: included file '%s'", e.what(), e.filename().c_str()), _includeLoc);
		}
		catch (exception& ex) {
			_ctrl->errHandler().error(ERROR_LVL_FATAL, "include file error", _includeLoc, &ex);
		}

		_inclFiles.push(f);

		// set current position
		PositionInfo *pos = f->newPositionInfo(filename, &(f->locSrc()));
		outStartPos(*pos, locNr);
		_curPos.push_back(pos);
		
#if CHDIR
		string path;
		string name = splitPathName(filename, path);
		if (!path.empty()) {
			chgDir = true;
			PROTO_OUTL("  change to dir '" << path << "'");
			if (chdir(path.c_str()) != 0)
				_ctrl->errHandler().error(ERROR_LVL_FATAL, _ctrl->printBuffer("directory '%s' for include file not found", path.c_str()), _includeLoc);
		}

		if (chgDir)
			_inclDirName.push(curpath);
		else
			_inclDirName.push(string());
#endif

		return fp;
	}

	/**
	 * close the included file
	 */
	void Precompiler::closeIncludeFile()
	{
		if (_inclFiles.empty())
			return;		// buffer wasn't opened by include

		if (_curPos.size() < 2)
			_ctrl->errHandler().internalError("include position stack underflow");

		// close file
		FileInput *f = _inclFiles.top();
		bool eof = (bool) f->extra();
		f->close();

		_inclFiles.pop();

		delete curPos();
		_curPos.pop_back();

		PROTO_OUTL("back to file " << curPos()->name());

#if CHDIR
		string path = _inclDirName.top();
		_inclDirName.pop();

		if (!path.empty()) {
			if (chdir(path.c_str()) != 0)
				_ctrl->errHandler().error(ERROR_LVL_FATAL, _ctrl->printBuffer("cannot change to directory '%s'", path.c_str()), *curPos());
		}
#endif

		// if included after the end of the including file, then close this file too
		if (eof)
			closeIncludeFile();
	}


	/**
	 * write location definition to result stream, or find number of already defined location
	 * @param loc               location
	 * @param outTN     		output also type and name
	 * @param defNr				definition number of location / 0: use a new one
	 * @return                  definition number of location
	 */
	int Precompiler::outDefLoc(const LocationInfo *loc, bool outTN, int defNr)
	{
		// search for already defined location
		for (map<int, LocationInfo*>::iterator it = _defLoc.begin(); it != _defLoc.end(); it++) {
			if ((it->second == loc || *(it->second) == *loc) && (defNr == 0 || defNr == it->first))
				return it->first;
		}

		// recursive call for inLoc
		const LocationInfo *inLoc = (loc->inLoc() ?: loc->fixLoc());
		int inLocNr = (inLoc ? outDefLoc(inLoc, true, 0) : 0);

		// new location definition
		if (_defLoc.empty())
			_curDefLocNr = _startDefLocNr;

		int defLocNr;
		if (defNr) {
			defLocNr = defNr;
			map<int, LocationInfo*>::iterator it = _defLoc.find(defNr);
			if (it != _defLoc.end())
				_ctrl->errHandler().error(ERROR_LVL_NORMAL, _ctrl->printBuffer("definition number %d for DefLoc is not unique", defNr), *(curPos()));
		}
		else {
			defLocNr = _curDefLocNr++;
		}

		_defLoc[defLocNr] = new LocationInfo(*loc);

		// write location definition
		resStream() << "##DefLoc:" << defLocNr << ':';
		loc->outForDefLoc(resStream(), inLocNr, outTN);
		resStream() << endl;

		return defLocNr;
	}

	/**
	 * write start position to result stream
	 * @param pos				position object
	 */
	void Precompiler::outStartPos(const PositionInfo& pos)
	{
		const LocationInfo *inLoc = (pos.inLoc() ?: pos.fixLoc());
		int locNr = (inLoc ? outDefLoc(inLoc, true) : 0);
		outStartPos(pos, locNr);
	}

	/**
	 * write start position to result stream
	 * @param pos				position object
	 * @param locNr				definition number of location within the position lies / 0: no one
	 */
	void Precompiler::outStartPos(const PositionInfo& pos, int locNr)
	{
		resStream() << "##StartPos:";
		if (locNr)
			resStream() << locNr;
		resStream() << ',' << pos.type() << ',';

		if (pos.hasName())
			resStream() << pos.name();

		if (pos.hasLine() && pos.line() != 1)
			resStream() << endl << "##Line:" << pos.line();

		resStream() << endl;
	}

	/**
	 * stores current position as start position of a c comment
	 */
	void Precompiler::storeCommentStartPos()
	{
		if (_startCommentPos) {
			_ctrl->errHandler().error(ERROR_LVL_NORMAL, "start of comment within comment", *(curPos()));
			return;
		}

		_startCommentPos = new PositionInfo(*(curPos()));
	}

	/**
	 * output to result stream after a c comment
	 * @param eol		after comment follows end of line
	 */
	void Precompiler::outPositionAfterComment(bool eol)
	{
		PositionInfo *pe = curPos();
		if (!_startCommentPos) {
			_ctrl->errHandler().error(ERROR_LVL_NORMAL, "end of comment without start", *pe);
			return;
		}

		int lines = pe->line() - _startCommentPos->line();
		int cols, i;

		if (lines == 0) {
			cols = pe->col() - _startCommentPos->col();
			for (i = 0; i < cols; i++)
				resStream() << ' ';
		}
		else {
			if (lines > 10) {
				resStream() << endl << "##Line:" << pe->line() << endl;
			}
			else {
				for (i = 0; i < lines; i++)
					resStream() << endl;
			}

			if (!eol) {
				cols = pe->col() - 1;
				if (cols > 20) {
					resStream() << "##Col:" << pe->col() << endl;
				}
				else {
					for (i = 0; i < cols; i++)
						resStream() << ' ';
				}
			}
		}

		delete _startCommentPos;
		_startCommentPos = NULL;
	}


	/**
	 * set InLoc in position
	 * @param posOrg			original position for error message only
	 * @param pos				position to set inLoc within
	 * @param inLoc				inLoc number as parsed from position mark
	 * @return					inLoc for output / 0: no one
	 */
	int Precompiler::setInLocPM(PositionInfo& posOrg, PositionInfo& pos, int inLoc)
	{
		if (inLoc == 0)
			return 0;

		if (inLoc > 0) {
			map<int, LocationInfo*>::iterator it = _defLoc.find(inLoc);
			if (it == _defLoc.end()) {
				_ctrl->errHandler().error(ERROR_LVL_NORMAL, _ctrl->printBuffer("use of undefined DefLoc number %d", inLoc), posOrg);
				return 0;
			}

			pos.setInLoc(it->second);
			return inLoc;
		}

		else if (inLoc <= -2) {
			size_t i = -inLoc - 2;
			if (i < _curPos.size()) {
				const LocationInfo *loc = _curPos[_curPos.size()-i-1]->inLoc();
				if (loc) {
					pos.setInLoc(loc);
					return outDefLoc(loc, true);
				}
			}

			return 0;
		}

		else if (inLoc == -1) {
			PositionInfo p = *(curPos());
			LocationInfo loc(p);

			inLoc = outDefLoc(&loc, true);
			pos.setInLoc(_defLoc[inLoc]);
			return inLoc;
		}

		return 0;
	}

	/**
	 * parse DefLoc position mark
	 * @param txt               text with DefLoc info
	 */
	void Precompiler::parseDefLocPM(const char *txt)
	{
		LocationInfo loc;
		int defNr;
		int inLoc = curPos()->parseDefLocPM(_ctrl, txt, _data->globStrings(), defNr, loc);

		if (inLoc) {
			PositionInfo pos = loc.begin();
			inLoc = setInLocPM(*(curPos()), pos, inLoc);

			PositionInfo pos2 = pos;
			pos2.setLine(loc.end().line());
			pos2.setCol(loc.end().col());

			loc = LocationInfo(pos, pos2);
		}

		outDefLoc(&loc, true, defNr);
	}

	/**
	 * parse StartPos position mark
	 * @param txt               text with StartPos info
	 */
	void Precompiler::parseStartPosPM(const char *txt)
	{
		PositionInfo posOrg = *(curPos());
		int inLoc = curPos()->parseStartPosPM(_ctrl, txt, _data->globStrings());
		if (inLoc)
			inLoc = setInLocPM(posOrg, *(curPos()), inLoc);

		outStartPos(*(curPos()), inLoc);
	}

	/**
	 * parse Line position mark
	 * @param txt               text with line number
	 */
	void Precompiler::parseLinePM(const char *txt)
	{
		curPos()->parseLinePM(_ctrl, txt);
		resStream() << "##Line:" << curPos()->line() << endl;
	}

	/**
	 * parse Col position mark
	 * @param txt               text with col number
	 */
	void Precompiler::parseColPM(const char *txt)
	{
		curPos()->parseColPM(_ctrl, txt);
		resStream() << "##Col:" << curPos()->col() << endl;
	}

	/**
	 * output error message about wrong position mark
	 * @param txt				text with error
	 */
	void Precompiler::outErrorPositionMark(const char *txt)
	{
		_ctrl->errHandler().error(ERROR_LVL_NORMAL, ctrl()->printBuffer("illegal character in position mark '%s'", txt), *(curPos()));
	}
}

