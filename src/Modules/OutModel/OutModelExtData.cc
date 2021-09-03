
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


#include "OutModelExtData.hh"
#include "../../Control/MainControl.hh"
#include "../../CommonData/ExtensionSteps.hh"


namespace cmpl
{
	/*********** OutModelExtDataBase **********/

    /**
     * get info of special extension data for output from all registered extension classes
     * @param modp          calling module
     * @param mki           filled by caller with all OutModelExtData*::key the calling module can handle, return the list of data infos to handle
     * @return              true if there is special extension data for the calling module
     */
    bool OutModelExtDataBase::getInfo(ModuleBase *modp, map<int, list<Info>>& mki)
    {
        // get all data from all registered extension classes
        list<Info> lst;
        modp->ctrl()->runExtension(modp, EXT_STEP_OUTMODEL_SPECIAL_INFO, &lst);

        // check for handled and unhandled data
        bool res = false;
        for (Info info : lst) {
            if (info.typeKey) {
                if (mki.count(info.typeKey)) {
                    list<Info>& hl = mki[info.typeKey];
                    hl.push_back(info);
                    res = true;
                }
                else if (info.errLvl) {
                    if (info.errLoc) {
                        modp->ctrl()->errHandler().error(info.errLvl, modp->ctrl()->printBuffer("module '%s' cannot handle special model data: %s", modp->moduleName(), info.errMsg.c_str()), *(info.errLoc));
                    }
                    else {
                        LocationInfo loc(PositionInfo(POSITION_TYPE_DESCR, "(unknown)"));
                        modp->ctrl()->errHandler().error(info.errLvl, modp->ctrl()->printBuffer("module '%s' cannot handle special model data: %s", modp->moduleName(), info.errMsg.c_str()), loc);
                    }
                }
            }
        }

        return res;
    }

}

