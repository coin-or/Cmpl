
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


#include "RemodelBaseMod.hh"
#include "RemodelBase.hh"
#include "../../Control/MainControl.hh"


namespace cmpl
{
    /**
     * run module function
     */
    void RemodelBaseMod::run()
    {
        PROTO_OUTL("Start run module " << moduleName());

        // register for module "interpret"
        RemodelBase *ext = create(this);
        int id = _ctrl->registerExtension((_registerMods.size() > 0 ? _registerMods[0].c_str() : "interpret"), EXT_STEP_INTERPRET_REMODEL, ext);

        regExtOptions(ext, id);

        PROTO_OUTL("End run module " << moduleName());
    }


    /*********** handling of command line options **********/

    /**
     * initialize modul, to call immediately after the constructor.
     * @param ctrl			main object
     * @param data			data object
     * @param name			module name (or alias)
     */
    void RemodelBaseMod::init(MainControl *ctrl, MainData *data, const char *name)
    {
        ExtensionModule::init(ctrl, data, name);

        _maxThreads = 0;
        _bigM = 1e+10;
        _namePref = data->globStrings()->storeInd("__");
        _nameSep = data->globStrings()->storeInd("_");
    }

    /**
     * register command line options for the extension
     * @param ext           extension object
     * @param id			extension identificator
     */
    void RemodelBaseMod::regExtOptions(ExtensionBase *ext, int id)
    {
        const char *m = modNameRemodel();

        REG_CMDL_OPTION_EXT( OPTION_EXT_REMODEL_ENABLE, "remodel", 0, 0, CMDL_OPTION_NEG_NO_ARG, true, id, m, EXT_CMDLOPT_INTERPRET_SIMPLE, ext );
        REG_CMDL_OPTION_EXT( OPTION_EXT_THREADS, "threads", 1, 1, CMDL_OPTION_NEG_NO_ARG, true, id, m, EXT_CMDLOPT_INTERPRET_SIMPLE, ext );

        if (useBigM()) {
            REG_CMDL_OPTION_EXT( OPTION_EXT_REMODEL_BIGM, "big-M", 1, 1, CMDL_OPTION_NEG_ERROR, true, id, m, EXT_CMDLOPT_INTERPRET_SIMPLE, ext );
        }

        if (useNameSep()) {
            REG_CMDL_OPTION_EXT( OPTION_EXT_NAMEPREF, "nameprefix", 1, 1, CMDL_OPTION_NEG_NO_ARG, true, id, m, EXT_CMDLOPT_INTERPRET_SIMPLE, ext );
            REG_CMDL_OPTION_EXT( OPTION_EXT_NAMESEP, "namesep", 1, 1, CMDL_OPTION_NEG_NO_ARG, true, id, m, EXT_CMDLOPT_INTERPRET_SIMPLE, ext );
        }
    }

    /**
     * parse single command line option containing a string
     * @param mod           module
     * @param opt           option
     */
    unsigned RemodelBaseMod::parseOptString(ModuleBase *mod, CmdLineOptList::SingleOption *opt)
    {
        if (opt->neg())
            return 0;
        else
            return mod->data()->globStrings()->storeInd((*opt)[0]);
    }

    /**
     * writes usage info for the module to stream
     * @param s				stream to write to
     */
    void RemodelBaseMod::usage(ostream& s)
    {
        ExtensionModule::usage(s);

        s << "  -remodel                      enable remodeling with this extension (default is true)" << endl;
        s << "  -threads <n>                  use maximal n concurrently running worker threads (0: no threading)" << endl;
        if (useBigM()) {
            s << "  -big-M <number>               huge number used for linearization (default is " << _bigM << ")" << endl;
        }
        if (useNameSep()) {
            s << "  -nameprefix <string>          prefix string for names of generated constraints or variables (default is '" << data()->globStrings()->at(_namePref) << "')" << endl;
            s << "  -namesep <string>             separator string between original constraint or variable name and postfix (default is '" << data()->globStrings()->at(_nameSep) << "')" << endl;
        }
    }


	/**
	 * create the extension object from serialization
	 * @param si        serialization info
	 * @param data      main data object
	 * @param subCnt    count of sub data objects
	 * @param lc		line count per element or 0 if elements are len coded
	 * @param rline		remaining section start line
	 */
	ExtensionBase *RemodelBaseMod::createFromSerialization(MainData::SerializeInfo& si, const MainData *data, int subCnt, int lc, string &rline)
	{
        RemodelBase *ext = create(this);
		ext->deserializeFrom(si, data, subCnt, lc, rline);
        return ext;
	}
}


