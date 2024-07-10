
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


#ifndef LINEARCONDITIONAL_HH
#define LINEARCONDITIONAL_HH


#include "RemodelBase.hh"
#include "LinearLogCon.hh"


using namespace std;



namespace cmpl
{
    class Interpreter;
    class OptModel;
    class OptCon;
    class CmplVal;
    class LinearConditionalMod;
    class ValFormulaCompare;
    class AddPropOptVarConValCond;
    class ValFormulaCondOp;


    /*********** command line options delivered to the extension by run() **********/

    #define OPTION_EXT_NAMECONDVAR            91
    #define OPTION_EXT_NAMECONDCON            92



    /**
     * <code>LinearConditional</code> is the extension for
     * linearization of conditional constraints and constraints with conditional parts (direct or via conversion to logical connected constraints)
     */
    class LinearConditional : public LinearLogCon
	{
    private:
        unsigned _nameCondVar;      ///< name for variable which replaces a conditional expression
        unsigned _nameCondCon;      ///< name for constraint which sets the variable for a conditional expression
        atomic<unsigned> _nameInd;  ///< current index for variable or constraint

        mutex _chkLinMtx;           ///< mutex for check whether a variable for a conditional value is already linearized


    public:
        /**
         * constructor
         * @param mod			module creating this extension object
         */
        LinearConditional(LinearConditionalMod *mod);



		/************** overwritten methods of <code>ExtensionBase</code> **********/

	public:
#if PROTO
        /**
         * get name of the extension, only used for protocol output
         */
        const char *extName() override				{ return "linearConditional"; }
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
         * remodel all constraints and objectives
         * @param modp      intepreter module calling the extension
         * @param om		optimization model
         * @param size      count of potential work steps (usually count of matrix rows)
         */
        void remodelAll(Interpreter *modp, OptModel *om, unsigned long size) override;

        /**
         * remodel one constraint and objective
         * @param modp      intepreter module calling the extension
         * @param om		optimization model
         * @param oc        constraint and objective
         * @return          count of executed remodelations
         */
        unsigned remodelCon(Interpreter *modp, OptModel *om, OptCon *oc) override;

        /**
         * check if variable for conditional value is not linearized yet, and set flag for it
         * @param apvc      additional property for an optimization variable which is used as replacement for a conditional value
         * @return          true if not linearized yet
         */
        bool checkSetLinearizedVarConValCond(AddPropOptVarConValCond *apvc);

        /**
         * add constraint for variable used for a conditional value
         * @param modp      intepreter module calling the extension
         * @param om        optimization model
         * @param apvc      additional property for an optimization variable which is used as replacement for a conditional value
         * @param se        id of syntax element in the cmpl text creating the product of variables
         * @return          success: true / error: false
         */
        bool addConForValCond(Interpreter *modp, OptModel *om, AddPropOptVarConValCond *apvc, unsigned se);

        /**
         * linearization of a conditional constraint
         * @param modp      intepreter module calling the extension
         * @param om		optimization model
         * @param oc        conditional constraint / NULL: generate new constraint
         * @param fcond     conditional formula of the constraint
         * @param res       return of resulting formula for constraint / NULL: set resulting formula within oc
         * @param deloc     return whether oc must be removed
         * @return          true if ok / false if error
         */
        bool linearizeConditionalCond(Interpreter *modp, OptModel *om, OptCon *oc, ValFormulaCondOp *fcond, CmplVal *res, bool& deloc);

        //TODO
		
    };

}

#endif // LINEARCONDITIONAL_HH

