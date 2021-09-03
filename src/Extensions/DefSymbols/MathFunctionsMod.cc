
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


#include "MathFunctionsMod.hh"
#include "MathFunctions.hh"
#include "../../Control/MainControl.hh"


namespace cmpl
{
	/*********** module definition **********/

	// defines the module "mathFunctions" that is implemented in class "MathFunctionsMod".
	// to register this module you have to insert "MODULE_CLASS_REG(1, mathFunctions)" to file "modules.reg"
	MODULE_CLASS( mathFunctions, MathFunctionsMod )




	/**
	 * run module function
	 */
	void MathFunctionsMod::run()
	{
		PROTO_OUTL("Start run mathFunctions module " << moduleName());

        MathFunctions *ext = new MathFunctions(this, _namespace);
        unsigned cnt = ext->countFunc() + _namespace.size();

		// register once for module "compile", but reserving cnt ids
		int id = _ctrl->registerExtension((_registerMods.size() > 0 ? _registerMods[0].c_str() : "compile"), EXT_STEP_COMPILE_PREDEF_SYMBOLS, ext, cnt, false);

		// register for every id for module "interpret"
		_ctrl->registerExtension((_registerMods.size() > 1 ? _registerMods[1].c_str() : "interpret"), EXT_STEP_INTERPRET_PREDEF_SYMBOLS_INIT, id, ext, cnt);

		PROTO_OUTL("End run mathFunctions module " << moduleName());
	}


    /*********** handling of command line options **********/

    #define OPTION_FUNCTION_NAMESPACE       20

    /**
     * register command line options options for delivery to this module
     * @param modOptReg		vector to register in
     */
    void MathFunctionsMod::regModOptions(vector<CmdLineOptList::RegOption> &modOptReg)
    {
        ExtensionModule::regModOptions(modOptReg);

        REG_CMDL_OPTION( OPTION_FUNCTION_NAMESPACE, "namespace", 0, 1, CMDL_OPTION_NEG_NO_ARG, true );
    }

    /**
     * parse single option from command line options, this function is called for every delivered option
     * @param ref			reference number of option registration, should be used for discriminate the options
     * @param prio			priority value of option
     * @param opt			option
     * @return				true if option is used by the module
     */
    bool MathFunctionsMod::parseOption(int ref, int prio, CmdLineOptList::SingleOption *opt)
    {
        if (ExtensionModule::parseOption(ref, prio, opt))
            return true;

        switch (ref) {
            case OPTION_FUNCTION_NAMESPACE:
                _namespace.clear();
                if (!opt->neg()) {
                    if (opt->size() > 0) {
                        string& ns = (*opt)[0];
                        string w;
                        size_t p = 0;
                        while (StringStore::iterWords(ns, w, p, ".")) {
                            _namespace.push_back(data()->globStrings()->storeInd(w));
                        }
                    }
                    else {
                        _namespace.push_back(data()->globStrings()->storeInd("math"));
                    }
                }
                return true;
        }

        return false;
    }

    /**
     * writes usage info for the module to stream
     * @param s				stream to write to
     */
    void MathFunctionsMod::usage(ostream& s)
    {
        ExtensionModule::usage(s);

        s << "  -namespace [<namespace>]      define function symbols within this namespace (defaults to 'math') instead without namespace" << endl;
    }


    /**
     * create the extension object from serialization
     * @param si        serialization info
     * @param data      main data object
     * @param subCnt    count of sub data objects
     * @param lc		line count per element or 0 if elements are len coded
     * @param rline		remaining section start line
     */
    ExtensionBase *MathFunctionsMod::createFromSerialization(MainData::SerializeInfo& si, const MainData *data, int subCnt, int lc, string &rline)
    {
        MathFunctions *ext = new MathFunctions(this);
        ext->deserializeFrom(si, data, subCnt, lc, rline);
        return ext;
    }

}


