
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


#include "LinearVarProdMod.hh"
#include "LinearVarProd.hh"
#include "../../Control/MainControl.hh"


namespace cmpl
{
    /*********** module definition **********/

    // defines the module "linearVarProd" that is implemented in class "LinearVarProdMod".
    // to register this module you have to insert "MODULE_CLASS_REG(1, linearVarProd)" to file "modules.reg"
    MODULE_CLASS( linearVarProd, LinearVarProdMod )



    /*********** handling of command line options **********/

    /**
     * initialize modul, to call immediately after the constructor.
     * @param ctrl			main object
     * @param data			data object
     * @param name			module name (or alias)
     */
    void LinearVarProdMod::init(MainControl *ctrl, MainData *data, const char *name)
    {
        RemodelBaseMod::init(ctrl, data, name);

        _prodLinearLvl = 3;
        _prodRealWarn = 1;

        _prodDecomp = true;
        _useBigMBound = true;

        _attachNameVarDecomp = data->globStrings()->storeInd("b");
        _attachNameVarNorm = data->globStrings()->storeInd("n");
    }

    /**
     * register command line options for the extension
     * @param ext           extension object
     * @param id			extension identificator
     */
    void LinearVarProdMod::regExtOptions(ExtensionBase *ext, int id)
    {
        // command line options delivered to the extension by run()
        RemodelBaseMod::regExtOptions(ext, id);

        const char *m = modNameRemodel();
        REG_CMDL_OPTION_EXT( OPTION_EXT_VARPRODLINEARLVL, "var-prod-linear", 0, 1, CMDL_OPTION_NEG_NO_ARG, true, id, m, EXT_CMDLOPT_INTERPRET_SIMPLE, ext );
        REG_CMDL_OPTION_EXT( OPTION_EXT_PRODREALWARNLVL, "real-prod-warning", 0, 1, CMDL_OPTION_NEG_NO_ARG, true, id, m, EXT_CMDLOPT_INTERPRET_SIMPLE, ext );

        REG_CMDL_OPTION_EXT( OPTION_EXT_VARPRODDECOMP, "var-prod-decomp", 0, 0, CMDL_OPTION_NEG_NO_ARG, true, id, m, EXT_CMDLOPT_INTERPRET_SIMPLE, ext );
        REG_CMDL_OPTION_EXT( OPTION_EXT_USEBIGMBOUND, "bigM-bound", 0, 0, CMDL_OPTION_NEG_NO_ARG, true, id, m, EXT_CMDLOPT_INTERPRET_SIMPLE, ext );

        REG_CMDL_OPTION_EXT( OPTION_EXT_ATTACHNAMEVARDECOMP, "an-var-bin-decomp", 1, 1, CMDL_OPTION_NEG_NO_ARG, true, id, m, EXT_CMDLOPT_INTERPRET_SIMPLE, ext );
        REG_CMDL_OPTION_EXT( OPTION_EXT_ATTACHNAMEVARNORM, "an-var-norm-real", 1, 1, CMDL_OPTION_NEG_NO_ARG, true, id, m, EXT_CMDLOPT_INTERPRET_SIMPLE, ext );
    }

    /**
     * writes usage info for the module to stream
     * @param s				stream to write to
     */
    void LinearVarProdMod::usage(ostream& s)
    {
        RemodelBaseMod::usage(s);

        s << "  -var-prod-linear <lvl>        level of lineariztion of products of variables (0:no / 1:only bin variable / 2:also int variable / 3:also approximation for real variables) (default is " << _prodLinearLvl << ")" << endl;
        s << "  -real-prod-warning <lvl>      show warning if approximating a product of real variables (0:no / 1:yes for first such product / 2:yes for each such product) (default is " << _prodRealWarn << ")" << endl;

        s << "  -var-prod-decomp              for non-linearized products of more than two variables decomposite to products each with only two variables (default is " << (_prodDecomp ? "true" : "false") << ")" << endl;

        s << "  -bigM-bound                   allow variables without bounds in products using big-M as replacement bound (default is " << (_useBigMBound ? "true" : "false") << ")" << endl;

        s << "  -an-var-bin-decomp <string>   string attached to name for binary variable name for decomposition of an integer variable (default is '" << data()->globStrings()->at(_attachNameVarDecomp) << "')" << endl;
        s << "  -an-var-norm-real <string>    string attached to name for variable name for normalization of a real variable (default is '" << data()->globStrings()->at(_attachNameVarNorm) << "')" << endl;
    }


    /**
     * create derived class object for execution of the remodeling
     */
    RemodelBase *LinearVarProdMod::create(RemodelBaseMod *mod)
    {
        return new LinearVarProd((LinearVarProdMod *)mod);
    }
}


