
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


#include "LinearSosMod.hh"
#include "LinearSos.hh"
#include "../../Control/MainControl.hh"


namespace cmpl
{
    /*********** module definition **********/

    // defines the module "linearSos" that is implemented in class "LinearSosMod".
    // to register this module you have to insert "MODULE_CLASS_REG(1, linearSos)" to file "modules.reg"
    MODULE_CLASS( linearSos, LinearSosMod )



    /**
     * run module function
     */
    void LinearSosMod::run()
    {
        PROTO_OUTL("Start run module " << moduleName());

        LinearSos *ext = (LinearSos *) create(this);
        unsigned cnt = ext->countFunc() + _namespace.size();

        // register once for module "compile"
        int id = _ctrl->registerExtension((_registerMods.size() > 0 ? _registerMods[0].c_str() : "compile"), EXT_STEP_COMPILE_PREDEF_SYMBOLS, ext, cnt, false);

        // register for every id for module "interpret"
        _ctrl->registerExtension((_registerMods.size() > 1 ? _registerMods[1].c_str() : "interpret"), EXT_STEP_INTERPRET_PREDEF_SYMBOLS_INIT, id, ext, cnt);

        // register for final initialization in module "interpret"
        _ctrl->registerExtension((_registerMods.size() > 2 ? _registerMods[2].c_str() : "interpret"), EXT_STEP_INTERPRET_INIT_FINAL, id, ext);

        // register setting of model extension flags in module "interpret"
        _ctrl->registerExtension((_registerMods.size() > 3 ? _registerMods[3].c_str() : "interpret"), EXT_STEP_INTERPRET_MODELPROP, id, ext);

        // register for remodelation in module "interpret"
        _ctrl->registerExtension((_registerMods.size() > 4 ? _registerMods[4].c_str() : "interpret"), EXT_STEP_INTERPRET_REMODEL, id, ext);

        // register for special data for model output
        _ctrl->registerExtension((_registerMods.size() > 5 ? _registerMods[5].c_str() : NULL), EXT_STEP_OUTMODEL_SPECIAL_INFO, id, ext);

        regExtOptions(ext, id);

        PROTO_OUTL("End run module " << moduleName());
    }


    /*********** handling of command line options **********/

#define OPTION_EXT_FUNCTION_NAMESPACE       70
#define OPTION_EXT_SOSTYPEONLY              80



    /**
     * initialize modul, to call immediately after the constructor.
     * @param ctrl			main object
     * @param data			data object
     * @param name			module name (or alias)
     */
    void LinearSosMod::init(MainControl *ctrl, MainData *data, const char *name)
    {
        RemodelBaseMod::init(ctrl, data, name);

        _namespace.push_back(data->globStrings()->storeInd("sos"));
        _useForSos = sosTypeBoth;

        _warnInvalid = false;

        _attachNameVarSos = data->globStrings()->storeInd("b");
        _attachNameVarSosNeg = data->globStrings()->storeInd("bu");
        _attachNameConSos = data->globStrings()->storeInd("sos");

        _attachNameVarSos2 = data->globStrings()->storeInd("d");
        _attachNameConSos2 = data->globStrings()->storeInd("sos2");
    }

    /**
     * register command line options options for delivery to this module
     * @param modOptReg		vector to register in
     */
    void LinearSosMod::regModOptions(vector<CmdLineOptList::RegOption> &modOptReg)
    {
        // initial command line options
        RemodelBaseMod::regModOptions(modOptReg);

        REG_CMDL_OPTION( OPTION_EXT_FUNCTION_NAMESPACE, "namespace", 0, 1, CMDL_OPTION_NEG_NO_ARG, true );
        REG_CMDL_OPTION( OPTION_EXT_SOSTYPEONLY, "sos-type-only", 1, 1, CMDL_OPTION_NEG_NO_ARG, true );
    }

    /**
     * register command line options for the extension
     * @param ext           extension object
     * @param id			extension identificator
     */
    void LinearSosMod::regExtOptions(ExtensionBase *ext, int id)
    {
        // command line options delivered to the extension by run()
        RemodelBaseMod::regExtOptions(ext, id);

        const char *m = modNameRemodel();
        REG_CMDL_OPTION_EXT( OPTION_EXT_SOSNATIVE, "native-sos", 0, 0, CMDL_OPTION_NEG_DELIV, true, id, m, EXT_CMDLOPT_INTERPRET_SIMPLE, ext );
        REG_CMDL_OPTION_EXT( OPTION_EXT_SOS2NATIVE, "native-sos2", 0, 0, CMDL_OPTION_NEG_DELIV, true, id, m, EXT_CMDLOPT_INTERPRET_SIMPLE, ext );

        REG_CMDL_OPTION_EXT( OPTION_EXT_WARNINVALID, "warn-invalid-sos", 0, 0, CMDL_OPTION_NEG_DELIV, true, id, m, EXT_CMDLOPT_INTERPRET_SIMPLE, ext );

        REG_CMDL_OPTION_EXT( OPTION_EXT_ATTACHNAMEVARSOS, "an-var-sos", 1, 1, CMDL_OPTION_NEG_NO_ARG, true, id, m, EXT_CMDLOPT_INTERPRET_SIMPLE, ext );
        REG_CMDL_OPTION_EXT( OPTION_EXT_ATTACHNAMEVARSOSNEG, "an-var-sosneg", 1, 1, CMDL_OPTION_NEG_NO_ARG, true, id, m, EXT_CMDLOPT_INTERPRET_SIMPLE, ext );
        REG_CMDL_OPTION_EXT( OPTION_EXT_ATTACHNAMECONSOS, "an-con-sos", 1, 1, CMDL_OPTION_NEG_NO_ARG, true, id, m, EXT_CMDLOPT_INTERPRET_SIMPLE, ext );

        REG_CMDL_OPTION_EXT( OPTION_EXT_ATTACHNAMEVARSOS2, "an-var-sos2", 1, 1, CMDL_OPTION_NEG_NO_ARG, true, id, m, EXT_CMDLOPT_INTERPRET_SIMPLE, ext );
        REG_CMDL_OPTION_EXT( OPTION_EXT_ATTACHNAMECONSOS2, "an-con-sos2", 1, 1, CMDL_OPTION_NEG_NO_ARG, true, id, m, EXT_CMDLOPT_INTERPRET_SIMPLE, ext );
    }

    /**
     * parse single option from command line options, this function is called for every delivered option
     * @param ref			reference number of option registration, should be used for discriminate the options
     * @param prio			priority value of option
     * @param opt			option
     * @return				true if option is used by the module
     */
    bool LinearSosMod::parseOption(int ref, int prio, CmdLineOptList::SingleOption *opt)
    {
        if (RemodelBaseMod::parseOption(ref, prio, opt))
            return true;

        parseOptString(opt);
        switch (ref) {
            case OPTION_EXT_FUNCTION_NAMESPACE:
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
                        _namespace.push_back(data()->globStrings()->storeInd("sos"));
                    }
                }
                return true;

            case OPTION_EXT_SOSTYPEONLY:
                if (opt->neg())
                    _useForSos = sosTypeBoth;
                else if ((*opt)[0] == "1")
                    _useForSos = sosTypeOnlySos1;
                else if ((*opt)[0] == "2")
                    _useForSos = sosTypeOnlySos2;
                else
                    ERRHANDLER.error(ERROR_LVL_NORMAL, "argument for option 'sos-type-only' must be 1 or 2", opt->argPos(0));
                return true;
        }

        return false;
    }

    /**
     * writes usage info for the module to stream
     * @param s				stream to write to
     */
    void LinearSosMod::usage(ostream& s)
    {
        RemodelBaseMod::usage(s);

        s << "  -sos-type-only <1|2>          use extension only for this SOS type (default is both SOS and SOS2)" << endl;
        s << "  -namespace [<namespace>]      define function symbols within this namespace (defaults to 'sos')" << endl;
        s << "  -native-sos                   don't linearize SOS and SOS2, but let it for native handling by the solver" << endl;
        s << "  -native-sos2                  like '-native-sos' if setting for native use or linearization is different for SOS2" << endl;
        s << "  -warn-invalid-sos             show warning if SOS is invalid because of contained non zero parameters" << endl;
        s << "  -an-var-sos <string>          string attached to name for variable for linearization of of SOS (positive part) (default is '" << data()->globStrings()->at(_attachNameVarSos) << "')" << endl;
        s << "  -an-var-sosneg <string>       string attached to name for variable for linearization of of SOS (negative part) (default is '" << data()->globStrings()->at(_attachNameVarSosNeg) << "')" << endl;
        s << "  -an-con-sos <string>          string attached to name of constraint for linearization of of SOS (max non zero count restriction) (default is '" << data()->globStrings()->at(_attachNameConSos) << "')" << endl;
        s << "  -an-var-sos2 <string>         string attached to name for variable for linearization of order for SOS2 (default is '" << data()->globStrings()->at(_attachNameVarSos2) << "')" << endl;
        s << "  -an-con-sos2 <string>         string attached to name of constraint for linearization of of SOS2 (order of non zero elements restriction) (default is '" << data()->globStrings()->at(_attachNameConSos2) << "')" << endl;
    }


    /**
     * create derived class object for execution of the remodeling
     */
    RemodelBase *LinearSosMod::create(RemodelBaseMod *mod)
    {
        return new LinearSos((LinearSosMod *)mod);
    }
}


