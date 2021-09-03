/***********************************************************************
 *  This code is part of CMPL
 *
 *  Copyright (C) 2007, 2008, 2009, 2010 Thomas Schleiff - Halle(Saale),
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

/* generate C++ parser class CMPL_Parser in the namespace */
%skeleton "lalr1.cc"
%require "3.0.4"
%name-prefix "cmpl"
%defines
%define parser_class_name {CMPL_Parser}

%{
namespace cmpl
{
    class CompilerContext;
	class SyntaxElement;
}
%}

%parse-param { CompilerContext& cmplcomp }
%lex-param   { yyscan_t CMPLCOMP_FOR_SCANNER }
%lex-param   { CompilerContext& cmplcomp }

%locations
%debug
%error-verbose


%{
#include "../../CommonData/SyntaxElements.hh"
#include "CompilerUtil.hh"
#include "CompilerContext.hh"

#define CMPLCOMP_FOR_SCANNER    cmplcomp._scanner

// in the skeleton the definition of yyerrok is missing, thats why it is defined here
#define yyerrok     (yyerrstatus_ = 0)
%}


%union {
	struct {
		// syntax element
		SyntaxElement *_elem;
		bool _empty;

		// name and type of element
		const char *_name;
		SemanticValueType _tp;

		// semantic value
		union {
			// generic values
			const char *s;
			string *str;
			int i;
			bool b;
			unsigned u;

			// symbol / expression
			ExprParseInfo *exp;

			// previous scanner token type
			ScanPrevTokenType sptt;

			// code block reference
			struct {
				unsigned a;
				bool b;
			} cbr;

			// code block function info
			struct {
				ExprParseInfo *exp;
				bool useSubs;
				unsigned short cntLocal;
			} fnc;
		} _u;

		inline void init() 							{ _elem = NULL; _empty = true; _name = NULL; _tp = semanticValueElse; _u.exp = NULL; }
		inline void setExp(ExprParseInfo *e)		{ _tp = (e ? semanticValueExpr : semanticValueElse); _u.exp = e; }
	} s;
}


%{
#include "../../Control/MainControl.hh"
#define PROTO_PC_OUT(a)		PROTO_MOD_OUT((&cmplcomp), a)
#define PROTO_PC_OUTL(a)	PROTO_MOD_OUTL((&cmplcomp), a)
#define PARSE_CONTEXT		cmplcomp


// defines for location output
#define CMPLELEMENTRY(s,n,lf,lt)			{ s._empty = false; s._elem = SyntaxElement::newEntry(PARSE_CONTEXT.nextIdSyntaxElem(), n, &lf, &lt); }
#define CMPLELEMENTRY_TXT(s,n,lf,lt,t)		{ s._empty = false; s._elem = SyntaxElement::newEntry(PARSE_CONTEXT.nextIdSyntaxElem(), n, t, &lf, &lt); }
#define CMPLELEMENTRY_ALT(s,n,lf,lt,t,a)	{ s._empty = false; s._elem = SyntaxElement::newEntry(PARSE_CONTEXT.nextIdSyntaxElem(), n, t, a, &lf, &lt); }
#define CMPLELEMENTRY_SYM(s,n,lf,lt,sym)	{ s._empty = false; s._elem = SyntaxElement::newEntry(PARSE_CONTEXT.nextIdSyntaxElem(), n, (sym)->nameGlob(), (sym)->defId(), &lf, &lt); }
#define CMPLELEMENTRY_ERR(s,lf)				{ s._empty = false; s._elem = PARSE_CONTEXT.errorRecoveryElem(&lf); }
#define CMPLELEMENTRY_EMPTY(s,lf)			{ s._empty = false; s._elem = SyntaxElement::newEntry(PARSE_CONTEXT.nextIdSyntaxElem(), SyntaxElementEmpty, &lf, &lf); }

#define CMPLELEM_SET(s,b)					{ s._empty = b._empty; s._elem = b._elem; }
#define CMPLELEM_NEXT(s,n)					{ s._elem->appendNext(n._elem); }
#define CMPLELEM_CONCAT(s,b,n)				{ s._empty = b._empty; s._elem = b._elem; if (s._empty) { CMPLELEM_SET(s,n) } else s._elem->appendNext(n._elem); }
#define CMPLELEM_CHILD(s,c)					{ s._elem->appendChild(c._elem); }
#define CMPLELEM_CHILD2(s,c,d)				{ s._elem->appendChild(c._elem); s._elem->appendChild(d._elem); }

#define SET_CMPLELEM(c,lf)					{ PARSE_CONTEXT.setSyntaxStructure(c._elem, &lf); }


// defines for generating intermediary code
#define COMP_END(e)							{ if (!PARSE_CONTEXT.checkOnly()) PARSE_CONTEXT.compFunction(e, false, false, 0, 0, false); }
#define COMP_LIT_VALUE(e,s,t)				{ if (!PARSE_CONTEXT.checkOnly()) PARSE_CONTEXT.compLitVal(e, s, t); }
#define COMP_LIT_BLANK(ar)					{ if (!PARSE_CONTEXT.checkOnly()) ar = PARSE_CONTEXT.compLitVal(NULL, "", ICS_LIT_BLANK); }
#define COMP_FETCH_SYM(e,s)					{ if (!PARSE_CONTEXT.checkOnly()) PARSE_CONTEXT.compFetchSym(e, s); }
#define COMP_INCDEC_FETCH(e,b,i)			{ if (!PARSE_CONTEXT.checkOnly()) PARSE_CONTEXT.compIncDecFetch(e, b, i); }
#define COMP_ASSIGN(e,lhs,rhs)				{ if (!PARSE_CONTEXT.checkOnly()) PARSE_CONTEXT.compAssign(e, lhs, rhs); }
#define COMP_OPERATION(e,op,c)				{ if (!PARSE_CONTEXT.checkOnly()) PARSE_CONTEXT.compOperation(e, false, op, c); }
#define COMP_CONSTRUCT(e,op,c)				{ if (!PARSE_CONTEXT.checkOnly()) PARSE_CONTEXT.compOperation(e, true, op, c); }
#define COMP_ARRCONSTRUCT_MODE(e,b)			{ if (!PARSE_CONTEXT.checkOnly()) PARSE_CONTEXT.compArrayConstructMode(e, b); }
#define COMP_FUNC_START(e,ar)				{ if (!PARSE_CONTEXT.checkOnly()) ar = PARSE_CONTEXT.compFunction(e, true, true, 0, 0, false); }
#define COMP_FUNC_END(e,a,cl,us)			{ if (!PARSE_CONTEXT.checkOnly()) PARSE_CONTEXT.compFunction(e, true, false, a, cl, us); }
#define COMP_LNP_COMPAT(e,s)				{ if (!PARSE_CONTEXT.checkOnly()) PARSE_CONTEXT.compLineNamePrefCompat(e, s); }
#define COMP_CBHEADER_START(e,ar)			{ if (!PARSE_CONTEXT.checkOnly()) ar = PARSE_CONTEXT.compCBHeaderStart(e); }
#define COMP_CBHEADER_END(e,x,s)			{ if (!PARSE_CONTEXT.checkOnly()) PARSE_CONTEXT.compCBHeaderEnd(e, x, s); }
#define COMP_SET_SYNTAXELEM(a,e)			{ if (!PARSE_CONTEXT.checkOnly()) PARSE_CONTEXT.compSetCmdSyntaxElem(a, e); }


// does nothing, only to suppress warnings about not used token values, that bison prints out when use %destructor
#define NOT_USED(a)
%}


/* priority of operations */
%right OBJ_TO
%right ASSIGN ASSIGN_ADD ASSIGN_SUB ASSIGN_MUL ASSIGN_DIV
%left LOG_OR
%left LOG_AND
%left COMP_EQ COMP_EQ2 COMP_NE COMP_NE2 COMP_LT COMP_GT COMP_LE COMP_GE
%left OPERATOR_IN OPERATOR_OF
%left '+' '-'
%left '*' '/' INTERSECT_RR
%nonassoc OPERATOR_TRANSPOSE
%right ARRCAST
%right '^'
%left POINTS
%nonassoc NUMBER_SIGN INDEX_SIGN LOG_NOT
%left '(' '[' CODEBLOCK_BEGIN
%left SUBSYM_OP


/* token */
%token END  0   "end of file"
%token <s> INT_NUMBER "int literal"
%token <s> DOUBLE_NUMBER "double literal"
%token <s> STRING "string literal"
%token <s> LINENAME_PREF "linename prefix"
%token <s> INT_NUMBER_MULT "int literal factor"
%token <s> DOUBLE_NUMBER_MULT "double literal factor"
%token <s> SYMBOL "symbol"
%token <s> SUBSYMBOL "subsymbol"
%token <s> SYMBOL_OBJTYPE "object type symbol"
%token <s> SYMBOL_DATATYPE "data type symbol"
%token <s> MOD_ASSIGN "assign modificator"
%token <s> MOD_ORDERED "order modificator"
%token <s> OPERATOR_IN "operator `in`"
%token <s> OPERATOR_OF "operator `of`"
%token <s> OPERATOR_TRANSPOSE "transpose operator `T`"
%token <s> SUBSYM_OP "subsymbol operator `.`"
%token <s> ASSIGN "assign operator `:=`"
%token <s> ASSIGN_ND "assign operator `::=`"
%token <s> ASSIGN_ND_ADD "assign operator `+=`"
%token <s> ASSIGN_ND_SUB "assign operator `-=`"
%token <s> ASSIGN_ND_MUL "assign operator `*=`"
%token <s> ASSIGN_ND_DIV "assign operator `/=`"
%token <s> INCDEC "increment/decrement operator"
%token <s> COLON "`:`"
%token <s> COMP_EQ "operator `=`"
%token <s> COMP_EQ2 "operator `==`"
%token <s> COMP_NE "operator `<>`"
%token <s> COMP_NE2 "operator `!=`"
%token <s> COMP_LT "operator `<`"
%token <s> COMP_GT "operator `>`"
%token <s> COMP_LE "operator `<=`"
%token <s> COMP_GE "operator `>=`"
%token <s> LOG_AND "operator `&&`"
%token <s> LOG_OR "operator `||`"
%token <s> LOG_NOT "operator `!`"
%token <s> POINTS "operator `..`"
%token <s> ARRCAST "array cast operation"
%token <s> INTERSECT_RR "operator `*>`"
%token <s> OBJ_TO "operator `->`"
%token <s> TUPLE_END "`]`"
%token <s> CODEBLOCK_BEGIN "`{`"
%token <s> CODEBLOCK_END "`}`"
%token <s> CODEBLOCK_ALT "`|`"
%token <s> REF_CODEBLOCK "`&`"
%token <s> CB_SYMBOL_MARK "`@`"
%token <s> EOSTATEMENT "`;`"

%type <s> cmpl cmplParts cmplPart directive directiveParts directivePart objDataType
%type <s> statement statementExpr assignLhsOp assignRhs objDataTypeRhs assignOp
%type <s> codeblock blockStart blockModifiers blockModifier blockParts blockPart blockAltStart blockHeaders blockHeader blockHeaderStart blockPartStart blockBody expressionWithCodeblockSymbolDef refcodeblock
%type <s> expression expressionList symbolIncDec symbol literal literalMult arrayExpr arrayStart tupleExpr tupleStart listParts listPart intervalExpr intervalOp setExpr setExprEnd
%type <s> unaryOp binaryOp numOpSignAdd numOpSignMult logOpSignNot inOpPre ofOpPre compareOp subsymbolOp subsymbolIncDec subsymbol functionCall indexation arraycast errorExpression
%type <s> error


%destructor { } cmpl
%destructor { if (!$$._empty) { PARSE_CONTEXT.insertErrElem($$._elem); }  /*if ($$._tp == semanticValueExpr) { delete $$._u.exp; }*/ } <s>

%%


/*
 * cmpl main structure
 */
cmpl			: cmplParts														{ $$ = $1; PROTO_PC_OUTL("Parse cmpl End"); SET_CMPLELEM($1, @1); COMP_END($1._elem); }
				;

cmplParts		: /* empty */													{ $$.init(); }
		 		| cmplPart cmplParts											{ $$.init(); CMPLELEM_CONCAT($$, $1, $2); }
				;

cmplPart		: directive														{ $$ = $1; PARSE_CONTEXT.setCbExprModeStatementStart(); PARSE_CONTEXT.setDirectiveFromCurrent(&@1); }
				| statement														{ $$ = $1; PARSE_CONTEXT.setCbExprModeStatementStart(); PARSE_CONTEXT.clearModificators(); }
				;

directive		: directiveParts COLON											{ $$.init(); CMPLELEMENTRY($$, SyntaxElementDirective, @1, @2); CMPLELEM_CHILD($$, $1); NOT_USED($2); }
		  		;

directiveParts	: /* empty */													{ $$.init(); }
			   	| directivePart directiveParts									{ $$.init(); CMPLELEM_CONCAT($$, $1, $2); }
				;

directivePart	: MOD_ASSIGN													{ $$.init(); CMPLELEMENTRY_TXT($$, SyntaxElementModificator, @1, @1, $1._name); PARSE_CONTEXT.setModificator(&@1, (ModificatorKey) $1._u.i); }
				| MOD_ORDERED													{ $$.init(); CMPLELEMENTRY_TXT($$, SyntaxElementModificator, @1, @1, $1._name); PARSE_CONTEXT.setModificator(&@1, (ModificatorKey) $1._u.i); }
				| objDataType													{ $$ = $1; CMPLELEM_SET($$, $1); PARSE_CONTEXT.setModificator(&@1, $1._u.exp->sym(), (SPECIALSYMBOL_IS_OBJTYPE($1._u.exp->sym()->special())));
																				  delete $1._u.exp; }
				| LINENAME_PREF													{ $$.init(); CMPLELEMENTRY($$, SyntaxElementLineNamePrefix, @1, @1);
																				  PARSE_CONTEXT.setLineNamePref($$._elem, &@1, $1._u.s); }
				;

objDataType		: SYMBOL_OBJTYPE												{ $$ = $1; CMPLELEMENTRY_SYM($$, SyntaxElementSymbol, @1, @1, $1._u.exp->sym()); }
			 	| SYMBOL_DATATYPE												{ $$ = $1; CMPLELEMENTRY_SYM($$, SyntaxElementSymbol, @1, @1, $1._u.exp->sym()); }
				;



/*
 * statements and assignment
 */
statement		: directiveParts EOSTATEMENT									{ $$.init();
		   																		  if (!$1._empty && PARSE_CONTEXT.testCompControlCB($1._elem, @1, false)) { CMPLELEMENTRY($$, SyntaxElementStatement, @1, @2); CMPLELEM_CHILD($$, $1); }
																				  else { CMPLELEMENTRY_EMPTY($2, @2); CMPLELEMENTRY($$, SyntaxElementStatement, @2, @2); CMPLELEM_CHILD($$, $2); }
																				  PARSE_CONTEXT.endOfStatement($$._elem); }
		 		| directiveParts statementExpr EOSTATEMENT						{ $$.init();
																				  if ($2._u.b) PARSE_CONTEXT.testCompControlCB($1._elem, @1, true);
																				  CMPLELEMENTRY($$, SyntaxElementStatement, ($1._empty ? (@2) : (@1)), @3); CMPLELEM_CHILD2($$, $1, $2); PARSE_CONTEXT.endOfStatement($$._elem); NOT_USED($3); }
				| error EOSTATEMENT												{ $$.init(); CMPLELEMENTRY_ERR($1, @1); CMPLELEMENTRY($$, SyntaxElementStatement, @1, @2); CMPLELEM_CHILD($$, $1); PARSE_CONTEXT.endOfStatement($$._elem); NOT_USED($2);
																				  yyerrok; }
				;

statementExpr	: assignLhsOp assignRhs											{ $$.init(); CMPLELEMENTRY($$, SyntaxElementAssignRhs, @2, @2); CMPLELEM_CHILD($$, $2); CMPLELEM_SET($2, $$);
			  																	  CMPLELEMENTRY_TXT($$, SyntaxElementAssignment, @1, @2, $1._u.exp->assignOp()); CMPLELEM_CHILD2($$, $1, $2);
																				  if ($1._u.exp->isRefAssign()) { $2._u.exp->compChangeFetchLvalue(&PARSE_CONTEXT); }
																				  COMP_ASSIGN($$._elem, $1._u.exp, $2._u.exp);
																				  $2._u.exp->checkClearLvalSyms(&PARSE_CONTEXT);
																				  $1._u.exp->assignSpecialToConst($2._u.exp->special());
																				  delete $1._u.exp; delete $2._u.exp; $$._u.b = false; }
				| expressionList												{ $$ = $1;
																				  bool controlCB = false;
																				  bool amEmpty = PARSE_CONTEXT.assignModCurrentEmpty(true);
																				  if ($1._u.exp->setAssignIfLhs(&PARSE_CONTEXT)) {
																				  	$1._u.exp->checkLNPError(&PARSE_CONTEXT);
																				  	CMPLELEMENTRY($$, SyntaxElementAssignLhs, @1, @1); CMPLELEM_CHILD($$, $1);
																					COMP_ASSIGN($$._elem, $1._u.exp, NULL); }
																				  else {
																				    controlCB = PARSE_CONTEXT.assignModControlCB(&@1);
																					if (!controlCB) {
																				  		if (!amEmpty) PARSE_CONTEXT.outParseError(ERROR_LVL_NORMAL, "statement cannot have assign modificators", @1);
																						COMP_ASSIGN($$._elem, NULL, $1._u.exp); } }
																				  delete $1._u.exp; $$._u.b = controlCB; }
				;

assignLhsOp		: expressionList assignOp										{ $$ = $1; CMPLELEMENTRY($$, SyntaxElementAssignLhs, @1, @1); CMPLELEM_CHILD($$, $1);
			 																	  $1._u.exp->checkLNPError(&PARSE_CONTEXT);
			 																	  $$._u.exp->setAssignLhs(&PARSE_CONTEXT, $2._name, &@2); }
		  		;

assignRhs		: expression													{ $$ = $1; $1._u.exp->checkLNPError(&PARSE_CONTEXT); }
				| objDataTypeRhs												{ $$ = $1; }
				| objDataTypeRhs tupleExpr										{ $$.init(); CMPLELEMENTRY($$, SyntaxElementIndexation, @1, @2); CMPLELEM_CHILD2($$, $1, $2);
																				  $$.setExp($1._u.exp->oper(&PARSE_CONTEXT, EXPR_INFO_OP_IND, $2._u.exp, true)); }
				;

objDataTypeRhs	: objDataType													{ COMP_FETCH_SYM($$._elem, $1._u.exp); $$._u.exp->checkSetCbhExpr(&PARSE_CONTEXT); }
				;

assignOp		: ASSIGN														{ $$ = $1; PARSE_CONTEXT.setCbExprModeReset(); }
		  		| ASSIGN_ND														{ $$ = $1; PARSE_CONTEXT.setCbExprModeReset(); }
		  		| ASSIGN_ND_ADD													{ $$ = $1; PARSE_CONTEXT.setCbExprModeReset(); }
				| ASSIGN_ND_SUB													{ $$ = $1; PARSE_CONTEXT.setCbExprModeReset(); }
				| ASSIGN_ND_MUL													{ $$ = $1; PARSE_CONTEXT.setCbExprModeReset(); }
				| ASSIGN_ND_DIV													{ $$ = $1; PARSE_CONTEXT.setCbExprModeReset(); }
				| COLON															{ $$ = $1; PARSE_CONTEXT.setCbExprModeReset(); }
				;


/*
 * codeblock
 */
codeblock		: blockStart blockParts CODEBLOCK_END							{ $$.init(); CMPLELEMENTRY($$, SyntaxElementCodeblock, @1, @3); CMPLELEM_CHILD($$, $2); NOT_USED($1); NOT_USED($3);
		   																		  $$.setExp(new ExprParseInfo(&PARSE_CONTEXT, $$._elem->loc(), specialSymbolNo, false, true));
																				  $$._u.fnc.exp = $$._u.exp; $$._u.fnc.cntLocal = PARSE_CONTEXT.funcParseScopeInfo($$._u.fnc.useSubs);
		   																		  PARSE_CONTEXT.endParseScope($$._elem, @3); }
				| blockStart error CODEBLOCK_END								{ $$.init(); CMPLELEMENTRY_ERR($2, @2); CMPLELEMENTRY($$, SyntaxElementCodeblock, @1, @3); CMPLELEM_CHILD($$, $2); NOT_USED($1); NOT_USED($3);
																				  $$.setExp(new ExprParseInfo(&PARSE_CONTEXT, $$._elem->loc(), specialSymbolNo, false, true));
																				  $$._u.fnc.exp = $$._u.exp; $$._u.fnc.cntLocal = PARSE_CONTEXT.funcParseScopeInfo($$._u.fnc.useSubs);
																				  PARSE_CONTEXT.endParseScope($$._elem, @3);
																				  yyerrok; }
				;

blockStart		: CODEBLOCK_BEGIN												{ $$ = $1; PARSE_CONTEXT.startParseScope($$._elem, @1, parseScopeTypeBlock, $1._u.sptt);
																				  PARSE_CONTEXT.setCbExprModeReset(); }
	   			;

blockParts		: blockPart														{ $$ = $1; }
		  		| blockPart blockAltStart blockParts							{ $$.init(); CMPLELEM_CONCAT($$, $1, $3); NOT_USED($2); }
				;

blockAltStart	: CODEBLOCK_ALT													{ $$ = $1; PARSE_CONTEXT.startParseScope($$._elem, @1, parseScopeTypePart); PARSE_CONTEXT.setCbExprModeReset(); }
			  	;

blockPart		: blockPartStart blockBody										{ $$.init(); CMPLELEMENTRY($$, SyntaxElementBlockPart, @1, @2); CMPLELEM_CHILD2($$, $1, $2); delete $1._u.exp; PARSE_CONTEXT.setPartParseScopeElem($$._elem); }
				;

blockPartStart	: blockModifiers blockHeaders COLON								{ $$ = $2; PARSE_CONTEXT.startParseScope($3._elem, @3, parseScopeTypeBody); PARSE_CONTEXT.setCbExprModeCBBodyStart(); NOT_USED($1); }
			   	;

blockModifiers	: /* empty */													{ $$.init(); }
			   	| blockModifier blockModifiers									{ $$.init(); CMPLELEM_CONCAT($$, $1, $2); }
				;

blockModifier	: MOD_ORDERED													{ $$ = $1; CMPLELEMENTRY_TXT($$, SyntaxElementModificator, @1, @1, $1._name); PARSE_CONTEXT.setParseScopeParBlockPart(ICPAR_BLOCK_ORDERED); }
			  	;

blockHeaders	: blockHeader													{ $$.init();
			 																	  if ($1._empty) { CMPLELEMENTRY_EMPTY($1, @1); $1.setExp(new ExprParseInfo(&PARSE_CONTEXT, @1)); }
			 																	  CMPLELEMENTRY($$, SyntaxElementBlockHeader, @1, @1); CMPLELEM_CHILD($$, $1);
																				  $$.setExp($1._u.exp); }
				| blockHeader ','  blockHeaders									{ $$.init();
																				  if ($1._empty) { CMPLELEMENTRY_EMPTY($1, @1); $1.setExp(new ExprParseInfo(&PARSE_CONTEXT, @1)); }
																				  CMPLELEMENTRY($$, SyntaxElementBlockHeader, @1, @1); CMPLELEM_CHILD2($$, $1, $3);
																				  $$.setExp($1._u.exp->oper(&PARSE_CONTEXT, EXPR_INFO_OP_CBHCAT, $3._u.exp, true)); }
				;

blockHeader		: /* empty */													{ $$.init(); }
				| blockHeaderStart expressionWithCodeblockSymbolDef				{ $$ = $2; $$._u.exp->setAssignCbh(&PARSE_CONTEXT); COMP_CBHEADER_END($$._elem, $$._u.exp, $1._u.cbr.a); $$._u.exp->resetCbhExpr(); }
				;

blockHeaderStart: /* empty */													{ $$.init(); COMP_CBHEADER_START(NULL, $$._u.cbr.a); }
				;

blockBody		: directiveParts expression										{ $$.init(); CMPLELEMENTRY($$, SyntaxElementBlockBody, @1, @2); CMPLELEM_CHILD2($$, $1, $2);
		   																		  if (!$1._empty) {
																				  	if (!PARSE_CONTEXT.assignModCurrentEmpty()) PARSE_CONTEXT.outParseError(ERROR_LVL_NORMAL, "expression as code block body cannot have assign modificators", @1);
																					if (PARSE_CONTEXT.hasLineNamePref()) PARSE_CONTEXT.outParseError(ERROR_LVL_NORMAL, "expression as code block body cannot have line name prefix", @1);
																				  }
																				  $2._u.exp->checkLNPError(&PARSE_CONTEXT); $2._u.exp->checkClearLvalSyms(&PARSE_CONTEXT);
																				  if ($2._u.exp->ncSymAccess()) PARSE_CONTEXT.setParseScopeParBlockPart(ICPAR_BLOCK_NCSYM);
		   																		  delete $2._u.exp; }
		 		| cmplParts														{ $$.init(); CMPLELEMENTRY($$, SyntaxElementBlockBody, @1, @1); CMPLELEM_CHILD($$, $1); PARSE_CONTEXT.setParseScopeParBlockPart(ICPAR_BLOCK_STMNT); }
				;

expressionWithCodeblockSymbolDef: expression									{ $$ = $1; $1._u.exp->checkLNPError(&PARSE_CONTEXT); }
				| expression ASSIGN expression									{ $1._u.exp->checkCbhAssignError(&PARSE_CONTEXT, @2);
																				  $$.init(); CMPLELEMENTRY_ALT($$, SyntaxElementCompareOp, @1, @3, $1._name, "=="); CMPLELEM_CHILD2($$, $1, $3); NOT_USED($2);
                                                                                  $$.setExp($1._u.exp->oper(&PARSE_CONTEXT, EXPR_INFO_OP_CMP2, $3._u.exp, true));
																				  COMP_OPERATION($$._elem, ICS_OPER_EQ, 2); }
				;

refcodeblock	: REF_CODEBLOCK													{ $$ = $1; PARSE_CONTEXT.setCbExprModeReset(); $$._u.cbr.b = $1._u.b; COMP_FUNC_START($$._elem, $$._u.cbr.a); }


/*
 * expression
 */
expression		: symbolIncDec													{ $$ = $1; PARSE_CONTEXT.setCbExprModeResetStorePrv(); }
				| literal														{ $$ = $1; PARSE_CONTEXT.setCbExprModeReset(); }
				| arrayExpr														{ $$ = $1; PARSE_CONTEXT.setCbExprModeReset(); }
				| tupleExpr														{ $$ = $1; PARSE_CONTEXT.setCbExprModeReset(); }
				| intervalExpr													{ $$ = $1; PARSE_CONTEXT.setCbExprModeReset(); }
				| setExpr														{ $$ = $1; PARSE_CONTEXT.setCbExprModeReset(); }
				| unaryOp														{ $$ = $1; PARSE_CONTEXT.setCbExprModeReset(); }
				| binaryOp														{ $$ = $1; PARSE_CONTEXT.setCbExprModeReset(); }
				| compareOp														{ $$ = $1; PARSE_CONTEXT.setCbExprModeReset(); }
				| subsymbolOp													{ $$ = $1; PARSE_CONTEXT.setCbExprModeReset(); }
				| functionCall													{ $$ = $1; PARSE_CONTEXT.setCbExprModeReset(); }
				| indexation													{ $$ = $1; PARSE_CONTEXT.setCbExprModeReset(); }
				| arraycast														{ $$ = $1; PARSE_CONTEXT.setCbExprModeReset(); }
				| codeblock														{ $$ = $1; PARSE_CONTEXT.setCbExprModeReset(); }
				| refcodeblock codeblock										{ $$ = $2; $$._u.exp = $2._u.fnc.exp; $$._u.exp->resetVolaExp();
																				  CMPLELEMENTRY_TXT($$, SyntaxElementFunction, @1, @2, ($1._u.cbr.b ? "pure" : "nonpure")); CMPLELEM_CHILD($$, $2);
																				  COMP_FUNC_END($$._elem, $1._u.cbr.a, $2._u.fnc.cntLocal, $2._u.fnc.useSubs);
																				  PARSE_CONTEXT.setCbExprModeReset(); }
				| errorExpression												{ $$ = $1; PARSE_CONTEXT.setCbExprModeReset();
																				  PARSE_CONTEXT.outParseError(ERROR_LVL_NORMAL, "illegal expression, maybe missing operation sign", @1); }
				;

symbolIncDec	: symbol														{ $$ = $1; $$._u.exp->checkSetCbhExpr(&PARSE_CONTEXT); }
				| INCDEC symbol													{ $$ = $2; CMPLELEMENTRY_TXT($$, SyntaxElementIncDec, @1, @2, ($1._u.i > 0 ? "prefix inc" : "prefix dec")); CMPLELEM_CHILD($$, $2);
																				  $$.setExp($2._u.exp->oper(&PARSE_CONTEXT, EXPR_INFO_OP_INC, NULL, false));
																				  $$._u.exp->checkSetCbhExpr(&PARSE_CONTEXT);
																				  COMP_INCDEC_FETCH($$._elem, true, $1._u.i); }
				| symbol INCDEC													{ $$ = $1; CMPLELEMENTRY_TXT($$, SyntaxElementIncDec, @1, @2, ($2._u.i > 0 ? "postfix inc" : "postfix dec")); CMPLELEM_CHILD($$, $1);
																				  $$.setExp($1._u.exp->oper(&PARSE_CONTEXT, EXPR_INFO_OP_INC, NULL, false));
																				  $$._u.exp->checkSetCbhExpr(&PARSE_CONTEXT);
																				  COMP_INCDEC_FETCH($$._elem, false, $2._u.i); }
				;

symbol			: SYMBOL														{ $$ = $1; CMPLELEMENTRY_SYM($$, SyntaxElementSymbol, @1, @1, $1._u.exp->sym());
		 																		  COMP_FETCH_SYM($$._elem, $1._u.exp);
																				  $$._u.exp->setLvalueSyntaxElem($$._elem); }
		 		;

expressionList	: expression													{ $$ = $1; }
			 	| expression ',' expressionList									{ $$.init(); CMPLELEM_CONCAT($$, $1, $3); $$.setExp($1._u.exp->oper(&PARSE_CONTEXT, EXPR_INFO_OP_CAT, $3._u.exp, true)); }
				;

literal			: DOUBLE_NUMBER													{ $$ = $1; CMPLELEMENTRY($$, SyntaxElementLitDouble, @1, @1);
		  																		  COMP_LIT_VALUE($$._elem, $1._u.s, ICS_LIT_REAL); $$.setExp(new ExprParseInfo(&PARSE_CONTEXT, @1)); }
		  		| INT_NUMBER													{ $$ = $1; CMPLELEMENTRY($$, SyntaxElementLitInt, @1, @1);
																				  COMP_LIT_VALUE($$._elem, $1._u.s, ICS_LIT_INT); $$.setExp(new ExprParseInfo(&PARSE_CONTEXT, @1)); }
				| STRING														{ $$ = $1; CMPLELEMENTRY($$, SyntaxElementLitString, @1, @1);
																				  COMP_LIT_VALUE($$._elem, $1._u.s, ICS_LIT_STR); $$.setExp(new ExprParseInfo(&PARSE_CONTEXT, @1)); }
				;

literalMult		: DOUBLE_NUMBER_MULT											{ $$ = $1; CMPLELEMENTRY($$, SyntaxElementLitDouble, @1, @1);
			 																	  COMP_LIT_VALUE($$._elem, $1._u.s, ICS_LIT_REAL); $$.setExp(new ExprParseInfo(&PARSE_CONTEXT, @1));
			 																	  PARSE_CONTEXT.setCbExprModeReset(); }
			 	| INT_NUMBER_MULT												{ $$ = $1; CMPLELEMENTRY($$, SyntaxElementLitInt, @1, @1);
																				  COMP_LIT_VALUE($$._elem, $1._u.s, ICS_LIT_INT); $$.setExp(new ExprParseInfo(&PARSE_CONTEXT, @1));
																				  PARSE_CONTEXT.setCbExprModeReset(); }
				;


arrayExpr		: arrayStart listParts ')'										{ $$.init(); CMPLELEMENTRY($$, SyntaxElementArray, @1, @3); CMPLELEM_CHILD($$, $2); NOT_USED($1);
		   																		  if ($2._u.exp->listCnt() || $2._u.exp->empty()) { COMP_CONSTRUCT($$._elem, ICS_CONSTRUCT_ARRAY, ($2._u.exp->listCnt() ?: 1)); }
		   																		  $$.setExp($2._u.exp->oper(&PARSE_CONTEXT, EXPR_INFO_OP_ARR, NULL, false)); }
				| arrayStart error ')'											{ $$.init(); CMPLELEMENTRY_ERR($2, @2); CMPLELEMENTRY($$, SyntaxElementArray, @1, @3); CMPLELEM_CHILD($$, $2); NOT_USED($1);
																				  $$.setExp(new ExprParseInfo(&PARSE_CONTEXT, @1));
																				  yyerrok; }
				;

arrayStart		: '('															{ $$.init(); PARSE_CONTEXT.setCbExprModeReset(); }
			 	;

tupleExpr		: tupleStart listParts TUPLE_END								{ $$.init(); CMPLELEMENTRY($$, SyntaxElementTupel, @1, @3); CMPLELEM_CHILD($$, $2); NOT_USED($1); NOT_USED($3)
                                                                                  COMP_CONSTRUCT($$._elem, ICS_CONSTRUCT_TUPLE, ($2._u.exp->listCnt() ?: 1));
		   																		  $$.setExp($2._u.exp->oper(&PARSE_CONTEXT, EXPR_INFO_OP_TUP, NULL, false)); }
				| tupleStart error TUPLE_END									{ $$.init(); CMPLELEMENTRY_ERR($2, @2); CMPLELEMENTRY($$, SyntaxElementTupel, @1, @3); CMPLELEM_CHILD($$, $2); NOT_USED($1); NOT_USED($3);
																				  $$.setExp(new ExprParseInfo(&PARSE_CONTEXT, @1));
																				  yyerrok; }
				;

tupleStart		: '['															{ $$.init(); PARSE_CONTEXT.setCbExprModeReset(); }
			 	;

listParts		: listPart														{ $$.init(); 
		   																		  if ($1._empty) { CMPLELEMENTRY_EMPTY($$, @1); COMP_SET_SYNTAXELEM($1._u.u, $$._elem); $1.setExp(new ExprParseInfo(&PARSE_CONTEXT, @1, specialSymbolKnownVal, true)); }
		   																		  else { CMPLELEM_SET($$, $1); };
		   																		  $$.setExp($1._u.exp); }
		   		| listPart ',' listParts										{ $$.init(); 
																				  if ($1._empty) { CMPLELEMENTRY_EMPTY($1, @1); COMP_SET_SYNTAXELEM($1._u.u, $1._elem); $1.setExp(new ExprParseInfo(&PARSE_CONTEXT, @1, specialSymbolKnownVal, true)); }
																				  CMPLELEM_CONCAT($$, $1, $3);
																				  $$.setExp($1._u.exp->oper(&PARSE_CONTEXT, EXPR_INFO_OP_CAT, $3._u.exp, true)); }
		   		;

listPart		: /* empty */													{ $$.init(); COMP_LIT_BLANK($$._u.u); }
		  		| expression													{ $$ = $1; $1._u.exp->checkLNPError(&PARSE_CONTEXT); }
				;

intervalExpr	: expression intervalOp expression		%prec POINTS			{ $$.init(); CMPLELEMENTRY($$, SyntaxElementInterval, @1, @3); CMPLELEM_CHILD2($$, $1, $3); NOT_USED($2);
			 																	  $$.setExp($1._u.exp->oper(&PARSE_CONTEXT, EXPR_INFO_OP_SET, $3._u.exp, true));
																				  COMP_CONSTRUCT($$._elem, ICS_CONSTRUCT_IV_LB, 2); }
		  		| expression intervalOp					%prec POINTS			{ $$.init(); CMPLELEMENTRY($$, SyntaxElementInterval, @1, @2); CMPLELEMENTRY_EMPTY($2, @2); CMPLELEM_CHILD2($$, $1, $2);
																				  $$.setExp($1._u.exp->oper(&PARSE_CONTEXT, EXPR_INFO_OP_SET, NULL, false));
																				  COMP_CONSTRUCT($$._elem, ICS_CONSTRUCT_IV_LB, 1); }
		  		| intervalOp expression					%prec POINTS			{ $$.init(); CMPLELEMENTRY_EMPTY($1, @1); CMPLELEMENTRY($$, SyntaxElementInterval, @1, @2); CMPLELEM_CHILD2($$, $1, $2);
																				  $$.setExp($2._u.exp->oper(&PARSE_CONTEXT, EXPR_INFO_OP_SET, NULL, false));
																				  COMP_CONSTRUCT($$._elem, ICS_CONSTRUCT_IV_WOLB, 1); }
		  		| intervalOp							%prec POINTS			{ $$.init(); CMPLELEMENTRY($$, SyntaxElementInterval, @1, @1);
																				  CMPLELEMENTRY_EMPTY($1, @1); CMPLELEM_CHILD($$, $1);
																				  CMPLELEMENTRY_EMPTY($1, @1); CMPLELEM_CHILD($$, $1);
																				  $$.setExp(new ExprParseInfo(&PARSE_CONTEXT, @1));
																				  COMP_CONSTRUCT($$._elem, ICS_CONSTRUCT_IV_WOLB, 0); }
				;

intervalOp		: POINTS														{ $$ = $1; PARSE_CONTEXT.setCbExprModeReset(); }
				;

setExpr			: expression arrayExpr setExprEnd								{ $$.init(); CMPLELEMENTRY($$, SyntaxElementSet, @1, @3); CMPLELEM_CHILD2($$, $1, $2); CMPLELEM_CHILD($$, $3);
		  																		  if (!$2._u.exp->oneElemArr()) PARSE_CONTEXT.outParseError(ERROR_LVL_NORMAL, "step for algorithmic set must be a single value", @2);
		  																		  $$.setExp($1._u.exp->oper(&PARSE_CONTEXT, EXPR_INFO_OP_SET, $3._u.exp, true)); delete $2._u.exp;
																				  COMP_CONSTRUCT($$._elem, ICS_CONSTRUCT_SET, 3); }
		  		;

setExprEnd		: literal														{ $$ = $1; }
				| symbol														{ $$ = $1; }
				;


/*
 * operations
 */
unaryOp			: numOpSignAdd expression			%prec NUMBER_SIGN			{ $$.init(); CMPLELEMENTRY_TXT($$, SyntaxElementUnaryOp, @1, @2, ($1._u.b ? "+" : "-")); CMPLELEM_CHILD($$, $2); $$.setExp($2._u.exp->oper(&PARSE_CONTEXT, EXPR_INFO_OP_NUM, NULL, false));
		  																		  COMP_OPERATION($$._elem, ($1._u.b ? ICS_OPER_ADD : ICS_OPER_SUB), 1); }
				| numOpSignMult						%prec INDEX_SIGN			{ $$.init(); CMPLELEMENTRY_EMPTY($1, @1); CMPLELEMENTRY_TXT($$, SyntaxElementUnaryOp, @1, @1, ($1._u.b ? "*" : "/")); CMPLELEM_CHILD($$, $1); $$.setExp(new ExprParseInfo(&PARSE_CONTEXT, @1));
																				  COMP_OPERATION($$._elem, ($1._u.b ? ICS_OPER_MUL : ICS_OPER_DIV), 0); }
                | numOpSignMult expression 			%prec INDEX_SIGN			{ bool arr = $2._u.exp->expTypeArray(false);
                                                                                  $$.init(); CMPLELEMENTRY_TXT($$, SyntaxElementUnaryOp, @1, @2, ($1._u.b ? "*" : "/")); CMPLELEM_CHILD($$, $2); $$.setExp($2._u.exp->oper(&PARSE_CONTEXT, EXPR_INFO_OP_NUM, NULL, false));
                                                                                  if (arr) {
                                                                                    COMP_ARRCONSTRUCT_MODE($$._elem, $1._u.b);
																				  } else {
																					COMP_OPERATION($$._elem, ($1._u.b ? ICS_OPER_MUL : ICS_OPER_DIV), 1);
																				} }
				| expression OPERATOR_TRANSPOSE									{ $$.init(); CMPLELEMENTRY_TXT($$, SyntaxElementUnaryOp, @1, @2, $2._name); CMPLELEM_CHILD($$, $1); $$.setExp($1._u.exp->oper(&PARSE_CONTEXT, EXPR_INFO_OP_NUM, NULL, false));
																				  COMP_OPERATION($$._elem, ICS_OPER_TRP, 1); }
                | expression '^' OPERATOR_TRANSPOSE								{ $$.init(); CMPLELEMENTRY_TXT($$, SyntaxElementUnaryOp, @1, @3, $3._name); CMPLELEM_CHILD($$, $1); $$.setExp($1._u.exp->oper(&PARSE_CONTEXT, EXPR_INFO_OP_NUM, NULL, false));
                                                                                  COMP_OPERATION($$._elem, ICS_OPER_TRP, 1); }
                | logOpSignNot expression			%prec LOG_NOT				{ $$.init(); CMPLELEMENTRY_TXT($$, SyntaxElementUnaryOp, @1, @2, $1._name); CMPLELEM_CHILD($$, $2); $$.setExp($2._u.exp->oper(&PARSE_CONTEXT, EXPR_INFO_OP_LOG, NULL, false));
																				  COMP_OPERATION($$._elem, ICS_OPER_NOT, 1); }
		  		;

binaryOp		: expression numOpSignAdd expression		%prec '+'			{ $$.init(); CMPLELEMENTRY_TXT($$, SyntaxElementBinaryOp, @1, @3, ($2._u.b ? "+" : "-")); CMPLELEM_CHILD2($$, $1, $3); $$.setExp($1._u.exp->oper(&PARSE_CONTEXT, EXPR_INFO_OP_NUM, $3._u.exp, true));
		  																		  COMP_OPERATION($$._elem, ($2._u.b ? ICS_OPER_ADD : ICS_OPER_SUB), 2); }
		  		| expression numOpSignMult expression		%prec '*'			{ $$.init(); CMPLELEMENTRY_TXT($$, SyntaxElementBinaryOp, @1, @3, ($2._u.b ? "*" : "/")); CMPLELEM_CHILD2($$, $1, $3); $$.setExp($1._u.exp->oper(&PARSE_CONTEXT, EXPR_INFO_OP_NUM, $3._u.exp, true));
																				  COMP_OPERATION($$._elem, ($2._u.b ? ICS_OPER_MUL : ICS_OPER_DIV), 2); }
		  		| expression '^' expression										{ $$.init(); CMPLELEMENTRY_TXT($$, SyntaxElementBinaryOp, @1, @3, "^"); CMPLELEM_CHILD2($$, $1, $3); $$.setExp($1._u.exp->oper(&PARSE_CONTEXT, EXPR_INFO_OP_NUM, $3._u.exp, true));
                                                                                  COMP_OPERATION($$._elem, ICS_OPER_POW, 2); }
				| expression LOG_AND expression									{ $$.init(); CMPLELEMENTRY_TXT($$, SyntaxElementBinaryOp, @1, @3, $2._name); CMPLELEM_CHILD2($$, $1, $3); $$.setExp($1._u.exp->oper(&PARSE_CONTEXT, EXPR_INFO_OP_LOG_AND, $3._u.exp, true));
																				  COMP_OPERATION($$._elem, ICS_OPER_AND, 2); }
				| expression LOG_OR expression									{ $$.init(); CMPLELEMENTRY_TXT($$, SyntaxElementBinaryOp, @1, @3, $2._name); CMPLELEM_CHILD2($$, $1, $3); $$.setExp($1._u.exp->oper(&PARSE_CONTEXT, EXPR_INFO_OP_LOG, $3._u.exp, true));
																				  COMP_OPERATION($$._elem, ICS_OPER_OR, 2); }
				| expression INTERSECT_RR expression							{ $$.init(); CMPLELEMENTRY_TXT($$, SyntaxElementBinaryOp, @1, @3, $2._name); CMPLELEM_CHILD2($$, $1, $3); $$.setExp($1._u.exp->oper(&PARSE_CONTEXT, EXPR_INFO_OP_NUM, $3._u.exp, true));
																				  COMP_OPERATION($$._elem, ICS_OPER_IRR, 2); }
				| expression OBJ_TO symbol										{ $$.init(); CMPLELEMENTRY_TXT($$, SyntaxElementBinaryOp, @1, @3, $2._name); CMPLELEM_CHILD2($$, $1, $3); $$.setExp($1._u.exp->oper(&PARSE_CONTEXT, EXPR_INFO_OP_OBJTO, $3._u.exp, true));
																				  COMP_OPERATION($$._elem, ICS_OPER_OBJTO, 2); }
				| inOpPre expression						%prec OPERATOR_IN	{ $$.init(); CMPLELEMENTRY_TXT($$, SyntaxElementBinaryOp, @1, @2, "in"); CMPLELEM_CHILD2($$, $1, $2); $$.setExp($1._u.exp->oper(&PARSE_CONTEXT, EXPR_INFO_OP_IN, $2._u.exp, true));
																				  COMP_OPERATION($$._elem, ICS_OPER_IN, 2); }
				| ofOpPre expression						%prec OPERATOR_OF	{ $$.init(); CMPLELEMENTRY_TXT($$, SyntaxElementBinaryOp, @1, @2, "of"); CMPLELEM_CHILD2($$, $1, $2); $$.setExp($1._u.exp->oper(&PARSE_CONTEXT, EXPR_INFO_OP_IN, $2._u.exp, true));
																				  COMP_OPERATION($$._elem, ICS_OPER_OF, 2); }
				| literalMult expression					%prec '*'			{ $$.init(); CMPLELEMENTRY_ALT($$, SyntaxElementBinaryOp, @1, @2, "", "*"); CMPLELEM_CHILD2($$, $1, $2); $$.setExp($1._u.exp->oper(&PARSE_CONTEXT, EXPR_INFO_OP_NUM, $2._u.exp, true));
																				  COMP_OPERATION($$._elem, ICS_OPER_MUL, 2); }
		  		;

numOpSignAdd    : '+'															{ $$.init(); $$._u.b = true; PARSE_CONTEXT.setCbExprModeReset(); }
				| '-'															{ $$.init(); $$._u.b = false; PARSE_CONTEXT.setCbExprModeReset(); }
			 	;

numOpSignMult   : '*'															{ $$.init(); $$._u.b = true; PARSE_CONTEXT.setCbExprModeReset(); }
				| '/'															{ $$.init(); $$._u.b = false; PARSE_CONTEXT.setCbExprModeReset(); }
			 	;

logOpSignNot	: LOG_NOT														{ $$ = $1; PARSE_CONTEXT.setCbExprModeReset(); }
			 	;

inOpPre			: expression OPERATOR_IN										{ $$ = $1; NOT_USED($2); }
			  	| literalMult OPERATOR_IN										{ $$ = $1; NOT_USED($2); /* $1 doesn't mean multiplication here, but is delivered so by flex, because "in" is a keyword operator */ }
				;

ofOpPre			: expression OPERATOR_OF										{ $$ = $1; NOT_USED($2); }
			  	| literalMult OPERATOR_OF										{ $$ = $1; NOT_USED($2); /* $1 doesn't mean multiplication here, but is delivered so by flex, because "of" is a keyword operator */ }
				;

compareOp		: expression COMP_EQ expression									{ $$.init(); CMPLELEMENTRY_TXT($$, SyntaxElementCompareOp, @1, @3, $2._name); CMPLELEM_CHILD2($$, $1, $3); $$.setExp($1._u.exp->oper(&PARSE_CONTEXT, EXPR_INFO_OP_CMP, $3._u.exp, true));
		   																		  COMP_OPERATION($$._elem, ICS_OPER_EQ, 2); }
		   		| expression COMP_EQ2 expression								{ $$.init(); CMPLELEMENTRY_TXT($$, SyntaxElementCompareOp, @1, @3, $2._name); CMPLELEM_CHILD2($$, $1, $3); $$.setExp($1._u.exp->oper(&PARSE_CONTEXT, EXPR_INFO_OP_CMP2, $3._u.exp, true));
																				  COMP_OPERATION($$._elem, ICS_OPER_EQ2, 2); }
				| expression COMP_NE expression									{ $$.init(); CMPLELEMENTRY_TXT($$, SyntaxElementCompareOp, @1, @3, $2._name); CMPLELEM_CHILD2($$, $1, $3); $$.setExp($1._u.exp->oper(&PARSE_CONTEXT, EXPR_INFO_OP_CMP, $3._u.exp, true));
																				  COMP_OPERATION($$._elem, ICS_OPER_NE, 2); }
				| expression COMP_NE2 expression								{ $$.init(); CMPLELEMENTRY_TXT($$, SyntaxElementCompareOp, @1, @3, $2._name); CMPLELEM_CHILD2($$, $1, $3); $$.setExp($1._u.exp->oper(&PARSE_CONTEXT, EXPR_INFO_OP_CMP2, $3._u.exp, true));
																				  COMP_OPERATION($$._elem, ICS_OPER_NE2, 2); }
				| expression COMP_LT expression									{ $$.init(); CMPLELEMENTRY_TXT($$, SyntaxElementCompareOp, @1, @3, $2._name); CMPLELEM_CHILD2($$, $1, $3); $$.setExp($1._u.exp->oper(&PARSE_CONTEXT, EXPR_INFO_OP_CMP, $3._u.exp, true));
																				  COMP_OPERATION($$._elem, ICS_OPER_LT, 2); }
				| expression COMP_GT expression									{ $$.init(); CMPLELEMENTRY_TXT($$, SyntaxElementCompareOp, @1, @3, $2._name); CMPLELEM_CHILD2($$, $1, $3); $$.setExp($1._u.exp->oper(&PARSE_CONTEXT, EXPR_INFO_OP_CMP, $3._u.exp, true));
																				  COMP_OPERATION($$._elem, ICS_OPER_GT, 2); }
				| expression COMP_LE expression									{ $$.init(); CMPLELEMENTRY_TXT($$, SyntaxElementCompareOp, @1, @3, $2._name); CMPLELEM_CHILD2($$, $1, $3); $$.setExp($1._u.exp->oper(&PARSE_CONTEXT, EXPR_INFO_OP_CMP, $3._u.exp, true));
																				  COMP_OPERATION($$._elem, ICS_OPER_LE, 2); }
				| expression COMP_GE expression									{ $$.init(); CMPLELEMENTRY_TXT($$, SyntaxElementCompareOp, @1, @3, $2._name); CMPLELEM_CHILD2($$, $1, $3); $$.setExp($1._u.exp->oper(&PARSE_CONTEXT, EXPR_INFO_OP_CMP, $3._u.exp, true));
																				  COMP_OPERATION($$._elem, ICS_OPER_GE, 2); }
				;


subsymbolOp		: expression SUBSYM_OP subsymbolIncDec							{ $$.init(); CMPLELEMENTRY_TXT($$, SyntaxElementSubSymbolOp, @1, @3, "."); CMPLELEM_CHILD2($$, $1, $3); NOT_USED($2);
			 																	  $$.setExp($1._u.exp->oper(&PARSE_CONTEXT, EXPR_INFO_OP_SUB, $3._u.exp, true)); }
				;

subsymbolIncDec	: subsymbol														{ $$ = $1; }
				| INCDEC subsymbol												{ $$ = $2; CMPLELEMENTRY_TXT($$, SyntaxElementIncDec, @1, @2, ($1._u.i > 0 ? "prefix inc" : "prefix dec")); CMPLELEM_CHILD($$, $2);
																				  $$.setExp($2._u.exp->oper(&PARSE_CONTEXT, EXPR_INFO_OP_INC, NULL, false));
																				  COMP_INCDEC_FETCH($$._elem, true, $1._u.i); }
				| subsymbol INCDEC												{ $$ = $1; CMPLELEMENTRY_TXT($$, SyntaxElementIncDec, @1, @2, ($2._u.i > 0 ? "postfix inc" : "postfix dec")); CMPLELEM_CHILD($$, $1);
																				  $$.setExp($1._u.exp->oper(&PARSE_CONTEXT, EXPR_INFO_OP_INC, NULL, false));
																				  COMP_INCDEC_FETCH($$._elem, false, $2._u.i); }
				;

subsymbol		: SUBSYMBOL														{ $$ = $1; CMPLELEMENTRY_SYM($$, SyntaxElementSymbol, @1, @1, $1._u.exp->sym());
																				  COMP_FETCH_SYM($$._elem, $1._u.exp); $$._u.exp->setLvalueSyntaxElem($$._elem); }
				;


functionCall	: expression arrayExpr											{ $$.init(); CMPLELEMENTRY($$, SyntaxElementFuncCall, @1, @2); CMPLELEM_CHILD2($$, $1, $2); /* auch als Set-Konstruktor der Art 1(1)(n-1) */
			 																	  $$.setExp($1._u.exp->oper(&PARSE_CONTEXT, EXPR_INFO_OP_CALL, $2._u.exp, true));
																				  COMP_OPERATION($$._elem, ICS_OPER_FUNC, 2); }
		  		| expression codeblock											{ $$.init();
																				  if (PARSE_CONTEXT.modpt()->compatLineNamePref() && $1._u.exp->sym() && $1._u.exp->sym()->notDefined()) {
																					// no function call, $1 is a line name prefix
																					COMP_LNP_COMPAT($1._elem, $1._u.exp);
																					delete $1._u.exp; $$ = $2; $$._u.exp->setHasLNP(@1);
																					CMPLELEMENTRY($$, SyntaxElementLineNamePrefix, @1, @1); CMPLELEM_CHILD($$, $1); CMPLELEM_NEXT($$, $2);
																				  } else {
																				  	CMPLELEMENTRY($$, SyntaxElementFuncCall, @1, @2); CMPLELEM_CHILD2($$, $1, $2);
																				  	$$.setExp($1._u.exp->oper(&PARSE_CONTEXT, EXPR_INFO_OP_CALL, $2._u.exp, true));
																					COMP_OPERATION($$._elem, ICS_OPER_FUNC, 2);
																				} }
		  		;

indexation		: expression tupleExpr											{ $$.init(); CMPLELEMENTRY($$, SyntaxElementIndexation, @1, @2); CMPLELEM_CHILD2($$, $1, $2);
																				  $$.setExp($1._u.exp->oper(&PARSE_CONTEXT, EXPR_INFO_OP_IND, $2._u.exp, true));
																				  COMP_OPERATION($$._elem, ICS_OPER_INDEX, 2); }
				;

arraycast		: expression ARRCAST expression									{ $$.init(); NOT_USED($2);
		   																		  if ($1._u.exp->expTypeTuple()) {
																					CMPLELEMENTRY($$, SyntaxElementArrayCast, @1, @3); CMPLELEM_CHILD2($$, $1, $3);
																					$$.setExp($1._u.exp->oper(&PARSE_CONTEXT, EXPR_INFO_OP_ARRC, $3._u.exp, true));
																					COMP_OPERATION($$._elem, ICS_OPER_ARRCAST, 2);
																				  } else {
																				  	if ($3._u.exp->expTypeTuple()) {
																						// indexation
																						CMPLELEMENTRY($$, SyntaxElementIndexation, @1, @3); CMPLELEM_CHILD2($$, $1, $3);
																						$$.setExp($1._u.exp->oper(&PARSE_CONTEXT, EXPR_INFO_OP_IND, $3._u.exp, true));
																						COMP_OPERATION($$._elem, ICS_OPER_INDEX, 2);
																					} else if ($3._u.exp->expTypeArray()) {
																						// function call
																						CMPLELEMENTRY($$, SyntaxElementFuncCall, @1, @3); CMPLELEM_CHILD2($$, $1, $3);
																						$$.setExp($1._u.exp->oper(&PARSE_CONTEXT, EXPR_INFO_OP_CALL, $3._u.exp, true));
																						COMP_OPERATION($$._elem, ICS_OPER_FUNC, 2);
																					} else {
																						// error
																						CMPLELEMENTRY_TXT($$, SyntaxElementError, @1, @3, "illegal expression"); CMPLELEM_CHILD2($$, $1, $3);
																						$$.setExp($1._u.exp->oper(&PARSE_CONTEXT, EXPR_INFO_OP_NUM, $3._u.exp, true));
																					}
																				} }
				;

errorExpression : symbol symbol													{ $$.init(); CMPLELEMENTRY_TXT($$, SyntaxElementError, @1, @2, "illegal expression"); CMPLELEM_CHILD2($$, $1, $2); $$.setExp($1._u.exp->oper(&PARSE_CONTEXT, EXPR_INFO_OP_NUM, $2._u.exp, true)); }
				| symbol literal												{ $$.init(); CMPLELEMENTRY_TXT($$, SyntaxElementError, @1, @2, "illegal expression"); CMPLELEM_CHILD2($$, $1, $2); $$.setExp($1._u.exp->oper(&PARSE_CONTEXT, EXPR_INFO_OP_NUM, $2._u.exp, true)); }
				;

%%

void cmpl::CMPL_Parser::error(const cmpl::CMPL_Parser::location_type& l, const std::string& m)
{
	PARSE_CONTEXT.outParseError(ERROR_LVL_NORMAL, m, l, true);
}


