
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


#ifndef EXECCONTEXT_HH
#define EXECCONTEXT_HH

#include <map>
#include <set>

#include "../../CommonData/CmplVal.hh"
#include "../../CommonData/IntCode.hh"
#include "../../Control/ErrorHandler.hh"
#include "ValueStore.hh"
#include "StackValue.hh"
#include "CodeBlockContext.hh"


using namespace std;



namespace cmpl
{
	class Interpreter;
	class ModuleBase;
	class ValContainer;
    class VarCondMapping;
    class VarCondMapVS;


    /**
	 * the <code>ExecContext</code> class constitutes an execution context for the intermediary code.
	 * it contains the main loop for this execution.
	 */
	class ExecContext
	{
        friend class VarCondMapping;

    private:
        /**
         * info for codeblock context and name prefix
         */
        struct CBContextNmPref
        {
            CodeBlockContext *cbContext;            ///< codeblock context / NULL: codeblock doesn't need a context object for execution
            intType nmPref;                         ///< row/col name prefix valid within the codeblock (as index in global StringStore) / -1: not specified
            unsigned nmPrefSet;                     ///< type of setting the name prefix: 0: not new set / 1: set by name before the codeblock / 2: set by using $curDestName
        };

        /**
         * part of current full tuple within iterations
         */
        struct IterTuplePart
        {
            CmplVal tplPart;                       ///< part of tuple (must be scalar index value or index tuple / TP_EMPTY if not set)
            CodeBlockIteration *iter;              ///< iteration to which this part belongs

            void init()     { tplPart.unset(); iter = NULL; }             ///< initialize empty element
            void dispose()  { tplPart.dispUnset(); iter = NULL; }         ///< dispose and empty element
            bool empty()    { return (tplPart.t == TP_EMPTY); }            ///< get whether element is empty
        };

	private:
		Interpreter *_modp;							///< interpreter module
		ExecContext *_parent;						///< parent execution context / NULL: this is the root execution context

		unsigned _level;							///< level of execution context
		bool _fctContext;							///< execution context is a function context
        bool _needLock;                             ///< lock needed for access to global symbols

        IntCode::IcElem *_curCommand;				///< current command in intermediary code (only set in run())

		SymbolValue *_localSymbols;					///< array for values of local symbols, indexed by local symbol index
		unsigned _localSymbolCap;					///< size of <code>_localSymbols</code>
        unsigned _localSymbolCreateTo;              ///< count of initialized local symbols in <code>_localSymbols</code> at creation time of this execution context

		StackValue *_stack;							///< array for computation stack
		unsigned long _stackCap;					///< size of <code>_stack</code>
		unsigned long _stackTop;					///< stack pointer

        StackValue *_fctArg;                        ///< only for function context: pointer to function argument
        ValContainer *_fctThis;                     ///< only for function context: pointer to "$this" container if existing
        ValContainer *_callThis;                    ///< only within start of function call: pointer to "$this" container of called function

        CBContextNmPref *_cbContext;                ///< array for stack of code block contexts and name prefixes
        unsigned _cbContextCap;                     ///< size of <code>_cbContext</code>
        unsigned _cbContextTop;                     ///< current code block context pointer
        unsigned _cbContextCreateTop;               ///< code block context pointer at creation time of this execution context object

        CBContextNmPref _nmPrefStart;               ///< start value of row/rcol name prefix (<code>_nmPrefStart.cbContext</code> is not used)
        intType _nmPrefCmd;                         ///< value of row/col name prefix for the current command / -1: not specified

        IterTuplePart _topIterVal;                  ///< top part of current iteration tuple
        IterTuplePart *_iterVals;                   ///< array of non-top parts of current iteration tuple
        unsigned _iterValsCap;                      ///< size of <code>_iterVals</code>
        unsigned _iterValsTop;                      ///< current length of <code>_iterVals</code>

        VarCondMapping *_curVarCondMap;             ///< topmost mapping info for execution within a codeblock part with conditions over optimization variables / NULL: not within such codeblock part

		CmplVal _opRes;								///< store for operation result

        StackValue *_assRhs;						///< current assign: right hand side or NULL
        intType _assObjType;						///< current assign: object type or -1 when no one
        ValType *_assDataType;						///< current assign: data type or NULL
        unsigned _assSyntaxElem;					///< current assign: id of syntax element
        unsigned _assStartVolaRhs;					///< current assign: start position of code for the value of the right hand side, if another execution of this code may compute a changed value, else 0
        bool _assNextRhs;							///< current assign: next right hand side value needed
        bool _assOrdered;                           ///< current assign: ordered assign

        volatile bool _cancel;                      ///< cancel flag
        volatile bool _cancelCont;                  ///< control command for cancel is "continue"
        volatile unsigned _cancelCbLvl;             ///< nesting codeblock level for cancel
        recursive_mutex _cancelMtx;                 ///< mutex for setting cancel flag


    public:
		/**
		 * constructor
         * @param mod			interpreter module
         * @param prv			parent execution context / NULL: this is the root execution context
         * @param fct           execution context is a function context
         * @param fctArg		only for function context: pointer to function argument
         */
        ExecContext(Interpreter *mod, ExecContext *prv, bool fct, StackValue *fctArg = NULL);

		/**
		 * destructor
		 */
		~ExecContext();


        /**
         * creates new execution context object for iteration execution within own thread
         * @param lsdt          count of initialized local symbols in <code>prv</code>
         * @return              new execution context
         */
        ExecContext *newAsIterCopy(unsigned lsdt);

        /**
         * reinitialize execution context object for reuse in another thread for iteration execution
         */
        void reinitIterCopy();

        /**
         * release execution context object created for iteration execution within own thread
         */
        void disposeIterCopy();


		/**
		 * main function, runs the main loop for execution of the intermediary code
         * @param startPos      start position within the intermediary code
         * @param userFct       execution of a user defined function
         * @param res           pointer for storing result (NULL if not used)
         */
        void run(unsigned startPos, bool userFct, CmplVal *res);


		/**
		 * get interpreter module
		 */
        inline Interpreter *modp() const    					{ return _modp; }

        /**
         * get start of the array with intermediary code
         */
        IntCode::IcElem *codeBase() const;
		
		/**
		 * get reference to store for operation result
		 */
        inline CmplVal& opResult()                              { return _opRes; }

        /**
         * get whether lock needed for access to global symbols
         */
        inline bool needLock()                                  { return _needLock; }

        /**
         * get pointer to function argument (only for function context)
         */
        inline StackValue *getFctArg() const                    { return _fctArg; }

        /**
         * pointer to "$this" container if existing (only for function context)
         */
        inline ValContainer *getFctThis() const                 { return _fctThis; }

        /**
         * pointer to "$this" container of called function (only within start of function call)
         */
        inline ValContainer *getCallThis() const                { return _callThis; }

        /**
         * set container for called function (directly before function call)
         * @return      previous value of "$this" container of called function
         */
        inline ValContainer *setCallThis(ValContainer *c)       { ValContainer *pt = _callThis; _callThis = c; return pt; }

        /**
         * get topmost mapping info for execution within a codeblock part with conditions over optimization variables / NULL: not within such codeblock part
         */
        inline VarCondMapping *curVarCondMap()                  { return _curVarCondMap; }

		/****** execution of intermediary code commands ****/

	private:
		/**
		 * executes intermediary code command for literal value
		 * @param cd		intermediary code command
		 * @return			next intermediary code position / NULL if you have to leave this execution context after the command
		 */
		IntCode::IcElem* execCodeLitVal(IntCode::IcElem *cd);

		/**
		 * executes intermediary code command for fetch a value
		 * @param cd		intermediary code command
		 * @return			next intermediary code position / NULL if you have to leave this execution context after the command
		 */
		IntCode::IcElem* execCodeFetch(IntCode::IcElem *cd);

        /**
         * executes intermediary code command for fetch of a pseudo symbol
         * @param cd		intermediary code command
         * @return			next intermediary code position / NULL if you have to leave this execution context after the command
         */
        IntCode::IcElem* execCodeFetchSpecial(IntCode::IcElem *cd);

        /**
		 * executes intermediary code command for assign a value
		 * @param cd		intermediary code command
		 * @return			next intermediary code position / NULL if you have to leave this execution context after the command
		 */
		IntCode::IcElem* execCodeAssign(IntCode::IcElem *cd);

		/**
		 * executes intermediary code command for execution of an operation
		 * @param cd		intermediary code command
		 * @return			next intermediary code position / NULL if you have to leave this execution context after the command
		 */
		IntCode::IcElem* execCodeOperation(IntCode::IcElem *cd);

		/**
		 * executes intermediary code command for execution of a value construction
		 * @param cd		intermediary code command
		 * @return			next intermediary code position / NULL if you have to leave this execution context after the command
		 */
		IntCode::IcElem* execCodeConstruct(IntCode::IcElem *cd);

        /**
         * executes intermediary code command for name prefix for naming of rows and cols in the result matrix
         * @param cd		intermediary code command
         * @return			next intermediary code position / NULL if you have to leave this execution context after the command
         */
        IntCode::IcElem* execCodeLineNamePref(IntCode::IcElem *cd);

		/**
		 * executes intermediary code command for functions and jumps
		 * @param cd		intermediary code command
		 * @return			next intermediary code position / NULL if you have to leave this execution context after the command
		 */
		IntCode::IcElem* execCodeFunction(IntCode::IcElem *cd);

        /**
         * executes intermediary code command for code blocks
         * @param cd		intermediary code command
         * @param cbBody    execution for a single codeblock body
         * @param cbLvl     level of nested codeblocks
         * @param cbCancel  cancelling codeblock at level _cancelCbLvl; skip new codeblock if new one starts
         * @param endCtx    return whether execution in this execution context is ended
         * @return			next intermediary code position / NULL if you have to leave this execution context after the command
         */
        IntCode::IcElem* execCodeCodeblock(IntCode::IcElem *cd, bool cbBody, unsigned &cbLvl, bool& cbCancel, bool& endCtx);

        /**
         * executes intermediary code command for code block headers
         * @param cd		intermediary code command
         * @param cbHeader  execution for a single codeblock header
         * @return			next intermediary code position / NULL if you have to leave this execution context after the command
         */
        IntCode::IcElem* execCodeCBHeader(IntCode::IcElem *cd, bool cbHeader);

        /**
         * executes intermediary code command for code blocks
         * @param cd		intermediary code command
         * @param cbHB      execution for a single codeblock header or body
         * @param cbLvl     current level of nested codeblocks
         * @param cbTS      start level of nested codeblocks
         * @return			next intermediary code position / NULL if you have to leave this execution context after the command
         */
        IntCode::IcElem* execCodeCBControl(IntCode::IcElem *cd, bool cbHB, unsigned cbLvl, unsigned cbTS);


		/****** stack handling ****/

    public:
        /**
		 * prepare stack to push a value
		 * @param se		id of syntax element in the cmpl text creating this stack value
		 * @return			stack element to take up the value to push
		 */
		StackValue *pushPre(unsigned se);

		/**
		 * pointer to current element on the stack
		 */
		inline StackValue *stackCur()						{ return (_stack + (_stackTop-1)); }

		/**
		 * pointer to previous element on the stack relative to a given element
		 * @param sv		given base element
		 * @param err		true: if sv is already the first element then error / false: then return NULL
		 * @return			element before sv
		 */
		StackValue *stackPrev(StackValue *sv, bool err = true);

		/**
		 * pointer to previous element on the stack relative to a given element (static version, no check for underflow possible)
		 * @param sv		given base element
		 * @return			element before sv
		 */
		static StackValue* stackPrevStatic(StackValue *sv);

        /**
         * get total count of stack values for a list
         * @param sv		list header element
         * @return			total count of elements on the stack for the list
         */
        static unsigned long stackListTotalCnt(StackValue *sv);

		/**
		 * push a literal int value to the stack
		 * @param i			value to push
		 * @param se		id of syntax element in the cmpl text creating this stack value
		 */
		inline void pushInt(intType i, unsigned se)			{ StackValue *sv = pushPre(se); sv->_val.set(TP_INT, i); }

		/**
		 * push a literal real value to the stack
		 * @param r			value to push
		 * @param se		id of syntax element in the cmpl text creating this stack value
		 */
		inline void pushReal(realType r, unsigned se)		{ StackValue *sv = pushPre(se); sv->_val.set(TP_REAL, r); }

		/**
		 * push a literal string value to the stack
		 * @param i			value to push (index in string store)
		 * @param se		id of syntax element in the cmpl text creating this stack value
		 */
		inline void pushString(intType i, unsigned se)		{ StackValue *sv = pushPre(se); sv->_val.set(TP_STR, i); }

		/**
		 * push a literal blank value to the stack
		 */
		inline void pushBlank(unsigned se)					{ StackValue *sv = pushPre(se); sv->_val.set(TP_BLANK); }

        /**
         * push a null value to the stack
         */
        inline void pushNull(unsigned se)					{ StackValue *sv = pushPre(se); sv->_val.set(TP_NULL); }

		/**
		 * push a empty value to the stack
         */
		inline void pushEmpty(unsigned se)					{ pushPre(se); }

        /**
         * push a value to the stack
         * @param v			value to push
         * @param se		id of syntax element in the cmpl text creating this stack value
         * @param disp		dispose value v
         */
        inline void pushVal(CmplVal& v, unsigned se, bool disp = false)		{ StackValue *sv = pushPre(se); if (disp) { sv->_val.moveFrom(v, false); } else { sv->_val.copyFrom(v, true, false); } }

        /**
		 * push value stored in _opRes
		 */
		inline void pushOpResult(unsigned se)				{ StackValue *sv = pushPre(se); sv->_val.moveFrom(_opRes); /*TODO: nur verwendbar wenn _addVal und _transpose nicht zu setzen*/ }

		/**
		 * discards the value on top of stack
		 */
		void pop();

		/**
		 * discards value on the stack up to given element
		 * @param sv		given base element
		 * @param incl		discard also element sv
		 */
		void stackPopTo(StackValue *sv, bool incl = true);

        /**
         * replace a list value on the stack by an array or tuple
         * @param sv		list header
         * @return 			result value
         */
        StackValue *replaceListOnStack(StackValue *sv);

        /**
         * get from stack value (which can be array or list) the first scalar values
         * @param sv            source stack value
         * @param maxCnt        max count of scalar values to fetch
         * @param args          array for returning fetched scalar values
         * @param se            array for returning syntax element numbers of fetched scalar values
         * @return              count of found values (can be greater than maxCnt if more values are found) / 0: found values are not scalar
         */
        unsigned getFirstSimpleValues(StackValue *sv, unsigned maxCnt, CmplVal *args, unsigned *se);


        /****** code block handling ****/

        /**
         * push an object to the code block context start
         * @param cb        code block context object or NULL if no context is needed
         * @param se		syntax element in the cmpl text creating this code block context object
         */
        void pushCBContext(CodeBlockContext *cb, unsigned se);

        /**
         * get code block context object
         * @param offset    offset to current context
         * @return          code block context object or NULL if no context is needed (or invalid offset)
         */
        inline CodeBlockContext *getCBContext(unsigned offset = 0)              { return (offset < _cbContextTop ? _cbContext[_cbContextTop - offset - 1].cbContext : NULL); }

        /**
         * replace empty top element on code block context stack by a new code block context object
         * @param cd        intermediary code command (must be INTCODE_CB_HEADER/ICS_CBHEADER_END)
         * @return          code block context object
         */
        CodeBlockContext *setCBContext(IntCode::IcElem *cd);

        /**
         * get next upper existing code block context object on the code block stack
         * @param lvl       start position in the code block stack
         * @return          code block context object or NULL if no such context object exists
         */
        CodeBlockContext *getNextUpperCBContext(unsigned lvl);

        /**
         * pop and discard code block context objects
         * @param se		syntax element in the cmpl text creating this code block context object
         * @param cnt       count of code block context objects to discard
         */
        void popCBContext(unsigned se, unsigned cnt = 1);

        /**
         * get current codeblock nesting level
         */
        inline unsigned curCBLevel() const                                      { return _cbContextTop; }

        /**
         * set value for codeblock symbol
         * @param n         number of codeblock symbol within array _localSymbols
         * @param va        value as CmplVal
         * @param vb        value as StackValue (only used if no <code>va</code>)
         */
        void setCBSymbolVal(unsigned n, const CmplVal *va, StackValue *vb = NULL);

        /**
         * uninitialize local symbols
         * @param n         number of first symbol within array _localSymbols
         * @param to        max number of symbol + 1
         * @param delId		unset also SymbolValue::_defId
         */
        void uninitCBSymbols(unsigned n, unsigned to, bool delId);

        /**
         * set and propagate cancel flag
         * @param cmd		control command (one of ICS_CBC_BREAK, ICS_CBC_CONTINUE, ICS_CBC_REPEAT)
         * @param dstLvl	nesting codeblock level of destination codeblock of the control command
         */
        void setCancel(int cmd, unsigned dstLvl);

        /**
         * get current row/col name prefix
         * @param cmd       return whether a row/col name prefix is set for the current command
         * @return          current row/col name prefix as index in global string store / -1: no name prefix
         */
        intType curNmPref()                         { return (_nmPrefCmd >= 0 ? _nmPrefCmd : (_cbContextTop ? _cbContext[_cbContextTop - 1].nmPref : _nmPrefStart.nmPref)); }

        /**
         * get current row/col name prefix
         * @param cmd       return whether a row/col name prefix is set for the current command
         * @return          current row/col name prefix as index in global string store / -1: no name prefix
         */
        intType curNmPref(bool& cmd)                { cmd = (_nmPrefCmd >= 0); return (cmd ? _nmPrefCmd : (_cbContextTop ? _cbContext[_cbContextTop - 1].nmPref : _nmPrefStart.nmPref)); }

        /**
         * set current row/col name prefix
         * @param nmp       new row/col name prefix
         * @param expl      true if name prefix is set explicit by $curDestName
         */
        void setCurNmPref(intType nmp, bool expl)   { CBContextNmPref& curCB = (_cbContextTop ? _cbContext[_cbContextTop - 1] : _nmPrefStart); curCB.nmPref = nmp; curCB.nmPrefSet = (expl ? 2 : 1); }

        /**
         * set current row/col name prefix from codeblock
         * @param cb        codeblock
         */
        void setCurNmPrefCB(CodeBlockContext *cb)   { CBContextNmPref& curCB = (_cbContextTop ? _cbContext[_cbContextTop - 1] : _nmPrefStart); curCB.nmPref = cb->_nmPrefStart; curCB.nmPrefSet = cb->_nmPrefStartSet; }

        /**
         * set top part of current iteration tuple
         * @param iter      iteration to which the tuple part belongs
         * @param val       part of tuple (must be scalar index value or index tuple)
         */
        void setIterTplPart(CodeBlockIteration *iter, const CmplVal &val);

        /**
         * unset top part of current iteration tuple
         * @param iter      iteration to which the tuple part belongs
         */
        void delIterTplPart(CodeBlockIteration *iter);

        /**
         * get all parts of current iteration tuple
         * @param parts     vector for return of the parts
         * @return          count of found tuple parts
         */
        unsigned getIterTplParts(vector<CmplValAuto>& parts);

    private:
        /**
         * get parts of current iteration tuple, from start of given iteration
         * @param parts     vector for return of the parts
         * @param start     iteration
         * @return          count of found tuple parts
         */
        unsigned getIterTplParts(vector<CmplValAuto>& parts, CodeBlockIteration *start);

        /**
         * get all parts of current iteration tuple which are stored in this execution context
         * @param parts     vector for return of the parts
         * @param start     start index within _iterVals
         * @return          count of found tuple parts
         */
        unsigned getIterTplPartsThis(vector<CmplValAuto>& parts, unsigned start);

    public:
        /**
         * get parts of current iteration tuple, only from start of the topmost iteration
         * @param parts     vector for return of the parts
         * @return          count of found tuple parts
         */
        unsigned getIterTplPartsIter(vector<CmplValAuto>& parts);

        /**
         * get parts of current iteration tuple, only from start of the topmost change of name prefix with $curDestName
         * @param parts     vector for return of the parts
         * @return          count of found tuple parts
         */
        unsigned getIterTplPartsNmPref(vector<CmplValAuto>& parts);


        /****** conditions over optimization variables ****/

        /**
         * start mapping for codeblock part with conditions over optimization variables
         * @param cb        codeblock context
         * @param cbp       number of current codeblock part
         * @param varCond   condition formula ver optimization variables (must be TP_FORMULA or TP_BIN with value true)
         * @param lsf       number of first symbol within array _localSymbols that doesn't need mapping
         * @param init      init new mapping object (if false then there must be a current mapping object matching the codeblock)
         */
        void varCondMapStartPart(CodeBlockContext *cb, int cbp, CmplVal& varCond, unsigned lsf, bool init);

        /**
         * end mapping for codeblock part with conditions over optimization variables
         * @param cb        codeblock context (must match the current mapping object)
         * @param cbp       number of current codeblock part (must match the current mapping object)
         */
        void varCondMapEndPart(CodeBlockContext *cb, int cbp);

        /**
         * set codeblock result using mapping for codeblock part with conditions over optimization variables
         * @param cb        codeblock context (must match the current mapping object)
         * @param cbp       number of current codeblock part (must match the current mapping object)
         * @param val       codeblock result value of current codeblock part
         * @param ovr       override all existing results
         * @param se		syntax element id of source value
         */
        void varCondMapSetCBResult(CodeBlockContext *cb, int cbp, CmplVal &val, bool ovr, unsigned se);

        /**
         * merge mapped values for conditions over optimization variables at the end of codeblock, and remove mapping object
         * @param cb        codeblock context (must match the current mapping object)
         * @param res       return of merged codeblock result
         */
        void varCondMapMerge(CodeBlockContext *cb, CmplVal& res);

        /**
         * check if mapping is needed and given, and get the mapped value store
         * @param sym       mapping for the value store of this symbol
         * @param nw        if mapping is needed but doesn't exist, then create a new mapped value store
         * @param nocond    nocond modificator is given for assignment
         * @param ncerr     return whether error of mixing conditional and non-conditional assignments
         * @return          mapping info for the value store / NULL: no mapping
         */
        VarCondMapVS *checkGetMappedVS(SymbolValue *sym, bool nw, bool nocond, bool &ncerr);

        /**
         * check if mapping is needed and given, and get the mapped value store
         * @param sym       mapping for the value store of this symbol
         */
        VarCondMapVS *checkGetMappedVS(SymbolValue *sym)            { bool ncerr; return checkGetMappedVS(sym, false, false, ncerr); }


        /****** assignment ****/

    public:
        /**
         * get right hand side value for the current assignment
         * @param se		id of syntax element for use as default
         * @return 			right hand side value on the stack
         */
        StackValue *getCurAssignRhs(unsigned se);


        /****** utility functions ****/

    public:
        /**
         * check if source value is a container with a defined special convert function, and call this function
         * @param dst       store for result value of the called function
         * @param src       source value
         * @param tp        type of convert
         * @return          true if special convert function is found and called
         */
        bool checkContainerConvSpecial(CmplVal& dst, const CmplVal& src, tp_e tp);


		/****** error handling ****/

    public:
		/**
		 * internal error
		 * @param msg		error message
		 */		
		void internalError(const char *msg);

        /**
         * other execution error
         * @param msg		error message
         * @param se		syntax element id of source value
         * @param level		one of ERROR_LVL_*
         */
        void execError(const char *msg, unsigned se, int level = ERROR_LVL_NORMAL);

		/**
         * value is not suited for the current intermediary code command
		 * @param msg		error message
         * @param val		value
         * @param se		syntax element id of source value
         * @param level		one of ERROR_LVL_*
		 */
        void valueError(const char *msg, CmplVal& val, unsigned se, int level = ERROR_LVL_NORMAL);

        /**
         * value on the stack is not suited for the current intermediary code command
         * @param msg		error message
         * @param sv		value on the stack
         * @param level		one of ERROR_LVL_*
         */
        void valueError(const char *msg, StackValue *sv, int level = ERROR_LVL_NORMAL)		{ valueError(msg, sv->val(), sv->syntaxElem(), level); }

        /**
         * assertion failed
         * @param tp		type of failing assertion
         * @param ase		syntax element id of assert command
         * @param vse       syntax element id of failing value / 0: in assert command itself
         * @param level		one of ERROR_LVL_*
         */
        void assertionError(const char *tp, unsigned ase, unsigned vse, int level = ERROR_LVL_NORMAL);
    };


    class VarCondMapVS;

    /**
     * the <code>VarCondMapping</code> class holds the mapping info for accessing symbols
     * within a codeblock part with conditions over optimization variables
     */
    class VarCondMapping
    {
        friend class ExecContext;
        friend class VarCondMapVS;

    private:
        ExecContext *_execContext;                  ///< execution context in which this mapping object is created
        VarCondMapping *_parent;                    ///< parent mapping info / NULL: this is the base mapping info

        CodeBlockContext *_cbContext;               ///< codeblock context of the codeblock this mapping is created for
        unsigned _unmappedLSFrom;                   ///< number of first symbol within the execution context that doesn't need mapping
        unsigned _partCnt;                          ///< count of codeblock parts
        int _curCBPart;                             ///< number of current codeblock part / -1: not in execution of a codeblock part

        vector<CmplValAuto> _partVarCond;           ///< condition over optimization variables per codeblock part (length is _partCnt)
                                                            // can have the following values:
                                                            //  TP_FORMULA containing a boolean formula
                                                            //  TP_BIN with value true (only possible for last non empty part)
                                                            //  TP_EMPTY: codeblock part is not used
        vector<vector<CmplValAuto>*> _prvPartConds; ///< conditions of previous parts
        int _trueCondPart;                          ///< number of codeblock part with condition value "true" / -1: no one

        CmplValAuto _curFullCond;                   ///< full condition (including from parent) for current codeblock part / TP_EMPTY: not set

        vector<CmplValAuto> *_mapCBRes;             ///< mapping for codeblock result / NULL: not used or not initialized
        map<ValueStore *, VarCondMapVS *> _mapVS;   ///< mapping per mapped value store object (key is non-owning reference, but value is owning reference)

        set<ValueStore *> _nocond;                  ///< set of value store objects which are assigned with the nocond modificator (element is non-owning reference)

        /*
            Mapping-Objekt:
                Gibt an, dass bei Zugriff auf einen ValueStore nicht dieser, sondern eine Kopie davon zu verwenden ist
                    zusätzlich allgemeines Array/ValueStore mit Flags, dass pro Index angibt, ob ein Element in der Kopie in irgendeinem Teil geändert wurde (zusätzlich auch Flag für Gesamtarray)
                Mappings können hierarchisch angeordnet sein. Aber auf jeder Ebene ist als Mappingquelle der ursprüngliche ValueStore eingetragen.
                    D.h. bei der Anwendung eines Mappings zum Zugriff muss nur die aktuell oberste Mappingebene berücksichtigt werden.
                    Bei der Neuanlage eines Mappings muss dagegen eine Mappingregel für den ValueStore auch in alle darunterliegenden Ebenen eingetragen werden, soweit dort
                    noch nicht existent. Und kopiert werden muss aus dem Mappingziel der jeweils darunter liegenden Ebene.

            Mapping fuer Zugriff:
                wenn im ExecContext ein Mapping-Objekt existiert, und das Symbol kein lokales Symbol mit Nummer >= _unmappedLSFrom ist (die 2.Bedingung koennte auch weggelassen werden):
                    beim FETCH für ein Symbol statt des regulären ValueStore den gemappten verwenden und das Array daraus auf den Stack (SimpleConst-Werte brauchen nie Mapping)
                        (aber doch bei skalarem Wert aus Array mit Nulltupleset als Definitionsset! -> Fetch ueber SymbolValue::simpleValue())
                    nur das oberste Mapping-Objekt braucht einbezogen werden.
                    ValueStore in _mapVS suchen, wenn dazu VarCondMapVS-Objekt eingetragen ist:
                        destination ValueStore aus dem VarCondMapVS-Objekt holen, zum _curCBPart
                        wenn eingetragen, dann Array aus dem destination ValueStore holen statt aus regulaerem ValueStore des Symbols

             Mapping fuer Zuweisung:
                ref-Zuweisung -> Nutzerfehler
                Pruefung ob Zuweisung ueberhaupt Mapping verwendet:
                    nein wenn lokales Symbol mit Nummer >= _unmappedLSFrom: dann keine weitere Pruefung notwendig; normale Ausfuehrung ohne Mapping
                    sonst nein wenn Zuweisung mit nocond, Inkrement/Dekrement (ist Operation, nicht Zuweisung!), Zuweisung mit const:
                        Mapping pruefen wie bei FETCH
                            wenn Mapping vorhanden -> Nutzerfehler inkonsistenter Zugriff
                        normale Ausfuehrung ohne Mapping
                    sonst wenn zugewiesenes Symbol Single-Const, dann Fehler (ist sowieso Fehler)
                Sonst:
                    Zuweisungsziel-ValueStore (ValueStore des zugewiesenen Symbols) bestimmen, ist Quell-ValueStore des Mappings
                        wenn Symbol noch uninitialisiert, dann erst neuen ValueStore anlegen, dann als Quell-ValueStore des Mappings verwenden
                    rekursiv in _parent, bis zum Wurzel-Info
                        aber bei lokalem Symbol Rekursion nicht in Mapping-Info mit >= _unmappedLSFrom (welcher ExecContext?); sondern auf Ebene darueber beenden
                                    (wenn ExecContext der Rekursionsstufe fuer Funktions, dann bei lokalem Symbol keine tiefere Rekursion)
                    ValueStore in _mapVS suchen, und darin im VarCondMapVS-Objekt zum _curCBPart
                        wenn nicht in _mapVS: neues VarCondMapVS-Objekt anlegen und in _mapVS
                        wenn zum _curCBPart nicht im VarCondMapVS-Objekt:
                            neuen ValueStore als Kopie des Quell-ValueStore machen und eintragen (benutzt dann erstmal Array des Quell-ValueStore mit, bis zu erstem copy-on-write)
                    eine Rekursionsstufe hinauf, wieder dasselbe (der Quell-ValueStore ist jedesmal derselbe)
                        (es muss also dann auf jeder Rekursionsstufe ein Mapping geben, die alle vom selben Quell-ValueStore ausgehen)
                    Ziel-ValueStore auf oberster Ebene (am Ende der Rekursion) ist gemappter ValueStore fuer die Zuweisung
                    Zuweisung muss ausserdem VarCondMapVS-Objekt aus dem obersten Mapping-Info kennen:
                        dorthinein Kennzeichen fuer ausgefuehrte Aenderungen schreiben
                    Sperren? (noetig fuer tiefere Mapping-Infos zu anderen ExecContext, wenn durch Threading entstanden)

                (Bei Zuweisung mit Operation: wie fuer andere Zuweisungen: wenn Mapping vorgesehen, dann wie sonst fuer Zuweisung ausfuehren, gemappter ValueStore ist dann Quelle und Ziel der Zuweisung)
                (Bei Zuweisung zu Pseudosymbolen: generell kein Mapping (ist intern Funktionsaufruf und keine Zuweisung; spezielle Funktionen koennten vielleicht selbst das Mapping-Info irgendwie auswerten))

                StackValue::doAssign() -> Mapping wie beschrieben
                StackValue::importExtern() -> Mapping wie beschrieben (externe Werte dann in gemappten ValueStore schreiben; werden spaeter wie andere Werte gemerged)
                StackValue::checkAssert() -> Mapping wie beim FETCH; Assertions in gemapptem ValueStore pruefen
                (in allen diesen 3 Funktionen enthaelt der StackValue einen SymbolValue)

            Merge:
                Einzeln jeden Eintrag in _mapVS des obersten Mapping-Infos durchgehen, fuer jedes VarCondMapVS-Objekt:
                    Ziel-ValueStore ist:
                        wenn kein _parent-Mapping, oder im _parent-Mapping dieser ValueStore nicht gemappt: -> Quell-ValueStore des Mappings
                        sonst: -> Ziel-ValueStore fuer diesen Quell-ValueStore im _parent-Mapping
                        (geeignete Sperren fuer Ziel-ValueStore)
                    Zieldefinitionsset ist Vereinigung aus allen Indextupeln, fuer die Werte in mindestens einem CBPart geaendert/hinzugefuegt/geloescht wurden
                        (aber ohne Indextupel, die fuer jeden CBPart geloescht wurden)
                        und Indextupeln, fuer die es gar keine Aenderung gab
                        (das Zieldefinitionsset kann durchaus identisch zum urspruenglichen des Arrays sein, diese Moeglichkeit performant behandeln)
                    fuer jedes Array-Element:
                        unveraendert: gar nichts tun (Wert Array des Ziel-ValueStore darf nicht geschrieben werden, weil vielleicht Wert durch anderen Thread geaendert)
                        sonst: Bedingungsformel zusammenstellen
                            (wenn fuer ein Teil geloescht, dann Wert TP_NULL)
                            (wenn keine true-Pseudobedingung als letztes, dann urspruenglichen Wert dafuer (wenn keiner, dann TP_NULL))

            Neuer assign-Modificator "nocond":
                Wenn angegeben, dann kein Mapping verwenden; Mischung mit gemappten Zuweisungen ist Fehler (nur soweit Mischung tatsaechlich auftritt)
                    (solange nicht innerhalb von Variablenbedingung, macht Angabe keinen Unterschied)
                    dafuer:
                        Kennzeichen ob nocond muss fuer getMappedVSRec mitgegeben werden
                        wenn ja:
                            Suche nach vorhandenem Mapping im aktuellen oder vorhergehenden Codeblockteilen, wenn gefunden, dann fehlerhafte Vermischung
                            Kennzeichen, dass mit nocond zugewiesen, merken
                        wenn nein:
                            Pruefung ob Kennzeichen, dass mit nocond zugewiesen, gesetzt ist, wenn ja, dann fehlerhafte Vermischung
                - Beschreibung anpassen     -> erledigt
                - Syntax-Coloring anpassen  -> erledigt
                - Compiler anpassen         -> erledigt
                - Zwischencode anpassen     -> erledigt
         */

        /**
         * constructor
         * @param ctx       execution context
         * @param cb        codeblock context of the codeblock this mapping is created for
         * @param lsf       number of first symbol within the execution context that doesn't need mapping
         */
        VarCondMapping(ExecContext *ctx, CodeBlockContext *cb, unsigned lsf);

        /**
         * destructor
         */
        ~VarCondMapping();

    public:
        /**
         * get full condition (including from parent) for current codeblock part
         */
        CmplValAuto &getFullCond();

        /**
         * get id of syntax element of the codeblock
         */
        unsigned syntaxElem()                   { return _cbContext->syntaxElem(); }

    private:
        /**
         * check and get current mapping object within the execution context
         * @param ctx       execution context
         * @param cb        codeblock context of the codeblock this mapping is created for
         * @param cbp       number of current codeblock part / -1: no one
         * @return          pointer to current mapping object within the execution context
         */
        static VarCondMapping *check(ExecContext *ctx, CodeBlockContext *cb, int cbp);

        /**
         * start of execution for one codeblock part
         * @param cbp       number of current codeblock part
         * @param varCond   condition formula ver optimization variables (must be TP_FORMULA or TP_BIN with value true)
         */
        void startPart(int cbp, CmplVal& varCond);

        /**
         * end of execution for one codeblock part
         */
        void endPart();

        /**
         * get mapping info for an value store
         * @param lvl       recursion level
         * @param ctxOrg    original execution context
         * @param locSymNo  symbol no of local symbol within ctxOrg / -1: no local symbol
         * @param srcVS     source value store of mapping
         * @param nw        if mapping doesn't exist, then insert new mapping
         * @param nocond    nocond modificator is given for assignment
         * @param ncerr     return whether error of mixing conditional and non-conditional assignments
         * @return          mapping info for the value store / NULL: no mapping
         */
        VarCondMapVS *getMappedVSRec(unsigned lvl, ExecContext *ctxOrg, int locSymNo, ValueStore *srcVS, bool nw, bool nocond, bool &ncerr);

        /**
         * set codeblock result
         * @param val       codeblock result value of current codeblock part
         * @param ovr       override all existing results
         * @param se		syntax element id of source value
         */
        void setMappedCBResult(CmplVal &val, bool ovr, unsigned se);

        /**
         * merge symbol values and codeblock result
         * @param res       return of merged codeblock result
         */
        void mergeAll(CmplVal& res);

        /**
         * merge all values of an array
         * @param res       return of merged result (array of values or scalar value) or value store to set result values in
         * @param dstVS     value store to set result values in (alternative to res) / NULL: no such alternative
         * @param map       values or value store per codeblock part
         * @param chg       original values and change info / NULL: no original values
         * @return          true if result is returned in res
         */
        bool mergeArray(CmplVal& res, ValueStore *dstVS, vector<CmplValAuto>& map, VSChangeInfo *chg);

        /**
         * merge one value
         * @param res       return of merged result
         * @param pvals     source value per codeblock part
         * @param pfrom     first used codeblock part
         * @param pend      after last used codeblock part
         * @param ptc       part in pvals with condition TP_TRUE
         */
        void mergeVal(CmplVal& res, vector<CmplValAuto>& pvals, unsigned pfrom, unsigned pend, unsigned ptc);

        /**
         * check whether value has valid type for merge
         * @param res       set this to replacement value if type is invalid
         * @param v         value to check
         * @return          true if value has valid type
         */
        bool checkType(CmplVal& res, CmplVal& v, OptCon *&oc);
    };

    /**
     * the <code>VarCondMapVS</code> class holds the mapping info for one value store object
     * within a codeblock part with conditions over optimization variables
     */
    class VarCondMapVS
    {
        friend class VarCondMapping;

    private:
        VarCondMapping *_owner;                     ///< mapping to which this object belongs

        CmplValAuto _srcVS;                         ///< source and destination value store (TP_VALUESTORE)
        VarCondMapVS *_srcMap;                      ///< _srcVS belongs to this mapping info (_srcMap->getDstVS() == _srcVS.valueStore()) / NULL: _srcVS is not mapped

        VSChangeInfo _chgInfo;                      ///< info for made changes in destination values
        vector<CmplValAuto> _dstMapVS;              ///< destination value store of mapping per codeblock part (TP_VALUESTORE) / TP_EMPTY if there is no mapping for the source array in the codeblock part


        /**
         * constructor
         */
        VarCondMapVS(VarCondMapping *owner, ValueStore *src, VarCondMapVS *srcmap): _owner(owner), _srcVS(TP_VALUESTORE, src), _srcMap(srcmap), _chgInfo(src), _dstMapVS(owner->_partCnt)    { }

        /**
         * set copy of source value store as mapping destination for current codeblock part
         */
        void setCopyVS()                                { _dstMapVS[_owner->_curCBPart].dispSet(TP_VALUESTORE, new ValueStore(_chgInfo.baseVS())); }

    public:
        /**
         * get destination value store of mapping for current codeblock part
         */
        ValueStore *getDstVS()                          { return (_owner->_curCBPart >= 0 && _dstMapVS[_owner->_curCBPart].t == TP_VALUESTORE ? _dstMapVS[_owner->_curCBPart].valueStore() : NULL); }

        /**
         * get info for made changes in destination values
         */
        VSChangeInfo *chgInfo()                         { return &_chgInfo; }
    };


#if 0
    /**
     * exception while using a cmpl value, e.g. in evaluation of a cmpl expression
     */
    class CmplValueException : public std::runtime_error
            // von irgendeiner anderen CMPL-exception ableiten / oder von std::runtime_error (würde what(), destructor, und Konstruktor mit what-Argument schon definieren; vielleicht andere CMPL-exceptions schon davon ableiten)
    {
    private:
        int _valueNo;                     ///< number of inappropriate value (-1: whole evaluated expression / 0: no specific value / >=1: number of argument value)

    public:
        /**
         * constructor
         * @param txt       error message
         * @param valNo     number of inappropriate value (-1: whole evaluated expression / 0: no specific value / >=1: number of argument value)
         */
        CmplValueException(const string& txt, int valNo) noexcept : std::runtime_error(txt), _valueNo(valNo)    { }

        /**
         * get number of inappropriate value
         */
        int valueNo() const noexcept            { return _valueNo; }
    };
#endif
}

#endif // EXECCONTEXT_HH

