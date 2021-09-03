
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


#include "ValType.hh"
#include "Interval.hh"
#include "ValContainer.hh"
#include "OperationBase.hh"
#include "Interpreter.hh"
#include "SetUtil.hh"
#include "../../CommonData/OptModel.hh"
#include "../../Control/MainControl.hh"


namespace cmpl
{

	/****** ValType ****/

	/**
	 * calls the cmpl index operation and store result in execution context, if this value has a special handling for it
	 * @param ctx			execution context
	 * @param arg			pointer to argument value
	 * @return				true if this has handled the operation
	 */
	bool ValType::operIndex(ExecContext *ctx, StackValue *arg)
	{
		CmplVal *v = arg->simpleValue();
		if (!v || !v->isTuple()) {
			//TODO: v ist TP_LIST_TUPLE: dann hier in Tuple umwandeln und damit weiter
			ctx->valueError("data type parameter is not a tuple", arg);
			ctx->opResult().set(TP_DATA_TYPE, this);
			return true;
		}

		ValType *t = newParam(ctx, arg, v);
		if (t)
			ctx->opResult().set(TP_DATA_TYPE, t);
		else
			ctx->opResult().set(TP_DATA_TYPE, this);

		return true;
	}

	/**
	 * get a new data type object with applied parameters
	 * @param ctx			execution context
	 * @param arg			pointer to argument, only for error messages
	 * @param vtp			data type parameter as tuple value
	 * @return				new data type object / NULL, if this has already the given parameters
	 */
	ValType *ValType::newParam(ExecContext *ctx, StackValue *arg, CmplVal *vtp)
	{
		ctx->valueError(ctx->modp()->ctrl()->printBuffer("data type '%s' cannot have type parameters", typeName()), arg);
		return NULL;
	}

    /**
     * convert value to this data type
     * @param ctx			execution context
     * @param res			store for result value
     * @param src			source value
     * @param se			syntax element id of source value
     */
    void ValType::convertSimpleTo(ExecContext *ctx, CmplVal& res, CmplVal& src, unsigned se)
    {
        if (src.t == _baseType) {
            res.copyFrom(src, true, false);
        }
        else {
            notImplementedError(ctx, res, src, se);
            res.set(TP_SET_EMPTY);
        }
    }

    /**
     * error type conversion not implemented
     * @param ctx			execution context
     * @param res			store for result value
     * @param src			source value
     * @param se			syntax element id of source value
     */
    void ValType::notImplementedError(ExecContext *ctx, CmplVal& res, CmplVal& src, unsigned se)
    {
        //not implemented
        ostringstream o;
        o << "type conversion to type '" << typeName() << "' not implemented for value";

        ctx->valueError(o.str().c_str(), src, se);
        defValCopy(res);
    }



	/****** ValTypeUserOr ****/

    /**
     * write contents of the object to a stream
     * @param modp			calling module
     * @param mode			mode for output: 0=direct; 1=part of other value
     */
    void ValTypeUserOr::write(ostream& ostr, ModuleBase *modp, int mode) const
    {
        if (mode == 0)
            ostr << "<union data type '";

        bool start = true;
        for (const CmplVal& v : _types) {
            if (start)
                start = false;
            else
                ostr << "||";

            v.write(ostr, modp, 1);
        }

        if (_alsoInf)
            ostr << "||inf";
        if (_alsoEmpty)
            ostr << "||invalid";
        if (_alsoNull)
            ostr << "||null";

        if (mode == 0)
            ostr << "'>";
    }


	/**
	 * get a new data type object with applied parameters
	 * @param ctx			execution context
	 * @param arg			pointer to argument, only for error messages
	 * @param vtp			data type parameter as tuple value
	 * @return				new data type object / NULL, if this has already the given parameters
	 */
	ValType *ValTypeUserOr::newParam(ExecContext *ctx, StackValue *arg, CmplVal *vtp)
	{
		// construct new data type object by applying type parameter to every subtype
		ValTypeUserOr *res = new ValTypeUserOr();
		bool chg = false;
		bool start = true;

        res->_alsoInf = _alsoInf;
        res->_alsoEmpty = _alsoEmpty;
        res->_alsoNull = _alsoNull;

		for (vector<CmplVal>::iterator it = _types.begin(); it != _types.end(); it++) {
			ValType *t = it->valType()->newParam(ctx, arg, vtp);
			if (t) {
				res->_types.push_back(CmplVal(TP_DATA_TYPE, t));
				chg = true;
                if (start && !_alsoEmpty)
					res->_defVal.copyFrom(it->valType()->defVal(), true, true);
			}
			else {
				res->_types.push_back(CmplVal(TP_DATA_TYPE, it->valType()));
			}
			start = false;
		}

		if (chg) {
			res->_param.copyFrom(*vtp, true, true);
		}
		else {
			// no subtype is changed, discard new data type object and use this instead
			delete res;
			res = NULL;
		}

		return res;
	}


    /**
     * convert simple value (no array) to this data type
     * @param ctx			execution context
     * @param res			store for result value
     * @param src			source value
     * @param se			syntax element id of source value
     */
    void ValTypeUserOr::convertSimpleTo(ExecContext *ctx, CmplVal& res, CmplVal& src, unsigned se)
    {
        for (TypeConversionLevel tcl = typeConversionNoConv; tcl <= typeConversionAll; tcl = (TypeConversionLevel)(tcl + 1)) {
            if (tryConvertSimpleTo(ctx, res, src, tcl))
                return;
        }

        ctx->valueError(ctx->modp()->ctrl()->printBuffer("value is invalid for converting to '%s'", typeName()), src, se);
        res.copyFrom(_defVal, true, false);
    }

    /**
     * try to convert simple value (no array) to this data type
     * @param ctx			execution context
     * @param res			store for result value
     * @param src			source value
     * @param tcl			conversion level
     * @return              true if conversion is successful
     */
    bool ValTypeUserOr::tryConvertSimpleTo(ExecContext *ctx, CmplVal& res, CmplVal& src, TypeConversionLevel tcl) const
    {
        if (src.t == TP_INFINITE && _alsoInf || src.t == TP_EMPTY && _alsoEmpty || src.t == TP_NULL && _alsoNull) {
            res.copyFrom(src, true, false);
            return true;
        }

        for (const CmplVal& t : _types) {
            if (t.valType()->tryConvertSimpleTo(ctx, res, src, tcl))
                return true;
        }

        return false;
    }


    /**
     * check whether value has this data type
     * @param v             value to check
     * @return              true if value has this data type
     */
    bool ValTypeUserOr::checkValue(CmplVal& v) const
    {
        if (v) {
            for (const CmplVal& t : _types) {
                if (t.valType()->checkValue(v))
                    return true;
            }

            if ((v.t == TP_INFINITE && _alsoInf) || (v.t == TP_NULL && _alsoNull))
                return true;
        }
        else if (_alsoEmpty) {
            return true;
        }

        return false;
    }


    /**
     * construct data type by or-concatenation of two arguments
     * @param res           store for result value
     * @param a1            argument one (must be TP_DATA_TYPE)
     * @param a2            argument two
     * @return              true if result data type is successfully constructed
     */
    bool ValTypeUserOr::construct(CmplVal *res, CmplVal *a1, CmplVal *a2)
    {
        if (a1->t != TP_DATA_TYPE || (a2->t != TP_DATA_TYPE && a2->t != TP_INFINITE && a2->t != TP_EMPTY && a2->t != TP_NULL))
            return false;

        ValType *vt = (ValType *)(a1->v.vp);
        ValTypeUserOr *vuo = dynamic_cast<ValTypeUserOr*>(vt);

        // check if a1 is already the result
        if (vuo && a2->t != TP_DATA_TYPE) {
            if ((a2->t == TP_INFINITE && vuo->_alsoInf) || (a2->t == TP_EMPTY && vuo->_alsoEmpty) || (a2->t == TP_NULL && vuo->_alsoNull)) {
                res->copyFrom(a1, true, false);
                return true;
            }
        }

        // construct result from a1
        ValTypeUserOr *vr = new ValTypeUserOr();
        res->set(TP_DATA_TYPE, vr);

        if (vuo) {
            for (CmplVal& v : vuo->_types)
                vr->_types.push_back(v);

            vr->_alsoInf = vuo->_alsoInf;
            vr->_alsoEmpty = vuo->_alsoEmpty;
            vr->_alsoNull = vuo->_alsoNull;
        }
        else {
            vr->_types.push_back(CmplVal(TP_DATA_TYPE, vt, true));
        }

        vr->copyBaseFrom(vt);

        // add a2 to result
        if (a2->t == TP_DATA_TYPE) {
            ValType *vt2 = (ValType *)(a2->v.vp);
            ValTypeUserOr *vuo2 = dynamic_cast<ValTypeUserOr*>(vt2);

            if (vuo2) {
                for (CmplVal& v : vuo2->_types)
                    vr->_types.push_back(v);

                vr->_alsoInf |= vuo2->_alsoInf;
                vr->_alsoEmpty |= vuo2->_alsoEmpty;
                vr->_alsoNull |= vuo2->_alsoNull;
            }
            else {
                vr->_types.push_back(CmplVal(TP_DATA_TYPE, vt2, true));
            }
        }
        else {
            if (a2->t == TP_INFINITE)
                vr->_alsoInf = true;
            else if (a2->t == TP_EMPTY)
                vr->_alsoEmpty = true;
            else if (a2->t == TP_NULL)
                vr->_alsoNull = true;
        }

        if (vr->_alsoEmpty)
            vr->_defVal.dispUnset();

        return true;
    }



    /****** ValTypeInternal ****/

    /**
     * convert value to this data type
     * @param ctx			execution context
     * @param res			store for result value
     * @param src			source value
     * @param se			syntax element id of source value
     */
    void ValTypeInternal::convertSimpleTo(ExecContext *ctx, CmplVal& res, CmplVal& src, unsigned se)
    {
        if (!tryConvertSimpleTo(ctx, res, src, typeConversionAll)) {
            ctx->valueError(ctx->modp()->ctrl()->printBuffer("value is invalid for converting to '%s'", typeName()), src, se);
            defValCopy(res);
        }
    }



	/****** ValTypeReal ****/

	/**
	 * get a new data type object with applied parameters
	 * @param ctx			execution context
	 * @param arg			pointer to argument, only for error messages
	 * @param vtp			data type parameter as tuple value
	 * @return				new data type object / NULL, if this has already the given parameters
	 */
	ValType *ValTypeReal::newParam(ExecContext *ctx, StackValue *arg, CmplVal *vtp)
	{
		CmplVal *par = (vtp->t == TP_TUPLE && vtp->tuple()->rank() == 1 ? vtp->tuple()->at(0) : NULL);
		if (!par || !par->isInterval()) {
			ctx->valueError(ctx->modp()->ctrl()->printBuffer("data type parameter for '%s' must be a tuple of a single interval, given parameter is invalid", typeName()), arg);
			return NULL;
		}

		bool hasLowBound = Interval::hasLowBound(*par);
		bool hasUppBound = Interval::hasUppBound(*par);

		realType lowBound = (hasLowBound ? Interval::lowBoundReal(*par) : 0.0);
		realType uppBound = (hasUppBound ? Interval::uppBoundReal(*par) : 0.0);

        if (_param && hasLowBound == _hasLowBound && hasUppBound == _hasUppBound && (!hasLowBound || lowBound == _lowBound) && (!hasUppBound || uppBound == _uppBound))
			return NULL;

		ValTypeReal *res = new ValTypeReal();
		res->_param.copyFrom(*vtp, true, true);

		res->_hasLowBound = hasLowBound;
		res->_hasUppBound = hasUppBound;
		res->_lowBound = lowBound;
		res->_uppBound = uppBound;

		if (hasLowBound && lowBound > 0.0)
			res->_defVal.set(TP_REAL, lowBound);

		return res;
	}

    /**
     * set info about base type and bounds for this data type within an optimization variable
     * @param v				optimization variable
     * @return				false if this data type is not suitable for optimization variables
     */
    bool ValTypeReal::setForOptVar(OptVar *v)
    {
        v->setIntVar(false);

        CmplVal& vlb = v->lowBound();
        if (_hasLowBound)
            vlb.set(TP_REAL, _lowBound);
        else
            vlb.unset();

        CmplVal& vub = v->uppBound();
        if (_hasUppBound)
            vub.set(TP_REAL, _uppBound);
        else
            vub.unset();

        return true;
    }

    /**
     * try to convert simple value (no array) to this data type
     * @param ctx			execution context
     * @param res			store for result value
     * @param src			source value
     * @param tcl			conversion level
     * @return              true if conversion is successful
     */
    bool ValTypeReal::tryConvertSimpleTo(ExecContext *ctx, CmplVal& res, CmplVal& src, TypeConversionLevel tcl) const
    {
        realType r;
        if (!src.toReal(r, tcl, ctx->modp()))
            return false;

        if ((_hasLowBound && _lowBound > r) || (_hasUppBound && _uppBound < r))
            return false;

        res.set(TP_REAL, r);
        return true;
    }

    /**
     * check whether value has this data type
     * @param v             value to check
     * @return              true if value has this data type
     */
    bool ValTypeReal::checkValue(CmplVal& v) const
    {
        if (v.t == TP_OPT_VAR) {
            OptVar *o = v.optVar();
            if (!o->intVar()) {
                CmplVal& lb = o->lowBound();
                CmplVal& ub = o->uppBound();

                if ((!_hasLowBound || (lb && _lowBound <= (lb.t == TP_REAL ? lb.v.r : (realType)lb.v.i))) && (!_hasUppBound || (ub && _uppBound >= (ub.t == TP_REAL ? ub.v.r : (realType)ub.v.i))))
                    return true;
            }
        }
        else {
            if (v.t == TP_REAL && (!_hasLowBound || _lowBound <= v.v.r) && (!_hasUppBound || _uppBound >= v.v.r))
                return true;
        }

        return false;
    }

    /**
     * make type parameter from bounds
     * @param param         return of resulting type parameter (TP_TUPLE* or TP_EMPTY)
     */
    void ValTypeReal::paramFromBounds(CmplVal& param) const
    {
        CmplVal lb, ub;
        if (_hasLowBound)
            lb.set(TP_REAL, _lowBound);
        else
            lb.set(TP_INFINITE, -1);

        if (_hasUppBound)
            ub.set(TP_REAL, _uppBound);
        else
            ub.set(TP_INFINITE, 1);

        CmplValAuto iv;
        Interval::construct(iv, lb, ub);

        CmplVal nt(TP_ITUPLE_NULL);
        Tuple::constructTuple(param, iv, nt);
    }



	/****** ValTypeInt ****/

	/**
	 * get a new data type object with applied parameters
	 * @param ctx			execution context
	 * @param arg			pointer to argument, only for error messages
	 * @param vtp			data type parameter as tuple value
	 * @return				new data type object / NULL, if this has already the given parameters
	 */
	ValType *ValTypeInt::newParam(ExecContext *ctx, StackValue *arg, CmplVal *vtp)
	{
		CmplVal *par = (vtp->t == TP_TUPLE && vtp->tuple()->rank() == 1 ? vtp->tuple()->at(0) : NULL);
		if (!par || !par->isInterval()) {
			ctx->valueError(ctx->modp()->ctrl()->printBuffer("data type parameter for '%s' must be a tuple of a single interval, given parameter is invalid", typeName()), arg);
			return NULL;
		}

		bool hasLowBound = Interval::hasLowBound(*par);
		bool hasUppBound = Interval::hasUppBound(*par);

		intType lowBound = (hasLowBound ? Interval::lowBoundInt(*par) : 0);
		intType uppBound = (hasUppBound ? Interval::uppBoundInt(*par) : 0);

        if (_param && hasLowBound == _hasLowBound && hasUppBound == _hasUppBound && (!hasLowBound || lowBound == _lowBound) && (!hasUppBound || uppBound == _uppBound))
			return NULL;

		ValTypeInt *res = new ValTypeInt();
		res->_param.copyFrom(*vtp, true, true);

		res->_hasLowBound = hasLowBound;
		res->_hasUppBound = hasUppBound;
		res->_lowBound = lowBound;
		res->_uppBound = uppBound;

		if (hasLowBound && lowBound > 0)
			res->_defVal.set(TP_INT, lowBound);

		return res;
	}

    /**
     * set info about base type and bounds for this data type within an optimization variable
     * @param v				optimization variable
     * @return				false if this data type is not suitable for optimization variables
     */
    bool ValTypeInt::setForOptVar(OptVar *v)
    {
        v->setIntVar(true);

        CmplVal& vlb = v->lowBound();
        if (_hasLowBound)
            vlb.set(TP_INT, _lowBound);
        else
            vlb.unset();

        CmplVal& vub = v->uppBound();
        if (_hasUppBound)
            vub.set(TP_INT, _uppBound);
        else
            vub.unset();

        return true;
    }

    /**
     * try to convert simple value (no array) to this data type
     * @param ctx			execution context
     * @param res			store for result value
     * @param src			source value
     * @param tcl			conversion level
     * @return              true if conversion is successful
     */
    bool ValTypeInt::tryConvertSimpleTo(ExecContext *ctx, CmplVal& res, CmplVal& src, TypeConversionLevel tcl) const
    {
        intType i;
        if (!src.toInt(i, tcl, ctx->modp()))
            return false;

        if ((_hasLowBound && _lowBound > i) || (_hasUppBound && _uppBound < i))
            return false;

        res.set(TP_INT, i);
        return true;
    }

    /**
     * check whether value has this data type
     * @param v             value to check
     * @return              true if value has this data type
     */
    bool ValTypeInt::checkValue(CmplVal& v) const
    {
        if (v.t == TP_OPT_VAR) {
            OptVar *o = v.optVar();
            if (o->intVar()) {
                CmplVal& lb = o->lowBound();
                CmplVal& ub = o->uppBound();

                if ((!_hasLowBound || (lb && _lowBound <= (lb.t == TP_REAL ? (intType)lb.v.r : lb.v.i))) && (!_hasUppBound || (ub && _uppBound >= (ub.t == TP_REAL ? (intType)ub.v.r : ub.v.i))))
                    return true;
            }
        }
        else {
            if ((v.t == TP_INT || v.t == TP_BIN) && (!_hasLowBound || _lowBound <= v.v.i) && (!_hasUppBound || _uppBound >= v.v.i))
                return true;
        }

        return false;
    }

    /**
     * make type parameter from bounds
     * @param param         return of resulting type parameter (TP_TUPLE* or TP_EMPTY)
     */
    void ValTypeInt::paramFromBounds(CmplVal& param) const
    {
        CmplVal lb, ub;
        if (_hasLowBound)
            lb.set(TP_INT, _lowBound);
        else
            lb.set(TP_INFINITE, -1);

        if (_hasUppBound)
            ub.set(TP_INT, _uppBound);
        else
            ub.set(TP_INFINITE, 1);

        CmplValAuto iv;
        Interval::construct(iv, lb, ub);

        CmplVal nt(TP_ITUPLE_NULL);
        Tuple::constructTuple(param, iv, nt);
    }



	/****** ValTypeBin ****/

	/**
	 * get a new data type object with applied parameters
	 * @param ctx			execution context
	 * @param arg			pointer to argument, only for error messages
	 * @param vtp			data type parameter as tuple value
	 * @return				new data type object / NULL, if this has already the given parameters
	 */
	ValType *ValTypeBin::newParam(ExecContext *ctx, StackValue *arg, CmplVal *vtp)
	{
		ctx->valueError(ctx->modp()->ctrl()->printBuffer("data type '%s' cannot have type parameters", typeName()), arg);
		return NULL;
	}

    /**
     * try to convert simple value (no array) to this data type
     * @param ctx			execution context
     * @param res			store for result value
     * @param src			source value
     * @param tcl			conversion level
     * @return              true if conversion is successful
     */
    bool ValTypeBin::tryConvertSimpleTo(ExecContext *ctx, CmplVal& res, CmplVal& src, TypeConversionLevel tcl) const
    {
        bool b;
        if (!src.toBool(b, tcl, ctx->modp()))
            return false;

        res.set(TP_BIN, b);
        return true;
    }



    /****** ValTypeNumeric ****/

    /**
     * convert simple value (no array) to this data type
     * @param ctx			execution context
     * @param res			store for result value
     * @param src			source value
     * @param se			syntax element id of source value
     */
    void ValTypeNumeric::convertSimpleTo(ExecContext *ctx, CmplVal& res, CmplVal& src, unsigned se)
    {
        for (TypeConversionLevel tcl = typeConversionNoConv; tcl <= typeConversionAll; tcl = (TypeConversionLevel)(tcl + 1)) {
            if (tryConvertSimpleTo(ctx, res, src, tcl)) {
                if (checkValue(res))
                    return;
                else
                    break;
            }
        }

        ctx->valueError(ctx->modp()->ctrl()->printBuffer("value is invalid for converting to '%s'", typeName()), src, se);
        res.copyFrom(_defVal, true, false);
    }

    /**
     * try to convert simple value (no array) to this data type
     * @param ctx			execution context
     * @param res			store for result value
     * @param src			source value
     * @param tcl			conversion level
     * @return              true if conversion is successful
     */
    bool ValTypeNumeric::tryConvertSimpleTo(ExecContext *ctx, CmplVal& res, CmplVal& src, TypeConversionLevel tcl) const
    {
        return src.toNumber(res, tcl, ctx->modp());
    }

    /**
     * check whether value has this data type
     * @param v             value to check
     * @return              true if value has this data type
     */
    bool ValTypeNumeric::checkValue(CmplVal& v) const
    {
        if (v.t == TP_OPT_VAR) {
            OptVar *o = v.optVar();
            CmplVal& lb = o->lowBound();
            CmplVal& ub = o->uppBound();

            if ((!_hasLowBound || (lb && _lowBound <= (lb.t == TP_REAL ? lb.v.r : (realType)lb.v.i))) && (!_hasUppBound || (ub && _uppBound >= (ub.t == TP_REAL ? ub.v.r : (realType)ub.v.i))))
                return true;
        }
        else {
            if (v.t == TP_REAL) {
                if ((!_hasLowBound || _lowBound <= v.v.r) && (!_hasUppBound || _uppBound >= v.v.r))
                    return true;
            }
            else if (v.t == TP_INT || v.t == TP_BIN) {
                if ((!_hasLowBound || (intType)_lowBound <= v.v.i) && (!_hasUppBound || (intType)_uppBound >= v.v.i))
                    return true;
            }
            else if (v.t == TP_INFINITE) {
                if ((!_hasLowBound && v.v.i < 0) || (!_hasUppBound && v.v.i > 0))
                    return true;
            }
        }

        return false;
    }



    /****** ValTypeString ****/

    /**
     * try to convert simple value (no array) to this data type
     * @param ctx			execution context
     * @param res			store for result value
     * @param src			source value
     * @param tcl			conversion level
     * @return              true if conversion is successful
     */
    bool ValTypeString::tryConvertSimpleTo(ExecContext *ctx, CmplVal& res, CmplVal& src, TypeConversionLevel tcl) const
    {
        CmplValAuto t;
        bool ccs;
        if ((ccs = ctx->checkContainerConvSpecial(t, src, TP_STR))) {
            if (t.isScalarString()) {
                res.moveFrom(t, true);
                return true;
            }
        }

        CmplVal& s = (ccs ? t : src);
        bool disp;
        if (!s.toString(res, tcl, ctx->modp(), disp))
            return false;

        if (res.useStringP() && !disp) {
            // string must be copied
            string *s = new string(*(res.v.s));
            res.v.s = s;
        }

        return true;
    }



    /****** ValTypeIndexpart ****/

    /**
     * convert simple value (no array) to this data type
     * @param ctx			execution context
     * @param res			store for result value
     * @param src			source value
     * @param se			syntax element id of source value
     */
    void ValTypeIndexpart::convertSimpleTo(ExecContext *ctx, CmplVal& res, CmplVal& src, unsigned se)
    {
        for (TypeConversionLevel tcl = typeConversionNoConv; tcl <= typeConversionAll; tcl = (TypeConversionLevel)(tcl + 1)) {
            if (tryConvertSimpleTo(ctx, res, src, tcl))
                return;
        }

        ctx->valueError(ctx->modp()->ctrl()->printBuffer("value is invalid for converting to '%s'", typeName()), src, se);
        res.copyFrom(_defVal, true, false);
    }

    /**
     * try to convert simple value (no array) to this data type
     * @param ctx			execution context
     * @param res			store for result value
     * @param src			source value
     * @param tcl			conversion level
     * @return              true if conversion is successful
     */
    bool ValTypeIndexpart::tryConvertSimpleTo(ExecContext *ctx, CmplVal& res, CmplVal& src, TypeConversionLevel tcl) const
    {
        intType i;
        if (src.toInt(i, tcl, ctx->modp())) {
            res.set(TP_INT, i);
            return true;
        }

        bool disp;
        if (!src.toString(res, tcl, ctx->modp(), disp))
            return false;

        if (res.useStringP()) {
            // string must be converted to TP_STR
            unsigned i = ctx->modp()->data()->globStrings()->storeInd(*(res.v.s));
            if (disp)
                res.dispose();

            res.set(TP_STR, (intType)i);
        }

        return true;
    }



	/****** ValTypeTuple ****/

	/**
	 * get a new data type object with applied parameters
	 * @param ctx			execution context
	 * @param arg			pointer to argument, only for error messages
	 * @param vtp			data type parameter as tuple value
	 * @return				new data type object / NULL, if this has already the given parameters
	 */
	ValType *ValTypeTuple::newParam(ExecContext *ctx, StackValue *arg, CmplVal *vtp)
	{
		intType p;
		bool hasP = false;

		if (vtp->isTuple()) {
			if (vtp->t == TP_ITUPLE_1INT) {
				p = vtp->v.i;
				hasP = true;
			}
			else if (vtp->useValP() && vtp->tuple()->rank() == 1) {
				hasP = vtp->tuple()->at(0)->toInt(p, typeConversionValue, ctx->modp());
			}
		}

		if (!hasP || p < 0) {
			ctx->valueError(ctx->modp()->ctrl()->printBuffer("data type parameter for '%s' must be a tuple of a single non-negative int value, given parameter is invalid", typeName()), arg);
			return NULL;
		}

        if (_param && _hasFixRank && _fixRank == p)
			return NULL;

		ValTypeTuple *res = newValTypeObject();
		res->_param.copyFrom(*vtp, true, true);

		res->_hasFixRank = true;
		res->_fixRank = p;

		return res;
	}

    /**
     * make type parameter from bounds
     * @param param         return of resulting type parameter (TP_TUPLE* or TP_EMPTY)
     */
    void ValTypeTuple::paramFromBounds(CmplVal& param) const
    {
        if (_hasFixRank)
            param.dispSet(TP_ITUPLE_1INT, (intType)_fixRank);
        else
            param.dispUnset();
    }



    /****** ValTypeSet ****/

    /**
     * convert value to this data type
     * @param ctx			execution context
     * @param res			store for result value
     * @param src			source value
     */
    void ValTypeSet::convertTo(ExecContext *ctx, CmplVal& res, StackValue *src)
    {
        callForArrayAggr(ctx, res, src);

        CmplVal rc;
        if (SetUtil::canonicalSet(rc, res, false))
            res.moveFrom(rc, true);
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
    bool ValTypeSet::operCallSimple(ExecContext *ctx, CmplVal& res, CmplVal& src, bool aggr, unsigned se, void *info)
    {
        if (!res || res.t == TP_NULL) {
            convertSimpleTo(ctx, res, src, se);
        }
        else {
            CmplValAuto a2;
            convertSimpleTo(ctx, a2, src, se);
            if (a2.t != TP_SET_EMPTY) {
                CmplValAuto a1;
                a1.moveFrom(res, false);
                SetUtil::addSet(ctx, &res, se, &a1, &a2, true, false);
            }
        }

        return false;
    }

    /**
     * try to convert simple value (no array) to this data type
     * @param ctx			execution context
     * @param res			store for result value
     * @param src			source value
     * @param tcl			conversion level
     * @return              true if conversion is successful
     */
    bool ValTypeSet::tryConvertSimpleTo(ExecContext *ctx, CmplVal& res, CmplVal& src, TypeConversionLevel tcl) const
    {
        if (src.t == TP_NULL || src.t == TP_EMPTY) {
            res.set(TP_SET_EMPTY);
            return true;
        }
        else if (!SetUtil::convertToSetOrTuple(ctx, res, src, tcl, false)) {
            return false;
        }

        if (_hasFixRank && SetBase::rank(res) != _fixRank)
            res.dispSet(TP_SET_EMPTY);

        return true;
    }

    /**
     * check whether value has this data type
     * @param v             value to check
     * @return              true if value has this data type
     */
    bool ValTypeSet::checkValue(CmplVal& v) const
    {
        if (v.isSet()) {
            if (!_hasFixRank || (SetBase::minRank(v) == _fixRank && SetBase::oneRank(v)))
                return true;
        }

        // accept also interval as set
        if (v.isInterval()) {
            if (!_hasFixRank || _fixRank == 1)
                return true;
        }

        // accept also tuple as set if convertible in set
        if (v.isTuple()) {
            if (v.t == TP_ITUPLE_NULL) {
                if (!_hasFixRank || _fixRank == 0)
                    return true;
            }
            else if (!v.isITuple()) {
                bool isSet = false;
                bool oneRank = true;
                unsigned setrank = 0;

                unsigned rank = Tuple::rank(v);
                const CmplVal *p = Tuple::at(v, 0);
                for (unsigned r = 0; r < rank; r++, p++) {
                    if (p->isSet()) {
                        isSet = true;
                        if (oneRank) {
                            if (SetBase::oneRank(*p))
                                setrank += SetBase::minRank(*p);
                            else
                                oneRank = false;
                        }
                    }
                    else if (p->isInterval()) {
                        isSet = true;
                        setrank++;
                    }
                    else if (p->isScalarIndex()) {
                        setrank++;
                    }
                    else {
                        return false;
                    }
                }

                if (isSet && (!_hasFixRank || (oneRank && _fixRank == setrank)))
                    return true;
            }
        }

        return false;
    }



    /****** ValTypeContainer ****/

    /**
     * constructor
     */
    ValTypeContainer::ValTypeContainer()
    {
        _defVal.set(TP_CONTAINER, new ValContainer());
        _baseType = TP_CONTAINER;
    }


    /**
     * get default value for the type as copy if needed
     * @param res           return of result value
     */
    void ValTypeContainer::defValCopy(CmplVal& res) const
    {
        // give a new container object instead of _defVal, because the returned container object will be modified
        res.set(TP_CONTAINER, new ValContainer());
    }

    /**
     * try to convert simple value (no array) to this data type
     * @param ctx			execution context
     * @param res			store for result value
     * @param src			source value
     * @param tcl			conversion level
     * @return              true if conversion is successful
     */
    bool ValTypeContainer::tryConvertSimpleTo(ExecContext *ctx, CmplVal& res, CmplVal& src, TypeConversionLevel tcl) const
    {
        if (src.t == TP_CONTAINER)
            res.copyFrom(src, true, false);
        else if (src.t == TP_EMPTY || src.t == TP_NULL)
            defValCopy(res);
        else
            return false;

        return true;
    }



    /****** ValTypeType ****/

    /**
     * try to convert simple value (no array) to this data type
     * @param ctx			execution context
     * @param res			store for result value
     * @param src			source value
     * @param tcl			conversion level
     * @return              true if conversion is successful
     */
    bool ValTypeType::tryConvertSimpleTo(ExecContext *ctx, CmplVal& res, CmplVal& src, TypeConversionLevel tcl) const
    {
        if (src.t != TP_DATA_TYPE)
            return false;

        res.copyFrom(src, true, false);
        return true;
    }



    /****** ObjectTypeUtil ****/

    /**
     * convert to object of given type or create object
     * @param ctx			execution context
     * @param res			store for result value
     * @param objType		object type (one of VAL_OBJECT_TYPE_*)
     * @param arg			argument value / NULL: no argument
     * @param maSe			syntax element id for new matrix element object
     * @param se			syntax element id, only used if arg is not given
     * @param ord           use user order if exists
     */
    void ObjectTypeUtil::convertTo(ExecContext *ctx, CmplVal& res, intType objType, StackValue *arg, unsigned maSe, unsigned se, bool ord)
    {
        if (arg) {
            CmplVal *s = arg->simpleValue();
            if (s) {
                convertTo(ctx, res, objType, s, maSe, se);
            }
            else if (arg->val().t == TP_ARRAY) {
                if (arg->val().array()->objType(true) == objType) {
                    res.copyFrom(arg->val(), true, false);
                }
                else {
                    CmplArray *arr = new CmplArray(arg->val().array()->defset());
                    arr->copyValidInfo(arg->val().array());

                    if (!ord || !SetBase::hasUserOrder(arr->defset())) {
                        CmplVal *r = arr->at(0);
                        s = arg->val().array()->at(0);

                        for (unsigned long i = 0; i < arr->size(); i++, r++, s++)
                            convertTo(ctx, *r, objType, s, maSe, se);
                            //TODO: durch ctx->checkContainerConvSpecial() kann das hier gelieferte r wiederum ein TP_ARRAY sein, muss dieser Fall verboten oder besonders behandelt werden?
                    }
                    else {
                        CmplArrayIterator itersrc(*(arg->val().array()), false, true);
                        CmplArrayIterator iterdst(*arr, false, true);

                        for (itersrc.begin(), iterdst.begin(); itersrc; itersrc++, iterdst++) {
                            CmplVal *vs = *itersrc;
                            CmplVal *vd = *iterdst;
                            convertTo(ctx, *vd, objType, vs, maSe, se);
                            //TODO: durch ctx->checkContainerConvSpecial() kann das hier gelieferte r wiederum ein TP_ARRAY sein, muss dieser Fall verboten oder besonders behandelt werden?
                        }
                    }

                    arr->setObjType(objType);
                    res.set(TP_ARRAY, arr);
                }
            }
            else {
                ctx->valueError("invalid argument for object type conversion or creation", arg);
                res.unset();
            }
        }
        else {
            if (objType == VAL_OBJECT_TYPE_VAR) {
                createDefaultVar(ctx, res, maSe, se);
            }
            else {
                res.unset();
                if (objType == VAL_OBJECT_TYPE_CON || objType == VAL_OBJECT_TYPE_OBJ)
                    ctx->valueError("empty 'obj' or 'con' not allowed", res, se);
            }
        }
    }

    /**
     * convert to object of given type or create object
     * @param ctx			execution context
     * @param res			store for result value
     * @param objType		object type (one of VAL_OBJECT_TYPE_*)
     * @param s				source value (must not be an array)
     * @param maSe			syntax element id for new matrix element object
     * @param se			syntax element id, only used if arg is not given
     */
    void ObjectTypeUtil::convertTo(ExecContext *ctx, CmplVal& res, intType objType, CmplVal *s, unsigned maSe, unsigned se)
    {
        switch (objType) {
            case VAL_OBJECT_TYPE_VAR:
                convertToVar(ctx, res, *s, maSe, se);
                break;

            case VAL_OBJECT_TYPE_CON:
            case VAL_OBJECT_TYPE_OBJ:
                convertToCon(ctx, res, *s, maSe, se, (objType == VAL_OBJECT_TYPE_OBJ));
                break;

            default:
                // TODO
                res.copyFrom(s, true, false);
                break;
        }
    }

    /**
     * create cmpl variable object from a source value
     * @param ctx			execution context
     * @param res			store for result value
     * @param src			source value
     * @param maSe			syntax element id for new matrix element object
     * @param se			syntax element id of source value
     */
    void ObjectTypeUtil::convertToVar(ExecContext *ctx, CmplVal& res, CmplVal& src, unsigned maSe, unsigned se)
    {
        // if source value is already a cmpl variable then just use it
        if (src.t == TP_OPT_VAR) {
            res.copyFrom(src, true, false);
        }
        else if (src.t == TP_DATA_TYPE) {
            OptVar *v = new OptVar(ctx->modp()->getResModel(), src, false, maSe);
            res.set(TP_OPT_VAR, v);

            if (!src.valType()->setForOptVar(v))
                ctx->valueError("given data type is not allowed for optimization variable", src, se);
        }
        else {
            CmplValAuto t;
            if (ctx->checkContainerConvSpecial(t, src, TP_OPT_VAR)) {
                StackValue arg;
                arg.init(se);
                arg.val().moveFrom(t);

                convertTo(ctx, res, TP_OPT_VAR, &arg, maSe, se);
            }
            else {
                ctx->valueError("argument for variable creation must be a data type", src, se);
                createDefaultVar(ctx, res, maSe, se);
            }
        }
    }

    /**
     * create new default cmpl variable object
     * @param ctx			execution context
     * @param res			store for result value
     * @param maSe			syntax element id for new matrix element object
     * @param se			syntax element id of source value
     */
    void ObjectTypeUtil::createDefaultVar(ExecContext *ctx, CmplVal& res, unsigned maSe, unsigned se)
    {
        Interpreter *modp = ctx->modp();
        OptVar *v = new OptVar(modp->getResModel(), modp->defTypeVar(), true, maSe);
        modp->defTypeVar().valType()->setForOptVar(v);
        res.set(TP_OPT_VAR, v);
    }

    /**
     * create optimization matrix row object from a source value
     * @param ctx			execution context
     * @param res			store for result value
     * @param src			source value
     * @param maSe			syntax element id for new matrix element object
     * @param se			syntax element id of source value
     * @param obj			true: optimization objective / false: optimization constraint
     */
    void ObjectTypeUtil::convertToCon(ExecContext *ctx, CmplVal& res, CmplVal& src, unsigned maSe, unsigned se, bool obj)
    {
        // if source is already optimization matrix row object then just use it
        if (src.isOptRow()) {
            res.copyFrom(src, true, false);
        }
        else {
            ValFormula *f = (src.t == TP_FORMULA ? src.valFormula() : (src.t == TP_OPT_VAR ? new ValFormulaVarOp(se, src.optVar()) : NULL));
            CmplValAuto v;

            if (f) {
                if (!f->canOptRow(obj)) {
                    if (obj)
                        ctx->valueError("formula not valid for objective", src, se);
                    else
                        ctx->valueError("formula not valid for constraint", src, se);
                }
                v.set(TP_FORMULA, f);
            }
            else {
                CmplValAuto t;
                if (src.t == TP_CONTAINER && ctx->checkContainerConvSpecial(t, src, (obj ? TP_OPT_OBJ : TP_OPT_CON))) {
                    StackValue arg;
                    arg.init(se);
                    arg.val().moveFrom(t);

                    convertTo(ctx, res, (obj ? TP_OPT_OBJ : TP_OPT_CON), &arg, maSe, se);
                    return;
                }
                else {
                    // allow binary value as pseudo constraint and numeric value as pseudo objective (to replace later by result matrix dummy row)
                    if (!((obj && src.isScalarNumber()) || (!obj && src.t == TP_BIN)))
                        ctx->valueError("argument for constraint/objective creation must be a formula", src, se);

                    v.copyFrom(src, true, false);
                }
            }

            VarCondMapping *vcm = ctx->curVarCondMap();
            if (vcm) {
                if (obj) {
                    ctx->valueError("objective cannot be defined within a condition over optimization variables", src, se);
                }
                else {
                    CmplValAuto t;
                    ValFormulaCondOp *nf = new ValFormulaCondOp(vcm->syntaxElem());
                    t.set(TP_FORMULA, nf);

                    CmplValAuto& fc = vcm->getFullCond();
                    vector<CmplValAuto> d;
                    nf->insPart(fc, d, v);

                    v.moveFrom(t, true);
                }
            }

            OptCon *oc = new OptCon(ctx->modp()->getResModel(), v, obj, maSe);
            res.set((obj ? TP_OPT_OBJ : TP_OPT_CON), oc);
        }

    }
 }

