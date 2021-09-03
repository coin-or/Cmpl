
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


#ifndef REMODELBASEMOD_HH
#define REMODELBASEMOD_HH


#include "../../Control/ExtensionModule.hh"
#include "../../CommonData/CmplVal.hh"


using namespace std;



namespace cmpl
{
	class RemodelBase;

	
    /**
     * the <code>RemodelBaseMod</code> is the base class of the extension Modules
     * for linearization or other remodeling of constraints and objectives
     */
	class RemodelBaseMod : public ExtensionModule
	{
        friend class RemodelBase;


        /************** command line options **********/

    protected:
        unsigned _maxThreads;       ///< max number of worker threads (0: no threading)
        realType _bigM;             ///< huge number used for linearization

        unsigned _namePref;         ///< prefix string for names of generated constraints or variables / 0: no prefix
        unsigned _nameSep;          ///< separator string between original constraint or variable name and postfix / 0: no separator


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



		/************** overwritten methods of <code>ExtensionModule</code> **********/

	public:
		/**
		 * create the extension object from serialization
		 * @param si        serialization info
		 * @param data      main data object
		 * @param subCnt    count of sub data objects
		 * @param lc		line count per element or 0 if elements are len coded
		 * @param rline		remaining section start line
		 */
		virtual ExtensionBase *createFromSerialization(MainData::SerializeInfo& si, const MainData *data, int subCnt, int lc, string &rline);



        /************** internal methods **********/

    protected:
        /**
         * create derived class object for execution of the remodeling
         */
        virtual RemodelBase *create(RemodelBaseMod *mod) = 0;

        /**
         * flag whether _bigM is used for remodeling
         */
        virtual bool useBigM()                      { return false; }

        /**
         * flag whether _nameSep is used for remodeling
         */
        virtual bool useNameSep()                   { return true; }

        /**
         * return registration name of the module calling the remodeling, used for registration of command line options
         */
        virtual const char *modNameRemodel()        { return (_registerMods.size() > 0 ? _registerMods[0].c_str() : "interpret"); }

    public:
        /**
         * parse single command line option containing a string
         * @param opt           option
         */
        unsigned parseOptString(CmdLineOptList::SingleOption *opt)      { return parseOptString(this, opt); }

        /**
         * parse single command line option containing a string
         * @param mod           module
         * @param opt           option
         */
        static unsigned parseOptString(ModuleBase *mod, CmdLineOptList::SingleOption *opt);
    };
}

#endif // REMODELBASEMOD_HH

