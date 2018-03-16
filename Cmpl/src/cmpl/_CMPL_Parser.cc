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
#line 120 "src/cmpl/CMPL_Parser.yy"

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
#line 196 "src/cmpl/CMPL_Parser.yy"
    { COMP(PROG_END, 0); ;}
    break;

  case 8:
#line 212 "src/cmpl/CMPL_Parser.yy"
    { GLV->sect = (int) token::SECT_PARAM; ;}
    break;

  case 12:
#line 218 "src/cmpl/CMPL_Parser.yy"
    { yyerrok; ;}
    break;

  case 14:
#line 224 "src/cmpl/CMPL_Parser.yy"
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
														  PARSE_CONTEXT.output_error_parse(ERROR_TYPE_COMP, ERROR_LVL_FATAL, "not a lvalue");	/*TODO*/

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
													  int *symblock = PARSE_CONTEXT.tupel_assign_symbols(&tupel_cnt, 0);

													  /* Zur Übergabe weiter nach hinten in dieser Klausel in $1 speichern, dafuer andere Felder missbrauchen */
													  (yysemantic_stack_[(3) - (1)].exp).symtab = (SymbolTab *) symblock;
													  (yysemantic_stack_[(3) - (1)].exp).dim_t_rank2 = tupel_cnt;
												  }
												;}
    break;

  case 15:
#line 273 "src/cmpl/CMPL_Parser.yy"
    { /* Unterscheidung ob linke Seite normales Symbol oder Tupel */
				 								  if ((yysemantic_stack_[(6) - (1)].exp).dim_t_rank1 == 0)
												  {
													  /* Dimensionen muessen gleich sein, wenn feststehend, sonst muss auf einer Seite die Dimension feststehen */
													  if ((yysemantic_stack_[(6) - (1)].exp).dim_out >= 0 && (yysemantic_stack_[(6) - (5)].exp).dim_out >= 0 && (yysemantic_stack_[(6) - (1)].exp).dim_out != (yysemantic_stack_[(6) - (5)].exp).dim_out && (yysemantic_stack_[(6) - (5)].exp).dim_out != 1)
														PARSE_CONTEXT.output_error_parse(ERROR_TYPE_COMP, ERROR_LVL_NORMAL, "array dimensions don't match");/*TODO*/
													  else if ((yysemantic_stack_[(6) - (1)].exp).dim_out < 0 && (yysemantic_stack_[(6) - (5)].exp).dim_out < 0)
														PARSE_CONTEXT.output_error_parse(ERROR_TYPE_COMP, ERROR_LVL_NORMAL, "array dimensions not known");/*TODO*/
													  else if ((yysemantic_stack_[(6) - (5)].exp).dim_out < 0 && (yysemantic_stack_[(6) - (5)].exp).addr_dim)
														COMP_INT_AT((yysemantic_stack_[(6) - (5)].exp).addr_dim, (yysemantic_stack_[(6) - (1)].exp).dim_out);	/* Dimension fuer $5 nachtragen */
													  else if ((yysemantic_stack_[(6) - (1)].exp).dim_out < 0 && (yysemantic_stack_[(6) - (1)].exp).addr_dim)
													  {
														COMP_INT_AT((yysemantic_stack_[(6) - (1)].exp).addr_dim, (yysemantic_stack_[(6) - (5)].exp).dim_out);	/* Dimension fuer $1 nachtragen */
														(yysemantic_stack_[(6) - (1)].exp).dim_in = (yysemantic_stack_[(6) - (1)].exp).sym_dim = (yysemantic_stack_[(6) - (5)].exp).dim_out;
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
														PARSE_CONTEXT.init_symbol(&(yysemantic_stack_[(6) - (1)].exp), PTR_TP_CONST, (yysemantic_stack_[(6) - (1)].exp).tp, (yysemantic_stack_[(6) - (1)].exp).sym_dim, false);
													  else
														PARSE_CONTEXT.test_tpk_symbol(symp, (yysemantic_stack_[(6) - (1)].exp).tp, true);

													  if ((yysemantic_stack_[(6) - (1)].exp).sub_sym_ind < 0)
													  {
														COMP_SUB_PAR_CNT(DEF_CONST, SYM_MAIN + 256*((yysemantic_stack_[(6) - (5)].exp).dim_out>=0 ? (yysemantic_stack_[(6) - (5)].exp).dim_out : (yysemantic_stack_[(6) - (1)].exp).dim_out), (yysemantic_stack_[(6) - (2)].t), 1);
														COMP_INT(symp->symno * MAX_ANZ_IND_1 + (yysemantic_stack_[(6) - (1)].exp).sym_dim);
													  }
													  else
													  {
														COMP_SUB_PAR_CNT(DEF_CONST, SYM_SUB + 256*((yysemantic_stack_[(6) - (5)].exp).dim_out>=0 ? (yysemantic_stack_[(6) - (5)].exp).dim_out : (yysemantic_stack_[(6) - (1)].exp).dim_out), (yysemantic_stack_[(6) - (2)].t), 1);
														COMP_INT((yysemantic_stack_[(6) - (1)].exp).sub_sym_ind);
													  }
												  }
												  else
												  {
												      /* Werte aus $1 holen */
													  int *symblock = (int *) ((yysemantic_stack_[(6) - (1)].exp).symtab);
													  int tupel_cnt = (yysemantic_stack_[(6) - (1)].exp).dim_t_rank2;

													  /* Auf der rechten Seite muss einfaches Tupel stehen (kein Array), mit passendem Tupelrang */
													  if ((yysemantic_stack_[(6) - (5)].exp).dim_out != 0)
														PARSE_CONTEXT.output_error_parse(ERROR_TYPE_COMP, ERROR_LVL_NORMAL, "array dimensions don't match");/*TODO*/
                                                      //if (! TP_IS_TUPEL($5.tp))
                                                        //	PARSE_CONTEXT.output_error_parse(ERROR_TYPE_COMP, ERROR_LVL_NORMAL, "right hand side is not a tupel");/*TODO*/
                                                      //if (RANK_TP_TUPEL($5.tp) != tupel_cnt)
                                                          //PARSE_CONTEXT.output_error_parse(ERROR_TYPE_COMP, ERROR_LVL_NORMAL, "tupel rank doesn't match");/*TODO*/

													  COMP_PAR(TUPEL_DISAGGR_REV, tupel_cnt);
													  PARSE_CONTEXT.comp_symbols_assign(symblock, tupel_cnt, true);
												  }
												  //GLV->is_scann_const_vekt = false;	/*TODO: entfaellt*/
												;}
    break;

  case 16:
#line 341 "src/cmpl/CMPL_Parser.yy"
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
#line 357 "src/cmpl/CMPL_Parser.yy"
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
												    PARSE_CONTEXT.init_symbol(&(yysemantic_stack_[(7) - (2)].exp), PTR_TP_CONST, (yysemantic_stack_[(7) - (6)].exp).tp, (yysemantic_stack_[(7) - (2)].exp).sym_dim, false);
												  else
												  	PARSE_CONTEXT.test_tpk_symbol(symp2, (yysemantic_stack_[(7) - (6)].exp).tp, true);

												  if ((yysemantic_stack_[(7) - (2)].exp).sub_sym_ind < 0)
												  {
				 								  	COMP_SUB_PAR_CNT(DEF_CONST, SYM_MAIN_REF + 256*(yysemantic_stack_[(7) - (6)].exp).dim_out, (yysemantic_stack_[(7) - (3)].t), 1);
				 								  	COMP_INT(symp2->symno * MAX_ANZ_IND_1 + (yysemantic_stack_[(7) - (2)].exp).sym_dim);
												  }
												  else
												  {
				 								  	COMP_SUB_PAR_CNT(DEF_CONST, SYM_SUB_REF + 256*(yysemantic_stack_[(7) - (6)].exp).dim_out, (yysemantic_stack_[(7) - (3)].t), 1);
				 								  	COMP_INT((yysemantic_stack_[(7) - (2)].exp).sub_sym_ind);
												  }
												;}
    break;

  case 23:
#line 416 "src/cmpl/CMPL_Parser.yy"
    { (yyval.exp) = (yysemantic_stack_[(1) - (1)].exp);
		   										;}
    break;

  case 24:
#line 420 "src/cmpl/CMPL_Parser.yy"
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
#line 436 "src/cmpl/CMPL_Parser.yy"
    { (yyval.exp) = (yysemantic_stack_[(1) - (1)].exp); ;}
    break;

  case 26:
#line 437 "src/cmpl/CMPL_Parser.yy"
    { (yyval.exp) = (yysemantic_stack_[(1) - (1)].exp); ;}
    break;

  case 27:
#line 441 "src/cmpl/CMPL_Parser.yy"
    { (yyval.t) = TPP_EMPTY; ;}
    break;

  case 28:
#line 442 "src/cmpl/CMPL_Parser.yy"
    { (yyval.t) = (yysemantic_stack_[(2) - (2)].t); ;}
    break;

  case 29:
#line 446 "src/cmpl/CMPL_Parser.yy"
    { (yyval.t) = TP_DBL; ;}
    break;

  case 30:
#line 447 "src/cmpl/CMPL_Parser.yy"
    { (yyval.t) = TP_INT; ;}
    break;

  case 31:
#line 448 "src/cmpl/CMPL_Parser.yy"
    { (yyval.t) = TP_BIN; ;}
    break;

  case 32:
#line 449 "src/cmpl/CMPL_Parser.yy"
    { (yyval.t) = TP_STR; ;}
    break;

  case 33:
#line 450 "src/cmpl/CMPL_Parser.yy"
    { (yyval.t) = TPK_PROBLEM_MAIN; ;}
    break;

  case 35:
#line 455 "src/cmpl/CMPL_Parser.yy"
    { PARSE_CONTEXT.out_warn_old_assign(false); ;}
    break;

  case 39:
#line 466 "src/cmpl/CMPL_Parser.yy"
    { COMP_PAR_INT(LIT_VAL, (yysemantic_stack_[(2) - (2)].exp).dim_in);	/*TODO: Anzahl Elemente als besonderer Typ, zur Kennzeichnung das Array*/
												  // Array-Parameter sind derzeit nicht erlaubt
												  if ((yysemantic_stack_[(2) - (2)].exp).dim_elem != 0)
													  PARSE_CONTEXT.output_error_parse(ERROR_TYPE_COMP, ERROR_LVL_NORMAL, "array parameters are not allowed");	/*TODO*/
												  COMP_SUB(CMPL_SOLVE, CMPL_PROBLEM_SOLVE); ;}
    break;

  case 40:
#line 476 "src/cmpl/CMPL_Parser.yy"
    { GLV->sect = (int) token::SECT_VAR; ;}
    break;

  case 44:
#line 482 "src/cmpl/CMPL_Parser.yy"
    { yyerrok; ;}
    break;

  case 46:
#line 487 "src/cmpl/CMPL_Parser.yy"
    { COMP_SUB(DEF_VAR_TYPE, (yysemantic_stack_[(4) - (3)].i)); ;}
    break;

  case 53:
#line 499 "src/cmpl/CMPL_Parser.yy"
    { COMP_PAR(LIT_VAL_SHORT, (yysemantic_stack_[(1) - (1)].exp).dim_in);
												  COMP_SUB(SET_INDEXES, IND_GIVEN);
												  PARSE_CONTEXT.init_symbol(&(yysemantic_stack_[(1) - (1)].exp), PTR_TP_VAR, TPK_VAR, (yysemantic_stack_[(1) - (1)].exp).sym_dim, false);
												  COMP(DEF_VAR, 1);
												  COMP_INT((yysemantic_stack_[(1) - (1)].exp).symp()->symno * MAX_ANZ_IND_1 + (yysemantic_stack_[(1) - (1)].exp).sym_dim); ;}
    break;

  case 54:
#line 504 "src/cmpl/CMPL_Parser.yy"
    { COMP(DEF_VAR, 1);
												  COMP_INT((yysemantic_stack_[(1) - (1)].exp).symp()->symno * MAX_ANZ_IND_1 + (yysemantic_stack_[(1) - (1)].exp).sym_dim); ;}
    break;

  case 55:
#line 509 "src/cmpl/CMPL_Parser.yy"
    { (yyval.i) = (int)TP_VAR_DBL; ;}
    break;

  case 56:
#line 510 "src/cmpl/CMPL_Parser.yy"
    { (yyval.i) = (int)TP_VAR_INT; ;}
    break;

  case 57:
#line 511 "src/cmpl/CMPL_Parser.yy"
    { (yyval.i) = (int)TP_VAR_BIN; ;}
    break;

  case 58:
#line 515 "src/cmpl/CMPL_Parser.yy"
    { COMP(RESET_VAR_B, 0); ;}
    break;

  case 59:
#line 516 "src/cmpl/CMPL_Parser.yy"
    { COMP(RESET_VAR_B, 0);
												  if((yysemantic_stack_[(5) - (4)].f)) COMP(DEF_UP_VAR, 0);
												  if((yysemantic_stack_[(5) - (2)].f)) COMP(DEF_DN_VAR, 0); ;}
    break;

  case 60:
#line 525 "src/cmpl/CMPL_Parser.yy"
    { /*COMP_PAR(LIT_VAL_SHORT, $2.dim_in);
									  COMP_SUB(SET_INDEXES, IND_GIVEN);*/
									  //GLV->index_info.i_anz_ind = $2.dim_in;
									  //GLV->index_info.i_free_ind = ($2.dim_out >= 0 ? $2.dim_out : 0);
									  /*$$.set((void*)$1, TPP_EMPTY, 0, true, $2.dim_in, $2.dim_out, $2.dim_fr_end);*/
									  (yyval.exp) = (yysemantic_stack_[(2) - (1)].exp);
									  (yyval.exp).sym_dim = 0;
									  if ((yysemantic_stack_[(2) - (2)].exp).dim_in > 0) {
									  	(yyval.exp).set_dim((yysemantic_stack_[(2) - (2)].exp).dim_in, (yysemantic_stack_[(2) - (2)].exp).dim_out, (yysemantic_stack_[(2) - (2)].exp).dim_fr_end);
										(yyval.exp).sym_dim = (yysemantic_stack_[(2) - (2)].exp).dim_t_rank1;
									  }
									;}
    break;

  case 64:
#line 552 "src/cmpl/CMPL_Parser.yy"
    { GLV->sect = (int) token::SECT_OBJ; ;}
    break;

  case 68:
#line 558 "src/cmpl/CMPL_Parser.yy"
    { yyerrok; ;}
    break;

  case 70:
#line 563 "src/cmpl/CMPL_Parser.yy"
    { GLV->is_left = true; ;}
    break;

  case 71:
#line 564 "src/cmpl/CMPL_Parser.yy"
    { COMP_PAR(LINE_MOD, MODE_N);
												  COMP(LINE_END, 0); ;}
    break;

  case 77:
#line 575 "src/cmpl/CMPL_Parser.yy"
    { COMP_PAR(LINE_OBJ_TO, 1); ;}
    break;

  case 78:
#line 576 "src/cmpl/CMPL_Parser.yy"
    { COMP_PAR(LINE_OBJ_TO, -1); ;}
    break;

  case 82:
#line 591 "src/cmpl/CMPL_Parser.yy"
    { GLV->sect = (int) token::SECT_CSTR; ;}
    break;

  case 86:
#line 597 "src/cmpl/CMPL_Parser.yy"
    { yyerrok; ;}
    break;

  case 88:
#line 602 "src/cmpl/CMPL_Parser.yy"
    { COMP(LINE_END, 0); ;}
    break;

  case 93:
#line 611 "src/cmpl/CMPL_Parser.yy"
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
#line 635 "src/cmpl/CMPL_Parser.yy"
    { (yyval.i) = 1; ;}
    break;

  case 102:
#line 638 "src/cmpl/CMPL_Parser.yy"
    { (yyval.i) = 1; ;}
    break;

  case 103:
#line 639 "src/cmpl/CMPL_Parser.yy"
    { (yyval.i) = -1; ;}
    break;

  case 111:
#line 657 "src/cmpl/CMPL_Parser.yy"
    { COMP(TERM_NEG, 0); /* Negation */ ;}
    break;

  case 112:
#line 658 "src/cmpl/CMPL_Parser.yy"
    { COMP(TERM_NEG, 0); /* Negation (um Negation am Anfang zurueckzunehmen) */ ;}
    break;

  case 115:
#line 667 "src/cmpl/CMPL_Parser.yy"
    { COMP(TERM_BEG, 0);
												  COMP_PAR(LIT_VAL_SHORT, (yysemantic_stack_[(0) - (0)].i)); /* Vorzeichen Term */ ;}
    break;

  case 116:
#line 669 "src/cmpl/CMPL_Parser.yy"
    { COMP(TERM_END, 0);
												  PARSE_CONTEXT.var_test_prod((yysemantic_stack_[(2) - (2)].i)); ;}
    break;

  case 117:
#line 673 "src/cmpl/CMPL_Parser.yy"
    { if((yysemantic_stack_[(1) - (1)].i)==0) COMP_SUB(NUM_OP, MULT);
												  (yyval.i) = (yysemantic_stack_[(1) - (1)].i); ;}
    break;

  case 118:
#line 675 "src/cmpl/CMPL_Parser.yy"
    { if((yysemantic_stack_[(3) - (1)].i)==0) COMP_SUB(NUM_OP, MULT);
												  (yyval.i) = (yysemantic_stack_[(3) - (1)].i) + (yysemantic_stack_[(3) - (3)].i); ;}
    break;

  case 119:
#line 679 "src/cmpl/CMPL_Parser.yy"
    { COMP(TERM_VAR_FREE, 1);
												  COMP_INT((yysemantic_stack_[(2) - (1)].exp).symp()->symno * MAX_ANZ_IND_1 + (yysemantic_stack_[(2) - (1)].exp).sym_dim);
												  (yyval.i) = 1; ;}
    break;

  case 120:
#line 682 "src/cmpl/CMPL_Parser.yy"
    { if ((yysemantic_stack_[(1) - (1)].exp).dim_t_undef > 0)
													PARSE_CONTEXT.output_error_parse(ERROR_TYPE_COMP, ERROR_LVL_NORMAL, "undefined symbol");/*TODO*/
												  (yyval.i) = 0; ;}
    break;

  case 121:
#line 685 "src/cmpl/CMPL_Parser.yy"
    { COMP_SUB_PAR_CNT(F_CONST_FREE, SYM_MAIN, 0, 1);
												  COMP_INT((yysemantic_stack_[(2) - (1)].exp).symp()->symno * MAX_ANZ_IND_1 + (yysemantic_stack_[(2) - (1)].exp).sym_dim);
												  (yyval.i) = 0; ;}
    break;

  case 123:
#line 691 "src/cmpl/CMPL_Parser.yy"
    { COMP(IND_TRANSPOSE, 0); ;}
    break;

  case 127:
#line 707 "src/cmpl/CMPL_Parser.yy"
    { (yyval.exp) = (yysemantic_stack_[(1) - (1)].exp); ;}
    break;

  case 128:
#line 708 "src/cmpl/CMPL_Parser.yy"
    { (yyval.exp) = (yysemantic_stack_[(1) - (1)].exp); ;}
    break;

  case 129:
#line 709 "src/cmpl/CMPL_Parser.yy"
    { (yyval.exp) = (yysemantic_stack_[(1) - (1)].exp); ;}
    break;

  case 130:
#line 711 "src/cmpl/CMPL_Parser.yy"
    { (yyval.exp) = (yysemantic_stack_[(1) - (1)].exp); ;}
    break;

  case 131:
#line 712 "src/cmpl/CMPL_Parser.yy"
    { (yyval.exp) = (yysemantic_stack_[(1) - (1)].exp); ;}
    break;

  case 132:
#line 713 "src/cmpl/CMPL_Parser.yy"
    { COMP(FETCH_TMP, 1);
												  //COMP_INT((-$1-2));
												  COMP_INT((yysemantic_stack_[(1) - (1)].exp).symp()->symno);
												  (yyval.exp).set(/*TPK_VALUE*/ (yysemantic_stack_[(1) - (1)].exp).symp()->tpk, 0); ;}
    break;

  case 133:
#line 722 "src/cmpl/CMPL_Parser.yy"
    { COMP_PAR_INT(LIT_VAL, (yysemantic_stack_[(1) - (1)].i));
												  (yyval.exp).set(TP_INT, 0); ;}
    break;

  case 134:
#line 724 "src/cmpl/CMPL_Parser.yy"
    { COMP(LIT_VAL, 1);
												  COMP_DOUBLE((yysemantic_stack_[(1) - (1)].d));
												  (yyval.exp).set(TP_DBL, 0); ;}
    break;

  case 135:
#line 727 "src/cmpl/CMPL_Parser.yy"
    { COMP(LIT_VAL, 1);
												  COMP_ISTR((yysemantic_stack_[(1) - (1)].i));
												  (yyval.exp).set(TP_ISTR, 0); ;}
    break;

  case 136:
#line 732 "src/cmpl/CMPL_Parser.yy"
    { /* Anzahl Elemente als Listenzähler auf den Stack */
		  										  COMP(LIT_VAL, 1);
												  COMP_VAL(TPP_LIST_VAL, i, (yysemantic_stack_[(1) - (1)].exp).dim_in);
												  /* Tupelkonstruktion */
												  COMP(CONSTRUCT_TUPEL, 0);
												  (yyval.exp).set(((yysemantic_stack_[(1) - (1)].exp).dim_out > 0 ? TPK_TUPELSET_N((yysemantic_stack_[(1) - (1)].exp).dim_t_rank1) : TPK_TUPEL_N((yysemantic_stack_[(1) - (1)].exp).dim_t_rank1)), 0);
												  (yyval.exp).lval = (yysemantic_stack_[(1) - (1)].exp).lval;
												  (yyval.exp).dim_t_rank1 = (yysemantic_stack_[(1) - (1)].exp).dim_t_rank1;
												  (yyval.exp).dim_t_undef = (yysemantic_stack_[(1) - (1)].exp).dim_t_undef; ;}
    break;

  case 137:
#line 743 "src/cmpl/CMPL_Parser.yy"
    { if ((yysemantic_stack_[(4) - (3)].exp).dim_t_undef > 0)
												  	PARSE_CONTEXT.output_error_parse(ERROR_TYPE_COMP, ERROR_LVL_NORMAL, "undefined symbol");/*TODO*/
												  COMP_SUB_PAR(CONV_OP, NO_FORMAT, (yysemantic_stack_[(4) - (1)].t));
												  (yyval.exp).set((yysemantic_stack_[(4) - (1)].t), 0); ;}
    break;

  case 138:
#line 749 "src/cmpl/CMPL_Parser.yy"
    { (yysemantic_stack_[(2) - (2)].i) = (int) token::FCT_MAX; ;}
    break;

  case 139:
#line 750 "src/cmpl/CMPL_Parser.yy"
    { COMP_PAR_INT(LIT_VAL, (yysemantic_stack_[(5) - (4)].i));
												  COMP_SUB(AGGR_OP, MAX_VAL);
												  (yyval.exp).set(TPK_VALUE, 0); ;}
    break;

  case 140:
#line 753 "src/cmpl/CMPL_Parser.yy"
    { (yysemantic_stack_[(2) - (2)].i) = (int) token::FCT_MIN; ;}
    break;

  case 141:
#line 754 "src/cmpl/CMPL_Parser.yy"
    { COMP_PAR_INT(LIT_VAL, (yysemantic_stack_[(5) - (4)].i));
												  COMP_SUB(AGGR_OP, MIN_VAL);
												  (yyval.exp).set(TPK_VALUE, 0); ;}
    break;

  case 142:
#line 757 "src/cmpl/CMPL_Parser.yy"
    { /*COMP_PAR(LIT_VAL_SHORT, $3.dim_in);
												  COMP_SUB(SET_INDEXES, IND_GIVEN);*/
												  COMP_SUB_PAR_CNT(DEFSET_EXTNUM, MAX_VAL, 0, 1);
												  COMP_INT((yysemantic_stack_[(4) - (3)].exp).symp()->symno * MAX_ANZ_IND_1 + (yysemantic_stack_[(4) - (3)].exp).sym_dim);
												  (yyval.exp).set(TP_INT, 0); ;}
    break;

  case 143:
#line 762 "src/cmpl/CMPL_Parser.yy"
    { (yyval.exp).set(TP_INT, 0); ;}
    break;

  case 144:
#line 764 "src/cmpl/CMPL_Parser.yy"
    { if ((yysemantic_stack_[(6) - (3)].exp).dim_t_undef > 0 || (yysemantic_stack_[(6) - (5)].exp).dim_t_undef > 0)
													PARSE_CONTEXT.output_error_parse(ERROR_TYPE_COMP, ERROR_LVL_NORMAL, "undefined symbol");/*TODO*/
												  COMP_SUB_PAR(CONV_OP, WITH_FORMAT, TP_ISTR);
												  (yyval.exp).set(TP_ISTR, 0); ;}
    break;

  case 145:
#line 768 "src/cmpl/CMPL_Parser.yy"
    { if ((yysemantic_stack_[(4) - (3)].exp).dim_t_undef > 0)
													PARSE_CONTEXT.output_error_parse(ERROR_TYPE_COMP, ERROR_LVL_NORMAL, "undefined symbol");/*TODO*/
												  COMP_SUB(TYPE_STR, TYPE_EXPR);
												  (yyval.exp).set(TP_ISTR, 0); ;}
    break;

  case 146:
#line 772 "src/cmpl/CMPL_Parser.yy"
    { COMP_SUB_PAR_CNT(TYPE_STR, TYPE_VAR, 0, 1);
												  COMP_INT((yysemantic_stack_[(4) - (3)].exp).symp()->symno * MAX_ANZ_IND_1 + (yysemantic_stack_[(4) - (3)].exp).sym_dim);
												  (yyval.exp).set(TP_ISTR, 0); ;}
    break;

  case 147:
#line 775 "src/cmpl/CMPL_Parser.yy"
    { if ((yysemantic_stack_[(4) - (3)].exp).dim_t_undef > 0)
                                                                                                  PARSE_CONTEXT.output_error_parse(ERROR_TYPE_COMP, ERROR_LVL_NORMAL, "undefined symbol");/*TODO*/
												  COMP_SUB(UTIL_OP, LEN_VAL);
												  (yyval.exp).set(TP_INT, 0); ;}
    break;

  case 148:
#line 779 "src/cmpl/CMPL_Parser.yy"
    { if ((yysemantic_stack_[(4) - (3)].exp).dim_t_undef > 0)
													PARSE_CONTEXT.output_error_parse(ERROR_TYPE_COMP, ERROR_LVL_NORMAL, "undefined symbol");/*TODO*/
												  COMP_SUB(MATH_FUNC, (yysemantic_stack_[(4) - (1)].i));
												  (yyval.exp).set(TPU_NUM, 0); ;}
    break;

  case 149:
#line 783 "src/cmpl/CMPL_Parser.yy"
    { if ((yysemantic_stack_[(4) - (3)].exp).dim_t_undef > 0)
													PARSE_CONTEXT.output_error_parse(ERROR_TYPE_COMP, ERROR_LVL_NORMAL, "undefined symbol");/*TODO*/
												  COMP(READ_STDIN, 0);
												  (yyval.exp).set(TPK_VALUE, 0); ;}
    break;

  case 150:
#line 787 "src/cmpl/CMPL_Parser.yy"
    { COMP_PAR_INT(LIT_VAL, (yysemantic_stack_[(2) - (2)].exp).dim_in); /*TODO: Anzahl Elemente als besonderer Typ, zur Kennzeichnung das Array*/
												  // Array-Parameter sind derzeit nicht erlaubt
												  if ((yysemantic_stack_[(2) - (2)].exp).dim_elem != 0)
													  PARSE_CONTEXT.output_error_parse(ERROR_TYPE_COMP, ERROR_LVL_NORMAL, "array parameters are not allowed");	/*TODO*/
												  COMP_SUB(CMPL_SOLVE, (yysemantic_stack_[(2) - (1)].i));
												  (yyval.exp).set(TP_INT, 0); ;}
    break;

  case 151:
#line 796 "src/cmpl/CMPL_Parser.yy"
    { (yyval.exp) = PARSE_CONTEXT.parse_type_dev_subsym_op(&(yysemantic_stack_[(4) - (1)].exp), (yysemantic_stack_[(4) - (3)].i));
												  COMP_PAR_INT(LIT_VAL, (yysemantic_stack_[(4) - (4)].exp).dim_in);	//Anzahl muss hier auf den Stack, damit feststeht, wo auf dem Stack die Subsymboltabelle kommt
												  COMP_SUB_PAR_CNT(FETCH_CONST, SYM_SUB, 0, 1);
												  COMP_INT((yyval.exp).sub_sym_ind);
												  (yyval.exp).sym_dim = 0;
												  if ((yysemantic_stack_[(4) - (4)].exp).dim_in > 0)
												  {
												    if ((yyval.exp).dim_out >= 0 && (yyval.exp).dim_out != (yysemantic_stack_[(4) - (4)].exp).dim_in)
														PARSE_CONTEXT.output_error_parse(ERROR_TYPE_COMP, ERROR_LVL_NORMAL, "array dimensions don't match");/*TODO*/
												  	(yyval.exp).set_dim((yysemantic_stack_[(4) - (4)].exp).dim_in, (yysemantic_stack_[(4) - (4)].exp).dim_out, (yysemantic_stack_[(4) - (4)].exp).dim_fr_end);
													(yyval.exp).sym_dim = (yysemantic_stack_[(4) - (4)].exp).dim_t_rank1;
												  }
												  if (!((yyval.exp).symp()->isvalid()) && TP_SUB_AUTODEF((yysemantic_stack_[(4) - (1)].exp).tp))
												  {
												  	(yyval.exp).set_dim((yysemantic_stack_[(4) - (4)].exp).dim_in, (yysemantic_stack_[(4) - (4)].exp).dim_out, (yysemantic_stack_[(4) - (4)].exp).dim_fr_end);
												  	PARSE_CONTEXT.init_symbol(&(yyval.exp), PTR_TP_CONST, TPK_VALUE, (yyval.exp).sym_dim, false);
												  } ;}
    break;

  case 152:
#line 815 "src/cmpl/CMPL_Parser.yy"
    { (yyval.exp) = (yysemantic_stack_[(1) - (1)].exp); ;}
    break;

  case 153:
#line 816 "src/cmpl/CMPL_Parser.yy"
    { (yyval.exp) = (yysemantic_stack_[(1) - (1)].exp); ;}
    break;

  case 154:
#line 817 "src/cmpl/CMPL_Parser.yy"
    { (yyval.exp) = (yysemantic_stack_[(1) - (1)].exp); ;}
    break;

  case 155:
#line 820 "src/cmpl/CMPL_Parser.yy"
    { (yyval.i) = (yysemantic_stack_[(1) - (1)].i); ;}
    break;

  case 156:
#line 821 "src/cmpl/CMPL_Parser.yy"
    { (yyval.i) = (yysemantic_stack_[(1) - (1)].i); ;}
    break;

  case 157:
#line 825 "src/cmpl/CMPL_Parser.yy"
    { (yyval.i) = 0; ;}
    break;

  case 158:
#line 829 "src/cmpl/CMPL_Parser.yy"
    { /* $$.set((void*)$1, TPK_VAR, 0, true, $2.dim_in, $2.dim_out, $2.dim_fr_end); */
												  (yyval.exp) = (yysemantic_stack_[(2) - (1)].exp);
												  (yyval.exp).sym_dim = 0;
												  if ((yysemantic_stack_[(2) - (2)].exp).dim_in > 0) {
													(yyval.exp).set_dim((yysemantic_stack_[(2) - (2)].exp).dim_in, (yysemantic_stack_[(2) - (2)].exp).dim_out, (yysemantic_stack_[(2) - (2)].exp).dim_fr_end);
													(yyval.exp).sym_dim = (yysemantic_stack_[(2) - (2)].exp).dim_t_rank1;
												  }
												  if ((yysemantic_stack_[(2) - (1)].exp).dim_out != (yyval.exp).sym_dim)
												  	PARSE_CONTEXT.output_error_parse(ERROR_TYPE_COMP, ERROR_LVL_NORMAL, "array dimensions don't match");/*TODO*/
												;}
    break;

  case 159:
#line 845 "src/cmpl/CMPL_Parser.yy"
    { // TODO: Test ob Indexanzahl passt: $2.dim_in == 0 || $1.dim_out == $2.dim_in
												  //		Wenn $2.dim_in == 0, dann COMP fuer freies Indexinfo; setzen $2.dim_in = $1.dim_out 
												  COMP_PAR(LIT_VAL_SHORT, (yysemantic_stack_[(2) - (2)].exp).dim_in);
												  COMP_SUB(SET_INDEXES, IND_GIVEN);
												  //GLV->index_info.i_anz_ind = $2.dim_in;
												  //GLV->index_info.i_free_ind = ($2.dim_out >= 0 ? $2.dim_out : 0);
									  			  /* $$.set((void*)$1, TPK_VAR, 0, true, $2.dim_in, $2.dim_out, $2.dim_fr_end); */
												  (yyval.exp) = (yysemantic_stack_[(2) - (1)].exp);
												  (yyval.exp).sym_dim = 0;
												  if ((yysemantic_stack_[(2) - (2)].exp).dim_in > 0) {
													(yyval.exp).set_dim((yysemantic_stack_[(2) - (2)].exp).dim_in, (yysemantic_stack_[(2) - (2)].exp).dim_out, (yysemantic_stack_[(2) - (2)].exp).dim_fr_end);
													(yyval.exp).sym_dim = (yysemantic_stack_[(2) - (2)].exp).dim_t_rank1;
												  }
												  if ((yysemantic_stack_[(2) - (1)].exp).dim_out != (yyval.exp).sym_dim)
												  	PARSE_CONTEXT.output_error_parse(ERROR_TYPE_COMP, ERROR_LVL_NORMAL, "array dimensions don't match");/*TODO*/
												;}
    break;

  case 160:
#line 864 "src/cmpl/CMPL_Parser.yy"
    { (yyval.i) = 1; ;}
    break;

  case 161:
#line 865 "src/cmpl/CMPL_Parser.yy"
    { (yysemantic_stack_[(2) - (2)].i) = (yysemantic_stack_[(2) - (-1)].i); ;}
    break;

  case 162:
#line 866 "src/cmpl/CMPL_Parser.yy"
    { (yyval.i) = (yysemantic_stack_[(4) - (4)].i) + 1; ;}
    break;

  case 163:
#line 870 "src/cmpl/CMPL_Parser.yy"
    { (yyval.i) = 0; ;}
    break;

  case 164:
#line 874 "src/cmpl/CMPL_Parser.yy"
    { if ((yysemantic_stack_[(1) - (1)].exp).dim_t_undef > 0)
													PARSE_CONTEXT.output_error_parse(ERROR_TYPE_COMP, ERROR_LVL_NORMAL, "undefined symbol");/*TODO*/
												;}
    break;

  case 165:
#line 877 "src/cmpl/CMPL_Parser.yy"
    { if ((yysemantic_stack_[(2) - (2)].exp).dim_in > 0)
												  {
												  	if ((yysemantic_stack_[(2) - (1)].exp).dim_out != (yysemantic_stack_[(2) - (2)].exp).dim_t_rank1)
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
#line 894 "src/cmpl/CMPL_Parser.yy"
    { EXPSTR exp = PARSE_CONTEXT.parse_type_dev_subsym_op(&(yysemantic_stack_[(4) - (1)].exp), (yysemantic_stack_[(4) - (3)].i));
												  if ((yysemantic_stack_[(4) - (4)].exp).dim_in > 0)
												  {
												  	if (exp.dim_out >= 0 && exp.dim_out != (yysemantic_stack_[(4) - (4)].exp).dim_t_rank1)
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
												  	PARSE_CONTEXT.init_symbol(&exp, PTR_TP_CONST, TPK_VALUE, (yysemantic_stack_[(4) - (4)].exp).dim_t_rank1, false);
												  }
												  if((yysemantic_stack_[(4) - (-1)].i) == (int) token::FCT_MAX) COMP_SUB(AGGR_OP, MAX_VAL_NULL);
												  else COMP_SUB(AGGR_OP, MIN_VAL_NULL); ;}
    break;

  case 167:
#line 919 "src/cmpl/CMPL_Parser.yy"
    { /*$$.set((void*)$1, TPK_VAR, 0, true, $2.dim_in, $2.dim_out, $2.dim_fr_end);*/ /*$$ = $1 * MAX_ANZ_IND_1 + $2.dim_in;*/
												  if ((yysemantic_stack_[(2) - (2)].exp).dim_in > 0)
												  {

												  	if ((yysemantic_stack_[(2) - (1)].exp).dim_out != (yysemantic_stack_[(2) - (2)].exp).dim_t_rank1)
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
												  if ((yysemantic_stack_[(2) - (2)].exp).dim_in > 0) {
													(yyval.exp).set_dim((yysemantic_stack_[(2) - (2)].exp).dim_in, (yysemantic_stack_[(2) - (2)].exp).dim_out, (yysemantic_stack_[(2) - (2)].exp).dim_fr_end);
													(yyval.exp).sym_dim = (yysemantic_stack_[(2) - (2)].exp).dim_t_rank1;
												  }
												  else {
												  	(yyval.exp).set_dim((yysemantic_stack_[(2) - (1)].exp).dim_out, (yysemantic_stack_[(2) - (1)].exp).dim_out, (yysemantic_stack_[(2) - (1)].exp).dim_out);
													(yyval.exp).sym_dim = (yysemantic_stack_[(2) - (1)].exp).dim_out;
												  }
												;}
    break;

  case 168:
#line 945 "src/cmpl/CMPL_Parser.yy"
    { (yyval.exp) = (yysemantic_stack_[(1) - (1)].exp); ;}
    break;

  case 169:
#line 946 "src/cmpl/CMPL_Parser.yy"
    { (yyval.exp) = (yysemantic_stack_[(1) - (1)].exp); ;}
    break;

  case 170:
#line 952 "src/cmpl/CMPL_Parser.yy"
    { /*COMP_PAR(LIT_VAL_SHORT, $1.dim_in);
												  COMP_SUB(SET_INDEXES, IND_GIVEN);*/
												  COMP(DEF_FUNC, 1);
												  COMP_INT((yysemantic_stack_[(1) - (1)].exp).symp()->symno * MAX_ANZ_IND_1 + (yysemantic_stack_[(1) - (1)].exp).sym_dim); ;}
    break;

  case 171:
#line 956 "src/cmpl/CMPL_Parser.yy"
    { COMP_PAR(LIT_VAL_SHORT, (yysemantic_stack_[(2) - (2)].exp).dim_in);
												  COMP_SUB(SET_INDEXES, IND_GIVEN);
												  COMP(DEL_INDEXES, 0);
												  COMP_PAR(LIT_VAL_SHORT, 0); ;}
    break;

  case 172:
#line 960 "src/cmpl/CMPL_Parser.yy"
    { COMP_PAR(LIT_VAL_SHORT, 1); ;}
    break;

  case 173:
#line 966 "src/cmpl/CMPL_Parser.yy"
    { /*COMP_PAR(LIT_VAL_SHORT, 0);*/
										  (yyval.exp).set_dim(0, 0, 0);
										  /*$$ = 0;*/ ;}
    break;

  case 174:
#line 969 "src/cmpl/CMPL_Parser.yy"
    { (yyval.exp) = (yysemantic_stack_[(1) - (1)].exp);
										  // Undefinierte Symbole hier nicht erlaubt
										  if ((yysemantic_stack_[(1) - (1)].exp).dim_t_undef > 0)
											PARSE_CONTEXT.output_error_parse(ERROR_TYPE_COMP, ERROR_LVL_NORMAL, "undefined symbols in list");/*TODO*/
										;}
    break;

  case 175:
#line 1006 "src/cmpl/CMPL_Parser.yy"
    { (yyval.f) = false; ;}
    break;

  case 176:
#line 1007 "src/cmpl/CMPL_Parser.yy"
    { if ((yysemantic_stack_[(1) - (1)].exp).dim_t_undef > 0)
											PARSE_CONTEXT.output_error_parse(ERROR_TYPE_COMP, ERROR_LVL_NORMAL, "undefined symbol");/*TODO*/
										  (yyval.f) = true; ;}
    break;

  case 177:
#line 1014 "src/cmpl/CMPL_Parser.yy"
    { (yyval.exp) = (yysemantic_stack_[(1) - (1)].exp); ;}
    break;

  case 178:
#line 1015 "src/cmpl/CMPL_Parser.yy"
    { (yyval.exp) = (yysemantic_stack_[(1) - (1)].exp); ;}
    break;

  case 179:
#line 1016 "src/cmpl/CMPL_Parser.yy"
    { (yyval.exp) = (yysemantic_stack_[(1) - (1)].exp); ;}
    break;

  case 180:
#line 1017 "src/cmpl/CMPL_Parser.yy"
    { (yyval.exp) = (yysemantic_stack_[(1) - (1)].exp); ;}
    break;

  case 181:
#line 1024 "src/cmpl/CMPL_Parser.yy"
    { (yyval.exp) = (yysemantic_stack_[(1) - (1)].exp); ;}
    break;

  case 182:
#line 1025 "src/cmpl/CMPL_Parser.yy"
    { (yyval.exp) = (yysemantic_stack_[(1) - (1)].exp); ;}
    break;

  case 183:
#line 1026 "src/cmpl/CMPL_Parser.yy"
    { (yyval.exp) = (yysemantic_stack_[(1) - (1)].exp); ;}
    break;

  case 184:
#line 1027 "src/cmpl/CMPL_Parser.yy"
    { (yyval.exp) = (yysemantic_stack_[(1) - (1)].exp); ;}
    break;

  case 185:
#line 1028 "src/cmpl/CMPL_Parser.yy"
    { (yyval.exp) = (yysemantic_stack_[(1) - (1)].exp); ;}
    break;

  case 186:
#line 1029 "src/cmpl/CMPL_Parser.yy"
    { if ((yysemantic_stack_[(2) - (2)].exp).dim_t_undef > 0)
													PARSE_CONTEXT.output_error_parse(ERROR_TYPE_COMP, ERROR_LVL_NORMAL, "undefined symbol");/*TODO*/
												  COMP_SUB(LOG_OP, NOT);
												  (yyval.exp).set(TP_BIN, 0); ;}
    break;

  case 187:
#line 1036 "src/cmpl/CMPL_Parser.yy"
    { (yyval.exp) = (yysemantic_stack_[(1) - (1)].exp); ;}
    break;

  case 188:
#line 1041 "src/cmpl/CMPL_Parser.yy"
    { COMP(LIT_VAL, 1);
												  COMP_VAL(TPP_NULL, i, 0); ;}
    break;

  case 189:
#line 1044 "src/cmpl/CMPL_Parser.yy"
    { EXPSTR a, b = PARSE_CONTEXT.parse_type_dev_switch(&(yysemantic_stack_[(6) - (4)].exp), &(yysemantic_stack_[(6) - (5)].exp));
												  a.set(TPP_NULL, 0);
												  (yyval.exp) = PARSE_CONTEXT.parse_type_dev_num_op(ADD, &a, &b); ;}
    break;

  case 190:
#line 1049 "src/cmpl/CMPL_Parser.yy"
    { COMP_PAR(LIT_VAL_SHORT, 0); ;}
    break;

  case 191:
#line 1051 "src/cmpl/CMPL_Parser.yy"
    { EXPSTR a, b = PARSE_CONTEXT.parse_type_dev_switch(&(yysemantic_stack_[(6) - (4)].exp), &(yysemantic_stack_[(6) - (5)].exp));
												  a.set(TP_INT, 0);
												  (yyval.exp) = PARSE_CONTEXT.parse_type_dev_num_op(ADD, &a, &b);
												  if (TYPECLASS((yyval.exp).tp) != TPK_VALUE)
													PARSE_CONTEXT.output_error_parse(ERROR_TYPE_COMP, ERROR_LVL_NORMAL, ERROR_NOT_A_NUMBER_PARSE);
												  COMP_PAR(LIT_SET, TP_SET_ENUM);
												  (yyval.exp).set(TPK_SET, 0); ;}
    break;

  case 192:
#line 1061 "src/cmpl/CMPL_Parser.yy"
    { COMP_SUB((TYPECLASS((yysemantic_stack_[(3) - (1)].exp).tp)==TPK_SET ? SET_OP : NUM_OP), ADD);
															  (yyval.exp) = PARSE_CONTEXT.parse_type_dev_num_op(ADD, &(yysemantic_stack_[(3) - (1)].exp), &(yysemantic_stack_[(3) - (3)].exp)); ;}
    break;

  case 193:
#line 1063 "src/cmpl/CMPL_Parser.yy"
    { COMP_SUB(NUM_OP, SUB);
															  (yyval.exp) = PARSE_CONTEXT.parse_type_dev_num_op(SUB, &(yysemantic_stack_[(3) - (1)].exp), &(yysemantic_stack_[(3) - (3)].exp)); ;}
    break;

  case 194:
#line 1065 "src/cmpl/CMPL_Parser.yy"
    { COMP_SUB((TYPECLASS((yysemantic_stack_[(3) - (1)].exp).tp)==TPK_SET ? SET_OP : NUM_OP), MULT);
															  (yyval.exp) = PARSE_CONTEXT.parse_type_dev_num_op(MULT, &(yysemantic_stack_[(3) - (1)].exp), &(yysemantic_stack_[(3) - (3)].exp)); ;}
    break;

  case 195:
#line 1067 "src/cmpl/CMPL_Parser.yy"
    { COMP_SUB(NUM_OP, DIVD);
															  (yyval.exp) = PARSE_CONTEXT.parse_type_dev_num_op(DIVD, &(yysemantic_stack_[(3) - (1)].exp), &(yysemantic_stack_[(3) - (3)].exp)); ;}
    break;

  case 196:
#line 1069 "src/cmpl/CMPL_Parser.yy"
    { COMP_SUB(NUM_OP, POW);
															  (yyval.exp) = PARSE_CONTEXT.parse_type_dev_num_op(POW, &(yysemantic_stack_[(3) - (1)].exp), &(yysemantic_stack_[(3) - (3)].exp)); ;}
    break;

  case 197:
#line 1071 "src/cmpl/CMPL_Parser.yy"
    { COMP_SUB(NUM_OP, DIVI);
															  (yyval.exp) = PARSE_CONTEXT.parse_type_dev_num_op(DIVI, &(yysemantic_stack_[(3) - (1)].exp), &(yysemantic_stack_[(3) - (3)].exp)); ;}
    break;

  case 198:
#line 1073 "src/cmpl/CMPL_Parser.yy"
    { COMP_SUB(NUM_OP, MODI);
															  (yyval.exp) = PARSE_CONTEXT.parse_type_dev_num_op(MODI, &(yysemantic_stack_[(3) - (1)].exp), &(yysemantic_stack_[(3) - (3)].exp)); ;}
    break;

  case 199:
#line 1075 "src/cmpl/CMPL_Parser.yy"
    { COMP_SUB(NUM_OP, MODI);
															  (yyval.exp) = PARSE_CONTEXT.parse_type_dev_num_op(MODI, &(yysemantic_stack_[(3) - (1)].exp), &(yysemantic_stack_[(3) - (3)].exp)); ;}
    break;

  case 200:
#line 1078 "src/cmpl/CMPL_Parser.yy"
    { COMP_SUB(SET_OP, INTERSECT_RR);
															  (yyval.exp) = PARSE_CONTEXT.parse_type_dev_num_op(INTERSECT_RR, &(yysemantic_stack_[(3) - (1)].exp), &(yysemantic_stack_[(3) - (3)].exp)); ;}
    break;

  case 201:
#line 1080 "src/cmpl/CMPL_Parser.yy"
    { EXPSTR t;
															  t.set(TPP_EMPTY, 0);
															  COMP_SUB(NUM_OP, NEG);
															  (yyval.exp) = PARSE_CONTEXT.parse_type_dev_num_op(NEG, &(yysemantic_stack_[(2) - (2)].exp), &t); ;}
    break;

  case 202:
#line 1084 "src/cmpl/CMPL_Parser.yy"
    { EXPSTR t;
															  t.set(TPP_EMPTY, 0);
															  COMP_SUB_PAR(CONV_OP, NO_FORMAT, TPU_NUM);	/* fuer sichern nummerischer Wert */
															  (yyval.exp) = PARSE_CONTEXT.parse_type_dev_num_op(NEG, &(yysemantic_stack_[(2) - (2)].exp), &t); ;}
    break;

  case 203:
#line 1100 "src/cmpl/CMPL_Parser.yy"
    { if ((yysemantic_stack_[(3) - (1)].exp).dim_t_undef > 0 || (yysemantic_stack_[(3) - (3)].exp).dim_t_undef > 0)
																PARSE_CONTEXT.output_error_parse(ERROR_TYPE_COMP, ERROR_LVL_NORMAL, "undefined symbol in operation");/*TODO*/
															  COMP_SUB(LOG_OP, AND);
															  (yyval.exp).set(TP_BIN, 0); ;}
    break;

  case 204:
#line 1104 "src/cmpl/CMPL_Parser.yy"
    { if ((yysemantic_stack_[(3) - (1)].exp).dim_t_undef > 0 || (yysemantic_stack_[(3) - (3)].exp).dim_t_undef > 0)
																PARSE_CONTEXT.output_error_parse(ERROR_TYPE_COMP, ERROR_LVL_NORMAL, "undefined symbol in operation");/*TODO*/
															  COMP_SUB(LOG_OP, OR);
															  (yyval.exp).set(TP_BIN, 0); ;}
    break;

  case 205:
#line 1111 "src/cmpl/CMPL_Parser.yy"
    { COMP_SUB(CMP_OP, EQ);
															  (yyval.exp) = PARSE_CONTEXT.parse_type_dev_cmp_op(EQ, &(yysemantic_stack_[(3) - (1)].exp), &(yysemantic_stack_[(3) - (3)].exp)); ;}
    break;

  case 206:
#line 1113 "src/cmpl/CMPL_Parser.yy"
    { COMP_SUB(CMP_OP, EQ2);
															  (yyval.exp) = PARSE_CONTEXT.parse_type_dev_cmp_op(EQ2, &(yysemantic_stack_[(3) - (1)].exp), &(yysemantic_stack_[(3) - (3)].exp)); ;}
    break;

  case 207:
#line 1115 "src/cmpl/CMPL_Parser.yy"
    { COMP_SUB(CMP_OP, EQ); COMP_SUB(LOG_OP, NOT);
															  (yyval.exp) = PARSE_CONTEXT.parse_type_dev_cmp_op(EQ, &(yysemantic_stack_[(3) - (1)].exp), &(yysemantic_stack_[(3) - (3)].exp)); ;}
    break;

  case 208:
#line 1117 "src/cmpl/CMPL_Parser.yy"
    { COMP_SUB(CMP_OP, EQ2); COMP_SUB(LOG_OP, NOT);
															  (yyval.exp) = PARSE_CONTEXT.parse_type_dev_cmp_op(EQ2, &(yysemantic_stack_[(3) - (1)].exp), &(yysemantic_stack_[(3) - (3)].exp)); ;}
    break;

  case 209:
#line 1119 "src/cmpl/CMPL_Parser.yy"
    { COMP_SUB(CMP_OP, LE);
															  (yyval.exp) = PARSE_CONTEXT.parse_type_dev_cmp_op(LE, &(yysemantic_stack_[(3) - (1)].exp), &(yysemantic_stack_[(3) - (3)].exp)); ;}
    break;

  case 210:
#line 1121 "src/cmpl/CMPL_Parser.yy"
    { if (TYPECLASS((yysemantic_stack_[(3) - (1)].exp).tp) == TPK_SET)
																{ COMP_SUB(CMP_OP, GT); }	/*TODO: GT auch fuer TPK_VALUE*/
															  else
																{ COMP_SUB(CMP_OP, LE); COMP_SUB(LOG_OP, NOT); }
															  (yyval.exp) = PARSE_CONTEXT.parse_type_dev_cmp_op(GT, &(yysemantic_stack_[(3) - (1)].exp), &(yysemantic_stack_[(3) - (3)].exp)); ;}
    break;

  case 211:
#line 1126 "src/cmpl/CMPL_Parser.yy"
    { COMP_SUB(CMP_OP, GE);
															  (yyval.exp) = PARSE_CONTEXT.parse_type_dev_cmp_op(GE, &(yysemantic_stack_[(3) - (1)].exp), &(yysemantic_stack_[(3) - (3)].exp)); ;}
    break;

  case 212:
#line 1128 "src/cmpl/CMPL_Parser.yy"
    { if (TYPECLASS((yysemantic_stack_[(3) - (1)].exp).tp) == TPK_SET)
																{ COMP_SUB(CMP_OP, LT); }	/*TODO: LT auch fuer TPK_VALUE*/
															  else
																{ COMP_SUB(CMP_OP, GE); COMP_SUB(LOG_OP, NOT); }
															  (yyval.exp) = PARSE_CONTEXT.parse_type_dev_cmp_op(LT, &(yysemantic_stack_[(3) - (1)].exp), &(yysemantic_stack_[(3) - (3)].exp)); ;}
    break;

  case 213:
#line 1133 "src/cmpl/CMPL_Parser.yy"
    { COMP_SUB(CMP_OP, INSET);
															  (yyval.exp) = PARSE_CONTEXT.parse_type_dev_cmp_op(INSET, &(yysemantic_stack_[(3) - (1)].exp), &(yysemantic_stack_[(3) - (3)].exp)); ;}
    break;

  case 214:
#line 1160 "src/cmpl/CMPL_Parser.yy"
    { if ((yysemantic_stack_[(1) - (1)].exp).dim_t_undef > 0)
													  			PARSE_CONTEXT.output_error_parse(ERROR_TYPE_COMP, ERROR_LVL_NORMAL, "undefined symbol");/*TODO*/
			  												  COMP_SUB(NUM_OP, ADD);
												 			  (yyval.exp) = (yysemantic_stack_[(1) - (1)].exp); ;}
    break;

  case 215:
#line 1167 "src/cmpl/CMPL_Parser.yy"
    { (yyval.exp) = PARSE_CONTEXT.parse_type_dev_switch(&(yysemantic_stack_[(3) - (2)].exp), &(yysemantic_stack_[(3) - (3)].exp)); ;}
    break;

  case 216:
#line 1168 "src/cmpl/CMPL_Parser.yy"
    { (yyval.exp).set(TPP_EMPTY, 0); ;}
    break;

  case 217:
#line 1171 "src/cmpl/CMPL_Parser.yy"
    { if ((yysemantic_stack_[(1) - (1)].exp).dim_t_undef > 0)
													  			PARSE_CONTEXT.output_error_parse(ERROR_TYPE_COMP, ERROR_LVL_NORMAL, "undefined symbol");/*TODO*/
															  COMP_SUB(COUNT_OP, CNT_INC);
												 			  (yyval.exp) = (yysemantic_stack_[(1) - (1)].exp); ;}
    break;

  case 218:
#line 1178 "src/cmpl/CMPL_Parser.yy"
    { (yyval.exp) = PARSE_CONTEXT.parse_type_dev_switch(&(yysemantic_stack_[(3) - (2)].exp), &(yysemantic_stack_[(3) - (3)].exp)); ;}
    break;

  case 219:
#line 1179 "src/cmpl/CMPL_Parser.yy"
    { (yyval.exp).set(TPP_EMPTY, 0); ;}
    break;

  case 220:
#line 1198 "src/cmpl/CMPL_Parser.yy"
    { // TODO: Test ob Indexanzahl passt: $2.dim_in == 0 || $1.dim_out == $2.dim_in
		                                      //        Wenn $2.dim_in == 0, dann COMP fuer freies Indexinfo; setzen $2.dim_in = $1.dim_out
											  (yyval.exp) = (yysemantic_stack_[(2) - (1)].exp);
											  (yyval.exp).sym_dim = 0;
											  if ((yysemantic_stack_[(2) - (2)].exp).dim_in > 0)
											  {
											  	(yyval.exp).set_dim((yysemantic_stack_[(2) - (2)].exp).dim_in, (yysemantic_stack_[(2) - (2)].exp).dim_out, (yysemantic_stack_[(2) - (2)].exp).dim_fr_end);
												(yyval.exp).sym_dim = (yysemantic_stack_[(2) - (2)].exp).dim_t_rank1;
											  }
		// DEFS_VALUE d_vekt		{
											  COMP_PAR(LIT_VAL_SHORT, (yysemantic_stack_[(2) - (2)].exp).dim_in);
											  COMP_SUB(SET_INDEXES, IND_GIVEN);
											  //GLV->index_info.i_anz_ind = $2.dim_in;
											  //GLV->index_info.i_free_ind = ($2.dim_out >= 0 ? $2.dim_out : 0);
											  /*$$.set((void*)$1, TPK_VALUE, 0, true, $2.dim_in, $2.dim_out, $2.dim_fr_end);*/ ;}
    break;

  case 221:
#line 1228 "src/cmpl/CMPL_Parser.yy"
    { (yyval.exp) = (yysemantic_stack_[(2) - (1)].exp);
											  (yyval.exp).sym_dim = 0;
											  if ((yysemantic_stack_[(2) - (2)].exp).dim_in > 0) {
											  	(yyval.exp).set_dim((yysemantic_stack_[(2) - (2)].exp).dim_in, (yysemantic_stack_[(2) - (2)].exp).dim_out, (yysemantic_stack_[(2) - (2)].exp).dim_fr_end);
												(yyval.exp).sym_dim = (yysemantic_stack_[(2) - (2)].exp).dim_t_rank1;
											  }
											  COMP_PAR_INT(LIT_VAL, (yysemantic_stack_[(2) - (2)].exp).dim_in);	//Anzahl muss hier auf den Stack, damit Dimensionalität geprüft werden kann
											  COMP_SUB_PAR_CNT(FETCH_CONST, SYM_MAIN, 0, 1);
											  //COMP_INT($$.symp()->symno * MAX_ANZ_IND_1 + $$.dim_in);
											  COMP_INT((yyval.exp).symp()->symno * MAX_ANZ_IND_1 + (yyval.exp).sym_dim);
											;}
    break;

  case 222:
#line 1253 "src/cmpl/CMPL_Parser.yy"
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

  case 223:
#line 1282 "src/cmpl/CMPL_Parser.yy"
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

  case 224:
#line 1302 "src/cmpl/CMPL_Parser.yy"
    { // Auf einfachen skalaren Funktionsparameter prüfen
													  if ((yysemantic_stack_[(1) - (1)].exp).dim_in != 1 && (yysemantic_stack_[(1) - (1)].exp).dim_elem != 0)
														PARSE_CONTEXT.output_error_parse(ERROR_TYPE_COMP, ERROR_LVL_NORMAL, "function argument must be a single scalar value");/*TODO*/
															//TODO: Prüfung besser in der Regel der Funktion, damit Fehlermeldung Funktionsname zeigen kann
													  (yyval.exp) = (yysemantic_stack_[(1) - (1)].exp);
													  (yyval.exp).set_dim(1, 0, 0, 0); ;}
    break;

  case 225:
#line 1310 "src/cmpl/CMPL_Parser.yy"
    { // Alle Listenelemente müssen skalar sein
													  if ((yysemantic_stack_[(3) - (2)].exp).dim_elem != 0)
														PARSE_CONTEXT.output_error_parse(ERROR_TYPE_COMP, ERROR_LVL_NORMAL, "index values must be scalar");/*TODO*/
													  (yyval.exp) = (yysemantic_stack_[(3) - (2)].exp);
													  // Undefinierte Elemente nur erlaubt, wenn potentieller lvalue
													  if ((yysemantic_stack_[(3) - (2)].exp).dim_t_undef > 0 && !(yysemantic_stack_[(3) - (2)].exp).lval)
													  	PARSE_CONTEXT.output_error_parse(ERROR_TYPE_COMP, ERROR_LVL_NORMAL, "undefined symbols in list");/*TODO*/
													;}
    break;

  case 226:
#line 1318 "src/cmpl/CMPL_Parser.yy"
    { (yyval.exp).tp = TPP_NULL;
													  (yyval.exp).set_dim(0, 0, 0, 0);
													  (yyval.exp).lval = false; ;}
    break;

  case 227:
#line 1323 "src/cmpl/CMPL_Parser.yy"
    { (yyval.exp) = (yysemantic_stack_[(1) - (1)].exp); ;}
    break;

  case 228:
#line 1324 "src/cmpl/CMPL_Parser.yy"
    { (yyval.exp) = PARSE_CONTEXT.parse_type_dev_array(&(yysemantic_stack_[(3) - (3)].exp), &(yysemantic_stack_[(3) - (1)].exp));	/*TODO: in parse_type_dev_array dim-Bestimmung einbeziehen */
													  (yyval.exp).set_dim((yysemantic_stack_[(3) - (1)].exp).dim_in + (yysemantic_stack_[(3) - (3)].exp).dim_in, (yysemantic_stack_[(3) - (1)].exp).dim_out + (yysemantic_stack_[(3) - (3)].exp).dim_out,
														(yysemantic_stack_[(3) - (3)].exp).dim_fr_end + ((yysemantic_stack_[(3) - (3)].exp).dim_fr_end==(yysemantic_stack_[(3) - (3)].exp).dim_in ? (yysemantic_stack_[(3) - (1)].exp).dim_fr_end : 0),
														((yysemantic_stack_[(3) - (1)].exp).dim_elem==(yysemantic_stack_[(3) - (3)].exp).dim_elem || (yysemantic_stack_[(3) - (3)].exp).dim_fr_end==(yysemantic_stack_[(3) - (3)].exp).dim_in ? (yysemantic_stack_[(3) - (1)].exp).dim_elem :
															((yysemantic_stack_[(3) - (1)].exp).dim_fr_end==(yysemantic_stack_[(3) - (1)].exp).dim_in ? (yysemantic_stack_[(3) - (3)].exp).dim_elem : -1)),
														(yysemantic_stack_[(3) - (1)].exp).dim_t_rank1 + (yysemantic_stack_[(3) - (3)].exp).dim_t_rank1, ((yysemantic_stack_[(3) - (1)].exp).dim_t_rank2 == (yysemantic_stack_[(3) - (3)].exp).dim_t_rank2 ? (yysemantic_stack_[(3) - (1)].exp).dim_t_rank2 : -1), (yysemantic_stack_[(3) - (1)].exp).dim_t_undef + (yysemantic_stack_[(3) - (3)].exp).dim_t_undef);
													  (yyval.exp).lval = ((yysemantic_stack_[(3) - (1)].exp).lval && (yysemantic_stack_[(3) - (3)].exp).lval); ;}
    break;

  case 229:
#line 1333 "src/cmpl/CMPL_Parser.yy"
    { COMP_PAR(LIT_SET, TP_SET_ALL);
                                                      (yyval.exp).tp = TPP_EMPTY;
													  (yyval.exp).set_dim(1, 1, 1, 0, 1, 1, 0);
													  (yyval.exp).lval = false; ;}
    break;

  case 230:
#line 1337 "src/cmpl/CMPL_Parser.yy"
    { // Undefinierte Symbole hier nicht erlaubt
													  if ((yysemantic_stack_[(1) - (1)].exp).dim_t_undef > 0)
													  	PARSE_CONTEXT.output_error_parse(ERROR_TYPE_COMP, ERROR_LVL_NORMAL, "undefined symbols in list");/*TODO*/
													  (yyval.exp) = (yysemantic_stack_[(1) - (1)].exp);
													  (yyval.exp).set_dim(1, ((yysemantic_stack_[(1) - (1)].exp).dim_out==0 && (TYPECLASS((yysemantic_stack_[(1) - (1)].exp).tp)==TPK_SET || TP_IS_TUPELSET((yysemantic_stack_[(1) - (1)].exp).tp)) ? (TP_IS_TUPELSET((yysemantic_stack_[(1) - (1)].exp).tp) ? RANK_TP_TUPEL((yysemantic_stack_[(1) - (1)].exp).tp) : 1) : 0), 0, (yysemantic_stack_[(1) - (1)].exp).dim_out,
													  	(TP_IS_TUPEL((yysemantic_stack_[(1) - (1)].exp).tp) || TP_IS_TUPELSET((yysemantic_stack_[(1) - (1)].exp).tp) ? RANK_TP_TUPEL((yysemantic_stack_[(1) - (1)].exp).tp) : 1),
														(TP_IS_TUPEL((yysemantic_stack_[(1) - (1)].exp).tp) || TP_IS_TUPELSET((yysemantic_stack_[(1) - (1)].exp).tp) ? RANK_TP_TUPEL((yysemantic_stack_[(1) - (1)].exp).tp) : 1), 0); ;}
    break;

  case 231:
#line 1344 "src/cmpl/CMPL_Parser.yy"
    { (yyval.exp) = (yysemantic_stack_[(1) - (1)].exp);
													  (yyval.exp).set_dim(1, 0, 0, 0, 1, -1, 1);
													  // Compilieren vorläufigen Zugriff auf Symbol; muss später ersetzt werden
											  		  COMP_SUB_PAR_CNT(FETCH_CONST, SYM_UNDEF_ERROR, 0, 1);
											  		  COMP_INT((yyval.exp).symp()->symno * MAX_ANZ_IND_1); ;}
    break;

  case 232:
#line 1354 "src/cmpl/CMPL_Parser.yy"
    { if ((yysemantic_stack_[(1) - (1)].exp).dim_t_undef > 0)
														PARSE_CONTEXT.output_error_parse(ERROR_TYPE_COMP, ERROR_LVL_NORMAL, "undefined symbol");/*TODO*/
													  (yyval.exp) = (yysemantic_stack_[(1) - (1)].exp); ;}
    break;

  case 233:
#line 1357 "src/cmpl/CMPL_Parser.yy"
    { (yyval.exp).set(TPK_VALUE, -1);
													  if (TYPECLASS((yysemantic_stack_[(2) - (2)].exp).tp) != TPK_VALUE)
														PARSE_CONTEXT.output_error_parse(ERROR_TYPE_COMP, ERROR_LVL_NORMAL, "wrong argument type for function");/*TODO*/
													  COMP(READ_CSV, 1);	//TODO: READ_CSV verwendet eigenständigen Parameter statt bisher eingebetteten
													  (yyval.exp).addr_dim = ADDR_COMP; 
													  COMP_INT(0); ;}
    break;

  case 234:
#line 1396 "src/cmpl/CMPL_Parser.yy"
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

  case 235:
#line 1435 "src/cmpl/CMPL_Parser.yy"
    { COMP_PAR(LIT_SET, TP_SET_ALL_NUM);
													  (yyval.exp).set(TP_SET_ALL_NUM, 0); ;}
    break;

  case 236:
#line 1437 "src/cmpl/CMPL_Parser.yy"
    { COMP_PAR(LIT_SET, TP_SET_ALL_STR);
													  (yyval.exp).set(TP_SET_ALL_STR, 0); ;}
    break;

  case 237:
#line 1439 "src/cmpl/CMPL_Parser.yy"
    { COMP_PAR(LIT_SET, TP_SET_ALL);
													  (yyval.exp).set(TP_SET_ALL, 0); ;}
    break;

  case 238:
#line 1441 "src/cmpl/CMPL_Parser.yy"
    { EXPSTR n;
													  n.set(TPP_NULL, 0);
													  (yyval.exp) = PARSE_CONTEXT.parse_type_dev_lit_set(TP_SET_HALF_UB, &(yysemantic_stack_[(2) - (2)].exp), &n, &n);
													  COMP_PAR(LIT_SET, TP_SET_HALF_UB); ;}
    break;

  case 239:
#line 1445 "src/cmpl/CMPL_Parser.yy"
    { EXPSTR n;
													  n.set(TPP_NULL, 0);
													  (yyval.exp) = PARSE_CONTEXT.parse_type_dev_lit_set(TP_SET_HALF_LB, &(yysemantic_stack_[(2) - (1)].exp), &n, &n);
													  COMP_PAR(LIT_SET, TP_SET_HALF_LB); ;}
    break;

  case 240:
#line 1450 "src/cmpl/CMPL_Parser.yy"
    { EXPSTR t;
													  t.set(TP_INT, 0);
													  (yyval.exp) = PARSE_CONTEXT.parse_type_dev_lit_set(TP_SET_ALG, &(yysemantic_stack_[(3) - (1)].exp), &t, &(yysemantic_stack_[(3) - (3)].exp));
													  COMP_PAR(LIT_SET, TP_SET_ALG_S1); ;}
    break;

  case 241:
#line 1455 "src/cmpl/CMPL_Parser.yy"
    { (yyval.exp) = PARSE_CONTEXT.parse_type_dev_lit_set(TP_SET_ALG, &(yysemantic_stack_[(5) - (1)].exp), &(yysemantic_stack_[(5) - (3)].exp), &(yysemantic_stack_[(5) - (5)].exp));
													  COMP_PAR(LIT_SET, TP_SET_ALG); ;}
    break;

  case 242:
#line 1459 "src/cmpl/CMPL_Parser.yy"
    { /*COMP_PAR(LIT_VAL_SHORT, $3.dim_in);
													  COMP_SUB(SET_INDEXES, IND_GIVEN);*/
													  COMP(DEFSET_FUNC, 1);
													  COMP_INT((yysemantic_stack_[(4) - (3)].exp).symp()->symno * MAX_ANZ_IND_1 + (yysemantic_stack_[(4) - (3)].exp).sym_dim);
													  (yyval.exp).set(TPK_SET, 0); ;}
    break;

  case 243:
#line 1469 "src/cmpl/CMPL_Parser.yy"
    { (yyval.exp) = (yysemantic_stack_[(1) - (1)].exp); ;}
    break;

  case 244:
#line 1470 "src/cmpl/CMPL_Parser.yy"
    { COMP_SUB(CONSTRUCT_TPS_REV, TPS_IND_FREE);
													  (yyval.exp).set(TPK_TUPELSET_N(TP_IS_TUPEL((yysemantic_stack_[(2) - (2)].exp).tp) || TP_IS_TUPELSET((yysemantic_stack_[(2) - (2)].exp).tp) ? RANK_TP_TUPEL((yysemantic_stack_[(2) - (2)].exp).tp) : 1), 0); ;}
    break;

  case 245:
#line 1472 "src/cmpl/CMPL_Parser.yy"
    { COMP_SUB(CONSTRUCT_TPS_REV, TPS_IND_NON_FREE);
													  (yyval.exp).set(TPK_TUPELSET_N(TP_IS_TUPEL((yysemantic_stack_[(2) - (2)].exp).tp) || TP_IS_TUPELSET((yysemantic_stack_[(2) - (2)].exp).tp) ? RANK_TP_TUPEL((yysemantic_stack_[(2) - (2)].exp).tp) : 1), 0); ;}
    break;

  case 246:
#line 1476 "src/cmpl/CMPL_Parser.yy"
    { COMP_PAR(LIT_SET, TP_SET_ALL);
													  (yyval.exp).set(TP_SET_ALL, 0); ;}
    break;

  case 247:
#line 1478 "src/cmpl/CMPL_Parser.yy"
    { (yyval.exp) = (yysemantic_stack_[(1) - (1)].exp); ;}
    break;

  case 248:
#line 1479 "src/cmpl/CMPL_Parser.yy"
    { (yyval.exp) = (yysemantic_stack_[(1) - (1)].exp); ;}
    break;

  case 249:
#line 1484 "src/cmpl/CMPL_Parser.yy"
    { (yyval.i) = EQ; ;}
    break;

  case 250:
#line 1485 "src/cmpl/CMPL_Parser.yy"
    { (yyval.i) = GE; ;}
    break;

  case 251:
#line 1486 "src/cmpl/CMPL_Parser.yy"
    { (yyval.i) = LE; ;}
    break;

  case 252:
#line 1491 "src/cmpl/CMPL_Parser.yy"
    { (yyval.exp) = (yysemantic_stack_[(1) - (1)].exp);
													  (yyval.exp).tp = TPK_VALUE;
													  if (!((yysemantic_stack_[(1) - (1)].exp).symp()->isvalid()))
														PARSE_CONTEXT.init_symbol(&(yysemantic_stack_[(1) - (1)].exp), PTR_TP_LINENAME, TPK_VALUE, 0, false); ;}
    break;

  case 253:
#line 1495 "src/cmpl/CMPL_Parser.yy"
    { (yyval.exp) = (yysemantic_stack_[(1) - (1)].exp);
													  (yyval.exp).tp = TPK_VALUE;
													  PARSE_CONTEXT.init_symbol(&(yysemantic_stack_[(1) - (1)].exp), PTR_TP_LINENAME, TPK_VALUE, 0, false); ;}
    break;

  case 254:
#line 1500 "src/cmpl/CMPL_Parser.yy"
    { COMP(LINE_BEG, 1);
													  PARSE_CONTEXT.linename_repl(GLV->last_str, (char *)0);
													  COMP_STR(GLV->last_str); ;}
    break;

  case 255:
#line 1503 "src/cmpl/CMPL_Parser.yy"
    { COMP(LINE_BEG, 1);
													  PARSE_CONTEXT.linename_repl(GLV->last_str, (yysemantic_stack_[(2) - (1)].exp).symp()->s_name);
													  COMP_STR(GLV->last_str);
													  // TODO: auch resultierende Zeilennamen als Symbol einfuegen? (bzw. in linename_repl()?)
													;}
    break;

  case 256:
#line 1514 "src/cmpl/CMPL_Parser.yy"
    { /* Naechsten loop-Kontexte initialisieren, als Basis */
												  PARSE_CONTEXT.init_loop_comp(GLV->tmp_level, true, LOOP_EXEC, ((yysemantic_stack_[(2) - (1)].exp).tp==TPP_EMPTY ? (char *)0 : (yysemantic_stack_[(2) - (1)].exp).symp()->s_name)); ;}
    break;

  case 257:
#line 1519 "src/cmpl/CMPL_Parser.yy"
    { /* Naechsten loop-Kontexte initialisieren, als Basis fuer Summe */
												  PARSE_CONTEXT.init_loop_comp(GLV->tmp_level, true, LOOP_SUM, (char *)0); ;}
    break;

  case 258:
#line 1523 "src/cmpl/CMPL_Parser.yy"
    { /* Naechsten loop-Kontexte initialisieren, als Basis fuer Set */
												  PARSE_CONTEXT.init_loop_comp(GLV->tmp_level, true, LOOP_SET, (char *)0); ;}
    break;

  case 259:
#line 1527 "src/cmpl/CMPL_Parser.yy"
    { (yyval.exp).set(TPP_EMPTY, 0); ;}
    break;

  case 260:
#line 1528 "src/cmpl/CMPL_Parser.yy"
    { (yyval.exp) = (yysemantic_stack_[(1) - (1)].exp); ;}
    break;

  case 261:
#line 1532 "src/cmpl/CMPL_Parser.yy"
    { /* Setzt loop-Kontexte bis einschliesslich naechstem Basiskontext zurueck; compiliert entsprechende Beendigungen */
												  PARSE_CONTEXT.unset_loop_comp(); ;}
    break;

  case 262:
#line 1537 "src/cmpl/CMPL_Parser.yy"
    { /* Test fuer Alternative compilieren */
												  loop_ctrl_str *loopp = GLV->tmp_table + (GLV->tmp_level - 1);
												  COMP_SUB_PAR_CNT(TEST_ALTER, TEST_ALTER_IF, GLV->tmp_level - 1, 1);
												  loopp->last_jump = COMP_ADDR(0); ;}
    break;

  case 263:
#line 1541 "src/cmpl/CMPL_Parser.yy"
    { /* Beginn Default-Alternative compilieren */
												  loop_ctrl_str *loopp = GLV->tmp_table + (GLV->tmp_level - 1);
												  COMP_SUB_PAR(TEST_ALTER, TEST_ALTER_DEF, GLV->tmp_level - 1);
												  loopp->last_jump = 0; ;}
    break;

  case 264:
#line 1547 "src/cmpl/CMPL_Parser.yy"
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

  case 268:
#line 1574 "src/cmpl/CMPL_Parser.yy"
    { /* loop-Kontext erzeugen (START_EMPTY als Platzhalter, wird gegebenenfalls spaeter ersetzt) */
												  loop_ctrl_str *loopp = PARSE_CONTEXT.set_loop_comp(START_EMPTY, (yysemantic_stack_[(1) - (1)].i));
												  loopp->loop_start = ADDR_COMP; ;}
    break;

  case 273:
#line 1599 "src/cmpl/CMPL_Parser.yy"
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

  case 274:
#line 1611 "src/cmpl/CMPL_Parser.yy"
    { (yyval.i) = (yysemantic_stack_[(2) - (1)].i) | (yysemantic_stack_[(2) - (2)].i); ;}
    break;

  case 275:
#line 1612 "src/cmpl/CMPL_Parser.yy"
    { (yyval.i) = 0; ;}
    break;

  case 276:
#line 1615 "src/cmpl/CMPL_Parser.yy"
    { (yyval.i) = HEAD_MOD_REF; ;}
    break;

  case 277:
#line 1616 "src/cmpl/CMPL_Parser.yy"
    { (yyval.i) = HEAD_MOD_WHILE; ;}
    break;

  case 278:
#line 1621 "src/cmpl/CMPL_Parser.yy"
    { if ((yysemantic_stack_[(1) - (1)].exp).dim_t_rank1 > 0) {
												  	  /* Linke Seite ist Tupel, dafür vorläufig compilierte Konstruktion entfernen und daraus Symbole holen */
													  int tupel_cnt;
													  int *symblock = PARSE_CONTEXT.tupel_assign_symbols(&tupel_cnt, (yysemantic_stack_[(1) - (1)].exp).dim_t_rank1);

													  /* Zur Übergabe weiter nach hinten in dieser Klausel in $1 speichern, dafuer andere Felder missbrauchen */
													  (yysemantic_stack_[(1) - (1)].exp).symtab = (SymbolTab *) symblock;
												  }
												;}
    break;

  case 279:
#line 1630 "src/cmpl/CMPL_Parser.yy"
    { /* Unterscheidung ob Iteration über Set oder lokale Definition */
												  //TODO: Typangabe erlauben
												  //TODO: Auch lokale Definition eines Sets erlauben
												  int assign = (yysemantic_stack_[(3) - (1)].exp).sym_dim;	// sym_dim hier entgegen des Namens fuer Art des Zuweisungsoperators verwendet
												  unsigned short sub = (assign == 2 || TYPECLASS((yysemantic_stack_[(3) - (3)].exp).tp) == TPK_SET || TP_IS_TUPELSET((yysemantic_stack_[(3) - (3)].exp).tp) ? START_LOOP : START_ASSIGN);
												  /* Art und Name der Laufkonstante eintragen */
												  if ((yysemantic_stack_[(3) - (1)].exp).dim_t_rank1 > 0)
												  {
												  	// Tupelrang muss passen
                                                  //	if ($1.dim_in != ((TP_IS_TUPEL($3.tp) || TP_IS_TUPELSET($3.tp)) ? RANK_TP_TUPEL($3.tp) : 1))
                                                    //	PARSE_CONTEXT.output_error_parse(ERROR_TYPE_COMP, ERROR_LVL_NORMAL, "rank of iteration tupel doesn't match");/*TODO*/
                                                    (yysemantic_stack_[(3) - (1)].exp).tp = TPK_TUPEL_N((yysemantic_stack_[(3) - (1)].exp).dim_in);
												  }
												  else
												  {
												  	(yysemantic_stack_[(3) - (1)].exp).tp = (sub == START_LOOP ? ((TP_IS_TUPEL((yysemantic_stack_[(3) - (3)].exp).tp) || TP_IS_TUPELSET((yysemantic_stack_[(3) - (3)].exp).tp)) ? TPK_TUPEL_N(RANK_TP_TUPEL((yysemantic_stack_[(3) - (3)].exp).tp)) : TPK_VALUE) : TYPECLASS((yysemantic_stack_[(3) - (3)].exp).tp));
												  }
												  PARSE_CONTEXT.set_loop_sname(sub, &(yysemantic_stack_[(3) - (1)].exp), ((yysemantic_stack_[(3) - (1)].exp).dim_t_rank1 > 0 ? (int *)(yysemantic_stack_[(3) - (1)].exp).symtab : (int *)NULL));
												  /* Wenn assign == 0 (alte Zuweisung): Wenn $2 != TPK_SET (einfacher Wert), dann Fehler; sonst Warnung */
												  if (assign == 0) PARSE_CONTEXT.out_warn_old_assign((TYPECLASS((yysemantic_stack_[(3) - (3)].exp).tp) != TPK_SET));
												;}
    break;

  case 280:
#line 1653 "src/cmpl/CMPL_Parser.yy"
    { (yyval.i) = 1; ;}
    break;

  case 281:
#line 1654 "src/cmpl/CMPL_Parser.yy"
    { (yyval.i) = 2; ;}
    break;

  case 282:
#line 1657 "src/cmpl/CMPL_Parser.yy"
    { (yyval.i) = 0; ;}
    break;

  case 283:
#line 1658 "src/cmpl/CMPL_Parser.yy"
    { (yyval.i) = (yysemantic_stack_[(1) - (1)].i); ;}
    break;

  case 284:
#line 1661 "src/cmpl/CMPL_Parser.yy"
    { if ((yysemantic_stack_[(1) - (1)].exp).dim_t_undef > 0)
													PARSE_CONTEXT.output_error_parse(ERROR_TYPE_COMP, ERROR_LVL_NORMAL, "undefined symbol");/*TODO*/
												  (yyval.exp) = (yysemantic_stack_[(1) - (1)].exp); ;}
    break;

  case 285:
#line 1666 "src/cmpl/CMPL_Parser.yy"
    { (yyval.exp) = (yysemantic_stack_[(2) - (1)].exp);
												  (yyval.exp).sym_dim = (yysemantic_stack_[(2) - (2)].i);	// sym_dim hier entgegen des Namens fuer Art des Zuweisungsoperators verwendet
												;}
    break;

  case 286:
#line 1669 "src/cmpl/CMPL_Parser.yy"
    { if (!(yysemantic_stack_[(2) - (1)].exp).lval)
													PARSE_CONTEXT.output_error_parse(ERROR_TYPE_COMP, ERROR_LVL_NORMAL, "iteration tupel are not a lvalue");
												  if ((yysemantic_stack_[(2) - (1)].exp).dim_t_undef != (yysemantic_stack_[(2) - (1)].exp).dim_in)
												  	PARSE_CONTEXT.output_error_parse(ERROR_TYPE_COMP, ERROR_LVL_NORMAL, "not all symbols in iteration tupel are undefined");
												  (yyval.exp) = (yysemantic_stack_[(2) - (1)].exp);
												  (yyval.exp).sym_dim = (yysemantic_stack_[(2) - (2)].i);	// sym_dim hier entgegen des Namens fuer Art des Zuweisungsoperators verwendet
												;}
    break;

  case 287:
#line 1679 "src/cmpl/CMPL_Parser.yy"
    { if ((yysemantic_stack_[(1) - (1)].exp).dim_t_undef > 0)
													PARSE_CONTEXT.output_error_parse(ERROR_TYPE_COMP, ERROR_LVL_NORMAL, "undefined symbol");/*TODO*/
												  PARSE_CONTEXT.set_loop_sname(START_IF, (EXPSTR *)NULL, (int *)NULL);
												  (yyval.exp) = (yysemantic_stack_[(1) - (1)].exp); ;}
    break;

  case 288:
#line 1686 "src/cmpl/CMPL_Parser.yy"
    { loop_ctrl_str *loopp = PARSE_CONTEXT.search_loop_name_mod((yysemantic_stack_[(2) - (2)].i_s).s, (yysemantic_stack_[(2) - (2)].i_s).i, "break", false);
												  if (loopp) { COMP_SUB_PAR_CNT(GOTO, GOTO_INDIREKT, 0, 1); COMP_ADDR(loopp->end_jump); } ;}
    break;

  case 289:
#line 1688 "src/cmpl/CMPL_Parser.yy"
    { loop_ctrl_str *loopp = PARSE_CONTEXT.search_loop_name_mod((yysemantic_stack_[(2) - (2)].i_s).s, (yysemantic_stack_[(2) - (2)].i_s).i, "continue", true);
												  if (loopp) { COMP_SUB_PAR_CNT(GOTO, GOTO_DIREKT, 0, 1); COMP_ADDR(loopp->iter_start); } ;}
    break;

  case 290:
#line 1690 "src/cmpl/CMPL_Parser.yy"
    { loop_ctrl_str *loopp = PARSE_CONTEXT.search_loop_name_mod((yysemantic_stack_[(2) - (2)].i_s).s, (yysemantic_stack_[(2) - (2)].i_s).i, "repeat", false);
												  if (loopp) { COMP_SUB_PAR_CNT(GOTO, GOTO_DIREKT, 0, 1); COMP_ADDR(loopp->loop_start); } ;}
    break;

  case 291:
#line 1694 "src/cmpl/CMPL_Parser.yy"
    { (yyval.i_s).s = (char *)0; (yyval.i_s).i = 0; ;}
    break;

  case 292:
#line 1695 "src/cmpl/CMPL_Parser.yy"
    { (yyval.i_s).s = (yysemantic_stack_[(1) - (1)].exp).symp()->s_name; (yyval.i_s).i = 0; ;}
    break;

  case 293:
#line 1696 "src/cmpl/CMPL_Parser.yy"
    { (yyval.i_s).s = (yysemantic_stack_[(1) - (1)].exp).symp()->s_name; /*GLV->last_str;*/ (yyval.i_s).i = 0; ;}
    break;

  case 294:
#line 1697 "src/cmpl/CMPL_Parser.yy"
    { (yyval.i_s).s = (char *)0; (yyval.i_s).i = (yysemantic_stack_[(1) - (1)].i); ;}
    break;

  case 295:
#line 1702 "src/cmpl/CMPL_Parser.yy"
    { if ((yysemantic_stack_[(3) - (3)].exp).dim_t_undef > 0)
													PARSE_CONTEXT.output_error_parse(ERROR_TYPE_COMP, ERROR_LVL_NORMAL, "undefined symbol");/*TODO*/
												  COMP(STORE_TMP, 1);
												  //COMP_INT((-$1-2));
												  COMP_INT((yysemantic_stack_[(3) - (1)].exp).symp()->symno); ;}
    break;

  case 296:
#line 1716 "src/cmpl/CMPL_Parser.yy"
    { if ((yysemantic_stack_[(2) - (2)].exp).dim_t_undef > 0)
											  	PARSE_CONTEXT.output_error_parse(ERROR_TYPE_COMP, ERROR_LVL_NORMAL, "undefined symbol");/*TODO*/
		   									  COMP_SUB(OUTPUT_MSG, ERROR_LVL_FATAL); ;}
    break;

  case 297:
#line 1719 "src/cmpl/CMPL_Parser.yy"
    { if ((yysemantic_stack_[(2) - (2)].exp).dim_t_undef > 0)
											  	PARSE_CONTEXT.output_error_parse(ERROR_TYPE_COMP, ERROR_LVL_NORMAL, "undefined symbol");/*TODO*/
											  COMP_SUB(OUTPUT_MSG, USR_MSG_NL); ;}
    break;


    /* Line 675 of lalr1.cc.  */
#line 2190 "src/cmpl/_CMPL_Parser.cc"
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
  const short int CMPL_Parser::yypact_ninf_ = -400;
  const short int
  CMPL_Parser::yypact_[] =
  {
       256,  -400,  -400,  -400,  -400,    57,   256,  -400,  -400,  -400,
    -400,    -7,    11,    22,    26,  -400,  -400,   986,  1187,  1057,
    1128,   -34,  1670,  1670,  1620,  1620,  1670,  1670,   -47,   -50,
    -400,  -400,    92,  -400,  -400,  -400,    29,    44,    52,    59,
      59,    70,    97,    30,   113,   137,   141,   153,    44,  -400,
    -400,  -400,  -400,    -4,  -400,  1620,  1620,  1300,    46,    46,
      46,  -400,  -400,  1232,  1353,  -400,   491,   136,  -400,   175,
    -400,   163,  -400,   233,  -400,  -400,  -400,  -400,   238,   254,
     728,  -400,  -400,  -400,   255,  -400,  -400,  -400,  -400,    91,
    -400,  -400,   205,   213,   214,   216,   218,   -50,  -400,   701,
     220,   219,  -400,  -400,  -400,    91,   223,   224,   227,   228,
    -400,  -400,   570,  -400,   230,  -400,    91,   236,   242,   243,
     246,  -400,   642,  -400,    60,    91,   247,   250,   253,  -400,
    -400,  -400,   728,  -400,  -400,  -400,  -400,   182,  -400,  -400,
    -400,  1620,  1620,  1353,  -400,  1620,  -400,  -400,  -400,    86,
     131,  -400,  1620,  1620,  1406,    86,  -400,  -400,  -400,   127,
    1220,  -400,  -400,  -400,  -400,  -400,   -50,   136,  -400,  -400,
    -400,  -400,  -400,  -400,  -400,  -400,  -400,   235,  -400,   259,
     232,   251,  -400,   211,    85,  1620,  -400,   116,  1670,  1670,
    1670,  1670,  1670,  1670,  1670,  1670,  1670,  1670,  1620,   764,
     258,   264,  -400,    91,    91,    91,  -400,  -400,  -400,  -400,
    -400,  -400,  -400,    87,   148,   393,  -400,  -400,  -400,  -400,
    -400,  -400,    60,   842,  -400,  -400,  -400,  -400,  -400,   284,
     284,   266,    73,   170,  -400,  -400,  -400,  -400,    91,   914,
    -400,  -400,  -400,   491,   127,    32,   262,    50,  1754,  1754,
    -400,  -400,   268,   -50,   -50,  -400,  -400,   273,   275,    63,
     -50,   276,   277,   278,  1620,  1620,  1670,  1670,  1670,  1670,
    1670,  1670,  1670,  1670,  1670,    85,  -400,  -400,  1353,  -400,
    -400,  -400,  -400,  -400,   279,  -400,  -400,   -50,   171,   171,
     340,   340,   340,   340,   340,   340,   340,   182,    61,   281,
    -400,    91,  1459,  -400,  1620,  1620,   701,   285,   285,  -400,
     286,  -400,   281,   570,   303,   281,   642,    91,  -400,  -400,
    -400,  -400,  -400,  -400,  -400,  -400,   170,  -400,  -400,  1712,
    -400,    60,   281,  -400,  -400,  -400,  -400,   -50,   233,  -400,
     341,   288,   293,  -400,   290,  -400,  -400,  -400,  -400,  -400,
    1620,  -400,  -400,  -400,  -400,   370,  -400,   182,   182,   182,
     182,   182,   182,   182,   182,   182,  -400,  -400,  1512,  -400,
    -400,  1670,  -400,   291,   764,  1565,  -400,    96,   127,   209,
    -400,  -400,  -400,  -400,  -400,   281,  -400,   281,  -400,  1670,
    -400,  -400,  -400,   291,   393,  -400,   181,   297,   291,   842,
    -400,    60,    91,    60,  -400,  -400,   -50,  -400,   360,  -400,
     313,   313,   281,   291,   914,   352,   116,  -400,  -400,  -400,
    -400,   298,  1512,    44,  -400,   304,   182,  -400,  -400,   281,
     307,   308,  -400,  -400,  -400,  -400,  -400,  -400,  -400,  1620,
     291,  1620,   291,  1620,   364,   182,  -400,   281,  -400,  -400,
    -400,  -400,   281,   281,    60,  -400,  -400,    73,  -400,  1712,
    -400,  -400,  -400,   291,    60,  -400,   281,   -50,  1754,  -400,
     316,  -400,  -400,  -400,  -400,  -400,  -400,  -400,  -400,  -400,
     281,  -400,   281,  1670,  -400,  -400,   291,   281,  -400,  -400,
     281,  -400,   369,  -400,  -400,  -400,  -400,   317,  -400,   291,
    -400,  -400,  -400
  };

  /* YYDEFACT[S] -- default rule to reduce with in state S when YYTABLE
     doesn't specify something else to do.  Zero means the default is an
     error.  */
  const unsigned short int
  CMPL_Parser::yydefact_[] =
  {
         2,     8,    40,    64,    82,     0,     2,     4,     5,     6,
       7,     0,     0,     0,     0,     1,     3,     0,     0,     0,
       0,     0,     0,     0,   246,   246,     0,   237,   173,   173,
     252,   132,     0,   133,   135,   134,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    29,
      30,    31,    32,     0,    33,     0,     0,     0,   291,   291,
     291,   235,   236,   229,   229,     9,     0,    27,    24,     0,
      18,     0,    23,   181,   127,   131,   128,   129,   184,     0,
      25,   185,   243,   182,   183,   130,   136,    26,   260,   275,
     188,   190,     0,     0,     0,     0,     0,   173,    41,     0,
       0,    51,    53,    47,    54,   275,     0,     0,     0,     0,
     253,    65,     0,    72,   260,    70,   275,     0,     0,     0,
       0,    83,     0,    89,   101,   275,     0,     0,     0,    12,
     202,   201,   247,   248,   244,   245,   186,   238,    60,   174,
     221,     0,     0,   229,   150,     0,   157,   140,   138,     0,
       0,   257,     0,     0,     0,     0,    39,   258,   234,   187,
     177,   296,   178,   179,   180,   297,   173,    27,   292,   293,
     277,   276,   294,   288,   289,   290,   231,     0,   230,     0,
     227,     0,    11,     0,     0,     0,    22,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   239,     0,     0,
       0,   266,   268,   275,   275,   275,   256,    19,    20,    21,
      44,   159,    43,     0,     0,     0,    48,    49,    50,    68,
      67,   255,   101,     0,    73,    74,    75,    86,    85,   102,
     103,     0,     0,   104,   115,   115,   100,   109,   275,     0,
      90,    91,    92,     0,   295,     0,     0,     0,     0,     0,
     168,   169,     0,   173,   173,   172,   170,     0,     0,     0,
     173,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   226,   225,   229,   222,
      28,    34,    35,    14,     0,   155,   156,   173,   192,   193,
     194,   195,   199,   200,   198,   197,   196,   240,     0,    38,
     265,   275,   270,   274,     0,     0,     0,    58,    58,    57,
       0,    52,    63,     0,    76,    81,     0,   275,   111,    88,
     249,   251,   250,    93,   115,    97,   104,   107,    98,     0,
      99,   101,   126,    13,   148,   223,   149,   173,   164,   154,
       0,     0,   160,   153,     0,   142,   167,   171,   143,   147,
       0,   158,   146,   145,   242,   204,   203,   205,   208,   206,
     212,   210,   209,   211,   207,   213,    16,   228,     0,   137,
     151,     0,   264,     0,     0,     0,   267,     0,   287,   136,
     273,   271,   278,   272,   214,   216,   217,   219,    45,   175,
      55,    56,    46,     0,     0,    69,     0,     0,     0,     0,
      87,   101,   275,   101,   106,   105,   173,   116,   117,   120,
     122,   122,   114,     0,     0,   165,     0,   141,   163,   161,
     139,     0,     0,     0,   232,     0,   241,   261,    36,    38,
       0,     0,   281,   280,   282,   283,   285,   286,   269,     0,
       0,     0,     0,     0,     0,   176,    61,    63,    78,    77,
      71,    79,    81,   114,   101,    96,    94,    95,   220,     0,
     123,   119,   121,     0,   101,   124,   126,   173,     0,   144,
       0,   224,   233,    15,    37,   263,   262,   284,   279,   189,
     216,   191,   216,   175,    62,    80,     0,   114,   118,   108,
     114,   125,   166,   162,    17,   215,   218,     0,   110,     0,
     113,    59,   112
  };

  /* YYPGOTO[NTERM-NUM].  */
  const short int
  CMPL_Parser::yypgoto_[] =
  {
      -400,   395,  -400,  -400,  -400,   -54,  -400,  -400,  -400,  -400,
     346,  -400,   237,   222,   133,  -400,   -23,  -400,  -400,  -400,
     -77,  -400,  -400,   195,  -400,  -400,   102,    -5,  -400,   -36,
    -400,  -400,   -89,  -400,  -400,  -400,  -400,  -400,   -40,  -400,
    -400,  -108,  -400,  -400,    12,  -400,  -400,  -162,  -400,  -400,
    -212,    94,  -400,  -400,  -200,  -400,  -399,  -199,  -400,   -42,
    -400,    14,  -400,   -39,  -173,  -400,  -400,  -400,  -400,  -400,
    -400,  -224,  -201,    10,   390,  -400,  -300,  -197,  -400,  -400,
    -400,   -85,  -400,  -400,   -18,   -51,   187,   -17,   -25,  -400,
    -400,  -400,  -400,  -400,  -400,  -400,    -6,  -374,    -9,  -400,
    -400,  -198,  -400,   -33,  -400,   -26,   -48,  -400,    18,    17,
     411,  -400,    39,   -19,    -1,  -120,  -400,  -400,  -301,    36,
    -400,   -52,   142,  -400,  -400,  -400,  -400,   241,   159,  -400,
    -400,    66,  -400,  -400,  -400,    71,     8,   186,    25,    43
  };

  /* YYDEFGOTO[NTERM-NUM].  */
  const short int
  CMPL_Parser::yydefgoto_[] =
  {
        -1,     5,     6,     7,    11,    65,   243,    66,   368,   422,
      67,    68,   184,    69,   283,    70,   373,    71,     8,    12,
      98,   306,    99,   100,   101,   310,   390,    72,   103,   393,
       9,    13,   111,   313,   112,   222,   397,   113,   398,    10,
      14,   121,   316,   122,   231,   403,   456,   232,   233,   234,
     235,   325,   326,   236,   237,   402,   463,   328,   329,   407,
     408,   461,   123,   413,    73,    74,    75,    76,    77,   249,
     248,    78,    79,   287,   147,   261,   104,   341,   468,   419,
     342,   252,   253,   257,   138,   444,   159,   160,   178,    81,
     204,    82,   205,    83,   162,   163,   385,   440,   387,   442,
     411,    84,    85,   144,   472,    86,   179,   180,   425,   164,
     134,   323,    88,   115,    89,    90,    91,    92,   428,   464,
     375,   199,   200,   201,   302,   380,   438,   202,   203,   381,
     439,   435,   436,   478,   382,   383,    93,   173,    94,    95
  };

  /* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule which
     number is the opposite.  If zero, do what YYDEFACT says.  */
  const short int CMPL_Parser::yytable_ninf_ = -260;
  const short int
  CMPL_Parser::yytable_[] =
  {
        80,   124,   139,   139,   238,   130,   131,   132,   132,   136,
     137,   140,   182,   102,   228,   156,   181,   105,   116,   125,
     158,   324,   212,   220,   339,   339,   106,   117,   126,   410,
     161,   165,    63,   327,    87,    63,   330,   264,   265,  -253,
      80,   133,   133,   107,   118,   127,   129,   340,   340,    80,
     343,   343,   344,   215,   486,   264,   265,    15,   114,   114,
     314,   108,   119,   128,   223,   256,   264,   265,   264,   265,
     263,   139,    17,   239,    87,   338,   338,   229,   230,   211,
     143,   320,   157,    87,   168,   169,   321,   322,   499,   281,
      18,   500,   446,   282,   102,   246,   141,   451,   105,   432,
     433,    19,   238,   124,   434,    20,   495,   106,   496,   317,
     318,   116,   465,   142,   324,    43,   151,   334,   170,   171,
     117,   125,   250,   251,   107,   404,   327,   258,   143,   262,
     126,   332,   264,   265,   315,   336,   145,   118,   312,   479,
     139,   481,   108,   146,   350,   299,   371,   127,   307,   308,
     309,   114,   304,   305,   149,   119,   409,   285,   286,   410,
     284,   114,   489,   170,   171,   128,   254,   250,   251,   412,
     255,   288,   289,   290,   291,   292,   293,   294,   295,   296,
     297,   150,    80,   166,    97,   498,   331,   229,   230,   333,
     190,   191,   192,   193,   194,   195,   196,   152,   502,   188,
     189,   190,   191,   192,   193,   194,   195,   196,   400,   102,
     102,   238,   432,   433,   105,   183,    87,   172,   172,   172,
     124,   153,   116,   106,   395,   154,    80,   139,   139,   388,
     367,   117,   448,   449,   139,   346,   347,   155,   125,   453,
     107,   457,   351,   186,   339,   174,   175,   126,   118,   357,
     358,   359,   360,   361,   362,   363,   364,   365,   108,   185,
      87,   139,   114,  -152,   127,   401,   119,   340,  -154,   370,
     343,   493,    49,    50,    51,    52,   379,    54,   114,   384,
     386,   238,   128,   238,   187,  -153,   409,     1,     2,     3,
       4,   206,   487,   207,   208,   338,   209,   124,   210,   213,
     214,   102,   490,   216,   217,   105,   466,   218,   219,   221,
     452,   139,   116,   278,   106,   125,   224,   447,   276,   415,
     429,   117,   225,   226,   126,   421,   227,   240,   244,   245,
     241,   107,   247,   242,   238,   374,   279,   300,   118,    43,
     259,   127,   277,   424,   238,   301,   319,   335,   394,   108,
     454,   399,   114,   345,   426,   114,   119,    80,   348,   128,
     349,   352,   353,   354,   369,   196,   392,   389,   414,   372,
     396,   416,   445,   417,   418,   420,   265,   450,   427,   459,
     139,   460,  -221,   469,   473,   298,   475,   476,   458,   102,
     471,    87,   483,   105,    96,   124,   494,   424,   116,  -151,
     501,    16,   106,   167,   275,   280,   474,   117,   366,   311,
     391,   484,   485,   125,   477,   455,   384,   488,   386,   107,
     405,   441,   126,   443,   118,   462,   467,   491,    28,    97,
     148,    30,   497,    32,   482,   480,   135,   108,   114,   127,
     470,   139,   119,   376,   303,   437,   431,     0,     0,   492,
       0,   355,   356,   114,     0,     0,     0,   128,     0,     0,
       0,     0,    55,    56,     0,   374,   445,    58,    59,    60,
       0,     0,     0,     0,     0,     0,     0,     0,     0,  -259,
     -42,   -42,     0,   394,     0,     0,     0,     0,   399,   378,
       0,   -10,    21,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   414,     0,     0,     0,     0,     0,    22,    23,
      24,    25,     0,     0,     0,     0,   441,    26,   441,    27,
       0,     0,   -10,   -10,   -10,   -10,    28,     0,    29,    30,
      31,    32,     0,     0,    33,    34,    35,    36,    37,     0,
       0,    38,    39,    40,    41,    42,    43,    44,    45,    46,
      47,    48,    49,    50,    51,    52,    53,    54,     0,     0,
      55,    56,   378,     0,    57,    58,    59,    60,    61,    62,
     -66,   109,     0,    63,     0,    64,     0,  -259,   -10,   -10,
       0,     0,     0,     0,     0,     0,     0,  -254,  -254,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   -66,   -66,   -66,   -66,   110,  -254,  -254,    30,  -254,
      32,     0,     0,  -254,  -254,  -254,  -254,  -254,     0,     0,
    -254,  -254,  -254,  -254,  -254,  -254,  -254,  -254,  -254,     0,
       0,  -254,  -254,  -254,  -254,     0,  -254,     0,     0,    55,
      56,     0,   -84,   120,    58,    59,    60,     0,     0,     0,
       0,     0,  -254,     0,  -254,     0,  -259,   -66,   -66,  -254,
    -254,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   -84,   -84,   -84,   -84,   110,  -254,  -254,
      30,  -254,    32,     0,     0,  -254,  -254,  -254,  -254,  -254,
       0,     0,  -254,  -254,  -254,  -254,  -254,  -254,  -254,  -254,
    -254,   -42,    96,  -254,  -254,  -254,  -254,     0,  -254,     0,
       0,    55,    56,     0,     0,     0,    58,    59,    60,     0,
       0,     0,     0,     0,  -254,     0,  -254,     0,  -259,   -84,
     -84,     0,   -42,   -42,   -42,   -42,    28,    97,     0,    30,
       0,    32,     0,     0,     0,   188,   189,   190,   191,   192,
     193,   194,   195,   196,     0,     0,   197,     0,     0,     0,
       0,     0,     0,     0,     0,    21,     0,     0,     0,     0,
      55,    56,     0,     0,     0,    58,    59,    60,     0,     0,
       0,    22,    23,    24,    25,     0,     0,  -259,   -42,   -42,
      26,     0,    27,     0,     0,     0,     0,     0,     0,    28,
       0,    29,    30,    31,    32,     0,     0,    33,    34,    35,
      36,    37,   198,     0,    38,    39,    40,    41,    42,    43,
      44,    45,    46,    47,    48,    49,    50,    51,    52,    53,
      54,     0,     0,    55,    56,     0,     0,    57,    58,    59,
      60,    61,    62,   109,     0,     0,    63,     0,    64,     0,
    -259,   -10,   -10,     0,     0,     0,     0,     0,     0,  -254,
    -254,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   110,  -254,  -254,
      30,  -254,    32,     0,     0,  -254,  -254,  -254,  -254,  -254,
       0,     0,  -254,  -254,  -254,  -254,  -254,  -254,  -254,  -254,
    -254,     0,     0,  -254,  -254,  -254,  -254,     0,  -254,     0,
       0,    55,    56,     0,     0,   120,    58,    59,    60,     0,
       0,     0,     0,     0,  -254,     0,  -254,     0,  -259,   -66,
     -66,  -254,  -254,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   110,
    -254,  -254,    30,  -254,    32,     0,     0,  -254,  -254,  -254,
    -254,  -254,     0,     0,  -254,  -254,  -254,  -254,  -254,  -254,
    -254,  -254,  -254,     0,     0,  -254,  -254,  -254,  -254,     0,
    -254,     0,     0,    55,    56,     0,   -10,    21,    58,    59,
      60,     0,     0,     0,     0,     0,  -254,     0,  -254,     0,
    -259,   -84,   -84,    22,    23,    24,    25,     0,     0,     0,
       0,     0,    26,     0,    27,     0,     0,   -10,   -10,   -10,
     -10,    28,     0,    29,    30,    31,    32,     0,     0,    33,
      34,    35,    36,    37,     0,     0,    38,    39,    40,    41,
      42,    43,    44,    45,    46,    47,    48,    49,    50,    51,
      52,    53,    54,     0,     0,    55,    56,   -66,   109,    57,
      58,    59,    60,    61,    62,     0,     0,     0,    63,     0,
      64,     0,  -259,     0,  -254,  -254,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   -66,   -66,
     -66,   -66,   110,  -254,  -254,    30,  -254,    32,     0,     0,
    -254,  -254,  -254,  -254,  -254,     0,     0,  -254,  -254,  -254,
    -254,  -254,  -254,  -254,  -254,  -254,     0,     0,  -254,  -254,
    -254,  -254,     0,  -254,     0,     0,    55,    56,   -84,   120,
       0,    58,    59,    60,     0,     0,     0,     0,     0,  -254,
       0,  -254,     0,  -259,     0,  -254,  -254,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   -84,
     -84,   -84,   -84,   110,  -254,  -254,    30,  -254,    32,     0,
       0,  -254,  -254,  -254,  -254,  -254,     0,     0,  -254,  -254,
    -254,  -254,  -254,  -254,  -254,  -254,  -254,   -42,    96,  -254,
    -254,  -254,  -254,     0,  -254,     0,     0,    55,    56,     0,
       0,     0,    58,    59,    60,     0,     0,     0,     0,     0,
    -254,     0,  -254,     0,  -259,     0,     0,     0,   -42,   -42,
     -42,   -42,    28,    97,     0,    30,     0,    32,   266,   267,
     268,   269,   270,   271,   272,   273,   274,   188,   189,   190,
     191,   192,   193,   194,   195,   196,     0,     0,   197,    22,
      23,    24,    25,     0,     0,     0,    55,    56,    26,     0,
      27,    58,    59,    60,     0,     0,     0,   176,     0,    29,
       0,    31,     0,  -259,     0,    33,    34,    35,    36,    37,
     177,     0,    38,    39,    40,    41,    42,    43,    44,    45,
      46,    47,     0,    49,    50,    51,    52,    53,    54,     0,
       0,     0,     0,     0,   198,     0,     0,     0,     0,    61,
      62,     0,     0,     0,    63,     0,    64,    22,    23,    24,
      25,     0,     0,     0,     0,     0,    26,     0,    27,     0,
       0,     0,     0,     0,     0,   166,     0,    29,     0,    31,
       0,     0,     0,    33,    34,    35,    36,    37,     0,     0,
      38,    39,    40,    41,    42,    43,    44,    45,    46,    47,
       0,    49,    50,    51,    52,    53,    54,     0,     0,     0,
      22,    23,    24,    25,     0,     0,     0,    61,    62,    26,
       0,    27,    63,     0,    64,     0,     0,     0,   176,     0,
      29,     0,    31,     0,     0,     0,    33,    34,    35,    36,
      37,     0,     0,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,     0,    49,    50,    51,    52,    53,    54,
       0,     0,     0,    22,    23,    24,    25,     0,     0,     0,
      61,    62,    26,     0,    27,    63,     0,    64,     0,     0,
       0,     0,   260,    29,     0,    31,     0,     0,     0,    33,
      34,    35,    36,    37,     0,     0,    38,    39,    40,    41,
      42,    43,    44,    45,    46,    47,     0,    49,    50,    51,
      52,    53,    54,     0,     0,     0,    22,    23,    24,    25,
       0,     0,     0,    61,    62,    26,     0,    27,    63,     0,
      64,     0,     0,     0,   377,     0,    29,     0,    31,     0,
       0,     0,    33,    34,    35,    36,    37,     0,     0,    38,
      39,    40,    41,    42,    43,    44,    45,    46,    47,     0,
      49,    50,    51,    52,    53,    54,     0,     0,     0,    22,
      23,    24,    25,     0,     0,     0,    61,    62,    26,     0,
      27,    63,     0,    64,     0,     0,     0,     0,     0,    29,
       0,    31,     0,     0,     0,    33,    34,    35,    36,    37,
       0,   423,    38,    39,    40,    41,    42,    43,    44,    45,
      46,    47,     0,    49,    50,    51,    52,    53,    54,     0,
       0,     0,    22,    23,    24,    25,     0,     0,     0,    61,
      62,    26,     0,    27,    63,     0,    64,     0,     0,     0,
       0,     0,    29,     0,    31,     0,     0,     0,    33,    34,
      35,    36,    37,     0,     0,    38,    39,    40,    41,    42,
      43,    44,    45,    46,    47,     0,    49,    50,    51,    52,
      53,    54,     0,     0,     0,     0,   430,    22,    23,    24,
      25,     0,    61,    62,     0,     0,    26,    63,    27,    64,
       0,     0,     0,     0,     0,     0,     0,    29,     0,    31,
       0,     0,     0,    33,    34,    35,    36,    37,     0,     0,
      38,    39,    40,    41,    42,    43,    44,    45,    46,    47,
       0,    49,    50,    51,    52,    53,    54,    22,    23,     0,
       0,     0,     0,     0,     0,     0,    26,    61,    62,     0,
       0,     0,    63,     0,    64,     0,     0,    29,     0,    31,
       0,     0,     0,    33,    34,    35,    36,    37,     0,     0,
      38,    39,    40,    41,    42,    43,    44,    45,    46,     0,
       0,    49,    50,    51,    52,     0,    54,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    97,   406,
       0,    31,    63,     0,    64,    33,    34,    35,    36,    37,
       0,     0,    38,    39,    40,    41,    42,     0,    44,    45,
      46,     0,     0,    49,    50,    51,    52,     0,    54,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   337,     0,    31,    63,     0,    64,    33,    34,    35,
      36,    37,     0,     0,    38,    39,    40,    41,    42,     0,
      44,    45,    46,     0,     0,    49,    50,    51,    52,     0,
      54,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    63,     0,    64
  };

  /* YYCHECK.  */
  const short int
  CMPL_Parser::yycheck_[] =
  {
        17,    20,    28,    29,   124,    22,    23,    24,    25,    26,
      27,    29,    66,    18,   122,    48,    64,    18,    19,    20,
      53,   233,    99,   112,   248,   249,    18,    19,    20,   329,
      55,    56,    82,   233,    17,    82,   235,     5,     6,    86,
      57,    24,    25,    18,    19,    20,    80,   248,   249,    66,
     248,   249,   249,   105,   453,     5,     6,     0,    19,    20,
     222,    18,    19,    20,   116,   150,     5,     6,     5,     6,
     155,    97,    79,   125,    57,   248,   249,    17,    18,    97,
      84,     8,    86,    66,    38,    39,    13,    14,   487,     4,
      79,   490,   393,     8,    99,   143,     4,   398,    99,     3,
       4,    79,   222,   122,     8,    79,   480,    99,   482,   229,
     230,   112,   413,    84,   326,    55,    86,    85,    72,    73,
     112,   122,    36,    37,    99,   324,   326,   152,    84,   154,
     122,   239,     5,     6,   223,    85,    84,   112,   215,   440,
     166,   442,    99,    84,    81,   199,    85,   122,    61,    62,
      63,   112,   204,   205,    84,   112,   329,    41,    42,   459,
     185,   122,   463,    72,    73,   122,    35,    36,    37,   331,
      39,   188,   189,   190,   191,   192,   193,   194,   195,   196,
     197,    84,   199,    35,    36,   486,   238,    17,    18,   243,
      19,    20,    21,    22,    23,    24,    25,    84,   499,    17,
      18,    19,    20,    21,    22,    23,    24,    25,   316,   214,
     215,   331,     3,     4,   215,    79,   199,    58,    59,    60,
     239,    84,   223,   215,   313,    84,   243,   253,   254,   306,
     278,   223,    51,    52,   260,   253,   254,    84,   239,   401,
     215,   403,   260,    80,   468,    59,    60,   239,   223,   266,
     267,   268,   269,   270,   271,   272,   273,   274,   215,    84,
     243,   287,   223,    30,   239,   317,   223,   468,    30,   287,
     468,   468,    61,    62,    63,    64,   302,    66,   239,   304,
     305,   401,   239,   403,    30,    30,   459,    31,    32,    33,
      34,    86,   454,    80,    80,   468,    80,   316,    80,    79,
      81,   306,   464,    80,    80,   306,   414,    80,    80,    79,
     399,   337,   313,    81,   306,   316,    80,   394,    83,   337,
     374,   313,    80,    80,   316,   350,    80,    80,   141,   142,
      80,   306,   145,    80,   454,   299,    85,    79,   313,    55,
     153,   316,    83,   368,   464,    81,    80,    85,   312,   306,
     402,   315,   313,    85,   371,   316,   313,   374,    85,   316,
      85,    85,    85,    85,    85,    25,    80,    82,   332,    88,
      67,    30,   389,    85,    81,    85,     6,    80,    87,    19,
     406,    68,    30,    85,    80,   198,    79,    79,   406,   394,
     423,   374,    28,   394,     1,   414,    80,   422,   399,    30,
      83,     6,   394,    57,   167,   183,   429,   399,   275,   214,
     308,   447,   452,   414,   439,   403,   441,   459,   443,   394,
     326,   385,   414,   387,   399,   411,   416,   466,    35,    36,
      40,    38,   483,    40,   443,   441,    25,   394,   399,   414,
     422,   467,   399,   301,   203,   379,   375,    -1,    -1,   467,
      -1,   264,   265,   414,    -1,    -1,    -1,   414,    -1,    -1,
      -1,    -1,    69,    70,    -1,   429,   483,    74,    75,    76,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    86,
      87,    88,    -1,   447,    -1,    -1,    -1,    -1,   452,   302,
      -1,     0,     1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   466,    -1,    -1,    -1,    -1,    -1,    17,    18,
      19,    20,    -1,    -1,    -1,    -1,   480,    26,   482,    28,
      -1,    -1,    31,    32,    33,    34,    35,    -1,    37,    38,
      39,    40,    -1,    -1,    43,    44,    45,    46,    47,    -1,
      -1,    50,    51,    52,    53,    54,    55,    56,    57,    58,
      59,    60,    61,    62,    63,    64,    65,    66,    -1,    -1,
      69,    70,   375,    -1,    73,    74,    75,    76,    77,    78,
       0,     1,    -1,    82,    -1,    84,    -1,    86,    87,    88,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    17,    18,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    31,    32,    33,    34,    35,    36,    37,    38,    39,
      40,    -1,    -1,    43,    44,    45,    46,    47,    -1,    -1,
      50,    51,    52,    53,    54,    55,    56,    57,    58,    -1,
      -1,    61,    62,    63,    64,    -1,    66,    -1,    -1,    69,
      70,    -1,     0,     1,    74,    75,    76,    -1,    -1,    -1,
      -1,    -1,    82,    -1,    84,    -1,    86,    87,    88,    17,
      18,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    31,    32,    33,    34,    35,    36,    37,
      38,    39,    40,    -1,    -1,    43,    44,    45,    46,    47,
      -1,    -1,    50,    51,    52,    53,    54,    55,    56,    57,
      58,     0,     1,    61,    62,    63,    64,    -1,    66,    -1,
      -1,    69,    70,    -1,    -1,    -1,    74,    75,    76,    -1,
      -1,    -1,    -1,    -1,    82,    -1,    84,    -1,    86,    87,
      88,    -1,    31,    32,    33,    34,    35,    36,    -1,    38,
      -1,    40,    -1,    -1,    -1,    17,    18,    19,    20,    21,
      22,    23,    24,    25,    -1,    -1,    28,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,     1,    -1,    -1,    -1,    -1,
      69,    70,    -1,    -1,    -1,    74,    75,    76,    -1,    -1,
      -1,    17,    18,    19,    20,    -1,    -1,    86,    87,    88,
      26,    -1,    28,    -1,    -1,    -1,    -1,    -1,    -1,    35,
      -1,    37,    38,    39,    40,    -1,    -1,    43,    44,    45,
      46,    47,    84,    -1,    50,    51,    52,    53,    54,    55,
      56,    57,    58,    59,    60,    61,    62,    63,    64,    65,
      66,    -1,    -1,    69,    70,    -1,    -1,    73,    74,    75,
      76,    77,    78,     1,    -1,    -1,    82,    -1,    84,    -1,
      86,    87,    88,    -1,    -1,    -1,    -1,    -1,    -1,    17,
      18,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    35,    36,    37,
      38,    39,    40,    -1,    -1,    43,    44,    45,    46,    47,
      -1,    -1,    50,    51,    52,    53,    54,    55,    56,    57,
      58,    -1,    -1,    61,    62,    63,    64,    -1,    66,    -1,
      -1,    69,    70,    -1,    -1,     1,    74,    75,    76,    -1,
      -1,    -1,    -1,    -1,    82,    -1,    84,    -1,    86,    87,
      88,    17,    18,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    35,
      36,    37,    38,    39,    40,    -1,    -1,    43,    44,    45,
      46,    47,    -1,    -1,    50,    51,    52,    53,    54,    55,
      56,    57,    58,    -1,    -1,    61,    62,    63,    64,    -1,
      66,    -1,    -1,    69,    70,    -1,     0,     1,    74,    75,
      76,    -1,    -1,    -1,    -1,    -1,    82,    -1,    84,    -1,
      86,    87,    88,    17,    18,    19,    20,    -1,    -1,    -1,
      -1,    -1,    26,    -1,    28,    -1,    -1,    31,    32,    33,
      34,    35,    -1,    37,    38,    39,    40,    -1,    -1,    43,
      44,    45,    46,    47,    -1,    -1,    50,    51,    52,    53,
      54,    55,    56,    57,    58,    59,    60,    61,    62,    63,
      64,    65,    66,    -1,    -1,    69,    70,     0,     1,    73,
      74,    75,    76,    77,    78,    -1,    -1,    -1,    82,    -1,
      84,    -1,    86,    -1,    17,    18,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    31,    32,
      33,    34,    35,    36,    37,    38,    39,    40,    -1,    -1,
      43,    44,    45,    46,    47,    -1,    -1,    50,    51,    52,
      53,    54,    55,    56,    57,    58,    -1,    -1,    61,    62,
      63,    64,    -1,    66,    -1,    -1,    69,    70,     0,     1,
      -1,    74,    75,    76,    -1,    -1,    -1,    -1,    -1,    82,
      -1,    84,    -1,    86,    -1,    17,    18,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    31,
      32,    33,    34,    35,    36,    37,    38,    39,    40,    -1,
      -1,    43,    44,    45,    46,    47,    -1,    -1,    50,    51,
      52,    53,    54,    55,    56,    57,    58,     0,     1,    61,
      62,    63,    64,    -1,    66,    -1,    -1,    69,    70,    -1,
      -1,    -1,    74,    75,    76,    -1,    -1,    -1,    -1,    -1,
      82,    -1,    84,    -1,    86,    -1,    -1,    -1,    31,    32,
      33,    34,    35,    36,    -1,    38,    -1,    40,     8,     9,
      10,    11,    12,    13,    14,    15,    16,    17,    18,    19,
      20,    21,    22,    23,    24,    25,    -1,    -1,    28,    17,
      18,    19,    20,    -1,    -1,    -1,    69,    70,    26,    -1,
      28,    74,    75,    76,    -1,    -1,    -1,    35,    -1,    37,
      -1,    39,    -1,    86,    -1,    43,    44,    45,    46,    47,
      48,    -1,    50,    51,    52,    53,    54,    55,    56,    57,
      58,    59,    -1,    61,    62,    63,    64,    65,    66,    -1,
      -1,    -1,    -1,    -1,    84,    -1,    -1,    -1,    -1,    77,
      78,    -1,    -1,    -1,    82,    -1,    84,    17,    18,    19,
      20,    -1,    -1,    -1,    -1,    -1,    26,    -1,    28,    -1,
      -1,    -1,    -1,    -1,    -1,    35,    -1,    37,    -1,    39,
      -1,    -1,    -1,    43,    44,    45,    46,    47,    -1,    -1,
      50,    51,    52,    53,    54,    55,    56,    57,    58,    59,
      -1,    61,    62,    63,    64,    65,    66,    -1,    -1,    -1,
      17,    18,    19,    20,    -1,    -1,    -1,    77,    78,    26,
      -1,    28,    82,    -1,    84,    -1,    -1,    -1,    35,    -1,
      37,    -1,    39,    -1,    -1,    -1,    43,    44,    45,    46,
      47,    -1,    -1,    50,    51,    52,    53,    54,    55,    56,
      57,    58,    59,    -1,    61,    62,    63,    64,    65,    66,
      -1,    -1,    -1,    17,    18,    19,    20,    -1,    -1,    -1,
      77,    78,    26,    -1,    28,    82,    -1,    84,    -1,    -1,
      -1,    -1,    36,    37,    -1,    39,    -1,    -1,    -1,    43,
      44,    45,    46,    47,    -1,    -1,    50,    51,    52,    53,
      54,    55,    56,    57,    58,    59,    -1,    61,    62,    63,
      64,    65,    66,    -1,    -1,    -1,    17,    18,    19,    20,
      -1,    -1,    -1,    77,    78,    26,    -1,    28,    82,    -1,
      84,    -1,    -1,    -1,    35,    -1,    37,    -1,    39,    -1,
      -1,    -1,    43,    44,    45,    46,    47,    -1,    -1,    50,
      51,    52,    53,    54,    55,    56,    57,    58,    59,    -1,
      61,    62,    63,    64,    65,    66,    -1,    -1,    -1,    17,
      18,    19,    20,    -1,    -1,    -1,    77,    78,    26,    -1,
      28,    82,    -1,    84,    -1,    -1,    -1,    -1,    -1,    37,
      -1,    39,    -1,    -1,    -1,    43,    44,    45,    46,    47,
      -1,    49,    50,    51,    52,    53,    54,    55,    56,    57,
      58,    59,    -1,    61,    62,    63,    64,    65,    66,    -1,
      -1,    -1,    17,    18,    19,    20,    -1,    -1,    -1,    77,
      78,    26,    -1,    28,    82,    -1,    84,    -1,    -1,    -1,
      -1,    -1,    37,    -1,    39,    -1,    -1,    -1,    43,    44,
      45,    46,    47,    -1,    -1,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    -1,    61,    62,    63,    64,
      65,    66,    -1,    -1,    -1,    -1,    71,    17,    18,    19,
      20,    -1,    77,    78,    -1,    -1,    26,    82,    28,    84,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    37,    -1,    39,
      -1,    -1,    -1,    43,    44,    45,    46,    47,    -1,    -1,
      50,    51,    52,    53,    54,    55,    56,    57,    58,    59,
      -1,    61,    62,    63,    64,    65,    66,    17,    18,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    26,    77,    78,    -1,
      -1,    -1,    82,    -1,    84,    -1,    -1,    37,    -1,    39,
      -1,    -1,    -1,    43,    44,    45,    46,    47,    -1,    -1,
      50,    51,    52,    53,    54,    55,    56,    57,    58,    -1,
      -1,    61,    62,    63,    64,    -1,    66,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    36,    37,
      -1,    39,    82,    -1,    84,    43,    44,    45,    46,    47,
      -1,    -1,    50,    51,    52,    53,    54,    -1,    56,    57,
      58,    -1,    -1,    61,    62,    63,    64,    -1,    66,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    37,    -1,    39,    82,    -1,    84,    43,    44,    45,
      46,    47,    -1,    -1,    50,    51,    52,    53,    54,    -1,
      56,    57,    58,    -1,    -1,    61,    62,    63,    64,    -1,
      66,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    82,    -1,    84
  };

  /* STOS_[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
  const unsigned char
  CMPL_Parser::yystos_[] =
  {
         0,    31,    32,    33,    34,    90,    91,    92,   107,   119,
     128,    93,   108,   120,   129,     0,    90,    79,    79,    79,
      79,     1,    17,    18,    19,    20,    26,    28,    35,    37,
      38,    39,    40,    43,    44,    45,    46,    47,    50,    51,
      52,    53,    54,    55,    56,    57,    58,    59,    60,    61,
      62,    63,    64,    65,    66,    69,    70,    73,    74,    75,
      76,    77,    78,    82,    84,    94,    96,    99,   100,   102,
     104,   106,   116,   153,   154,   155,   156,   157,   160,   161,
     176,   178,   180,   182,   190,   191,   194,   198,   201,   203,
     204,   205,   206,   225,   227,   228,     1,    36,   109,   111,
     112,   113,   116,   117,   165,   203,   225,   227,   228,     1,
      35,   121,   123,   126,   201,   202,   203,   225,   227,   228,
       1,   130,   132,   151,   202,   203,   225,   227,   228,    80,
     176,   176,   176,   198,   199,   199,   176,   176,   173,   194,
     173,     4,    84,    84,   192,    84,    84,   163,   163,    84,
      84,    86,    84,    84,    84,    84,   192,    86,   192,   175,
     176,   177,   183,   184,   198,   177,    35,    99,    38,    39,
      72,    73,   217,   226,   226,   226,    35,    48,   177,   195,
     196,   195,    94,    79,   101,    84,    80,    30,    17,    18,
      19,    20,    21,    22,    23,    24,    25,    28,    84,   210,
     211,   212,   216,   217,   179,   181,    86,    80,    80,    80,
      80,   173,   109,    79,    81,   210,    80,    80,    80,    80,
     121,    79,   124,   210,    80,    80,    80,    80,   130,    17,
      18,   133,   136,   137,   138,   139,   142,   143,   204,   210,
      80,    80,    80,    95,   175,   175,   195,   175,   159,   158,
      36,    37,   170,   171,    35,    39,   170,   172,   177,   175,
      36,   164,   177,   170,     5,     6,     8,     9,    10,    11,
      12,    13,    14,    15,    16,   101,    83,    83,    81,    85,
     102,     4,     8,   103,   177,    41,    42,   162,   176,   176,
     176,   176,   176,   176,   176,   176,   176,   176,   175,    94,
      79,    81,   213,   216,   210,   210,   110,    61,    62,    63,
     114,   112,   109,   122,   136,   121,   131,   204,   204,    80,
       8,    13,    14,   200,   139,   140,   141,   143,   146,   147,
     146,   210,   130,    94,    85,    85,    85,    37,   153,   160,
     161,   166,   169,   190,   166,    85,   173,   173,    85,    85,
      81,   173,    85,    85,    85,   175,   175,   176,   176,   176,
     176,   176,   176,   176,   176,   176,   103,   195,    97,    85,
     173,    85,    88,   105,   208,   209,   211,    35,   175,   194,
     214,   218,   223,   224,   177,   185,   177,   187,   109,    82,
     115,   115,    80,   118,   208,   121,    67,   125,   127,   208,
     130,   210,   144,   134,   146,   140,    37,   148,   149,   153,
     165,   189,   136,   152,   208,   173,    30,    85,    81,   168,
      85,   177,    98,    49,   177,   197,   176,    87,   207,    94,
      71,   224,     3,     4,     8,   220,   221,   220,   215,   219,
     186,   208,   188,   208,   174,   176,   207,   109,    51,    52,
      80,   207,   121,   136,   210,   133,   135,   136,   173,    19,
      68,   150,   150,   145,   208,   207,   130,   162,   167,    85,
     197,   192,   193,    80,   105,    79,    79,   177,   222,   207,
     185,   207,   187,    28,   118,   127,   145,   136,   148,   207,
     136,   152,   173,   166,    80,   186,   186,   174,   207,   145,
     145,    83,   207
  };

#if YYDEBUG
  /* TOKEN_NUMBER_[YYLEX-NUM] -- Internal symbol number corresponding
     to YYLEX-NUM.  */
  const unsigned short int
  CMPL_Parser::yytoken_number_[] =
  {
         0,   256,   257,   258,   259,   260,   261,   262,    61,   263,
     264,   265,   266,   267,   268,   269,   270,    43,    45,    42,
      47,    37,   271,   272,   273,    94,    33,   274,   275,   276,
     277,   278,   279,   280,   281,   282,   283,   284,   285,   286,
     287,   288,   289,   290,   291,   292,   293,   294,   295,   296,
     297,   298,   299,   300,   301,   302,   303,   304,   305,   306,
     307,   308,   309,   310,   311,   312,   313,   314,   315,   316,
     317,   318,   319,   320,   321,   322,   323,   324,   325,    58,
      59,    44,    91,    93,    40,    41,   123,   125,   124
  };
#endif

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
  const unsigned char
  CMPL_Parser::yyr1_[] =
  {
         0,    89,    90,    90,    91,    91,    91,    91,    93,    92,
      94,    94,    95,    94,    97,    96,    98,    96,    96,    96,
      96,    96,    96,    99,    99,   100,   100,   101,   101,   102,
     102,   102,   102,   102,   103,   103,   104,   105,   105,   106,
     108,   107,   109,   109,   110,   109,   111,   111,   111,   111,
     111,   112,   112,   113,   113,   114,   114,   114,   115,   115,
     116,   117,   118,   118,   120,   119,   121,   121,   122,   121,
     124,   123,   123,   123,   123,   123,   125,   125,   125,   126,
     127,   127,   129,   128,   130,   130,   131,   130,   132,   132,
     132,   132,   132,   134,   133,   135,   135,   136,   137,   137,
     137,   138,   139,   139,   140,   140,   141,   141,   142,   142,
     143,   144,   143,   145,   145,   147,   146,   148,   148,   149,
     149,   149,   150,   150,   151,   152,   152,   153,   153,   153,
     153,   153,   153,   154,   154,   154,   155,   156,   158,   157,
     159,   157,   157,   157,   157,   157,   157,   157,   157,   157,
     157,   160,   161,   161,   161,   162,   162,   163,   164,   165,
     166,   167,   166,   168,   169,   169,   169,   170,   171,   171,
     172,   172,   172,   173,   173,   174,   174,   175,   175,   175,
     175,   176,   176,   176,   176,   176,   176,   177,   179,   178,
     181,   180,   182,   182,   182,   182,   182,   182,   182,   182,
     182,   182,   182,   183,   183,   184,   184,   184,   184,   184,
     184,   184,   184,   184,   185,   186,   186,   187,   188,   188,
     189,   190,   191,   192,   193,   194,   194,   195,   195,   196,
     196,   196,   197,   197,   198,   198,   198,   198,   198,   198,
     198,   198,   198,   198,   198,   198,   199,   199,   199,   200,
     200,   200,   201,   201,   202,   202,   203,   204,   205,   206,
     206,   207,   208,   208,   209,   210,   211,   211,   213,   212,
     214,   214,   214,   215,   216,   216,   217,   217,   219,   218,
     220,   220,   221,   221,   222,   223,   223,   224,   225,   225,
     225,   226,   226,   226,   226,   227,   228,   228
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
       0,     6,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     2,     2,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     1,     3,     0,     1,     3,     0,
       2,     2,     3,     3,     1,     3,     3,     1,     3,     0,
       1,     1,     1,     2,     2,     1,     1,     1,     2,     2,
       3,     5,     4,     1,     2,     2,     0,     1,     1,     1,
       1,     1,     1,     1,     0,     2,     2,     2,     2,     0,
       1,     1,     3,     3,     1,     2,     1,     3,     0,     4,
       0,     1,     1,     0,     2,     0,     1,     1,     0,     3,
       1,     1,     1,     1,     1,     2,     2,     1,     2,     2,
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
  "'-'", "'*'", "'/'", "'%'", "TPS_INTERSECT_RR", "MOD", "DIV", "'^'",
  "'!'", "VORZEICHEN", "POINTS", "TPS_FREE_IND_REV", "SUBSYM_OP",
  "SECT_PARAM", "SECT_VAR", "SECT_OBJ", "SECT_CSTR", "SYMBOL_UNDEF",
  "SYMBOL_VAR", "SYMBOL_VALUE", "SYMBOL_LINENAME", "DEFS_TMP",
  "DEFS_TMP_ASSIGN", "SUBSYMBOL_UNDEF", "SUBSYMBOL_VALUE", "INT_NUMBER",
  "STRING", "DOUBLE_NUMBER", "MATHFUNC", "PROBLEM_FUNC", "NULL_VAL",
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
  "vekt_def", "set_expr", "tps_fir_expr", "verg_op", "line_name_sym",
  "line_name", "loop_start", "loop_sum_start", "loop_set_start",
  "loop_name", "loop_stop", "loop_alt", "loop_alt_start", "loop_header",
  "loop_heads", "loop_head", "@20", "loop_head_expr", "loop_comp_start",
  "loop_mods", "loop_mod", "loop_for", "@21", "loop_for_assign",
  "loop_for_assign_alt", "loop_for_set", "loop_for_var", "loop_if",
  "loop_ctrl", "loop_ctrl_ref", "local_assign", "usr_err_msg", 0
  };
#endif

#if YYDEBUG
  /* YYRHS -- A `-1'-separated list of the rules' RHS.  */
  const CMPL_Parser::rhs_number_type
  CMPL_Parser::yyrhs_[] =
  {
        90,     0,    -1,    -1,    91,    90,    -1,    92,    -1,   107,
      -1,   119,    -1,   128,    -1,    -1,    31,    93,    79,    94,
      -1,    -1,    96,    94,    -1,    -1,     1,    80,    95,    94,
      -1,    -1,    99,   101,   103,    97,   197,    80,    -1,    -1,
      73,    99,   101,   103,    98,   197,    80,    -1,   104,    -1,
     225,    80,    -1,   227,    80,    -1,   228,    80,    -1,   106,
      80,    -1,   116,    -1,   100,    -1,   176,    -1,   198,    -1,
      -1,    79,   102,    -1,    61,    -1,    62,    -1,    63,    -1,
      64,    -1,    66,    -1,     4,    -1,     8,    -1,   203,   210,
      94,   105,   207,    -1,   208,    94,   105,    -1,    -1,    60,
     192,    -1,    -1,    32,   108,    79,   109,    -1,    -1,   111,
     109,    -1,    -1,     1,    80,   110,   109,    -1,   112,    79,
     114,    80,    -1,   117,    -1,   225,    80,    -1,   227,    80,
      -1,   228,    80,    -1,   113,    -1,   113,    81,   112,    -1,
     116,    -1,   165,    -1,    61,   115,    -1,    62,   115,    -1,
      63,    -1,    -1,    82,   174,    28,   174,    83,    -1,    35,
     173,    -1,   203,   210,   109,   118,   207,    -1,   208,   109,
     118,    -1,    -1,    -1,    33,   120,    79,   121,    -1,    -1,
     123,   121,    -1,    -1,     1,    80,   122,   121,    -1,    -1,
     202,   124,   136,   125,    80,    -1,   126,    -1,   225,    80,
      -1,   227,    80,    -1,   228,    80,    -1,    -1,    67,    52,
      -1,    67,    51,    -1,   203,   210,   121,   127,   207,    -1,
     208,   121,   127,    -1,    -1,    -1,    34,   129,    79,   130,
      -1,    -1,   132,   130,    -1,    -1,     1,    80,   131,   130,
      -1,   202,   133,    80,    -1,   151,    -1,   225,    80,    -1,
     227,    80,    -1,   228,    80,    -1,    -1,   136,   200,   134,
     135,    -1,   136,    -1,   133,    -1,   137,   140,    -1,   138,
     146,    -1,   139,   146,    -1,   142,    -1,    -1,    17,    -1,
      18,    -1,    -1,   141,   140,    -1,   139,   146,    -1,   143,
      -1,   204,   210,   136,   145,   207,    -1,   143,    -1,    17,
     204,   210,   136,   145,   207,    -1,    -1,    18,   204,   144,
     210,   136,   145,   207,    -1,   208,   136,   145,    -1,    -1,
      -1,   147,   148,    -1,   149,    -1,   149,    19,   148,    -1,
     165,   150,    -1,   153,    -1,   189,   150,    -1,    -1,    68,
      -1,   203,   210,   130,   152,   207,    -1,   208,   130,   152,
      -1,    -1,   154,    -1,   156,    -1,   157,    -1,   191,    -1,
     155,    -1,    39,    -1,    43,    -1,    45,    -1,    44,    -1,
     194,    -1,   102,    84,   177,    85,    -1,    -1,    52,   163,
     158,   166,    85,    -1,    -1,    51,   163,   159,   166,    85,
      -1,    53,    84,   170,    85,    -1,    54,    84,   172,    85,
      -1,    57,    84,   175,    81,   177,    85,    -1,    58,    84,
     177,    85,    -1,    58,    84,   164,    85,    -1,    56,    84,
     177,    85,    -1,    46,    84,   175,    85,    -1,    50,    84,
     175,    85,    -1,    47,   192,    -1,   161,    30,   162,   173,
      -1,   153,    -1,   190,    -1,   160,    -1,    41,    -1,    42,
      -1,    84,    -1,    36,   173,    -1,    36,   173,    -1,   169,
      -1,    -1,   169,   168,   167,   166,    -1,    81,    -1,   153,
      -1,    37,   173,    -1,   161,    30,   162,   173,    -1,   171,
     173,    -1,    36,    -1,    37,    -1,   170,    -1,    35,   173,
      -1,    39,    -1,    -1,   194,    -1,    -1,   176,    -1,   176,
      -1,   183,    -1,   184,    -1,   198,    -1,   153,    -1,   182,
      -1,   190,    -1,   160,    -1,   178,    -1,    26,   176,    -1,
     175,    -1,    -1,   204,   179,   210,   185,   186,   207,    -1,
      -1,   205,   181,   210,   187,   188,   207,    -1,   176,    17,
     176,    -1,   176,    18,   176,    -1,   176,    19,   176,    -1,
     176,    20,   176,    -1,   176,    25,   176,    -1,   176,    24,
     176,    -1,   176,    23,   176,    -1,   176,    21,   176,    -1,
     176,    22,   176,    -1,    18,   176,    -1,    17,   176,    -1,
     175,     6,   175,    -1,   175,     5,   175,    -1,   176,     8,
     176,    -1,   176,    10,   176,    -1,   176,    15,   176,    -1,
     176,     9,   176,    -1,   176,    13,   176,    -1,   176,    12,
     176,    -1,   176,    14,   176,    -1,   176,    11,   176,    -1,
     176,    16,   176,    -1,   177,    -1,   208,   185,   186,    -1,
      -1,   177,    -1,   208,   187,   186,    -1,    -1,    37,   173,
      -1,    37,   173,    -1,    84,   195,    85,    -1,    84,   195,
      85,    -1,   192,    -1,    82,   195,    83,    -1,    82,    48,
      83,    -1,   196,    -1,   196,    81,   195,    -1,    -1,   177,
      -1,    35,    -1,   177,    -1,    49,   193,    -1,    65,   192,
      -1,    77,    -1,    78,    -1,    28,    -1,    28,   176,    -1,
     176,    28,    -1,   176,    28,   176,    -1,   176,    84,   175,
      85,   176,    -1,    59,    84,   170,    85,    -1,   180,    -1,
      19,   199,    -1,    20,   199,    -1,    -1,   176,    -1,   198,
      -1,     8,    -1,    14,    -1,    13,    -1,    38,    -1,    35,
      -1,    -1,   201,    79,    -1,   206,    86,    -1,    55,    86,
      -1,    65,    86,    -1,    -1,   201,    -1,    87,    -1,   209,
     224,    79,    -1,   209,    71,    79,    -1,    88,    -1,   211,
      79,    -1,   212,    -1,   212,    81,   211,    -1,    -1,   216,
     213,   214,   215,    -1,    -1,   218,    -1,   224,    -1,    -1,
     217,   216,    -1,    -1,    73,    -1,    72,    -1,    -1,   223,
     219,   222,    -1,     4,    -1,     3,    -1,     8,    -1,   220,
      -1,   177,    -1,    35,   221,    -1,   194,   220,    -1,   175,
      -1,    74,   226,    -1,    75,   226,    -1,    76,   226,    -1,
      -1,    38,    -1,    39,    -1,   217,    -1,    40,     4,   175,
      -1,    69,   177,    -1,    70,   177,    -1
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
     570,   574,   577,   580,   584,   588,   592,   596,   600,   604,
     608,   612,   616,   620,   624,   626,   630,   631,   633,   637,
     638,   641,   644,   648,   652,   654,   658,   662,   664,   668,
     669,   671,   673,   675,   678,   681,   683,   685,   687,   690,
     693,   697,   703,   708,   710,   713,   716,   717,   719,   721,
     723,   725,   727,   729,   731,   732,   735,   738,   741,   744,
     745,   747,   749,   753,   757,   759,   762,   764,   768,   769,
     774,   775,   777,   779,   780,   783,   784,   786,   788,   789,
     793,   795,   797,   799,   801,   803,   806,   809,   811,   814,
     817,   820,   821,   823,   825,   827,   831,   834
  };

  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
  const unsigned short int
  CMPL_Parser::yyrline_[] =
  {
         0,   196,   196,   197,   202,   203,   204,   205,   212,   212,
     216,   217,   218,   218,   224,   223,   341,   340,   408,   409,
     410,   412,   413,   416,   420,   436,   437,   441,   442,   446,
     447,   448,   449,   450,   454,   455,   459,   462,   463,   466,
     476,   476,   480,   481,   482,   482,   487,   488,   489,   490,
     492,   495,   496,   499,   504,   509,   510,   511,   515,   516,
     522,   541,   544,   545,   552,   552,   556,   557,   558,   558,
     563,   563,   566,   567,   568,   570,   574,   575,   576,   580,
     583,   584,   591,   591,   595,   596,   597,   597,   602,   603,
     604,   605,   607,   611,   611,   622,   623,   627,   630,   631,
     632,   635,   638,   639,   642,   643,   646,   647,   652,   653,
     656,   657,   657,   661,   662,   667,   667,   673,   675,   679,
     682,   685,   690,   691,   695,   698,   699,   707,   708,   709,
     711,   712,   713,   722,   724,   727,   732,   743,   749,   749,
     753,   753,   757,   762,   763,   768,   772,   775,   779,   783,
     787,   795,   815,   816,   817,   820,   821,   825,   829,   845,
     864,   865,   865,   870,   874,   877,   893,   919,   945,   946,
     952,   956,   960,   966,   969,  1006,  1007,  1014,  1015,  1016,
    1017,  1024,  1025,  1026,  1027,  1028,  1029,  1036,  1041,  1041,
    1049,  1049,  1061,  1063,  1065,  1067,  1069,  1071,  1073,  1075,
    1077,  1080,  1084,  1100,  1104,  1111,  1113,  1115,  1117,  1119,
    1121,  1126,  1128,  1133,  1160,  1166,  1168,  1171,  1177,  1179,
    1198,  1228,  1253,  1282,  1302,  1310,  1318,  1323,  1324,  1333,
    1337,  1344,  1354,  1357,  1396,  1435,  1437,  1439,  1441,  1445,
    1449,  1454,  1459,  1469,  1470,  1472,  1476,  1478,  1479,  1484,
    1485,  1486,  1491,  1495,  1500,  1503,  1514,  1519,  1523,  1527,
    1528,  1532,  1537,  1541,  1547,  1567,  1570,  1571,  1574,  1574,
    1580,  1581,  1582,  1599,  1611,  1612,  1615,  1616,  1621,  1621,
    1653,  1654,  1657,  1658,  1661,  1666,  1669,  1679,  1686,  1688,
    1690,  1694,  1695,  1696,  1697,  1701,  1716,  1719
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
       2,     2,     2,    26,     2,     2,     2,    21,     2,     2,
      84,    85,    19,    17,    81,    18,     2,    20,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    79,    80,
       2,     8,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    82,     2,    83,    25,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    86,    88,    87,     2,     2,     2,     2,
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
      16,    22,    23,    24,    27,    28,    29,    30,    31,    32,
      33,    34,    35,    36,    37,    38,    39,    40,    41,    42,
      43,    44,    45,    46,    47,    48,    49,    50,    51,    52,
      53,    54,    55,    56,    57,    58,    59,    60,    61,    62,
      63,    64,    65,    66,    67,    68,    69,    70,    71,    72,
      73,    74,    75,    76,    77,    78
    };
    if ((unsigned int) t <= yyuser_token_number_max_)
      return translate_table[t];
    else
      return yyundef_token_;
  }

  const int CMPL_Parser::yyeof_ = 0;
  const int CMPL_Parser::yylast_ = 1838;
  const int CMPL_Parser::yynnts_ = 140;
  const int CMPL_Parser::yyempty_ = -2;
  const int CMPL_Parser::yyfinal_ = 15;
  const int CMPL_Parser::yyterror_ = 1;
  const int CMPL_Parser::yyerrcode_ = 256;
  const int CMPL_Parser::yyntokens_ = 89;

  const unsigned int CMPL_Parser::yyuser_token_number_max_ = 325;
  const CMPL_Parser::token_number_type CMPL_Parser::yyundef_token_ = 2;

} // namespace cmpl

#line 1725 "src/cmpl/CMPL_Parser.yy"


void cmpl::CMPL_Parser::error(const cmpl::CMPL_Parser::location_type& l, const std::string& m)
{
	PARSE_CONTEXT.output_error_parse(ERROR_TYPE_COMP, ERROR_LVL_NORMAL, m.c_str());
}


