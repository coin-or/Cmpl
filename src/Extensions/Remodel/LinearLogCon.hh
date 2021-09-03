
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


#ifndef LINEARLOGCON_HH
#define LINEARLOGCON_HH


#include "RemodelBase.hh"


using namespace std;



namespace cmpl
{
    class Interpreter;
    class OptModel;
    class OptCon;
    class CmplVal;
    class LinearLogConMod;
    class ValFormulaCompare;


    /*********** command line options delivered to the extension by run() **********/

    #define OPTION_EXT_ATTACHNAMECONAND         80

    #define OPTION_EXT_ATTACHNAMECONOR          82
    #define OPTION_EXT_ATTACHNAMEVAROR          83

    #define OPTION_EXT_ATTACHNAMECONDLTNEG      84
    #define OPTION_EXT_ATTACHNAMEVARDLTNEG      85



    /**
     * <code>LinearLogConMod</code> is the extension Module for
     * linearization of constraints connected by logical operations
     */
    class LinearLogCon : public RemodelBase
	{
    private:
        unsigned _attachNameConAnd;       ///< postfix for constraint name for linearization of logical And

        unsigned _attachNameConOr;        ///< postfix for constraint name for linearization of logical Or
        unsigned _attachNameVarOr;        ///< postfix for binary variable name for linearization of logical Or

        unsigned _attachNameConDltNeg;    ///< postfix for delta constraint name for linearization of negated comparison
        unsigned _attachNameVarDltNeg;    ///< postfix for delta variable name for linearization of negated comparison

        RemodelCache<unordered_map<ValFormula *, OptVar *>> _linOrCache;    ///< cache for linearizations of logical Or

        const bool _frstReplOC = false;     ///< first generated constraint replace original constraint

	public:
        /**
         * constructor
         * @param mod			module creating this extension object
         */
        LinearLogCon(LinearLogConMod *mod);



		/************** overwritten methods of <code>ExtensionBase</code> **********/

	public:
#if PROTO
        /**
         * get name of the extension, only used for protocol output
         */
        virtual const char *extName()				{ return "linearLogCon"; }
#endif //PROTO

        /**
         * run the extension function for processing a command line option
         * @param mod			module calling the extension
         * @param step			execution step within the module
         * @param id			additional identificator
         * @param ref           reference number of option registration, should be used for discriminate the options
         * @param prio          priority value of option
         * @param opt           command line option
         * @param par			additional parameter
         * @return              true if option is used by the extension
         */
        bool run(ModuleBase *mod, int step, int id, int ref, int prio, CmdLineOptList::SingleOption *opt, void *par) override;

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


        /************** linearization **********/
    private:
        /**
         * remodel one constraint and objective
         * @param modp      intepreter module calling the extension
         * @param om		optimization model
         * @param oc        constraint and objective
         * @return          count of executed remodelations
         */
        virtual unsigned remodelCon(Interpreter *modp, OptModel *om, OptCon *oc);

        /**
         * generate constraints from the linearization result
         * @param modp      intepreter module calling the extension
         * @param om		optimization model
         * @param oc        original constraint
         * @param frml      formula with linearization result
         * @param name      base name for new constraints
         * @param tpl       base tuple for new constraints
         * @param repl      replace original constraint
         */
        bool formulaToCon(Interpreter *modp, OptModel *om, OptCon *oc, CmplVal& frml, string& name, CmplVal& tpl, bool repl);

        /**
         * recursive linearization of a logical connected expression
         * @param modp      intepreter module calling the extension
         * @param om		optimization model
         * @param oc        original constraint
         * @param frml      formula with logical connected expression
         * @param oneg      outer negation
         * @param name      base name for new constraint and optimization variables
         * @param tpl       base index tuple for new constraint and optimization variables
         * @param res       return of resulting constraint as formula
         * @return          true if ok / false if error
         */
        bool remodelRec(Interpreter *modp, OptModel *om, OptCon *oc, CmplVal& frml, bool oneg, string& name, CmplVal& tpl, CmplVal& res);

        /**
         * linearization of parts connected by logical And
         * @param modp      intepreter module calling the extension
         * @param om		optimization model
         * @param oc        original constraint
         * @param parts     parts connected by logical And
         * @param neg       negation of every part
         * @param name      base name for new constraint and optimization variables / empty: no name
         * @param tpl       base index tuple for new constraint and optimization variables / empty or TP_NULL: no tuple
         * @param res       return of resulting constraint as formula
         * @return          true if ok / false if error
         */
        bool linearizeAnd(Interpreter *modp, OptModel *om, OptCon *oc, vector<CmplVal>& parts, bool neg, string& name, CmplVal& tpl, CmplVal& res);

        /**
         * linearization of parts connected by logical Or
         * @param modp      intepreter module calling the extension
         * @param om		optimization model
         * @param oc        original constraint
         * @param parts     parts connected by logical Or
         * @param neg       negation of every part
         * @param name      base name for new constraint and optimization variables / empty: no name
         * @param tpl       base index tuple for new constraint and optimization variables / empty or TP_NULL: no tuple
         * @param res       return of resulting constraint as formula
         * @return          true if ok / false if error
         */
        bool linearizeOr(Interpreter *modp, OptModel *om, OptCon *oc, vector<CmplVal>& parts, bool neg, string& name, CmplVal& tpl, CmplVal& res);

        /**
         * generate constraint for linearization of a part of logical Or
         * @param modp      intepreter module calling the extension
         * @param om		optimization model
         * @param oc        original constraint
         * @param frml      formula of the part
         * @param vbv       binary variable to include in the formula of the part
         * @param name      name for new constraint / empty: no name
         * @param tpl       index tuple for new constraint
         */
        void constraintForOrPart(Interpreter *modp, OptModel *om, OptCon *oc, CmplVal& frml, CmplVal& vbv, string& name, CmplVal& tpl);

        /**
         * linearization of negated comparison formula
         * @param modp      intepreter module calling the extension
         * @param om		optimization model
         * @param oc        original constraint
         * @param fc        comparison formula to negate
         * @param name      base name for new constraint and optimization variables / empty: no name
         * @param tpl       base index tuple for new constraint and optimization variables / empty or TP_NULL: no tuple
         * @param res       return of resulting constraint as formula
         * @return          true if ok / false if error
         */
        bool linearizeNeg(Interpreter *modp, OptModel *om, OptCon *oc, ValFormulaCompare *fc, string& name, CmplVal& tpl, CmplVal& res);
    };

}

#endif // LINEARLOGCON_HH

