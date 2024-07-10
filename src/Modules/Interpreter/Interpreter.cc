
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


#include "Interpreter.hh"
#include "ExecContext.hh"
#include "ValueStore.hh"
#include "ValFunction.hh"
#include "ValType.hh"
#include "ValContainer.hh"
#include "../../Control/MainControl.hh"
#include "../../CommonData/IntCode.hh"
#include "../../CommonData/SyntaxElements.hh"
#include "../../CommonData/ExtensionSteps.hh"


namespace cmpl
{
	/*********** module definition **********/

	// defines the module "interpret" that is implemented in class "Interpreter".
	// to register this module you have to insert "MODULE_CLASS_REG(1, interpret)" to file "modules.reg"
	MODULE_CLASS( interpret, Interpreter )



	/*********** initialization **********/

	/**
	 * destructor
	 */
	Interpreter::~Interpreter()
	{
		if (_syntaxElems)
			delete _syntaxElems;

        if (_globSymbols)
            delete[] _globSymbols;

        // (_resMatrix darf nicht freigegeben werden, da in _data referenziert)
        //TODO
	}

	/**
	 * initialize modul, to call immediately after the constructor.
	 * @param ctrl			main object
	 * @param data			data object
	 * @param name			module name (or alias)
	 */
	void Interpreter::init(MainControl *ctrl, MainData *data, const char *name)
	{
		ModuleBase::init(ctrl, data, name);

		_code = NULL;
		_codeCnt = 0;
		_symbolInfo = NULL;
		_syntaxStructure = NULL;
		_syntaxElems = NULL;

		_globSymbols = NULL;

        _intOutOfRangeMode = INT_OPER_OORANGE_ERROR;
        _echoDuration = false;
        _nmPrefSep = 0;
        _nmPrefAfter = -1;

        _resModel = NULL;

        _externInfo = NULL;

        srand(time(NULL));

		//TODO
	}


	/*********** handling of command line options **********/

#define OPTION_INTERPRETER_INT_OORANGE_MODE	10

#define OPTION_INTERPRETER_INT_THREADS      20
#define OPTION_INTERPRETER_ORDERED          21

#define OPTION_INTERPRETER_INT_ECHO_DUR     22

#define OPTION_INTERPRETER_NAMEPREFSEP      23
#define OPTION_INTERPRETER_NAMEPREFAFTER    24

#define OPTION_INTERPRETER_FILEALIAS        30

	//TODO


	/**
	 * register command line options options for delivery to this module
	 * @param modOptReg		vector to register in
	 */
	void Interpreter::regModOptions(vector<CmdLineOptList::RegOption> &modOptReg)
	{
		ModuleBase::regModOptions(modOptReg);

        REG_CMDL_OPTION( OPTION_INTERPRETER_INT_OORANGE_MODE, "int-out-of-range", 1, 1, CMDL_OPTION_NEG_ERROR, true );

        REG_CMDL_OPTION( OPTION_INTERPRETER_INT_THREADS, "threads", 1, 1, CMDL_OPTION_NEG_NO_ARG, true );
        REG_CMDL_OPTION( OPTION_INTERPRETER_ORDERED, "ordered", 0, 0, CMDL_OPTION_NEG_NO_ARG, true );
        REG_CMDL_OPTION( OPTION_INTERPRETER_INT_ECHO_DUR, "echoduration", 0, 0, CMDL_OPTION_NEG_NO_ARG, true );

        REG_CMDL_OPTION( OPTION_INTERPRETER_NAMEPREFSEP, "name-prefix-separator", 1, 1, CMDL_OPTION_NEG_NO_ARG, true );
        REG_CMDL_OPTION( OPTION_INTERPRETER_NAMEPREFSEP, "name-prefix-after", 1, 1, CMDL_OPTION_NEG_NO_ARG, true );

        REG_CMDL_OPTION( OPTION_INTERPRETER_FILEALIAS, "filealias", 0, -1, CMDL_OPTION_NEG_ERROR, false );

        regExtOption(EXT_CMDLOPT_INTERPRET_SIMPLE);
    }

	/**
	 * parse single option from command line options, this function is called for every delivered option
	 * @param ref			reference number of option registration, should be used for discriminate the options
	 * @param prio			priority value of option
	 * @param opt			option
	 * @return				true if option is used by the module
	 */
	bool Interpreter::parseOption(int ref, int prio, CmdLineOptList::SingleOption *opt)
	{
		if (ModuleBase::parseOption(ref, prio, opt))
			return true;

		switch (ref) {
            case OPTION_INTERPRETER_INT_OORANGE_MODE:
                switch ((*opt)[0][0]) {
                    case 'i':
                    case 'I':
                        _intOutOfRangeMode = INT_OPER_OORANGE_IGNORE;
                        break;
                    case 'e':
                    case 'E':
                        _intOutOfRangeMode = INT_OPER_OORANGE_ERROR;
                        break;
                    case 'c':
                    case 'C':
                        _intOutOfRangeMode = INT_OPER_OORANGE_CONVERT;
                        break;
                    default:
                        ERRHANDLER.error(ERROR_LVL_EASY, "unknown mode for handling of integer out of range", opt->argPos(0));
                }
                return true;

            case OPTION_INTERPRETER_INT_THREADS:
                _threadHandler.setMaxThreads((unsigned)(opt->neg() ? 0 : opt->argAsInt(0, _ctrl)));
                return true;

            case OPTION_INTERPRETER_ORDERED:
                _orderedIter = !(opt->neg());
                return true;

            case OPTION_INTERPRETER_INT_ECHO_DUR:
                _echoDuration = !(opt->neg());
                return true;

            case OPTION_INTERPRETER_NAMEPREFSEP:
                _nmPrefSep = (opt->neg() ? 0 : _data->globStrings()->storeInd((*opt)[0]));
                return true;

            case OPTION_INTERPRETER_NAMEPREFAFTER:
                _nmPrefAfter = (opt->neg() ? -1 : (int) _data->globStrings()->storeInd((*opt)[0]));
                return true;

            case OPTION_INTERPRETER_FILEALIAS:
                parseFileAliases(opt, _fileAlias, _fileAliasPrio);
                return true;

            //TODO
		}

		return false;
	}

    /**
     * execute step for every registered extension for a single command line option
     * @param extStep       extension execution step
     * @param extRef        map from extension identifier to command line option reference key
     * @param prio          priority value of option
     * @param opt           option
     * @return              true if option is used by an extension
     */
    bool Interpreter::execExtOption(int extStep, map<int, int>& extRef, int prio, CmdLineOptList::SingleOption *opt)
    {
        switch (extStep) {
            case EXT_CMDLOPT_INTERPRET_SIMPLE:
                return execExtOptionSimple(extStep, extRef, prio, opt);
        }

        return false;
    }

	/**
	 * writes usage info for the module to stream
	 * @param s				stream to write to
	 */
	void Interpreter::usage(ostream& s)
	{
		ModuleBase::usage(s);

        s << "  -int-out-of-range <mode>      mode for handling integer operation results out of range (ignore/error/convert)" << endl;
        s << "  -threads <n>                  use maximal n concurrently running worker threads (0: no threading)" << endl;
        s << "  -ordered                      use ordered execution in all explicit and implicit iterations" << endl;
        s << "  -echoduration                 include duration since start in all outputs of echo function" << endl;
        s << "  -name-prefix-separator <s>    use string s as separator between line name prefixes" << endl;
        s << "  -name-prefix-after <s>        use string s as separator only after line name prefix (if not given then -name-prefix-separator is used)" << endl;
        s << "  -filealias <from1>=<to1> ...  defines aliases for data file names" << endl;

        //TODO
	}


	/**
	 * run module function
	 */
	void Interpreter::run()
	{

        PROTO_OUTL("Start run interpreter module " << moduleName());

		// get necessary data from global data object
		GET_DATA(IntCode, code);
		if (code) {
			_code = code->getCode(_codeCnt);
		}

		if (!_code || _codeCnt == 0)
			_ctrl->errHandler().internalError("no intermediary code found in global data object");

		GET_DATA(SyntaxElements, se);
		if (se) {
			_symbolInfo = se->getSymbolInfo();
			_syntaxStructure = se->getRoot();
		}

		if (!_symbolInfo || !_syntaxStructure)
			_ctrl->errHandler().internalError("no syntax structure information found in global data object");

        SET_NEW_DATA(OptModel, om);
        _resModel = om;

		// initialize predefined symbols
		initSymbols();

		// execute intermediary code
		execCode();

        _resModel = NULL;
		PROTO_OUTL("End run interpreter module " << moduleName());
	}

	/**
	 * initialize symbols
	 */
	void Interpreter::initSymbols()
	{
		_ctrl->errHandler().setExecStep("init symbols");

        initDataTypes();
        _defTypeVar.copyFrom(_dataTypes[TP_REAL], true, false);

		// array for global symbols
		if (_symbolInfo->size() > 0) {
			unsigned maxGlobId = 0;
			for (map<unsigned, SymbolInfo *>::iterator it = _symbolInfo->begin(); it != _symbolInfo->end(); it++) {
				if (maxGlobId < it->second->globId())
					maxGlobId = it->second->globId();
			}

			_globSymbols = new SymbolValue[maxGlobId + 1];

            // initialize global symbols
            map<int, SymbolValue *> predefMap;

			for (map<unsigned, SymbolInfo *>::iterator it = _symbolInfo->begin(); it != _symbolInfo->end(); it++) {
				if (it->second->globId()) {
                    SymbolValue *sv = _globSymbols + it->second->globId();
                    sv->init(it->second, true, (it->second->predef() < 0));

                    // init predefined symbol
                    if (it->second->predef() < 0)
                        initPredef(sv, it->second->predef(), predefMap);
				}
			}

            // initialize predefined subsymbols
            for (map<unsigned, SymbolInfo *>::iterator it = _symbolInfo->begin(); it != _symbolInfo->end(); it++) {
                SymbolInfo *sy = it->second;
                if (!sy->globId()) {
                    if (sy->predefCont())
                        initPredefSub(sy, predefMap);
                    else if (sy->subSymbol() && sy->predef())
                        initSpecialSub(sy);
                }
            }
		}

        _ctrl->runExtension(this, EXT_STEP_INTERPRET_INIT_FINAL);
	}

    /**
     * initialize map _dataTypes
     */
    void Interpreter::initDataTypes()
    {
        _dataTypes.emplace(TP_REAL, CmplVal(TP_DATA_TYPE, new ValTypeReal()));
        _dataTypes.emplace(TP_INT, CmplVal(TP_DATA_TYPE, new ValTypeInt()));
        _dataTypes.emplace(TP_BIN, CmplVal(TP_DATA_TYPE, new ValTypeBin()));

        _dataTypes.emplace(TP_STR, CmplVal(TP_DATA_TYPE, new ValTypeString()));
        _dataTypes.emplace(TP_TUPLE, CmplVal(TP_DATA_TYPE, new ValTypeTuple()));
        _dataTypes.emplace(TP_SET_INF_SET, CmplVal(TP_DATA_TYPE, new ValTypeSet()));
        _dataTypes.emplace(TP_INTERVAL, CmplVal(TP_DATA_TYPE, new ValTypeInterval()));

        _dataTypes.emplace(TP_FORMULA, CmplVal(TP_DATA_TYPE, new ValTypeFormula()));
        _dataTypes.emplace(TP_FUNCTION, CmplVal(TP_DATA_TYPE, new ValTypeFunction()));
        _dataTypes.emplace(TP_CONTAINER, CmplVal(TP_DATA_TYPE, new ValTypeContainer()));

        _dataTypes.emplace(TP_DATA_TYPE, CmplVal(TP_DATA_TYPE, new ValTypeType()));
        _dataTypes.emplace(TP_OBJECT_TYPE, CmplVal(TP_DATA_TYPE, new ValTypeObjecttype()));
    }

	/**
	 * initialize predefined values
	 * @param sv		pointer to symbol value
	 * @param predef	id of predefined value
	 * @param predefMap	map with already initialized predefines
	 */
	void Interpreter::initPredef(SymbolValue *sv, int predef, map<int, SymbolValue *>& predefMap)
	{
		if (predefMap.count(predef)) {
			SymbolValue *src = predefMap[predef];
			sv->copyFrom(src, true);
		}

		else {
			switch (predef) {
				// object types
				case PREDEFSYMVAL_OBJTYPE_PAR:
					sv->simpleConstVal().set(TP_OBJECT_TYPE, (intType)VAL_OBJECT_TYPE_PAR);
					break;

				case PREDEFSYMVAL_OBJTYPE_VAR:
					sv->simpleConstVal().set(TP_OBJECT_TYPE, (intType)VAL_OBJECT_TYPE_VAR);
					break;

				case PREDEFSYMVAL_OBJTYPE_OBJ:
					sv->simpleConstVal().set(TP_OBJECT_TYPE, (intType)VAL_OBJECT_TYPE_OBJ);
					break;

				case PREDEFSYMVAL_OBJTYPE_CON:
					sv->simpleConstVal().set(TP_OBJECT_TYPE, (intType)VAL_OBJECT_TYPE_CON);
					break;

				// data types
				case PREDEFSYMVAL_DATATYPE_REAL:
                    sv->simpleConstVal().copyFrom(_dataTypes[TP_REAL], true, false);
					break;

				case PREDEFSYMVAL_DATATYPE_INT:
                    sv->simpleConstVal().copyFrom(_dataTypes[TP_INT], true, false);
					break;

				case PREDEFSYMVAL_DATATYPE_BIN:
                    sv->simpleConstVal().copyFrom(_dataTypes[TP_BIN], true, false);
					break;

				case PREDEFSYMVAL_DATATYPE_NUMERIC:
					sv->simpleConstVal().set(TP_DATA_TYPE, new ValTypeNumeric());
					break;

				case PREDEFSYMVAL_DATATYPE_FORMULA:
                    sv->simpleConstVal().copyFrom(_dataTypes[TP_FORMULA], true, false);
					break;

				case PREDEFSYMVAL_DATATYPE_STRING:
                    sv->simpleConstVal().copyFrom(_dataTypes[TP_STR], true, false);
					break;

				case PREDEFSYMVAL_DATATYPE_INDEXPART:
					sv->simpleConstVal().set(TP_DATA_TYPE, new ValTypeIndexpart());
					break;

				case PREDEFSYMVAL_DATATYPE_TUPLE:
                    sv->simpleConstVal().copyFrom(_dataTypes[TP_TUPLE], true, false);
					break;

				case PREDEFSYMVAL_DATATYPE_SET:
                    sv->simpleConstVal().copyFrom(_dataTypes[TP_SET_INF_SET], true, false);
					break;

				case PREDEFSYMVAL_DATATYPE_INTERVAL:
                    sv->simpleConstVal().copyFrom(_dataTypes[TP_INTERVAL], true, false);
					break;

				case PREDEFSYMVAL_DATATYPE_FUNCTION:
                    sv->simpleConstVal().copyFrom(_dataTypes[TP_FUNCTION], true, false);
					break;

				case PREDEFSYMVAL_DATATYPE_CONTAINER:
                    sv->simpleConstVal().copyFrom(_dataTypes[TP_CONTAINER], true, false);
					break;

				case PREDEFSYMVAL_DATATYPE_TYPE:
                    sv->simpleConstVal().copyFrom(_dataTypes[TP_DATA_TYPE], true, false);
					break;

				case PREDEFSYMVAL_DATATYPE_OBJTYPE:
                    sv->simpleConstVal().copyFrom(_dataTypes[TP_OBJECT_TYPE], true, false);
					break;

				// special values
				case PREDEFSYMVAL_VALUE_TRUE:
					sv->simpleConstVal().set(TP_BIN, (intType)1);
					break;

				case PREDEFSYMVAL_VALUE_FALSE:
					sv->simpleConstVal().set(TP_BIN, (intType)0);
					break;

				case PREDEFSYMVAL_VALUE_NULL:
					sv->simpleConstVal().set(TP_NULL);
					break;

				case PREDEFSYMVAL_VALUE_INFINITE:
					sv->simpleConstVal().set(TP_INFINITE, (intType)1);
					break;

				case PREDEFSYMVAL_VALUE_INVALID:
					sv->simpleConstVal().set(TP_EMPTY);
					break;

				// functions
				case PREDEFSYMVAL_FUNC_SUM:
					sv->simpleConstVal().set(TP_FUNCTION, new ValFunctionInternSum());
					break;

				case PREDEFSYMVAL_FUNC_MIN:
					sv->simpleConstVal().set(TP_FUNCTION, new ValFunctionInternMin());
					break;

				case PREDEFSYMVAL_FUNC_MAX:
					sv->simpleConstVal().set(TP_FUNCTION, new ValFunctionInternMax());
					break;

                case PREDEFSYMVAL_FUNC_AND:
                    sv->simpleConstVal().set(TP_FUNCTION, new ValFunctionInternAnd());
                    break;

                case PREDEFSYMVAL_FUNC_OR:
                    sv->simpleConstVal().set(TP_FUNCTION, new ValFunctionInternOr());
                    break;

				case PREDEFSYMVAL_FUNC_COUNT:
					sv->simpleConstVal().set(TP_FUNCTION, new ValFunctionInternCount());
					break;

				case PREDEFSYMVAL_FUNC_DEF:
					sv->simpleConstVal().set(TP_FUNCTION, new ValFunctionInternDef());
					break;

				case PREDEFSYMVAL_FUNC_VALID:
					sv->simpleConstVal().set(TP_FUNCTION, new ValFunctionInternValid());
					break;

				case PREDEFSYMVAL_FUNC_DIM:
					sv->simpleConstVal().set(TP_FUNCTION, new ValFunctionInternDim());
					break;

				case PREDEFSYMVAL_FUNC_DEFSET:
					sv->simpleConstVal().set(TP_FUNCTION, new ValFunctionInternDefset());
					break;

				case PREDEFSYMVAL_FUNC_VALIDSET:
					sv->simpleConstVal().set(TP_FUNCTION, new ValFunctionInternValidset());
					break;

				case PREDEFSYMVAL_FUNC_LEN:
					sv->simpleConstVal().set(TP_FUNCTION, new ValFunctionInternLen());
					break;

				case PREDEFSYMVAL_FUNC_RANK:
					sv->simpleConstVal().set(TP_FUNCTION, new ValFunctionInternRank());
					break;

                case PREDEFSYMVAL_FUNC_ARRAY:
                    sv->simpleConstVal().set(TP_FUNCTION, new ValFunctionInternArray());
                    break;

                case PREDEFSYMVAL_FUNC_ECHO:
					sv->simpleConstVal().set(TP_FUNCTION, new ValFunctionInternEcho());
					break;

				case PREDEFSYMVAL_FUNC_ERROR:
					sv->simpleConstVal().set(TP_FUNCTION, new ValFunctionInternError());
					break;

				case PREDEFSYMVAL_FUNC_FORMAT:
					sv->simpleConstVal().set(TP_FUNCTION, new ValFunctionInternFormat());
					break;

				case PREDEFSYMVAL_FUNC_MOD:
					sv->simpleConstVal().set(TP_FUNCTION, new ValFunctionInternMod());
					break;

				case PREDEFSYMVAL_FUNC_DIV:
					sv->simpleConstVal().set(TP_FUNCTION, new ValFunctionInternDiv());
					break;

				case PREDEFSYMVAL_FUNC_SRAND:
					sv->simpleConstVal().set(TP_FUNCTION, new ValFunctionInternSrand());
					break;

				case PREDEFSYMVAL_FUNC_RAND:
					sv->simpleConstVal().set(TP_FUNCTION, new ValFunctionInternRand());
					break;

				default:
					if (predef <= -EXTENSION_SYMBOL_BASE) {
						if (_ctrl->runExtension(this, EXT_STEP_INTERPRET_PREDEF_SYMBOLS_INIT, -(predef + EXTENSION_SYMBOL_BASE), sv) == 0)
							_ctrl->errHandler().internalError(_ctrl->printBuffer("no extension inits predef id = %d", predef));
					}
					else {
						_ctrl->errHandler().internalError(_ctrl->printBuffer("unknown predef id = %d", predef));
					}
			}

			predefMap[predef] = sv;
		}
	}

    /**
     * initialize predefined values
     * @param sy		pointer to symbol info
     * @param predefMap	map with already initialized predefines
     */
    void Interpreter::initPredefSub(SymbolInfo *sy, map<int, SymbolValue *>& predefMap)
    {
        // container value must be already initialized
        if (!predefMap.count(sy->predefCont())) {
            _ctrl->errHandler().internalError(_ctrl->printBuffer("predefined subsymbol without initialized container predef id = %d", sy->predef()));
            return;
        }

        SymbolValue *sv = predefMap[sy->predefCont()];
        CmplVal& cont = sv->simpleConstVal();
        if (cont.t != TP_CONTAINER) {
            _ctrl->errHandler().internalError(_ctrl->printBuffer("predefined subsymbol with invalid container predef id = %d", sy->predef()));
            return;
        }

        // initialize predefined value
        // there are no such standard values, only possibly in extensions
        if (sy->predef() <= -EXTENSION_SYMBOL_BASE) {
            SymbolValue *sub = cont.valContainer()->insertPredef(this, sy->defId());
            predefMap[sy->predef()] = sub;

            if (_ctrl->runExtension(this, EXT_STEP_INTERPRET_PREDEF_SYMBOLS_INIT, -(sy->predef() + EXTENSION_SYMBOL_BASE), sub) == 0)
                _ctrl->errHandler().internalError(_ctrl->printBuffer("no extension inits predefined subsymbol predef id = %d", sy->predef()));
        }
        else {
            _ctrl->errHandler().internalError(_ctrl->printBuffer("unknown predefined subsymbol predef id = %d", sy->predef()));
        }
    }

    /**
     * set special symbol definition numbers for subsymbols
     * @param sy        pointer to symbol info
     */
    void Interpreter::initSpecialSub(SymbolInfo *sy)
    {
        // special predefined subsymbols
        switch (sy->predef()) {
            case PREDEFSYMVAL_SUB_ASVAR:
                _symIdSubAsVar = sy->defId();
                break;

            case PREDEFSYMVAL_SUB_ASCON:
                _symIdSubAsCon = sy->defId();
                break;

            case PREDEFSYMVAL_SUB_ASOBJ:
                _symIdSubAsObj = sy->defId();
                break;

            case PREDEFSYMVAL_SUB_ASSTRING:
                _symIdSubAsString = sy->defId();
                break;
        }
    }



	/**
	 * execute intermediary code
	 */
	void Interpreter::execCode()
	{
		_ctrl->errHandler().setExecStep("execution");

		// run execution within initial execution context
        ExecContext ctx(this, NULL, true);
        _baseExecCtx = &ctx;

        ctx.run(0, false, NULL);

        // set model properties
        _resModel->setModelProperties(this);

        // solver selection
        _ctrl->errHandler().setExecStep("solver selection");
        _ctrl->runExtension(this, EXT_STEP_INTERPRET_SOLVERSEL, _resModel);

        // linearization or other remodeling
        _ctrl->errHandler().setExecStep("remodeling");
        _ctrl->runExtension(this, EXT_STEP_INTERPRET_REMODEL, _resModel);

        // if model type now unknown by remodeling, then set model properties anew
        if (_resModel->modelProp().modelType() == OptModel::modelTypeUnknown)
            _resModel->setModelProperties(this);

        _baseExecCtx = NULL;
    }


	/**
	 * get the syntax element for a given id
	 * @param id		id of syntax element
	 * @return			syntax element
	 */
	const SyntaxElement *Interpreter::syntaxElement(unsigned id)
	{
		if (!_syntaxElems) {
			_syntaxElems = new map<unsigned, const SyntaxElement *>();
			_syntaxStructure->fillMap(_syntaxElems);
		}

		return _syntaxElems->at(id);
	}


    /**
     * import external data values
     * @param ctx       execution context
     * @param se        id of syntax element
     * @param res		return values
     * @param symNmsName symbol name including namespace
     */
    void Interpreter::importExtern(ExecContext *ctx, unsigned se, CmplVal& res, string& symNmsName)
    {
        LockGuard<mutex> lck(ctx->needLock(), _externMtx);

        if (!_externInfo) {
            GET_DATA(ExternDataInfo, extInfo);
            if (extInfo)
                _externInfo = extInfo;
            else
                _ctrl->errHandler().internalError("no info about external data found in global data object");
        }

        int intsym = data()->globStrings()->search(symNmsName);
        ExternDataInfo::EIRecord *ei = _externInfo->getExternInfo(intsym);
        if (!ei) {
            ctx->execError(ctrl()->printBuffer("no info for external data symbol '%s' found", symNmsName.c_str()), se);
            res.unset();
        }

        unsigned runExt = 1;
        try {
            ImportExternDataInfo info(res, ei, ctx, se);
            runExt = ctrl()->runExtension(this, EXT_STEP_INTERPRET_EXTDATA_IMPORT, ei->srctype(), &info);
        }
        catch (ModuleException&) {
            throw;
        }
        catch (FatalException&) {
            throw;
        }
        catch (FileException& ex) {
            ctx->execError(ctrl()->printBuffer("error while reading value of external data symbol '%s': %s, in '%s' line %u", symNmsName.c_str(), ex.what(), ex.filename().c_str(), ex.line()), se);
            res.unset();
        }
        catch (exception& ex) {
            ctx->execError(ctrl()->printBuffer("error while reading value of external data symbol '%s': %s", symNmsName.c_str(), ex.what()), se);
            res.unset();
        }

        if (!runExt) {
            ctx->execError(ctrl()->printBuffer("unhandled type of external data source for external data symbol '%s' found", symNmsName.c_str()), se);
            res.unset();
        }
    }

}


