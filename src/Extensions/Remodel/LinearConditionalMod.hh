
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


#ifndef LINEARCONDITIONALMOD_HH
#define LINEARCONDITIONALMOD_HH


#include "RemodelBaseMod.hh"
#include "LinearLogConMod.hh"


using namespace std;



namespace cmpl
{
	class LinearConditional;


    /**
     * <code>LinearConditionalMod</code> is the extension Module for
     * linearization of conditional constraints and constraints with conditional parts (direct or via conversion to logical connected constraints)
	 */
    class LinearConditionalMod : public LinearLogConMod
	{
        friend class LinearConditional;


        /************** command line options **********/

    private:
        unsigned _nameCondVar;      ///< name for variable which replaces a conditional expression
        unsigned _nameCondCon;      ///< name for constraint which sets the variable for a conditional expression


        /************** overwritten methods of <code>ModuleBase</code> **********/

    protected:
        /**
         * initialize modul, to call immediately after the constructor.
         * @param ctrl			main object
         * @param data			data object
         * @param name			module name (or alias)
         */
        void init(MainControl *ctrl, MainData *data, const char *name) override;

        /**
         * register command line options for the extension
         * @param ext           extension object
         * @param id			extension identificator
         */
        void regExtOptions(ExtensionBase *ext, int id) override;

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
    };
}

#endif // LINEARCONDITIONALMOD_HH

