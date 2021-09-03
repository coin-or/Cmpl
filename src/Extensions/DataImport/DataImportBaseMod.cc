
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


#include "DataImportBaseMod.hh"
#include "DataImportBase.hh"
#include "../../Control/MainControl.hh"


namespace cmpl
{

    /*********** handling of command line options **********/

#define OPTION_EXT_SOURCE_TYPE		20
#define OPTION_EXT_HANDLE_PRIO		21

#define OPTION_EXT_REG_PRECOMP		25
#define OPTION_EXT_REG_INTERPRET	26


    /**
     * initialize modul, to call immediately after the constructor.
     * @param ctrl			main object
     * @param data			data object
     * @param name			module name (or alias)
     */
    void DataImportBaseMod::init(MainControl *ctrl, MainData *data, const char *name)
    {
        ExtensionModule::init(ctrl, data, name);

        _extSrcType = defaultSrcType();
        _handlePrio = 0;

		_regPrecompiler = true;
		_regInterpreter = true;
    }

    /**
     * register command line options options for delivery to this module
     * @param modOptReg		vector to register in
     */
    void DataImportBaseMod::regModOptions(vector<CmdLineOptList::RegOption> &modOptReg)
    {
        ExtensionModule::regModOptions(modOptReg);

        REG_CMDL_OPTION( OPTION_EXT_SOURCE_TYPE, "ext-source-type", 1, 1, CMDL_OPTION_NEG_ERROR, true );
        REG_CMDL_OPTION( OPTION_EXT_HANDLE_PRIO, "handle-prio", 1, 1, CMDL_OPTION_NEG_ERROR, true );

        REG_CMDL_OPTION( OPTION_EXT_REG_PRECOMP, "reg-precompiler", 0, 0, CMDL_OPTION_NEG_DELIV, true );
        REG_CMDL_OPTION( OPTION_EXT_REG_INTERPRET, "reg-interpreter", 0, 0, CMDL_OPTION_NEG_DELIV, true );
    }

    /**
     * parse single option from command line options, this function is called for every delivered option
     * @param ref			reference number of option registration, should be used for discriminate the options
     * @param prio			priority value of option
     * @param opt			option
     * @return				true if option is used by the module
     */
    bool DataImportBaseMod::parseOption(int ref, int prio, CmdLineOptList::SingleOption *opt)
    {
        if (ExtensionModule::parseOption(ref, prio, opt))
            return true;

        switch (ref) {
            case OPTION_EXT_SOURCE_TYPE:
                _extSrcType = opt->argAsInt(0, _ctrl);
                return true;

            case OPTION_EXT_HANDLE_PRIO:
                _handlePrio = opt->argAsInt(0, _ctrl);
                return true;

			case OPTION_EXT_REG_PRECOMP:
                _regPrecompiler = !opt->neg();
				return true;

			case OPTION_EXT_REG_INTERPRET:
                _regInterpreter = !opt->neg();
				return true;
        }

        return false;
    }

    /**
     * writes usage info for the module to stream
     * @param s				stream to write to
     */
    void DataImportBaseMod::usage(ostream& s)
    {
        ExtensionModule::usage(s);

        s << "  -ext-source-type <no>         used identificator number for type of external data source handled by this extension (default: " << _extSrcType << ")" << endl;
        s << "  -handle-prio <n>              priority of handling with this extension if more than one extension can handle it (default: " << _handlePrio << ")" << endl;

        s << "  -reg-precompiler              register this extension for the precompiler module (default: " << (_regPrecompiler ? "true" : "false") << ")" << endl;
        s << "  -reg-interpreter              register this extension for the interpreter module (default: " << (_regInterpreter ? "true" : "false") << ")" << endl;
    }



    /*********** run module **********/

    /**
	 * run module function
	 */
	void DataImportBaseMod::run()
	{
		PROTO_OUTL("Start run DataImportBaseMod module " << moduleName());

        DataImportBase *ext = createExecObj();

		if (_regPrecompiler) {
            // register for module "precompile"
            int res = _ctrl->registerExtension((_registerMods.size() > 0 ? _registerMods[0].c_str() : "precompile"), EXT_STEP_PRECOMP_REG_DUMMY, _extSrcType, ext, 1, false);

            if (res != _extSrcType) {
                PositionInfo p(POSITION_TYPE_DESCR, "(TODO)");  //TODO: Position des Aufrufparameters ext-source-type bzw. dieses Moduls in den Aufrufparametern
                ERRHANDLER.error(ERROR_LVL_WARN, _ctrl->printBuffer("external data source type number %d is already handled by another extension in the precompiler module", _extSrcType), p);
                _regPrecompiler = false;
            }
		}

		if (_regInterpreter) {
            // register for module "interpret"
            int res = _ctrl->registerExtension((_registerMods.size() > 1 ? _registerMods[1].c_str() : "interpret"), EXT_STEP_INTERPRET_EXTDATA_IMPORT, _extSrcType, ext, 1, false);

            if (res != _extSrcType) {
				PositionInfo p(POSITION_TYPE_DESCR, "(TODO)");  //TODO: Position des Aufrufparameters ext-source-type bzw. dieses Moduls in den Aufrufparametern
                ERRHANDLER.error(ERROR_LVL_WARN, _ctrl->printBuffer("external data source type number %d is already handled by another extension in the interpreter module", _extSrcType), p);
                _regInterpreter = false;
			}
		}

        regExtOptions(ext, _extSrcType);

		PROTO_OUTL("End run DataImportBaseMod module " << moduleName());
	}

	/**
	 * create the extension object from serialization
	 * @param si        serialization info
	 * @param data      main data object
	 * @param subCnt    count of sub data objects
	 * @param lc		line count per element or 0 if elements are len coded
	 * @param rline		remaining section start line
	 */
	ExtensionBase *DataImportBaseMod::createFromSerialization(MainData::SerializeInfo& si, const MainData *data, int subCnt, int lc, string &rline)
	{
        DataImportBase *ext = createExecObj();
		ext->deserializeFrom(si, data, subCnt, lc, rline);
        regExtOptions(ext, _extSrcType);
        return ext;
	}
}


