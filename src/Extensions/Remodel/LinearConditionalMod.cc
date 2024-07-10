
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


#include "LinearConditionalMod.hh"
#include "LinearConditional.hh"
#include "../../Control/MainControl.hh"


namespace cmpl
{
    /*********** module definition **********/

    // defines the module "linearConditional" that is implemented in class "LinearConditionalMod".
    // to register this module you have to insert "MODULE_CLASS_REG(1, linearConditional)" to file "modules.reg"
    MODULE_CLASS( linearConditional, LinearConditionalMod )




    /*********** handling of command line options **********/

    /**
     * initialize modul, to call immediately after the constructor.
     * @param ctrl			main object
     * @param data			data object
     * @param name			module name (or alias)
     */
    void LinearConditionalMod::init(MainControl *ctrl, MainData *data, const char *name)
    {
        LinearLogConMod::init(ctrl, data, name);

        _nameCondVar = 0;
        _nameCondCon = 0;
    }

    /**
     * register command line options for the extension
     * @param ext           extension object
     * @param id			extension identificator
     */
    void LinearConditionalMod::regExtOptions(ExtensionBase *ext, int id)
    {
        // command line options delivered to the extension by run()
        LinearLogConMod::regExtOptions(ext, id);

        const char *m = modNameRemodel();
        REG_CMDL_OPTION_EXT( OPTION_EXT_NAMECONDVAR, "name-cond-var", 1, 1, CMDL_OPTION_NEG_NO_ARG, true, id, m, EXT_CMDLOPT_INTERPRET_SIMPLE, ext );
        REG_CMDL_OPTION_EXT( OPTION_EXT_NAMECONDCON, "name-cond-con", 1, 1, CMDL_OPTION_NEG_NO_ARG, true, id, m, EXT_CMDLOPT_INTERPRET_SIMPLE, ext );
    }

    /**
     * writes usage info for the module to stream
     * @param s				stream to write to
     */
    void LinearConditionalMod::usage(ostream& s)
    {
        LinearLogConMod::usage(s);

        s << "  -name-cond-var <string>       use given name for variable which replaces a conditional expression" << endl;
        s << "  -name-cond-con <string>       use given name for constraint which sets the variable for a conditional expression" << endl;
    }


    /**
     * create derived class object for execution of the remodeling
     */
    RemodelBase *LinearConditionalMod::create(RemodelBaseMod *mod)
    {
        return new LinearConditional((LinearConditionalMod *)mod);
    }
}


