
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


#include "ValueStore.hh"
#include "ValType.hh"
#include "SetUtil.hh"
#include "ExecContext.hh"
#include "Interpreter.hh"
#include "OperationBase.hh"
#include "../../CommonData/OptModel.hh"



namespace cmpl
{

    /****** ValueAssertionBase ****/

    /**
     * destructor
     */
    ValueAssertionBase::~ValueAssertionBase()
    {
        if (_nextAss)
            delete _nextAss;
    }

    /**
     * add assertion
     * @param a         assertion to add
     */
    void ValueAssertionBase::addAssertion(ValueAssertion *a)
    {
        if (_nextAss)
            _nextAss->addAssertion(a);
        else
            _nextAss = a;
    }


    /****** ValueAssertion ****/

    /**
     * copy constructor
     * @param src
     */
    ValueAssertion::ValueAssertion(ValueAssertion& src)
    {
        _objType.copyFrom(src._objType, true, false);
        _dataType.copyFrom(src._dataType, true, false);
        _defSet.copyFrom(src._defSet, true, false);

        _subSet = src._subSet;
        _syntaxElem = src._syntaxElem;

        if (src._nextAss) {
            _nextAss = new ValueAssertion(*(src._nextAss));
        }
    }

    /**
     * check assertions, reports error if assertion fails
     * @param ctx               execution context
     * @param v                 value to check
     * @param ctp               check data type and object type
     * @param cds               check array definition set
     * @param vse               syntax element of value to check / 0: in assert command itself
     */
    void ValueAssertion::checkAsssertion(ExecContext *ctx, CmplVal& v, bool ctp, bool cds, unsigned vse)
    {
        if (ctp && _dataType) {
            if (!_dataType.valType()->checkValue(v)) {
                ctx->assertionError("data type", _syntaxElem, vse);
                return;
            }
        }

        if (ctp && _objType) {
            if (!ObjectTypeUtil::checkValue(_objType.v.i, v)) {
                ctx->assertionError("object type", _syntaxElem, vse);
                return;
            }
        }

        if (cds && _defSet) {
            if (_defSet.t != TP_SET_NULL && (!_subSet || !SetBase::containNull(_defSet))) {
                ctx->assertionError("array definition set", _syntaxElem, vse);
                return;
            }
        }

        if (_nextAss)
            _nextAss->checkAsssertion(ctx, v, ctp, cds, vse);
    }


    /**
     * check assertions, reports error if assertion fails
     * @param ctx               execution context
     * @param v                 array with values to check
     * @param ctp               check data type and object type
     * @param cds               check array definition set
     * @param vse               syntax element of value to check / 0: in assert command itself
     */
    void ValueAssertion::checkAsssertion(ExecContext *ctx, CmplArray& v, bool ctp, bool cds, unsigned vse)
    {
        if (ctp && (_dataType || _objType)) {
            CmplVal *p = v.at(0);
            for (unsigned long i = 0; i < v.size(); i++, p++) {
                if (_dataType && !_dataType.valType()->checkValue(*p)) {
                    ctx->assertionError("data type", _syntaxElem, vse);
                    return;
                }

                if (_objType && !ObjectTypeUtil::checkValue(_objType.v.i, *p)) {
                    ctx->assertionError("object type", _syntaxElem, vse);
                    return;
                }
            }
        }

        if (cds && _defSet) {
            if (!SetUtil::compareEq(&_defSet, &(v.defset()))) {
                if (!_subSet || !SetUtil::isSubSet(ctx, &(v.defset()), &_defSet)) {
                    ctx->assertionError("array definition set", _syntaxElem, vse);
                    return;
                }
            }
        }

        if (_nextAss)
            _nextAss->checkAsssertion(ctx, v, ctp, cds, vse);
    }


    /**
     * set <code>_subSet</code> from <code>_dataType</code>
     */
    void ValueAssertion::setSubSet()
    {
        if (!_dataType || !_defSet) {
            _subSet = false;
        }
        else {
            CmplVal n(TP_NULL);
            _subSet = _dataType.valType()->checkValue(n);
        }
    }



	/****** ValueStore ****/

    /**
      * constructor as copy of another value (can be TP_VALUESTORE, TP_ARRAY, or ordinary value (no array or list))
      */
    ValueStore::ValueStore(CmplVal& v): ValueStore()
    {
        if (v.t == TP_VALUESTORE) {
            ValueStore *src = v.valueStore();
            if (src->_values) {
                src->_values->incRef();
                _values = src->_values;
            }

            if (src->_nextAss)
                _nextAss = new ValueAssertion(*(src->_nextAss));
        }

        else {
            CmplArray *arr;
            if (v.t == TP_ARRAY) {
                arr = v.array();
            }
            else {
                CmplVal ds(TP_SET_NULL);
                arr = new CmplArray(ds, v);
            }

            arr->incRef();
            _values = arr;
        }
    }


    /**
	 * set value from a stack value
	 * @param ctx				execution context
	 * @param sv				value to copy (can be also an array or an list, but no symbol value) (if a list it must be the top element on the value stack)
	 * @param disp				discard all previous values
     * @param srn				set row/col name for result matrix to this name
     * @param va                value, alternative to <code>sv</code>
     * @param se                syntax element, only used if <code>va</code> is given
     * @param chgInfo           track changes in this object / NULL: no change tracking
     * @param ord               use user order of definition set
     */
    void ValueStore::setValue(ExecContext *ctx, StackValue *sv, bool disp, const char *srn, CmplVal *va, unsigned se, VSChangeInfo *chgInfo, bool ord)
	{
        if (!sv && !va)
            return;

        bool dispOrg = disp;
        if (!disp && !_values) {
            disp = true;
        }
        else if (!disp && _values) {
            // if all existing values will be overwritten then disp can be used
            if (_values->isEmpty() || (_values->isScalar() && ((sv && sv->isSimple()) || (va && !va->isSpecial()))) ||
                    (((sv && sv->val().t == TP_ARRAY) || (va && va->t == TP_ARRAY)) && SetUtil::isSubSet(ctx, &(_values->defset()), (sv ? &(sv->val().array()->defset()) : &(va->array()->defset())))))
                disp = true;
        }

        copyOnWrite(disp);

        if ((sv && sv->isSimple()) || (va && !va->isSpecial())) {
            // check assertions
//                if (_constSet && _values && !_values->isScalar()) {
//                    //TODO: noch einfügen: Art der Assertion; Syntaxelement der Assertion-Definition
//                    if (sv)
//                        ctx->assertionError(sv);
//                    else
//                        ctx->assertionError(*va, se);
//                }

//				//TODO: assertions für Typ und Objekttyp

            CmplVal& v = (sv ? *(sv->simpleValue()) : *va);

            if (disp) {
                if (v.t != TP_NULL) {
                    // set value
                    if (_values && !_values->isScalar()) {
                        delete _values;
                        _values = NULL;
                    }
                    if (!_values) {
                        CmplVal ds = CmplVal(TP_SET_NULL);
                        _values = new CmplArray(ds);
                        _values->incRef();
                    }

                    _values->at(0)->copyFrom(v, true, true);
                    _values->setValidInfo((v ? TP_EMPTY : TP_SET_EMPTY));
                    _values->setObjType(v.getObjectType());

                    if (v.isOptRC() && (srn || ctx->curNmPref() > 0)) {
                        CmplVal tpl(TP_ITUPLE_NULL);
                        CmplVal symName((srn ? TP_STR : TP_EMPTY), (intType)(srn ? ctx->modp()->data()->globStrings()->search(srn) : 0));
                        setValInValueTree(ctx, &v, (sv ? sv->syntaxElem() : se), ctx->modp()->getResModel(), symName, tpl);
                    }

                    if (chgInfo) {
                        if (dispOrg)
                            chgInfo->chgFull();
                        else
                            chgInfo->chgScalar(ctx);
                    }
                }
                else if (!_values || _values->defset().t != TP_SET_EMPTY) {
                    // set to empty array
                    if (_values)
                        delete _values;

                    _values = new CmplArray();
                    _values->incRef();

                    if (chgInfo)
                        chgInfo->chgFull();
                }
			}

            else if (v.t != TP_NULL) {
                CmplVal tpl(TP_ITUPLE_NULL);
                setSingleValue(ctx, &tpl, 0, v, (sv ? sv->syntaxElem() : se), srn, '\0', chgInfo);
            }
        }

        else {
            // a list should be already be replaced by an array or tuple
            if (sv && sv->isList())
                    ctx->valueError("internal error: list value for assign", sv);

            if ((sv ? sv->val() : *va).t != TP_ARRAY) {
                if (sv)
                    ctx->valueError("internal error: value to assign must be an array", sv);
                else
                    ctx->valueError("internal error: value to assign must be an array", *va, se);
            }

            CmplArray *arr = (sv ? sv->val().array() : va->array());

            // check assertions
//                if (_constSet && _values && _values->defset() != arr->defset())	{ //TODO: muss Set-Vergleich sein, statt Pointer-Vergleich
//                    //TODO: noch einfügen: Art der Assertion; Syntaxelement der Assertion-Definition
//                    if (sv)
//                        ctx->assertionError(sv);
//                    else
//                        ctx->assertionError(*va, se);
//                }

//				//TODO: assertions für Typ und Objekttyp

            if (disp) {
                // set value as reference to given array
				if (_values) {
					delete _values;
					_values = NULL;
				}

				arr->incRef();
				_values = arr;

                if (srn || ctx->curNmPref()) {
                    OptModel *resModel = ctx->modp()->getResModel();
                    CmplVal symName((srn ? TP_STR : TP_EMPTY), (intType)(srn ? ctx->modp()->data()->globStrings()->search(srn) : 0));

                    CmplArrayIterator iter(*arr, false, ord);
                    for (iter.begin(); iter; iter++) {
                        CmplVal *v = *iter;
                        if (v->isOptRC())
                            setValInValueTree(ctx, v, (sv ? sv->syntaxElem() : se), resModel, symName, iter.curTuple());
                    }
                }

                if (chgInfo) {
                    if (dispOrg) {
                        chgInfo->chgFull();
                    }
                    else {
                        CmplArrayIterator iter(*arr);
                        for (iter.begin(); iter; iter++)
                            chgInfo->chgTpl(ctx, &(iter.curTuple()));
                    }
                }
			}

            else {
                CmplArrayIterator iter(*arr, false, ord);
                for (iter.begin(); iter; iter++) {
                    CmplVal *v = *iter;
                    setSingleValue(ctx, &(iter.curTuple()), 0, *v, (sv ? sv->syntaxElem() : se), srn, '\0', chgInfo);
                }
            }
        }
    }

    /**
     * set single value within the array
     * @param ctx				execution context
     * @param tpl               indexing tuple for the value within the array / NULL: not given
     * @param ind1              direct index+1 for the value within the array / 0: not given (at least tpl or ind1 must be given, if both given it must match)
     * @param val				value to set within the array, must be a single value (no array or list)
     * @param se                syntax element
     * @param srn				set row/col name for result matrix to this name
     * @param op                assign operation (+,-,*,/) or '\0'
     * @param chgInfo           track changes in this object / NULL: no change tracking
     */
    void ValueStore::setSingleValue(ExecContext *ctx, const CmplVal *tpl, unsigned long ind1, CmplVal& val, unsigned se, const char *srn, char op, VSChangeInfo *chgInfo)
    {
        // copy on write if neccessary
        if (!_values) {
            CmplVal ds = CmplVal(TP_SET_EMPTY);
            _values = new CmplArray(ds);
            _values->incRef();
        }
        else if (_values->refCnt() > 1) {
            copyOnWrite(false);
        }

        if (!tpl && !ind1) {
            ctx->valueError("internal error: no index given in ValueStore::setSingleValue()", val, se);
            return;
        }
        else if (ind1 && ind1 > _values->size()) {
            ctx->valueError("internal error: invalid direct index in ValueStore::setSingleValue()", val, se);
            return;
        }

        if (!op && val.t == TP_STRINGP)
            val.stringPToStr(ctx->modp());

        CmplVal& ds = _values->defset();
        if (val.isOptRC() && (srn || ctx->curNmPref())) {
            OptModel *resModel = ctx->modp()->getResModel();
            CmplVal symName((srn ? TP_STR : TP_EMPTY), (intType)(srn ? ctx->modp()->data()->globStrings()->search(srn) : 0));

            CmplValAuto tpds;
            if (!tpl)
                SetIterator::at(tpds, ds, ind1-1);

            setValInValueTree(ctx, &val, se, resModel, symName, (tpl ? *tpl : tpds));
        }

        // search tuple in the definition set of the value array of the symbol
        unsigned long ind = (ind1 ? ind1 - 1 : 0);
        bool found = (ind1 ? true : SetUtil::tupleInSet(ctx, ds, *tpl, ind));

        if (found) {
            // set value in array
            if (!op) {
                _values->at(ind)->copyFrom(val, true, true);
            }
            else {
                CmplVal org;
                org.moveFrom(_values->at(ind));
                unsigned short opc = (op=='+' ? ICS_OPER_ADD : (op=='-' ? ICS_OPER_SUB : (op=='*' ? ICS_OPER_MUL : ICS_OPER_DIV)));

                OperationBase::execBinaryOper(ctx, _values->at(ind), se, opc, false, &org, &val);
            }

            if (_values->allValid() && !(_values->at(ind)))
                _values->delValidInfo();
            if (_values->hasObjType() && (_values->at(ind)) && !(_values->at(ind)->hasObjectType(_values->objType())))
                _values->delObjType(true);

            if (chgInfo)
                chgInfo->chgTpl(ctx, tpl, false, ind1, &ds);
        }
        else {
            if (!op) {
                // grow definition set and array, and insert value
                CmplValAuto nds;
                unsigned long ii = SetUtil::addTupleToSet(ctx, nds, ds, *tpl, false, true);
                _values->insertValues(nds, ii, &val);

                if (chgInfo)
                    chgInfo->chgTpl(ctx, tpl);
            }
            else {
                 ctx->valueError("left hand side element not found", val, se);
            }
        }

    }


    /**
     * fetch and increment single int value
     * @param ctx               execution context
     * @param pre               prefix or postfix incrementation
     * @param inc               increment value
     * @param se                syntax element
     * @return                  fetched value
     */
    intType ValueStore::fetchInc(ExecContext *ctx, bool pre, int inc, unsigned se)
    {
        if (_values->defset().t != TP_SET_NULL) {
            CmplVal v(TP_ARRAY, this, false);
            ctx->valueError("increment/decrement used with array value, possible only for scalar int", v, se);
            return 0;
        }

        if (_values->at(0)->t != TP_INT) {
            ctx->valueError("increment/decrement used with non-int value, possible only for scalar int", *(_values->at(0)), se);
            return 0;
        }

        copyOnWrite(false);

        intType res;
        CmplVal *p = _values->at(0);

        if (pre) {
            p->v.i += inc;
            res = p->v.i;
        }
        else {
            res = p->v.i;
            p->v.i += inc;
        }

        return res;
    }


	/**
	 * copy values if necessary before write to the array
	 * @param disp				writing will discard all previous values
	 */
	void ValueStore::copyOnWrite(bool disp)
	{
		if (_values && _values->refCnt() > 1) {
			// copy on write
			if (disp && !_constSet) {
				_values->decRef();
				_values = NULL;
			}
            else {
                CmplArray *nv;

                if (disp)
                    nv = new CmplArray(_values->defset());
                else
                    nv = new CmplArray(_values);

                nv->incRef();
                _values->decRef();
                _values = nv;
			}
		}
	}


    /**
     * copy all data of another value store to this
     * @param src               source symbol value
     * @param assertions        copy also assertions
     */
    void ValueStore::copyAllFrom(ValueStore *src, bool assertions)
    {
        if (assertions) {
            if (_nextAss) {
                delete _nextAss;
                _nextAss = NULL;
            }

            if (src->_nextAss) {
                _nextAss = new ValueAssertion(*(src->_nextAss));
            }
        }

        if (_values && _values->decRef()) {
            delete _values;
            _values = NULL;
        }

        if (src->_values) {
            src->_values->incRef();
            _values = src->_values;
        }
    }


    /**
     * set or move value for variable or constraint within the value tree for the result matrix
     * @param ctx				execution context
     * @param v					value to set in the value tree
     * @param se				syntax element id of the value
     * @param resMatrix			result matrix
     * @param fi				first index part (can be TP_INT, TP_STR, TP_ITUPLE* or TP_LIST_TUPLE), or TP_EMPTY then <code>sc</code> holds only the remaining tuple parts up from <code>sub</code>
     * @param sc				second index part (can be TP_INT, TP_STR, TP_ITUPLE* or TP_LIST_TUPLE)
     * @param rec               recursive call after applying name prefix
     */
    void ValueStore::setValInValueTree(ExecContext *ctx, CmplVal *v, unsigned se, OptModel *resMatrix, const CmplVal& fi, const CmplVal& sc, bool rec)
    {
        if (!rec) {
            if (!v->isOptRC()) {
                ctx->valueError("Internal Error: value for setValInValueTree() is not a variable or constraint", *v, se);
                return;
            }

            if (v->optRC()->isInTree()) {
                ctx->valueError("optimization variable/constraint has had already name and tuple for result matrix", *v, se, ERROR_LVL_WARN);	//TODO: Ausgabe bisheriger Name+Tuple
            }

            intType nmp = ctx->curNmPref();
            if (nmp >= 0 && (fi.t == TP_STR || fi.t == TP_EMPTY)) {
                CmplValAuto nfi, nsc;
                if (fi && nmp > 0) {
                    string rp(ctx->modp()->data()->globStrings()->at(nmp));
                    if (ctx->modp()->nmPrefSep(true))
                        rp.append(ctx->modp()->data()->globStrings()->at(ctx->modp()->nmPrefSep(true)));
                    rp.append(fi.stringAsCharP(ctx->modp()));
                    nfi.set(TP_STR, (intType)(ctx->modp()->data()->globStrings()->storeInd(rp)));
                }
                else if (fi || nmp > 0) {
                    nfi.set(TP_STR, (fi ? fi.v.i : nmp));
                }
                else {
                    // no name, do nothing
                    return;
                }

                vector<CmplValAuto> tplParts;
                if (ctx->getIterTplPartsNmPref(tplParts)) {
                    if (sc && sc.t != TP_ITUPLE_NULL) {
                        tplParts.emplace_back();
                        tplParts.back().copyFrom(sc);
                    }
                    Tuple::constructTuple(nsc, tplParts);
                }
                else {
                    nsc.copyFrom(sc);
                }

                return setValInValueTree(ctx, v, se, resMatrix, nfi, nsc, true);
            }
        }

        ValueTreeRoot *vtr = (v->t == TP_OPT_VAR ? &(resMatrix->cols()) : &(resMatrix->rows()));
        LockGuard<mutex> lck(ctx->needLock(), vtr->accMtx());

        int err = v->optRC()->setInValueTree(vtr, fi, sc);
        if (err == 1) {
            if (v->t == TP_OPT_VAR)
                ctx->valueError("optimization variable with this name and tuple already exists", *v, se);	//TODO: Ausgabe Tuple aus fi+sc
            else
                ctx->valueError("optimization constraint with this name and tuple already exists", *v, se);	//TODO: Ausgabe Tuple aus fi+sc
        }
    }

    /**
     * set or move value for variable or constraint within the value tree for the result matrix, using only current name prefix
     * @param ctx				execution context
     * @param v					value to set in the value tree
     * @param se				syntax element id of the value
     * @param tp                additional index tuple / NULL: no one
     */
    void ValueStore::setValInValueTree(ExecContext *ctx, CmplVal *v, unsigned se, OptModel *resMatrix, const CmplVal *tp)
    {
        CmplValAuto snm(TP_EMPTY), tpl(TP_ITUPLE_NULL);
        if (tp && *tp)
            tpl.copyFrom(tp);

        if (v->isOptRC()) {
            ValueStore::setValInValueTree(ctx, v, se, resMatrix, snm, tpl);
        }
        else if (v->t == TP_ARRAY) {
            CmplVal *vs = v->array()->at(0);

            SetIterator iter(v->array()->defset());

            for (iter.begin(); iter; iter++, vs++) {
                const CmplVal& ts = iter.curTuple();

                if (tpl.t == TP_ITUPLE_NULL) {
                    if (vs->isOptRC())
                        ValueStore::setValInValueTree(ctx, vs, se, resMatrix, snm, ts);
                    else
                        setValInValueTree(ctx, vs, se, resMatrix, &ts);
                }
                else {
                    CmplValAuto nts;
                    Tuple::constructTuple(nts, tpl, ts);
                    if (vs->isOptRC())
                        ValueStore::setValInValueTree(ctx, vs, se, resMatrix, snm, nts);
                    else
                        setValInValueTree(ctx, vs, se, resMatrix, &nts);
                }
            }
        }
    }



    /****** VSChangeInfo ****/

    /**
     * get whether there is at least one changed element
     * @param allChg        return whether all elements are changed
     * @return              true if at least one element is changed
     */
    bool VSChangeInfo::hasChgElem(bool& allChg)
    {
        if (_fullChg) {
            allChg = true;
            return true;
        }

        if (_chgFlags.empty() || _chgIndFrom >= _chgIndEnd) {
            allChg = _baseVS.isNull();
            return false;
        }

        if (_chgIndFrom > 0 || _chgIndEnd < _chgFlags.size()) {
            allChg = false;
        }
        else {
            allChg = true;
            for (unsigned long i = 0; i < _chgFlags.size(); i++) {
                if (!_chgFlags[i]) {
                    allChg = false;
                    break;
                }
            }
        }

        return true;
    }

    /**
     * mark change of one element
     * @param ctx				execution context
     * @param tpl               indexing tuple for the value within the array / NULL: not given
     * @param del               element is deleted
     * @param ind1              direct index+1 for the value within the array / 0: not given (at least tpl or ind1 must be given, if both given it must match)
     * @param set               set to which ind1 belongs (must be given if ind1 is used)
     */
    void VSChangeInfo::chgTpl(ExecContext *ctx, const CmplVal *tpl, bool del, unsigned long ind1, CmplVal *set)
    {
        if (_fullChg)
            return;

        if (_baseVS.isNull()) {
            _fullChg = true;
            return;
        }

        CmplVal& ds = _baseVS.values()->defset();
        unsigned long ind;
        bool found;

        if (ind1 && set && *set == ds) {
            ind = ind1 - 1;
            found = true;
        }
        else {
            found = SetUtil::tupleInSet(ctx, ds, *tpl, ind);
        }

        if (found) {
            if (_chgFlags.size() == 0)
                _chgFlags.resize(_baseVS.values()->size(), false);

            if (!_chgFlags[ind]) {
                _chgFlags[ind] = true;
                if (ind < _chgIndFrom || (!_chgIndFrom && !_chgIndEnd))
                    _chgIndFrom = ind;
                if (ind >= _chgIndEnd)
                    _chgIndEnd = ind + 1;
            }

            if (del)
                _defsetDel = true;
        }
        else if (!del) {
            _defsetAdd = true;
        }
    }



    /****** SymbolValue ****/

    /**
     * copy contents of another symbol value to this
     * @param src               source symbol value
     * @param ref               copy value store as reference
     */
    void SymbolValue::copyAllFrom(SymbolValue *src, bool ref)
    {
        if (ref || !src->hasValueStore()) {
            copyFrom(src, true);
        }
        else {
            unset();

            bool needLockVS = (src->valueStore()->refCnt() > 1);
            LockGuard<mutex> lckV(needLockVS, &(src->valueStore()->accMtx()));

            ValueStore *v = valueStore(true);
            v->copyAllFrom(src->valueStore(), true);
        }

        _defId = src->_defId;
        _global = src->_global;
        _readOnly = src->_readOnly;
    }

}

