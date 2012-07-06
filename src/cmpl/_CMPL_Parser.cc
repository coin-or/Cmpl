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
#line 195 "src/cmpl/CMPL_Parser.yy"
    { COMP(PROG_END, 0); ;}
    break;

  case 8:
#line 211 "src/cmpl/CMPL_Parser.yy"
    { GLV->sect = (int) token::SECT_PARAM; ;}
    break;

  case 12:
#line 217 "src/cmpl/CMPL_Parser.yy"
    { yyerrok; ;}
    break;

  case 14:
#line 223 "src/cmpl/CMPL_Parser.yy"
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
#line 272 "src/cmpl/CMPL_Parser.yy"
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
#line 339 "src/cmpl/CMPL_Parser.yy"
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
#line 355 "src/cmpl/CMPL_Parser.yy"
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
#line 414 "src/cmpl/CMPL_Parser.yy"
    { (yyval.exp) = (yysemantic_stack_[(1) - (1)].exp);
		   										;}
    break;

  case 24:
#line 418 "src/cmpl/CMPL_Parser.yy"
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
#line 434 "src/cmpl/CMPL_Parser.yy"
    { (yyval.exp) = (yysemantic_stack_[(1) - (1)].exp); ;}
    break;

  case 26:
#line 435 "src/cmpl/CMPL_Parser.yy"
    { (yyval.exp) = (yysemantic_stack_[(1) - (1)].exp); ;}
    break;

  case 27:
#line 439 "src/cmpl/CMPL_Parser.yy"
    { (yyval.t) = TPP_EMPTY; ;}
    break;

  case 28:
#line 440 "src/cmpl/CMPL_Parser.yy"
    { (yyval.t) = (yysemantic_stack_[(2) - (2)].t); ;}
    break;

  case 29:
#line 444 "src/cmpl/CMPL_Parser.yy"
    { (yyval.t) = TP_DBL; ;}
    break;

  case 30:
#line 445 "src/cmpl/CMPL_Parser.yy"
    { (yyval.t) = TP_INT; ;}
    break;

  case 31:
#line 446 "src/cmpl/CMPL_Parser.yy"
    { (yyval.t) = TP_BIN; ;}
    break;

  case 32:
#line 447 "src/cmpl/CMPL_Parser.yy"
    { (yyval.t) = TP_STR; ;}
    break;

  case 33:
#line 448 "src/cmpl/CMPL_Parser.yy"
    { (yyval.t) = TPK_PROBLEM_MAIN; ;}
    break;

  case 35:
#line 453 "src/cmpl/CMPL_Parser.yy"
    { PARSE_CONTEXT.out_warn_old_assign(false); ;}
    break;

  case 39:
#line 464 "src/cmpl/CMPL_Parser.yy"
    { COMP_PAR_INT(LIT_VAL, (yysemantic_stack_[(2) - (2)].exp).dim_in);	/*TODO: Anzahl Elemente als besonderer Typ, zur Kennzeichnung das Array*/
												  // Array-Parameter sind derzeit nicht erlaubt
												  if ((yysemantic_stack_[(2) - (2)].exp).dim_elem != 0)
													  PARSE_CONTEXT.output_error_parse(ERROR_TYPE_COMP, ERROR_LVL_NORMAL, "array parameters are not allowed");	/*TODO*/
												  COMP_SUB(CMPL_SOLVE, CMPL_PROBLEM_SOLVE); ;}
    break;

  case 40:
#line 474 "src/cmpl/CMPL_Parser.yy"
    { GLV->sect = (int) token::SECT_VAR; ;}
    break;

  case 44:
#line 480 "src/cmpl/CMPL_Parser.yy"
    { yyerrok; ;}
    break;

  case 46:
#line 485 "src/cmpl/CMPL_Parser.yy"
    { COMP_SUB(DEF_VAR_TYPE, (yysemantic_stack_[(4) - (3)].i)); ;}
    break;

  case 53:
#line 497 "src/cmpl/CMPL_Parser.yy"
    { COMP_PAR(LIT_VAL_SHORT, (yysemantic_stack_[(1) - (1)].exp).dim_in);
												  COMP_SUB(SET_INDEXES, IND_GIVEN);
												  PARSE_CONTEXT.init_symbol(&(yysemantic_stack_[(1) - (1)].exp), PTR_TP_VAR, TPK_VAR, (yysemantic_stack_[(1) - (1)].exp).sym_dim, false);
												  COMP(DEF_VAR, 1);
												  COMP_INT((yysemantic_stack_[(1) - (1)].exp).symp()->symno * MAX_ANZ_IND_1 + (yysemantic_stack_[(1) - (1)].exp).sym_dim); ;}
    break;

  case 54:
#line 502 "src/cmpl/CMPL_Parser.yy"
    { COMP(DEF_VAR, 1);
												  COMP_INT((yysemantic_stack_[(1) - (1)].exp).symp()->symno * MAX_ANZ_IND_1 + (yysemantic_stack_[(1) - (1)].exp).sym_dim); ;}
    break;

  case 55:
#line 507 "src/cmpl/CMPL_Parser.yy"
    { (yyval.i) = (int)TP_VAR_DBL; ;}
    break;

  case 56:
#line 508 "src/cmpl/CMPL_Parser.yy"
    { (yyval.i) = (int)TP_VAR_INT; ;}
    break;

  case 57:
#line 509 "src/cmpl/CMPL_Parser.yy"
    { (yyval.i) = (int)TP_VAR_BIN; ;}
    break;

  case 58:
#line 513 "src/cmpl/CMPL_Parser.yy"
    { COMP(RESET_VAR_B, 0); ;}
    break;

  case 59:
#line 514 "src/cmpl/CMPL_Parser.yy"
    { COMP(RESET_VAR_B, 0);
												  if((yysemantic_stack_[(5) - (4)].f)) COMP(DEF_UP_VAR, 0);
												  if((yysemantic_stack_[(5) - (2)].f)) COMP(DEF_DN_VAR, 0); ;}
    break;

  case 60:
#line 523 "src/cmpl/CMPL_Parser.yy"
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
#line 550 "src/cmpl/CMPL_Parser.yy"
    { GLV->sect = (int) token::SECT_OBJ; ;}
    break;

  case 68:
#line 556 "src/cmpl/CMPL_Parser.yy"
    { yyerrok; ;}
    break;

  case 70:
#line 561 "src/cmpl/CMPL_Parser.yy"
    { GLV->is_left = true; ;}
    break;

  case 71:
#line 562 "src/cmpl/CMPL_Parser.yy"
    { COMP_PAR(LINE_MOD, MODE_N);
												  COMP(LINE_END, 0); ;}
    break;

  case 77:
#line 573 "src/cmpl/CMPL_Parser.yy"
    { COMP_PAR(LINE_OBJ_TO, 1); ;}
    break;

  case 78:
#line 574 "src/cmpl/CMPL_Parser.yy"
    { COMP_PAR(LINE_OBJ_TO, -1); ;}
    break;

  case 82:
#line 589 "src/cmpl/CMPL_Parser.yy"
    { GLV->sect = (int) token::SECT_CSTR; ;}
    break;

  case 86:
#line 595 "src/cmpl/CMPL_Parser.yy"
    { yyerrok; ;}
    break;

  case 88:
#line 600 "src/cmpl/CMPL_Parser.yy"
    { COMP(LINE_END, 0); ;}
    break;

  case 93:
#line 609 "src/cmpl/CMPL_Parser.yy"
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
#line 633 "src/cmpl/CMPL_Parser.yy"
    { (yyval.i) = 1; ;}
    break;

  case 102:
#line 636 "src/cmpl/CMPL_Parser.yy"
    { (yyval.i) = 1; ;}
    break;

  case 103:
#line 637 "src/cmpl/CMPL_Parser.yy"
    { (yyval.i) = -1; ;}
    break;

  case 111:
#line 655 "src/cmpl/CMPL_Parser.yy"
    { COMP(TERM_NEG, 0); /* Negation */ ;}
    break;

  case 112:
#line 656 "src/cmpl/CMPL_Parser.yy"
    { COMP(TERM_NEG, 0); /* Negation (um Negation am Anfang zurueckzunehmen) */ ;}
    break;

  case 115:
#line 665 "src/cmpl/CMPL_Parser.yy"
    { COMP(TERM_BEG, 0);
												  COMP_PAR(LIT_VAL_SHORT, (yysemantic_stack_[(0) - (0)].i)); /* Vorzeichen Term */ ;}
    break;

  case 116:
#line 667 "src/cmpl/CMPL_Parser.yy"
    { COMP(TERM_END, 0);
												  PARSE_CONTEXT.var_test_prod((yysemantic_stack_[(2) - (2)].i)); ;}
    break;

  case 117:
#line 671 "src/cmpl/CMPL_Parser.yy"
    { if((yysemantic_stack_[(1) - (1)].i)==0) COMP_SUB(NUM_OP, MULT);
												  (yyval.i) = (yysemantic_stack_[(1) - (1)].i); ;}
    break;

  case 118:
#line 673 "src/cmpl/CMPL_Parser.yy"
    { if((yysemantic_stack_[(3) - (1)].i)==0) COMP_SUB(NUM_OP, MULT);
												  (yyval.i) = (yysemantic_stack_[(3) - (1)].i) + (yysemantic_stack_[(3) - (3)].i); ;}
    break;

  case 119:
#line 677 "src/cmpl/CMPL_Parser.yy"
    { COMP(TERM_VAR_FREE, 1);
												  COMP_INT((yysemantic_stack_[(2) - (1)].exp).symp()->symno * MAX_ANZ_IND_1 + (yysemantic_stack_[(2) - (1)].exp).sym_dim);
												  (yyval.i) = 1; ;}
    break;

  case 120:
#line 680 "src/cmpl/CMPL_Parser.yy"
    { if ((yysemantic_stack_[(1) - (1)].exp).dim_t_undef > 0)
													PARSE_CONTEXT.output_error_parse(ERROR_TYPE_COMP, ERROR_LVL_NORMAL, "undefined symbol");/*TODO*/
												  (yyval.i) = 0; ;}
    break;

  case 121:
#line 683 "src/cmpl/CMPL_Parser.yy"
    { COMP_SUB_PAR_CNT(F_CONST_FREE, SYM_MAIN, 0, 1);
												  COMP_INT((yysemantic_stack_[(2) - (1)].exp).symp()->symno * MAX_ANZ_IND_1 + (yysemantic_stack_[(2) - (1)].exp).sym_dim);
												  (yyval.i) = 0; ;}
    break;

  case 123:
#line 689 "src/cmpl/CMPL_Parser.yy"
    { COMP(IND_TRANSPOSE, 0); ;}
    break;

  case 127:
#line 705 "src/cmpl/CMPL_Parser.yy"
    { (yyval.exp) = (yysemantic_stack_[(1) - (1)].exp); ;}
    break;

  case 128:
#line 706 "src/cmpl/CMPL_Parser.yy"
    { (yyval.exp) = (yysemantic_stack_[(1) - (1)].exp); ;}
    break;

  case 129:
#line 707 "src/cmpl/CMPL_Parser.yy"
    { (yyval.exp) = (yysemantic_stack_[(1) - (1)].exp); ;}
    break;

  case 130:
#line 709 "src/cmpl/CMPL_Parser.yy"
    { (yyval.exp) = (yysemantic_stack_[(1) - (1)].exp); ;}
    break;

  case 131:
#line 710 "src/cmpl/CMPL_Parser.yy"
    { (yyval.exp) = (yysemantic_stack_[(1) - (1)].exp); ;}
    break;

  case 132:
#line 711 "src/cmpl/CMPL_Parser.yy"
    { COMP(FETCH_TMP, 1);
												  //COMP_INT((-$1-2));
												  COMP_INT((yysemantic_stack_[(1) - (1)].exp).symp()->symno);
												  (yyval.exp).set(/*TPK_VALUE*/ (yysemantic_stack_[(1) - (1)].exp).symp()->tpk, 0); ;}
    break;

  case 133:
#line 720 "src/cmpl/CMPL_Parser.yy"
    { COMP_PAR_INT(LIT_VAL, (yysemantic_stack_[(1) - (1)].i));
												  (yyval.exp).set(TP_INT, 0); ;}
    break;

  case 134:
#line 722 "src/cmpl/CMPL_Parser.yy"
    { COMP(LIT_VAL, 1);
												  COMP_DOUBLE((yysemantic_stack_[(1) - (1)].d));
												  (yyval.exp).set(TP_DBL, 0); ;}
    break;

  case 135:
#line 725 "src/cmpl/CMPL_Parser.yy"
    { COMP(LIT_VAL, 1);
												  COMP_ISTR((yysemantic_stack_[(1) - (1)].i));
												  (yyval.exp).set(TP_ISTR, 0); ;}
    break;

  case 136:
#line 730 "src/cmpl/CMPL_Parser.yy"
    { /* Anzahl Elemente als Listenzähler auf den Stack */
		  										  COMP(LIT_VAL, 1);
												  COMP_VAL(TPP_LIST_VAL, i, (yysemantic_stack_[(1) - (1)].exp).dim_in);
												  /* Tupelkonstruktion */
												  COMP(CONSTRUCT_TUPEL, 0);
												  (yyval.exp).set(((yysemantic_stack_[(1) - (1)].exp).dim_out > 0 ? TPK_TUPELSET_N((yysemantic_stack_[(1) - (1)].exp).dim_t_rank1) : TPK_TUPEL_N((yysemantic_stack_[(1) - (1)].exp).dim_t_rank1)), 0);
												  (yyval.exp).lval = (yysemantic_stack_[(1) - (1)].exp).lval;
												  (yyval.exp).dim_t_undef = (yysemantic_stack_[(1) - (1)].exp).dim_t_undef; ;}
    break;

  case 137:
#line 740 "src/cmpl/CMPL_Parser.yy"
    { if ((yysemantic_stack_[(4) - (3)].exp).dim_t_undef > 0)
												  	PARSE_CONTEXT.output_error_parse(ERROR_TYPE_COMP, ERROR_LVL_NORMAL, "undefined symbol");/*TODO*/
												  COMP_SUB_PAR(CONV_OP, NO_FORMAT, (yysemantic_stack_[(4) - (1)].t));
												  (yyval.exp).set((yysemantic_stack_[(4) - (1)].t), 0); ;}
    break;

  case 138:
#line 746 "src/cmpl/CMPL_Parser.yy"
    { (yysemantic_stack_[(2) - (2)].i) = (int) token::FCT_MAX; ;}
    break;

  case 139:
#line 747 "src/cmpl/CMPL_Parser.yy"
    { COMP_PAR_INT(LIT_VAL, (yysemantic_stack_[(5) - (4)].i));
												  COMP_SUB(AGGR_OP, MAX_VAL);
												  (yyval.exp).set(TPK_VALUE, 0); ;}
    break;

  case 140:
#line 750 "src/cmpl/CMPL_Parser.yy"
    { (yysemantic_stack_[(2) - (2)].i) = (int) token::FCT_MIN; ;}
    break;

  case 141:
#line 751 "src/cmpl/CMPL_Parser.yy"
    { COMP_PAR_INT(LIT_VAL, (yysemantic_stack_[(5) - (4)].i));
												  COMP_SUB(AGGR_OP, MIN_VAL);
												  (yyval.exp).set(TPK_VALUE, 0); ;}
    break;

  case 142:
#line 754 "src/cmpl/CMPL_Parser.yy"
    { /*COMP_PAR(LIT_VAL_SHORT, $3.dim_in);
												  COMP_SUB(SET_INDEXES, IND_GIVEN);*/
												  COMP_SUB_PAR_CNT(DEFSET_EXTNUM, MAX_VAL, 0, 1);
												  COMP_INT((yysemantic_stack_[(4) - (3)].exp).symp()->symno * MAX_ANZ_IND_1 + (yysemantic_stack_[(4) - (3)].exp).sym_dim);
												  (yyval.exp).set(TP_INT, 0); ;}
    break;

  case 143:
#line 759 "src/cmpl/CMPL_Parser.yy"
    { (yyval.exp).set(TP_INT, 0); ;}
    break;

  case 144:
#line 761 "src/cmpl/CMPL_Parser.yy"
    { if ((yysemantic_stack_[(6) - (3)].exp).dim_t_undef > 0 || (yysemantic_stack_[(6) - (5)].exp).dim_t_undef > 0)
													PARSE_CONTEXT.output_error_parse(ERROR_TYPE_COMP, ERROR_LVL_NORMAL, "undefined symbol");/*TODO*/
												  COMP_SUB_PAR(CONV_OP, WITH_FORMAT, TP_ISTR);
												  (yyval.exp).set(TP_ISTR, 0); ;}
    break;

  case 145:
#line 765 "src/cmpl/CMPL_Parser.yy"
    { if ((yysemantic_stack_[(4) - (3)].exp).dim_t_undef > 0)
													PARSE_CONTEXT.output_error_parse(ERROR_TYPE_COMP, ERROR_LVL_NORMAL, "undefined symbol");/*TODO*/
												  COMP_SUB(TYPE_STR, TYPE_EXPR);
												  (yyval.exp).set(TP_ISTR, 0); ;}
    break;

  case 146:
#line 769 "src/cmpl/CMPL_Parser.yy"
    { COMP_SUB_PAR_CNT(TYPE_STR, TYPE_VAR, 0, 1);
												  COMP_INT((yysemantic_stack_[(4) - (3)].exp).symp()->symno * MAX_ANZ_IND_1 + (yysemantic_stack_[(4) - (3)].exp).sym_dim);
												  (yyval.exp).set(TP_ISTR, 0); ;}
    break;

  case 147:
#line 772 "src/cmpl/CMPL_Parser.yy"
    { if ((yysemantic_stack_[(4) - (3)].exp).dim_t_undef > 0)
                                                                                                  PARSE_CONTEXT.output_error_parse(ERROR_TYPE_COMP, ERROR_LVL_NORMAL, "undefined symbol");/*TODO*/
												  COMP_SUB(UTIL_OP, LEN_VAL);
												  (yyval.exp).set(TP_INT, 0); ;}
    break;

  case 148:
#line 776 "src/cmpl/CMPL_Parser.yy"
    { if ((yysemantic_stack_[(4) - (3)].exp).dim_t_undef > 0)
													PARSE_CONTEXT.output_error_parse(ERROR_TYPE_COMP, ERROR_LVL_NORMAL, "undefined symbol");/*TODO*/
												  COMP_SUB(MATH_FUNC, (yysemantic_stack_[(4) - (1)].i));
												  (yyval.exp).set(TPU_NUM, 0); ;}
    break;

  case 149:
#line 780 "src/cmpl/CMPL_Parser.yy"
    { if ((yysemantic_stack_[(4) - (3)].exp).dim_t_undef > 0)
													PARSE_CONTEXT.output_error_parse(ERROR_TYPE_COMP, ERROR_LVL_NORMAL, "undefined symbol");/*TODO*/
												  COMP(READ_STDIN, 0);
												  (yyval.exp).set(TPK_VALUE, 0); ;}
    break;

  case 150:
#line 784 "src/cmpl/CMPL_Parser.yy"
    { COMP_PAR_INT(LIT_VAL, (yysemantic_stack_[(2) - (2)].exp).dim_in); /*TODO: Anzahl Elemente als besonderer Typ, zur Kennzeichnung das Array*/
												  // Array-Parameter sind derzeit nicht erlaubt
												  if ((yysemantic_stack_[(2) - (2)].exp).dim_elem != 0)
													  PARSE_CONTEXT.output_error_parse(ERROR_TYPE_COMP, ERROR_LVL_NORMAL, "array parameters are not allowed");	/*TODO*/
												  COMP_SUB(CMPL_SOLVE, (yysemantic_stack_[(2) - (1)].i));
												  (yyval.exp).set(TP_INT, 0); ;}
    break;

  case 151:
#line 793 "src/cmpl/CMPL_Parser.yy"
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
#line 812 "src/cmpl/CMPL_Parser.yy"
    { (yyval.exp) = (yysemantic_stack_[(1) - (1)].exp); ;}
    break;

  case 153:
#line 813 "src/cmpl/CMPL_Parser.yy"
    { (yyval.exp) = (yysemantic_stack_[(1) - (1)].exp); ;}
    break;

  case 154:
#line 814 "src/cmpl/CMPL_Parser.yy"
    { (yyval.exp) = (yysemantic_stack_[(1) - (1)].exp); ;}
    break;

  case 155:
#line 817 "src/cmpl/CMPL_Parser.yy"
    { (yyval.i) = (yysemantic_stack_[(1) - (1)].i); ;}
    break;

  case 156:
#line 818 "src/cmpl/CMPL_Parser.yy"
    { (yyval.i) = (yysemantic_stack_[(1) - (1)].i); ;}
    break;

  case 157:
#line 822 "src/cmpl/CMPL_Parser.yy"
    { (yyval.i) = 0; ;}
    break;

  case 158:
#line 826 "src/cmpl/CMPL_Parser.yy"
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
#line 842 "src/cmpl/CMPL_Parser.yy"
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
#line 861 "src/cmpl/CMPL_Parser.yy"
    { (yyval.i) = 1; ;}
    break;

  case 161:
#line 862 "src/cmpl/CMPL_Parser.yy"
    { (yysemantic_stack_[(2) - (2)].i) = (yysemantic_stack_[(2) - (-1)].i); ;}
    break;

  case 162:
#line 863 "src/cmpl/CMPL_Parser.yy"
    { (yyval.i) = (yysemantic_stack_[(4) - (4)].i) + 1; ;}
    break;

  case 163:
#line 867 "src/cmpl/CMPL_Parser.yy"
    { (yyval.i) = 0; ;}
    break;

  case 164:
#line 871 "src/cmpl/CMPL_Parser.yy"
    { if ((yysemantic_stack_[(1) - (1)].exp).dim_t_undef > 0)
													PARSE_CONTEXT.output_error_parse(ERROR_TYPE_COMP, ERROR_LVL_NORMAL, "undefined symbol");/*TODO*/
												;}
    break;

  case 165:
#line 874 "src/cmpl/CMPL_Parser.yy"
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
#line 891 "src/cmpl/CMPL_Parser.yy"
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
#line 916 "src/cmpl/CMPL_Parser.yy"
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
#line 942 "src/cmpl/CMPL_Parser.yy"
    { (yyval.exp) = (yysemantic_stack_[(1) - (1)].exp); ;}
    break;

  case 169:
#line 943 "src/cmpl/CMPL_Parser.yy"
    { (yyval.exp) = (yysemantic_stack_[(1) - (1)].exp); ;}
    break;

  case 170:
#line 949 "src/cmpl/CMPL_Parser.yy"
    { /*COMP_PAR(LIT_VAL_SHORT, $1.dim_in);
												  COMP_SUB(SET_INDEXES, IND_GIVEN);*/
												  COMP(DEF_FUNC, 1);
												  COMP_INT((yysemantic_stack_[(1) - (1)].exp).symp()->symno * MAX_ANZ_IND_1 + (yysemantic_stack_[(1) - (1)].exp).sym_dim); ;}
    break;

  case 171:
#line 953 "src/cmpl/CMPL_Parser.yy"
    { COMP_PAR(LIT_VAL_SHORT, (yysemantic_stack_[(2) - (2)].exp).dim_in);
												  COMP_SUB(SET_INDEXES, IND_GIVEN);
												  COMP(DEL_INDEXES, 0);
												  COMP_PAR(LIT_VAL_SHORT, 0); ;}
    break;

  case 172:
#line 957 "src/cmpl/CMPL_Parser.yy"
    { COMP_PAR(LIT_VAL_SHORT, 1); ;}
    break;

  case 173:
#line 963 "src/cmpl/CMPL_Parser.yy"
    { /*COMP_PAR(LIT_VAL_SHORT, 0);*/
										  (yyval.exp).set_dim(0, 0, 0);
										  /*$$ = 0;*/ ;}
    break;

  case 174:
#line 966 "src/cmpl/CMPL_Parser.yy"
    { (yyval.exp) = (yysemantic_stack_[(1) - (1)].exp);
										  // Undefinierte Symbole hier nicht erlaubt
										  if ((yysemantic_stack_[(1) - (1)].exp).dim_t_undef > 0)
											PARSE_CONTEXT.output_error_parse(ERROR_TYPE_COMP, ERROR_LVL_NORMAL, "undefined symbols in list");/*TODO*/
										;}
    break;

  case 175:
#line 1003 "src/cmpl/CMPL_Parser.yy"
    { (yyval.f) = false; ;}
    break;

  case 176:
#line 1004 "src/cmpl/CMPL_Parser.yy"
    { if ((yysemantic_stack_[(1) - (1)].exp).dim_t_undef > 0)
											PARSE_CONTEXT.output_error_parse(ERROR_TYPE_COMP, ERROR_LVL_NORMAL, "undefined symbol");/*TODO*/
										  (yyval.f) = true; ;}
    break;

  case 177:
#line 1011 "src/cmpl/CMPL_Parser.yy"
    { (yyval.exp) = (yysemantic_stack_[(1) - (1)].exp); ;}
    break;

  case 178:
#line 1012 "src/cmpl/CMPL_Parser.yy"
    { (yyval.exp) = (yysemantic_stack_[(1) - (1)].exp); ;}
    break;

  case 179:
#line 1013 "src/cmpl/CMPL_Parser.yy"
    { (yyval.exp) = (yysemantic_stack_[(1) - (1)].exp); ;}
    break;

  case 180:
#line 1014 "src/cmpl/CMPL_Parser.yy"
    { (yyval.exp) = (yysemantic_stack_[(1) - (1)].exp); ;}
    break;

  case 181:
#line 1021 "src/cmpl/CMPL_Parser.yy"
    { (yyval.exp) = (yysemantic_stack_[(1) - (1)].exp); ;}
    break;

  case 182:
#line 1022 "src/cmpl/CMPL_Parser.yy"
    { (yyval.exp) = (yysemantic_stack_[(1) - (1)].exp); ;}
    break;

  case 183:
#line 1023 "src/cmpl/CMPL_Parser.yy"
    { (yyval.exp) = (yysemantic_stack_[(1) - (1)].exp); ;}
    break;

  case 184:
#line 1024 "src/cmpl/CMPL_Parser.yy"
    { (yyval.exp) = (yysemantic_stack_[(1) - (1)].exp); ;}
    break;

  case 185:
#line 1025 "src/cmpl/CMPL_Parser.yy"
    { (yyval.exp) = (yysemantic_stack_[(1) - (1)].exp); ;}
    break;

  case 186:
#line 1026 "src/cmpl/CMPL_Parser.yy"
    { if ((yysemantic_stack_[(2) - (2)].exp).dim_t_undef > 0)
													PARSE_CONTEXT.output_error_parse(ERROR_TYPE_COMP, ERROR_LVL_NORMAL, "undefined symbol");/*TODO*/
												  COMP_SUB(LOG_OP, NOT);
												  (yyval.exp).set(TP_BIN, 0); ;}
    break;

  case 187:
#line 1033 "src/cmpl/CMPL_Parser.yy"
    { (yyval.exp) = (yysemantic_stack_[(1) - (1)].exp); ;}
    break;

  case 188:
#line 1038 "src/cmpl/CMPL_Parser.yy"
    { COMP(LIT_VAL, 1);
												  COMP_VAL(TPP_NULL, i, 0); ;}
    break;

  case 189:
#line 1041 "src/cmpl/CMPL_Parser.yy"
    { EXPSTR a, b = PARSE_CONTEXT.parse_type_dev_switch(&(yysemantic_stack_[(6) - (4)].exp), &(yysemantic_stack_[(6) - (5)].exp));
												  a.set(TPP_NULL, 0);
												  (yyval.exp) = PARSE_CONTEXT.parse_type_dev_num_op(ADD, &a, &b); ;}
    break;

  case 190:
#line 1046 "src/cmpl/CMPL_Parser.yy"
    { COMP_PAR(LIT_VAL_SHORT, 0); ;}
    break;

  case 191:
#line 1048 "src/cmpl/CMPL_Parser.yy"
    { EXPSTR a, b = PARSE_CONTEXT.parse_type_dev_switch(&(yysemantic_stack_[(6) - (4)].exp), &(yysemantic_stack_[(6) - (5)].exp));
												  a.set(TP_INT, 0);
												  (yyval.exp) = PARSE_CONTEXT.parse_type_dev_num_op(ADD, &a, &b);
												  if (TYPECLASS((yyval.exp).tp) != TPK_VALUE)
													PARSE_CONTEXT.output_error_parse(ERROR_TYPE_COMP, ERROR_LVL_NORMAL, ERROR_NOT_A_NUMBER_PARSE);
												  COMP_PAR(LIT_SET, TP_SET_ENUM);
												  (yyval.exp).set(TPK_SET, 0); ;}
    break;

  case 192:
#line 1058 "src/cmpl/CMPL_Parser.yy"
    { COMP_SUB((TYPECLASS((yysemantic_stack_[(3) - (1)].exp).tp)==TPK_SET ? SET_OP : NUM_OP), ADD);
															  (yyval.exp) = PARSE_CONTEXT.parse_type_dev_num_op(ADD, &(yysemantic_stack_[(3) - (1)].exp), &(yysemantic_stack_[(3) - (3)].exp)); ;}
    break;

  case 193:
#line 1060 "src/cmpl/CMPL_Parser.yy"
    { COMP_SUB(NUM_OP, SUB);
															  (yyval.exp) = PARSE_CONTEXT.parse_type_dev_num_op(SUB, &(yysemantic_stack_[(3) - (1)].exp), &(yysemantic_stack_[(3) - (3)].exp)); ;}
    break;

  case 194:
#line 1062 "src/cmpl/CMPL_Parser.yy"
    { COMP_SUB((TYPECLASS((yysemantic_stack_[(3) - (1)].exp).tp)==TPK_SET ? SET_OP : NUM_OP), MULT);
															  (yyval.exp) = PARSE_CONTEXT.parse_type_dev_num_op(MULT, &(yysemantic_stack_[(3) - (1)].exp), &(yysemantic_stack_[(3) - (3)].exp)); ;}
    break;

  case 195:
#line 1064 "src/cmpl/CMPL_Parser.yy"
    { COMP_SUB(NUM_OP, DIVD);
															  (yyval.exp) = PARSE_CONTEXT.parse_type_dev_num_op(DIVD, &(yysemantic_stack_[(3) - (1)].exp), &(yysemantic_stack_[(3) - (3)].exp)); ;}
    break;

  case 196:
#line 1066 "src/cmpl/CMPL_Parser.yy"
    { COMP_SUB(NUM_OP, POW);
															  (yyval.exp) = PARSE_CONTEXT.parse_type_dev_num_op(POW, &(yysemantic_stack_[(3) - (1)].exp), &(yysemantic_stack_[(3) - (3)].exp)); ;}
    break;

  case 197:
#line 1068 "src/cmpl/CMPL_Parser.yy"
    { COMP_SUB(NUM_OP, DIVI);
															  (yyval.exp) = PARSE_CONTEXT.parse_type_dev_num_op(DIVI, &(yysemantic_stack_[(3) - (1)].exp), &(yysemantic_stack_[(3) - (3)].exp)); ;}
    break;

  case 198:
#line 1070 "src/cmpl/CMPL_Parser.yy"
    { COMP_SUB(NUM_OP, MODI);
															  (yyval.exp) = PARSE_CONTEXT.parse_type_dev_num_op(MODI, &(yysemantic_stack_[(3) - (1)].exp), &(yysemantic_stack_[(3) - (3)].exp)); ;}
    break;

  case 199:
#line 1072 "src/cmpl/CMPL_Parser.yy"
    { EXPSTR t;
															  t.set(TPP_EMPTY, 0);
															  COMP_SUB(NUM_OP, NEG);
															  (yyval.exp) = PARSE_CONTEXT.parse_type_dev_num_op(NEG, &(yysemantic_stack_[(2) - (2)].exp), &t); ;}
    break;

  case 200:
#line 1076 "src/cmpl/CMPL_Parser.yy"
    { EXPSTR t;
															  t.set(TPP_EMPTY, 0);
															  COMP_SUB_PAR(CONV_OP, NO_FORMAT, TPU_NUM);	/* fuer sichern nummerischer Wert */
															  (yyval.exp) = PARSE_CONTEXT.parse_type_dev_num_op(NEG, &(yysemantic_stack_[(2) - (2)].exp), &t); ;}
    break;

  case 201:
#line 1092 "src/cmpl/CMPL_Parser.yy"
    { if ((yysemantic_stack_[(3) - (1)].exp).dim_t_undef > 0 || (yysemantic_stack_[(3) - (3)].exp).dim_t_undef > 0)
																PARSE_CONTEXT.output_error_parse(ERROR_TYPE_COMP, ERROR_LVL_NORMAL, "undefined symbol in operation");/*TODO*/
															  COMP_SUB(LOG_OP, AND);
															  (yyval.exp).set(TP_BIN, 0); ;}
    break;

  case 202:
#line 1096 "src/cmpl/CMPL_Parser.yy"
    { if ((yysemantic_stack_[(3) - (1)].exp).dim_t_undef > 0 || (yysemantic_stack_[(3) - (3)].exp).dim_t_undef > 0)
																PARSE_CONTEXT.output_error_parse(ERROR_TYPE_COMP, ERROR_LVL_NORMAL, "undefined symbol in operation");/*TODO*/
															  COMP_SUB(LOG_OP, OR);
															  (yyval.exp).set(TP_BIN, 0); ;}
    break;

  case 203:
#line 1103 "src/cmpl/CMPL_Parser.yy"
    { COMP_SUB(CMP_OP, EQ);
															  (yyval.exp) = PARSE_CONTEXT.parse_type_dev_cmp_op(EQ, &(yysemantic_stack_[(3) - (1)].exp), &(yysemantic_stack_[(3) - (3)].exp)); ;}
    break;

  case 204:
#line 1105 "src/cmpl/CMPL_Parser.yy"
    { COMP_SUB(CMP_OP, EQ2);
															  (yyval.exp) = PARSE_CONTEXT.parse_type_dev_cmp_op(EQ2, &(yysemantic_stack_[(3) - (1)].exp), &(yysemantic_stack_[(3) - (3)].exp)); ;}
    break;

  case 205:
#line 1107 "src/cmpl/CMPL_Parser.yy"
    { COMP_SUB(CMP_OP, EQ); COMP_SUB(LOG_OP, NOT);
															  (yyval.exp) = PARSE_CONTEXT.parse_type_dev_cmp_op(EQ, &(yysemantic_stack_[(3) - (1)].exp), &(yysemantic_stack_[(3) - (3)].exp)); ;}
    break;

  case 206:
#line 1109 "src/cmpl/CMPL_Parser.yy"
    { COMP_SUB(CMP_OP, EQ2); COMP_SUB(LOG_OP, NOT);
															  (yyval.exp) = PARSE_CONTEXT.parse_type_dev_cmp_op(EQ2, &(yysemantic_stack_[(3) - (1)].exp), &(yysemantic_stack_[(3) - (3)].exp)); ;}
    break;

  case 207:
#line 1111 "src/cmpl/CMPL_Parser.yy"
    { COMP_SUB(CMP_OP, LE);
															  (yyval.exp) = PARSE_CONTEXT.parse_type_dev_cmp_op(LE, &(yysemantic_stack_[(3) - (1)].exp), &(yysemantic_stack_[(3) - (3)].exp)); ;}
    break;

  case 208:
#line 1113 "src/cmpl/CMPL_Parser.yy"
    { if (TYPECLASS((yysemantic_stack_[(3) - (1)].exp).tp) == TPK_SET)
																{ COMP_SUB(CMP_OP, GT); }	/*TODO: GT auch fuer TPK_VALUE*/
															  else
																{ COMP_SUB(CMP_OP, LE); COMP_SUB(LOG_OP, NOT); }
															  (yyval.exp) = PARSE_CONTEXT.parse_type_dev_cmp_op(GT, &(yysemantic_stack_[(3) - (1)].exp), &(yysemantic_stack_[(3) - (3)].exp)); ;}
    break;

  case 209:
#line 1118 "src/cmpl/CMPL_Parser.yy"
    { COMP_SUB(CMP_OP, GE);
															  (yyval.exp) = PARSE_CONTEXT.parse_type_dev_cmp_op(GE, &(yysemantic_stack_[(3) - (1)].exp), &(yysemantic_stack_[(3) - (3)].exp)); ;}
    break;

  case 210:
#line 1120 "src/cmpl/CMPL_Parser.yy"
    { if (TYPECLASS((yysemantic_stack_[(3) - (1)].exp).tp) == TPK_SET)
																{ COMP_SUB(CMP_OP, LT); }	/*TODO: LT auch fuer TPK_VALUE*/
															  else
																{ COMP_SUB(CMP_OP, GE); COMP_SUB(LOG_OP, NOT); }
															  (yyval.exp) = PARSE_CONTEXT.parse_type_dev_cmp_op(LT, &(yysemantic_stack_[(3) - (1)].exp), &(yysemantic_stack_[(3) - (3)].exp)); ;}
    break;

  case 211:
#line 1125 "src/cmpl/CMPL_Parser.yy"
    { COMP_SUB(CMP_OP, INSET);
															  (yyval.exp) = PARSE_CONTEXT.parse_type_dev_cmp_op(INSET, &(yysemantic_stack_[(3) - (1)].exp), &(yysemantic_stack_[(3) - (3)].exp)); ;}
    break;

  case 212:
#line 1152 "src/cmpl/CMPL_Parser.yy"
    { if ((yysemantic_stack_[(1) - (1)].exp).dim_t_undef > 0)
													  			PARSE_CONTEXT.output_error_parse(ERROR_TYPE_COMP, ERROR_LVL_NORMAL, "undefined symbol");/*TODO*/
			  												  COMP_SUB(NUM_OP, ADD);
												 			  (yyval.exp) = (yysemantic_stack_[(1) - (1)].exp); ;}
    break;

  case 213:
#line 1159 "src/cmpl/CMPL_Parser.yy"
    { (yyval.exp) = PARSE_CONTEXT.parse_type_dev_switch(&(yysemantic_stack_[(3) - (2)].exp), &(yysemantic_stack_[(3) - (3)].exp)); ;}
    break;

  case 214:
#line 1160 "src/cmpl/CMPL_Parser.yy"
    { (yyval.exp).set(TPP_EMPTY, 0); ;}
    break;

  case 215:
#line 1163 "src/cmpl/CMPL_Parser.yy"
    { if ((yysemantic_stack_[(1) - (1)].exp).dim_t_undef > 0)
													  			PARSE_CONTEXT.output_error_parse(ERROR_TYPE_COMP, ERROR_LVL_NORMAL, "undefined symbol");/*TODO*/
															  COMP_SUB(COUNT_OP, CNT_INC);
												 			  (yyval.exp) = (yysemantic_stack_[(1) - (1)].exp); ;}
    break;

  case 216:
#line 1170 "src/cmpl/CMPL_Parser.yy"
    { (yyval.exp) = PARSE_CONTEXT.parse_type_dev_switch(&(yysemantic_stack_[(3) - (2)].exp), &(yysemantic_stack_[(3) - (3)].exp)); ;}
    break;

  case 217:
#line 1171 "src/cmpl/CMPL_Parser.yy"
    { (yyval.exp).set(TPP_EMPTY, 0); ;}
    break;

  case 218:
#line 1190 "src/cmpl/CMPL_Parser.yy"
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

  case 219:
#line 1220 "src/cmpl/CMPL_Parser.yy"
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

  case 220:
#line 1245 "src/cmpl/CMPL_Parser.yy"
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
#line 1274 "src/cmpl/CMPL_Parser.yy"
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
#line 1294 "src/cmpl/CMPL_Parser.yy"
    { // Auf einfachen skalaren Funktionsparameter prüfen
													  if ((yysemantic_stack_[(1) - (1)].exp).dim_in != 1 && (yysemantic_stack_[(1) - (1)].exp).dim_elem != 0)
														PARSE_CONTEXT.output_error_parse(ERROR_TYPE_COMP, ERROR_LVL_NORMAL, "function argument must be a single scalar value");/*TODO*/
															//TODO: Prüfung besser in der Regel der Funktion, damit Fehlermeldung Funktionsname zeigen kann
													  (yyval.exp) = (yysemantic_stack_[(1) - (1)].exp);
													  (yyval.exp).set_dim(1, 0, 0, 0); ;}
    break;

  case 223:
#line 1302 "src/cmpl/CMPL_Parser.yy"
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
#line 1310 "src/cmpl/CMPL_Parser.yy"
    { (yyval.exp).tp = TPP_NULL;
													  (yyval.exp).set_dim(0, 0, 0, 0);
													  (yyval.exp).lval = false; ;}
    break;

  case 225:
#line 1315 "src/cmpl/CMPL_Parser.yy"
    { (yyval.exp) = (yysemantic_stack_[(1) - (1)].exp); ;}
    break;

  case 226:
#line 1316 "src/cmpl/CMPL_Parser.yy"
    { (yyval.exp) = PARSE_CONTEXT.parse_type_dev_array(&(yysemantic_stack_[(3) - (3)].exp), &(yysemantic_stack_[(3) - (1)].exp));	/*TODO: in parse_type_dev_array dim-Bestimmung einbeziehen */
													  (yyval.exp).set_dim((yysemantic_stack_[(3) - (1)].exp).dim_in + (yysemantic_stack_[(3) - (3)].exp).dim_in, (yysemantic_stack_[(3) - (1)].exp).dim_out + (yysemantic_stack_[(3) - (3)].exp).dim_out,
														(yysemantic_stack_[(3) - (3)].exp).dim_fr_end + ((yysemantic_stack_[(3) - (3)].exp).dim_fr_end==(yysemantic_stack_[(3) - (3)].exp).dim_in ? (yysemantic_stack_[(3) - (1)].exp).dim_fr_end : 0),
														((yysemantic_stack_[(3) - (1)].exp).dim_elem==(yysemantic_stack_[(3) - (3)].exp).dim_elem || (yysemantic_stack_[(3) - (3)].exp).dim_fr_end==(yysemantic_stack_[(3) - (3)].exp).dim_in ? (yysemantic_stack_[(3) - (1)].exp).dim_elem :
															((yysemantic_stack_[(3) - (1)].exp).dim_fr_end==(yysemantic_stack_[(3) - (1)].exp).dim_in ? (yysemantic_stack_[(3) - (3)].exp).dim_elem : -1)),
														(yysemantic_stack_[(3) - (1)].exp).dim_t_rank1 + (yysemantic_stack_[(3) - (3)].exp).dim_t_rank1, ((yysemantic_stack_[(3) - (1)].exp).dim_t_rank2 == (yysemantic_stack_[(3) - (3)].exp).dim_t_rank2 ? (yysemantic_stack_[(3) - (1)].exp).dim_t_rank2 : -1), (yysemantic_stack_[(3) - (1)].exp).dim_t_undef + (yysemantic_stack_[(3) - (3)].exp).dim_t_undef);
													  (yyval.exp).lval = ((yysemantic_stack_[(3) - (1)].exp).lval && (yysemantic_stack_[(3) - (3)].exp).lval); ;}
    break;

  case 227:
#line 1325 "src/cmpl/CMPL_Parser.yy"
    { COMP_PAR(LIT_SET, TP_SET_ALL);
                                                      (yyval.exp).tp = TPP_EMPTY;
													  (yyval.exp).set_dim(1, 1, 1, 0, 1, 1, 0);
													  (yyval.exp).lval = false; ;}
    break;

  case 228:
#line 1329 "src/cmpl/CMPL_Parser.yy"
    { // Undefinierte Symbole hier nicht erlaubt
													  if ((yysemantic_stack_[(1) - (1)].exp).dim_t_undef > 0)
													  	PARSE_CONTEXT.output_error_parse(ERROR_TYPE_COMP, ERROR_LVL_NORMAL, "undefined symbols in list");/*TODO*/
													  (yyval.exp) = (yysemantic_stack_[(1) - (1)].exp);
													  (yyval.exp).set_dim(1, ((yysemantic_stack_[(1) - (1)].exp).dim_out==0 && (TYPECLASS((yysemantic_stack_[(1) - (1)].exp).tp)==TPK_SET || TP_IS_TUPELSET((yysemantic_stack_[(1) - (1)].exp).tp)) ? 1 : 0), 0, (yysemantic_stack_[(1) - (1)].exp).dim_out,
													  	(TP_IS_TUPEL((yysemantic_stack_[(1) - (1)].exp).tp) || TP_IS_TUPELSET((yysemantic_stack_[(1) - (1)].exp).tp) ? RANK_TP_TUPEL((yysemantic_stack_[(1) - (1)].exp).tp) : 1),
														(TP_IS_TUPEL((yysemantic_stack_[(1) - (1)].exp).tp) || TP_IS_TUPELSET((yysemantic_stack_[(1) - (1)].exp).tp) ? RANK_TP_TUPEL((yysemantic_stack_[(1) - (1)].exp).tp) : 1), 0); ;}
    break;

  case 229:
#line 1336 "src/cmpl/CMPL_Parser.yy"
    { (yyval.exp) = (yysemantic_stack_[(1) - (1)].exp);
													  (yyval.exp).set_dim(1, 0, 0, 0, 1, -1, 1);
													  // Compilieren vorläufigen Zugriff auf Symbol; muss später ersetzt werden
											  		  COMP_SUB_PAR_CNT(FETCH_CONST, SYM_UNDEF_ERROR, 0, 1);
											  		  COMP_INT((yyval.exp).symp()->symno * MAX_ANZ_IND_1); ;}
    break;

  case 230:
#line 1346 "src/cmpl/CMPL_Parser.yy"
    { if ((yysemantic_stack_[(1) - (1)].exp).dim_t_undef > 0)
														PARSE_CONTEXT.output_error_parse(ERROR_TYPE_COMP, ERROR_LVL_NORMAL, "undefined symbol");/*TODO*/
													  (yyval.exp) = (yysemantic_stack_[(1) - (1)].exp); ;}
    break;

  case 231:
#line 1349 "src/cmpl/CMPL_Parser.yy"
    { (yyval.exp).set(TPK_VALUE, -1);
													  if (TYPECLASS((yysemantic_stack_[(2) - (2)].exp).tp) != TPK_VALUE)
														PARSE_CONTEXT.output_error_parse(ERROR_TYPE_COMP, ERROR_LVL_NORMAL, "wrong argument type for function");/*TODO*/
													  COMP(READ_CSV, 1);	//TODO: READ_CSV verwendet eigenständigen Parameter statt bisher eingebetteten
													  (yyval.exp).addr_dim = ADDR_COMP; 
													  COMP_INT(0); ;}
    break;

  case 232:
#line 1388 "src/cmpl/CMPL_Parser.yy"
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

  case 233:
#line 1427 "src/cmpl/CMPL_Parser.yy"
    { COMP_PAR(LIT_SET, TP_SET_ALL_NUM);
													  (yyval.exp).set(TP_SET_ALL_NUM, 0); ;}
    break;

  case 234:
#line 1429 "src/cmpl/CMPL_Parser.yy"
    { COMP_PAR(LIT_SET, TP_SET_ALL_STR);
													  (yyval.exp).set(TP_SET_ALL_STR, 0); ;}
    break;

  case 235:
#line 1431 "src/cmpl/CMPL_Parser.yy"
    { COMP_PAR(LIT_SET, TP_SET_ALL);
													  (yyval.exp).set(TP_SET_ALL, 0); ;}
    break;

  case 236:
#line 1433 "src/cmpl/CMPL_Parser.yy"
    { EXPSTR n;
													  n.set(TPP_NULL, 0);
													  (yyval.exp) = PARSE_CONTEXT.parse_type_dev_lit_set(TP_SET_HALF_UB, &(yysemantic_stack_[(2) - (2)].exp), &n, &n);
													  COMP_PAR(LIT_SET, TP_SET_HALF_UB); ;}
    break;

  case 237:
#line 1437 "src/cmpl/CMPL_Parser.yy"
    { EXPSTR n;
													  n.set(TPP_NULL, 0);
													  (yyval.exp) = PARSE_CONTEXT.parse_type_dev_lit_set(TP_SET_HALF_LB, &(yysemantic_stack_[(2) - (1)].exp), &n, &n);
													  COMP_PAR(LIT_SET, TP_SET_HALF_LB); ;}
    break;

  case 238:
#line 1442 "src/cmpl/CMPL_Parser.yy"
    { EXPSTR t;
													  t.set(TP_INT, 0);
													  (yyval.exp) = PARSE_CONTEXT.parse_type_dev_lit_set(TP_SET_ALG, &(yysemantic_stack_[(3) - (1)].exp), &t, &(yysemantic_stack_[(3) - (3)].exp));
													  COMP_PAR(LIT_SET, TP_SET_ALG_S1); ;}
    break;

  case 239:
#line 1447 "src/cmpl/CMPL_Parser.yy"
    { (yyval.exp) = PARSE_CONTEXT.parse_type_dev_lit_set(TP_SET_ALG, &(yysemantic_stack_[(5) - (1)].exp), &(yysemantic_stack_[(5) - (3)].exp), &(yysemantic_stack_[(5) - (5)].exp));
													  COMP_PAR(LIT_SET, TP_SET_ALG); ;}
    break;

  case 240:
#line 1451 "src/cmpl/CMPL_Parser.yy"
    { /*COMP_PAR(LIT_VAL_SHORT, $3.dim_in);
													  COMP_SUB(SET_INDEXES, IND_GIVEN);*/
													  COMP(DEFSET_FUNC, 1);
													  COMP_INT((yysemantic_stack_[(4) - (3)].exp).symp()->symno * MAX_ANZ_IND_1 + (yysemantic_stack_[(4) - (3)].exp).sym_dim);
													  (yyval.exp).set(TPK_SET, 0); ;}
    break;

  case 241:
#line 1461 "src/cmpl/CMPL_Parser.yy"
    { (yyval.exp) = (yysemantic_stack_[(1) - (1)].exp); ;}
    break;

  case 242:
#line 1466 "src/cmpl/CMPL_Parser.yy"
    { (yyval.i) = EQ; ;}
    break;

  case 243:
#line 1467 "src/cmpl/CMPL_Parser.yy"
    { (yyval.i) = GE; ;}
    break;

  case 244:
#line 1468 "src/cmpl/CMPL_Parser.yy"
    { (yyval.i) = LE; ;}
    break;

  case 245:
#line 1473 "src/cmpl/CMPL_Parser.yy"
    { (yyval.exp) = (yysemantic_stack_[(1) - (1)].exp);
													  (yyval.exp).tp = TPK_VALUE;
													  if (!((yysemantic_stack_[(1) - (1)].exp).symp()->isvalid()))
														PARSE_CONTEXT.init_symbol(&(yysemantic_stack_[(1) - (1)].exp), PTR_TP_LINENAME, TPK_VALUE, 0, false); ;}
    break;

  case 246:
#line 1477 "src/cmpl/CMPL_Parser.yy"
    { (yyval.exp) = (yysemantic_stack_[(1) - (1)].exp);
													  (yyval.exp).tp = TPK_VALUE;
													  PARSE_CONTEXT.init_symbol(&(yysemantic_stack_[(1) - (1)].exp), PTR_TP_LINENAME, TPK_VALUE, 0, false); ;}
    break;

  case 247:
#line 1482 "src/cmpl/CMPL_Parser.yy"
    { COMP(LINE_BEG, 1);
													  PARSE_CONTEXT.linename_repl(GLV->last_str, (char *)0);
													  COMP_STR(GLV->last_str); ;}
    break;

  case 248:
#line 1485 "src/cmpl/CMPL_Parser.yy"
    { COMP(LINE_BEG, 1);
													  PARSE_CONTEXT.linename_repl(GLV->last_str, (yysemantic_stack_[(2) - (1)].exp).symp()->s_name);
													  COMP_STR(GLV->last_str);
													  // TODO: auch resultierende Zeilennamen als Symbol einfuegen? (bzw. in linename_repl()?)
													;}
    break;

  case 249:
#line 1496 "src/cmpl/CMPL_Parser.yy"
    { /* Naechsten loop-Kontexte initialisieren, als Basis */
												  PARSE_CONTEXT.init_loop_comp(GLV->tmp_level, true, LOOP_EXEC, ((yysemantic_stack_[(2) - (1)].exp).tp==TPP_EMPTY ? (char *)0 : (yysemantic_stack_[(2) - (1)].exp).symp()->s_name)); ;}
    break;

  case 250:
#line 1501 "src/cmpl/CMPL_Parser.yy"
    { /* Naechsten loop-Kontexte initialisieren, als Basis fuer Summe */
												  PARSE_CONTEXT.init_loop_comp(GLV->tmp_level, true, LOOP_SUM, (char *)0); ;}
    break;

  case 251:
#line 1505 "src/cmpl/CMPL_Parser.yy"
    { /* Naechsten loop-Kontexte initialisieren, als Basis fuer Set */
												  PARSE_CONTEXT.init_loop_comp(GLV->tmp_level, true, LOOP_SET, (char *)0); ;}
    break;

  case 252:
#line 1509 "src/cmpl/CMPL_Parser.yy"
    { (yyval.exp).set(TPP_EMPTY, 0); ;}
    break;

  case 253:
#line 1510 "src/cmpl/CMPL_Parser.yy"
    { (yyval.exp) = (yysemantic_stack_[(1) - (1)].exp); ;}
    break;

  case 254:
#line 1514 "src/cmpl/CMPL_Parser.yy"
    { /* Setzt loop-Kontexte bis einschliesslich naechstem Basiskontext zurueck; compiliert entsprechende Beendigungen */
												  PARSE_CONTEXT.unset_loop_comp(); ;}
    break;

  case 255:
#line 1519 "src/cmpl/CMPL_Parser.yy"
    { /* Test fuer Alternative compilieren */
												  loop_ctrl_str *loopp = GLV->tmp_table + (GLV->tmp_level - 1);
												  COMP_SUB_PAR_CNT(TEST_ALTER, TEST_ALTER_IF, GLV->tmp_level - 1, 1);
												  loopp->last_jump = COMP_ADDR(0); ;}
    break;

  case 256:
#line 1523 "src/cmpl/CMPL_Parser.yy"
    { /* Beginn Default-Alternative compilieren */
												  loop_ctrl_str *loopp = GLV->tmp_table + (GLV->tmp_level - 1);
												  COMP_SUB_PAR(TEST_ALTER, TEST_ALTER_DEF, GLV->tmp_level - 1);
												  loopp->last_jump = 0; ;}
    break;

  case 257:
#line 1529 "src/cmpl/CMPL_Parser.yy"
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

  case 261:
#line 1556 "src/cmpl/CMPL_Parser.yy"
    { /* Leeren loop-Kontext erzeugen */
												  loop_ctrl_str *loopp = PARSE_CONTEXT.set_loop_comp(START_EMPTY, 0);
												  loopp->loop_start = ADDR_COMP; ;}
    break;

  case 263:
#line 1560 "src/cmpl/CMPL_Parser.yy"
    { /* loop-Kontext muss jetzt reserviert werden, da in Zuweisung Summenschleifen vorkommen könnten */
												  loop_ctrl_str *loopp = PARSE_CONTEXT.set_loop_comp(START_ASSIGN, (yysemantic_stack_[(1) - (1)].i));
												  loopp->loop_start = ADDR_COMP; ;}
    break;

  case 265:
#line 1564 "src/cmpl/CMPL_Parser.yy"
    { /* loop-Kontext muss jetzt reserviert werden, da in loop_if Summenschleifen vorkommen könnten */
												  loop_ctrl_str *loopp = PARSE_CONTEXT.set_loop_comp(START_IF, (yysemantic_stack_[(1) - (1)].i));
												  loopp->loop_start = ADDR_COMP; ;}
    break;

  case 267:
#line 1570 "src/cmpl/CMPL_Parser.yy"
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

  case 268:
#line 1582 "src/cmpl/CMPL_Parser.yy"
    { (yyval.i) = (yysemantic_stack_[(2) - (1)].i) | (yysemantic_stack_[(2) - (2)].i); ;}
    break;

  case 269:
#line 1583 "src/cmpl/CMPL_Parser.yy"
    { (yyval.i) = 0; ;}
    break;

  case 270:
#line 1586 "src/cmpl/CMPL_Parser.yy"
    { (yyval.i) = HEAD_MOD_REF; ;}
    break;

  case 271:
#line 1587 "src/cmpl/CMPL_Parser.yy"
    { (yyval.i) = HEAD_MOD_WHILE; ;}
    break;

  case 272:
#line 1593 "src/cmpl/CMPL_Parser.yy"
    { /* Unterscheidung ob Iteration über Set oder lokale Definition */
												  //TODO: Typangabe erlauben
												  //TODO: Auch lokale Definition eines Sets erlauben
												  unsigned short sub = ((yysemantic_stack_[(3) - (2)].i) == 2 || TYPECLASS((yysemantic_stack_[(3) - (3)].exp).tp) == TPK_SET ? START_LOOP : START_ASSIGN);
												  /* Art und Name der Laufkonstante eintragen */
												  (yysemantic_stack_[(3) - (1)].exp).tp = (sub == START_LOOP ? ((TP_IS_TUPEL((yysemantic_stack_[(3) - (3)].exp).tp) || TP_IS_TUPELSET((yysemantic_stack_[(3) - (3)].exp).tp)) ? TPK_TUPEL_N(RANK_TP_TUPEL((yysemantic_stack_[(3) - (3)].exp).tp)) : TPK_VALUE) : TYPECLASS((yysemantic_stack_[(3) - (3)].exp).tp));
												  PARSE_CONTEXT.set_loop_sname(sub, &(yysemantic_stack_[(3) - (1)].exp));
												  /* Wenn $2 == 0 (alte Zuweisung): Wenn $3 != TPK_SET (einfacher Wert), dann Fehler; sonst Warnung */
												  if ((yysemantic_stack_[(3) - (2)].i) == 0) PARSE_CONTEXT.out_warn_old_assign((TYPECLASS((yysemantic_stack_[(3) - (3)].exp).tp) != TPK_SET));
												;}
    break;

  case 273:
#line 1606 "src/cmpl/CMPL_Parser.yy"
    { (yyval.i) = 1; ;}
    break;

  case 274:
#line 1607 "src/cmpl/CMPL_Parser.yy"
    { (yyval.i) = 2; ;}
    break;

  case 275:
#line 1608 "src/cmpl/CMPL_Parser.yy"
    { (yyval.i) = 0; ;}
    break;

  case 276:
#line 1611 "src/cmpl/CMPL_Parser.yy"
    { if ((yysemantic_stack_[(1) - (1)].exp).dim_t_undef > 0)
													PARSE_CONTEXT.output_error_parse(ERROR_TYPE_COMP, ERROR_LVL_NORMAL, "undefined symbol");/*TODO*/
												  (yyval.exp) = (yysemantic_stack_[(1) - (1)].exp); ;}
    break;

  case 277:
#line 1616 "src/cmpl/CMPL_Parser.yy"
    { (yyval.exp) = (yysemantic_stack_[(1) - (1)].exp); /*strcpy((char *) (GLV->c_getmem(sizeof(char) * strlen(GLV->last_str))), GLV->last_str);*/ ;}
    break;

  case 278:
#line 1623 "src/cmpl/CMPL_Parser.yy"
    { if ((yysemantic_stack_[(1) - (1)].exp).dim_t_undef > 0)
													PARSE_CONTEXT.output_error_parse(ERROR_TYPE_COMP, ERROR_LVL_NORMAL, "undefined symbol");/*TODO*/
												  (yyval.exp) = (yysemantic_stack_[(1) - (1)].exp); ;}
    break;

  case 279:
#line 1629 "src/cmpl/CMPL_Parser.yy"
    { loop_ctrl_str *loopp = PARSE_CONTEXT.search_loop_name_mod((yysemantic_stack_[(2) - (2)].i_s).s, (yysemantic_stack_[(2) - (2)].i_s).i, "break", false);
												  if (loopp) { COMP_SUB_PAR_CNT(GOTO, GOTO_INDIREKT, 0, 1); COMP_ADDR(loopp->end_jump); } ;}
    break;

  case 280:
#line 1631 "src/cmpl/CMPL_Parser.yy"
    { loop_ctrl_str *loopp = PARSE_CONTEXT.search_loop_name_mod((yysemantic_stack_[(2) - (2)].i_s).s, (yysemantic_stack_[(2) - (2)].i_s).i, "continue", true);
												  if (loopp) { COMP_SUB_PAR_CNT(GOTO, GOTO_DIREKT, 0, 1); COMP_ADDR(loopp->iter_start); } ;}
    break;

  case 281:
#line 1633 "src/cmpl/CMPL_Parser.yy"
    { loop_ctrl_str *loopp = PARSE_CONTEXT.search_loop_name_mod((yysemantic_stack_[(2) - (2)].i_s).s, (yysemantic_stack_[(2) - (2)].i_s).i, "repeat", false);
												  if (loopp) { COMP_SUB_PAR_CNT(GOTO, GOTO_DIREKT, 0, 1); COMP_ADDR(loopp->loop_start); } ;}
    break;

  case 282:
#line 1637 "src/cmpl/CMPL_Parser.yy"
    { (yyval.i_s).s = (char *)0; (yyval.i_s).i = 0; ;}
    break;

  case 283:
#line 1638 "src/cmpl/CMPL_Parser.yy"
    { (yyval.i_s).s = (yysemantic_stack_[(1) - (1)].exp).symp()->s_name; (yyval.i_s).i = 0; ;}
    break;

  case 284:
#line 1639 "src/cmpl/CMPL_Parser.yy"
    { (yyval.i_s).s = (yysemantic_stack_[(1) - (1)].exp).symp()->s_name; /*GLV->last_str;*/ (yyval.i_s).i = 0; ;}
    break;

  case 285:
#line 1640 "src/cmpl/CMPL_Parser.yy"
    { (yyval.i_s).s = (char *)0; (yyval.i_s).i = (yysemantic_stack_[(1) - (1)].i); ;}
    break;

  case 286:
#line 1645 "src/cmpl/CMPL_Parser.yy"
    { if ((yysemantic_stack_[(3) - (3)].exp).dim_t_undef > 0)
													PARSE_CONTEXT.output_error_parse(ERROR_TYPE_COMP, ERROR_LVL_NORMAL, "undefined symbol");/*TODO*/
												  COMP(STORE_TMP, 1);
												  //COMP_INT((-$1-2));
												  COMP_INT((yysemantic_stack_[(3) - (1)].exp).symp()->symno); ;}
    break;

  case 287:
#line 1659 "src/cmpl/CMPL_Parser.yy"
    { if ((yysemantic_stack_[(2) - (2)].exp).dim_t_undef > 0)
											  	PARSE_CONTEXT.output_error_parse(ERROR_TYPE_COMP, ERROR_LVL_NORMAL, "undefined symbol");/*TODO*/
		   									  COMP_SUB(OUTPUT_MSG, ERROR_LVL_FATAL); ;}
    break;

  case 288:
#line 1662 "src/cmpl/CMPL_Parser.yy"
    { if ((yysemantic_stack_[(2) - (2)].exp).dim_t_undef > 0)
											  	PARSE_CONTEXT.output_error_parse(ERROR_TYPE_COMP, ERROR_LVL_NORMAL, "undefined symbol");/*TODO*/
											  COMP_SUB(OUTPUT_MSG, USR_MSG_NL); ;}
    break;


    /* Line 675 of lalr1.cc.  */
#line 2119 "src/cmpl/_CMPL_Parser.cc"
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
  const short int CMPL_Parser::yypact_ninf_ = -393;
  const short int
  CMPL_Parser::yypact_[] =
  {
       168,  -393,  -393,  -393,  -393,    25,   168,  -393,  -393,  -393,
    -393,   -21,   -15,    13,    35,  -393,  -393,   953,  1145,  1021,
    1089,    38,  1549,  1549,  1549,  1549,   -51,    39,  -393,  -393,
     121,  -393,  -393,  -393,    48,    68,    74,   129,   129,   138,
     149,    50,   154,   156,   159,   170,    68,  -393,  -393,  -393,
    -393,    81,  -393,  1501,  1501,  1241,    16,    16,    16,  -393,
    -393,  1189,  1293,  -393,   469,    66,  -393,   180,  -393,    60,
    -393,   148,  -393,  -393,  -393,  -393,   175,   186,   538,  -393,
    -393,  -393,   238,  -393,  -393,  -393,  -393,    -1,  -393,  -393,
     184,   192,   200,   202,   204,    39,  -393,   678,   209,   208,
    -393,  -393,  -393,    -1,   210,   212,   213,   215,  -393,  -393,
     545,  -393,   217,  -393,    -1,   223,   231,   232,   236,  -393,
     614,  -393,    56,    -1,   239,   241,   243,  -393,  -393,  -393,
    -393,   316,  -393,  -393,  -393,  1501,  1501,  1293,  -393,  1501,
    -393,  -393,  -393,    44,   144,  -393,  1501,  1501,  1345,    44,
    -393,  -393,  -393,    94,  1175,  -393,  -393,  -393,  -393,  -393,
      39,    66,  -393,  -393,  -393,  -393,  -393,  -393,  -393,  -393,
    -393,   235,  -393,   244,   225,   246,  -393,    98,    84,  1501,
    -393,   143,  1549,  1549,  1549,  1549,  1549,  1549,  1549,  1549,
    1501,   740,   245,   247,    17,    -1,    -1,    -1,  -393,  -393,
    -393,  -393,  -393,  -393,  -393,   126,   161,   353,  -393,  -393,
    -393,  -393,  -393,  -393,    56,   815,  -393,  -393,  -393,  -393,
    -393,   271,   271,   263,    83,   188,  -393,  -393,  -393,  -393,
      -1,   884,  -393,  -393,  -393,   469,    94,    57,   261,    61,
    1633,  1633,  -393,  -393,   262,    39,    39,  -393,  -393,   266,
     269,    52,    39,   270,   273,   274,  1501,  1501,  1549,  1549,
    1549,  1549,  1549,  1549,  1549,  1549,  1549,    84,  -393,  -393,
    1293,  -393,  -393,  -393,  -393,  -393,   275,  -393,  -393,    39,
     253,   253,   322,   322,   322,   322,   322,   316,    70,   268,
    -393,    -1,  -393,   326,  1501,  -393,  1501,  1501,   678,   280,
     280,  -393,   285,  -393,   268,   545,   300,   268,   614,    -1,
    -393,  -393,  -393,  -393,  -393,  -393,  -393,  -393,   188,  -393,
    -393,  1591,  -393,    56,   268,  -393,  -393,  -393,  -393,    39,
     148,  -393,   338,   284,   291,  -393,   288,  -393,  -393,  -393,
    -393,  -393,  1501,  -393,  -393,  -393,  -393,   367,  -393,   316,
     316,   316,   316,   316,   316,   316,   316,   316,  -393,  -393,
    1397,  -393,  -393,  1549,  -393,   292,   740,  1449,  -393,  -393,
    -393,  -393,   128,    94,  -393,  -393,   268,  -393,   268,  -393,
    1549,  -393,  -393,  -393,   292,   353,  -393,   167,   298,   292,
     815,  -393,    56,    -1,    56,  -393,  -393,    39,  -393,   360,
    -393,   319,   319,   268,   292,   884,   362,   143,  -393,  -393,
    -393,  -393,   309,  1397,    68,  -393,   317,   316,  -393,  -393,
     268,   320,   321,  -393,  -393,  -393,  -393,  1501,  -393,   292,
    1501,   292,  1501,   369,   316,  -393,   268,  -393,  -393,  -393,
    -393,   268,   268,    56,  -393,  -393,    83,  -393,  1591,  -393,
    -393,  -393,   292,    56,  -393,   268,    39,  1633,  -393,   323,
    -393,  -393,  -393,  -393,  -393,  -393,  -393,  -393,  -393,   268,
    -393,   268,  1549,  -393,  -393,   292,   268,  -393,  -393,   268,
    -393,   366,  -393,  -393,  -393,  -393,   325,  -393,   292,  -393,
    -393,  -393
  };

  /* YYDEFACT[S] -- default rule to reduce with in state S when YYTABLE
     doesn't specify something else to do.  Zero means the default is an
     error.  */
  const unsigned short int
  CMPL_Parser::yydefact_[] =
  {
         2,     8,    40,    64,    82,     0,     2,     4,     5,     6,
       7,     0,     0,     0,     0,     1,     3,     0,     0,     0,
       0,     0,     0,     0,     0,   235,   173,   173,   245,   132,
       0,   133,   135,   134,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    29,    30,    31,
      32,     0,    33,     0,     0,     0,   282,   282,   282,   233,
     234,   227,   227,     9,     0,    27,    24,     0,    18,     0,
      23,   181,   127,   131,   128,   129,   184,     0,    25,   185,
     241,   182,   183,   130,   136,    26,   253,   269,   188,   190,
       0,     0,     0,     0,     0,   173,    41,     0,     0,    51,
      53,    47,    54,   269,     0,     0,     0,     0,   246,    65,
       0,    72,   253,    70,   269,     0,     0,     0,     0,    83,
       0,    89,   101,   269,     0,     0,     0,    12,   200,   199,
     186,   236,    60,   174,   219,     0,     0,   227,   150,     0,
     157,   140,   138,     0,     0,   250,     0,     0,     0,     0,
      39,   251,   232,   187,   177,   287,   178,   179,   180,   288,
     173,    27,   283,   284,   271,   270,   285,   279,   280,   281,
     229,     0,   228,     0,   225,     0,    11,     0,     0,     0,
      22,     0,     0,     0,     0,     0,     0,     0,     0,   237,
       0,     0,     0,   259,   265,   269,   269,   269,   249,    19,
      20,    21,    44,   159,    43,     0,     0,     0,    48,    49,
      50,    68,    67,   248,   101,     0,    73,    74,    75,    86,
      85,   102,   103,     0,     0,   104,   115,   115,   100,   109,
     269,     0,    90,    91,    92,     0,   286,     0,     0,     0,
       0,     0,   168,   169,     0,   173,   173,   172,   170,     0,
       0,     0,   173,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   224,   223,
     227,   220,    28,    34,    35,    14,     0,   155,   156,   173,
     192,   193,   194,   195,   198,   197,   196,   238,     0,    38,
     258,   269,   267,     0,     0,   268,     0,     0,     0,    58,
      58,    57,     0,    52,    63,     0,    76,    81,     0,   269,
     111,    88,   242,   244,   243,    93,   115,    97,   104,   107,
      98,     0,    99,   101,   126,    13,   148,   221,   149,   173,
     164,   154,     0,     0,   160,   153,     0,   142,   167,   171,
     143,   147,     0,   158,   146,   145,   240,   202,   201,   203,
     206,   204,   210,   208,   207,   209,   205,   211,    16,   226,
       0,   137,   151,     0,   257,     0,     0,     0,   260,   262,
     277,   267,     0,   278,   267,   212,   214,   215,   217,    45,
     175,    55,    56,    46,     0,     0,    69,     0,     0,     0,
       0,    87,   101,   269,   101,   106,   105,   173,   116,   117,
     120,   122,   122,   114,     0,     0,   165,     0,   141,   163,
     161,   139,     0,     0,     0,   230,     0,   239,   254,    36,
      38,     0,     0,   264,   274,   273,   275,     0,   266,     0,
       0,     0,     0,     0,   176,    61,    63,    78,    77,    71,
      79,    81,   114,   101,    96,    94,    95,   218,     0,   123,
     119,   121,     0,   101,   124,   126,   173,     0,   144,     0,
     222,   231,    15,    37,   256,   255,   276,   272,   189,   214,
     191,   214,   175,    62,    80,     0,   114,   118,   108,   114,
     125,   166,   162,    17,   213,   216,     0,   110,     0,   113,
      59,   112
  };

  /* YYPGOTO[NTERM-NUM].  */
  const short int
  CMPL_Parser::yypgoto_[] =
  {
      -393,   397,  -393,  -393,  -393,   -40,  -393,  -393,  -393,  -393,
     351,  -393,   249,   234,   145,  -393,    -5,  -393,  -393,  -393,
     -66,  -393,  -393,   211,  -393,  -393,   116,    19,  -393,   -14,
    -393,  -393,   -91,  -393,  -393,  -393,  -393,  -393,   -20,  -393,
    -393,  -117,  -393,  -393,    29,  -393,  -393,  -185,  -393,  -393,
    -199,   110,  -393,  -393,  -198,  -393,  -392,  -188,  -393,   -13,
    -393,    32,  -393,   -16,  -195,  -393,  -393,  -393,  -393,  -393,
    -393,  -207,  -205,    33,   403,  -393,  -301,  -193,  -393,  -393,
    -393,    -4,  -393,  -393,   -23,   -30,   124,   -17,     0,  -393,
    -393,  -393,  -393,  -393,  -393,  -393,    14,  -268,    15,  -393,
    -393,  -200,  -393,   -37,  -393,   -25,   -32,  -393,    36,     1,
    -393,     2,     3,    -7,  -112,  -393,  -393,  -325,    23,  -393,
     -43,   155,  -393,  -393,  -393,  -393,  -273,   250,   132,  -393,
    -393,  -393,  -393,    85,    -3,   226,    24,    63
  };

  /* YYDEFGOTO[NTERM-NUM].  */
  const short int
  CMPL_Parser::yydefgoto_[] =
  {
        -1,     5,     6,     7,    11,    63,   235,    64,   360,   413,
      65,    66,   178,    67,   275,    68,   365,    69,     8,    12,
      96,   298,    97,    98,    99,   302,   381,    70,   101,   384,
       9,    13,   109,   305,   110,   214,   388,   111,   389,    10,
      14,   119,   308,   120,   223,   394,   445,   224,   225,   226,
     227,   317,   318,   228,   229,   393,   452,   320,   321,   398,
     399,   450,   121,   404,    71,    72,    73,    74,    75,   241,
     240,    76,    77,   279,   141,   253,   102,   333,   457,   410,
     334,   244,   245,   249,   132,   433,   153,   154,   172,    79,
     196,    80,   197,    81,   156,   157,   376,   429,   378,   431,
     402,    82,    83,   138,   461,    84,   173,   174,   416,   158,
     315,    86,   113,    87,    88,    89,    90,   419,   453,   367,
     191,   192,   193,   292,   293,   294,   369,   194,   195,   371,
     427,   467,   372,   374,    91,   167,    92,    93
  };

  /* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule which
     number is the opposite.  If zero, do what YYDEFACT says.  */
  const short int CMPL_Parser::yytable_ninf_ = -264;
  const short int
  CMPL_Parser::yytable_[] =
  {
        78,   133,   133,   220,   134,   128,   129,   130,   131,   150,
     230,   103,   114,   123,   152,   104,   115,   124,    85,   212,
     401,   112,   112,   122,   176,    15,   316,   319,    61,   306,
     175,   204,  -246,   331,   331,   332,   332,   100,    78,   322,
     335,   335,   105,   116,   125,   330,   330,    78,   336,  -263,
     475,   162,   163,   155,   159,    17,    85,   256,   257,   435,
     207,    18,   256,   257,   440,    85,   256,   257,   164,   165,
     133,   215,   203,   221,   222,   256,   257,   242,   243,   454,
     231,   106,   117,   126,   488,   164,   165,   489,   273,    19,
     103,   312,   274,  -261,   104,  -261,   313,   314,   423,   256,
     257,   428,   230,   114,   468,   238,   470,   115,    41,   309,
     310,    20,   112,   123,   324,   127,   100,   124,    61,   316,
     319,   105,   112,   122,   307,   135,   400,   478,   395,   136,
     342,   424,   425,   145,   116,   133,   426,   180,   403,   326,
     248,   304,   177,   328,   125,   255,   250,   401,   254,   137,
     487,   289,   363,   296,   297,   139,    47,    48,    49,    50,
     106,    52,   137,   491,   151,   280,   281,   282,   283,   284,
     285,   286,   287,   117,    78,  -152,   246,   242,   243,   276,
     247,   277,   278,   126,   299,   300,   301,   323,   166,   166,
     166,   391,    85,   160,    95,   325,     1,     2,     3,     4,
     103,   484,  -154,   485,   104,   221,   222,   442,   114,   446,
     140,   230,   115,   181,   386,   437,   438,   112,    78,   143,
     133,   133,   338,   339,   123,   100,   100,   133,   124,   343,
     144,   105,   379,   112,   122,   146,    85,   147,   359,   116,
     148,   349,   350,   351,   352,   353,   354,   355,   356,   357,
     331,   149,   332,   400,   133,   125,   362,   335,   476,   236,
     237,   179,   330,   239,   482,  -153,   392,   198,   479,   199,
     106,   251,   184,   185,   186,   187,   188,   200,   117,   201,
     230,   202,   230,   168,   169,   205,   206,   208,   455,   209,
     210,   103,   211,   213,   126,   104,   375,   377,   114,   441,
     216,   123,   115,   270,   133,   124,   406,   112,   217,   218,
     112,   122,   366,   219,   288,   268,   232,   100,   233,   436,
     234,   290,   105,    41,   269,   291,   420,   385,   271,   116,
     390,   230,   125,   182,   183,   184,   185,   186,   187,   188,
     311,   230,   412,   327,   337,   188,   417,   405,   340,    78,
     443,   341,   344,   364,    94,   345,   346,   361,   370,   380,
     415,   106,   383,   434,   387,   407,   408,    85,   117,   409,
     411,   126,   133,   257,   447,   439,   418,   460,   103,   448,
     347,   348,   104,   114,   449,    26,    95,   115,    28,  -219,
      30,   458,   112,  -151,   462,   472,   464,   465,   123,   430,
     483,   432,   124,    16,   100,   490,   161,   112,   122,   105,
     267,   272,   358,   415,   116,   463,   382,   303,   373,    53,
      54,   474,   473,   444,    56,    57,    58,   466,   396,   125,
     375,   133,   377,   481,   451,   477,  -252,   -42,   -42,   480,
     456,   142,   486,   366,   469,   295,   368,   471,   106,   459,
       0,     0,   422,   117,     0,   434,     0,     0,     0,   385,
       0,     0,     0,     0,   390,     0,     0,     0,   126,   -10,
      21,     0,     0,     0,     0,     0,     0,     0,   405,     0,
       0,     0,     0,     0,     0,     0,    22,    23,     0,     0,
       0,   373,   430,    24,   430,    25,     0,   -10,   -10,   -10,
     -10,    26,     0,    27,    28,    29,    30,     0,     0,    31,
      32,    33,    34,    35,     0,     0,    36,    37,    38,    39,
      40,    41,    42,    43,    44,    45,    46,    47,    48,    49,
      50,    51,    52,     0,     0,    53,    54,     0,     0,    55,
      56,    57,    58,    59,    60,   -66,   107,     0,    61,     0,
      62,     0,  -252,   -10,   -10,   182,   183,   184,   185,   186,
     187,   188,  -247,  -247,   189,     0,     0,     0,     0,     0,
       0,     0,     0,   -66,   -66,   -66,   -66,   108,  -247,  -247,
      28,  -247,    30,     0,     0,  -247,  -247,  -247,  -247,  -247,
       0,     0,  -247,  -247,  -247,  -247,  -247,  -247,  -247,  -247,
    -247,     0,     0,  -247,  -247,  -247,  -247,     0,  -247,     0,
       0,    53,    54,     0,   -84,   118,    56,    57,    58,   190,
       0,     0,     0,     0,  -247,     0,  -247,     0,  -252,   -66,
     -66,  -247,  -247,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   -84,   -84,   -84,   -84,   108,  -247,  -247,    28,
    -247,    30,     0,     0,  -247,  -247,  -247,  -247,  -247,     0,
       0,  -247,  -247,  -247,  -247,  -247,  -247,  -247,  -247,  -247,
       0,     0,  -247,  -247,  -247,  -247,     0,  -247,   -42,    94,
      53,    54,     0,     0,     0,    56,    57,    58,     0,     0,
       0,     0,     0,  -247,     0,  -247,     0,  -252,   -84,   -84,
       0,     0,     0,     0,     0,     0,   -42,   -42,   -42,   -42,
      26,    95,     0,    28,     0,    30,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    21,     0,     0,    53,    54,     0,     0,     0,    56,
      57,    58,     0,     0,     0,     0,     0,    22,    23,     0,
       0,  -252,   -42,   -42,    24,     0,    25,     0,     0,     0,
       0,     0,    26,     0,    27,    28,    29,    30,     0,     0,
      31,    32,    33,    34,    35,     0,     0,    36,    37,    38,
      39,    40,    41,    42,    43,    44,    45,    46,    47,    48,
      49,    50,    51,    52,     0,     0,    53,    54,     0,     0,
      55,    56,    57,    58,    59,    60,   107,     0,     0,    61,
       0,    62,     0,  -252,   -10,   -10,     0,     0,     0,     0,
       0,     0,  -247,  -247,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   108,  -247,  -247,
      28,  -247,    30,     0,     0,  -247,  -247,  -247,  -247,  -247,
       0,     0,  -247,  -247,  -247,  -247,  -247,  -247,  -247,  -247,
    -247,     0,     0,  -247,  -247,  -247,  -247,     0,  -247,     0,
       0,    53,    54,     0,     0,   118,    56,    57,    58,     0,
       0,     0,     0,     0,  -247,     0,  -247,     0,  -252,   -66,
     -66,  -247,  -247,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   108,  -247,  -247,    28,
    -247,    30,     0,     0,  -247,  -247,  -247,  -247,  -247,     0,
       0,  -247,  -247,  -247,  -247,  -247,  -247,  -247,  -247,  -247,
       0,     0,  -247,  -247,  -247,  -247,     0,  -247,     0,     0,
      53,    54,     0,   -10,    21,    56,    57,    58,     0,     0,
       0,     0,     0,  -247,     0,  -247,     0,  -252,   -84,   -84,
      22,    23,     0,     0,     0,     0,     0,    24,     0,    25,
       0,   -10,   -10,   -10,   -10,    26,     0,    27,    28,    29,
      30,     0,     0,    31,    32,    33,    34,    35,     0,     0,
      36,    37,    38,    39,    40,    41,    42,    43,    44,    45,
      46,    47,    48,    49,    50,    51,    52,     0,     0,    53,
      54,   -66,   107,    55,    56,    57,    58,    59,    60,     0,
       0,     0,    61,     0,    62,     0,  -252,     0,  -247,  -247,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   -66,
     -66,   -66,   -66,   108,  -247,  -247,    28,  -247,    30,     0,
       0,  -247,  -247,  -247,  -247,  -247,     0,     0,  -247,  -247,
    -247,  -247,  -247,  -247,  -247,  -247,  -247,     0,     0,  -247,
    -247,  -247,  -247,     0,  -247,     0,     0,    53,    54,   -84,
     118,     0,    56,    57,    58,     0,     0,     0,     0,     0,
    -247,     0,  -247,     0,  -252,     0,  -247,  -247,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   -84,   -84,   -84,
     -84,   108,  -247,  -247,    28,  -247,    30,     0,     0,  -247,
    -247,  -247,  -247,  -247,     0,     0,  -247,  -247,  -247,  -247,
    -247,  -247,  -247,  -247,  -247,   -42,    94,  -247,  -247,  -247,
    -247,     0,  -247,     0,     0,    53,    54,     0,     0,     0,
      56,    57,    58,     0,     0,     0,     0,     0,  -247,     0,
    -247,     0,  -252,   -42,   -42,   -42,   -42,    26,    95,     0,
      28,     0,    30,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   182,   183,   184,   185,   186,   187,   188,     0,
       0,   189,     0,     0,     0,     0,    22,    23,     0,     0,
       0,    53,    54,    24,     0,    25,    56,    57,    58,     0,
       0,   170,     0,    27,     0,    29,     0,     0,  -252,    31,
      32,    33,    34,    35,   171,     0,    36,    37,    38,    39,
      40,    41,    42,    43,    44,    45,     0,    47,    48,    49,
      50,    51,    52,     0,     0,     0,   190,     0,    22,    23,
       0,     0,     0,    59,    60,    24,     0,    25,    61,     0,
      62,     0,     0,   160,     0,    27,     0,    29,     0,     0,
       0,    31,    32,    33,    34,    35,     0,     0,    36,    37,
      38,    39,    40,    41,    42,    43,    44,    45,     0,    47,
      48,    49,    50,    51,    52,     0,     0,     0,     0,     0,
      22,    23,     0,     0,     0,    59,    60,    24,     0,    25,
      61,     0,    62,     0,     0,   170,     0,    27,     0,    29,
       0,     0,     0,    31,    32,    33,    34,    35,     0,     0,
      36,    37,    38,    39,    40,    41,    42,    43,    44,    45,
       0,    47,    48,    49,    50,    51,    52,     0,     0,     0,
       0,     0,    22,    23,     0,     0,     0,    59,    60,    24,
       0,    25,    61,     0,    62,     0,     0,     0,   252,    27,
       0,    29,     0,     0,     0,    31,    32,    33,    34,    35,
       0,     0,    36,    37,    38,    39,    40,    41,    42,    43,
      44,    45,     0,    47,    48,    49,    50,    51,    52,     0,
       0,     0,     0,     0,    22,    23,     0,     0,     0,    59,
      60,    24,     0,    25,    61,     0,    62,     0,     0,     0,
       0,    27,     0,    29,     0,     0,     0,    31,    32,    33,
      34,    35,     0,   414,    36,    37,    38,    39,    40,    41,
      42,    43,    44,    45,     0,    47,    48,    49,    50,    51,
      52,     0,     0,     0,     0,     0,    22,    23,     0,     0,
       0,    59,    60,    24,     0,    25,    61,     0,    62,     0,
       0,     0,     0,    27,     0,    29,     0,     0,     0,    31,
      32,    33,    34,    35,     0,     0,    36,    37,    38,    39,
      40,    41,    42,    43,    44,    45,     0,    47,    48,    49,
      50,    51,    52,     0,     0,     0,     0,   421,    22,    23,
       0,     0,     0,    59,    60,    24,     0,    25,    61,     0,
      62,     0,     0,     0,     0,    27,     0,    29,     0,     0,
       0,    31,    32,    33,    34,    35,     0,     0,    36,    37,
      38,    39,    40,    41,    42,    43,    44,    45,     0,    47,
      48,    49,    50,    51,    52,     0,    22,    23,     0,     0,
       0,     0,     0,    24,     0,    59,    60,     0,     0,     0,
      61,     0,    62,    27,     0,    29,     0,     0,     0,    31,
      32,    33,    34,    35,     0,     0,    36,    37,    38,    39,
      40,    41,    42,    43,    44,     0,     0,    47,    48,    49,
      50,     0,    52,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    95,   397,     0,    29,    61,     0,
      62,    31,    32,    33,    34,    35,     0,     0,    36,    37,
      38,    39,    40,     0,    42,    43,    44,     0,     0,    47,
      48,    49,    50,     0,    52,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   329,     0,    29,
      61,     0,    62,    31,    32,    33,    34,    35,     0,     0,
      36,    37,    38,    39,    40,     0,    42,    43,    44,     0,
       0,    47,    48,    49,    50,     0,    52,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    61,     0,    62
  };

  /* YYCHECK.  */
  const short int
  CMPL_Parser::yycheck_[] =
  {
        17,    26,    27,   120,    27,    22,    23,    24,    25,    46,
     122,    18,    19,    20,    51,    18,    19,    20,    17,   110,
     321,    19,    20,    20,    64,     0,   225,   225,    79,   214,
      62,    97,    83,   240,   241,   240,   241,    18,    55,   227,
     240,   241,    18,    19,    20,   240,   241,    64,   241,    32,
     442,    35,    36,    53,    54,    76,    55,     5,     6,   384,
     103,    76,     5,     6,   389,    64,     5,     6,    69,    70,
      95,   114,    95,    17,    18,     5,     6,    33,    34,   404,
     123,    18,    19,    20,   476,    69,    70,   479,     4,    76,
      97,     8,     8,    76,    97,    78,    13,    14,   371,     5,
       6,   374,   214,   110,   429,   137,   431,   110,    52,   221,
     222,    76,   110,   120,   231,    77,    97,   120,    79,   318,
     318,    97,   120,   120,   215,     4,   321,   452,   316,    81,
      78,     3,     4,    83,   110,   160,     8,    77,   323,    82,
     144,   207,    76,    82,   120,   149,   146,   448,   148,    81,
     475,   191,    82,   196,   197,    81,    58,    59,    60,    61,
      97,    63,    81,   488,    83,   182,   183,   184,   185,   186,
     187,   188,   189,   110,   191,    27,    32,    33,    34,   179,
      36,    38,    39,   120,    58,    59,    60,   230,    56,    57,
      58,   308,   191,    32,    33,   235,    28,    29,    30,    31,
     207,   469,    27,   471,   207,    17,    18,   392,   215,   394,
      81,   323,   215,    27,   305,    48,    49,   215,   235,    81,
     245,   246,   245,   246,   231,   206,   207,   252,   231,   252,
      81,   207,   298,   231,   231,    81,   235,    81,   270,   215,
      81,   258,   259,   260,   261,   262,   263,   264,   265,   266,
     457,    81,   457,   448,   279,   231,   279,   457,   443,   135,
     136,    81,   457,   139,   457,    27,   309,    83,   453,    77,
     207,   147,    19,    20,    21,    22,    23,    77,   215,    77,
     392,    77,   394,    57,    58,    76,    78,    77,   405,    77,
      77,   298,    77,    76,   231,   298,   296,   297,   305,   390,
      77,   308,   305,    78,   329,   308,   329,   305,    77,    77,
     308,   308,   289,    77,   190,    80,    77,   298,    77,   385,
      77,    76,   298,    52,    80,    78,   366,   304,    82,   305,
     307,   443,   308,    17,    18,    19,    20,    21,    22,    23,
      77,   453,   342,    82,    82,    23,   363,   324,    82,   366,
     393,    82,    82,    85,     1,    82,    82,    82,    32,    79,
     360,   298,    77,   380,    64,    27,    82,   366,   305,    78,
      82,   308,   397,     6,   397,    77,    84,   414,   385,    19,
     256,   257,   385,   390,    65,    32,    33,   390,    35,    27,
      37,    82,   390,    27,    77,    26,    76,    76,   405,   376,
      77,   378,   405,     6,   385,    80,    55,   405,   405,   385,
     161,   177,   267,   413,   390,   420,   300,   206,   294,    66,
      67,   441,   436,   394,    71,    72,    73,   427,   318,   405,
     430,   456,   432,   456,   402,   448,    83,    84,    85,   455,
     407,    38,   472,   420,   430,   195,   291,   432,   385,   413,
      -1,    -1,   367,   390,    -1,   472,    -1,    -1,    -1,   436,
      -1,    -1,    -1,    -1,   441,    -1,    -1,    -1,   405,     0,
       1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   455,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    17,    18,    -1,    -1,
      -1,   367,   469,    24,   471,    26,    -1,    28,    29,    30,
      31,    32,    -1,    34,    35,    36,    37,    -1,    -1,    40,
      41,    42,    43,    44,    -1,    -1,    47,    48,    49,    50,
      51,    52,    53,    54,    55,    56,    57,    58,    59,    60,
      61,    62,    63,    -1,    -1,    66,    67,    -1,    -1,    70,
      71,    72,    73,    74,    75,     0,     1,    -1,    79,    -1,
      81,    -1,    83,    84,    85,    17,    18,    19,    20,    21,
      22,    23,    17,    18,    26,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    -1,    -1,    40,    41,    42,    43,    44,
      -1,    -1,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    -1,    -1,    58,    59,    60,    61,    -1,    63,    -1,
      -1,    66,    67,    -1,     0,     1,    71,    72,    73,    81,
      -1,    -1,    -1,    -1,    79,    -1,    81,    -1,    83,    84,
      85,    17,    18,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    28,    29,    30,    31,    32,    33,    34,    35,
      36,    37,    -1,    -1,    40,    41,    42,    43,    44,    -1,
      -1,    47,    48,    49,    50,    51,    52,    53,    54,    55,
      -1,    -1,    58,    59,    60,    61,    -1,    63,     0,     1,
      66,    67,    -1,    -1,    -1,    71,    72,    73,    -1,    -1,
      -1,    -1,    -1,    79,    -1,    81,    -1,    83,    84,    85,
      -1,    -1,    -1,    -1,    -1,    -1,    28,    29,    30,    31,
      32,    33,    -1,    35,    -1,    37,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,     1,    -1,    -1,    66,    67,    -1,    -1,    -1,    71,
      72,    73,    -1,    -1,    -1,    -1,    -1,    17,    18,    -1,
      -1,    83,    84,    85,    24,    -1,    26,    -1,    -1,    -1,
      -1,    -1,    32,    -1,    34,    35,    36,    37,    -1,    -1,
      40,    41,    42,    43,    44,    -1,    -1,    47,    48,    49,
      50,    51,    52,    53,    54,    55,    56,    57,    58,    59,
      60,    61,    62,    63,    -1,    -1,    66,    67,    -1,    -1,
      70,    71,    72,    73,    74,    75,     1,    -1,    -1,    79,
      -1,    81,    -1,    83,    84,    85,    -1,    -1,    -1,    -1,
      -1,    -1,    17,    18,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    32,    33,    34,
      35,    36,    37,    -1,    -1,    40,    41,    42,    43,    44,
      -1,    -1,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    -1,    -1,    58,    59,    60,    61,    -1,    63,    -1,
      -1,    66,    67,    -1,    -1,     1,    71,    72,    73,    -1,
      -1,    -1,    -1,    -1,    79,    -1,    81,    -1,    83,    84,
      85,    17,    18,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    32,    33,    34,    35,
      36,    37,    -1,    -1,    40,    41,    42,    43,    44,    -1,
      -1,    47,    48,    49,    50,    51,    52,    53,    54,    55,
      -1,    -1,    58,    59,    60,    61,    -1,    63,    -1,    -1,
      66,    67,    -1,     0,     1,    71,    72,    73,    -1,    -1,
      -1,    -1,    -1,    79,    -1,    81,    -1,    83,    84,    85,
      17,    18,    -1,    -1,    -1,    -1,    -1,    24,    -1,    26,
      -1,    28,    29,    30,    31,    32,    -1,    34,    35,    36,
      37,    -1,    -1,    40,    41,    42,    43,    44,    -1,    -1,
      47,    48,    49,    50,    51,    52,    53,    54,    55,    56,
      57,    58,    59,    60,    61,    62,    63,    -1,    -1,    66,
      67,     0,     1,    70,    71,    72,    73,    74,    75,    -1,
      -1,    -1,    79,    -1,    81,    -1,    83,    -1,    17,    18,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    28,
      29,    30,    31,    32,    33,    34,    35,    36,    37,    -1,
      -1,    40,    41,    42,    43,    44,    -1,    -1,    47,    48,
      49,    50,    51,    52,    53,    54,    55,    -1,    -1,    58,
      59,    60,    61,    -1,    63,    -1,    -1,    66,    67,     0,
       1,    -1,    71,    72,    73,    -1,    -1,    -1,    -1,    -1,
      79,    -1,    81,    -1,    83,    -1,    17,    18,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    28,    29,    30,
      31,    32,    33,    34,    35,    36,    37,    -1,    -1,    40,
      41,    42,    43,    44,    -1,    -1,    47,    48,    49,    50,
      51,    52,    53,    54,    55,     0,     1,    58,    59,    60,
      61,    -1,    63,    -1,    -1,    66,    67,    -1,    -1,    -1,
      71,    72,    73,    -1,    -1,    -1,    -1,    -1,    79,    -1,
      81,    -1,    83,    28,    29,    30,    31,    32,    33,    -1,
      35,    -1,    37,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    -1,
      -1,    26,    -1,    -1,    -1,    -1,    17,    18,    -1,    -1,
      -1,    66,    67,    24,    -1,    26,    71,    72,    73,    -1,
      -1,    32,    -1,    34,    -1,    36,    -1,    -1,    83,    40,
      41,    42,    43,    44,    45,    -1,    47,    48,    49,    50,
      51,    52,    53,    54,    55,    56,    -1,    58,    59,    60,
      61,    62,    63,    -1,    -1,    -1,    81,    -1,    17,    18,
      -1,    -1,    -1,    74,    75,    24,    -1,    26,    79,    -1,
      81,    -1,    -1,    32,    -1,    34,    -1,    36,    -1,    -1,
      -1,    40,    41,    42,    43,    44,    -1,    -1,    47,    48,
      49,    50,    51,    52,    53,    54,    55,    56,    -1,    58,
      59,    60,    61,    62,    63,    -1,    -1,    -1,    -1,    -1,
      17,    18,    -1,    -1,    -1,    74,    75,    24,    -1,    26,
      79,    -1,    81,    -1,    -1,    32,    -1,    34,    -1,    36,
      -1,    -1,    -1,    40,    41,    42,    43,    44,    -1,    -1,
      47,    48,    49,    50,    51,    52,    53,    54,    55,    56,
      -1,    58,    59,    60,    61,    62,    63,    -1,    -1,    -1,
      -1,    -1,    17,    18,    -1,    -1,    -1,    74,    75,    24,
      -1,    26,    79,    -1,    81,    -1,    -1,    -1,    33,    34,
      -1,    36,    -1,    -1,    -1,    40,    41,    42,    43,    44,
      -1,    -1,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    -1,    58,    59,    60,    61,    62,    63,    -1,
      -1,    -1,    -1,    -1,    17,    18,    -1,    -1,    -1,    74,
      75,    24,    -1,    26,    79,    -1,    81,    -1,    -1,    -1,
      -1,    34,    -1,    36,    -1,    -1,    -1,    40,    41,    42,
      43,    44,    -1,    46,    47,    48,    49,    50,    51,    52,
      53,    54,    55,    56,    -1,    58,    59,    60,    61,    62,
      63,    -1,    -1,    -1,    -1,    -1,    17,    18,    -1,    -1,
      -1,    74,    75,    24,    -1,    26,    79,    -1,    81,    -1,
      -1,    -1,    -1,    34,    -1,    36,    -1,    -1,    -1,    40,
      41,    42,    43,    44,    -1,    -1,    47,    48,    49,    50,
      51,    52,    53,    54,    55,    56,    -1,    58,    59,    60,
      61,    62,    63,    -1,    -1,    -1,    -1,    68,    17,    18,
      -1,    -1,    -1,    74,    75,    24,    -1,    26,    79,    -1,
      81,    -1,    -1,    -1,    -1,    34,    -1,    36,    -1,    -1,
      -1,    40,    41,    42,    43,    44,    -1,    -1,    47,    48,
      49,    50,    51,    52,    53,    54,    55,    56,    -1,    58,
      59,    60,    61,    62,    63,    -1,    17,    18,    -1,    -1,
      -1,    -1,    -1,    24,    -1,    74,    75,    -1,    -1,    -1,
      79,    -1,    81,    34,    -1,    36,    -1,    -1,    -1,    40,
      41,    42,    43,    44,    -1,    -1,    47,    48,    49,    50,
      51,    52,    53,    54,    55,    -1,    -1,    58,    59,    60,
      61,    -1,    63,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    33,    34,    -1,    36,    79,    -1,
      81,    40,    41,    42,    43,    44,    -1,    -1,    47,    48,
      49,    50,    51,    -1,    53,    54,    55,    -1,    -1,    58,
      59,    60,    61,    -1,    63,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    34,    -1,    36,
      79,    -1,    81,    40,    41,    42,    43,    44,    -1,    -1,
      47,    48,    49,    50,    51,    -1,    53,    54,    55,    -1,
      -1,    58,    59,    60,    61,    -1,    63,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    79,    -1,    81
  };

  /* STOS_[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
  const unsigned char
  CMPL_Parser::yystos_[] =
  {
         0,    28,    29,    30,    31,    87,    88,    89,   104,   116,
     125,    90,   105,   117,   126,     0,    87,    76,    76,    76,
      76,     1,    17,    18,    24,    26,    32,    34,    35,    36,
      37,    40,    41,    42,    43,    44,    47,    48,    49,    50,
      51,    52,    53,    54,    55,    56,    57,    58,    59,    60,
      61,    62,    63,    66,    67,    70,    71,    72,    73,    74,
      75,    79,    81,    91,    93,    96,    97,    99,   101,   103,
     113,   150,   151,   152,   153,   154,   157,   158,   173,   175,
     177,   179,   187,   188,   191,   195,   197,   199,   200,   201,
     202,   220,   222,   223,     1,    33,   106,   108,   109,   110,
     113,   114,   162,   199,   220,   222,   223,     1,    32,   118,
     120,   123,   197,   198,   199,   220,   222,   223,     1,   127,
     129,   148,   198,   199,   220,   222,   223,    77,   173,   173,
     173,   173,   170,   191,   170,     4,    81,    81,   189,    81,
      81,   160,   160,    81,    81,    83,    81,    81,    81,    81,
     189,    83,   189,   172,   173,   174,   180,   181,   195,   174,
      32,    96,    35,    36,    69,    70,   214,   221,   221,   221,
      32,    45,   174,   192,   193,   192,    91,    76,    98,    81,
      77,    27,    17,    18,    19,    20,    21,    22,    23,    26,
      81,   206,   207,   208,   213,   214,   176,   178,    83,    77,
      77,    77,    77,   170,   106,    76,    78,   206,    77,    77,
      77,    77,   118,    76,   121,   206,    77,    77,    77,    77,
     127,    17,    18,   130,   133,   134,   135,   136,   139,   140,
     200,   206,    77,    77,    77,    92,   172,   172,   192,   172,
     156,   155,    33,    34,   167,   168,    32,    36,   167,   169,
     174,   172,    33,   161,   174,   167,     5,     6,     8,     9,
      10,    11,    12,    13,    14,    15,    16,    98,    80,    80,
      78,    82,    99,     4,     8,   100,   174,    38,    39,   159,
     173,   173,   173,   173,   173,   173,   173,   173,   172,    91,
      76,    78,   209,   210,   211,   213,   206,   206,   107,    58,
      59,    60,   111,   109,   106,   119,   133,   118,   128,   200,
     200,    77,     8,    13,    14,   196,   136,   137,   138,   140,
     143,   144,   143,   206,   127,    91,    82,    82,    82,    34,
     150,   157,   158,   163,   166,   187,   163,    82,   170,   170,
      82,    82,    78,   170,    82,    82,    82,   172,   172,   173,
     173,   173,   173,   173,   173,   173,   173,   173,   100,   192,
      94,    82,   170,    82,    85,   102,   204,   205,   207,   212,
      32,   215,   218,   172,   219,   174,   182,   174,   184,   106,
      79,   112,   112,    77,   115,   204,   118,    64,   122,   124,
     204,   127,   206,   141,   131,   143,   137,    34,   145,   146,
     150,   162,   186,   133,   149,   204,   170,    27,    82,    78,
     165,    82,   174,    95,    46,   174,   194,   173,    84,   203,
      91,    68,   219,   212,     3,     4,     8,   216,   212,   183,
     204,   185,   204,   171,   173,   203,   106,    48,    49,    77,
     203,   118,   133,   206,   130,   132,   133,   170,    19,    65,
     147,   147,   142,   204,   203,   127,   159,   164,    82,   194,
     189,   190,    77,   102,    76,    76,   174,   217,   203,   182,
     203,   184,    26,   115,   124,   142,   133,   145,   203,   133,
     149,   170,   163,    77,   183,   183,   171,   203,   142,   142,
      80,   203
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
     318,   319,   320,   321,   322,   323,    58,    59,    44,    91,
      93,    40,    41,   123,   125,   124
  };
#endif

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
  const unsigned char
  CMPL_Parser::yyr1_[] =
  {
         0,    86,    87,    87,    88,    88,    88,    88,    90,    89,
      91,    91,    92,    91,    94,    93,    95,    93,    93,    93,
      93,    93,    93,    96,    96,    97,    97,    98,    98,    99,
      99,    99,    99,    99,   100,   100,   101,   102,   102,   103,
     105,   104,   106,   106,   107,   106,   108,   108,   108,   108,
     108,   109,   109,   110,   110,   111,   111,   111,   112,   112,
     113,   114,   115,   115,   117,   116,   118,   118,   119,   118,
     121,   120,   120,   120,   120,   120,   122,   122,   122,   123,
     124,   124,   126,   125,   127,   127,   128,   127,   129,   129,
     129,   129,   129,   131,   130,   132,   132,   133,   134,   134,
     134,   135,   136,   136,   137,   137,   138,   138,   139,   139,
     140,   141,   140,   142,   142,   144,   143,   145,   145,   146,
     146,   146,   147,   147,   148,   149,   149,   150,   150,   150,
     150,   150,   150,   151,   151,   151,   152,   153,   155,   154,
     156,   154,   154,   154,   154,   154,   154,   154,   154,   154,
     154,   157,   158,   158,   158,   159,   159,   160,   161,   162,
     163,   164,   163,   165,   166,   166,   166,   167,   168,   168,
     169,   169,   169,   170,   170,   171,   171,   172,   172,   172,
     172,   173,   173,   173,   173,   173,   173,   174,   176,   175,
     178,   177,   179,   179,   179,   179,   179,   179,   179,   179,
     179,   180,   180,   181,   181,   181,   181,   181,   181,   181,
     181,   181,   182,   183,   183,   184,   185,   185,   186,   187,
     188,   189,   190,   191,   191,   192,   192,   193,   193,   193,
     194,   194,   195,   195,   195,   195,   195,   195,   195,   195,
     195,   195,   196,   196,   196,   197,   197,   198,   198,   199,
     200,   201,   202,   202,   203,   204,   204,   205,   206,   207,
     207,   209,   208,   210,   208,   211,   208,   212,   213,   213,
     214,   214,   215,   216,   216,   216,   217,   218,   219,   220,
     220,   220,   221,   221,   221,   221,   222,   223,   223
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
       3,     3,     1,     3,     3,     1,     3,     0,     1,     1,
       1,     2,     2,     1,     1,     1,     2,     2,     3,     5,
       4,     1,     1,     1,     1,     1,     1,     0,     2,     2,
       2,     2,     0,     1,     1,     3,     3,     1,     2,     1,
       3,     0,     3,     0,     4,     0,     4,     0,     2,     0,
       1,     1,     3,     1,     1,     1,     1,     1,     1,     2,
       2,     2,     0,     1,     1,     1,     3,     2,     2
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
  "NULL_VAL", "READCSV", "READSTDIN", "FCT_MIN", "FCT_MAX", "DIM", "DEF",
  "SUM", "LEN", "FORMAT", "TYPE", "DEFSET", "PROBLEM_SOLVE", "TD_REAL",
  "TD_INT", "TD_BINARY", "TD_STRING", "TD_SET", "TD_PROBLEM", "OBJ_TO",
  "TRANSPOSE", "ERRMSG", "USRMSG", "DEFAULT", "WHILE", "REF", "BREAK",
  "CONTINUE", "REPEAT", "SET_ALL_NUM", "SET_ALL_STR", "':'", "';'", "','",
  "'['", "']'", "'('", "')'", "'{'", "'}'", "'|'", "$accept", "prog",
  "prog_sec", "const_sec", "@1", "const_defs", "@2", "const_def", "@3",
  "@4", "ul_sym_vekt", "lvalue_expr", "const_type_def", "const_type",
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
        87,     0,    -1,    -1,    88,    87,    -1,    89,    -1,   104,
      -1,   116,    -1,   125,    -1,    -1,    28,    90,    76,    91,
      -1,    -1,    93,    91,    -1,    -1,     1,    77,    92,    91,
      -1,    -1,    96,    98,   100,    94,   194,    77,    -1,    -1,
      70,    96,    98,   100,    95,   194,    77,    -1,   101,    -1,
     220,    77,    -1,   222,    77,    -1,   223,    77,    -1,   103,
      77,    -1,   113,    -1,    97,    -1,   173,    -1,   195,    -1,
      -1,    76,    99,    -1,    58,    -1,    59,    -1,    60,    -1,
      61,    -1,    63,    -1,     4,    -1,     8,    -1,   199,   206,
      91,   102,   203,    -1,   204,    91,   102,    -1,    -1,    57,
     189,    -1,    -1,    29,   105,    76,   106,    -1,    -1,   108,
     106,    -1,    -1,     1,    77,   107,   106,    -1,   109,    76,
     111,    77,    -1,   114,    -1,   220,    77,    -1,   222,    77,
      -1,   223,    77,    -1,   110,    -1,   110,    78,   109,    -1,
     113,    -1,   162,    -1,    58,   112,    -1,    59,   112,    -1,
      60,    -1,    -1,    79,   171,    26,   171,    80,    -1,    32,
     170,    -1,   199,   206,   106,   115,   203,    -1,   204,   106,
     115,    -1,    -1,    -1,    30,   117,    76,   118,    -1,    -1,
     120,   118,    -1,    -1,     1,    77,   119,   118,    -1,    -1,
     198,   121,   133,   122,    77,    -1,   123,    -1,   220,    77,
      -1,   222,    77,    -1,   223,    77,    -1,    -1,    64,    49,
      -1,    64,    48,    -1,   199,   206,   118,   124,   203,    -1,
     204,   118,   124,    -1,    -1,    -1,    31,   126,    76,   127,
      -1,    -1,   129,   127,    -1,    -1,     1,    77,   128,   127,
      -1,   198,   130,    77,    -1,   148,    -1,   220,    77,    -1,
     222,    77,    -1,   223,    77,    -1,    -1,   133,   196,   131,
     132,    -1,   133,    -1,   130,    -1,   134,   137,    -1,   135,
     143,    -1,   136,   143,    -1,   139,    -1,    -1,    17,    -1,
      18,    -1,    -1,   138,   137,    -1,   136,   143,    -1,   140,
      -1,   200,   206,   133,   142,   203,    -1,   140,    -1,    17,
     200,   206,   133,   142,   203,    -1,    -1,    18,   200,   141,
     206,   133,   142,   203,    -1,   204,   133,   142,    -1,    -1,
      -1,   144,   145,    -1,   146,    -1,   146,    19,   145,    -1,
     162,   147,    -1,   150,    -1,   186,   147,    -1,    -1,    65,
      -1,   199,   206,   127,   149,   203,    -1,   204,   127,   149,
      -1,    -1,   151,    -1,   153,    -1,   154,    -1,   188,    -1,
     152,    -1,    36,    -1,    40,    -1,    42,    -1,    41,    -1,
     191,    -1,    99,    81,   174,    82,    -1,    -1,    49,   160,
     155,   163,    82,    -1,    -1,    48,   160,   156,   163,    82,
      -1,    50,    81,   167,    82,    -1,    51,    81,   169,    82,
      -1,    54,    81,   172,    78,   174,    82,    -1,    55,    81,
     174,    82,    -1,    55,    81,   161,    82,    -1,    53,    81,
     174,    82,    -1,    43,    81,   172,    82,    -1,    47,    81,
     172,    82,    -1,    44,   189,    -1,   158,    27,   159,   170,
      -1,   150,    -1,   187,    -1,   157,    -1,    38,    -1,    39,
      -1,    81,    -1,    33,   170,    -1,    33,   170,    -1,   166,
      -1,    -1,   166,   165,   164,   163,    -1,    78,    -1,   150,
      -1,    34,   170,    -1,   158,    27,   159,   170,    -1,   168,
     170,    -1,    33,    -1,    34,    -1,   167,    -1,    32,   170,
      -1,    36,    -1,    -1,   191,    -1,    -1,   173,    -1,   173,
      -1,   180,    -1,   181,    -1,   195,    -1,   150,    -1,   179,
      -1,   187,    -1,   157,    -1,   175,    -1,    24,   173,    -1,
     172,    -1,    -1,   200,   176,   206,   182,   183,   203,    -1,
      -1,   201,   178,   206,   184,   185,   203,    -1,   173,    17,
     173,    -1,   173,    18,   173,    -1,   173,    19,   173,    -1,
     173,    20,   173,    -1,   173,    23,   173,    -1,   173,    22,
     173,    -1,   173,    21,   173,    -1,    18,   173,    -1,    17,
     173,    -1,   172,     6,   172,    -1,   172,     5,   172,    -1,
     173,     8,   173,    -1,   173,    10,   173,    -1,   173,    15,
     173,    -1,   173,     9,   173,    -1,   173,    13,   173,    -1,
     173,    12,   173,    -1,   173,    14,   173,    -1,   173,    11,
     173,    -1,   173,    16,   173,    -1,   174,    -1,   204,   182,
     183,    -1,    -1,   174,    -1,   204,   184,   183,    -1,    -1,
      34,   170,    -1,    34,   170,    -1,    81,   192,    82,    -1,
      81,   192,    82,    -1,   189,    -1,    79,   192,    80,    -1,
      79,    45,    80,    -1,   193,    -1,   193,    78,   192,    -1,
      -1,   174,    -1,    32,    -1,   174,    -1,    46,   190,    -1,
      62,   189,    -1,    74,    -1,    75,    -1,    26,    -1,    26,
     173,    -1,   173,    26,    -1,   173,    26,   173,    -1,   173,
      81,   172,    82,   173,    -1,    56,    81,   167,    82,    -1,
     177,    -1,     8,    -1,    14,    -1,    13,    -1,    35,    -1,
      32,    -1,    -1,   197,    76,    -1,   202,    83,    -1,    52,
      83,    -1,    62,    83,    -1,    -1,   197,    -1,    84,    -1,
     205,   219,    76,    -1,   205,    68,    76,    -1,    85,    -1,
     207,    76,    -1,   208,    -1,   208,    78,   207,    -1,    -1,
     213,   209,   212,    -1,    -1,   213,   210,   215,   212,    -1,
      -1,   213,   211,   219,   212,    -1,    -1,   214,   213,    -1,
      -1,    70,    -1,    69,    -1,   218,   216,   217,    -1,     4,
      -1,     3,    -1,     8,    -1,   174,    -1,    32,    -1,   172,
      -1,    71,   221,    -1,    72,   221,    -1,    73,   221,    -1,
      -1,    35,    -1,    36,    -1,   214,    -1,    37,     4,   172,
      -1,    66,   174,    -1,    67,   174,    -1
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
     636,   640,   644,   646,   650,   654,   656,   660,   661,   663,
     665,   667,   670,   673,   675,   677,   679,   682,   685,   689,
     695,   700,   702,   704,   706,   708,   710,   712,   713,   716,
     719,   722,   725,   726,   728,   730,   734,   738,   740,   743,
     745,   749,   750,   754,   755,   760,   761,   766,   767,   770,
     771,   773,   775,   779,   781,   783,   785,   787,   789,   791,
     794,   797,   800,   801,   803,   805,   807,   811,   814
  };

  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
  const unsigned short int
  CMPL_Parser::yyrline_[] =
  {
         0,   195,   195,   196,   201,   202,   203,   204,   211,   211,
     215,   216,   217,   217,   223,   222,   339,   338,   406,   407,
     408,   410,   411,   414,   418,   434,   435,   439,   440,   444,
     445,   446,   447,   448,   452,   453,   457,   460,   461,   464,
     474,   474,   478,   479,   480,   480,   485,   486,   487,   488,
     490,   493,   494,   497,   502,   507,   508,   509,   513,   514,
     520,   539,   542,   543,   550,   550,   554,   555,   556,   556,
     561,   561,   564,   565,   566,   568,   572,   573,   574,   578,
     581,   582,   589,   589,   593,   594,   595,   595,   600,   601,
     602,   603,   605,   609,   609,   620,   621,   625,   628,   629,
     630,   633,   636,   637,   640,   641,   644,   645,   650,   651,
     654,   655,   655,   659,   660,   665,   665,   671,   673,   677,
     680,   683,   688,   689,   693,   696,   697,   705,   706,   707,
     709,   710,   711,   720,   722,   725,   730,   740,   746,   746,
     750,   750,   754,   759,   760,   765,   769,   772,   776,   780,
     784,   792,   812,   813,   814,   817,   818,   822,   826,   842,
     861,   862,   862,   867,   871,   874,   890,   916,   942,   943,
     949,   953,   957,   963,   966,  1003,  1004,  1011,  1012,  1013,
    1014,  1021,  1022,  1023,  1024,  1025,  1026,  1033,  1038,  1038,
    1046,  1046,  1058,  1060,  1062,  1064,  1066,  1068,  1070,  1072,
    1076,  1092,  1096,  1103,  1105,  1107,  1109,  1111,  1113,  1118,
    1120,  1125,  1152,  1158,  1160,  1163,  1169,  1171,  1190,  1220,
    1245,  1274,  1294,  1302,  1310,  1315,  1316,  1325,  1329,  1336,
    1346,  1349,  1388,  1427,  1429,  1431,  1433,  1437,  1441,  1446,
    1451,  1461,  1466,  1467,  1468,  1473,  1477,  1482,  1485,  1496,
    1501,  1505,  1509,  1510,  1514,  1519,  1523,  1529,  1549,  1552,
    1553,  1556,  1556,  1560,  1560,  1564,  1564,  1570,  1582,  1583,
    1586,  1587,  1592,  1606,  1607,  1608,  1611,  1616,  1623,  1629,
    1631,  1633,  1637,  1638,  1639,  1640,  1644,  1659,  1662
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
      81,    82,    19,    17,    78,    18,     2,    20,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    76,    77,
       2,     8,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    79,     2,    80,    23,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    83,    85,    84,     2,     2,     2,     2,
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
      72,    73,    74,    75
    };
    if ((unsigned int) t <= yyuser_token_number_max_)
      return translate_table[t];
    else
      return yyundef_token_;
  }

  const int CMPL_Parser::yyeof_ = 0;
  const int CMPL_Parser::yylast_ = 1714;
  const int CMPL_Parser::yynnts_ = 138;
  const int CMPL_Parser::yyempty_ = -2;
  const int CMPL_Parser::yyfinal_ = 15;
  const int CMPL_Parser::yyterror_ = 1;
  const int CMPL_Parser::yyerrcode_ = 256;
  const int CMPL_Parser::yyntokens_ = 86;

  const unsigned int CMPL_Parser::yyuser_token_number_max_ = 323;
  const CMPL_Parser::token_number_type CMPL_Parser::yyundef_token_ = 2;

} // namespace cmpl

#line 1668 "src/cmpl/CMPL_Parser.yy"


void cmpl::CMPL_Parser::error(const cmpl::CMPL_Parser::location_type& l, const std::string& m)
{
	PARSE_CONTEXT.output_error_parse(ERROR_TYPE_COMP, ERROR_LVL_NORMAL, m.c_str());
}


