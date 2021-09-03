
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


#include "ModuleBase.hh"
#include "CmdLineOptList.hh"
#include "MainControl.hh"


namespace cmpl
{
	/**
	 * initialize modul, to call immediately after the constructor.
	 * @param ctrl          main object
	 * @param data			data object
	 * @param name          module name (or alias)
	 */
	void ModuleBase::init(MainControl *ctrl, MainData *data, const char *name)
	{
		_ctrl = ctrl;
		_data = data;

		_modName = name;
#if PROTO
		_protoStream = NULL;
#endif

        _isSilent=false;
        _deactivated = false;
        _startTime = chrono::system_clock::now();

		// register command line options
		regModOptions(_modOptReg);
	}

    /**
     * get time duration since start of the application
     */
    chrono::milliseconds ModuleBase::durStartApp()
    {
        return _ctrl->durStartMod();
    }

    /**
     * parse command line options for the module
     * @param opts			command line options
     * @param step          parse step (one of <code>CMDLINE_OPT_PARSE_*</code>)
     * @param modNames		relevant names for module specification in command line options
     */
    void ModuleBase::parseCmdLineOpts(CmdLineOptList *opts, int step, vector<const char *> &modNames)
    {
		// iteration over options in opts
		int iter = -1;
        int dummy, modspec;
		CmdLineOptList::SingleOption *opt;
        bool hasExtOpt = !(_extOptReg.empty());

        while ((opt = opts->iterOptions(NULL, iter, dummy))) {
            if ((modspec = opt->testModName(modNames)) >= 0) {
                // search in registered options
                for (vector<CmdLineOptList::RegOption>::iterator it = _modOptReg.begin(); it != _modOptReg.end(); it++) {
                    if (opt->opt() == it->opt) {
                        CmdLineOptList::RegOption& reg = *it;
                        checkParseSingleCmdLineOpt(opt, &reg, step, modspec, true);
                        break;
                    }
                }
            }

            if (hasExtOpt) {
                // search in extension options
                if (_extOptReg.count(opt->opt())) {
                    vector<int>& stlst = _extOptReg[opt->opt()];
                    for (int st: stlst) {
                        vector<CmdLineOptList::RegOptionExt> *eorlst = ctrl()->getRegCmdOptExtension(st);
                        if (!eorlst->empty()) {
                            // collect registered extensions for the option and the execution step
                            map<int, int> extRef;
                            const char *regOptName = NULL;

                            for (vector<CmdLineOptList::RegOptionExt>::iterator it = eorlst->begin(); it != eorlst->end(); it++) {
                                if (opt->opt() == it->opt && (modspec = opt->testModName(it->module)) >= 0) {
                                    CmdLineOptList::RegOptionExt& reg = *it;
                                    if (checkParseSingleCmdLineOpt(opt, &reg, step, modspec, false)) {
                                        extRef[reg.extID] = reg.ref;
                                        if (!regOptName)
                                            regOptName = reg.opt;
                                    }
                                }
                            }

                            if (!extRef.empty()) {
                                // call execution for all extensions for the option and the execution step
                                int prio = CMDL_OPTION_MAKE_PRIORITY(opt->prio(), step, modspec);
                                if (execExtOption(st, extRef, prio, opt)) {
                                    // mark option as used and handle priority
                                    opt->markUsed();
                                    if (prio > _ctrl->getOptionPrio(_modName, regOptName))
                                        _ctrl->setOptionPrio(_modName, regOptName, prio);
                                }
                            }
                        }
                    }
                }
            }
		}
	}

    /**
     * check and parse a single command line option
     * @param opt           command line option
     * @param reg           registration of the command line option for the module
     * @param step          step of command line option parsing or origin of command line options (one of <code>CMDLINE_OPT_PARSE*</code>)
     * @param modspec       level of module specification for the option
     * @param parseOpt      call parsing function for the command line option
     * @return              true if command line option successfully checked
     */
    bool ModuleBase::checkParseSingleCmdLineOpt(CmdLineOptList::SingleOption *opt, CmdLineOptList::RegOption *reg, int step, int modspec, bool parseOpt)
    {
        // check arg count
        if (((!opt->neg() || reg->allowNeg == CMDL_OPTION_NEG_DELIV) && ((int)opt->size() < reg->minArgs || (reg->maxArgs >= 0 && (int)opt->size() > reg->maxArgs)))
                || (opt->neg() && reg->allowNeg == CMDL_OPTION_NEG_NO_ARG && opt->size() > 0)) {
            _ctrl->errHandler().error(ERROR_LVL_EASY, _ctrl->printBuffer("wrong argument count %d for command line option '%s'", (int)opt->size(), opt->opt().c_str()), opt->loc(true));
            return false;
        }

        // check negated option
        else if (opt->neg() && (reg->allowNeg == CMDL_OPTION_NEG_IGNORE || reg->allowNeg == CMDL_OPTION_NEG_ERROR)) {
            if (reg->allowNeg == CMDL_OPTION_NEG_ERROR)
                _ctrl->errHandler().error(ERROR_LVL_EASY, _ctrl->printBuffer("negation not allowed for option '%s'", opt->opt().c_str()), opt->loc(false));
            return false;
        }

        else {
            // check priority
            int prio = CMDL_OPTION_MAKE_PRIORITY(opt->prio(), step, modspec);
            int used = _ctrl->getOptionPrio(_modName, reg->opt);

            // don't deliver is priority not higher as previous delivered
            if (reg->handlePrio && used >= prio) {
                // option count as used
                opt->markUsed();
                return false;
            }
            else if (parseOpt && parseOption(reg->ref, prio, opt)) {
                if (prio > used)
                    _ctrl->setOptionPrio(_modName, reg->opt, prio);
                opt->markUsed();
            }
        }

        return true;
    }

    /**
     * register handling of command line options for a given extension execution step
     * @param extStep       extension execution step
     * @return              true if there is at least one concerned extension
     */
    bool ModuleBase::regExtOption(int extStep)
    {
        vector<CmdLineOptList::RegOptionExt> *reo = _ctrl->getRegCmdOptExtension(extStep);
        if (!reo)
            return false;

        for (vector<CmdLineOptList::RegOptionExt>::iterator it = reo->begin(); it != reo->end(); it++) {
            string s(it->opt);
            vector<int> &v = _extOptReg[s];

            for (int i: v) {
                if (i == extStep)
                    continue;
            }

            v.push_back(extStep);
        }

        return true;
    }


	/*********** handling of command line options options **********/
	// module classes must overwrite this methods

// defines for reference numbers of options
#define OPTION_PROTO_FILE	1
#define OPTION_SILENT       2
#define OPTION_DEACTIVATE   3

	/**
	 * register command line options for delivery to this module
	 * @param modOptReg     vector to register in
	 */
	void ModuleBase::regModOptions(vector<CmdLineOptList::RegOption> &modOptReg)
	{
#if PROTO
		REG_CMDL_OPTION( OPTION_PROTO_FILE, "p", 0, 1, CMDL_OPTION_NEG_NO_ARG, true );
#endif
        REG_CMDL_OPTION( OPTION_SILENT ,"silent", 0, 0, CMDL_OPTION_NEG_NO_ARG, false );
        REG_CMDL_OPTION( OPTION_DEACTIVATE ,"deactivate", 0, 0, CMDL_OPTION_NEG_NO_ARG, true );
    }


	/**
	 * parse single option from command line options, this function is called for every delivered option
	 * @param ref           reference number of option registration, should be used for discriminate the options
	 * @param prio          priority value of option
	 * @param opt           option
	 * @return              true if option is used by the module
	 */
	bool ModuleBase::parseOption(int ref, int prio, CmdLineOptList::SingleOption *opt)
	{
		PROTO_OUTL("parseOption module " << moduleName() << ": " << opt->opt() << " (" << ref << "): " << prio);

		switch (ref) {
#if PROTO
			// protocol file
			case OPTION_PROTO_FILE:
				_protoStream = parseProtoOption(opt, "protocol output");
				return true;
#endif

        case OPTION_SILENT:
            _isSilent = !opt->neg();
            return true;

        case OPTION_DEACTIVATE:
            _deactivated = !opt->neg();
            return true;
		}

		return false;
	}

#if PROTO
	/**
	 * parse protocol file or trace file option
	 * @param opt			option
	 * @param err			name for error message
	 * @return				output stream or NULL
	 */
	ostream* ModuleBase::parseProtoOption(CmdLineOptList::SingleOption *opt, const char *err)
	{
		ostream *res = NULL;

		if (! opt->neg()) {
			try {
				FileOutput temp;
				temp.setFile(_data, (opt->size() == 1 ? IO_MODE_FILE : IO_MODE_STD_IO), (opt->size() == 1 ? &((*opt)[0]) : NULL), IO_FILE_STANDARD_PROTO, true);
				res = ctrl()->initProto(temp);
			}
			catch (FileException& e) {
				ctrl()->errHandler().error(ERROR_LVL_EASY, _ctrl->printBuffer("%s: %s file '%s'", e.what(), err, e.filename().c_str()), opt->loc(true));
			}
		}

		return res;
	}
#endif

	/**
	 * writes usage info for the module to stream
	 * @param s				stream to write to
	 */
	void ModuleBase::usage(ostream& s)
	{
        s << "  -silent                       suppresses all outputs" << endl;
        s << "  -deactivate                   deactivate the module" << endl;
#if PROTO
		s << "  -p [<file>]                   output protocol messages for this module to <file> or stdout" << endl;
#endif
    }


	/*********** utility functions for options **********/

	/**
	 * parse command line option with file name aliases
	 */
	void ModuleBase::parseFileAliases(CmdLineOptList::SingleOption *opt, map<string,string>& fileAlias, map<string,unsigned>& fileAliasPrio)
	{
		for (size_t i = 0; i < opt->size(); i++) {
			string& a = (*opt)[i];
			size_t t = a.find('=');
			if (t == string::npos) {
				ctrl()->errHandler().error(ERROR_LVL_NORMAL, ctrl()->printBuffer("argument '%s' for option '%s' has not the format '<from>=[<to>]'", a.c_str(), opt->opt().c_str()), opt->loc(true));
				continue;
			}

			string from = a.substr(0, t);
			string to = a.substr(t+1);

			map<string,unsigned>::iterator it = fileAliasPrio.find(from);
			if (it == fileAliasPrio.end() || opt->prio() > it->second) {
				fileAlias[from] = to;
				fileAliasPrio[from] = opt->prio();
			}
		}
	}


    /**
     * execute step for every registered extension for a single command line option, for options simply to deliver to all concerned extensions
     * @param extStep       extension execution step
     * @param extRef        map from extension identifier to command line option reference key
     * @param prio          priority value of option
     * @param opt           option
     * @return              true if option is used by an extension
     */
    bool ModuleBase::execExtOptionSimple(int extStep, map<int, int>& extRef, int prio, CmdLineOptList::SingleOption *opt)
    {
        bool ret = false;

        for (map<int, int>::iterator it = extRef.begin(); it != extRef.end(); it++) {
            if (ctrl()->runExtension(this, extStep, it->first, it->second, prio, opt))
                ret = true;
        }

        return ret;
    }

}

