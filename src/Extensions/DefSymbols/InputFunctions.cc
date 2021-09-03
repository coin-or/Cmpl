
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


#include "InputFunctions.hh"
#include "../../Modules/Compiler/CompilerContext.hh"
#include "../../Modules/Interpreter/ValueStore.hh"
#include "../../Modules/Interpreter/ExecContext.hh"
#include "../../Modules/Interpreter/Interpreter.hh"
#include "../../Modules/Interpreter/ValContainer.hh"


// offset numbers for functions
#define OFFSET_INPUTFUNC_READSTDIN		0
#define OFFSET_INPUTFUNC_READCSV		1


#define M(n,c,s,p)    { (c>=0 ? comp->insertExtensionSymbol(n,true,false,specialSymbolNo,p,c) : comp->insertExtensionSymbol(n,false,true,(s ? specialSymbolNamespace : specialSymbolKnownVal),p,0)); }


namespace cmpl
{
	/**
	 * return count of functions to register
	 */
	unsigned InputFunctions::countFunc()
	{
		return 2;
	}

	/**
	 * run the extension function
	 * @param mod			module calling the extension
	 * @param step			execution step within the module
	 * @param id			additional identificator
	 * @param par			additional parameter
	 */
	void InputFunctions::run(ModuleBase *mod, int step, int id, void *par)
	{
		if (step == EXT_STEP_COMPILE_PREDEF_SYMBOLS) {
			PROTO_MOD_OUTL(mod, " extension inputFunctions: define symbols: " << step << " " << id);

			CompilerContext *comp = (CompilerContext *)par;
            int cont = -1;
            unsigned offset = 0;
            _baseId = id;

            // define namespace symbols
            if (!_namespace.empty()) {
                for (unsigned i = 0; i < _namespace.size(); i++) {
                    const char *nsp = mod->data()->globStrings()->at(_namespace[i]);
                    M ( nsp, cont, true, _baseId + offset);

                    cont = _baseId + offset;
                    offset++;
                }
            }

            // define function symbols
            M ( "readstdin",	cont, false,	_baseId + offset + OFFSET_INPUTFUNC_READSTDIN )
            M ( "readcsv",		cont, false,	_baseId + offset + OFFSET_INPUTFUNC_READCSV )
		}

		else if (step == EXT_STEP_INTERPRET_PREDEF_SYMBOLS_INIT) {
			PROTO_MOD_OUTL(mod, " extension inputFunctions: initialize symbol values: " << step << " " << id);

			SymbolValue *sv = (SymbolValue *)par;
            int offset = id - _baseId;

            if (offset < (int)_namespace.size() && offset >= 0) {
                sv->setSimpleConstVal(TP_CONTAINER, new ValContainer(true));
            }
            else {
                switch (offset - _namespace.size()) {
                    case OFFSET_INPUTFUNC_READSTDIN:
                        sv->setSimpleConstVal(TP_FUNCTION, new InputFunctionReadStdin());
                        break;

                    case OFFSET_INPUTFUNC_READCSV:
                        sv->setSimpleConstVal(TP_FUNCTION, new InputFunctionReadCSV());
                        break;

                    default:
                        mod->ctrl()->errHandler().internalError(mod->ctrl()->printBuffer("unknown identificator for extension inputFunctions, called for '%s' %d %d", mod->moduleName(), step, id));
                        break;
                }
			}
		}

		else {
			mod->ctrl()->errHandler().internalError(mod->ctrl()->printBuffer("invalid execution step for extension inputFunctions, called for '%s' %d %d", mod->moduleName(), step, id));
		}

		PROTO_MOD_OUTL(mod, " extension inputFunctions: end");
	}

	/**
	 * write data object to output stream
	 * @param si        serialization info
	 * @param data      main data object
	 */
	void InputFunctions::serializeTo(MainData::SerializeInfo& si, const MainData *data)
	{
        // write _baseId
        (*si.ostr) << ';' << _baseId;

        // write namespace parts
        (*si.ostr) << ';' << _namespace.size();
        for (unsigned i = 0; i < _namespace.size(); i++)
            (*si.ostr) << ';' << _namespace[i];

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
	void InputFunctions::deserializeFrom(MainData::SerializeInfo& si, const MainData *data, int subCnt, int lc, string &rline)
	{
        // read _baseId
        _baseId = MainData::getNextLong(rline, si.pos);

        // read namespace parts
        _namespace.clear();
        unsigned nsc = MainData::getNextLong(rline, si.pos);
        for (unsigned i = 0; i < nsc; i++)
            _namespace.push_back(MainData::getNextLong(rline, si.pos));

        si.pos->addLines();
    }



	/**
	 * calls the cmpl function call operation and store result in execution context, if this value has a special handling for it
	 * @param ctx			execution context
	 * @param arg			pointer to argument value
	 * @return				true
	 */
	bool InputFunctionReadStdin::operCall(ExecContext *ctx, StackValue *arg)
	{
		//TODO
		ctx->valueError("not implemented: readstdin", arg);
		return true;
	}


	/**
	 * calls the cmpl function call operation and store result in execution context, if this value has a special handling for it
	 * @param ctx			execution context
	 * @param arg			pointer to argument value
	 * @return				true
	 */
	bool InputFunctionReadCSV::operCall(ExecContext *ctx, StackValue *arg)
	{
		//TODO
        ctx->valueError("not implemented: readcsv", arg);
		return true;
	}

}


