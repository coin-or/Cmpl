
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


#ifndef LINEARVARPROD_HH
#define LINEARVARPROD_HH


#include "RemodelBase.hh"


using namespace std;



namespace cmpl
{
    class Interpreter;
    class ExecContext;
    class OptModel;
    class OptCon;
    class CmplVal;
    class LinearVarProdMod;
    class ValFormulaVarProd;
    class ValFormulaVarProdOp;


    /*********** command line options delivered to the extension by run() **********/

    #define OPTION_EXT_USEBIGMBOUND             70

    #define OPTION_EXT_VARPRODLINEARLVL         80
    #define OPTION_EXT_PRODREALWARNLVL          81
    #define OPTION_EXT_VARPRODDECOMP            82

    #define OPTION_EXT_ATTACHNAMEVARDECOMP      90
    #define OPTION_EXT_ATTACHNAMEVARNORM        91


    /**
     * <code>LinearVarProdMod</code> is the extension module for
     * linearization of products of optimization variables
     */
    class LinearVarProd : public RemodelBase
	{
    private:
        /**
         * pair of variables, for use as key in the cache of variable products
         */
        struct VarPair {
            unsigned long v1;           ///< definition number of first variable
            unsigned long v2;           ///< definition number of second variable

            bool operator == (const VarPair& p2) const          { return (v1 == p2.v1 && v2 == p2.v2); }
        };

        /**
         * hash for varpair
         */
        struct VarPairHash {
            size_t operator() (VarPair const& p) const noexcept { return (hash<unsigned long>{}(p.v1) ^ (hash<unsigned long>{}(p.v2) << 1)); }
        };


        /**
         * info for binary decomposition of an integer variable
         */
        struct IntVarDecomp {
            unsigned long iv = 0;       ///< original integer variable
            unsigned long oc = 0;       ///< optimization constraint for decomposition
            intType lb;                 ///< lower bound of integer variable
            intType ub;                 ///< upper bound of integer variable
            vector<unsigned long> bv;   ///< vector of binary variables created for the decomposition

            /** get whether info object is used */
            explicit operator bool() const              { return (bool)iv; }

            /** get count of binary variables for decomposition */
            unsigned size() const                       { return bv.size(); }

            /** get factor to be used with binary variable i */
            unsigned long fact(unsigned i) const        { return (1ul << i); }
        };


    private:
        int _prodLinearLvl;                     ///< level of lineariztion of products of variables (0:no / 1:only bin variable / 2:also int variable / 3:also approximation for real variables)
        int _prodRealWarn;                      ///< only for _prodLvl == 3: show warning if approximating a product of real variables (0:no / 1:yes for first such product / 2:yes for each such product)
        bool _hasWarnedProdReal;                ///< warning is already shown

        bool _prodDecomp;                       ///< for non-linearized products of more than two variables decomposite to products each with only two variables
        bool _useBigMBound;                     ///< use bigM as bound if a variable within a product has no bounds

        unsigned _attachNameVarDecomp;          ///< postfix for binary variable name for decomposition of an integer variable
        unsigned _attachNameVarNorm;            ///< postfix for variable name for normalization of a real variable

        RemodelCache<unordered_map<VarPair, unsigned long, VarPairHash>> _varProdCache;     ///< cache for product of two optimization variables
        RemodelCache<unordered_map<unsigned long, IntVarDecomp>> _intBinCache;              ///< cache for binary decomposition of an integer variable
        RemodelCache<unordered_map<unsigned long, unsigned long>> _realNormCache;           ///< cache for normalization of a real variable

	public:
        /**
         * constructor
         * @param mod			module creating this extension object
         */
        LinearVarProd(LinearVarProdMod *mod);



		/************** overwritten methods of <code>ExtensionBase</code> **********/

	public:
#if PROTO
        /**
         * get name of the extension, only used for protocol output
         */
        const char *extName() override				{ return "linearVarProd"; }
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
         * get whether remodeling is enabled
         */
        bool remodelEnabled() override                              { return (_remodelEnabled && (_prodLinearLvl || _prodDecomp)); }

        /**
         * initialization before remodeling
         * @param modp      intepreter module calling the extension
         * @param om		optimization model
         */
        void remodelInit(Interpreter *modp, OptModel *om) override  { _hasWarnedProdReal = false; }

        /**
         * remodel one constraint and objective
         * @param modp      intepreter module calling the extension
         * @param om		optimization model
         * @param oc        constraint and objective
         * @return          count of executed remodelations
         */
        unsigned remodelCon(Interpreter *modp, OptModel *om, OptCon *oc) override;

        /**
         * search recursive for product of optimization variables and linearize and replace all found
         * @param modp      intepreter module calling the extension
         * @param om		optimization model
         * @param f         formula to search in
         * @return          count of executed remodelations
         */
        unsigned remodelRecVarProd(Interpreter *modp, OptModel *om, ValFormula *f);

        /**
         * comparison order of optimization variables for products of variables
         * @param v1        first variable
         * @param v2        second variable
         * @return          true if v1 should be used as first factor in a product
         */
        static bool compVarLess(OptVar *v1, OptVar *v2);

        /**
         * linearize product of two or more optimization variables
         * @param modp      intepreter module calling the extension
         * @param om        optimization model
         * @param vvp       formula with product of optimization variables
         * @param cnt       count for linearizations
         * @return          new optimization variable representing the product / NULL: don't replace product
         */
        OptVar *linearizeVarProds(Interpreter *modp, OptModel *om, ValFormulaVarProdOp *vvp, unsigned& cnt);

        /**
         * linearize or decomposite product of two optimization variables
         * @param modp      intepreter module calling the extension
         * @param om        optimization model
         * @param v1        first optimization variable
         * @param v2        second optimization variable
         * @param se        id of syntax element in the cmpl text creating the product of variables
         * @param lin       linearize product
         * @return          new optimization variable representing the product
         */
        OptVar *linearizeVarProd(Interpreter *modp, OptModel *om, OptVar *v1, OptVar *v2, unsigned se, bool lin);

        /**
         * check if one variable for the product is fixed (or invalid) by its bounds, and make the product then
         * @param modp      intepreter module calling the extension
         * @param om        optimization model
         * @param v1        first optimization variable
         * @param v2        second optimization variable
         * @param se        id of syntax element in the cmpl text creating the product of variables
         * @return          if one variable is fixed, then new optimization variable representing the product, else NULL
         */
        OptVar *checkLinearizeFixedProd(Interpreter *modp, OptModel *om, OptVar *v1, OptVar *v2, unsigned se);

        /**
         * decomposite product of two optimization variables by setting a new variable for the product
         * @param modp      intepreter module calling the extension
         * @param om        optimization model
         * @param v1        first optimization variable
         * @param v2        second optimization variable
         * @param se        id of syntax element in the cmpl text creating the product of variables
         * @return          new optimization variable representing the product
         */
        OptVar *decompositeProd(Interpreter *modp, OptModel *om, OptVar *v1, OptVar *v2, unsigned se);

        /**
         * linearize product of a binary variable and another variable
         * @param modp      intepreter module calling the extension
         * @param om        optimization model
         * @param v1        binary optimization variable
         * @param v2        other optimization variable
         * @param se        id of syntax element in the cmpl text creating the product of variables
         * @return          new optimization variable representing the product
         */
        OptVar *linearizeProdBin(Interpreter *modp, OptModel *om, OptVar *v1, OptVar *v2, unsigned se);

        /**
         * linearize product of an integer variable and another variable
         * @param modp      intepreter module calling the extension
         * @param om        optimization model
         * @param v1        integer optimization variable
         * @param v2        other optimization variable
         * @param se        id of syntax element in the cmpl text creating the product of variables
         * @return          new optimization variable representing the product
         */
        OptVar *linearizeProdInt(Interpreter *modp, OptModel *om, OptVar *v1, OptVar *v2, unsigned se);

        /**
         * binary decomposition of an integer variable
         * @param modp      intepreter module calling the extension
         * @param om        optimization model
         * @param v1        integer optimization variable
         * @param se        id of syntax element in the cmpl text creating the product of variables
         * @return          reference to info for decomposition, stored in cache _intBinCache
         */
        IntVarDecomp& binaryDecomp(Interpreter *modp, OptModel *om, OptVar *v1, unsigned se);

        /**
         * linearize product of two real optimization variables by an approximation
         * @param modp      intepreter module calling the extension
         * @param om        optimization model
         * @param v1        first optimization variable
         * @param v2        second optimization variable
         * @param se        id of syntax element in the cmpl text creating the product of variables
         * @return          new optimization variable representing the product
         */
        OptVar *linearizeProdReal(Interpreter *modp, OptModel *om, OptVar *v1, OptVar *v2, unsigned se);

        /**
         * set lower and upper bound for product variable
         * @param ctx		execution context
         * @param pv        new optimization variable representing the product
         * @param lb1       first lower bound or TP_EMPTY if no such bound
         * @param ub1       first upper bound or TP_EMPTY if no such bound
         * @param lb2       second lower bound or TP_EMPTY if no such bound
         * @param ub2       second upper bound or TP_EMPTY if no such bound
         * @param se        id of syntax element in the cmpl text creating the product of variables
         */
        void setBoundsForProduct(ExecContext *ctx, OptVar *pv, CmplVal& lb1, CmplVal& ub1, CmplVal& lb2, CmplVal& ub2, unsigned se);
    };

}

#endif // LINEARVARPROD_HH

