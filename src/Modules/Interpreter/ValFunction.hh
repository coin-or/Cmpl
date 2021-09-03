
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


#ifndef VALFUNCTION_HH
#define VALFUNCTION_HH

#include <math.h>

#include "../../CommonData/CmplVal.hh"
#include "../../Control/ThreadHandler.hh"
#include "ExecContext.hh"



using namespace std;



namespace cmpl
{
	class ExecContext;


    /**
	 * the <code>ValFunctionBase</code> is the base class for cmpl values
	 * with a function-like behaviour.
	 */
	class ValFunctionBase : public CmplObjBase
	{
    public:
		/**
		 * calls the cmpl function call operation and store result in execution context, if this value has a special handling for it
		 * @param ctx			execution context
		 * @param arg			pointer to argument value
		 * @return				true if this has handled the operation
		 */
		virtual bool operCall(ExecContext *ctx, StackValue *arg)				{ return false; }

		/**
		 * calls the cmpl index operation and store result in execution context, if this value has a special handling for it
		 * @param ctx			execution context
		 * @param arg			pointer to argument value
		 * @return				true if this has handled the operation
		 */
		virtual bool operIndex(ExecContext *ctx, StackValue *arg)				{ return false; }


        /****** utility functions, only used for internal cmpl functions or types ****/
    protected:
        /**
         * get name of the function
         */
        virtual const char *funcName() const                                    { return NULL; }

        /**
         * calls the function for a simple source value (no array or list)
         * @param ctx			execution context
         * @param res			store for result value
         * @param src			source value
         * @param aggr          called for aggregating elements of an array or a list
         * @param se			syntax element id of source value
         * @return              only used if aggr: true if result is final
         */
        virtual bool operCallSimple(ExecContext *ctx, CmplVal& res, CmplVal& src, bool aggr, unsigned se, void *info = NULL)      { return true; }

    private:
        /**
         * recursive call for aggregating the elements of an array or a list
         * @param ctx			execution context
         * @param res           return of result value
         * @param arg			pointer to argument value
         * @param info          info object for use by the caller
         */
        void callForArrayAggrRek(ExecContext *ctx, CmplVal &res, StackValue *arg, void *info);

    protected:
        /**
         * calls function for aggregating the elements of an array or a list (result is a simple value, even if src is an array)
         * @param ctx			execution context
         * @param res           return of result value
         * @param arg			pointer to argument value
         * @param info          info object for use by the caller
         */
        void callForArrayAggr(ExecContext *ctx, CmplVal &res, StackValue *arg, void *info = NULL)       { res.set(TP_NULL); callForArrayAggrRek(ctx, res, arg, info); }

        /**
         * calls function for every single element of an array or a list (result is array over the same definition set as argument value)
         * @param ctx			execution context
         * @param res           return of result value
         * @param arg			pointer to argument value
         * @param info          info object for use by the caller
         */
        void callForArrayElements(ExecContext *ctx, CmplVal &res, StackValue *arg, void *info = NULL);

        /**
         * set a value within a result array
         * @param ctx           execution context
         * @param res           result array in which the value is to set (is not necessary an array in input)
         * @param tp1           first tuple part for the value
         * @param tp2           second tuple part for the value
         * @param val           value
         * @param se            syntax element id
         */
        void setInResultArray(ExecContext *ctx, CmplVal &res, const CmplVal &tp1, const CmplVal &tp2, const CmplVal &val, unsigned se);
    };


	/**
	 * base class for internal and for user defined functions in cmpl
	 */
	class ValFunction : public ValFunctionBase
	{
	protected:
		virtual bool userDef() const = 0;					///< function is an user defined function (written as cmpl code)

	public:
		/**
		 * write contents of the object to a stream
		 * @param modp			calling module
		 * @param mode			mode for output: 0=direct; 1=part of other value
		 */
        virtual void write(ostream& ostr, ModuleBase *modp, int mode = 0) const override    { ostr << "<function object>"; }

        /**
         * get whether function has special meaning for optimization direction
         * @return				0:no / 1:max / -1:min
         */
        virtual int optDir() const                                                          { return 0; }
	};


    /**
	 * the <code>ValFunctionIntern</code> class is for a cmpl value
	 * that represents an user defined function.
	 */
	class ValFunctionUser : public ValFunction
	{
    private:
        unsigned _startPos;                                 ///< start position of function within the intermediary code

        //TODO: Subsymbolalias-Tabelle

    public:
        /**
         * constructor
         * @param startPos      start position within the intermediary code
         */
        ValFunctionUser(unsigned startPos): _startPos(startPos)         { }

        /**
         * calls the cmpl function call operation and store result in execution context, if this value has a special handling for it
         * @param ctx			execution context
         * @param arg			pointer to argument value
         * @return				true if this has handled the operation
         */
        virtual bool operCall(ExecContext *ctx, StackValue *arg) override;

    protected:
        virtual bool userDef() const override	{ return true; }	///< function is an user defined function (written as cmpl code)

	public:
		/**
		 * write contents of the object to a stream
		 * @param modp			calling module
		 * @param mode			mode for output: 0=direct; 1=part of other value
		 */
        virtual void write(ostream& ostr, ModuleBase *modp, int mode = 0) const override     { ostr << "<user defined function>"; }	//TODO: Hier auch Syntaxelement mit rein, das die Funktion darstellt
	};


    /**
	 * the <code>ValFunctionInternal</code> class is the base class for the internal cmpl functions
	 */
	class ValFunctionInternal : public ValFunction
	{
	protected:
        virtual bool userDef() const override	{ return false; }	///< function is an user defined function (written as cmpl code)


    public:
		/**
		 * write contents of the object to a stream
		 * @param modp			calling module
		 * @param mode			mode for output: 0=direct; 1=part of other value
		 */
        virtual void write(ostream& ostr, ModuleBase *modp, int mode = 0) const override     { ostr << "<predefined function '" << funcName() << "'>"; }
	};


	/****** aggregat functions ****/

    /**
	 * base class for internal cmpl functions for aggregating an array of values
	 */
	class ValFunctionAggregat : public ValFunctionInternal
	{
    public:
        /**
         * calls the cmpl function call operation and store result in execution context, if this value has a special handling for it
         * @param ctx			execution context
         * @param arg			pointer to argument value
         * @return				true
         */
        virtual bool operCall(ExecContext *ctx, StackValue *arg) override                { callForArrayAggr(ctx, ctx->opResult(), arg); return true; }
    };

    /**
	 * internal function "sum"
	 */
	class ValFunctionInternSum : public ValFunctionAggregat
	{
	protected:
        virtual const char *funcName() const override 		{ return "sum"; }			///< name of the internal function

	public:
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
        virtual bool operCallSimple(ExecContext *ctx, CmplVal& res, CmplVal& src, bool aggr, unsigned se, void *info = NULL) override;
	};

    /**
	 * internal function "max"
	 */
	class ValFunctionInternMax : public ValFunctionAggregat
	{
	protected:
        virtual const char *funcName() const override 		{ return "max"; }			///< name of the internal function

	public:
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
        virtual bool operCallSimple(ExecContext *ctx, CmplVal& res, CmplVal& src, bool aggr, unsigned se, void *info = NULL) override;

        /**
         * get whether function has special meaning for optimization direction
         * @return				0:no / 1:max / -1:min
         */
        virtual int optDir() const override											{ return 1; }
    };

    /**
	 * internal function "min"
	 */
	class ValFunctionInternMin : public ValFunctionAggregat
	{
	protected:
        virtual const char *funcName() const override 		{ return "min"; }			///< name of the internal function

	public:
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
        virtual bool operCallSimple(ExecContext *ctx, CmplVal& res, CmplVal& src, bool aggr, unsigned se, void *info = NULL) override;

        /**
         * get whether function has special meaning for optimization direction
         * @return				0:no / 1:max / -1:min
         */
        virtual int optDir() const override											{ return -1; }
    };

    /**
     * internal function "and"
     */
    class ValFunctionInternAnd : public ValFunctionAggregat
    {
    protected:
        virtual const char *funcName() const override 		{ return "and"; }			///< name of the internal function

    public:
        /**
         * calls the cmpl function call operation and store result in execution context, if this value has a special handling for it
         * @param ctx			execution context
         * @param arg			pointer to argument value
         * @return				true
         */
        virtual bool operCall(ExecContext *ctx, StackValue *arg) override;

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
        virtual bool operCallSimple(ExecContext *ctx, CmplVal& res, CmplVal& src, bool aggr, unsigned se, void *info = NULL) override;
    };

    /**
     * internal function "or"
     */
    class ValFunctionInternOr : public ValFunctionAggregat
    {
    protected:
        virtual const char *funcName() const override 		{ return "or"; }			///< name of the internal function

    public:
        /**
         * calls the cmpl function call operation and store result in execution context, if this value has a special handling for it
         * @param ctx			execution context
         * @param arg			pointer to argument value
         * @return				true
         */
        virtual bool operCall(ExecContext *ctx, StackValue *arg) override;

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
        virtual bool operCallSimple(ExecContext *ctx, CmplVal& res, CmplVal& src, bool aggr, unsigned se, void *info = NULL) override;
    };


	/****** array info functions ****/

    /**
	 * base class for internal cmpl functions for getting an info over an array of values
	 */
	class ValFunctionArrayInfo : public ValFunctionInternal
	{
	};

    /**
	 * internal function "count"
	 */
	class ValFunctionInternCount : public ValFunctionArrayInfo
	{
	protected:
        virtual const char *funcName() const override 		{ return "count"; }			///< name of the internal function

	public:
		/**
		 * calls the cmpl function call operation and store result in execution context, if this value has a special handling for it
		 * @param ctx			execution context
		 * @param arg			pointer to argument value
		 * @return				true
		 */
        virtual bool operCall(ExecContext *ctx, StackValue *arg) override;
	};

    /**
	 * internal function "def"
	 */
	class ValFunctionInternDef : public ValFunctionArrayInfo
	{
	protected:
        virtual const char *funcName() const override 		{ return "def"; }			///< name of the internal function

	public:
		/**
		 * calls the cmpl function call operation and store result in execution context, if this value has a special handling for it
		 * @param ctx			execution context
		 * @param arg			pointer to argument value
		 * @return				true
		 */
        virtual bool operCall(ExecContext *ctx, StackValue *arg) override;
	};

    /**
	 * internal function "valid"
	 */
	class ValFunctionInternValid : public ValFunctionArrayInfo
	{
	protected:
        virtual const char *funcName() const override 		{ return "valid"; }			///< name of the internal function

	public:
		/**
		 * calls the cmpl function call operation and store result in execution context, if this value has a special handling for it
		 * @param ctx			execution context
		 * @param arg			pointer to argument value
		 * @return				true
		 */
        virtual bool operCall(ExecContext *ctx, StackValue *arg) override;
	};

    /**
	 * internal function "dim"
	 */
	class ValFunctionInternDim : public ValFunctionArrayInfo
	{
	protected:
        virtual const char *funcName() const override 		{ return "dim"; }			///< name of the internal function

	public:
		/**
		 * calls the cmpl function call operation and store result in execution context, if this value has a special handling for it
		 * @param ctx			execution context
		 * @param arg			pointer to argument value
		 * @return				true
		 */
        virtual bool operCall(ExecContext *ctx, StackValue *arg) override;
	};

    /**
	 * internal function "defset"
	 */
	class ValFunctionInternDefset : public ValFunctionArrayInfo
	{
	protected:
        virtual const char *funcName() const override 		{ return "defset"; }		///< name of the internal function

	public:
		/**
		 * calls the cmpl function call operation and store result in execution context, if this value has a special handling for it
		 * @param ctx			execution context
		 * @param arg			pointer to argument value
		 * @return				true
		 */
        virtual bool operCall(ExecContext *ctx, StackValue *arg) override;
	};

    /**
	 * internal function "validset"
	 */
	class ValFunctionInternValidset : public ValFunctionArrayInfo
	{
	protected:
        virtual const char *funcName() const override 		{ return "validset"; }		///< name of the internal function

	public:
		/**
		 * calls the cmpl function call operation and store result in execution context, if this value has a special handling for it
		 * @param ctx			execution context
		 * @param arg			pointer to argument value
		 * @return				true
		 */
        virtual bool operCall(ExecContext *ctx, StackValue *arg) override;
	};


    /****** set and tuple info functions ****/

    /**
     * base class for internal cmpl functions for output of values
     */
    class ValFunctionSetInfo : public ValFunctionInternal
    {
    public:
        /**
         * calls the cmpl function call operation and store result in execution context, if this value has a special handling for it
         * @param ctx			execution context
         * @param arg			pointer to argument value
         * @return				true
         */
        virtual bool operCall(ExecContext *ctx, StackValue *arg) override                               { callForArrayElements(ctx, ctx->opResult(), arg); return true; }
    };

    /**
	 * internal function "len"
	 */
    class ValFunctionInternLen : public ValFunctionSetInfo
	{
	protected:
        virtual const char *funcName() const override 		{ return "len"; }			///< name of the internal function

	public:
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
        virtual bool operCallSimple(ExecContext *ctx, CmplVal& res, CmplVal& src, bool aggr, unsigned se, void *info = NULL) override;
	};

    /**
	 * internal function "rank"
	 */
    class ValFunctionInternRank : public ValFunctionSetInfo
	{
	protected:
        virtual const char *funcName() const override 		{ return "rank"; }			///< name of the internal function

	public:
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
        virtual bool operCallSimple(ExecContext *ctx, CmplVal& res, CmplVal& src, bool aggr, unsigned se, void *info = NULL) override;
	};

    /**
     * internal function "array"
     */
    class ValFunctionInternArray : public ValFunctionInternal
    {
    protected:
        virtual const char *funcName() const override 		{ return "array"; }			///< name of the internal function

    public:
        /**
         * calls the cmpl function call operation and store result in execution context, if this value has a special handling for it
         * @param ctx			execution context
         * @param arg			pointer to argument value
         * @return				true
         */
        virtual bool operCall(ExecContext *ctx, StackValue *arg) override;

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
        virtual bool operCallSimple(ExecContext *ctx, CmplVal& res, CmplVal& src, bool aggr, unsigned se, void *info = NULL) override;
    };


	/****** output functions ****/

    /**
	 * base class for internal cmpl functions for output of values
	 */
	class ValFunctionOutput : public ValFunctionInternal
	{
	};

    /**
	 * internal function "echo"
	 */
	class ValFunctionInternEcho : public ValFunctionOutput
	{
	protected:
        virtual const char *funcName() const override 		{ return "echo"; }			///< name of the internal function

	public:
		/**
		 * calls the cmpl function call operation and store result in execution context, if this value has a special handling for it
		 * @param ctx			execution context
		 * @param arg			pointer to argument value
		 * @return				true
		 */
        virtual bool operCall(ExecContext *ctx, StackValue *arg) override;

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
        virtual bool operCallSimple(ExecContext *ctx, CmplVal& res, CmplVal& src, bool aggr, unsigned se, void *info = NULL) override;

//    private:
//        /**
//         * simple array output
//         * @param ctx			execution context
//         * @param arr			array
//         */
//        void echoArraySimple(ExecContext *ctx, CmplArray *arr);
	};

    /**
	 * internal function "error"
	 */
	class ValFunctionInternError : public ValFunctionOutput
	{
	protected:
        virtual const char *funcName() const override 		{ return "error"; }			///< name of the internal function

	public:
		/**
		 * calls the cmpl function call operation and store result in execution context, if this value has a special handling for it
		 * @param ctx			execution context
		 * @param arg			pointer to argument value
		 * @return				true
		 */
        virtual bool operCall(ExecContext *ctx, StackValue *arg) override;
	};

    /**
	 * internal function "format"
	 */
	class ValFunctionInternFormat : public ValFunctionOutput
	{
	protected:
        virtual const char *funcName() const override 		{ return "format"; }		///< name of the internal function

	public:
		/**
		 * calls the cmpl function call operation and store result in execution context, if this value has a special handling for it
		 * @param ctx			execution context
		 * @param arg			pointer to argument value
		 * @return				true
		 */
        virtual bool operCall(ExecContext *ctx, StackValue *arg) override;

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
        virtual bool operCallSimple(ExecContext *ctx, CmplVal& res, CmplVal& src, bool aggr, unsigned se, void *info = NULL) override;

    private:
        /**
         * info structure used in operCallSimple()
         */
        struct Info
        {
            const char *fullformat = NULL;              ///< full format string (first argument of the function)
            unsigned formatse = 0;                      ///< syntax element id of format
            unsigned nxtformat = 0;                     ///< start position of next part in <code>fullformat</code>
            char formatbuf[105];                        ///< buffer for current format part copied from <code>fullformat</code>

            tp_e formattp = TP_EMPTY;                   ///< type of current format specifier (TP_REAL/TP_INT/TP_STR) / TP_EMPTY: no one
            bool strc = false;                          ///< true if TP_STR used for format specifier %c
            unsigned cntadd = 0;                        ///< count of additional needed arguments (e.g. for width) (max is 2)

            unsigned readadd = 0;                       ///< count of additional needed arguments already read
            int add1 = 0;                               ///< first additional argument
            int add2 = 0;                               ///< second additional argument

            char *buf = NULL;                           ///< output buffer
            size_t buflen = 0;                          ///< length of output buffer

            string *res;                                ///< pointer to result string

            Info(string *rp): res(rp)   { }             ///< constructor
            ~Info()                     { if (buf) delete buf; }    ///< destructor
        };

        /**
         * get next format specifier and set info for it
         * @param ctx           execution context
         * @param inf           info object
         * @return              false if error or end of format
         */
        bool extractNextFormat(ExecContext *ctx, Info *inf);

        /**
         * write value to result string using current format
         * @param ctx           execution context
         * @param val           value
         * @param se            syntax element id of the value
         * @param inf           info object
         */
        void writeValue(ExecContext *ctx, CmplVal& val, unsigned se, Info *inf);

        /**
         * printf the value to buffer
         * @param inf           info object
         * @param valr          value if real
         * @param vali          value if int
         * @param vals          value if string
         * @return              false if error
         */
        bool printfValue(Info *inf, realType valr, intType vali, const char *vals);
    };


	/****** integer division functions ****/

    /**
	 * base class for internal cmpl functions for division operation for integer numbers
	 */
	class ValFunctionDivInteger : public ValFunctionInternal
	{
    public:
        /**
         * calls the cmpl function call operation and store result in execution context, if this value has a special handling for it
         * @param ctx			execution context
         * @param arg			pointer to argument value
         * @return				true
         */
        virtual bool operCall(ExecContext *ctx, StackValue *arg) override;

        /**
         * executes the division operation for integer numbers
         * @param a1            first argument value
         * @param a2            second argument value
         * @return              result value
         */
        virtual intType execDiv(intType a1, intType a2) = 0;
    };

    /**
	 * internal function "div"
	 */
	class ValFunctionInternDiv : public ValFunctionDivInteger
	{
	protected:
        virtual const char *funcName() const override 		{ return "div"; }			///< name of the internal function

	public:
        /**
         * executes the division operation for integer numbers
         * @param a1            first argument value
         * @param a2            second argument value
         * @return              result value
         */
        virtual intType execDiv(intType a1, intType a2) override     { return a1 / a2; }
	};

    /**
	 * internal function "mod"
	 */
	class ValFunctionInternMod : public ValFunctionDivInteger
	{
	protected:
        virtual const char *funcName() const override 		{ return "mod"; }			///< name of the internal function

	public:
        /**
         * executes the division operation for integer numbers
         * @param a1            first argument value
         * @param a2            second argument value
         * @return              result value
         */
        virtual intType execDiv(intType a1, intType a2) override     { return a1 % a2; }
	};


	/****** random number functions ****/

    /**
	 * base class for internal cmpl functions for random number generation
	 */
	class ValFunctionGenerRand : public ValFunctionInternal
	{
	};

    /**
	 * internal function "srand"
	 */
	class ValFunctionInternSrand : public ValFunctionGenerRand
	{
	protected:
        virtual const char *funcName() const override 		{ return "srand"; }			///< name of the internal function

	public:
		/**
		 * calls the cmpl function call operation and store result in execution context, if this value has a special handling for it
		 * @param ctx			execution context
		 * @param arg			pointer to argument value
		 * @return				true
		 */
        virtual bool operCall(ExecContext *ctx, StackValue *arg) override;
	};

    /**
	 * internal function "rand"
	 */
	class ValFunctionInternRand : public ValFunctionGenerRand
	{
	protected:
        virtual const char *funcName() const override		{ return "rand"; }			///< name of the internal function

	public:
		/**
		 * calls the cmpl function call operation and store result in execution context, if this value has a special handling for it
		 * @param ctx			execution context
		 * @param arg			pointer to argument value
		 * @return				true
		 */
        virtual bool operCall(ExecContext *ctx, StackValue *arg) override;
	};

}

#endif // VALFUNCTION_HH

