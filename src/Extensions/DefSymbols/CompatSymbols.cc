
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


#include "CompatSymbols.hh"
#include "../../Modules/Compiler/CompilerContext.hh"


// use of insertPredefSymbol() because all here defined symbols are synonyms to standard cmpl symbols
// (if you would define real new symbols then insertExtensionSymbol() must be used, and run() must also handle a step within the Interpreter module)
#define M(n,s,p)	{ comp->insertPredefSymbol(n,false,true,s,p,0); }


namespace cmpl
{
	/**
	 * run the extension function
	 * @param mod			module calling the extension
	 * @param step			execution step within the module
	 * @param id			additional identificator
	 * @param par			additional parameter
	 */
	void CompatSymbols::run(ModuleBase *mod, int step, int id, void *par)
	{
		if (step == EXT_STEP_COMPILE_PREDEF_SYMBOLS) {
			PROTO_MOD_OUTL(mod, " extension CompatSymbols: define symbols: " << step << " " << id);

			CompilerContext *comp = (CompilerContext *)par;

			// synonyms for object types
			M ( "parameters",			specialSymbolObjTypePar,	PREDEFSYMVAL_OBJTYPE_PAR )
			M ( "variables",			specialSymbolObjType,		PREDEFSYMVAL_OBJTYPE_VAR )
			M ( "objectives",			specialSymbolObjType,		PREDEFSYMVAL_OBJTYPE_OBJ )
			M ( "constraints",			specialSymbolObjType,		PREDEFSYMVAL_OBJTYPE_CON )

			// synonyms for data types
			M ( "integer",				specialSymbolDataType,		PREDEFSYMVAL_DATATYPE_INT )
			M ( "binary",				specialSymbolDataType,		PREDEFSYMVAL_DATATYPE_BIN )
		}
		else {
			mod->ctrl()->errHandler().internalError(mod->ctrl()->printBuffer("invalid execution step for extension CompatSymbols, called for '%s' %d %d", mod->moduleName(), step, id));
		}

		PROTO_MOD_OUTL(mod, " extension CompatSymbols: end");
	}

	/**
	 * write data object to output stream
	 * @param si        serialization info
	 * @param data      main data object
	 */
	void CompatSymbols::serializeTo(MainData::SerializeInfo& si, const MainData *data)
	{
		// extension has no own data
		(*si.ostr) << endl;
	}

	/**
	 * fill data object from input stream
	 * @param si        serialization info
	 * @param data      main data object
	 * @param subCnt    count of sub data objects
	 * @param lc		line count per element or 0 if elements are len coded
	 * @param rline		remaining section start line
	 */
	void CompatSymbols::deserializeFrom(MainData::SerializeInfo& si, const MainData *data, int subCnt, int lc, string &rline)
	{
		// extension has no own data
		si.pos->addLines();
	}
}


