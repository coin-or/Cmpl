
/***********************************************************************
 *  This code is part of CMPL
 *
 *  Copyright (C) 2007, 2008, 2009, 2010, 2011 Thomas Schleiff - Halle(Saale),
 *  Germany and Mike Steglich - Technical University of Applied Sciences
 *  Wildau, Germany 
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


#include "CompilerUtil.hh"
#include "CompilerContext.hh"
#include "../../Control/MainControl.hh"
#include "../../CommonData/ExternDataInfo.hh"


namespace cmpl
{

	/****** AssignModificators ****/

	/**
	 * set a modificator.
	 * @param comp          compiler context object, only for error handling
	 * @param loc			location, only for error handling
	 * @param mod           modificator (only used if odType is NULL)
	 * @param odType        symbol for object type or data type
	 * @param isObjType     odType is object type
	 */
	void AssignModificators::setModificator(CompilerContext *comp, LocationInfo *loc, ModificatorKey mod, SymbolDef *odType, bool isObjType)
	{
		bool redef = false;

		if (odType) {
			// set object type or data type
			if (isObjType) {
				if (_defObjType == assignModLevelUndefined) {
					_defObjType = assignModLevelAssign;
					_locObjType = *loc;
					_objType = odType;
				}
				else
					redef = true;
			}
			else {
				if (_defDataType == assignModLevelUndefined) {
					_defDataType = assignModLevelAssign;
					_locDataType = *loc;
					_dataType = odType;
				}
				else
					redef = true;
			}
		}
		else {
			// set a single keyword modificator
			if (mod == modificatorPublic || mod == modificatorPrivate || mod == modificatorLocal) {
				if (_defSymScope == assignModLevelUndefined) {
					_defSymScope = assignModLevelAssign;
					_locSymScope = *loc;
					_symScope = (mod == modificatorPublic ? symbolScopeTypePublic : (mod == modificatorPrivate ? symbolScopeTypePrivate : symbolScopeTypeLocal));
				}
				else
					redef = true;
			}
			else if (mod == modificatorConst) {
				if (_defIsConst == assignModLevelUndefined) {
					_defIsConst = assignModLevelAssign;
					_locIsConst = *loc;
					_isConst = true;
				}
				else
					redef = true;
			}
			else if (mod == modificatorNew) {
				if (_defIsNew == assignModLevelUndefined) {
					_defIsNew = assignModLevelAssign;
					_locIsNew = *loc;
					_isNew = true;
				}
				else
					redef = true;
			}
			else if (mod == modificatorExtern) {
				if (_defIsExtern == assignModLevelUndefined) {
					_defIsExtern = assignModLevelAssign;
					_locIsExtern = *loc;
					_isExtern = true;
				}
				else
					redef = true;
			}
            else if (mod == modificatorInitial) {
                if (_defIsInitial == assignModLevelUndefined) {
                    _defIsInitial = assignModLevelAssign;
                    _locIsInitial = *loc;
                    _isInitial = true;
                }
                else
                    redef = true;
            }
            else if (mod == modificatorRef) {
				if (_defIsRef == assignModLevelUndefined) {
					_defIsRef = assignModLevelAssign;
					_locIsRef = *loc;
					_isRef = true;
				}
				else
					redef = true;
			}
			else if (mod == modificatorAssert) {
				if (_defIsAssert == assignModLevelUndefined) {
					_defIsAssert = assignModLevelAssign;
					_locIsAssert = *loc;
					_isAssert = true;
				}
				else
					redef = true;
			}
			else if (mod == modificatorDeclare) {
				if (_defIsDeclare == assignModLevelUndefined) {
					_defIsDeclare = assignModLevelAssign;
					_locIsDeclare = *loc;
					_isDeclare = true;
				}
				else
					redef = true;
			}
            else if (mod == modificatorNocond) {
                if (_defIsNocond == assignModLevelUndefined) {
                    _defIsNocond = assignModLevelAssign;
                    _locIsNocond = *loc;
                    _isNocond = true;
                }
                else
                    redef = true;
            }
            else if (mod == modificatorOrdered) {
				if (_defIsOrdered == assignModLevelUndefined) {
					_defIsOrdered = assignModLevelAssign;
					_locIsOrdered = *loc;
                    _isOrdered = true;
				}
				else
					redef = true;
			}
			else if (mod == modificatorBreak || mod == modificatorContinue || mod == modificatorRepeat || mod == modificatorReturn) {
				if (_defControlCB == assignModLevelUndefined) {
					_defControlCB = assignModLevelAssign;
					_locControlCB = *loc;
					_controlCB = mod;
				}
				else
					redef = true;
			}
		}

		if (redef)
			ERRHANDLERCOMPC->error(ERROR_LVL_WARN, "multiple modificator of same type, only first is considered", *loc);
	}

	/**
	 * add modificators from another modificators object.
	 * @param mod           other modificators object
	 */
	void AssignModificators::addModificators(AssignModificators& mod)
	{
		if (_defObjType == assignModLevelUndefined && mod._defObjType != assignModLevelUndefined) {
			_defObjType = mod._defObjType;
			_locObjType = mod._locObjType;
			_objType = mod._objType;
		}

		if (_defDataType == assignModLevelUndefined && mod._defDataType != assignModLevelUndefined) {
			_defDataType = mod._defDataType;
			_locDataType = mod._locDataType;
			_dataType = mod._dataType;
		}

		if (_defSymScope == assignModLevelUndefined && mod._defSymScope != assignModLevelUndefined) {
			_defSymScope = mod._defSymScope;
			_locSymScope = mod._locSymScope;
			_symScope = mod._symScope;
		}

		if (_defIsConst == assignModLevelUndefined && mod._defIsConst != assignModLevelUndefined) {
			_defIsConst = mod._defIsConst;
			_locIsConst = mod._locIsConst;
			_isConst = mod._isConst;
		}

		if (_defIsNew == assignModLevelUndefined && mod._defIsNew != assignModLevelUndefined) {
			_defIsNew = mod._defIsNew;
			_locIsNew = mod._locIsNew;
			_isNew = mod._isNew;
		}

		if (_defIsExtern == assignModLevelUndefined && mod._defIsExtern != assignModLevelUndefined) {
			_defIsExtern = mod._defIsExtern;
			_locIsExtern = mod._locIsExtern;
			_isExtern = mod._isExtern;
		}

        if (_defIsInitial == assignModLevelUndefined && mod._defIsInitial != assignModLevelUndefined) {
            _defIsInitial = mod._defIsInitial;
            _locIsInitial = mod._locIsInitial;
            _isInitial = mod._isInitial;
        }

		if (_defIsRef == assignModLevelUndefined && mod._defIsRef != assignModLevelUndefined) {
			_defIsRef = mod._defIsRef;
			_locIsRef = mod._locIsRef;
			_isRef = mod._isRef;
		}

		if (_defIsAssert == assignModLevelUndefined && mod._defIsAssert != assignModLevelUndefined) {
			_defIsAssert = mod._defIsAssert;
			_locIsAssert = mod._locIsAssert;
			_isAssert = mod._isAssert;
		}

		if (_defIsDeclare == assignModLevelUndefined && mod._defIsDeclare != assignModLevelUndefined) {
			_defIsDeclare = mod._defIsDeclare;
			_locIsDeclare = mod._locIsDeclare;
			_isDeclare = mod._isDeclare;
		}

        if (_defIsNocond == assignModLevelUndefined && mod._defIsNocond != assignModLevelUndefined) {
            _defIsNocond = mod._defIsNocond;
            _locIsNocond = mod._locIsNocond;
            _isNocond = mod._isNocond;
        }

		if (_defIsOrdered == assignModLevelUndefined && mod._defIsOrdered != assignModLevelUndefined) {
			_defIsOrdered = mod._defIsOrdered;
			_locIsOrdered = mod._locIsOrdered;
			_isOrdered = mod._isOrdered;
		}

		if (_defControlCB == assignModLevelUndefined && mod._defControlCB != assignModLevelUndefined) {
			_defControlCB = mod._defControlCB;
			_locControlCB = mod._locControlCB;
			_controlCB = mod._controlCB;
		}
	}

	/**
	 * change level for set assign modificators
	 * @param comp          compiler context object, only for error handling
	 * @param loc			location, only for error handling
	 * @param lvl           dest level
	 */
	void AssignModificators::chgAssignModLevel(CompilerContext *comp, LocationInfo *loc, AssignModLevel lvl)
	{
		if (_defObjType != assignModLevelUndefined)
			_defObjType = lvl;

		if (_defDataType != assignModLevelUndefined)
			_defDataType = lvl;

		if (_defSymScope != assignModLevelUndefined)
			_defSymScope = lvl;

		if (_defIsConst != assignModLevelUndefined)
			_defIsConst = lvl;

		if (_defIsNew != assignModLevelUndefined)
			_defIsNew = lvl;

		if (_defIsExtern != assignModLevelUndefined)
			_defIsExtern = lvl;

        if (_defIsInitial != assignModLevelUndefined)
            _defIsInitial = lvl;

		if (_defIsRef != assignModLevelUndefined)
			_defIsRef = lvl;

		if (_defIsAssert != assignModLevelUndefined)
			_defIsAssert = lvl;

		if (_defIsDeclare != assignModLevelUndefined)
			_defIsDeclare = lvl;

        if (_defIsNocond != assignModLevelUndefined)
            _defIsNocond = lvl;

        if (_defIsOrdered != assignModLevelUndefined)
			_defIsOrdered = lvl;

		if (_defControlCB != assignModLevelUndefined) {
			_defControlCB = lvl;
			if (lvl == assignModLevelDirective) {
				ERRHANDLERCOMPC->error(ERROR_LVL_NORMAL, "codeblock control modificators not allowed in directive", _locControlCB);
				_defControlCB = assignModLevelUndefined;
			}
		}
	}

	/**
	 * get whether assign modificators indicate a codeblock control assignment
	 * @param comp          compiler context object, only for error handling
	 * @param loc			location, only for error handling
	 * @param cb			return codeblock control (if true, then only modificatorBreak, modificatorContinue, modificatorRepeat or modificatorReturn)
	 */
	bool AssignModificators::isControlCBAssign(CompilerContext *comp, LocationInfo *loc, ModificatorKey& cb)
	{
		cb = _controlCB;
		if (_defControlCB == assignModLevelUndefined)
			return false;

		ParseScope *pp = comp->curParseScope();

		// must be within a codeblock, for modificatorReturn within a function
		if (_controlCB == modificatorReturn) {
			if (!(pp->outerFunc()) || pp->outerFunc()->isBasisScope()) {
				ERRHANDLERCOMPC->error(ERROR_LVL_NORMAL, "'return' cannot be used outside of a function", _locControlCB);
				_defControlCB = assignModLevelUndefined;
				return false;
			}
		}
		else {
			if (pp->isBasisScope()) {
				ERRHANDLERCOMPC->error(ERROR_LVL_NORMAL, "codeblock control modificator cannot be used outside of a codeblock", _locControlCB);
				_defControlCB = assignModLevelUndefined;
				return false;
			}
		}

		// no other assign modificator can be used together with a codeblock control command (save _defIsOrdered)
		if (_defObjType != assignModLevelUndefined || _defDataType != assignModLevelUndefined || _defSymScope != assignModLevelUndefined ||
				_defIsConst != assignModLevelUndefined || _defIsNew != assignModLevelUndefined || _defIsExtern != assignModLevelUndefined ||
                _defIsInitial != assignModLevelUndefined || _defIsRef != assignModLevelUndefined || _defIsAssert != assignModLevelUndefined ||
                _defIsDeclare != assignModLevelUndefined || _defIsNocond != assignModLevelUndefined) {
			ERRHANDLERCOMPC->error(ERROR_LVL_NORMAL, "codeblock control modificator cannot be combined with other assign modificators", _locControlCB);
			_defControlCB = assignModLevelUndefined;
			return false;
		}

		return true;
	}

#ifdef PROTO
	/**
	 * write AssignModificators to stream
	 */
	ostream& operator<< (ostream& ostr, const AssignModificators& mod)
	{
		ostr << "Modificators <";

		if (mod._defObjType != assignModLevelUndefined)
			ostr << " ObjectType: " << mod._objType->name() << " (" << (mod._defObjType==assignModLevelDirective ? 'D' : 'A') << ")";

		if (mod._defDataType != assignModLevelUndefined)
			ostr << " DataType: " << mod._dataType->name() << " (" << (mod._defDataType==assignModLevelDirective ? 'D' : 'A') << ")";

		if (mod._defSymScope != assignModLevelUndefined)
			ostr << " Scope: " << (mod._symScope==symbolScopeTypePublic ? "public" : (mod._symScope==symbolScopeTypePrivate ? "private" : "local")) << " (" << (mod._defSymScope==assignModLevelDirective ? 'D' : 'A') << ")";

		if (mod._defIsConst != assignModLevelUndefined)
			ostr << " Const: " << (mod._isConst ? "yes" : "no") << " (" << (mod._defIsConst==assignModLevelDirective ? 'D' : 'A') << ")";

		if (mod._defIsNew != assignModLevelUndefined)
			ostr << " New: " << (mod._isNew ? "yes" : "no") << " (" << (mod._defIsNew==assignModLevelDirective ? 'D' : 'A') << ")";

		if (mod._defIsExtern != assignModLevelUndefined)
			ostr << " Extern: " << (mod._isExtern ? "yes" : "no") << " (" << (mod._defIsExtern==assignModLevelDirective ? 'D' : 'A') << ")";

		if (mod._defIsRef != assignModLevelUndefined)
			ostr << " Ref: " << (mod._isRef ? "yes" : "no") << " (" << (mod._defIsRef==assignModLevelDirective ? 'D' : 'A') << ")";

		if (mod._defIsAssert != assignModLevelUndefined)
			ostr << " Assert: " << (mod._isAssert ? "yes" : "no") << " (" << (mod._defIsAssert==assignModLevelDirective ? 'D' : 'A') << ")";

		if (mod._defIsDeclare != assignModLevelUndefined)
			ostr << " Declare: " << (mod._isDeclare ? "yes" : "no") << " (" << (mod._defIsDeclare==assignModLevelDirective ? 'D' : 'A') << ")";

		if (mod._defIsOrdered != assignModLevelUndefined)
			ostr << " Ordered: " << (mod._isOrdered ? "yes" : "no") << " (" << (mod._defIsOrdered==assignModLevelDirective ? 'D' : 'A') << ")";

		if (mod._defControlCB != assignModLevelUndefined)
			ostr << " ControlCB: " << (mod._controlCB==modificatorBreak ? "break" : (mod._controlCB==modificatorContinue ? "continue" : (mod._controlCB==modificatorRepeat ? "repeat" : "return"))) << " (" << (mod._defControlCB==assignModLevelDirective ? 'D' : 'A') << ")";
		
		ostr << " >";

		return ostr;
	}
#endif //PROTO



	/****** ParseScope ****/

	/**
	 * constructor
	 * @param comp      parse context
	 * @param lvl       level of this scope within the parse context
	 * @param scopeType type of parse scope
	 * @param cbm       mode for code block for distinction between expression and statement
	 * @param op		outer parse scope
	 */
	ParseScope::ParseScope(CompilerContext *comp, unsigned lvl, ParseScopeType scopeType, CodeBlockExprMode cbm, ParseScope *op):
		_assignModCurrent(), _assignModDirective()
	{
		_comp = comp;
		_level = lvl;
		_scopeType = scopeType;
		_cbExprMode = cbm;
		_inFuncDef = parseScopeTypeBasis;
        _codeStart = comp->curCodeAddr();

		if (_scopeType == parseScopeTypeBasis)
			_outerFunc = NULL;
		else
			_outerFunc = (op->isFuncOrBasisScope() ? op : op->_outerFunc);

		_localSymCur = (isFuncOrBasisScope() ? 0 : _outerFunc->_localSymCur);
		_localSymMax = 0;
		_hasSubSymbol = false;

		_blockScope = (isBlockScope() ? this : (op ? op->_blockScope : NULL));
		_outerPartScope = (isBlockScope() || !op ? NULL : op->_outerPartScope);
		_blockPos = NULL;
		_controlCBPos = NULL;
		_cbHeaderPos = NULL;
        _subValidPos = NULL;
		_blockLevel = (isFuncOrBasisScope() ? 0 : (op->_blockLevel + (isBlockScope() ? 1 : 0))); 

		_nameSymId = 0;
		_lineNamePref = NULL;

		_elem = NULL;
		_cntBlockPart = 0;
		_parBlockPart = 0;
	}

	/**
	 * get local symbol index for a new local symbol defined in this scope
	 * @param loc       location, only for error handling
	 */
	unsigned ParseScope::newLocalSym(LocationInfo *loc)
	{
		ParseScope *pp = (isFuncOrBasisScope() ? this : _outerFunc);
		unsigned r = (pp->_localSymCur)++;

		if (pp->_localSymCur > pp->_localSymMax)
			pp->_localSymMax = pp->_localSymCur;

		if (r >= USHRT_MAX) {
			CompilerContext *comp = _comp;
			ERRHANDLERCOMPC->error(ERROR_LVL_NORMAL, "too many local symbols", *loc);
		}

		return r;
	}

	/**
	 * set directive from current assign modificators
	 * @param loc       location, only for error handling
	 */
	void ParseScope::setDirectiveFromCurrent(LocationInfo *loc)
	{
		_assignModDirective = _assignModCurrent;
		_assignModDirective.chgAssignModLevel(_comp, loc, assignModLevelDirective);
		_assignModCurrent.init();

		if (_lineNamePref) {
			CompilerContext *comp = _comp;
			ERRHANDLERCOMPC->error(ERROR_LVL_NORMAL, "line name prefix not allowed in directive", _locLineNamePref);
			_lineNamePref = NULL;
		}
	}


	/**
	 * set line name prefix
	 * @param elem		syntax element
	 * @param loc		location, only for error handling
	 * @param lnp		line name prefix
	 */
	void ParseScope::setLineNamePref(SyntaxElement *elem, LocationInfo *loc, const char *lnp)
	{
		if (_lineNamePref) {
			CompilerContext *comp = _comp;
			ERRHANDLERCOMPC->error(ERROR_LVL_NORMAL, "multiple line name prefix not allowed in one statement", *loc);
			return;
		}

		if (!_comp->checkOnly())
			_comp->compLineNamePref(elem, lnp);
		
		_lineNamePref = lnp;
		_locLineNamePref = *loc;
	}


	/**
	 * end of statement processing
	 * @param elem		syntax element
	 */
	void ParseScope::endOfStatement(SyntaxElement *elem)
	{
		if (_lineNamePref) {
			if (!_comp->checkOnly())
				_comp->compLineNamePref(elem, NULL);

			_lineNamePref = NULL;
		}

		_assignModCurrent.init();
	}



	/****** SymbolDef ****/

	/**
	 * get symbol definition entry for declaring symbol as valid
	 * @param st        type of symbol definition scope
	 * @param dl        definition scope level for local symbol
	 * @return			symbol definition to call declSymbol() at
	 */
	SymbolDef * SymbolDef::declSymbolEntry(SymbolScopeType st, unsigned dl)
	{
		// start search from innerst symbol definition
		if (_inner)
			return innerst()->declSymbolEntry(st, dl);

		// delete entries with higher definition level, save outerst entry
		if (st == symbolScopeTypePublic)
			dl = 0;

		SymbolDef *s = this;
		while (s->_defLevel > dl && s->_outer)
			s = s->_outer;

		return s;
	}

	/**
	 * declare symbol as valid
	 * @param comp		compiler context
	 * @param loc		symbol location
	 * @param st        type of symbol definition scope
	 * @param dl        definition scope level for local symbol
	 * @param ro        read-only symbol
	 * @param sp        special meaning for the symbol
	 * @return			symbol definition declared as valid
	 */
	SymbolDef * SymbolDef::declSymbol(CompilerContext *comp, LocationInfo& loc, SymbolScopeType st, unsigned dl, bool ro, SpecialSymbol sp)
	{
		SymbolDef *s = this;

		// delete inner entries
		if (_inner) {
			s = innerst();
			while (s != this) {
				SymbolDef *t = s->_outer;
				delete s;
				s = t;
			}
		}

		// if this is not undefined, then new symbol is necessary
		if (_valid) {
			if (st != symbolScopeTypePublic && _defLevel < dl) {
				s = new SymbolDef(this, dl);
				if (_hideLevel == 0 || _hideLevel > dl) {
					_hideLevel = dl;
					_hidePure = false;
				}
			}

			s->setNewDefId(comp->newSymbolDefCnt());
            s->insertSymbolInfo(comp, s->defId(), loc, 0, 0);
		}

		PROTO_MOD_OUTL(comp, "declSymbol: " << _name << " (" << (_symTab->sub() ? 1 : 0) << ", " << s->_defId << "): " << (int)st << " " << (st == symbolScopeTypePublic ? 0 : dl));
		
		// set as valid
		s->_valid = true;
		s->_scopeType = st;
		s->_defLevel = (st == symbolScopeTypePublic ? 0 : dl);
		s->_hideLevel = 0;
		s->_hidePure = false;
		s->_readOnly = ro;
		s->_special = sp;

		if (!s->global())
			s->_localInd = comp->curParseScope()->newLocalSym(&loc);
		else if (!_symTab->sub())
			s->setNewGlobId(comp->newSymbolGlobCnt());

		if (s->_symInfo)
			s->_symInfo->setType(ro, (int)st, (int)sp, s->_globId);

		return s;
	}


	/**
	 * set special meaning for a already declared symbol
	 * @param sp        special meaning for the symbol
	 */
	void SymbolDef::setSpecial(SpecialSymbol sp)
	{
		_special = sp;
		if (_symInfo)
			_symInfo->setType(_readOnly, (int)_scopeType, (int)sp, _globId);
	}
	

	/**
	 * get predefined command number for a pseudo symbol
	 */
	unsigned short SymbolDef::pseudoSymbolDef()
	{
        return (unsigned short)(pseudoSymbol() && _symInfo->predef() > 0 && _symInfo->predef() < 10000 ? _symInfo->predef() : 0);
	}


	/**
	 * get symbol name (from global string store)
	 */
	const char *SymbolDef::nameGlob()
	{
		return _symInfo->name();
	}


	/**
	 * get whether the symbol is a subsymbol
	 */
    bool SymbolDef::subsymbol()
	{
		return _symTab->sub();
	}

	
	/**
	 * return whether symbol is predefined
	 */
    bool SymbolDef::predef()
	{
		return _symInfo->predef();
	}


	/**
	 * insert symbol in symbol info vector
	 * @param comp			compiler context
     * @param symkey        unique key for symbol, normally symbol defId
	 * @param loc			symbol location
	 * @param predef		predefined symbol value (0: no predefined symbol; >0: pseudo symbol (one of ICS_SPECIAL_*); <0: predefined symbol (one of PREDEFSYMVAL_*)
     * @param predefCont    only for predefined subsymbol: predef of container the subsymbol belongs to
	 */
    void SymbolDef::insertSymbolInfo(CompilerContext *comp, unsigned symkey, LocationInfo& loc, int predef, int predefCont)
	{
		LocationInfo l = loc.withoutInLoc();
        SymbolInfo *sym = new SymbolInfo(comp->data(), _name, _defId, _globId, l, predef, _symTab->sub(), _readOnly, (int)_scopeType, (int)_special, predefCont);

        // symkey can only differ from _defId for a predefined subsymbol for another predefined container or for a predefined namespace symbol, then _symInfo is already set for the symbol
        if (symkey == _defId)
            _symInfo = sym;

        comp->addSymbolInfo(symkey, sym);
	}

	/**
	 * set the symbol state after entering a new parse scope
	 * @param lvl           parse scope level
	 * @param pst           parse scope type
	 */
	void SymbolDef::enterParseScope(unsigned lvl, ParseScopeType pst)
	{
		if (pst == parseScopeTypePure || pst == parseScopeTypeFunc) {
			SymbolDef *sym = innerst();
			if (sym->toHideForFuncScope(pst == parseScopeTypePure)) {
				// additional pure function hide if necessary
				if (pst == parseScopeTypePure && sym->_hideLevel > 0 && !sym->_hidePure)
					sym = new SymbolDef(sym, lvl);

				// hide symbol
				if (sym->_hideLevel == 0) {
					sym->_hideLevel = lvl;
					sym->_hidePure = (pst == parseScopeTypePure);
				}
			}
		}
	}

	/**
	 * set the symbol state before leaving a parse scope
	 * @param lvl           parse scope level
	 * @param pst           parse scope type
	 */
	void SymbolDef::leaveParseScope(unsigned lvl, ParseScopeType pst)
	{
		SymbolDef *sym = innerst();
		if (sym->_valid) {
			// symbol out of scope
			if (sym->_defLevel == lvl) {
				if (sym->_outer) {
					sym = sym->_outer;
					delete sym->_inner;
					sym->_inner = NULL;
				}
				else {
					sym->_valid = false;
					sym->_defLevel = sym->_hideLevel = 0;
					sym->_scopeType = symbolScopeTypeUndefined;
				}
			}
			
			// unhide symbol
			if (sym->_hideLevel == lvl)
				sym->_hideLevel = 0;
		}
	}

	/**
	 * symbol is left hand side of an assignment, check compatibility with its definition or provide it to a new definition
	 * @param comp          compiler object
	 * @param scp			current parse scope
	 * @param am            assign modificators
	 * @param localAssign	assign without use of directive
	 * @param rep			repeated occurence of same symbol in one assignment
	 * @param dl			definition scope level for local symbol
	 * @param nms			name of container symbol if symbol is subsymbol in expression without indizes
	 * @param sst			return type of symbol definition scope if symbol is to define / symbolScopeTypeUndefined: don't define symbol
	 * @param ro			return whether the symbol should be defined as readonly
	 * @param sp			return of special meaning for the symbol
	 * @param symLoc		location of symbol
	 * @param expLoc		location of expression
	 * @return				true: ok / false: error
	 */
	bool SymbolDef::checkDefSymbol(CompilerContext *comp, ParseScope *scp, AssignModificators *am, bool localAssign, bool rep, unsigned dl, const char *nms, SymbolScopeType& sst, bool& ro, SpecialSymbol& sp, LocationInfo& symLoc, LocationInfo& expLoc)
	{
		PROTO_MOD_OUTL(comp, "checkDefSymbol: " << _name << " (" << (_symTab->sub() ? 1 : 0) << ", " << _defId << ")");
		PROTO_MOD_OUTL(comp, "  modificators: " << *am);

		sst = symbolScopeTypeUndefined;
		ro = false;
		sp = specialSymbolNo;

		if (_symTab->sub()) {
			// subsymbol (most checks cannot be executed here, but at runtime)
			if (am->_defControlCB != assignModLevelUndefined) {
				ERRHANDLERCOMPC->error(ERROR_LVL_NORMAL, comp->ctrl()->printBuffer("assignment for subsymbol '%s'", name()), symLoc, "cannot be used with a codeblock control modificator", am->_locControlCB);
				am->_defControlCB = assignModLevelUndefined;
				return false;
			}

            if (_readOnly && pseudoSymbol()) {
                ERRHANDLERCOMPC->error(ERROR_LVL_NORMAL, comp->ctrl()->printBuffer("cannot assign readonly pseudo symbol '%s'", name()), symLoc);
                return false;
            }

            //if ((am->_defIsNew != assignModLevelUndefined && am->_isNew && (am->_defSymScope == assignModLevelUndefined || am->_symScope != symbolScopeTypePrivate))
            //			|| (am->_defSymScope != assignModLevelUndefined && am->_symScope == symbolScopeTypePrivate && (am->_defIsNew == assignModLevelUndefined || !am->_isNew))
            //			|| (am->_defSymScope != assignModLevelUndefined && am->_symScope != symbolScopeTypePublic && am->_symScope != symbolScopeTypePrivate)) {
            //		ERRHANDLERCOMPC->error(ERROR_LVL_NORMAL, comp->ctrl()->printBuffer("definition of non-public subsymbol '%s'", name()), symLoc, "must use both modificators new and private", (am->_defSymScope != assignModLevelUndefined ? am->_locSymScope : am->_locIsNew));
            //		return false;
            //}
			 
			// check extern symbol
			if (am->_defIsExtern != assignModLevelUndefined && am->_isExtern) {
				if (!comp->checkExternSymbol(nms, _name, sp)) {
					ERRHANDLERCOMPC->error(ERROR_LVL_NORMAL, comp->ctrl()->printBuffer("no extern symbol '%s' found in the namespace '%s'", name(), (nms ?: "(unknown)")), symLoc);
					return false;
				}
			}
		}

		else { 
			if (!visible() || (am->_defIsNew != assignModLevelUndefined && am->_isNew && !rep)) {
				// new define of symbol
				sst = (am->_defSymScope != assignModLevelUndefined ? am->_symScope : (localAssign ? symbolScopeTypeLocal : symbolScopeTypePublic));

				// if hidden and not new then not accessible
				if (!notDefined() && (am->_defIsNew == assignModLevelUndefined || !am->_isNew)) {
					ERRHANDLERCOMPC->error(ERROR_LVL_NORMAL, comp->ctrl()->printBuffer("symbol '%s' is not accessible", name()), symLoc);
					return false;
				}

				// check if new is allowed
				if (am->_defIsNew != assignModLevelUndefined && am->_isNew && _valid && (_hideLevel == 0 || _hideLevel > _hidePure)) {
					if (!(comp->modpt()->allowNewPublic()) && (sst == symbolScopeTypePublic || _defLevel == dl)) {
						ERRHANDLERCOMPC->error(ERROR_LVL_NORMAL, "definition with 'new'", am->_locIsNew, comp->ctrl()->printBuffer("would permanently discard the previous symbol '%s'", name()), symLoc);
						am->_defIsNew = assignModLevelUndefined;
						return false;
					}

					if (!(comp->modpt()->allowNewConst()) && _readOnly && _hideLevel == 0) {
						ERRHANDLERCOMPC->error(ERROR_LVL_NORMAL, "definition with 'new'", am->_locIsNew, comp->ctrl()->printBuffer("would break const of symbol '%s'", name()), symLoc);
						am->_defIsNew = assignModLevelUndefined;
						return false;
					}
				}

				// codeblock control cannot be used with new symbol
				if (am->_defControlCB != assignModLevelUndefined) {
					ERRHANDLERCOMPC->error(ERROR_LVL_NORMAL, comp->ctrl()->printBuffer("yet undefined or inaccessible symbol '%s'", name()), symLoc, "cannot be used with codeblock control modificator", am->_locControlCB);
					am->_defControlCB = assignModLevelUndefined;
					return false;
				}

				// assert cannot be used in new definition
				if (am->_defIsAssert != assignModLevelUndefined && am->_isAssert) {
					ERRHANDLERCOMPC->error(ERROR_LVL_NORMAL, comp->ctrl()->printBuffer("yet undefined or inaccessible symbol '%s'", name()), symLoc, "cannot be used with 'assert'", am->_locIsAssert);
					am->_defIsAssert = assignModLevelUndefined;
					return false;
				}

				// definition of new symbol must have given scope within a function context
				if (scp->_inFuncDef != parseScopeTypeBasis && !localAssign && am->_defSymScope == assignModLevelUndefined) {
					ERRHANDLERCOMPC->error(ERROR_LVL_NORMAL, comp->ctrl()->printBuffer("scope type must be given for definition of symbol '%s' within function context", name()), symLoc);
					return false;
				}

				// "public" not allowed in pure function context
				if (scp->_inFuncDef == parseScopeTypePure && sst == symbolScopeTypePublic) {
					if (am->_defSymScope != assignModLevelUndefined && am->_symScope == symbolScopeTypePublic) {
						ERRHANDLERCOMPC->error(ERROR_LVL_NORMAL, "scope type cannot be public within a pure function context", am->_locSymScope, comp->ctrl()->printBuffer("for symbol '%s'", name()), symLoc);
						am->_defSymScope = assignModLevelUndefined;
					}
					else {
						ERRHANDLERCOMPC->error(ERROR_LVL_NORMAL, comp->ctrl()->printBuffer("scope type cannot be public within a pure function context for symbol '%s'", name()), symLoc);
					}
					return false;
				}

				// check extern symbol
				if (am->_defIsExtern != assignModLevelUndefined && am->_isExtern) {
					if (!comp->checkExternSymbol(NULL, _name, sp)) {
						ERRHANDLERCOMPC->error(ERROR_LVL_NORMAL, comp->ctrl()->printBuffer("no extern data symbol '%s' found", name()), symLoc, "in definition with 'extern'", am->_locIsExtern);
						return false;
					}
				}

				// new codeblock symbol can be only once defined
				if (sst == symbolScopeTypeCodeBlock && rep) {
					ERRHANDLERCOMPC->error(ERROR_LVL_NORMAL, comp->ctrl()->printBuffer("repeated occurence of codeblock symbol '%s' in one codeblock header expression is not allowed", name()), symLoc);
					return false;
				}

				ro = (am->_defIsConst != assignModLevelUndefined && am->_isConst);
			}

			else {
				// check compatibility of assign modificators to symbol definition
				if (am->_defIsDeclare != assignModLevelUndefined && am->_isDeclare) {
					if ((am->_defSymScope == assignModLevelUndefined || am->_symScope == symbolScopeTypePublic) && _scopeType != symbolScopeTypePublic) {
						ERRHANDLERCOMPC->error(ERROR_LVL_NORMAL, comp->ctrl()->printBuffer("cannot declare public symbol '%s' because it is already defined as non-public", name()), symLoc);
						return false;
					}
				}
				else {
					if (am->_defSymScope != assignModLevelUndefined && !rep) {
						ERRHANDLERCOMPC->error(ERROR_LVL_NORMAL, "given definition scope", am->_locSymScope, comp->ctrl()->printBuffer("is not allowed for already defined symbol '%s'", name()), symLoc);
						am->_defSymScope = assignModLevelUndefined;
						return false;
					}
				}

				if (am->_defControlCB != assignModLevelUndefined && _scopeType != symbolScopeTypeCodeBlock && _special != specialSymbolNameOnly) {
					ERRHANDLERCOMPC->error(ERROR_LVL_NORMAL, "use of codeblock control modificator", am->_locControlCB, comp->ctrl()->printBuffer("with symbol '%s' as left hand side which is not a codeblock symbol", name()), symLoc);
					return false;
				}

				if (am->_defIsConst != assignModLevelUndefined && am->_isConst && (am->_defIsDeclare == assignModLevelUndefined || !am->_isDeclare) && !rep) {
					ERRHANDLERCOMPC->error(ERROR_LVL_NORMAL, "given 'const'", am->_locIsConst, comp->ctrl()->printBuffer("is not allowed for already defined symbol '%s'", name()), symLoc);
					am->_defIsConst = assignModLevelUndefined;
					return false;
				}

                if (_readOnly && pseudoSymbol()) {
                    ERRHANDLERCOMPC->error(ERROR_LVL_NORMAL, comp->ctrl()->printBuffer("cannot assign readonly pseudo symbol '%s'", name()), symLoc);
                    return false;
                }

                if (_readOnly && am->_defControlCB == assignModLevelUndefined && (am->_defIsAssert == assignModLevelUndefined || !am->_isAssert) && (am->_defIsDeclare == assignModLevelUndefined || !am->_isDeclare) && !rep) {
					ERRHANDLERCOMPC->error(ERROR_LVL_NORMAL, comp->ctrl()->printBuffer("cannot re-assign const defined symbol '%s'", name()), symLoc);
					return false;
				}

				if (am->_defIsExtern != assignModLevelUndefined && am->_isExtern) {
					ERRHANDLERCOMPC->error(ERROR_LVL_NORMAL, comp->ctrl()->printBuffer("cannot re-define symbol '%s'", name()), symLoc, "in definition with 'extern'", am->_locIsExtern);
					return false;
				}

				if (SPECIALSYMBOL_NO_VALUE(_special) && am->_defControlCB == assignModLevelUndefined) {
					ERRHANDLERCOMPC->error(ERROR_LVL_NORMAL, comp->ctrl()->printBuffer("symbol '%s' cannot be assigned because it is only a codeblock reference name", name()), symLoc);
					return false;
				}
			}
		}

		return true;
	}
			


	/****** SymbolTab ****/

	/**
	 * destructor
	 */
	SymbolTab::~SymbolTab()
	{
		if (_symbols) {
			for (unsigned i = 0; i < _len; i++) {
				SymbolDef *s = _symbols[i];
				while (s) {
					SymbolDef *s2 = s->inner();
					delete s;
					s = s2;
				}
			}

			delete _symbols;
		}
	
		if (_names)
			delete _names;
	}

	/**
	 * search and insert a symbol to the symbol table.
	 * @param str           symbol name
	 * @param free          free original string for symbol name (or don't copy it when inserting, so it is freed in the destructor)
	 * @param insertId      insert symbol name and definition if not found, with this unique definition number / 0: don't insert
	 * @param found         return if symbol is found
	 * @return              pointer to symbol definition
	 */
	SymbolDef *SymbolTab::searchInsert(const char *str, bool free, unsigned insertId, bool &found)
	{
		// search string
		unsigned i = _names->searchInsert(str, free, (insertId > 0), found);
		if (found)
			return _symbols[i];

		if (insertId == 0)
			return NULL;

		if (i >= _cap) {
			// grow the arrays
			unsigned newcap = ARR_NEWCAP(_cap);
			PROTO_OUTL("    grow symbols array from " << _cap << " to " << newcap);
			_cap = newcap;

			ARR_REALLOC(_symbols, SymbolDef *, _len, _cap);
		}

		// insert symbol
		SymbolDef *s = new SymbolDef((*_names)[i], this, i, insertId);
		_symbols[i] = s;
		_len = i + 1;

		return s;
	}


	/****** SymbolLoc ****/

	/**
	 * set symbol
	 */
	void SymbolLoc::setSym(SymbolDef *s)
	{
		_sym = s;
		if (_elem) {
			_elem->setSymId(s->defId());
		}
	}


	/****** ExprParseInfo ****/

	/**
	 * constructor
	 * @param comp			compiler context / NULL: no check for _cbhExpr
	 * @param loc           location of expression (can be NULL)
	 * @param spec          special property of expression
	 * @param empty			expression is empty expression
	 * @param cb			expression is a code block
	 */
	ExprParseInfo::ExprParseInfo(CompilerContext *comp, const LocationInfo &loc, SpecialSymbol spec, bool empty, bool cb)
	{
		_loc = loc;
		_sym = NULL;
		_special = spec;
        _expOp = (cb ? EXPR_INFO_OP_CODEBL : 0);
		_lval = false;
		_indexLval = false;
		_subSymLval = false;
		_empty = empty;
		_hasLNP = false;
		_oneElemArr = false;
		_listCnt = 0;
		_assignOp = NULL;
		_assignMod = NULL;
        _cbhExpr = false;
		_cbhValid = false;
		_volaExp = cb;
        _ncSymAccess = false;
        _cbSymAccess = cb;

		if (comp)
			checkSetCbhExpr(comp);
	}

	/**
	 * constructor
     * @param comp			compiler context
     * @param loc           location of expression (must be set)
	 * @param sym           symbol
	 * @param nw            symbol is new inserted
	 * @param mark			marking of the symbol
	 */
    ExprParseInfo::ExprParseInfo(CompilerContext *comp, const LocationInfo &loc, SymbolDef *sym, bool nw, MarkForSymbolName mark)
	{
		_loc = loc;
		_sym = sym;
		_special = sym->special();
        _expOp = 0;
		_lval = true;
		_indexLval = false;
		_subSymLval = false;
		_empty = false;
		_hasLNP = false;
		_oneElemArr = false;
		_listCnt = 0;

		_lvalSym.push_back(new SymbolLoc(_sym, _loc, mark));

		_assignOp = NULL;
		_assignMod = NULL;
		_cbhExpr = false;
		_cbhValid = false;
		_volaExp = sym->pseudoSymbol();
        _ncSymAccess = (sym->valid() && (!sym->readOnly() || sym->pseudoSymbol()));
        _cbSymAccess = (sym->valid() && sym->scopeType() == symbolScopeTypeCodeBlock && sym->defLevel() == comp->curParseScopeLevel());
    }

	/**
	 * destructor
	 */
	ExprParseInfo::~ExprParseInfo()
	{
		checkClearLvalSyms(NULL, false);
	}


	/**
	 * set _cbhExpr if in an appropriate scope
	 * @param comp          compiler context
	 */
	void ExprParseInfo::checkSetCbhExpr(CompilerContext *comp)
	{
		ParseScope *scp = comp->curParseScope();
		if (scp->_scopeType == parseScopeTypePart) {
			_cbhExpr = true;
			
			if (_sym && _lval && _lvalSym.size() == 1) {
				SymbolLoc *l = _lvalSym[0];
				if (l->mark() == markForSymbolNameCB || (l->mark() == markForSymbolNameNo && l->sym()->notDefined() && comp->modpt()->compatCBSymbols()))
					return;
			}

			_lval = false;
			checkClearLvalSyms(comp);
		}
	}


	/**
	 * set expression as left hand side of assignment, and define the lvalue symbols
	 * @param comp          compiler context
	 * @param op            assign operator / NULL: left hand side only
	 * @param locOp			location of assign operator / NULL: left hand side only
	 */
	void ExprParseInfo::setAssignLhs(CompilerContext *comp, const char *op, LocationInfo *locOp)
	{
		// expression must be an lvalue
		if (!_lval || _lvalSym.size()==0) {
			ERRHANDLERCOMPC->error(ERROR_LVL_NORMAL, "expression is not an lvalue", _loc);
			return;
		}

		_assignOp = op;

		bool localAssign = (op && (op[0] != ':' || op[1] == ':'));
		
		// get resulting modificators
		ParseScope *scp = comp->curParseScope();
		_assignMod = &(scp->_assignModCurrent);

		if (localAssign || _assignMod->_defControlCB != assignModLevelUndefined) {
			// ignore directive in assignment with +=, -=, *=, /= and ::=; or when codeblock control modificator
			if (localAssign && op[1] == '=')
				checkClearLvalSyms(comp, true, false);	// even lval symbols must already exist
		}
		else if (_assignMod->empty()) {
			_assignMod = &(scp->_assignModDirective);
		}
		else {
			_assignMod->addModificators(scp->_assignModDirective);
		}
		
		// definition of lvalue symbols
        assignSymbols(comp, locOp, localAssign, false);
        _expOp = 0;
    }


	/**
	 * set expression as left hand side of assignment without right hand side, if this is a left hand side
	 * @param comp          compiler context
	 * @return				expression is left hand side
	 */
	bool ExprParseInfo::setAssignIfLhs(CompilerContext *comp)
	{
		ParseScope *scp = comp->curParseScope();
		_assignMod = &(scp->_assignModCurrent);
		_assignOp = NULL;

		if (_assignMod->_defControlCB == assignModLevelUndefined) {
			if (_assignMod->empty())
				_assignMod = &(scp->_assignModDirective);
			else
				_assignMod->addModificators(scp->_assignModDirective);
		}

		if (_lval && _lvalSym.size() > 0) {
			SymbolLoc *l = _lvalSym[0];

			// if modificator is "break", "continue" or "repeat" then only codeblock symbol or codeblock reference name can be a left hand side without right hand side
			// if modificator is "return" then the expression cannot be a left hand side
			if (_assignMod->_defControlCB == assignModLevelUndefined || (_assignMod->_controlCB != modificatorReturn && 
						(l->mark() == markForSymbolNameCB || (l->mark() == markForSymbolNameNo && (l->sym()->scopeType() == symbolScopeTypeCodeBlock || l->sym()->special() == specialSymbolNameOnly))))) {
				// definition of lvalue symbols
                assignSymbols(comp, NULL, false, false);
                _expOp = 0;
                return true;
			}
		}

		// not a left hand side, cannot be list of expressions
		if (_listCnt)
			ERRHANDLERCOMPC->error(ERROR_LVL_NORMAL, "expression list not allowed as expression statement", _loc);

		// check if codeblock control modificator is used outside of a codeblock, or together with other modificators
        if (_lval && _lvalSym.size() > 0 && _assignMod->_defControlCB != assignModLevelUndefined) {
			ModificatorKey cb;
			_assignMod->isControlCBAssign(comp, &_loc, cb);
		}

		return false;
	}


	/**
	 * set expression as codeblock header expression and define the new codeblock symbols
	 * @param comp			compiler context
	 */
	void ExprParseInfo::setAssignCbh(CompilerContext *comp)
	{
		// define symbols
		if ((_cbhValid || _sym) && _lvalSym.size() > 0) {
			if (_assignOp && *_assignOp == 'i')
				comp->setParseScopeParBlockPart(ICPAR_BLOCK_OP_IN);

			_assignMod = comp->modAssignCb();

            assignSymbols(comp, NULL, true, true);
		}
		else {
			// no new codeblock symbol
			checkClearLvalSyms(comp);
		}

		// set codeblock header properties
		ParseScope *scp = comp->curParseScope();
		if (scp->_scopeType != parseScopeTypePart)
			ERRHANDLERCOMPC->internalError("setAssignCbh: illegal parse scope type");

		if (scp->_cntBlockPart++ >= ICPAR_CBHP_HCNT - 1)
			ERRHANDLERCOMPC->error(ERROR_LVL_NORMAL, "too many code block headers", _loc);

        _expOp = 0;
	}


	/**
	 * declare or check assigned symbols if necessary
	 * @param comp			compiler context
	 * @param locOp			location of assignment operator / NULL: left hand side only
	 * @param localAssign	assign without use of directive
     * @param cbhDef        define code block symbol within code block header
     */
    void ExprParseInfo::assignSymbols(CompilerContext *comp, LocationInfo *locOp, bool localAssign, bool cbhDef)
	{
        // general error checks
		// check max count of lvalues
		if (_lvalSym.size() > ICPAR_ASSIGN_CNTLHS) {
			ERRHANDLERCOMPC->error(ERROR_LVL_NORMAL, "to much symbols on left hand side of an assignment", _loc);
		}

		// "ref" not allowed to indexed lvalue
		if (_assignMod->_defIsRef != assignModLevelUndefined && _assignMod->_isRef && _indexLval) {
			ERRHANDLERCOMPC->error(ERROR_LVL_NORMAL, "within an assignment with 'ref' modificator", _assignMod->_locIsRef, "an indexed expression is not allowed", _loc);
			_assignMod->_defIsRef = assignModLevelUndefined;
		}

		// "extern" not allowed to indexed lvalue
		if (_assignMod->_defIsExtern != assignModLevelUndefined && _assignMod->_isExtern && _indexLval) {
			ERRHANDLERCOMPC->error(ERROR_LVL_NORMAL, "within a definition with 'extern' modificator", _assignMod->_locIsExtern, "an indexed expression is not allowed", _loc);
		}

		// "declare" not allowed to indexed lvalue or for subsymbol
		if (_assignMod->_defIsDeclare != assignModLevelUndefined && _assignMod->_isDeclare && (_indexLval || _subSymLval)) {
			if (_indexLval)
				ERRHANDLERCOMPC->error(ERROR_LVL_NORMAL, "within a definition with 'declare' modificator", _assignMod->_locIsDeclare, "an indexed expression is not allowed", _loc);
			else
				ERRHANDLERCOMPC->error(ERROR_LVL_NORMAL, "within a definition with 'declare' modificator", _assignMod->_locIsDeclare, "the use of a subsymbol is not allowed", _loc);
		}

        // scope for subsymbol can only be public or private
        if (_subSymLval && _assignMod->_defSymScope != assignModLevelUndefined && _assignMod->_symScope != symbolScopeTypePublic && _assignMod->_symScope != symbolScopeTypePrivate) {
            ERRHANDLERCOMPC->error(ERROR_LVL_NORMAL, "invalid scope modificator", _assignMod->_locSymScope, "for subsymbol assignment", _loc);
            _assignMod->_defSymScope = assignModLevelUndefined;
        }

		// codeblock control not with other modificators (save ordered)
		if (_assignMod->_defControlCB != assignModLevelUndefined &&
				(_assignMod->_defObjType != assignModLevelUndefined || _assignMod->_defDataType != assignModLevelUndefined || _assignMod->_defSymScope != assignModLevelUndefined
				 || _assignMod->_defIsConst != assignModLevelUndefined || _assignMod->_defIsNew != assignModLevelUndefined || _assignMod->_defIsExtern != assignModLevelUndefined
                 || _assignMod->_defIsInitial != assignModLevelUndefined || _assignMod->_defIsRef != assignModLevelUndefined || _assignMod->_defIsAssert != assignModLevelUndefined
                 || _assignMod->_defIsDeclare != assignModLevelUndefined || _assignMod->_defIsNocond != assignModLevelUndefined)) {
			ERRHANDLERCOMPC->error(ERROR_LVL_NORMAL, "assignment cannot have codeblock control modificator combined with other modificators", _assignMod->_locControlCB);
			_assignMod->_defControlCB = assignModLevelUndefined;
		}

		// codeblock control "return" not allowed with assignment
		if (_assignMod->_defControlCB != assignModLevelUndefined && _assignMod->_controlCB == modificatorReturn) {
			if (locOp)
				ERRHANDLERCOMPC->error(ERROR_LVL_NORMAL, "assignment", *locOp, "cannot be used with 'return'", _assignMod->_locControlCB);
			else
				ERRHANDLERCOMPC->internalError("assignSymbols: use of 'return' with lvalue");

			_assignMod->_defControlCB = assignModLevelUndefined;
		}
		
		// codeblock control not with more than one lvalue symbol
		if (_assignMod->_defControlCB != assignModLevelUndefined && _lvalSym.size() != 1) {
			ERRHANDLERCOMPC->error(ERROR_LVL_NORMAL, "codeblock control modificator", _assignMod->_locControlCB, "cannot be used with more than one lvalue symbol", _loc);
			_assignMod->_defControlCB = assignModLevelUndefined;
		}

		// "extern" only allowed in combination with "public" and without right hand side
		if (_assignMod->_defIsExtern != assignModLevelUndefined && _assignMod->_isExtern) {
			if (locOp) {
				ERRHANDLERCOMPC->error(ERROR_LVL_NORMAL, "assignment", *locOp, "cannot be used with 'extern' definition", _assignMod->_locIsExtern);
			}
			if (_assignMod->_defSymScope != assignModLevelUndefined && _assignMod->_symScope != symbolScopeTypePublic) {
				ERRHANDLERCOMPC->error(ERROR_LVL_NORMAL, "extern definition", _assignMod->_locIsExtern, "cannot be used with definition scope other than 'public'", _assignMod->_locSymScope);
				_assignMod->_defSymScope = assignModLevelUndefined;
			}
		}

        // "initial" not allowed without right hand side
        if (_assignMod->_defIsInitial != assignModLevelUndefined && _assignMod->_isInitial) {
            if (!locOp) {
                ERRHANDLERCOMPC->error(ERROR_LVL_NORMAL, "assignment with 'initial' must have a right hand side", _assignMod->_locIsInitial);
            }
        }

        // "assert" only allowed without right hand side, and not in combination with "new", "extern", "initial", "ref"
		if (_assignMod->_defIsAssert != assignModLevelUndefined && _assignMod->_isAssert) {
			if (locOp) {
				ERRHANDLERCOMPC->error(ERROR_LVL_NORMAL, "assignment", *locOp, "cannot be used with 'assert'", _assignMod->_locIsAssert);
			}

			if (_assignMod->_defIsNew != assignModLevelUndefined && _assignMod->_isNew) {
				ERRHANDLERCOMPC->error(ERROR_LVL_NORMAL, "'assert'", _assignMod->_locIsAssert, "cannot be used with 'new' modificator", _assignMod->_locIsNew);
				_assignMod->_defIsAssert = assignModLevelUndefined;
			}
            if (_assignMod->_defIsInitial != assignModLevelUndefined && _assignMod->_isInitial) {
                ERRHANDLERCOMPC->error(ERROR_LVL_NORMAL, "'assert'", _assignMod->_locIsAssert, "cannot be used with 'initial' modificator", _assignMod->_locIsInitial);
                _assignMod->_defIsAssert = assignModLevelUndefined;
            }
            if (_assignMod->_defIsRef != assignModLevelUndefined && _assignMod->_isRef) {
				ERRHANDLERCOMPC->error(ERROR_LVL_NORMAL, "'assert'", _assignMod->_locIsAssert, "cannot be used with 'ref' modificator", _assignMod->_locIsRef);
				_assignMod->_defIsAssert = assignModLevelUndefined;
			}
			if (_assignMod->_defIsExtern != assignModLevelUndefined && _assignMod->_isExtern) {
				ERRHANDLERCOMPC->error(ERROR_LVL_NORMAL, "'assert'", _assignMod->_locIsAssert, "cannot be used with 'extern' modificator", _assignMod->_locIsExtern);
				_assignMod->_defIsAssert = assignModLevelUndefined;
			}
        }

        // "declare" only allowed without right hand side, and not in combination with "new", "extern", "initial", "ref"
		if (_assignMod->_defIsDeclare != assignModLevelUndefined && _assignMod->_isDeclare) {
			if (locOp) {
				ERRHANDLERCOMPC->error(ERROR_LVL_NORMAL, "assignment", *locOp, "cannot be used with 'declare'", _assignMod->_locIsDeclare);
			}

			if (_assignMod->_defIsNew != assignModLevelUndefined && _assignMod->_isNew) {
				ERRHANDLERCOMPC->error(ERROR_LVL_NORMAL, "'declare'", _assignMod->_locIsDeclare, "cannot be used with 'new' modificator", _assignMod->_locIsNew);
				_assignMod->_defIsDeclare = assignModLevelUndefined;
			}
            if (_assignMod->_defIsInitial != assignModLevelUndefined && _assignMod->_isInitial) {
                ERRHANDLERCOMPC->error(ERROR_LVL_NORMAL, "'declare'", _assignMod->_locIsDeclare, "cannot be used with 'initial' modificator", _assignMod->_locIsInitial);
                _assignMod->_defIsDeclare = assignModLevelUndefined;
            }
            if (_assignMod->_defIsRef != assignModLevelUndefined && _assignMod->_isRef) {
				ERRHANDLERCOMPC->error(ERROR_LVL_NORMAL, "'declare'", _assignMod->_locIsDeclare, "cannot be used with 'ref' modificator", _assignMod->_locIsRef);
				_assignMod->_defIsDeclare = assignModLevelUndefined;
			}
			if (_assignMod->_defIsExtern != assignModLevelUndefined && _assignMod->_isExtern) {
				ERRHANDLERCOMPC->error(ERROR_LVL_NORMAL, "'declare'", _assignMod->_locIsDeclare, "cannot be used with 'extern' modificator", _assignMod->_locIsExtern);
				_assignMod->_defIsDeclare = assignModLevelUndefined;
			}
        }

        // "nocond" not allowed without right hand side
        if (_assignMod->_defIsNocond != assignModLevelUndefined && _assignMod->_isNocond) {
            if (!locOp) {
                ERRHANDLERCOMPC->error(ERROR_LVL_NORMAL, "assignment with 'nocond' must have a right hand side", _assignMod->_locIsNocond);
            }
        }


		// assign symbols
		ParseScope *scp = comp->curParseScope();
		unsigned dl = comp->curParseScopeLevel();
		vector<SymbolLoc *>::iterator it, it2;
		SymbolScopeType sst;
		SpecialSymbol sp;
		bool ro, rep, nd;
        unsigned pnsubAddr = 0;

		for (it = _lvalSym.begin(); it != _lvalSym.end(); it++) {
			SymbolLoc *l = *it;
			SymbolDef *s = l->sym();
			const char *nms = l->contSub();

			rep = nd = false;
			for (it2 = _lvalSym.begin(); it2 != it && !rep; it2++) {
				if (s->innerst() == (*it2)->sym()->innerst())
					rep = true;
			}
			
			if (s->checkDefSymbol(comp, scp, _assignMod, localAssign, rep, dl, nms, sst, ro, sp, l->loc(), _loc)) {
				if (sst != symbolScopeTypeUndefined) {
					// define symbol
                    if (_cbhExpr && _sym)
						sp = specialSymbolNameOnly;

					SymbolDef *t = s->declSymbolEntry(sst, dl);
					t = t->declSymbol(comp, l->loc(), sst, dl, ro, sp);

					if (t != s)
						l->setSym(t);

					if (_cbhExpr && !scp->_nameSymId)
						scp->_nameSymId = t->defId();

					nd = true;
				}
				else if (rep) {
					l->setSym((*it2)->sym());
					nd = true;
				}
				else if (_assignMod->_defIsDeclare != assignModLevelUndefined && _assignMod->_isDeclare && _assignMod->_defIsConst != assignModLevelUndefined && _assignMod->_isConst && !s->readOnly()) {
					s->setReadOnly();
				}

                // change FETCH to FETCH_LVALUE in the generated code; but if symbol is only name then replace the fetch command by a boolean true
                if (sp == specialSymbolNameOnly) {
                    comp->compReplaceFetchByLit(l->codeAddr(), 1);
                }
                else {
                    comp->compChangeFetchLvalue(l->codeAddr(), l->sym()->defId(), (l->sym()->subsymbol() ? 0 : (l->sym()->codeRefId())), (nd && !l->sym()->global()), cbhDef);

                    if (l->sym()->subsymbol() && (_assignMod->_defSymScope != assignModLevelUndefined || (_assignMod->_defIsNew != assignModLevelUndefined && _assignMod->_isNew)))
                        pnsubAddr = comp->compChangeFetchSubLval(l->codeAddr(), pnsubAddr,
                                                           (_assignMod->_defIsNew != assignModLevelUndefined && _assignMod->_isNew),
                                                           (_assignMod->_defSymScope != assignModLevelUndefined && _assignMod->_symScope == symbolScopeTypePublic),
                                                           (_assignMod->_defSymScope != assignModLevelUndefined && _assignMod->_symScope == symbolScopeTypePrivate));
                }

				if (_assignMod->_defControlCB != assignModLevelUndefined) {
					ParseScope *dp = comp->parseScopeAt(s->defLevel());
					if (s->defId() != dp->_nameSymId)
						ERRHANDLERCOMPC->error(ERROR_LVL_NORMAL, comp->ctrl()->printBuffer("symbol '%s' doesn't reference any codeblock", s->name()), l->loc());

					_lvlControlCB = scp->_blockLevel - dp->_blockLevel;
					if (_lvlControlCB >= ICPAR_CBC_CNTLVL)
						ERRHANDLERCOMPC->error(ERROR_LVL_NORMAL, "codeblock control command over to many nested code blocks", l->loc());
				}
			}
		}

		// store next code address
		_codeAddrNext = comp->curCodeAddr();
	}


	/**
	 * assign special meaning to const defined lvalue symbols of the expression
	 * @param sp            special meaning for the symbols
	 */
	void ExprParseInfo::assignSpecialToConst(SpecialSymbol sp)
	{
		// only do anything for lvalue expression without indizes, and not for an objecttype other than par
		if (!_lval || _indexLval || (_assignMod->_defObjType != assignModLevelUndefined && _assignMod->_objType->special() != specialSymbolObjTypePar))
			return;

		// if assign modifiers contain a data type, then sp is altered
		if (_assignMod->_defDataType != assignModLevelUndefined) {
			if (_assignMod->_dataType->special() == specialSymbolDataTypeType)
				sp = (SPECIALSYMBOL_IS_DATATYPE(sp) ? sp : specialSymbolDataType);
			else if (_assignMod->_dataType->special() == specialSymbolDataTypeObj)
				sp = (SPECIALSYMBOL_IS_OBJTYPE(sp) ? sp : specialSymbolObjType);
			else
				sp = (sp == specialSymbolNo ? sp : specialSymbolKnownVal);
		}

		// only assign special meaning if any
		if (sp != specialSymbolNo) {	
			for (vector<SymbolLoc *>::iterator it = _lvalSym.begin(); it != _lvalSym.end(); it++) {
				SymbolLoc *l = *it;
				SymbolDef *s = l->sym();

				if (s->visible() && s->readOnly() && !s->subsymbol())
					s->setSpecial(sp);
			}
		}
	}


	/**
	 * add to this expression a second expression by an operation
	 * @param comp			compiler context, only for error handling
	 * @param op			operation
	 * @param e2            second expression / NULL: no second operand
	 * @param free2			free second expression
	 * @return				this
	 */
	ExprParseInfo *ExprParseInfo::oper(CompilerContext *comp, int op, ExprParseInfo *e2, bool free2)
	{
		PROTO_MOD_OUTL(comp, " ExprParseInfo::oper: " << op << ", " << (e2 ? 2 : 1));

		checkLNPError(comp);

		_sym = NULL;

		if (e2) {
            if (op == EXPR_INFO_OP_CBHCAT || (_cbhExpr && op == EXPR_INFO_OP_LOG_AND)) {
                if (op == EXPR_INFO_OP_LOG_AND) {
                    _cbhValid = ((_cbhValid && e2->_cbhValid) || (_cbhValid && !e2->_lval) || (e2->_cbhValid && !_lval));
                    if (!_cbhValid) {
                        checkClearLvalSyms(comp);
                        e2->checkClearLvalSyms(comp);
                    }
                }
                else {
                    _cbhValid = false;
                }
				_lval = false;
                _special = (SPECIALSYMBOL_KNOWN_VALUE(_special) && SPECIALSYMBOL_KNOWN_VALUE(e2->_special) && op == EXPR_INFO_OP_LOG_AND ? specialSymbolKnownVal : specialSymbolNo);
			}
            else if (_cbhExpr && (op == EXPR_INFO_OP_IN || op == EXPR_INFO_OP_CMP || op == EXPR_INFO_OP_CMP2)) {
                if (_expOp == EXPR_INFO_OP_IN || _expOp == EXPR_INFO_OP_CMP || _expOp == EXPR_INFO_OP_CMP2)
                    ERRHANDLERCOMPC->error(ERROR_LVL_NORMAL, "continued compare operation is not allowed in code block header", e2->_loc);

				if (_lval && !e2->_lval && !e2->_cbhValid) {
                    _cbhValid = true;
				}
				else {
					checkClearLvalSyms(comp);
					e2->checkClearLvalSyms(comp);
				}
				_lval = false;
				_special = specialSymbolNo;
				_assignOp = (op == EXPR_INFO_OP_IN ? "in" : ":=");
			}
			else if (op == EXPR_INFO_OP_NUM || op == EXPR_INFO_OP_IN || op == EXPR_INFO_OP_SET || op == EXPR_INFO_OP_LOG || op == EXPR_INFO_OP_LOG_AND || op == EXPR_INFO_OP_CMP || op == EXPR_INFO_OP_CMP2) {
                if ((_expOp == EXPR_INFO_OP_CMP || _expOp == EXPR_INFO_OP_CMP2) && (op == EXPR_INFO_OP_CMP || op == EXPR_INFO_OP_CMP2)) {
                    if (!comp->checkOnly())
                        comp->compSetCompareCont(_codeAddrNext);
                    if (_expOp == EXPR_INFO_OP_CMP2 || op == EXPR_INFO_OP_CMP2)
                        ERRHANDLERCOMPC->error(ERROR_LVL_NORMAL, "continued compare operation is not allowed for '==' or '!='", e2->_loc);
                }

				_lval = false;
				_cbhValid = false;
				_special = (op == EXPR_INFO_OP_IN || op == EXPR_INFO_OP_SET || op == EXPR_INFO_OP_CMP2 || (SPECIALSYMBOL_KNOWN_VALUE(_special) && SPECIALSYMBOL_KNOWN_VALUE(e2->_special)) ? specialSymbolKnownVal : specialSymbolNo);
				checkClearLvalSyms(comp);
				e2->checkClearLvalSyms(comp);
			}
			else if (op == EXPR_INFO_OP_OBJTO) {
				_lval = false;
				_cbhValid = false;
				_special = (SPECIALSYMBOL_KNOWN_VALUE(_special) ? specialSymbolKnownVal : specialSymbolNo);
				checkClearLvalSyms(comp);
				e2->checkClearLvalSyms(comp);
			}
			else if (op == EXPR_INFO_OP_SUB) {
                if (!_cbhExpr && _lval && e2->_lval && _lvalSym.size() == 1 && e2->_lvalSym.size() == 1 && !_indexLval && !e2->_indexLval)
                    e2->_lvalSym[0]->setContSub(_lvalSym[0]);

				checkClearLvalSyms(comp);
				//if (!e2->_sym)
				//	ERRHANDLERCOMPC->error(ERROR_LVL_NORMAL, "right operand of subsymbol operation must be a symbol name", e2->_loc);

				_special = e2->_special;
				_cbhValid = false;

				if (_cbhExpr) {
					_lval = false;
					e2->checkClearLvalSyms(comp);
				}
				else {
					_lval = e2->_lval;
					_indexLval = e2->_indexLval;
					_subSymLval = true;
					if (_lval)
						moveSymbolLocs(_lvalSym, e2->_lvalSym);
				}
			}
			else if (op == EXPR_INFO_OP_IND) {
				if (_cbhExpr) {
					_lval = false;
					checkClearLvalSyms(comp);
				}
				else if (_lval)
					_indexLval = true;

				_cbhValid = false;
				e2->checkClearLvalSyms(comp);
			}
			else if (op == EXPR_INFO_OP_CALL) {
				_lval = false;
				_cbhValid = false;

				if (_special == specialSymbolDataTypeType || _special == specialSymbolFuncDataType)
					_special = specialSymbolDataType;
				else if (_special == specialSymbolDataTypeObj || _special == specialSymbolFuncObjType)
					_special = specialSymbolObjType;
				else if (SPECIALSYMBOL_FUNC_KNOWN_VALUE(_special, SPECIALSYMBOL_KNOWN_VALUE(e2->_special)))
					_special = specialSymbolKnownVal;
				else
					_special = specialSymbolNo;

				checkClearLvalSyms(comp);
				e2->checkClearLvalSyms(comp);
			}
			else if (op == EXPR_INFO_OP_CAT) {
				_listCnt = (_listCnt ?: 1) + (e2->_listCnt ?: 1);
				_empty = e2->_empty;

				if (_cbhExpr) {
					_lval = _lval || e2->_lval;
					_indexLval = false;
				}
				else {
					_lval = _lval && e2->_lval;
					_indexLval = _indexLval || e2->_indexLval;
					_subSymLval = _subSymLval || e2->_subSymLval;
				}

				_cbhValid = false;
				_special = (SPECIALSYMBOL_KNOWN_VALUE(_special) && SPECIALSYMBOL_KNOWN_VALUE(e2->_special) ? specialSymbolKnownVal : specialSymbolNo);

				if (_lval)
					moveSymbolLocs(_lvalSym, e2->_lvalSym);
				else {
					checkClearLvalSyms(comp);
					e2->checkClearLvalSyms(comp);
				}
			}

			_volaExp = (_volaExp || e2->_volaExp || op == EXPR_INFO_OP_CALL);
            _ncSymAccess = (_ncSymAccess || e2->_ncSymAccess || op == EXPR_INFO_OP_SUB || (op == EXPR_INFO_OP_CALL && !_special));
            _cbSymAccess = (_cbSymAccess || e2->_cbSymAccess);
		}
		else {
			if (op == EXPR_INFO_OP_NUM || op == EXPR_INFO_OP_LOG) {
				_lval = false;
				_cbhValid = false;
				_special = (SPECIALSYMBOL_KNOWN_VALUE(_special) ? specialSymbolKnownVal : specialSymbolNo);
				checkClearLvalSyms(comp);
			}
			else if (op == EXPR_INFO_OP_ARR) {
				_oneElemArr = (!_empty && !_listCnt);
				_listCnt = 0;
				_empty = false;
				_lval = false;
				_cbhValid = false;
				checkClearLvalSyms(comp);
			}
			else if (op == EXPR_INFO_OP_INC) {
                if (_lvalSym.size() != 1)
                    ERRHANDLERCOMPC->error(ERROR_LVL_NORMAL, "increment/decrement not direct adjacent to a symbol name", _loc);
                else if (_lvalSym[0]->sym()->readOnly())
                    ERRHANDLERCOMPC->error(ERROR_LVL_NORMAL, "increment/decrement for constant symbol", _loc);

				_lval = false;
				_cbhValid = false;
				checkClearLvalSyms(comp);
			}
			else if (op == EXPR_INFO_OP_TUP || op == EXPR_INFO_OP_SET) {
				if (!_cbhExpr || op == EXPR_INFO_OP_SET) {
					_lval = false;
					checkClearLvalSyms(comp);
				}

				_listCnt = 0;
				_empty = false;
				_cbhValid = false;
				_special = specialSymbolKnownVal;
			}
		}
		
		if (e2) {
			_loc = _loc + e2->_loc;

			if (free2)
				delete e2;
		}

        _expOp = op;
        _codeAddrNext = comp->curCodeAddr();

		return this;
	}


	/**
	 * symbols in _lvalSym are not used as lvalue, so check for definition und visibility
	 * @param comp			compiler context, only for error handling
	 * @param check			check symbols for definition und visibility
	 * @param clear			clear _lvalSym
	 */
	void ExprParseInfo::checkClearLvalSyms(CompilerContext *comp, bool check, bool clear)
	{
		if (_lvalSym.size() > 0) {
			for (vector<SymbolLoc *>::iterator it = _lvalSym.begin(); it != _lvalSym.end(); it++) {
				SymbolLoc *l = *it;

				if (check) {
					const char *err = NULL;
					if (l->mark() == markForSymbolNameCB)
						err = "invalid use of codeblock symbol mark '@' for symbol '%s'";
					else if (!(l->sym()->visible()))
						err = (l->sym()->notDefined() ? "symbol '%s' is not defined" : "symbol '%s' is not accessible");
					else if (SPECIALSYMBOL_NO_VALUE(l->sym()->special()))
						err = "symbol '%s' cannot be used in expressions because it is only a codeblock reference name";

					if (err)
						ERRHANDLERCOMPC->error(ERROR_LVL_NORMAL, comp->ctrl()->printBuffer(err, l->sym()->name()), l->loc());
				}

				if (clear)
					delete l;
			}
		}

		if (clear)
			_lvalSym.clear();
	}

	/**
	 * check if expression has a line name prefix and then print an error message
	 * @param comp			compiler context, only for error handling
	 */
	void ExprParseInfo::checkLNPError(CompilerContext *comp)
	{
		if (_hasLNP) {
			// this error appears if in an expression like "f { ... }" the function name f is not defined and treated as a line name prefix, but then the expression is not used as separate statement.
			// the error is probably that f is meant as function name but not defined by mistake.
			ERRHANDLERCOMPC->error(ERROR_LVL_NORMAL, "unknown function name, or line name prefix not allowed in expression", _locLNP);
		}
	}

	/**
	 * check if expression is an code block header lvalue, and the compatibility mode for assign operator in code block header is set
	 * @param comp			compiler context, only for error handling
	 * @param loc			location of assign operator
	 */
	void ExprParseInfo::checkCbhAssignError(CompilerContext *comp, LocationInfo& loc)
	{
		if (!comp->modpt()->compatCBHAssign())
			ERRHANDLERCOMPC->error(ERROR_LVL_NORMAL, "assign operator not allowed in code block header expression", loc);

		else if (!_lval)
			ERRHANDLERCOMPC->error(ERROR_LVL_NORMAL, "expression is not a code block header lvalue expression", _loc);
	}

	/**
	 * change FETCH command for lvalue symbols to FETCH_LVALUE
	 * @param comp			compiler context
	 */
	void ExprParseInfo::compChangeFetchLvalue(CompilerContext *comp)
	{
		for (vector<SymbolLoc *>::iterator it = _lvalSym.begin(); it != _lvalSym.end(); it++) {
            comp->compChangeFetchLvalue((*it)->codeAddr(), 0, 0, false, false);
		}
	}

	/**
	 * change FETCH command for lvalue symbols to NOP
	 * @param comp			compiler context
	 */
	void ExprParseInfo::compChangeFetchNop(CompilerContext *comp)
	{
		for (vector<SymbolLoc *>::iterator it = _lvalSym.begin(); it != _lvalSym.end(); it++) {
			comp->compNop(NULL, (*it)->codeAddr(), INTCODE_FETCH, INTCODE_FETCH_SPECIAL, 0);
		}
	}

	/**
	 * get code address after this expression and a set of the lvalue symbol numbers
	 * @param symnrs		set for filling the lvalue symbol numbers in
	 * @return				code address after this expression
	 */
	unsigned ExprParseInfo::codeAddrNextAndSymbols(set<unsigned>& symnrs)
	{
		for (vector<SymbolLoc *>::iterator it = _lvalSym.begin(); it != _lvalSym.end(); it++) {
			symnrs.insert((*it)->sym()->defId());
		}

		return _codeAddrNext;
	}
}

