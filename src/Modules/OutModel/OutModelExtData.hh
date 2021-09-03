
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


#ifndef OUTMODELEXTDATA_HH
#define OUTMODELEXTDATA_HH

#include <string>
#include <map>
#include <list>

#include "OutModelExtData.hh"
#include "../../Control/ModuleBase.hh"


using namespace std;


namespace cmpl
{
    /**
	 * base class for data interface of special extensions to optimization model
	 * between the extension class providing the data and the model output class
	 */
	class OutModelExtDataBase
	{
    public:
        struct Info
        {
            int typeKey = 0;                    ///< type of the special data, one of OutModelExtData*::key / 0: empty
            OutModelExtDataBase *dp = NULL;     ///< object for getting the data, must be of the class associated with typeKey
            unsigned rows = 0;                  ///< count of data rows to fetch from extension
            bool onlyRem = false;               ///< data is only for comment and don't affect the optimization result (especially if it is already otherwise part of the model, e.g. by linearization)
            int errLvl = 0;                     ///< error level if output format cannot handle the data / 0: ignore
            string errMsg;                      ///< error message if output format cannot handle the data (only used if errLvl > 0)
            const LocationInfo *errLoc;         ///< location info for error message (only used if errLvl > 0)

            /**
             * constructor
             */
            Info(int k, OutModelExtDataBase *p, unsigned r, bool on, int el, string em, const LocationInfo *ec): typeKey(k), dp(p), rows(r), onlyRem(on), errLvl(el), errMsg(em), errLoc(ec)      { }
        };

        /**
         * get info of special extension data for output from all registered extension classes
         * @param modp          calling module
         * @param mki           filled by caller with all OutModelExtData*::key the calling module can handle, return the list of data infos to handle
         * @return              true if there is special extension data for the calling module
         */
        static bool getInfo(ModuleBase *modp, map<int, list<Info>>& mki);
	};


    #define OutModelExtDataSOS1Key           101

	/**
     * class for data for SOS
	 */
    class OutModelExtDataSOS1: public OutModelExtDataBase
    {
    public:
        //constexpr static int key = 101;         ///< key identifying this type
        //const static int key = 101;         ///< key identifying this type

        /**
         * get data for one SOS
         * @param info          info for special extension data
         * @param row           data row number, must be lesser than info.rows
         * @param vars          return of identy numbers of the optimization variables in the SOS
         * @param name          name for SOS / 0: no name given
         * @return              identity number of SOS/SOS2
         */
        //virtual unsigned getData(Info& info, unsigned row, vector<unsigned long>* vars, unsigned& name) = 0;
        virtual unsigned getData(Info& info, unsigned row, vector<unsigned long>& vars, unsigned& name) = 0;
    };


    #define OutModelExtDataSOS2Key           102
    /**
     * class for data for SOS2
     */
    class OutModelExtDataSOS2: public OutModelExtDataSOS1
    {
    public:
        //constexpr static int key = 102;         ///< key identifying this type
        //const static int key = 102;         ///< key identifying this type
    };


}

#endif // OUTMODELEXTDATA_HH

