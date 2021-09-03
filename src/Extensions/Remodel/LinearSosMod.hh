
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


#ifndef LINEARSOSMOD_HH
#define LINEARSOSMOD_HH


#include "RemodelBaseMod.hh"


using namespace std;



namespace cmpl
{
	class LinearSos;

	
    /**
     * <code>LinearSosMod</code> is the extension Module for
	 * subsymbols for SOS and SOS2 definition and the possible linearization of SOS.
	 */
    class LinearSosMod : public RemodelBaseMod
	{
        friend class LinearSos;


        /************** command line options **********/

    private:
        enum SosType { sosTypeNone = 0, sosTypeOnlySos1 = 1, sosTypeOnlySos2 = 2, sosTypeBoth = 3 };

        vector<unsigned> _namespace;        ///< namespace parts of the namespace for the functions

        SosType _useForSos;                 ///< use extension for this SOS type (cannot be sosTypeNone)

        bool _warnInvalid;                  ///< show warning if SOS is invalid because of contained non zero parameters

        unsigned _attachNameVarSos;         ///< postfix for variable name for linearization of SOS (positive part)
        unsigned _attachNameVarSosNeg;      ///< postfix for variable name for linearization of SOS (negative part)
        unsigned _attachNameConSos;         ///< postfix for constraint name for linearization of SOS (max non zero count restriction)

        unsigned _attachNameVarSos2;        ///< postfix for variable name for linearization of order for SOS2
        unsigned _attachNameConSos2;        ///< postfix for constraint name for linearization of SOS2 (order of non zero elements restriction)


        /************** overwritten methods of <code>ModuleBase</code> **********/

    public:
        /**
         * run module function
         */
        void run() override;

    protected:
        /**
         * initialize modul, to call immediately after the constructor.
         * @param ctrl			main object
         * @param data			data object
         * @param name			module name (or alias)
         */
        void init(MainControl *ctrl, MainData *data, const char *name) override;

        /**
         * register command line options options for delivery to this module
         * @param modOptReg		vector to register in
         */
        void regModOptions(vector<CmdLineOptList::RegOption> &modOptReg) override;

        /**
         * register command line options for the extension
         * @param ext           extension object
         * @param id			extension identificator
         */
        void regExtOptions(ExtensionBase *ext, int id) override;

        /**
         * parse single option from command line options, this function is called for every delivered option
         * @param ref			reference number of option registration, should be used for discriminate the options
         * @param prio			priority value of option
         * @param opt			option
         * @return				true if option is used by the module
         */
        bool parseOption(int ref, int prio, CmdLineOptList::SingleOption *opt) override;

        /**
         * writes usage info for the module to stream
         * @param s				stream to write to
         */
        void usage(ostream& s) override;



        /************** internal methods **********/

    protected:
        /**
         * create derived class object for execution of the remodeling
         */
        RemodelBase *create(RemodelBaseMod *mod) override;

        /**
         * flag whether _bigM is used for remodeling
         */
        bool useBigM() override                      { return true; }

        /**
         * flag whether _nameSep is used for remodeling
         */
        bool useNameSep() override                   { return true; }

        /**
         * return registration name of the module calling the remodeling, used for registration of command line options
         */
        const char *modNameRemodel() override        { return (_registerMods.size() > 3 ? _registerMods[3].c_str() : "interpret"); }
    };
}

#endif // LINEARSOSMOD_HH

