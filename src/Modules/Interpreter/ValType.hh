
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


#ifndef VALTYPE_HH
#define VALTYPE_HH

#include <vector>

#include "../../CommonData/CmplVal.hh"
#include "../../CommonData/Tuple.hh"
#include "ValFunction.hh"



using namespace std;



namespace cmpl
{
	/**
	 * base class for internal and for user defined value types in cmpl
	 */
	class ValType : public ValFunctionBase
	{
	protected:
		CmplVal _param;						///< tuple with type parameters (must be TP_TUPLE* or TP_EMPTY)
		CmplVal _defVal;					///< default value for the type
		tp_e _baseType;						///< base type

        virtual const char *typeName() const = 0;		///< name of the internal data type
        virtual const char *funcName() const override           { return typeName(); }

	public:
        /**
         * destructor
         */
        virtual ~ValType()                                                          { _param.dispose(); _defVal.dispose(); }

		/**
		 * get tuple with type parameters
		 */
		inline CmplVal& param()														{ return _param; }

        /**
		 * get default value for the type
		 */
		inline CmplVal& defVal()													{ return _defVal; }

        /**
         * get default value for the type as copy if needed
         * @param res           return of result value
         */
        virtual void defValCopy(CmplVal& res) const                                 { res.copyFrom(_defVal, true, false); }

		/**
		 * get base type
		 */
        inline tp_e baseType()														{ return _baseType; }

        /**
         * copy instance variables for this base class from another ValType object
         * @param src           source object
         */
        inline void copyBaseFrom(ValType *src)                                      { _param.copyFrom(src->_param); _defVal.copyFrom(src->_defVal); _baseType = src->baseType(); }

		/**
		 * write contents of the object to a stream
		 * @param modp			calling module
		 * @param mode			mode for output: 0=direct; 1=part of other value
		 */
        virtual void write(ostream& ostr, ModuleBase *modp, int mode = 0) const override     { ostr << "<data type object>"; }

		/**
		 * calls the cmpl index operation and store result in execution context, if this value has a special handling for it
		 * @param ctx			execution context
		 * @param arg			pointer to argument value
		 * @return				true if this has handled the operation
		 */
        virtual bool operIndex(ExecContext *ctx, StackValue *arg) override;

		/**
		 * get a new data type object with applied parameters
		 * @param ctx			execution context
		 * @param arg			pointer to argument, only for error messages
		 * @param vtp			data type parameter as tuple value
		 * @return				new data type object / NULL, if this has already the given parameters
		 */
		virtual ValType *newParam(ExecContext *ctx, StackValue *arg, CmplVal *vtp);

        /**
         * convert simple value (no array) to this data type
         * @param ctx			execution context
         * @param res			store for result value
         * @param src			source value
         * @param se			syntax element id of source value
         */
        virtual void convertSimpleTo(ExecContext *ctx, CmplVal& res, CmplVal& src, unsigned se);

        /**
         * try to convert simple value (no array) to this data type
         * @param ctx			execution context
         * @param res			store for result value
         * @param src			source value
         * @param tcl			conversion level
         * @return              true if conversion is successful
         */
        virtual bool tryConvertSimpleTo(ExecContext *ctx, CmplVal& res, CmplVal& src, TypeConversionLevel tcl) const      { return false; }

        /**
         * convert value to this data type
         * @param ctx			execution context
         * @param res			store for result value
         * @param src			source value
         */
        virtual void convertTo(ExecContext *ctx, CmplVal& res, StackValue *src)     { callForArrayElements(ctx, res, src); }

        /**
         * check whether value has this data type
         * @param v             value to check
         * @return              true if value has this data type
         */
        virtual bool checkValue(CmplVal& v) const                                   { return false; }

        /**
         * make type parameter from bounds
         * @param param         return of resulting type parameter (TP_TUPLE* or TP_EMPTY)
         */
        virtual void paramFromBounds(CmplVal& param) const                          { param.dispUnset(); }

        /**
         * set info about base type and bounds for this data type within an optimization variable
         * @param v				optimization variable
         * @return				false if this data type is not suitable for optimization variables
         */
        virtual bool setForOptVar(OptVar *v)										{ return false; }

    protected:
        /**
         * error type conversion not implemented
         * @param ctx			execution context
         * @param res			store for result value
         * @param src			source value
         * @param se			syntax element id of source value
         */
        void notImplementedError(ExecContext *ctx, CmplVal& res, CmplVal& src, unsigned se);

    public:
        /**
         * calls the cmpl function call operation and store result in execution context, function calls means type conversion here
         * @param ctx			execution context
         * @param arg			pointer to argument value
         * @return				true
         */
        virtual bool operCall(ExecContext *ctx, StackValue *arg) override                                   { convertTo(ctx, ctx->opResult(), arg); return true; }

        /**
         * calls the function for a simple source value (no array or list)
         * @param ctx			execution context
         * @param res			store for result value
         * @param src			source value
         * @param aggr          called for aggregating elements of an array or a list
         * @param se			syntax element id of source value
         * @return              only used if aggr: true if result is final
         */
        virtual bool operCallSimple(ExecContext *ctx, CmplVal& res, CmplVal& src, bool aggr, unsigned se, void *info = NULL) override     { convertSimpleTo(ctx, res, src, se); return false; }

		//TODO
	};


	/**
	 * the <code>ValTypeUserOr</code> class is used for a cmpl value
	 * that represents an user defined data type, defined by or-concatenation of other data types.
	 */
	class ValTypeUserOr : public ValType
	{
	protected:
		vector<CmplVal> _types;				///< concatenate data types (values must be TP_DATA_TYPE)

        bool _alsoInf;                      ///< accept also infinite numeric values for this data type
        bool _alsoEmpty;                    ///< accept also empty (invalid) values for this data type
        bool _alsoNull;                     ///< accept also null values for this data type; and if used together with array assertion accept definition set which is a subset of the given set

        virtual const char *typeName() const override		{ return "<user defined union type>"; }

	public:
		/**
		 * constructor
		 */
        ValTypeUserOr()										{ _defVal.unset(); _baseType = TP_NULL; _alsoInf = _alsoEmpty = _alsoNull = false; }

        /**
         * destructor
         */
        virtual ~ValTypeUserOr()        					{ for (CmplVal& v : _types) { v.dispose(); } }

		/**
		 * write contents of the object to a stream
		 * @param modp			calling module
		 * @param mode			mode for output: 0=direct; 1=part of other value
		 */
        virtual void write(ostream& ostr, ModuleBase *modp, int mode = 0) const override;

		/**
		 * get a new data type object with applied parameters
		 * @param ctx			execution context
		 * @param arg			pointer to argument, only for error messages
		 * @param vtp			data type parameter as tuple value
		 * @return				new data type object / NULL, if this has already the given parameters
		 */
        virtual ValType *newParam(ExecContext *ctx, StackValue *arg, CmplVal *vtp) override;

        /**
         * convert simple value (no array) to this data type
         * @param ctx			execution context
         * @param res			store for result value
         * @param src			source value
         * @param se			syntax element id of source value
         */
        virtual void convertSimpleTo(ExecContext *ctx, CmplVal& res, CmplVal& src, unsigned se) override;

        /**
         * try to convert simple value (no array) to this data type
         * @param ctx			execution context
         * @param res			store for result value
         * @param src			source value
         * @param tcl			conversion level
         * @return              true if conversion is successful
         */
        virtual bool tryConvertSimpleTo(ExecContext *ctx, CmplVal& res, CmplVal& src, TypeConversionLevel tcl) const override;

        /**
         * check whether value has this data type
         * @param v             value to check
         * @return              true if value has this data type
         */
        virtual bool checkValue(CmplVal& v) const override;

        /**
         * construct data type by or-concatenation of two arguments
         * @param res           store for result value
         * @param a1            argument one (must be TP_DATA_TYPE)
         * @param a2            argument two
         * @return              true if result data type is successfully constructed
         */
        static bool construct(CmplVal *res, CmplVal *a1, CmplVal *a2);
    };



	/**
     * the <code>ValTypeInternal</code> class is the base class for the internal cmpl types
	 */
	class ValTypeInternal : public ValType
	{
	protected:
        /**
         * write parameter to stream
         * @param ostr          stream
         * @param modp          calling module
         */
        virtual void writeParam(ostream& ostr, ModuleBase *modp) const                          { if (_param) _param.write(ostr, modp, 1); }		///< write parameter to stream

	public:
		/**
		 * write contents of the object to a stream
         * @param ostr          stream
         * @param modp			calling module
		 * @param mode			mode for output: 0=direct; 1=part of other value
		 */
        virtual void write(ostream& ostr, ModuleBase *modp, int mode = 0) const override		{ /*if (mode == 0) { ostr << "<data type '" << typeName(); writeParam(ostr); ostr << "'>"; } else*/ { ostr << typeName(); writeParam(ostr, modp); } }

        /**
         * convert simple value (no array) to this data type
         * @param ctx			execution context
         * @param res			store for result value
         * @param src			source value
         * @param se			syntax element id of source value
         */
        virtual void convertSimpleTo(ExecContext *ctx, CmplVal& res, CmplVal& src, unsigned se) override;
    };


	/**
	 * data type "real"
	 */
	class ValTypeReal : public ValTypeInternal
	{
	protected:
		bool _hasLowBound;					///< flag whether the type has a lower bound
		bool _hasUppBound;					///< flag whether the type has an upper bound

		realType _lowBound;					///< lower bound (only if _hasLowBound == true)
		realType _uppBound;					///< upper bound (only if _hasUppBound == true)
		
	protected:
        virtual const char *typeName() const override           { return "real"; }
        //virtual void writeParam(ostream& ostr) const override	{ ostr << '['; if (_hasLowBound) { ostr << _lowBound; } ostr << ".."; if (_hasUppBound) { ostr << _uppBound; } ostr << ']'; }

	public:
		/**
		 * constructor
		 */
        ValTypeReal()                                           { _hasLowBound = true; _hasUppBound = false; _lowBound = _uppBound = 0.0; _defVal.set(TP_REAL, 0.0); _baseType = TP_REAL; }

		/**
		 * get a new data type object with applied parameters
		 * @param ctx			execution context
		 * @param arg			pointer to argument, only for error messages
		 * @param vtp			data type parameter as tuple value
		 * @return				new data type object / NULL, if this has already the given parameters
		 */
        virtual ValType *newParam(ExecContext *ctx, StackValue *arg, CmplVal *vtp) override;

        /**
         * set info about base type and bounds for this data type within an optimization variable
         * @param v				optimization variable
         * @return				false if this data type is not suitable for optimization variables
         */
        virtual bool setForOptVar(OptVar *v) override;

        /**
         * try to convert simple value (no array) to this data type
         * @param ctx			execution context
         * @param res			store for result value
         * @param src			source value
         * @param tcl			conversion level
         * @return              true if conversion is successful
         */
        virtual bool tryConvertSimpleTo(ExecContext *ctx, CmplVal& res, CmplVal& src, TypeConversionLevel tcl) const override;

        /**
         * check whether value has this data type
         * @param v             value to check
         * @return              true if value has this data type
         */
        virtual bool checkValue(CmplVal& v) const override;

        /**
         * make type parameter from bounds
         * @param param         return of resulting type parameter (TP_TUPLE* or TP_EMPTY)
         */
        virtual void paramFromBounds(CmplVal& param) const override;
    };


	/**
	 * data type "int"
	 */
	class ValTypeInt : public ValTypeInternal
	{
	protected:
		bool _hasLowBound;					///< flag whether the type has a lower bound
		bool _hasUppBound;					///< flag whether the type has an upper bound

		intType _lowBound;					///< lower bound (only if _hasLowBound == true)
		intType _uppBound;					///< upper bound (only if _hasUppBound == true)
		
	protected:
        virtual const char *typeName() const override           { return "int"; }
        //virtual void writeParam(ostream& ostr) const override	{ ostr << '['; if (_hasLowBound) { ostr << _lowBound; } ostr << ".."; if (_hasUppBound) { ostr << _uppBound; } ostr << ']'; }

	public:
		/**
		 * constructor
		 */
        ValTypeInt()                                            { _hasLowBound = true; _hasUppBound = false; _lowBound = _uppBound = 0; _defVal.set(TP_INT, (intType)0); _baseType = TP_INT; }

		/**
		 * get a new data type object with applied parameters
		 * @param ctx			execution context
		 * @param arg			pointer to argument, only for error messages
		 * @param vtp			data type parameter as tuple value
		 * @return				new data type object / NULL, if this has already the given parameters
		 */
        virtual ValType *newParam(ExecContext *ctx, StackValue *arg, CmplVal *vtp) override;

        /**
         * set info about base type and bounds for this data type within an optimization variable
         * @param v				optimization variable
         * @return				false if this data type is not suitable for optimization variables
         */
        virtual bool setForOptVar(OptVar *v) override;

        /**
         * try to convert simple value (no array) to this data type
         * @param ctx			execution context
         * @param res			store for result value
         * @param src			source value
         * @param tcl			conversion level
         * @return              true if conversion is successful
         */
        virtual bool tryConvertSimpleTo(ExecContext *ctx, CmplVal& res, CmplVal& src, TypeConversionLevel tcl) const override;

        /**
         * check whether value has this data type
         * @param v             value to check
         * @return              true if value has this data type
         */
        virtual bool checkValue(CmplVal& v) const override;

        /**
         * make type parameter from bounds
         * @param param         return of resulting type parameter (TP_TUPLE* or TP_EMPTY)
         */
        virtual void paramFromBounds(CmplVal& param) const override;
    };
	

	/**
	 * data type "bin"
	 */
	class ValTypeBin : public ValTypeInt
	{
	protected:
        virtual const char *typeName() const override           { return "bin"; }
        //virtual void writeParam(ostream& ostr) const override	{ }

	public:
		/**
		 * constructor
		 */
        ValTypeBin()                                            { _hasLowBound = true; _hasUppBound = true; _lowBound = 0; _uppBound = 1; _defVal.set(TP_BIN, (intType)0); _baseType = TP_BIN; }

		/**
		 * get a new data type object with applied parameters
		 * @param ctx			execution context
		 * @param arg			pointer to argument, only for error messages
		 * @param vtp			data type parameter as tuple value
		 * @return				new data type object / NULL, if this has already the given parameters
		 */
        virtual ValType *newParam(ExecContext *ctx, StackValue *arg, CmplVal *vtp) override;

        /**
         * try to convert simple value (no array) to this data type
         * @param ctx			execution context
         * @param res			store for result value
         * @param src			source value
         * @param tcl			conversion level
         * @return              true if conversion is successful
         */
        virtual bool tryConvertSimpleTo(ExecContext *ctx, CmplVal& res, CmplVal& src, TypeConversionLevel tcl) const override;

        /**
         * make type parameter from bounds
         * @param param         return of resulting type parameter (TP_TUPLE* or TP_EMPTY)
         */
        virtual void paramFromBounds(CmplVal& param) const override         { param.dispUnset(); }
    };


	/**
	 * data type "numeric"
     * (it is really the type "int[..] || real[..] || inf", but here implemented as its own class)
	 */
	class ValTypeNumeric : public ValTypeReal
	{
	protected:
        virtual const char *typeName() const override		{ return "numeric"; }

	public:
		/**
		 * constructor
		 */
        ValTypeNumeric()									{ _hasLowBound = false; _hasUppBound = false; _lowBound = _uppBound = 0.0; _defVal.set(TP_INT, (intType)0); _baseType = TP_NULL; }

        /**
         * convert simple value (no array) to this data type
         * @param ctx			execution context
         * @param res			store for result value
         * @param src			source value
         * @param se			syntax element id of source value
         */
        virtual void convertSimpleTo(ExecContext *ctx, CmplVal& res, CmplVal& src, unsigned se) override;

        /**
         * try to convert simple value (no array) to this data type
         * @param ctx			execution context
         * @param res			store for result value
         * @param src			source value
         * @param tcl			conversion level
         * @return              true if conversion is successful
         */
        virtual bool tryConvertSimpleTo(ExecContext *ctx, CmplVal& res, CmplVal& src, TypeConversionLevel tcl) const override;

        /**
         * set info about base type and bounds for this data type within an optimization variable
         * @param v				optimization variable
         * @return				false if this data type is not suitable for optimization variables
         */
        virtual bool setForOptVar(OptVar *v) override       { return false; }

        /**
         * check whether value has this data type
         * @param v             value to check
         * @return              true if value has this data type
         */
        virtual bool checkValue(CmplVal& v) const override;
    };


	/**
	 * data type "formula"
	 */
	class ValTypeFormula : public ValTypeInternal
	{
	protected:
        virtual const char *typeName() const override		{ return "formula"; }

	public:
		/**
		 * constructor
		 */
		ValTypeFormula()									{ _defVal.unset(); _baseType = TP_FORMULA; }

        /**
         * check whether value has this data type
         * @param v             value to check
         * @return              true if value has this data type
         */
        virtual bool checkValue(CmplVal& v) const override  { return (v.t == TP_FORMULA); }
    };


	/**
	 * data type "string"
	 */
	class ValTypeString : public ValTypeInternal
	{
	protected:
        virtual const char *typeName() const override		{ return "string"; }

	public:
		/**
		 * constructor
		 */
        ValTypeString()										{ _defVal.set(TP_STR, 0 /*is the empty string*/ ); _baseType = TP_STR; }

        /**
         * try to convert simple value (no array) to this data type
         * @param ctx			execution context
         * @param res			store for result value
         * @param src			source value
         * @param tcl			conversion level
         * @return              true if conversion is successful
         */
        virtual bool tryConvertSimpleTo(ExecContext *ctx, CmplVal& res, CmplVal& src, TypeConversionLevel tcl) const override;

        /**
         * check whether value has this data type
         * @param v             value to check
         * @return              true if value has this data type
         */
        virtual bool checkValue(CmplVal& v) const override  { return (v.isScalarString()); }
    };


	/**
	 * data type "indexpart"
	 * (it is really the type "int[..] || string", but here implemented as its own class)
	 */
	class ValTypeIndexpart : public ValTypeInternal
	{
	protected:
        virtual const char *typeName() const override		{ return "indexpart"; }

	public:
		/**
		 * constructor
		 */
		ValTypeIndexpart()									{ _defVal.set(TP_INT, (intType)0); _baseType = TP_NULL; }

        /**
         * convert simple value (no array) to this data type
         * @param ctx			execution context
         * @param res			store for result value
         * @param src			source value
         * @param se			syntax element id of source value
         */
        virtual void convertSimpleTo(ExecContext *ctx, CmplVal& res, CmplVal& src, unsigned se) override;

        /**
         * try to convert simple value (no array) to this data type
         * @param ctx			execution context
         * @param res			store for result value
         * @param src			source value
         * @param tcl			conversion level
         * @return              true if conversion is successful
         */
        virtual bool tryConvertSimpleTo(ExecContext *ctx, CmplVal& res, CmplVal& src, TypeConversionLevel tcl) const override;

        /**
         * check whether value has this data type
         * @param v             value to check
         * @return              true if value has this data type
         */
        virtual bool checkValue(CmplVal& v) const override           { return (v.isScalarIndex()); }
    };


	/**
	 * data type "tuple"
	 */
	class ValTypeTuple : public ValTypeInternal
	{
	protected:
		bool _hasFixRank;					///< flag whether the type has a fixed tuple rank
		unsigned _fixRank;					///< fixed tuple rank (only if _hasFixRank == true)

	protected:
        virtual const char *typeName() const override			{ return "tuple"; }
        //virtual void writeParam(ostream& ostr) const override	{ if (_hasFixRank) { ostr << '[' << _fixRank << ']'; } }

		/**
		 * get a new object of this class
		 */
        virtual ValTypeTuple *newValTypeObject() const          { return new ValTypeTuple(); }

	public:
		/**
		 * constructor
		 */
        ValTypeTuple()                                          { _hasFixRank = false; _fixRank = 0; _defVal.set(TP_ITUPLE_NULL); _baseType = TP_TUPLE; }

		/**
		 * get a new data type object with applied parameters
		 * @param ctx			execution context
		 * @param arg			pointer to argument, only for error messages
		 * @param vtp			data type parameter as tuple value
		 * @return				new data type object / NULL, if this has already the given parameters
		 */
        virtual ValType *newParam(ExecContext *ctx, StackValue *arg, CmplVal *vtp) override;

        /**
         * check whether value has this data type
         * @param v             value to check
         * @return              true if value has this data type
         */
        virtual bool checkValue(CmplVal& v) const override  { return (v.isTuple()); }

        /**
         * make type parameter from bounds
         * @param param         return of resulting type parameter (TP_TUPLE* or TP_EMPTY)
         */
        virtual void paramFromBounds(CmplVal& param) const override;
    };


	/**
	 * data type "set"
	 */
	class ValTypeSet : public ValTypeTuple
	{
	protected:
        virtual const char *typeName() const override		{ return "set"; }

		/**
		 * get a new object of this class
		 */
		virtual ValTypeTuple *newValTypeObject() const		{ return new ValTypeSet(); }

	public:
		/**
		 * constructor
		 */
        ValTypeSet()										{ _hasFixRank = false; _fixRank = 0; _defVal.set(TP_SET_EMPTY); _baseType = TP_SET_INF_SET; }

        /**
         * convert value to this data type
         * @param ctx			execution context
         * @param res			store for result value
         * @param src			source value
         */
        virtual void convertTo(ExecContext *ctx, CmplVal& res, StackValue *src) override;

        /**
         * calls the function for a simple source value (no array or list)
         * @param ctx			execution context
         * @param res			store for result value
         * @param src			source value
         * @param se			syntax element id of source value
         * @param info          info object for use by the caller
         * @return              only used if aggr: true if result is final
         */
        virtual bool operCallSimple(ExecContext *ctx, CmplVal& res, CmplVal& src, bool aggr, unsigned se, void *info = NULL) override;

        /**
         * try to convert simple value (no array) to this data type
         * @param ctx			execution context
         * @param res			store for result value
         * @param src			source value
         * @param tcl			conversion level
         * @return              true if conversion is successful
         */
        virtual bool tryConvertSimpleTo(ExecContext *ctx, CmplVal& res, CmplVal& src, TypeConversionLevel tcl) const override;

        /**
         * check whether value has this data type
         * @param v             value to check
         * @return              true if value has this data type
         */
        virtual bool checkValue(CmplVal& v) const override;
    };


	/**
	 * data type "interval"
	 */
	class ValTypeInterval : public ValTypeInternal
	{
	protected:
        virtual const char *typeName() const override		{ return "interval"; }

	public:
		/**
		 * constructor
		 */
		ValTypeInterval()									{ _defVal.set(TP_INTERVAL_0_INF); _baseType = TP_INTERVAL; }

        /**
         * check whether value has this data type
         * @param v             value to check
         * @return              true if value has this data type
         */
        virtual bool checkValue(CmplVal& v) const override  { return (v.isInterval()); }
    };


	/**
	 * data type "function"
	 */
	class ValTypeFunction : public ValTypeInternal
	{
	protected:
        virtual const char *typeName() const override		{ return "function"; }

	public:
		/**
		 * constructor
		 */
        ValTypeFunction()									{ _defVal.unset(); _baseType = TP_FUNCTION; }   //TODO: _defVal muss Dummy-Funktion sein, die immer null liefert; braucht bei defValCopy() nicht extra kopiert werden

        /**
         * check whether value has this data type
         * @param v             value to check
         * @return              true if value has this data type
         */
        virtual bool checkValue(CmplVal& v) const override  { return (v.t == TP_FUNCTION); }
    };


	/**
	 * data type "container"
	 */
	class ValTypeContainer : public ValTypeInternal
	{
	protected:
        virtual const char *typeName() const override		{ return "container"; }

	public:
		/**
		 * constructor
		 */
        ValTypeContainer();

        /**
         * get default value for the type as copy if needed
         * @param res           return of result value
         */
        virtual void defValCopy(CmplVal& res) const override;

        /**
         * check whether value has this data type
         * @param v             value to check
         * @return              true if value has this data type
         */
        virtual bool checkValue(CmplVal& v) const override  { return (v.t == TP_CONTAINER); }

        /**
         * try to convert simple value (no array) to this data type
         * @param ctx			execution context
         * @param res			store for result value
         * @param src			source value
         * @param tcl			conversion level
         * @return              true if conversion is successful
         */
        virtual bool tryConvertSimpleTo(ExecContext *ctx, CmplVal& res, CmplVal& src, TypeConversionLevel tcl) const override;
    };


	/**
	 * data type "type"
	 */
	class ValTypeType : public ValTypeInternal
	{
	protected:
        virtual const char *typeName() const override		{ return "type"; }

	public:
		/**
		 * constructor
		 */
        ValTypeType()										{ _defVal.unset(); _baseType = TP_DATA_TYPE; }

        /**
         * try to convert simple value (no array) to this data type
         * @param ctx			execution context
         * @param res			store for result value
         * @param src			source value
         * @param tcl			conversion level
         * @return              true if conversion is successful
         */
        virtual bool tryConvertSimpleTo(ExecContext *ctx, CmplVal& res, CmplVal& src, TypeConversionLevel tcl) const override;

        /**
         * check whether value has this data type
         * @param v             value to check
         * @return              true if value has this data type
         */
        virtual bool checkValue(CmplVal& v) const override  { return (v.t == TP_DATA_TYPE || v.t == TP_DATA_BASETYPE); }
    };


	/**
	 * data type "objecttype"
	 */
	class ValTypeObjecttype : public ValTypeInternal
	{
	protected:
        virtual const char *typeName() const override		{ return "objecttype"; }

	public:
		/**
		 * constructor
		 */
        ValTypeObjecttype()									{ _defVal.unset(); _baseType = TP_OBJECT_TYPE; }    //TODO: _defVal auf "par" ?

        /**
         * check whether value has this data type
         * @param v             value to check
         * @return              true if value has this data type
         */
        virtual bool checkValue(CmplVal& v) const override  { return (v.t == TP_OBJECT_TYPE); }
    };
	



    /**
     * class for static utility functions for object types
     */
    class ObjectTypeUtil
    {
    public:
        /**
         * convert to object of given type or create object
         * @param ctx			execution context
         * @param res			store for result value
         * @param objType		object type (one of VAL_OBJECT_TYPE_*)
         * @param arg			argument value
         * @param maSe			syntax element id for new matrix element object
         * @param se			syntax element id, only used if arg is not given
         * @param ord           use user order if exists
         */
        static void convertTo(ExecContext *ctx, CmplVal& res, intType objType, StackValue *arg, unsigned maSe, unsigned se, bool ord = false);

        /**
         * convert to object of given type or create object
         * @param ctx			execution context
         * @param res			store for result value
         * @param objType		object type (one of VAL_OBJECT_TYPE_*)
         * @param s				source value (must not be an array)
         * @param maSe			syntax element id for new matrix element object
         * @param se			syntax element id, only used if arg is not given
         */
        static void convertTo(ExecContext *ctx, CmplVal& res, intType objType, CmplVal *s, unsigned maSe, unsigned se);

        /**
         * create cmpl variable object from a source value
         * @param ctx			execution context
         * @param res			store for result value
         * @param src			source value
         * @param maSe			syntax element id for new matrix element object
         * @param se			syntax element id of source value
         */
        static void convertToVar(ExecContext *ctx, CmplVal& res, CmplVal& src, unsigned maSe, unsigned se);

        /**
         * create new default cmpl variable object
         * @param ctx			execution context
         * @param res			store for result value
         * @param maSe			syntax element id for new matrix element object
         * @param se			syntax element id of source value
         */
        static void createDefaultVar(ExecContext *ctx, CmplVal& res, unsigned maSe, unsigned se);

        /**
         * create optimization matrix row object from a source value
         * @param ctx			execution context
         * @param res			store for result value
         * @param src			source value
         * @param maSe			syntax element id for new matrix element object
         * @param se			syntax element id of source value
         * @param obj			true: optimization objective / false: optimization constraint
         */
        static void convertToCon(ExecContext *ctx, CmplVal& res, CmplVal& src, unsigned maSe, unsigned se, bool obj);

        /**
         * check whether value has a certain object type
         * @param objType		object type (one of VAL_OBJECT_TYPE_*)
         * @param v             value to check
         * @return              true if value has the object type
         */
        inline static bool checkValue(intType objType, CmplVal& v)                  { return v.hasObjectType(objType); }
    };

}

#endif // VALTYPE_HH

