
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
#include "Compiler.hh"
#include "FileIO.hh"


/* By default yylex returns int, we use token_type.
Unfortunately yyterminate by default returns 0, which is
not of token_type.  */
#define yyterminate() return token::END

// Scanner verwendet YYSTYPE und YYLTYPE, waehrend Parser yy::CMPL_Parser::semantic_type und yy::CMPL_Parser::location_type verwendet
#define YYSTYPE cmpl::CMPL_Parser::semantic_type
#define YYLTYPE cmpl::CMPL_Parser::location_type

typedef cmpl::CMPL_Parser::token token;
%}

%option prefix="cmpl"
%option reentrant
%option bison-bridge bison-locations
%option noyywrap
%option ansi-prototypes
%option yylineno



/* 1.1.3 TS: the "incl" state is used for picking up the name of an include file */
%x incl
%x subsym


/* Definition von Abkuerzungen */
INT_CONST		[0-9]+
DOUBLE_CONST	(([0-9]+)|([0-9]*\.[0-9]+))([eE][-+]?[0-9]+)?
SYM_NAME		[a-zA-Z_][a-zA-Z0-9_]*
LINE_NAME		[a-zA-Z_$][a-zA-Z0-9_$]*

%%

	/* 1.1.3 TS: Behandlung von include (nach der Dokumentation von flex) */
include|INCLUDE				BEGIN(incl);		/* include beginnt */
<incl>[\ \t\015]*			;					/* Whitespace zwischen include und Dateiname überlesen */
<incl>\"[^\"]*\"			|
<incl>\'[^\']*\'			{ 					/* Dateiname der einzufügenden Datei */ 
								yyin = PARSE_CONTEXT.open_include_file(yylval);
								yypush_buffer_state(yy_create_buffer( yyin, YY_BUF_SIZE, yyg ), yyg );
								GLV->akt_src_line = 1;
								BEGIN(INITIAL); }
<<EOF>>						{ yypop_buffer_state( yyg );
								PARSE_CONTEXT.include_pop();
								if ( YY_CURRENT_BUFFER ) { GLV->akt_src_line = yylineno; }
								else { yyterminate(); } }


	/* program sections (es muss ":" folgen) */
param|parameters|const/:	{ return(token::SECT_PARAM); }
var|variables/:				{ return(token::SECT_VAR); }
objective|objectives/:		{ return(token::SECT_OBJ); }
constraint|constraints/:	{ return(token::SECT_CSTR); }

	/* funcions and  operators */
readstdin					{ return(token::READSTDIN); }
readcsv				     	{ return(token::READCSV); }

sqrt						{ yylval->i=F_SQRT; return(token::MATHFUNC);}
exp							{ yylval->i=F_EXP; return(token::MATHFUNC);}
ln							{ yylval->i=F_LOG; return(token::MATHFUNC);}
lg							{ yylval->i=F_LOG10; return(token::MATHFUNC);}
ld							{ yylval->i=F_LOG2; return(token::MATHFUNC);}
rand						{ yylval->i=F_RAND; return(token::MATHFUNC);}
sin							{ yylval->i=F_SIN; return(token::MATHFUNC);}
cos							{ yylval->i=F_COS; return(token::MATHFUNC);}
tan							{ yylval->i=F_TAN; return(token::MATHFUNC);}
acos						{ yylval->i=F_ACOS; return(token::MATHFUNC);}
asin						{ yylval->i=F_ASIN; return(token::MATHFUNC);}
atan						{ yylval->i=F_ATAN; return(token::MATHFUNC);}
sinh						{ yylval->i=F_SINH; return(token::MATHFUNC);}
cosh						{ yylval->i=F_COSH; return(token::MATHFUNC);}
tanh						{ yylval->i=F_TANH; return(token::MATHFUNC);}
abs							{ yylval->i=F_ABS; return(token::MATHFUNC);}
ceil						{ yylval->i=F_CEIL; return(token::MATHFUNC);}
floor						{ yylval->i=F_FLOOR; return(token::MATHFUNC);}
round						{ yylval->i=F_ROUND; return(token::MATHFUNC);}
srand						{ yylval->i=F_SEED; return(token::MATHFUNC);}

min							{ return(token::FCT_MIN); }
max							{ return(token::FCT_MAX); }
dim							{ return(token::DIM); }
def|count					{ return(token::DEF); }
div							{ GLV->exp_transp = 0; return(token::DIV); }
mod							{ GLV->exp_transp = 0; return(token::MOD); }
sum							{ return(token::SUM); }
len							{ return(token::LEN); }
format						{ return(token::FORMAT); }
type						{ return(token::TYPE); }
defset						{ return(token::DEFSET); }
element						{ GLV->exp_transp = 0; return(token::IN_SET); }

solve						{ return(token::PROBLEM_SOLVE); }
status						{ yylval->i=CMPL_PROBLEM_STAT; return(token::PROBLEM_FUNC); }
wait						{ yylval->i=CMPL_PROBLEM_WAIT; return(token::PROBLEM_FUNC); }
kill						{ yylval->i=CMPL_PROBLEM_KILL; return(token::PROBLEM_FUNC); }

	/* Variablentypen und Konstantentypen (in getypten Zuweisungen folgt ":=", deshalb das auch vorsehen, solange es die Regel "{SYM_NAME}/[[:space:]]*:=" noch gibt) */
real						{ return(token::TD_REAL); }
real/[[:space:]]*:=			{ return(token::TD_REAL); }
integer						{ return(token::TD_INT); }
integer/[[:space:]]*:=		{ return(token::TD_INT); }
binary						{ return(token::TD_BINARY); }
binary/[[:space:]]*:=		{ return(token::TD_BINARY); }
string						{ return(token::TD_STRING); }
string/[[:space:]]*:=		{ return(token::TD_STRING); }
set							{ return(token::TD_SET); }
set/[[:space:]]*:=			{ return(token::TD_SET); }
problem						{ return(token::TD_PROBLEM); }
problem/[[:space:]]*:=		{ return(token::TD_PROBLEM); }

	/* Error-Anweisung */
error						{ return(token::ERRMSG); }

	/* 1.1.3 stegger user defined message */
echo						{ return(token::USRMSG); }

	/* Schleifenkontrolle */
default						{ return(token::DEFAULT); }
	/* while					{ return(token::WHILE); } */	/* Nur hier als Token auskommentiert, ansonsten weiter verwendbar */
ref							{ return(token::REF); }
break						{ return(token::BREAK); }
continue					{ return(token::CONTINUE); }
repeat						{ return(token::REPEAT); }

	/* Zuweisung/Iteration */
":="						{ GLV->exp_transp = 0; return(token::ASSIGN); }
in							{ GLV->exp_transp = 0; return(token::ASSIGN_ITER); }

	/* Zahlenkonstante */
{INT_CONST}					{ return(PARSE_CONTEXT.test_number(yylval)); }
{DOUBLE_CONST}				{ sscanf(yytext, "%lf", &(yylval->d));
							  return(token::DOUBLE_NUMBER); }
null						{ return(token::NULL_VAL); }

	/* Namen */
{SYM_NAME}/[[:space:]]*:=	{ /* Symbol mit Zuweisung, fuer Zuweisung von temporaeren Konstanten in allen Abschnitten benoetigt */
							  cmpl::CMPL_Parser::token_type t = PARSE_CONTEXT.test_symbol(yylval, false);
							  return (t == token::DEFS_TMP ? token::DEFS_TMP_ASSIGN : t); }
{SYM_NAME}					{ return(PARSE_CONTEXT.test_symbol(yylval, false)); }
{LINE_NAME}					{ /*strncpy(GLV->last_str, yytext, COMP_STR_LEN);
							  GLV->last_str[COMP_STR_LEN] = '\0';
							  return(token::LINENAME_SYMBOL);*/
							  return(PARSE_CONTEXT.test_symbol(yylval, true)); }

	/* Indexbereich bzw. Angabe fuer Sets */
".."						{ GLV->exp_transp = 0; return(token::POINTS); }
".integer."					{ GLV->exp_transp = 0; return(token::SET_ALL_NUM); }
".string."			     	{ GLV->exp_transp = 0; return(token::SET_ALL_STR); }

	/* Angabe Optimierungsrichtung */
"->"						{ GLV->exp_transp = 0; return(token::OBJ_TO); }

	/* Vergleichsoperatoren */
"="							{ GLV->exp_transp = 0; return(cmpl::CMPL_Parser::token_type('=')); } /* auch Zuweisung (veraltet) */
"=="						{ GLV->exp_transp = 0; return(token::VERG_EQ2); }
"<>"						{ GLV->exp_transp = 0; return(token::VERG_NE); }
"!="						{ GLV->exp_transp = 0; return(token::VERG_NE2); }
"<"							{ GLV->exp_transp = 0; return(token::VERG_LT); }
">"							{ GLV->exp_transp = 0; return(token::VERG_GT); }
"<="						{ GLV->exp_transp = 0; return(token::VERG_LE); }
">="						{ GLV->exp_transp = 0; return(token::VERG_GE); }
"<<"						{ GLV->exp_transp = 0; return(token::IN_SET); }

"&&"						{ GLV->exp_transp = 0; return(token::LOG_AND); }
"||"						{ GLV->exp_transp = 0; return(token::LOG_OR); }

	/* Subsymbols */
"\."						{ BEGIN(subsym); return(token::SUBSYM_OP); }
<subsym>{SYM_NAME}			{ BEGIN(INITIAL); return PARSE_CONTEXT.test_subsymbol(yylval); }
<subsym>(#|"//").*						;
<subsym>"/*"([^*]|"*"+[^*/])*"*"+"/"	;
<subsym>[\ \t\015]*				;
<subsym>\n					{ COMP(SRC_LINE, 1);
							  GLV->akt_src_line = yylineno;
							  COMP_INT(GLV->akt_src_line); }
<subsym>.					{ BEGIN(INITIAL); return(cmpl::CMPL_Parser::token_type(*yytext)); /* wrong token, for error handling only */ }

	/* String */
\"[^\"]*\"					|
\'[^\']*\'					{ /* String als TP_ISTR einfuegen und Nummer an Parser */
                              cmpl::val_str val;
							  strncpy(GLV->last_str, yytext+1, COMP_STR_LEN); 
							  GLV->last_str[COMP_STR_LEN] = '\0';
							  GLV->last_str[strlen(GLV->last_str)-1] = '\0';
							  GLV->string_to_istr(&val, GLV->last_str, true);
							  yylval->i = val.v.i;
							  return(token::STRING); }

	/* arithmetische Klammer oder Beginn Wertliste bei Vektordefinition */
\(					 	{ /*if(GLV->is_scann_const_vekt &&
							GLV->index_info.i_akt_ind < GLV->index_info.i_free_ind) {
								GLV->index_info.i_akt_ind++;
								return(token::VEKT_BEG); }	// Beginn Wertliste
							else */ return(cmpl::CMPL_Parser::token_type('('));	}	/* arithmetische Klammer */

	/* Kommentar */
(#|"//"|%).*						;
"/*"([^*]|"*"+[^*/])*"*"+"/"	;

	/* Whitespaces */
[\ \t\015]*				;
	/* neue Zeile, Zeilennummer in den Zwischencode */
\n						{ COMP(SRC_LINE, 1);
						  GLV->akt_src_line = yylineno;		/* 1.1.3 TS: akt_src_line nicht mehr selber zählen, sondern aus yylineno übernehmen */
						  COMP_INT(GLV->akt_src_line); }

	/* uebrige Zeichen (Operatoren, Klammern, Semikolon, Doppelpunkt, Komma) */
\]						{ GLV->exp_transp = 1; return(cmpl::CMPL_Parser::token_type(*yytext)); }
.						{ GLV->exp_transp = 0; return(cmpl::CMPL_Parser::token_type(*yytext)); }


%%


namespace cmpl
{
	/* Startet den Scanner */
	void Compiler::scan_begin()
	{
		io_type_str *ios;

		/* Scanner bereitstellen */
		yylex_init(&scanner);

		/* Datei fuer Scanner setzen */
		ios = &(glvar->aufruf_p->input);

		/* 1.1.3: Initialisierung fuer include */
		include_init();

		/* 1.1.3: Eingabedatei als ersten Eintrag fuer include-Stack setzen */
		if (ios->filename == (char *)0)
		{
#ifdef CHDIR
			glvar->incl_dir_name[glvar->incl_stack_ptr] = glvar->dir_start_name;
#endif
			glvar->incl_file_name[glvar->incl_stack_ptr] = (char *)0;
			glvar->incl_stack_ptr++;

			include_set_filename((char *)0);
		}
		else
		{
			include_push(ios->filename);
		}
		
		/* Erstmal nur Datei als Eingabe moeglich */
		if (ios->file_fp != (FILE *)0)
			yyrestart (ios->file_fp, scanner);
	}

	/* Beendet den Scanner */
	void Compiler::scan_end()
	{
#ifdef CHDIR
		/* 1.1.3: Ausgangs-Directory wieder setzen */
		chdir(glvar->dir_start_name.c_str());
#endif

		/* Scanner wieder freigeben */
		yylex_destroy(scanner);
		scanner = (void *)0;
	}

	/* Test, ob ganze Zahl im int-Wertebereich liegt */
	CMPL_Parser::token_type Compiler::test_number(YYSTYPE *yyvalp)
	{
		char str[20];
		struct yyguts_t * yyg = (struct yyguts_t*)scanner;

		yyvalp->i = atoi(yytext);
		sprintf(str, "%d", yyvalp->i);
		if(strncmp(yytext, str, 20)==0) return(token::INT_NUMBER);
		sscanf(yytext, "%lf", &(yyvalp->d));
		return(token::DOUBLE_NUMBER);	/* wenn ganze Zahl zu gross fuer int, dann double */
	}


	/* Test, ob Transponierkennzeichen oder welche Symbolart */
	CMPL_Parser::token_type Compiler::test_symbol(YYSTYPE *yyvalp, bool linename)
	{ 
		struct yyguts_t * yyg = (struct yyguts_t*)scanner;

		// TODO: Transpose besser ueber scanner state
		if(glvar->exp_transp && strcmp(yytext, "T")==0) return(token::TRANSPOSE);

		// Symbol suchen und noetigenfalls Name einfuegen
		bool found; 
		int sym = glvar->symbol_main_names.search_insert(glvar, yytext, true, &found);
		PROTO_OUT_4("\n    searched for symbol %s, returns #%d (found:%d)", yytext, sym, (int)found);

		if (!found)
		{
			// Symbolinfo einfuegen, erstmal ohne Typ und ungueltig
			glvar->symbol_main_table.insert(glvar, glvar->symbol_main_names.elem(sym), sym, sym);
		}
		symbol_comp_str *symp = glvar->symbol_main_table.elem(sym);

		yyvalp->exp.set_sym(&(glvar->symbol_main_table), sym);

		if (linename)
		{
			// TODO: Spezialsymbole fuer Zeilennamenindex behandeln

			// Symbol, das nur Zeilenname sein kann, bekommt jetzt schon passenden Symboltyp, damit gleich richtiges Token
			symp->symtp = PTR_TP_LINENAME;
		}
		else
		{
			// Wenn lokales Symbol außerhalb des Gültigkeitsbereichs, dann hier als undefiniert zurück
			if (!(symp->valid))
				return (token::SYMBOL_UNDEF);
		}

		switch (symp->symtp)
		{
			case PTR_TP_CONST:
				//TODO: DEFS_TMP entfaellt?
				return (symp->local ? token::DEFS_TMP : token::SYMBOL_VALUE);
			case PTR_TP_VAR:
				return (token::SYMBOL_VAR);
			case PTR_TP_LINENAME:
				return (token::SYMBOL_LINENAME);
			default:
				return (token::SYMBOL_UNDEF);
		}


		/*
		int n;
		struct yyguts_t * yyg = (struct yyguts_t*)scanner;

		if(glvar->exp_transp && strcmp(yytext, "T")==0) return(token::TRANSPOSE);
		strncpy(glvar->last_str, yytext, SYM_NAME_LEN-1);
		glvar->last_str[SYM_NAME_LEN-1] = '\0';

		if((yyvalp->i = search_symbol(glvar->last_str, &n)) == -1) 
		{
			// Vorlauefig hier Symbol noch nicht einfügen, weil fuer temporaeres Symbol oder Zeilenname falsch
			yyvalp->exp.set((void*)0, TPP_EMPTY, 0, true, 0, 0, 0);
			return(token::SYMBOL_UNDEF);
		}
		else if(yyvalp->i < 0)
		{
			return(token::DEFS_TMP);		// Laufkonstante
		}
		else
		{
			// Erstmal yyvalp->exp.sym_p fuer die Symbolnummer verwenden
			// Vorlauefig wird hier dim_out = 0 verwendet, damit Symbole ohne folgende Indexangabe die richtige Dimension haben
			yyvalp->exp.set((void*)n, glvar->symbol_table[n].tp_sym, 0, true, 0, 0, 0);

			PROTO_OUT_2("  tp_sym=%d", (int)glvar->symbol_table[n].tp_sym);
			switch(glvar->symbol_table[n].tp_sym)
			{
				case TPK_VAR:	// Variable
					return(token::SYMBOL_VAR);
				case TPK_VALUE:	// Einfacher Wert
				case TPK_SET:	// Set
					return(token::SYMBOL_VALUE);

				default:		// Fehler wegen Nutzung Symbol, das noch in Definition ist
					SPRINT_ERR_2(ERROR_SYMBOL_INDEF, glvar->last_str);
					output_error_parse(ERROR_TYPE_COMP, ERROR_LVL_NORMAL, glvar->errbuf);
					return(token::SYMBOL_VALUE);
			}
		}
		*/
	}

	/* Test Subsymbol */
	CMPL_Parser::token_type Compiler::test_subsymbol(YYSTYPE *yyvalp)
	{ 
		struct yyguts_t * yyg = (struct yyguts_t*)scanner;

		// Subsymbol suchen und noetigenfalls Name einfuegen
		bool found; 
		int sym = glvar->symbol_sub_names.search_insert(glvar, yytext, true, &found);
		PROTO_OUT_4("\n    searched for subsymbol %s, returns #%d (found:%d)", yytext, sym, (int)found);

		yyvalp->i = sym;
		return (found ? token::SUBSYMBOL_VALUE : token::SUBSYMBOL_UNDEF);
	}


	FILE* Compiler::open_include_file(YYSTYPE *yyvalp)
	{
		FILE *incl_fp;

		/* Dateiname bestimmen */
		struct yyguts_t * yyg = (struct yyguts_t*)scanner;
		strncpy(glvar->last_str, yytext+1, COMP_STR_LEN);
		glvar->last_str[COMP_STR_LEN] = '\0';
		glvar->last_str[strlen(glvar->last_str)-1] = '\0';

	#ifdef CHDIR
		/* Pfad-Separator tauschen, damit in CMPL unabhaengig vom Betriebssystem immer '/' verwendet werden kann */
		if (glvar->dir_sep_char != '/')
		{
			char *p;
			for (p = glvar->last_str; *p != '\0'; p++)
			{
				if (*p == '/')
					*p = glvar->dir_sep_char;
			}
		}
	#endif

		PROTO_OUT_2("\ninclude '%s'", glvar->last_str);

		/* Datei oeffnen */
		incl_fp = fopen( glvar->last_str, "r" );
		if (! incl_fp)
		{
			/* File not found */
			SPRINT_ERR_2(ERROR_FILE_INPUT_INCL, glvar->last_str);
			output_error_parse(ERROR_TYPE_IO, ERROR_LVL_FATAL, glvar->errbuf);
		}

		/* Info auf den include-Stack */
		include_push(glvar->last_str);

		return incl_fp;
	}
}
