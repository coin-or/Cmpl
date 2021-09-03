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


%{
#include "CompilerContext.hh"


/* By default yylex returns int, we use token_type.
Unfortunately yyterminate by default returns 0, which is
not of token_type.  */
#define yyterminate() return token::END

// scanner uses YYSTYPE and YYLTYPE, but parser yy::Cmpl_Parser::semantic_type and yy::Cmpl_Parser::location_type
#define YYSTYPE cmpl::CMPL_Parser::semantic_type
#define YYLTYPE cmpl::CMPL_Parser::location_type

// handle locations
#define YY_USER_ACTION  yylloc->step(); yylloc->columns(yyleng);

// cmpl main object
#define glvar PARSE_CONTEXT.glvar

typedef cmpl::CMPL_Parser::token token;
int prevSC;
%}

%option prefix="cmpl"
%option reentrant
%option bison-bridge bison-locations
%option noyywrap
/*%option ansi-prototypes*/
%option yylineno
%option debug


	/* Scanner states */
		/* standard cmpl state */
%x SC_Cmpl
		/* scan cmpl input, possible assign modifier or directive */
%x SC_CmplAM
		/* after end of a codeblock: insert semicolon */
%x SC_CmplCBEndA
		/* after end of a codeblock: insert semicolon but not if '}' follow */
%x SC_CmplCBEndB
		/* after closing ']': possible array cast */
%x SC_ArrCast
		/* cmpl subsymbol expected */
%x SC_Subsym
		/* position mark */
%x SC_Position



INT_CONST		[0-9]+
DOUBLE_CONST	(([0-9]+)|([0-9]*\.[0-9]+))([eE][-+]?[0-9]+)?
SYM_NAME		[a-zA-Z_$][a-zA-Z0-9_]*

%%

	/* set initial scanner state */
	{
		if (YY_START == 0) {
			BEGIN(SC_CmplAM);
		}
	}


    /* position marks */
<*>^"##"/(([Dd]ef[Ll]oc)|([Ss]tart[Pp]os)|([Ll]ine)|([Cc]ol)):.*	{ PARSE_CONTEXT.setScanModeRestore(YY_START); BEGIN(SC_Position); }

<SC_Position>[Dd]ef[Ll]oc:.*										{ PARSE_CONTEXT.parseDefLocPM(yylloc, yytext+7); }
<SC_Position>[Ss]tart[Pp]os:.*										{ PARSE_CONTEXT.parseStartPosPM(yylloc, yytext+9); }
<SC_Position>[Ll]ine:.*												{ PARSE_CONTEXT.parseLinePM(yylloc, yytext+5); }
<SC_Position>[Cc]ol:.*												{ PARSE_CONTEXT.parseColPM(yylloc, yytext+4); }

<SC_Position>\n+													{ BEGIN(PARSE_CONTEXT.scanModeRestore()); yylloc->lines(yyleng-1); }
<SC_Position>.*														{ PARSE_CONTEXT.outErrorPositionMark(yylloc, yytext); }


	/* Whitespaces */
<*>[\ \t\015]+														;
<*>\n+																{ yylloc->lines(yyleng); }

	/* comments (only line comments starting with '#' are allowed here (the precompiler must handle c comments and c++ comments) */
<*>#.*																;

<SC_CmplCBEndA,SC_CmplCBEndB><<EOF>>								{ BEGIN(SC_CmplAM); yylloc->columns(-1); PARSE_CONTEXT.warnCBInsertEOS(yylloc); PARSE_CONTEXT.resetScanPrevTokenType(); yylval->s.init(); return(token::EOSTATEMENT); }
<SC_Cmpl,SC_CmplAM,SC_ArrCast,SC_Subsym,SC_Position><<EOF>>			{ yyterminate(); }


	/* cmpl keywords */
<SC_Cmpl,SC_CmplAM,SC_ArrCast>public/[[:space:]]*[([{]				|
<SC_Cmpl,SC_CmplAM,SC_ArrCast>public								{ if (YY_START==SC_ArrCast) { BEGIN(SC_Cmpl); } yylval->s.init(); yylval->s._u.i = (int)cmpl::modificatorPublic;   yylval->s._name = "public";   PARSE_CONTEXT.resetScanPrevTokenType(); return(token::MOD_ASSIGN); }
<SC_Cmpl,SC_CmplAM,SC_ArrCast>private/[[:space:]]*[([{]				|
<SC_Cmpl,SC_CmplAM,SC_ArrCast>private								{ if (YY_START==SC_ArrCast) { BEGIN(SC_Cmpl); } yylval->s.init(); yylval->s._u.i = (int)cmpl::modificatorPrivate;  yylval->s._name = "private";  PARSE_CONTEXT.resetScanPrevTokenType(); return(token::MOD_ASSIGN); }
<SC_Cmpl,SC_CmplAM,SC_ArrCast>local/[[:space:]]*[([{]				|
<SC_Cmpl,SC_CmplAM,SC_ArrCast>local									{ if (YY_START==SC_ArrCast) { BEGIN(SC_Cmpl); } yylval->s.init(); yylval->s._u.i = (int)cmpl::modificatorLocal;    yylval->s._name = "local";    PARSE_CONTEXT.resetScanPrevTokenType(); return(token::MOD_ASSIGN); }					
<SC_Cmpl,SC_CmplAM,SC_ArrCast>const/[[:space:]]*[([{]				|
<SC_Cmpl,SC_CmplAM,SC_ArrCast>const									{ if (YY_START==SC_ArrCast) { BEGIN(SC_Cmpl); } yylval->s.init(); yylval->s._u.i = (int)cmpl::modificatorConst;    yylval->s._name = "const";    PARSE_CONTEXT.resetScanPrevTokenType(); return(token::MOD_ASSIGN); }					
<SC_Cmpl,SC_CmplAM,SC_ArrCast>new/[[:space:]]*[([{]					|
<SC_Cmpl,SC_CmplAM,SC_ArrCast>new									{ if (YY_START==SC_ArrCast) { BEGIN(SC_Cmpl); } yylval->s.init(); yylval->s._u.i = (int)cmpl::modificatorNew;      yylval->s._name = "new";      PARSE_CONTEXT.resetScanPrevTokenType(); return(token::MOD_ASSIGN); }					
<SC_Cmpl,SC_CmplAM,SC_ArrCast>extern/[[:space:]]*[([{]				|
<SC_Cmpl,SC_CmplAM,SC_ArrCast>extern								{ if (YY_START==SC_ArrCast) { BEGIN(SC_Cmpl); } yylval->s.init(); yylval->s._u.i = (int)cmpl::modificatorExtern;   yylval->s._name = "extern";   PARSE_CONTEXT.resetScanPrevTokenType(); return(token::MOD_ASSIGN); }					
<SC_Cmpl,SC_CmplAM,SC_ArrCast>assert/[[:space:]]*[([{]				|
<SC_Cmpl,SC_CmplAM,SC_ArrCast>assert								{ if (YY_START==SC_ArrCast) { BEGIN(SC_Cmpl); } yylval->s.init(); yylval->s._u.i = (int)cmpl::modificatorAssert;   yylval->s._name = "assert";   PARSE_CONTEXT.resetScanPrevTokenType(); return(token::MOD_ASSIGN); }					
<SC_Cmpl,SC_CmplAM,SC_ArrCast>declare/[[:space:]]*[([{]				|
<SC_Cmpl,SC_CmplAM,SC_ArrCast>declare								{ if (YY_START==SC_ArrCast) { BEGIN(SC_Cmpl); } yylval->s.init(); yylval->s._u.i = (int)cmpl::modificatorDeclare;  yylval->s._name = "declare";  PARSE_CONTEXT.resetScanPrevTokenType(); return(token::MOD_ASSIGN); }					
<SC_Cmpl,SC_CmplAM,SC_ArrCast>initial/[[:space:]]*[([{]				|
<SC_Cmpl,SC_CmplAM,SC_ArrCast>initial								{ if (YY_START==SC_ArrCast) { BEGIN(SC_Cmpl); } yylval->s.init(); yylval->s._u.i = (int)cmpl::modificatorInitial;  yylval->s._name = "initial";  PARSE_CONTEXT.resetScanPrevTokenType(); return(token::MOD_ASSIGN); }
<SC_Cmpl,SC_CmplAM,SC_ArrCast>nocond/[[:space:]]*[([{]				|
<SC_Cmpl,SC_CmplAM,SC_ArrCast>nocond								{ if (YY_START==SC_ArrCast) { BEGIN(SC_Cmpl); } yylval->s.init(); yylval->s._u.i = (int)cmpl::modificatorNocond;   yylval->s._name = "nocond";   PARSE_CONTEXT.resetScanPrevTokenType(); return(token::MOD_ASSIGN); }
<SC_Cmpl,SC_CmplAM,SC_ArrCast>ref/[[:space:]]*[([{]					|
<SC_Cmpl,SC_CmplAM,SC_ArrCast>ref									{ if (YY_START==SC_ArrCast) { BEGIN(SC_Cmpl); } yylval->s.init(); yylval->s._u.i = (int)cmpl::modificatorRef;      yylval->s._name = "ref";      PARSE_CONTEXT.resetScanPrevTokenType(); return(token::MOD_ASSIGN); }
<SC_Cmpl,SC_CmplAM,SC_ArrCast>break/[[:space:]]*[([{]				|
<SC_Cmpl,SC_CmplAM,SC_ArrCast>break									{ if (YY_START==SC_ArrCast) { BEGIN(SC_Cmpl); } yylval->s.init(); yylval->s._u.i = (int)cmpl::modificatorBreak;    yylval->s._name = "break";    PARSE_CONTEXT.resetScanPrevTokenType(); return(token::MOD_ASSIGN); }					
<SC_Cmpl,SC_CmplAM,SC_ArrCast>continue/[[:space:]]*[([{]			|
<SC_Cmpl,SC_CmplAM,SC_ArrCast>continue								{ if (YY_START==SC_ArrCast) { BEGIN(SC_Cmpl); } yylval->s.init(); yylval->s._u.i = (int)cmpl::modificatorContinue; yylval->s._name = "continue"; PARSE_CONTEXT.resetScanPrevTokenType(); return(token::MOD_ASSIGN); }					
<SC_Cmpl,SC_CmplAM,SC_ArrCast>repeat/[[:space:]]*[([{]				|
<SC_Cmpl,SC_CmplAM,SC_ArrCast>repeat								{ if (YY_START==SC_ArrCast) { BEGIN(SC_Cmpl); } yylval->s.init(); yylval->s._u.i = (int)cmpl::modificatorRepeat;   yylval->s._name = "repeat";   PARSE_CONTEXT.resetScanPrevTokenType(); return(token::MOD_ASSIGN); }
<SC_Cmpl,SC_CmplAM,SC_ArrCast>return/[[:space:]]*[([{]				|
<SC_Cmpl,SC_CmplAM,SC_ArrCast>return								{ if (YY_START==SC_ArrCast) { BEGIN(SC_Cmpl); } yylval->s.init(); yylval->s._u.i = (int)cmpl::modificatorReturn;   yylval->s._name = "return";   PARSE_CONTEXT.resetScanPrevTokenType(); return(token::MOD_ASSIGN); }
<SC_Cmpl,SC_CmplAM,SC_ArrCast>ordered/[[:space:]]*[([{]				|
<SC_Cmpl,SC_CmplAM,SC_ArrCast>ordered								{ if (YY_START==SC_ArrCast) { BEGIN(SC_Cmpl); } yylval->s.init(); yylval->s._u.i = (int)cmpl::modificatorOrdered;  yylval->s._name = "ordered";  PARSE_CONTEXT.resetScanPrevTokenType(); return(token::MOD_ORDERED); }

<SC_Cmpl,SC_CmplAM,SC_ArrCast>in/[[:space:]]*[([{]					|
<SC_Cmpl,SC_CmplAM,SC_ArrCast>in									{ BEGIN(SC_Cmpl); yylval->s.init(); yylval->s._name = "in"; PARSE_CONTEXT.resetScanPrevTokenType(); return(token::OPERATOR_IN); }
<SC_Cmpl,SC_CmplAM,SC_ArrCast>of/[[:space:]]*[([{]					|
<SC_Cmpl,SC_CmplAM,SC_ArrCast>of									{ BEGIN(SC_Cmpl); yylval->s.init(); yylval->s._name = "of"; PARSE_CONTEXT.resetScanPrevTokenType(); return(token::OPERATOR_OF); }
<SC_Cmpl,SC_CmplAM,SC_ArrCast>T/[[:space:]]*[([{]                   |
<SC_Cmpl,SC_CmplAM,SC_ArrCast>T                                     { BEGIN(SC_Cmpl); yylval->s.init(); yylval->s._name = "^T"; PARSE_CONTEXT.resetScanPrevTokenType(); return(token::OPERATOR_TRANSPOSE); }

	/* symbols */
<SC_Cmpl,SC_CmplAM>{SYM_NAME}/[[:space:]]*[([{]						{ BEGIN(SC_Cmpl); yylval->s.init();
																	  yylval->s.setExp(PARSE_CONTEXT.searchInsertSymbolExp(yytext, *yylloc, false, true, cmpl::markForSymbolNameNo));
																	  PARSE_CONTEXT.setScanPrevTokenType(yylval->s._u.exp->sym()->valid() ? cmpl::scanPrevTokenDefSymbol : cmpl::scanPrevTokenUndefSymbol);
																	  return(token::SYMBOL); }
<SC_Cmpl,SC_CmplAM>[@\\]?{SYM_NAME}									{ cmpl::MarkForSymbolName mark = (yytext[0] == '\\' ? cmpl::markForSymbolNameReg : (yytext[0] == '@' ? cmpl::markForSymbolNameCB : cmpl::markForSymbolNameNo));
																	  yylval->s.init();
																	  yylval->s.setExp(PARSE_CONTEXT.searchInsertSymbolExp(yytext + (mark==cmpl::markForSymbolNameNo ? 0 : 1), *yylloc, false, true, mark));
																	  PARSE_CONTEXT.setScanPrevTokenType(yylval->s._u.exp->sym()->valid() || mark != cmpl::markForSymbolNameNo ? cmpl::scanPrevTokenDefSymbol : cmpl::scanPrevTokenUndefSymbol);
																	  if (YY_START == SC_CmplAM && mark == cmpl::markForSymbolNameNo && yylval->s._u.exp->sym()->visible()) {
																		cmpl::SpecialSymbol spec = yylval->s._u.exp->sym()->special();
																		if (SPECIALSYMBOL_IS_MODIFICATOR(spec)) { return(SPECIALSYMBOL_IS_OBJTYPE(spec) ? token::SYMBOL_OBJTYPE : token::SYMBOL_DATATYPE); }
																	  }
																	  BEGIN(SC_Cmpl);
																	  return(token::SYMBOL);
																	}

	/* array cast */
<SC_Cmpl,SC_CmplAM,SC_ArrCast>"]"									{ BEGIN(SC_ArrCast); yylval->s.init(); PARSE_CONTEXT.resetScanPrevTokenType(); return(token::TUPLE_END); }
<SC_ArrCast>[a-zA-Z_$0-9([{\"]										{ /* possible array cast, return token::ARRCAST here, and unput the current char to be scanned again */
																	  BEGIN(SC_Cmpl); unput(*yytext); yylloc->columns(-1); yylval->s.init(); return(token::ARRCAST); }
<SC_ArrCast>.														{ /* no array cast, unput the current char to be scanned again */
																	  BEGIN(SC_Cmpl); unput(*yytext); yylloc->columns(-1); /*no return*/ }

	/* subsymbols */
<SC_Cmpl,SC_CmplAM,SC_CmplCBEndA,SC_CmplCBEndB>"\."					{ BEGIN(SC_Subsym); yylval->s.init(); PARSE_CONTEXT.resetScanPrevTokenType(); return(token::SUBSYM_OP); }
<SC_Subsym>[\\]?{SYM_NAME}											{ BEGIN(SC_Cmpl); yylval->s.init();
																	  cmpl::MarkForSymbolName mark = (yytext[0] == '\\' ? cmpl::markForSymbolNameReg : cmpl::markForSymbolNameNo);
																	  yylval->s.setExp(PARSE_CONTEXT.searchInsertSymbolExp(yytext + (mark==cmpl::markForSymbolNameNo ? 0 : 1), *yylloc, true, true, mark));
																	  PARSE_CONTEXT.setScanPrevTokenType(cmpl::scanPrevTokenDefSymbol);
																	  return(token::SUBSYMBOL); }

	/* linename prefix */
<SC_Cmpl,SC_CmplAM>\`{SYM_NAME}										{ BEGIN(SC_Cmpl); yylval->s.init(); PARSE_CONTEXT.resetScanPrevTokenType(); yylval->s._u.s = PARSE_CONTEXT.data()->globStrings()->store(yytext+1); return(token::LINENAME_PREF); }

	/* literals */
<SC_Cmpl,SC_CmplAM>{INT_CONST}/[[:space:]]*[a-zA-Z_$]				{ BEGIN(SC_Cmpl); yylval->s.init(); PARSE_CONTEXT.resetScanPrevTokenType(); yylval->s._u.s = yytext; return(token::INT_NUMBER_MULT); }
<SC_Cmpl,SC_CmplAM>{DOUBLE_CONST}/[[:space:]]*[a-zA-Z_$]			{ BEGIN(SC_Cmpl); yylval->s.init(); PARSE_CONTEXT.resetScanPrevTokenType(); yylval->s._u.s = yytext; return(token::DOUBLE_NUMBER_MULT); }

<SC_Cmpl,SC_CmplAM>{INT_CONST}										{ BEGIN(SC_Cmpl); yylval->s.init(); PARSE_CONTEXT.resetScanPrevTokenType(); yylval->s._u.s = yytext; return(token::INT_NUMBER); }
<SC_Cmpl,SC_CmplAM>{DOUBLE_CONST}									{ BEGIN(SC_Cmpl); yylval->s.init(); PARSE_CONTEXT.resetScanPrevTokenType(); yylval->s._u.s = yytext; return(token::DOUBLE_NUMBER); }

<SC_Cmpl,SC_CmplAM>\"([^\"\n]|\\\")*\"								{ BEGIN(SC_Cmpl); yylval->s.init(); PARSE_CONTEXT.resetScanPrevTokenType(); yylval->s._u.s = yytext; return(token::STRING); }

	/* codeblock */
<SC_Cmpl,SC_CmplAM>"{"												{ BEGIN(SC_CmplAM); yylval->s.init(); yylval->s._u.sptt = PARSE_CONTEXT.scanPrevTokenType();
																	  PARSE_CONTEXT.startScannerScopeLevel();
																	  PARSE_CONTEXT.resetScanPrevTokenType(); return(token::CODEBLOCK_BEGIN); }
<SC_Cmpl,SC_CmplAM,SC_CmplCBEndB,SC_Subsym>"}"						{ switch (PARSE_CONTEXT.endScannerScopeLevel(yylloc)) {
																	  	case cmpl::codeBlockExprModeStatementStart: BEGIN(SC_CmplCBEndA); break;
																		case cmpl::codeBlockExprModeCBBodyStart:    BEGIN(SC_CmplCBEndB); break;
																		default: BEGIN(SC_CmplAM); break;
																	  }
																	  yylval->s.init(); PARSE_CONTEXT.resetScanPrevTokenType(); return(token::CODEBLOCK_END); }
<SC_Cmpl,SC_CmplAM,SC_CmplCBEndB>"|"								{ BEGIN(SC_CmplAM); yylval->s.init(); PARSE_CONTEXT.resetScanPrevTokenType(); return(token::CODEBLOCK_ALT); }

<SC_Cmpl,SC_CmplAM>"&+"												{ BEGIN(SC_Cmpl); yylval->s.init(); yylval->s._name = "&+"; yylval->s._u.b = false;  PARSE_CONTEXT.setScanPrevTokenType(cmpl::scanPrevTokenCBRef); return(token::REF_CODEBLOCK); }
<SC_Cmpl,SC_CmplAM>"&"												{ BEGIN(SC_Cmpl); yylval->s.init(); yylval->s._name = "&";  yylval->s._u.b = true; PARSE_CONTEXT.setScanPrevTokenType(cmpl::scanPrevTokenCBRefPure); return(token::REF_CODEBLOCK); }

<SC_CmplCBEndA,SC_CmplCBEndB>.										{ /* return token::EOSTATEMENT here, and unput the current char to be scanned again */
																	  if (*yytext != ';') { unput(*yytext); yylloc->columns(-1); PARSE_CONTEXT.warnCBInsertEOS(yylloc); }
																	  BEGIN(SC_CmplAM); yylval->s.init(); PARSE_CONTEXT.resetScanPrevTokenType(); PARSE_CONTEXT.setPrvEOSScopeLevel(); return(token::EOSTATEMENT); }

	/* operator signs */
<SC_Cmpl,SC_CmplAM>":="												{ BEGIN(SC_Cmpl); yylval->s.init(); yylval->s._name = ":=";  PARSE_CONTEXT.resetScanPrevTokenType(); return(token::ASSIGN); }
<SC_Cmpl,SC_CmplAM>"::="											{ BEGIN(SC_Cmpl); yylval->s.init(); yylval->s._name = "::="; PARSE_CONTEXT.resetScanPrevTokenType(); return(token::ASSIGN_ND); }
<SC_Cmpl,SC_CmplAM>"+="												{ BEGIN(SC_Cmpl); yylval->s.init(); yylval->s._name = "+=";  PARSE_CONTEXT.resetScanPrevTokenType(); return(token::ASSIGN_ND_ADD); }
<SC_Cmpl,SC_CmplAM>"-="												{ BEGIN(SC_Cmpl); yylval->s.init(); yylval->s._name = "-=";  PARSE_CONTEXT.resetScanPrevTokenType(); return(token::ASSIGN_ND_SUB); }
<SC_Cmpl,SC_CmplAM>"*="												{ BEGIN(SC_Cmpl); yylval->s.init(); yylval->s._name = "*=";  PARSE_CONTEXT.resetScanPrevTokenType(); return(token::ASSIGN_ND_MUL); }
<SC_Cmpl,SC_CmplAM>"/="												{ BEGIN(SC_Cmpl); yylval->s.init(); yylval->s._name = "/=";  PARSE_CONTEXT.resetScanPrevTokenType(); return(token::ASSIGN_ND_DIV); }

<SC_Cmpl,SC_CmplAM,SC_Subsym>"++"									{ yylval->s.init(); yylval->s._name = "++"; yylval->s._u.i =  1; PARSE_CONTEXT.resetScanPrevTokenType(); return(token::INCDEC); }
<SC_Cmpl,SC_CmplAM,SC_Subsym>"--"									{ yylval->s.init(); yylval->s._name = "--"; yylval->s._u.i = -1; PARSE_CONTEXT.resetScanPrevTokenType(); return(token::INCDEC); }

<SC_Cmpl,SC_CmplAM>":"												{ BEGIN(SC_CmplAM); yylval->s.init(); yylval->s._name = ":";	PARSE_CONTEXT.resetScanPrevTokenType();
																	  PARSE_CONTEXT.setScanPrevTokenType(cmpl::scanPrevTokenColon); return(token::COLON); }

<SC_Cmpl,SC_CmplAM>"="												{ BEGIN(SC_Cmpl); yylval->s.init(); yylval->s._name = "=";   PARSE_CONTEXT.resetScanPrevTokenType(); return(token::COMP_EQ); }
<SC_Cmpl,SC_CmplAM>"=="												{ BEGIN(SC_Cmpl); yylval->s.init(); yylval->s._name = "==";  PARSE_CONTEXT.resetScanPrevTokenType(); return(token::COMP_EQ2); }
<SC_Cmpl,SC_CmplAM>"<>"												{ BEGIN(SC_Cmpl); yylval->s.init(); yylval->s._name = "<>";  PARSE_CONTEXT.resetScanPrevTokenType(); return(token::COMP_NE); }
<SC_Cmpl,SC_CmplAM>"!="												{ BEGIN(SC_Cmpl); yylval->s.init(); yylval->s._name = "!=";  PARSE_CONTEXT.resetScanPrevTokenType(); return(token::COMP_NE2); }
<SC_Cmpl,SC_CmplAM>"<"												{ BEGIN(SC_Cmpl); yylval->s.init(); yylval->s._name = "<";   PARSE_CONTEXT.resetScanPrevTokenType(); return(token::COMP_LT); }
<SC_Cmpl,SC_CmplAM>">"												{ BEGIN(SC_Cmpl); yylval->s.init(); yylval->s._name = ">";   PARSE_CONTEXT.resetScanPrevTokenType(); return(token::COMP_GT); }
<SC_Cmpl,SC_CmplAM>"<="												{ BEGIN(SC_Cmpl); yylval->s.init(); yylval->s._name = "<=";  PARSE_CONTEXT.resetScanPrevTokenType(); return(token::COMP_LE); }
<SC_Cmpl,SC_CmplAM>">="												{ BEGIN(SC_Cmpl); yylval->s.init(); yylval->s._name = ">=";  PARSE_CONTEXT.resetScanPrevTokenType(); return(token::COMP_GE); }

<SC_Cmpl,SC_CmplAM>"&&"												{ BEGIN(SC_Cmpl); yylval->s.init(); yylval->s._name = "&&";  PARSE_CONTEXT.resetScanPrevTokenType(); return(token::LOG_AND); }
<SC_Cmpl,SC_CmplAM>"||"												{ BEGIN(SC_Cmpl); yylval->s.init(); yylval->s._name = "||";  PARSE_CONTEXT.resetScanPrevTokenType(); return(token::LOG_OR); }
<SC_Cmpl,SC_CmplAM>"!"												{ BEGIN(SC_Cmpl); yylval->s.init(); yylval->s._name = "!";   PARSE_CONTEXT.resetScanPrevTokenType(); return(token::LOG_NOT); }

<SC_Cmpl,SC_CmplAM>".."												{ BEGIN(SC_Cmpl); yylval->s.init(); yylval->s._name = "..";  PARSE_CONTEXT.resetScanPrevTokenType(); return(token::POINTS); }
<SC_Cmpl,SC_CmplAM>"*>"												{ BEGIN(SC_Cmpl); yylval->s.init(); yylval->s._name = "*>";  PARSE_CONTEXT.resetScanPrevTokenType(); return(token::INTERSECT_RR); }

<SC_Cmpl,SC_CmplAM>"->"												{ BEGIN(SC_Cmpl); yylval->s.init(); yylval->s._name = "->";  PARSE_CONTEXT.resetScanPrevTokenType(); return(token::OBJ_TO); }

<SC_Cmpl,SC_CmplAM,SC_Subsym>";"									{ BEGIN(SC_CmplAM); yylval->s.init(); PARSE_CONTEXT.resetScanPrevTokenType(); PARSE_CONTEXT.setPrvEOSScopeLevel(); return(token::EOSTATEMENT); }
	
	/* other characters */
<SC_Cmpl,SC_CmplAM,SC_Subsym>.										{ BEGIN(SC_Cmpl); yylval->s.init(); PARSE_CONTEXT.resetScanPrevTokenType(); return(cmpl::CMPL_Parser::token_type(*yytext)); }



%%

namespace cmpl
{
	/**
	 * Starts the scanner
	 */
	void CompilerContext::scanBegin()
	{
		// initialize the scanner
		yylex_init(&_scanner);
		cmplset_debug(_modp->traceScanning(), _scanner);

		GET_DATA(CmplSourceInput, inp);
		const char *buf = inp->contents().c_str();
		yy_scan_string (buf, _scanner);
	}

	/**
	 * Stops the scanner
	 */
	void CompilerContext::scanEnd()
	{
		// release scanner
		yylex_destroy(_scanner);
	}
}

