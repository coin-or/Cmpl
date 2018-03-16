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
// include of CmplCore.hh must stand before %union, because the definition of enum etp is needed
#include "CmplCore.hh"

#define CMPLCOMP_FOR_SCANNER	cmplcomp.scanner

// in the skeleton the definition of yyerrok is missing, thats why it is defined here
#define yyerrok     (yyerrstatus_ = 0)
%}


%union {
	int i;						/* integer value, symbol number, counter, ... */
	double d;					/* double value */
	bool f;						/* boolean value */
	enum etp t;					/* parameter type */
	char *s;					/* string */
	struct int_str {            /* integer value and string */
		int i;
		const char *s;
	} i_s;
	struct exp_str {            /* expression */
		//symbol_comp_str *sym_p; /* symbol pointer if the expression is a symbol / null: no symbol */
		SymbolTab *symtab;		/* symbol table if the expression is a symbol / null: no symbol */
		int sym_ind;			/* index of symbol in symtab if the expression is a symbol / -1: no symbol */
		bool lval;              /* expression is a lvalue (symp must be set then) */
		bool readonly;			/* expression cannot be left of an assign even if it is a lvalue */
		enum etp tp;            /* type or type class of the expression */
		int tp_sub_tab;         /* index number for the subsymbol table if the expression can have subsymbols */
		int sub_sym_ind;		/* index number of the subsymbol / -1: symbol is not a subsymbol */
		int dim_in;           	/* count of array dimensions before index operator / -1: unknown */
		int dim_out;			/* count of free array dimensions (is the count of array dimensions after the index operator) / -1: unknown */
		int dim_fr_end;       	/* count of full free array dimensions at the end */
		int dim_elem;			/* count of dimensions of the elements of a list / -1: unknown or mixed / -2: na */
		int dim_t_rank1;		/* rank of tupel to construct: tupel construction */
		int dim_t_rank2;		/* rank of tupel to construct: set construction */
		int dim_t_undef;		/* count of undefined symbols in tupel */
		int sym_dim;			/* count of dimensions for a symbol */
		t_addr addr_dim;		/* address to which compile the count of dimensions / 0: nothing */

		inline void set(struct exp_str *p)
			{ symtab = p->symtab; sym_ind = p->sym_ind; lval = p->lval; readonly = p->readonly; tp = p->tp; tp_sub_tab = p->tp_sub_tab; sub_sym_ind = p->sub_sym_ind; dim_in = p->dim_in; dim_out = p->dim_out; dim_fr_end = p->dim_fr_end; dim_elem = p->dim_elem; dim_t_rank1 = p->dim_t_rank1; dim_t_rank2 = p->dim_t_rank2; dim_t_undef = p->dim_t_undef; addr_dim = p->addr_dim; }
		inline void set(enum etp p_tp, int p_dim)
			{ symtab = NULL; sym_ind = -1; lval = readonly = false; tp = p_tp; tp_sub_tab = 0; sub_sym_ind = -1; dim_in = dim_out = p_dim; dim_fr_end = dim_elem = dim_t_rank1 = dim_t_rank2 = dim_t_undef = 0; addr_dim = 0; }
	//	inline void set(SymbolTab *p_symtab, int p_sym_ind, enum etp p_tp, int p_tp_nr, bool p_lval)
	//		{ symtab = p_symtab; sym_ind = p_sym_ind; lval = p_lval; readonly = false; tp = p_tp; tp_sub_tab = p_tp_nr; sub_sym_ind = -1; dim_in = dim_out = dim_fr_end = dim_elem = 0; addr_dim = 0; }
	//	inline void set(SymbolTab *p_symtab, int p_sym_ind, enum etp p_tp, int p_tp_nr, bool p_lval, int p_dim_in, int p_dim_out, int p_dim_fr_end)
	//		{ symtab = p_symtab; sym_ind = p_sym_ind; lval = p_lval; readonly = false; tp = p_tp; tp_sub_tab = p_tp_nr; sub_sym_ind = -1; dim_in = p_dim_in; dim_out = p_dim_out; dim_fr_end = p_dim_fr_end; dim_elem = 0; addr_dim = 0; }

		inline void set_dim(int p_dim_in, int p_dim_out, int p_dim_fr_end)
			{ dim_in = p_dim_in; dim_out = p_dim_out; dim_fr_end = p_dim_fr_end; dim_elem = -1; dim_t_rank1 = dim_t_rank2 = dim_t_undef = 0; }
		inline void set_dim(int p_dim_in, int p_dim_out, int p_dim_fr_end, int p_dim_elem)
			{ dim_in = p_dim_in; dim_out = p_dim_out; dim_fr_end = p_dim_fr_end; dim_elem = p_dim_elem; dim_t_rank1 = dim_t_rank2 = dim_t_undef = 0; }
		inline void set_dim(int p_dim_in, int p_dim_out, int p_dim_fr_end, int p_dim_elem, int p_dim_t_rank1, int p_dim_t_rank2, int p_dim_t_undef)
			{ dim_in = p_dim_in; dim_out = p_dim_out; dim_fr_end = p_dim_fr_end; dim_elem = p_dim_elem; dim_t_rank1 = p_dim_t_rank1; dim_t_rank2 = p_dim_t_rank2; dim_t_undef = p_dim_t_undef; }

		inline void set_sym(SymbolTab *symt, int symn)
			{ symbol_comp_str *symp = symt->elem(symn); symtab = symt; sym_ind = symn; lval = true; tp = symp->tpk; readonly = TP_READONLY(tp); tp_sub_tab = symp->tp_sub_tab; sub_sym_ind = -1; dim_in = 0; dim_out = symp->dim; dim_fr_end = dim_elem = dim_t_rank1 = dim_t_rank2 = dim_t_undef = 0; addr_dim = 0; }
		inline void set_sym(SymbolTab *symt, int symn, int p_sub_sym_ind)
			{ symbol_comp_str *symp = symt->elem(symn); symtab = symt; sym_ind = symn; lval = true; tp = symp->tpk; readonly = TP_READONLY(tp); tp_sub_tab = symp->tp_sub_tab; sub_sym_ind = p_sub_sym_ind; dim_in = 0; dim_out = symp->dim; dim_fr_end = dim_elem = dim_t_rank1 = dim_t_rank2 = dim_t_undef = 0; addr_dim = 0; }

		inline symbol_comp_str *symp()
			{ return (symtab ? symtab->elem(sym_ind) : NULL); }
	} exp;
}


%{
// include of Compiler.hh can stand only after %union, because it needs cmpl::CMPL_Parser already defined
#include "Compiler.hh"
%}


/* Festlegung Prioritaet und Assoziativitaet fuer Operatoren */
%right ASSIGN ASSIGN_ITER
%left LOG_OR
%left LOG_AND
%left VERG_SET
%left '=' VERG_NE VERG_GE VERG_LE VERG_GT VERG_LT VERG_EQ2 VERG_NE2
%left IN_SET
%left '+' '-'
%left '*' '/' DIV MOD '%' TPS_INTERSECT_RR
%right '^'
%nonassoc VORZEICHEN '!'
%left POINTS
%nonassoc TPS_FREE_IND_REV
%left SUBSYM_OP

%token END	0	"end of file"
%token SECT_PARAM SECT_VAR SECT_OBJ SECT_CSTR
%token <exp> SYMBOL_UNDEF SYMBOL_VAR SYMBOL_VALUE SYMBOL_LINENAME
%token <exp> DEFS_TMP DEFS_TMP_ASSIGN
%token <i> SUBSYMBOL_UNDEF SUBSYMBOL_VALUE
/*%token <i> UNDEF_SYMBOL DEFS_VAR DEFS_VALUE DEFS_SET*/
%token <i> INT_NUMBER STRING
%token <d> DOUBLE_NUMBER
%token <i> MATHFUNC PROBLEM_FUNC
%token NULL_VAL
%token READCSV READSTDIN
%token FCT_MIN FCT_MAX
%token DIM DEF SUM LEN FORMAT TYPE DEFSET PROBLEM_SOLVE
%token TD_REAL TD_INT TD_BINARY TD_STRING TD_SET TD_PROBLEM
%token OBJ_TO TPS_INTERSECT_RR TRANSPOSE ERRMSG USRMSG
/*%token VEKT_BEG*/
/*%token LINENAME_SYMBOL*/
%token DEFAULT WHILE REF BREAK CONTINUE REPEAT
%token SET_ALL_NUM SET_ALL_STR
/*%token SET_START SET_END*/

/*%type <i> vekt vekt_inds*/
%type <exp> d_vekt //d_vekt_inds d_vekt_ind
%type <exp> ul_sym_vekt d_undef_sym_vekt
%type <exp> d_def_var_vekt def_var_vekt lvalue_expr
%type <exp> d_def_const_vekt def_const_vekt //def_set_vekt
/*%type <exp> d_def_set_vekt*/
%type <exp> bel_sym_vekt bel_sym_vekt_sym
%type <exp> expr_val_list func_val_list func_ssv_val_list index_val_list
%type <exp> val_list val_list_elem
%type <i> verg_op var_type
%type <i> leer term_sgn start_list
%type <i> tterm t_expr const_list sep_list
%type <f> o_c_expr
%type <exp> loop_for_var loop_name
%type <i> loop_mods loop_mod loop_for_assign loop_for_assign_alt
%type <t> const_type const_type_def
%type <exp> vekt_def //vekt_val
%type <exp> loop_for_set
%type <exp> const_expr const_expr_no_log all_expr literal_expr convert_expr function_expr
%type <exp> num_op_expr log_op_expr cmp_op_expr loop_if // set_op_expr cmp_set_op_expr
%type <exp> subsym_expr subsym_pre
%type <i> subsym_sub
%type <exp> const_loop_sum const_loop_set
%type <exp> set_expr const_set_expr tupel_expr tps_fir_expr
%type <exp> add_const_expr add_const_expr_alts count_const_expr count_const_expr_alts
/*%type <exp> vekt_val_list set_val_list*/
%type <exp> line_name_sym
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
				 const_set_op					{ //GLV->index_info.i_akt_ind = 0;
				 								  //GLV->index_info.i_free_ind = ($1.dim_out >= 0 ? $1.dim_out : 0);
												  //GLV->is_scann_const_vekt = true;

												  /* Ausdruck auf der linken Seite muss lvalue sein */
												  if (!($1.lval))
												  	  PARSE_CONTEXT.output_error_parse(ERROR_TYPE_COMP, ERROR_LVL_NORMAL, "not a lvalue");  /*TODO*/
												  if ($1.readonly)
												  	  PARSE_CONTEXT.output_error_parse(ERROR_TYPE_COMP, ERROR_LVL_NORMAL, "expression is readonly");  /*TODO*/

												  /* Unterscheidung ob linke Seite normales Symbol oder Tupel */
												  if ($1.dim_t_rank1 == 0)
												  {
													  /* Ausdruck auf der linken Seite muss Symbol enthalten) */
													  symbol_comp_str *symp = $1.symp();
													  if (!symp)
														  PARSE_CONTEXT.output_error_parse(ERROR_TYPE_COMP, ERROR_LVL_FATAL, "not a lvalue");	/*TODO*/

													  /* Wenn bereits definiertes Symbol oder wenn Subsymbol:
														Beim Parsen wurde zuletzt FETCH in den Zwischencode geschrieben, das muss hier wieder weg; ebenso Indexanzahl */
													  if (symp->isvalid() || $1.sub_sym_ind >= 0)
														  BACK_ADDR_COMP(4);
														  //BACK_ADDR_COMP(($1.sub_sym_ind >= 0 ? 4 : 2));	//für Subsymbol muss derzeit auch noch eine Indexanzahlangabe weg

													  /* DEF_CONST verlangt vorhergehendes SET_INDEXES */
													  if ($1.dim_out < 0)
													  {
														COMP(LIT_VAL, 1);
														$1.addr_dim = ADDR_COMP;
														COMP_INT(0);
														COMP_SUB(SET_INDEXES, IND_FREE);
													  }
													  else
													  {
														COMP_PAR(LIT_VAL_SHORT, $1.dim_in);
														COMP_SUB(SET_INDEXES, IND_GIVEN);
													  }
												  }
												  else
												  {
												  	  /* Linke Seite ist Tupel, dafür vorläufig compilierte Konstruktion entfernen und daraus Symbole holen */
													  int tupel_cnt;
													  int *symblock = PARSE_CONTEXT.tupel_assign_symbols(&tupel_cnt, 0);

													  /* Zur Übergabe weiter nach hinten in dieser Klausel in $1 speichern, dafuer andere Felder missbrauchen */
													  $1.symtab = (SymbolTab *) symblock;
													  $1.dim_t_rank2 = tupel_cnt;
												  }
												}
				 vekt_def ';'					{ /* Unterscheidung ob linke Seite normales Symbol oder Tupel */
				 								  if ($1.dim_t_rank1 == 0)
												  {
													  /* Dimensionen muessen gleich sein, wenn feststehend, sonst muss auf einer Seite die Dimension feststehen */
													  if ($1.dim_out >= 0 && $5.dim_out >= 0 && $1.dim_out != $5.dim_out && $5.dim_out != 1)
														PARSE_CONTEXT.output_error_parse(ERROR_TYPE_COMP, ERROR_LVL_NORMAL, "array dimensions don't match");/*TODO*/
													  else if ($1.dim_out < 0 && $5.dim_out < 0)
														PARSE_CONTEXT.output_error_parse(ERROR_TYPE_COMP, ERROR_LVL_NORMAL, "array dimensions not known");/*TODO*/
													  else if ($5.dim_out < 0 && $5.addr_dim)
														COMP_INT_AT($5.addr_dim, $1.dim_out);	/* Dimension fuer $5 nachtragen */
													  else if ($1.dim_out < 0 && $1.addr_dim)
													  {
														COMP_INT_AT($1.addr_dim, $5.dim_out);	/* Dimension fuer $1 nachtragen */
														$1.dim_in = $1.sym_dim = $5.dim_out;
													  }

													  /* Typklasse pruefen */
													  if ($2 != TPP_EMPTY)
													  {
														/* Verschiedene Typklassen sind erstmal akzeptabel, es wird dann Konvertierung versucht */
														$1.tp = TYPECLASS($2);
													  }
													  else
													  {
														/* Alle Werte muessen zur selben Typklasse gehoeren, sonst Fehler */
														if ($5.tp == TPP_MIX_TPK)
															PARSE_CONTEXT.output_error_parse(ERROR_TYPE_COMP, ERROR_LVL_NORMAL, ERROR_CONST_DEF_TPK_MIX);
														$1.tp = TYPECLASS($5.tp);
													  }

													  /* Unterscheidung ob neues oder bereits definiertes Symbol */
													  symbol_comp_str *symp = $1.symp();
													  if (!(symp->isvalid()))
														PARSE_CONTEXT.init_symbol(&$1, PTR_TP_CONST, $1.tp, $1.sym_dim, false);
													  else
														PARSE_CONTEXT.test_tpk_symbol(symp, $1.tp, true);

													  if ($1.sub_sym_ind < 0)
													  {
														COMP_SUB_PAR_CNT(DEF_CONST, SYM_MAIN + 256*($5.dim_out>=0 ? $5.dim_out : $1.dim_out), $2, 1);
														COMP_INT(symp->symno * MAX_ANZ_IND_1 + $1.sym_dim);
													  }
													  else
													  {
														COMP_SUB_PAR_CNT(DEF_CONST, SYM_SUB + 256*($5.dim_out>=0 ? $5.dim_out : $1.dim_out), $2, 1);
														COMP_INT($1.sub_sym_ind);
													  }
												  }
												  else
												  {
												      /* Werte aus $1 holen */
													  int *symblock = (int *) ($1.symtab);
													  int tupel_cnt = $1.dim_t_rank2;

													  /* Auf der rechten Seite muss einfaches Tupel stehen (kein Array), mit passendem Tupelrang */
													  if ($5.dim_out != 0)
														PARSE_CONTEXT.output_error_parse(ERROR_TYPE_COMP, ERROR_LVL_NORMAL, "array dimensions don't match");/*TODO*/
                                                      //if (! TP_IS_TUPEL($5.tp))
                                                        //	PARSE_CONTEXT.output_error_parse(ERROR_TYPE_COMP, ERROR_LVL_NORMAL, "right hand side is not a tupel");/*TODO*/
                                                      //if (RANK_TP_TUPEL($5.tp) != tupel_cnt)
                                                          //PARSE_CONTEXT.output_error_parse(ERROR_TYPE_COMP, ERROR_LVL_NORMAL, "tupel rank doesn't match");/*TODO*/

													  COMP_PAR(TUPEL_DISAGGR_REV, tupel_cnt);
													  PARSE_CONTEXT.comp_symbols_assign(symblock, tupel_cnt, true);
												  }
												  //GLV->is_scann_const_vekt = false;	/*TODO: entfaellt*/
												}
		|	REF ul_sym_vekt const_type_def
				const_set_op					{ /* Ausdruck auf der linken Seite muss lvalue sein (und damit auch Symbol enthalten) */
												  symbol_comp_str *symp = $2.symp();
												  if (!($2.lval) || !symp)
													  PARSE_CONTEXT.output_error_parse(ERROR_TYPE_COMP, ERROR_LVL_NORMAL, "not a lvalue");	/*TODO*/
												  if ($2.readonly)
												  	  PARSE_CONTEXT.output_error_parse(ERROR_TYPE_COMP, ERROR_LVL_NORMAL, "expression is readonly");  /*TODO*/

												  /* Ausdruck auf der linken Seite darf keine Indexangabe enthalten */
												  if ($2.dim_in != 0)
												  	  PARSE_CONTEXT.output_error_parse(ERROR_TYPE_COMP, ERROR_LVL_NORMAL, "ref assignment cannot have index info");  /*TODO*/

												  /* Wenn bereits definiertes Symbol oder wenn Subsymbol:
												  	Beim Parsen wurde zuletzt FETCH in den Zwischencode geschrieben, das muss hier wieder weg; ebenso Indexanzahl */
												  if (symp->isvalid() || $2.sub_sym_ind >= 0)
												  	  BACK_ADDR_COMP(4);
												}
				vekt_def ';'					{ /* Ausdruck auf der rechten Seite muss lvalue sein (und damit auch Symbol enthalten) */
												  symbol_comp_str *symp = $6.symp();
												  if (!($6.lval) || !symp)
													  PARSE_CONTEXT.output_error_parse(ERROR_TYPE_COMP, ERROR_LVL_NORMAL, "not a lvalue");	/*TODO*/
												  if ($6.readonly)
												  	  PARSE_CONTEXT.output_error_parse(ERROR_TYPE_COMP, ERROR_LVL_NORMAL, "expression is readonly");  /*TODO*/

												  /* Ausdruck auf der rechten Seite darf keine Indexangabe enthalten */
												  if ($6.dim_in != 0)
												  	  PARSE_CONTEXT.output_error_parse(ERROR_TYPE_COMP, ERROR_LVL_NORMAL, "ref assignment cannot have index info");  /*TODO*/
					
												  /* Beim Parsen wurde zuletzt FETCH in den Zwischencode geschrieben, das muss hier wieder weg; ebenso Indexanzahl,
												     stattdessen ein FETCH für die Referenz generieren */
												  BACK_ADDR_COMP(4);
												  if ($6.sub_sym_ind < 0)
												  {
												  	COMP_SUB_PAR_CNT(FETCH_CONST, SYM_MAIN_REF, 0, 1);
													COMP_INT(symp->symno * MAX_ANZ_IND_1 + $6.dim_in);
												  }
												  else
												  {
				 								  	COMP_SUB_PAR_CNT(FETCH_CONST, SYM_SUB_REF, 0, 1);
				 								  	COMP_INT($6.sub_sym_ind);
												  }

												  /* Dimensionen müssen gleich sein falls linke Seite schon definiert */
												  if ($2.dim_out >= 0 && $2.dim_out != $6.dim_out)
													PARSE_CONTEXT.output_error_parse(ERROR_TYPE_COMP, ERROR_LVL_NORMAL, "array dimensions don't match");/*TODO*/

												  /* Typklasse pruefen */
												  if ($3 != TPP_EMPTY && TYPECLASS($3) != TYPECLASS($6.tp))
												  	PARSE_CONTEXT.output_error_parse(ERROR_TYPE_COMP, ERROR_LVL_NORMAL, "type class don't match");/*TODO*/

												  /* Unterscheidung ob neues oder bereits definiertes Symbol */
												  symbol_comp_str *symp2 = $2.symp();
												  if (!(symp2->isvalid()))
												    PARSE_CONTEXT.init_symbol(&$2, PTR_TP_CONST, $6.tp, $2.sym_dim, false);
												  else
												  	PARSE_CONTEXT.test_tpk_symbol(symp2, $6.tp, true);

												  if ($2.sub_sym_ind < 0)
												  {
				 								  	COMP_SUB_PAR_CNT(DEF_CONST, SYM_MAIN_REF + 256*$6.dim_out, $3, 1);
				 								  	COMP_INT(symp2->symno * MAX_ANZ_IND_1 + $2.sym_dim);
												  }
												  else
												  {
				 								  	COMP_SUB_PAR_CNT(DEF_CONST, SYM_SUB_REF + 256*$6.dim_out, $3, 1);
				 								  	COMP_INT($2.sub_sym_ind);
												  }
												}
		|	const_loop
		|	loop_ctrl ';'
		|	local_assign ';'		//TODO: Beruecksichtigung HEAD_MOD_REF
                /* 1.1.3 stegger user defined message -> err_msg changed to usr_err_message */
		|	usr_err_msg ';'
		|	problem_solve ';'
		;

ul_sym_vekt:d_undef_sym_vekt					{ $$ = $1;
		   										}
		/*|	d_def_const_vekt					{ $$ = $1; }
		  |	d_def_set_vekt						{ $$ = $1; }*/
		|	lvalue_expr							{ /*
												  // Ausdruck muss lvalue sein
												  if (!($1.lval))
													  PARSE_CONTEXT.output_error_parse(ERROR_TYPE_COMP, ERROR_LVL_NORMAL, "not a lvalue");
												  // Anzahl Dimensionen muss bekannt sein
												  if ($1.dim_in < 0)
												  	  PARSE_CONTEXT.output_error_parse(ERROR_TYPE_COMP, ERROR_LVL_NORMAL, "unknown count of dimensions");
												  // Beim Parsen wurde zuletzt FETCH in den Zwischencode geschrieben, das muss hier wieder weg
												  BACK_ADDR_COMP(2);
												  // DEF_CONST verlangt vorhergehendes SET_INDEXES
												  COMP_PAR(LIT_VAL_SHORT, $1.dim_in);
												  COMP_SUB(SET_INDEXES, IND_GIVEN); */
												  $$ = $1; }
		;

lvalue_expr:
			const_expr_no_log					{ $$ = $1; }
		|	set_expr							{ $$ = $1; }
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
		|	TD_PROBLEM							{ $$ = TPK_PROBLEM_MAIN; }
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

problem_solve: PROBLEM_SOLVE func_val_list		{ COMP_PAR_INT(LIT_VAL, $2.dim_in);	/*TODO: Anzahl Elemente als besonderer Typ, zur Kennzeichnung das Array*/
												  // Array-Parameter sind derzeit nicht erlaubt
												  if ($2.dim_elem != 0)
													  PARSE_CONTEXT.output_error_parse(ERROR_TYPE_COMP, ERROR_LVL_NORMAL, "array parameters are not allowed");	/*TODO*/
												  COMP_SUB(CMPL_SOLVE, CMPL_PROBLEM_SOLVE); }


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

d_var_vekt:	d_undef_sym_vekt					{ COMP_PAR(LIT_VAL_SHORT, $1.dim_in);
												  COMP_SUB(SET_INDEXES, IND_GIVEN);
												  PARSE_CONTEXT.init_symbol(&$1, PTR_TP_VAR, TPK_VAR, $1.sym_dim, false);
												  COMP(DEF_VAR, 1);
												  COMP_INT($1.symp()->symno * MAX_ANZ_IND_1 + $1.sym_dim); }
		|	d_def_var_vekt						{ COMP(DEF_VAR, 1);
												  COMP_INT($1.symp()->symno * MAX_ANZ_IND_1 + $1.sym_dim); }
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
d_undef_sym_vekt: SYMBOL_UNDEF
									/* { $1.sym_p = (void*) PARSE_CONTEXT.insert_symbol(GLV->last_str,
										(GLV->sect == (int) token::SECT_PARAM ? TPP_EMPTY *Typklasse wird spaeter gesetzt* : TPK_VAR)); } */
						d_vekt    	{ /*COMP_PAR(LIT_VAL_SHORT, $2.dim_in);
									  COMP_SUB(SET_INDEXES, IND_GIVEN);*/
									  //GLV->index_info.i_anz_ind = $2.dim_in;
									  //GLV->index_info.i_free_ind = ($2.dim_out >= 0 ? $2.dim_out : 0);
									  /*$$.set((void*)$1, TPP_EMPTY, 0, true, $2.dim_in, $2.dim_out, $2.dim_fr_end);*/
									  $$ = $1;
									  $$.sym_dim = 0;
									  if ($2.dim_in > 0) {
									  	$$.set_dim($2.dim_in, $2.dim_out, $2.dim_fr_end);
										$$.sym_dim = $2.dim_t_rank1;
									  }
									}
										//TODO: GLV->index_info entfällt?
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
term	: 										{ COMP(TERM_BEG, 0);
												  COMP_PAR(LIT_VAL_SHORT, $<i>0); /* Vorzeichen Term */ }
			tterm								{ COMP(TERM_END, 0);
												  PARSE_CONTEXT.var_test_prod($2); }
		;

tterm	: t_expr								{ if($1==0) COMP_SUB(NUM_OP, MULT);
												  $$ = $1; }
		| t_expr '*' tterm						{ if($1==0) COMP_SUB(NUM_OP, MULT);
												  $$ = $1 + $3; }
		;

t_expr	:  	d_def_var_vekt transp				{ COMP(TERM_VAR_FREE, 1);
												  COMP_INT($1.symp()->symno * MAX_ANZ_IND_1 + $1.sym_dim);
												  $$ = 1; }
		|	all_expr							{ if ($1.dim_t_undef > 0)
													PARSE_CONTEXT.output_error_parse(ERROR_TYPE_COMP, ERROR_LVL_NORMAL, "undefined symbol");/*TODO*/
												  $$ = 0; }
		|	d_def_const_vekt transp				{ COMP_SUB_PAR_CNT(F_CONST_FREE, SYM_MAIN, 0, 1);
												  COMP_INT($1.symp()->symno * MAX_ANZ_IND_1 + $1.sym_dim);
												  $$ = 0; }
		;

transp	:
		|	TRANSPOSE							{ COMP(IND_TRANSPOSE, 0); }
		;

/* Schleife im constraint-Abschnitt */
cstr_loop: loop_start loop_header cstr_defs cstr_alts loop_stop
		;

cstr_alts: loop_alt cstr_defs cstr_alts
		|
		;


/***************************************************************************************
 * expressions
 **************************************************************************************/
/* Konstante Ausdruecke, die ueberall verwendet werden koennen */
all_expr:	literal_expr						{ $$ = $1; }
		|	convert_expr						{ $$ = $1; }	
		|	function_expr						{ $$ = $1; }
//		|	'(' const_expr ')'					{ $$ = $2; $$.symtab = NULL; $$.lval = false; }
		|	expr_val_list						{ $$ = $1; }
		|   tupel_expr							{ $$ = $1; }
		|	DEFS_TMP							{ COMP(FETCH_TMP, 1);
												  //COMP_INT((-$1-2));
												  COMP_INT($1.symp()->symno);
												  $$.set(/*TPK_VALUE*/ $1.symp()->tpk, 0); }	//TODO: Symbol, lvalue, 0 indizes (alles vorläufig)
												  					//TODO: hier erstmal nur als einfacher Wert (hier sowieso nie lvalue)
																	//TODO: DEFS_TMP entfällt? (falls nicht, dann auch mit Indizes)
		;

/* literal value for an expression */
literal_expr: INT_NUMBER						{ COMP_PAR_INT(LIT_VAL, $1);
												  $$.set(TP_INT, 0); }
		|	DOUBLE_NUMBER						{ COMP(LIT_VAL, 1);
												  COMP_DOUBLE($1);
												  $$.set(TP_DBL, 0); }
		|	STRING								{ COMP(LIT_VAL, 1);
												  COMP_ISTR($1);
												  $$.set(TP_ISTR, 0); }
		;

tupel_expr: index_val_list						{ /* Anzahl Elemente als Listenzähler auf den Stack */
		  										  COMP(LIT_VAL, 1);
												  COMP_VAL(TPP_LIST_VAL, i, $1.dim_in);
												  /* Tupelkonstruktion */
												  COMP(CONSTRUCT_TUPEL, 0);
												  $$.set(($1.dim_out > 0 ? TPK_TUPELSET_N($1.dim_t_rank1) : TPK_TUPEL_N($1.dim_t_rank1)), 0);
												  $$.lval = $1.lval;
												  $$.dim_t_rank1 = $1.dim_t_rank1;
												  $$.dim_t_undef = $1.dim_t_undef; }
		;

convert_expr: const_type '(' const_set_expr ')'	{ if ($3.dim_t_undef > 0)
												  	PARSE_CONTEXT.output_error_parse(ERROR_TYPE_COMP, ERROR_LVL_NORMAL, "undefined symbol");/*TODO*/
												  COMP_SUB_PAR(CONV_OP, NO_FORMAT, $1);
												  $$.set($1, 0); }
		;

function_expr:	FCT_MAX start_list				{ $2 = (int) token::FCT_MAX; }
				 const_list ')'					{ COMP_PAR_INT(LIT_VAL, $4);
												  COMP_SUB(AGGR_OP, MAX_VAL);
												  $$.set(TPK_VALUE, 0); }
		|	FCT_MIN start_list					{ $2 = (int) token::FCT_MIN; }
				 const_list ')'					{ COMP_PAR_INT(LIT_VAL, $4);
												  COMP_SUB(AGGR_OP, MIN_VAL);
												  $$.set(TPK_VALUE, 0); }
		|	DIM '(' bel_sym_vekt ')'			{ /*COMP_PAR(LIT_VAL_SHORT, $3.dim_in);
												  COMP_SUB(SET_INDEXES, IND_GIVEN);*/
												  COMP_SUB_PAR_CNT(DEFSET_EXTNUM, MAX_VAL, 0, 1);
												  COMP_INT($3.symp()->symno * MAX_ANZ_IND_1 + $3.sym_dim);
												  $$.set(TP_INT, 0); }
		|	DEF '(' func_def_par ')'			{ $$.set(TP_INT, 0); }
		|	FORMAT '(' const_expr
				',' const_set_expr ')'			{ if ($3.dim_t_undef > 0 || $5.dim_t_undef > 0)
													PARSE_CONTEXT.output_error_parse(ERROR_TYPE_COMP, ERROR_LVL_NORMAL, "undefined symbol");/*TODO*/
												  COMP_SUB_PAR(CONV_OP, WITH_FORMAT, TP_ISTR);
												  $$.set(TP_ISTR, 0); }
		|	TYPE '(' const_set_expr ')'			{ if ($3.dim_t_undef > 0)
													PARSE_CONTEXT.output_error_parse(ERROR_TYPE_COMP, ERROR_LVL_NORMAL, "undefined symbol");/*TODO*/
												  COMP_SUB(TYPE_STR, TYPE_EXPR);
												  $$.set(TP_ISTR, 0); }
		|	TYPE '(' def_var_vekt ')'			{ COMP_SUB_PAR_CNT(TYPE_STR, TYPE_VAR, 0, 1);
												  COMP_INT($3.symp()->symno * MAX_ANZ_IND_1 + $3.sym_dim);
												  $$.set(TP_ISTR, 0); }
		|	LEN '(' const_set_expr ')'			{ if ($3.dim_t_undef > 0)
                                                                                                  PARSE_CONTEXT.output_error_parse(ERROR_TYPE_COMP, ERROR_LVL_NORMAL, "undefined symbol");/*TODO*/
												  COMP_SUB(UTIL_OP, LEN_VAL);
												  $$.set(TP_INT, 0); }
		|	MATHFUNC '(' const_expr ')'			{ if ($3.dim_t_undef > 0)
													PARSE_CONTEXT.output_error_parse(ERROR_TYPE_COMP, ERROR_LVL_NORMAL, "undefined symbol");/*TODO*/
												  COMP_SUB(MATH_FUNC, $1);
												  $$.set(TPU_NUM, 0); }
		|	READSTDIN '(' const_expr ')'		{ if ($3.dim_t_undef > 0)
													PARSE_CONTEXT.output_error_parse(ERROR_TYPE_COMP, ERROR_LVL_NORMAL, "undefined symbol");/*TODO*/
												  COMP(READ_STDIN, 0);
												  $$.set(TPK_VALUE, 0); }
		|	PROBLEM_FUNC func_val_list			{ COMP_PAR_INT(LIT_VAL, $2.dim_in); /*TODO: Anzahl Elemente als besonderer Typ, zur Kennzeichnung das Array*/
												  // Array-Parameter sind derzeit nicht erlaubt
												  if ($2.dim_elem != 0)
													  PARSE_CONTEXT.output_error_parse(ERROR_TYPE_COMP, ERROR_LVL_NORMAL, "array parameters are not allowed");	/*TODO*/
												  COMP_SUB(CMPL_SOLVE, $1);
												  $$.set(TP_INT, 0); }
		;

subsym_expr: subsym_pre SUBSYM_OP subsym_sub d_vekt
												{ $$ = PARSE_CONTEXT.parse_type_dev_subsym_op(&$1, $3);
												  COMP_PAR_INT(LIT_VAL, $4.dim_in);	//Anzahl muss hier auf den Stack, damit feststeht, wo auf dem Stack die Subsymboltabelle kommt
												  COMP_SUB_PAR_CNT(FETCH_CONST, SYM_SUB, 0, 1);
												  COMP_INT($$.sub_sym_ind);
												  $$.sym_dim = 0;
												  if ($4.dim_in > 0)
												  {
												    if ($$.dim_out >= 0 && $$.dim_out != $4.dim_in)
														PARSE_CONTEXT.output_error_parse(ERROR_TYPE_COMP, ERROR_LVL_NORMAL, "array dimensions don't match");/*TODO*/
												  	$$.set_dim($4.dim_in, $4.dim_out, $4.dim_fr_end);
													$$.sym_dim = $4.dim_t_rank1;
												  }
												  if (!($$.symp()->isvalid()) && TP_SUB_AUTODEF($1.tp))
												  {
												  	$$.set_dim($4.dim_in, $4.dim_out, $4.dim_fr_end);
												  	PARSE_CONTEXT.init_symbol(&$$, PTR_TP_CONST, TPK_VALUE, $$.sym_dim, false);
												  } }
		;

subsym_pre:	all_expr							{ $$ = $1; }
		|	def_const_vekt						{ $$ = $1; }
		|	subsym_expr							{ $$ = $1; }
		;

subsym_sub: SUBSYMBOL_UNDEF						{ $$ = $1; }
		|	SUBSYMBOL_VALUE						{ $$ = $1; }
		;

/* Oeffnende Klammer als Listenstart: Separate Regel, damit Wert zuweisbar */
start_list: '('									{ $$ = 0; }
		;

/* Variable mit Indizes */
def_var_vekt:	SYMBOL_VAR d_vekt				{ /* $$.set((void*)$1, TPK_VAR, 0, true, $2.dim_in, $2.dim_out, $2.dim_fr_end); */
												  $$ = $1;
												  $$.sym_dim = 0;
												  if ($2.dim_in > 0) {
													$$.set_dim($2.dim_in, $2.dim_out, $2.dim_fr_end);
													$$.sym_dim = $2.dim_t_rank1;
												  }
												  if ($1.dim_out != $$.sym_dim)
												  	PARSE_CONTEXT.output_error_parse(ERROR_TYPE_COMP, ERROR_LVL_NORMAL, "array dimensions don't match");/*TODO*/
												}
													// TODO: eigentlich vekt, derzeit keine freien Indizes erlaubt
													// TODO: Unterscheidung def_var_vekt / d_def_var_vekt entfällt
													// TODO: Test ob Indexanzahl passt: $1.dim_out == $2.dim_in
		;

/* Variable auch mit freien Indizes */
d_def_var_vekt:	SYMBOL_VAR d_vekt				{ // TODO: Test ob Indexanzahl passt: $2.dim_in == 0 || $1.dim_out == $2.dim_in
												  //		Wenn $2.dim_in == 0, dann COMP fuer freies Indexinfo; setzen $2.dim_in = $1.dim_out 
												  COMP_PAR(LIT_VAL_SHORT, $2.dim_in);
												  COMP_SUB(SET_INDEXES, IND_GIVEN);
												  //GLV->index_info.i_anz_ind = $2.dim_in;
												  //GLV->index_info.i_free_ind = ($2.dim_out >= 0 ? $2.dim_out : 0);
									  			  /* $$.set((void*)$1, TPK_VAR, 0, true, $2.dim_in, $2.dim_out, $2.dim_fr_end); */
												  $$ = $1;
												  $$.sym_dim = 0;
												  if ($2.dim_in > 0) {
													$$.set_dim($2.dim_in, $2.dim_out, $2.dim_fr_end);
													$$.sym_dim = $2.dim_t_rank1;
												  }
												  if ($1.dim_out != $$.sym_dim)
												  	PARSE_CONTEXT.output_error_parse(ERROR_TYPE_COMP, ERROR_LVL_NORMAL, "array dimensions don't match");/*TODO*/
												}
		;

/* Liste aus Werten (fuer max, min) */
const_list:	const_elem							{ $$ = 1; }
		|	const_elem sep_list					{ $2 = $<i>-1; }
					const_list					{ $$ = $4 + 1; }
		;

/* Komma als Listenseparator: Separate Regel, damit Wert zuweisbar */
sep_list: ','									{ $$ = 0; }
		;

/* Als Element auch Konstanten mit freien Indizes moeglich */
const_elem:	all_expr							{ if ($1.dim_t_undef > 0)
													PARSE_CONTEXT.output_error_parse(ERROR_TYPE_COMP, ERROR_LVL_NORMAL, "undefined symbol");/*TODO*/
												}
		|	SYMBOL_VALUE d_vekt					{ if ($2.dim_in > 0)
												  {
												  	if ($1.dim_out != $2.dim_t_rank1)
														PARSE_CONTEXT.output_error_parse(ERROR_TYPE_COMP, ERROR_LVL_NORMAL, "array dimensions don't match");/*TODO*/
												  	COMP_PAR(LIT_VAL_SHORT, $2.dim_in);
												  	COMP_SUB(SET_INDEXES, IND_GIVEN);
												  }
												  else
												  {
													COMP_PAR(LIT_VAL_SHORT, $1.dim_out);
												  	COMP_SUB(SET_INDEXES, IND_FREE);
												  }
												  COMP_SUB_PAR_CNT(FETCH_VALS, SYM_MAIN, 0, 1);
												  COMP_INT($1.symp()->symno * MAX_ANZ_IND_1 + $1.dim_out);
												  if($<i>-1 == (int) token::FCT_MAX) COMP_SUB(AGGR_OP, MAX_VAL_NULL);
												  else COMP_SUB(AGGR_OP, MIN_VAL_NULL); }
		|	subsym_pre SUBSYM_OP
				subsym_sub d_vekt				{ EXPSTR exp = PARSE_CONTEXT.parse_type_dev_subsym_op(&$1, $3);
												  if ($4.dim_in > 0)
												  {
												  	if (exp.dim_out >= 0 && exp.dim_out != $4.dim_t_rank1)
														PARSE_CONTEXT.output_error_parse(ERROR_TYPE_COMP, ERROR_LVL_NORMAL, "array dimensions don't match");/*TODO*/
												  	COMP_PAR(LIT_VAL_SHORT, $4.dim_in);
												  	COMP_SUB(SET_INDEXES, IND_GIVEN);
												  }
												  else
												  {
													COMP_PAR(LIT_VAL_SHORT, exp.dim_out);
												  	COMP_SUB(SET_INDEXES, IND_FREE);
												  }
												  COMP_SUB_PAR_CNT(FETCH_VALS, SYM_SUB, 0, 1);
												  COMP_INT(exp.sub_sym_ind);
												  if (!(exp.symp()->isvalid()) && TP_SUB_AUTODEF($1.tp))
												  {
													exp.set_dim($4.dim_in, $4.dim_out, $4.dim_fr_end);
												  	PARSE_CONTEXT.init_symbol(&exp, PTR_TP_CONST, TPK_VALUE, $4.dim_t_rank1, false);
												  }
												  if($<i>-1 == (int) token::FCT_MAX) COMP_SUB(AGGR_OP, MAX_VAL_NULL);
												  else COMP_SUB(AGGR_OP, MIN_VAL_NULL); }
		;

/* Vektor fuer dim */
bel_sym_vekt: bel_sym_vekt_sym d_vekt			{ /*$$.set((void*)$1, TPK_VAR, 0, true, $2.dim_in, $2.dim_out, $2.dim_fr_end);*/ /*$$ = $1 * MAX_ANZ_IND_1 + $2.dim_in;*/
												  if ($2.dim_in > 0)
												  {

												  	if ($1.dim_out != $2.dim_t_rank1)
														PARSE_CONTEXT.output_error_parse(ERROR_TYPE_COMP, ERROR_LVL_NORMAL, "array dimensions don't match");/*TODO*/
												  	COMP_PAR(LIT_VAL_SHORT, $2.dim_in);
												  	COMP_SUB(SET_INDEXES, IND_GIVEN);
												  }
												  else
												  {
													COMP_PAR(LIT_VAL_SHORT, $1.dim_out);
												  	COMP_SUB(SET_INDEXES, IND_FREE);
												  }
												  $$ = $1;
												  if ($2.dim_in > 0) {
													$$.set_dim($2.dim_in, $2.dim_out, $2.dim_fr_end);
													$$.sym_dim = $2.dim_t_rank1;
												  }
												  else {
												  	$$.set_dim($1.dim_out, $1.dim_out, $1.dim_out);
													$$.sym_dim = $1.dim_out;
												  }
												}
		;

bel_sym_vekt_sym: SYMBOL_VAR					{ $$ = $1; }
		|		SYMBOL_VALUE					{ $$ = $1; }
//		|		DEFS_VALUE						{ $$.set((void*)$1, TPK_VALUE, 0, true, $2.dim_in, $2.dim_out, $2.dim_fr_end); /*$$ = $1 * MAX_ANZ_IND_1 + $2.dim_in;*/ }
//		|		DEFS_SET						{ $$.set((void*)$1, TPK_SET, 0, true, $2.dim_in, $2.dim_out, $2.dim_fr_end); /*$$ = $1 * MAX_ANZ_IND_1 + $2.dim_in;*/ }
		;

/* Parameter fuer def */
func_def_par:	bel_sym_vekt					{ /*COMP_PAR(LIT_VAL_SHORT, $1.dim_in);
												  COMP_SUB(SET_INDEXES, IND_GIVEN);*/
												  COMP(DEF_FUNC, 1);
												  COMP_INT($1.symp()->symno * MAX_ANZ_IND_1 + $1.sym_dim); }
		|		SYMBOL_UNDEF d_vekt				{ COMP_PAR(LIT_VAL_SHORT, $2.dim_in);
												  COMP_SUB(SET_INDEXES, IND_GIVEN);
												  COMP(DEL_INDEXES, 0);
												  COMP_PAR(LIT_VAL_SHORT, 0); }
		|		DEFS_TMP						{ COMP_PAR(LIT_VAL_SHORT, 1); }
																	//TODO: DEFS_TMP entfällt? (falls nicht, dann auch mit Indizes)
		;


/* Indizes, auch mit freien Indizes */
d_vekt:									{ /*COMP_PAR(LIT_VAL_SHORT, 0);*/
										  $$.set_dim(0, 0, 0);
										  /*$$ = 0;*/ }
		| index_val_list				{ $$ = $1;
										  // Undefinierte Symbole hier nicht erlaubt
										  if ($1.dim_t_undef > 0)
											PARSE_CONTEXT.output_error_parse(ERROR_TYPE_COMP, ERROR_LVL_NORMAL, "undefined symbols in list");/*TODO*/
										}
//		|	'[' d_vekt_inds ']'			{ /*COMP_PAR_INT(LIT_VAL, ($2 % MAX_ANZ_IND_1));*/
//										  /*COMP_PAR(LIT_VAL_SHORT, $2.dim_in);*/
//										  /*$$ = $2.dim_out * MAX_ANZ_IND_1 + $2.dim_in;*/	/*TODO: als exp-Typ*/
//										  $$ = $2;
//										  /*$$ = $2;*/ }
		;

//d_vekt_inds:d_vekt_ind						{ $$ = $1; }
//		|	d_vekt_ind ',' d_vekt_inds		{ $$.set_dim($1.dim_in + $3.dim_in, $1.dim_out + $3.dim_out,
//													$3.dim_fr_end + ($3.dim_fr_end==$3.dim_in ? $1.dim_fr_end : 0));
//											  /*$$ = $1 + 1 + $3;*/
//											  if ($$.dim_in >= MAX_ANZ_IND_1)
//											  	PARSE_CONTEXT.output_error_parse(ERROR_TYPE_COMP, ERROR_LVL_NORMAL, ERROR_INDIZES_MAX); }
//		;
//
//d_vekt_ind:								{ COMP_PAR(LIT_SET, TP_SET_ALL);
//										  $$.set_dim(1, 1, 1); 
//										  /*$$ = MAX_ANZ_IND_1;*/ }
//		|	const_set_expr				{ $$.set_dim(1, (TYPECLASS($1.tp) == TPK_SET ? 1 : 0), 0);
//										  /*$$ = (TYPECLASS($1.tp) == TPK_SET ? MAX_ANZ_IND_1 : 0);*/ }
///*										  COMP_PAR(LIT_VAL_SHORT, IND_ONE); }*/
///*		|	o_c_expr POINTS o_c_expr	{ int ind_type;
//										  if($1 && $3) ind_type = IND_DNUP_B;
//										  else if($1) ind_type = IND_DN_B;
//										  else if($3) ind_type = IND_UP_B;
//										  else ind_type = IND_FREE;
//										  COMP_PAR(LIT_VAL_SHORT, ind_type);
//										  $$ = MAX_ANZ_IND_1; }
//*/
//		;

/* optionaler konstanter Ausdruck */
o_c_expr:								{ $$ = false; }
		|	const_expr_no_log			{ if ($1.dim_t_undef > 0)
											PARSE_CONTEXT.output_error_parse(ERROR_TYPE_COMP, ERROR_LVL_NORMAL, "undefined symbol");/*TODO*/
										  $$ = true; }
		;

/* konstanter Ausdruck */
	//TODO: in expr allgemein undefinierte Symbole erlauben, darauf dann in allen Operationen prüfen und Fehler wenn nicht erlaubt
const_expr:	const_expr_no_log					{ $$ = $1; }
		|	log_op_expr							{ $$ = $1; }
		|	cmp_op_expr							{ $$ = $1; }
		|	set_expr							{ $$ = $1; }
//		|	cmp_set_op_expr						{ $$ = $1; }
		;

	/* Benoetigt, um Konflikte in der Grammatik zu vermeiden. Schraenkt moegliche Ausdruecke aber nicht ein, da logische Operatoren
	   und Vergleichsoperatoren sowieso niedrigere Prioritaet haben als die Regeln, in denen const_expr_no_log verwendet wird */
const_expr_no_log:
			all_expr							{ $$ = $1; }
		|	num_op_expr							{ $$ = $1; }
		|	def_const_vekt						{ $$ = $1; }
		|	subsym_expr							{ $$ = $1; }
		|	const_loop_sum						{ $$ = $1; }
		|	'!' const_expr_no_log				{ if ($2.dim_t_undef > 0)
													PARSE_CONTEXT.output_error_parse(ERROR_TYPE_COMP, ERROR_LVL_NORMAL, "undefined symbol");/*TODO*/
												  COMP_SUB(LOG_OP, NOT);
												  $$.set(TP_BIN, 0); }
		;

/*TODO: Unterscheidung const_set_expr / const_expr entfällt, stattdessen jeweils Prüfung Typklasse*/
const_set_expr: const_expr						{ $$ = $1; }
//		|	set_expr							{ $$ = $1; }
	/*		|	const_loop_sum						{ $$ = $1; } */
		;

const_loop_sum: loop_sum_start					{ COMP(LIT_VAL, 1);
												  COMP_VAL(TPP_NULL, i, 0); }
			loop_header	add_const_expr add_const_expr_alts loop_stop
												{ EXPSTR a, b = PARSE_CONTEXT.parse_type_dev_switch(&$4, &$5);
												  a.set(TPP_NULL, 0);
												  $$ = PARSE_CONTEXT.parse_type_dev_num_op(ADD, &a, &b); }
		;

const_loop_set: loop_set_start					{ COMP_PAR(LIT_VAL_SHORT, 0); }
			loop_header count_const_expr count_const_expr_alts loop_stop
												{ EXPSTR a, b = PARSE_CONTEXT.parse_type_dev_switch(&$4, &$5);
												  a.set(TP_INT, 0);
												  $$ = PARSE_CONTEXT.parse_type_dev_num_op(ADD, &a, &b);
												  if (TYPECLASS($$.tp) != TPK_VALUE)
													PARSE_CONTEXT.output_error_parse(ERROR_TYPE_COMP, ERROR_LVL_NORMAL, ERROR_NOT_A_NUMBER_PARSE);
												  COMP_PAR(LIT_SET, TP_SET_ENUM);
												  $$.set(TPK_SET, 0); }
		;

num_op_expr:
			const_expr_no_log '+' const_expr_no_log			{ COMP_SUB((TYPECLASS($1.tp)==TPK_SET ? SET_OP : NUM_OP), ADD);
															  $$ = PARSE_CONTEXT.parse_type_dev_num_op(ADD, &$1, &$3); }
		|	const_expr_no_log '-' const_expr_no_log			{ COMP_SUB(NUM_OP, SUB);
															  $$ = PARSE_CONTEXT.parse_type_dev_num_op(SUB, &$1, &$3); }
		|	const_expr_no_log '*' const_expr_no_log			{ COMP_SUB((TYPECLASS($1.tp)==TPK_SET ? SET_OP : NUM_OP), MULT);
															  $$ = PARSE_CONTEXT.parse_type_dev_num_op(MULT, &$1, &$3); }
		|	const_expr_no_log '/' const_expr_no_log			{ COMP_SUB(NUM_OP, DIVD);
															  $$ = PARSE_CONTEXT.parse_type_dev_num_op(DIVD, &$1, &$3); }
		|	const_expr_no_log '^' const_expr_no_log			{ COMP_SUB(NUM_OP, POW);
															  $$ = PARSE_CONTEXT.parse_type_dev_num_op(POW, &$1, &$3); }
		|	const_expr_no_log DIV const_expr_no_log			{ COMP_SUB(NUM_OP, DIVI);
															  $$ = PARSE_CONTEXT.parse_type_dev_num_op(DIVI, &$1, &$3); }
		|	const_expr_no_log MOD const_expr_no_log			{ COMP_SUB(NUM_OP, MODI);
															  $$ = PARSE_CONTEXT.parse_type_dev_num_op(MODI, &$1, &$3); }
		|	const_expr_no_log '%' const_expr_no_log			{ COMP_SUB(NUM_OP, MODI);
															  $$ = PARSE_CONTEXT.parse_type_dev_num_op(MODI, &$1, &$3); }
		|	const_expr_no_log TPS_INTERSECT_RR const_expr_no_log
															{ COMP_SUB(SET_OP, INTERSECT_RR);
															  $$ = PARSE_CONTEXT.parse_type_dev_num_op(INTERSECT_RR, &$1, &$3); }
		|   '-' const_expr_no_log %prec VORZEICHEN     		{ EXPSTR t;
															  t.set(TPP_EMPTY, 0);
															  COMP_SUB(NUM_OP, NEG);
															  $$ = PARSE_CONTEXT.parse_type_dev_num_op(NEG, &$2, &t); }
		|	'+' const_expr_no_log %prec VORZEICHEN			{ EXPSTR t;
															  t.set(TPP_EMPTY, 0);
															  COMP_SUB_PAR(CONV_OP, NO_FORMAT, TPU_NUM);	/* fuer sichern nummerischer Wert */
															  $$ = PARSE_CONTEXT.parse_type_dev_num_op(NEG, &$2, &t); }
		;

/*
set_op_expr:
			set_expr '+' set_expr							{ COMP_SUB(SET_OP, ADD);
															  $$.set(TPK_SET, 0); }
		|	set_expr '*' set_expr							{ COMP_SUB(SET_OP, MULT);
															  $$.set(TPK_SET, 0); }
		;
*/

log_op_expr:
			const_expr LOG_AND const_expr					{ if ($1.dim_t_undef > 0 || $3.dim_t_undef > 0)
																PARSE_CONTEXT.output_error_parse(ERROR_TYPE_COMP, ERROR_LVL_NORMAL, "undefined symbol in operation");/*TODO*/
															  COMP_SUB(LOG_OP, AND);
															  $$.set(TP_BIN, 0); }
		|	const_expr LOG_OR const_expr					{ if ($1.dim_t_undef > 0 || $3.dim_t_undef > 0)
																PARSE_CONTEXT.output_error_parse(ERROR_TYPE_COMP, ERROR_LVL_NORMAL, "undefined symbol in operation");/*TODO*/
															  COMP_SUB(LOG_OP, OR);
															  $$.set(TP_BIN, 0); }
		;

cmp_op_expr:
			const_expr_no_log '=' const_expr_no_log			{ COMP_SUB(CMP_OP, EQ);
															  $$ = PARSE_CONTEXT.parse_type_dev_cmp_op(EQ, &$1, &$3); }
		|	const_expr_no_log VERG_EQ2 const_expr_no_log	{ COMP_SUB(CMP_OP, EQ2);
															  $$ = PARSE_CONTEXT.parse_type_dev_cmp_op(EQ2, &$1, &$3); }
		|	const_expr_no_log VERG_NE const_expr_no_log		{ COMP_SUB(CMP_OP, EQ); COMP_SUB(LOG_OP, NOT);
															  $$ = PARSE_CONTEXT.parse_type_dev_cmp_op(EQ, &$1, &$3); }
		|	const_expr_no_log VERG_NE2 const_expr_no_log	{ COMP_SUB(CMP_OP, EQ2); COMP_SUB(LOG_OP, NOT);
															  $$ = PARSE_CONTEXT.parse_type_dev_cmp_op(EQ2, &$1, &$3); }
		|	const_expr_no_log VERG_LE const_expr_no_log		{ COMP_SUB(CMP_OP, LE);
															  $$ = PARSE_CONTEXT.parse_type_dev_cmp_op(LE, &$1, &$3); }
		|	const_expr_no_log VERG_GT const_expr_no_log		{ if (TYPECLASS($1.tp) == TPK_SET)
																{ COMP_SUB(CMP_OP, GT); }	/*TODO: GT auch fuer TPK_VALUE*/
															  else
																{ COMP_SUB(CMP_OP, LE); COMP_SUB(LOG_OP, NOT); }
															  $$ = PARSE_CONTEXT.parse_type_dev_cmp_op(GT, &$1, &$3); }
		|	const_expr_no_log VERG_GE const_expr_no_log		{ COMP_SUB(CMP_OP, GE);
															  $$ = PARSE_CONTEXT.parse_type_dev_cmp_op(GE, &$1, &$3); }
		|	const_expr_no_log VERG_LT const_expr_no_log		{ if (TYPECLASS($1.tp) == TPK_SET)
																{ COMP_SUB(CMP_OP, LT); }	/*TODO: LT auch fuer TPK_VALUE*/
															  else
																{ COMP_SUB(CMP_OP, GE); COMP_SUB(LOG_OP, NOT); }
															  $$ = PARSE_CONTEXT.parse_type_dev_cmp_op(LT, &$1, &$3); }
		|	const_expr_no_log IN_SET const_expr_no_log		{ COMP_SUB(CMP_OP, INSET);
															  $$ = PARSE_CONTEXT.parse_type_dev_cmp_op(INSET, &$1, &$3); }
		;

/*
cmp_set_op_expr:
			set_expr '=' set_expr %prec VERG_SET			{ COMP_SUB(CMP_OP, EQ);
															  $$ = PARSE_CONTEXT.parse_type_dev_cmp_op(EQ, &$1, &$3); }
		|	set_expr VERG_EQ2 set_expr %prec VERG_SET		{ COMP_SUB(CMP_OP, EQ2);
															  $$ = PARSE_CONTEXT.parse_type_dev_cmp_op(EQ2, &$1, &$3); }
		|	set_expr VERG_NE set_expr %prec VERG_SET		{ COMP_SUB(CMP_OP, EQ); COMP_SUB(LOG_OP, NOT);
															  $$ = PARSE_CONTEXT.parse_type_dev_cmp_op(EQ, &$1, &$3); }
		|	set_expr VERG_NE2 set_expr %prec VERG_SET		{ COMP_SUB(CMP_OP, EQ2); COMP_SUB(LOG_OP, NOT);
															  $$ = PARSE_CONTEXT.parse_type_dev_cmp_op(EQ2, &$1, &$3); }
		|	set_expr VERG_LE set_expr %prec VERG_SET		{ COMP_SUB(CMP_OP, LE);
															  $$ = PARSE_CONTEXT.parse_type_dev_cmp_op(LE, &$1, &$3); }
		|	set_expr VERG_LT set_expr %prec VERG_SET		{ COMP_SUB(CMP_OP, LT);
															  $$ = PARSE_CONTEXT.parse_type_dev_cmp_op(LT, &$1, &$3); }
		|	set_expr VERG_GE set_expr %prec VERG_SET		{ COMP_SUB(CMP_OP, GE);
															  $$ = PARSE_CONTEXT.parse_type_dev_cmp_op(GE, &$1, &$3); }
		|	set_expr VERG_GT set_expr %prec VERG_SET		{ COMP_SUB(CMP_OP, GT);
															  $$ = PARSE_CONTEXT.parse_type_dev_cmp_op(GT, &$1, &$3); }
		|	const_expr_no_log IN_SET set_expr				{ COMP_SUB(CMP_OP, INSET);
															  $$ = PARSE_CONTEXT.parse_type_dev_cmp_op(INSET, &$1, &$3); }
		;
*/

add_const_expr: const_set_expr								{ if ($1.dim_t_undef > 0)
													  			PARSE_CONTEXT.output_error_parse(ERROR_TYPE_COMP, ERROR_LVL_NORMAL, "undefined symbol");/*TODO*/
			  												  COMP_SUB(NUM_OP, ADD);
												 			  $$ = $1; }
		;

add_const_expr_alts: loop_alt add_const_expr add_const_expr_alts
															{ $$ = PARSE_CONTEXT.parse_type_dev_switch(&$2, &$3); }
		|													{ $$.set(TPP_EMPTY, 0); }
		;

count_const_expr: const_set_expr							{ if ($1.dim_t_undef > 0)
													  			PARSE_CONTEXT.output_error_parse(ERROR_TYPE_COMP, ERROR_LVL_NORMAL, "undefined symbol");/*TODO*/
															  COMP_SUB(COUNT_OP, CNT_INC);
												 			  $$ = $1; }
		;

count_const_expr_alts: loop_alt count_const_expr add_const_expr_alts
															{ $$ = PARSE_CONTEXT.parse_type_dev_switch(&$2, &$3); }
		|													{ $$.set(TPP_EMPTY, 0); }
		;

/* Indizes ohne freie Indizes */
/*
vekt	:								{ $$ = 0; }
		| '[' vekt_inds ']'				{ $$ = $2; }
		;

vekt_inds:	vekt_ind					{ $$ = 1; }
		|	vekt_ind ',' vekt_inds		{ $$ = $3 + 1; }
		;

vekt_ind:	const_expr
		;
*/

/* Konstante mit auch freien Indizes */
d_def_const_vekt:
		SYMBOL_VALUE d_vekt					{ // TODO: Test ob Indexanzahl passt: $2.dim_in == 0 || $1.dim_out == $2.dim_in
		                                      //        Wenn $2.dim_in == 0, dann COMP fuer freies Indexinfo; setzen $2.dim_in = $1.dim_out
											  $$ = $1;
											  $$.sym_dim = 0;
											  if ($2.dim_in > 0)
											  {
											  	$$.set_dim($2.dim_in, $2.dim_out, $2.dim_fr_end);
												$$.sym_dim = $2.dim_t_rank1;
											  }
		// DEFS_VALUE d_vekt		{
											  COMP_PAR(LIT_VAL_SHORT, $2.dim_in);
											  COMP_SUB(SET_INDEXES, IND_GIVEN);
											  //GLV->index_info.i_anz_ind = $2.dim_in;
											  //GLV->index_info.i_free_ind = ($2.dim_out >= 0 ? $2.dim_out : 0);
											  /*$$.set((void*)$1, TPK_VALUE, 0, true, $2.dim_in, $2.dim_out, $2.dim_fr_end);*/ }
											  	// TODO: d_vekt liefert exp_str (auch dim_fr_end); Füllen GLV->index_info entfällt?
		;

/*
d_def_set_vekt:		DEFS_SET d_vekt			{ COMP_PAR(LIT_VAL_SHORT, $2.dim_in);
											  COMP_SUB(SET_INDEXES, IND_GIVEN);
											  GLV->index_info.i_anz_ind = $2.dim_in;
											  GLV->index_info.i_free_ind = $2.dim_out;
											  $$.set((void*)$1, TPK_SET, 0, true, $2.dim_in, $2.dim_out, $2.dim_fr_end); }
											  	// TODO: d_vekt liefert exp_str (auch dim_fr_end); Füllen GLV->index_info entfällt?
		;
*/

/* Konstante mit Indizes */
def_const_vekt:	//DEFS_VALUE d_vekt			{ $$.set((void*)$1, TPK_VALUE, 0, true, $2.dim_in, $2.dim_out, $2.dim_fr_end); }
				SYMBOL_VALUE d_vekt			{ $$ = $1;
											  $$.sym_dim = 0;
											  if ($2.dim_in > 0) {
											  	$$.set_dim($2.dim_in, $2.dim_out, $2.dim_fr_end);
												$$.sym_dim = $2.dim_t_rank1;
											  }
											  COMP_PAR_INT(LIT_VAL, $2.dim_in);	//Anzahl muss hier auf den Stack, damit Dimensionalität geprüft werden kann
											  COMP_SUB_PAR_CNT(FETCH_CONST, SYM_MAIN, 0, 1);
											  //COMP_INT($$.symp()->symno * MAX_ANZ_IND_1 + $$.dim_in);
											  COMP_INT($$.symp()->symno * MAX_ANZ_IND_1 + $$.sym_dim);
											}
													// TODO: eigentlich vekt, derzeit keine freien Indizes erlaubt
													// TODO: Unterscheidung d_def_const_vekt / def_const_vekt entfällt
													// TODO: Test ob Indexanzahl passt: $1.dim_out == $2.dim_in
		;

//def_set_vekt: //DEFS_SET d_vekt				{ $$.set((void*)$1, TPK_SET, 0, true, $2.dim_in, $2.dim_out, $2.dim_fr_end); }
//													// TODO: eigentlich vekt, derzeit keine freien Indizes erlaubt
//													// TODO: Unterscheidung d_def_set_vekt / def_set_vekt entfällt
//				SYMBOL_VALUE d_vekt			{ $$ = $1;
//											  if ($2.dim_in > 0)
//											  	$$.set_dim($2.dim_in, $2.dim_out, $2.dim_fr_end); }
//		;


expr_val_list: '(' val_list ')'						{ $$ = $2;
													  $$.symtab = NULL;
													  $$.lval = false;
													  // Undefinierte Symbole hier nicht erlaubt
													  if ($2.dim_t_undef > 0)
													  	PARSE_CONTEXT.output_error_parse(ERROR_TYPE_COMP, ERROR_LVL_NORMAL, "undefined symbols in list");/*TODO*/
													  if ($2.dim_in == 1 && $2.dim_fr_end == 0)
													  {
													  	// Arithmetische Klammer um einzelnen Ausdruck; Dimensionalität davon übernehmen
														$$.set_dim(1, $2.dim_elem, 0, $2.dim_elem);
													  }
													  else
													  {
													  	// Liste von Werten, die ein Array bilden
														if ($2.dim_elem < 0)
															PARSE_CONTEXT.output_error_parse(ERROR_TYPE_COMP, ERROR_LVL_NORMAL, "array dimensions not known");/*TODO*/
														// Wenn letztes Element fehlt, dann zählt es nicht
														if ($2.dim_fr_end > 0)
														{
															BACK_ADDR_COMP(1);
															$$.dim_in--;
															$$.dim_fr_end--;
														}
														COMP_PAR_INT(LIT_VAL, $$.dim_in);	/*TODO: Anzahl Elemente als besonderer Typ, zur Kennzeichnung das Array*/
														// Eine weitere Dimension kommt zu der der Elemente
														$$.set_dim($2.dim_in, $2.dim_elem+1, $$.dim_fr_end, $2.dim_elem);
													  } }
		;

func_val_list: '(' val_list ')'						{ // (Auf passende Anzahl/Typ/Dimensionalität der Elemente muss Funktion selbst prüfen; und gegebenenfalls noch Anzahl Elemente aus $$.dim_in compilieren
													  $$ = $2;
													  $$.lval = false;
													  // Undefinierte Symbole hier nicht erlaubt
													  if ($2.dim_t_undef > 0)
													  	PARSE_CONTEXT.output_error_parse(ERROR_TYPE_COMP, ERROR_LVL_NORMAL, "undefined symbols in list");/*TODO*/
													  // Leere Liste richtig darstellen
													  if ($2.dim_in == 1 && $2.dim_fr_end == 1)
													  {
													  	BACK_ADDR_COMP(1);
													  	$$.set_dim(0, 0, 0, 0);
													  }
													  else
													  {
														  // Fehlende Elemente sollten nicht vorkommen
														  if ($2.dim_fr_end > 0)
															PARSE_CONTEXT.output_error_parse(ERROR_TYPE_COMP, ERROR_LVL_NORMAL, "no empty list elements allowed");/*TODO*/
													  } }
		;

func_ssv_val_list: func_val_list					{ // Auf einfachen skalaren Funktionsparameter prüfen
													  if ($1.dim_in != 1 && $1.dim_elem != 0)
														PARSE_CONTEXT.output_error_parse(ERROR_TYPE_COMP, ERROR_LVL_NORMAL, "function argument must be a single scalar value");/*TODO*/
															//TODO: Prüfung besser in der Regel der Funktion, damit Fehlermeldung Funktionsname zeigen kann
													  $$ = $1;
													  $$.set_dim(1, 0, 0, 0); }
		;

index_val_list: '[' val_list ']'					{ // Alle Listenelemente müssen skalar sein
													  if ($2.dim_elem != 0)
														PARSE_CONTEXT.output_error_parse(ERROR_TYPE_COMP, ERROR_LVL_NORMAL, "index values must be scalar");/*TODO*/
													  $$ = $2;
													  // Undefinierte Elemente nur erlaubt, wenn potentieller lvalue
													  if ($2.dim_t_undef > 0 && !$2.lval)
													  	PARSE_CONTEXT.output_error_parse(ERROR_TYPE_COMP, ERROR_LVL_NORMAL, "undefined symbols in list");/*TODO*/
													}
		| '[' NULL_VAL ']'							{ $$.tp = TPP_NULL;
													  $$.set_dim(0, 0, 0, 0);
													  $$.lval = false; }
		;

val_list: val_list_elem								{ $$ = $1; }
		| val_list_elem ',' val_list				{ $$ = PARSE_CONTEXT.parse_type_dev_array(&$3, &$1);	/*TODO: in parse_type_dev_array dim-Bestimmung einbeziehen */
													  $$.set_dim($1.dim_in + $3.dim_in, $1.dim_out + $3.dim_out,
														$3.dim_fr_end + ($3.dim_fr_end==$3.dim_in ? $1.dim_fr_end : 0),
														($1.dim_elem==$3.dim_elem || $3.dim_fr_end==$3.dim_in ? $1.dim_elem :
															($1.dim_fr_end==$1.dim_in ? $3.dim_elem : -1)),
														$1.dim_t_rank1 + $3.dim_t_rank1, ($1.dim_t_rank2 == $3.dim_t_rank2 ? $1.dim_t_rank2 : -1), $1.dim_t_undef + $3.dim_t_undef);
													  $$.lval = ($1.lval && $3.lval); }
		;

val_list_elem:										{ COMP_PAR(LIT_SET, TP_SET_ALL);
                                                      $$.tp = TPP_EMPTY;
													  $$.set_dim(1, 1, 1, 0, 1, 1, 0);
													  $$.lval = false; }
		| const_set_expr							{ // Undefinierte Symbole hier nicht erlaubt
													  if ($1.dim_t_undef > 0)
													  	PARSE_CONTEXT.output_error_parse(ERROR_TYPE_COMP, ERROR_LVL_NORMAL, "undefined symbols in list");/*TODO*/
													  $$ = $1;
													  $$.set_dim(1, ($1.dim_out==0 && (TYPECLASS($1.tp)==TPK_SET || TP_IS_TUPELSET($1.tp)) ? (TP_IS_TUPELSET($1.tp) ? RANK_TP_TUPEL($1.tp) : 1) : 0), 0, $1.dim_out,
													  	(TP_IS_TUPEL($1.tp) || TP_IS_TUPELSET($1.tp) ? RANK_TP_TUPEL($1.tp) : 1),
														(TP_IS_TUPEL($1.tp) || TP_IS_TUPELSET($1.tp) ? RANK_TP_TUPEL($1.tp) : 1), 0); }
		| SYMBOL_UNDEF								{ $$ = $1;
													  $$.set_dim(1, 0, 0, 0, 1, -1, 1);
													  // Compilieren vorläufigen Zugriff auf Symbol; muss später ersetzt werden
											  		  COMP_SUB_PAR_CNT(FETCH_CONST, SYM_UNDEF_ERROR, 0, 1);
											  		  COMP_INT($$.symp()->symno * MAX_ANZ_IND_1); }
		;


/* Wertliste bei Konstantendefinition */
//TODO: entfällt; READCSV wird normale Funktion
vekt_def: const_set_expr							{ if ($1.dim_t_undef > 0)
														PARSE_CONTEXT.output_error_parse(ERROR_TYPE_COMP, ERROR_LVL_NORMAL, "undefined symbol");/*TODO*/
													  $$ = $1; }
		| READCSV func_ssv_val_list					{ $$.set(TPK_VALUE, -1);
													  if (TYPECLASS($2.tp) != TPK_VALUE)
														PARSE_CONTEXT.output_error_parse(ERROR_TYPE_COMP, ERROR_LVL_NORMAL, "wrong argument type for function");/*TODO*/
													  COMP(READ_CSV, 1);	//TODO: READ_CSV verwendet eigenständigen Parameter statt bisher eingebetteten
													  $$.addr_dim = ADDR_COMP; 
													  COMP_INT(0); }
		;

/*
vekt_def:	vekt_val								{ $$ = $1; }
		|	READCSV									{ GLV->is_scann_const_vekt = false; }
				'(' const_expr ')'					{ COMP_PAR(READ_CSV, GLV->index_info.i_free_ind);
													  $$.set(TPK_VALUE, GLV->index_info.i_free_ind); }
		;

vekt_val: const_set_expr							{ if(GLV->index_info.i_akt_ind != GLV->index_info.i_free_ind)
														 PARSE_CONTEXT.output_error_parse(ERROR_TYPE_COMP, ERROR_LVL_NORMAL, ERROR_CONST_DEF_DIM);
													  $$ = $1; }
		| VEKT_BEG				
			  vekt_val_list ')'						{ GLV->index_info.i_akt_ind--;
													  COMP_PAR_INT(LIT_VAL, $2.cnt);
													  $$ = $2;
													  $$.dim_out++; }
		;

vekt_val_list:	vekt_val							{ $$ = $1;
													  $$.cnt = 1; }
		|	vekt_val ',' vekt_val_list				{ $$ = PARSE_CONTEXT.parse_type_dev_array(&$3, &$1);
													  $$.cnt = $3.cnt + 1; }
		;

set_val_list:	const_expr							{ $$ = $1;
													  $$.cnt = 1; }
		|	const_expr ',' set_val_list				{ $$ = PARSE_CONTEXT.parse_type_dev_array(&$3, &$1);
													  $$.cnt = $3.cnt + 1; }
		;
*/

	// TODO: Unterscheidung const_expr / set_expr entfaellt, nur ueber Typ des Ausdrucks, Test in Typableitung
set_expr: TD_SET func_val_list						{ if ($2.dim_in == 0)
													  {
													    // Leerer Set
													  	COMP_PAR(LIT_SET, TP_SET_EMPTY);
														$$.set(TP_SET_EMPTY, 0);
													  }
													  else if (TP_IS_TUPEL($2.tp) || TP_IS_TUPELSET($2.tp))
													  {
													  	// Tupelset konstruieren
		  										  		COMP(LIT_VAL, 1);
												  		COMP_VAL(TPP_LIST_VAL, i, $2.dim_in);
												  		COMP(CONSTRUCT_TUPELSET, 0);
												  		$$.set(TPK_TUPELSET_N($2.dim_t_rank2), 0);
													  }
													  else if ($2.dim_in == 1)
													  {
													  	// Konvertierung einzelnes Argument in einen Set
														COMP_SUB_PAR(CONV_OP, NO_FORMAT, TPK_SET);
														$$.set(TPK_SET, 0);
													  }
													  else
													  {
														// Aufzählungsset aus Elementen
													  	EXPSTR n;
														n.set(TPP_NULL, 0);
														$$ = PARSE_CONTEXT.parse_type_dev_lit_set(TP_SET_ENUM, &$2, &n, &n);
														COMP_PAR(LIT_VAL_SHORT, $2.dim_in);		/*TODO:Typ für Elementanzahl*/
														COMP_PAR(LIT_SET, TP_SET_ENUM);
													  }
													}
	/*
		| TD_SET '(' ')'							{ COMP_PAR(LIT_SET, TP_SET_EMPTY);
													  $$.set(TP_SET_EMPTY, 0); }
		| TD_SET '(' set_val_list ')'				{ EXPSTR n;
													  n.set(TPP_NULL, 0);
													  $$ = PARSE_CONTEXT.parse_type_dev_lit_set(TP_SET_ENUM, &$3, &n, &n);
													  COMP_PAR(LIT_VAL_SHORT, $3.cnt);
													  COMP_PAR(LIT_SET, TP_SET_ENUM); }
	*/
		| SET_ALL_NUM								{ COMP_PAR(LIT_SET, TP_SET_ALL_NUM);
													  $$.set(TP_SET_ALL_NUM, 0); }
		| SET_ALL_STR								{ COMP_PAR(LIT_SET, TP_SET_ALL_STR);
													  $$.set(TP_SET_ALL_STR, 0); }
		| POINTS									{ COMP_PAR(LIT_SET, TP_SET_ALL);
													  $$.set(TP_SET_ALL, 0); }
		| POINTS const_expr_no_log					{ EXPSTR n;
													  n.set(TPP_NULL, 0);
													  $$ = PARSE_CONTEXT.parse_type_dev_lit_set(TP_SET_HALF_UB, &$2, &n, &n);
													  COMP_PAR(LIT_SET, TP_SET_HALF_UB); }
		| const_expr_no_log POINTS					{ EXPSTR n;
													  n.set(TPP_NULL, 0);
													  $$ = PARSE_CONTEXT.parse_type_dev_lit_set(TP_SET_HALF_LB, &$1, &n, &n);
													  COMP_PAR(LIT_SET, TP_SET_HALF_LB); }
		| const_expr_no_log POINTS const_expr_no_log
													{ EXPSTR t;
													  t.set(TP_INT, 0);
													  $$ = PARSE_CONTEXT.parse_type_dev_lit_set(TP_SET_ALG, &$1, &t, &$3);
													  COMP_PAR(LIT_SET, TP_SET_ALG_S1); }
		| const_expr_no_log '(' const_expr ')' const_expr_no_log %prec VORZEICHEN
													{ $$ = PARSE_CONTEXT.parse_type_dev_lit_set(TP_SET_ALG, &$1, &$3, &$5);
													  COMP_PAR(LIT_SET, TP_SET_ALG); }
//		| TD_SET '(' set_expr ')'					{ COMP_SUB_PAR(CONV_OP, NO_FORMAT, TPK_SET);
//													  $$.set(TPK_SET, 0); }
		| DEFSET '(' bel_sym_vekt ')'				{ /*COMP_PAR(LIT_VAL_SHORT, $3.dim_in);
													  COMP_SUB(SET_INDEXES, IND_GIVEN);*/
													  COMP(DEFSET_FUNC, 1);
													  COMP_INT($3.symp()->symno * MAX_ANZ_IND_1 + $3.sym_dim);
													  $$.set(TPK_SET, 0); }		/*TODO: statt bel_sym_vekt beliebiger lvalue, aus Funktionswertliste */
//		| def_set_vekt								{ COMP_SUB_PAR_CNT(FETCH_CONST, SYM_MAIN, 0, 1);
//												  	  COMP_INT($1.symp()->symno * MAX_ANZ_IND_1 + $1.dim_in);
//													  $$ = $1; }
//		| '(' set_expr ')'							{ $$ = $2; $$.symtab = NULL; $$.lval = false; }
//		| set_op_expr								{ $$ = $1; }
		| const_loop_set							{ $$ = $1; }
		|	'*' tps_fir_expr %prec TPS_FREE_IND_REV	{ COMP_SUB(CONSTRUCT_TPS_REV, TPS_IND_FREE);
													  $$.set(TPK_TUPELSET_N(TP_IS_TUPEL($2.tp) || TP_IS_TUPELSET($2.tp) ? RANK_TP_TUPEL($2.tp) : 1), 0); }
		|	'/' tps_fir_expr %prec TPS_FREE_IND_REV	{ COMP_SUB(CONSTRUCT_TPS_REV, TPS_IND_NON_FREE);
													  $$.set(TPK_TUPELSET_N(TP_IS_TUPEL($2.tp) || TP_IS_TUPELSET($2.tp) ? RANK_TP_TUPEL($2.tp) : 1), 0); }
		;

tps_fir_expr:										{ COMP_PAR(LIT_SET, TP_SET_ALL);
													  $$.set(TP_SET_ALL, 0); }
		|	const_expr_no_log						{ $$ = $1; }
		|	set_expr								{ $$ = $1; }
		;


/* Vergleichsoperatoren fuer Nebenbedingungen */
verg_op : '='										{ $$ = EQ; }
		| VERG_GE									{ $$ = GE; }
		| VERG_LE									{ $$ = LE; }
		;

/* Zeilenname */
//TODO: line_name_sym wird beliebiger Ausdruck, muss lval mit gesetztem Symbol sein, Symbol Zeilenname oder undefiniert
line_name_sym: SYMBOL_LINENAME						{ $$ = $1;
													  $$.tp = TPK_VALUE;
													  if (!($1.symp()->isvalid()))
														PARSE_CONTEXT.init_symbol(&$1, PTR_TP_LINENAME, TPK_VALUE, 0, false); }
		| SYMBOL_UNDEF								{ $$ = $1;
													  $$.tp = TPK_VALUE;
													  PARSE_CONTEXT.init_symbol(&$1, PTR_TP_LINENAME, TPK_VALUE, 0, false); }
		;

line_name:											{ COMP(LINE_BEG, 1);
													  PARSE_CONTEXT.linename_repl(GLV->last_str, (char *)0);
													  COMP_STR(GLV->last_str); }
		| line_name_sym ':'							{ COMP(LINE_BEG, 1);
													  PARSE_CONTEXT.linename_repl(GLV->last_str, $1.symp()->s_name);
													  COMP_STR(GLV->last_str);
													  // TODO: auch resultierende Zeilennamen als Symbol einfuegen? (bzw. in linename_repl()?)
													}
		;


/***************************************************************************************
 * Bedingungsschleife
 **************************************************************************************/
loop_start: loop_name '{'						{ /* Naechsten loop-Kontexte initialisieren, als Basis */
												  PARSE_CONTEXT.init_loop_comp(GLV->tmp_level, true, LOOP_EXEC, ($1.tp==TPP_EMPTY ? (char *)0 : $1.symp()->s_name)); }
												  	//TODO: Doppelpunkt nach loop_name sollte optional möglich sein
		;

loop_sum_start: SUM '{'							{ /* Naechsten loop-Kontexte initialisieren, als Basis fuer Summe */
												  PARSE_CONTEXT.init_loop_comp(GLV->tmp_level, true, LOOP_SUM, (char *)0); }
		;

loop_set_start: TD_SET '{'						{ /* Naechsten loop-Kontexte initialisieren, als Basis fuer Set */
												  PARSE_CONTEXT.init_loop_comp(GLV->tmp_level, true, LOOP_SET, (char *)0); }
		;

loop_name:										{ $$.set(TPP_EMPTY, 0); }
		| line_name_sym							{ $$ = $1; }
												// { $$ = strcpy((char *) (GLV->c_getmem(sizeof(char) * strlen(GLV->last_str))), GLV->last_str); }
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

loop_head: loop_mods							{ /* loop-Kontext erzeugen (START_EMPTY als Platzhalter, wird gegebenenfalls spaeter ersetzt) */
												  loop_ctrl_str *loopp = PARSE_CONTEXT.set_loop_comp(START_EMPTY, $1);
												  loopp->loop_start = ADDR_COMP; }
				loop_head_expr loop_comp_start
		;

loop_head_expr:
		|	loop_for
		|	loop_if
		;

//loop_head: loop_mods							{ /* Leeren loop-Kontext erzeugen */
//												  loop_ctrl_str *loopp = PARSE_CONTEXT.set_loop_comp(START_EMPTY, 0);
//												  loopp->loop_start = ADDR_COMP; }
//				loop_comp_start
//		|  loop_mods							{ /* loop-Kontext muss jetzt reserviert werden, da in Zuweisung Summenschleifen vorkommen könnten */
//												  loop_ctrl_str *loopp = PARSE_CONTEXT.set_loop_comp(START_ASSIGN, $1);
//												  loopp->loop_start = ADDR_COMP; }
//				loop_for loop_comp_start
//		|  loop_mods							{ /* loop-Kontext muss jetzt reserviert werden, da in loop_if Summenschleifen vorkommen könnten */
//												  loop_ctrl_str *loopp = PARSE_CONTEXT.set_loop_comp(START_IF, $1);
//												  loopp->loop_start = ADDR_COMP; }
//				loop_if	loop_comp_start
//		;

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

loop_mod: REF									{ $$ = HEAD_MOD_REF; }	//TODO:Berücksichtigung
		| WHILE									{ $$ = HEAD_MOD_WHILE; }
												/* (Kuenftige weitere Modifikatoren) */
		;

/* Kopf fuer for-Schleife */
loop_for: loop_for_var							{ if ($1.dim_t_rank1 > 0) {
												  	  /* Linke Seite ist Tupel, dafür vorläufig compilierte Konstruktion entfernen und daraus Symbole holen */
													  int tupel_cnt;
													  int *symblock = PARSE_CONTEXT.tupel_assign_symbols(&tupel_cnt, $1.dim_t_rank1);

													  /* Zur Übergabe weiter nach hinten in dieser Klausel in $1 speichern, dafuer andere Felder missbrauchen */
													  $1.symtab = (SymbolTab *) symblock;
												  }
												}
			loop_for_set						{ /* Unterscheidung ob Iteration über Set oder lokale Definition */
												  //TODO: Typangabe erlauben
												  //TODO: Auch lokale Definition eines Sets erlauben
												  int assign = $1.sym_dim;	// sym_dim hier entgegen des Namens fuer Art des Zuweisungsoperators verwendet
												  unsigned short sub = (assign == 2 || TYPECLASS($3.tp) == TPK_SET || TP_IS_TUPELSET($3.tp) ? START_LOOP : START_ASSIGN);
												  /* Art und Name der Laufkonstante eintragen */
												  if ($1.dim_t_rank1 > 0)
												  {
												  	// Tupelrang muss passen
                                                  //	if ($1.dim_in != ((TP_IS_TUPEL($3.tp) || TP_IS_TUPELSET($3.tp)) ? RANK_TP_TUPEL($3.tp) : 1))
                                                    //	PARSE_CONTEXT.output_error_parse(ERROR_TYPE_COMP, ERROR_LVL_NORMAL, "rank of iteration tupel doesn't match");/*TODO*/
                                                    $1.tp = TPK_TUPEL_N($1.dim_in);
												  }
												  else
												  {
												  	$1.tp = (sub == START_LOOP ? ((TP_IS_TUPEL($3.tp) || TP_IS_TUPELSET($3.tp)) ? TPK_TUPEL_N(RANK_TP_TUPEL($3.tp)) : TPK_VALUE) : TYPECLASS($3.tp));
												  }
												  PARSE_CONTEXT.set_loop_sname(sub, &$1, ($1.dim_t_rank1 > 0 ? (int *)$1.symtab : (int *)NULL));
												  /* Wenn assign == 0 (alte Zuweisung): Wenn $2 != TPK_SET (einfacher Wert), dann Fehler; sonst Warnung */
												  if (assign == 0) PARSE_CONTEXT.out_warn_old_assign((TYPECLASS($3.tp) != TPK_SET));
												}
		;

loop_for_assign: ASSIGN							{ $$ = 1; }
		|	ASSIGN_ITER							{ $$ = 2; }
		;

loop_for_assign_alt: '='						{ $$ = 0; }
		|	loop_for_assign						{ $$ = $1; }
		;

loop_for_set: const_set_expr					{ if ($1.dim_t_undef > 0)
													PARSE_CONTEXT.output_error_parse(ERROR_TYPE_COMP, ERROR_LVL_NORMAL, "undefined symbol");/*TODO*/
												  $$ = $1; }
		;

loop_for_var: SYMBOL_UNDEF loop_for_assign_alt	{ $$ = $1;
												  $$.sym_dim = $2;	// sym_dim hier entgegen des Namens fuer Art des Zuweisungsoperators verwendet
												}
		| index_val_list loop_for_assign		{ if (!$1.lval)
													PARSE_CONTEXT.output_error_parse(ERROR_TYPE_COMP, ERROR_LVL_NORMAL, "iteration tupel are not a lvalue");
												  if ($1.dim_t_undef != $1.dim_in)
												  	PARSE_CONTEXT.output_error_parse(ERROR_TYPE_COMP, ERROR_LVL_NORMAL, "not all symbols in iteration tupel are undefined");
												  $$ = $1;
												  $$.sym_dim = $2;	// sym_dim hier entgegen des Namens fuer Art des Zuweisungsoperators verwendet
												}
		;

/* Kopf fuer Bedingung */
loop_if: const_expr								{ if ($1.dim_t_undef > 0)
													PARSE_CONTEXT.output_error_parse(ERROR_TYPE_COMP, ERROR_LVL_NORMAL, "undefined symbol");/*TODO*/
												  PARSE_CONTEXT.set_loop_sname(START_IF, (EXPSTR *)NULL, (int *)NULL);
												  $$ = $1; }
		;

/* Sprunganweisungen in Schleifen */
loop_ctrl: BREAK loop_ctrl_ref					{ loop_ctrl_str *loopp = PARSE_CONTEXT.search_loop_name_mod($2.s, $2.i, "break", false);
												  if (loopp) { COMP_SUB_PAR_CNT(GOTO, GOTO_INDIREKT, 0, 1); COMP_ADDR(loopp->end_jump); } }
		|  CONTINUE loop_ctrl_ref				{ loop_ctrl_str *loopp = PARSE_CONTEXT.search_loop_name_mod($2.s, $2.i, "continue", true);
												  if (loopp) { COMP_SUB_PAR_CNT(GOTO, GOTO_DIREKT, 0, 1); COMP_ADDR(loopp->iter_start); } }
		|  REPEAT loop_ctrl_ref					{ loop_ctrl_str *loopp = PARSE_CONTEXT.search_loop_name_mod($2.s, $2.i, "repeat", false);
												  if (loopp) { COMP_SUB_PAR_CNT(GOTO, GOTO_DIREKT, 0, 1); COMP_ADDR(loopp->loop_start); } }
		;

loop_ctrl_ref:									{ $$.s = (char *)0; $$.i = 0; }
		|	SYMBOL_LINENAME						{ $$.s = $1.symp()->s_name; $$.i = 0; }
		|	DEFS_TMP							{ $$.s = $1.symp()->s_name; /*GLV->last_str;*/ $$.i = 0; }	//TODO: beliebiges Symbol, dann testen auf temporärer Parameter; $$.s aus Symbolnamen von $1.symp()
		|	loop_mod							{ $$.s = (char *)0; $$.i = $1; }
		/* |	SYMBOL_UNDEF						{ $$.s = GLV->last_str; $$.i = 0; }	//TODO: entfällt, kann nicht undefiniertes Symbol sein, wenn Zeilennamen immer definiertes Symbol */
		;

local_assign: DEFS_TMP_ASSIGN
			ASSIGN const_expr					{ if ($3.dim_t_undef > 0)
													PARSE_CONTEXT.output_error_parse(ERROR_TYPE_COMP, ERROR_LVL_NORMAL, "undefined symbol");/*TODO*/
												  COMP(STORE_TMP, 1);
												  //COMP_INT((-$1-2));
												  COMP_INT($1.symp()->symno); }
												  	// TODO: entfällt
		;


/***************************************************************************************
 * Ausgabefunktionen
 **************************************************************************************/
/* nutzerdefinierten Fehlermeldung */
/*TODO: Syntax fuer uebrige sub-Kommandos */
usr_err_msg: ERRMSG const_set_expr			{ if ($2.dim_t_undef > 0)
											  	PARSE_CONTEXT.output_error_parse(ERROR_TYPE_COMP, ERROR_LVL_NORMAL, "undefined symbol");/*TODO*/
		   									  COMP_SUB(OUTPUT_MSG, ERROR_LVL_FATAL); }
		|    USRMSG const_set_expr	        { if ($2.dim_t_undef > 0)
											  	PARSE_CONTEXT.output_error_parse(ERROR_TYPE_COMP, ERROR_LVL_NORMAL, "undefined symbol");/*TODO*/
											  COMP_SUB(OUTPUT_MSG, USR_MSG_NL); }
		;


%%

void cmpl::CMPL_Parser::error(const cmpl::CMPL_Parser::location_type& l, const std::string& m)
{
	PARSE_CONTEXT.output_error_parse(ERROR_TYPE_COMP, ERROR_LVL_NORMAL, m.c_str());
}

