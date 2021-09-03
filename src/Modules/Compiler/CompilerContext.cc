
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


#include "CompilerContext.hh"
#include "Compiler.hh"
#include "../../Control/MainControl.hh"
#include "../../CommonData/ExternDataInfo.hh"
#include "../../CommonData/IntCode.hh"
#include "../../CommonData/SyntaxElements.hh"


namespace cmpl
{
	/**
	 * constructor
	 */
	CompilerContext::CompilerContext(Compiler *mod)
	{
		_modp = mod;

		_syntaxStructure = NULL;
		_symInfo = new map<unsigned, SymbolInfo *>();
		_curIdSyntaxElem = 0;

		_symbols = new SymbolTab(_modp, false);
		_subSymbols = new SymbolTab(_modp, true);

		_predefLoc = LocationInfo(PositionInfo(POSITION_TYPE_DESCR, "pre-defined symbol"));

		_modAssignCb.setForCb();

		_code = NULL;
		_codeCnt = _codeCap = 0;
	}

	/**
	 * destructor
	 */
	CompilerContext::~CompilerContext()
	{
		for (size_t i = _parseScopes.size(); i > 0; i--)
			delete _parseScopes[i-1];

		delete _symbols;
		delete _subSymbols;

		if (_code)
			delete _code;
	}


	/**
	 * main function, executes the parser and generate the intermediary code
	 */
	void CompilerContext::parse()
	{
		// init the first scope
		ParseScope *scope = new ParseScope(this, 0, parseScopeTypeBasis, codeBlockExprModeElse, NULL);
		_parseScopes.push_back(scope);

		_scanPrevTokenType = scanPrevTokenStart;
		_cbExprModeCur = _cbExprModePrvSymbol = codeBlockExprModeStatementStart;
		_prvEOSScopeLevel = 0;

		_hasFirstPosition = false;
		_prvErrorMsg.clear();
		_errElems.clear();
		_parserError = false;

		// compile start
		if (!checkOnly())
			compFunction(NULL, false, true, 0, 0, false);

		// init predefined symbols
		initPredefSymbols();

		// starts the scanner
		scanBegin();

		// parse cmpl file
		CMPL_Parser parser(*this);
		if (_modp->traceParsing())
			parser.set_debug_level(1);
		parser.parse();

		// stops the scanner
		scanEnd();
        _scanner = NULL;

        if (!checkOnly() && scope->_subValidPos)
            compSetArgSubValid(scope->_subValidPos, curCodeAddr());

		// if parsing ends while error recovery, use found syntactic elements
		if (!_syntaxStructure) {
			LocationInfo loc(_firstPosition);
			SyntaxElement *s = errorRecoveryElem(&loc);
			setSyntaxStructure(s, &loc);
		}

		// store info about syntax structure of cmpl input
		SET_NEW_DATA(SyntaxElements, se);
		se->setRoot(_syntaxStructure, _symInfo, _modp->data());

		// store generated intermediary code
		if (!checkOnly()) {
			SET_NEW_DATA(IntCode, code);
			code->setCode(_codeCnt, _code);

			_code = NULL;
			_codeCnt = _codeCap = 0;
		}

		_syntaxStructure = NULL;
		_symInfo = NULL;
	}


	/**
	 * set root element of syntax structure of cmpl input
	 * @param r					root element
	 * @param loc				location of root element
	 */
	void CompilerContext::setSyntaxStructure(SyntaxElement *r, LocationInfo *loc)
	{
		if (_syntaxStructure)
			delete _syntaxStructure;

		// set first position
		if (_hasFirstPosition)
			loc->setFrom(_firstPosition, loc->end());

		_syntaxStructure = SyntaxElement::newEntry(nextIdSyntaxElem(), SyntaxElementCode, loc, loc);
		_syntaxStructure->appendChild(r);
	}


	/**
	 * output parser error message
	 * @param lvl				error level
	 * @param m					error message
	 * @param l					location of error
	 * @param pe				set _parserError to true
	 */
	void CompilerContext::outParseError(int lvl, const string& m, const LocationInfo& l, bool pe)
	{
		LocationInfo loc = l;

		// replace all quotation marks ` by ", but ``` by "`"
		_prvErrorMsg = m;
		size_t p1, px = _prvErrorMsg.find("```");

		while ((p1 = _prvErrorMsg.find('`')) != string::npos)
		_prvErrorMsg.replace(p1, 1, 1, '"');

		if (px != string::npos)
		_prvErrorMsg.replace(px+1, 1, 1, '`');

		ERRHANDLERCOMP->error(ERROR_LVL_NORMAL, _prvErrorMsg.c_str(), loc);

		if (pe)
			_parserError = true;
	}


	/**
	 * get syntactic element of error recovery
	 * @param loc				location of error
	 */
	SyntaxElement* CompilerContext::errorRecoveryElem(LocationInfo *loc)
	{
		SyntaxElement *s = SyntaxElement::newEntry(nextIdSyntaxElem(), SyntaxElementError, prvParseErrorCStr(), loc, loc);

		for (vector<SyntaxElement *>::iterator it = _errElems.begin(); it != _errElems.end(); it++)
			s->appendChild(*it);
	
		_errElems.clear();

		return s;
	}



	/************** location handling **********/

	/*
	 * set InLoc in position
	 * @param curLoc			current location
	 * @param pos               position to set inLoc within
	 * @param inLoc             inLoc number as parsed from position mark
	 */
	void CompilerContext::setInLocPM(LocationInfo *curLoc, PositionInfo& pos, int inLoc)
	{
		map<int, LocationInfo*>::iterator it = _defLoc.find(inLoc);
		if (it == _defLoc.end())
			ERRHANDLERCOMP->error(ERROR_LVL_NORMAL, ctrl()->printBuffer("use of undefined DefLoc number %d", inLoc), *curLoc);

		pos.setInLoc(it->second);
	}

	/**
	 * parse DefLoc position mark
	 * @param curLoc			current location
	 * @param txt               text with DefLoc info
	 */
	void CompilerContext::parseDefLocPM(LocationInfo *curLoc, const char *txt)
	{
		PositionInfo pos = curLoc->begin();
		pos.addCols(7);

		LocationInfo loc;
		int defNr;
		int inLoc = pos.parseDefLocPM(ctrl(), txt, data()->globStrings(), defNr, loc);

		if (inLoc) {
			PositionInfo posB = loc.begin();
			setInLocPM(curLoc, posB, inLoc);

			PositionInfo posE = posB;
			posE.setLine(loc.end().line());
			posE.setCol(loc.end().col());

			loc = LocationInfo(posB, posE);
		}

		_defLoc[defNr] = new LocationInfo(loc);
	}

	/**
	 * parse StartPos position mark
	 * @param curLoc			current location
	 * @param txt               text with StartPos info
	 */
	void CompilerContext::parseStartPosPM(LocationInfo *curLoc, const char *txt)
	{
		PositionInfo pos = curLoc->begin();
		pos.addCols(9);

		int inLoc = pos.parseStartPosPM(ctrl(), txt, data()->globStrings());
		if (inLoc)
			setInLocPM(curLoc, pos, inLoc);

		curLoc->setFrom(pos, pos);

		if (!_hasFirstPosition) {
			_firstPosition = pos;
			_hasFirstPosition = true;
		}
	}

	/**
	 * parse Line position mark
	 * @param curLoc			current location
	 * @param txt               text with line number
	 */
	void CompilerContext::parseLinePM(LocationInfo *curLoc, const char *txt)
	{
		PositionInfo pos = curLoc->begin();
		pos.addCols(5);
		pos.parseLinePM(ctrl(), txt);
		curLoc->setFrom(pos, pos);
	}

	/**
	 * parse Col position mark
	 * @param curLoc			current location
	 * @param txt               text with col number
	 */
	void CompilerContext::parseColPM(LocationInfo *curLoc, const char *txt)
	{
		PositionInfo pos = curLoc->begin();
		pos.addCols(4);
		pos.parseColPM(ctrl(), txt);
		curLoc->setFrom(pos, pos);
	}

	/**
	 * output error message about wrong position mark
	 * @param curLoc			current location
	 * @param txt				text with error
	 */
	void CompilerContext::outErrorPositionMark(LocationInfo *curLoc, const char *txt)
	{
		ERRHANDLERCOMP->error(ERROR_LVL_NORMAL, ctrl()->printBuffer("illegal character in position mark '%s'", txt), *curLoc);
	}



	/************** symbol management **********/

	/**
	 * initialize predefined symbols
	 */
	void CompilerContext::initPredefSymbols()
	{
		_symbolDefCnt = 1;
		_symbolGlobCnt = 1;

		#include "PredefSymbols.hh"

		// run all extensions which define additional predefined symbols
		ctrl()->runExtension(modp(), EXT_STEP_COMPILE_PREDEF_SYMBOLS, this);
	}


	/**
	 * insert one predefined symbol
	 * @param name      symbol name
	 * @param sub		symbol or subsymbol
	 * @param ro        read-only symbol
	 * @param special   special meaning for the symbol
	 * @param predef	predefined symbol value (0: no predefined symbol; >0: pseudo symbol (one of ICS_SPECIAL_*); <0: predefined symbol (one of PREDEFSYMVAL_*) (if >1000 or <-1000 then predefined by an extension)
     * @param predefCont only for predefined subsymbol: predef of container the subsymbol belongs to
	 * @return          symbol info
	 */
    SymbolDef * CompilerContext::insertPredefSymbol(const char *name, bool sub, bool ro, SpecialSymbol special, int predef, int predefCont)
	{
		bool found;
		SymbolDef *sym = (sub ? _subSymbols : _symbols)->searchInsert(name, false, _symbolDefCnt, found);

        if (found && !sub && (special != specialSymbolNamespace || sym->special() != specialSymbolNamespace))
			ERRHANDLERCOMP->internalError(ctrl()->printBuffer("predefined symbol '%s' already exists", name));

        if (!found)
            sym->setPublicReadOnlySpecial(true, ro, special, (sub ? 0u : newSymbolGlobCnt()));

        sym->insertSymbolInfo(this, _symbolDefCnt, _predefLoc, predef, predefCont);
		_symbolDefCnt++;

		return sym;
	}

	/**
	 * insert one symbol defined by an extension
	 * @param name      symbol name
	 * @param sub		symbol or subsymbol
	 * @param ro        read-only symbol
	 * @param special   special meaning for the symbol
	 * @param extId		identification number of the extension registration
     * @param extCont   only for predefined subsymbol: extId of container the subsymbol belongs to
	 */
    void CompilerContext::insertExtensionSymbol(const char *name, bool sub, bool ro, SpecialSymbol special, int extId, int extCont)
	{
		bool pseudo = (name[0] == '$');
		int predef = (pseudo ? (EXTENSION_SYMBOL_BASE + extId) : (-(EXTENSION_SYMBOL_BASE + extId)));
        insertPredefSymbol(name, sub, ro, special, predef, (sub && extCont ? (-(EXTENSION_SYMBOL_BASE + extCont)) : 0u));
	}

	/**
	 * search a symbol, and insert a new one if not found
	 * @param name      symbol name
	 * @param loc		location of the symbol
	 * @param sub       symbol or subsymbol
	 * @param ins		insert if not found
	 * @param nw        return whether the symbol is new inserted
	 * @return          symbol info
	 */
	SymbolDef * CompilerContext::searchInsertSymbol(const char *name, LocationInfo& loc, bool sub, bool ins, bool &nw)
	{
        bool pseudoSym = (*name == '$');

		bool found;
        SymbolDef *sym = (sub ? _subSymbols : _symbols)->searchInsert(name, false, (ins && !pseudoSym ? _symbolDefCnt : 0), found);

		nw = !found && ins;
		if (nw) {
            if (pseudoSym) {
                // cannot define new pseudosymbols
                ERRHANDLERCOMP->error(ERROR_LVL_NORMAL, ctrl()->printBuffer("unknown pseudo symbol '%s'", name), loc);
                nw = false;
                sym = (sub ? _subSymbols : _symbols)->first();
            }
            else {
                sym->insertSymbolInfo(this, sym->defId(), loc, 0, 0);
                _symbolDefCnt++;

                // all subsymbols are treated here as public, set it as valid now
                if (sub && !sym->pseudoSymbol())
                    sym->declSymbol(this, loc, symbolScopeTypePublic, 0, false);
            }
		}

		return (found || ins ? sym->innerst() : NULL);
	}


	/**
     * check if extern symbol exists
     * @param nms           namespace / NULL: no namespace
     * @param name          symbol name
	 * @param special		return of special meaning for the symbol
	 * @return				whether extern symbol exists
	 */
    bool CompilerContext::checkExternSymbol(const char *nms, const char *name, SpecialSymbol& special)
	{
		GET_DATA(ExternDataInfo, extInfo);
		if (!extInfo)
			return false;
	
        int tp, sname, a1, a2;
        bool file, optval;
		long sp;

        bool found;
        if (nms) {
            string s(nms);
            s += '.';
            s += name;
            found = extInfo->getExternInfo(data(), s.c_str(), tp, sname, file, optval, a1, a2, sp);
        }
        else {
            found = extInfo->getExternInfo(data(), name, tp, sname, file, optval, a1, a2, sp);
        }

        if (!found)
			return false;

        sp = (optval ? specialSymbolNo : specialSymbolKnownVal);
        return true;
	}


	/**
	 * start a parse scope
	 * @param elem			syntax element
	 * @param loc			location, only for error handling
	 * @param tp			type of parse scope
	 * @param sptt			type of previous scanned token
	 */
	void CompilerContext::startParseScope(SyntaxElement *elem, LocationInfo& loc, ParseScopeType tp, ScanPrevTokenType sptt)
	{
		PROTO_OUTL("startParseScope: " << (int)tp << ", " << (int)sptt << ", " << (int)_cbExprModeCur);
		
		if (tp == parseScopeTypeBlock) {
			// modify parse scope type if codeblock is referenced
			if (sptt == scanPrevTokenCBRef)
				tp = parseScopeTypeFunc;
			else if (sptt == scanPrevTokenCBRefPure)
				tp = parseScopeTypePure;

			// store info for distinction between expression and statement for codeblocks
			CodeBlockExprMode cbm = _cbExprModeCur;
			if (_modp->compatLineNamePref() && sptt == scanPrevTokenUndefSymbol)
				cbm = _cbExprModePrvSymbol;

			_scannerScopeLevel.top() = curParseScopeLevel() + 1;

			// enter parse scope for codeblock and parse scope for first codeblock part
			enterParseScope(tp, cbm);
			compBlock(elem, ICS_CB_BLOCK_START);

			enterParseScope(parseScopeTypePart);
			compBlock(elem, ICS_CB_PART_START);
		}
		else if (tp == parseScopeTypePart) {
			// leave previous codeblock part and its body, and enter a new part
			unsigned lsc = curParseScope()->_outerFunc->_localSymCur;
			leaveParseScope(parseScopeTypeBody);

			compBlock(curParseScope()->_elem, ICS_CB_PART_END, lsc);
			leaveParseScope(parseScopeTypePart);

			if (curParseScope()->_cntBlockPart++ >= ICPAR_BLOCK_CNTPARTS - 1)
                ERRHANDLERCOMP->error(ERROR_LVL_NORMAL, "too many code block parts", loc);

			enterParseScope(parseScopeTypePart);
			compBlock(elem, ICS_CB_PART_START);
		}
		else if (tp == parseScopeTypeBody) {
			enterParseScope(parseScopeTypeBody);
			compBlock(elem, ICS_CB_PART_BODY);
		}
		else {
			ERRHANDLERCOMP->internalError("startParseScope: illegal parse scope type");
		}
	}

	/**
	 * ends a parse scope
	 * @param elem			syntax element
	 * @param loc			location, only for error handling
	 */
	void CompilerContext::endParseScope(SyntaxElement *elem, LocationInfo& loc)
	{
		PROTO_OUTL("endParseScope");
		
		// leave body of last body, last part, and codeblock
		unsigned lsc = curParseScope()->_outerFunc->_localSymCur;
		leaveParseScope(parseScopeTypeBody);

		compBlock(curParseScope()->_elem, ICS_CB_PART_END, lsc);
		leaveParseScope(parseScopeTypePart);

		compBlock(elem, ICS_CB_BLOCK_END);
		leaveParseScope(parseScopeTypeBlock); 
	}

	/**
	 * get info about current function parse scope
	 * @param useSubs		return whether function uses subsymbols
	 * @return				count of local symbols used in the function
	 */
	unsigned short CompilerContext::funcParseScopeInfo(bool& useSubs)
	{
		ParseScope *pp = curParseScope();
		if (!pp->isFuncOrBasisScope())
			pp = pp->_outerFunc;

		useSubs = pp->_hasSubSymbol;
		return (unsigned short) pp->_localSymMax;
	}

	/**
	 * set single parameter value for code block intermediary code command within parse scopes
	 * @param p				parameter to set (one of ICPAR_BLOCK_*)
	 * @param lvl			codeblock level count from the top parse scope
	 * @return				block parse scope
	 */
	ParseScope * CompilerContext::setParseScopeParBlockPart(unsigned short p, unsigned lvl)
	{
		ParseScope *cp = curParseScope();
		ParseScope *pp;

        if (p == ICPAR_BLOCK_NCSYM) {
            pp = cp;
            while (pp) {
                pp->setParBlockPart(p);
                pp = ((pp->_scopeType == parseScopeTypeBlock || pp->_scopeType == parseScopeTypePart || pp->_scopeType == parseScopeTypeBody) ? _parseScopes[pp->_level - 1] : NULL);
            }
        }
        else {
            if (lvl == 0) {
                cp->setParBlockPart(p);
                pp = cp->_blockScope;
                if (pp)
                    pp = _parseScopes[pp->_level + 1];
            }
            else {
                pp = cp;
                for (unsigned i = 0; i < lvl && pp; i++) {
                    pp = pp->_blockScope;
                    if (pp)
                        pp = pp->_outerPartScope;
                }
            }

            if (pp) {
                pp->setParBlockPart(p);
                if (pp->_blockScope)
                    pp->_blockScope->setParBlockPart(p);
            }
        }

		return (pp ? pp->_blockScope : NULL);
	}

	/**
	 * insert address of code block control command in parse scopes for code block and code block part
	 * @param pb			parse scope of code block
	 * @param addr			address of code block control command
	 */
	void CompilerContext::insertParseScopeControlCBPos(ParseScope *pb, unsigned addr)
	{
		for (int i = 0; i < 2; i++, pb = _parseScopes[pb->_level + 1]) {
			if (!pb->_controlCBPos)
				pb->_controlCBPos = new vector<unsigned>();

			pb->_controlCBPos->push_back(addr);
		}
	}

	/**
	 * check whether the current statement is a code block control command and compile it to the intermediary code
	 * @param elem			syntax element
	 * @param loc			location, only for error handling
	 * @param rhs			right hand side present
	 * @return				statement is a code block control command
	 */
	bool CompilerContext::testCompControlCB(SyntaxElement *elem, LocationInfo& loc, bool rhs)
	{
		AssignModificators& am = curParseScope()->_assignModCurrent;
		ModificatorKey cb;

		if (am.isControlCBAssign(this, &loc, cb)) {
			compControlCB(elem, cb, NULL, rhs);
			return true;
		}

		return false;
	}

	/**
	 * enter a new parse scope
	 * @param pst           parse scope type
	 * @param cbm			mode for code block for distinction between expression and statement
	 */
	void CompilerContext::enterParseScope(ParseScopeType pst, CodeBlockExprMode cbm)
	{
		ParseScope *pp = curParseScope();

		unsigned lvl = curParseScopeLevel() + 1;
		PROTO_OUTL("  enterParseScope " << lvl << ": " << (int)pst << ", " << (int)cbm);

		ParseScope *ps = new ParseScope(this, lvl, pst, cbm, pp);
		bool func = (pst == parseScopeTypeFunc || pst == parseScopeTypePure);

		ps->_inFuncDef = (func && pp->_inFuncDef != parseScopeTypePure ? pst : pp->_inFuncDef);
		if (!func)
			ps->_assignModDirective = pp->_assignModDirective;

		if (ps->isBlockScope() && pp->_blockScope)
			ps->_outerPartScope = (pp->_scopeType == parseScopeTypePart ? pp : secParseScope());

		_parseScopes.push_back(ps);

		_symbols->enterParseScope(lvl, pst);
	}

	/**
	 * set the symbol state before leaving a parse scope
	 * @param pst           parse scope type
	 */
	void CompilerContext::leaveParseScope(ParseScopeType pst)
	{
		PROTO_OUTL("  leaveParseScope " << curParseScopeLevel() << ": " << (int)pst);

		unsigned lvl = curParseScopeLevel();
		if (lvl == 0) {
			ERRHANDLERCOMP->internalError("leaveParseScope: parse scope stack is empty");
			return;
		}

		ParseScope *ps = _parseScopes.back();
        if ((ps->isBlockScope() ? parseScopeTypeBlock : ps->_scopeType) != pst) {
			if (!_parserError)
				ERRHANDLERCOMP->internalError("leaveParseScope: parse scope type doesn't match");

			// missing parse scope, probably caused by a previous parser error; skip
			return;
		}

		_symbols->leaveParseScope(lvl, pst);
        if (!checkOnly() && ps->_subValidPos)
            compSetArgSubValid(ps->_subValidPos, curCodeAddr());

		if (!ps->isFuncOrBasisScope())
			ps->_outerFunc->_localSymCur = ps->_localSymCur;

		delete ps;
		_parseScopes.pop_back();
	}



	/************** distinction between expression and statement for codeblocks **********/

	/**
	 * ends a scanner scope
	 * @return	codeblock mode effective for the ending codeblock
	 */
    CodeBlockExprMode CompilerContext::endScannerScopeLevel(LocationInfo *loc)
	{
		CodeBlockExprMode cbm = codeBlockExprModeElse;

		// get code block mode from parse scope
        int lvl;
        if (!_scannerScopeLevel.empty()) {
            lvl = _scannerScopeLevel.top();
        }
        else {
            ERRHANDLERCOMP->error(ERROR_LVL_NORMAL, "unexpected end of codeblock", *loc);
            return cbm;
        }

		if (lvl >= 0 && lvl < (int)curParseScopeLevel() && (_prvEOSScopeLevel == _scannerScopeLevel.size() || _scanPrevTokenType == scanPrevTokenColon)) {
			ParseScope *ps = _parseScopes[lvl];
			cbm = ps->_cbExprMode;

			PROTO_OUTL("  endScannerScopeLevel (" << lvl << ") -> " << (int)cbm << " :" << _prvEOSScopeLevel << " - " << _scannerScopeLevel.size() << ", " << (int)_scanPrevTokenType);
		}

		// leave scanner scope
		_scannerScopeLevel.pop();

		if (_prvEOSScopeLevel > _scannerScopeLevel.size())
			_prvEOSScopeLevel = _scannerScopeLevel.size();

		return cbm;
	}

	/**
	 * output warning about inserting end-of-statement token after codeblock
	 */
	void CompilerContext::warnCBInsertEOS(LocationInfo *loc)
	{
		if (_modp->warnCBInsertEOS())
			ERRHANDLERCOMP->error(ERROR_LVL_WARN, "end-of-statement token inserted after codeblock", *loc);
	}



	/************** generating of intermediary code **********/

	/**
	 * grow array for intermediary code
	 * @param minCap	grow array at least to this capacity
	 */
	void CompilerContext::growCodeArray(unsigned minCap)
	{
		try {
			if (!_code) {
				// start at least with 1000 elements
				_codeCap = (minCap > 1000u ? minCap : 1000u);
				PROTO_OUTL("allocate array for intermediary code with initial capacity: " << _codeCap);
				_code = new IntCode::IcElem[_codeCap];
			}
			else if (_codeCap < minCap) {
				unsigned grow = (minCap > 1000u ? minCap : 1000u);
				_codeCap += (grow >= (unsigned)(0.3 * _codeCap) ? grow : (unsigned)(0.3 * _codeCap)); 
				PROTO_OUTL("grow array for intermediary code to capacity: " << _codeCap);

				IntCode::IcElem *p = new IntCode::IcElem[_codeCap];
				memcpy(p, _code, sizeof(IntCode::IcElem) * _codeCnt);

				delete _code;
				_code = p;
			}
		} catch (exception& ex) {
			_modp->ctrl()->errHandler().internalError("error allocating memory for intermediary code", &ex);
		}
	}


    /**
	 * append command element to intermediary code
     * @param se		syntax element
	 * @param major		major number of command
	 * @param minor		minor number of command
	 * @param par		parameter for command
	 * @param cnt		count of following arguments
	 * @return			address of code element
	 */
    unsigned CompilerContext::compCmd(unsigned se, unsigned short major, unsigned short minor, unsigned short par, unsigned short cnt)
	{
		ensureCodeArray(cnt + 1);
		unsigned a = _codeCnt;
		IntCode::IcElem *p = _code + (_codeCnt++);

		p->tp = IntCode::icTypeCommand;
        p->se = se;
		p->v.c.major = major;
		p->v.c.minor = minor;
		p->v.c.par = par;
		p->v.c.cnt = cnt;

		PROTO_MOD_OUT_IC(_modp, a, p);
		return a;
	}

	/**
	 * append integer argument to intermediary code
     * @param se		syntax element
	 * @param v			argument value
	 * @return			address of code element
	 */
    unsigned CompilerContext::compArg(unsigned se, intType v)
	{
		ensureCodeArray();
		unsigned a = _codeCnt;
		IntCode::IcElem *p = _code + (_codeCnt++);

		p->tp = IntCode::icTypeInt;
        p->se = se;
		p->v.i = v;

		PROTO_MOD_OUT_IC(_modp, a, p);
		return a;
	}

	/**
	 * append real argument to intermediary code
     * @param se		syntax element
	 * @param v			argument value
	 * @return			address of code element
	 */
    unsigned CompilerContext::compArg(unsigned se, realType v)
	{
		ensureCodeArray();
		unsigned a = _codeCnt;
		IntCode::IcElem *p = _code + (_codeCnt++);

		p->tp = IntCode::icTypeReal;
        p->se = se;
		p->v.r = v;

		PROTO_MOD_OUT_IC(_modp, a, p);
		return a;
	}

	/**
	 * append up to two symbol numbers or similar as argument to intermediary code
     * @param se		syntax element
	 * @param n1		first argument value
	 * @param n2		second argument value
	 * @return			address of code element
	 */
    unsigned CompilerContext::compArg(unsigned se, unsigned int n1, unsigned int n2)
	{
		ensureCodeArray();
		unsigned a = _codeCnt;
		IntCode::IcElem *p = _code + (_codeCnt++);

		p->tp = IntCode::icTypeNo2;
        p->se = se;
		p->v.n.n1 = n1;
		p->v.n.n2 = n2;

		PROTO_MOD_OUT_IC(_modp, a, p);
		return a;
	}


    /**
	 * compile NOP command to an address (effectively delete the command on the address)
	 * @param elem		syntax element / NULL: syntax element remains unchanged on the code address
	 * @param addr		address to compile to
	 * @param argCnt	argument count for command
	 * @return			address of code element after the NOP command
	 */
	unsigned CompilerContext::compNop(SyntaxElement *elem, unsigned addr, unsigned short argCnt)
	{
		unsigned a = _codeCnt;

		_codeCnt = addr;
		IntCode::IcElem *p = _code + _codeCnt;
		unsigned elemId = (elem ? elem->id() : p->se);

        compCmd(elemId, INTCODE_NOP, 0, 0, argCnt);

		if (argCnt > 0) {
			unsigned short i;
			for (i = 0, p++; i < argCnt; i++, p++) {
                compArg(elemId, (intType)0);
			}
		}

		_codeCnt = a;
		return (addr + argCnt + 1);
	}

	/**
	 * compile NOP command to an address to replace another command 
	 * @param elem		syntax element / NULL: syntax element remains unchanged on the code address
	 * @param addr		address to compile to
	 * @param major1	check for this original command / 0: no check
	 * @param major2	alternatively check for this original command / 0: no alternative check
	 * @param minor		check for this original minor command / 0: no check
	 * @return			address of code element after the NOP command
	 */
	unsigned CompilerContext::compNop(SyntaxElement *elem, unsigned addr, unsigned short major1, unsigned short major2, unsigned short minor)
	{
		IntCode::IcElem *p = _code + addr;

		if (p->tp != IntCode::icTypeCommand || (major1 != 0 && p->v.c.major != major1 && (major2 == 0 || p->v.c.major != major2)) || (minor != 0 && p->v.c.minor != minor))
			_modp->ctrl()->errHandler().internalError("wrong code address in CompilerContext::compNop()");

		return compNop(elem, addr, p->v.c.cnt);
	}

	/**
	 * compile literal value to intermediary code
	 * @param elem		syntax element
	 * @param s			literal value as string
	 * @param minor		type of literal value
	 * @return			address of code element
	 */
	unsigned CompilerContext::compLitVal(SyntaxElement *elem, const char *s, unsigned short minor)
	{
		realType r;
		intType i;
		unsigned a = _codeCnt;

		if (minor == ICS_LIT_BLANK) {
			compCmd(elem, INTCODE_LIT_VAL, ICS_LIT_BLANK, 0, 0);
			return a;
		}

        if (minor == ICS_LIT_REAL || (minor == ICS_LIT_INT && _modp->intOutOfRangeMode() != INT_OPER_OORANGE_IGNORE)) {
			r = STR_TO_CMPLREAL(s);
            if (minor == ICS_LIT_INT && (r > (double)CMPLINT_MAX || r < (double)CMPLINT_MIN)) {
                if (_modp->intOutOfRangeMode() == INT_OPER_OORANGE_ERROR)
                    outParseError(ERROR_LVL_NORMAL, "literal integer value is out of range", elem->loc(), true);

				minor = ICS_LIT_REAL;
            }

			if (minor == ICS_LIT_REAL) {
				compCmd(elem, INTCODE_LIT_VAL, minor, 0, 1);
				compArg(elem, r);
				return a;
			}
		}

		if (minor == ICS_LIT_INT) {
			i = STR_TO_CMPLINT(s);
		}
		else {
			// strip quotes
            string str(s+1, strlen(s)-2);
            if (str.find("\\\"") != string::npos)
                str = StringStore::replaceAll(str, "\\\"", "\"");

            i = data()->globStrings()->storeInd(str);

            /*
			char *n = new char[strlen(s)];
			strncpy(n, s+1, strlen(s)-2);
			n[strlen(s)-2] = '\0';

			bool found;
            i = data()->globStrings()->searchInsert(str, false, true, found);
			delete n;
            */
        }

		bool sepArg = (i < 0 || i > USHRT_MAX);
		compCmd(elem, INTCODE_LIT_VAL, minor, (sepArg ? 0 : (unsigned short)i), (sepArg ? 1 : 0));
		if (sepArg)
			compArg(elem, i);

		return a;
	}

	/**
	 * compile fetch of symbol value to intermediary code
	 * @param elem		syntax element
	 * @param exp		expression info with the symbol
	 * @return			address of code element
	 */
	unsigned CompilerContext::compFetchSym(SyntaxElement *elem, ExprParseInfo *exp)
	{
		unsigned a = _codeCnt;
		SymbolDef *sym = exp->sym();

		if (sym->pseudoSymbol()) {
			compCmd(elem, INTCODE_FETCH_SPECIAL, sym->pseudoSymbolDef(), (sym->subsymbol() ? ICPAR_FETCH_SUBSYM : 0), 0);
		}
		else {
			compCmd(elem, INTCODE_FETCH, (sym->global() ? ICS_FETCH_GLOBAL : ICS_FETCH_LOCAL), (sym->subsymbol() ? ICPAR_FETCH_SUBSYM : 0), 1);
			compArg(elem, sym->defId(), (sym->subsymbol() ? 0u : sym->codeRefId()));

            if (sym->subsymbol())
                curParseScope()->setHasSubSymbol();
		}

		exp->setLvalueCodeAddr(a);
		return a;
	}

	/**
	 * expand previous fetch command by prefix/postfix increment or decrement
	 * @param elem		syntax element
	 * @param pre		prefix or postfix
	 * @param val		value to increment (1) or to decrement (-1)
	 */
	unsigned CompilerContext::compIncDecFetch(SyntaxElement *elem, bool pre, int val)
	{
		unsigned a = _codeCnt;

		IntCode::IcElem *p = _code + a - 2;
		if (p->tp != IntCode::icTypeCommand || (p->v.c.major != INTCODE_FETCH && p->v.c.major != INTCODE_FETCH_SPECIAL))
			_modp->ctrl()->errHandler().internalError("wrong code address in CompilerContext::compIncDecFetch()");

		p->v.c.cnt = 2;
        p->v.c.par |= ICPAR_FETCH_INCDEC;

		compArg(elem, (pre ? 1 : 0), (val > 0 ? 1 : 0));

		return a;
	}

	/**
	 * change compiled fetch to lvalue-fetch
	 * @param addr		code address of the fetch command
	 * @param symnr		change symbol number to this number / 0: don't change symbol number
	 * @param symSecNr	number for global symbol or index for local symbol or effective symbol number for private new subsymbol
	 * @param local		if true then change fetch to local-fetch
     * @param cbhDef    if true then change fetch to definition of code block symbol within code block header
     */
    void CompilerContext::compChangeFetchLvalue(unsigned addr, unsigned symnr, unsigned symSecNr, bool local, bool cbhDef)
	{
		if (!checkOnly()) {
			IntCode::IcElem *p = _code + addr;
			if (p->tp != IntCode::icTypeCommand || (p->v.c.major != INTCODE_FETCH && p->v.c.major != INTCODE_FETCH_SPECIAL))
				_modp->ctrl()->errHandler().internalError("wrong code address in CompilerContext::compChangeFetchLvalue()");

			p->v.c.par |= ICPAR_FETCH_LVALUE;
			PROTO_MOD_OUT_IC(_modp, addr, p);

			if (p->v.c.major == INTCODE_FETCH) {
                if (cbhDef)
                    p->v.c.minor = ICS_FETCH_DEF_CB;
                else if (local)
					p->v.c.minor = ICS_FETCH_LOCAL;

                if ((p->v.c.par & ICPAR_FETCH_SUBSYM) && addr >= 2)
                    compChangeFetchNmspace(addr - 2);

				if (symnr) {
					p++;
					p->v.n.n1 = symnr;
					p->v.n.n2 = symSecNr;
					PROTO_MOD_OUT_IC(_modp, addr+1, p);
				}
			}
			else if (p->v.c.major == INTCODE_FETCH_SPECIAL && p->v.c.minor == ICS_SPECIAL_CURDESTNAME) {
				setParseScopeParBlockPart(ICPAR_BLOCK_LNP);
			}
		}
	}

    /**
     * change compiled subsymbol lvalue-fetch
     * @param addr		code address of the fetch command
     * @param argAddr   code address of additional argument with validity range for the subsymbol / 0: compile new such additional argument if needed
     * @param mNew      modificator "new" is given in the assignment
     * @param mPublic	modificator "public" is given in the assignment
     * @param mPrivate	modificator "private" is given in the assignment
     * @return          code address of additional argument with validity range for the subsymbol
     */
    unsigned CompilerContext::compChangeFetchSubLval(unsigned addr, unsigned argAddr, bool mNew, bool mPublic, bool mPrivate)
    {
        if (!checkOnly()) {
            IntCode::IcElem *p = _code + addr;
            if (p->tp != IntCode::icTypeCommand || p->v.c.major != INTCODE_FETCH || p->v.c.par & ICPAR_FETCH_SUBSYM == 0)
                _modp->ctrl()->errHandler().internalError("wrong code address in CompilerContext::compChangeFetchSubLval()");

            if (mNew)
                p->v.c.par |= ICPAR_FETCH_LVAL_NEW;
            if (mPublic)
                p->v.c.par |= ICPAR_FETCH_LVAL_PUBLIC;
            if (mPrivate)
                p->v.c.par |= ICPAR_FETCH_LVAL_PRIVATE;

            PROTO_MOD_OUT_IC(_modp, addr, p);

            if (mPrivate || mNew) {
                if (!argAddr) {
                    if (addr + p->v.c.cnt + 1 == _codeCnt) {
                        // put additional argument in this fetch command
                        p->v.c.cnt++;
                    }
                    else {
                        // put additional argument in a new NOP command
                        compCmd(p->se, INTCODE_NOP, 0, 0, 1);
                    }

                    ParseScope *cp = curParseScope();
                    argAddr = compArg(p->se, (mNew ? addr : cp->_codeStart), 0);

                    if (mPrivate) {
                        if (!cp->_subValidPos)
                            cp->_subValidPos = new vector<unsigned>();

                        cp->_subValidPos->push_back(argAddr);
                    }
                }

                p++;
                p->v.n.n2 = argAddr;
                PROTO_MOD_OUT_IC(_modp, addr+1, p);
            }
        }

        return argAddr;
    }

    /**
     * change compiled fetch to namespace fetch
     * @param addr		code address of the fetch command
     */
    void CompilerContext::compChangeFetchNmspace(unsigned addr)
    {
        if (!checkOnly()) {
            IntCode::IcElem *p = _code + addr;

            if (p->tp == IntCode::icTypeCommand && p->v.c.major == INTCODE_FETCH) {
                p->v.c.par |= ICPAR_FETCH_NMSPACE;
                PROTO_MOD_OUT_IC(_modp, addr, p);

                if ((p->v.c.par & ICPAR_FETCH_SUBSYM) && addr >= 2)
                    compChangeFetchNmspace(addr - 2);
            }
        }
    }

    /**
     * compile argument for validity range for subsymbols
     * @param vp        vector of argument positions
     * @param addr      address to write as argument
     */
    void CompilerContext::compSetArgSubValid(vector<unsigned> *vp, unsigned addr)
    {
        if (vp) {
            vector<unsigned>::iterator it;
            for (it = vp->begin(); it != vp->end(); it++) {
                IntCode::IcElem *p = _code + *it;
                if (p->tp != IntCode::icTypeNo2 || p->v.n.n2 != 0)
                    _modp->ctrl()->errHandler().internalError("wrong code address in CompilerContext::compSetArgSubValid()");

                p->v.n.n2 = addr;
            }
        }
    }

    /**
     * replace fetch command by a literal short int value
     * @param addr      code address of the fetch command
     * @param val       literal short int value
     */
    void CompilerContext::compReplaceFetchByLit(unsigned addr, unsigned short val)
    {
        if (!checkOnly()) {
            IntCode::IcElem *p = _code + addr;
            if (p->tp != IntCode::icTypeCommand || (p->v.c.major != INTCODE_FETCH && p->v.c.major != INTCODE_FETCH_SPECIAL))
                _modp->ctrl()->errHandler().internalError("wrong code address in CompilerContext::compReplaceFetchByLit()");

            unsigned short cnt = p->v.c.cnt;

            p->v.c.major = INTCODE_LIT_VAL;
            p->v.c.minor = ICS_LIT_INT;
            p->v.c.par = val;
            p->v.c.cnt = 0;

            if (cnt)
                compNop(NULL, addr+1, cnt-1);
        }
    }

	/**
	 * compile assign statement
	 * @param elem		syntax element
	 * @param expLhs	left hand side of assignment / NULL: no left hand side
	 * @param expRhs	right hand side of assignment / NULL: no right hand side
	 * @return			address of code element
	 */
	unsigned CompilerContext::compAssign(SyntaxElement *elem, ExprParseInfo *expLhs, ExprParseInfo *expRhs)
	{
		unsigned a = _codeCnt;
		ExprParseInfo *e = (expLhs ?: expRhs);
		IntCode::IcElem *p;

		// codeblock control has own statements
		ModificatorKey cb;
		if (e->isControlCBAssign(cb)) {
			return compControlCB(elem, cb, expLhs, (expRhs ? true : false));
		}

		// declare doesn't generate code
		if (!expRhs && e->isDeclareAssign()) {
			expLhs->compChangeFetchNop(this);
			return a;
		}

		// check if expRhs may result to different values on different valuations
		bool volaRhs = false;
		unsigned addr;

		if (expRhs && expLhs) {
            if (expRhs->volaExp()) {
				volaRhs = true;
                addr = expLhs->codeAddrNext();
			}
			else {
				// search for a FETCH with a symbol number out of the lvalue symbols within the code of the right hand side expression
                set<unsigned> symnrs;
                addr = expLhs->codeAddrNextAndSymbols(symnrs);

                p = _code + addr;
				for (unsigned i = addr; i < _codeCnt-1; i++, p++) {
					if (p->tp == IntCode::icTypeCommand && p->v.c.major == INTCODE_FETCH) {
						p++;
						if (symnrs.count(p->v.n.n1) > 0) {
							volaRhs = true;
							break;
						}
					}
				}
			}
		}

		// get minor command code
		unsigned short minor;
		const char *op = e->assignOp();

		if (!expLhs)
			minor = ICS_ASSIGN_RHSONLY;
		else if (!expRhs)
			minor = (e->isExternAssign() ? ICS_ASSIGN_EXTERN : (e->isAssertAssign() ? ICS_ASSIGN_ASSERT : ICS_ASSIGN_LHSONLY));
		else if (e->isRefAssign())
			minor = ICS_ASSIGN_REF;
		else if (*op == ':')
			minor = ICS_ASSIGN_NORMAL;
		else
			minor = (*op == '+' ? ICS_ASSIGN_ADD : (*op == '-' ? ICS_ASSIGN_SUB : (*op == '*' ? ICS_ASSIGN_MUL : ICS_ASSIGN_DIV)));

		// get parameter
		unsigned short par = (unsigned short) (expLhs ? expLhs->countLvalues() : 0);
		if (e->isConstAssign())
			par += ICPAR_ASSIGN_CONST;
		if (e->isOrderedAssign())
			par += ICPAR_ASSIGN_ORDERED;
        if (e->isInitialAssign())
            par += ICPAR_ASSIGN_INITIAL;
        if (e->isNocondAssign())
            par += ICPAR_ASSIGN_NOCOND;
        if ((op && *op == ':' && op[1] == '\0') || !expRhs || !expLhs)
			par += ICPAR_ASSIGN_RESNAME;

		// object type and data type
		SymbolDef *objType = e->assignModObjType();
		SymbolDef *dataType = e->assignModDataType();

		if (objType && !objType->global())
			par += ICPAR_ASSIGN_OBJTYPE_LOCAL;
		if (dataType && !dataType->global())
			par += ICPAR_ASSIGN_DATATYPE_LOCAL;

		// compile assign statement
        unsigned short cnt = (objType || dataType || volaRhs ? (volaRhs ? 2 : 1) : 0);
		compCmd(elem, INTCODE_ASSIGN, minor, par, cnt);
		if (cnt) {
            compArg(elem, (objType ? objType->codeRefId() : 0u), (dataType ? dataType->codeRefId() : 0u));
            if (volaRhs)
                compArg(elem, addr, 0);
		}

        // store whether expressions access non-const symbols
        if ((expLhs && expLhs->ncSymAccess()) || (expRhs && expRhs->ncSymAccess()))
            setParseScopeParBlockPart(ICPAR_BLOCK_NCSYM);

		return a;
	}

	/**
	 * compile codeblock control command
	 * @param elem		syntax element
	 * @param cb		codeblock control (only modificatorBreak, modificatorContinue, modificatorRepeat or modificatorReturn)
	 * @param expLhs	left hand side of assignment / NULL: no left hand side
	 * @param hasRhs	assignment has right hand side
	 * @return			address of code element
	 */
	unsigned CompilerContext::compControlCB(SyntaxElement *elem, ModificatorKey cb, ExprParseInfo *expLhs, bool hasRhs)
	{
		unsigned a = _codeCnt;

		unsigned short lvl, par;
		if (expLhs) {
			lvl = expLhs->lvlControlCB();
            par = (lvl & ICPAR_CBC_CNTLVL) + ICPAR_CBC_HASLHS + (expLhs->indexLval() ? ICPAR_CBC_LHSTUPLE : 0) + (hasRhs ? ICPAR_CBC_RHSVALUE : 0);

			// remove lvalue fetch for the codeblock symbol, and remove indexation operation
			unsigned c = expLhs->getLvalueCodeAddr();
			compNop(NULL, c, INTCODE_FETCH, INTCODE_FETCH_SPECIAL, 0);

			if (expLhs->indexLval()) {
				c = expLhs->codeAddrNext() - 1;
				compNop(NULL, c, INTCODE_OPERATION, 0, ICS_OPER_INDEX);
			}
		}
		else if (cb == modificatorReturn) {
			ParseScope *pp = curParseScope();
			ParseScope *pf = pp->outerFunc();
			lvl = pp->_blockLevel - pf->_blockLevel;

            if (lvl > ICPAR_CBC_CNTLVL)
                ERRHANDLERCOMP->error(ERROR_LVL_NORMAL, "codeblock control command over to many nested code blocks", elem->loc());

			cb = modificatorBreak;
            par = (hasRhs ? ICPAR_CBC_RHSVALUE : 0) + (lvl & ICPAR_CBC_CNTLVL) + ICPAR_CBC_HASLHS;
		}
		else {
			lvl = 0;
            par = (hasRhs ? ICPAR_CBC_RHSVALUE : 0);
		}
		
        //for (unsigned i = 0; i < lvl; i++)
        //	setParseScopeParBlockPart(ICPAR_BLOCK_CTRL, i);

		setParseScopeParBlockPart(ICPAR_BLOCK_CTRL, lvl);
		if (cb == modificatorRepeat)
			setParseScopeParBlockPart(ICPAR_BLOCK_REPEAT, lvl);
		
		unsigned short minor = (cb == modificatorBreak ? ICS_CBC_BREAK : (cb == modificatorContinue ? ICS_CBC_CONTINUE : ICS_CBC_REPEAT));
		compCmd(elem, INTCODE_CB_CONTROL, minor, par, 1);
		compArg(elem, 0, 0);

		insertParseScopeControlCBPos(curParseScope()->_blockScope, a);

		return a;
	}

    /**
     * compile codeblock header start
     * @param elem		syntax element
     * @return			address of code element
     */
    unsigned CompilerContext::compCBHeaderStart(SyntaxElement *elem)
    {
        unsigned a = _codeCnt;

        compCmd(elem, INTCODE_CB_HEADER, ICS_CBHEADER_START, 0, 1);
        compArg(elem, 0, 0);

        return a;
    }

	/**
     * compile codeblock header end
	 * @param elem		syntax element
	 * @param exp		codeblock header expression
     * @param addrStart code address of the start command of the codeblock header
     * @return			address of code element
	 */
    unsigned CompilerContext::compCBHeaderEnd(SyntaxElement *elem, ExprParseInfo *exp, unsigned addrStart)
	{
		unsigned a = _codeCnt;

		ParseScope *pp = curParseScope();
        ParseScope *pb = pp->_blockScope;

        unsigned short par = ((pp->_cntBlockPart - 1) & ICPAR_CBHP_HCNT)
                + (exp->countLvalues() > 0 ? ICPAR_CBHP_CBS : 0)
                + (exp->assignOp() && *(exp->assignOp()) == 'i' ? ICPAR_CBHP_IN : 0)
                + (exp->ncSymAccess() ? ICPAR_CBHP_NCSYM : 0)
                + (exp->cbSymAccess() ? ICPAR_CBHP_PHCBS : 0);

        compCmd(elem, INTCODE_CB_HEADER, ICS_CBHEADER_END, par, 2);
		compArg(elem, 0, 0);

        vector<unsigned> *vp = pb->_blockPos;
        unsigned abcs = (vp && !vp->empty() ? vp->at(0) : 0);
        unsigned apcs = (vp && !vp->empty() ? vp->at(vp->size() - 1) : 0);
        if (_code[abcs].tp != IntCode::icTypeCommand || _code[abcs].v.c.major != INTCODE_CODEBLOCK || _code[abcs].v.c.minor != ICS_CB_BLOCK_START
                || _code[apcs].tp != IntCode::icTypeCommand || _code[apcs].v.c.major != INTCODE_CODEBLOCK || _code[apcs].v.c.minor != ICS_CB_PART_START)
            _modp->ctrl()->errHandler().internalError("wrong code address in CompilerContext::compCBHeaderEnd()");

        compArg(elem, abcs, apcs);

        compCBHeaderInsParAddr(addrStart, par, a, elem);

		if (!pb->_cbHeaderPos)
			pb->_cbHeaderPos = new vector<unsigned>();

		pb->_cbHeaderPos->push_back(a);

        if (exp->ncSymAccess() && pp->_cntBlockPart > 1)
            setParseScopeParBlockPart(ICPAR_BLOCK_NCSYM);

		return a;
	}

    /**
     * insert parameter and header end address for code block header start command
     * @param addr      address of header start command
     * @param par       parameter value to insert
     * @param addrEnd   address of header end command
     * @param elem      syntax element
     */
    void CompilerContext::compCBHeaderInsParAddr(unsigned addr, unsigned short par, unsigned addrEnd, SyntaxElement *elem)
    {
        IntCode::IcElem *p = _code + addr;
        if (p->tp != IntCode::icTypeCommand || p->v.c.major != INTCODE_CB_HEADER || p->v.c.minor != ICS_CBHEADER_START)
            _modp->ctrl()->errHandler().internalError("wrong code address in CompilerContext::compCBHeaderInsParAddr()");

        p->v.c.par = par;
        if (elem)
            p->se = elem->id();

        p++;
        p->v.n.n1 = addrEnd;
        if (elem)
            p->se = elem->id();
    }

	/**
	 * compile operation or construction statement
	 * @param elem		syntax element
	 * @param construct	construction statement
	 * @param op		minor command code of operation or construction
	 * @param cnt		count of source values for the operation
	 * @return			address of code element
	 */
	unsigned CompilerContext::compOperation(SyntaxElement *elem, bool construct, unsigned short op, unsigned int cnt)
	{
		unsigned a = _codeCnt;

		bool sepArg = (construct && cnt > USHRT_MAX);
		compCmd(elem, (construct ? INTCODE_CONSTRUCT : INTCODE_OPERATION), op, (sepArg ? 0 : (unsigned short)cnt), (sepArg ? 1 : 0));
		if (sepArg)
			compArg(elem, (intType)cnt);

		return a;
	}

    /**
     * set flag for continued compare operation
     * @param addr      code address of the compare command
     */
    void CompilerContext::compSetCompareCont(unsigned addr)
    {
        IntCode::IcElem *p = _code + addr;
        if (p->tp != IntCode::icTypeCommand || p->v.c.major != INTCODE_OPERATION)
            _modp->ctrl()->errHandler().internalError("wrong code address in CompilerContext::compSetCompareCont()");

        p->v.c.par |= ICPAR_OPER_CMP_FOLLOW;
        PROTO_MOD_OUT_IC(_modp, addr, p);
    }

    /**
     * change mode for previous array construction
     * @param elem		syntax element
     * @param arrind    true: change to construct with adding new index part / false: change to construct without adding new index part
     * @return			address of code element
     */
    unsigned CompilerContext::compArrayConstructMode(SyntaxElement *elem, bool arrind)
    {
        // get previous command
        unsigned a = _codeCnt;
        IntCode::IcElem *p = NULL;

        if (a > 0) {
            p = _code + a;
            for (a--, p--; a > 0 && p->tp != IntCode::icTypeCommand; a--, p--) ;
        }

        if (!p || p->v.c.major != INTCODE_CONSTRUCT || p->v.c.minor != ICS_CONSTRUCT_ARRAY) {
            if (arrind) {
                // compile new array construction
                return compOperation(elem, true, ICS_CONSTRUCT_ARRAY_IND, 1);
            }
        }
        else {
            p->v.c.minor = (arrind ? ICS_CONSTRUCT_ARRAY_IND : ICS_CONSTRUCT_ARRAY_WOI);
        }

        return a;
    }

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
	unsigned CompilerContext::compFunction(SyntaxElement *elem, bool func, bool start, unsigned addrStart, unsigned short cntLocal, bool useSubs)
	{
		unsigned a = _codeCnt;

		if (start) {
			if (func) {
				compCmd(elem, INTCODE_FUNCTION, ICS_FUNC_STORE, 0, 1);
				compArg(elem, a + 2, 0);
			}

			compCmd(elem, INTCODE_FUNCTION, ICS_FUNC_START, 0, 0);
		}
		else {
			compCmd((func ? elem : _syntaxStructure), INTCODE_FUNCTION, ICS_FUNC_END, 0, 0);

			IntCode::IcElem *p = _code + (func ? addrStart : 0);
			if (p->tp != IntCode::icTypeCommand || p->v.c.major != INTCODE_FUNCTION || p->v.c.minor != (func ? ICS_FUNC_STORE : ICS_FUNC_START))
				_modp->ctrl()->errHandler().internalError("wrong code address in CompilerContext::compFunction()");

			if (func) {
                p->se = elem->id();
                p->v.c.par = (useSubs ? ICPAR_FUNC_WITH_SUBS : 0);
				p++;

                p->se = elem->id();
                p->v.n.n2 = a + 1;
                p++;

				if (p->tp != IntCode::icTypeCommand || p->v.c.major != INTCODE_FUNCTION || p->v.c.minor != ICS_FUNC_START)
					_modp->ctrl()->errHandler().internalError("wrong code address in CompilerContext::compFunction()");
                p->se = elem->id();
                p->v.c.par = cntLocal;
            }
			else {
				p->se = _syntaxStructure->id();
				p->v.c.par = _parseScopes[0]->_localSymMax;
			}
		}

		return a;
	}

	/**
	 * compile start or end of a code block or a code block part
	 * @param elem		syntax element
	 * @param minor     minor number of command
	 * @param lsc		count of local symbols before leaving code block part (only for minor==ICS_CB_PART_END)
	 * @return			address of code element
	 */
	unsigned CompilerContext::compBlock(SyntaxElement *elem, unsigned short minor, unsigned lsc)
	{
		if (checkOnly())
			return 0;

		unsigned a = _codeCnt;

		// store position "a" in vector _blockPos of the block scope
		ParseScope *cp = curParseScope();
		ParseScope *bp = cp->_blockScope;
		if (!bp->_blockPos) {
			bp->_blockPos = new vector<unsigned>();
			bp->_blockPos->reserve(5);
		}
		bp->_blockPos->push_back(a);

		bool isBlock = (minor == ICS_CB_BLOCK_START || minor == ICS_CB_BLOCK_END);
		unsigned short par = ((unsigned short) ((bp->_cntBlockPart + (isBlock ? 1 : 0)) & ICPAR_BLOCK_CNTPARTS)) | bp->_parBlockPart;

		compCmd(elem, INTCODE_CODEBLOCK, minor, par, (minor == ICS_CB_PART_END ? 2 : 1));
		compArg(elem, 0, 0);

		if ((minor == ICS_CB_PART_START || minor == ICS_CB_BLOCK_END) && bp->_cbHeaderPos) {
			// write address of ICS_CB_BLOCK_END to all code block headers of the previous part
			compBlockArgAddress(bp->_cbHeaderPos, a, true);
			compBlockArgAddress(bp->_cbHeaderPos, (minor == ICS_CB_BLOCK_END ? 1 : 0), false);
			delete bp->_cbHeaderPos;
			bp->_cbHeaderPos = NULL;
		}

		if (minor == ICS_CB_PART_END) {
			compArg(elem, lsc, cp->_localSymCur);

            // store parameters and address of this ICS_CB_PART_END also in ICS_CB_PART_START and ICS_CB_PART_BODY
            compBlockInsParArg(bp->_blockPos, -2, par, a, elem);
            compBlockInsParArg(bp->_blockPos, -3, par, a, elem);

            // write address of ICS_CB_PART_END to direct inner code block end commands
            compBlockArgAddress(cp->_blockPos, a, false, NULL, ICS_CB_BLOCK_END);

			// write address of ICS_CB_PART_END to code block control commands within this code block part
			compBlockArgAddress(cp->_controlCBPos, a, true);
		}
		else if (minor == ICS_CB_BLOCK_END) {
			// store parameters also in ICS_CB_BLOCK_START
            compBlockInsParArg(bp->_blockPos, 0, par);

			// write address of ICS_CB_BLOCK_END to all code block commands of this code block
			compBlockArgAddress(bp->_blockPos, a, true, elem);

			// write address of ICS_CB_BLOCK_END to code block control commands within this code block
			compBlockArgAddress(cp->_controlCBPos, a, false);

            // store positions of code block commands of this code block in the direct outer code block part
            ParseScope *opp = bp->_outerPartScope;
            if (opp) {
                if (!opp->_blockPos) {
                    opp->_blockPos = bp->_blockPos;
                    bp->_blockPos = NULL;
                }
                else {
                    opp->_blockPos->insert(opp->_blockPos->end(), bp->_blockPos->begin(), bp->_blockPos->end());
                    delete bp->_blockPos;
                    bp->_blockPos = NULL;
                }
            }
		}

		return a;
	}

	/**
	 * compile argument address to vector of code block commands
	 * @param vp		vector of code block command positions
	 * @param addr		address to write as argument
	 * @param n1		true: compile to v.n.n1 / false: compile to v.n.n2
	 * @param elem		syntax element
     * @param minor     only compile address to code block commands with this minor command code / 0: no such condition
	 */
    void CompilerContext::compBlockArgAddress(vector<unsigned> *vp, unsigned addr, bool n1, SyntaxElement *elem, unsigned short minor)
	{
		if (vp) {
			vector<unsigned>::iterator it;
			for (it = vp->begin(); it != vp->end(); it++) {
				IntCode::IcElem *p = _code + *it;
                if (p->tp != IntCode::icTypeCommand || (p->v.c.major != INTCODE_CODEBLOCK && p->v.c.major != INTCODE_CB_HEADER && p->v.c.major != INTCODE_CB_CONTROL))
					_modp->ctrl()->errHandler().internalError("wrong code address in CompilerContext::compBlockArgAddress()");

                if (minor && (p->v.c.major != INTCODE_CODEBLOCK || p->v.c.minor != minor))
                        continue;

				if (elem && p->se == 0)
					p->se = elem->id();

				IntCode::IcElem *pa = p + 1;
				if (n1)
					pa->v.n.n1 = addr;
				else
					pa->v.n.n2 = addr;

				if (elem && pa->se == 0)
					pa->se = elem->id();

				if (p->v.c.cnt >= 2 && elem) {
					pa++;
					if (pa->se == 0)
						pa->se = elem->id();
				}
			}
		}
	}

	/**
     * insert parameter for code block command, and optional address to first argument in n2
	 * @param vp		vector of code block command positions
	 * @param offset	position in vp (if negative from end)
	 * @param par		parameter value to insert
     * @param addr      address to write in n2 of first argument value / 0: no address to write
	 * @param elem		syntax element
	 */
    void CompilerContext::compBlockInsParArg(vector<unsigned> *vp, int offset, unsigned short par, unsigned addr, SyntaxElement *elem)
	{
		if (vp) {
			int pos = (offset >= 0 ? offset : (vp->size() + offset));
			if (pos >= 0 && pos < (int)(vp->size())) {
				IntCode::IcElem *p = _code + vp->at(pos);
				if (p->tp != IntCode::icTypeCommand || p->v.c.major != INTCODE_CODEBLOCK)
					_modp->ctrl()->errHandler().internalError("wrong code address in CompilerContext::compBlockArgAddress()");

				p->v.c.par = par;

                if (addr) {
                    p[1].v.n.n2 = addr;
                }

				if (elem) {
					p->se = elem->id();
                    p[1].se = elem->id();
				}
			}
		}
	}

	/**
	 * compile extending or reseting of line name prefix
	 * @param elem		syntax element
	 * @param lnp		extend line name prefix by that / NULL: reset line name prefix
	 * @return			address of code element
	 */
	unsigned CompilerContext::compLineNamePref(SyntaxElement *elem, const char *lnp)
	{
		unsigned a = _codeCnt;

		if (lnp) {
			bool found;
			unsigned i = data()->globStrings()->searchInsert(lnp, false, true, found);

			bool sepArg = (i > USHRT_MAX);
			compCmd(elem, INTCODE_LINENAMEPREF, ICS_LNP_EXTEND, (sepArg ? 0 : (unsigned short)i), (sepArg ? 1 : 0));
			if (sepArg)
				compArg(elem, (intType)i);
		}
		else {
			compCmd(elem, INTCODE_LINENAMEPREF, ICS_LNP_RESET, 0, 0);
		}

		return a;
	}

	/**
	 * compile line name prefix compatibility
	 * @param elem		syntax element
	 * @param expSym	expression containing an invalid symbol to use as line name prefix
	 * @return			address of code element
	 */
	unsigned CompilerContext::compLineNamePrefCompat(SyntaxElement *elem, ExprParseInfo *expSym)
	{
		unsigned a = _codeCnt;

		// fetch command for the invalid symbol must be changed
		const char *lnp = expSym->getLvalueSymName();
		if (!lnp)
			_modp->ctrl()->errHandler().internalError("no line name prefix in CompilerContext::compLineNamePrefCompat()");

		unsigned addr = expSym->getLvalueCodeAddr();
		IntCode::IcElem *p = _code + addr;
		if (p->tp != IntCode::icTypeCommand || p->v.c.major != INTCODE_FETCH)
			_modp->ctrl()->errHandler().internalError("wrong code address in CompilerContext::compLineNamePrefCompat()");

		_codeCnt = addr;
		compLineNamePref(elem, lnp);
		if (_codeCnt == addr + 1);
			compNop(elem, _codeCnt, 0);

		// compile reset of line name prefix
		_codeCnt = a;
		compLineNamePref(elem, NULL);
			
		return a;
	}

	/**
	 * set for a compiled command and its args the syntax element
	 * @param addr		address of the compiled command
	 * @param elem		syntax element
	 */
	void CompilerContext::compSetCmdSyntaxElem(unsigned addr, SyntaxElement *elem)
	{
		PROTO_OUTL("compSetCmdSyntaxElem: " << addr << ", " << elem->id());

		IntCode::IcElem *p = _code + addr;
		p->se = elem ? elem->id() : 0;

		if (p->v.c.cnt > 0) {
			for (unsigned i = 0; i < p->v.c.cnt; i++, p++)
				p->se = elem ? elem->id() : 0;
		}
	}

}

