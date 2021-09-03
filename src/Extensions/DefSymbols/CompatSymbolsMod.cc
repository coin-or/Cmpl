
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


#include "CompatSymbolsMod.hh"
#include "CompatSymbols.hh"
#include "../../Control/MainControl.hh"


namespace cmpl
{
	/*********** module definition **********/

	// defines the module "compatSymbols" that is implemented in class "CompatSymbolsMod".
	// to register this module you have to insert "MODULE_CLASS_REG(1, compatSymbols)" to file "modules.reg"
	MODULE_CLASS( compatSymbols, CompatSymbolsMod )




	/**
	 * run module function
	 */
	void CompatSymbolsMod::run()
	{
		PROTO_OUTL("Start run compatSymbols module " << moduleName());

		CompatSymbols *ext = new CompatSymbols(this);
		_ctrl->registerExtension((_registerMods.size() > 0 ? _registerMods[0].c_str() : "compile"), EXT_STEP_COMPILE_PREDEF_SYMBOLS, ext);

		PROTO_OUTL("End run compatSymbols module " << moduleName());
	}

	/**
	 * create the extension object from serialization
	 * @param si        serialization info
	 * @param data      main data object
	 * @param subCnt    count of sub data objects
	 * @param lc		line count per element or 0 if elements are len coded
	 * @param rline		remaining section start line
	 */
	ExtensionBase *CompatSymbolsMod::createFromSerialization(MainData::SerializeInfo& si, const MainData *data, int subCnt, int lc, string &rline)
	{
		CompatSymbols *ext = new CompatSymbols(this);
		ext->deserializeFrom(si, data, subCnt, lc, rline);
		return ext;
	}
}


