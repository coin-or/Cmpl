
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


#ifndef MATHFUNCTIONS_HH
#define MATHFUNCTIONS_HH


#include <vector>

#include "../../Control/ExtensionBase.hh"
#include "../../CommonData/ExtensionSteps.hh"
#include "../../Modules/Interpreter/ValFunction.hh"


using namespace std;



namespace cmpl
{
	class MathFunctionsMod;

    /**
	 * the <code>MathFunctions</code> class is an extension to the compiler module
	 * which defines math functions for cmpl.
	 */
	class MathFunctions : public ExtensionBase
	{
		friend class MathFunctionsMod;

	private:
        vector<unsigned> _namespace;		///< namespace parts of the namespace for the functions
        int _baseId;						///< first registration id


	public:
        /**
         * constructor
         * @param mod			module creating this extension object
         */
        MathFunctions(ModuleBase *mod): ExtensionBase(mod), _namespace()	{ }

        /**
		 * constructor
		 * @param mod			module creating this extension object
         * @param ns            namespace parts of the namespace for the functions
		 */
        MathFunctions(ModuleBase *mod, const vector<unsigned>& ns): ExtensionBase(mod), _namespace(ns)	{ }


		/************** overwritten methods of <code>ExtensionBase</code> **********/

	protected:
		/**
		 * run the extension function
		 * @param mod			module calling the extension
		 * @param step			execution step within the module
		 * @param id			additional identificator
		 * @param par			additional parameter
		 */
		virtual void run(ModuleBase *mod, int step, int id, void *par);

		/**
		 * called after running the module, before deleting the module object
		 * @param mod			module calling the extension
		 * @param id			additional identificator
		 */
		virtual void moduleEnd(ModuleBase *mod, int id)		{ }

	public:
#if PROTO
		/**
		 * get name of the extension, only used for protocol output
		 */
		virtual const char *extName()				{ return "mathFunctions"; }
#endif //PROTO

		/**
		 * get count of sub data objects
		 */
		virtual int subCnt()						{ return 0; }

		/**
		 * write data object to output stream
		 * @param si        serialization info
		 * @param data      main data object
		 */
		virtual void serializeTo(MainData::SerializeInfo& si, const MainData *data);

		/**
		 * fill data object from input stream
		 * @param si        serialization info
		 * @param data      main data object
		 * @param subCnt    count of sub data objects
		 * @param lc		line count per element or 0 if elements are len coded
		 * @param rline		remaining section start line
		 */
		virtual void deserializeFrom(MainData::SerializeInfo& si, const MainData *data, int subCnt, int lc, string &rline);


		/************** extension registration **********/

	private:
		/**
		 * return count of functions to register
		 */
		unsigned countFunc();

	};



	/************** math functions **********/

    /**
	 * base class for math functions with one real argument and a real result value
	 */
	class MathFunctionRealToReal : public ValFunctionInternal
	{
	protected:
		virtual realType exec(realType arg) const = 0;								///< execute the math function to single argument

		virtual bool hasExecInt() const				{ return false; }				///< function has separate execution for int argument
        virtual intType execInt(intType arg) const	{ return 0; }					///< execution for int argument (only used if hasExecInd() == true)

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
	 * math function "sqrt"
	 */
	class MathFunctionSqrt : public MathFunctionRealToReal
	{
	protected:
        virtual const char *funcName() const override 		{ return "sqrt"; }				///< name of the math function
        virtual realType exec(realType arg) const override	{ return sqrt(arg); }			///< execute the math function to single argument
	};

    /**
	 * math function "exp"
	 */
	class MathFunctionExp : public MathFunctionRealToReal
	{
	protected:
        virtual const char *funcName() const override 		{ return "exp"; }				///< name of the math function
        virtual realType exec(realType arg) const override	{ return exp(arg); }			///< execute the math function to single argument
	};

    /**
	 * math function "ln"
	 */
	class MathFunctionLn : public MathFunctionRealToReal
	{
	protected:
        virtual const char *funcName() const override 		{ return "ln"; }				///< name of the math function
        virtual realType exec(realType arg) const override	{ return log(arg); }			///< execute the math function to single argument
	};

    /**
	 * math function "lg"
	 */
	class MathFunctionLg : public MathFunctionRealToReal
	{
	protected:
        virtual const char *funcName() const override 		{ return "lg"; }				///< name of the math function
        virtual realType exec(realType arg) const override	{ return log10(arg); }			///< execute the math function to single argument
	};

    /**
	 * math function "ld"
	 */
	class MathFunctionLd : public MathFunctionRealToReal
	{
	protected:
        virtual const char *funcName() const override 		{ return "ld"; }				///< name of the math function
        virtual realType exec(realType arg) const override	{ return log(arg)/log(2);; }	///< execute the math function to single argument
	};

    /**
	 * math function "sin"
	 */
	class MathFunctionSin : public MathFunctionRealToReal
	{
	protected:
        virtual const char *funcName() const override 		{ return "sin"; }				///< name of the math function
        virtual realType exec(realType arg) const override	{ return sin(arg); }			///< execute the math function to single argument
	};

    /**
	 * math function "cos"
	 */
	class MathFunctionCos : public MathFunctionRealToReal
	{
	protected:
        virtual const char *funcName() const override 		{ return "cos"; }				///< name of the math function
        virtual realType exec(realType arg) const override	{ return cos(arg); }			///< execute the math function to single argument
	};

    /**
	 * math function "tan"
	 */
	class MathFunctionTan : public MathFunctionRealToReal
	{
	protected:
        virtual const char *funcName() const override 		{ return "tan"; }				///< name of the math function
        virtual realType exec(realType arg) const override	{ return tan(arg); }			///< execute the math function to single argument
	};

    /**
	 * math function "asin"
	 */
	class MathFunctionAsin : public MathFunctionRealToReal
	{
	protected:
        virtual const char *funcName() const override 		{ return "asin"; }				///< name of the math function
        virtual realType exec(realType arg) const override	{ return asin(arg); }			///< execute the math function to single argument
	};

    /**
	 * math function "acos"
	 */
	class MathFunctionAcos : public MathFunctionRealToReal
	{
	protected:
        virtual const char *funcName() const override 		{ return "acos"; }				///< name of the math function
        virtual realType exec(realType arg) const override	{ return acos(arg); }			///< execute the math function to single argument
	};

    /**
	 * math function "atan"
	 */
	class MathFunctionAtan : public MathFunctionRealToReal
	{
	protected:
        virtual const char *funcName() const override 		{ return "atan"; }				///< name of the math function
        virtual realType exec(realType arg) const override	{ return atan(arg); }			///< execute the math function to single argument
	};

    /**
	 * math function "sinh"
	 */
	class MathFunctionSinh : public MathFunctionRealToReal
	{
	protected:
        virtual const char *funcName() const override 		{ return "sinh"; }				///< name of the math function
        virtual realType exec(realType arg) const override	{ return sinh(arg); }			///< execute the math function to single argument
	};

    /**
	 * math function "cosh"
	 */
	class MathFunctionCosh : public MathFunctionRealToReal
	{
	protected:
        virtual const char *funcName() const override 		{ return "cosh"; }				///< name of the math function
        virtual realType exec(realType arg) const override	{ return cosh(arg); }			///< execute the math function to single argument
	};

    /**
	 * math function "tanh"
	 */
	class MathFunctionTanh : public MathFunctionRealToReal
	{
	protected:
        virtual const char *funcName() const override 		{ return "tanh"; }				///< name of the math function
        virtual realType exec(realType arg) const override	{ return tanh(arg); }			///< execute the math function to single argument
	};

    /**
	 * math function "abs"
	 */
	class MathFunctionAbs : public MathFunctionRealToReal
	{
	protected:
        virtual const char *funcName() const override 		{ return "abs"; }				///< name of the math function
        virtual realType exec(realType arg) const override	{ return fabs(arg); }			///< execute the math function to single argument

        virtual bool hasExecInt() const override			{ return true; }				///< function has separate execution for int argument
        virtual intType execInt(intType arg) const override	{ return abs(arg); }			///< execution for int argument (only used if hasExecInd() == true)
	};

    /**
	 * math function "ceil"
	 */
	class MathFunctionCeil : public MathFunctionRealToReal
	{
	protected:
        virtual const char *funcName() const override 		{ return "ceil"; }				///< name of the math function
        virtual realType exec(realType arg) const override	{ return ceil(arg); }			///< execute the math function to single argument

        virtual bool hasExecInt() const override			{ return true; }				///< function has separate execution for int argument
        virtual intType execInt(intType arg) const override	{ return arg; }					///< execution for int argument (only used if hasExecInd() == true)
	};

    /**
	 * math function "floor"
	 */
	class MathFunctionFloor : public MathFunctionRealToReal
	{
	protected:
        virtual const char *funcName() const override 		{ return "floor"; }				///< name of the math function
        virtual realType exec(realType arg) const override	{ return floor(arg); }			///< execute the math function to single argument

        virtual bool hasExecInt() const override			{ return true; }				///< function has separate execution for int argument
        virtual intType execInt(intType arg) const override	{ return arg; }					///< execution for int argument (only used if hasExecInd() == true)
	};

    /**
	 * math function "round"
	 */
	class MathFunctionRound : public MathFunctionRealToReal
	{
	protected:
        virtual const char *funcName() const override 		{ return "round"; }				///< name of the math function
        virtual realType exec(realType arg) const override	{ return round(arg); }			///< execute the math function to single argument

        virtual bool hasExecInt() const override			{ return true; }				///< function has separate execution for int argument
        virtual intType execInt(intType arg) const override	{ return arg; }					///< execution for int argument (only used if hasExecInd() == true)
	};

		
}

#endif // MATHFUNCTIONS_HH

