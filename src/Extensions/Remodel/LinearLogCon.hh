
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
    class ValFormulaLogConOp;
    class ValFormulaCompare;
    class ValFormulaCompareOp;


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
    protected:
        unsigned _attachNameConAnd;       ///< postfix for constraint name for linearization of logical And

        unsigned _attachNameConOr;        ///< postfix for constraint name for linearization of logical Or
        unsigned _attachNameVarOr;        ///< postfix for binary variable name for linearization of logical Or

        unsigned _attachNameConDltNeg;    ///< postfix for delta constraint name for linearization of negated comparison
        unsigned _attachNameVarDltNeg;    ///< postfix for delta variable name for linearization of negated comparison

        RemodelCache<unordered_map<FormulaCacheKey, OptVar *, FormulaCacheKeyHash>> _linOrCache;    ///< cache for linearizations of logical Or

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
        const char *extName() override				{ return "linearLogCon"; }
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
        int subCnt() override						{ return 0; }
										   
		/**
		 * write data object to output stream
		 * @param si        serialization info
		 * @param data      main data object
		 */
        void serializeTo(MainData::SerializeInfo& si, const MainData *data) override;

		/**
		 * fill data object from input stream
		 * @param si        serialization info
		 * @param data      main data object
		 * @param subCnt    count of sub data objects
		 * @param lc		line count per element or 0 if elements are len coded
		 * @param rline		remaining section start line
		 */
        void deserializeFrom(MainData::SerializeInfo& si, const MainData *data, int subCnt, int lc, string &rline) override;


        /************** linearization **********/
    private:
        /**
         * remodel one constraint and objective
         * @param modp      intepreter module calling the extension
         * @param om		optimization model
         * @param oc        constraint and objective
         * @return          count of executed remodelations
         */
        unsigned remodelCon(Interpreter *modp, OptModel *om, OptCon *oc) override;

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


        /************** spezial linearisation for or-connected binary optimization variables **********/
    protected:
        /**
         * or-connected binary optimization variables
         */
        struct OrConnectedBinVars {
            vector<pair<OptVar *, short>> bins;         ///< binary variables. pair second part is: 1 (non negated bin var) / 0 (negated bin var) / -1 (second bin var within expression of one condition)

            bool empty() const                                          { return bins.empty(); }
            size_t size() const                                         { return bins.size(); }
            pair<OptVar *, short>& operator [] (size_t i)               { return bins[i]; }
            bool operator == (const OrConnectedBinVars& p2) const       { return (bins == p2.bins); }

            void add(OptVar *bv, short i)                               { bins.emplace_back(pair<OptVar *, short>(bv, i)); }
        };

    private:
        /**
         * hash for OrConnectedBinVars
         */
        struct OrConnectedBinVarsHash {
            size_t operator() (OrConnectedBinVars const& p) const noexcept { return (hash<unsigned long>{}((p.bins.size() > 0 ? (unsigned long)(p.bins[0].first) + p.bins[0].second : 0UL) + 3*p.bins.size())); }
        };

        /**
         * cache for linearization of or-connected binary optimization variables.
         * value is number of a binary variable which has value 0, if no one of the conditions in the key is fulfilled.
         */
        RemodelCache<unordered_map<OrConnectedBinVars, unsigned long, OrConnectedBinVarsHash>> _orBinsCache;

    protected:
        /**
         * linearization of formula or-connected with binary variables
         * @param modp      intepreter module calling the extension
         * @param om		optimization model
         * @param oc        original constraint / NULL: no one
         * @param frm       formula / NULL: use formula from oc
         * @param bins      or-connected binary variables
         * @param res       return of resulting formula / TP_BIN if fixed result
         * @return          true if ok / false if error
         */
        bool linearizeFormulaOrBins(Interpreter *modp, OptModel *om, OptCon *oc, CmplVal *frm, OrConnectedBinVars &bins, CmplVal& res);


        /************** express fulfillment of a comparison formula by a binary optimization variable **********/
    private:
        /**
         * key for cache used in linearizeCondToBin()
         */
        struct CondToBinCacheKey {
            CmplValAuto frm;            ///< formula (can only be TP_FORMULA or TP_BIN)
            bool alsoNeg;               ///< also non-fulfillment of the formula is expressed by the value of the binary variable

            CondToBinCacheKey(CmplVal& vf, bool an): frm(vf), alsoNeg(an)                   { }
            CondToBinCacheKey(ValFormula *f, bool an): frm(), alsoNeg(an)                   { frm.set(TP_FORMULA, (CmplObjBase *)f); }
            CondToBinCacheKey(bool fb, bool an): frm(), alsoNeg(an)                         { frm.set(TP_BIN, fb); }

            bool operator == (const CondToBinCacheKey& p2) const;
        };

        /**
         * hash for CondToBinCacheKey
         */
        struct CondToBinCacheKeyHash {
            size_t operator() (CondToBinCacheKey const& p) const noexcept;
        };

        /**
         * value for cache used in linearizeCondToBin()
         */
        struct CondToBinCacheVal {
            CmplValAuto var1;           ///< binary variable (can only be TP_OPT_VAR or TP_EMPTY)
            CmplValAuto var2;           ///< second binary variable when needed (then the sum of both variables express the fulfillment of the formula) (can only be TP_OPT_VAR or TP_EMPTY)
            unsigned binval;            ///< binary value which express fulfillment of the formula (can only be 0 or 1)
            bool fixed;                 ///< fixed value (if a binary variable is stored in var1, then it is fixed to value 1)
            bool used;                  ///< flag whether this entry is used

            CondToBinCacheVal(): var1(), var2(), binval(0), fixed(false), used(false)       { }
            CondToBinCacheVal(CmplVal& v1, CmplVal& v2, unsigned bv, bool fx): var1(v1), var2(v2), binval(bv), fixed(fx), used(true)    { }

            inline explicit operator bool() const                               { return used; }

            /**
             * set result for LinearLogCon::linearizeCondToBin() from this
             * @param modp      intepreter module calling the extension
             * @param om		optimization model
             * @param llc       calling object
             * @param se        syntax element
             * @param neg       set negation of this as result
             * @param fixVar    true: if fulfillment of the formula is already known then generate a fixed binary optimization variable / false: then return as TP_BIN
             * @param res       resulting binary variable (TP_OPT_VAR) (can also be TP_BIN if !fixVar)
             * @param res2      only used if two binary variables are needed, and the sum of it express the fulfillment of the formula, then second resulting binary variable
             * @param negres    if fulfillment of the formula is expressed by value 0 of the binary variable, then true is returned here
             * @return          count of resulting binary variables (0 - 2)
             */
            int getRes(Interpreter *modp, OptModel *om, LinearLogCon *llc, unsigned se, bool neg, bool fixVar, CmplVal& res, CmplVal& res2, bool &negres) const;
        };

        RemodelCache<unordered_map<CondToBinCacheKey, CondToBinCacheVal, CondToBinCacheKeyHash>> _condToBinCache;     ///< cache for linearization of the fulfillment of a formula to a new binary optimization variable

    protected:
        /**
         * linearization of a formula to a new binary optimization variable, so that the fulfillment of the formula is expressed by the value of the binary variable
         * @param modp      intepreter module calling the extension
         * @param om		optimization model
         * @param oc        original constraint / NULL: no one
         * @param frm       formula
         * @param oneg      outer negation
         * @param alsoNeg   also non-fulfillment of the formula must be expressed by the value of the binary variable
         * @param fixVar    true: if fulfillment of the formula is already known then generate a fixed binary optimization variable / false: then return as TP_BIN
         * @param res       resulting binary variable (TP_OPT_VAR) (can also be TP_BIN if !fixVar)
         * @param res2      only used if two binary variables are needed, and the sum of it express the fulfillment of the formula, then second resulting binary variable
         * @param negres    if fulfillment of the formula is expressed by value 0 of the binary variable, then true is returned here
         * @return          count of resulting binary variables (0 - 2) / -1: not linearized
         */
        int linearizeCondToBin(Interpreter *modp, OptModel *om, OptCon *oc, CmplVal& frm, bool oneg, bool alsoNeg, bool fixVar, CmplVal& res, CmplVal& res2, bool& negres);

    private:
        /**
         * linearization of a compare formula to a new binary optimization variable, so that the fulfillment of the formula is expressed by the value of the binary variable
         * @param modp      intepreter module calling the extension
         * @param om		optimization model
         * @param se        syntax element
         * @param frm       formula
         * @param lin1      generate constraints and variables to express fulfillment of the formula
         * @param lin2      generate additional constraints to express non-fulfillment of the formula
         * @param bvar1     resulting binary variable (TP_OPT_VAR)
         * @param bvar2     only used if two binary variables are needed, and the sum of it express the fulfillment of the formula, then second resulting binary variable
         * @param binval    return of the value of the binary variable, which expresses fulfillment of the formula (only 0 or 1)
         * @param resfix    return whether the formula has a fixed result (then bvar1 and bvar2 are not used)
         * @return          whether non-fulfillment of the formula is also expressed by the resulting binary variable
         */
        bool linearizeCmpToBin(Interpreter *modp, OptModel *om, unsigned se, ValFormulaCompareOp *frm, bool lin1, bool lin2, CmplVal& bvar1, CmplVal& bvar2, unsigned& binval, bool& resfix);

        /**
         * linearization of a logical connected formula to a new binary optimization variable, so that the fulfillment of the formula is expressed by the value of the binary variable
         * @param modp      intepreter module calling the extension
         * @param om		optimization model
         * @param se        syntax element
         * @param frm       formula
         * @param lin1      generate constraints and variables to express fulfillment of the formula
         * @param lin2      generate additional constraints to express non-fulfillment of the formula
         * @param bvar1     resulting binary variable (TP_OPT_VAR)
         * @param bvar2     only used if two binary variables are needed, and the sum of it express the fulfillment of the formula, then second resulting binary variable
         * @param binval    return of the value of the binary variable, which expresses fulfillment of the formula (only 0 or 1)
         * @param resfix    return whether the formula has a fixed result (then bvar1 and bvar2 are not used)
         * @return          whether non-fulfillment of the formula is also expressed by the resulting binary variable
         */
        bool linearizeLogToBin(Interpreter *modp, OptModel *om, unsigned se, ValFormulaLogConOp *frm, bool lin1, bool lin2, CmplVal& bvar1, CmplVal& bvar2, unsigned& binval, bool& resfix);
    };

}

#endif // LINEARLOGCON_HH

