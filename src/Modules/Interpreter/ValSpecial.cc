
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


#include "ValSpecial.hh"
#include "ValContainer.hh"
#include "Interpreter.hh"
#include "ExecContext.hh"
#include "ValType.hh"
#include "../../Control/MainControl.hh"
#include "../../CommonData/ExtensionSteps.hh"



namespace cmpl
{

    /****** ValSpecialBase ****/

    /**
     * create the appropriate subclass object for handling a given pseudo symbol
     * @param ctx			execution context
     * @param code			code number of the pseudo symbol
     * @param se            syntax element
     * @param lval          pseudo subsymbol used as lvalue
     * @param base			base object for pseudo subsymbol
     * @return				object for pseudo symbol functionality
     */
    ValSpecialBase *ValSpecialBase::create(ExecContext *ctx, unsigned code, unsigned se, bool lval, CmplVal *base)
    {
        ValSpecialBase *v;
        switch (code) {
            case ICS_SPECIAL_ARG:           v = new ValSpecialArg(ctx, se, lval, base); break;
            case ICS_SPECIAL_THIS:          v = new ValSpecialThis(ctx, se, lval, base); break;
            case ICS_SPECIAL_CURTUPLE:      v = new ValSpecialCurTuple(ctx, se, lval, base); break;
            case ICS_SPECIAL_CURFULLTUPLE:  v = new ValSpecialCurFullTuple(ctx, se, lval, base); break;
            case ICS_SPECIAL_CURDESTNAME:   v = new ValSpecialCurDestName(ctx, se, lval, base); break;
            case ICS_SPECIAL_CURDESTTUPLE:  v = new ValSpecialCurDestTuple(ctx, se, lval, base); break;

            case ICS_SPECIAL_VARBYNAME:     v = new ValSpecialVarByName(ctx, se, lval, base); break;
            case ICS_SPECIAL_VARBYNO:       v = new ValSpecialVarByNo(ctx, se, lval, base); break;
            case ICS_SPECIAL_CONBYNAME:     v = new ValSpecialConByName(ctx, se, lval, base); break;
            case ICS_SPECIAL_CONBYNO:       v = new ValSpecialConByNo(ctx, se, lval, base); break;

            case ICS_SPECIAL_OBJECTTYPE:    v = new ValSpecialObjectType(ctx, se, lval, base); break;
            case ICS_SPECIAL_DATATYPE:      v = new ValSpecialDataType(ctx, se, lval, base); break;
            case ICS_SPECIAL_TYPEBASE:      v = new ValSpecialTypeBase(ctx, se, lval, base); break;
            case ICS_SPECIAL_TYPEPAR:       v = new ValSpecialTypePar(ctx, se, lval, base); break;
            case ICS_SPECIAL_ISREADONLY:    v = new ValSpecialIsReadOnly(ctx, se, lval, base); break;

            case ICS_SPECIAL_DESTNAME:      v = new ValSpecialDestName(ctx, se, lval, base); break;
            case ICS_SPECIAL_DESTTUPLE:     v = new ValSpecialDestTuple(ctx, se, lval, base); break;
            case ICS_SPECIAL_DESTNAMETUPLE: v = new ValSpecialDestNameTuple(ctx, se, lval, base); break;
            case ICS_SPECIAL_DESTFULLNAME:  v = new ValSpecialDestFullName(ctx, se, lval, base); break;
            case ICS_SPECIAL_FULLFORMULA:   v = new ValSpecialFullFormula(ctx, se, lval, base); break;
            case ICS_SPECIAL_FORMULAARRAY:  v = new ValSpecialFormulaArray(ctx, se, lval, base); break;

            default:
                int id = code - EXTENSION_SYMBOL_BASE;   // TODO: ist das richtig?
                v = new ValSpecialDummy(ctx, se, lval, base);

                if (id >= 0) {
                    // TODO: in Extension, die EXT_STEP_INTERPRET_PSEUDO_SYMBOL_OBJ behandelt:
                    //      Auf Vorlage von v muss neues Objekt einer von ValSpecialBase abgeleiteten Klasse erstellt werden, das die eigentliche Funktionalitaet bereitstellt
                    //      Das Vorlageobjekt in v ist freizugeben, und stattdessen das neue Objekt einzutragen
                    if (ctx->modp()->ctrl()->runExtension(ctx->modp(), EXT_STEP_INTERPRET_PSEUDO_SYMBOL_OBJ, id, &v) == 0)
                        ctx->internalError(ctx->modp()->ctrl()->printBuffer("no extension handles pseudo symbol = %d", code));
                }
                else {
                    ctx->internalError(ctx->modp()->ctrl()->printBuffer("unknown pseudo symbol = %d", code));
                }
        }

        // if lvalue not possible then return NULL as error
        if (lval && !v->canLvalue()) {
            ctx->execError(ctx->modp()->ctrl()->printBuffer("cannot write to pseudo symbol %s", v->funcName()), se);
            delete v;
            v = new ValSpecialDummy(ctx, se, lval, base);
        }

        // if base not possible then return NULL as error
        else if ((base && v->canBase() == 0) || (!base && v->canBase() == 1)) {
            ctx->execError(ctx->modp()->ctrl()->printBuffer((base ? "pseudo symbol %s cannot be used as subsymbol" : "pseudo symbol %s can only be used as subsymbol"), v->funcName()), se);
            delete v;
            v = new ValSpecialDummy(ctx, se, lval, base);
        }

        return v;
    }

    /**
     * read functionality of the pseudo symbol
     * @param res			return of result value
     */
    void ValSpecialBase::get(CmplVal& res)
    {
        _ctx->execError(_ctx->modp()->ctrl()->printBuffer("cannot read from pseudo symbol %s", funcName()), _se);
        res.set(TP_NULL);
    }

    /**
     * write functionality of the pseudo symbol
     * @param val			value to write
     * @param se            syntax element of value
     */
    void ValSpecialBase::set(CmplVal& val, unsigned se)
    {
        _ctx->execError(_ctx->modp()->ctrl()->printBuffer("cannot write to pseudo symbol %s", funcName()), _se);
    }

    /**
     * read functionality for indexed value of the pseudo symbol, only called if hasIndexedHandling()
     * @param res           return of result value
     * @param ind           index tuple or set
     * @return              false if index is invalid
     */
    bool ValSpecialBase::getIndexed(CmplVal& res, CmplVal& ind)
    {
        _ctx->execError(_ctx->modp()->ctrl()->printBuffer("cannot indexed read from pseudo symbol %s", funcName()), _se);
        return true;
    }

    /**
     * write functionality for indexed value of the pseudo symbol, only called if hasIndexedHandling()
     * @param val           return of result value
     * @param se            syntax element of value
     * @param ind           index tuple or set
     * @return              false if index is invalid
     */
    bool ValSpecialBase::setIndexed(CmplVal& val, unsigned se, CmplVal& ind)
    {
        _ctx->execError(_ctx->modp()->ctrl()->printBuffer("cannot indexed write to pseudo symbol %s", funcName()), _se);
        return true;
    }

    /**
     * get definition set for pseudo symbol, only called if hasIndexedHandling()
     * @param res           return of result set
     */
    void ValSpecialBase::getDefset(CmplVal& res)
    {
        _ctx->execError(_ctx->modp()->ctrl()->printBuffer("cannot get definition set from pseudo symbol %s", funcName()), _se);
    }



    /****** ValSpecialArg ****/

    /**
     * read functionality of the pseudo symbol
     * @param res			return of result value
     */
    void ValSpecialArg::get(CmplVal& res)
    {
        res.copyFrom(_ctx->getFctArg()->val(), true, false);
    }



    /****** ValSpecialThis ****/

    /**
     * read functionality of the pseudo symbol
     * @param res			return of result value
     */
    void ValSpecialThis::get(CmplVal& res)
    {
        ValContainer *c = _ctx->getFctThis();
        if (c) {
            res.set(TP_CONTAINER, c);
        }
        else {
            _ctx->execError("no '$this' container available", _se);
            res.set(TP_NULL);
        }
    }



    /****** ValSpecialCurTuple ****/

    /**
     * read functionality of the pseudo symbol
     * @param res			return of result value
     */
    void ValSpecialCurTuple::get(CmplVal& res)
    {
        vector<CmplValAuto> tplParts;
        unsigned c = _ctx->getIterTplPartsIter(tplParts);

        if (c)
            Tuple::constructTuple(res, tplParts);
        else
            res.set(TP_NULL);
    }



    /****** ValSpecialCurFullTuple ****/

    /**
     * read functionality of the pseudo symbol
     * @param res			return of result value
     */
    void ValSpecialCurFullTuple::get(CmplVal& res)
    {
        vector<CmplValAuto> tplParts;
        unsigned c = _ctx->getIterTplParts(tplParts);

        if (c)
            Tuple::constructTuple(res, tplParts);
        else
            res.set(TP_ITUPLE_NULL);
    }



    /****** ValSpecialCurDestName ****/

    /**
     * read functionality of the pseudo symbol
     * @param res			return of result value
     */
    void ValSpecialCurDestName::get(CmplVal& res)
    {
        intType nmp = _ctx->curNmPref();
        if (nmp >= 0)
            res.set(TP_STR, nmp);
        else
            res.set(TP_NULL);
    }

    /**
     * write functionality of the pseudo symbol
     * @param val			value to write
     * @param se            syntax element of value
     */
    void ValSpecialCurDestName::set(CmplVal& val, unsigned se)
    {
        intType nmp;
        if (val.t == TP_NULL) {
            nmp = -1;
        }
        else if (val.t == TP_STR) {
            nmp = val.v.i;
        }
        else {
            CmplVal sv;
            bool disp;
            if (val.toString(sv, typeConversionValue, _ctx->modp(), disp)) {
                sv.stringPToStr(_ctx->modp(), disp);
                nmp = sv.v.i;
            }
            else {
                _ctx->valueError("value must be a string", val, se);
                return;
            }
        }

        bool cmd;
        _ctx->curNmPref(cmd);
        if (cmd) {
            _ctx->valueError("cannot assign value within a command with own line name prefix", val, se);
        }
        else {
            _ctx->setCurNmPref(nmp, true);
        }
    }



    /****** ValSpecialCurDestTuple ****/

    /**
     * read functionality of the pseudo symbol
     * @param res			return of result value
     */
    void ValSpecialCurDestTuple::get(CmplVal& res)
    {
        vector<CmplValAuto> tplParts;
        unsigned c = _ctx->getIterTplPartsNmPref(tplParts);

        if (c)
            Tuple::constructTuple(res, tplParts);
        else
            res.set(TP_NULL);
    }



    /****** ValSpecialVarByName ****/

    /**
     * read functionality of the pseudo symbol
     * @param res			return of result value
     */
    void ValSpecialVarByName::get(CmplVal& res)
    {
        //TODO
        res.set(TP_NULL);
    }

    /**
     * read functionality for indexed value of the pseudo symbol, only called if hasIndexedHandling()
     * @param res           return of result value
     * @param ind           index tuple or set
     * @return              false if index is invalid
     */
    bool ValSpecialVarByName::getIndexed(CmplVal& res, CmplVal& ind)
    {
        //TODO
        res.set(TP_NULL);
        return true;
    }

    /**
     * get definition set for pseudo symbol, only called if hasIndexedHandling()
     * @param res           return of result set
     */
    void ValSpecialVarByName::getDefset(CmplVal& res)
    {
        //TODO
        res.set(TP_SET_EMPTY);
    }



    /****** ValSpecialVarByNo ****/

    /**
     * read functionality of the pseudo symbol
     * @param res			return of result value
     */
    void ValSpecialVarByNo::get(CmplVal& res)
    {
        //TODO
        res.set(TP_NULL);
    }

    /**
     * read functionality for indexed value of the pseudo symbol, only called if hasIndexedHandling()
     * @param res           return of result value
     * @param ind           index tuple or set
     * @return              false if index is invalid
     */
    bool ValSpecialVarByNo::getIndexed(CmplVal& res, CmplVal& ind)
    {
        //TODO
        res.set(TP_NULL);
        return true;
    }

    /**
     * get definition set for pseudo symbol, only called if hasIndexedHandling()
     * @param res           return of result set
     */
    void ValSpecialVarByNo::getDefset(CmplVal& res)
    {
        //TODO
        res.set(TP_SET_EMPTY);
    }



    /****** ValSpecialConByName ****/

    /**
     * read functionality of the pseudo symbol
     * @param res			return of result value
     */
    void ValSpecialConByName::get(CmplVal& res)
    {
        //TODO
        res.set(TP_NULL);
    }

    /**
     * read functionality for indexed value of the pseudo symbol, only called if hasIndexedHandling()
     * @param res           return of result value
     * @param ind           index tuple or set
     * @return              false if index is invalid
     */
    bool ValSpecialConByName::getIndexed(CmplVal& res, CmplVal& ind)
    {
        //TODO
        res.set(TP_NULL);
        return true;
    }

    /**
     * get definition set for pseudo symbol, only called if hasIndexedHandling()
     * @param res           return of result set
     */
    void ValSpecialConByName::getDefset(CmplVal& res)
    {
        //TODO
        res.set(TP_SET_EMPTY);
    }



    /****** ValSpecialConByNo ****/

    /**
     * read functionality of the pseudo symbol
     * @param res			return of result value
     */
    void ValSpecialConByNo::get(CmplVal& res)
    {
        //TODO
        res.set(TP_NULL);
    }

    /**
     * read functionality for indexed value of the pseudo symbol, only called if hasIndexedHandling()
     * @param res           return of result value
     * @param ind           index tuple or set
     * @return              false if index is invalid
     */
    bool ValSpecialConByNo::getIndexed(CmplVal& res, CmplVal& ind)
    {
        //TODO
        res.set(TP_NULL);
        return true;
    }

    /**
     * get definition set for pseudo symbol, only called if hasIndexedHandling()
     * @param res           return of result set
     */
    void ValSpecialConByNo::getDefset(CmplVal& res)
    {
        //TODO
        res.set(TP_SET_EMPTY);
    }



    /****** ValSpecialObjectType ****/

    /**
     * read functionality of the pseudo symbol
     * @param res			return of result value
     */
    void ValSpecialObjectType::get(CmplVal& res)
    {
        if (_base.isOptCol())
            res.set(TP_OBJECT_TYPE, VAL_OBJECT_TYPE_VAR);
        else if (_base.isOptRow())
            res.set(TP_OBJECT_TYPE, (_base.optCon()->objective() ? VAL_OBJECT_TYPE_OBJ : VAL_OBJECT_TYPE_CON));
        else
            res.set(TP_OBJECT_TYPE, VAL_OBJECT_TYPE_PAR);
    }



    /****** ValSpecialDataType ****/

    /**
     * read functionality of the pseudo symbol
     * @param res			return of result value
     */
    void ValSpecialDataType::get(CmplVal& res)
    {
        if (_base.isOptCol()) {
            res.copyFrom(_base.optVar()->dataType());
        }
        else {
            tp_e bt = (_base.isOptRow() ? TP_REAL : _base.getBaseType());
            if (_ctx->modp()->dataTypes().count(bt))
                res.copyFrom(_ctx->modp()->dataTypes()[bt]);
            else
                res.set(TP_NULL);
        }

        //TODO:
        // wenn Erweiterung wie fuer ValSpecialIsReadOnly notwendig und beschrieben:
        //      entsprechend wie fuer ValSpecialTypePar beschrieben
    }



    /****** ValSpecialTypeBase ****/

    /**
     * read functionality of the pseudo symbol
     * @param res			return of result value
     */
    void ValSpecialTypeBase::get(CmplVal& res)
    {
        tp_e bt;
        if (_base.isOptCol())
            bt = (_base.optVar()->binVar() ? TP_BIN : (_base.optVar()->intVar() ? TP_INT : TP_REAL));
        else
            bt = (_base.isOptRow() ? TP_REAL : _base.getBaseType());

        if (_ctx->modp()->dataTypes().count(bt))
            res.copyFrom(_ctx->modp()->dataTypes()[bt]);
        else
            res.set(TP_NULL);

        //TODO:
        // wenn Erweiterung wie fuer ValSpecialIsReadOnly notwendig und beschrieben:
        //      entsprechend wie fuer ValSpecialTypePar beschrieben
    }



    /****** ValSpecialTypePar ****/

    /**
     * read functionality of the pseudo symbol
     * @param res			return of result value
     */
    void ValSpecialTypePar::get(CmplVal& res)
    {
        if (_base.isOptCol()) {
            CmplVal& tpar = _base.optVar()->dataType().valType()->param();
            if (tpar) {
                res.copyFrom(tpar);
            }
            else {
                _base.optVar()->dataType().valType()->paramFromBounds(res);
                if (!res)
                    res.set(TP_NULL);
            }
        }
        else {
            res.set(TP_NULL);
        }

        //TODO
        // wenn Erweiterung wie fuer ValSpecialIsReadOnly notwendig und beschrieben:
        //     wenn auf Symbol aufgerufen:
        //          wenn assertions ueber Typ fuer Symbol: Typ aus assertions zurueck, daraus Typparameter
        //          sonst wenn trotzdem einfacher Wert in _base: Ausfuehrung wie wenn kein Symbol
        //          sonst: TP_NULL fuer nicht festgelegten Typ
    }



    /****** ValSpecialIsReadOnly ****/

    /**
     * read functionality of the pseudo symbol
     * @param res			return of result value
     */
    void ValSpecialIsReadOnly::get(CmplVal& res)
    {
        //TODO
        res.set(TP_BIN, 0);

        //TODO: allgemeine Erweiterung notwendig: wenn auf ein Symbol angewendet, dann muss auch Symbol in this gespeichert sein
        //  ValSpecialBaseSub abgeleitete Klassen brauchen Eigenschaften, ob auf Symbol anwendbar (dann braucht _base kein einfacher Wert sein!)
        //  und ob (wie hier) nur auf Symbol anwendbar (dann Fehler, wenn auf Nicht-lvalue aufgerufen)
    }



    /****** ValSpecialDestName ****/

    /**
     * read functionality of the pseudo symbol
     * @param res			return of result value
     */
    void ValSpecialDestName::get(CmplVal& res)
    {
        // if not optimization row or col then no result
        if (!_base.isOptRC()) {
            res.set(TP_EMPTY);
            return;
        }

        OptModel *resModel = _ctx->modp()->getResModel();
        ValueTreeRoot *vtr = (_base.t == TP_OPT_VAR ? &(resModel->cols()) : &(resModel->rows()));
        OptVarConBase *ov = _base.optRC();

        // if not in a value tree then there is no name
        if (!ov->isInTree()) {
            res.set(TP_NULL);
            return;
        }

        // if names are set then return these names
        res.copyFrom(ov->names(), true, false);
        if (res)
            return;

        // name is first index value in the value tree
        LockGuard<mutex> lck(_ctx->needLock(), vtr->accMtx());
        ov->getIndexOfLevel(1, res);
    }

    /**
     * write functionality of the pseudo symbol
     * @param val			value to write
     * @param se            syntax element of value
     */
    void ValSpecialDestName::set(CmplVal& val, unsigned se)
    {
        // only for optimization row or col, otherwise do nothing
        if (!_base.isOptRC())
            return;

        OptModel *resModel = _ctx->modp()->getResModel();
        ValueTreeRoot *vtr = (_base.t == TP_OPT_VAR ? &(resModel->cols()) : &(resModel->rows()));
        OptVarConBase *ov = _base.optRC();

        if (val.t == TP_STRINGP)
            val.stringPToStr(_ctx->modp());

        CmplValAuto name;
        if (val.t == TP_ARRAY) {
            // first element of array is used as name
            CmplArray *arr = val.array();
            if (arr->cnt() > 0)
                name.copyFrom(arr->at(0), true, false);

            // array stored as additional names
            ov->names().copyFrom(val, true, true);
        }
        else {
            name.copyFrom(val, true, false);
            ov->names().dispUnset();
        }

        // name must be a string
        if (name.t != TP_NULL && name.t != TP_STR) {
            CmplVal n;
            bool disp;

            if (!name.toString(n, typeConversionValue, _ctx->modp(), disp)) {
                _ctx->valueError("name must be a string value", val, se);
                name.dispSet(TP_NULL);
            }
            else {
                if (n.t == TP_STRINGP)
                    n.stringPToStr(_ctx->modp());

                name.moveFrom(n, true);
            }
        }

        if (name.t == TP_STR) {
            LockGuard<mutex> lck(_ctx->needLock(), vtr->accMtx());

            // tuple is needed to set the name
            CmplValAuto tpl;
            ov->getIndexOfLevel(2, 0, tpl);

            if (ov->setInValueTree(vtr, name, tpl) != 0) {
                _ctx->valueError("name is already used otherwise", val, se);
                name.set(TP_NULL);
            }
        }

        if (name.t == TP_NULL && ov->isInTree()) {
            LockGuard<mutex> lck(_ctx->needLock(), vtr->accMtx());
            ov->unsetInValueTree();
        }
    }

    /**
     * write functionality for indexed value of the pseudo symbol, only called if hasIndexedHandling()
     * @param val           return of result value
     * @param se            syntax element of value
     * @param ind           index tuple or set
     * @return              false if index is invalid
     */
    bool ValSpecialDestName::setIndexed(CmplVal& val, unsigned se, CmplVal& ind)
    {
        // only for optimization row or col, otherwise do nothing
        if (!_base.isOptRC())
            return true;

        OptVarConBase *ov = _base.optRC();

        if (val.t != TP_ARRAY && (ind.t == TP_SET_NULL || ind.t == TP_SET_ALL)) {
            if (ov->names().t == TP_ARRAY) {
                // keep additional names, only replace within its array the element with null tuple index
                CmplValAuto namesOrg;
                namesOrg.moveFrom(ov->names());
                set(val, se);
                ov->names().moveFrom(namesOrg);

                CmplArray *arr = ov->names().array();
                if (arr->cnt() > 0 && SetBase::minRank(arr->defset()) == 0)
                    arr->at(0)->copyFrom(val, true, true);
            }
            else {
                set(val, se);
            }

            return true;
        }

        //TODO
        _ctx->internalError("not implemented");
        return false;
    }



    /****** ValSpecialDestTuple ****/

    /**
     * read functionality of the pseudo symbol
     * @param res			return of result value
     */
    void ValSpecialDestTuple::get(CmplVal& res)
    {
        // if not optimization row or col then no result
        if (!_base.isOptRC()) {
            res.set(TP_EMPTY);
            return;
        }

        OptModel *resModel = _ctx->modp()->getResModel();
        ValueTreeRoot *vtr = (_base.t == TP_OPT_VAR ? &(resModel->cols()) : &(resModel->rows()));
        OptVarConBase *ov = _base.optRC();

        // if not in a value tree then there is no tuple
        if (!ov->isInTree()) {
            res.set(TP_NULL);
            return;
        }

        // tuple is from second index value in the value tree
        LockGuard<mutex> lck(_ctx->needLock(), vtr->accMtx());
        ov->getIndexOfLevel(2, 0, res);
    }

    /**
     * write functionality of the pseudo symbol
     * @param val			value to write
     * @param se            syntax element of value
     */
    void ValSpecialDestTuple::set(CmplVal& val, unsigned se)
    {
        // only for optimization row or col, otherwise do nothing
        if (!_base.isOptRC())
            return;

        OptModel *resModel = _ctx->modp()->getResModel();
        ValueTreeRoot *vtr = (_base.t == TP_OPT_VAR ? &(resModel->cols()) : &(resModel->rows()));
        OptVarConBase *ov = _base.optRC();

        CmplValAuto tpl;
        if (val.t == TP_EMPTY || val.t == TP_NULL) {
            tpl.set(TP_ITUPLE_NULL);
        }
        else if (!val.toIndexVal(tpl, typeConversionExact, _ctx->modp())) {
            _ctx->valueError("value must be an index tuple", val, se);
            return;
        }

        // name is needed to set the index tuple
        CmplValAuto name;
        if (!ov->isInTree()) {
            // use empty string as name (has number 0 in string store)
            name.set(TP_STR, 0);
        }

        LockGuard<mutex> lck(_ctx->needLock(), vtr->accMtx());

        if (ov->isInTree())
            ov->getIndexOfLevel(1, name);

        if (ov->setInValueTree(vtr, name, tpl) != 0) {
            _ctx->valueError("name+tuple is already used otherwise", val, se);
            ov->unsetInValueTree();
        }
    }



    /****** ValSpecialDestNameTuple ****/

    /**
     * read functionality of the pseudo symbol
     * @param res			return of result value
     */
    void ValSpecialDestNameTuple::get(CmplVal& res)
    {
        // if not optimization row or col then no result
        if (!_base.isOptRC()) {
            res.set(TP_EMPTY);
            return;
        }

        OptModel *resModel = _ctx->modp()->getResModel();
        ValueTreeRoot *vtr = (_base.t == TP_OPT_VAR ? &(resModel->cols()) : &(resModel->rows()));
        OptVarConBase *ov = _base.optRC();

        // if not in a value tree then there is no name or tuple
        if (!ov->isInTree()) {
            res.set(TP_NULL);
            return;
        }

        // get full index value in the value tree
        LockGuard<mutex> lck(_ctx->needLock(), vtr->accMtx());
        ov->getIndexOfLevel(1, 0, res);
    }

    /**
     * write functionality of the pseudo symbol
     * @param val			value to write
     * @param se            syntax element of value
     */
    void ValSpecialDestNameTuple::set(CmplVal& val, unsigned se)
    {
        // only for optimization row or col, otherwise do nothing
        if (!_base.isOptRC())
            return;

        OptModel *resModel = _ctx->modp()->getResModel();
        ValueTreeRoot *vtr = (_base.t == TP_OPT_VAR ? &(resModel->cols()) : &(resModel->rows()));
        OptVarConBase *ov = _base.optRC();

        CmplValAuto tpl;
        if (val.t == TP_EMPTY || val.t == TP_NULL) {
            tpl.set(TP_ITUPLE_NULL);
        }
        else if (!val.toIndexVal(tpl, typeConversionExact, _ctx->modp())) {
            _ctx->valueError("value must be an index tuple", val, se);
            return;
        }

        LockGuard<mutex> lck(_ctx->needLock(), vtr->accMtx());

        CmplVal tplNull(TP_ITUPLE_NULL);
        if (ov->setInValueTree(vtr, tpl, tplNull) != 0) {
            _ctx->valueError("name+tuple is already used otherwise", val, se);
            ov->unsetInValueTree();
        }
    }



    /****** ValSpecialDestFullName ****/

    /**
     * read functionality of the pseudo symbol
     * @param res			return of result value
     */
    void ValSpecialDestFullName::get(CmplVal& res)
    {
        // if not optimization row or col then no result
        if (!_base.isOptRC()) {
            res.set(TP_EMPTY);
            return;
        }

        OptModel *resModel = _ctx->modp()->getResModel();
        ValueTreeRoot *vtr = (_base.t == TP_OPT_VAR ? &(resModel->cols()) : &(resModel->rows()));
        OptVarConBase *ov = _base.optRC();

        // if not in a value tree then there is no name
        if (!ov->isInTree()) {
            res.set(TP_NULL);
            return;
        }

        ostringstream ostr;
        {
            LockGuard<mutex> lck(_ctx->needLock(), vtr->accMtx());
            ov->recOutName(ostr, _ctx->modp()->data()->globStrings(), 0, true);
        }

        string *s = new string(StringStore::asName(ostr.str(), 0, 9999));
        res.set(TP_STRINGP, s);
    }



    /****** ValSpecialFullFormula ****/

    /**
     * read functionality of the pseudo symbol
     * @param res			return of result value
     */
    void ValSpecialFullFormula::get(CmplVal& res)
    {
        //TODO
        res.set(TP_NULL);
    }

    /**
     * write functionality of the pseudo symbol
     * @param val			value to write
     * @param se            syntax element of value
     */
    void ValSpecialFullFormula::set(CmplVal& val, unsigned se)
    {
        //TODO
    }



    /****** ValSpecialFormulaArray ****/

    /**
     * read functionality of the pseudo symbol
     * @param res			return of result value
     */
    void ValSpecialFormulaArray::get(CmplVal& res)
    {
        //TODO
        res.set(TP_NULL);
    }

    /**
     * write functionality of the pseudo symbol
     * @param val			value to write
     * @param se            syntax element of value
     */
    void ValSpecialFormulaArray::set(CmplVal& val, unsigned se)
    {
        //TODO
    }

}

