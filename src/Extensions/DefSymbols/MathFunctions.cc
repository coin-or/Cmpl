
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


#include "MathFunctions.hh"
#include "../../Modules/Compiler/CompilerContext.hh"
#include "../../Modules/Interpreter/ValueStore.hh"
#include "../../Modules/Interpreter/ExecContext.hh"
#include "../../Modules/Interpreter/Interpreter.hh"
#include "../../Modules/Interpreter/ValContainer.hh"


// offset numbers for functions
#define OFFSET_MATHFUNC_SQRT			0
#define OFFSET_MATHFUNC_EXP				1
#define OFFSET_MATHFUNC_LN				2
#define OFFSET_MATHFUNC_LG				3
#define OFFSET_MATHFUNC_LD				4
#define OFFSET_MATHFUNC_SIN				5
#define OFFSET_MATHFUNC_COS				6
#define OFFSET_MATHFUNC_TAN				7
#define OFFSET_MATHFUNC_ASIN			8
#define OFFSET_MATHFUNC_ACOS			9
#define OFFSET_MATHFUNC_ATAN			10
#define OFFSET_MATHFUNC_SINH			11
#define OFFSET_MATHFUNC_COSH			12
#define OFFSET_MATHFUNC_TANH			13
#define OFFSET_MATHFUNC_ABS				14
#define OFFSET_MATHFUNC_CEIL			15
#define OFFSET_MATHFUNC_FLOOR			16
#define OFFSET_MATHFUNC_ROUND			17


#define M(n,c,s,p)    { (c>=0 ? comp->insertExtensionSymbol(n,true,false,specialSymbolNo,p,c) : comp->insertExtensionSymbol(n,false,true,(s ? specialSymbolNamespace : specialSymbolKnownVal),p,0)); }


namespace cmpl
{
	/**
	 * return count of functions to register
	 */
	unsigned MathFunctions::countFunc()
	{
		return 18;
	}

	/**
	 * run the extension function
	 * @param mod			module calling the extension
	 * @param step			execution step within the module
	 * @param id			additional identificator
	 * @param par			additional parameter
	 */
	void MathFunctions::run(ModuleBase *mod, int step, int id, void *par)
	{
		if (step == EXT_STEP_COMPILE_PREDEF_SYMBOLS) {
			PROTO_MOD_OUTL(mod, " extension mathFunctions: define symbols: " << step << " " << id);

			CompilerContext *comp = (CompilerContext *)par;
            int cont = -1;
            unsigned offset = 0;
			_baseId = id;

            // define namespace symbols
            if (!_namespace.empty()) {
                for (unsigned i = 0; i < _namespace.size(); i++) {
                    const char *nsp = mod->data()->globStrings()->at(_namespace[i]);
                    M ( nsp, cont, true, _baseId + offset );

                    cont = _baseId + offset;
                    offset++;
                }
            }

            // define function symbols
            M ( "sqrt",		cont, false,	_baseId + offset + OFFSET_MATHFUNC_SQRT )
            M ( "exp",		cont, false,	_baseId + offset + OFFSET_MATHFUNC_EXP )
            M ( "ln",		cont, false,	_baseId + offset + OFFSET_MATHFUNC_LN )
            M ( "lg",		cont, false,	_baseId + offset + OFFSET_MATHFUNC_LG )
            M ( "ld",		cont, false,	_baseId + offset + OFFSET_MATHFUNC_LD )
            M ( "sin",		cont, false,	_baseId + offset + OFFSET_MATHFUNC_SIN )
            M ( "cos",		cont, false,	_baseId + offset + OFFSET_MATHFUNC_COS )
            M ( "tan",		cont, false,	_baseId + offset + OFFSET_MATHFUNC_TAN )
            M ( "asin",		cont, false,	_baseId + offset + OFFSET_MATHFUNC_ASIN )
            M ( "acos",		cont, false,	_baseId + offset + OFFSET_MATHFUNC_ACOS )
            M ( "atan",		cont, false,	_baseId + offset + OFFSET_MATHFUNC_ATAN )
            M ( "sinh",		cont, false,	_baseId + offset + OFFSET_MATHFUNC_SINH )
            M ( "cosh",		cont, false,	_baseId + offset + OFFSET_MATHFUNC_COSH )
            M ( "tanh",		cont, false,	_baseId + offset + OFFSET_MATHFUNC_TANH )
            M ( "abs",		cont, false,	_baseId + offset + OFFSET_MATHFUNC_ABS )
            M ( "ceil",		cont, false,	_baseId + offset + OFFSET_MATHFUNC_CEIL )
            M ( "floor",	cont, false,	_baseId + offset + OFFSET_MATHFUNC_FLOOR )
            M ( "round",	cont, false,	_baseId + offset + OFFSET_MATHFUNC_ROUND )
		}

		else if (step == EXT_STEP_INTERPRET_PREDEF_SYMBOLS_INIT) {
			PROTO_MOD_OUTL(mod, " extension mathFunctions: initialize symbol values: " << step << " " << id);

			SymbolValue *sv = (SymbolValue *)par;
            int offset = id - _baseId;

            if (offset < (int)_namespace.size() && offset >= 0) {
                sv->setSimpleConstVal(TP_CONTAINER, new ValContainer(true));
            }
            else {
                switch (offset - _namespace.size()) {
                    case OFFSET_MATHFUNC_SQRT:
                        sv->setSimpleConstVal(TP_FUNCTION, new MathFunctionSqrt());
                        break;

                    case OFFSET_MATHFUNC_EXP:
                        sv->setSimpleConstVal(TP_FUNCTION, new MathFunctionExp());
                        break;

                    case OFFSET_MATHFUNC_LN:
                        sv->setSimpleConstVal(TP_FUNCTION, new MathFunctionLn());
                        break;

                    case OFFSET_MATHFUNC_LG:
                        sv->setSimpleConstVal(TP_FUNCTION, new MathFunctionLg());
                        break;

                    case OFFSET_MATHFUNC_LD:
                        sv->setSimpleConstVal(TP_FUNCTION, new MathFunctionLd());
                        break;

                    case OFFSET_MATHFUNC_SIN:
                        sv->setSimpleConstVal(TP_FUNCTION, new MathFunctionSin());
                        break;

                    case OFFSET_MATHFUNC_COS:
                        sv->setSimpleConstVal(TP_FUNCTION, new MathFunctionCos());
                        break;

                    case OFFSET_MATHFUNC_TAN:
                        sv->setSimpleConstVal(TP_FUNCTION, new MathFunctionTan());
                        break;

                    case OFFSET_MATHFUNC_ASIN:
                        sv->setSimpleConstVal(TP_FUNCTION, new MathFunctionAsin());
                        break;

                    case OFFSET_MATHFUNC_ACOS:
                        sv->setSimpleConstVal(TP_FUNCTION, new MathFunctionAcos());
                        break;

                    case OFFSET_MATHFUNC_ATAN:
                        sv->setSimpleConstVal(TP_FUNCTION, new MathFunctionAtan());
                        break;

                    case OFFSET_MATHFUNC_SINH:
                        sv->setSimpleConstVal(TP_FUNCTION, new MathFunctionSinh());
                        break;

                    case OFFSET_MATHFUNC_COSH:
                        sv->setSimpleConstVal(TP_FUNCTION, new MathFunctionCosh());
                        break;

                    case OFFSET_MATHFUNC_TANH:
                        sv->setSimpleConstVal(TP_FUNCTION, new MathFunctionTanh());
                        break;

                    case OFFSET_MATHFUNC_ABS:
                        sv->setSimpleConstVal(TP_FUNCTION, new MathFunctionAbs());
                        break;

                    case OFFSET_MATHFUNC_CEIL:
                        sv->setSimpleConstVal(TP_FUNCTION, new MathFunctionCeil());
                        break;

                    case OFFSET_MATHFUNC_FLOOR:
                        sv->setSimpleConstVal(TP_FUNCTION, new MathFunctionFloor());
                        break;

                    case OFFSET_MATHFUNC_ROUND:
                        sv->setSimpleConstVal(TP_FUNCTION, new MathFunctionRound());
                        break;

                    default:
                        mod->ctrl()->errHandler().internalError(mod->ctrl()->printBuffer("unknown identificator for extension mathFunctions, called for '%s' %d %d", mod->moduleName(), step, id));
                        break;
                }
            }
		}

		else {
			mod->ctrl()->errHandler().internalError(mod->ctrl()->printBuffer("invalid execution step for extension mathFunctions, called for '%s' %d %d", mod->moduleName(), step, id));
		}

		PROTO_MOD_OUTL(mod, " extension mathFunctions: end");
	}

	/**
	 * write data object to output stream
	 * @param si        serialization info
	 * @param data      main data object
	 */
	void MathFunctions::serializeTo(MainData::SerializeInfo& si, const MainData *data)
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
	void MathFunctions::deserializeFrom(MainData::SerializeInfo& si, const MainData *data, int subCnt, int lc, string &rline)
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
	bool MathFunctionRealToReal::operCall(ExecContext *ctx, StackValue *arg)
	{
		CmplVal *v = arg->simpleValue();
		if (v && !v->isList()) {
			intType i;
			if (hasExecInt() && v->toInt(i, typeConversionExact, ctx->modp())) {
                ctx->opResult().set(TP_INT, execInt(i));
			}
			else {
				realType a, res;
				if (v->toReal(a, typeConversionValue, ctx->modp())) {
					res = exec(a);
				}
				else {
					ctx->valueError(ctx->modp()->ctrl()->printBuffer("not numeric argument for math function %s", funcName()), arg);
					res = 0;
				}

                if (std::isnan(res)) {
					ctx->valueError(ctx->modp()->ctrl()->printBuffer("illegal argument for math function %s", funcName()), arg);
					res = 0;
				}

				if (res == HUGE_VAL || res == -HUGE_VAL)
                    ctx->opResult().set(TP_INFINITE, (res == HUGE_VAL ? 1 : -1));
				else
					ctx->opResult().set(TP_REAL, res);
			}
		}
		else {
			//TODO: array/list/symbol: iterieren und für jedes Element einzeln Ergebnis ermitteln
			ctx->valueError(ctx->modp()->ctrl()->printBuffer("not implemented: array/list/symbol argument for math function %s", funcName()), arg);
			ctx->opResult().set(TP_NULL);
		}

		return true;
	}

}


