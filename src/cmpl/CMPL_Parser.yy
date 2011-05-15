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

/* C++ Parser-Klasse CMPL_Parser im namespace cmpl generieren */
%skeleton "lalr1.cc"
%require "2.1a"
%name-prefix="cmpl"
%defines
%define "parser_class_name" "CMPL_Parser"


%{
namespace cmpl
{
	class Compiler;
}
%}

%parse-param { Compiler& cmplcomp }
%lex-param   { yyscan_t CMPLCOMP_FOR_SCANNER }
%lex-param   { Compiler& cmplcomp }

%locations
%debug
%error-verbose


%{
// Include fuer Main.hh muss vor %union stehen, damit enum etp definiert ist
#include "Main.hh"

#define CMPLCOMP_FOR_SCANNER	cmplcomp.scanner

// Im Skeleton fehlt die Definition yyerrok, deshalb hier definieren
#define yyerrok     (yyerrstatus_ = 0)
%}


%union {
	int i;						/* Integerzahl, Symbolnummer, Anzahl, ... */
	double d;					/* Doublezahl */
	bool f;						/* true/false */
	enum etp t;					/* Konstantentyp */
	char *s;					/* String */
	void *p;					/* Pointer auf import-Struktur */
	struct parse_info_ti {		/* Konstantentyp und Anzahl */
		enum etp ti_t;
		int ti_i;
	} ti;
	struct int_str {			/* Integer und String */
		int i;
		char *s;
	} i_s;
}


%{
// Include fuer Compiler.hh darf erst nach %union stehen, damit darin schon cmpl::CMPL_Parser definiert ist
#include "Compiler.hh"
%}


/* Festlegung Prioritaet und Assoziativitaet fuer Operatoren */
%left ASSIGN ASSIGN_ITER
%left LOG_OR
%left LOG_AND
%left VERG_SET
%left '=' VERG_NE VERG_GE VERG_LE VERG_GT VERG_LT VERG_EQ2 VERG_NE2
%left IN_SET
%left '+' '-'
%left '*' '/' DIV MOD
%right '^'
%nonassoc VORZEICHEN '!'
%left POINTS

%token END	0	"end of file"
%token SECT_PARAM SECT_VAR SECT_OBJ SECT_CSTR
%token <i> UNDEF_SYMBOL DEFS_VAR DEFS_VALUE DEFS_SET DEFS_TMP DEFS_TMP_ASSIGN
%token <i> INT_NUMBER STRING
%token <d> DOUBLE_NUMBER
%token <i> MATHFUNC
%token READCSV READSTDIN
%token FCT_MIN FCT_MAX
%token DIM DEF SUM LEN FORMAT TYPE DEFSET
%token TD_REAL TD_INT TD_BINARY TD_STRING TD_SET
%token VEKT_BEG OBJ_TO TRANSPOSE ERRMSG USRMSG
%token LINENAME_SYMBOL
%token DEFAULT WHILE BREAK CONTINUE REPEAT
%token SET_ALL_NUM SET_ALL_STR
/*%token SET_START SET_END*/

%type <i> d_vekt vekt vekt_inds
%type <i> d_vekt_inds d_vekt_ind
%type <i> ul_sym_vekt d_undef_sym_vekt
%type <i> d_def_var_vekt def_var_vekt
%type <i> d_def_const_vekt def_const_vekt d_def_set_vekt def_set_vekt
%type <i> bel_sym_vekt
%type <i> verg_op var_type
%type <i> leer term_sgn start_list
%type <i> tterm t_expr const_list sep_list
%type <f> o_c_expr
%type <s> loop_name loop_for_var
%type <i> loop_mods loop_mod loop_for_assign
%type <t> const_type const_type_def loop_for_set
%type <t> vekt_def vekt_val
%type <t> const_expr const_expr_no_log all_expr literal_expr convert_expr function_expr
%type <t> num_op_expr set_op_expr log_op_expr cmp_op_expr cmp_set_op_expr loop_if
%type <t> const_loop_sum const_loop_set
%type <t> set_expr const_set_expr
%type <t> add_const_expr add_const_expr_alts count_const_expr count_const_expr_alts
%type <ti> vekt_val_list set_val_list
%type <i_s> loop_ctrl_ref

%%

/***************************************************************************************
 * Hauptabschnitte
 **************************************************************************************/
prog 	:										{ COMP(PROG_END, 0); }
		| prog_sec prog						
		;

/* Modellbeschreibung besteht aus vier Abschnitten, die in beliebiger Reihen-
   folge beliebig oft auftauchen koennen */
prog_sec: const_sec								/* Konstanten */
		| var_sec								/* Variablen */
		| obj_sec								/* Zielfunktionen */
		| cstr_sec								/* Restriktionen */
		;


/***************************************************************************************
 * const-Abschnitt : besteht aus Konstantendefinitionen
 **************************************************************************************/
const_sec:	SECT_PARAM 							{ GLV->sect = (int) token::SECT_PARAM; }
				':' const_defs
		;

const_defs:
		| const_def const_defs
		| error ';'								{ yyerrok; }
		const_defs
		;

/* Konstantendefinition oder Schleife damit */
const_def:	ul_sym_vekt const_type_def
				 const_set_op					{ GLV->index_info.i_akt_ind = 0;
												  GLV->is_scann_const_vekt = true; }
				 vekt_def ';'					{ COMP_SUB_PAR_CNT(GLV->last_bef, 0, $2, 1);
												  COMP_INT($1 +
												  		GLV->index_info.i_anz_ind);
												  GLV->is_scann_const_vekt = false;
												  if ($2 != TPP_EMPTY)
												  {
												  	/* Verschiedene Typklassen sind erstmal akzeptabel, es wird dann Konvertierung versucht */
													PARSE_CONTEXT.set_test_tpcl_symbol($1, TYPECLASS($2));
												  }
												  else
												  {
												  	/* Alle Werte muessen zur selben Typklasse gehoeren, sonst Fehler */
													if ($5 == TPP_MIX_TPK)
														PARSE_CONTEXT.output_error_parse(ERROR_TYPE_COMP, ERROR_LVL_NORMAL, ERROR_CONST_DEF_TPK_MIX);
													PARSE_CONTEXT.set_test_tpcl_symbol($1, TYPECLASS($5));
												  }
												}
		|	const_loop
		|	loop_ctrl ';'
		|	local_assign ';'
                /* 1.1.3 stegger user defined message -> err_msg changed to usr_err_message */
		|	usr_err_msg ';'
		;

ul_sym_vekt:d_undef_sym_vekt					{ GLV->last_bef = DEF_CONST;
												  $$ = $1; }
		|	d_def_const_vekt					{ GLV->last_bef = DEF_CONST;
												  $$ = $1; }
		|	d_def_set_vekt						{ GLV->last_bef = DEF_CONST;
												  $$ = $1; }
		;

/* Typangabe bei Konstantendefinition */
const_type_def:									{ $$ = TPP_EMPTY; }
		|	':' const_type						{ $$ = $2; }
		;

/* Konstantentyp */
const_type:	TD_REAL								{ $$ = TP_DBL; }
		|	TD_INT								{ $$ = TP_INT; }
		|	TD_BINARY							{ $$ = TP_BIN; }
		|	TD_STRING							{ $$ = TP_STR; }
		;

/* Zuweisungsoperator fuer Konstante */
const_set_op:	ASSIGN
		|	'='									{ PARSE_CONTEXT.out_warn_old_assign(false); }
		;

/* Schleife im const-Abschnitt */
const_loop: loop_start loop_header const_defs const_alts loop_stop
		;

const_alts: loop_alt const_defs const_alts
		|
		;


/***************************************************************************************
 * var-Abschnitt : besteht aus Variablendefinitionen
 **************************************************************************************/
var_sec	: SECT_VAR 								{ GLV->sect = (int) token::SECT_VAR; }
			':' var_defs
		;

var_defs:
		| var_def var_defs
		| error ';'								{ yyerrok; }
				var_defs
		;

/* Variablendefinition oder Schleife damit */
var_def	:	d_var_vekts ':' var_type ';'		{ COMP_SUB(DEF_VAR_TYPE, $3); }
		|	var_loop
		|	loop_ctrl ';'
		|	local_assign ';'
				/* 1.1.3 stegger user defined message -> err_msg changed to usr_err_message */
        |	usr_err_msg ';'
		;

d_var_vekts:d_var_vekt
		|	d_var_vekt ',' d_var_vekts
		;

d_var_vekt:	d_undef_sym_vekt		{ COMP(DEF_VAR, 1);
									  COMP_INT($1 + GLV->index_info.i_anz_ind); }
		|	d_def_var_vekt			{ COMP(DEF_VAR, 1);
									  COMP_INT($1 + GLV->index_info.i_anz_ind); }
		;

/* Variablentyp */
var_type:	TD_REAL var_defber					{ $$ = (int)TP_VAR_DBL; }
		|	TD_INT var_defber					{ $$ = (int)TP_VAR_INT; }
		|	TD_BINARY							{ $$ = (int)TP_VAR_BIN; }
		;

/* Definitionsbereich */
var_defber:										{ COMP(RESET_VAR_B, 0); }
		|	'[' o_c_expr POINTS o_c_expr ']'	{ COMP(RESET_VAR_B, 0);
												  if($4) COMP(DEF_UP_VAR, 0);
												  if($2) COMP(DEF_DN_VAR, 0); }
		;

/* Undefiniertes Symbol bei Konstanten- oder Variablenneudefinition */
d_undef_sym_vekt: UNDEF_SYMBOL 		{ $1 = PARSE_CONTEXT.insert_symbol(GLV->last_str,
										(GLV->sect == (int) token::SECT_PARAM ? TPP_EMPTY /* Typklasse wird spaeter gesetzt */ : TPK_VAR)); }
						d_vekt    	{ COMP(SET_INDEXES, 0);
									  GLV->index_info.i_anz_ind =
													$3 % MAX_ANZ_IND_1;
									  GLV->index_info.i_free_ind =
													$3 / MAX_ANZ_IND_1;
									  $$ = $1; }
		;

/* Schleife im var-Abschnitt */
var_loop: loop_start loop_header var_defs var_alts loop_stop
		;

var_alts: loop_alt var_defs var_alts
		|
		;


/***************************************************************************************
 * objective-Abschnitt : Beschreibung Zielfunktionen
 **************************************************************************************/
obj_sec: SECT_OBJ 								{ GLV->sect = (int) token::SECT_OBJ; }
			':' obj_defs
		;

obj_defs:
		| obj_def obj_defs
		| error ';'								{ yyerrok; }
				obj_defs
		;

/* Berechnungsvorschrift mit Zeilennamen oder Schleife */
obj_def:	line_name							{ GLV->is_left = true; }
				sum_line obj_def_to ';'			{ COMP_PAR(LINE_MOD, MODE_N);
												  COMP(LINE_END, 0); }
		|	obj_loop
		|	loop_ctrl ';'
		|	local_assign ';'
				/* 1.1.3 stegger user defined message -> err_msg changed to usr_err_message */
        |	usr_err_msg ';'
		;

/* 1.1.3 TS: Optionale Angabe der Optimierungsrichtung */
obj_def_to:
		| OBJ_TO FCT_MAX						{ COMP_PAR(LINE_OBJ_TO, 1); }
		| OBJ_TO FCT_MIN						{ COMP_PAR(LINE_OBJ_TO, -1); }
		;

/* Schleife im objective-Abschnitt */
obj_loop: loop_start loop_header obj_defs obj_alts loop_stop
		;

obj_alts: loop_alt obj_defs obj_alts
		|
		;


/***************************************************************************************
 * constraint-Abschnitt : Beschreibung Nebenbedingungen
 **************************************************************************************/
cstr_sec: SECT_CSTR 							{ GLV->sect = (int) token::SECT_CSTR; }
			':' cstr_defs
		;

cstr_defs:
		| cstr_def cstr_defs
		| error ';'								{ yyerrok; }
				cstr_defs
		;

/* Vergleich Berechnungsvorschriften oder Schleife */
cstr_def:	line_name cstr_line ';'				{ COMP(LINE_END, 0); }
		|	cstr_loop
		|	loop_ctrl ';'
		|	local_assign ';'
				/* 1.1.3 stegger user defined message -> err_msg changed to usr_err_message */
        |	usr_err_msg ';'
		;

/* auch mehrere Vergleiche in einer Anweisung */
cstr_line:	sum_line verg_op	 			{ switch($2) {
												case EQ:
											  		COMP_PAR(LINE_MOD, MODE_G); break;
												case GE:
											  		COMP_PAR(LINE_MOD, MODE_U); break;
												case LE:
											  		COMP_PAR(LINE_MOD, MODE_O); break;
											} }
					o_cstr_line
		;

o_cstr_line: sum_line
		| 	 cstr_line
		;

/* Berechnungsvorschrift besteht aus Summe von Termen */
sum_line: f_term o_terms
		;

f_term	: leer term
		| term_sgn term %prec VORZEICHEN
		| f_sum_term
		;

leer	:										{ $$ = 1; }
		;

term_sgn: '+'									{ $$ = 1; }
		| '-'									{ $$ = -1; }
		;

o_terms	:
		| o_term o_terms
		;

o_term	: term_sgn term %prec VORZEICHEN
		| sum_term
		;

/* sum-Term enthaelt Schleife */
/* 1.1.3 TS: Vorzeichen vor Summe erlauben (ist so nicht richtig schoen, aber ja auch nur vorlaeufig) */
f_sum_term: loop_sum_start loop_header sum_line sum_line_alts loop_stop
		| sum_term
		;

sum_term: '+' loop_sum_start loop_header sum_line sum_line_alts loop_stop
		| '-' loop_sum_start												{ COMP(TERM_NEG, 0); /* Negation */ }
				loop_header sum_line sum_line_alts loop_stop				{ COMP(TERM_NEG, 0); /* Negation (um Negation am Anfang zurueckzunehmen) */ }
		;

sum_line_alts: loop_alt sum_line sum_line_alts
		|
		;


/* Term besteht aus Produkt von Konstanten und Variablen */
term	: 							{ COMP(TERM_BEG, 0);
									  COMP_PAR(LIT_VAL_SHORT, $<i>0); /* Vorzeichen Term */ }
			tterm					{ COMP(TERM_END, 0);
									  PARSE_CONTEXT.var_test_prod($2);
									}
		;

tterm	: t_expr					{ if($1==0) COMP_SUB(NUM_OP, MULT);
									  $$ = $1; }
		| t_expr '*' tterm			{ if($1==0) COMP_SUB(NUM_OP, MULT);
									  $$ = $1 + $3; }
		;

t_expr	:  	d_def_var_vekt transp	{ COMP(TERM_VAR_FREE, 1);
									  COMP_INT($1 + GLV->index_info.i_anz_ind);
									  $$ = 1; }
		|	all_expr				{ $$ = 0; }
		|	d_def_const_vekt transp	{ COMP(F_CONST_FREE, 1);
									  COMP_INT($1 + GLV->index_info.i_anz_ind);
									  $$ = 0; }
		;

transp	:
		|	TRANSPOSE				{ COMP(IND_TRANSPOSE, 0); }
		;

/* Schleife im constraint-Abschnitt */
cstr_loop: loop_start loop_header cstr_defs cstr_alts loop_stop
		;

cstr_alts: loop_alt cstr_defs cstr_alts
		|
		;


/***************************************************************************************
 * Allgemeine Ausdruecke
 **************************************************************************************/
/* Konstante Ausdruecke, die ueberall verwendet werden koennen */
all_expr:	literal_expr						{ $$ = $1; }
		|	convert_expr						{ $$ = $1; }					
		|	function_expr						{ $$ = $1; }
		|	'(' const_expr ')'					{ $$ = $2; }
		|	DEFS_TMP							{ COMP(FETCH_TMP, 1);
												  COMP_INT((-$1-2));
												  $$ = TPK_VALUE; }
		;

/* Literale Angabe eines Werts fuer einen konstanten Ausdruck */
literal_expr: INT_NUMBER						{ COMP_PAR_INT(LIT_VAL, $1);
												  $$ = TP_INT; }
		|	DOUBLE_NUMBER						{ COMP(LIT_VAL, 1);
												  COMP_DOUBLE($1);
												  $$ = TP_DBL; }
		|	STRING								{ COMP(LIT_VAL, 1);
												  COMP_ISTR($1);
												  $$ = TP_ISTR; }
		;

convert_expr: const_type '(' const_set_expr ')'	{ COMP_SUB_PAR(CONV_OP, NO_FORMAT, $1);
												  $$ = $1; }
		;

function_expr:	FCT_MAX start_list				{ $2 = (int) token::FCT_MAX; }
				 const_list ')'					{ COMP_PAR_INT(LIT_VAL, $4);
												  COMP_SUB(AGGR_OP, MAX_VAL);
												  $$ = TPK_VALUE; }
		|	FCT_MIN start_list					{ $2 = (int) token::FCT_MIN; }
				 const_list ')'					{ COMP_PAR_INT(LIT_VAL, $4);
												  COMP_SUB(AGGR_OP, MIN_VAL);
												  $$ = TPK_VALUE; }
		|	DIM '(' bel_sym_vekt ')'			{ COMP(SET_INDEXES, 0);
												  COMP_SUB_PAR_CNT(DEFSET_EXTNUM, MAX_VAL, 0, 1);
												  COMP_INT($3);
												  $$ = TP_INT; }
		|	DEF '(' func_def_par ')'			{ $$ = TP_INT; }
		|	FORMAT '(' const_expr
				',' const_set_expr ')'			{ COMP_SUB_PAR(CONV_OP, WITH_FORMAT, TP_ISTR);
												  $$ = TP_ISTR; }
		|	TYPE '(' const_set_expr ')'			{ COMP_SUB(TYPE_STR, TYPE_EXPR);
												  $$ = TP_ISTR; }
		|	TYPE '(' def_var_vekt ')'			{ COMP_SUB_PAR_CNT(TYPE_STR, TYPE_VAR, 0, 1);
												  COMP_INT($3);
												  $$ = TP_ISTR; }
		|	LEN '(' const_set_expr ')'			{ COMP_SUB(UTIL_OP, LEN_VAL);
												  $$ = TP_INT; }
		|	MATHFUNC '(' const_expr ')'			{ COMP_SUB(MATH_FUNC, $1);
												  $$ = TPU_NUM; }
		|	READSTDIN '(' const_expr ')'		{ COMP(READ_STDIN, 0);
												  $$=TPK_VALUE; }
		;

/* Oeffnende Klammer als Listenstart: Separate Regel, damit Wert zuweisbar */
start_list: '('								{ $$ = 0; }
		;

/* Variable mit Indizes */
def_var_vekt:	DEFS_VAR vekt				{ $$ = $1 + $2; }
		;

/* Variable auch mit freien Indizes */
d_def_var_vekt:	DEFS_VAR d_vekt				{ COMP(SET_INDEXES, 0);
											  GLV->index_info.i_anz_ind =
											  		$2 % MAX_ANZ_IND_1;
											  GLV->index_info.i_free_ind =
											  		$2 / MAX_ANZ_IND_1;
											  $$ = $1; }
		;

/* Liste aus Werten (fuer max, min) */
const_list:	const_elem						{ $$ = 1; }
		|	const_elem sep_list				{ $2 = $<i>-1; }
					const_list				{ $$ = $4 + 1; }
		;

/* Komma als Listenseparator: Separate Regel, damit Wert zuweisbar */
sep_list: ','								{ $$ = 0; }
		;

/* Als Element auch Konstanten mit freien Indizes moeglich */
const_elem:	all_expr
		|	DEFS_VALUE d_vekt				{ COMP(SET_INDEXES, 0);
											  COMP(FETCH_VALS, 1);
											  COMP_INT($1 +
											  		$2 % MAX_ANZ_IND_1);
											  if($<i>-1 == (int) token::FCT_MAX) COMP_SUB(AGGR_OP, MAX_VAL_NULL);
											  else COMP_SUB(AGGR_OP, MIN_VAL_NULL); }
		;

/* Vektor fuer dim */
bel_sym_vekt:	DEFS_VAR d_vekt			{ $$ = $1 + $2 % MAX_ANZ_IND_1; }
		|		DEFS_VALUE d_vekt		{ $$ = $1 + $2 % MAX_ANZ_IND_1; }
		|		DEFS_SET d_vekt			{ $$ = $1 + $2 % MAX_ANZ_IND_1; }
		;

/* Parameter fuer def */
func_def_par:	bel_sym_vekt					{ COMP(SET_INDEXES, 0);
												  COMP(DEF_FUNC, 1);
												  COMP_INT($1); }
		|		UNDEF_SYMBOL d_vekt				{ COMP(SET_INDEXES, 0);
												  COMP(DEL_INDEXES, 0);
												  COMP_PAR(LIT_VAL_SHORT, 0); }
		|		DEFS_TMP						{ COMP_PAR(LIT_VAL_SHORT, 1); }
		;


/* Indizes, auch mit freien Indizes */
d_vekt:									{ COMP_PAR(LIT_VAL_SHORT, 0);
										  $$ = 0; }
		|	'[' d_vekt_inds ']'			{ COMP_PAR_INT(LIT_VAL, ($2 % MAX_ANZ_IND_1));
										  $$ = $2; }
		;

d_vekt_inds:d_vekt_ind						{ $$ = $1 + 1; }
		|	d_vekt_ind ',' d_vekt_inds		{ $$ = $1 + 1 + $3;
											  if($$ % MAX_ANZ_IND_1 == 0)
											  	PARSE_CONTEXT.output_error_parse(ERROR_TYPE_COMP, ERROR_LVL_NORMAL, ERROR_INDIZES_MAX); }
		;

d_vekt_ind:								{ COMP_PAR(LIT_SET, TP_SET_ALL);
										  $$ = MAX_ANZ_IND_1; }
		|	const_set_expr				{ $$ = (TYPECLASS($1) == TPK_SET ? MAX_ANZ_IND_1 : 0); }
/*										  COMP_PAR(LIT_VAL_SHORT, IND_ONE); }*/
/*		|	o_c_expr POINTS o_c_expr	{ int ind_type;
										  if($1 && $3) ind_type = IND_DNUP_B;
										  else if($1) ind_type = IND_DN_B;
										  else if($3) ind_type = IND_UP_B;
										  else ind_type = IND_FREE;
										  COMP_PAR(LIT_VAL_SHORT, ind_type);
										  $$ = MAX_ANZ_IND_1; }
*/
		;

/* optionaler konstanter Ausdruck */
o_c_expr:								{ $$ = false; }
		|	const_expr_no_log			{ $$ = true; }
		;

/* konstanter Ausdruck */
const_expr:	const_expr_no_log					{ $$ = $1; }
		|	log_op_expr							{ $$ = $1; }
		|	cmp_op_expr							{ $$ = $1; }
		|	cmp_set_op_expr						{ $$ = $1; }
		;

	/* Benoetigt, um Konflikte in der Grammatik zu vermeiden. Schraenkt moegliche Ausdruecke aber nicht ein, da logische Operatoren
	   und Vergleichsoperatoren sowieso niedrigere Prioritaet haben als die Regeln, in denen const_expr_no_log verwendet wird */
const_expr_no_log:
			all_expr							{ $$ = $1; }
		|	num_op_expr							{ $$ = $1; }
		|	def_const_vekt						{ COMP(FETCH_CONST, 1);
												  COMP_INT($1);
												  $$ = TPK_VALUE; }
		|	const_loop_sum						{ $$ = $1; }
		|	'!' const_expr_no_log				{ COMP_SUB(LOG_OP, NOT);
												  $$ = TP_BIN; }
		;

const_set_expr: const_expr						{ $$ = $1; }
		|	set_expr							{ $$ = $1; }
	/*		|	const_loop_sum						{ $$ = $1; } */
		;

const_loop_sum: loop_sum_start					{ COMP(LIT_VAL, 1);
												  COMP_VAL(TPP_NULL, i, 0); }
			loop_header	add_const_expr add_const_expr_alts loop_stop
												{ $$ = PARSE_CONTEXT.parse_type_dev_num_op(ADD, TPP_NULL, PARSE_CONTEXT.parse_type_dev_switch($4, $5)); }
		;

const_loop_set: loop_set_start					{ COMP_PAR(LIT_VAL_SHORT, 0); }
			loop_header count_const_expr count_const_expr_alts loop_stop
												{ $$ = PARSE_CONTEXT.parse_type_dev_num_op(ADD, TP_INT, PARSE_CONTEXT.parse_type_dev_switch($4, $5));
												  if (TYPECLASS($$) != TPK_VALUE)
													PARSE_CONTEXT.output_error_parse(ERROR_TYPE_COMP, ERROR_LVL_NORMAL, ERROR_NOT_A_NUMBER_PARSE);
												  COMP_PAR(LIT_SET, TP_SET_ENUM);
												  $$ = TPK_SET; }
		;

num_op_expr:
			const_expr_no_log '+' const_expr_no_log			{ COMP_SUB(NUM_OP, ADD);
															  $$ = PARSE_CONTEXT.parse_type_dev_num_op(ADD, $1, $3); }
		|	const_expr_no_log '-' const_expr_no_log			{ COMP_SUB(NUM_OP, SUB);
															  $$ = PARSE_CONTEXT.parse_type_dev_num_op(SUB, $1, $3); }
		|	const_expr_no_log '*' const_expr_no_log			{ COMP_SUB(NUM_OP, MULT);
															  $$ = PARSE_CONTEXT.parse_type_dev_num_op(MULT, $1, $3); }
		|	const_expr_no_log '/' const_expr_no_log			{ COMP_SUB(NUM_OP, DIVD);
															  $$ = PARSE_CONTEXT.parse_type_dev_num_op(DIVD, $1, $3); }
		|	const_expr_no_log '^' const_expr_no_log			{ COMP_SUB(NUM_OP, POW);
															  $$ = PARSE_CONTEXT.parse_type_dev_num_op(POW, $1, $3); }
		|	const_expr_no_log DIV const_expr_no_log			{ COMP_SUB(NUM_OP, DIVI);
															  $$ = PARSE_CONTEXT.parse_type_dev_num_op(DIVI, $1, $3); }
		|	const_expr_no_log MOD const_expr_no_log			{ COMP_SUB(NUM_OP, MODI);
															  $$ = PARSE_CONTEXT.parse_type_dev_num_op(MODI, $1, $3); }
		|   '-' const_expr_no_log %prec VORZEICHEN     		{ COMP_SUB(NUM_OP, NEG);
															  $$ = PARSE_CONTEXT.parse_type_dev_num_op(NEG, $2, TPP_EMPTY); }
		|	'+' const_expr_no_log %prec VORZEICHEN			{ COMP_SUB_PAR(CONV_OP, NO_FORMAT, TPU_NUM);	/* fuer sichern nummerischer Wert */
															  $$ = PARSE_CONTEXT.parse_type_dev_num_op(NEG, $2, TPP_EMPTY); }
		;

set_op_expr:
			set_expr '+' set_expr							{ COMP_SUB(SET_OP, ADD);
															  $$ = TPK_SET; }
		|	set_expr '*' set_expr							{ COMP_SUB(SET_OP, MULT);
															  $$ = TPK_SET; }
		;

log_op_expr:
			const_expr LOG_AND const_expr					{ COMP_SUB(LOG_OP, AND);
															  $$ = TP_BIN; }
		|	const_expr LOG_OR const_expr					{ COMP_SUB(LOG_OP, OR);
															  $$ = TP_BIN; }
		;

cmp_op_expr:
			const_expr_no_log '=' const_expr_no_log			{ COMP_SUB(CMP_OP, EQ);
															  $$ = PARSE_CONTEXT.parse_type_dev_cmp_op(EQ, $1, $3); }
		|	const_expr_no_log VERG_EQ2 const_expr_no_log	{ COMP_SUB(CMP_OP, EQ2);
															  $$ = PARSE_CONTEXT.parse_type_dev_cmp_op(EQ2, $1, $3); }
		|	const_expr_no_log VERG_NE const_expr_no_log		{ COMP_SUB(CMP_OP, EQ); COMP_SUB(LOG_OP, NOT);
															  $$ = PARSE_CONTEXT.parse_type_dev_cmp_op(EQ, $1, $3); }
		|	const_expr_no_log VERG_NE2 const_expr_no_log	{ COMP_SUB(CMP_OP, EQ2); COMP_SUB(LOG_OP, NOT);
															  $$ = PARSE_CONTEXT.parse_type_dev_cmp_op(EQ2, $1, $3); }
		|	const_expr_no_log VERG_LE const_expr_no_log		{ COMP_SUB(CMP_OP, LE);
															  $$ = PARSE_CONTEXT.parse_type_dev_cmp_op(LE, $1, $3); }
		|	const_expr_no_log VERG_GT const_expr_no_log		{ COMP_SUB(CMP_OP, LE); COMP_SUB(LOG_OP, NOT);
															  $$ = PARSE_CONTEXT.parse_type_dev_cmp_op(GT, $1, $3); }
		|	const_expr_no_log VERG_GE const_expr_no_log		{ COMP_SUB(CMP_OP, GE);
															  $$ = PARSE_CONTEXT.parse_type_dev_cmp_op(GE, $1, $3); }
		|	const_expr_no_log VERG_LT const_expr_no_log		{ COMP_SUB(CMP_OP, GE); COMP_SUB(LOG_OP, NOT);
															  $$ = PARSE_CONTEXT.parse_type_dev_cmp_op(LT, $1, $3); }
		;

cmp_set_op_expr:
			set_expr '=' set_expr %prec VERG_SET			{ COMP_SUB(CMP_OP, EQ);
															  $$ = PARSE_CONTEXT.parse_type_dev_cmp_op(EQ, $1, $3); }
		|	set_expr VERG_EQ2 set_expr %prec VERG_SET		{ COMP_SUB(CMP_OP, EQ2);
															  $$ = PARSE_CONTEXT.parse_type_dev_cmp_op(EQ2, $1, $3); }
		|	set_expr VERG_NE set_expr %prec VERG_SET		{ COMP_SUB(CMP_OP, EQ); COMP_SUB(LOG_OP, NOT);
															  $$ = PARSE_CONTEXT.parse_type_dev_cmp_op(EQ, $1, $3); }
		|	set_expr VERG_NE2 set_expr %prec VERG_SET		{ COMP_SUB(CMP_OP, EQ2); COMP_SUB(LOG_OP, NOT);
															  $$ = PARSE_CONTEXT.parse_type_dev_cmp_op(EQ2, $1, $3); }
		|	set_expr VERG_LE set_expr %prec VERG_SET		{ COMP_SUB(CMP_OP, LE);
															  $$ = PARSE_CONTEXT.parse_type_dev_cmp_op(LE, $1, $3); }
		|	set_expr VERG_LT set_expr %prec VERG_SET		{ COMP_SUB(CMP_OP, LT);
															  $$ = PARSE_CONTEXT.parse_type_dev_cmp_op(LT, $1, $3); }
		|	set_expr VERG_GE set_expr %prec VERG_SET		{ COMP_SUB(CMP_OP, GE);
															  $$ = PARSE_CONTEXT.parse_type_dev_cmp_op(GE, $1, $3); }
		|	set_expr VERG_GT set_expr %prec VERG_SET		{ COMP_SUB(CMP_OP, GT);
															  $$ = PARSE_CONTEXT.parse_type_dev_cmp_op(GT, $1, $3); }
		|	const_expr_no_log IN_SET set_expr				{ COMP_SUB(CMP_OP, INSET);
															  $$ = PARSE_CONTEXT.parse_type_dev_cmp_op(INSET, $1, $3); }
		;

add_const_expr: const_set_expr								{ COMP_SUB(NUM_OP, ADD);
												 			  $$ = $1; }
		;

add_const_expr_alts: loop_alt add_const_expr add_const_expr_alts
															{ $$ = PARSE_CONTEXT.parse_type_dev_switch($2, $3); }
		|													{ $$ = TPP_EMPTY; }
		;

count_const_expr: const_set_expr							{ COMP_SUB(COUNT_OP, CNT_INC);
												 			  $$ = $1; }
		;

count_const_expr_alts: loop_alt count_const_expr add_const_expr_alts
															{ $$ = PARSE_CONTEXT.parse_type_dev_switch($2, $3); }
		|													{ $$ = TPP_EMPTY; }
		;

/* Indizes ohne freie Indizes */
vekt	:								{ $$ = 0; }
		| '[' vekt_inds ']'				{ $$ = $2; }
		;

vekt_inds:	vekt_ind					{ $$ = 1; }
		|	vekt_ind ',' vekt_inds		{ $$ = $3 + 1; }
		;

vekt_ind:	const_expr
		;

/* Konstante mit auch freien Indizes */
d_def_const_vekt:	DEFS_VALUE d_vekt		{ COMP(SET_INDEXES, 0);
											  GLV->index_info.i_anz_ind =
											  		$2 % MAX_ANZ_IND_1;
											  GLV->index_info.i_free_ind =
											  		$2 / MAX_ANZ_IND_1;
											  $$ = $1; }
		;

d_def_set_vekt:		DEFS_SET d_vekt			{ COMP(SET_INDEXES, 0);
											  GLV->index_info.i_anz_ind =
											  		$2 % MAX_ANZ_IND_1;
											  GLV->index_info.i_free_ind =
											  		$2 / MAX_ANZ_IND_1;
											  $$ = $1; }
		;

/* Konstante mit Indizes */
def_const_vekt:	DEFS_VALUE vekt				{ $$ = $1 + $2; }
		;

def_set_vekt: DEFS_SET vekt					{ $$ = $1 + $2; }


/* Wertliste bei Konstantendefinition */
vekt_def:	vekt_val								{ $$ = $1; }
		|	READCSV									{ GLV->is_scann_const_vekt = false; }
				'(' const_expr ')'					{ COMP_PAR(READ_CSV, GLV->index_info.i_free_ind);
													  $$=TPK_VALUE; }

vekt_val: const_set_expr							{ if(GLV->index_info.i_akt_ind != GLV->index_info.i_free_ind)
														 PARSE_CONTEXT.output_error_parse(ERROR_TYPE_COMP, ERROR_LVL_NORMAL, ERROR_CONST_DEF_DIM);
													  $$ = $1; }
		| VEKT_BEG				
			  vekt_val_list ')'						{ GLV->index_info.i_akt_ind--;
													  COMP_PAR_INT(LIT_VAL, $2.ti_i);
													  $$ = $2.ti_t; }
		;

vekt_val_list:	vekt_val							{ $$.ti_i = 1;
													  $$.ti_t = $1; }
		|	vekt_val ',' vekt_val_list				{ $$.ti_i = $3.ti_i + 1;
													  $$.ti_t = PARSE_CONTEXT.parse_type_dev_array($3.ti_t, $1); }
		;

set_val_list:	const_expr							{ $$.ti_i = 1;
													  $$.ti_t = $1; }
		|	const_expr ',' set_val_list				{ $$.ti_i = $3.ti_i + 1;
													  $$.ti_t = PARSE_CONTEXT.parse_type_dev_array($3.ti_t, $1); }
		;

set_expr: TD_SET '(' ')'							{ COMP_PAR(LIT_SET, TP_SET_EMPTY);
													  $$ = TP_SET_EMPTY; }
		| TD_SET '(' set_val_list ')'				{ $$ = PARSE_CONTEXT.parse_type_dev_lit_set(TP_SET_ENUM, $3.ti_t, TPP_NULL, TPP_NULL);
													  COMP_PAR(LIT_VAL_SHORT, $3.ti_i);
													  COMP_PAR(LIT_SET, TP_SET_ENUM); }
		| SET_ALL_NUM								{ COMP_PAR(LIT_SET, TP_SET_ALL_NUM);
													  $$ = TP_SET_ALL_NUM; }
		| SET_ALL_STR								{ COMP_PAR(LIT_SET, TP_SET_ALL_STR);
													  $$ = TP_SET_ALL_STR; }
		| POINTS									{ COMP_PAR(LIT_SET, TP_SET_ALL);
													  $$ = TP_SET_ALL; }
		| POINTS const_expr_no_log					{ $$ = PARSE_CONTEXT.parse_type_dev_lit_set(TP_SET_HALF_UB, $2, TPP_NULL, TPP_NULL);
													  COMP_PAR(LIT_SET, TP_SET_HALF_UB); }
		| const_expr_no_log POINTS					{ $$ = PARSE_CONTEXT.parse_type_dev_lit_set(TP_SET_HALF_LB, $1, TPP_NULL, TPP_NULL);
													  COMP_PAR(LIT_SET, TP_SET_HALF_LB); }
		| const_expr_no_log POINTS const_expr_no_log
													{ $$ = PARSE_CONTEXT.parse_type_dev_lit_set(TP_SET_ALG, $1, TP_INT, $3);
													  COMP_PAR(LIT_SET, TP_SET_ALG_S1); }
		| const_expr_no_log '(' const_expr ')' const_expr_no_log %prec VORZEICHEN
													{ $$ = PARSE_CONTEXT.parse_type_dev_lit_set(TP_SET_ALG, $1, $3, $5);
													  COMP_PAR(LIT_SET, TP_SET_ALG); }
		| TD_SET '(' set_expr ')'					{ COMP_SUB_PAR(CONV_OP, NO_FORMAT, TPK_SET);
													  $$ = TPK_SET; }
		| DEFSET '(' bel_sym_vekt ')'				{ COMP(SET_INDEXES, 0);
													  COMP(DEFSET_FUNC, 1);
													  COMP_INT($3);
													  $$ = TPK_SET; }
		| def_set_vekt								{ COMP(FETCH_CONST, 1);
													  COMP_INT($1);
													  $$ = TPK_SET; }
		| '(' set_expr ')'							{ $$ = $2; }
		| set_op_expr								{ $$ = $1; }
		| const_loop_set							{ $$ = TPK_SET; }
		;


/* Vergleichsoperatoren fuer Nebenbedingungen */
verg_op : '='							{ $$ = EQ; }
		| VERG_GE						{ $$ = GE; }
		| VERG_LE						{ $$ = LE; }
		;

/* Zeilenname */
line_name:							{ COMP(LINE_BEG, 1);
									  PARSE_CONTEXT.linename_repl(GLV->last_str, (char *)0);
									  COMP_STR(GLV->last_str);    }
		| UNDEF_SYMBOL ':'			{ COMP(LINE_BEG, 1);
									  PARSE_CONTEXT.linename_repl(GLV->last_str, GLV->last_str);
									  COMP_STR(GLV->last_str);    }
		| LINENAME_SYMBOL ':'		{ COMP(LINE_BEG, 1);
									  PARSE_CONTEXT.linename_repl(GLV->last_str, GLV->last_str);
									  COMP_STR(GLV->last_str);    }
		;


/***************************************************************************************
 * Bedingungsschleife
 **************************************************************************************/
loop_start: loop_name '{'						{ /* Naechsten loop-Kontexte initialisieren, als Basis */
												  PARSE_CONTEXT.init_loop_comp(GLV->tmp_level, true, LOOP_EXEC, $1); }
		;

loop_sum_start: SUM '{'							{ /* Naechsten loop-Kontexte initialisieren, als Basis fuer Summe */
												  PARSE_CONTEXT.init_loop_comp(GLV->tmp_level, true, LOOP_SUM, (char *)0); }
		;

loop_set_start: TD_SET '{'						{ /* Naechsten loop-Kontexte initialisieren, als Basis fuer Set */
												  PARSE_CONTEXT.init_loop_comp(GLV->tmp_level, true, LOOP_SET, (char *)0); }
		;

loop_name:	UNDEF_SYMBOL						{ $$ = strcpy((char *) (GLV->c_getmem(sizeof(char) * strlen(GLV->last_str))), GLV->last_str); }
		|	LINENAME_SYMBOL						{ $$ = strcpy((char *) (GLV->c_getmem(sizeof(char) * strlen(GLV->last_str))), GLV->last_str); }
		|										{ $$ = (char *)0; }
		;

loop_stop: '}'									{ /* Setzt loop-Kontexte bis einschliesslich naechstem Basiskontext zurueck; compiliert entsprechende Beendigungen */
												  PARSE_CONTEXT.unset_loop_comp(); }
		;

/* Alternativen in Bedingungsschleife */
loop_alt: loop_alt_start loop_if ':'			{ /* Test fuer Alternative compilieren */
												  loop_ctrl_str *loopp = GLV->tmp_table + (GLV->tmp_level - 1);
												  COMP_SUB_PAR_CNT(TEST_ALTER, TEST_ALTER_IF, GLV->tmp_level - 1, 1);
												  loopp->last_jump = COMP_ADDR(0); }
		| loop_alt_start DEFAULT ':'			{ /* Beginn Default-Alternative compilieren */
												  loop_ctrl_str *loopp = GLV->tmp_table + (GLV->tmp_level - 1);
												  COMP_SUB_PAR(TEST_ALTER, TEST_ALTER_DEF, GLV->tmp_level - 1);
												  loopp->last_jump = 0; }
		;

loop_alt_start: '|'								{ loop_ctrl_str *loopp = GLV->tmp_table + (GLV->tmp_level - 1);
												  if (loopp->sub == START_EMPTY || loopp->sub == START_ASSIGN)
												  	PARSE_CONTEXT.output_error_parse(ERROR_TYPE_COMP, ERROR_LVL_NORMAL, ERROR_LOOP_ALT_EMPTY);
												  if (!loopp->last_jump)
												  	PARSE_CONTEXT.output_error_parse(ERROR_TYPE_COMP, ERROR_LVL_WARN, ERROR_LOOP_ALT_NEVER);
												  /* Bisheriger Block endet, Ruecksprung */
												  if (loopp->jump_end)
												  	{ COMP_SUB_PAR_CNT(GOTO, GOTO_INDIREKT, 0, 1); COMP_ADDR(loopp->end_jump); }
												  else
												  	{ COMP_SUB_PAR_CNT(GOTO, GOTO_DIREKT, 0, 1); COMP_ADDR(loopp->iter_start); }
												  /* Ab der naechsten Adresse startet die neue Alternative */
												  if (loopp->last_jump != 0)
													  COMP_ADDR_AT(loopp->last_jump, ADDR_COMP);
												  loopp->last_jump = 0;
												  /* Iterationsvariable ist in den Alternativen jedenfalls nicht gueltig */
												  loopp->s_name = (char *)0;
												}
		;

/* Schleifenkopf */
loop_header: loop_heads ':'
		;

loop_heads: loop_head
		| loop_head ',' loop_heads
		;

loop_head: loop_mods							{ /* Leeren loop-Kontext erzeugen */
												  loop_ctrl_str *loopp = PARSE_CONTEXT.set_loop_comp(START_EMPTY, 0, (char *)0, true);
												  loopp->loop_start = ADDR_COMP; }
				loop_comp_start
		|  loop_mods							{ /* loop-Kontext muss jetzt reserviert werden, da in Zuweisung Summenschleifen vorkommen könnten */
												  loop_ctrl_str *loopp = PARSE_CONTEXT.set_loop_comp(START_ASSIGN, $1, (char *)0, true);
												  loopp->loop_start = ADDR_COMP; }
				loop_for loop_comp_start
		|  loop_mods							{ /* loop-Kontext muss jetzt reserviert werden, da in loop_if Summenschleifen vorkommen könnten */
												  loop_ctrl_str *loopp = PARSE_CONTEXT.set_loop_comp(START_IF, $1, (char *)0, true);
												  loopp->loop_start = ADDR_COMP; }
				loop_if	loop_comp_start
		;

loop_comp_start:								{ /* Start fuer loop compilieren */
												  loop_ctrl_str *loopp = GLV->tmp_table + (GLV->tmp_level - 1);
												  COMP_SUB_PAR_CNT(LOOP_CTRL, loopp->sub, GLV->tmp_level-1, 1);
												  COMP_INT(loopp->mod);
												  /* Iteration compilieren */
												  loopp->iter_start = COMP_SUB_PAR_CNT(TEST_ITER, 0, GLV->tmp_level-1, 3);
												  COMP_ADDR(loopp->loop_start);
												  loopp->end_jump = COMP_ADDR(0);
												  loopp->last_jump = COMP_ADDR(0); }
		;

/* Modifikatoren fuer Schleifenkopf */
loop_mods: loop_mod loop_mods					{ $$ = $1 | $2; }
		|										{ $$ = 0; }
		;

loop_mod: WHILE									{ $$ = LOOP_MOD_WHILE; }
												/* (Kuenftige weitere Modifikatoren) */
		;

/* Kopf fuer for-Schleife */
loop_for: loop_for_var loop_for_assign
			loop_for_set						{ /* Unterscheidung ob Iteration über Set oder lokale Definition */
												  unsigned short sub = ($2 == 2 || TYPECLASS($3) == TPK_SET ? START_LOOP : START_ASSIGN);
												  /* Art und Name der Laufkonstante eintragen */
												  PARSE_CONTEXT.set_loop_sname(sub, $1);
												  /* Wenn $2 == 0 (alte Zuweisung): Wenn $3 != TPK_SET (einfacher Wert), dann Fehler; sonst Warnung */
												  if ($2 == 0) PARSE_CONTEXT.out_warn_old_assign((TYPECLASS($3) != TPK_SET));
												}
		;

loop_for_assign: ASSIGN							{ $$ = 1; }
		|	ASSIGN_ITER							{ $$ = 2; }
		|	'='									{ $$ = 0; }
		;

loop_for_set: const_set_expr					{ $$ = $1; }
		;

loop_for_var: UNDEF_SYMBOL						{ $$ = strcpy((char *) (GLV->c_getmem(sizeof(char) * strlen(GLV->last_str))), GLV->last_str); }
		;

/* Kopf fuer Bedingung */
loop_if: const_expr								{ $$ = $1; }
		;

/* Sprunganweisungen in Schleifen */
loop_ctrl: BREAK loop_ctrl_ref					{ loop_ctrl_str *loopp = PARSE_CONTEXT.search_loop_name_mod($2.s, $2.i, "break", false);
												  if (loopp) { COMP_SUB_PAR_CNT(GOTO, GOTO_INDIREKT, 0, 1); COMP_ADDR(loopp->end_jump); } }
		|  CONTINUE loop_ctrl_ref				{ loop_ctrl_str *loopp = PARSE_CONTEXT.search_loop_name_mod($2.s, $2.i, "continue", true);
												  if (loopp) { COMP_SUB_PAR_CNT(GOTO, GOTO_DIREKT, 0, 1); COMP_ADDR(loopp->iter_start); } }
		|  REPEAT loop_ctrl_ref					{ loop_ctrl_str *loopp = PARSE_CONTEXT.search_loop_name_mod($2.s, $2.i, "repeat", false);
												  if (loopp) { COMP_SUB_PAR_CNT(GOTO, GOTO_DIREKT, 0, 1); COMP_ADDR(loopp->loop_start); } }
		;

loop_ctrl_ref: UNDEF_SYMBOL						{ $$.s = GLV->last_str; $$.i = 0; }
		|	LINENAME_SYMBOL						{ $$.s = GLV->last_str; $$.i = 0; }
		|	DEFS_TMP							{ $$.s = GLV->last_str; $$.i = 0; }
		|	loop_mod							{ $$.s = (char *)0; $$.i = $1; }
		|										{ $$.s = (char *)0; $$.i = 0; }
		;

local_assign: DEFS_TMP_ASSIGN
			ASSIGN const_expr					{ COMP(STORE_TMP, 1);
												  COMP_INT((-$1-2)); }
		;


/***************************************************************************************
 * Ausgabefunktionen
 **************************************************************************************/
/* nutzerdefinierten Fehlermeldung */
/*TODO: Syntax fuer uebrige sub-Kommandos */
usr_err_msg: ERRMSG const_set_expr			{ COMP_SUB(OUTPUT_MSG, ERROR_LVL_FATAL); }
		|    USRMSG const_set_expr	        { COMP_SUB(OUTPUT_MSG, USR_MSG_NL); }
		;


%%

void cmpl::CMPL_Parser::error(const cmpl::CMPL_Parser::location_type& l, const std::string& m)
{
	PARSE_CONTEXT.output_error_parse(ERROR_TYPE_COMP, ERROR_LVL_NORMAL, m.c_str());
}

