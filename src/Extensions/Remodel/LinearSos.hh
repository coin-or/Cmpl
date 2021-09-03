
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


#ifndef LINEARSOS_HH
#define LINEARSOS_HH

#include <list>
#include <array>

#include "RemodelBase.hh"
#include "LinearSosMod.hh"
#include "../../Modules/Interpreter/ValFunction.hh"
#include "../../Modules/OutModel/OutModelExtData.hh"


using namespace std;



namespace cmpl
{
    class Interpreter;
    class OptModel;
    class OptCon;
    class CmplVal;
    class LinearSosMod;
    class SosFunction;
    class SosContFunctionBase;
    class SosContFunctionName;
    class SosContFunctionAdd;
    class SosContFunctionAsVar;
    class SosContFunctionAsString;


    /*********** command line options delivered to the extension by run() **********/

    #define OPTION_EXT_SOSNATIVE                81
    #define OPTION_EXT_SOS2NATIVE               82

    #define OPTION_EXT_WARNINVALID              85

    #define OPTION_EXT_ATTACHNAMEVARSOS         90
    #define OPTION_EXT_ATTACHNAMEVARSOSNEG      91
    #define OPTION_EXT_ATTACHNAMECONSOS         92

    #define OPTION_EXT_ATTACHNAMEVARSOS2        95
    #define OPTION_EXT_ATTACHNAMECONSOS2        96


    /**
     * <code>LinearSosMod</code> is the extension Module for
	 * subsymbols for SOS and SOS2 definition and the possible linearization of SOS.
     */
    class LinearSos : public RemodelBase
	{
        friend class LinearSosMod;
        friend class SosFunction;
        friend class SosContFunctionBase;
        friend class SosContFunctionName;
        friend class SosContFunctionAdd;
        friend class SosContFunctionAsVar;
        friend class SosContFunctionAsString;



    private:
        /**
         * store for one SOS or SOS2
         */

        struct SOSStore
        {
            bool sos2;                      ///< true: SOS2 / false: SOS
            unsigned syntaxElem;            ///< defining syntax element

            vector<unsigned long> vars;     ///< identy numbers of the concerning optimization variables in their order
            bool linearized;                ///< SOS is already linearized

            unsigned name;                  ///< name for SOS / 0: no one
            unsigned name2;                 ///< separate name for SOS2 / 0: no one

            // following properties are used only if SOS was defined also containing parameters instead of variables
            bool noSolution;                ///< SOS cannot have a valid solution because of contained non zero parameters

            unsigned cntConst;              ///< count of non zero parameters
            vector<unsigned> posConst;      ///< position of parameters within <code>vars</code>, then there 0 for zero parameter and 1 for non zero parameter (only used for SOS2)
            vector<unsigned long> vars2;    ///< only used for SOS2 containing constant parameters, and only used when model output, then here only variables without parameters

            /**
             * constructor
             */
            SOSStore(bool s2, unsigned se): sos2(s2), syntaxElem(se), vars(), linearized(false), name(0), name2(0), noSolution(false), cntConst(0), posConst()      { }

            /**
             * add optimization variable (or parameter) to SOS
             * @param ctx			execution context
             * @param src			source value
             * @param se			syntax element id of source value
             */
            void addSOSVar(ExecContext *ctx, CmplVal& src, unsigned se);

            /**
             * write info for sos to stream
             * @param ostr          stream
             * @param modp          interpreter module
             */
            void write(ostream& ostr, Interpreter *modp);
        };

        private:
            SOSStore *sos;


        /**
         * info object for one component of SOS or SOS2, used only in linearization
         */
        struct SOSLinearizeInfo
        {
            bool isVar = true;                  ///< true: variable / false: parameter
            unsigned long varUB = 0;            ///< identy number + 1 of generated binary variable for upper bound / 0: no such variable / if !isVar then instead of: 0 for zero parameter and 1 for non zero parameter
            unsigned long varLB = 0;            ///< identy number + 1 of generated binary variable for upper bound / 0: no such variable
            unsigned long varOrder = 0;         ///< identy number + 1 of generated binary variable for order within SOS2 / 0: no such variable
        };

        /**
         * interface class for data transfer to modules for model output.
         * used for base classes OutModelExtDataSOS1 (indirect base) and OutModelExtDataSOS2
         */
        class OutModelExtData : public OutModelExtDataSOS2
        {
            friend class LinearSos;

        private:
            LinearSos *_ext;                 ///< extension object
            bool _sos2;                      ///< true: SOS2 / false: SOS
            bool _linearized;                ///< true: SOS is already linearized
            const LocationInfo *_fLoc;       ///< location info of first element

            vector<unsigned> _rows;          ///< index numbers within LinearSos::_storeSos

            /**
             * get index number within LinearSos::_transfer
             */
            static unsigned num(bool sos2, bool linearized)     { return (sos2 ? 2 : 0) + (linearized ? 1 : 0); }

        public:
            /**
             * get data for one SOS
             * @param info          info for special extension data
             * @param row           data row number, must be lesser than info.rows
             * @param vars          return of identy numbers of the optimization variables in the SOS
             * @param name          name for SOS / 0: no name given
             * @return              identity number of SOS/SOS2
             */
            //unsigned getData(Info& info, unsigned row, vector<unsigned long> *vars, unsigned& name) override;
            unsigned getData(Info& info, unsigned row, vector<unsigned long>& vars, unsigned& name) override;
        };


    private:
        vector<unsigned> _namespace;		///< namespace parts of the namespace for the functions
        int _baseId;						///< first registration id

        LinearSosMod::SosType _sosNative;   ///< don't linearize this SOS type (let it for native handling by the solver) (only used for evaluating the command line options)
        int _sos2NativeOptPrio;             ///< priority of given option '-native-sos2' (only used for evaluating the command line options)

        bool _useForSos1;                   ///< use extension for SOS
        bool _linForSos1;                   ///< linearize SOS with this extension
        int _idForSos1;                     ///< function registration id for SOS

        bool _useForSos2;                   ///< use extension for SOS2
        bool _linForSos2;                   ///< linearize SOS2 with this extension
        int _idForSos2;                     ///< function registration id for SOS2

        bool _warnInvalid;                  ///< show warning if SOS is invalid because of contained non zero parameters

        unsigned _attachNameVarSos;         ///< postfix for variable name for linearization of SOS (positive part)
        unsigned _attachNameVarSosNeg;      ///< postfix for variable name for linearization of SOS (negative part)
        unsigned _attachNameConSos;         ///< postfix for constraint name for linearization of SOS (max non zero count restriction)

        unsigned _attachNameVarSos2;        ///< postfix for variable name for linearization of order for SOS2
        unsigned _attachNameConSos2;        ///< postfix for constraint name for linearization of SOS2 (order of non zero elements restriction)

        unsigned _defIdSubOrd;              ///< definition number of subsymbol "ord"
        unsigned _defIdSubName;             ///< definition number of subsymbol "name"
        unsigned _defIdSubAdd;              ///< definition number of subsymbol "add"
        unsigned _defIdSubVarArr;           ///< definition number of subsymbol "*vararray"

        CmplValAuto _templContainer;        ///< template container for SOS container object

        vector<SOSStore> _storeSos;         ///< store for SOS and SOS2
        mutex _accMtx;                      ///< mutex for access lock

        array<OutModelExtData, 4> _transfer; ///< objects for data transfer to modules for model output

        RemodelCache<unordered_map<unsigned long, unsigned long>> _gt0Cache;    ///< cache for check whether a variable (key is id of this variable) is greater than 0, value is id + 1 of the binary variable expressing that
        RemodelCache<unordered_map<unsigned long, unsigned long>> _lt0Cache;    ///< cache for check whether a variable (key is id of this variable) is lesser than 0, value is id + 1 of the binary variable expressing that

	public:
        /**
         * constructor
         * @param mod			module creating this extension object
         */
        LinearSos(LinearSosMod *mod);



		/************** overwritten methods of <code>ExtensionBase</code> **********/

    protected:
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
         * run the extension function
         * @param mod			module calling the extension
         * @param step			execution step within the module
         * @param id			additional identificator
         * @param par			additional parameter
         */
        void run(ModuleBase *mod, int step, int id, void *par) override;

	public:
#if PROTO
        /**
         * get name of the extension, only used for protocol output
         */
        const char *extName() override				{ return "linearSos"; }
#endif //PROTO

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

    private:
        /**
         * return count of functions to register
         */
        unsigned countFunc()                    { return (_useForSos1 ? 1 : 0) + (_useForSos2 ? 1 : 0); }

        /**
         * create new sos store and insert it to _storeSos
         * @param sos2      true: SOS2 / false: SOS
         * @param se        defining syntax element
         * @return          pointer to created sos store
         */
        SOSStore* createSOSStore(bool sos2, unsigned se)     { _storeSos.emplace_back(sos2, se); return &(_storeSos.back()); }

        /**
         * get sos store belonging to sos container object
         * @param modp      intepreter module
         * @param cont      container object
         * @return          pointer to sos store
         */
        SOSStore *getSOSStore(Interpreter *modp, ValContainer *cont);

        /**
         * get the array of variables stored in the sos container object, and delete it in the container object
         * @param modp      intepreter module
         * @param cont      container object
         * @param dst       store the array here / NULL: only delete the array
         */
        void getDelSOSVarArray(Interpreter *modp, ValContainer *cont, CmplVal *dst);


        /************** linearization **********/
    protected:
        /**
         * get whether remodeling is enabled
         */
        bool remodelEnabled() override                              { return (_remodelEnabled && (_linForSos1 || _linForSos2)); }

        /**
         * return next potential workstep for remodeling
         * @param modp      intepreter module calling the extension
         * @param om        optimization model
         * @param rowno     row count of workstep
         * @return          pointer to workstep / NULL: no workstep
         */
        void *getWorkStep(Interpreter *modp, OptModel *om, unsigned long rowno) override;

        /**
         * execute one potential workstep for remodeling
         * @param modp      intepreter module calling the extension
         * @param om        optimization model
         * @param ws        pointer to workstep (already checked for correct type)
         */
        unsigned remodelWorkStep(Interpreter *modp, OptModel *om, void *ws) override;

        /**
         * output error message
         * @param modp      intepreter module calling the extension
         * @param ws        workstep
         * @param msg       error message
         * @param errLvl    error level
         */
        void outError(Interpreter *modp, void *ws, const char *msg, int errLvl) override;

    private:
        /**
         * remodel one SOS or SOS2
         * @param modp      intepreter module calling the extension
         * @param om		optimization model
         * @param sos       SOS
         * @return          count of executed remodelations
         */
        unsigned remodelSOS(Interpreter *modp, OptModel *om, SOSStore *sos);

        /**
         * linearize restriction about count of non zero values within SOS or SOS2
         * @param modp      intepreter module calling the extension
         * @param om		optimization model
         * @param sos       SOS
         * @param linInfo   info object for components of SOS or SOS2
         * @return          first variable in SOS (only used for name)
         */
        OptVar *linearizeSOSCount(Interpreter *modp, OptModel *om, SOSStore *sos, vector<SOSLinearizeInfo>& linInfo);

        /**
         * create variable and constraint for check if source variable has a value greater or lesser than zero
         * @param modp      intepreter module calling the extension
         * @param om        optimization model
         * @param src       source variable
         * @param se        id of syntax element in the cmpl text of SOS
         * @param upp       true: check for greater than zero / false: check for lesser than zero
         * @return          identy number of created binary variable + 1 / 0: source variable cannot have such a value
         */
        unsigned long createSOSPartVar(Interpreter *modp, OptModel *om, OptVar *src, unsigned se, bool upp);

        /**
         * linearize restriction order of non zero values within SOS2
         * @param modp      intepreter module calling the extension
         * @param om		optimization model
         * @param sos       SOS
         * @param linInfo   info object for components of SOS2
         * @param fv        first variable in SOS2 (only used for name)
         */
        void linearizeSOS2Order(Interpreter *modp, OptModel *om, SOSStore *sos, vector<SOSLinearizeInfo>& linInfo, OptVar *fv);

        /**
         * linearize degenerated SOS by fixing all variables to zero
         * @param modp      intepreter module calling the extension
         * @param om        optimization model
         * @param sos       SOS
         * @param addNoSol  add constraint for no valid solution
         */
        void linearizeZeroSOS(Interpreter *modp, OptModel *om, SOSStore *sos, bool addNoSol);


        /************** model properties and data transfer to model output **********/

        /**
         * get value for model properties
         * @return          0:extension not used / <0:used but already linearized / >0:used and handling still necessary
         */
        int getModelProp();

        /**
         * initialize info for data transfer to model output
         * @param modp      intepreter module calling the extension
         */
        void initTransferInfo(Interpreter *modp);

        /**
         * fill info for data transfer to model output
         * @param mod       module calling the extension
         * @param lst       list to fill info in
         */
        void fillTransferInfo(ModuleBase *mod, list<OutModelExtDataBase::Info> *lst);
    };


    /**
     * function for creating new SOS
     */
    class SosFunction : public ValFunctionInternal
    {
        friend class LinearSos;

    private:
        LinearSos *_ext;            ///< extension belonging to
        unsigned _sosType;          ///< only 1 (SOS) or 2 (SOS2)


        /**
         * constructor (called only from LinearSos)
         */
        SosFunction(LinearSos *ext, unsigned sosType): ValFunctionInternal(), _ext(ext), _sosType(sosType)     { }


    protected:
        const char *funcName() const override           { return (_sosType == 1 ? "sos1" : "sos2"); }                    ///< name of the function

    public:
        /**
         * calls the cmpl function call operation and store result in execution context, if this value has a special handling for it
         * @param ctx			execution context
         * @param arg			pointer to argument value
         * @return				true
         */
        bool operCall(ExecContext *ctx, StackValue *arg) override;

    private:
        /**
         * create new SOS
         * @param ctx			execution context
         * @param arg			pointer to argument value
         * @param vds           return of array of new defined variables
         * @return				index number of the SOS in LinearSos::_storeSos
         */
        unsigned createSOS(ExecContext *ctx, StackValue *arg, CmplVal &vda);

        /**
         * create new optimization variable within an array and add the variable to SOS
         * @param ctx           execution context
         * @param sos           SOS store
         * @param dst           array element for the new variable
         * @param tp            data type of the variable
         * @param se            syntax element id of source value
         */
        void createSOSVar(ExecContext *ctx, CmplVal& sosVal, CmplVal *dst, CmplVal *tp, unsigned se);

        /**
         * add optimization variable (or parameter) to SOS
         * @param ctx			execution context
         * @param sosVal        SOS store
         * @param src			source value
         * @param se			syntax element id of source value
         */
        void addSOSVar(ExecContext *ctx, CmplVal& sosVal, CmplVal& src, unsigned se);

    protected:
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
        bool operCallSimple(ExecContext *ctx, CmplVal& res, CmplVal& src, bool aggr, unsigned se, void *info = NULL) override     { addSOSVar(ctx, res, src, se); return false; }
    };


    /**
     * base class for functions of SOS container object
     */
    class SosContFunctionBase : public ValFunctionInternal
    {
        friend class LinearSos;

    protected:
        LinearSos *_ext;            ///< extension belonging to

    public:
        /**
         * constructor
         */
        SosContFunctionBase(LinearSos *ext): _ext(ext)      { }

    protected:
        /**
         * get sos store for the $this container object
         * @param ctx           execution context
         * @param setRes        set container object also as result value
         * @return              pointer to sos store
         */
        LinearSos::SOSStore *getSOSForThis(ExecContext *ctx, bool setRes);

        /**
         * get the array of variables stored in the $this container object, and delete it in the container object
         * @param ctx           execution context
         * @param dst       store the array here / NULL: only delete the array
         */
        void getDelVarArrayForThis(ExecContext *ctx, CmplVal *dst)          { _ext->getDelSOSVarArray(ctx->modp(), ctx->getCallThis(), dst); }
    };

    /**
     * function for setting name of SOS
     */
    class SosContFunctionName : public SosContFunctionBase
    {
        friend class LinearSos;

    public:
        /**
         * constructor
         */
        SosContFunctionName(LinearSos *ext): SosContFunctionBase(ext)      { }

    protected:
        virtual const char *funcName() const override 		{ return "<sos>.name"; }                    ///< name of the function

        /**
         * calls the cmpl function call operation and store result in execution context, if this value has a special handling for it
         * @param ctx			execution context
         * @param arg			pointer to argument value
         * @return				true
         */
        bool operCall(ExecContext *ctx, StackValue *arg) override;
    };

    /**
     * function for add a variable to SOS
     */
    class SosContFunctionAdd : public SosContFunctionBase
    {
        friend class LinearSos;

    public:
        /**
         * constructor
         */
        SosContFunctionAdd(LinearSos *ext): SosContFunctionBase(ext)      { }

    protected:
        virtual const char *funcName() const override 		{ return "<sos>.add"; }                    ///< name of the function

        /**
         * calls the cmpl function call operation and store result in execution context, if this value has a special handling for it
         * @param ctx			execution context
         * @param arg			pointer to argument value
         * @return				true
         */
        bool operCall(ExecContext *ctx, StackValue *arg) override;

        /**
         * calls the function for a simple source value (no array or list)
         * @param ctx			execution context
         * @param sosVal    	store for result value
         * @param src			source value
         * @param aggr          called for aggregating elements of an array or a list
         * @param se			syntax element id of source value
         * @param info          info object for use by the caller
         * @return              only used if aggr: true if result is final
         */
        bool operCallSimple(ExecContext *ctx, CmplVal& sosVal, CmplVal& src, bool aggr, unsigned se, void *info = NULL) override;
    };

    /**
     * function for giving array of variables defined for the SOS
     */
    class SosContFunctionAsVar : public SosContFunctionBase
    {
        friend class LinearSos;

    public:
        /**
         * constructor
         */
        SosContFunctionAsVar(LinearSos *ext): SosContFunctionBase(ext)      { }

    protected:
        virtual const char *funcName() const override 		{ return "<sos>.as_var"; }                    ///< name of the function

        /**
         * calls the cmpl function call operation and store result in execution context, if this value has a special handling for it
         * @param ctx			execution context
         * @param arg			pointer to argument value
         * @return				true
         */
        bool operCall(ExecContext *ctx, StackValue *arg) override;
    };

    /**
     * function for string representation of the SOS
     */
    class SosContFunctionAsString : public SosContFunctionBase
    {
        friend class LinearSos;

    public:
        /**
         * constructor
         */
        SosContFunctionAsString(LinearSos *ext): SosContFunctionBase(ext)      { }

    protected:
        virtual const char *funcName() const override 		{ return "<sos>.as_string"; }                    ///< name of the function

        /**
         * calls the cmpl function call operation and store result in execution context, if this value has a special handling for it
         * @param ctx			execution context
         * @param arg			pointer to argument value
         * @return				true
         */
        bool operCall(ExecContext *ctx, StackValue *arg) override;
    };
}

#endif // LINEARSOS_HH

