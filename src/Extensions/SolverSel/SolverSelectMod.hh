
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


#ifndef SOLVERSELECTMOD_HH
#define SOLVERSELECTMOD_HH


#include "../../Control/ExtensionModule.hh"


using namespace std;



namespace cmpl
{
    class SolverSelect;


    /**
	 * <code>SolverSelectMod</code> is the extension module class for extension <code>SolverSelect</code>
	 * which defines compatibility symbols for cmpl.
	 */
	class SolverSelectMod : public ExtensionModule
	{
        friend class SolverSelect;



		/************** overwritten methods of <code>ModuleBase</code> **********/

	public:
		/**
		 * run module function
		 */
		virtual void run();

    protected:
        /**
         * register command line options for the extension
         * @param ext           extension object
         * @param id			extension identificator
         */
        virtual void regExtOptions(ExtensionBase *ext, int id);

        /**
         * writes usage info for the module to stream
         * @param s				stream to write to
         */
        virtual void usage(ostream& s);

        static const char *defaultConfigFile()          { return "solversel.opt"; }


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
	};
}

#endif // SOLVERSELECTMOD_HH

