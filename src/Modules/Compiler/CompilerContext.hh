
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


#ifndef COMPILERCONTEXT_HH
#define COMPILERCONTEXT_HH

#include <stack>

#include "Compiler.hh"
#include "CompilerUtil.hh"
#include "../../Control/MainControl.hh"
#include "../../CommonData/ExtensionSteps.hh"
#include "../../CommonData/CmplSourceInput.hh"
#include "../../CommonData/SyntaxElements.hh"
#include "../../CommonData/IntCode.hh"
#include "_CMPL_Parser.hh"


#define EXPSTR		CMPL_Parser::semantic_type::exp_str


using namespace std;


namespace cmpl
{
	class Compiler;
	class MainControl;
	class MainData;
	class ModuleBase;

	
    /**
	 * the <code>CompilerContext</code> class holds the context for the parser
	 */
	class CompilerContext
	{
		friend class CMPL_Parser;

	private:
		Compiler *_modp;							///< compiler module
		void *_scanner;								///< scanner context

    public:
		/**
		 * constructor
		 */
		CompilerContext(Compiler *mod);

		/**
		 * destructor
		 */
		~CompilerContext();

		/**
		 * main function, executes the parser and generate the intermediary code
		 */
		void parse();

	public:
		/**
		 * get name of the compiler module
		 */
		inline const char *moduleName() const      { return _modp->moduleName(); }

		/**
		 * get main object
		 */
		inline MainControl *ctrl() const            { return _modp->ctrl(); }

		/**
		 * get data object
		 */
		inline MainData *data() const               { return _modp->data(); }

		/**
		 * get this module
		 */
		inline ModuleBase *modp()                   { return _modp; }

		/**
		 * get this module, as the real type
		 */
		inline Compiler *modpt()					{ return _modp; }

		/**
		 * get whether to make only syntax check, no generating of intermediary code
		 */
		inline bool checkOnly()						{ return _modp->checkOnly(); }


	private:
		/**
		 * Starts the scanner
		 */
		void scanBegin();

		/**
		 * Stops the scanner
		 */
		void scanEnd();



		/************** syntax structure **********/
	private:
		SyntaxElement *_syntaxStructure;			///< root element of syntax structure of cmpl input
		map<unsigned, SymbolInfo *> *_symInfo;		///< symbol infos

		string _prvErrorMsg;						///< previous parser error message
		vector<SyntaxElement *> _errElems;			///< syntactic elements discarded by error recovery
		bool _parserError;							///< error while parsing the cmpl input

		unsigned _curIdSyntaxElem;					///< current id of syntax element


		/**
		 * set root element of syntax structure of cmpl input
		 * @param r                 root element
		 * @param loc               location of root element
		 */
		void setSyntaxStructure(SyntaxElement *r, LocationInfo *loc);

	public:
		/**
		 * add symbol info
		 * @param n					symbol definition number
		 * @param sym				symbol info
		 */
		inline void addSymbolInfo(unsigned n, SymbolInfo *sym)			{ (*_symInfo)[n] = sym; }

		/**
		 * output parser error message
	 	 * @param lvl				error level
		 * @param m					error message
		 * @param l					location of error
		 * @param pe				set _parserError to true
		 */
		void outParseError(int lvl, const string& m, const LocationInfo& l, bool pe = false);

		/**
		 * get previous parser error message as c-string stored in the global string store
		 */
		inline const char *prvParseErrorCStr()							{ return data()->globStrings()->store(_prvErrorMsg); }

		/**
		 * insert syntactic element while error recovery
		 */
		inline void insertErrElem(SyntaxElement *e)						{ _errElems.push_back(e); }

		/**
		 * get syntactic element of error recovery
		 * @param loc				location of error
		 */
		SyntaxElement* errorRecoveryElem(LocationInfo *loc);

		/**
		 * get next id for syntax element
		 */
		inline unsigned nextIdSyntaxElem()								{ return ++_curIdSyntaxElem; }



		/************** location handling **********/
	private:
		map<int, LocationInfo *> _defLoc;			///< defined locations

		PositionInfo _firstPosition;				///< first known position in cmpl input
		bool _hasFirstPosition;						///< flag whether _firstPosition is set

		int _scanModeRestore;						///< scanner mode to restore after processing of position mark

	public:
		/**
		 * set InLoc in position
		 * @param curLoc			current location
		 * @param pos               position to set inLoc within
		 * @param inLoc             inLoc number as parsed from position mark
		 */
		void setInLocPM(LocationInfo *curLoc, PositionInfo& pos, int inLoc);

		/**
		 * parse DefLoc position mark
		 * @param curLoc			current location
		 * @param txt               text with DefLoc info
		 */
		void parseDefLocPM(LocationInfo *curLoc, const char *txt);

		/**
		 * parse StartPos position mark
		 * @param curLoc			current location
		 * @param txt               text with StartPos info
		 */
		void parseStartPosPM(LocationInfo *curLoc, const char *txt);

		/**
		 * parse Line position mark
		 * @param curLoc			current location
		 * @param txt               text with line number
		 */
		void parseLinePM(LocationInfo *curLoc, const char *txt);

		/**
		 * parse Col position mark
		 * @param curLoc			current location
		 * @param txt               text with col number
		 */
		void parseColPM(LocationInfo *curLoc, const char *txt);

		/**
		 * output error message about wrong position mark
		 * @param curLoc			current location
		 * @param txt               text with error
		 */
		void outErrorPositionMark(LocationInfo *curLoc, const char *txt);


		/**
		 * get scanner mode to restore after processing of position mark
		 */
		inline int scanModeRestore()					{ return _scanModeRestore; }

		/**
		 * set scanner mode to restore after processing of position mark
		 */
		inline void setScanModeRestore(int m)			{ _scanModeRestore = m; }



		/************** symbol management and parse scopes **********/
	private:
		vector<ParseScope *> _parseScopes;		///< stack with parsing scopes

		SymbolTab *_symbols;					///< definition info for symbols
		SymbolTab *_subSymbols;					///< definition info for subsymbols

		unsigned _symbolDefCnt;					///< counter for SymbolDef::_defId
		unsigned _symbolGlobCnt;				///< counter for SymbolDef::_globId

		LocationInfo _predefLoc;				///< pseudo definition location for pre-defined symbols

		AssignModificators _modAssignCb;		///< assign modificators used for codeblock symbols

	public:
		/**
		 * stack with parsing scopes
		 */
		inline vector<ParseScope *>& parseScopes()		{ return _parseScopes; }

		/**
		 * current parse scope info
		 */
		inline ParseScope *curParseScope()				{ return _parseScopes.back(); }

		/**
		 * get second topmost parse scope info
		 */
		inline ParseScope *secParseScope()				{ return (_parseScopes.size() >= 2 ? _parseScopes[_parseScopes.size() - 2] : NULL); }

		/**
		 * get parse scope of a given level
		 * @param i				scope level
		 */
        inline ParseScope *parseScopeAt(unsigned i)		{ return (_parseScopes.size() > i ? _parseScopes[i] : NULL); }

		/**
		 * current parse scope level
		 */
		inline unsigned curParseScopeLevel()			{ return _parseScopes.size() - 1; }


		/**
		 * reset the current assign modificators
		 */
		inline void clearModificators()					{ curParseScope()->_assignModCurrent.init(); }

		/**
		 * set single modificator in current assign modificators
		 * @param loc           location, only for error handling
		 * @param mod           modificator (only used if odType is NULL)
		 * @param odType        symbol for object type or data type
		 * @param isObjType     odType is object type
		 */
		inline void setModificator(LocationInfo *loc, ModificatorKey mod)						{ curParseScope()->_assignModCurrent.setModificator(this, loc, mod, NULL, false); }

		/**
		 * set single modificator (data type or object type) in current assign modificators
		 * @param loc           location, only for error handling
		 * @param mod           modificator (only used if odType is NULL)
		 * @param odType        symbol for object type or data type
		 * @param isObjType     odType is object type
		 */
		inline void setModificator(LocationInfo *loc, SymbolDef *odType, bool isObjType)		{ curParseScope()->_assignModCurrent.setModificator(this, loc, modificatorPublic, odType, isObjType); }

		/**
		 * get whether there are no current assign modificators
		 * @param saveObj		don't check for object type
		 */
		inline bool assignModCurrentEmpty(bool saveObj = false)									{ return curParseScope()->_assignModCurrent.empty(saveObj); }

		/**
		 * get whether assign modificators indicate a codeblock control assignment
		 * @param loc           location, only for error handling
		 */
		inline bool assignModControlCB(LocationInfo *loc)										{ ModificatorKey cb; return curParseScope()->_assignModCurrent.isControlCBAssign(this, loc, cb); }

		/**
		 * set directive from current assign modificators
		 * @param loc           location, only for error handling
		 */
		inline void setDirectiveFromCurrent(LocationInfo *loc)									{ curParseScope()->setDirectiveFromCurrent(loc); }

		/**
		 * set line name prefix
		 * @param elem		syntax element
		 * @param loc		location, only for error handling
		 * @param lnp		line name prefix
		 */
		inline void setLineNamePref(SyntaxElement *elem, LocationInfo *loc, const char *lnp)	{ curParseScope()->setLineNamePref(elem, loc, lnp); }

		/**
		 * get whether a line name prefix is set
		 */
		inline bool hasLineNamePref()															{ return (curParseScope()->_lineNamePref != NULL); }

		/**
		 * get pointer to assign modificators used for codeblock symbols
		 */
		inline AssignModificators *modAssignCb()												{ return &_modAssignCb; }

		/**
		 * end of statement processing
		 * @param elem		syntax element
		 */
		inline void endOfStatement(SyntaxElement *elem)											{ curParseScope()->endOfStatement(elem); }

		/**
		 * get new unique symbol definition number
		 */
		inline unsigned newSymbolDefCnt()														{ return _symbolDefCnt++; }

		/**
		 * get new unique number for global symbol
		 */
		inline unsigned newSymbolGlobCnt()														{ return _symbolGlobCnt++; }

		/**
		 * initialize predefined symbols
		 */
		void initPredefSymbols();

		/**
		 * insert one predefined symbol
		 * @param name      symbol name
		 * @param sub       symbol or subsymbol
		 * @param ro        read-only symbol
		 * @param special   special meaning for the symbol
		 * @param predef	predefined symbol value (0: no predefined symbol; >0: pseudo symbol (one of ICS_SPECIAL_*); <0: predefined symbol (one of PREDEFSYMVAL_*) (if >1000 or <-1000 then predefined by an extension)
         * @param predefCont only for predefined subsymbol: predef of container the subsymbol belongs to
		 * @return          symbol info
		 */
        SymbolDef *insertPredefSymbol(const char *name, bool sub, bool ro, SpecialSymbol special, int predef, int predefCont);

		/**
		 * insert one symbol defined by an extension
		 * @param name      symbol name
		 * @param sub		symbol or subsymbol
		 * @param ro        read-only symbol
		 * @param special   special meaning for the symbol
		 * @param extId		identification number of the extension registration
         * @param extCont   only for predefined subsymbol: extId of container the subsymbol belongs to
		 */
        void insertExtensionSymbol(const char *name, bool sub, bool ro, SpecialSymbol special, int extId, int extCont);

        /**
         * search plain subsymbol and insert it if not found, for use by extensions
         * @param name      symbol name
         * @param ins		insert if not found
         * @param nw        return whether the symbol is new inserted
         * @return          symbol info
         */
        SymbolDef *insertExtensionPlainSubSymbol(const char *name, bool ins, bool &nw)     { return searchInsertSymbol(name, _predefLoc, true, ins, nw); }

		/**
		 * search a symbol, and insert a new one if not found
		 * @param name      symbol name
		 * @param loc       location of the symbol
		 * @param sub       symbol or subsymbol
		 * @param ins       insert if not found
		 * @param nw        return whether the symbol is new inserted
		 * @return          symbol info
		 */
		SymbolDef *searchInsertSymbol(const char *name, LocationInfo& loc, bool sub, bool ins, bool &nw);

		/**
		 * search a symbol, and insert a new one if not found, and return as a new parse expression
		 * @param name      symbol name
		 * @param loc       location of the symbol
		 * @param sub       symbol or subsymbol
		 * @param ins       insert if not found
		 * @param mark		marking of symbol
		 * @return          symbol info
		 */
        ExprParseInfo *searchInsertSymbolExp(const char *name, LocationInfo& loc, bool sub, bool ins, MarkForSymbolName mark)		{ bool nw; SymbolDef *sym = searchInsertSymbol(name, loc, sub, ins, nw); return new ExprParseInfo(this, loc, sym, nw, mark); }

		/**
		 * check if extern symbol exists
         * @param nms           namespace / NULL: no namespace
         * @param name          symbol name
         * @param special		return of special meaning for the symbol
		 * @return				whether extern symbol exists
		 */
        bool checkExternSymbol(const char *nms, const char *name, SpecialSymbol& special);

		/**
		 * start a parse scope
	 	 * @param elem			syntax element
		 * @param loc			location, only for error handling
		 * @param tp			type of parse scope
		 * @param sptt			type of previous scanned token
		 */
		void startParseScope(SyntaxElement *elem, LocationInfo& loc, ParseScopeType tp, ScanPrevTokenType sptt = scanPrevTokenElse);

		/**
		 * ends a parse scope
	 	 * @param elem			syntax element
		 * @param loc			location, only for error handling
		 */
		void endParseScope(SyntaxElement *elem, LocationInfo& loc);

		/**
		 * get info about current function parse scope
		 * @param useSubs		return whether function uses subsymbols
		 * @return				count of local symbols used in the function
		 */
		unsigned short funcParseScopeInfo(bool& useSubs);

		/**
		 * set syntax element for code block part parse scope
		 * @param elem			syntax element
		 */
		void setPartParseScopeElem(SyntaxElement *elem)				{ ParseScope *ps = (curParseScope()->_scopeType == parseScopeTypePart ? curParseScope() : secParseScope()); if (ps) ps->_elem = elem; }

		/**
		 * set single parameter value for code block intermediary code command within parse scopes
		 * @param p				parameter to set (one of ICPAR_BLOCK_*)
		 * @param lvl			codeblock level count from the top parse scope
		 * @return				block parse scope
		 */
		ParseScope *setParseScopeParBlockPart(unsigned short p, unsigned lvl = 0);

		/**
		 * insert address of code block control command in parse scopes for code block and code block part
		 * @param pb			parse scope of code block
		 * @param addr			address of code block control command
		 */
		void insertParseScopeControlCBPos(ParseScope *pb, unsigned addr);

		/**
		 * check whether the current statement is a code block control command and compile it to the intermediary code
		 * @param elem			syntax element
		 * @param loc			location, only for error handling
		 * @param rhs			right hand side present
		 * @return				statement is a code block control command
		 */
		bool testCompControlCB(SyntaxElement *elem, LocationInfo& loc, bool rhs);

	private:
		/**
		 * enter a new parse scope
		 * @param pst           parse scope type
		 * @param cbm			mode for code block for distinction between expression and statement
		 */
		void enterParseScope(ParseScopeType pst, CodeBlockExprMode cbm = codeBlockExprModeElse);

		/**
		 * set the symbol state before leaving a parse scope
		 * @param pst           parse scope type
		 */
		void leaveParseScope(ParseScopeType pst);



		/************** distinction between expression and statement for codeblocks **********/
	private:
		ScanPrevTokenType _scanPrevTokenType;				///< type of previous scanned token

		std::stack<int> _scannerScopeLevel;					///< stack of scope levels maintained by the scanner; contains the scope level number in _parseScopes

		CodeBlockExprMode _cbExprModeCur;					///< current mode
		CodeBlockExprMode _cbExprModePrvSymbol;				///< mode before the previous symbol

		unsigned _prvEOSScopeLevel;							///< scanner scope level of previous end-of-statement token

	public:
		/**
		 * get type of previous scanned token
		 */
		inline ScanPrevTokenType scanPrevTokenType()				{ return _scanPrevTokenType; }

		/**
		 * set type of previous scanned token
		 */
		inline void setScanPrevTokenType(ScanPrevTokenType t)		{ _scanPrevTokenType = t; }

		/**
		 * reset type of previous scanned token
		 */
		inline void resetScanPrevTokenType()						{ _scanPrevTokenType = scanPrevTokenElse; }


		/**
		 * starts a scanner scope
		 */
		inline void startScannerScopeLevel()						{ _scannerScopeLevel.push(-1); /* parse scope number not known here */ }

		/**
		 * ends a scanner scope
		 * @return	codeblock mode effective for the ending codeblock
		 */
        CodeBlockExprMode endScannerScopeLevel(LocationInfo *loc);

		/**
		 * set scanner scope level of previous end-of-statement token to the current scanner scope level
		 */
		inline void setPrvEOSScopeLevel()							{ _prvEOSScopeLevel = _scannerScopeLevel.size(); }


		/**
		 * set mode for potential code block at statement start
		 */
		inline void setCbExprModeStatementStart()					{ _cbExprModeCur = codeBlockExprModeStatementStart; /*PROTO_OUTL("set codeBlockExprModeStatementStart");*/ }

		/**
		 * set mode for potential code block at code block body
		 */
		inline void setCbExprModeCBBodyStart()						{ _cbExprModeCur = codeBlockExprModeCBBodyStart; /*PROTO_OUTL("set codeBlockExprModeCBBodyStart");*/ }

		/**
		 * reset mode
		 */
		inline void setCbExprModeReset()							{ _cbExprModeCur = _cbExprModePrvSymbol = codeBlockExprModeElse; /*PROTO_OUTL("set codeBlockExprModeElse");*/ }

		/**
		 * reset mode, but store previous mode
		 */
		inline void setCbExprModeResetStorePrv()					{ _cbExprModePrvSymbol = _cbExprModeCur; _cbExprModeCur = codeBlockExprModeElse; /*PROTO_OUTL("reset codeBlockExprMode");*/ }


		/**
		 * output warning about inserting end-of-statement token after codeblock
		 */
		void warnCBInsertEOS(LocationInfo *loc);



		/************** generating of intermediary code **********/
	private:
		IntCode::IcElem *_code;								///< array with intermediary code
		unsigned _codeCnt;									///< count of elements in _code
		unsigned _codeCap;									///< capacity of array _code

		
		/**
		 * grow array for intermediary code
		 * @param minCap	grow array at least to this capacity
		 */
		void growCodeArray(unsigned minCap);

		/**
		 * ensure that array _code has free elements
		 * @param cnt		count of needed free elements
		 */
		inline void ensureCodeArray(unsigned cnt = 1)				{ if (!_code || _codeCap < _codeCnt + cnt) growCodeArray(_codeCnt + cnt); }


	public:
		/**
		 * get current address for next code element
		 */
		inline unsigned curCodeAddr()								{ return _codeCnt; }

        /**
         * append command element to intermediary code
         * @param se		syntax element
         * @param major     major number of command
         * @param minor     minor number of command
         * @param par       parameter for command
         * @param cnt       count of following arguments
         * @return			address of code element
         */
        unsigned compCmd(unsigned se, unsigned short major, unsigned short minor, unsigned short par, unsigned short cnt = 0);

		/**
		 * append command element to intermediary code
		 * @param elem		syntax element
		 * @param major     major number of command
		 * @param minor     minor number of command
		 * @param par       parameter for command
		 * @param cnt       count of following arguments
		 * @return			address of code element
		 */
        inline unsigned compCmd(SyntaxElement *elem, unsigned short major, unsigned short minor, unsigned short par, unsigned short cnt = 0)   { return compCmd((elem ? elem->id() : 0), major, minor, par, cnt); }

		/**
		 * append integer argument to intermediary code
         * @param se		syntax element
		 * @param v         argument value
		 * @return			address of code element
		 */
        unsigned compArg(unsigned se, intType v);

        /**
         * append real argument to intermediary code
         * @param elem		syntax element
         * @param v         argument value
         * @return			address of code element
         */
        inline unsigned compArg(SyntaxElement *elem, intType v)                                 { return compArg((elem ? elem->id() : 0), v); }

		/**
		 * append real argument to intermediary code
         * @param se		syntax element
		 * @param v         argument value
		 * @return			address of code element
		 */
        unsigned compArg(unsigned se, realType v);

        /**
         * append real argument to intermediary code
         * @param elem		syntax element
         * @param v         argument value
         * @return			address of code element
         */
        inline unsigned compArg(SyntaxElement *elem, realType v)                                { return compArg((elem ? elem->id() : 0), v); }

        /**
         * append up to two symbol numbers or similar as argument to intermediary code
         * @param se		syntax element
         * @param n1		first argument value
         * @param n2		second argument value
         * @return			address of code element
         */
        unsigned compArg(unsigned se, unsigned int n1, unsigned int n2);

		/**
		 * append up to two symbol numbers or similar as argument to intermediary code
		 * @param elem		syntax element
		 * @param n1        first argument value
		 * @param n2        second argument value
		 * @return			address of code element
		 */
        inline unsigned compArg(SyntaxElement *elem, unsigned int n1, unsigned int n2)          { return compArg((elem ? elem->id() : 0), n1, n2); }

        /**
         * compile NOP command
         * @param se		syntax element
         * @param argCnt	argument count for command
         * @return			code address of the NOP command
         */
        unsigned compNop(unsigned se, unsigned short argCnt);

		/**
		 * compile NOP command to an address (effectively delete the command on the address)
		 * @param elem		syntax element / NULL: syntax element remains unchanged on the code address
		 * @param addr		address to compile to
		 * @param argCnt	argument count for command
		 * @return			address of code element after the NOP command
		 */
		unsigned compNop(SyntaxElement *elem, unsigned addr, unsigned short argCnt);

		/**
		 * compile NOP command to an address to replace another command 
		 * @param elem		syntax element / NULL: syntax element remains unchanged on the code address
		 * @param addr		address to compile to
		 * @param major1	check for this original command / 0: no check
		 * @param major2	alternatively check for this original command / 0: no alternative check
		 * @param minor		check for this original minor command / 0: no check
		 * @return			address of code element after the NOP command
		 */
		unsigned compNop(SyntaxElement *elem, unsigned addr, unsigned short major1, unsigned short major2, unsigned short minor);

		/**
		 * compile literal value to intermediary code
		 * @param elem		syntax element
		 * @param s         literal value as string
		 * @param minor     type of literal value
		 * @return			address of code element
		 */
		unsigned compLitVal(SyntaxElement *elem, const char *s, unsigned short minor);

		/**
		 * compile fetch of symbol value to intermediary code
		 * @param elem		syntax element
		 * @param exp		expression info with the symbol
		 * @return			address of code element
		 */
		unsigned compFetchSym(SyntaxElement *elem, ExprParseInfo *exp);

		/**
		 * expand previous fetch command by prefix/postfix increment or decrement
		 * @param elem		syntax element
		 * @param pre		prefix or postfix
		 * @param val		value to increment (1) or to decrement (-1)
		 */
		unsigned compIncDecFetch(SyntaxElement *elem, bool pre, int val);

		/**
		 * change compiled fetch to lvalue-fetch
		 * @param addr		code address of the fetch command
		 * @param symnr		change symbol number to this number / 0: don't change symbol number
	 	 * @param symSecNr	number for global symbol or index for local symbol or effective symbol number for private new subsymbol
		 * @param local		if true then change fetch to local-fetch
         * @param cbhDef    if true then change fetch to definition of code block symbol within code block header
		 */
        void compChangeFetchLvalue(unsigned addr, unsigned symnr, unsigned symSecNr, bool local, bool cbhDef);

        /**
         * change compiled subsymbol lvalue-fetch
         * @param addr		code address of the fetch command
         * @param argAddr   code address of additional argument with validity range for the subsymbol / 0: compile new such additional argument if needed
         * @param mNew      modificator "new" is given in the assignment
         * @param mPublic	modificator "public" is given in the assignment
         * @param mPrivate	modificator "private" is given in the assignment
         * @return          code address of additional argument with validity range for the subsymbol
         */
        unsigned compChangeFetchSubLval(unsigned addr, unsigned argAddr, bool mNew, bool mPublic, bool mPrivate);

        /**
         * change compiled fetch to namespace fetch
         * @param addr		code address of the fetch command
         */
        void compChangeFetchNmspace(unsigned addr);

        /**
         * compile argument for validity range for subsymbols
         * @param vp        vector of argument positions
         * @param addr      address to write as argument
         */
        void compSetArgSubValid(vector<unsigned> *vp, unsigned addr);

        /**
         * replace fetch command by a literal short int value
         * @param addr      code address of the fetch command
         * @param val       literal short int value
         */
        void compReplaceFetchByLit(unsigned addr, unsigned short val);

		/**
		 * compile assign statement
		 * @param elem		syntax element
		 * @param expLhs	left hand side of assignment / NULL: no left hand side
		 * @param expRhs	right hand side of assignment / NULL: no right hand side
		 * @return			address of code element
		 */
		unsigned compAssign(SyntaxElement *elem, ExprParseInfo *expLhs, ExprParseInfo *expRhs);

		/**
		 * compile codeblock control command
		 * @param elem		syntax element
		 * @param cb		codeblock control (only modificatorBreak, modificatorContinue, modificatorRepeat or modificatorReturn)
		 * @param expLhs	left hand side of assignment / NULL: no left hand side
		 * @param hasRhs	assignment has right hand side
		 * @return			address of code element
		 */
		unsigned compControlCB(SyntaxElement *elem, ModificatorKey cb, ExprParseInfo *expLhs, bool hasRhs);

        /**
         * compile codeblock header start
         * @param elem		syntax element
         * @return			address of code element
         */
        unsigned compCBHeaderStart(SyntaxElement *elem);

		/**
         * compile codeblock header end
		 * @param elem		syntax element
		 * @param exp		codeblock header expression
         * @param addrStart code address of the start command of the codeblock header
		 * @return			address of code element
		 */
        unsigned compCBHeaderEnd(SyntaxElement *elem, ExprParseInfo *exp, unsigned addrStart);

        /**
         * insert parameter and header end address for code block header start command
         * @param addr      address of header start command
         * @param par       parameter value to insert
         * @param addrEnd   address of header end command
         * @param elem      syntax element
         */
        void compCBHeaderInsParAddr(unsigned addr, unsigned short par, unsigned addrEnd, SyntaxElement *elem);

		/**
		 * compile operation or construction statement
		 * @param elem		syntax element
		 * @param construct	construction statement
		 * @param op		minor command code of operation or construction
		 * @param cnt		count of source values for the operation
		 * @return			address of code element
		 */
		unsigned compOperation(SyntaxElement *elem, bool construct, unsigned short op, unsigned int cnt);

        /**
         * set flag for continued compare operation
         * @param addr      code address of the compare command
         */
        void compSetCompareCont(unsigned addr);

        /**
         * change mode for previous array construction
         * @param elem		syntax element
         * @param arrind    true: change to construct with adding new index part / false: change to construct without adding new index part
         * @return			address of code element
         */
        unsigned compArrayConstructMode(SyntaxElement *elem, bool arrind);

		/**
		 * compile start or end of function definition, also for global start and end
		 * @param elem		syntax element
		 * @param func		true: function / false: global scope
		 * @param start		true: start / false: end
		 * @param addrStart	address of store or start command (only used if !start)
		 * @param cntLocal	count of local symbols within the function (only used if !start && func)
		 * @param useSubs	function uses subsymbols (only used if !start && func)
		 * @return			address of code element
		 */
		unsigned compFunction(SyntaxElement *elem, bool func, bool start, unsigned addrStart, unsigned short cntLocal, bool useSubs);

		/**
		 * compile start or end of a code block or a code block part
		 * @param elem		syntax element
		 * @param minor     minor number of command
		 * @param lsc		count of local symbols before leaving code block part (only for minor==ICS_CB_PART_END)
		 * @return			address of code element
		 */
		unsigned compBlock(SyntaxElement *elem, unsigned short minor, unsigned lsc = 0);

		/**
		 * compile argument address to vector of code block commands
		 * @param vp		vector of code block command positions
		 * @param addr		address to write as argument
		 * @param n1		true: compile to v.n.n1 / false: compile to v.n.n2
		 * @param elem		syntax element
         * @param minor     only compile address to code block commands with this minor command code / 0: no such condition
         */
        void compBlockArgAddress(vector<unsigned> *vp, unsigned addr, bool n1, SyntaxElement *elem = NULL, unsigned short minor = 0);

        /**
         * insert parameter for code block command, and optional address to first argument in n2
         * @param vp		vector of code block command positions
         * @param offset	position in vp (if negative from end)
         * @param par		parameter value to insert
         * @param addr      address to write in n2 of first argument value / 0: no address to write
         * @param elem		syntax element
         */
        void compBlockInsParArg(vector<unsigned> *vp, int offset, unsigned short par, unsigned addr = 0, SyntaxElement *elem = NULL);

		/**
		 * compile extending or reseting of line name prefix
		 * @param elem		syntax element
		 * @param lnp		extend line name prefix by that / NULL: reset line name prefix
		 * @return			address of code element
		 */
		unsigned compLineNamePref(SyntaxElement *elem, const char *lnp);

		/**
		 * compile line name prefix compatibility
		 * @param elem		syntax element
		 * @param expSym	expression containing an invalid symbol to use as line name prefix
		 * @return			address of code element
		 */
		unsigned compLineNamePrefCompat(SyntaxElement *elem, ExprParseInfo *expSym);

		/**
		 * set for a compiled command and its args the syntax element
		 * @param addr		address of the compiled command
		 * @param elem		syntax element
		 */
		void compSetCmdSyntaxElem(unsigned addr, SyntaxElement *elem);
	};
}


#define ERRHANDLERCOMP		(modpt())
#define ERRHANDLERCOMPC		(comp->modpt())


#define INS_PREDEF_SYM(n,sb,ro,sp,pv)      { insertPredefSymbol(n,sb,ro,sp,pv,0); }


// function prototype for the scanner
#define YY_DECL				\
	cmpl::CMPL_Parser::token_type	\
		cmpllex (cmpl::CMPL_Parser::semantic_type* yylval_param,	\
				cmpl::CMPL_Parser::location_type* yylloc_param,		\
				void* yyscanner, cmpl::CompilerContext& PARSE_CONTEXT)
YY_DECL;


#endif // COMPILERCONTEXT_HH


