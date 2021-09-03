
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


#ifndef VALSPECIAL_HH
#define VALSPECIAL_HH

#include "../../CommonData/CmplVal.hh"
#include "ExecContext.hh"



using namespace std;



namespace cmpl
{
	class ExecContext;


    /**
	 * the <code>ValSpecialBase</code> is the base class
	 * for the special functionality of pseudo symbols in cmpl.
	 */
	class ValSpecialBase : public CmplObjBase
	{
	protected:
        ExecContext *_ctx;                                  ///< execution context
        unsigned _se;                                       ///< syntax element
        bool _lvalue;                                       ///< pseudo subsymbol used as lvalue
        CmplValAuto _base;      							///< base object for pseudo subsymbol

        /**
         * constructor
         * @param ctx			execution context
         * @param se            syntax element
         * @param lval          pseudo subsymbol used as lvalue
         * @param base			base object for pseudo subsymbol
         */
        ValSpecialBase(ExecContext *ctx, unsigned se, bool lval, CmplVal *base): _ctx(ctx), _se(se), _lvalue(lval)  { if (base) _base.copyFrom(base, true, false); else _base.unset(); }

    public:
		/**
		 * create the appropriate subclass object for handling a given pseudo symbol
		 * @param ctx			execution context
		 * @param code			code number of the pseudo symbol
         * @param se            syntax element
         * @param lval          pseudo subsymbol used as lvalue
         * @param base			base object for pseudo subsymbol
		 * @return				object for pseudo symbol functionality
		 */
        static ValSpecialBase *create(ExecContext *ctx, unsigned code, unsigned se, bool lval, CmplVal *base = NULL);

        /**
         * get name of the pseudo symbol
         */
        virtual const char *funcName() const                                    { return "<dummy>"; }

        /**
         * get whether pseudo symbol can be used as an lvalue
         */
        virtual bool canLvalue()                                                { return false; }

        /**
         * get whether pseudo symbol can be used with a base object (0:only no / 1:only yes / -1:both)
         */
        virtual short canBase()                                                 { return 0; }

		/**
		 * read functionality of the pseudo symbol
		 * @param res			return of result value
		 */
        virtual void get(CmplVal& res);

		/**
		 * write functionality of the pseudo symbol
		 * @param val			value to write
         * @param se            syntax element of value
		 */
        virtual void set(CmplVal& val, unsigned se);

        /**
         * get whether pseudo symbol has special handling for indexed expressions and for function defset
         */
        virtual bool hasIndexedHandling()                                       { return false; }

        /**
         * read functionality for indexed value of the pseudo symbol, only called if hasIndexedHandling()
         * @param res           return of result value
         * @param ind           index tuple or set
         * @return              false if index is invalid
         */
        virtual bool getIndexed(CmplVal& res, CmplVal& ind);

        /**
         * write functionality for indexed value of the pseudo symbol, only called if hasIndexedHandling()
         * @param val           return of result value
         * @param se            syntax element of value
         * @param ind           index tuple or set
         */
        virtual bool setIndexed(CmplVal& val, unsigned se, CmplVal& ind);

        /**
         * get definition set for pseudo symbol, only called if hasIndexedHandling()
         * @param res           return of result set
         */
        virtual void getDefset(CmplVal& res);
    };



    /**
     * dummy class for pseudo symbol value without any function.
     * only used for error handling.
     */
    class ValSpecialDummy : public ValSpecialBase
    {
    public:
        /**
         * constructor
         * @param ctx			execution context
         * @param se            syntax element
         * @param lval          pseudo subsymbol used as lvalue
         * @param base			base object for pseudo subsymbol
         */
        ValSpecialDummy(ExecContext *ctx, unsigned se, bool lval, CmplVal *base): ValSpecialBase(ctx, se, lval, base)   { }

        /**
         * get whether pseudo symbol can be used as an lvalue
         */
        virtual bool canLvalue()                            { return true; }

        /**
         * get whether pseudo symbol can be used with a base object (0:only no / 1:only yes / -1:both)
         */
        virtual short canBase()                             { return -1; }

        /**
         * read functionality of the pseudo symbol
         * @param res			return of result value
         */
        virtual void get(CmplVal& res)                      { res.set(TP_NULL); }

        /**
         * write functionality of the pseudo symbol
         * @param val			value to write
         * @param se            syntax element of value
         */
        virtual void set(CmplVal& val, unsigned se)         { }

        /**
         * get whether pseudo symbol has special handling for indexed expressions and for function defset
         */
        virtual bool hasIndexedHandling()                   { return true; }

        /**
         * read functionality for indexed value of the pseudo symbol, only called if hasIndexedHandling()
         * @param res           return of result value
         * @param ind           index tuple or set
         * @return              false if index is invalid
         */
        virtual bool getIndexed(CmplVal& res, CmplVal& ind) { res.set(TP_NULL); return true; }

        /**
         * write functionality for indexed value of the pseudo symbol, only called if hasIndexedHandling()
         * @param val           return of result value
         * @param se            syntax element of value
         * @param ind           index tuple or set
         * @return              false if index is invalid
         */
        virtual bool setIndexed(CmplVal& val, unsigned se, CmplVal& ind) { return true; }

        /**
         * get definition set for pseudo symbol, only called if hasIndexedHandling()
         * @param res           return of result set
         */
        virtual void getDefset(CmplVal& res)                { res.set(TP_SET_EMPTY); }
    };


    /**
     * class for pseudo symbol $arg.
     */
    class ValSpecialArg : public ValSpecialBase
    {
    public:
        /**
         * constructor
         * @param ctx			execution context
         * @param se            syntax element
         * @param lval          pseudo subsymbol used as lvalue
         * @param base			base object for pseudo subsymbol
         */
        ValSpecialArg(ExecContext *ctx, unsigned se, bool lval, CmplVal *base): ValSpecialBase(ctx, se, lval, base)   { }

        /**
         * get name of the pseudo symbol
         */
        virtual const char *funcName() const                                    { return "$arg"; }

        /**
         * read functionality of the pseudo symbol
         * @param res			return of result value
         */
        virtual void get(CmplVal& res);
    };


    /**
     * class for pseudo symbol $this.
     */
    class ValSpecialThis : public ValSpecialBase
    {
    public:
        /**
         * constructor
         * @param ctx			execution context
         * @param se            syntax element
         * @param lval          pseudo subsymbol used as lvalue
         * @param base			base object for pseudo subsymbol
         */
        ValSpecialThis(ExecContext *ctx, unsigned se, bool lval, CmplVal *base): ValSpecialBase(ctx, se, lval, base)   { }

        /**
         * get name of the pseudo symbol
         */
        virtual const char *funcName() const                                    { return "$this"; }

        /**
         * read functionality of the pseudo symbol
         * @param res			return of result value
         */
        virtual void get(CmplVal& res);
    };


    /**
     * class for pseudo symbol $curTuple.
     */
    class ValSpecialCurTuple : public ValSpecialBase
    {
    public:
        /**
         * constructor
         * @param ctx			execution context
         * @param se            syntax element
         * @param lval          pseudo subsymbol used as lvalue
         * @param base			base object for pseudo subsymbol
         */
        ValSpecialCurTuple(ExecContext *ctx, unsigned se, bool lval, CmplVal *base): ValSpecialBase(ctx, se, lval, base)   { }

        /**
         * get name of the pseudo symbol
         */
        virtual const char *funcName() const                                    { return "$curTuple"; }

        /**
         * read functionality of the pseudo symbol
         * @param res			return of result value
         */
        virtual void get(CmplVal& res);
    };


    /**
     * class for pseudo symbol $curFullTuple.
     */
    class ValSpecialCurFullTuple : public ValSpecialBase
    {
    public:
        /**
         * constructor
         * @param ctx			execution context
         * @param se            syntax element
         * @param lval          pseudo subsymbol used as lvalue
         * @param base			base object for pseudo subsymbol
         */
        ValSpecialCurFullTuple(ExecContext *ctx, unsigned se, bool lval, CmplVal *base): ValSpecialBase(ctx, se, lval, base)   { }

        /**
         * get name of the pseudo symbol
         */
        virtual const char *funcName() const                                    { return "$curFullTuple"; }

        /**
         * read functionality of the pseudo symbol
         * @param res			return of result value
         */
        virtual void get(CmplVal& res);
    };


    /**
     * class for pseudo symbol $curDestName.
     */
    class ValSpecialCurDestName : public ValSpecialBase
    {
    public:
        /**
         * constructor
         * @param ctx			execution context
         * @param se            syntax element
         * @param lval          pseudo subsymbol used as lvalue
         * @param base			base object for pseudo subsymbol
         */
        ValSpecialCurDestName(ExecContext *ctx, unsigned se, bool lval, CmplVal *base): ValSpecialBase(ctx, se, lval, base)   { }

        /**
         * get name of the pseudo symbol
         */
        virtual const char *funcName() const                                    { return "$curDestName"; }

        /**
         * get whether pseudo symbol can be used as an lvalue
         */
        virtual bool canLvalue()                                                { return true; }

        /**
         * read functionality of the pseudo symbol
         * @param res			return of result value
         */
        virtual void get(CmplVal& res);

        /**
         * write functionality of the pseudo symbol
         * @param val			value to write
         * @param se            syntax element of value
         */
        virtual void set(CmplVal& val, unsigned se);
    };


    /**
     * class for pseudo symbol $curDestTuple.
     */
    class ValSpecialCurDestTuple : public ValSpecialBase
    {
    public:
        /**
         * constructor
         * @param ctx			execution context
         * @param se            syntax element
         * @param lval          pseudo subsymbol used as lvalue
         * @param base			base object for pseudo subsymbol
         */
        ValSpecialCurDestTuple(ExecContext *ctx, unsigned se, bool lval, CmplVal *base): ValSpecialBase(ctx, se, lval, base)   { }

        /**
         * get name of the pseudo symbol
         */
        virtual const char *funcName() const                                    { return "$curDestTuple"; }

        /**
         * read functionality of the pseudo symbol
         * @param res			return of result value
         */
        virtual void get(CmplVal& res);
    };


    /**
     * class for pseudo symbol $varByName.
     */
    class ValSpecialVarByName : public ValSpecialBase
    {
    public:
        /**
         * constructor
         * @param ctx			execution context
         * @param se            syntax element
         * @param lval          pseudo subsymbol used as lvalue
         * @param base			base object for pseudo subsymbol
         */
        ValSpecialVarByName(ExecContext *ctx, unsigned se, bool lval, CmplVal *base): ValSpecialBase(ctx, se, lval, base)   { }

        /**
         * get name of the pseudo symbol
         */
        virtual const char *funcName() const                                    { return "$varByName"; }

        /**
         * read functionality of the pseudo symbol
         * @param res			return of result value
         */
        virtual void get(CmplVal& res);

        /**
         * get whether pseudo symbol has special handling for indexed expressions and for function defset
         */
        virtual bool hasIndexedHandling()                                       { return true; }

        /**
         * read functionality for indexed value of the pseudo symbol, only called if hasIndexedHandling()
         * @param res           return of result value
         * @param ind           index tuple or set
         * @return              false if index is invalid
         */
        virtual bool getIndexed(CmplVal& res, CmplVal& ind);

        /**
         * get definition set for pseudo symbol, only called if hasIndexedHandling()
         * @param res           return of result set
         */
        virtual void getDefset(CmplVal& res);
    };


    /**
     * class for pseudo symbol $varByNo.
     */
    class ValSpecialVarByNo : public ValSpecialBase
    {
    public:
        /**
         * constructor
         * @param ctx			execution context
         * @param se            syntax element
         * @param lval          pseudo subsymbol used as lvalue
         * @param base			base object for pseudo subsymbol
         */
        ValSpecialVarByNo(ExecContext *ctx, unsigned se, bool lval, CmplVal *base): ValSpecialBase(ctx, se, lval, base)   { }

        /**
         * get name of the pseudo symbol
         */
        virtual const char *funcName() const                                    { return "$varByNo"; }

        /**
         * read functionality of the pseudo symbol
         * @param res			return of result value
         */
        virtual void get(CmplVal& res);

        /**
         * get whether pseudo symbol has special handling for indexed expressions and for function defset
         */
        virtual bool hasIndexedHandling()                                       { return true; }

        /**
         * read functionality for indexed value of the pseudo symbol, only called if hasIndexedHandling()
         * @param res           return of result value
         * @param ind           index tuple or set
         * @return              false if index is invalid
         */
        virtual bool getIndexed(CmplVal& res, CmplVal& ind);

        /**
         * get definition set for pseudo symbol, only called if hasIndexedHandling()
         * @param res           return of result set
         */
        virtual void getDefset(CmplVal& res);
    };


    /**
     * class for pseudo symbol $conByName.
     */
    class ValSpecialConByName : public ValSpecialBase
    {
    public:
        /**
         * constructor
         * @param ctx			execution context
         * @param se            syntax element
         * @param lval          pseudo subsymbol used as lvalue
         * @param base			base object for pseudo subsymbol
         */
        ValSpecialConByName(ExecContext *ctx, unsigned se, bool lval, CmplVal *base): ValSpecialBase(ctx, se, lval, base)   { }

        /**
         * get name of the pseudo symbol
         */
        virtual const char *funcName() const                                    { return "$conByName"; }

        /**
         * read functionality of the pseudo symbol
         * @param res			return of result value
         */
        virtual void get(CmplVal& res);

        /**
         * get whether pseudo symbol has special handling for indexed expressions and for function defset
         */
        virtual bool hasIndexedHandling()                                       { return true; }

        /**
         * read functionality for indexed value of the pseudo symbol, only called if hasIndexedHandling()
         * @param res           return of result value
         * @param ind           index tuple or set
         * @return              false if index is invalid
         */
        virtual bool getIndexed(CmplVal& res, CmplVal& ind);

        /**
         * get definition set for pseudo symbol, only called if hasIndexedHandling()
         * @param res           return of result set
         */
        virtual void getDefset(CmplVal& res);
    };


    /**
     * class for pseudo symbol $conByNo.
     */
    class ValSpecialConByNo : public ValSpecialBase
    {
    public:
        /**
         * constructor
         * @param ctx			execution context
         * @param se            syntax element
         * @param lval          pseudo subsymbol used as lvalue
         * @param base			base object for pseudo subsymbol
         */
        ValSpecialConByNo(ExecContext *ctx, unsigned se, bool lval, CmplVal *base): ValSpecialBase(ctx, se, lval, base)   { }

        /**
         * get name of the pseudo symbol
         */
        virtual const char *funcName() const                                    { return "$conByNo"; }

        /**
         * read functionality of the pseudo symbol
         * @param res			return of result value
         */
        virtual void get(CmplVal& res);

        /**
         * get whether pseudo symbol has special handling for indexed expressions and for function defset
         */
        virtual bool hasIndexedHandling()                                       { return true; }

        /**
         * read functionality for indexed value of the pseudo symbol, only called if hasIndexedHandling()
         * @param res           return of result value
         * @param ind           index tuple or set
         * @return              false if index is invalid
         */
        virtual bool getIndexed(CmplVal& res, CmplVal& ind);

        /**
         * get definition set for pseudo symbol, only called if hasIndexedHandling()
         * @param res           return of result set
         */
        virtual void getDefset(CmplVal& res);
    };



    /**
     * base class for pseudo subsymbols.
     */
    class ValSpecialBaseSub : public ValSpecialBase
    {
    public:
        /**
         * constructor
         * @param ctx			execution context
         * @param se            syntax element
         * @param lval          pseudo subsymbol used as lvalue
         * @param base			base object for pseudo subsymbol
         */
        ValSpecialBaseSub(ExecContext *ctx, unsigned se, bool lval, CmplVal *base): ValSpecialBase(ctx, se, lval, base)   { }

        /**
         * get whether pseudo symbol can be used with a base object (0:only no / 1:only yes / -1:both)
         */
        virtual short canBase()                                                 { return 1; }
    };


    /**
     * class for pseudo subsymbol $objectType.
     */
    class ValSpecialObjectType : public ValSpecialBaseSub
    {
    public:
        /**
         * constructor
         * @param ctx			execution context
         * @param se            syntax element
         * @param lval          pseudo subsymbol used as lvalue
         * @param base			base object for pseudo subsymbol
         */
        ValSpecialObjectType(ExecContext *ctx, unsigned se, bool lval, CmplVal *base): ValSpecialBaseSub(ctx, se, lval, base)   { }

        /**
         * get name of the pseudo symbol
         */
        virtual const char *funcName() const                                    { return "$objectType"; }

        /**
         * read functionality of the pseudo symbol
         * @param res			return of result value
         */
        virtual void get(CmplVal& res);
    };


    /**
     * class for pseudo subsymbol $dataType.
     */
    class ValSpecialDataType : public ValSpecialBaseSub
    {
    public:
        /**
         * constructor
         * @param ctx			execution context
         * @param se            syntax element
         * @param lval          pseudo subsymbol used as lvalue
         * @param base			base object for pseudo subsymbol
         */
        ValSpecialDataType(ExecContext *ctx, unsigned se, bool lval, CmplVal *base): ValSpecialBaseSub(ctx, se, lval, base)   { }

        /**
         * get name of the pseudo symbol
         */
        virtual const char *funcName() const                                    { return "$dataType"; }

        /**
         * read functionality of the pseudo symbol
         * @param res			return of result value
         */
        virtual void get(CmplVal& res);
    };


    /**
     * class for pseudo subsymbol $typeBase.
     */
    class ValSpecialTypeBase : public ValSpecialBaseSub
    {
    public:
        /**
         * constructor
         * @param ctx			execution context
         * @param se            syntax element
         * @param lval          pseudo subsymbol used as lvalue
         * @param base			base object for pseudo subsymbol
         */
        ValSpecialTypeBase(ExecContext *ctx, unsigned se, bool lval, CmplVal *base): ValSpecialBaseSub(ctx, se, lval, base)   { }

        /**
         * get name of the pseudo symbol
         */
        virtual const char *funcName() const                                    { return "$typeBase"; }

        /**
         * read functionality of the pseudo symbol
         * @param res			return of result value
         */
        virtual void get(CmplVal& res);
    };


    /**
     * class for pseudo subsymbol $typePar.
     */
    class ValSpecialTypePar : public ValSpecialBaseSub
    {
    public:
        /**
         * constructor
         * @param ctx			execution context
         * @param se            syntax element
         * @param lval          pseudo subsymbol used as lvalue
         * @param base			base object for pseudo subsymbol
         */
        ValSpecialTypePar(ExecContext *ctx, unsigned se, bool lval, CmplVal *base): ValSpecialBaseSub(ctx, se, lval, base)   { }

        /**
         * get name of the pseudo symbol
         */
        virtual const char *funcName() const                                    { return "$typePar"; }

        /**
         * read functionality of the pseudo symbol
         * @param res			return of result value
         */
        virtual void get(CmplVal& res);
    };


    /**
     * class for pseudo subsymbol $isReadOnly.
     */
    class ValSpecialIsReadOnly : public ValSpecialBaseSub
    {
    public:
        /**
         * constructor
         * @param ctx			execution context
         * @param se            syntax element
         * @param lval          pseudo subsymbol used as lvalue
         * @param base			base object for pseudo subsymbol
         */
        ValSpecialIsReadOnly(ExecContext *ctx, unsigned se, bool lval, CmplVal *base): ValSpecialBaseSub(ctx, se, lval, base)   { }

        /**
         * get name of the pseudo symbol
         */
        virtual const char *funcName() const                                    { return "$isReadOnly"; }

        /**
         * read functionality of the pseudo symbol
         * @param res			return of result value
         */
        virtual void get(CmplVal& res);
    };


    /**
     * class for pseudo subsymbol $destName.
     */
    class ValSpecialDestName : public ValSpecialBaseSub
    {
    public:
        /**
         * constructor
         * @param ctx			execution context
         * @param se            syntax element
         * @param lval          pseudo subsymbol used as lvalue
         * @param base			base object for pseudo subsymbol
         */
        ValSpecialDestName(ExecContext *ctx, unsigned se, bool lval, CmplVal *base): ValSpecialBaseSub(ctx, se, lval, base)   { }

        /**
         * get name of the pseudo symbol
         */
        virtual const char *funcName() const                                    { return "$destName"; }

        /**
         * get whether pseudo symbol can be used as an lvalue
         */
        virtual bool canLvalue()                                                { return _base.isOptRC(); }

        /**
         * read functionality of the pseudo symbol
         * @param res			return of result value
         */
        virtual void get(CmplVal& res);

        /**
         * write functionality of the pseudo symbol
         * @param val			value to write
         * @param se            syntax element of value
         */
        virtual void set(CmplVal& val, unsigned se);

        /**
         * get whether pseudo symbol has special handling for indexed expressions and for function defset
         */
        virtual bool hasIndexedHandling()                                       { return _lvalue; }

        /**
         * write functionality for indexed value of the pseudo symbol, only called if hasIndexedHandling()
         * @param val           return of result value
         * @param se            syntax element of value
         * @param ind           index tuple or set
         * @return              false if index is invalid
         */
        virtual bool setIndexed(CmplVal& val, unsigned se, CmplVal& ind);
    };


    /**
     * class for pseudo subsymbol $destTuple.
     */
    class ValSpecialDestTuple : public ValSpecialBaseSub
    {
    public:
        /**
         * constructor
         * @param ctx			execution context
         * @param se            syntax element
         * @param lval          pseudo subsymbol used as lvalue
         * @param base			base object for pseudo subsymbol
         */
        ValSpecialDestTuple(ExecContext *ctx, unsigned se, bool lval, CmplVal *base): ValSpecialBaseSub(ctx, se, lval, base)   { }

        /**
         * get name of the pseudo symbol
         */
        virtual const char *funcName() const                                    { return "$destTuple"; }

        /**
         * get whether pseudo symbol can be used as an lvalue
         */
        virtual bool canLvalue()                                                { return _base.isOptRC(); }

        /**
         * read functionality of the pseudo symbol
         * @param res			return of result value
         */
        virtual void get(CmplVal& res);

        /**
         * write functionality of the pseudo symbol
         * @param val			value to write
         * @param se            syntax element of value
         */
        virtual void set(CmplVal& val, unsigned se);
    };


    /**
     * class for pseudo subsymbol $destNameTuple.
     */
    class ValSpecialDestNameTuple : public ValSpecialBaseSub
    {
    public:
        /**
         * constructor
         * @param ctx			execution context
         * @param se            syntax element
         * @param lval          pseudo subsymbol used as lvalue
         * @param base			base object for pseudo subsymbol
         */
        ValSpecialDestNameTuple(ExecContext *ctx, unsigned se, bool lval, CmplVal *base): ValSpecialBaseSub(ctx, se, lval, base)   { }

        /**
         * get name of the pseudo symbol
         */
        virtual const char *funcName() const                                    { return "$destNameTuple"; }

        /**
         * get whether pseudo symbol can be used as an lvalue
         */
        virtual bool canLvalue()                                                { return _base.isOptRC(); }

        /**
         * read functionality of the pseudo symbol
         * @param res			return of result value
         */
        virtual void get(CmplVal& res);

        /**
         * write functionality of the pseudo symbol
         * @param val			value to write
         * @param se            syntax element of value
         */
        virtual void set(CmplVal& val, unsigned se);
    };


    /**
     * class for pseudo subsymbol $destFullName.
     */
    class ValSpecialDestFullName : public ValSpecialBaseSub
    {
    public:
        /**
         * constructor
         * @param ctx			execution context
         * @param se            syntax element
         * @param lval          pseudo subsymbol used as lvalue
         * @param base			base object for pseudo subsymbol
         */
        ValSpecialDestFullName(ExecContext *ctx, unsigned se, bool lval, CmplVal *base): ValSpecialBaseSub(ctx, se, lval, base)   { }

        /**
         * get name of the pseudo symbol
         */
        virtual const char *funcName() const                                    { return "$destFullName"; }

        /**
         * read functionality of the pseudo symbol
         * @param res			return of result value
         */
        virtual void get(CmplVal& res);
    };


    /**
     * class for pseudo subsymbol $fullFormula.
     */
    class ValSpecialFullFormula : public ValSpecialBaseSub
    {
    public:
        /**
         * constructor
         * @param ctx			execution context
         * @param se            syntax element
         * @param lval          pseudo subsymbol used as lvalue
         * @param base			base object for pseudo subsymbol
         */
        ValSpecialFullFormula(ExecContext *ctx, unsigned se, bool lval, CmplVal *base): ValSpecialBaseSub(ctx, se, lval, base)   { }

        /**
         * get name of the pseudo symbol
         */
        virtual const char *funcName() const                                    { return "$fullFormula"; }

        /**
         * get whether pseudo symbol can be used as an lvalue
         */
        virtual bool canLvalue()                                                { return _base.isOptRow(); }

        /**
         * read functionality of the pseudo symbol
         * @param res			return of result value
         */
        virtual void get(CmplVal& res);

        /**
         * write functionality of the pseudo symbol
         * @param val			value to write
         * @param se            syntax element of value
         */
        virtual void set(CmplVal& val, unsigned se);
    };


    /**
     * class for pseudo subsymbol $formulaArray.
     */
    class ValSpecialFormulaArray : public ValSpecialBaseSub
    {
    public:
        /**
         * constructor
         * @param ctx			execution context
         * @param se            syntax element
         * @param lval          pseudo subsymbol used as lvalue
         * @param base			base object for pseudo subsymbol
         */
        ValSpecialFormulaArray(ExecContext *ctx, unsigned se, bool lval, CmplVal *base): ValSpecialBaseSub(ctx, se, lval, base)   { }

        /**
         * get name of the pseudo symbol
         */
        virtual const char *funcName() const                                    { return "$formulaArray"; }

        /**
         * get whether pseudo symbol can be used as an lvalue
         */
        virtual bool canLvalue()                                                { return _base.isOptRow() || _base.t == TP_FORMULA; }

        /**
         * read functionality of the pseudo symbol
         * @param res			return of result value
         */
        virtual void get(CmplVal& res);

        /**
         * write functionality of the pseudo symbol
         * @param val			value to write
         * @param se            syntax element of value
         */
        virtual void set(CmplVal& val, unsigned se);
    };
}

#endif // VALSPECIAL_HH

