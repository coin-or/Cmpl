
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


#ifndef EXTERNXLSDATAFILE_HH
#define EXTERNXLSDATAFILE_HH


#include "ExternDataFile.hh"


using namespace std;



namespace cmpl
{
    /**
	 * the <code>ExternXlsDataFile</code> class is an extension to the interpreter module
	 * for importing data from a cmpl data file, which is originated from Excel data.
	 * because the data is read from a regular cmpl data file, this class is identical to its base class.
	 */
	class ExternXlsDataFile : public ExternDataFile
	{
    public:
		/**
		 * constructor
		 * @param mod			module creating this extension object
         * @param est           type of external data source handled by this extension class
         * @param hp            priority of handling with this extension if more than one extension can handle it
         * @param ra            read values for all symbols from a data file at once
         */
        ExternXlsDataFile(ModuleBase *mod, int est, int hp): ExternDataFile(mod, est, hp)  { }

        /**
         * get standard name for cmpl data file
         */
        const char *cdatStandard() override         { return IO_FILE_STANDARD_XDAT; }

#if PROTO
		/**
		 * get name of the extension, only used for protocol output
		 */
        const char *extName() override  			{ return "externXlsDataFile"; }
#endif //PROTO

    protected:
        /**
         * read all symbols from a data file
         * @param fh                file handling object
         * @param inStr             stream for reading data file
         * @param fn                name of data file
         * @param nm                namespace name / -1: no namespace
         * @param loc				location of data header line
         */
        void readFileAllSymbols(ExternDataFileHandleOpt *fh, istream *inStr, string& fn, int nm, LocationInfo& loc) override;
    };
}

#endif // EXTERNXLSDATAFILE_HH

