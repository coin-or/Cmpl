
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


#ifndef BOUNDROW_HH
#define BOUNDROW_HH


#include "RemodelBase.hh"


using namespace std;



namespace cmpl
{
    class Interpreter;
    class OptModel;
    class OptCon;
    class CmplVal;
    class BoundRowMod;
    class ValFormulaLogCon;
    class ValFormulaCompare;


    /*********** command line options delivered to the extension by run() **********/

    #define OPTION_EXT_DELALLBOUNDROW		70
    #define OPTION_EXT_DELUNMBOUNDROW		71


    /**
     * the <code>BoundRow</code> class replaces rows with only one variable by bounds
     */
    class BoundRow : public RemodelBase
	{
    private:
        bool _delAllBoundRow;           ///< remove all rows which are replaced by bounds
        bool _delUnnamedBoundRow;       ///< remove only unnamed rows which are replaced by bounds (only when !_delAllBoundRow)

        RemodelCache<unordered_map<unsigned long, int>> _accessGuardCache;          ///< pseudo cache only as access guard

	public:
        /**
         * constructor
         * @param mod			module creating this extension object
         */
        BoundRow(BoundRowMod *mod);



		/************** overwritten methods of <code>ExtensionBase</code> **********/

	public:
#if PROTO
        /**
         * get name of the extension, only used for protocol output
         */
        const char *extName() override				{ return "boundRow"; }
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
    protected:
        /**
         * remodel one constraint and objective
         * @param modp      intepreter module calling the extension
         * @param om		optimization model
         * @param oc        constraint and objective
         * @return          count of executed remodelations
         */
        unsigned remodelCon(Interpreter *modp, OptModel *om, OptCon *oc) override;

        /**
         * check a logical-and formula whether its parts can be remodeled as bounds, and make this remodeling
         * @param modp      intepreter module calling the extension
         * @param fl        not negated logical-and formula
         * @param del       return if the whole formula can be deleted after remodeling
         * @return          count of executed remodelations
         */
        unsigned checkRemodelBound(Interpreter *modp, ValFormulaLogCon *fl, bool& del);

        /**
         * check a compare formula whether it can be remodeled as a bound, and make this remodeling
         * @param modp      intepreter module calling the extension
         * @param fc        compare formula
         * @param del       return if the formula can be deleted after remodeling
         * @return          count of executed remodelations
         */
        unsigned checkRemodelBound(Interpreter *modp, ValFormulaCompare *fc, bool& del);

        /**
         * check and set bound for an optimization variable
         * @param modp      intepreter module calling the extension
         * @param ov        optimization variable
         * @param bnd       bound value
         * @param le        variable must be lesser equal bound
         * @param ge        variable must be greater equal bound
         * @param neg       condition is negated (never with: le && ge)
         * @param del       return if the formula can be deleted after remodeling
         * @return          false if no valid range is left for the variable
         */
        bool checkSetBound(Interpreter *modp, OptVar *ov, CmplVal *bnd, bool le, bool ge, bool neg, bool& del);

        /**
         * check and set lower or upper bound for an optimization variable
         * @param ov        optimization variable
         * @param bnd       bound value
         * @param upp       true: upper bound / false: lower bound
         * @param invalid   return whether no valid range is left for the variable
         * @return          1: bound new set / 0: bound has already value bnd / -1: otherwise
         */
        int checkSetSingleBound(OptVar *ov, CmplVal *bnd, bool upp, bool& invalid);
    };

}

#endif // BOUNDROW_HH

