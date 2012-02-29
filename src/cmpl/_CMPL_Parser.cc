/* A Bison parser, made by GNU Bison 2.3.  */

/* Skeleton implementation for Bison LALR(1) parsers in C++

   Copyright (C) 2002, 2003, 2004, 2005, 2006 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

// Take the name prefix into account.
#define yylex   cmpllex

#include "_CMPL_Parser.hh"

/* User implementation prologue.  */
#line 119 "src/cmpl/CMPL_Parser.yy"

// include of Compiler.hh can stand only after %union, because it needs cmpl::CMPL_Parser already defined
#include "Compiler.hh"


/* Line 317 of lalr1.cc.  */
#line 49 "src/cmpl/_CMPL_Parser.cc"

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* FIXME: INFRINGES ON USER NAME SPACE */
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#define YYUSE(e) ((void) (e))

/* A pseudo ostream that takes yydebug_ into account.  */
# define YYCDEBUG							\
  for (bool yydebugcond_ = yydebug_; yydebugcond_; yydebugcond_ = false)	\
    (*yycdebug_)

/* Enable debugging if requested.  */
#if YYDEBUG

# define YY_SYMBOL_PRINT(Title, Type, Value, Location)	\
do {							\
  if (yydebug_)						\
    {							\
      *yycdebug_ << Title << ' ';			\
      yy_symbol_print_ ((Type), (Value), (Location));	\
      *yycdebug_ << std::endl;				\
    }							\
} while (false)

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug_)				\
    yy_reduce_print_ (Rule);		\
} while (false)

# define YY_STACK_PRINT()		\
do {					\
  if (yydebug_)				\
    yystack_print_ ();			\
} while (false)

#else /* !YYDEBUG */

# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_REDUCE_PRINT(Rule)
# define YY_STACK_PRINT()

#endif /* !YYDEBUG */

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrorlab

namespace cmpl
{
#if YYERROR_VERBOSE

  /* Return YYSTR after stripping away unnecessary quotes and
     backslashes, so that it's suitable for yyerror.  The heuristic is
     that double-quoting is unnecessary unless the string contains an
     apostrophe, a comma, or backslash (other than backslash-backslash).
     YYSTR is taken from yytname.  */
  std::string
  CMPL_Parser::yytnamerr_ (const char *yystr)
  {
    if (*yystr == '"')
      {
        std::string yyr = "";
        char const *yyp = yystr;

        for (;;)
          switch (*++yyp)
            {
            case '\'':
            case ',':
              goto do_not_strip_quotes;

            case '\\':
              if (*++yyp != '\\')
                goto do_not_strip_quotes;
              /* Fall through.  */
            default:
              yyr += *yyp;
              break;

            case '"':
              return yyr;
            }
      do_not_strip_quotes: ;
      }

    return yystr;
  }

#endif

  /// Build a parser object.
  CMPL_Parser::CMPL_Parser (Compiler& cmplcomp_yyarg)
    : yydebug_ (false),
      yycdebug_ (&std::cerr),
      cmplcomp (cmplcomp_yyarg)
  {
  }

  CMPL_Parser::~CMPL_Parser ()
  {
  }

#if YYDEBUG
  /*--------------------------------.
  | Print this symbol on YYOUTPUT.  |
  `--------------------------------*/

  inline void
  CMPL_Parser::yy_symbol_value_print_ (int yytype,
			   const semantic_type* yyvaluep, const location_type* yylocationp)
  {
    YYUSE (yylocationp);
    YYUSE (yyvaluep);
    switch (yytype)
      {
         default:
	  break;
      }
  }


  void
  CMPL_Parser::yy_symbol_print_ (int yytype,
			   const semantic_type* yyvaluep, const location_type* yylocationp)
  {
    *yycdebug_ << (yytype < yyntokens_ ? "token" : "nterm")
	       << ' ' << yytname_[yytype] << " ("
	       << *yylocationp << ": ";
    yy_symbol_value_print_ (yytype, yyvaluep, yylocationp);
    *yycdebug_ << ')';
  }
#endif /* ! YYDEBUG */

  void
  CMPL_Parser::yydestruct_ (const char* yymsg,
			   int yytype, semantic_type* yyvaluep, location_type* yylocationp)
  {
    YYUSE (yylocationp);
    YYUSE (yymsg);
    YYUSE (yyvaluep);

    YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

    switch (yytype)
      {
  
	default:
	  break;
      }
  }

  void
  CMPL_Parser::yypop_ (unsigned int n)
  {
    yystate_stack_.pop (n);
    yysemantic_stack_.pop (n);
    yylocation_stack_.pop (n);
  }

  std::ostream&
  CMPL_Parser::debug_stream () const
  {
    return *yycdebug_;
  }

  void
  CMPL_Parser::set_debug_stream (std::ostream& o)
  {
    yycdebug_ = &o;
  }


  CMPL_Parser::debug_level_type
  CMPL_Parser::debug_level () const
  {
    return yydebug_;
  }

  void
  CMPL_Parser::set_debug_level (debug_level_type l)
  {
    yydebug_ = l;
  }


  int
  CMPL_Parser::parse ()
  {
    /// Look-ahead and look-ahead in internal form.
    int yychar = yyempty_;
    int yytoken = 0;

    /* State.  */
    int yyn;
    int yylen = 0;
    int yystate = 0;

    /* Error handling.  */
    int yynerrs_ = 0;
    int yyerrstatus_ = 0;

    /// Semantic value of the look-ahead.
    semantic_type yylval;
    /// Location of the look-ahead.
    location_type yylloc;
    /// The locations where the error started and ended.
    location yyerror_range[2];

    /// $$.
    semantic_type yyval;
    /// @$.
    location_type yyloc;

    int yyresult;

    YYCDEBUG << "Starting parse" << std::endl;


    /* Initialize the stacks.  The initial state will be pushed in
       yynewstate, since the latter expects the semantical and the
       location values to have been already stored, initialize these
       stacks with a primary value.  */
    yystate_stack_ = state_stack_type (0);
    yysemantic_stack_ = semantic_stack_type (0);
    yylocation_stack_ = location_stack_type (0);
    yysemantic_stack_.push (yylval);
    yylocation_stack_.push (yylloc);

    /* New state.  */
  yynewstate:
    yystate_stack_.push (yystate);
    YYCDEBUG << "Entering state " << yystate << std::endl;
    goto yybackup;

    /* Backup.  */
  yybackup:

    /* Try to take a decision without look-ahead.  */
    yyn = yypact_[yystate];
    if (yyn == yypact_ninf_)
      goto yydefault;

    /* Read a look-ahead token.  */
    if (yychar == yyempty_)
      {
	YYCDEBUG << "Reading a token: ";
	yychar = yylex (&yylval, &yylloc, CMPLCOMP_FOR_SCANNER, cmplcomp);
      }


    /* Convert token to internal form.  */
    if (yychar <= yyeof_)
      {
	yychar = yytoken = yyeof_;
	YYCDEBUG << "Now at end of input." << std::endl;
      }
    else
      {
	yytoken = yytranslate_ (yychar);
	YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
      }

    /* If the proper action on seeing token YYTOKEN is to reduce or to
       detect an error, take that action.  */
    yyn += yytoken;
    if (yyn < 0 || yylast_ < yyn || yycheck_[yyn] != yytoken)
      goto yydefault;

    /* Reduce or error.  */
    yyn = yytable_[yyn];
    if (yyn <= 0)
      {
	if (yyn == 0 || yyn == yytable_ninf_)
	goto yyerrlab;
	yyn = -yyn;
	goto yyreduce;
      }

    /* Accept?  */
    if (yyn == yyfinal_)
      goto yyacceptlab;

    /* Shift the look-ahead token.  */
    YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

    /* Discard the token being shifted unless it is eof.  */
    if (yychar != yyeof_)
      yychar = yyempty_;

    yysemantic_stack_.push (yylval);
    yylocation_stack_.push (yylloc);

    /* Count tokens shifted since error; after three, turn off error
       status.  */
    if (yyerrstatus_)
      --yyerrstatus_;

    yystate = yyn;
    goto yynewstate;

  /*-----------------------------------------------------------.
  | yydefault -- do the default action for the current state.  |
  `-----------------------------------------------------------*/
  yydefault:
    yyn = yydefact_[yystate];
    if (yyn == 0)
      goto yyerrlab;
    goto yyreduce;

  /*-----------------------------.
  | yyreduce -- Do a reduction.  |
  `-----------------------------*/
  yyreduce:
    yylen = yyr2_[yyn];
    /* If YYLEN is nonzero, implement the default value of the action:
       `$$ = $1'.  Otherwise, use the top of the stack.

       Otherwise, the following line sets YYVAL to garbage.
       This behavior is undocumented and Bison
       users should not rely upon it.  */
    if (yylen)
      yyval = yysemantic_stack_[yylen - 1];
    else
      yyval = yysemantic_stack_[0];

    {
      slice<location_type, location_stack_type> slice (yylocation_stack_, yylen);
      YYLLOC_DEFAULT (yyloc, slice, yylen);
    }
    YY_REDUCE_PRINT (yyn);
    switch (yyn)
      {
	  case 2:
#line 193 "src/cmpl/CMPL_Parser.yy"
    { COMP(PROG_END, 0); ;}
    break;

  case 8:
#line 209 "src/cmpl/CMPL_Parser.yy"
    { GLV->sect = (int) token::SECT_PARAM; ;}
    break;

  case 12:
#line 215 "src/cmpl/CMPL_Parser.yy"
    { yyerrok; ;}
    break;

  case 14:
#line 221 "src/cmpl/CMPL_Parser.yy"
    { //GLV->index_info.i_akt_ind = 0;
				 								  //GLV->index_info.i_free_ind = ($1.dim_out >= 0 ? $1.dim_out : 0);
												  //GLV->is_scann_const_vekt = true;

												  /* Ausdruck auf der linken Seite muss lvalue sein */
												  if (!((yysemantic_stack_[(3) - (1)].exp).lval))
												  	  PARSE_CONTEXT.output_error_parse(ERROR_TYPE_COMP, ERROR_LVL_NORMAL, "not a lvalue");  /*TODO*/
												  if ((yysemantic_stack_[(3) - (1)].exp).readonly)
												  	  PARSE_CONTEXT.output_error_parse(ERROR_TYPE_COMP, ERROR_LVL_NORMAL, "expression is readonly");  /*TODO*/

												  /* Unterscheidung ob linke Seite normales Symbol oder Tupel */
												  if ((yysemantic_stack_[(3) - (1)].exp).dim_t_rank1 == 0)
												  {
													  /* Ausdruck auf der linken Seite muss Symbol enthalten) */
													  symbol_comp_str *symp = (yysemantic_stack_[(3) - (1)].exp).symp();
													  if (!symp)
														  PARSE_CONTEXT.output_error_parse(ERROR_TYPE_COMP, ERROR_LVL_NORMAL, "not a lvalue");	/*TODO*/

													  /* Wenn bereits definiertes Symbol oder wenn Subsymbol:
														Beim Parsen wurde zuletzt FETCH in den Zwischencode geschrieben, das muss hier wieder weg; ebenso Indexanzahl */
													  if (symp->isvalid() || (yysemantic_stack_[(3) - (1)].exp).sub_sym_ind >= 0)
														  BACK_ADDR_COMP(4);
														  //BACK_ADDR_COMP(($1.sub_sym_ind >= 0 ? 4 : 2));	//für Subsymbol muss derzeit auch noch eine Indexanzahlangabe weg

													  /* DEF_CONST verlangt vorhergehendes SET_INDEXES */
													  if ((yysemantic_stack_[(3) - (1)].exp).dim_out < 0)
													  {
														COMP(LIT_VAL, 1);
														(yysemantic_stack_[(3) - (1)].exp).addr_dim = ADDR_COMP;
														COMP_INT(0);
														COMP_SUB(SET_INDEXES, IND_FREE);
													  }
													  else
													  {
														COMP_PAR(LIT_VAL_SHORT, (yysemantic_stack_[(3) - (1)].exp).dim_in);
														COMP_SUB(SET_INDEXES, IND_GIVEN);
													  }
												  }
												  else
												  {
												  	  /* Linke Seite ist Tupel, dafür vorläufig compilierte Konstruktion entfernen und daraus Symbole holen */
													  int tupel_cnt;
													  symbol_comp_str **tupel_syms = PARSE_CONTEXT.tupel_assign_symbols(&tupel_cnt);

													  /* Zur Übergabe weiter nach hinten in dieser Klausel in $1 speichern, dafuer andere Felder missbrauchen */
													  (yysemantic_stack_[(3) - (1)].exp).symtab = (SymbolTab *) tupel_syms;
													  (yysemantic_stack_[(3) - (1)].exp).dim_t_rank2 = tupel_cnt;
												  }
												;}
    break;

  case 15:
#line 270 "src/cmpl/CMPL_Parser.yy"
    { /* Unterscheidung ob linke Seite normales Symbol oder Tupel */
				 								  if ((yysemantic_stack_[(6) - (1)].exp).dim_t_rank1 == 0)
												  {
													  /* Dimensionen muessen gleich sein, wenn feststehend, sonst muss auf einer Seite die Dimension feststehen */
													  if ((yysemantic_stack_[(6) - (1)].exp).dim_out >= 0 && (yysemantic_stack_[(6) - (5)].exp).dim_out >= 0 && (yysemantic_stack_[(6) - (1)].exp).dim_out != (yysemantic_stack_[(6) - (5)].exp).dim_out)
														PARSE_CONTEXT.output_error_parse(ERROR_TYPE_COMP, ERROR_LVL_NORMAL, "array dimensions don't match");/*TODO*/
													  else if ((yysemantic_stack_[(6) - (1)].exp).dim_out < 0 && (yysemantic_stack_[(6) - (5)].exp).dim_out < 0)
														PARSE_CONTEXT.output_error_parse(ERROR_TYPE_COMP, ERROR_LVL_NORMAL, "array dimensions not known");/*TODO*/
													  else if ((yysemantic_stack_[(6) - (5)].exp).dim_out < 0 && (yysemantic_stack_[(6) - (5)].exp).addr_dim)
														COMP_INT_AT((yysemantic_stack_[(6) - (5)].exp).addr_dim, (yysemantic_stack_[(6) - (1)].exp).dim_out);	/* Dimension fuer $5 nachtragen */
													  else if ((yysemantic_stack_[(6) - (1)].exp).dim_out < 0 && (yysemantic_stack_[(6) - (1)].exp).addr_dim)
													  {
														COMP_INT_AT((yysemantic_stack_[(6) - (1)].exp).addr_dim, (yysemantic_stack_[(6) - (5)].exp).dim_out);	/* Dimension fuer $1 nachtragen */
														(yysemantic_stack_[(6) - (1)].exp).dim_in = (yysemantic_stack_[(6) - (5)].exp).dim_out;
													  }

													  /* Typklasse pruefen */
													  if ((yysemantic_stack_[(6) - (2)].t) != TPP_EMPTY)
													  {
														/* Verschiedene Typklassen sind erstmal akzeptabel, es wird dann Konvertierung versucht */
														(yysemantic_stack_[(6) - (1)].exp).tp = TYPECLASS((yysemantic_stack_[(6) - (2)].t));
													  }
													  else
													  {
														/* Alle Werte muessen zur selben Typklasse gehoeren, sonst Fehler */
														if ((yysemantic_stack_[(6) - (5)].exp).tp == TPP_MIX_TPK)
															PARSE_CONTEXT.output_error_parse(ERROR_TYPE_COMP, ERROR_LVL_NORMAL, ERROR_CONST_DEF_TPK_MIX);
														(yysemantic_stack_[(6) - (1)].exp).tp = TYPECLASS((yysemantic_stack_[(6) - (5)].exp).tp);
													  }

													  /* Unterscheidung ob neues oder bereits definiertes Symbol */
													  symbol_comp_str *symp = (yysemantic_stack_[(6) - (1)].exp).symp();
													  if (!(symp->isvalid()))
														PARSE_CONTEXT.init_symbol(&(yysemantic_stack_[(6) - (1)].exp), PTR_TP_CONST, (yysemantic_stack_[(6) - (1)].exp).tp, false);
													  else
														PARSE_CONTEXT.test_tpk_symbol(symp, (yysemantic_stack_[(6) - (1)].exp).tp, true);

													  if ((yysemantic_stack_[(6) - (1)].exp).sub_sym_ind < 0)
													  {
														COMP_SUB_PAR_CNT(DEF_CONST, SYM_MAIN, (yysemantic_stack_[(6) - (2)].t), 1);
														COMP_INT(symp->symno * MAX_ANZ_IND_1 + (yysemantic_stack_[(6) - (1)].exp).dim_in);
													  }
													  else
													  {
														COMP_SUB_PAR_CNT(DEF_CONST, SYM_SUB, (yysemantic_stack_[(6) - (2)].t), 1);
														COMP_INT((yysemantic_stack_[(6) - (1)].exp).sub_sym_ind);
													  }
												  }
												  else
												  {
												      /* Werte aus $1 holen */
													  int tupel_cnt = (yysemantic_stack_[(6) - (1)].exp).dim_t_rank2;
													  symbol_comp_str **tupel_syms = (symbol_comp_str **) ((yysemantic_stack_[(6) - (1)].exp).symtab);

													  /* Auf der rechten Seite muss einfaches Tupel stehen (kein Array), mit passendem Tupelrang */
													  if ((yysemantic_stack_[(6) - (5)].exp).dim_out != 0)
														PARSE_CONTEXT.output_error_parse(ERROR_TYPE_COMP, ERROR_LVL_NORMAL, "array dimensions don't match");/*TODO*/
													  if (! TP_IS_TUPEL((yysemantic_stack_[(6) - (5)].exp).tp))
														PARSE_CONTEXT.output_error_parse(ERROR_TYPE_COMP, ERROR_LVL_NORMAL, "right hand side is not a tupel");/*TODO*/
													  if (RANK_TP_TUPEL((yysemantic_stack_[(6) - (5)].exp).tp) != tupel_cnt)
														PARSE_CONTEXT.output_error_parse(ERROR_TYPE_COMP, ERROR_LVL_NORMAL, "tupel rank doesn't match");/*TODO*/

													  PARSE_CONTEXT.comp_tupel_assign(tupel_syms, tupel_cnt, false);
												  }
												  //GLV->is_scann_const_vekt = false;	/*TODO: entfaellt*/
												;}
    break;

  case 16:
#line 337 "src/cmpl/CMPL_Parser.yy"
    { /* Ausdruck auf der linken Seite muss lvalue sein (und damit auch Symbol enthalten) */
												  symbol_comp_str *symp = (yysemantic_stack_[(4) - (2)].exp).symp();
												  if (!((yysemantic_stack_[(4) - (2)].exp).lval) || !symp)
													  PARSE_CONTEXT.output_error_parse(ERROR_TYPE_COMP, ERROR_LVL_NORMAL, "not a lvalue");	/*TODO*/
												  if ((yysemantic_stack_[(4) - (2)].exp).readonly)
												  	  PARSE_CONTEXT.output_error_parse(ERROR_TYPE_COMP, ERROR_LVL_NORMAL, "expression is readonly");  /*TODO*/

												  /* Ausdruck auf der linken Seite darf keine Indexangabe enthalten */
												  if ((yysemantic_stack_[(4) - (2)].exp).dim_in != 0)
												  	  PARSE_CONTEXT.output_error_parse(ERROR_TYPE_COMP, ERROR_LVL_NORMAL, "ref assignment cannot have index info");  /*TODO*/

												  /* Wenn bereits definiertes Symbol oder wenn Subsymbol:
												  	Beim Parsen wurde zuletzt FETCH in den Zwischencode geschrieben, das muss hier wieder weg; ebenso Indexanzahl */
												  if (symp->isvalid() || (yysemantic_stack_[(4) - (2)].exp).sub_sym_ind >= 0)
												  	  BACK_ADDR_COMP(4);
												;}
    break;

  case 17:
#line 353 "src/cmpl/CMPL_Parser.yy"
    { /* Ausdruck auf der rechten Seite muss lvalue sein (und damit auch Symbol enthalten) */
												  symbol_comp_str *symp = (yysemantic_stack_[(7) - (6)].exp).symp();
												  if (!((yysemantic_stack_[(7) - (6)].exp).lval) || !symp)
													  PARSE_CONTEXT.output_error_parse(ERROR_TYPE_COMP, ERROR_LVL_NORMAL, "not a lvalue");	/*TODO*/
												  if ((yysemantic_stack_[(7) - (6)].exp).readonly)
												  	  PARSE_CONTEXT.output_error_parse(ERROR_TYPE_COMP, ERROR_LVL_NORMAL, "expression is readonly");  /*TODO*/

												  /* Ausdruck auf der rechten Seite darf keine Indexangabe enthalten */
												  if ((yysemantic_stack_[(7) - (6)].exp).dim_in != 0)
												  	  PARSE_CONTEXT.output_error_parse(ERROR_TYPE_COMP, ERROR_LVL_NORMAL, "ref assignment cannot have index info");  /*TODO*/
					
												  /* Beim Parsen wurde zuletzt FETCH in den Zwischencode geschrieben, das muss hier wieder weg; ebenso Indexanzahl,
												     stattdessen ein FETCH für die Referenz generieren */
												  BACK_ADDR_COMP(4);
												  if ((yysemantic_stack_[(7) - (6)].exp).sub_sym_ind < 0)
												  {
												  	COMP_SUB_PAR_CNT(FETCH_CONST, SYM_MAIN_REF, 0, 1);
													COMP_INT(symp->symno * MAX_ANZ_IND_1 + (yysemantic_stack_[(7) - (6)].exp).dim_in);
												  }
												  else
												  {
				 								  	COMP_SUB_PAR_CNT(FETCH_CONST, SYM_SUB_REF, 0, 1);
				 								  	COMP_INT((yysemantic_stack_[(7) - (6)].exp).sub_sym_ind);
												  }

												  /* Dimensionen müssen gleich sein falls linke Seite schon definiert */
												  if ((yysemantic_stack_[(7) - (2)].exp).dim_out >= 0 && (yysemantic_stack_[(7) - (2)].exp).dim_out != (yysemantic_stack_[(7) - (6)].exp).dim_out)
													PARSE_CONTEXT.output_error_parse(ERROR_TYPE_COMP, ERROR_LVL_NORMAL, "array dimensions don't match");/*TODO*/

												  /* Typklasse pruefen */
												  if ((yysemantic_stack_[(7) - (3)].t) != TPP_EMPTY && TYPECLASS((yysemantic_stack_[(7) - (3)].t)) != TYPECLASS((yysemantic_stack_[(7) - (6)].exp).tp))
												  	PARSE_CONTEXT.output_error_parse(ERROR_TYPE_COMP, ERROR_LVL_NORMAL, "type class don't match");/*TODO*/

												  /* Unterscheidung ob neues oder bereits definiertes Symbol */
												  symbol_comp_str *symp2 = (yysemantic_stack_[(7) - (2)].exp).symp();
												  if (!(symp2->isvalid()))
												    PARSE_CONTEXT.init_symbol(&(yysemantic_stack_[(7) - (2)].exp), PTR_TP_CONST, (yysemantic_stack_[(7) - (6)].exp).tp, false);
												  else
												  	PARSE_CONTEXT.test_tpk_symbol(symp2, (yysemantic_stack_[(7) - (6)].exp).tp, true);

												  if ((yysemantic_stack_[(7) - (2)].exp).sub_sym_ind < 0)
												  {
				 								  	COMP_SUB_PAR_CNT(DEF_CONST, SYM_MAIN_REF, (yysemantic_stack_[(7) - (3)].t), 1);
				 								  	COMP_INT(symp2->symno * MAX_ANZ_IND_1 + (yysemantic_stack_[(7) - (2)].exp).dim_in);
												  }
												  else
												  {
				 								  	COMP_SUB_PAR_CNT(DEF_CONST, SYM_SUB_REF, (yysemantic_stack_[(7) - (3)].t), 1);
				 								  	COMP_INT((yysemantic_stack_[(7) - (2)].exp).sub_sym_ind);
												  }
												;}
    break;

  case 23:
#line 412 "src/cmpl/CMPL_Parser.yy"
    { (yyval.exp) = (yysemantic_stack_[(1) - (1)].exp); ;}
    break;

  case 24:
#line 415 "src/cmpl/CMPL_Parser.yy"
    { /*
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
												  (yyval.exp) = (yysemantic_stack_[(1) - (1)].exp); ;}
    break;

  case 25:
#line 431 "src/cmpl/CMPL_Parser.yy"
    { (yyval.exp) = (yysemantic_stack_[(1) - (1)].exp); ;}
    break;

  case 26:
#line 432 "src/cmpl/CMPL_Parser.yy"
    { (yyval.exp) = (yysemantic_stack_[(1) - (1)].exp); ;}
    break;

  case 27:
#line 436 "src/cmpl/CMPL_Parser.yy"
    { (yyval.t) = TPP_EMPTY; ;}
    break;

  case 28:
#line 437 "src/cmpl/CMPL_Parser.yy"
    { (yyval.t) = (yysemantic_stack_[(2) - (2)].t); ;}
    break;

  case 29:
#line 441 "src/cmpl/CMPL_Parser.yy"
    { (yyval.t) = TP_DBL; ;}
    break;

  case 30:
#line 442 "src/cmpl/CMPL_Parser.yy"
    { (yyval.t) = TP_INT; ;}
    break;

  case 31:
#line 443 "src/cmpl/CMPL_Parser.yy"
    { (yyval.t) = TP_BIN; ;}
    break;

  case 32:
#line 444 "src/cmpl/CMPL_Parser.yy"
    { (yyval.t) = TP_STR; ;}
    break;

  case 33:
#line 445 "src/cmpl/CMPL_Parser.yy"
    { (yyval.t) = TPK_PROBLEM_MAIN; ;}
    break;

  case 35:
#line 450 "src/cmpl/CMPL_Parser.yy"
    { PARSE_CONTEXT.out_warn_old_assign(false); ;}
    break;

  case 39:
#line 461 "src/cmpl/CMPL_Parser.yy"
    { COMP_PAR_INT(LIT_VAL, (yysemantic_stack_[(2) - (2)].exp).dim_in);	/*TODO: Anzahl Elemente als besonderer Typ, zur Kennzeichnung das Array*/
												  // Array-Parameter sind derzeit nicht erlaubt
												  if ((yysemantic_stack_[(2) - (2)].exp).dim_elem != 0)
													  PARSE_CONTEXT.output_error_parse(ERROR_TYPE_COMP, ERROR_LVL_NORMAL, "array parameters are not allowed");	/*TODO*/
												  COMP_SUB(CMPL_SOLVE, CMPL_PROBLEM_SOLVE); ;}
    break;

  case 40:
#line 471 "src/cmpl/CMPL_Parser.yy"
    { GLV->sect = (int) token::SECT_VAR; ;}
    break;

  case 44:
#line 477 "src/cmpl/CMPL_Parser.yy"
    { yyerrok; ;}
    break;

  case 46:
#line 482 "src/cmpl/CMPL_Parser.yy"
    { COMP_SUB(DEF_VAR_TYPE, (yysemantic_stack_[(4) - (3)].i)); ;}
    break;

  case 53:
#line 494 "src/cmpl/CMPL_Parser.yy"
    { COMP_PAR(LIT_VAL_SHORT, (yysemantic_stack_[(1) - (1)].exp).dim_in);
												  COMP_SUB(SET_INDEXES, IND_GIVEN);
												  PARSE_CONTEXT.init_symbol(&(yysemantic_stack_[(1) - (1)].exp), PTR_TP_VAR, TPK_VAR, false);
												  COMP(DEF_VAR, 1);
												  COMP_INT((yysemantic_stack_[(1) - (1)].exp).symp()->symno * MAX_ANZ_IND_1 + (yysemantic_stack_[(1) - (1)].exp).dim_in); ;}
    break;

  case 54:
#line 499 "src/cmpl/CMPL_Parser.yy"
    { COMP(DEF_VAR, 1);
												  COMP_INT((yysemantic_stack_[(1) - (1)].exp).symp()->symno * MAX_ANZ_IND_1 + (yysemantic_stack_[(1) - (1)].exp).dim_in); ;}
    break;

  case 55:
#line 504 "src/cmpl/CMPL_Parser.yy"
    { (yyval.i) = (int)TP_VAR_DBL; ;}
    break;

  case 56:
#line 505 "src/cmpl/CMPL_Parser.yy"
    { (yyval.i) = (int)TP_VAR_INT; ;}
    break;

  case 57:
#line 506 "src/cmpl/CMPL_Parser.yy"
    { (yyval.i) = (int)TP_VAR_BIN; ;}
    break;

  case 58:
#line 510 "src/cmpl/CMPL_Parser.yy"
    { COMP(RESET_VAR_B, 0); ;}
    break;

  case 59:
#line 511 "src/cmpl/CMPL_Parser.yy"
    { COMP(RESET_VAR_B, 0);
												  if((yysemantic_stack_[(5) - (4)].f)) COMP(DEF_UP_VAR, 0);
												  if((yysemantic_stack_[(5) - (2)].f)) COMP(DEF_DN_VAR, 0); ;}
    break;

  case 60:
#line 520 "src/cmpl/CMPL_Parser.yy"
    { /*COMP_PAR(LIT_VAL_SHORT, $2.dim_in);
									  COMP_SUB(SET_INDEXES, IND_GIVEN);*/
									  //GLV->index_info.i_anz_ind = $2.dim_in;
									  //GLV->index_info.i_free_ind = ($2.dim_out >= 0 ? $2.dim_out : 0);
									  /*$$.set((void*)$1, TPP_EMPTY, 0, true, $2.dim_in, $2.dim_out, $2.dim_fr_end);*/
									  (yyval.exp) = (yysemantic_stack_[(2) - (1)].exp);
									  if ((yysemantic_stack_[(2) - (2)].exp).dim_in > 0)
									  	(yyval.exp).set_dim((yysemantic_stack_[(2) - (2)].exp).dim_in, (yysemantic_stack_[(2) - (2)].exp).dim_out, (yysemantic_stack_[(2) - (2)].exp).dim_fr_end);
									;}
    break;

  case 64:
#line 544 "src/cmpl/CMPL_Parser.yy"
    { GLV->sect = (int) token::SECT_OBJ; ;}
    break;

  case 68:
#line 550 "src/cmpl/CMPL_Parser.yy"
    { yyerrok; ;}
    break;

  case 70:
#line 555 "src/cmpl/CMPL_Parser.yy"
    { GLV->is_left = true; ;}
    break;

  case 71:
#line 556 "src/cmpl/CMPL_Parser.yy"
    { COMP_PAR(LINE_MOD, MODE_N);
												  COMP(LINE_END, 0); ;}
    break;

  case 77:
#line 567 "src/cmpl/CMPL_Parser.yy"
    { COMP_PAR(LINE_OBJ_TO, 1); ;}
    break;

  case 78:
#line 568 "src/cmpl/CMPL_Parser.yy"
    { COMP_PAR(LINE_OBJ_TO, -1); ;}
    break;

  case 82:
#line 583 "src/cmpl/CMPL_Parser.yy"
    { GLV->sect = (int) token::SECT_CSTR; ;}
    break;

  case 86:
#line 589 "src/cmpl/CMPL_Parser.yy"
    { yyerrok; ;}
    break;

  case 88:
#line 594 "src/cmpl/CMPL_Parser.yy"
    { COMP(LINE_END, 0); ;}
    break;

  case 93:
#line 603 "src/cmpl/CMPL_Parser.yy"
    { switch((yysemantic_stack_[(2) - (2)].i)) {
												case EQ:
											  		COMP_PAR(LINE_MOD, MODE_G); break;
												case GE:
											  		COMP_PAR(LINE_MOD, MODE_U); break;
												case LE:
											  		COMP_PAR(LINE_MOD, MODE_O); break;
											} ;}
    break;

  case 101:
#line 627 "src/cmpl/CMPL_Parser.yy"
    { (yyval.i) = 1; ;}
    break;

  case 102:
#line 630 "src/cmpl/CMPL_Parser.yy"
    { (yyval.i) = 1; ;}
    break;

  case 103:
#line 631 "src/cmpl/CMPL_Parser.yy"
    { (yyval.i) = -1; ;}
    break;

  case 111:
#line 649 "src/cmpl/CMPL_Parser.yy"
    { COMP(TERM_NEG, 0); /* Negation */ ;}
    break;

  case 112:
#line 650 "src/cmpl/CMPL_Parser.yy"
    { COMP(TERM_NEG, 0); /* Negation (um Negation am Anfang zurueckzunehmen) */ ;}
    break;

  case 115:
#line 659 "src/cmpl/CMPL_Parser.yy"
    { COMP(TERM_BEG, 0);
												  COMP_PAR(LIT_VAL_SHORT, (yysemantic_stack_[(0) - (0)].i)); /* Vorzeichen Term */ ;}
    break;

  case 116:
#line 661 "src/cmpl/CMPL_Parser.yy"
    { COMP(TERM_END, 0);
												  PARSE_CONTEXT.var_test_prod((yysemantic_stack_[(2) - (2)].i)); ;}
    break;

  case 117:
#line 665 "src/cmpl/CMPL_Parser.yy"
    { if((yysemantic_stack_[(1) - (1)].i)==0) COMP_SUB(NUM_OP, MULT);
												  (yyval.i) = (yysemantic_stack_[(1) - (1)].i); ;}
    break;

  case 118:
#line 667 "src/cmpl/CMPL_Parser.yy"
    { if((yysemantic_stack_[(3) - (1)].i)==0) COMP_SUB(NUM_OP, MULT);
												  (yyval.i) = (yysemantic_stack_[(3) - (1)].i) + (yysemantic_stack_[(3) - (3)].i); ;}
    break;

  case 119:
#line 671 "src/cmpl/CMPL_Parser.yy"
    { COMP(TERM_VAR_FREE, 1);
												  COMP_INT((yysemantic_stack_[(2) - (1)].exp).symp()->symno * MAX_ANZ_IND_1 + (yysemantic_stack_[(2) - (1)].exp).dim_in);
												  (yyval.i) = 1; ;}
    break;

  case 120:
#line 674 "src/cmpl/CMPL_Parser.yy"
    { (yyval.i) = 0; ;}
    break;

  case 121:
#line 675 "src/cmpl/CMPL_Parser.yy"
    { COMP_SUB_PAR_CNT(F_CONST_FREE, SYM_MAIN, 0, 1);
												  COMP_INT((yysemantic_stack_[(2) - (1)].exp).symp()->symno * MAX_ANZ_IND_1 + (yysemantic_stack_[(2) - (1)].exp).dim_in);
												  (yyval.i) = 0; ;}
    break;

  case 123:
#line 681 "src/cmpl/CMPL_Parser.yy"
    { COMP(IND_TRANSPOSE, 0); ;}
    break;

  case 127:
#line 697 "src/cmpl/CMPL_Parser.yy"
    { (yyval.exp) = (yysemantic_stack_[(1) - (1)].exp); ;}
    break;

  case 128:
#line 698 "src/cmpl/CMPL_Parser.yy"
    { (yyval.exp) = (yysemantic_stack_[(1) - (1)].exp); ;}
    break;

  case 129:
#line 699 "src/cmpl/CMPL_Parser.yy"
    { (yyval.exp) = (yysemantic_stack_[(1) - (1)].exp); ;}
    break;

  case 130:
#line 701 "src/cmpl/CMPL_Parser.yy"
    { (yyval.exp) = (yysemantic_stack_[(1) - (1)].exp); ;}
    break;

  case 131:
#line 702 "src/cmpl/CMPL_Parser.yy"
    { (yyval.exp) = (yysemantic_stack_[(1) - (1)].exp); ;}
    break;

  case 132:
#line 703 "src/cmpl/CMPL_Parser.yy"
    { COMP(FETCH_TMP, 1);
												  //COMP_INT((-$1-2));
												  COMP_INT((yysemantic_stack_[(1) - (1)].exp).symp()->symno);
												  (yyval.exp).set(TPK_VALUE, 0); ;}
    break;

  case 133:
#line 712 "src/cmpl/CMPL_Parser.yy"
    { COMP_PAR_INT(LIT_VAL, (yysemantic_stack_[(1) - (1)].i));
												  (yyval.exp).set(TP_INT, 0); ;}
    break;

  case 134:
#line 714 "src/cmpl/CMPL_Parser.yy"
    { COMP(LIT_VAL, 1);
												  COMP_DOUBLE((yysemantic_stack_[(1) - (1)].d));
												  (yyval.exp).set(TP_DBL, 0); ;}
    break;

  case 135:
#line 717 "src/cmpl/CMPL_Parser.yy"
    { COMP(LIT_VAL, 1);
												  COMP_ISTR((yysemantic_stack_[(1) - (1)].i));
												  (yyval.exp).set(TP_ISTR, 0); ;}
    break;

  case 136:
#line 722 "src/cmpl/CMPL_Parser.yy"
    { /* Anzahl Elemente als Listenzähler auf den Stack */
		  										  COMP(LIT_VAL, 1);
												  COMP_VAL(TPP_LIST_VAL, i, (yysemantic_stack_[(1) - (1)].exp).dim_in);
												  /* Tupelkonstruktion */
												  COMP(CONSTRUCT_TUPEL, 0);
												  (yyval.exp).set(((yysemantic_stack_[(1) - (1)].exp).dim_out > 0 ? TPK_TUPELSET_N((yysemantic_stack_[(1) - (1)].exp).dim_t_rank1) : TPK_TUPEL_N((yysemantic_stack_[(1) - (1)].exp).dim_t_rank1)), 0); ;}
    break;

  case 137:
#line 730 "src/cmpl/CMPL_Parser.yy"
    { COMP_SUB_PAR(CONV_OP, NO_FORMAT, (yysemantic_stack_[(4) - (1)].t));
												  (yyval.exp).set((yysemantic_stack_[(4) - (1)].t), 0); ;}
    break;

  case 138:
#line 734 "src/cmpl/CMPL_Parser.yy"
    { (yysemantic_stack_[(2) - (2)].i) = (int) token::FCT_MAX; ;}
    break;

  case 139:
#line 735 "src/cmpl/CMPL_Parser.yy"
    { COMP_PAR_INT(LIT_VAL, (yysemantic_stack_[(5) - (4)].i));
												  COMP_SUB(AGGR_OP, MAX_VAL);
												  (yyval.exp).set(TPK_VALUE, 0); ;}
    break;

  case 140:
#line 738 "src/cmpl/CMPL_Parser.yy"
    { (yysemantic_stack_[(2) - (2)].i) = (int) token::FCT_MIN; ;}
    break;

  case 141:
#line 739 "src/cmpl/CMPL_Parser.yy"
    { COMP_PAR_INT(LIT_VAL, (yysemantic_stack_[(5) - (4)].i));
												  COMP_SUB(AGGR_OP, MIN_VAL);
												  (yyval.exp).set(TPK_VALUE, 0); ;}
    break;

  case 142:
#line 742 "src/cmpl/CMPL_Parser.yy"
    { /*COMP_PAR(LIT_VAL_SHORT, $3.dim_in);
												  COMP_SUB(SET_INDEXES, IND_GIVEN);*/
												  COMP_SUB_PAR_CNT(DEFSET_EXTNUM, MAX_VAL, 0, 1);
												  COMP_INT((yysemantic_stack_[(4) - (3)].exp).symp()->symno * MAX_ANZ_IND_1 + (yysemantic_stack_[(4) - (3)].exp).dim_in);
												  (yyval.exp).set(TP_INT, 0); ;}
    break;

  case 143:
#line 747 "src/cmpl/CMPL_Parser.yy"
    { (yyval.exp).set(TP_INT, 0); ;}
    break;

  case 144:
#line 749 "src/cmpl/CMPL_Parser.yy"
    { COMP_SUB_PAR(CONV_OP, WITH_FORMAT, TP_ISTR);
												  (yyval.exp).set(TP_ISTR, 0); ;}
    break;

  case 145:
#line 751 "src/cmpl/CMPL_Parser.yy"
    { COMP_SUB(TYPE_STR, TYPE_EXPR);
												  (yyval.exp).set(TP_ISTR, 0); ;}
    break;

  case 146:
#line 753 "src/cmpl/CMPL_Parser.yy"
    { COMP_SUB_PAR_CNT(TYPE_STR, TYPE_VAR, 0, 1);
												  COMP_INT((yysemantic_stack_[(4) - (3)].exp).symp()->symno * MAX_ANZ_IND_1 + (yysemantic_stack_[(4) - (3)].exp).dim_in);
												  (yyval.exp).set(TP_ISTR, 0); ;}
    break;

  case 147:
#line 756 "src/cmpl/CMPL_Parser.yy"
    { COMP_SUB(UTIL_OP, LEN_VAL);
												  (yyval.exp).set(TP_INT, 0); ;}
    break;

  case 148:
#line 758 "src/cmpl/CMPL_Parser.yy"
    { COMP_SUB(MATH_FUNC, (yysemantic_stack_[(4) - (1)].i));
												  (yyval.exp).set(TPU_NUM, 0); ;}
    break;

  case 149:
#line 760 "src/cmpl/CMPL_Parser.yy"
    { COMP(READ_STDIN, 0);
												  (yyval.exp).set(TPK_VALUE, 0); ;}
    break;

  case 150:
#line 762 "src/cmpl/CMPL_Parser.yy"
    { COMP_PAR_INT(LIT_VAL, (yysemantic_stack_[(2) - (2)].exp).dim_in); /*TODO: Anzahl Elemente als besonderer Typ, zur Kennzeichnung das Array*/
												  // Array-Parameter sind derzeit nicht erlaubt
												  if ((yysemantic_stack_[(2) - (2)].exp).dim_elem != 0)
													  PARSE_CONTEXT.output_error_parse(ERROR_TYPE_COMP, ERROR_LVL_NORMAL, "array parameters are not allowed");	/*TODO*/
												  COMP_SUB(CMPL_SOLVE, (yysemantic_stack_[(2) - (1)].i));
												  (yyval.exp).set(TP_INT, 0); ;}
    break;

  case 151:
#line 771 "src/cmpl/CMPL_Parser.yy"
    { (yyval.exp) = PARSE_CONTEXT.parse_type_dev_subsym_op(&(yysemantic_stack_[(4) - (1)].exp), (yysemantic_stack_[(4) - (3)].i));
												  COMP_PAR_INT(LIT_VAL, (yysemantic_stack_[(4) - (4)].exp).dim_in);	//Anzahl muss hier auf den Stack, damit feststeht, wo auf dem Stack die Subsymboltabelle kommt
												  COMP_SUB_PAR_CNT(FETCH_CONST, SYM_SUB, 0, 1);
												  COMP_INT((yyval.exp).sub_sym_ind);
												  if ((yysemantic_stack_[(4) - (4)].exp).dim_in > 0)
												  {
												    if ((yyval.exp).dim_out >= 0 && (yyval.exp).dim_out != (yysemantic_stack_[(4) - (4)].exp).dim_in)
														PARSE_CONTEXT.output_error_parse(ERROR_TYPE_COMP, ERROR_LVL_NORMAL, "array dimensions don't match");/*TODO*/
												  	(yyval.exp).set_dim((yysemantic_stack_[(4) - (4)].exp).dim_in, (yysemantic_stack_[(4) - (4)].exp).dim_out, (yysemantic_stack_[(4) - (4)].exp).dim_fr_end);
												  }
												  if (!((yyval.exp).symp()->isvalid()) && TP_SUB_AUTODEF((yysemantic_stack_[(4) - (1)].exp).tp))
												  {
												  	(yyval.exp).set_dim((yysemantic_stack_[(4) - (4)].exp).dim_in, (yysemantic_stack_[(4) - (4)].exp).dim_out, (yysemantic_stack_[(4) - (4)].exp).dim_fr_end);
												  	PARSE_CONTEXT.init_symbol(&(yyval.exp), PTR_TP_CONST, TPK_VALUE, false);
												  } ;}
    break;

  case 152:
#line 788 "src/cmpl/CMPL_Parser.yy"
    { (yyval.exp) = (yysemantic_stack_[(1) - (1)].exp); ;}
    break;

  case 153:
#line 789 "src/cmpl/CMPL_Parser.yy"
    { (yyval.exp) = (yysemantic_stack_[(1) - (1)].exp); ;}
    break;

  case 154:
#line 790 "src/cmpl/CMPL_Parser.yy"
    { (yyval.exp) = (yysemantic_stack_[(1) - (1)].exp); ;}
    break;

  case 155:
#line 793 "src/cmpl/CMPL_Parser.yy"
    { (yyval.i) = (yysemantic_stack_[(1) - (1)].i); ;}
    break;

  case 156:
#line 794 "src/cmpl/CMPL_Parser.yy"
    { (yyval.i) = (yysemantic_stack_[(1) - (1)].i); ;}
    break;

  case 157:
#line 798 "src/cmpl/CMPL_Parser.yy"
    { (yyval.i) = 0; ;}
    break;

  case 158:
#line 802 "src/cmpl/CMPL_Parser.yy"
    { /* $$.set((void*)$1, TPK_VAR, 0, true, $2.dim_in, $2.dim_out, $2.dim_fr_end); */
												  (yyval.exp) = (yysemantic_stack_[(2) - (1)].exp);
												  if ((yysemantic_stack_[(2) - (2)].exp).dim_in > 0)
													(yyval.exp).set_dim((yysemantic_stack_[(2) - (2)].exp).dim_in, (yysemantic_stack_[(2) - (2)].exp).dim_out, (yysemantic_stack_[(2) - (2)].exp).dim_fr_end);
												;}
    break;

  case 159:
#line 813 "src/cmpl/CMPL_Parser.yy"
    { // TODO: Test ob Indexanzahl passt: $2.dim_in == 0 || $1.dim_out == $2.dim_in
												  //		Wenn $2.dim_in == 0, dann COMP fuer freies Indexinfo; setzen $2.dim_in = $1.dim_out 
												  COMP_PAR(LIT_VAL_SHORT, (yysemantic_stack_[(2) - (2)].exp).dim_in);
												  COMP_SUB(SET_INDEXES, IND_GIVEN);
												  //GLV->index_info.i_anz_ind = $2.dim_in;
												  //GLV->index_info.i_free_ind = ($2.dim_out >= 0 ? $2.dim_out : 0);
									  			  /* $$.set((void*)$1, TPK_VAR, 0, true, $2.dim_in, $2.dim_out, $2.dim_fr_end); */
												  (yyval.exp) = (yysemantic_stack_[(2) - (1)].exp);
												  if ((yysemantic_stack_[(2) - (2)].exp).dim_in > 0)
													(yyval.exp).set_dim((yysemantic_stack_[(2) - (2)].exp).dim_in, (yysemantic_stack_[(2) - (2)].exp).dim_out, (yysemantic_stack_[(2) - (2)].exp).dim_fr_end);
												;}
    break;

  case 160:
#line 827 "src/cmpl/CMPL_Parser.yy"
    { (yyval.i) = 1; ;}
    break;

  case 161:
#line 828 "src/cmpl/CMPL_Parser.yy"
    { (yysemantic_stack_[(2) - (2)].i) = (yysemantic_stack_[(2) - (-1)].i); ;}
    break;

  case 162:
#line 829 "src/cmpl/CMPL_Parser.yy"
    { (yyval.i) = (yysemantic_stack_[(4) - (4)].i) + 1; ;}
    break;

  case 163:
#line 833 "src/cmpl/CMPL_Parser.yy"
    { (yyval.i) = 0; ;}
    break;

  case 165:
#line 838 "src/cmpl/CMPL_Parser.yy"
    { if ((yysemantic_stack_[(2) - (2)].exp).dim_in > 0)
												  {
												  	if ((yysemantic_stack_[(2) - (1)].exp).dim_out != (yysemantic_stack_[(2) - (2)].exp).dim_in)
														PARSE_CONTEXT.output_error_parse(ERROR_TYPE_COMP, ERROR_LVL_NORMAL, "array dimensions don't match");/*TODO*/
												  	COMP_PAR(LIT_VAL_SHORT, (yysemantic_stack_[(2) - (2)].exp).dim_in);
												  	COMP_SUB(SET_INDEXES, IND_GIVEN);
												  }
												  else
												  {
													COMP_PAR(LIT_VAL_SHORT, (yysemantic_stack_[(2) - (1)].exp).dim_out);
												  	COMP_SUB(SET_INDEXES, IND_FREE);
												  }
												  COMP_SUB_PAR_CNT(FETCH_VALS, SYM_MAIN, 0, 1);
												  COMP_INT((yysemantic_stack_[(2) - (1)].exp).symp()->symno * MAX_ANZ_IND_1 + (yysemantic_stack_[(2) - (1)].exp).dim_out);
												  if((yysemantic_stack_[(2) - (-1)].i) == (int) token::FCT_MAX) COMP_SUB(AGGR_OP, MAX_VAL_NULL);
												  else COMP_SUB(AGGR_OP, MIN_VAL_NULL); ;}
    break;

  case 166:
#line 855 "src/cmpl/CMPL_Parser.yy"
    { EXPSTR exp = PARSE_CONTEXT.parse_type_dev_subsym_op(&(yysemantic_stack_[(4) - (1)].exp), (yysemantic_stack_[(4) - (3)].i));
												  if ((yysemantic_stack_[(4) - (4)].exp).dim_in > 0)
												  {
												  	if (exp.dim_out >= 0 && exp.dim_out != (yysemantic_stack_[(4) - (4)].exp).dim_in)
														PARSE_CONTEXT.output_error_parse(ERROR_TYPE_COMP, ERROR_LVL_NORMAL, "array dimensions don't match");/*TODO*/
												  	COMP_PAR(LIT_VAL_SHORT, (yysemantic_stack_[(4) - (4)].exp).dim_in);
												  	COMP_SUB(SET_INDEXES, IND_GIVEN);
												  }
												  else
												  {
													COMP_PAR(LIT_VAL_SHORT, exp.dim_out);
												  	COMP_SUB(SET_INDEXES, IND_FREE);
												  }
												  COMP_SUB_PAR_CNT(FETCH_VALS, SYM_SUB, 0, 1);
												  COMP_INT(exp.sub_sym_ind);
												  if (!(exp.symp()->isvalid()) && TP_SUB_AUTODEF((yysemantic_stack_[(4) - (1)].exp).tp))
												  {
													exp.set_dim((yysemantic_stack_[(4) - (4)].exp).dim_in, (yysemantic_stack_[(4) - (4)].exp).dim_out, (yysemantic_stack_[(4) - (4)].exp).dim_fr_end);
												  	PARSE_CONTEXT.init_symbol(&exp, PTR_TP_CONST, TPK_VALUE, false);
												  }
												  if((yysemantic_stack_[(4) - (-1)].i) == (int) token::FCT_MAX) COMP_SUB(AGGR_OP, MAX_VAL_NULL);
												  else COMP_SUB(AGGR_OP, MIN_VAL_NULL); ;}
    break;

  case 167:
#line 880 "src/cmpl/CMPL_Parser.yy"
    { /*$$.set((void*)$1, TPK_VAR, 0, true, $2.dim_in, $2.dim_out, $2.dim_fr_end);*/ /*$$ = $1 * MAX_ANZ_IND_1 + $2.dim_in;*/
												  if ((yysemantic_stack_[(2) - (2)].exp).dim_in > 0)
												  {
												  	if ((yysemantic_stack_[(2) - (1)].exp).dim_out != (yysemantic_stack_[(2) - (2)].exp).dim_in)
														PARSE_CONTEXT.output_error_parse(ERROR_TYPE_COMP, ERROR_LVL_NORMAL, "array dimensions don't match");/*TODO*/
												  	COMP_PAR(LIT_VAL_SHORT, (yysemantic_stack_[(2) - (2)].exp).dim_in);
												  	COMP_SUB(SET_INDEXES, IND_GIVEN);
												  }
												  else
												  {
													COMP_PAR(LIT_VAL_SHORT, (yysemantic_stack_[(2) - (1)].exp).dim_out);
												  	COMP_SUB(SET_INDEXES, IND_FREE);
												  }
												  (yyval.exp) = (yysemantic_stack_[(2) - (1)].exp);
												  if ((yysemantic_stack_[(2) - (2)].exp).dim_in > 0)
													(yyval.exp).set_dim((yysemantic_stack_[(2) - (2)].exp).dim_in, (yysemantic_stack_[(2) - (2)].exp).dim_out, (yysemantic_stack_[(2) - (2)].exp).dim_fr_end);
												  else
												  	(yyval.exp).set_dim((yysemantic_stack_[(2) - (1)].exp).dim_out, (yysemantic_stack_[(2) - (1)].exp).dim_out, (yysemantic_stack_[(2) - (1)].exp).dim_out);
												;}
    break;

  case 168:
#line 901 "src/cmpl/CMPL_Parser.yy"
    { (yyval.exp) = (yysemantic_stack_[(1) - (1)].exp); ;}
    break;

  case 169:
#line 902 "src/cmpl/CMPL_Parser.yy"
    { (yyval.exp) = (yysemantic_stack_[(1) - (1)].exp); ;}
    break;

  case 170:
#line 908 "src/cmpl/CMPL_Parser.yy"
    { /*COMP_PAR(LIT_VAL_SHORT, $1.dim_in);
												  COMP_SUB(SET_INDEXES, IND_GIVEN);*/
												  COMP(DEF_FUNC, 1);
												  COMP_INT((yysemantic_stack_[(1) - (1)].exp).symp()->symno * MAX_ANZ_IND_1 + (yysemantic_stack_[(1) - (1)].exp).dim_in); ;}
    break;

  case 171:
#line 912 "src/cmpl/CMPL_Parser.yy"
    { COMP_PAR(LIT_VAL_SHORT, (yysemantic_stack_[(2) - (2)].exp).dim_in);
												  COMP_SUB(SET_INDEXES, IND_GIVEN);
												  COMP(DEL_INDEXES, 0);
												  COMP_PAR(LIT_VAL_SHORT, 0); ;}
    break;

  case 172:
#line 916 "src/cmpl/CMPL_Parser.yy"
    { COMP_PAR(LIT_VAL_SHORT, 1); ;}
    break;

  case 173:
#line 922 "src/cmpl/CMPL_Parser.yy"
    { /*COMP_PAR(LIT_VAL_SHORT, 0);*/
										  (yyval.exp).set_dim(0, 0, 0);
										  /*$$ = 0;*/ ;}
    break;

  case 174:
#line 925 "src/cmpl/CMPL_Parser.yy"
    { (yyval.exp) = (yysemantic_stack_[(1) - (1)].exp);
										  // Undefinierte Symbole hier nicht erlaubt
										  if ((yysemantic_stack_[(1) - (1)].exp).dim_t_undef > 0)
											PARSE_CONTEXT.output_error_parse(ERROR_TYPE_COMP, ERROR_LVL_NORMAL, "undefined symbols in list");/*TODO*/
										;}
    break;

  case 175:
#line 962 "src/cmpl/CMPL_Parser.yy"
    { (yyval.f) = false; ;}
    break;

  case 176:
#line 963 "src/cmpl/CMPL_Parser.yy"
    { (yyval.f) = true; ;}
    break;

  case 177:
#line 968 "src/cmpl/CMPL_Parser.yy"
    { (yyval.exp) = (yysemantic_stack_[(1) - (1)].exp); ;}
    break;

  case 178:
#line 969 "src/cmpl/CMPL_Parser.yy"
    { (yyval.exp) = (yysemantic_stack_[(1) - (1)].exp); ;}
    break;

  case 179:
#line 970 "src/cmpl/CMPL_Parser.yy"
    { (yyval.exp) = (yysemantic_stack_[(1) - (1)].exp); ;}
    break;

  case 180:
#line 971 "src/cmpl/CMPL_Parser.yy"
    { (yyval.exp) = (yysemantic_stack_[(1) - (1)].exp); ;}
    break;

  case 181:
#line 978 "src/cmpl/CMPL_Parser.yy"
    { (yyval.exp) = (yysemantic_stack_[(1) - (1)].exp); ;}
    break;

  case 182:
#line 979 "src/cmpl/CMPL_Parser.yy"
    { (yyval.exp) = (yysemantic_stack_[(1) - (1)].exp); ;}
    break;

  case 183:
#line 980 "src/cmpl/CMPL_Parser.yy"
    { (yyval.exp) = (yysemantic_stack_[(1) - (1)].exp); ;}
    break;

  case 184:
#line 981 "src/cmpl/CMPL_Parser.yy"
    { (yyval.exp) = (yysemantic_stack_[(1) - (1)].exp); ;}
    break;

  case 185:
#line 982 "src/cmpl/CMPL_Parser.yy"
    { (yyval.exp) = (yysemantic_stack_[(1) - (1)].exp); ;}
    break;

  case 186:
#line 983 "src/cmpl/CMPL_Parser.yy"
    { COMP_SUB(LOG_OP, NOT);
												  (yyval.exp).set(TP_BIN, 0); ;}
    break;

  case 187:
#line 988 "src/cmpl/CMPL_Parser.yy"
    { (yyval.exp) = (yysemantic_stack_[(1) - (1)].exp); ;}
    break;

  case 188:
#line 993 "src/cmpl/CMPL_Parser.yy"
    { COMP(LIT_VAL, 1);
												  COMP_VAL(TPP_NULL, i, 0); ;}
    break;

  case 189:
#line 996 "src/cmpl/CMPL_Parser.yy"
    { EXPSTR a, b = PARSE_CONTEXT.parse_type_dev_switch(&(yysemantic_stack_[(6) - (4)].exp), &(yysemantic_stack_[(6) - (5)].exp));
												  a.set(TPP_NULL, 0);
												  (yyval.exp) = PARSE_CONTEXT.parse_type_dev_num_op(ADD, &a, &b); ;}
    break;

  case 190:
#line 1001 "src/cmpl/CMPL_Parser.yy"
    { COMP_PAR(LIT_VAL_SHORT, 0); ;}
    break;

  case 191:
#line 1003 "src/cmpl/CMPL_Parser.yy"
    { EXPSTR a, b = PARSE_CONTEXT.parse_type_dev_switch(&(yysemantic_stack_[(6) - (4)].exp), &(yysemantic_stack_[(6) - (5)].exp));
												  a.set(TP_INT, 0);
												  (yyval.exp) = PARSE_CONTEXT.parse_type_dev_num_op(ADD, &a, &b);
												  if (TYPECLASS((yyval.exp).tp) != TPK_VALUE)
													PARSE_CONTEXT.output_error_parse(ERROR_TYPE_COMP, ERROR_LVL_NORMAL, ERROR_NOT_A_NUMBER_PARSE);
												  COMP_PAR(LIT_SET, TP_SET_ENUM);
												  (yyval.exp).set(TPK_SET, 0); ;}
    break;

  case 192:
#line 1013 "src/cmpl/CMPL_Parser.yy"
    { COMP_SUB((TYPECLASS((yysemantic_stack_[(3) - (1)].exp).tp)==TPK_SET ? SET_OP : NUM_OP), ADD);
															  (yyval.exp) = PARSE_CONTEXT.parse_type_dev_num_op(ADD, &(yysemantic_stack_[(3) - (1)].exp), &(yysemantic_stack_[(3) - (3)].exp)); ;}
    break;

  case 193:
#line 1015 "src/cmpl/CMPL_Parser.yy"
    { COMP_SUB(NUM_OP, SUB);
															  (yyval.exp) = PARSE_CONTEXT.parse_type_dev_num_op(SUB, &(yysemantic_stack_[(3) - (1)].exp), &(yysemantic_stack_[(3) - (3)].exp)); ;}
    break;

  case 194:
#line 1017 "src/cmpl/CMPL_Parser.yy"
    { COMP_SUB((TYPECLASS((yysemantic_stack_[(3) - (1)].exp).tp)==TPK_SET ? SET_OP : NUM_OP), MULT);
															  (yyval.exp) = PARSE_CONTEXT.parse_type_dev_num_op(MULT, &(yysemantic_stack_[(3) - (1)].exp), &(yysemantic_stack_[(3) - (3)].exp)); ;}
    break;

  case 195:
#line 1019 "src/cmpl/CMPL_Parser.yy"
    { COMP_SUB(NUM_OP, DIVD);
															  (yyval.exp) = PARSE_CONTEXT.parse_type_dev_num_op(DIVD, &(yysemantic_stack_[(3) - (1)].exp), &(yysemantic_stack_[(3) - (3)].exp)); ;}
    break;

  case 196:
#line 1021 "src/cmpl/CMPL_Parser.yy"
    { COMP_SUB(NUM_OP, POW);
															  (yyval.exp) = PARSE_CONTEXT.parse_type_dev_num_op(POW, &(yysemantic_stack_[(3) - (1)].exp), &(yysemantic_stack_[(3) - (3)].exp)); ;}
    break;

  case 197:
#line 1023 "src/cmpl/CMPL_Parser.yy"
    { COMP_SUB(NUM_OP, DIVI);
															  (yyval.exp) = PARSE_CONTEXT.parse_type_dev_num_op(DIVI, &(yysemantic_stack_[(3) - (1)].exp), &(yysemantic_stack_[(3) - (3)].exp)); ;}
    break;

  case 198:
#line 1025 "src/cmpl/CMPL_Parser.yy"
    { COMP_SUB(NUM_OP, MODI);
															  (yyval.exp) = PARSE_CONTEXT.parse_type_dev_num_op(MODI, &(yysemantic_stack_[(3) - (1)].exp), &(yysemantic_stack_[(3) - (3)].exp)); ;}
    break;

  case 199:
#line 1027 "src/cmpl/CMPL_Parser.yy"
    { EXPSTR t;
															  t.set(TPP_EMPTY, 0);
															  COMP_SUB(NUM_OP, NEG);
															  (yyval.exp) = PARSE_CONTEXT.parse_type_dev_num_op(NEG, &(yysemantic_stack_[(2) - (2)].exp), &t); ;}
    break;

  case 200:
#line 1031 "src/cmpl/CMPL_Parser.yy"
    { EXPSTR t;
															  t.set(TPP_EMPTY, 0);
															  COMP_SUB_PAR(CONV_OP, NO_FORMAT, TPU_NUM);	/* fuer sichern nummerischer Wert */
															  (yyval.exp) = PARSE_CONTEXT.parse_type_dev_num_op(NEG, &(yysemantic_stack_[(2) - (2)].exp), &t); ;}
    break;

  case 201:
#line 1047 "src/cmpl/CMPL_Parser.yy"
    { COMP_SUB(LOG_OP, AND);
															  (yyval.exp).set(TP_BIN, 0); ;}
    break;

  case 202:
#line 1049 "src/cmpl/CMPL_Parser.yy"
    { COMP_SUB(LOG_OP, OR);
															  (yyval.exp).set(TP_BIN, 0); ;}
    break;

  case 203:
#line 1054 "src/cmpl/CMPL_Parser.yy"
    { COMP_SUB(CMP_OP, EQ);
															  (yyval.exp) = PARSE_CONTEXT.parse_type_dev_cmp_op(EQ, &(yysemantic_stack_[(3) - (1)].exp), &(yysemantic_stack_[(3) - (3)].exp)); ;}
    break;

  case 204:
#line 1056 "src/cmpl/CMPL_Parser.yy"
    { COMP_SUB(CMP_OP, EQ2);
															  (yyval.exp) = PARSE_CONTEXT.parse_type_dev_cmp_op(EQ2, &(yysemantic_stack_[(3) - (1)].exp), &(yysemantic_stack_[(3) - (3)].exp)); ;}
    break;

  case 205:
#line 1058 "src/cmpl/CMPL_Parser.yy"
    { COMP_SUB(CMP_OP, EQ); COMP_SUB(LOG_OP, NOT);
															  (yyval.exp) = PARSE_CONTEXT.parse_type_dev_cmp_op(EQ, &(yysemantic_stack_[(3) - (1)].exp), &(yysemantic_stack_[(3) - (3)].exp)); ;}
    break;

  case 206:
#line 1060 "src/cmpl/CMPL_Parser.yy"
    { COMP_SUB(CMP_OP, EQ2); COMP_SUB(LOG_OP, NOT);
															  (yyval.exp) = PARSE_CONTEXT.parse_type_dev_cmp_op(EQ2, &(yysemantic_stack_[(3) - (1)].exp), &(yysemantic_stack_[(3) - (3)].exp)); ;}
    break;

  case 207:
#line 1062 "src/cmpl/CMPL_Parser.yy"
    { COMP_SUB(CMP_OP, LE);
															  (yyval.exp) = PARSE_CONTEXT.parse_type_dev_cmp_op(LE, &(yysemantic_stack_[(3) - (1)].exp), &(yysemantic_stack_[(3) - (3)].exp)); ;}
    break;

  case 208:
#line 1064 "src/cmpl/CMPL_Parser.yy"
    { if (TYPECLASS((yysemantic_stack_[(3) - (1)].exp).tp) == TPK_SET)
																{ COMP_SUB(CMP_OP, GT); }	/*TODO: GT auch fuer TPK_VALUE*/
															  else
																{ COMP_SUB(CMP_OP, LE); COMP_SUB(LOG_OP, NOT); }
															  (yyval.exp) = PARSE_CONTEXT.parse_type_dev_cmp_op(GT, &(yysemantic_stack_[(3) - (1)].exp), &(yysemantic_stack_[(3) - (3)].exp)); ;}
    break;

  case 209:
#line 1069 "src/cmpl/CMPL_Parser.yy"
    { COMP_SUB(CMP_OP, GE);
															  (yyval.exp) = PARSE_CONTEXT.parse_type_dev_cmp_op(GE, &(yysemantic_stack_[(3) - (1)].exp), &(yysemantic_stack_[(3) - (3)].exp)); ;}
    break;

  case 210:
#line 1071 "src/cmpl/CMPL_Parser.yy"
    { if (TYPECLASS((yysemantic_stack_[(3) - (1)].exp).tp) == TPK_SET)
																{ COMP_SUB(CMP_OP, LT); }	/*TODO: LT auch fuer TPK_VALUE*/
															  else
																{ COMP_SUB(CMP_OP, GE); COMP_SUB(LOG_OP, NOT); }
															  (yyval.exp) = PARSE_CONTEXT.parse_type_dev_cmp_op(LT, &(yysemantic_stack_[(3) - (1)].exp), &(yysemantic_stack_[(3) - (3)].exp)); ;}
    break;

  case 211:
#line 1076 "src/cmpl/CMPL_Parser.yy"
    { COMP_SUB(CMP_OP, INSET);
															  (yyval.exp) = PARSE_CONTEXT.parse_type_dev_cmp_op(INSET, &(yysemantic_stack_[(3) - (1)].exp), &(yysemantic_stack_[(3) - (3)].exp)); ;}
    break;

  case 212:
#line 1103 "src/cmpl/CMPL_Parser.yy"
    { COMP_SUB(NUM_OP, ADD);
												 			  (yyval.exp) = (yysemantic_stack_[(1) - (1)].exp); ;}
    break;

  case 213:
#line 1108 "src/cmpl/CMPL_Parser.yy"
    { (yyval.exp) = PARSE_CONTEXT.parse_type_dev_switch(&(yysemantic_stack_[(3) - (2)].exp), &(yysemantic_stack_[(3) - (3)].exp)); ;}
    break;

  case 214:
#line 1109 "src/cmpl/CMPL_Parser.yy"
    { (yyval.exp).set(TPP_EMPTY, 0); ;}
    break;

  case 215:
#line 1112 "src/cmpl/CMPL_Parser.yy"
    { COMP_SUB(COUNT_OP, CNT_INC);
												 			  (yyval.exp) = (yysemantic_stack_[(1) - (1)].exp); ;}
    break;

  case 216:
#line 1117 "src/cmpl/CMPL_Parser.yy"
    { (yyval.exp) = PARSE_CONTEXT.parse_type_dev_switch(&(yysemantic_stack_[(3) - (2)].exp), &(yysemantic_stack_[(3) - (3)].exp)); ;}
    break;

  case 217:
#line 1118 "src/cmpl/CMPL_Parser.yy"
    { (yyval.exp).set(TPP_EMPTY, 0); ;}
    break;

  case 218:
#line 1137 "src/cmpl/CMPL_Parser.yy"
    { // TODO: Test ob Indexanzahl passt: $2.dim_in == 0 || $1.dim_out == $2.dim_in
		                                      //        Wenn $2.dim_in == 0, dann COMP fuer freies Indexinfo; setzen $2.dim_in = $1.dim_out
											  (yyval.exp) = (yysemantic_stack_[(2) - (1)].exp);
											  if ((yysemantic_stack_[(2) - (2)].exp).dim_in > 0)
											  	(yyval.exp).set_dim((yysemantic_stack_[(2) - (2)].exp).dim_in, (yysemantic_stack_[(2) - (2)].exp).dim_out, (yysemantic_stack_[(2) - (2)].exp).dim_fr_end);
		// DEFS_VALUE d_vekt		{
											  COMP_PAR(LIT_VAL_SHORT, (yysemantic_stack_[(2) - (2)].exp).dim_in);
											  COMP_SUB(SET_INDEXES, IND_GIVEN);
											  //GLV->index_info.i_anz_ind = $2.dim_in;
											  //GLV->index_info.i_free_ind = ($2.dim_out >= 0 ? $2.dim_out : 0);
											  /*$$.set((void*)$1, TPK_VALUE, 0, true, $2.dim_in, $2.dim_out, $2.dim_fr_end);*/ ;}
    break;

  case 219:
#line 1163 "src/cmpl/CMPL_Parser.yy"
    { (yyval.exp) = (yysemantic_stack_[(2) - (1)].exp);
											  if ((yysemantic_stack_[(2) - (2)].exp).dim_in > 0)
											  	(yyval.exp).set_dim((yysemantic_stack_[(2) - (2)].exp).dim_in, (yysemantic_stack_[(2) - (2)].exp).dim_out, (yysemantic_stack_[(2) - (2)].exp).dim_fr_end);
											  COMP_PAR_INT(LIT_VAL, (yysemantic_stack_[(2) - (2)].exp).dim_in);	//Anzahl muss hier auf den Stack, damit Dimensionalität geprüft werden kann
											  COMP_SUB_PAR_CNT(FETCH_CONST, SYM_MAIN, 0, 1);
											  COMP_INT((yyval.exp).symp()->symno * MAX_ANZ_IND_1 + (yyval.exp).dim_in); ;}
    break;

  case 220:
#line 1183 "src/cmpl/CMPL_Parser.yy"
    { (yyval.exp) = (yysemantic_stack_[(3) - (2)].exp);
													  (yyval.exp).symtab = NULL;
													  (yyval.exp).lval = false;
													  // Undefinierte Symbole hier nicht erlaubt
													  if ((yysemantic_stack_[(3) - (2)].exp).dim_t_undef > 0)
													  	PARSE_CONTEXT.output_error_parse(ERROR_TYPE_COMP, ERROR_LVL_NORMAL, "undefined symbols in list");/*TODO*/
													  if ((yysemantic_stack_[(3) - (2)].exp).dim_in == 1 && (yysemantic_stack_[(3) - (2)].exp).dim_fr_end == 0)
													  {
													  	// Arithmetische Klammer um einzelnen Ausdruck; Dimensionalität davon übernehmen
														(yyval.exp).set_dim(1, (yysemantic_stack_[(3) - (2)].exp).dim_elem, 0, (yysemantic_stack_[(3) - (2)].exp).dim_elem);
													  }
													  else
													  {
													  	// Liste von Werten, die ein Array bilden
														if ((yysemantic_stack_[(3) - (2)].exp).dim_elem < 0)
															PARSE_CONTEXT.output_error_parse(ERROR_TYPE_COMP, ERROR_LVL_NORMAL, "array dimensions not known");/*TODO*/
														// Wenn letztes Element fehlt, dann zählt es nicht
														if ((yysemantic_stack_[(3) - (2)].exp).dim_fr_end > 0)
														{
															BACK_ADDR_COMP(1);
															(yyval.exp).dim_in--;
															(yyval.exp).dim_fr_end--;
														}
														COMP_PAR_INT(LIT_VAL, (yyval.exp).dim_in);	/*TODO: Anzahl Elemente als besonderer Typ, zur Kennzeichnung das Array*/
														// Eine weitere Dimension kommt zu der der Elemente
														(yyval.exp).set_dim((yysemantic_stack_[(3) - (2)].exp).dim_in, (yysemantic_stack_[(3) - (2)].exp).dim_elem+1, (yyval.exp).dim_fr_end, (yysemantic_stack_[(3) - (2)].exp).dim_elem);
													  } ;}
    break;

  case 221:
#line 1212 "src/cmpl/CMPL_Parser.yy"
    { // (Auf passende Anzahl/Typ/Dimensionalität der Elemente muss Funktion selbst prüfen; und gegebenenfalls noch Anzahl Elemente aus $$.dim_in compilieren
													  (yyval.exp) = (yysemantic_stack_[(3) - (2)].exp);
													  (yyval.exp).lval = false;
													  // Undefinierte Symbole hier nicht erlaubt
													  if ((yysemantic_stack_[(3) - (2)].exp).dim_t_undef > 0)
													  	PARSE_CONTEXT.output_error_parse(ERROR_TYPE_COMP, ERROR_LVL_NORMAL, "undefined symbols in list");/*TODO*/
													  // Leere Liste richtig darstellen
													  if ((yysemantic_stack_[(3) - (2)].exp).dim_in == 1 && (yysemantic_stack_[(3) - (2)].exp).dim_fr_end == 1)
													  {
													  	BACK_ADDR_COMP(1);
													  	(yyval.exp).set_dim(0, 0, 0, 0);
													  }
													  else
													  {
														  // Fehlende Elemente sollten nicht vorkommen
														  if ((yysemantic_stack_[(3) - (2)].exp).dim_fr_end > 0)
															PARSE_CONTEXT.output_error_parse(ERROR_TYPE_COMP, ERROR_LVL_NORMAL, "no empty list elements allowed");/*TODO*/
													  } ;}
    break;

  case 222:
#line 1232 "src/cmpl/CMPL_Parser.yy"
    { // Auf einfachen skalaren Funktionsparameter prüfen
													  if ((yysemantic_stack_[(1) - (1)].exp).dim_in != 1 && (yysemantic_stack_[(1) - (1)].exp).dim_elem != 0)
														PARSE_CONTEXT.output_error_parse(ERROR_TYPE_COMP, ERROR_LVL_NORMAL, "function argument must be a single scalar value");/*TODO*/
															//TODO: Prüfung besser in der Regel der Funktion, damit Fehlermeldung Funktionsname zeigen kann
													  (yyval.exp) = (yysemantic_stack_[(1) - (1)].exp);
													  (yyval.exp).set_dim(1, 0, 0, 0); ;}
    break;

  case 223:
#line 1240 "src/cmpl/CMPL_Parser.yy"
    { // Alle Listenelemente müssen skalar sein
													  if ((yysemantic_stack_[(3) - (2)].exp).dim_elem != 0)
														PARSE_CONTEXT.output_error_parse(ERROR_TYPE_COMP, ERROR_LVL_NORMAL, "index values must be scalar");/*TODO*/
													  (yyval.exp) = (yysemantic_stack_[(3) - (2)].exp);
													  // Undefinierte Elemente nur erlaubt, wenn potentieller lvalue
													  if ((yysemantic_stack_[(3) - (2)].exp).dim_t_undef > 0 && !(yysemantic_stack_[(3) - (2)].exp).lval)
													  	PARSE_CONTEXT.output_error_parse(ERROR_TYPE_COMP, ERROR_LVL_NORMAL, "undefined symbols in list");/*TODO*/
													;}
    break;

  case 224:
#line 1250 "src/cmpl/CMPL_Parser.yy"
    { (yyval.exp) = (yysemantic_stack_[(1) - (1)].exp); ;}
    break;

  case 225:
#line 1251 "src/cmpl/CMPL_Parser.yy"
    { (yyval.exp) = PARSE_CONTEXT.parse_type_dev_array(&(yysemantic_stack_[(3) - (3)].exp), &(yysemantic_stack_[(3) - (1)].exp));	/*TODO: in parse_type_dev_array dim-Bestimmung einbeziehen */
													  (yyval.exp).set_dim((yysemantic_stack_[(3) - (1)].exp).dim_in + (yysemantic_stack_[(3) - (3)].exp).dim_in, (yysemantic_stack_[(3) - (1)].exp).dim_out + (yysemantic_stack_[(3) - (3)].exp).dim_out,
														(yysemantic_stack_[(3) - (3)].exp).dim_fr_end + ((yysemantic_stack_[(3) - (3)].exp).dim_fr_end==(yysemantic_stack_[(3) - (3)].exp).dim_in ? (yysemantic_stack_[(3) - (1)].exp).dim_fr_end : 0),
														((yysemantic_stack_[(3) - (1)].exp).dim_elem==(yysemantic_stack_[(3) - (3)].exp).dim_elem || (yysemantic_stack_[(3) - (3)].exp).dim_fr_end==(yysemantic_stack_[(3) - (3)].exp).dim_in ? (yysemantic_stack_[(3) - (1)].exp).dim_elem :
															((yysemantic_stack_[(3) - (1)].exp).dim_fr_end==(yysemantic_stack_[(3) - (1)].exp).dim_in ? (yysemantic_stack_[(3) - (3)].exp).dim_elem : -1)),
														(yysemantic_stack_[(3) - (1)].exp).dim_t_rank1 + (yysemantic_stack_[(3) - (3)].exp).dim_t_rank1, ((yysemantic_stack_[(3) - (1)].exp).dim_t_rank2 == (yysemantic_stack_[(3) - (3)].exp).dim_t_rank2 ? (yysemantic_stack_[(3) - (1)].exp).dim_t_rank2 : -1), (yysemantic_stack_[(3) - (1)].exp).dim_t_undef + (yysemantic_stack_[(3) - (3)].exp).dim_t_undef);
													  (yyval.exp).lval = ((yysemantic_stack_[(3) - (1)].exp).lval && (yysemantic_stack_[(3) - (3)].exp).lval); ;}
    break;

  case 226:
#line 1260 "src/cmpl/CMPL_Parser.yy"
    { COMP_PAR(LIT_SET, TP_SET_ALL);
                                                      (yyval.exp).tp = TPP_EMPTY;
													  (yyval.exp).set_dim(1, 1, 1, 0, 1, 1, 0);
													  (yyval.exp).lval = false; ;}
    break;

  case 227:
#line 1264 "src/cmpl/CMPL_Parser.yy"
    { // Undefinierte Symbole hier nicht erlaubt
													  if ((yysemantic_stack_[(1) - (1)].exp).dim_t_undef > 0)
													  	PARSE_CONTEXT.output_error_parse(ERROR_TYPE_COMP, ERROR_LVL_NORMAL, "undefined symbols in list");/*TODO*/
													  (yyval.exp) = (yysemantic_stack_[(1) - (1)].exp);
													  (yyval.exp).set_dim(1, ((yysemantic_stack_[(1) - (1)].exp).dim_out==0 && (TYPECLASS((yysemantic_stack_[(1) - (1)].exp).tp)==TPK_SET || TP_IS_TUPELSET((yysemantic_stack_[(1) - (1)].exp).tp)) ? 1 : 0), 0, (yysemantic_stack_[(1) - (1)].exp).dim_out,
													  	(TP_IS_TUPEL((yysemantic_stack_[(1) - (1)].exp).tp) || TP_IS_TUPELSET((yysemantic_stack_[(1) - (1)].exp).tp) ? RANK_TP_TUPEL((yysemantic_stack_[(1) - (1)].exp).tp) : 1),
														(TP_IS_TUPEL((yysemantic_stack_[(1) - (1)].exp).tp) || TP_IS_TUPELSET((yysemantic_stack_[(1) - (1)].exp).tp) ? RANK_TP_TUPEL((yysemantic_stack_[(1) - (1)].exp).tp) : 1), 0); ;}
    break;

  case 228:
#line 1271 "src/cmpl/CMPL_Parser.yy"
    { (yyval.exp) = (yysemantic_stack_[(1) - (1)].exp);
													  (yyval.exp).dim_t_undef = 1;
													  // Compilieren vorläufigen Zugriff auf Symbol; muss später ersetzt werden
											  		  COMP_SUB_PAR_CNT(FETCH_CONST, SYM_UNDEF_ERROR, 0, 1);
											  		  COMP_INT((yyval.exp).symp()->symno * MAX_ANZ_IND_1); ;}
    break;

  case 229:
#line 1281 "src/cmpl/CMPL_Parser.yy"
    { (yyval.exp) = (yysemantic_stack_[(1) - (1)].exp); ;}
    break;

  case 230:
#line 1282 "src/cmpl/CMPL_Parser.yy"
    { (yyval.exp).set(TPK_VALUE, -1);
													  if (TYPECLASS((yysemantic_stack_[(2) - (2)].exp).tp) != TPK_VALUE)
														PARSE_CONTEXT.output_error_parse(ERROR_TYPE_COMP, ERROR_LVL_NORMAL, "wrong argument type for function");/*TODO*/
													  COMP(READ_CSV, 1);	//TODO: READ_CSV verwendet eigenständigen Parameter statt bisher eingebetteten
													  (yyval.exp).addr_dim = ADDR_COMP; 
													  COMP_INT(0); ;}
    break;

  case 231:
#line 1321 "src/cmpl/CMPL_Parser.yy"
    { if ((yysemantic_stack_[(2) - (2)].exp).dim_in == 0)
													  {
													    // Leerer Set
													  	COMP_PAR(LIT_SET, TP_SET_EMPTY);
														(yyval.exp).set(TP_SET_EMPTY, 0);
													  }
													  else if (TP_IS_TUPEL((yysemantic_stack_[(2) - (2)].exp).tp) || TP_IS_TUPELSET((yysemantic_stack_[(2) - (2)].exp).tp))
													  {
													  	// Tupelset konstruieren
		  										  		COMP(LIT_VAL, 1);
												  		COMP_VAL(TPP_LIST_VAL, i, (yysemantic_stack_[(2) - (2)].exp).dim_in);
												  		COMP(CONSTRUCT_TUPELSET, 0);
												  		(yyval.exp).set(TPK_TUPELSET_N((yysemantic_stack_[(2) - (2)].exp).dim_t_rank2), 0);
													  }
													  else if ((yysemantic_stack_[(2) - (2)].exp).dim_in == 1)
													  {
													  	// Konvertierung einzelnes Argument in einen Set
														COMP_SUB_PAR(CONV_OP, NO_FORMAT, TPK_SET);
														(yyval.exp).set(TPK_SET, 0);
													  }
													  else
													  {
														// Aufzählungsset aus Elementen
													  	EXPSTR n;
														n.set(TPP_NULL, 0);
														(yyval.exp) = PARSE_CONTEXT.parse_type_dev_lit_set(TP_SET_ENUM, &(yysemantic_stack_[(2) - (2)].exp), &n, &n);
														COMP_PAR(LIT_VAL_SHORT, (yysemantic_stack_[(2) - (2)].exp).dim_in);		/*TODO:Typ für Elementanzahl*/
														COMP_PAR(LIT_SET, TP_SET_ENUM);
													  }
													;}
    break;

  case 232:
#line 1360 "src/cmpl/CMPL_Parser.yy"
    { COMP_PAR(LIT_SET, TP_SET_ALL_NUM);
													  (yyval.exp).set(TP_SET_ALL_NUM, 0); ;}
    break;

  case 233:
#line 1362 "src/cmpl/CMPL_Parser.yy"
    { COMP_PAR(LIT_SET, TP_SET_ALL_STR);
													  (yyval.exp).set(TP_SET_ALL_STR, 0); ;}
    break;

  case 234:
#line 1364 "src/cmpl/CMPL_Parser.yy"
    { COMP_PAR(LIT_SET, TP_SET_ALL);
													  (yyval.exp).set(TP_SET_ALL, 0); ;}
    break;

  case 235:
#line 1366 "src/cmpl/CMPL_Parser.yy"
    { EXPSTR n;
													  n.set(TPP_NULL, 0);
													  (yyval.exp) = PARSE_CONTEXT.parse_type_dev_lit_set(TP_SET_HALF_UB, &(yysemantic_stack_[(2) - (2)].exp), &n, &n);
													  COMP_PAR(LIT_SET, TP_SET_HALF_UB); ;}
    break;

  case 236:
#line 1370 "src/cmpl/CMPL_Parser.yy"
    { EXPSTR n;
													  n.set(TPP_NULL, 0);
													  (yyval.exp) = PARSE_CONTEXT.parse_type_dev_lit_set(TP_SET_HALF_LB, &(yysemantic_stack_[(2) - (1)].exp), &n, &n);
													  COMP_PAR(LIT_SET, TP_SET_HALF_LB); ;}
    break;

  case 237:
#line 1375 "src/cmpl/CMPL_Parser.yy"
    { EXPSTR t;
													  t.set(TP_INT, 0);
													  (yyval.exp) = PARSE_CONTEXT.parse_type_dev_lit_set(TP_SET_ALG, &(yysemantic_stack_[(3) - (1)].exp), &t, &(yysemantic_stack_[(3) - (3)].exp));
													  COMP_PAR(LIT_SET, TP_SET_ALG_S1); ;}
    break;

  case 238:
#line 1380 "src/cmpl/CMPL_Parser.yy"
    { (yyval.exp) = PARSE_CONTEXT.parse_type_dev_lit_set(TP_SET_ALG, &(yysemantic_stack_[(5) - (1)].exp), &(yysemantic_stack_[(5) - (3)].exp), &(yysemantic_stack_[(5) - (5)].exp));
													  COMP_PAR(LIT_SET, TP_SET_ALG); ;}
    break;

  case 239:
#line 1384 "src/cmpl/CMPL_Parser.yy"
    { /*COMP_PAR(LIT_VAL_SHORT, $3.dim_in);
													  COMP_SUB(SET_INDEXES, IND_GIVEN);*/
													  COMP(DEFSET_FUNC, 1);
													  COMP_INT((yysemantic_stack_[(4) - (3)].exp).symp()->symno * MAX_ANZ_IND_1 + (yysemantic_stack_[(4) - (3)].exp).dim_in);
													  (yyval.exp).set(TPK_SET, 0); ;}
    break;

  case 240:
#line 1394 "src/cmpl/CMPL_Parser.yy"
    { (yyval.exp) = (yysemantic_stack_[(1) - (1)].exp); ;}
    break;

  case 241:
#line 1399 "src/cmpl/CMPL_Parser.yy"
    { (yyval.i) = EQ; ;}
    break;

  case 242:
#line 1400 "src/cmpl/CMPL_Parser.yy"
    { (yyval.i) = GE; ;}
    break;

  case 243:
#line 1401 "src/cmpl/CMPL_Parser.yy"
    { (yyval.i) = LE; ;}
    break;

  case 244:
#line 1406 "src/cmpl/CMPL_Parser.yy"
    { (yyval.exp) = (yysemantic_stack_[(1) - (1)].exp);
													  (yyval.exp).tp = TPK_VALUE;
													  if (!((yysemantic_stack_[(1) - (1)].exp).symp()->isvalid()))
														PARSE_CONTEXT.init_symbol(&(yysemantic_stack_[(1) - (1)].exp), PTR_TP_LINENAME, TPK_VALUE, false); ;}
    break;

  case 245:
#line 1410 "src/cmpl/CMPL_Parser.yy"
    { (yyval.exp) = (yysemantic_stack_[(1) - (1)].exp);
													  (yyval.exp).tp = TPK_VALUE;
													  PARSE_CONTEXT.init_symbol(&(yysemantic_stack_[(1) - (1)].exp), PTR_TP_LINENAME, TPK_VALUE, false); ;}
    break;

  case 246:
#line 1415 "src/cmpl/CMPL_Parser.yy"
    { COMP(LINE_BEG, 1);
													  PARSE_CONTEXT.linename_repl(GLV->last_str, (char *)0);
													  COMP_STR(GLV->last_str); ;}
    break;

  case 247:
#line 1418 "src/cmpl/CMPL_Parser.yy"
    { COMP(LINE_BEG, 1);
													  PARSE_CONTEXT.linename_repl(GLV->last_str, (yysemantic_stack_[(2) - (1)].exp).symp()->s_name);
													  COMP_STR(GLV->last_str);
													  // TODO: auch resultierende Zeilennamen als Symbol einfuegen? (bzw. in linename_repl()?)
													;}
    break;

  case 248:
#line 1429 "src/cmpl/CMPL_Parser.yy"
    { /* Naechsten loop-Kontexte initialisieren, als Basis */
												  PARSE_CONTEXT.init_loop_comp(GLV->tmp_level, true, LOOP_EXEC, ((yysemantic_stack_[(2) - (1)].exp).tp==TPP_EMPTY ? (char *)0 : (yysemantic_stack_[(2) - (1)].exp).symp()->s_name)); ;}
    break;

  case 249:
#line 1434 "src/cmpl/CMPL_Parser.yy"
    { /* Naechsten loop-Kontexte initialisieren, als Basis fuer Summe */
												  PARSE_CONTEXT.init_loop_comp(GLV->tmp_level, true, LOOP_SUM, (char *)0); ;}
    break;

  case 250:
#line 1438 "src/cmpl/CMPL_Parser.yy"
    { /* Naechsten loop-Kontexte initialisieren, als Basis fuer Set */
												  PARSE_CONTEXT.init_loop_comp(GLV->tmp_level, true, LOOP_SET, (char *)0); ;}
    break;

  case 251:
#line 1442 "src/cmpl/CMPL_Parser.yy"
    { (yyval.exp).set(TPP_EMPTY, 0); ;}
    break;

  case 252:
#line 1443 "src/cmpl/CMPL_Parser.yy"
    { (yyval.exp) = (yysemantic_stack_[(1) - (1)].exp); ;}
    break;

  case 253:
#line 1447 "src/cmpl/CMPL_Parser.yy"
    { /* Setzt loop-Kontexte bis einschliesslich naechstem Basiskontext zurueck; compiliert entsprechende Beendigungen */
												  PARSE_CONTEXT.unset_loop_comp(); ;}
    break;

  case 254:
#line 1452 "src/cmpl/CMPL_Parser.yy"
    { /* Test fuer Alternative compilieren */
												  loop_ctrl_str *loopp = GLV->tmp_table + (GLV->tmp_level - 1);
												  COMP_SUB_PAR_CNT(TEST_ALTER, TEST_ALTER_IF, GLV->tmp_level - 1, 1);
												  loopp->last_jump = COMP_ADDR(0); ;}
    break;

  case 255:
#line 1456 "src/cmpl/CMPL_Parser.yy"
    { /* Beginn Default-Alternative compilieren */
												  loop_ctrl_str *loopp = GLV->tmp_table + (GLV->tmp_level - 1);
												  COMP_SUB_PAR(TEST_ALTER, TEST_ALTER_DEF, GLV->tmp_level - 1);
												  loopp->last_jump = 0; ;}
    break;

  case 256:
#line 1462 "src/cmpl/CMPL_Parser.yy"
    { loop_ctrl_str *loopp = GLV->tmp_table + (GLV->tmp_level - 1);
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
												;}
    break;

  case 260:
#line 1489 "src/cmpl/CMPL_Parser.yy"
    { /* Leeren loop-Kontext erzeugen */
												  loop_ctrl_str *loopp = PARSE_CONTEXT.set_loop_comp(START_EMPTY, 0);
												  loopp->loop_start = ADDR_COMP; ;}
    break;

  case 262:
#line 1493 "src/cmpl/CMPL_Parser.yy"
    { /* loop-Kontext muss jetzt reserviert werden, da in Zuweisung Summenschleifen vorkommen könnten */
												  loop_ctrl_str *loopp = PARSE_CONTEXT.set_loop_comp(START_ASSIGN, (yysemantic_stack_[(1) - (1)].i));
												  loopp->loop_start = ADDR_COMP; ;}
    break;

  case 264:
#line 1497 "src/cmpl/CMPL_Parser.yy"
    { /* loop-Kontext muss jetzt reserviert werden, da in loop_if Summenschleifen vorkommen könnten */
												  loop_ctrl_str *loopp = PARSE_CONTEXT.set_loop_comp(START_IF, (yysemantic_stack_[(1) - (1)].i));
												  loopp->loop_start = ADDR_COMP; ;}
    break;

  case 266:
#line 1503 "src/cmpl/CMPL_Parser.yy"
    { /* Start fuer loop compilieren */
												  loop_ctrl_str *loopp = GLV->tmp_table + (GLV->tmp_level - 1);
												  COMP_SUB_PAR_CNT(LOOP_CTRL, loopp->sub, GLV->tmp_level-1, 1);
												  COMP_INT(loopp->mod);
												  /* Iteration compilieren */
												  loopp->iter_start = COMP_SUB_PAR_CNT(TEST_ITER, 0, GLV->tmp_level-1, 3);
												  COMP_ADDR(loopp->loop_start);
												  loopp->end_jump = COMP_ADDR(0);
												  loopp->last_jump = COMP_ADDR(0); ;}
    break;

  case 267:
#line 1515 "src/cmpl/CMPL_Parser.yy"
    { (yyval.i) = (yysemantic_stack_[(2) - (1)].i) | (yysemantic_stack_[(2) - (2)].i); ;}
    break;

  case 268:
#line 1516 "src/cmpl/CMPL_Parser.yy"
    { (yyval.i) = 0; ;}
    break;

  case 269:
#line 1519 "src/cmpl/CMPL_Parser.yy"
    { (yyval.i) = HEAD_MOD_REF; ;}
    break;

  case 270:
#line 1520 "src/cmpl/CMPL_Parser.yy"
    { (yyval.i) = HEAD_MOD_WHILE; ;}
    break;

  case 271:
#line 1526 "src/cmpl/CMPL_Parser.yy"
    { /* Unterscheidung ob Iteration über Set oder lokale Definition */
												  //TODO: Typangabe erlauben
												  //TODO: Auch lokale Definition eines Sets erlauben
												  unsigned short sub = ((yysemantic_stack_[(3) - (2)].i) == 2 || TYPECLASS((yysemantic_stack_[(3) - (3)].exp).tp) == TPK_SET ? START_LOOP : START_ASSIGN);
												  /* Art und Name der Laufkonstante eintragen */
												  (yysemantic_stack_[(3) - (1)].exp).tp = (sub == START_LOOP ? TPK_VALUE : TYPECLASS((yysemantic_stack_[(3) - (3)].exp).tp));
												  PARSE_CONTEXT.set_loop_sname(sub, &(yysemantic_stack_[(3) - (1)].exp));
												  /* Wenn $2 == 0 (alte Zuweisung): Wenn $3 != TPK_SET (einfacher Wert), dann Fehler; sonst Warnung */
												  if ((yysemantic_stack_[(3) - (2)].i) == 0) PARSE_CONTEXT.out_warn_old_assign((TYPECLASS((yysemantic_stack_[(3) - (3)].exp).tp) != TPK_SET));
												;}
    break;

  case 272:
#line 1539 "src/cmpl/CMPL_Parser.yy"
    { (yyval.i) = 1; ;}
    break;

  case 273:
#line 1540 "src/cmpl/CMPL_Parser.yy"
    { (yyval.i) = 2; ;}
    break;

  case 274:
#line 1541 "src/cmpl/CMPL_Parser.yy"
    { (yyval.i) = 0; ;}
    break;

  case 275:
#line 1544 "src/cmpl/CMPL_Parser.yy"
    { (yyval.exp) = (yysemantic_stack_[(1) - (1)].exp); ;}
    break;

  case 276:
#line 1547 "src/cmpl/CMPL_Parser.yy"
    { (yyval.exp) = (yysemantic_stack_[(1) - (1)].exp); /*strcpy((char *) (GLV->c_getmem(sizeof(char) * strlen(GLV->last_str))), GLV->last_str);*/ ;}
    break;

  case 277:
#line 1554 "src/cmpl/CMPL_Parser.yy"
    { (yyval.exp) = (yysemantic_stack_[(1) - (1)].exp); ;}
    break;

  case 278:
#line 1558 "src/cmpl/CMPL_Parser.yy"
    { loop_ctrl_str *loopp = PARSE_CONTEXT.search_loop_name_mod((yysemantic_stack_[(2) - (2)].i_s).s, (yysemantic_stack_[(2) - (2)].i_s).i, "break", false);
												  if (loopp) { COMP_SUB_PAR_CNT(GOTO, GOTO_INDIREKT, 0, 1); COMP_ADDR(loopp->end_jump); } ;}
    break;

  case 279:
#line 1560 "src/cmpl/CMPL_Parser.yy"
    { loop_ctrl_str *loopp = PARSE_CONTEXT.search_loop_name_mod((yysemantic_stack_[(2) - (2)].i_s).s, (yysemantic_stack_[(2) - (2)].i_s).i, "continue", true);
												  if (loopp) { COMP_SUB_PAR_CNT(GOTO, GOTO_DIREKT, 0, 1); COMP_ADDR(loopp->iter_start); } ;}
    break;

  case 280:
#line 1562 "src/cmpl/CMPL_Parser.yy"
    { loop_ctrl_str *loopp = PARSE_CONTEXT.search_loop_name_mod((yysemantic_stack_[(2) - (2)].i_s).s, (yysemantic_stack_[(2) - (2)].i_s).i, "repeat", false);
												  if (loopp) { COMP_SUB_PAR_CNT(GOTO, GOTO_DIREKT, 0, 1); COMP_ADDR(loopp->loop_start); } ;}
    break;

  case 281:
#line 1566 "src/cmpl/CMPL_Parser.yy"
    { (yyval.i_s).s = (char *)0; (yyval.i_s).i = 0; ;}
    break;

  case 282:
#line 1567 "src/cmpl/CMPL_Parser.yy"
    { (yyval.i_s).s = (yysemantic_stack_[(1) - (1)].exp).symp()->s_name; (yyval.i_s).i = 0; ;}
    break;

  case 283:
#line 1568 "src/cmpl/CMPL_Parser.yy"
    { (yyval.i_s).s = (yysemantic_stack_[(1) - (1)].exp).symp()->s_name; /*GLV->last_str;*/ (yyval.i_s).i = 0; ;}
    break;

  case 284:
#line 1569 "src/cmpl/CMPL_Parser.yy"
    { (yyval.i_s).s = (char *)0; (yyval.i_s).i = (yysemantic_stack_[(1) - (1)].i); ;}
    break;

  case 285:
#line 1574 "src/cmpl/CMPL_Parser.yy"
    { COMP(STORE_TMP, 1);
												  //COMP_INT((-$1-2));
												  COMP_INT((yysemantic_stack_[(3) - (1)].exp).symp()->symno); ;}
    break;

  case 286:
#line 1586 "src/cmpl/CMPL_Parser.yy"
    { COMP_SUB(OUTPUT_MSG, ERROR_LVL_FATAL); ;}
    break;

  case 287:
#line 1587 "src/cmpl/CMPL_Parser.yy"
    { COMP_SUB(OUTPUT_MSG, USR_MSG_NL); ;}
    break;


    /* Line 675 of lalr1.cc.  */
#line 2035 "src/cmpl/_CMPL_Parser.cc"
	default: break;
      }
    YY_SYMBOL_PRINT ("-> $$ =", yyr1_[yyn], &yyval, &yyloc);

    yypop_ (yylen);
    yylen = 0;
    YY_STACK_PRINT ();

    yysemantic_stack_.push (yyval);
    yylocation_stack_.push (yyloc);

    /* Shift the result of the reduction.  */
    yyn = yyr1_[yyn];
    yystate = yypgoto_[yyn - yyntokens_] + yystate_stack_[0];
    if (0 <= yystate && yystate <= yylast_
	&& yycheck_[yystate] == yystate_stack_[0])
      yystate = yytable_[yystate];
    else
      yystate = yydefgoto_[yyn - yyntokens_];
    goto yynewstate;

  /*------------------------------------.
  | yyerrlab -- here on detecting error |
  `------------------------------------*/
  yyerrlab:
    /* If not already recovering from an error, report this error.  */
    if (!yyerrstatus_)
      {
	++yynerrs_;
	error (yylloc, yysyntax_error_ (yystate, yytoken));
      }

    yyerror_range[0] = yylloc;
    if (yyerrstatus_ == 3)
      {
	/* If just tried and failed to reuse look-ahead token after an
	 error, discard it.  */

	if (yychar <= yyeof_)
	  {
	  /* Return failure if at end of input.  */
	  if (yychar == yyeof_)
	    YYABORT;
	  }
	else
	  {
	    yydestruct_ ("Error: discarding", yytoken, &yylval, &yylloc);
	    yychar = yyempty_;
	  }
      }

    /* Else will try to reuse look-ahead token after shifting the error
       token.  */
    goto yyerrlab1;


  /*---------------------------------------------------.
  | yyerrorlab -- error raised explicitly by YYERROR.  |
  `---------------------------------------------------*/
  yyerrorlab:

    /* Pacify compilers like GCC when the user code never invokes
       YYERROR and the label yyerrorlab therefore never appears in user
       code.  */
    if (false)
      goto yyerrorlab;

    yyerror_range[0] = yylocation_stack_[yylen - 1];
    /* Do not reclaim the symbols of the rule which action triggered
       this YYERROR.  */
    yypop_ (yylen);
    yylen = 0;
    yystate = yystate_stack_[0];
    goto yyerrlab1;

  /*-------------------------------------------------------------.
  | yyerrlab1 -- common code for both syntax error and YYERROR.  |
  `-------------------------------------------------------------*/
  yyerrlab1:
    yyerrstatus_ = 3;	/* Each real token shifted decrements this.  */

    for (;;)
      {
	yyn = yypact_[yystate];
	if (yyn != yypact_ninf_)
	{
	  yyn += yyterror_;
	  if (0 <= yyn && yyn <= yylast_ && yycheck_[yyn] == yyterror_)
	    {
	      yyn = yytable_[yyn];
	      if (0 < yyn)
		break;
	    }
	}

	/* Pop the current state because it cannot handle the error token.  */
	if (yystate_stack_.height () == 1)
	YYABORT;

	yyerror_range[0] = yylocation_stack_[0];
	yydestruct_ ("Error: popping",
		     yystos_[yystate],
		     &yysemantic_stack_[0], &yylocation_stack_[0]);
	yypop_ ();
	yystate = yystate_stack_[0];
	YY_STACK_PRINT ();
      }

    if (yyn == yyfinal_)
      goto yyacceptlab;

    yyerror_range[1] = yylloc;
    // Using YYLLOC is tempting, but would change the location of
    // the look-ahead.  YYLOC is available though.
    YYLLOC_DEFAULT (yyloc, (yyerror_range - 1), 2);
    yysemantic_stack_.push (yylval);
    yylocation_stack_.push (yyloc);

    /* Shift the error token.  */
    YY_SYMBOL_PRINT ("Shifting", yystos_[yyn],
		   &yysemantic_stack_[0], &yylocation_stack_[0]);

    yystate = yyn;
    goto yynewstate;

    /* Accept.  */
  yyacceptlab:
    yyresult = 0;
    goto yyreturn;

    /* Abort.  */
  yyabortlab:
    yyresult = 1;
    goto yyreturn;

  yyreturn:
    if (yychar != yyeof_ && yychar != yyempty_)
      yydestruct_ ("Cleanup: discarding lookahead", yytoken, &yylval, &yylloc);

    /* Do not reclaim the symbols of the rule which action triggered
       this YYABORT or YYACCEPT.  */
    yypop_ (yylen);
    while (yystate_stack_.height () != 1)
      {
	yydestruct_ ("Cleanup: popping",
		   yystos_[yystate_stack_[0]],
		   &yysemantic_stack_[0],
		   &yylocation_stack_[0]);
	yypop_ ();
      }

    return yyresult;
  }

  // Generate an error message.
  std::string
  CMPL_Parser::yysyntax_error_ (int yystate, int tok)
  {
    std::string res;
    YYUSE (yystate);
#if YYERROR_VERBOSE
    int yyn = yypact_[yystate];
    if (yypact_ninf_ < yyn && yyn <= yylast_)
      {
	/* Start YYX at -YYN if negative to avoid negative indexes in
	   YYCHECK.  */
	int yyxbegin = yyn < 0 ? -yyn : 0;

	/* Stay within bounds of both yycheck and yytname.  */
	int yychecklim = yylast_ - yyn + 1;
	int yyxend = yychecklim < yyntokens_ ? yychecklim : yyntokens_;
	int count = 0;
	for (int x = yyxbegin; x < yyxend; ++x)
	  if (yycheck_[x + yyn] == x && x != yyterror_)
	    ++count;

	// FIXME: This method of building the message is not compatible
	// with internationalization.  It should work like yacc.c does it.
	// That is, first build a string that looks like this:
	// "syntax error, unexpected %s or %s or %s"
	// Then, invoke YY_ on this string.
	// Finally, use the string as a format to output
	// yytname_[tok], etc.
	// Until this gets fixed, this message appears in English only.
	res = "syntax error, unexpected ";
	res += yytnamerr_ (yytname_[tok]);
	if (count < 5)
	  {
	    count = 0;
	    for (int x = yyxbegin; x < yyxend; ++x)
	      if (yycheck_[x + yyn] == x && x != yyterror_)
		{
		  res += (!count++) ? ", expecting " : " or ";
		  res += yytnamerr_ (yytname_[x]);
		}
	  }
      }
    else
#endif
      res = YY_("syntax error");
    return res;
  }


  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
  const short int CMPL_Parser::yypact_ninf_ = -377;
  const short int
  CMPL_Parser::yypact_[] =
  {
       229,  -377,  -377,  -377,  -377,   102,   229,  -377,  -377,  -377,
    -377,    30,    55,    75,    78,  -377,  -377,   992,  1181,  1059,
    1126,    61,  1526,  1526,  1526,  1526,    15,    66,  -377,  -377,
     142,  -377,  -377,  -377,    96,   101,   113,   118,   118,   121,
     130,    81,   133,   135,   151,   153,   101,  -377,  -377,  -377,
    -377,    54,  -377,  1479,  1479,  1224,    20,    20,    20,  -377,
    -377,  1275,  1275,  -377,   478,   161,  -377,   158,  -377,   132,
    -377,   212,  -377,  -377,  -377,  -377,   236,   238,   262,  -377,
    -377,  -377,   247,  -377,  -377,  -377,  -377,    47,  -377,  -377,
     195,   202,   210,   214,   215,    66,  -377,   684,   219,   218,
    -377,  -377,  -377,    47,   222,   230,   231,   233,  -377,  -377,
     553,  -377,   241,  -377,    47,   248,   251,   252,   254,  -377,
     621,  -377,    58,    47,   255,   256,   257,  -377,  -377,  -377,
    -377,   166,  -377,  -377,  -377,  1479,  1479,  1275,  -377,  1479,
    -377,  -377,  -377,   128,   279,  -377,  1479,  1479,  1326,   128,
    -377,  -377,  -377,    90,  1211,  -377,  -377,  -377,  -377,  -377,
      66,   161,  -377,  -377,  -377,  -377,  -377,  -377,  -377,  -377,
    -377,  -377,   244,   240,   253,  -377,    98,   106,  1479,  -377,
     136,  1526,  1526,  1526,  1526,  1526,  1526,  1526,  1526,  1479,
     745,   260,   261,    -4,    47,    47,    47,  -377,  -377,  -377,
    -377,  -377,  -377,  -377,     8,   147,   931,  -377,  -377,  -377,
    -377,  -377,  -377,    58,   819,  -377,  -377,  -377,  -377,  -377,
     286,   286,   267,    71,   173,  -377,  -377,  -377,  -377,    47,
     887,  -377,  -377,  -377,   478,    90,    25,   264,    48,  1608,
    1608,  -377,  -377,   268,    66,    66,  -377,  -377,   269,   270,
      27,    66,   271,   272,   273,  1479,  1479,  1526,  1526,  1526,
    1526,  1526,  1526,  1526,  1526,  1526,   106,  -377,  1275,  -377,
    -377,  -377,  -377,  -377,   274,  -377,  -377,    66,   250,   250,
     325,   325,   325,   325,   325,   166,    57,   275,  -377,    47,
    -377,   328,  1479,  -377,  1479,  1479,   684,   284,   284,  -377,
     281,  -377,   275,   553,   300,   275,   621,    47,  -377,  -377,
    -377,  -377,  -377,  -377,  -377,  -377,   173,  -377,  -377,  1567,
    -377,    58,   275,  -377,  -377,  -377,  -377,    66,   212,  -377,
     337,   285,   290,  -377,   293,  -377,  -377,  -377,  -377,  -377,
    1479,  -377,  -377,  -377,  -377,   365,  -377,   166,   166,   166,
     166,   166,   166,   166,   166,   166,  -377,  -377,  1377,  -377,
    -377,  1526,  -377,   294,   745,  1428,  -377,  -377,  -377,  -377,
      83,    90,  -377,  -377,   275,  -377,   275,  -377,  1526,  -377,
    -377,  -377,   294,   931,  -377,   149,   306,   294,   819,  -377,
      58,    47,    58,  -377,  -377,    66,  -377,   364,  -377,   320,
     320,   275,   294,   887,   360,   136,  -377,  -377,  -377,  -377,
     307,  1377,   101,  -377,   313,   166,  -377,  -377,   275,   316,
     319,  -377,  -377,  -377,  -377,  1479,  -377,   294,  1479,   294,
    1479,   369,   166,  -377,   275,  -377,  -377,  -377,  -377,   275,
     275,    58,  -377,  -377,    71,  -377,  1567,  -377,  -377,  -377,
     294,    58,  -377,   275,    66,  1608,  -377,   321,  -377,  -377,
    -377,  -377,  -377,  -377,  -377,  -377,  -377,   275,  -377,   275,
    1526,  -377,  -377,   294,   275,  -377,  -377,   275,  -377,   371,
    -377,  -377,  -377,  -377,   322,  -377,   294,  -377,  -377,  -377
  };

  /* YYDEFACT[S] -- default rule to reduce with in state S when YYTABLE
     doesn't specify something else to do.  Zero means the default is an
     error.  */
  const unsigned short int
  CMPL_Parser::yydefact_[] =
  {
         2,     8,    40,    64,    82,     0,     2,     4,     5,     6,
       7,     0,     0,     0,     0,     1,     3,     0,     0,     0,
       0,     0,     0,     0,     0,   234,   173,   173,   244,   132,
       0,   133,   135,   134,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    29,    30,    31,
      32,     0,    33,     0,     0,     0,   281,   281,   281,   232,
     233,   226,   226,     9,     0,    27,    24,     0,    18,     0,
      23,   181,   127,   131,   128,   129,   184,     0,    25,   185,
     240,   182,   183,   130,   136,    26,   252,   268,   188,   190,
       0,     0,     0,     0,     0,   173,    41,     0,     0,    51,
      53,    47,    54,   268,     0,     0,     0,     0,   245,    65,
       0,    72,   252,    70,   268,     0,     0,     0,     0,    83,
       0,    89,   101,   268,     0,     0,     0,    12,   200,   199,
     186,   235,    60,   174,   219,     0,     0,   226,   150,     0,
     157,   140,   138,     0,     0,   249,     0,     0,     0,     0,
      39,   250,   231,   187,   177,   286,   178,   179,   180,   287,
     173,    27,   282,   283,   270,   269,   284,   278,   279,   280,
     228,   227,     0,   224,     0,    11,     0,     0,     0,    22,
       0,     0,     0,     0,     0,     0,     0,     0,   236,     0,
       0,     0,   258,   264,   268,   268,   268,   248,    19,    20,
      21,    44,   159,    43,     0,     0,     0,    48,    49,    50,
      68,    67,   247,   101,     0,    73,    74,    75,    86,    85,
     102,   103,     0,     0,   104,   115,   115,   100,   109,   268,
       0,    90,    91,    92,     0,   285,     0,     0,     0,     0,
       0,   168,   169,     0,   173,   173,   172,   170,     0,     0,
       0,   173,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   223,   226,   220,
      28,    34,    35,    14,     0,   155,   156,   173,   192,   193,
     194,   195,   198,   197,   196,   237,     0,    38,   257,   268,
     266,     0,     0,   267,     0,     0,     0,    58,    58,    57,
       0,    52,    63,     0,    76,    81,     0,   268,   111,    88,
     241,   243,   242,    93,   115,    97,   104,   107,    98,     0,
      99,   101,   126,    13,   148,   221,   149,   173,   164,   154,
       0,     0,   160,   153,     0,   142,   167,   171,   143,   147,
       0,   158,   146,   145,   239,   202,   201,   203,   206,   204,
     210,   208,   207,   209,   205,   211,    16,   225,     0,   137,
     151,     0,   256,     0,     0,     0,   259,   261,   276,   266,
       0,   277,   266,   212,   214,   215,   217,    45,   175,    55,
      56,    46,     0,     0,    69,     0,     0,     0,     0,    87,
     101,   268,   101,   106,   105,   173,   116,   117,   120,   122,
     122,   114,     0,     0,   165,     0,   141,   163,   161,   139,
       0,     0,     0,   229,     0,   238,   253,    36,    38,     0,
       0,   263,   273,   272,   274,     0,   265,     0,     0,     0,
       0,     0,   176,    61,    63,    78,    77,    71,    79,    81,
     114,   101,    96,    94,    95,   218,     0,   123,   119,   121,
       0,   101,   124,   126,   173,     0,   144,     0,   222,   230,
      15,    37,   255,   254,   275,   271,   189,   214,   191,   214,
     175,    62,    80,     0,   114,   118,   108,   114,   125,   166,
     162,    17,   213,   216,     0,   110,     0,   113,    59,   112
  };

  /* YYPGOTO[NTERM-NUM].  */
  const short int
  CMPL_Parser::yypgoto_[] =
  {
      -377,   393,  -377,  -377,  -377,   -39,  -377,  -377,  -377,  -377,
     347,  -377,   242,   237,   143,  -377,    -6,  -377,  -377,  -377,
     -61,  -377,  -377,   209,  -377,  -377,   117,    50,  -377,   -18,
    -377,  -377,   -91,  -377,  -377,  -377,  -377,  -377,   -22,  -377,
    -377,  -102,  -377,  -377,    29,  -377,  -377,  -190,  -377,  -377,
    -195,   103,  -377,  -377,  -184,  -377,  -376,  -206,  -377,   -24,
    -377,    24,  -377,   -30,  -180,  -377,  -377,  -377,  -377,  -377,
    -377,  -205,  -191,    22,   387,  -377,  -292,  -230,  -377,  -377,
    -377,  -105,  -377,  -377,   -23,   -42,   -78,   -17,    46,  -377,
    -377,  -377,  -377,  -377,  -377,  -377,     6,  -327,     0,  -377,
    -377,  -188,  -377,   -37,  -377,   -25,   -59,  -377,    21,    28,
    -377,     2,     4,    -7,   -72,  -377,  -377,    -9,    34,  -377,
     -77,   146,  -377,  -377,  -377,  -377,  -295,   243,    69,  -377,
    -377,  -377,  -377,    73,    -3,   148,    23,    62
  };

  /* YYDEFGOTO[NTERM-NUM].  */
  const short int
  CMPL_Parser::yydefgoto_[] =
  {
        -1,     5,     6,     7,    11,    63,   234,    64,   358,   411,
      65,    66,   177,    67,   273,    68,   363,    69,     8,    12,
      96,   296,    97,    98,    99,   300,   379,    70,   101,   382,
       9,    13,   109,   303,   110,   213,   386,   111,   387,    10,
      14,   119,   306,   120,   222,   392,   443,   223,   224,   225,
     226,   315,   316,   227,   228,   391,   450,   318,   319,   396,
     397,   448,   121,   402,    71,    72,    73,    74,    75,   240,
     239,    76,    77,   277,   141,   252,   102,   331,   455,   408,
     332,   243,   244,   248,   132,   431,   153,   154,   171,    79,
     195,    80,   196,    81,   156,   157,   374,   427,   376,   429,
     400,    82,    83,   138,   459,    84,   172,   173,   414,   158,
     313,    86,   113,    87,    88,    89,    90,   417,   451,   365,
     190,   191,   192,   290,   291,   292,   367,   193,   194,   369,
     425,   465,   370,   372,    91,   167,    92,    93
  };

  /* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule which
     number is the opposite.  If zero, do what YYDEFACT says.  */
  const short int CMPL_Parser::yytable_ninf_ = -263;
  const short int
  CMPL_Parser::yytable_[] =
  {
        78,   133,   133,   174,   134,   128,   129,   130,   131,   150,
     334,   103,   114,   123,   152,   104,   115,   124,   219,   211,
     320,   112,   112,   304,   122,   175,   206,   399,  -262,   314,
     255,   256,   255,   256,   329,   329,   203,   214,    78,   247,
     317,   105,   116,   125,   254,    85,   230,    78,   330,   330,
     229,   333,   333,   255,   256,   162,   163,   235,   236,   328,
     328,   238,   255,   256,   473,   297,   298,   299,   100,   250,
     133,  -260,   202,  -260,   421,   220,   221,   426,   237,   310,
     106,   117,   126,    85,   311,   312,   422,   423,   164,   165,
     103,   424,    85,    61,   104,   255,   256,  -245,   486,   155,
     159,   487,    15,   114,   340,    17,   324,   115,   393,    41,
     271,   286,   112,   123,   272,   164,   165,   124,   294,   295,
     105,   314,   112,   305,   122,   166,   166,   166,   322,   326,
      18,   401,   317,   116,   137,   133,   151,   127,   361,   398,
     482,   229,   483,   125,    61,   302,   135,   100,   307,   308,
      19,   287,   321,    20,   399,    47,    48,    49,    50,   106,
      52,   241,   242,   145,   278,   279,   280,   281,   282,   283,
     284,   285,   117,    78,   275,   276,   136,   345,   346,   160,
      95,   137,   126,   181,   182,   183,   184,   185,   186,   187,
     220,   221,   249,   139,   253,   323,   435,   436,   140,   103,
     440,   143,   444,   104,   389,   168,   169,   114,   179,   357,
     144,   115,   384,   146,   371,   147,   112,    78,    85,   133,
     133,   336,   337,   123,   274,   480,   133,   124,   341,   105,
     390,   148,   112,   149,   122,   377,   176,   116,   178,  -152,
     347,   348,   349,   350,   351,   352,   353,   354,   355,   229,
     329,   474,   133,   125,   360,   100,   100,     1,     2,     3,
       4,   477,    85,  -154,   330,   180,   398,   333,   106,   183,
     184,   185,   186,   187,  -153,   328,   117,   197,   198,   181,
     182,   183,   184,   185,   186,   187,   199,   371,   188,   103,
     200,   201,   126,   104,   204,   205,   114,   439,   207,   123,
     115,   453,   133,   124,   404,   112,   208,   209,   112,   210,
     122,   245,   241,   242,   441,   246,   212,   268,   229,   105,
     229,   364,   434,   267,   215,   418,   116,   216,   217,   125,
     218,   231,   232,   233,   269,   288,   383,    41,   289,   388,
     373,   375,   189,   309,   415,   325,   100,    78,   187,   335,
     338,   339,   342,   343,   344,   359,   403,   381,   106,   362,
     368,   432,   378,   385,   405,   117,   406,   407,   126,   229,
     133,   256,   445,   433,   409,   458,   103,   416,   438,   229,
     104,   114,   437,   446,   447,   115,   410,  -219,   456,   460,
     112,   462,    85,   452,   463,   470,   123,   481,  -151,    16,
     124,   488,   161,   266,   413,   112,   105,   122,   428,   356,
     430,   116,   461,   270,   301,   380,   471,   472,   466,   394,
     468,   442,   475,   478,   449,   142,   125,   454,   484,   133,
     469,   479,   457,   100,   467,   366,     0,   293,   420,     0,
       0,   476,     0,     0,     0,   106,     0,     0,     0,     0,
     117,     0,   364,   432,     0,     0,     0,   413,     0,     0,
       0,     0,     0,     0,   485,   126,     0,     0,   383,     0,
       0,   464,     0,   388,   373,     0,   375,   489,   -10,    21,
       0,     0,     0,     0,     0,     0,     0,   403,     0,     0,
       0,     0,     0,     0,     0,    22,    23,     0,     0,     0,
       0,   428,    24,   428,    25,     0,   -10,   -10,   -10,   -10,
      26,     0,    27,    28,    29,    30,     0,     0,    31,    32,
      33,    34,    35,     0,    36,    37,    38,    39,    40,    41,
      42,    43,    44,    45,    46,    47,    48,    49,    50,    51,
      52,     0,     0,    53,    54,     0,     0,    55,    56,    57,
      58,    59,    60,   -66,   107,     0,    61,     0,    62,     0,
    -251,   -10,   -10,     0,     0,     0,     0,     0,     0,     0,
    -246,  -246,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   -66,   -66,   -66,   -66,   108,  -246,  -246,    28,  -246,
      30,     0,     0,  -246,  -246,  -246,  -246,  -246,     0,  -246,
    -246,  -246,  -246,  -246,  -246,  -246,  -246,  -246,     0,     0,
    -246,  -246,  -246,  -246,     0,  -246,     0,     0,    53,    54,
       0,   -84,   118,    56,    57,    58,     0,     0,     0,     0,
       0,  -246,     0,  -246,     0,  -251,   -66,   -66,  -246,  -246,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   -84,
     -84,   -84,   -84,   108,  -246,  -246,    28,  -246,    30,     0,
       0,  -246,  -246,  -246,  -246,  -246,     0,  -246,  -246,  -246,
    -246,  -246,  -246,  -246,  -246,  -246,     0,     0,  -246,  -246,
    -246,  -246,     0,  -246,   -42,    94,    53,    54,     0,     0,
       0,    56,    57,    58,     0,     0,     0,     0,     0,  -246,
       0,  -246,     0,  -251,   -84,   -84,     0,     0,     0,     0,
       0,     0,   -42,   -42,   -42,   -42,    26,    95,     0,    28,
       0,    30,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    21,     0,     0,    53,
      54,     0,     0,     0,    56,    57,    58,     0,     0,     0,
       0,     0,    22,    23,     0,     0,  -251,   -42,   -42,    24,
       0,    25,     0,     0,     0,     0,     0,    26,     0,    27,
      28,    29,    30,     0,     0,    31,    32,    33,    34,    35,
       0,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,     0,     0,
      53,    54,     0,     0,    55,    56,    57,    58,    59,    60,
     107,     0,     0,    61,     0,    62,     0,  -251,   -10,   -10,
       0,     0,     0,     0,     0,     0,  -246,  -246,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   108,  -246,  -246,    28,  -246,    30,     0,     0,  -246,
    -246,  -246,  -246,  -246,     0,  -246,  -246,  -246,  -246,  -246,
    -246,  -246,  -246,  -246,     0,     0,  -246,  -246,  -246,  -246,
       0,  -246,     0,     0,    53,    54,     0,     0,   118,    56,
      57,    58,     0,     0,     0,     0,     0,  -246,     0,  -246,
       0,  -251,   -66,   -66,  -246,  -246,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   108,
    -246,  -246,    28,  -246,    30,     0,     0,  -246,  -246,  -246,
    -246,  -246,    94,  -246,  -246,  -246,  -246,  -246,  -246,  -246,
    -246,  -246,     0,     0,  -246,  -246,  -246,  -246,     0,  -246,
       0,     0,    53,    54,     0,     0,     0,    56,    57,    58,
       0,     0,     0,    26,    95,  -246,    28,  -246,    30,  -251,
     -84,   -84,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   -10,    21,     0,     0,    53,    54,     0,     0,
       0,    56,    57,    58,     0,     0,     0,     0,     0,    22,
      23,     0,     0,  -251,   -42,   -42,    24,     0,    25,     0,
     -10,   -10,   -10,   -10,    26,     0,    27,    28,    29,    30,
       0,     0,    31,    32,    33,    34,    35,     0,    36,    37,
      38,    39,    40,    41,    42,    43,    44,    45,    46,    47,
      48,    49,    50,    51,    52,     0,     0,    53,    54,   -66,
     107,    55,    56,    57,    58,    59,    60,     0,     0,     0,
      61,     0,    62,     0,  -251,     0,  -246,  -246,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   -66,   -66,   -66,
     -66,   108,  -246,  -246,    28,  -246,    30,     0,     0,  -246,
    -246,  -246,  -246,  -246,     0,  -246,  -246,  -246,  -246,  -246,
    -246,  -246,  -246,  -246,     0,     0,  -246,  -246,  -246,  -246,
       0,  -246,     0,     0,    53,    54,   -84,   118,     0,    56,
      57,    58,     0,     0,     0,     0,     0,  -246,     0,  -246,
       0,  -251,     0,  -246,  -246,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   -84,   -84,   -84,   -84,   108,  -246,
    -246,    28,  -246,    30,     0,     0,  -246,  -246,  -246,  -246,
    -246,     0,  -246,  -246,  -246,  -246,  -246,  -246,  -246,  -246,
    -246,   -42,    94,  -246,  -246,  -246,  -246,     0,  -246,     0,
       0,    53,    54,     0,     0,     0,    56,    57,    58,     0,
       0,     0,     0,     0,  -246,     0,  -246,     0,  -251,   -42,
     -42,   -42,   -42,    26,    95,     0,    28,     0,    30,   257,
     258,   259,   260,   261,   262,   263,   264,   265,   181,   182,
     183,   184,   185,   186,   187,     0,     0,   188,     0,     0,
       0,    22,    23,     0,     0,     0,    53,    54,    24,     0,
      25,    56,    57,    58,     0,     0,   160,     0,    27,     0,
      29,     0,     0,  -251,    31,    32,    33,    34,    35,     0,
      36,    37,    38,    39,    40,    41,    42,    43,    44,    45,
       0,    47,    48,    49,    50,    51,    52,     0,     0,     0,
       0,   189,    22,    23,     0,     0,     0,    59,    60,    24,
       0,    25,    61,     0,    62,     0,     0,   170,     0,    27,
       0,    29,     0,     0,     0,    31,    32,    33,    34,    35,
       0,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,     0,    47,    48,    49,    50,    51,    52,     0,     0,
       0,     0,     0,    22,    23,     0,     0,     0,    59,    60,
      24,     0,    25,    61,     0,    62,     0,     0,     0,   251,
      27,     0,    29,     0,     0,     0,    31,    32,    33,    34,
      35,     0,    36,    37,    38,    39,    40,    41,    42,    43,
      44,    45,     0,    47,    48,    49,    50,    51,    52,     0,
       0,     0,     0,     0,    22,    23,     0,     0,     0,    59,
      60,    24,     0,    25,    61,     0,    62,     0,     0,     0,
       0,    27,     0,    29,     0,     0,     0,    31,    32,    33,
      34,    35,   412,    36,    37,    38,    39,    40,    41,    42,
      43,    44,    45,     0,    47,    48,    49,    50,    51,    52,
       0,     0,     0,     0,     0,    22,    23,     0,     0,     0,
      59,    60,    24,     0,    25,    61,     0,    62,     0,     0,
       0,     0,    27,     0,    29,     0,     0,     0,    31,    32,
      33,    34,    35,     0,    36,    37,    38,    39,    40,    41,
      42,    43,    44,    45,     0,    47,    48,    49,    50,    51,
      52,     0,     0,     0,     0,   419,    22,    23,     0,     0,
       0,    59,    60,    24,     0,    25,    61,     0,    62,     0,
       0,     0,     0,    27,     0,    29,     0,     0,     0,    31,
      32,    33,    34,    35,     0,    36,    37,    38,    39,    40,
      41,    42,    43,    44,    45,     0,    47,    48,    49,    50,
      51,    52,     0,    22,    23,     0,     0,     0,     0,     0,
      24,     0,    59,    60,     0,     0,     0,    61,     0,    62,
      27,     0,    29,     0,     0,     0,    31,    32,    33,    34,
      35,     0,    36,    37,    38,    39,    40,    41,    42,    43,
      44,     0,     0,    47,    48,    49,    50,     0,    52,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      95,   395,     0,    29,    61,     0,    62,    31,    32,    33,
      34,    35,     0,    36,    37,    38,    39,    40,     0,    42,
      43,    44,     0,     0,    47,    48,    49,    50,     0,    52,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   327,     0,    29,    61,     0,    62,    31,    32,
      33,    34,    35,     0,    36,    37,    38,    39,    40,     0,
      42,    43,    44,     0,     0,    47,    48,    49,    50,     0,
      52,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    61,     0,    62
  };

  /* YYCHECK.  */
  const short int
  CMPL_Parser::yycheck_[] =
  {
        17,    26,    27,    62,    27,    22,    23,    24,    25,    46,
     240,    18,    19,    20,    51,    18,    19,    20,   120,   110,
     226,    19,    20,   213,    20,    64,   103,   319,    32,   224,
       5,     6,     5,     6,   239,   240,    97,   114,    55,   144,
     224,    18,    19,    20,   149,    17,   123,    64,   239,   240,
     122,   239,   240,     5,     6,    35,    36,   135,   136,   239,
     240,   139,     5,     6,   440,    57,    58,    59,    18,   147,
      95,    75,    95,    77,   369,    17,    18,   372,   137,     8,
      18,    19,    20,    55,    13,    14,     3,     4,    68,    69,
      97,     8,    64,    78,    97,     5,     6,    82,   474,    53,
      54,   477,     0,   110,    77,    75,    81,   110,   314,    51,
       4,   189,   110,   120,     8,    68,    69,   120,   195,   196,
      97,   316,   120,   214,   120,    56,    57,    58,   230,    81,
      75,   321,   316,   110,    80,   160,    82,    76,    81,   319,
     467,   213,   469,   120,    78,   206,     4,    97,   220,   221,
      75,   190,   229,    75,   446,    57,    58,    59,    60,    97,
      62,    33,    34,    82,   181,   182,   183,   184,   185,   186,
     187,   188,   110,   190,    38,    39,    80,   255,   256,    32,
      33,    80,   120,    17,    18,    19,    20,    21,    22,    23,
      17,    18,   146,    80,   148,   234,    47,    48,    80,   206,
     390,    80,   392,   206,   306,    57,    58,   214,    76,   268,
      80,   214,   303,    80,   292,    80,   214,   234,   190,   244,
     245,   244,   245,   230,   178,   455,   251,   230,   251,   206,
     307,    80,   230,    80,   230,   296,    75,   214,    80,    27,
     257,   258,   259,   260,   261,   262,   263,   264,   265,   321,
     455,   441,   277,   230,   277,   205,   206,    28,    29,    30,
      31,   451,   234,    27,   455,    27,   446,   455,   206,    19,
      20,    21,    22,    23,    27,   455,   214,    82,    76,    17,
      18,    19,    20,    21,    22,    23,    76,   365,    26,   296,
      76,    76,   230,   296,    75,    77,   303,   388,    76,   306,
     303,   403,   327,   306,   327,   303,    76,    76,   306,    76,
     306,    32,    33,    34,   391,    36,    75,    77,   390,   296,
     392,   287,   383,    79,    76,   364,   303,    76,    76,   306,
      76,    76,    76,    76,    81,    75,   302,    51,    77,   305,
     294,   295,    80,    76,   361,    81,   296,   364,    23,    81,
      81,    81,    81,    81,    81,    81,   322,    76,   296,    84,
      32,   378,    78,    63,    27,   303,    81,    77,   306,   441,
     395,     6,   395,   382,    81,   412,   383,    83,   387,   451,
     383,   388,    76,    19,    64,   388,   340,    27,    81,    76,
     388,    75,   364,   402,    75,    26,   403,    76,    27,     6,
     403,    79,    55,   161,   358,   403,   383,   403,   374,   266,
     376,   388,   418,   176,   205,   298,   434,   439,   427,   316,
     429,   392,   446,   453,   400,    38,   403,   405,   470,   454,
     430,   454,   411,   383,   428,   289,    -1,   194,   365,    -1,
      -1,   450,    -1,    -1,    -1,   383,    -1,    -1,    -1,    -1,
     388,    -1,   418,   470,    -1,    -1,    -1,   411,    -1,    -1,
      -1,    -1,    -1,    -1,   473,   403,    -1,    -1,   434,    -1,
      -1,   425,    -1,   439,   428,    -1,   430,   486,     0,     1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   453,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    17,    18,    -1,    -1,    -1,
      -1,   467,    24,   469,    26,    -1,    28,    29,    30,    31,
      32,    -1,    34,    35,    36,    37,    -1,    -1,    40,    41,
      42,    43,    44,    -1,    46,    47,    48,    49,    50,    51,
      52,    53,    54,    55,    56,    57,    58,    59,    60,    61,
      62,    -1,    -1,    65,    66,    -1,    -1,    69,    70,    71,
      72,    73,    74,     0,     1,    -1,    78,    -1,    80,    -1,
      82,    83,    84,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      17,    18,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    28,    29,    30,    31,    32,    33,    34,    35,    36,
      37,    -1,    -1,    40,    41,    42,    43,    44,    -1,    46,
      47,    48,    49,    50,    51,    52,    53,    54,    -1,    -1,
      57,    58,    59,    60,    -1,    62,    -1,    -1,    65,    66,
      -1,     0,     1,    70,    71,    72,    -1,    -1,    -1,    -1,
      -1,    78,    -1,    80,    -1,    82,    83,    84,    17,    18,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    28,
      29,    30,    31,    32,    33,    34,    35,    36,    37,    -1,
      -1,    40,    41,    42,    43,    44,    -1,    46,    47,    48,
      49,    50,    51,    52,    53,    54,    -1,    -1,    57,    58,
      59,    60,    -1,    62,     0,     1,    65,    66,    -1,    -1,
      -1,    70,    71,    72,    -1,    -1,    -1,    -1,    -1,    78,
      -1,    80,    -1,    82,    83,    84,    -1,    -1,    -1,    -1,
      -1,    -1,    28,    29,    30,    31,    32,    33,    -1,    35,
      -1,    37,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,     1,    -1,    -1,    65,
      66,    -1,    -1,    -1,    70,    71,    72,    -1,    -1,    -1,
      -1,    -1,    17,    18,    -1,    -1,    82,    83,    84,    24,
      -1,    26,    -1,    -1,    -1,    -1,    -1,    32,    -1,    34,
      35,    36,    37,    -1,    -1,    40,    41,    42,    43,    44,
      -1,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    -1,    -1,
      65,    66,    -1,    -1,    69,    70,    71,    72,    73,    74,
       1,    -1,    -1,    78,    -1,    80,    -1,    82,    83,    84,
      -1,    -1,    -1,    -1,    -1,    -1,    17,    18,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    32,    33,    34,    35,    36,    37,    -1,    -1,    40,
      41,    42,    43,    44,    -1,    46,    47,    48,    49,    50,
      51,    52,    53,    54,    -1,    -1,    57,    58,    59,    60,
      -1,    62,    -1,    -1,    65,    66,    -1,    -1,     1,    70,
      71,    72,    -1,    -1,    -1,    -1,    -1,    78,    -1,    80,
      -1,    82,    83,    84,    17,    18,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    32,
      33,    34,    35,    36,    37,    -1,    -1,    40,    41,    42,
      43,    44,     1,    46,    47,    48,    49,    50,    51,    52,
      53,    54,    -1,    -1,    57,    58,    59,    60,    -1,    62,
      -1,    -1,    65,    66,    -1,    -1,    -1,    70,    71,    72,
      -1,    -1,    -1,    32,    33,    78,    35,    80,    37,    82,
      83,    84,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,     0,     1,    -1,    -1,    65,    66,    -1,    -1,
      -1,    70,    71,    72,    -1,    -1,    -1,    -1,    -1,    17,
      18,    -1,    -1,    82,    83,    84,    24,    -1,    26,    -1,
      28,    29,    30,    31,    32,    -1,    34,    35,    36,    37,
      -1,    -1,    40,    41,    42,    43,    44,    -1,    46,    47,
      48,    49,    50,    51,    52,    53,    54,    55,    56,    57,
      58,    59,    60,    61,    62,    -1,    -1,    65,    66,     0,
       1,    69,    70,    71,    72,    73,    74,    -1,    -1,    -1,
      78,    -1,    80,    -1,    82,    -1,    17,    18,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    28,    29,    30,
      31,    32,    33,    34,    35,    36,    37,    -1,    -1,    40,
      41,    42,    43,    44,    -1,    46,    47,    48,    49,    50,
      51,    52,    53,    54,    -1,    -1,    57,    58,    59,    60,
      -1,    62,    -1,    -1,    65,    66,     0,     1,    -1,    70,
      71,    72,    -1,    -1,    -1,    -1,    -1,    78,    -1,    80,
      -1,    82,    -1,    17,    18,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    28,    29,    30,    31,    32,    33,
      34,    35,    36,    37,    -1,    -1,    40,    41,    42,    43,
      44,    -1,    46,    47,    48,    49,    50,    51,    52,    53,
      54,     0,     1,    57,    58,    59,    60,    -1,    62,    -1,
      -1,    65,    66,    -1,    -1,    -1,    70,    71,    72,    -1,
      -1,    -1,    -1,    -1,    78,    -1,    80,    -1,    82,    28,
      29,    30,    31,    32,    33,    -1,    35,    -1,    37,     8,
       9,    10,    11,    12,    13,    14,    15,    16,    17,    18,
      19,    20,    21,    22,    23,    -1,    -1,    26,    -1,    -1,
      -1,    17,    18,    -1,    -1,    -1,    65,    66,    24,    -1,
      26,    70,    71,    72,    -1,    -1,    32,    -1,    34,    -1,
      36,    -1,    -1,    82,    40,    41,    42,    43,    44,    -1,
      46,    47,    48,    49,    50,    51,    52,    53,    54,    55,
      -1,    57,    58,    59,    60,    61,    62,    -1,    -1,    -1,
      -1,    80,    17,    18,    -1,    -1,    -1,    73,    74,    24,
      -1,    26,    78,    -1,    80,    -1,    -1,    32,    -1,    34,
      -1,    36,    -1,    -1,    -1,    40,    41,    42,    43,    44,
      -1,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    -1,    57,    58,    59,    60,    61,    62,    -1,    -1,
      -1,    -1,    -1,    17,    18,    -1,    -1,    -1,    73,    74,
      24,    -1,    26,    78,    -1,    80,    -1,    -1,    -1,    33,
      34,    -1,    36,    -1,    -1,    -1,    40,    41,    42,    43,
      44,    -1,    46,    47,    48,    49,    50,    51,    52,    53,
      54,    55,    -1,    57,    58,    59,    60,    61,    62,    -1,
      -1,    -1,    -1,    -1,    17,    18,    -1,    -1,    -1,    73,
      74,    24,    -1,    26,    78,    -1,    80,    -1,    -1,    -1,
      -1,    34,    -1,    36,    -1,    -1,    -1,    40,    41,    42,
      43,    44,    45,    46,    47,    48,    49,    50,    51,    52,
      53,    54,    55,    -1,    57,    58,    59,    60,    61,    62,
      -1,    -1,    -1,    -1,    -1,    17,    18,    -1,    -1,    -1,
      73,    74,    24,    -1,    26,    78,    -1,    80,    -1,    -1,
      -1,    -1,    34,    -1,    36,    -1,    -1,    -1,    40,    41,
      42,    43,    44,    -1,    46,    47,    48,    49,    50,    51,
      52,    53,    54,    55,    -1,    57,    58,    59,    60,    61,
      62,    -1,    -1,    -1,    -1,    67,    17,    18,    -1,    -1,
      -1,    73,    74,    24,    -1,    26,    78,    -1,    80,    -1,
      -1,    -1,    -1,    34,    -1,    36,    -1,    -1,    -1,    40,
      41,    42,    43,    44,    -1,    46,    47,    48,    49,    50,
      51,    52,    53,    54,    55,    -1,    57,    58,    59,    60,
      61,    62,    -1,    17,    18,    -1,    -1,    -1,    -1,    -1,
      24,    -1,    73,    74,    -1,    -1,    -1,    78,    -1,    80,
      34,    -1,    36,    -1,    -1,    -1,    40,    41,    42,    43,
      44,    -1,    46,    47,    48,    49,    50,    51,    52,    53,
      54,    -1,    -1,    57,    58,    59,    60,    -1,    62,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      33,    34,    -1,    36,    78,    -1,    80,    40,    41,    42,
      43,    44,    -1,    46,    47,    48,    49,    50,    -1,    52,
      53,    54,    -1,    -1,    57,    58,    59,    60,    -1,    62,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    34,    -1,    36,    78,    -1,    80,    40,    41,
      42,    43,    44,    -1,    46,    47,    48,    49,    50,    -1,
      52,    53,    54,    -1,    -1,    57,    58,    59,    60,    -1,
      62,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    78,    -1,    80
  };

  /* STOS_[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
  const unsigned char
  CMPL_Parser::yystos_[] =
  {
         0,    28,    29,    30,    31,    86,    87,    88,   103,   115,
     124,    89,   104,   116,   125,     0,    86,    75,    75,    75,
      75,     1,    17,    18,    24,    26,    32,    34,    35,    36,
      37,    40,    41,    42,    43,    44,    46,    47,    48,    49,
      50,    51,    52,    53,    54,    55,    56,    57,    58,    59,
      60,    61,    62,    65,    66,    69,    70,    71,    72,    73,
      74,    78,    80,    90,    92,    95,    96,    98,   100,   102,
     112,   149,   150,   151,   152,   153,   156,   157,   172,   174,
     176,   178,   186,   187,   190,   194,   196,   198,   199,   200,
     201,   219,   221,   222,     1,    33,   105,   107,   108,   109,
     112,   113,   161,   198,   219,   221,   222,     1,    32,   117,
     119,   122,   196,   197,   198,   219,   221,   222,     1,   126,
     128,   147,   197,   198,   219,   221,   222,    76,   172,   172,
     172,   172,   169,   190,   169,     4,    80,    80,   188,    80,
      80,   159,   159,    80,    80,    82,    80,    80,    80,    80,
     188,    82,   188,   171,   172,   173,   179,   180,   194,   173,
      32,    95,    35,    36,    68,    69,   213,   220,   220,   220,
      32,   173,   191,   192,   191,    90,    75,    97,    80,    76,
      27,    17,    18,    19,    20,    21,    22,    23,    26,    80,
     205,   206,   207,   212,   213,   175,   177,    82,    76,    76,
      76,    76,   169,   105,    75,    77,   205,    76,    76,    76,
      76,   117,    75,   120,   205,    76,    76,    76,    76,   126,
      17,    18,   129,   132,   133,   134,   135,   138,   139,   199,
     205,    76,    76,    76,    91,   171,   171,   191,   171,   155,
     154,    33,    34,   166,   167,    32,    36,   166,   168,   173,
     171,    33,   160,   173,   166,     5,     6,     8,     9,    10,
      11,    12,    13,    14,    15,    16,    97,    79,    77,    81,
      98,     4,     8,    99,   173,    38,    39,   158,   172,   172,
     172,   172,   172,   172,   172,   172,   171,    90,    75,    77,
     208,   209,   210,   212,   205,   205,   106,    57,    58,    59,
     110,   108,   105,   118,   132,   117,   127,   199,   199,    76,
       8,    13,    14,   195,   135,   136,   137,   139,   142,   143,
     142,   205,   126,    90,    81,    81,    81,    34,   149,   156,
     157,   162,   165,   186,   162,    81,   169,   169,    81,    81,
      77,   169,    81,    81,    81,   171,   171,   172,   172,   172,
     172,   172,   172,   172,   172,   172,    99,   191,    93,    81,
     169,    81,    84,   101,   203,   204,   206,   211,    32,   214,
     217,   171,   218,   173,   181,   173,   183,   105,    78,   111,
     111,    76,   114,   203,   117,    63,   121,   123,   203,   126,
     205,   140,   130,   142,   136,    34,   144,   145,   149,   161,
     185,   132,   148,   203,   169,    27,    81,    77,   164,    81,
     173,    94,    45,   173,   193,   172,    83,   202,    90,    67,
     218,   211,     3,     4,     8,   215,   211,   182,   203,   184,
     203,   170,   172,   202,   105,    47,    48,    76,   202,   117,
     132,   205,   129,   131,   132,   169,    19,    64,   146,   146,
     141,   203,   202,   126,   158,   163,    81,   193,   188,   189,
      76,   101,    75,    75,   173,   216,   202,   181,   202,   183,
      26,   114,   123,   141,   132,   144,   202,   132,   148,   169,
     162,    76,   182,   182,   170,   202,   141,   141,    79,   202
  };

#if YYDEBUG
  /* TOKEN_NUMBER_[YYLEX-NUM] -- Internal symbol number corresponding
     to YYLEX-NUM.  */
  const unsigned short int
  CMPL_Parser::yytoken_number_[] =
  {
         0,   256,   257,   258,   259,   260,   261,   262,    61,   263,
     264,   265,   266,   267,   268,   269,   270,    43,    45,    42,
      47,   271,   272,    94,    33,   273,   274,   275,   276,   277,
     278,   279,   280,   281,   282,   283,   284,   285,   286,   287,
     288,   289,   290,   291,   292,   293,   294,   295,   296,   297,
     298,   299,   300,   301,   302,   303,   304,   305,   306,   307,
     308,   309,   310,   311,   312,   313,   314,   315,   316,   317,
     318,   319,   320,   321,   322,    58,    59,    44,    91,    93,
      40,    41,   123,   125,   124
  };
#endif

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
  const unsigned char
  CMPL_Parser::yyr1_[] =
  {
         0,    85,    86,    86,    87,    87,    87,    87,    89,    88,
      90,    90,    91,    90,    93,    92,    94,    92,    92,    92,
      92,    92,    92,    95,    95,    96,    96,    97,    97,    98,
      98,    98,    98,    98,    99,    99,   100,   101,   101,   102,
     104,   103,   105,   105,   106,   105,   107,   107,   107,   107,
     107,   108,   108,   109,   109,   110,   110,   110,   111,   111,
     112,   113,   114,   114,   116,   115,   117,   117,   118,   117,
     120,   119,   119,   119,   119,   119,   121,   121,   121,   122,
     123,   123,   125,   124,   126,   126,   127,   126,   128,   128,
     128,   128,   128,   130,   129,   131,   131,   132,   133,   133,
     133,   134,   135,   135,   136,   136,   137,   137,   138,   138,
     139,   140,   139,   141,   141,   143,   142,   144,   144,   145,
     145,   145,   146,   146,   147,   148,   148,   149,   149,   149,
     149,   149,   149,   150,   150,   150,   151,   152,   154,   153,
     155,   153,   153,   153,   153,   153,   153,   153,   153,   153,
     153,   156,   157,   157,   157,   158,   158,   159,   160,   161,
     162,   163,   162,   164,   165,   165,   165,   166,   167,   167,
     168,   168,   168,   169,   169,   170,   170,   171,   171,   171,
     171,   172,   172,   172,   172,   172,   172,   173,   175,   174,
     177,   176,   178,   178,   178,   178,   178,   178,   178,   178,
     178,   179,   179,   180,   180,   180,   180,   180,   180,   180,
     180,   180,   181,   182,   182,   183,   184,   184,   185,   186,
     187,   188,   189,   190,   191,   191,   192,   192,   192,   193,
     193,   194,   194,   194,   194,   194,   194,   194,   194,   194,
     194,   195,   195,   195,   196,   196,   197,   197,   198,   199,
     200,   201,   201,   202,   203,   203,   204,   205,   206,   206,
     208,   207,   209,   207,   210,   207,   211,   212,   212,   213,
     213,   214,   215,   215,   215,   216,   217,   218,   219,   219,
     219,   220,   220,   220,   220,   221,   222,   222
  };

  /* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
  const unsigned char
  CMPL_Parser::yyr2_[] =
  {
         0,     2,     0,     2,     1,     1,     1,     1,     0,     4,
       0,     2,     0,     4,     0,     6,     0,     7,     1,     2,
       2,     2,     2,     1,     1,     1,     1,     0,     2,     1,
       1,     1,     1,     1,     1,     1,     5,     3,     0,     2,
       0,     4,     0,     2,     0,     4,     4,     1,     2,     2,
       2,     1,     3,     1,     1,     2,     2,     1,     0,     5,
       2,     5,     3,     0,     0,     4,     0,     2,     0,     4,
       0,     5,     1,     2,     2,     2,     0,     2,     2,     5,
       3,     0,     0,     4,     0,     2,     0,     4,     3,     1,
       2,     2,     2,     0,     4,     1,     1,     2,     2,     2,
       1,     0,     1,     1,     0,     2,     2,     1,     5,     1,
       6,     0,     7,     3,     0,     0,     2,     1,     3,     2,
       1,     2,     0,     1,     5,     3,     0,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     4,     0,     5,
       0,     5,     4,     4,     6,     4,     4,     4,     4,     4,
       2,     4,     1,     1,     1,     1,     1,     1,     2,     2,
       1,     0,     4,     1,     1,     2,     4,     2,     1,     1,
       1,     2,     1,     0,     1,     0,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     2,     1,     0,     6,
       0,     6,     3,     3,     3,     3,     3,     3,     3,     2,
       2,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     1,     3,     0,     1,     3,     0,     2,     2,
       3,     3,     1,     3,     1,     3,     0,     1,     1,     1,
       2,     2,     1,     1,     1,     2,     2,     3,     5,     4,
       1,     1,     1,     1,     1,     1,     0,     2,     2,     2,
       2,     0,     1,     1,     3,     3,     1,     2,     1,     3,
       0,     3,     0,     4,     0,     4,     0,     2,     0,     1,
       1,     3,     1,     1,     1,     1,     1,     1,     2,     2,
       2,     0,     1,     1,     1,     3,     2,     2
  };

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
  /* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
     First, the terminals, then, starting at \a yyntokens_, nonterminals.  */
  const char*
  const CMPL_Parser::yytname_[] =
  {
    "\"end of file\"", "error", "$undefined", "ASSIGN_ITER", "ASSIGN",
  "LOG_OR", "LOG_AND", "VERG_SET", "'='", "VERG_NE2", "VERG_EQ2",
  "VERG_LT", "VERG_GT", "VERG_LE", "VERG_GE", "VERG_NE", "IN_SET", "'+'",
  "'-'", "'*'", "'/'", "MOD", "DIV", "'^'", "'!'", "VORZEICHEN", "POINTS",
  "SUBSYM_OP", "SECT_PARAM", "SECT_VAR", "SECT_OBJ", "SECT_CSTR",
  "SYMBOL_UNDEF", "SYMBOL_VAR", "SYMBOL_VALUE", "SYMBOL_LINENAME",
  "DEFS_TMP", "DEFS_TMP_ASSIGN", "SUBSYMBOL_UNDEF", "SUBSYMBOL_VALUE",
  "INT_NUMBER", "STRING", "DOUBLE_NUMBER", "MATHFUNC", "PROBLEM_FUNC",
  "READCSV", "READSTDIN", "FCT_MIN", "FCT_MAX", "DIM", "DEF", "SUM", "LEN",
  "FORMAT", "TYPE", "DEFSET", "PROBLEM_SOLVE", "TD_REAL", "TD_INT",
  "TD_BINARY", "TD_STRING", "TD_SET", "TD_PROBLEM", "OBJ_TO", "TRANSPOSE",
  "ERRMSG", "USRMSG", "DEFAULT", "WHILE", "REF", "BREAK", "CONTINUE",
  "REPEAT", "SET_ALL_NUM", "SET_ALL_STR", "':'", "';'", "','", "'['",
  "']'", "'('", "')'", "'{'", "'}'", "'|'", "$accept", "prog", "prog_sec",
  "const_sec", "@1", "const_defs", "@2", "const_def", "@3", "@4",
  "ul_sym_vekt", "lvalue_expr", "const_type_def", "const_type",
  "const_set_op", "const_loop", "const_alts", "problem_solve", "var_sec",
  "@5", "var_defs", "@6", "var_def", "d_var_vekts", "d_var_vekt",
  "var_type", "var_defber", "d_undef_sym_vekt", "var_loop", "var_alts",
  "obj_sec", "@7", "obj_defs", "@8", "obj_def", "@9", "obj_def_to",
  "obj_loop", "obj_alts", "cstr_sec", "@10", "cstr_defs", "@11",
  "cstr_def", "cstr_line", "@12", "o_cstr_line", "sum_line", "f_term",
  "leer", "term_sgn", "o_terms", "o_term", "f_sum_term", "sum_term", "@13",
  "sum_line_alts", "term", "@14", "tterm", "t_expr", "transp", "cstr_loop",
  "cstr_alts", "all_expr", "literal_expr", "tupel_expr", "convert_expr",
  "function_expr", "@15", "@16", "subsym_expr", "subsym_pre", "subsym_sub",
  "start_list", "def_var_vekt", "d_def_var_vekt", "const_list", "@17",
  "sep_list", "const_elem", "bel_sym_vekt", "bel_sym_vekt_sym",
  "func_def_par", "d_vekt", "o_c_expr", "const_expr", "const_expr_no_log",
  "const_set_expr", "const_loop_sum", "@18", "const_loop_set", "@19",
  "num_op_expr", "log_op_expr", "cmp_op_expr", "add_const_expr",
  "add_const_expr_alts", "count_const_expr", "count_const_expr_alts",
  "d_def_const_vekt", "def_const_vekt", "expr_val_list", "func_val_list",
  "func_ssv_val_list", "index_val_list", "val_list", "val_list_elem",
  "vekt_def", "set_expr", "verg_op", "line_name_sym", "line_name",
  "loop_start", "loop_sum_start", "loop_set_start", "loop_name",
  "loop_stop", "loop_alt", "loop_alt_start", "loop_header", "loop_heads",
  "loop_head", "@20", "@21", "@22", "loop_comp_start", "loop_mods",
  "loop_mod", "loop_for", "loop_for_assign", "loop_for_set",
  "loop_for_var", "loop_if", "loop_ctrl", "loop_ctrl_ref", "local_assign",
  "usr_err_msg", 0
  };
#endif

#if YYDEBUG
  /* YYRHS -- A `-1'-separated list of the rules' RHS.  */
  const CMPL_Parser::rhs_number_type
  CMPL_Parser::yyrhs_[] =
  {
        86,     0,    -1,    -1,    87,    86,    -1,    88,    -1,   103,
      -1,   115,    -1,   124,    -1,    -1,    28,    89,    75,    90,
      -1,    -1,    92,    90,    -1,    -1,     1,    76,    91,    90,
      -1,    -1,    95,    97,    99,    93,   193,    76,    -1,    -1,
      69,    95,    97,    99,    94,   193,    76,    -1,   100,    -1,
     219,    76,    -1,   221,    76,    -1,   222,    76,    -1,   102,
      76,    -1,   112,    -1,    96,    -1,   172,    -1,   194,    -1,
      -1,    75,    98,    -1,    57,    -1,    58,    -1,    59,    -1,
      60,    -1,    62,    -1,     4,    -1,     8,    -1,   198,   205,
      90,   101,   202,    -1,   203,    90,   101,    -1,    -1,    56,
     188,    -1,    -1,    29,   104,    75,   105,    -1,    -1,   107,
     105,    -1,    -1,     1,    76,   106,   105,    -1,   108,    75,
     110,    76,    -1,   113,    -1,   219,    76,    -1,   221,    76,
      -1,   222,    76,    -1,   109,    -1,   109,    77,   108,    -1,
     112,    -1,   161,    -1,    57,   111,    -1,    58,   111,    -1,
      59,    -1,    -1,    78,   170,    26,   170,    79,    -1,    32,
     169,    -1,   198,   205,   105,   114,   202,    -1,   203,   105,
     114,    -1,    -1,    -1,    30,   116,    75,   117,    -1,    -1,
     119,   117,    -1,    -1,     1,    76,   118,   117,    -1,    -1,
     197,   120,   132,   121,    76,    -1,   122,    -1,   219,    76,
      -1,   221,    76,    -1,   222,    76,    -1,    -1,    63,    48,
      -1,    63,    47,    -1,   198,   205,   117,   123,   202,    -1,
     203,   117,   123,    -1,    -1,    -1,    31,   125,    75,   126,
      -1,    -1,   128,   126,    -1,    -1,     1,    76,   127,   126,
      -1,   197,   129,    76,    -1,   147,    -1,   219,    76,    -1,
     221,    76,    -1,   222,    76,    -1,    -1,   132,   195,   130,
     131,    -1,   132,    -1,   129,    -1,   133,   136,    -1,   134,
     142,    -1,   135,   142,    -1,   138,    -1,    -1,    17,    -1,
      18,    -1,    -1,   137,   136,    -1,   135,   142,    -1,   139,
      -1,   199,   205,   132,   141,   202,    -1,   139,    -1,    17,
     199,   205,   132,   141,   202,    -1,    -1,    18,   199,   140,
     205,   132,   141,   202,    -1,   203,   132,   141,    -1,    -1,
      -1,   143,   144,    -1,   145,    -1,   145,    19,   144,    -1,
     161,   146,    -1,   149,    -1,   185,   146,    -1,    -1,    64,
      -1,   198,   205,   126,   148,   202,    -1,   203,   126,   148,
      -1,    -1,   150,    -1,   152,    -1,   153,    -1,   187,    -1,
     151,    -1,    36,    -1,    40,    -1,    42,    -1,    41,    -1,
     190,    -1,    98,    80,   173,    81,    -1,    -1,    48,   159,
     154,   162,    81,    -1,    -1,    47,   159,   155,   162,    81,
      -1,    49,    80,   166,    81,    -1,    50,    80,   168,    81,
      -1,    53,    80,   171,    77,   173,    81,    -1,    54,    80,
     173,    81,    -1,    54,    80,   160,    81,    -1,    52,    80,
     173,    81,    -1,    43,    80,   171,    81,    -1,    46,    80,
     171,    81,    -1,    44,   188,    -1,   157,    27,   158,   169,
      -1,   149,    -1,   186,    -1,   156,    -1,    38,    -1,    39,
      -1,    80,    -1,    33,   169,    -1,    33,   169,    -1,   165,
      -1,    -1,   165,   164,   163,   162,    -1,    77,    -1,   149,
      -1,    34,   169,    -1,   157,    27,   158,   169,    -1,   167,
     169,    -1,    33,    -1,    34,    -1,   166,    -1,    32,   169,
      -1,    36,    -1,    -1,   190,    -1,    -1,   172,    -1,   172,
      -1,   179,    -1,   180,    -1,   194,    -1,   149,    -1,   178,
      -1,   186,    -1,   156,    -1,   174,    -1,    24,   172,    -1,
     171,    -1,    -1,   199,   175,   205,   181,   182,   202,    -1,
      -1,   200,   177,   205,   183,   184,   202,    -1,   172,    17,
     172,    -1,   172,    18,   172,    -1,   172,    19,   172,    -1,
     172,    20,   172,    -1,   172,    23,   172,    -1,   172,    22,
     172,    -1,   172,    21,   172,    -1,    18,   172,    -1,    17,
     172,    -1,   171,     6,   171,    -1,   171,     5,   171,    -1,
     172,     8,   172,    -1,   172,    10,   172,    -1,   172,    15,
     172,    -1,   172,     9,   172,    -1,   172,    13,   172,    -1,
     172,    12,   172,    -1,   172,    14,   172,    -1,   172,    11,
     172,    -1,   172,    16,   172,    -1,   173,    -1,   203,   181,
     182,    -1,    -1,   173,    -1,   203,   183,   182,    -1,    -1,
      34,   169,    -1,    34,   169,    -1,    80,   191,    81,    -1,
      80,   191,    81,    -1,   188,    -1,    78,   191,    79,    -1,
     192,    -1,   192,    77,   191,    -1,    -1,   173,    -1,    32,
      -1,   173,    -1,    45,   189,    -1,    61,   188,    -1,    73,
      -1,    74,    -1,    26,    -1,    26,   172,    -1,   172,    26,
      -1,   172,    26,   172,    -1,   172,    80,   171,    81,   172,
      -1,    55,    80,   166,    81,    -1,   176,    -1,     8,    -1,
      14,    -1,    13,    -1,    35,    -1,    32,    -1,    -1,   196,
      75,    -1,   201,    82,    -1,    51,    82,    -1,    61,    82,
      -1,    -1,   196,    -1,    83,    -1,   204,   218,    75,    -1,
     204,    67,    75,    -1,    84,    -1,   206,    75,    -1,   207,
      -1,   207,    77,   206,    -1,    -1,   212,   208,   211,    -1,
      -1,   212,   209,   214,   211,    -1,    -1,   212,   210,   218,
     211,    -1,    -1,   213,   212,    -1,    -1,    69,    -1,    68,
      -1,   217,   215,   216,    -1,     4,    -1,     3,    -1,     8,
      -1,   173,    -1,    32,    -1,   171,    -1,    70,   220,    -1,
      71,   220,    -1,    72,   220,    -1,    -1,    35,    -1,    36,
      -1,   213,    -1,    37,     4,   171,    -1,    65,   173,    -1,
      66,   173,    -1
  };

  /* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
     YYRHS.  */
  const unsigned short int
  CMPL_Parser::yyprhs_[] =
  {
         0,     0,     3,     4,     7,     9,    11,    13,    15,    16,
      21,    22,    25,    26,    31,    32,    39,    40,    48,    50,
      53,    56,    59,    62,    64,    66,    68,    70,    71,    74,
      76,    78,    80,    82,    84,    86,    88,    94,    98,    99,
     102,   103,   108,   109,   112,   113,   118,   123,   125,   128,
     131,   134,   136,   140,   142,   144,   147,   150,   152,   153,
     159,   162,   168,   172,   173,   174,   179,   180,   183,   184,
     189,   190,   196,   198,   201,   204,   207,   208,   211,   214,
     220,   224,   225,   226,   231,   232,   235,   236,   241,   245,
     247,   250,   253,   256,   257,   262,   264,   266,   269,   272,
     275,   277,   278,   280,   282,   283,   286,   289,   291,   297,
     299,   306,   307,   315,   319,   320,   321,   324,   326,   330,
     333,   335,   338,   339,   341,   347,   351,   352,   354,   356,
     358,   360,   362,   364,   366,   368,   370,   372,   377,   378,
     384,   385,   391,   396,   401,   408,   413,   418,   423,   428,
     433,   436,   441,   443,   445,   447,   449,   451,   453,   456,
     459,   461,   462,   467,   469,   471,   474,   479,   482,   484,
     486,   488,   491,   493,   494,   496,   497,   499,   501,   503,
     505,   507,   509,   511,   513,   515,   517,   520,   522,   523,
     530,   531,   538,   542,   546,   550,   554,   558,   562,   566,
     569,   572,   576,   580,   584,   588,   592,   596,   600,   604,
     608,   612,   616,   618,   622,   623,   625,   629,   630,   633,
     636,   640,   644,   646,   650,   652,   656,   657,   659,   661,
     663,   666,   669,   671,   673,   675,   678,   681,   685,   691,
     696,   698,   700,   702,   704,   706,   708,   709,   712,   715,
     718,   721,   722,   724,   726,   730,   734,   736,   739,   741,
     745,   746,   750,   751,   756,   757,   762,   763,   766,   767,
     769,   771,   775,   777,   779,   781,   783,   785,   787,   790,
     793,   796,   797,   799,   801,   803,   807,   810
  };

  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
  const unsigned short int
  CMPL_Parser::yyrline_[] =
  {
         0,   193,   193,   194,   199,   200,   201,   202,   209,   209,
     213,   214,   215,   215,   221,   220,   337,   336,   404,   405,
     406,   408,   409,   412,   415,   431,   432,   436,   437,   441,
     442,   443,   444,   445,   449,   450,   454,   457,   458,   461,
     471,   471,   475,   476,   477,   477,   482,   483,   484,   485,
     487,   490,   491,   494,   499,   504,   505,   506,   510,   511,
     517,   533,   536,   537,   544,   544,   548,   549,   550,   550,
     555,   555,   558,   559,   560,   562,   566,   567,   568,   572,
     575,   576,   583,   583,   587,   588,   589,   589,   594,   595,
     596,   597,   599,   603,   603,   614,   615,   619,   622,   623,
     624,   627,   630,   631,   634,   635,   638,   639,   644,   645,
     648,   649,   649,   653,   654,   659,   659,   665,   667,   671,
     674,   675,   680,   681,   685,   688,   689,   697,   698,   699,
     701,   702,   703,   712,   714,   717,   722,   730,   734,   734,
     738,   738,   742,   747,   748,   751,   753,   756,   758,   760,
     762,   770,   788,   789,   790,   793,   794,   798,   802,   813,
     827,   828,   828,   833,   837,   838,   854,   880,   901,   902,
     908,   912,   916,   922,   925,   962,   963,   968,   969,   970,
     971,   978,   979,   980,   981,   982,   983,   988,   993,   993,
    1001,  1001,  1013,  1015,  1017,  1019,  1021,  1023,  1025,  1027,
    1031,  1047,  1049,  1054,  1056,  1058,  1060,  1062,  1064,  1069,
    1071,  1076,  1103,  1107,  1109,  1112,  1116,  1118,  1137,  1163,
    1183,  1212,  1232,  1240,  1250,  1251,  1260,  1264,  1271,  1281,
    1282,  1321,  1360,  1362,  1364,  1366,  1370,  1374,  1379,  1384,
    1394,  1399,  1400,  1401,  1406,  1410,  1415,  1418,  1429,  1434,
    1438,  1442,  1443,  1447,  1452,  1456,  1462,  1482,  1485,  1486,
    1489,  1489,  1493,  1493,  1497,  1497,  1503,  1515,  1516,  1519,
    1520,  1525,  1539,  1540,  1541,  1544,  1547,  1554,  1558,  1560,
    1562,  1566,  1567,  1568,  1569,  1573,  1586,  1587
  };

  // Print the state stack on the debug stream.
  void
  CMPL_Parser::yystack_print_ ()
  {
    *yycdebug_ << "Stack now";
    for (state_stack_type::const_iterator i = yystate_stack_.begin ();
	 i != yystate_stack_.end (); ++i)
      *yycdebug_ << ' ' << *i;
    *yycdebug_ << std::endl;
  }

  // Report on the debug stream that the rule \a yyrule is going to be reduced.
  void
  CMPL_Parser::yy_reduce_print_ (int yyrule)
  {
    unsigned int yylno = yyrline_[yyrule];
    int yynrhs = yyr2_[yyrule];
    /* Print the symbols being reduced, and their result.  */
    *yycdebug_ << "Reducing stack by rule " << yyrule - 1
	       << " (line " << yylno << "), ";
    /* The symbols being reduced.  */
    for (int yyi = 0; yyi < yynrhs; yyi++)
      YY_SYMBOL_PRINT ("   $" << yyi + 1 << " =",
		       yyrhs_[yyprhs_[yyrule] + yyi],
		       &(yysemantic_stack_[(yynrhs) - (yyi + 1)]),
		       &(yylocation_stack_[(yynrhs) - (yyi + 1)]));
  }
#endif // YYDEBUG

  /* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
  CMPL_Parser::token_number_type
  CMPL_Parser::yytranslate_ (int t)
  {
    static
    const token_number_type
    translate_table[] =
    {
           0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    24,     2,     2,     2,     2,     2,     2,
      80,    81,    19,    17,    77,    18,     2,    20,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    75,    76,
       2,     8,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    78,     2,    79,    23,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    82,    84,    83,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     9,    10,    11,    12,    13,    14,    15,
      16,    21,    22,    25,    26,    27,    28,    29,    30,    31,
      32,    33,    34,    35,    36,    37,    38,    39,    40,    41,
      42,    43,    44,    45,    46,    47,    48,    49,    50,    51,
      52,    53,    54,    55,    56,    57,    58,    59,    60,    61,
      62,    63,    64,    65,    66,    67,    68,    69,    70,    71,
      72,    73,    74
    };
    if ((unsigned int) t <= yyuser_token_number_max_)
      return translate_table[t];
    else
      return yyundef_token_;
  }

  const int CMPL_Parser::yyeof_ = 0;
  const int CMPL_Parser::yylast_ = 1688;
  const int CMPL_Parser::yynnts_ = 138;
  const int CMPL_Parser::yyempty_ = -2;
  const int CMPL_Parser::yyfinal_ = 15;
  const int CMPL_Parser::yyterror_ = 1;
  const int CMPL_Parser::yyerrcode_ = 256;
  const int CMPL_Parser::yyntokens_ = 85;

  const unsigned int CMPL_Parser::yyuser_token_number_max_ = 322;
  const CMPL_Parser::token_number_type CMPL_Parser::yyundef_token_ = 2;

} // namespace cmpl

#line 1591 "src/cmpl/CMPL_Parser.yy"


void cmpl::CMPL_Parser::error(const cmpl::CMPL_Parser::location_type& l, const std::string& m)
{
	PARSE_CONTEXT.output_error_parse(ERROR_TYPE_COMP, ERROR_LVL_NORMAL, m.c_str());
}


