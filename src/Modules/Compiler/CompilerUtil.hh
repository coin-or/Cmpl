
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

/**
 * This file defines utility classes and enums used in the class <code>CompilerContext</code>.
 * This is done by using flex and bison to build the scanner and the parser for the input file.
 */

#ifndef COMPILERUTIL_HH
#define COMPILERUTIL_HH

#include <vector>
#include <set>

#include "../../Control/LocationInfo.hh"
#include "../../Control/StringStore.hh"
#include "Compiler.hh"


namespace cmpl
{
	class Compiler;
	class CompilerContext;
	class ExprParseInfo;
	class SymbolInfo;
	class SymbolTab;
	class SymbolDef;
	class MainData;
	class SyntaxElement;


	/**
	 * type of semantic value on bison parse stack
	 */
	enum SemanticValueType {
		semanticValueExpr,					///< symbol / expression
		semanticValueElse,					///< other value or no value
	};

	/**
	 * type of previous scanned token
	 */
	enum ScanPrevTokenType {
		scanPrevTokenStart,					///< no previous token
		scanPrevTokenDefSymbol,				///< previous token is an already defined symbol
		scanPrevTokenUndefSymbol,			///< previous token is an undefined symbol
		scanPrevTokenCBRef,					///< previous token is the codeblock reference operator
		scanPrevTokenCBRefPure,				///< previous token is the codeblock reference operator for pure function
		scanPrevTokenColon,					///< previous token is the colon
		scanPrevTokenElse,					///< other previous token
	};

	/**
	 * type of parse scope
	 */
	enum ParseScopeType {
		parseScopeTypeBasis,				///< basis scope outside of all codeblocks
		parseScopeTypeBlock,				///< scope of a direct codeblock
		parseScopeTypeFunc,					///< scope of a function codeblock
		parseScopeTypePure,					///< scope of a pure function codeblock
		parseScopeTypePart,					///< scope of a part in a codeblock
		parseScopeTypeBody,					///< scope of the body of a part in a codeblock
	};

	/**
	 * mode for code block for distinction between expression and statement
	 */
	enum CodeBlockExprMode {
		codeBlockExprModeStatementStart,	///< code block starts a new statement
		codeBlockExprModeCBBodyStart,		///< code block starts a code block body
		codeBlockExprModeElse,				///< code block stands at another position
	};

	/**
	 * type of definition scope of a symbol
	 */
	enum SymbolScopeType {
		symbolScopeTypeUndefined,			///< symbol not defined yet
		symbolScopeTypePublic,				///< public symbol
		symbolScopeTypePrivate,				///< private symbol
		symbolScopeTypeLocal,				///< local symbol
		symbolScopeTypeCodeBlock,			///< symbol defined in a codeblock header
	};

	/**
	 * type of marking of a symbol name
	 */
	enum MarkForSymbolName {
		markForSymbolNameNo,				///< no marking
		markForSymbolNameReg,				///< mark as regular symbol (by '\')
		markForSymbolNameCB,				///< mark as free codeblock symbol
	};

	/**
	 * modificators
	 */
	enum ModificatorKey
	{
        modificatorDummy,                   ///< dummy value only for initialization
		modificatorPublic,					///< modificator keyword "public"
		modificatorPrivate,					///< modificator keyword "private"
		modificatorLocal,					///< modificator keyword "local"
		modificatorConst,					///< modificator keyword "const"
		modificatorNew,						///< modificator keyword "new"
        modificatorExtern,					///< modificator keyword "extern"
        modificatorInitial,					///< modificator keyword "initial"
        modificatorRef,						///< modificator keyword "ref"
		modificatorAssert,					///< modificator keyword "assert"
		modificatorDeclare,					///< modificator keyword "declare"
        modificatorNocond,					///< modificator keyword "nocond"
        modificatorOrdered,					///< modificator keyword "ordered"
		modificatorBreak,					///< modificator keyword "break"
		modificatorContinue,				///< modificator keyword "continue"
		modificatorRepeat,					///< modificator keyword "repeat"
		modificatorReturn,					///< modificator keyword "return"
	};

	/**
	 * setting level of a single assign modificator.
	 */
	enum AssignModLevel {
		assignModLevelUndefined,			///< not set yet
		assignModLevelDirective,			///< set in the directive
		assignModLevelAssign,				///< set in the assign
	};

	/**
	 * special meanings which can be associated with symbols
	 */
	enum SpecialSymbol {
		specialSymbolNo 			= 0x00,		///< no special meaning

		specialSymbolDataType		= 0x10,		///< symbol represents a data type
		specialSymbolDataTypeType	= 0x12,		///< symbol represents the data type of a data type
		specialSymbolDataTypeObj	= 0x13,		///< symbol represents the data type of an object type

		specialSymbolObjType		= 0x14,		///< symbol represents an object type
		specialSymbolObjTypePar		= 0x15,		///< symbol represents the object type par

		specialSymbolFuncDataType	= 0x1a,		///< function value of symbol is a data type
		specialSymbolFuncObjType	= 0x1b,		///< function value of symbol is an object type

		specialSymbolKnownVal		= 0x30,		///< symbol contains a value that is known while executing cmpl (par value but no formula)
		specialSymbolFuncKnownVal	= 0x31,		///< function value of symbol contains a value that is known while executing cmpl

		specialSymbolFuncEqKnownVal = 0x38,		///< if all function arguments are known values, then the function value of symbol contains a value that is known while executing cmpl
		specialSymbolOptDirMin		= 0x3c,		///< symbol represents optimization for minimum
		specialSymbolOptDirMax		= 0x3d,		///< symbol represents optimization for maximum

        specialSymbolNamespace      = 0x40,     ///< symbol is a namespace name

        specialSymbolNameOnly		= 0xf0,		///< symbol has no value but acts only as name
	};

	// test whether SpecialSymbol can be part of modificators
	#define SPECIALSYMBOL_IS_MODIFICATOR(s)		((((unsigned)(s)) & 0xf8) == 0x10)

	// test whether SpecialSymbol represents a data type
	#define SPECIALSYMBOL_IS_DATATYPE(s)		((((unsigned)(s)) & 0xfc) == 0x10)

	// test whether SpecialSymbol represents an object type
	#define SPECIALSYMBOL_IS_OBJTYPE(s)		((((unsigned)(s)) & 0xfc) == 0x14)

	// test whether SpecialSymbol represents an optimization direction
    #define SPECIALSYMBOL_IS_OPTDIR(s)			((((unsigned)(s)) & 0xfc) == 0x3c)

	// test whether SpecialSymbol has no value
	#define SPECIALSYMBOL_NO_VALUE(s)			((((unsigned)(s)) & 0xf0) == 0xf0)

	// test whether SpecialSymbol contains a known value while executing cmpl: all special symbols are known values, save symbols with no value
	#define SPECIALSYMBOL_KNOWN_VALUE(s)		((unsigned)(s) != 0 && !SPECIALSYMBOL_NO_VALUE(s))

	// test whether a function call on symbol <code>s</code> is a known value, under the condition <code>a</code> that all function arguments are known values
	#define SPECIALSYMBOL_FUNC_KNOWN_VALUE(s,a)	((s)==specialSymbolFuncKnownVal || ((((unsigned)(s)) & 0xf2) == 0x12) || (((((unsigned)(s)) & 0xf8) == 0x38) && (a)))


	/**
	 * class for modificators of an assign or of a directive.
	 */
	class AssignModificators
	{
		friend class SymbolDef;
		friend class ExprParseInfo;

	private:
		AssignModLevel _defObjType;			///< setting of object type
		LocationInfo _locObjType;			///< location of object type
		SymbolDef *_objType;				///< symbol of object type

		AssignModLevel _defDataType;		///< setting of data type
		LocationInfo _locDataType;			///< location of data type
		SymbolDef *_dataType;				///< symbol of data type

		AssignModLevel _defSymScope;		///< setting of symbol scope type
		LocationInfo _locSymScope;			///< location of symbol scope type
		SymbolScopeType _symScope;			///< symbol scope type (public, private or local)

		AssignModLevel _defIsConst;			///< setting of const or not
		LocationInfo _locIsConst;			///< location of const or not
		bool _isConst;						///< const or not

		AssignModLevel _defIsNew;			///< setting of new symbol or not
		LocationInfo _locIsNew;				///< location of new symbol or not
		bool _isNew;						///< new symbol or not

		AssignModLevel _defIsExtern;		///< setting of extern symbol or not
		LocationInfo _locIsExtern;			///< location of extern symbol or not
		bool _isExtern;						///< extern symbol or not

        AssignModLevel _defIsInitial;		///< setting of initial assignment or not
        LocationInfo _locIsInitial;			///< location of initial assignment or not
        bool _isInitial;					///< initial assignment or not

        AssignModLevel _defIsRef;			///< setting of ref assignment or not
		LocationInfo _locIsRef;				///< location of ref assignment or not
		bool _isRef;						///< ref assignment or not

		AssignModLevel _defIsAssert;		///< setting of assert symbol definition or not
		LocationInfo _locIsAssert;			///< location of assert symbol definition or not
		bool _isAssert;						///< assert symbol definition or not

		AssignModLevel _defIsDeclare;		///< setting of declare symbol definition or not
		LocationInfo _locIsDeclare;			///< location of declare symbol definition or not
		bool _isDeclare;					///< declare symbol definition or not

        AssignModLevel _defIsNocond;		///< setting of nocond assignment or not
        LocationInfo _locIsNocond;			///< location of nocond assignment or not
        bool _isNocond;                     ///< nocond assignment or not

		AssignModLevel _defIsOrdered;		///< setting of ordered assign or not
		LocationInfo _locIsOrdered;			///< location of ordered assign or not
		bool _isOrdered;					///< ordered assign or not

		AssignModLevel _defControlCB;		///< setting of codeblock control
		LocationInfo _locControlCB;			///< location of codeblock control
        ModificatorKey _controlCB;          ///< codeblock control (only modificatorBreak, modificatorContinue, modificatorRepeat or modificatorReturn)

	public:
		/**
		 * constructor
		 */
		inline AssignModificators()			{ init(); }
		
		/**
		 * initialize
		 */
        inline void init()					{ _defObjType = assignModLevelUndefined; _defDataType = assignModLevelUndefined; _defSymScope = assignModLevelUndefined; _defIsConst = assignModLevelUndefined; _defIsNew = assignModLevelUndefined; _defIsExtern = assignModLevelUndefined; _defIsInitial = assignModLevelUndefined;  _defIsRef = assignModLevelUndefined; _defIsAssert = assignModLevelUndefined; _defIsDeclare = assignModLevelUndefined; _defIsNocond = assignModLevelUndefined; _defIsOrdered = assignModLevelUndefined; _defControlCB = assignModLevelUndefined; _controlCB = modificatorDummy; }

		/**
		 * set assign modificators for use with codeblock symbols
		 */
		inline void setForCb()				{ init(); _defSymScope = _defIsConst = _defIsNew = assignModLevelAssign; _symScope = symbolScopeTypeCodeBlock; _isConst = _isNew = true; }

		/**
		 * test whether nothing is set
		 * @param so			don't check for object type
		 */
        inline bool empty(bool so = false)	{ return ((_defObjType == assignModLevelUndefined || so) && _defDataType == assignModLevelUndefined && _defSymScope == assignModLevelUndefined && _defIsConst == assignModLevelUndefined && _defIsNew == assignModLevelUndefined && _defIsExtern == assignModLevelUndefined && _defIsInitial == assignModLevelUndefined && _defIsRef == assignModLevelUndefined && _defIsAssert == assignModLevelUndefined && _defIsDeclare == assignModLevelUndefined && _defIsNocond == assignModLevelUndefined && _defIsOrdered == assignModLevelUndefined && _defControlCB == assignModLevelUndefined); }

		/**
		 * set a modificator.
		 * @param comp			compiler context object, only for error handling
		 * @param loc           location, only for error handling
		 * @param mod			modificator (only used if odType is NULL)
		 * @param odType		symbol for object type or data type
		 * @param isObjType		odType is object type
		 */
		void setModificator(CompilerContext *comp, LocationInfo *loc, ModificatorKey mod, SymbolDef *odType, bool isObjType);

		/**
		 * add modificators from another modificators object.
		 * @param mod			other modificators object
		 */
		void addModificators(AssignModificators& mod);

		/**
		 * change level for set assign modificators
		 * @param comp			compiler context object, only for error handling
		 * @param loc           location, only for error handling
		 * @param lvl			dest level
		 */
		void chgAssignModLevel(CompilerContext *comp, LocationInfo *loc, AssignModLevel lvl);

		/**
		 * get whether assign modificators indicate a codeblock control assignment
		 * @param comp			compiler context object, only for error handling
		 * @param loc           location, only for error handling
		 * @param cb			return codeblock control (if true, then only modificatorBreak, modificatorContinue, modificatorRepeat or modificatorReturn)
		 */
        bool isControlCBAssign(CompilerContext *comp, LocationInfo *loc, ModificatorKey& cb);

#ifdef PROTO
		/**
		 * write AssignModificators to stream
		 */
		friend ostream& operator<< (ostream& ostr, const AssignModificators& mod);
#endif //PROTO
	};


	/**
	 * context info for a parse scope.
	 */
	class ParseScope
	{
		friend class CompilerContext;
		friend class ExprParseInfo;
		friend class SymbolDef;

	private:
		CompilerContext *_comp;						///< parse context
		unsigned _level;							///< level of this scope within the parse context
		ParseScopeType _scopeType;					///< type of parse scope
		CodeBlockExprMode _cbExprMode;				///< mode for code block for distinction between expression and statement
		ParseScopeType _inFuncDef;					///< whether scope is within a function definition scope (only parseScopeTypeBasis, parseScopeTypeFunc or parseScopeTypePure)
        unsigned _codeStart;                        ///< code position of the start of this parse scope

		ParseScope *_outerFunc;						///< outer function scope or basis scope
		unsigned _localSymCur;						///< function scope or basis scope: current index for next local symbol; other scope: local symbol index at start of the scope
		unsigned _localSymMax;						///< maximum index for local symbols (only used in a function scope or basis scope)
		bool _hasSubSymbol;							///< within the scope a subsymbol is referenced (only used in a function scope or basis scope)

		ParseScope *_blockScope;					///< pointer to code block scope
		ParseScope *_outerPartScope;				///< pointer to code block part scope of outer code block
		vector<unsigned> *_blockPos;				///< positions of commands for code block within the intermediary code
		vector<unsigned> *_controlCBPos;			///< positions of code block control commands within this code block or code block part
		vector<unsigned> *_cbHeaderPos;				///< position of code block header commands of the previous code block part within this code block
        vector<unsigned> *_subValidPos;             ///< position of assign commands with argument for validity range for subsymbols
		unsigned _blockLevel;						///< level of code block within function
		
		AssignModificators _assignModCurrent;		///< current assign modificators
		AssignModificators _assignModDirective;		///< assign modificators from directive

		unsigned _nameSymId;						///< symbol identificator number of the symbol representing the reference name of a codeblock part (only used if _scopeType == parseScopeTypePart) (0: no such symbol)

		const char *_lineNamePref;					///< line name prefix for next statement
		LocationInfo _locLineNamePref;				///< location of line name prefix for next statement

		SyntaxElement *_elem;						///< syntax element (only used for block part scope)
		unsigned _cntBlockPart;						///< counter for block parts (in code block scope) or part headers (in code block part scope)
		unsigned short _parBlockPart;				///< parameter for code block (in code block scope) or code block part (in code block part scope) intermediary code command (ICPAR_BLOCK_*)

	public:
		/**
		 * constructor
		 * @param comp		parse context
		 * @param lvl		level of this scope within the parse context
		 * @param scopeType	type of parse scope
		 * @param cbm		mode for code block for distinction between expression and statement
		 * @param op		outer parse scope
		 */
		ParseScope(CompilerContext *comp, unsigned lvl, ParseScopeType scopeType, CodeBlockExprMode cbm, ParseScope *op);

		/**
		 * destructor
		 */
        ~ParseScope()									{ if (_blockPos) { delete _blockPos; } if (_controlCBPos) { delete _controlCBPos; } if (_cbHeaderPos) { delete _cbHeaderPos; } if (_subValidPos) { delete _subValidPos; } }

		/**
		 * get whether this scope is the basis scope
		 */
		inline bool isBasisScope()						{ return (_scopeType == parseScopeTypeBasis); }

		/**
		 * get whether this scope is a function scope or the basis scope
		 */
		inline bool isFuncOrBasisScope()				{ return (_scopeType == parseScopeTypeBasis || _scopeType == parseScopeTypeFunc || _scopeType == parseScopeTypePure); }

		/**
		 * get whether this scope is a code block scope
		 */
		inline bool isBlockScope()						{ return (_scopeType == parseScopeTypeBlock || _scopeType == parseScopeTypeFunc || _scopeType == parseScopeTypePure); }

		/**
		 * set that within the scope a subsymbol is referenced, also recursive for outer function scope
		 */
		inline void setHasSubSymbol()					{ if (!_hasSubSymbol) { _hasSubSymbol = true; if (_outerFunc) _outerFunc->setHasSubSymbol(); } }

		/**
		 * get outer function scope or basis scope
		 */
		inline ParseScope *outerFunc()					{ return _outerFunc; }

		/**
		 * set single parameter value within _parBlockPart
		 * @param p			parameter to set (one of ICPAR_BLOCK_*)
		 */
		inline void setParBlockPart(unsigned short p)	{ _parBlockPart |= p; }

		/**
		 * get local symbol index for a new local symbol defined in this scope
		 * @param loc       location, only for error handling
		 */
		inline unsigned newLocalSym(LocationInfo *loc);

		/**
		 * set directive from current assign modificators
		 * @param loc       location, only for error handling
		 */
		void setDirectiveFromCurrent(LocationInfo *loc);

		/**
	 	 * set line name prefix
		 * @param elem		syntax element
		 * @param loc		location, only for error handling
		 * @param lnp		line name prefix
		 */
		void setLineNamePref(SyntaxElement *elem, LocationInfo *loc, const char *lnp);

		/**
		 * end of statement processing
		 * @param elem		syntax element
		 */
		void endOfStatement(SyntaxElement *elem);
	};


	/**
	 * class for the definition info of a symbol.
	 */
	class SymbolDef
	{
		friend class SymbolInfo;

	private:
		const char *_name;						///< symbol name, stored in the string store in _symTab
		unsigned _index;						///< index of the symbol in symbol tab
		unsigned _defId;						///< unique definition number (count from 1)
		unsigned _globId;						///< unique number for global symbol (count from 1) / 0: no global symbol
		unsigned _localInd;						///< index for local symbol within the current function context (only used for local symbol)
		bool _valid;							///< symbol is valid in the current scope (but may be hidden)
		SymbolTab *_symTab;						///< symbol table to which the symbol belongs
		SymbolInfo *_symInfo;					///< persistent symbol info object for this symbol
		SymbolScopeType _scopeType;				///< type of symbol definition scope
		unsigned _defLevel;						///< definition scope level for local or private symbol
		unsigned _hideLevel;					///< symbol is hidden in this scope level / 0: not hidden
		unsigned _hidePure;						///< symbol is hidden because a pure function starts (only used if _hideLevel greater than 0)
		bool _readOnly;							///< symbol cannot be assign destination
		SpecialSymbol _special;					///< special meaning for the symbol

		SymbolDef *_inner;						///< inner scope symbol with the same name (only possible if <code>!_valid</code>)
		SymbolDef *_outer;						///< outer scope symbol with the same name (only possible if _defLevel > 0)

	public:
		/**
		 * constructor
		 * @param name		symbol name
		 * @param symTab	symbol table
		 * @param ind		index of the symbol in symbol tab
		 * @param defId		unique definition number
		 */
        inline SymbolDef(const char *name, SymbolTab *symTab, unsigned index, unsigned defId)	{ _name = name; _index = index; _defId = defId; _globId = 0; _localInd = 0; _valid = false; _symTab = symTab; _symInfo = NULL; _scopeType = symbolScopeTypeUndefined; _defLevel = _hideLevel = 0; _readOnly = false; _special = specialSymbolNo; _inner = _outer = NULL; }

		/**
		 * constructor for inner symbol definition
		 * @param s			outer symbol definition
		 * @param lvl		definition scope level
		 */
		inline SymbolDef(SymbolDef *s, unsigned lvl)								{ _name = s->_name; _index = s->_index; _defId = s->_defId; _globId = s->_globId; _localInd = s->_localInd; _valid = s->_valid; _symTab = s->_symTab; _symInfo = s->_symInfo; _scopeType = s->_scopeType; _defLevel = lvl; _hideLevel = 0; _readOnly = s->_readOnly; _special = s->_special; _inner = NULL; _outer = s; s->_inner = this; }

		/**
		 * get symbol definition entry for declaring symbol as valid
		 * @param st		type of symbol definition scope
		 * @param dl		definition scope level for local symbol
		 * @return          symbol definition to call declSymbol() at
		 */
		SymbolDef *declSymbolEntry(SymbolScopeType st, unsigned dl);

		/**
		 * set _readOnly and _special for a public symbol
		 * @param valid		set symbol as valid
		 * @param ro		read-only symbol
		 * @param special	special meaning for the symbol
		 * @param globId	unique number for global symbol
		 */
		inline void setPublicReadOnlySpecial(bool valid, bool ro, SpecialSymbol sp, unsigned globId)
																					{ _valid = valid; _scopeType = symbolScopeTypePublic; _defLevel = 0; _hideLevel = 0; _readOnly = ro; _special = sp; _globId = globId; }

		/**
		 * declare symbol as valid (only for not predefined symbols)
		 * @param comp      compiler context
		 * @param loc       symbol location
		 * @param st		type of symbol definition scope
		 * @param dl		definition scope level for local symbol
		 * @param ro		read-only symbol
		 * @param sp		special meaning for the symbol
		 * @return          symbol definition declared as valid
		 */
		SymbolDef *declSymbol(CompilerContext *comp, LocationInfo& loc, SymbolScopeType st, unsigned dl, bool ro, SpecialSymbol sp = specialSymbolNo);

		/**
		 * set special meaning for a already declared symbol
		 * @param sp		special meaning for the symbol
		 */
		void setSpecial(SpecialSymbol sp);

		/**
		 * set new definition number
		 * @param defId		unique definition number
		 */
		inline void setNewDefId(unsigned defId)										{ _defId = defId; }

		/**
		 * set new definition number
		 * @param globId	unique number for global symbol
		 */
		inline void setNewGlobId(unsigned globId)									{ _globId = globId; }

		/**
		 * get symbol name (from string store in _symTab)
		 */
		inline const char *name()													{ return _name; }

		/**
		 * get symbol name (from global string store)
		 */
		const char *nameGlob();

		/**
		 * get type of symbol definition scope
		 */
		inline SymbolScopeType scopeType()											{ return _scopeType; }

		/**
		 * get whether symbol with given scope type is global or local
		 */
		inline bool global(SymbolScopeType sst)										{ return (sst == symbolScopeTypePublic || sst == symbolScopeTypePrivate || sst == symbolScopeTypeUndefined); }

		/**
		 * get whether symbol is global or local
		 */
		inline bool global()														{ return global(_scopeType); }

		/**
		 * get unique definition number (count from 1)
		 */
		inline const unsigned defId()												{ return _defId; }

		/**
		 * get unique number for global symbol (count from 1)
		 */
		inline const unsigned globId()												{ return _globId; }

		/**
		 * get index for local symbol within the current function context
		 */
		inline const unsigned localInd()											{ return _localInd; }

		/**
		 * get number for referencing the symbol in the intermediary code (not for subsymbol or for pseudo symbol)
		 */
		inline const unsigned codeRefId()											{ return (global() ? _globId : _localInd); }

		/**
		 * test whether the symbol is a pseudo symbol
		 */
		inline bool pseudoSymbol()													{ return (_name[0] == '$'); }

		/**
		 * get predefined command number for a pseudo symbol
		 */
		unsigned short pseudoSymbolDef();

		/**
		 * get definition scope level for local symbol
		 */
		inline unsigned defLevel()													{ return _defLevel; }

		/**
		 * get whether symbol is hidden in this scope level / 0: not hidden
		 */
		inline unsigned hideLevel()													{ return _hideLevel; }

		/**
		 * get inner scope symbol
		 */
		inline SymbolDef *inner()													{ return _inner; }

		/**
		 * get special meaning for the symbol
		 */
		inline SpecialSymbol special()												{ return _special; }

		/**
		 * return whether symbol is predefined
		 */
		bool predef();

		/**
		 * return whether symbol is valid (but may be hidden in the current scope level)
		 */
		inline bool valid()															{ return _valid; }

		/**
		 * return whether symbol is valid and not hidden in the current scope level
		 */
		inline bool visible()														{ return (_valid && _hideLevel == 0); }

		/**
		 * return whether symbol is undefined in the current scope level
		 */
		inline bool notDefined()													{ return (!_valid || (_hideLevel > 0 && _hideLevel == _hidePure)); }

		/**
		 * return whether the symbol is to hide when entering a function parse scope
		 * @param pure			pure function scope
		 */
		inline bool toHideForFuncScope(bool pure)									{ return (_valid && (_scopeType == symbolScopeTypeLocal || _scopeType == symbolScopeTypeCodeBlock || (_scopeType == symbolScopeTypePublic && pure && !predef()))); }

		/**
		 * get the innerst symbol entry for this symbol
		 */
		inline SymbolDef *innerst()													{ return (!_inner ? this : _inner->innerst()); }

		/**
		 * get whether symbol cannot be assign destination
		 */
		inline bool readOnly()														{ return _readOnly; }

		/**
		 * set symbol to read-only
		 */
		inline void setReadOnly()													{ _readOnly = true; }

		/**
		 * get whether the symbol is a subsymbol
		 */
		bool subsymbol();

		/**
		 * insert symbol in symbol info vector
		 * @param comp          compiler context
         * @param symkey        unique key for symbol, normally symbol defId
		 * @param loc           symbol location
		 * @param predef        predefined symbol value (0: no predefined symbol; >0: pseudo symbol (one of ICS_SPECIAL_*); <0: predefined symbol (one of PREDEFSYMVAL_*)
         * @param predefCont    only for predefined subsymbol: predef of container the subsymbol belongs to
		 */
        void insertSymbolInfo(CompilerContext *comp, unsigned symkey, LocationInfo& loc, int predef, int predefCont);

		/**
		 * set the symbol state after entering a new parse scope
		 * @param lvl			parse scope level
		 * @param pst			parse scope type
		 */
		void enterParseScope(unsigned lvl, ParseScopeType pst);

		/**
		 * set the symbol state before leaving a parse scope
		 * @param lvl			parse scope level
		 * @param pst			parse scope type
		 */
		void leaveParseScope(unsigned lvl, ParseScopeType pst);

		/**
		 * symbol is left hand side of an assignment, check compatibility with its definition or provide it to a new definition
		 * @param comp			compiler object
		 * @param scp			current parse scope
		 * @param am			assign modificators
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
		bool checkDefSymbol(CompilerContext *comp, ParseScope *scp, AssignModificators *am, bool localAssign, bool rep, unsigned dl, const char *nms, SymbolScopeType& sst, bool& ro, SpecialSymbol& sp, LocationInfo& symLoc, LocationInfo& expLoc);
	};


	/**
	 * class for a table of defined symbols.
	 */
	class SymbolTab
	{
	private:
		Compiler *_modp;						///< compiler module
		StringStore *_names;					///< names of defined symbols
		SymbolDef **_symbols;					///< array with definition info for symbols, indexed like _names
		unsigned _cap;							///< capacity of _symbols
		unsigned _len;							///< last used element + 1 in _symbols

		bool _sub;								/// symbol table for subsymbols

	public:
		/**
		 * constructor
		 */
		SymbolTab(Compiler *modp, bool sub)							{ _modp = modp; _names = new StringStore(); _names->setModp(modp, "compiler symbol tab"); _symbols = NULL; _cap = _len = 0; _sub = sub; }

		/**
		 * destructor
		 */
		~SymbolTab();


		/**
		 * get main object
		 */
		inline MainControl *ctrl() const			{ return _modp->ctrl(); }

		/**
		 * get data object
		 */
		inline MainData *data() const				{ return _modp->data(); }

		/**
		 * get this module
		 */
		inline ModuleBase *modp()					{ return _modp; }

		/**
		 * get whether symbol table for subsymbols
		 */
		inline bool sub()							{ return _sub; }

        /**
         * get first symbol (only for error handling, to have a valid symbol pointer)
         */
        inline SymbolDef *first()                   { return *_symbols; }


		/**
		 * search and insert a symbol to the symbol table.
		 * @param str           symbol name
		 * @param free          free original string for symbol name (or don't copy it when inserting, so it is freed in the destructor)
		 * @param insertId		insert symbol name and definition if not found, with this unique definition number / 0: don't insert
		 * @param found         return if symbol is found
		 * @return              pointer to symbol definition
		 */
		SymbolDef *searchInsert(const char *str, bool free, unsigned insertId, bool &found);


		/**
		 * set the symbol state after entering a new parse scope
		 * @param lvl			parse scope level
		 * @param pst			parse scope type
		 */
		inline void enterParseScope(unsigned lvl, ParseScopeType pst)					{ for (unsigned i = 0; i < _len; i++) { _symbols[i]->enterParseScope(lvl, pst); } }

		/**
		 * set the symbol state before leaving a parse scope
		 * @param lvl			parse scope level
		 * @param pst			parse scope type
		 */
		inline void leaveParseScope(unsigned lvl, ParseScopeType pst)					{ for (unsigned i = 0; i < _len; i++) { _symbols[i]->leaveParseScope(lvl, pst); } }

	};


	/**
	 * class for a symbol reference at a location
	 */
	class SymbolLoc
	{
	private:
        SymbolDef *_sym;				///< symbol
        LocationInfo _loc;				///< location
        SyntaxElement *_elem;			///< syntax element of symbol location
        MarkForSymbolName _mark;		///< marking of the symbol
        string _contSub;				///< name of container symbol if symbol is subsymbol in expression without indizes
        unsigned _codeAddr;				///< address of FETCH command in generated code

	public:
		/**
		 * constructor
		 * @param sym			symbol
		 * @param loc			location
		 * @param mark			marking of the symbol
		 */
        inline SymbolLoc(SymbolDef *sym, LocationInfo& loc, MarkForSymbolName mark)			{ _sym = sym; _loc = loc; _elem = NULL; _mark = mark; _codeAddr = 0; }

		/**
		 * get symbol
		 */
		inline SymbolDef *sym()										{ return _sym; }

		/**
		 * set symbol
		 */
		void setSym(SymbolDef *s);

		/**
		 * get location
		 */
		inline LocationInfo& loc()									{ return _loc; }

		/**
		 * get syntax element
		 */
		inline SyntaxElement *syntaxElem()							{ return _elem; }

		/**
		 * get marking of the symbol
		 */
		inline MarkForSymbolName mark()								{ return _mark; }

		/**
		 * get address of FETCH command in generated code
		 */
		inline unsigned codeAddr()									{ return _codeAddr; }

		/**
		 * get name of container symbol
		 * @return		name of container symbol as c-string or NULL if not set
		 */
		inline const char *contSub()								{ return (_contSub.empty() ? NULL : _contSub.c_str()); }

		/**
		 * set name of container symbol
		 * @param c		container symbol
		 */
		inline void setContSub(SymbolLoc *c)						{ if (c->_contSub.empty()) { _contSub = c->_sym->name(); } else { _contSub = c->_contSub + '.' + c->_sym->name(); } }

		/**
		 * set code address of FETCH command for the symbol
		 * @param a		code address of FETCH command
		 */
		inline void setCodeAddr(unsigned a)							{ _codeAddr = a; }

		/**
		 * set syntax element for a lvalue symbol
		 */
		inline void setSyntaxElem(SyntaxElement *elem)				{ _elem = elem; }
	};


    // operation codes used for ExprParseInfo::_expOp
    #define EXPR_INFO_OP_NUM			1			// numerical operation or set operation
    #define EXPR_INFO_OP_IN				2			// operation "in"
    #define EXPR_INFO_OP_LOG			3			// logical operation (save "and")
    #define EXPR_INFO_OP_LOG_AND		4			// logical "and"
    #define EXPR_INFO_OP_CMP			5			// compare operation (save "==" and "!=")
    #define EXPR_INFO_OP_CMP2			6			// compare operation "==" or "!="
    #define EXPR_INFO_OP_OBJTO			7			// direction of optimisation objective
    #define EXPR_INFO_OP_SUB			8			// subsymbol operation
    #define EXPR_INFO_OP_ARR			9			// array construction
    #define EXPR_INFO_OP_TUP			10			// tupel construction
    #define EXPR_INFO_OP_SET			11			// set construction or interval construction
    #define EXPR_INFO_OP_IND			12			// index operation
    #define EXPR_INFO_OP_CALL			13			// function call
	#define EXPR_INFO_OP_ARRC			14			// array cast
    #define EXPR_INFO_OP_CAT			15			// list concatenation
    #define EXPR_INFO_OP_CBHCAT			16			// codeblock header concatenation
    #define EXPR_INFO_OP_INC			17			// increment or decrement
	#define EXPR_INFO_OP_CODEBL			18			// expression is an codeblock

    /**
	 * class for info about a parsed expression
	 */
	class ExprParseInfo
	{
	private:
        LocationInfo _loc;				///< location of this expression
        SymbolDef *_sym;                ///< symbol / NULL: other expression
        SpecialSymbol _special;         ///< special property of expression
        int _expOp;                     ///< operation generating the expression / 0: other expression

        bool _lval;						///< expression is lvalue
        bool _indexLval;				///< expression is lvalue with index operation
        bool _subSymLval;				///< expression is lvalue with subsymbol operation
        bool _empty;					///< expression is empty expression (if list: last part is empty expression)
        bool _oneElemArr;				///< array expression consist of exactly one element (non-empty element)
        unsigned _listCnt;				///< expression is list of expressions (0:no list; else count of list elements)

        vector<SymbolLoc *> _lvalSym;	///< vector of all symbols which are lvalues on a potential left side of assign

        const char *_assignOp;			///< assign operator (only set for left hand side expression or assign expression or codeblock header expression)
        AssignModificators *_assignMod;	///< assign modificator (only set for left hand side expression or assign expression)

        bool _cbhExpr;					///< expression for codeblock header (changes meaning of _lval and _lvalSym to an expression with new codeblock symbols which can stand on the left side of a compare operator)
        bool _cbhValid;					///< valid codeblock header expression with new codeblock symbols
        bool _volaExp;					///< expression may result to different values in different evaluations
        bool _ncSymAccess;              ///< expression contains access to a non-const symbol
        bool _cbSymAccess;              ///< expression contains access to a code block symbol defined in the same code block part

        bool _hasLNP;					///< expression has line name prefix (only needed if Compiler::compatLineNamePref())
        LocationInfo _locLNP;			///< location of the line name prefix

        unsigned _codeAddrNext;			///< code address after this expression (set in assignSymbols() and in oper())
        unsigned _lvlControlCB;			///< code block level count for code block control command

	public:
		/**
		 * constructor
		 * @param comp			compiler context / NULL: no check for _cbhExpr
		 * @param loc			location of expression
		 * @param spec			special property of expression
		 * @param empty			expression is empty expression
		 * @param cb			expression is a code block
		 */
		ExprParseInfo(CompilerContext *comp, const LocationInfo& loc, SpecialSymbol spec = specialSymbolKnownVal, bool empty = false, bool cb = false);

		/**
		 * constructor
         * @param comp			compiler context
         * @param loc			location of expression
		 * @param sym			symbol
		 * @param nw			symbol is new inserted
		 * @param mark			marking of the symbol
		 */
        ExprParseInfo(CompilerContext *comp, const LocationInfo& loc, SymbolDef *sym, bool nw, MarkForSymbolName mark);

		/**
		 * destructor
		 */
		~ExprParseInfo();

		/**
		 * set _cbhExpr if in an appropriate scope
		 * @param comp			compiler context
		 */
		void checkSetCbhExpr(CompilerContext *comp);

		/**
		 * reset expression after processing a codeblock header
		 */
		void resetCbhExpr()											{ _assignOp = NULL; }

		/**
		 * check whether expression is an array expression
		 * @param cb			check also for codeblock
		 */
		bool expTypeArray(bool cb = true)							{ return (_expOp == EXPR_INFO_OP_ARR || (cb && _expOp == EXPR_INFO_OP_CODEBL)); }

		/**
		 * check whether expression is a tuple expression
		 */
		bool expTypeTuple()											{ return (_expOp == EXPR_INFO_OP_TUP); }

		/**
		 * set expression as left hand side of assignment, and define the lvalue symbols
		 * @param comp			compiler context
		 * @param op			assign operator / NULL: left hand side only
		 * @param locOp			location of assign operator / NULL: left hand side only
		 */
		void setAssignLhs(CompilerContext *comp, const char *op, LocationInfo *locOp);

		/**
		 * set expression as left hand side of assignment without right hand side, if this is a left hand side
		 * @param comp			compiler context
		 * @return              expression is left hand side
		 */
		bool setAssignIfLhs(CompilerContext *comp);

		/**
		 * set expression as codeblock header expression and define the new codeblock symbols
		 * @param comp			compiler context
		 */
		void setAssignCbh(CompilerContext *comp);

		/**
		 * declare or check assigned symbols if necessary
		 * @param comp          compiler context
		 * @param locOp			location of assign operator / NULL: left hand side only
		 * @param localAssign   assign without use of directive
         * @param cbhDef        define code block symbol within code block header
		 */
        void assignSymbols(CompilerContext *comp, LocationInfo *locOp, bool localAssign, bool cbhDef);

		/**
		 * assign special meaning to const defined lvalue symbols of the expression
		 * @param sp			special meaning for the symbols
		 */
		void assignSpecialToConst(SpecialSymbol sp);

		/**
		 * get symbol
		 */
		inline SymbolDef *sym()											{ return _sym; }

		/**
		 * get assign operator
		 */
		inline const char *assignOp()									{ return _assignOp; }

		/**
		 * get special meaning for the expression
		 */
		inline SpecialSymbol special()									{ return _special; }

		/**
		 * get count of list elements if expression is list, otherwise 0
		 */
		inline unsigned listCnt()										{ return _listCnt; }

		/**
		 * get whether expression is lvalue with index operation
		 */
		inline bool indexLval()											{ return _indexLval; }

		/**
		 * get whether expression is lvalue with subsymbol operation
		 */
		inline bool subSymLval()										{ return _subSymLval; }

		/**
		 * get whether expression is empty expression (if list: last part is empty expression)
		 */
		inline bool empty()												{ return _empty; }

		/**
		 * get whether array expression consist of exactly one element (non-empty element)
		 */
		inline bool oneElemArr()										{ return _oneElemArr; }

        /**
         * get whether assign modificators indicate an initial assign
         */
        inline bool isInitialAssign()       							{ return (_assignMod && _assignMod->_defIsInitial != assignModLevelUndefined && _assignMod->_isInitial); }

		/**
		 * get whether assign modificators indicate a ref assign
		 */
		inline bool isRefAssign()										{ return (_assignMod && _assignMod->_defIsRef != assignModLevelUndefined && _assignMod->_isRef); }

		/**
		 * get whether assign modificators indicate an external symbol declaration
		 */
		inline bool isExternAssign()									{ return (_assignMod && _assignMod->_defIsExtern != assignModLevelUndefined && _assignMod->_isExtern); }

		/**
		 * get whether assign modificators indicate an assert symbol definition
		 */
		inline bool isAssertAssign()									{ return (_assignMod && _assignMod->_defIsAssert != assignModLevelUndefined && _assignMod->_isAssert); }

		/**
		 * get whether assign modificators indicate an declare symbol definition
		 */
		inline bool isDeclareAssign()									{ return (_assignMod && _assignMod->_defIsDeclare != assignModLevelUndefined && _assignMod->_isDeclare); }

		/**
		 * get whether assign modificators indicate a const assignment
		 */
		inline bool isConstAssign()										{ return (_assignMod && _assignMod->_defIsConst != assignModLevelUndefined && _assignMod->_isConst); }

        /**
         * get whether assign modificators indicate an nocond assignment
         */
        inline bool isNocondAssign()									{ return (_assignMod && _assignMod->_defIsNocond != assignModLevelUndefined && _assignMod->_isNocond); }

		/**
		 * get whether assign modificators indicate an ordered assignment
		 */
		inline bool isOrderedAssign()									{ return (_assignMod && _assignMod->_defIsOrdered != assignModLevelUndefined && _assignMod->_isOrdered); }

        /**
         * get whether assign modificators indicate a "new" assignment
         */
        inline bool isNewAssign()                                       { return (_assignMod && _assignMod->_defIsNew != assignModLevelUndefined && _assignMod->_isNew); }

        /**
         * get whether assign modificators indicate a validity scope
         * @param sc			return validity scope
         */
        inline bool isScopeAssign(SymbolScopeType& sc)                   { sc = (_assignMod ? _assignMod->_symScope : symbolScopeTypeUndefined); return (_assignMod && _assignMod->_defSymScope != assignModLevelUndefined); }

		/**
		 * get whether assign modificators indicate a codeblock control assignment
		 * @param cb			return codeblock control (if true, then only modificatorBreak, modificatorContinue, modificatorRepeat or modificatorReturn)
		 */
        inline bool isControlCBAssign(ModificatorKey& cb)				{ cb = (_assignMod ? _assignMod->_controlCB : modificatorBreak); return (_assignMod && _assignMod->_defControlCB != assignModLevelUndefined); }

		/**
		 * get symbol of object type if specified in the assign modificators, otherwise NULL
		 */
		inline SymbolDef *assignModObjType()								{ return (_assignMod && _assignMod->_defObjType != assignModLevelUndefined ? _assignMod->_objType : NULL); }

		/**
		 * get symbol of data type if specified in the assign modificators, otherwise NULL
		 */
		inline SymbolDef *assignModDataType()								{ return (_assignMod && _assignMod->_defDataType != assignModLevelUndefined ? _assignMod->_dataType : NULL); }


		/**
		 * move SymbolLoc entries from one vector to another
		 * @param dst			destination
		 * @param src			source
		 */
		inline void moveSymbolLocs(vector<SymbolLoc *>& dst, vector<SymbolLoc *>& src)		{ for (vector<SymbolLoc *>::iterator it = src.begin(); it != src.end(); it++) { dst.push_back(*it); } src.clear(); }

		/**
		 * add to this expression a second expression by an operation
		 * @param comp			compiler context, only for error handling
		 * @param op			operation
		 * @param e2			second expression / NULL: no second operand
		 * @param free2			free second expression
		 * @return				this
		 */
		ExprParseInfo *oper(CompilerContext *comp, int op, ExprParseInfo *e2, bool free2);



		/**
		 * symbols in _lvalSym are not used as lvalue, so check for definition und visibility
		 * @param comp			compiler context, only for error handling
		 * @param check			check symbols for definition und visibility
		 * @param clear			clear _lvalSym
		 */
		void checkClearLvalSyms(CompilerContext *comp, bool check = true, bool clear = true);

		/**
		 * check if expression has a line name prefix and then print an error message
		 * @param comp			compiler context, only for error handling
		 */
		void checkLNPError(CompilerContext *comp);

		/**
		 * check if expression is an code block header lvalue, and the compatibility mode for assign operator in code block header is set
		 * @param comp			compiler context, only for error handling
		 * @param loc			location of assign operator
		 */
		void checkCbhAssignError(CompilerContext *comp, LocationInfo& loc);

		/**
		 * set code address of FETCH command for a lvalue symbol
		 * @param a				code address of FETCH command
		 */
		inline void setLvalueCodeAddr(unsigned a)						{ if (_lvalSym.size() == 1) { _lvalSym[0]->setCodeAddr(a); } }

		/**
		 * get code address of FETCH command for a lvalue symbol
		 * @return				code address of FETCH command
		 */
		inline unsigned getLvalueCodeAddr()								{ return (_lvalSym.size() == 1 ? _lvalSym[0]->codeAddr() : 0u); }

		/**
		 * set syntax element for a lvalue symbol
		 */
		inline void setLvalueSyntaxElem(SyntaxElement *elem)			{ if (_lvalSym.size() == 1) { _lvalSym[0]->setSyntaxElem(elem); } }

		/**
		 * get code address after this expression
		 */
		inline unsigned codeAddrNext()									{ return _codeAddrNext; }

		/**
		 * get code block level count for code block control command
		 */
		inline unsigned lvlControlCB()									{ return _lvlControlCB; }

		/**
		 * get count of lvalue symbols
		 */
		inline unsigned countLvalues()									{ return _lvalSym.size(); }

		/**
		 * get symbol name of a lvalue symbol
		 */
		inline const char *getLvalueSymName()							{ return (_lvalSym.size() == 1 ? _lvalSym[0]->sym()->name() : NULL); }

		/**
		 * change FETCH command for lvalue symbols to FETCH_LVALUE
		 * @param comp			compiler context
		 */
        void compChangeFetchLvalue(CompilerContext *comp);

		/**
		 * change FETCH command for lvalue symbols to NOP
		 * @param comp			compiler context
		 */
		void compChangeFetchNop(CompilerContext *comp);

		/**
		 * get code address after this expression and a set of the lvalue symbol numbers
		 * @param symnrs		set for filling the lvalue symbol numbers in
		 * @return				code address after this expression
		 */
		unsigned codeAddrNextAndSymbols(set<unsigned>& symnrs);

		/**
		 * get whether expression may result to different values in different evaluations
		 */
		inline bool volaExp()											{ return _volaExp; }

        /**
         * reset flag for an expression which may result to different values in different evaluations
         */
        inline void resetVolaExp()                                      { _volaExp = false; }

        /**
         * get whether expression contains access to a non-const symbol
         */
        inline bool ncSymAccess()                                       { return _ncSymAccess; }

        /**
         * get whether expression contains access to a code block symbol defined in the same code block part
         */
        inline bool cbSymAccess()                                       { return _cbSymAccess; }

		/**
		 * set that the expression has a line name prefix
		 * @param loc			location of the line name prefix
		 */
		inline void setHasLNP(LocationInfo& loc)						{ _hasLNP = true; _locLNP = loc; }
	};


}

#endif // !COMPILERUTIL_HH

