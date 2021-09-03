
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


#include "ClassFunctions.hh"
#include "../../Modules/Compiler/CompilerContext.hh"
#include "../../Modules/Interpreter/ValueStore.hh"
#include "../../Modules/Interpreter/ExecContext.hh"
#include "../../Modules/Interpreter/Interpreter.hh"
#include "../../Modules/Interpreter/ValContainer.hh"


// offset numbers for functions
#define OFFSET_CLASSFUNC_RUNAT   				0
#define OFFSET_CLASSFUNC_CONSTRUCT				1
#define OFFSET_CLASSFUNC_COPY   				2
#define OFFSET_CLASSFUNC_REFCOPY				3
#define OFFSET_CLASSFUNC_FINALIZE				4


#define M(n,c,s,p)    { (c>=0 ? comp->insertExtensionSymbol(n,true,false,specialSymbolNo,p,c) : comp->insertExtensionSymbol(n,false,true,(s ? specialSymbolNamespace : specialSymbolKnownVal),p,0)); }


namespace cmpl
{
	/**
	 * return count of functions to register
	 */
	unsigned ClassFunctions::countFunc()
	{
        return 5;
	}

	/**
	 * run the extension function
	 * @param mod			module calling the extension
	 * @param step			execution step within the module
	 * @param id			additional identificator
	 * @param par			additional parameter
	 */
	void ClassFunctions::run(ModuleBase *mod, int step, int id, void *par)
	{
		if (step == EXT_STEP_COMPILE_PREDEF_SYMBOLS) {
            PROTO_MOD_OUTL(mod, " extension classFunctions: define symbols: " << step << " " << id);

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
            M ( "runat",	cont, false,	_baseId + offset + OFFSET_CLASSFUNC_RUNAT )
            M ( "construct",cont, false,	_baseId + offset + OFFSET_CLASSFUNC_CONSTRUCT )
            M ( "copy",		cont, false,	_baseId + offset + OFFSET_CLASSFUNC_COPY )
            M ( "refcopy",	cont, false,	_baseId + offset + OFFSET_CLASSFUNC_REFCOPY )
            M ( "finalize", cont, false,	_baseId + offset + OFFSET_CLASSFUNC_FINALIZE )
        }

		else if (step == EXT_STEP_INTERPRET_PREDEF_SYMBOLS_INIT) {
            PROTO_MOD_OUTL(mod, " extension classFunctions: initialize symbol values: " << step << " " << id);

			SymbolValue *sv = (SymbolValue *)par;
            int offset = id - _baseId;

            if (offset < (int)_namespace.size() && offset >= 0) {
                sv->setSimpleConstVal(TP_CONTAINER, new ValContainer(true));
            }
            else {
                switch (offset - _namespace.size()) {
                    case OFFSET_CLASSFUNC_RUNAT:
                        sv->setSimpleConstVal(TP_FUNCTION, new ClassFunctionRunAt());
                        break;

                    case OFFSET_CLASSFUNC_CONSTRUCT:
                        sv->setSimpleConstVal(TP_FUNCTION, new ClassFunctionConstruct());
                        break;

                    case OFFSET_CLASSFUNC_COPY:
                        sv->setSimpleConstVal(TP_FUNCTION, new ClassFunctionCopy());
                        break;

                    case OFFSET_CLASSFUNC_REFCOPY:
                        sv->setSimpleConstVal(TP_FUNCTION, new ClassFunctionRefCopy());
                        break;

                    case OFFSET_CLASSFUNC_FINALIZE:
                        sv->setSimpleConstVal(TP_FUNCTION, new ClassFunctionFinalize());
                        break;

                    default:
                        mod->ctrl()->errHandler().internalError(mod->ctrl()->printBuffer("unknown identificator for extension classFunctions, called for '%s' %d %d", mod->moduleName(), step, id));
                        break;
                }
			}
		}

		else {
            mod->ctrl()->errHandler().internalError(mod->ctrl()->printBuffer("invalid execution step for extension classFunctions, called for '%s' %d %d", mod->moduleName(), step, id));
		}

        PROTO_MOD_OUTL(mod, " extension classFunctions: end");
	}

	/**
	 * write data object to output stream
	 * @param si        serialization info
	 * @param data      main data object
	 */
	void ClassFunctions::serializeTo(MainData::SerializeInfo& si, const MainData *data)
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
	void ClassFunctions::deserializeFrom(MainData::SerializeInfo& si, const MainData *data, int subCnt, int lc, string &rline)
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
    bool ClassFunctionRunAt::operCall(ExecContext *ctx, StackValue *arg)
    {
        execRunAt(ctx, NULL, arg);
        return true;
    }

    /**
     * execute a function at the context of a container
     * @param ctx			execution context
     * @param cont          execute within this container / NULL: first part of arg is the container
     * @param arg			pointer to argument value
     */
    void ClassFunctionRunAt::execRunAt(ExecContext *ctx, ValContainer *cont, StackValue *arg)
    {
        unsigned iter = 0;
        if (!cont) {
            StackValue *c = arg->iterArray(ctx, iter);
            CmplVal *vc = (c ? c->simpleValue() : NULL);
            if (!vc || vc->t != TP_CONTAINER) {
                //TODO: vielleicht kuenftig auch Array von Containern zulassen, Funktion dann fuer jeden rufen, vielleicht auch parallel
                ctx->valueError("argument must be a container object", (c ?: arg));
                ctx->opResult().set(TP_NULL);
                return;
            }
            cont = vc->valContainer();
        }

        CmplVal *va = arg->simpleValue();
        StackValue *f = (va && va->t == TP_FUNCTION ? arg : arg->iterArray(ctx, iter));
        CmplVal *vf = (f ? f->simpleValue() : NULL);
        if (!vf || vf->t != TP_FUNCTION) {
            ctx->valueError("argument must be a function object", (f ?: arg));
            ctx->opResult().set(TP_NULL);
            return;
        }
        ValFunction *func = vf->valFunction();

        StackValue *a = arg->iterArray(ctx, iter);
        if (a) {
            StackValue *t = arg->iterArray(ctx, iter);
            if (t) {
                ctx->valueError("too much arguments for function", t);
                ctx->opResult().set(TP_NULL);
                return;
            }
        }
        else {
            a = ctx->pushPre(arg->syntaxElem());
            a->val().set(TP_NULL);
        }

        ValContainer *pt = ctx->setCallThis(cont);
        func->operCall(ctx, a);
        ctx->setCallThis(pt);
    }


    /**
     * calls the cmpl function call operation and store result in execution context, if this value has a special handling for it
     * @param ctx			execution context
     * @param arg			pointer to argument value
     * @return				true
     */
    bool ClassFunctionConstruct::operCall(ExecContext *ctx, StackValue *arg)
    {
        CmplVal vc;
        vc.set(TP_CONTAINER, new ValContainer());

        execRunAt(ctx, vc.valContainer(), arg);

        ctx->opResult().moveFrom(vc, true);
        return true;
    }


    /**
     * calls the function for a simple source value (no array or list)
     * @param ctx			execution context
     * @param res			store for result value
     * @param src			source value
     * @param aggr          called for aggregating elements of an array or a list
     * @param se			syntax element id of source value
     * @param info          info object for use by the caller
     * @return              only used if aggr: true if result is final
     */
    bool ClassFunctionCopy::operCallSimple(ExecContext *ctx, CmplVal& res, CmplVal& src, bool aggr, unsigned se, void *info)
    {
        // work only for containers
        if (src.t == TP_CONTAINER) {
            src.valContainer()->copy(ctx, res, false);
        }
        else {
            ctx->valueError("ignore value because not a container", src, se, ERROR_LVL_WARN);
            res.copyFrom(src, true, false);
        }

        return false;
    }


    /**
     * calls the function for a simple source value (no array or list)
     * @param ctx			execution context
     * @param res			store for result value
     * @param src			source value
     * @param aggr          called for aggregating elements of an array or a list
     * @param se			syntax element id of source value
     * @param info          info object for use by the caller
     * @return              only used if aggr: true if result is final
     */
    bool ClassFunctionRefCopy::operCallSimple(ExecContext *ctx, CmplVal& res, CmplVal& src, bool aggr, unsigned se, void *info)
    {
        // work only for containers
        if (src.t == TP_CONTAINER) {
            src.valContainer()->copy(ctx, res, true);
        }
        else {
            ctx->valueError("ignore value because not a container", src, se, ERROR_LVL_WARN);
            res.copyFrom(src, true, false);
        }

        return false;
    }


    /**
     * calls the function for a simple source value (no array or list)
     * @param ctx			execution context
     * @param res			store for result value
     * @param src			source value
     * @param aggr          called for aggregating elements of an array or a list
     * @param se			syntax element id of source value
     * @param info          info object for use by the caller
     * @return              only used if aggr: true if result is final
     */
    bool ClassFunctionFinalize::operCallSimple(ExecContext *ctx, CmplVal& res, CmplVal& src, bool aggr, unsigned se, void *info)
    {
        // work only for containers
        if (src.t == TP_CONTAINER) {
            src.valContainer()->setFinalized();
        }
        else {
            ctx->valueError("ignore value because not a container", src, se, ERROR_LVL_WARN);
        }

        // use source as result
        res.copyFrom(src, true, false);
        return false;
    }
}


