
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


#ifndef CODEBLOCKCONTEXT_HH
#define CODEBLOCKCONTEXT_HH

#include <vector>
#include <thread>
#include <mutex>
#include <atomic>

#include "../../Control/ThreadHandler.hh"
#include "../../CommonData/CmplVal.hh"
#include "../../CommonData/CmplArray.hh"
#include "../../CommonData/IntCode.hh"



using namespace std;



namespace cmpl
{
	class ExecContext;
    class CodeBlockHeaderBase;
    class CBAssignInfoBasis;
    class CBAssignInfoTuple;
    class CBHeaderIterInfo;
    class CodeBlockIteration;
    class CBThreadInfo;
    class StackValue;
    class SetIterator;


    /************** codeblock context **********/

    /**
	 * the <code>CodeBlockContext</code> class stores the context for executing a code block
	 */
	class CodeBlockContext
	{
        friend class CodeBlockIteration;
        friend class CBThreadInfo;
        friend class ExecContext;

    private:
        ExecContext *_execContext;                  ///< execution context in which this codeblock has started
        IntCode::IcElem *_icStart;                  ///< starting command of the codeblock within the intermediary code
        unsigned _lvl;                              ///< position of this codeblock context object within the codeblock context stack of the execution context

        int _execPart;                              ///< number of executed part of this codeblock (first executed part if condition over optimization variables) / -1: up to now no part is executed
        int _curPart;                               ///< number of current part of this codeblock / -1: before first part
        unsigned _curBodyCode;                      ///< start address of the body of the current part of this codeblock / 0: not known yet

        unsigned _localSymFrom;                     ///< first local symbol number (fetch id) of symbols defined within the codeblock part
        unsigned _localSymTo;                       ///< last local symbol number (fetch id) + 1 of symbols defined within the codeblock part

        unsigned short _curPartPar;                 ///< parameter of current codeblock part
        bool _lastPart;                             ///< current part is the last part of this codeblock

        bool _hasVarCond;                           ///< code block has condition over optimization variables
        CmplValAuto _curVarCondition;               ///< condition over optimization variables for current part of the codeblock (must be boolean TP_FORMULA or TP_OPT_VAR) / TP_EMPTY: no such condition

        CBHeaderIterInfo *_startIter;               ///< initial iteration info for current part of this codeblock / NULL: no iteration
        CodeBlockIteration *_startIterObj;          ///< initial iteration object for current part of this codeblock (only set while executing a codeblock body with iteration)
        vector<CodeBlockHeaderBase *> _headers;     ///< headers of the current part of this codeblock (only headers not fully processed yet)

        intType _nmPrefStart;                       ///< value of row/rcol name prefix at start of this codeblock / -1: not specified
        unsigned _nmPrefStartSet;                   ///< type of setting the name prefix: 0: not new set / 1: set by name before the codeblock / 2: set by using $curDestName

        volatile CmplVal _cbResult;                 ///< codeblock result value / TP_BLANK: not initialized or not needed
        volatile bool _cancel;                      ///< cancel execution of this codeblock (triggered by codeblock control command)
        volatile int _ctrlCmd;                      ///< control command (one of ICS_CBC_BREAK, ICS_CBC_CONTINUE, ICS_CBC_REPEAT) / 0: no one
        recursive_mutex _cancelMtx;                 ///< mutex for synchronizing access to _cbResult and _cancel


        /* Fuer Codeblock mit variablenhaltiger Bedingung:
            Auswertung Header:
                Wenn erster variablenhaltiger Header gefunden wird:
                    wenn noch kein Codeblockkontext: neues initialisieren und eintragen
                    wenn aktueller Teil (entsprechend seinen Parametern) Iterationen hat oder repeat darauf zielt   -> Fehler
                        TODO: Iteration davor im aktuellen Teil doch erlauben, wenn aktueller Teil der letzte Teil des Codeblocks ist (dann muss ganzes _curVarCondition-Handling separat für jeden Iterationsschritt ausgeführt werden)
                    Bedingung in _curVarCondition eintragen (weitere Bedingungen aus weiteren Headern im selben Teil werden dann dort hinzugefügt)
            weitere Auswertung Header, wenn _curVarCondition gesetzt:
                Iterationsheader -> Fehler
                Variablenhaltiger Header: mit Operation ICS_OPER_AND an _curVarCondition anfügen
                Sonst: normal auswerten (wenn false, dann zum nächsten Teil übergehen) (eventuell gesetztes _curVarCondition löschen, wie bei jedem Übergang zum nächsten Teil)
            Auswertung Körper:
                Flag setzen, dass Codeblock mit variablenhaltiger Bedingung (erst hier, da vorher Abbruch durch auf false auswertenden Header erfolgt sein kann)
                Initialisierung Mapping-Objekt im Ausführungskontext (dorthin auch _curVarCondition)
                Normale Ausführung (aber im Ausführungskontext dann Verwendung Mapping-Objekt)
                Ende des Körpers:
                    Wenn _cbResult gesetzt ist: als zusätzliches spezielles Symbol über das Mapping-Objekt eintragen
                    Wenn _curVarCondition gesetzt ist: KEIN Sprung zum Ende des Codeblocks, sondern fortfahren mit nächstem Teil (wenn es einen gibt)
            Beginn nächster Teil (wenn Flag gesetzt, dass Codeblock mit variablenhaltiger Bedingung):
                _curVarCondition löschen (dagegen Flag bleibt gesetzt)
                wenn aktueller Teil (entsprechend seinen Parametern) Iterationen hat oder repeat darauf zielt   -> Fehler
                Headerauswertung normal bzw. wie oben
            Auswertung Körper:
                wenn Flag gesetzt ist, dass variablenhaltige Bedingung, dann wie oben ausführen (auch wenn _curVarCondition hier nicht gesetzt sein sollte; Mapping-Objekt bekommt dann Bedingung "true")
                (wenn _curVarCondition nicht gesetzt ist, dann am Ende doch Sprung zum Codeblockende)
            Codeblockende bzw. Heraussprung mit break/continue (wenn Flag gesetzt, dass Codeblock mit variablenhaltiger Bedingung):
                (Sprung mit repeat auf diesen Codeblock -> Fehler)
                Mapping-Objekte aller ausgeführten Teile zusammenführen (einschließlich zusätzlicher spezieller Symbole für Codeblockergebnis, und für Variablen und Restriktionen)
                    (dabei Berücksichtigung, ob letzter Teil Bedingung "true" hatte)
                Zusammengeführtes Codeblockergebnis übernehmen
                (Ziel für Zusammenführung für Symbole kann durchaus Mapping-Objekt eines übergeordneten Codeblocks sein (in dem Fall können Variablen und Restriktionen noch nicht final zusammengeführt werden))
                Mapping aus Ausführungskontext löschen

            TODO:
                Vorgehen bei Codeblock-Control Kommandos?
                    (ist da noch zusaetzlich etwas anzupassen?)
         */
    private:
        /**
         * constructor
         * @param ctx       execution context
         * @param cd        intermediary code command (here INTCODE_CODEBLOCK/ICS_CB_BLOCK_START)
         * @param lvl       position of this codeblock context object within the codeblock context stack of the execution context
         */
        CodeBlockContext(ExecContext *ctx, IntCode::IcElem *cd, unsigned lvl):
            _execContext(ctx), _icStart(cd), _lvl(lvl), _execPart(-1), _curPart(-1), _curBodyCode(0),
            _hasVarCond(false), _curVarCondition(TP_EMPTY), _startIter(NULL), _startIterObj(NULL),
            _cbResult(TP_BLANK), _cancel(false), _ctrlCmd(0)                                            { }

    public:
        /**
         * destructor
         */
        ~CodeBlockContext();

        /**
         * create new code block context object if needed at code block start
         * @param ctx       execution context
         * @param cd        intermediary code command (here INTCODE_CODEBLOCK/ICS_CB_BLOCK_START)
         * @return          pointer to new code block context or NULL if no context is needed
         */
        static CodeBlockContext *newContextStart(ExecContext *ctx, IntCode::IcElem *cd);

        /**
         * create new code block context object when needed whitin the execution of the codeblock
         * @param ctx       execution context
         * @param hd        intermediary code command (here INTCODE_CB_HEADER/ICS_CBHEADER_END)
         * @return          pointer to new code block context
         */
        static CodeBlockContext *newContextAdd(ExecContext *ctx, IntCode::IcElem *hd);

        /**
         * stores a codeblock header for later execution
         * @param cd        intermediary code command for codeblock header start
         */
        void storeHeader(IntCode::IcElem *cd);

        /**
         * stores a codeblock header with iteration
         * @param cbhIter   iteration info / NULL: insert pseudo iteration if neccessary
         * @param cd        intermediary code command for codeblock header start
         */
        void storeHeader(CBHeaderIterInfo *cbhIter, IntCode::IcElem *cd);

        /**
         * get whether ordered execution for current part of this codeblock
         */
        bool ordered() const;

        /**
         * get count of codeblock parts
         */
        inline unsigned partCnt() const                     { return (_icStart->v.c.par & ICPAR_BLOCK_CNTPARTS); }

        /**
         * get id of syntax element of the codeblock
         */
        inline unsigned syntaxElem() const                  { return _icStart->se; }

        /**
         * get whether codeblock has conditions over optimization variables
         */
        inline bool hasVarCondition() const                 { return (_hasVarCond || _curVarCondition); }

        /**
         * add a conditions over optimization variable for the current codeblock part
         * @param v             value with the condition formula (must be TP_FORMULA or TP_BIN with value true)
         * @param se            syntax element of v
         */
        void addCurVarCondition(CmplVal *v, unsigned se);

        /**
         * set value of row/rcol name prefix at start of this codeblock
         * @param nmp           name prefix
         * @param nms           type of setting the name prefix
         */
        inline void setNmPrefStart(intType nmp, int nms)          { _nmPrefStart = nmp; _nmPrefStartSet = nms; }

        /**
         * get codeblock result value
         */
        CmplVal& cbResult();

        /**
         * start execution of a part of the codeblock
         * @param cd            intermediary code command
         */
        void startPart(IntCode::IcElem *cd);

        /**
         * execute the body of a codeblock part
         * @param cd            intermediary code command
         */
        void execPartBody(IntCode::IcElem *cd);

        /**
         * end of codeblock part
         * @return              true if codeblock completed (jump to codeblock end) or false if proceeding with next part
         */
        bool endPart();


    public:
        /**
         * set result value and cancel flag for codeblock control command; call this function in the destination codeblock of the control command
         * @param srcCtx	source execution context
         * @param cmd		control command (one of ICS_CBC_BREAK, ICS_CBC_CONTINUE, ICS_CBC_REPEAT)
         * @param srcLvl	nesting codeblock level of source codeblock
         * @param lhs		index tuple on left hand side of the control command / NULL: no left hand side / TP_EMPTY: left hand side without index tuple
         * @param rhs		right hand side of the control command / NULL: no right hand side
         */
        void controlCommand(ExecContext *srcCtx, int cmd, unsigned srcLvl, StackValue *lhs = NULL, StackValue *rhs = NULL);

        /**
         * set and propagate cancel flag in nesting codeblocks within the destination codeblock
         * @param cmd		control command (one of ICS_CBC_BREAK, ICS_CBC_CONTINUE, ICS_CBC_REPEAT)
         * @param dstLvl	nesting codeblock level of destination codeblock of the control command (must be lesser than <code>_lvl</code>)
         */
        void setCancel(int cmd, unsigned dstLvl);

        /**
         * assign value to codeblock result
         * @param val		value to assign
         * @param tpl		index tupel for value
         */
        void assignResult(StackValue *val, StackValue *tpl = NULL);

        /**
         * assign value to codeblock result
         * @param val		value to assign
         * @param ovr		override all existing results
         * @param se		syntax element id of source value
         */
        void assignResult(CmplVal &val, bool ovr, unsigned se);

        /**
         * check whether codeblock is subject of a repeat command, if yes then init codeblock for restart
         * @return          start command of codeblock in the intermediary code / 0: if no repeat command
         */
        IntCode::IcElem *checkStartRepeat();
    };



    /************** codeblock header **********/

    /**
     * base class for a single codeblock header
     */
    class CodeBlockHeaderBase
    {
    protected:
        unsigned short _headNum;			//< number of codeblock header
        unsigned _code;                     //< position in the intermediary code where the header starts

    public:
        /**
         * get resulting value from codeblock header
         * @param ctx           execution context
         * @param res           store for result value (can only be one of TP_BIN, TP_CBHEAD_ITER, TP_FORMULA)
         */
        virtual void getResult(ExecContext *ctx, CmplVal& res) = 0;

        /**
         * destructor
         */
        virtual ~CodeBlockHeaderBase()          { }

        /**
         * get syntax element number of the codeblock header
         * @param ctx           execution context
         */
        unsigned syntaxElem(ExecContext *ctx) const;


        /************** functions for evaluation of codeblock headers defining new codeblock symbols **********/
    public:
        /**
         * executes equality operation with codeblock symbol definition
         * @param ctx			execution context
         * @param cbs           value for codeblock symbol definition, must be TP_DEF_CB_SYM or TP_DEF_CB_SYM_TUPLE
         * @param a2            other operand
         * @param se			syntax element id of operation
         * @return              result value
         */
        static bool execCbsOperEq(ExecContext *ctx, CmplVal *cbs, StackValue *a2, unsigned se);

        /**
         * executes "in" or "of" operation with codeblock symbol definition
         * @param ctx			execution context
         * @param opIn          true if "in" operation, false if "of" operation
         * @param cbs           value for codeblock symbol definition, must be TP_DEF_CB_SYM or TP_DEF_CB_SYM_TUPLE
         * @param a2            other operand
         * @param se			syntax element id of operation
         * @return              result value
         */
        static CBHeaderIterInfo *execCbsOperIn(ExecContext *ctx, bool opIn, CmplVal *cbs, StackValue *a2, unsigned se);
    };


    /**
     * class for a single codeblock header, which must be executed
     */
    class CodeBlockHeaderExec : public CodeBlockHeaderBase
    {
    public:
        /**
         * get resulting value from codeblock header
         * @param ctx           execution context
         * @param res           store for result value (can only be one of TP_BIN, TP_CBHEAD_ITER, TP_FORMULA)
         */
        virtual void getResult(ExecContext *ctx, CmplVal& res);

    public:
        /**
         * constructor
         * @param ctx       execution context
         * @param cd        intermediary code command for codeblock header start
         */
        CodeBlockHeaderExec(ExecContext *ctx, IntCode::IcElem *cd);
    };


    /**
     * class for a single codeblock header, which contains a set for iteration
     */
    class CodeBlockHeaderIter : public CodeBlockHeaderBase
    {
    protected:
        CBHeaderIterInfo *_iterInfo;            ///< info for iteration

    public:
        /**
         * get resulting value from codeblock header
         * @param ctx           execution context
         * @param res           store for result value (can only be one of TP_BIN, TP_CBHEAD_ITER, TP_FORMULA)
         */
        virtual void getResult(ExecContext *ctx, CmplVal& res)        { res.setP(TP_CBHEAD_ITER, _iterInfo); }

    public:
        /**
         * constructor
         * @param ctx       execution context
         * @param cd        intermediary code command for codeblock header start
         * @param cbhIter   iteration info
         */
        CodeBlockHeaderIter(ExecContext *ctx, IntCode::IcElem *cd, CBHeaderIterInfo *cbhIter);

        /**
         * destructor
         */
        virtual ~CodeBlockHeaderIter();
    };



    /************** codeblock symbol assign info **********/

    /**
     * info for assignment of iteration values to codeblock symbols within an iteration for a codeblock header.
     * abstract base class.
     */
    class CBAssignInfoBasis
    {
    public:
        /**
         * destructor
         */
        virtual ~CBAssignInfoBasis()                { }

        /**
         * set the values for codeblock symbols within the execution context
         * @param ctx		execution context
         * @param curIter	current tuple within the iteration
         * @param curInd	current index number within the iteration
         * @return          symbol number (fetch id) of last set symbol + 1
         */
        virtual unsigned setCBSymbolValues(ExecContext *ctx, const CmplVal& curIter, unsigned long curInd) = 0;

        /**
         * get number of the first codeblock symbol set by this assignment info
         * @param ctx		execution context
         * @return          symbol number (fetch id)
         */
        virtual unsigned numCBSymbolsFrom(ExecContext *ctx) const = 0;

        /**
         * get this transformed to CBAssignInfoTuple
         * @param r         rank of iteration set this assignment info is used for
         * @return          CBAssignInfoTuple object (can be this)
         */
        virtual CBAssignInfoTuple *asAssignInfoTuple(unsigned r) = 0;
    };

    /**
     * info for assignment of iteration values to codeblock symbols within an iteration for a codeblock header.
     * empty info without any assignment.
     */
    class CBAssignInfoEmpty : public CBAssignInfoBasis
    {
    public:
        /**
         * set the values for codeblock symbols within the execution context
         * @param ctx		execution context
         * @param curIter	current tuple within the iteration
         * @param curInd	current index number within the iteration
         * @return          symbol number (fetch id) of last set symbol + 1
         */
        virtual unsigned setCBSymbolValues(ExecContext *ctx, const CmplVal& curIter, unsigned long curInd)	{ /* nothing to do */ return 0; }

        /**
         * get number of the first codeblock symbol set by this assignment info
         * @param ctx		execution context
         * @return          symbol number (fetch id)
         */
        virtual unsigned numCBSymbolsFrom(ExecContext *ctx) const           { return 0; }

        /**
         * get this transformed to CBAssignInfoTuple
         * @param r         rank of iteration set this assignment info is used for
         * @return          CBAssignInfoTuple object (can be this)
         */
        virtual CBAssignInfoTuple *asAssignInfoTuple(unsigned r);
    };

    /**
     * info for assignment of iteration values to codeblock symbols within an iteration for a codeblock header.
     * assignment of the current iteration tuple to a single codeblock symbol
     */
    class CBAssignInfoSingle : public CBAssignInfoBasis
    {
    private:
        unsigned _cbFetchID;		///< fetch id of the codeblock symbol within the execution context

    public:
        /**
         * constructor
         * @param cbFetchID	fetch id of the codeblock symbol within the execution context
         */
        CBAssignInfoSingle(unsigned cbFetchID): _cbFetchID(cbFetchID)	{ }

        /**
         * set the values for codeblock symbols within the execution context
         * @param ctx		execution context
         * @param curIter	current tuple within the iteration
         * @param curInd	current index number within the iteration
         * @return          symbol number (fetch id) of last set symbol + 1
         */
        virtual unsigned setCBSymbolValues(ExecContext *ctx, const CmplVal& curIter, unsigned long curInd);

        /**
         * get number of the first codeblock symbol set by this assignment info
         * @param ctx		execution context
         * @return		symbol number (fetch id)
         */
        virtual unsigned numCBSymbolsFrom(ExecContext *ctx) const
        {
            return _cbFetchID;
        }

        /**
         * get this transformed to CBAssignInfoTuple
         * @param r         rank of iteration set this assignment info is used for
         * @return          CBAssignInfoTuple object (can be this)
         */
        virtual CBAssignInfoTuple *asAssignInfoTuple(unsigned r);
    };

    /**
     * info for assignment of iteration values to codeblock symbols within an iteration for a codeblock header.
     * assignment of the current iteration tuple to a tuple of codeblock symbols
     */
    class CBAssignInfoTuple : public CBAssignInfoBasis
    {
    protected:
        unsigned _cnt;              ///< count of fetch ids (length of _cbFetchIDs)
        unsigned *_cbFetchIDs;		///< array with fetch ids of the codeblock symbols within the execution context
        unsigned *_tpLen;           ///< array with tuple part length per codeblock symbol

    public:
        /**
         * constructor
         * @param tpl		tuple with fetch ids of the codeblock symbols within the execution context
         * @param r         rank of iteration set this assignment info is used for / 0: not known
         */
        CBAssignInfoTuple(Tuple *tpl, unsigned r = 0);

        /**
         * constructor: merge two assignment info objects
         * @param ai1		assignment info object #1
         * @param ai2		assignment info object #2
         */
        CBAssignInfoTuple(CBAssignInfoTuple *ai1, CBAssignInfoTuple *ai2);

    protected:
        /**
         * constructor
         */
        CBAssignInfoTuple()     { }

    public:
        /**
         * constructor
         * @param cnt           count of fetch ids
         * @param cbFetchIDs    array with fetch ids of the codeblock symbols within the execution context
         * @param tpLen         array with tuple part length per codeblock symbol
         */
        CBAssignInfoTuple(unsigned cnt, unsigned *cbFetchIDs, unsigned *tpLen): _cnt(cnt), _cbFetchIDs(cbFetchIDs), _tpLen(tpLen)       { }

        /**
         * destructor
         */
        virtual ~CBAssignInfoTuple()                { DELETE_UNSET(_cbFetchIDs); DELETE_UNSET(_tpLen); }

        /**
         * set the values for codeblock symbols within the execution context
         * @param ctx		execution context
         * @param curIter	current tuple within the iteration
         * @param curInd	current index number within the iteration
         * @return          symbol number (fetch id) of last set symbol + 1
         */
        virtual unsigned setCBSymbolValues(ExecContext *ctx, const CmplVal& curIter, unsigned long curInd);

        /**
         * get number of the first codeblock symbol set by this assignment info
         * @param ctx		execution context
         * @return		symbol number (fetch id)
         */
        virtual unsigned numCBSymbolsFrom(ExecContext *ctx) const;

        /**
         * get this transformed to CBAssignInfoTuple
         * @param r         rank of iteration set this assignment info is used for
         * @return          CBAssignInfoTuple object (can be this)
         */
        virtual CBAssignInfoTuple *asAssignInfoTuple(unsigned r);
    };

    /**
     * info for assignment of iteration values to codeblock symbols within an iteration for a codeblock header.
     * individual assignment for any current iteration tuple to codeblock symbols
     */
    class CBAssignInfoComplex : public CBAssignInfoTuple
    {
    private:
        /**
         * single assignment info
         */
        struct oneInfo {
            unsigned long _start;	///< this info is valid from the element with this iteration index
            unsigned long _end;		///< this info is not valid from the element with this iteration index / 0: no upper index bound
            unsigned *_rank;		///< array with rank to assign (length is CBAssignInfoTuple::_cnt)

            /**
             * constructor
             */
            oneInfo(unsigned long s, unsigned long e, unsigned *r): _start(s), _end(e), _rank(r)    { }

            /**
             * compare if array with ranks is equal to another array
             * @param c     count of elements in the array
             * @param r     other array
             * @return      true if both arrays are equal
             */
            bool ranksEq(unsigned c, unsigned *r)       { for (unsigned i = 0; i < c; i++) { if (_rank[i] != r[i]) { return false; } } return true; }
        };

        vector<oneInfo> _info;		///< assignment info objects, ordered by oneInfo::_from
        unsigned _curInfo;          ///< current used info object

    public:
        /**
         * constructor
         * @param tpl		tuple with fetch ids of the codeblock symbols within the execution context
         * @param pos       if given then use only these positions of tuple
         */
        CBAssignInfoComplex(CmplVal &tpl, vector<unsigned> *pos = NULL);

        /**
         * destructor
         */
        ~CBAssignInfoComplex();

        /**
         * add single assignment info
         * @param pos       position of element the assign info is for
         * @param ranks     array with ranks to assign
         */
        void addInfo(unsigned long pos, unsigned *ranks);

        /**
         * set the values for codeblock symbols within the execution context
         * @param ctx		execution context
         * @param curIter	current tuple within the iteration
         * @param curInd	current index number within the iteration
         * @return          symbol number (fetch id) of last set symbol + 1
         */
        virtual unsigned setCBSymbolValues(ExecContext *ctx, const CmplVal& curIter, unsigned long curInd);

        /**
         * get this transformed to CBAssignInfoTuple
         * @param r         (not used here)
         * @return          CBAssignInfoTuple object / NULL: not convertible
         */
        virtual CBAssignInfoTuple *asAssignInfoTuple(unsigned r);
    };



    /************** codeblock iteration **********/

    /**
     * iteration info for a codeblock header
     */
    class CBHeaderIterInfo
    {
    protected:
        CmplVal _set;                       ///< iteration set
        CBAssignInfoBasis *_assignInfo;     ///< info for codeblock symbol assignment

    public:
        /**
         * constructor
         * @param set           iteration set
         * @param ai            info for codeblock symbol assignment
         */
        inline CBHeaderIterInfo(CmplVal& set, CBAssignInfoBasis *ai): _set(set), _assignInfo(ai)            { }

        /**
         * constructor
         * @param cbhii         iteration set
         * @param ai            info for codeblock symbol assignment
         */
        inline CBHeaderIterInfo(CBHeaderIterInfo& cbhii): _set(cbhii._set), _assignInfo(cbhii._assignInfo)  { }

        /**
         * constructor
         */
        inline CBHeaderIterInfo()               { _set.set(TP_SET_NULL); _assignInfo = new CBAssignInfoEmpty(); }

        /**
         * destructor
         */
        inline ~CBHeaderIterInfo()              { _set.dispose(); delete _assignInfo; }

        /**
         * get iteration set
         */
        inline CmplVal& iterSet()                { return _set; }

        /**
         * check if another iteration info can be merged to this and merge it
         * @param cbhIter       other iteration info
         * @param ordered       for ordered iteration
         * @return              true if cbhIter is merged to this
         */
        bool merge(CBHeaderIterInfo *cbhIter, bool ordered);
    };


    /**
     * class for execution of the iteration within a codeblock part
     */
    class CodeBlockIteration : public CBHeaderIterInfo
    {
        friend class CBThreadInfo;

    private:
        ExecContext *_execCtx;                  ///< execution context in which the execution is started
        CodeBlockContext *_cbContext;			///< codeblock context to which this belongs
        int _headNum;                           ///< number of codeblock header in _cbContext->_headers / -1: initial iteration in _cbContext->_startIter

        /************** instance variables only used within execute() **********/
        CBThreadInfo *_thdInfo;                 ///< array with thread infos / NULL: no threads used
        unsigned _thdInfoSize;                  ///< size of array _thdInfo
        mutex _thdInfoMtx;                      ///< mutex for synchronizing access to _thdInfo (and to _nextIter)

        int _nextHead;                          ///< number of next non-processed codeblock header / -1: no such codeblock header
        CodeBlockIteration *_nextIter;			///< next codeblock iteration / NULL: no next iteration or iteration in separate threads

        volatile CmplArray *_result;            ///< result array / NULL: not needed or not initialized

        volatile ModuleException *_excModule;   ///< pointer to module exception thrown by separate thread
        volatile FatalException *_excFatal;     ///< pointer to fatal exception thrown by separate thread

    public:
        /**
         * constructor
         * @param ctx                   execution context in which the execution is started
         * @param cbc                   codeblock context to which this belongs
         * @param cbhIter               iteration set and assign info for codeblock symbols
         * @param hn                    number of codeblock header or -1 for initial iteration
         */
        inline CodeBlockIteration(ExecContext *ctx, CodeBlockContext *cbc, CBHeaderIterInfo *cbhIter, int hn):
            CBHeaderIterInfo(*cbhIter), _execCtx(ctx), _cbContext(cbc), _headNum(hn),
            _thdInfo(NULL), _thdInfoSize(0), _nextHead(0), _nextIter(NULL),
            _result(NULL), _excModule(NULL), _excFatal(NULL)                         { }

        /**
         * destructor
         */
        ~CodeBlockIteration();


        /**
         * execute iteration for a code block part
         * @return		result array / NULL: no result
         */
        CmplArray *run();

        /**
         * execution for one iteration step
         * @param thd               thread info object / NULL: no threading
         * @param ctx               execution context
         * @param curIter           current element of iteration
         * @param curInd            current index number of iteration
         */
        void execIter(CBThreadInfo *thd, ExecContext *ctx, const CmplVal &curIter, unsigned long curInd);

        /**
         * execution for one iteration element
         * @param thd               thread info object / NULL: no threading
         * @param ctx               execution context
         * @param nextHead          number of next header in the codeblock context / -1: no next header
         * @param curInd            current index number of iteration
         */
        void execElem(CBThreadInfo *thd, ExecContext *ctx, int nextHead, unsigned long curInd);

        /**
         * set info for next iteration object
         * @param ni                next iteration object
         */
        void setNextIter(CodeBlockIteration *ni);

        /**
         * set and propagate cancel flag
         * @param cmd               control command (one of ICS_CBC_BREAK, ICS_CBC_CONTINUE, ICS_CBC_REPEAT)
         * @param dstLvl            nesting codeblock level of destination codeblock of the control command
         */
        void setCancel(int cmd, unsigned dstLvl);

    private:
        /**
         * init result array
         */
        inline void initResult()                                        { LockGuard<mutex>((_thdInfo != NULL), _thdInfoMtx); if (!_result) { _result = new CmplArray(_set, TP_NULL); } }

        /**
         * set value in the result array
         * @param i                 index of element in the result array
         * @param v                 value
         */
        inline void setResult(unsigned long i, CmplVal& v)              { if (!_result) { initResult(); } CmplArray *r = const_cast<CmplArray *>(_result); r->at(i)->moveFrom(v, true); }
                                                                            // _result is changed only in initialization, so after that discarding volatile is safe

        /**
         * set subarray as value in the result array
         * @param i                 index of element in the result array
         * @param v                 subarray
         */
        inline void setResult(unsigned long i, CmplArray *a)            { if (!_result) { initResult(); } CmplArray *r = const_cast<CmplArray *>(_result); r->at(i)->dispSet(TP_ARRAY_COMP, a); }
                                                                            // _result is changed only in initialization, so after that discarding volatile is safe

        /**
         * set exception thrown by separate thread
         * @param ex                exception
         */
        void setException(ModuleException& ex);

        /**
         * set exception thrown by separate thread
         * @param ex                exception
         */
        void setException(FatalException& ex);

        /**
         * set exception thrown by separate thread
         * @param ex                exception / NULL: unknown object thrown
         */
        void setException(exception *ex);
    };


    /**
     * info object class for starting a new thread
     */
    class CBThreadInfo
    {
        friend class CodeBlockIteration;

    private:
        volatile bool _used;                    ///< this object is currently in use
        thread *_thread;                        ///< started thread
        ExecContext *_execCtx;                  ///< execution context to use in the thread / NULL: not initialized
        CmplVal _curIter;                       ///< current iteration value

        volatile CodeBlockIteration *_nextIter; ///< next codeblock iteration / NULL: no next iteration
        mutex *_nextIterMtx;                    ///< mutex for synchronizing access to _nextIter (only set if _nextIter is first used)

        /**
         * constructor
         */
        CBThreadInfo(): _used(false), _thread(NULL), _execCtx(NULL), _nextIter(NULL), _nextIterMtx(NULL)	{ }

        /**
         * destructor
         */
        ~CBThreadInfo();

        /**
         * searches unused entry in array <code>cbIter->_thdInfo</code> and starts a new thread
         * @param cbIter        object for iteration in a codeblock
         * @param iter          iterator object, iterated to the current iteration step
         */
        static void start(CodeBlockIteration *cbIter, SetIterator& iter);

        /**
         * main function for a new thread, executed by the thread <code>thdInfo->_thread</code>
         * @param thdInfo       info object for threading
         * @param cbIter        object for iteration in a codeblock
         * @param curInd        current index number of iteration
         * @param decAfter      decrement count of worker threads at the end of this function
         */
        static void execThread(CBThreadInfo *thdInfo, CodeBlockIteration *cbIter, unsigned long curInd, bool decAfter);

        /**
         * wait for ending of the started thread
         */
        void join();

        /**
         * wait for ending of all threads started for a codeblock iteration
         * @param cbIter        object for iteration in a codeblock
         */
        static void joinAll(CodeBlockIteration *cbIter);

        /**
         * set info for next iteration object
         * @param ni            next iteration object
         */
        void setNextIter(CodeBlockIteration *ni);

        /**
         * set and propagate cancel flag
         * @param cbIter        object for iteration in a codeblock
         * @param cmd           control command (one of ICS_CBC_BREAK, ICS_CBC_CONTINUE, ICS_CBC_REPEAT)
         * @param dstLvl        nesting codeblock level of destination codeblock of the control command
         */
        static void setCancel(CodeBlockIteration *cbIter, int cmd, unsigned dstLvl);
    };

}

#endif // CODEBLOCKCONTEXT_HH

