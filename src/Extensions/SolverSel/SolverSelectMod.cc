
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


#include "SolverSelectMod.hh"
#include "SolverSelect.hh"
#include "../../Control/MainControl.hh"


namespace cmpl
{
	/*********** module definition **********/

	// defines the module "solverSelect" that is implemented in class "SolverSelectMod".
	// to register this module you have to insert "MODULE_CLASS_REG(1, solverSelect)" to file "modules.reg"
	MODULE_CLASS( solverSelect, SolverSelectMod )


    /**
     * run module function
     */
    void SolverSelectMod::run()
    {
        PROTO_OUTL("Start run solverSelect module " << moduleName());

        SolverSelect *ext = new SolverSelect(this);
        int id = _ctrl->registerExtension((_registerMods.size() > 0 ? _registerMods[0].c_str() : "interpret"), EXT_STEP_INTERPRET_SOLVERSEL, ext);

        regExtOptions(ext, id);

        PROTO_OUTL("End run solverSelect module " << moduleName());
    }


    /*********** handling of command line options **********/

    /**
     * register command line options for the extension
     * @param ext           extension object
     * @param id			extension identificator
     */
    void SolverSelectMod::regExtOptions(ExtensionBase *ext, int id)
    {
        const char *m = (_registerMods.size() > 0 ? _registerMods[0].c_str() : "interpret");
        REG_CMDL_OPTION_EXT( OPTION_EXT_EXEC_SOLVERSEL_CONFIG_FILE, "solversel-config", 1, 1, CMDL_OPTION_NEG_NO_ARG, true, id, m, EXT_CMDLOPT_INTERPRET_SIMPLE, ext );
        REG_CMDL_OPTION_EXT( OPTION_EXT_EXEC_SOLVERSEL_SOLVERNAME, "solver", 1, 1, CMDL_OPTION_NEG_NO_ARG, true, id, m, EXT_CMDLOPT_INTERPRET_SIMPLE, ext );
    }

    /**
     * writes usage info for the module to stream
     * @param s				stream to write to
     */
    void SolverSelectMod::usage(ostream& s)
    {
        ExtensionModule::usage(s);

        s << "  -solversel-config <file>      use this config file for solver selection instead of default '" << defaultConfigFile() << "'" << endl;
        s << "  -solver <name>                name of explicit selected solver" << endl;
    }


	/**
	 * create the extension object from serialization
	 * @param si        serialization info
	 * @param data      main data object
	 * @param subCnt    count of sub data objects
	 * @param lc		line count per element or 0 if elements are len coded
	 * @param rline		remaining section start line
	 */
	ExtensionBase *SolverSelectMod::createFromSerialization(MainData::SerializeInfo& si, const MainData *data, int subCnt, int lc, string &rline)
	{
		SolverSelect *ext = new SolverSelect(this);
		ext->deserializeFrom(si, data, subCnt, lc, rline);
		return ext;
	}
}


