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


#include "MainControl.hh"
#include "ModulesConf.hh"
#include "ModuleBase.hh"
#include "MainData.hh"
#include "FileBase.hh"
#include "ExtensionModule.hh"



namespace cmpl
{
	/**
	 * destructor
	 */
	MainControl::~MainControl()
	{
        if (_optOutFile)
            delete _optOutFile;

		// free _modules
		_itO = _modules.begin();
		while (_itO != _modules.end()) {
			vector<Module *> &modList = _itO->second;

			for (_indModule = 0; _indModule < modList.size(); _indModule++)
				delete modList[_indModule];

			_itO++;
		}

		// free extension objects
		deleteExtensions();

#if PROTO
		// close all protocol outputs
		PROTO_OUTL("");
		for (map<int, FileOutput *>::iterator itp = _protoFiles.begin(); itp != _protoFiles.end(); itp++) {
			try {
				itp->second->close();
			}
			catch (exception& e) { }
		}
		_protoStream = NULL;
#endif //PROTO

		// remove reference to this from _modConf
		if (_modConf)
			_modConf->setCtrl(NULL);

		if (_data && _data->globStrings())
			_data->globStrings()->setModp(NULL, "global strings");

        delete[] _printBuffer;
	}


	/**
	 * initialize modul, to call immediately after the constructor.
	 * @param ctrl          main object (this)
	 * @param data			data object
	 * @param name          module name (or alias)
	 */
	void MainControl::init(MainControl *ctrl, MainData *data, const char *name)
	{
		_printBufferCap = 1000;
		_printBuffer = new char[_printBufferCap];

		ModuleBase::init(ctrl, data, name);
		_errHandler.init(ctrl);

		if (_data && _data->globStrings())
			_data->globStrings()->setModp(this, "global strings");

		_cmdLineOptNames.setModp(this, "command line options");
#if PROTO
		_protoFileNames.setModp(this, "protocol file names");
#endif

		_errHandler.setCurModule(_modName);
		_modNameSet.insert(_modName);
		_modNames.push_back(_modName);

		_modConf = NULL;

		_nextExtensionId = 1;

		_instanceName = "cmpl";	// default for instance name

        _optOutFile = NULL;
        _optWarnUnused = true;
        _isSilentStart = false;

        _printVersion = _printUsage = _usageAll = false;
	}


	/**
	 * run execution of modules
	 * @param opts			command line options
	 * @return				exit code
	 */
	int MainControl::run(CmdLineOptList *opts)
	{
		int exitCode = -1;

		try {
			_errHandler.setCurModule(moduleName());

            _modConfigFile = modConfigFileDefault();
            _modConfLoc = LocationInfo(PositionInfo(POSITION_TYPE_DESCR, "default for module config file"));
            _startOpts = opts;

            // handle output of version or usage
            parseControlOpts(opts, true, false, false, false, false, false);
			if (_printVersion || _printUsage) {
				if (_printVersion)
					version(cout);

                if (_printUsage)
					usage(cout);
			}
			else {
                // search for command line option file
                parseControlOpts(opts, false, true, false, false, false, false);

				// search for file base name, then protocol output, and then module config file
                parseControlOpts(_startOpts, false, false, true, false, false, false);
#if PROTO
                parseControlOpts(_startOpts, false, false, false, true, false, false);
#endif
                parseControlOpts(_startOpts, false, false, false, false, true, false);

				// start main functionality
                run();
			}

			// success
			exitCode = 0;
		}
		catch (FatalException& e) {
			exitCode = e.exitCode();
		}
		catch (ModuleException) {
			exitCode = -2;
		}
        catch (exception& e) {
			cerr << "internal cmpl error: " << e.what() << endl;
			exitCode = -1;
		}

		return exitCode;
	}


	/**
	 * run module function
	 */
	void MainControl::run()
	{
		// initialize module names, if not done yet
		PROTO_OUTL("initialize module names from file: " << _modConfigFile);
		if (!_startOpts->_modConf || _modConfigFile != *(_startOpts->_modConf->filename())) {
			if (_startOpts->_modConf)
				delete _startOpts->_modConf;

			_modConf = _startOpts->_modConf = new ModulesConf(this, &_modConfigFile, _modConfLoc);
		}
		else {
			_startOpts->_modConf->setCtrl(this);
			_modConf = _startOpts->_modConf;
		}

		// parse regular command line options, including module names
		PROTO_OUTL("parse regular command line options, including module names");
		parseCmdLineOpts(_startOpts, CMDLINE_OPT_PARSE_START, _modNames);
		parseCmdLineOpts(&_modConfOpts, CMDLINE_OPT_PARSE_MODCONF, _modNames);

		// if no module, then use default
		if (_modules.size() == 0) {
			string defMod = "<default>";
			PositionInfo defPos(POSITION_TYPE_DESCR, "default for module aggregation");
			_modConf->parseCmdLineOptModule(defMod, true, defPos);
		}

        if (!_isSilent && !_isSilentStart) {
            version(cout);
            CmplOutput(cout, "Interpreting Cmpl code");
        }

		// iterating over modules
		PROTO_OUTL("start iterating over modules");
		map<int, vector<Module *>>::iterator itO;
		Module *mod;
		ModuleBase *modObj;

		_itO = _modules.begin();
		_indModule = 0;

		while (_itO != _modules.end()) {
			vector<Module *> &modList = _itO->second;

			if (_indModule >= modList.size()) {
				_itO++;
				_indModule = 0;
				continue;
			}
			mod = modList[_indModule++];
			if (mod->executed)
				continue;

			const char *modName = (mod->alias ?: mod->name); 

			// construct module class
			PROTO_OUTL("construct module class: " << modName);
			_errHandler.setCurModule(modName);

			try {
				modObj = _modConf->constructModule(mod->name, modName);

				if (_data->errorLevelMax() <= modObj->maxErrorLevelRun()) {
					// initialize module with command line options
					PROTO_OUTL("parse command line options for module: " << modName);
					modObj->parseCmdLineOpts(_startOpts, CMDLINE_OPT_PARSE_START, mod->modNames);
					modObj->parseCmdLineOpts(&_modConfOpts, CMDLINE_OPT_PARSE_MODCONF, mod->modNames);
					modObj->parseCmdLineOpts(&_headerOpts, CMDLINE_OPT_PARSE_HEADER, mod->modNames);
					
					// run module
					PROTO_OUTL("run module: " << modName);
					_runningMod[modObj] = mod;
					_data->addExecutedModules(modName, (mod->alias ? mod->name : NULL), instanceName());

                    if (!modObj->_deactivated)
                        modObj->run();

					moduleEndExtensions(modObj);
					_runningMod.erase(modObj);
				}
				else {
					PROTO_OUTL("skip module (because of previous errors): " << modName);
				}
			}
			catch (ModuleException) {
				_runningMod.erase(modObj);
				if (_data->errorLevelMax() < ERROR_LVL_CANCEL)
					_data->setErrorLevelMax(ERROR_LVL_CANCEL);
			}

			_errHandler.setCurModule(moduleName());

			// delete module object
			PROTO_OUTL("delete module class after run: " << modName);
			delete modObj;

			// mark the module as executed
			mod->executed = true;
			_data->setExecutedModuleFinished(modName, instanceName());
		}

		PROTO_OUTL("end iterating over modules");


        // warning if unused command line options (only checked if no errors)
        bool warnUnused = (_data->errorLevelMax() <= ERROR_LVL_WARN && _optWarnUnused);
        _startOpts->checkCmdLineOptUsed(this, _optMarkUsed, warnUnused);
        _modConfOpts.checkCmdLineOptUsed(this, _optMarkUsed, warnUnused);
        _headerOpts.checkCmdLineOptUsed(this, _optMarkUsed, warnUnused);

        if (_optOutFile) {
            writeOptFile(*_optOutFile);
            delete _optOutFile;
            _optOutFile = NULL;
        }

        _ctrl->errHandler().writeCmplMsg();

        if (_ctrl->data()->errorCnt()>0 || _ctrl->data()->warnCnt()>0) {
            string oMsg;
            if (_ctrl->data()->errorCnt()>0) {
                oMsg="errors";
                if (_ctrl->data()->warnCnt()>0)
                    oMsg+= " and warnings";
            }  else {
                 oMsg="warnings";
            }
            CmplOutput(cout,"Cmpl has finished with "+oMsg);
        }
    }


	/*********** handling of command line options **********/

// defines for reference numbers of options
#define OPTION_ERROR_HANDLE         10
#define OPTION_ERROR_CMPLMSG        11

#define OPTION_OPTS_OUTFILE         15
#define OPTION_OPTS_WARN_UNUSED     16
#define OPTION_OPTS_MARK_USED       17

#define OPTION_SILENT_START         18

	/**
	 * register command line options for delivery to this module
	 * @param modOptReg     vector to register in
	 */
	void MainControl::regModOptions(vector<CmdLineOptList::RegOption> &modOptReg)
	{
		ModuleBase::regModOptions(modOptReg);
		REG_CMDL_OPTION( OPTION_ERROR_HANDLE, "e", 0, 1, CMDL_OPTION_NEG_ERROR, true );
        REG_CMDL_OPTION( OPTION_ERROR_CMPLMSG, "cmsg", 0, 1, CMDL_OPTION_NEG_ERROR, true );

        REG_CMDL_OPTION( OPTION_OPTS_OUTFILE, "o-opt", 0, 1, CMDL_OPTION_NEG_NO_ARG, true );
        REG_CMDL_OPTION( OPTION_OPTS_WARN_UNUSED, "warn-unused", 0, 0, CMDL_OPTION_NEG_DELIV, true );
        REG_CMDL_OPTION( OPTION_OPTS_MARK_USED, "mark-used", 0, -1, CMDL_OPTION_NEG_DELIV, false );

        REG_CMDL_OPTION( OPTION_SILENT_START ,"silent-start", 0, 0, CMDL_OPTION_NEG_NO_ARG, false );
    }

    /**
     * get names of command line options which are not to write to a command line option file
     * @return              set with names of these command line options
     */
    set<string> MainControl::noWriteOptions()
    {
        set<string> res({ "h", "help", "v", "config", "instance", "o-opt", "i-opt", "warn-unused", "mark-used", "modules" });
        return res;
    }

	/**
	 * parse single option from command line options, this function is called for every delivered option
	 * @param ref           reference number of option registration, should be used for discriminate the options
	 * @param prio          priority value of option
	 * @param opt           option
	 * @return              true if option is used by the module
	 */
	bool MainControl::parseOption(int ref, int prio, CmdLineOptList::SingleOption *opt)
	{
		if (ModuleBase::parseOption(ref, prio, opt))
			return true;

        switch (ref) {
        // error handling
        case OPTION_ERROR_HANDLE:

            _errorFile.setFile(_data, IO_MODE_FILE, (opt->size() >0 ? &((*opt)[0]) : NULL), IO_FILE_STANDARD_ERROR, true);

            if (FileBase::exists(_errorFile.fileNameReplaced()))
                remove(_errorFile.fileNameReplaced().c_str());

            _errHandler.setErrOut(&_errorFile);
            return true;

        // error handling via cmplMsg file
        case OPTION_ERROR_CMPLMSG:

            _cmplMsgFile.setFile(_data, IO_MODE_FILE, (opt->size() >0 ? &((*opt)[0]) : NULL), IO_FILE_CMPLMSG, true);

            if (FileBase::exists(_cmplMsgFile.fileNameReplaced()))
                remove(_cmplMsgFile.fileNameReplaced().c_str());

            //_errHandler.setCmplMsg(true);
            _errHandler.setCmplMsgOut(&_cmplMsgFile);

            return true;

        // write all command line options
        case OPTION_OPTS_OUTFILE:
            if (_optOutFile) {
                delete _optOutFile;
                _optOutFile = NULL;
            }
            if (!(opt->neg())) {
                _optOutFile = new FileOutput();
                _optOutFile->setFile(_data, IO_MODE_FILE, (opt->size() > 0 ? &((*opt)[0]) : NULL), IO_FILE_STANDARD_OOPT, true);
                if (opt->size() > 0)
                    _optOutFile->setLocSrc(opt->argPos(0));
                else
                    _optOutFile->setLocSrc(opt->loc(true));
            }
            return true;

        // output warning if a command line option ist not used
        case OPTION_OPTS_WARN_UNUSED:
            _optWarnUnused = !opt->neg();
            return true;

        // mark command line options given as arguments as used
        case OPTION_OPTS_MARK_USED:
            for (size_t i = 0; i < opt->size(); i++) {
                if (!(opt->neg()))
                    _optMarkUsed.insert((*opt)[i]);
                else
                    _optMarkUsed.erase((*opt)[i]);
            }
            return true;

        case OPTION_SILENT_START:
            _isSilentStart = !opt->neg();
            return true;
        }

		return false;
	}


	/**
	 * parse command line options for the module
	 * @param opts			command line options
	 * @param step          parse step (one of <code>CMDLINE_OPT_PARSE_*</code>)
	 * @param modNames		relevant names for module specification in command line options
	 */
	void MainControl::parseCmdLineOpts(CmdLineOptList *opts, int step, vector<const char *> &modNames)
	{
		// parse registered options
		ModuleBase::parseCmdLineOpts(opts, step, modNames);

		// parse module names
		if (step != CMDLINE_OPT_PARSE_MODCONF)
            parseControlOpts(opts, false, false, false, false, false, true);
	}


	/**
	 * parse special command line options only for MainControl
	 * @param opts          command line options
     * @param vh			search for options for print version or usage
     * @param io            search for option for command line option file
	 * @param fbn			search for option for file base name and instance name
	 * @param prt			search for option for protocol output
	 * @param mcf			search for option for module config file
	 * @param mod			search for options for module names
	 */
    void MainControl::parseControlOpts(CmdLineOptList *opts, bool vh, bool io, bool fbn, bool prt, bool mcf, bool mod)
	{
		// iteration over options in opts
		int iter = -1;
		int modspec;
		CmdLineOptList::SingleOption *opt;

        while ((opt = opts->iterOptions(&_modNames, iter, modspec))) {
			if (vh) {
				// print version
				if (opt->opt() == "v" && !opt->neg()) {
					_printVersion = true;
					opt->markUsed();
				}

				// print usage
                if ((opt->opt() == "h" || opt->opt() == "help") && !opt->neg()) {
					_printUsage = true;
                    _usageAll = (opt->size() == 0 && opt->opt() == "help");
					opt->markUsed();

					if (opt->size() == 0)
						_usageModule.clear();
					else
						_usageModule = (*opt)[0];
				}
			}

            if (io) {
                // command line option file
                if (opt->opt() == "i-opt" && !opt->neg()) {
                    if (opt->size() != 1)
                        errHandler().error(ERROR_LVL_NORMAL, _ctrl->printBuffer("wrong argument count %d for command line option '%s'", (int)opt->size(), opt->opt().c_str()), opt->loc(true));
                    else
                        readFromOptFile((*opt)[0], opt->argPos(0), opt->loc(true));

                    opt->markUsed();
                }
            }

			if (fbn) {
				// file base name
				if (opt->opt() == "basename" && !opt->neg()) {
					if (opt->size() != 1)
						errHandler().error(ERROR_LVL_NORMAL, _ctrl->printBuffer("wrong argument count %d for command line option '%s'", (int)opt->size(), opt->opt().c_str()), opt->loc(true));
					else
						_data->setFileBaseBase((*opt)[0]);

					opt->markUsed();
				}

				// input file for file base name
				if (opt->opt() == "i" && !opt->neg() && opt->size() > 0 && !(_data->_cmplFileBase) && (*opt)[0] != "0") {
					_data->setFileBaseBase((*opt)[0].substr(0, (*opt)[0].find_last_of('.')));
				}

				// instance name
				if (opt->opt() == "instance" && !opt->neg()) {
					if (opt->size() != 1)
						errHandler().error(ERROR_LVL_NORMAL, _ctrl->printBuffer("wrong argument count %d for command line option '%s'", (int)opt->size(), opt->opt().c_str()), opt->loc(true));
					else
						_instanceName = (*opt)[0];

					opt->markUsed();
				}
			}

#if PROTO
			if (prt) {
				// protocol output
				if (opt->opt() == "p" && !opt->neg()) {
					try {
						FileOutput temp;
						temp.setFile(_data, (opt->size() == 1 ? IO_MODE_FILE : IO_MODE_STD_IO), (opt->size() == 1 ? &((*opt)[0]) : NULL), IO_FILE_STANDARD_PROTO, true);
						_protoStream = initProto(temp);
					}
					catch (FileException& e) {
						errHandler().error(ERROR_LVL_EASY, _ctrl->printBuffer("%s: protocol output file '%s'", e.what(), e.filename().c_str()), opt->loc(true));
					}
					opt->markUsed();
				}
			}
#endif

			if (mcf) {
				// module config file
				if (opt->opt() == "config" && !opt->neg()) {
					if (opt->size() != 1)
						errHandler().error(ERROR_LVL_NORMAL, _ctrl->printBuffer("wrong argument count %d for command line option '%s'", (int)opt->size(), opt->opt().c_str()), opt->loc(true));
					else
						_modConfigFile = (*opt)[0];

					_modConfLoc = opt->loc(true);
					opt->markUsed();
				}
			}

			if (mod) {
				// modules
				if (opt->opt() == "modules") {
					if (!opt->neg()) {
						for (size_t i=0; i < opt->size(); i++) {
							LocationInfo argLoc(opt->argPos(i));
							PositionInfo argPos(&argLoc, POSITION_TYPE_FIXLOC);
							_modConf->parseCmdLineOptModule((*opt)[i], true, argPos);
						}
					}
					else {
						errHandler().error(ERROR_LVL_EASY, _ctrl->printBuffer("negation not allowed for option '%s'", opt->opt().c_str()), opt->loc(false));
					}
					opt->markUsed();
				}
				else {
					if (_modConf->parseCmdLineOptModule(opt->opt(), false, opt))
						opt->markUsed();
				}
			}
		}

        // if no option, then print usage
        if (!iter && vh) {
            _printUsage = true;
        }
	}

    /**
     * read command line options from file
     * @param fn            file name
     * @param npos          position info for file name
     * @param nloc          location info of option with file name
     */
    void MainControl::readFromOptFile(string &fn, PositionInfo &npos, LocationInfo &nloc)
    {
        FileInput file;
        istream *inStr;

        try {
            file.setFile(_data, IO_MODE_FILE, &fn, NULL);
            file.setLocSrc(npos);

            inStr = file.open();
        }
        catch (FileException& e) {
            errHandler().error(ERROR_LVL_NORMAL, _ctrl->printBuffer("%s: command line option file '%s'", e.what(), fn.c_str()), npos);
        }

        PositionInfo fpos(&nloc, POSITION_TYPE_FILE, fn.c_str());
        try {
            string bn = CmdLineOptList::readFromOptFile(this, inStr, fpos, *_startOpts, _headerOpts, "basename");
            if (!bn.empty())
                _data->setFileBaseBase(bn);
        }
        catch (FileException& e) {
            errHandler().error(ERROR_LVL_NORMAL, _ctrl->printBuffer("%s within command line option file", e.what()), fpos);
        }
    }


	/**
	 * append command line options
	 * @param opts          command line options
	 * @param step          origin of command line options (one of <code>CMDLINE_OPT_PARSE*</code>)
	 * @param curMod        use this command line options also for current running module
	 * @param ctrlMod		use this command line options also for main control, especially for modules names
	 */
	void MainControl::appendCmdLineOpts(CmdLineOptList *opts, int step, bool curMod, bool ctrlMod)
	{
		// use for current running modules
		if (curMod) {
			for (map<ModuleBase*, Module*>::iterator it = _runningMod.begin(); it != _runningMod.end(); it++) {
				ModuleBase *modObj = it->first;
				Module *mod = it->second;

				modObj->parseCmdLineOpts(opts, step, mod->modNames);
			}
		}

		// use for main control
		if (ctrlMod) {
			parseCmdLineOpts(opts, step, _modNames);
		}
		
		// append command line options
		CmdLineOptList *cl = (step==CMDLINE_OPT_PARSE_START ? _startOpts : (step==CMDLINE_OPT_PARSE_MODCONF ? &_modConfOpts : (step==CMDLINE_OPT_PARSE_HEADER ? &_headerOpts : NULL)));
		if (cl)
			cl->append(*opts);
	}


	/**
	 * insert used strings and locations for command line options in the data object
	 */
	void MainControl::insertCmdLineOptsStringsAndLocs(MainData *data)
	{
		_startOpts->insertStringsAndLocs(data);
		_modConfOpts.insertStringsAndLocs(data);
		_headerOpts.insertStringsAndLocs(data);
	}

	/**
	 * write data for serialization of the command line options to output stream
	 * @param si		serialization info
	 * @param data      main data object
	 */
	void MainControl::serializeCmdLineOptsTo(MainData::SerializeInfo& si, const MainData *data)
	{
		(*si.ostr) << "*StartOpts:" << _startOpts->subCnt() << ";1" << endl;
		_startOpts->serializeTo(si, data);

		(*si.ostr) << "*ConfOpts:" << _modConfOpts.subCnt() << ";1" << endl;
		_modConfOpts.serializeTo(si, data);

		(*si.ostr) << "*HeaderOpts:" << _headerOpts.subCnt() << ";1" << endl;
		_headerOpts.serializeTo(si, data);
	}

	/**
	 * read command line options from serialization stream
	 * @param si		serialization info
	 * @param data      main data object
	 * @param cnt		count of elements within the section
	 */
	void MainControl::deserializeCmdLineOptsFrom(MainData::SerializeInfo& si, const MainData *data, int cnt)
	{
		int outLevel, subCnt, lc;
		string sect, rline;

		for (int ss = 0; ss < cnt; ss++) {
			sect = data->readSectionStart(si, 1, outLevel, subCnt, lc, rline);
			si.pos->addLines();

			if (sect == "StartOpts")
				_startOpts->deserializeFrom(si, data, subCnt);
			else if (sect == "ConfOpts")
				_modConfOpts.deserializeFrom(si, data, subCnt);
			else if (sect == "HeaderOpts")
				_headerOpts.deserializeFrom(si, data, subCnt);
			else
				data->skipSection(si, outLevel, subCnt, lc);
		}
	}


    /**
     * write command line options to file
     * @param file      file to write
     */
    void MainControl::writeOptFile(FileOutput& file)
    {
        try {
            PROTO_OUTL("write command line options to file '" << file.fileNameReplaced() << "'");

            ostream *ostr = file.open();

            set<string> nowrite = noWriteOptions();
            for (const ModulesConf::ModDescr& mod : _modConf->_modDescr)
                nowrite.insert(mod.name);

            bool optbn = false;
            if (_startOpts->writeToOptFile(ostr, 1, nowrite, "basename"))
                optbn = true;
            if (_headerOpts.writeToOptFile(ostr, 2, nowrite, "basename"))
                optbn = true;

            if (!optbn && _data->_cmplFileBase)
                CmdLineOptList::writeToOptFile(ostr, 0, "basename", _data->_cmplFileBase, NULL);

            file.close();
        }
        catch (FileException& e) {
            errHandler().error(ERROR_LVL_NORMAL, printBuffer("%s: write command line options file '%s'", e.what(), e.filename().c_str()), file.locSrc());
        }
        catch (exception& ex) {
            errHandler().error(ERROR_LVL_NORMAL, "write command line options file error", file.locSrc(), &ex);
        }
    }


	/**
	 * writes usage info for the module to stream
	 * @param s             stream to write to
	 */
	void MainControl::usage(ostream& s)
	{
        // read module config file
        parseControlOpts(_startOpts, false, false, false, false, true, false);
        ModulesConf modConf(this, &_modConfigFile, _modConfLoc);

        if (_usageAll || _usageModule.empty()) {
			s << endl << "usage info: cmpl [options ...]" << endl;
		
			s << endl << "options for cmpl main control:" << endl;
			s << "  -h [<module>]                 print this usage info or the usage info for a module" << endl;
            s << "  -help                         print this usage info and the usage info for all modules" << endl;
            s << "  -v                            print version info" << endl;
			s << "  -e [<file>]                   output error messages to file or stderr" << endl;
            s << "  -cmsg [<file>]                error handling via cmplMsg file" << endl;
			s << "  -config <file>                use <file> for module configuration instead of " << modConfigFileDefault() << endl;
			s << "  -basename <name>              use <name> to derive other file names. if not given then derive it from first cmpl input file" << endl;
			s << "  -instance <name>              set <name for this cmpl instance, only used for tracking of module execution in a client/server environment" << endl;
            s << "  -o-opt [<file>]               write all command line options to <file>" << endl;
            s << "  -i-opt <file>                 read command line options from <file>" << endl;
            s << "  -warn-unused                  output warning if a command line option ist not used (enabled by default)" << endl;
            s << "  -mark-used <opt1> ...         mark command line options given as arguments as used" << endl;
            s << "  -silent-start                 suppresses output of start message" << endl;

            ModuleBase::usage(s);

            s << "  -modules <mod1> ...           use this modules or module aggregations for execution" << endl;
            s << "                                    you can also use every module name as a single option" << endl;

			for (int ma = 0; ma < 2; ma++) {
				if (ma==0)
					s << endl << "following modules are registered:" << endl;
				else
					s << endl << "following module aggregations are configured in file \"" << modConfigFileDefault() << "\":" << endl;

                for (vector<ModulesConf::ModDescr>::iterator it = modConf._modDescr.begin(); it != modConf._modDescr.end(); it++) {
					if ((ma==1) == it->modAggr) {
						string a(it->name);
						string b((a.size() > 27 ? 3 : (30 - a.size())), ' ');
						s << "  " << a << b << it->descr << endl;
					}
				}
			}
			s << endl;

            if (_usageAll) {
                for (unsigned m = 0; m < modConf._modNames.size(); m++) {
                    const char *mod = modConf._modNames[m];
                    if (modConf._modules.count(mod) > 0)
                        usageModule(s, modConf, mod);
                }
            }
		}

        else {
            const char *mod = modConf._modNames.store(_usageModule);
            if (modConf._modules.count(mod) > 0)
                usageModule(s, modConf, mod);
            else
				cerr << "no module '" << _usageModule << "'" << endl;
		}
	}

    /**
     * writes usage info for a given module to stream
     * @param s             stream to write to
     * @param modConf       object for construction and configuration of module
     * @param mod           module name
     */
    void MainControl::usageModule(ostream& s, ModulesConf& modConf, const char *mod)
    {
        ModuleBase *modObj = modConf.constructModule(mod, mod);

        s << endl << "module '" << mod << "'";
        for (vector<ModulesConf::ModDescr>::iterator it = modConf._modDescr.begin(); it != modConf._modDescr.end(); it++) {
            if (!it->modAggr && it->name == mod) {
                s << ": " << it->descr;
            }
        }
        s << endl;

        s << "options:" << endl;
        modObj->usage(s);
        s << endl;
        delete modObj;
    }


	/**
	 * writes version info to stream
	 * @param s             stream to write to
	 */
	void MainControl::version(ostream& s)
	{
		s << endl << "CMPL version: " << CMPL_VERSION << endl;
		s << "Authors: Thomas Schleiff, Mike Steglich" << endl;
		s << "Distributed under the GPLv3 " << endl <<endl;
	}


	/**
	 * run directly another module with special options.
	 * @param modName       name of module to start
	 * @param opts          pseudo command line options to deliver the module arbitrary parameters (the module doesn't get the normal command line options)
	 * (if data is to deliver to or from the called module then use the global data object.)
	 */
	void MainControl::runModuleFunc(const char *modName, CmdLineOptList *opts)
	{
		const char *name = _modConf->testModuleName(modName);

		PROTO_OUTL("directly run module: " << name);
		const char *curMod = _errHandler.curModule();
		const char *curStp = _errHandler.execStep();
		_errHandler.setCurModule(name);

		// construct module class
		ModuleBase *modObj = _modConf->constructModule(name, name);

		try {
			// initialize module with command line options
			if (opts) {
				vector<const char *> modNames;
				modNames.push_back(name);

				PROTO_OUTL("  parse command line options for module: " << name);
				modObj->parseCmdLineOpts(opts, CMDLINE_OPT_PARSE_START, modNames);
			}

			// run module
			PROTO_OUTL("  run module: " << name);
			modObj->run();
		}
		catch (ModuleException) {
			PROTO_OUTL("exception in module: " << name);

			_errHandler.setCurModule(curMod);
			_errHandler.setExecStep(curStp, ERROR_LVL_FATAL);

			// delete module object
			delete modObj;

			// forward exception to calling module
			throw;
		}

		_errHandler.setCurModule(curMod);
		_errHandler.setExecStep(curStp, ERROR_LVL_FATAL);

		// delete module object
		PROTO_OUTL("delete module class after run: " << name);
		delete modObj;
	}


	/**
	 * run all registered extensions for a module after running the module
	 * @param mod			module calling the extension
	 */
	void MainControl::moduleEndExtensions(ModuleBase *mod)
	{
		const char *n = mod->moduleName();
        if (_regExtensionsB.count(n) > 0)
		{
			set<ExtensionBase *> called;
            map<int, map<int, ExtensionBase *>>& extm = _regExtensionsB[n];

			for (map<int, map<int, ExtensionBase *>>::iterator itm = extm.begin(); itm != extm.end(); itm++) {
				map<int, ExtensionBase *>& exts = itm->second;

				for (map<int, ExtensionBase *>::iterator its = exts.begin(); its != exts.end(); its++) {
					ExtensionBase *ext = its->second;
					if (called.count(ext) == 0) {
						ext->moduleEnd(mod, its->first);
						called.insert(ext);
					}
				}
			}
		}
	}

	/**
	 * delete extension objects in _regExtensions
	 */
	void MainControl::deleteExtensions()
	{
		set<ExtensionBase *> objs;

        for (map<int, map<int, ExtensionBase *>>::iterator itm = _regExtensionsA.begin(); itm != _regExtensionsA.end(); itm++) {
            map<int, ExtensionBase *>& exts = itm->second;

            for (map<int, ExtensionBase *>::iterator its = exts.begin(); its != exts.end(); its++)
                objs.insert(its->second);
        }

        for (map<const char *, map<int, map<int, ExtensionBase *>>>::iterator itc = _regExtensionsB.begin(); itc != _regExtensionsB.end(); itc++) {
			map<int, map<int, ExtensionBase *>>& extm = itc->second;

			for (map<int, map<int, ExtensionBase *>>::iterator itm = extm.begin(); itm != extm.end(); itm++) {
				map<int, ExtensionBase *>& exts = itm->second;

				for (map<int, ExtensionBase *>::iterator its = exts.begin(); its != exts.end(); its++)
					objs.insert(its->second);
			}
		}

        _regExtensionsA.clear();
        _regExtensionsB.clear();

		for (set<ExtensionBase *>::iterator it = objs.begin(); it != objs.end(); it++) {
			delete *it;
		}
	}

	/**
	 * count the registrations for an extension object
	 * @param ext			searched extension
	 * @return				count of registrations
	 */
	unsigned MainControl::countExtension(ExtensionBase *ext)
	{
		unsigned cnt = 0;

        for (map<int, map<int, ExtensionBase *>>::iterator itm = _regExtensionsA.begin(); itm != _regExtensionsA.end(); itm++) {
            map<int, ExtensionBase *>& exts = itm->second;

            for (map<int, ExtensionBase *>::iterator its = exts.begin(); its != exts.end(); its++) {
                if (its->second == ext)
                    cnt++;
            }
        }

        for (map<const char *, map<int, map<int, ExtensionBase *>>>::iterator itc = _regExtensionsB.begin(); itc != _regExtensionsB.end(); itc++) {
			map<int, map<int, ExtensionBase *>>& extm = itc->second;

			for (map<int, map<int, ExtensionBase *>>::iterator itm = extm.begin(); itm != extm.end(); itm++) {
				map<int, ExtensionBase *>& exts = itm->second;

				for (map<int, ExtensionBase *>::iterator its = exts.begin(); its != exts.end(); its++) {
					if (its->second == ext)
						cnt++;
				}
			}
		}

		return cnt;
	}

    /**
     * get registered extension object
     * @param mod			name of module calling the extension / NULL: n
     * @param step			execution step within the module
     * @param id			identificator
     * @return              extension object or NULL if no one is registered
     */
    ExtensionBase *MainControl::getExtension(const char *n, int step, int id)
    {
        // search first with module name and second without
        for (int intStep = 0; intStep < 2; intStep++) {
            if (intStep == 0 && _regExtensionsB.count(n) == 0)
                continue;

            map<int, map<int, ExtensionBase *>>& extm = (intStep == 0 ? _regExtensionsB[n] : _regExtensionsA);
            if (extm.count(step) == 0)
                continue;

            map<int, ExtensionBase *>& exts = extm[step];
            if (exts.count(id) == 0)
                continue;

            return exts[id];
        }

        return NULL;
    }

	/**
	 * run all registered extensions for a module and execution step
	 * @param mod			module calling the extension
	 * @param step			execution step within the module
	 * @param par			additional parameter
	 * @return				count of executed extensions
	 */
	unsigned MainControl::runExtension(ModuleBase *mod, int step, void *par)
	{
        unsigned cnt = 0;
		const char *n = mod->moduleName();
		PROTO_OUTL("run all extensions:  " << n << " (" << (unsigned long)n << ") " << step);

        // run first with module name and second without
        for (int intStep = 0; intStep < 2; intStep++) {
            if (intStep == 0 && _regExtensionsB.count(n) == 0)
                continue;

            map<int, map<int, ExtensionBase *>>& extm = (intStep == 0 ? _regExtensionsB[n] : _regExtensionsA);
            if (extm.count(step) == 0)
                continue;

            map<int, ExtensionBase *>& exts = extm[step];
            unsigned cntS = exts.size();

            if (cntS > 0) {
                cnt += cntS;
                for (map<int, ExtensionBase *>::iterator it = exts.begin(); it != exts.end(); it++)
                    it->second->run(mod, step, it->first, par);
            }
        }

		return cnt;
	}

	/**
	 * run registered extension for a given identificator
	 * @param mod			module calling the extension
	 * @param step			execution step within the module
	 * @param id			identificator
	 * @param par			additional parameter
	 * @return				count of executed extensions
	 */
	unsigned MainControl::runExtension(ModuleBase *mod, int step, int id, void *par)
	{
		const char *n = mod->moduleName();
		PROTO_OUTL("run one extension:  " << n << " (" << (unsigned long)n << ") " << step << " " << id);

        ExtensionBase *ext = getExtension(n, step, id);
        if (!ext)
			return 0;

		ext->run(mod, step, id, par);
		return 1;
	}

    /**
     * run registered extension for handling of a command line option
     * @param mod			module calling the extension
     * @param step			execution step within the module
     * @param id			identificator
     * @param ref			reference number of option registration, should be used for discriminate the options
     * @param prio			priority value of option
     * @param opt			option
     * @param par			additional parameter
     * @return				true if option is used by the extension
     */
    bool MainControl::runExtension(ModuleBase *mod, int step, int id, int ref, int prio, CmdLineOptList::SingleOption *opt, void *par)
    {
        const char *n = mod->moduleName();
        PROTO_OUTL("run one extension:  " << n << " (" << (unsigned long)n << ") " << step << " " << id << " for command line option " << opt->opt() << " (" << ref << ", " << prio << ")");

        ExtensionBase *ext = getExtension(n, step, id);
        if (!ext)
            return false;

        return ext->run(mod, step, id, ref, prio, opt, par);
    }

    /**
     * register command line option for extension
     * @param n				module name
     * @param step          execution step within the module
     * @param ext			extension object
     * @param ref           reference number for command line option
     * @param opt           option
     * @param mi            minimal count of args for the option
     * @param ma            maximal count of args for the option / -1: unbound
     * @param neg           negated option: 0: don't deliver; -1: throw error; 1: deliver with check of minArgs and maxArgs; 2: deliver but cannot have any arg
     * @param hp            deliver option only if priority is higher / false: deliver independent of priority
     * @param id            identificator of the extension
     * @param mod           extension module name
     */
    void MainControl::regCmdOptExtension(const char *n, int step, ExtensionBase *ext, int ref, const char *opt, int mi, int ma, int neg, bool hp, int id, const char *mod)
    {
        vector<CmdLineOptList::RegOptionExt>& vro = _regExtOptions[step];
        vro.push_back(CmdLineOptList::RegOptionExt(ref, opt, mi, ma, neg, hp, id, mod));

        registerExtension(n, step, id, ext);
    }

    /**
     * get registration of command line option for an extension step
     * @param step          execution step within the module
     * @return              registration for the extension step
     */
    vector<CmdLineOptList::RegOptionExt> *MainControl::getRegCmdOptExtension(int step)
    {
        if (_regExtOptions.count(step) > 0)
            return &(_regExtOptions[step]);
        else
            return NULL;
    }

	/**
	 * register extension using a new unique identificator
	 * @param n				module name
	 * @param step			execution step within the module
	 * @param ext			extension object
	 * @param cnt			count of needed identificators
	 * @param regAll		register for all identificators or only for the first
	 * @return				identificator
	 */
	int MainControl::registerExtension(const char *n, int step, ExtensionBase *ext, unsigned cnt, bool regAll)
	{
		int id = _nextExtensionId;
		_nextExtensionId += cnt;

		return registerExtension(n, step, id, ext, (regAll ? cnt : 1), true);
	}

	/**
	 * register extension using a given identificator
     * @param n				module name / NULL or empty: not for a certain module
	 * @param step			execution step within the module
	 * @param id			identificator
	 * @param ext			extension object
	 * @param cnt			if greater 1 then register extension with this count of identificators
	 * @param repl			replace existing extension registration
	 * @return				identificator / != id if registration exists and no replacing
	 */
	int MainControl::registerExtension(const char *n, int step, int id, ExtensionBase *ext, unsigned cnt, bool repl)
	{
        const char *name;
        if (n && *n) {
            name = _modConf->testModuleName(n);
            PROTO_OUTL("register extension:  " << ext->extName() << ": " << name << " (" << (unsigned long)name << ") " << step << " " << id << " (" << cnt << ')');
        }
        else {
            name = NULL;
            PROTO_OUTL("register extension:  " << ext->extName() << ": <any module> " << step << " " << id << " (" << cnt << ')');
        }

        map<int, map<int, ExtensionBase *>>& extm = (name ? _regExtensionsB[name] : _regExtensionsA);
		map<int, ExtensionBase *>& exts = extm[step];

		int idr = id;
		for (unsigned i = 0; i < cnt; i++, idr++) {
			if (exts.count(idr) > 0) {
				if (!repl)
					return (id >= 0 ? -1 : 0);

				ExtensionBase *e = exts[idr];
				if (countExtension(e) == 1)
					delete e;
			}

			exts[idr] = ext;
		}

		return id;
	}
	

	/**
	 * insert used strings and locations for extensions in the data object
	 */
	void MainControl::insertExtensionStringsAndLocs(MainData *data)
	{
		set<ExtensionBase *> extSet;

        for (map<int, map<int, ExtensionBase *>>::iterator it2 = _regExtensionsA.begin(); it2 != _regExtensionsA.end(); it2++) {
            map<int, ExtensionBase *>& m3 = it2->second;
            for (map<int, ExtensionBase *>::iterator it3 = m3.begin(); it3 != m3.end(); it3++) {
                ExtensionBase *eb = it3->second;

                if (extSet.count(eb) == 0) {
                    eb->insertStringsAndLocs(data);
                    extSet.insert(eb);
                }
            }
        }

        for (map<const char *, map<int, map<int, ExtensionBase *>>>::iterator it = _regExtensionsB.begin(); it != _regExtensionsB.end(); it++) {
			data->globStrings()->store(it->first);

			map<int, map<int, ExtensionBase *>>& m2 = it->second;
			for (map<int, map<int, ExtensionBase *>>::iterator it2 = m2.begin(); it2 != m2.end(); it2++) {
				map<int, ExtensionBase *>& m3 = it2->second;
				for (map<int, ExtensionBase *>::iterator it3 = m3.begin(); it3 != m3.end(); it3++) {
					ExtensionBase *eb = it3->second;

					if (extSet.count(eb) == 0) {
						eb->insertStringsAndLocs(data);
						extSet.insert(eb);
					}
				}
			}
		}
	}

	/**
	 * get count of elements for serialization of the extensions
	 */
	int MainControl::subCntExtensions()
	{
		int cnt = 0;
		set<ExtensionBase *> extSet;

        for (map<int, map<int, ExtensionBase *>>::iterator it2 = _regExtensionsA.begin(); it2 != _regExtensionsA.end(); it2++) {
            map<int, ExtensionBase *>& m3 = it2->second;
            for (map<int, ExtensionBase *>::iterator it3 = m3.begin(); it3 != m3.end(); it3++) {
                ExtensionBase *eb = it3->second;

                // one element for every different extension
                if (extSet.count(eb) == 0) {
                    cnt++;
                    extSet.insert(eb);
                }

                // and one element per extension registration
                cnt++;
            }
        }

        for (map<const char *, map<int, map<int, ExtensionBase *>>>::iterator it = _regExtensionsB.begin(); it != _regExtensionsB.end(); it++) {
			map<int, map<int, ExtensionBase *>>& m2 = it->second;
			for (map<int, map<int, ExtensionBase *>>::iterator it2 = m2.begin(); it2 != m2.end(); it2++) {
				map<int, ExtensionBase *>& m3 = it2->second;
				for (map<int, ExtensionBase *>::iterator it3 = m3.begin(); it3 != m3.end(); it3++) {
					ExtensionBase *eb = it3->second;

					// one element for every different extension
					if (extSet.count(eb) == 0) {
						cnt++;
						extSet.insert(eb);
					}

					// and one element per extension registration
					cnt++;
				}
			}
		}

		return cnt;
	}

	/**
	 * write data for serialization of the extensions to output stream
	 * @param si		serialization info
	 * @param data      main data object
	 */
	void MainControl::serializeExtensionsTo(MainData::SerializeInfo& si, const MainData *data)
	{
		map<ExtensionBase *, int> extMap;
		StringStore *s = data->globStrings();
		int e = 0, r = 0;

		// extension objects
        for (map<int, map<int, ExtensionBase *>>::iterator it2 = _regExtensionsA.begin(); it2 != _regExtensionsA.end(); it2++) {
            map<int, ExtensionBase *>& m3 = it2->second;
            for (map<int, ExtensionBase *>::iterator it3 = m3.begin(); it3 != m3.end(); it3++) {
                ExtensionBase *eb = it3->second;

                if (extMap.count(eb) == 0) {
                    (*si.ostr) << "*Extension" << ++e << ':' << eb->subCnt() << ';' << eb->linesPerData() << ';' << s->search(eb->modName());
                    eb->serializeTo(si, data);
                    extMap[eb] = e;
                }
            }
        }

        for (map<const char *, map<int, map<int, ExtensionBase *>>>::iterator it = _regExtensionsB.begin(); it != _regExtensionsB.end(); it++) {
			map<int, map<int, ExtensionBase *>>& m2 = it->second;
			for (map<int, map<int, ExtensionBase *>>::iterator it2 = m2.begin(); it2 != m2.end(); it2++) {
				map<int, ExtensionBase *>& m3 = it2->second;
				for (map<int, ExtensionBase *>::iterator it3 = m3.begin(); it3 != m3.end(); it3++) {
					ExtensionBase *eb = it3->second;

					if (extMap.count(eb) == 0) {
						(*si.ostr) << "*Extension" << ++e << ':' << eb->subCnt() << ';' << eb->linesPerData() << ';' << s->search(eb->modName());
						eb->serializeTo(si, data); 
						extMap[eb] = e;
					}
				}
			}
		}

		// extension registration
        for (map<int, map<int, ExtensionBase *>>::iterator it2 = _regExtensionsA.begin(); it2 != _regExtensionsA.end(); it2++) {
            map<int, ExtensionBase *>& m3 = it2->second;
            for (map<int, ExtensionBase *>::iterator it3 = m3.begin(); it3 != m3.end(); it3++) {
                ExtensionBase *eb = it3->second;

                (*si.ostr) << "*RegExt" << ++r << ":0;1;" << extMap[eb] << ";-1;" << it2->first << ';' << it3->first << endl;
            }
        }

        for (map<const char *, map<int, map<int, ExtensionBase *>>>::iterator it = _regExtensionsB.begin(); it != _regExtensionsB.end(); it++) {
			map<int, map<int, ExtensionBase *>>& m2 = it->second;
			for (map<int, map<int, ExtensionBase *>>::iterator it2 = m2.begin(); it2 != m2.end(); it2++) {
				map<int, ExtensionBase *>& m3 = it2->second;
				for (map<int, ExtensionBase *>::iterator it3 = m3.begin(); it3 != m3.end(); it3++) {
					ExtensionBase *eb = it3->second;

					(*si.ostr) << "*RegExt" << ++r << ":0;1;" << extMap[eb] << ';' << s->search(it->first) << ';' << it2->first << ';' << it3->first << endl;
				}
			}
		}
	}

	/**
	 * read extensions serialization stream
	 * @param si		serialization info
	 * @param data      main data object
	 * @param cnt		count of elements within the section
	 */
	void MainControl::deserializeExtensionsFrom(MainData::SerializeInfo& si, const MainData *data, int cnt)
	{
		int outLevel, subCnt, lc;
		string sect, rline;
        int no, md, st, id;
		const char *mn;
		map<int, ExtensionBase *> extMap;

		for (int ss = 0; ss < cnt; ss++) {
			sect = data->readSectionStart(si, 1, outLevel, subCnt, lc, rline);

			if (sect.compare(0, 9, "Extension") == 0) {
				no = stoi(sect.substr(9));
				mn = _modConf->testModuleName(data->globStrings()->at(MainData::getNextLong(rline, si.pos)));

				// construct extension module object
				// the name of the extension module must not be aliased!
				ExtensionModule *modObj = (ExtensionModule *)(_modConf->constructModule(mn, mn));

				// construct and fill extension object
				ExtensionBase *eb = modObj->createFromSerialization(si, data, subCnt, lc, rline);
				extMap[no] = eb;
			}
			else if (sect.compare(0, 6, "RegExt") == 0) {
				no = MainData::getNextLong(rline, si.pos);
                md = MainData::getNextLong(rline, si.pos);
                mn = (md >= 0 ? data->globStrings()->at(md) : NULL);
				st = MainData::getNextLong(rline, si.pos);
				id = MainData::getNextLong(rline, si.pos);

				// register extension
				registerExtension(mn, st, id, extMap[no], true);

				si.pos->addLines();
			}
			else {
				si.pos->addLines();
				data->skipSection(si, outLevel, subCnt, lc);
			}
		}
	}


	/**
	 * add another module for execution, if it not contained already
	 * @param modName       module name
	 * @param order         order number for execution
	 * @param opts          additional command line options, should be restricted by name to the module / NULL: no additional options
	 * @param alias         module alias / NULL: no alias
	 * @param modNames      vector with module names for use of command line options / NULL: use only module name
	 * @return              module new inserted
	 */
	bool MainControl::addModule(const char *modName, int order, CmdLineOptList *opts, const char *alias, vector<const char *> *modNames)
	{
		const char *name = _modConf->testModuleName(modName);

		if (opts) {
			return addModule(name, alias, order, *opts, modNames);
		}
		else {
			CmdLineOptList o;
			return addModule(name, alias, order, o, modNames);
		}
	}


	/**
	 * delete all modules still not executed within a range of order numbers
	 * @param orderFrom     start order number for range
	 * @param orderTo       end order number for range
	 * @return              count of deleted modules
	 */
	int MainControl::delModules(int orderFrom, int orderTo)
	{
		int ret = 0;

		// get running modules
		set<Module*> runMods;
		for (map<ModuleBase*, Module*>::iterator it = _runningMod.begin(); it != _runningMod.end(); it++)
			runMods.insert(it->second);

		// find modules to delete
		for (map<int, vector<Module *>>::iterator it = _modules.begin(); it != _modules.end(); it++) {
			if (it->first >= orderFrom && it->first <= orderTo) {
				vector<Module *>& mods = it->second;

				// delete as long module is still not executed and not running
				size_t s = mods.size();
				while (s) {
					Module *m = mods[s-1];
					if (m->executed || runMods.count(m))
						break;

					mods.pop_back();
					delete m;
					s--;
					ret++;
				}
			}
		}

		return ret;
	}


	/**
	 * insert used strings for modules in the data object
	 */
	void MainControl::insertModuleStrings(MainData *data)
	{
		StringStore *s = data->globStrings();

		for (map<int, vector<Module *>>::iterator it = _modules.begin(); it != _modules.end(); it++) {
			vector<Module *>& mods = it->second;
			for (vector<Module *>::iterator itm = mods.begin(); itm != mods.end(); itm++) {
				Module *m = *itm;
				
				s->store(m->name);

				if (m->alias)
					s->store(m->alias);

				vector<const char *>& mm = m->modNames;
				for (vector<const char *>::iterator itn = mm.begin(); itn != mm.end(); itn++)
					s->store(*itn);
			}
		}

		for (map<const char *, map<const char *, int>>::iterator it = _optUsed.begin(); it != _optUsed.end(); it++) {
			s->store(it->first);
		}
	}

	/**
	 * write data for serialization of the modules to output stream
	 * @param si		serialization info
	 * @param ostr      output stream
	 * @param data      main data object
	 * @param ln		output line numbers
	 */
	void MainControl::serializeModulesTo(MainData::SerializeInfo& si, const MainData *data)
	{
		int cnt, i;
		StringStore *s = data->globStrings();

		// module names
		cnt = 0;
		for (map<int, vector<Module *>>::iterator it = _modules.begin(); it != _modules.end(); it++)
			cnt += it->second.size();

		(*si.ostr) << "*ModuleNames:" << cnt << ";1" << endl;

		i = 0;
		for (map<int, vector<Module *>>::iterator it = _modules.begin(); it != _modules.end(); it++) {
			vector<Module *>& mods = it->second;
			for (vector<Module *>::iterator itm = mods.begin(); itm != mods.end(); itm++) {
				Module *m = *itm;

				if (si.lineNums)
					(*si.ostr) << i++ << '#';

				// also the running module counts as executed here
				bool exec = m->executed;
				if (!exec) {
					for (map<ModuleBase*, Module*>::iterator itrm = _runningMod.begin(); itrm != _runningMod.end() && !exec; itrm++) {
						if (m == itrm->second)
							exec = true;
					}
				}

				(*si.ostr) << it->first << ';' << s->search(m->name) << ';' << (m->alias ? (int)(s->search(m->alias)) : -1) << ';' << (exec ? 1 : 0) << ';' << m->modNames.size();

				vector<const char *>& mm = m->modNames;
				for (vector<const char *>::iterator itn = mm.begin(); itn != mm.end(); itn++)
					(*si.ostr) << ';' << s->search(*itn);

				(*si.ostr) << endl;
			}
		}

		// used options
		cnt = 0;
		for (map<const char *, map<const char *, int>>::iterator it = _optUsed.begin(); it != _optUsed.end(); it++)
			cnt += it->second.size();

		(*si.ostr) << "*UsedOptions:" << cnt << ";1" << endl;

		i = 0;
		for (map<const char *, map<const char *, int>>::iterator it = _optUsed.begin(); it != _optUsed.end(); it++) {
			unsigned o = s->search(it->first);

			map<const char *, int>& ou = it->second;
			for (map<const char *, int>::iterator ito = ou.begin(); ito != ou.end(); ito++) {
				if (si.lineNums)
					(*si.ostr) << i++ << '#';

				(*si.ostr) << o << ';' << s->search(ito->first) << ';' << ito->second << endl;
			}
		}
	}

	/**
	 * read modules from serialization stream
	 * @param si		serialization info
	 * @param data      main data object
	 * @param cnt		count of elements within the section
	 */
	void MainControl::deserializeModulesFrom(MainData::SerializeInfo& si, const MainData *data, int cnt)
	{
		int outLevel, subCnt, lc;
		string sect, rline, line;
		bool mn, uo;
		int w1, w2, w3;
		bool chgmod = false;

		for (int ss = 0; ss < cnt; ss++) {
			sect = data->readSectionStart(si, 1, outLevel, subCnt, lc, rline);
			si.pos->addLines();
			mn = uo = false;

			if (sect == "ModuleNames")
				mn = true;
			else if (sect == "UsedOptions")
				uo = true;
			else
				data->skipSection(si, outLevel, subCnt, lc);

			if (mn || uo) {
				for (int i = 0; i < subCnt; i++) {
					getline(*si.istr, line);
					if (!si.istr->good())
						throw MainDataInputException("unexpected end of file or read error", *si.pos);

					if (si.lineNums)
						MainData::getNextLong(line, si.pos);

					w1 = MainData::getNextLong(line, si.pos);
					w2 = MainData::getNextLong(line, si.pos);
					w3 = MainData::getNextLong(line, si.pos);

					if (mn) {
						// module names
						const char *mod = _modConf->_modNames.store(data->globStrings()->at(w2));
						const char *alm = (w3 >= 0 ? _modConf->_modNames.store(data->globStrings()->at(w3)) : NULL);
						bool exMod = (MainData::getNextLong(line, si.pos) != 0);

						const char *m = alm ?: mod;
						Module *modp = NULL;

						// search for module
						for (map<int, vector<Module *>>::iterator it = _modules.begin(); it != _modules.end() && !modp; it++) {
							vector<Module *>& mods = it->second;
							for (vector<Module *>::iterator itm = mods.begin(); itm != mods.end(); itm++) {
								Module *mp = *itm;
								if (mp->name == mod && mp->alias == alm) {
									modp = mp;
									break;
								}
							}
						}

						// insert module
						if (!modp) {
							int cntNames = MainData::getNextLong(line, si.pos);

							if (cntNames > 0) {
								vector<const char *> modNames;
								modNames.reserve(cntNames);
								for (int j = 0; j < cntNames; j++)
									modNames.push_back(_modConf->_modNames.store(data->globStrings()->at(MainData::getNextLong(line, si.pos))));

								modp = new Module(mod, alm, w1, &modNames);
							}
							else {
								modp = new Module(mod, alm, w1, NULL);
							}

							_modNameSet.insert(m);
							_modules[w1].push_back(modp);
						}
						else if (modp->executed || !exMod) {
							modp = NULL;
						}

						if (modp) {
							modp->executed = exMod;
							chgmod = true;
						}
					}
					else {
						// used options
						const char *opt = _cmdLineOptNames.store(data->globStrings()->at(w1));
						const char *mod = _modConf->_modNames.store(data->globStrings()->at(w2));

						int prio = getOptionPrio(mod, opt);
						if (prio == -1 || prio < w3)
							setOptionPrio(mod, opt, w3);
					}

					si.pos->addLines();
				}
			}
		}

		if (chgmod) {
			// reset module iteration
			_itO = _modules.begin();
			_indModule = 0;
		}
	}


#if PROTO
	/**
	 * gets the output stream for a protocol output
	 * @param temp			temporary output file object
	 * @return				output stream
	 */
	ostream *MainControl::initProto(FileOutput& temp)
	{
		int i = _protoFileNames.storeInd(temp.fileName());

		FileOutput *f;
		if (_protoFiles.count(i) == 0) {
			f = new FileOutput(temp);
			_protoFiles[i] = f;
		}
		else {
			f = _protoFiles[i];
		}

		return f->open();
	}
#endif //PROTO


	/**
	 * prints with vsnprintf to the global print buffer
	 */
	const char *MainControl::printBuffer(const char *format, ...)
	{
		va_list args;
		va_start (args, format);

        //TODO: _printBuffer braucht Schutz bei Multithreading

		while (true) {
			int n = vsnprintf(_printBuffer, _printBufferCap, format, args);
			if (n < 0)
				errHandler().internalError("internal formatted print error");

			if (n < _printBufferCap)
				break;

			// if buffer is to small then get a new buffer large enough
			delete _printBuffer;

			_printBufferCap = n + 1;
			_printBuffer = new char[_printBufferCap];
		}

		va_end (args);
		return _printBuffer;
	}


	/**
	 * gets the priority of an used option used for a module
	 * @param n             module name
	 * @param o             option name
	 * @return              used with this priority / -1: not used yet
	 */
	int MainControl::getOptionPrio(const char *n, const char *o)
	{
		const char *os = _cmdLineOptNames.store(o);
		if (_optUsed.count(os) == 0)
			return -1;

		map<const char *, int> &mm = _optUsed[os];
		if (mm.count(n) == 0)
			return -1;

		PROTO_OUTL(" getOptionPrio " << o << ": " << mm[n]);

		return mm[n];
	}

	/**
	 * sets an option as used by a module with a priority
	 * @param n             module name
	 * @param o             option name
	 * @param prio          used with this priority
	 */
	void MainControl::setOptionPrio(const char *n, const char *o, int prio)
	{
		const char *os = _cmdLineOptNames.store(o);

		map<const char *, int> &mm = _optUsed[os];
		mm[n] = prio;

		PROTO_OUTL(" setOptionPrio " << o << ": " << prio);
	}


	/**
	 * add a module to <code>_modules</code>, if it not contained already
	 * @param n             module name
	 * @param a             module alias / NULL: no alias
	 * @param o             order number
	 * @param opts			command line options from module configuration
	 * @param modNames		vector with rekursive module/module aggregation names / NULL: use only module name
	 * @return              module new inserted
	 */
	bool MainControl::addModule(const char *n, const char *a, int o, CmdLineOptList& opts, vector<const char *> *modNames)
	{
		const char *m = a ?: n;
		if (_modNameSet.count(m) > 0)
			return false;

		// add command line options
		_modConfOpts.append(opts);

		// add module
		_modNameSet.insert(m);
		_modules[o].push_back(new Module(n, a, o, modNames));

		// reset module iteration
		_itO = _modules.begin();
		_indModule=0;

		return true;
	}

	/**
	 * add a module aggregation, if it not contained already
	 * @param n             module aggregation name
	 * @param opts			command line options from module configuration
	 * @param modNames		vector with rekursive module/module aggregation names / NULL: use new vector
	 * @return              module aggregation new inserted
	 */
	bool MainControl::addModuleAggr(const char *n, CmdLineOptList& opts, vector<const char *> *modNames)
	{
		if (_modAggrSet.count(n) > 0)
			return false;

		// add command line options
		_modConfOpts.append(opts);

		// add module aggregation
		_modAggrSet.insert(n);

		// add to modNames
		bool newModNames = false;
		if (!modNames) {
			modNames = new vector<const char *>();
			newModNames = true;
		}
		modNames->push_back(n);

		// insert modules of aggregation
		_modConf->insertRekModules(n, modNames);

		if (newModNames)
			delete modNames;
		else
			modNames->pop_back();

		return true;
	}


	/**
	 * output a warning about an unused command line option
	 * @param opt           unused command line option
	 */
	void MainControl::outWarnUnusedCmdLineOpt(CmdLineOptList::SingleOption *opt)
	{
		if (_data->errorLevelMax() <= ERROR_LVL_WARN)
			errHandler().error(ERROR_LVL_WARN, _ctrl->printBuffer("command line option '%s' not used by any module", opt->opt().c_str()), opt->loc(true));
	}

}


