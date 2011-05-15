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
#line 82 "src/cmpl/CMPL_Parser.yy"

// Include fuer Compiler.hh darf erst nach %union stehen, damit darin schon cmpl::CMPL_Parser definiert ist
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
#line 144 "src/cmpl/CMPL_Parser.yy"
    { COMP(PROG_END, 0); ;}
    break;

  case 8:
#line 160 "src/cmpl/CMPL_Parser.yy"
    { GLV->sect = (int) token::SECT_PARAM; ;}
    break;

  case 12:
#line 166 "src/cmpl/CMPL_Parser.yy"
    { yyerrok; ;}
    break;

  case 14:
#line 172 "src/cmpl/CMPL_Parser.yy"
    { GLV->index_info.i_akt_ind = 0;
												  GLV->is_scann_const_vekt = true; ;}
    break;

  case 15:
#line 174 "src/cmpl/CMPL_Parser.yy"
    { COMP_SUB_PAR_CNT(GLV->last_bef, 0, (yysemantic_stack_[(6) - (2)].t), 1);
												  COMP_INT((yysemantic_stack_[(6) - (1)].i) +
												  		GLV->index_info.i_anz_ind);
												  GLV->is_scann_const_vekt = false;
												  if ((yysemantic_stack_[(6) - (2)].t) != TPP_EMPTY)
												  {
												  	/* Verschiedene Typklassen sind erstmal akzeptabel, es wird dann Konvertierung versucht */
													PARSE_CONTEXT.set_test_tpcl_symbol((yysemantic_stack_[(6) - (1)].i), TYPECLASS((yysemantic_stack_[(6) - (2)].t)));
												  }
												  else
												  {
												  	/* Alle Werte muessen zur selben Typklasse gehoeren, sonst Fehler */
													if ((yysemantic_stack_[(6) - (5)].t) == TPP_MIX_TPK)
														PARSE_CONTEXT.output_error_parse(ERROR_TYPE_COMP, ERROR_LVL_NORMAL, ERROR_CONST_DEF_TPK_MIX);
													PARSE_CONTEXT.set_test_tpcl_symbol((yysemantic_stack_[(6) - (1)].i), TYPECLASS((yysemantic_stack_[(6) - (5)].t)));
												  }
												;}
    break;

  case 20:
#line 198 "src/cmpl/CMPL_Parser.yy"
    { GLV->last_bef = DEF_CONST;
												  (yyval.i) = (yysemantic_stack_[(1) - (1)].i); ;}
    break;

  case 21:
#line 200 "src/cmpl/CMPL_Parser.yy"
    { GLV->last_bef = DEF_CONST;
												  (yyval.i) = (yysemantic_stack_[(1) - (1)].i); ;}
    break;

  case 22:
#line 202 "src/cmpl/CMPL_Parser.yy"
    { GLV->last_bef = DEF_CONST;
												  (yyval.i) = (yysemantic_stack_[(1) - (1)].i); ;}
    break;

  case 23:
#line 207 "src/cmpl/CMPL_Parser.yy"
    { (yyval.t) = TPP_EMPTY; ;}
    break;

  case 24:
#line 208 "src/cmpl/CMPL_Parser.yy"
    { (yyval.t) = (yysemantic_stack_[(2) - (2)].t); ;}
    break;

  case 25:
#line 212 "src/cmpl/CMPL_Parser.yy"
    { (yyval.t) = TP_DBL; ;}
    break;

  case 26:
#line 213 "src/cmpl/CMPL_Parser.yy"
    { (yyval.t) = TP_INT; ;}
    break;

  case 27:
#line 214 "src/cmpl/CMPL_Parser.yy"
    { (yyval.t) = TP_BIN; ;}
    break;

  case 28:
#line 215 "src/cmpl/CMPL_Parser.yy"
    { (yyval.t) = TP_STR; ;}
    break;

  case 30:
#line 220 "src/cmpl/CMPL_Parser.yy"
    { PARSE_CONTEXT.out_warn_old_assign(false); ;}
    break;

  case 34:
#line 235 "src/cmpl/CMPL_Parser.yy"
    { GLV->sect = (int) token::SECT_VAR; ;}
    break;

  case 38:
#line 241 "src/cmpl/CMPL_Parser.yy"
    { yyerrok; ;}
    break;

  case 40:
#line 246 "src/cmpl/CMPL_Parser.yy"
    { COMP_SUB(DEF_VAR_TYPE, (yysemantic_stack_[(4) - (3)].i)); ;}
    break;

  case 47:
#line 258 "src/cmpl/CMPL_Parser.yy"
    { COMP(DEF_VAR, 1);
									  COMP_INT((yysemantic_stack_[(1) - (1)].i) + GLV->index_info.i_anz_ind); ;}
    break;

  case 48:
#line 260 "src/cmpl/CMPL_Parser.yy"
    { COMP(DEF_VAR, 1);
									  COMP_INT((yysemantic_stack_[(1) - (1)].i) + GLV->index_info.i_anz_ind); ;}
    break;

  case 49:
#line 265 "src/cmpl/CMPL_Parser.yy"
    { (yyval.i) = (int)TP_VAR_DBL; ;}
    break;

  case 50:
#line 266 "src/cmpl/CMPL_Parser.yy"
    { (yyval.i) = (int)TP_VAR_INT; ;}
    break;

  case 51:
#line 267 "src/cmpl/CMPL_Parser.yy"
    { (yyval.i) = (int)TP_VAR_BIN; ;}
    break;

  case 52:
#line 271 "src/cmpl/CMPL_Parser.yy"
    { COMP(RESET_VAR_B, 0); ;}
    break;

  case 53:
#line 272 "src/cmpl/CMPL_Parser.yy"
    { COMP(RESET_VAR_B, 0);
												  if((yysemantic_stack_[(5) - (4)].f)) COMP(DEF_UP_VAR, 0);
												  if((yysemantic_stack_[(5) - (2)].f)) COMP(DEF_DN_VAR, 0); ;}
    break;

  case 54:
#line 278 "src/cmpl/CMPL_Parser.yy"
    { (yysemantic_stack_[(1) - (1)].i) = PARSE_CONTEXT.insert_symbol(GLV->last_str,
										(GLV->sect == (int) token::SECT_PARAM ? TPP_EMPTY /* Typklasse wird spaeter gesetzt */ : TPK_VAR)); ;}
    break;

  case 55:
#line 280 "src/cmpl/CMPL_Parser.yy"
    { COMP(SET_INDEXES, 0);
									  GLV->index_info.i_anz_ind =
													(yysemantic_stack_[(3) - (3)].i) % MAX_ANZ_IND_1;
									  GLV->index_info.i_free_ind =
													(yysemantic_stack_[(3) - (3)].i) / MAX_ANZ_IND_1;
									  (yyval.i) = (yysemantic_stack_[(3) - (1)].i); ;}
    break;

  case 59:
#line 300 "src/cmpl/CMPL_Parser.yy"
    { GLV->sect = (int) token::SECT_OBJ; ;}
    break;

  case 63:
#line 306 "src/cmpl/CMPL_Parser.yy"
    { yyerrok; ;}
    break;

  case 65:
#line 311 "src/cmpl/CMPL_Parser.yy"
    { GLV->is_left = true; ;}
    break;

  case 66:
#line 312 "src/cmpl/CMPL_Parser.yy"
    { COMP_PAR(LINE_MOD, MODE_N);
												  COMP(LINE_END, 0); ;}
    break;

  case 72:
#line 323 "src/cmpl/CMPL_Parser.yy"
    { COMP_PAR(LINE_OBJ_TO, 1); ;}
    break;

  case 73:
#line 324 "src/cmpl/CMPL_Parser.yy"
    { COMP_PAR(LINE_OBJ_TO, -1); ;}
    break;

  case 77:
#line 339 "src/cmpl/CMPL_Parser.yy"
    { GLV->sect = (int) token::SECT_CSTR; ;}
    break;

  case 81:
#line 345 "src/cmpl/CMPL_Parser.yy"
    { yyerrok; ;}
    break;

  case 83:
#line 350 "src/cmpl/CMPL_Parser.yy"
    { COMP(LINE_END, 0); ;}
    break;

  case 88:
#line 359 "src/cmpl/CMPL_Parser.yy"
    { switch((yysemantic_stack_[(2) - (2)].i)) {
												case EQ:
											  		COMP_PAR(LINE_MOD, MODE_G); break;
												case GE:
											  		COMP_PAR(LINE_MOD, MODE_U); break;
												case LE:
											  		COMP_PAR(LINE_MOD, MODE_O); break;
											} ;}
    break;

  case 96:
#line 383 "src/cmpl/CMPL_Parser.yy"
    { (yyval.i) = 1; ;}
    break;

  case 97:
#line 386 "src/cmpl/CMPL_Parser.yy"
    { (yyval.i) = 1; ;}
    break;

  case 98:
#line 387 "src/cmpl/CMPL_Parser.yy"
    { (yyval.i) = -1; ;}
    break;

  case 106:
#line 405 "src/cmpl/CMPL_Parser.yy"
    { COMP(TERM_NEG, 0); /* Negation */ ;}
    break;

  case 107:
#line 406 "src/cmpl/CMPL_Parser.yy"
    { COMP(TERM_NEG, 0); /* Negation (um Negation am Anfang zurueckzunehmen) */ ;}
    break;

  case 110:
#line 415 "src/cmpl/CMPL_Parser.yy"
    { COMP(TERM_BEG, 0);
									  COMP_PAR(LIT_VAL_SHORT, (yysemantic_stack_[(0) - (0)].i)); /* Vorzeichen Term */ ;}
    break;

  case 111:
#line 417 "src/cmpl/CMPL_Parser.yy"
    { COMP(TERM_END, 0);
									  PARSE_CONTEXT.var_test_prod((yysemantic_stack_[(2) - (2)].i));
									;}
    break;

  case 112:
#line 422 "src/cmpl/CMPL_Parser.yy"
    { if((yysemantic_stack_[(1) - (1)].i)==0) COMP_SUB(NUM_OP, MULT);
									  (yyval.i) = (yysemantic_stack_[(1) - (1)].i); ;}
    break;

  case 113:
#line 424 "src/cmpl/CMPL_Parser.yy"
    { if((yysemantic_stack_[(3) - (1)].i)==0) COMP_SUB(NUM_OP, MULT);
									  (yyval.i) = (yysemantic_stack_[(3) - (1)].i) + (yysemantic_stack_[(3) - (3)].i); ;}
    break;

  case 114:
#line 428 "src/cmpl/CMPL_Parser.yy"
    { COMP(TERM_VAR_FREE, 1);
									  COMP_INT((yysemantic_stack_[(2) - (1)].i) + GLV->index_info.i_anz_ind);
									  (yyval.i) = 1; ;}
    break;

  case 115:
#line 431 "src/cmpl/CMPL_Parser.yy"
    { (yyval.i) = 0; ;}
    break;

  case 116:
#line 432 "src/cmpl/CMPL_Parser.yy"
    { COMP(F_CONST_FREE, 1);
									  COMP_INT((yysemantic_stack_[(2) - (1)].i) + GLV->index_info.i_anz_ind);
									  (yyval.i) = 0; ;}
    break;

  case 118:
#line 438 "src/cmpl/CMPL_Parser.yy"
    { COMP(IND_TRANSPOSE, 0); ;}
    break;

  case 122:
#line 454 "src/cmpl/CMPL_Parser.yy"
    { (yyval.t) = (yysemantic_stack_[(1) - (1)].t); ;}
    break;

  case 123:
#line 455 "src/cmpl/CMPL_Parser.yy"
    { (yyval.t) = (yysemantic_stack_[(1) - (1)].t); ;}
    break;

  case 124:
#line 456 "src/cmpl/CMPL_Parser.yy"
    { (yyval.t) = (yysemantic_stack_[(1) - (1)].t); ;}
    break;

  case 125:
#line 457 "src/cmpl/CMPL_Parser.yy"
    { (yyval.t) = (yysemantic_stack_[(3) - (2)].t); ;}
    break;

  case 126:
#line 458 "src/cmpl/CMPL_Parser.yy"
    { COMP(FETCH_TMP, 1);
												  COMP_INT((-(yysemantic_stack_[(1) - (1)].i)-2));
												  (yyval.t) = TPK_VALUE; ;}
    break;

  case 127:
#line 464 "src/cmpl/CMPL_Parser.yy"
    { COMP_PAR_INT(LIT_VAL, (yysemantic_stack_[(1) - (1)].i));
												  (yyval.t) = TP_INT; ;}
    break;

  case 128:
#line 466 "src/cmpl/CMPL_Parser.yy"
    { COMP(LIT_VAL, 1);
												  COMP_DOUBLE((yysemantic_stack_[(1) - (1)].d));
												  (yyval.t) = TP_DBL; ;}
    break;

  case 129:
#line 469 "src/cmpl/CMPL_Parser.yy"
    { COMP(LIT_VAL, 1);
												  COMP_ISTR((yysemantic_stack_[(1) - (1)].i));
												  (yyval.t) = TP_ISTR; ;}
    break;

  case 130:
#line 474 "src/cmpl/CMPL_Parser.yy"
    { COMP_SUB_PAR(CONV_OP, NO_FORMAT, (yysemantic_stack_[(4) - (1)].t));
												  (yyval.t) = (yysemantic_stack_[(4) - (1)].t); ;}
    break;

  case 131:
#line 478 "src/cmpl/CMPL_Parser.yy"
    { (yysemantic_stack_[(2) - (2)].i) = (int) token::FCT_MAX; ;}
    break;

  case 132:
#line 479 "src/cmpl/CMPL_Parser.yy"
    { COMP_PAR_INT(LIT_VAL, (yysemantic_stack_[(5) - (4)].i));
												  COMP_SUB(AGGR_OP, MAX_VAL);
												  (yyval.t) = TPK_VALUE; ;}
    break;

  case 133:
#line 482 "src/cmpl/CMPL_Parser.yy"
    { (yysemantic_stack_[(2) - (2)].i) = (int) token::FCT_MIN; ;}
    break;

  case 134:
#line 483 "src/cmpl/CMPL_Parser.yy"
    { COMP_PAR_INT(LIT_VAL, (yysemantic_stack_[(5) - (4)].i));
												  COMP_SUB(AGGR_OP, MIN_VAL);
												  (yyval.t) = TPK_VALUE; ;}
    break;

  case 135:
#line 486 "src/cmpl/CMPL_Parser.yy"
    { COMP(SET_INDEXES, 0);
												  COMP_SUB_PAR_CNT(DEFSET_EXTNUM, MAX_VAL, 0, 1);
												  COMP_INT((yysemantic_stack_[(4) - (3)].i));
												  (yyval.t) = TP_INT; ;}
    break;

  case 136:
#line 490 "src/cmpl/CMPL_Parser.yy"
    { (yyval.t) = TP_INT; ;}
    break;

  case 137:
#line 492 "src/cmpl/CMPL_Parser.yy"
    { COMP_SUB_PAR(CONV_OP, WITH_FORMAT, TP_ISTR);
												  (yyval.t) = TP_ISTR; ;}
    break;

  case 138:
#line 494 "src/cmpl/CMPL_Parser.yy"
    { COMP_SUB(TYPE_STR, TYPE_EXPR);
												  (yyval.t) = TP_ISTR; ;}
    break;

  case 139:
#line 496 "src/cmpl/CMPL_Parser.yy"
    { COMP_SUB_PAR_CNT(TYPE_STR, TYPE_VAR, 0, 1);
												  COMP_INT((yysemantic_stack_[(4) - (3)].i));
												  (yyval.t) = TP_ISTR; ;}
    break;

  case 140:
#line 499 "src/cmpl/CMPL_Parser.yy"
    { COMP_SUB(UTIL_OP, LEN_VAL);
												  (yyval.t) = TP_INT; ;}
    break;

  case 141:
#line 501 "src/cmpl/CMPL_Parser.yy"
    { COMP_SUB(MATH_FUNC, (yysemantic_stack_[(4) - (1)].i));
												  (yyval.t) = TPU_NUM; ;}
    break;

  case 142:
#line 503 "src/cmpl/CMPL_Parser.yy"
    { COMP(READ_STDIN, 0);
												  (yyval.t)=TPK_VALUE; ;}
    break;

  case 143:
#line 508 "src/cmpl/CMPL_Parser.yy"
    { (yyval.i) = 0; ;}
    break;

  case 144:
#line 512 "src/cmpl/CMPL_Parser.yy"
    { (yyval.i) = (yysemantic_stack_[(2) - (1)].i) + (yysemantic_stack_[(2) - (2)].i); ;}
    break;

  case 145:
#line 516 "src/cmpl/CMPL_Parser.yy"
    { COMP(SET_INDEXES, 0);
											  GLV->index_info.i_anz_ind =
											  		(yysemantic_stack_[(2) - (2)].i) % MAX_ANZ_IND_1;
											  GLV->index_info.i_free_ind =
											  		(yysemantic_stack_[(2) - (2)].i) / MAX_ANZ_IND_1;
											  (yyval.i) = (yysemantic_stack_[(2) - (1)].i); ;}
    break;

  case 146:
#line 525 "src/cmpl/CMPL_Parser.yy"
    { (yyval.i) = 1; ;}
    break;

  case 147:
#line 526 "src/cmpl/CMPL_Parser.yy"
    { (yysemantic_stack_[(2) - (2)].i) = (yysemantic_stack_[(2) - (-1)].i); ;}
    break;

  case 148:
#line 527 "src/cmpl/CMPL_Parser.yy"
    { (yyval.i) = (yysemantic_stack_[(4) - (4)].i) + 1; ;}
    break;

  case 149:
#line 531 "src/cmpl/CMPL_Parser.yy"
    { (yyval.i) = 0; ;}
    break;

  case 151:
#line 536 "src/cmpl/CMPL_Parser.yy"
    { COMP(SET_INDEXES, 0);
											  COMP(FETCH_VALS, 1);
											  COMP_INT((yysemantic_stack_[(2) - (1)].i) +
											  		(yysemantic_stack_[(2) - (2)].i) % MAX_ANZ_IND_1);
											  if((yysemantic_stack_[(2) - (-1)].i) == (int) token::FCT_MAX) COMP_SUB(AGGR_OP, MAX_VAL_NULL);
											  else COMP_SUB(AGGR_OP, MIN_VAL_NULL); ;}
    break;

  case 152:
#line 545 "src/cmpl/CMPL_Parser.yy"
    { (yyval.i) = (yysemantic_stack_[(2) - (1)].i) + (yysemantic_stack_[(2) - (2)].i) % MAX_ANZ_IND_1; ;}
    break;

  case 153:
#line 546 "src/cmpl/CMPL_Parser.yy"
    { (yyval.i) = (yysemantic_stack_[(2) - (1)].i) + (yysemantic_stack_[(2) - (2)].i) % MAX_ANZ_IND_1; ;}
    break;

  case 154:
#line 547 "src/cmpl/CMPL_Parser.yy"
    { (yyval.i) = (yysemantic_stack_[(2) - (1)].i) + (yysemantic_stack_[(2) - (2)].i) % MAX_ANZ_IND_1; ;}
    break;

  case 155:
#line 551 "src/cmpl/CMPL_Parser.yy"
    { COMP(SET_INDEXES, 0);
												  COMP(DEF_FUNC, 1);
												  COMP_INT((yysemantic_stack_[(1) - (1)].i)); ;}
    break;

  case 156:
#line 554 "src/cmpl/CMPL_Parser.yy"
    { COMP(SET_INDEXES, 0);
												  COMP(DEL_INDEXES, 0);
												  COMP_PAR(LIT_VAL_SHORT, 0); ;}
    break;

  case 157:
#line 557 "src/cmpl/CMPL_Parser.yy"
    { COMP_PAR(LIT_VAL_SHORT, 1); ;}
    break;

  case 158:
#line 562 "src/cmpl/CMPL_Parser.yy"
    { COMP_PAR(LIT_VAL_SHORT, 0);
										  (yyval.i) = 0; ;}
    break;

  case 159:
#line 564 "src/cmpl/CMPL_Parser.yy"
    { COMP_PAR_INT(LIT_VAL, ((yysemantic_stack_[(3) - (2)].i) % MAX_ANZ_IND_1));
										  (yyval.i) = (yysemantic_stack_[(3) - (2)].i); ;}
    break;

  case 160:
#line 568 "src/cmpl/CMPL_Parser.yy"
    { (yyval.i) = (yysemantic_stack_[(1) - (1)].i) + 1; ;}
    break;

  case 161:
#line 569 "src/cmpl/CMPL_Parser.yy"
    { (yyval.i) = (yysemantic_stack_[(3) - (1)].i) + 1 + (yysemantic_stack_[(3) - (3)].i);
											  if((yyval.i) % MAX_ANZ_IND_1 == 0)
											  	PARSE_CONTEXT.output_error_parse(ERROR_TYPE_COMP, ERROR_LVL_NORMAL, ERROR_INDIZES_MAX); ;}
    break;

  case 162:
#line 574 "src/cmpl/CMPL_Parser.yy"
    { COMP_PAR(LIT_SET, TP_SET_ALL);
										  (yyval.i) = MAX_ANZ_IND_1; ;}
    break;

  case 163:
#line 576 "src/cmpl/CMPL_Parser.yy"
    { (yyval.i) = (TYPECLASS((yysemantic_stack_[(1) - (1)].t)) == TPK_SET ? MAX_ANZ_IND_1 : 0); ;}
    break;

  case 164:
#line 589 "src/cmpl/CMPL_Parser.yy"
    { (yyval.f) = false; ;}
    break;

  case 165:
#line 590 "src/cmpl/CMPL_Parser.yy"
    { (yyval.f) = true; ;}
    break;

  case 166:
#line 594 "src/cmpl/CMPL_Parser.yy"
    { (yyval.t) = (yysemantic_stack_[(1) - (1)].t); ;}
    break;

  case 167:
#line 595 "src/cmpl/CMPL_Parser.yy"
    { (yyval.t) = (yysemantic_stack_[(1) - (1)].t); ;}
    break;

  case 168:
#line 596 "src/cmpl/CMPL_Parser.yy"
    { (yyval.t) = (yysemantic_stack_[(1) - (1)].t); ;}
    break;

  case 169:
#line 597 "src/cmpl/CMPL_Parser.yy"
    { (yyval.t) = (yysemantic_stack_[(1) - (1)].t); ;}
    break;

  case 170:
#line 603 "src/cmpl/CMPL_Parser.yy"
    { (yyval.t) = (yysemantic_stack_[(1) - (1)].t); ;}
    break;

  case 171:
#line 604 "src/cmpl/CMPL_Parser.yy"
    { (yyval.t) = (yysemantic_stack_[(1) - (1)].t); ;}
    break;

  case 172:
#line 605 "src/cmpl/CMPL_Parser.yy"
    { COMP(FETCH_CONST, 1);
												  COMP_INT((yysemantic_stack_[(1) - (1)].i));
												  (yyval.t) = TPK_VALUE; ;}
    break;

  case 173:
#line 608 "src/cmpl/CMPL_Parser.yy"
    { (yyval.t) = (yysemantic_stack_[(1) - (1)].t); ;}
    break;

  case 174:
#line 609 "src/cmpl/CMPL_Parser.yy"
    { COMP_SUB(LOG_OP, NOT);
												  (yyval.t) = TP_BIN; ;}
    break;

  case 175:
#line 613 "src/cmpl/CMPL_Parser.yy"
    { (yyval.t) = (yysemantic_stack_[(1) - (1)].t); ;}
    break;

  case 176:
#line 614 "src/cmpl/CMPL_Parser.yy"
    { (yyval.t) = (yysemantic_stack_[(1) - (1)].t); ;}
    break;

  case 177:
#line 618 "src/cmpl/CMPL_Parser.yy"
    { COMP(LIT_VAL, 1);
												  COMP_VAL(TPP_NULL, i, 0); ;}
    break;

  case 178:
#line 621 "src/cmpl/CMPL_Parser.yy"
    { (yyval.t) = PARSE_CONTEXT.parse_type_dev_num_op(ADD, TPP_NULL, PARSE_CONTEXT.parse_type_dev_switch((yysemantic_stack_[(6) - (4)].t), (yysemantic_stack_[(6) - (5)].t))); ;}
    break;

  case 179:
#line 624 "src/cmpl/CMPL_Parser.yy"
    { COMP_PAR(LIT_VAL_SHORT, 0); ;}
    break;

  case 180:
#line 626 "src/cmpl/CMPL_Parser.yy"
    { (yyval.t) = PARSE_CONTEXT.parse_type_dev_num_op(ADD, TP_INT, PARSE_CONTEXT.parse_type_dev_switch((yysemantic_stack_[(6) - (4)].t), (yysemantic_stack_[(6) - (5)].t)));
												  if (TYPECLASS((yyval.t)) != TPK_VALUE)
													PARSE_CONTEXT.output_error_parse(ERROR_TYPE_COMP, ERROR_LVL_NORMAL, ERROR_NOT_A_NUMBER_PARSE);
												  COMP_PAR(LIT_SET, TP_SET_ENUM);
												  (yyval.t) = TPK_SET; ;}
    break;

  case 181:
#line 634 "src/cmpl/CMPL_Parser.yy"
    { COMP_SUB(NUM_OP, ADD);
															  (yyval.t) = PARSE_CONTEXT.parse_type_dev_num_op(ADD, (yysemantic_stack_[(3) - (1)].t), (yysemantic_stack_[(3) - (3)].t)); ;}
    break;

  case 182:
#line 636 "src/cmpl/CMPL_Parser.yy"
    { COMP_SUB(NUM_OP, SUB);
															  (yyval.t) = PARSE_CONTEXT.parse_type_dev_num_op(SUB, (yysemantic_stack_[(3) - (1)].t), (yysemantic_stack_[(3) - (3)].t)); ;}
    break;

  case 183:
#line 638 "src/cmpl/CMPL_Parser.yy"
    { COMP_SUB(NUM_OP, MULT);
															  (yyval.t) = PARSE_CONTEXT.parse_type_dev_num_op(MULT, (yysemantic_stack_[(3) - (1)].t), (yysemantic_stack_[(3) - (3)].t)); ;}
    break;

  case 184:
#line 640 "src/cmpl/CMPL_Parser.yy"
    { COMP_SUB(NUM_OP, DIVD);
															  (yyval.t) = PARSE_CONTEXT.parse_type_dev_num_op(DIVD, (yysemantic_stack_[(3) - (1)].t), (yysemantic_stack_[(3) - (3)].t)); ;}
    break;

  case 185:
#line 642 "src/cmpl/CMPL_Parser.yy"
    { COMP_SUB(NUM_OP, POW);
															  (yyval.t) = PARSE_CONTEXT.parse_type_dev_num_op(POW, (yysemantic_stack_[(3) - (1)].t), (yysemantic_stack_[(3) - (3)].t)); ;}
    break;

  case 186:
#line 644 "src/cmpl/CMPL_Parser.yy"
    { COMP_SUB(NUM_OP, DIVI);
															  (yyval.t) = PARSE_CONTEXT.parse_type_dev_num_op(DIVI, (yysemantic_stack_[(3) - (1)].t), (yysemantic_stack_[(3) - (3)].t)); ;}
    break;

  case 187:
#line 646 "src/cmpl/CMPL_Parser.yy"
    { COMP_SUB(NUM_OP, MODI);
															  (yyval.t) = PARSE_CONTEXT.parse_type_dev_num_op(MODI, (yysemantic_stack_[(3) - (1)].t), (yysemantic_stack_[(3) - (3)].t)); ;}
    break;

  case 188:
#line 648 "src/cmpl/CMPL_Parser.yy"
    { COMP_SUB(NUM_OP, NEG);
															  (yyval.t) = PARSE_CONTEXT.parse_type_dev_num_op(NEG, (yysemantic_stack_[(2) - (2)].t), TPP_EMPTY); ;}
    break;

  case 189:
#line 650 "src/cmpl/CMPL_Parser.yy"
    { COMP_SUB_PAR(CONV_OP, NO_FORMAT, TPU_NUM);	/* fuer sichern nummerischer Wert */
															  (yyval.t) = PARSE_CONTEXT.parse_type_dev_num_op(NEG, (yysemantic_stack_[(2) - (2)].t), TPP_EMPTY); ;}
    break;

  case 190:
#line 655 "src/cmpl/CMPL_Parser.yy"
    { COMP_SUB(SET_OP, ADD);
															  (yyval.t) = TPK_SET; ;}
    break;

  case 191:
#line 657 "src/cmpl/CMPL_Parser.yy"
    { COMP_SUB(SET_OP, MULT);
															  (yyval.t) = TPK_SET; ;}
    break;

  case 192:
#line 662 "src/cmpl/CMPL_Parser.yy"
    { COMP_SUB(LOG_OP, AND);
															  (yyval.t) = TP_BIN; ;}
    break;

  case 193:
#line 664 "src/cmpl/CMPL_Parser.yy"
    { COMP_SUB(LOG_OP, OR);
															  (yyval.t) = TP_BIN; ;}
    break;

  case 194:
#line 669 "src/cmpl/CMPL_Parser.yy"
    { COMP_SUB(CMP_OP, EQ);
															  (yyval.t) = PARSE_CONTEXT.parse_type_dev_cmp_op(EQ, (yysemantic_stack_[(3) - (1)].t), (yysemantic_stack_[(3) - (3)].t)); ;}
    break;

  case 195:
#line 671 "src/cmpl/CMPL_Parser.yy"
    { COMP_SUB(CMP_OP, EQ2);
															  (yyval.t) = PARSE_CONTEXT.parse_type_dev_cmp_op(EQ2, (yysemantic_stack_[(3) - (1)].t), (yysemantic_stack_[(3) - (3)].t)); ;}
    break;

  case 196:
#line 673 "src/cmpl/CMPL_Parser.yy"
    { COMP_SUB(CMP_OP, EQ); COMP_SUB(LOG_OP, NOT);
															  (yyval.t) = PARSE_CONTEXT.parse_type_dev_cmp_op(EQ, (yysemantic_stack_[(3) - (1)].t), (yysemantic_stack_[(3) - (3)].t)); ;}
    break;

  case 197:
#line 675 "src/cmpl/CMPL_Parser.yy"
    { COMP_SUB(CMP_OP, EQ2); COMP_SUB(LOG_OP, NOT);
															  (yyval.t) = PARSE_CONTEXT.parse_type_dev_cmp_op(EQ2, (yysemantic_stack_[(3) - (1)].t), (yysemantic_stack_[(3) - (3)].t)); ;}
    break;

  case 198:
#line 677 "src/cmpl/CMPL_Parser.yy"
    { COMP_SUB(CMP_OP, LE);
															  (yyval.t) = PARSE_CONTEXT.parse_type_dev_cmp_op(LE, (yysemantic_stack_[(3) - (1)].t), (yysemantic_stack_[(3) - (3)].t)); ;}
    break;

  case 199:
#line 679 "src/cmpl/CMPL_Parser.yy"
    { COMP_SUB(CMP_OP, LE); COMP_SUB(LOG_OP, NOT);
															  (yyval.t) = PARSE_CONTEXT.parse_type_dev_cmp_op(GT, (yysemantic_stack_[(3) - (1)].t), (yysemantic_stack_[(3) - (3)].t)); ;}
    break;

  case 200:
#line 681 "src/cmpl/CMPL_Parser.yy"
    { COMP_SUB(CMP_OP, GE);
															  (yyval.t) = PARSE_CONTEXT.parse_type_dev_cmp_op(GE, (yysemantic_stack_[(3) - (1)].t), (yysemantic_stack_[(3) - (3)].t)); ;}
    break;

  case 201:
#line 683 "src/cmpl/CMPL_Parser.yy"
    { COMP_SUB(CMP_OP, GE); COMP_SUB(LOG_OP, NOT);
															  (yyval.t) = PARSE_CONTEXT.parse_type_dev_cmp_op(LT, (yysemantic_stack_[(3) - (1)].t), (yysemantic_stack_[(3) - (3)].t)); ;}
    break;

  case 202:
#line 688 "src/cmpl/CMPL_Parser.yy"
    { COMP_SUB(CMP_OP, EQ);
															  (yyval.t) = PARSE_CONTEXT.parse_type_dev_cmp_op(EQ, (yysemantic_stack_[(3) - (1)].t), (yysemantic_stack_[(3) - (3)].t)); ;}
    break;

  case 203:
#line 690 "src/cmpl/CMPL_Parser.yy"
    { COMP_SUB(CMP_OP, EQ2);
															  (yyval.t) = PARSE_CONTEXT.parse_type_dev_cmp_op(EQ2, (yysemantic_stack_[(3) - (1)].t), (yysemantic_stack_[(3) - (3)].t)); ;}
    break;

  case 204:
#line 692 "src/cmpl/CMPL_Parser.yy"
    { COMP_SUB(CMP_OP, EQ); COMP_SUB(LOG_OP, NOT);
															  (yyval.t) = PARSE_CONTEXT.parse_type_dev_cmp_op(EQ, (yysemantic_stack_[(3) - (1)].t), (yysemantic_stack_[(3) - (3)].t)); ;}
    break;

  case 205:
#line 694 "src/cmpl/CMPL_Parser.yy"
    { COMP_SUB(CMP_OP, EQ2); COMP_SUB(LOG_OP, NOT);
															  (yyval.t) = PARSE_CONTEXT.parse_type_dev_cmp_op(EQ2, (yysemantic_stack_[(3) - (1)].t), (yysemantic_stack_[(3) - (3)].t)); ;}
    break;

  case 206:
#line 696 "src/cmpl/CMPL_Parser.yy"
    { COMP_SUB(CMP_OP, LE);
															  (yyval.t) = PARSE_CONTEXT.parse_type_dev_cmp_op(LE, (yysemantic_stack_[(3) - (1)].t), (yysemantic_stack_[(3) - (3)].t)); ;}
    break;

  case 207:
#line 698 "src/cmpl/CMPL_Parser.yy"
    { COMP_SUB(CMP_OP, LT);
															  (yyval.t) = PARSE_CONTEXT.parse_type_dev_cmp_op(LT, (yysemantic_stack_[(3) - (1)].t), (yysemantic_stack_[(3) - (3)].t)); ;}
    break;

  case 208:
#line 700 "src/cmpl/CMPL_Parser.yy"
    { COMP_SUB(CMP_OP, GE);
															  (yyval.t) = PARSE_CONTEXT.parse_type_dev_cmp_op(GE, (yysemantic_stack_[(3) - (1)].t), (yysemantic_stack_[(3) - (3)].t)); ;}
    break;

  case 209:
#line 702 "src/cmpl/CMPL_Parser.yy"
    { COMP_SUB(CMP_OP, GT);
															  (yyval.t) = PARSE_CONTEXT.parse_type_dev_cmp_op(GT, (yysemantic_stack_[(3) - (1)].t), (yysemantic_stack_[(3) - (3)].t)); ;}
    break;

  case 210:
#line 704 "src/cmpl/CMPL_Parser.yy"
    { COMP_SUB(CMP_OP, INSET);
															  (yyval.t) = PARSE_CONTEXT.parse_type_dev_cmp_op(INSET, (yysemantic_stack_[(3) - (1)].t), (yysemantic_stack_[(3) - (3)].t)); ;}
    break;

  case 211:
#line 708 "src/cmpl/CMPL_Parser.yy"
    { COMP_SUB(NUM_OP, ADD);
												 			  (yyval.t) = (yysemantic_stack_[(1) - (1)].t); ;}
    break;

  case 212:
#line 713 "src/cmpl/CMPL_Parser.yy"
    { (yyval.t) = PARSE_CONTEXT.parse_type_dev_switch((yysemantic_stack_[(3) - (2)].t), (yysemantic_stack_[(3) - (3)].t)); ;}
    break;

  case 213:
#line 714 "src/cmpl/CMPL_Parser.yy"
    { (yyval.t) = TPP_EMPTY; ;}
    break;

  case 214:
#line 717 "src/cmpl/CMPL_Parser.yy"
    { COMP_SUB(COUNT_OP, CNT_INC);
												 			  (yyval.t) = (yysemantic_stack_[(1) - (1)].t); ;}
    break;

  case 215:
#line 722 "src/cmpl/CMPL_Parser.yy"
    { (yyval.t) = PARSE_CONTEXT.parse_type_dev_switch((yysemantic_stack_[(3) - (2)].t), (yysemantic_stack_[(3) - (3)].t)); ;}
    break;

  case 216:
#line 723 "src/cmpl/CMPL_Parser.yy"
    { (yyval.t) = TPP_EMPTY; ;}
    break;

  case 217:
#line 727 "src/cmpl/CMPL_Parser.yy"
    { (yyval.i) = 0; ;}
    break;

  case 218:
#line 728 "src/cmpl/CMPL_Parser.yy"
    { (yyval.i) = (yysemantic_stack_[(3) - (2)].i); ;}
    break;

  case 219:
#line 731 "src/cmpl/CMPL_Parser.yy"
    { (yyval.i) = 1; ;}
    break;

  case 220:
#line 732 "src/cmpl/CMPL_Parser.yy"
    { (yyval.i) = (yysemantic_stack_[(3) - (3)].i) + 1; ;}
    break;

  case 222:
#line 739 "src/cmpl/CMPL_Parser.yy"
    { COMP(SET_INDEXES, 0);
											  GLV->index_info.i_anz_ind =
											  		(yysemantic_stack_[(2) - (2)].i) % MAX_ANZ_IND_1;
											  GLV->index_info.i_free_ind =
											  		(yysemantic_stack_[(2) - (2)].i) / MAX_ANZ_IND_1;
											  (yyval.i) = (yysemantic_stack_[(2) - (1)].i); ;}
    break;

  case 223:
#line 747 "src/cmpl/CMPL_Parser.yy"
    { COMP(SET_INDEXES, 0);
											  GLV->index_info.i_anz_ind =
											  		(yysemantic_stack_[(2) - (2)].i) % MAX_ANZ_IND_1;
											  GLV->index_info.i_free_ind =
											  		(yysemantic_stack_[(2) - (2)].i) / MAX_ANZ_IND_1;
											  (yyval.i) = (yysemantic_stack_[(2) - (1)].i); ;}
    break;

  case 224:
#line 756 "src/cmpl/CMPL_Parser.yy"
    { (yyval.i) = (yysemantic_stack_[(2) - (1)].i) + (yysemantic_stack_[(2) - (2)].i); ;}
    break;

  case 225:
#line 759 "src/cmpl/CMPL_Parser.yy"
    { (yyval.i) = (yysemantic_stack_[(2) - (1)].i) + (yysemantic_stack_[(2) - (2)].i); ;}
    break;

  case 226:
#line 763 "src/cmpl/CMPL_Parser.yy"
    { (yyval.t) = (yysemantic_stack_[(1) - (1)].t); ;}
    break;

  case 227:
#line 764 "src/cmpl/CMPL_Parser.yy"
    { GLV->is_scann_const_vekt = false; ;}
    break;

  case 228:
#line 765 "src/cmpl/CMPL_Parser.yy"
    { COMP_PAR(READ_CSV, GLV->index_info.i_free_ind);
													  (yyval.t)=TPK_VALUE; ;}
    break;

  case 229:
#line 768 "src/cmpl/CMPL_Parser.yy"
    { if(GLV->index_info.i_akt_ind != GLV->index_info.i_free_ind)
														 PARSE_CONTEXT.output_error_parse(ERROR_TYPE_COMP, ERROR_LVL_NORMAL, ERROR_CONST_DEF_DIM);
													  (yyval.t) = (yysemantic_stack_[(1) - (1)].t); ;}
    break;

  case 230:
#line 772 "src/cmpl/CMPL_Parser.yy"
    { GLV->index_info.i_akt_ind--;
													  COMP_PAR_INT(LIT_VAL, (yysemantic_stack_[(3) - (2)].ti).ti_i);
													  (yyval.t) = (yysemantic_stack_[(3) - (2)].ti).ti_t; ;}
    break;

  case 231:
#line 777 "src/cmpl/CMPL_Parser.yy"
    { (yyval.ti).ti_i = 1;
													  (yyval.ti).ti_t = (yysemantic_stack_[(1) - (1)].t); ;}
    break;

  case 232:
#line 779 "src/cmpl/CMPL_Parser.yy"
    { (yyval.ti).ti_i = (yysemantic_stack_[(3) - (3)].ti).ti_i + 1;
													  (yyval.ti).ti_t = PARSE_CONTEXT.parse_type_dev_array((yysemantic_stack_[(3) - (3)].ti).ti_t, (yysemantic_stack_[(3) - (1)].t)); ;}
    break;

  case 233:
#line 783 "src/cmpl/CMPL_Parser.yy"
    { (yyval.ti).ti_i = 1;
													  (yyval.ti).ti_t = (yysemantic_stack_[(1) - (1)].t); ;}
    break;

  case 234:
#line 785 "src/cmpl/CMPL_Parser.yy"
    { (yyval.ti).ti_i = (yysemantic_stack_[(3) - (3)].ti).ti_i + 1;
													  (yyval.ti).ti_t = PARSE_CONTEXT.parse_type_dev_array((yysemantic_stack_[(3) - (3)].ti).ti_t, (yysemantic_stack_[(3) - (1)].t)); ;}
    break;

  case 235:
#line 789 "src/cmpl/CMPL_Parser.yy"
    { COMP_PAR(LIT_SET, TP_SET_EMPTY);
													  (yyval.t) = TP_SET_EMPTY; ;}
    break;

  case 236:
#line 791 "src/cmpl/CMPL_Parser.yy"
    { (yyval.t) = PARSE_CONTEXT.parse_type_dev_lit_set(TP_SET_ENUM, (yysemantic_stack_[(4) - (3)].ti).ti_t, TPP_NULL, TPP_NULL);
													  COMP_PAR(LIT_VAL_SHORT, (yysemantic_stack_[(4) - (3)].ti).ti_i);
													  COMP_PAR(LIT_SET, TP_SET_ENUM); ;}
    break;

  case 237:
#line 794 "src/cmpl/CMPL_Parser.yy"
    { COMP_PAR(LIT_SET, TP_SET_ALL_NUM);
													  (yyval.t) = TP_SET_ALL_NUM; ;}
    break;

  case 238:
#line 796 "src/cmpl/CMPL_Parser.yy"
    { COMP_PAR(LIT_SET, TP_SET_ALL_STR);
													  (yyval.t) = TP_SET_ALL_STR; ;}
    break;

  case 239:
#line 798 "src/cmpl/CMPL_Parser.yy"
    { COMP_PAR(LIT_SET, TP_SET_ALL);
													  (yyval.t) = TP_SET_ALL; ;}
    break;

  case 240:
#line 800 "src/cmpl/CMPL_Parser.yy"
    { (yyval.t) = PARSE_CONTEXT.parse_type_dev_lit_set(TP_SET_HALF_UB, (yysemantic_stack_[(2) - (2)].t), TPP_NULL, TPP_NULL);
													  COMP_PAR(LIT_SET, TP_SET_HALF_UB); ;}
    break;

  case 241:
#line 802 "src/cmpl/CMPL_Parser.yy"
    { (yyval.t) = PARSE_CONTEXT.parse_type_dev_lit_set(TP_SET_HALF_LB, (yysemantic_stack_[(2) - (1)].t), TPP_NULL, TPP_NULL);
													  COMP_PAR(LIT_SET, TP_SET_HALF_LB); ;}
    break;

  case 242:
#line 805 "src/cmpl/CMPL_Parser.yy"
    { (yyval.t) = PARSE_CONTEXT.parse_type_dev_lit_set(TP_SET_ALG, (yysemantic_stack_[(3) - (1)].t), TP_INT, (yysemantic_stack_[(3) - (3)].t));
													  COMP_PAR(LIT_SET, TP_SET_ALG_S1); ;}
    break;

  case 243:
#line 808 "src/cmpl/CMPL_Parser.yy"
    { (yyval.t) = PARSE_CONTEXT.parse_type_dev_lit_set(TP_SET_ALG, (yysemantic_stack_[(5) - (1)].t), (yysemantic_stack_[(5) - (3)].t), (yysemantic_stack_[(5) - (5)].t));
													  COMP_PAR(LIT_SET, TP_SET_ALG); ;}
    break;

  case 244:
#line 810 "src/cmpl/CMPL_Parser.yy"
    { COMP_SUB_PAR(CONV_OP, NO_FORMAT, TPK_SET);
													  (yyval.t) = TPK_SET; ;}
    break;

  case 245:
#line 812 "src/cmpl/CMPL_Parser.yy"
    { COMP(SET_INDEXES, 0);
													  COMP(DEFSET_FUNC, 1);
													  COMP_INT((yysemantic_stack_[(4) - (3)].i));
													  (yyval.t) = TPK_SET; ;}
    break;

  case 246:
#line 816 "src/cmpl/CMPL_Parser.yy"
    { COMP(FETCH_CONST, 1);
													  COMP_INT((yysemantic_stack_[(1) - (1)].i));
													  (yyval.t) = TPK_SET; ;}
    break;

  case 247:
#line 819 "src/cmpl/CMPL_Parser.yy"
    { (yyval.t) = (yysemantic_stack_[(3) - (2)].t); ;}
    break;

  case 248:
#line 820 "src/cmpl/CMPL_Parser.yy"
    { (yyval.t) = (yysemantic_stack_[(1) - (1)].t); ;}
    break;

  case 249:
#line 821 "src/cmpl/CMPL_Parser.yy"
    { (yyval.t) = TPK_SET; ;}
    break;

  case 250:
#line 826 "src/cmpl/CMPL_Parser.yy"
    { (yyval.i) = EQ; ;}
    break;

  case 251:
#line 827 "src/cmpl/CMPL_Parser.yy"
    { (yyval.i) = GE; ;}
    break;

  case 252:
#line 828 "src/cmpl/CMPL_Parser.yy"
    { (yyval.i) = LE; ;}
    break;

  case 253:
#line 832 "src/cmpl/CMPL_Parser.yy"
    { COMP(LINE_BEG, 1);
									  PARSE_CONTEXT.linename_repl(GLV->last_str, (char *)0);
									  COMP_STR(GLV->last_str);    ;}
    break;

  case 254:
#line 835 "src/cmpl/CMPL_Parser.yy"
    { COMP(LINE_BEG, 1);
									  PARSE_CONTEXT.linename_repl(GLV->last_str, GLV->last_str);
									  COMP_STR(GLV->last_str);    ;}
    break;

  case 255:
#line 838 "src/cmpl/CMPL_Parser.yy"
    { COMP(LINE_BEG, 1);
									  PARSE_CONTEXT.linename_repl(GLV->last_str, GLV->last_str);
									  COMP_STR(GLV->last_str);    ;}
    break;

  case 256:
#line 847 "src/cmpl/CMPL_Parser.yy"
    { /* Naechsten loop-Kontexte initialisieren, als Basis */
												  PARSE_CONTEXT.init_loop_comp(GLV->tmp_level, true, LOOP_EXEC, (yysemantic_stack_[(2) - (1)].s)); ;}
    break;

  case 257:
#line 851 "src/cmpl/CMPL_Parser.yy"
    { /* Naechsten loop-Kontexte initialisieren, als Basis fuer Summe */
												  PARSE_CONTEXT.init_loop_comp(GLV->tmp_level, true, LOOP_SUM, (char *)0); ;}
    break;

  case 258:
#line 855 "src/cmpl/CMPL_Parser.yy"
    { /* Naechsten loop-Kontexte initialisieren, als Basis fuer Set */
												  PARSE_CONTEXT.init_loop_comp(GLV->tmp_level, true, LOOP_SET, (char *)0); ;}
    break;

  case 259:
#line 859 "src/cmpl/CMPL_Parser.yy"
    { (yyval.s) = strcpy((char *) (GLV->c_getmem(sizeof(char) * strlen(GLV->last_str))), GLV->last_str); ;}
    break;

  case 260:
#line 860 "src/cmpl/CMPL_Parser.yy"
    { (yyval.s) = strcpy((char *) (GLV->c_getmem(sizeof(char) * strlen(GLV->last_str))), GLV->last_str); ;}
    break;

  case 261:
#line 861 "src/cmpl/CMPL_Parser.yy"
    { (yyval.s) = (char *)0; ;}
    break;

  case 262:
#line 864 "src/cmpl/CMPL_Parser.yy"
    { /* Setzt loop-Kontexte bis einschliesslich naechstem Basiskontext zurueck; compiliert entsprechende Beendigungen */
												  PARSE_CONTEXT.unset_loop_comp(); ;}
    break;

  case 263:
#line 869 "src/cmpl/CMPL_Parser.yy"
    { /* Test fuer Alternative compilieren */
												  loop_ctrl_str *loopp = GLV->tmp_table + (GLV->tmp_level - 1);
												  COMP_SUB_PAR_CNT(TEST_ALTER, TEST_ALTER_IF, GLV->tmp_level - 1, 1);
												  loopp->last_jump = COMP_ADDR(0); ;}
    break;

  case 264:
#line 873 "src/cmpl/CMPL_Parser.yy"
    { /* Beginn Default-Alternative compilieren */
												  loop_ctrl_str *loopp = GLV->tmp_table + (GLV->tmp_level - 1);
												  COMP_SUB_PAR(TEST_ALTER, TEST_ALTER_DEF, GLV->tmp_level - 1);
												  loopp->last_jump = 0; ;}
    break;

  case 265:
#line 879 "src/cmpl/CMPL_Parser.yy"
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

  case 269:
#line 906 "src/cmpl/CMPL_Parser.yy"
    { /* Leeren loop-Kontext erzeugen */
												  loop_ctrl_str *loopp = PARSE_CONTEXT.set_loop_comp(START_EMPTY, 0, (char *)0, true);
												  loopp->loop_start = ADDR_COMP; ;}
    break;

  case 271:
#line 910 "src/cmpl/CMPL_Parser.yy"
    { /* loop-Kontext muss jetzt reserviert werden, da in Zuweisung Summenschleifen vorkommen könnten */
												  loop_ctrl_str *loopp = PARSE_CONTEXT.set_loop_comp(START_ASSIGN, (yysemantic_stack_[(1) - (1)].i), (char *)0, true);
												  loopp->loop_start = ADDR_COMP; ;}
    break;

  case 273:
#line 914 "src/cmpl/CMPL_Parser.yy"
    { /* loop-Kontext muss jetzt reserviert werden, da in loop_if Summenschleifen vorkommen könnten */
												  loop_ctrl_str *loopp = PARSE_CONTEXT.set_loop_comp(START_IF, (yysemantic_stack_[(1) - (1)].i), (char *)0, true);
												  loopp->loop_start = ADDR_COMP; ;}
    break;

  case 275:
#line 920 "src/cmpl/CMPL_Parser.yy"
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

  case 276:
#line 932 "src/cmpl/CMPL_Parser.yy"
    { (yyval.i) = (yysemantic_stack_[(2) - (1)].i) | (yysemantic_stack_[(2) - (2)].i); ;}
    break;

  case 277:
#line 933 "src/cmpl/CMPL_Parser.yy"
    { (yyval.i) = 0; ;}
    break;

  case 278:
#line 936 "src/cmpl/CMPL_Parser.yy"
    { (yyval.i) = LOOP_MOD_WHILE; ;}
    break;

  case 279:
#line 942 "src/cmpl/CMPL_Parser.yy"
    { /* Unterscheidung ob Iteration über Set oder lokale Definition */
												  unsigned short sub = ((yysemantic_stack_[(3) - (2)].i) == 2 || TYPECLASS((yysemantic_stack_[(3) - (3)].t)) == TPK_SET ? START_LOOP : START_ASSIGN);
												  /* Art und Name der Laufkonstante eintragen */
												  PARSE_CONTEXT.set_loop_sname(sub, (yysemantic_stack_[(3) - (1)].s));
												  /* Wenn $2 == 0 (alte Zuweisung): Wenn $3 != TPK_SET (einfacher Wert), dann Fehler; sonst Warnung */
												  if ((yysemantic_stack_[(3) - (2)].i) == 0) PARSE_CONTEXT.out_warn_old_assign((TYPECLASS((yysemantic_stack_[(3) - (3)].t)) != TPK_SET));
												;}
    break;

  case 280:
#line 951 "src/cmpl/CMPL_Parser.yy"
    { (yyval.i) = 1; ;}
    break;

  case 281:
#line 952 "src/cmpl/CMPL_Parser.yy"
    { (yyval.i) = 2; ;}
    break;

  case 282:
#line 953 "src/cmpl/CMPL_Parser.yy"
    { (yyval.i) = 0; ;}
    break;

  case 283:
#line 956 "src/cmpl/CMPL_Parser.yy"
    { (yyval.t) = (yysemantic_stack_[(1) - (1)].t); ;}
    break;

  case 284:
#line 959 "src/cmpl/CMPL_Parser.yy"
    { (yyval.s) = strcpy((char *) (GLV->c_getmem(sizeof(char) * strlen(GLV->last_str))), GLV->last_str); ;}
    break;

  case 285:
#line 963 "src/cmpl/CMPL_Parser.yy"
    { (yyval.t) = (yysemantic_stack_[(1) - (1)].t); ;}
    break;

  case 286:
#line 967 "src/cmpl/CMPL_Parser.yy"
    { loop_ctrl_str *loopp = PARSE_CONTEXT.search_loop_name_mod((yysemantic_stack_[(2) - (2)].i_s).s, (yysemantic_stack_[(2) - (2)].i_s).i, "break", false);
												  if (loopp) { COMP_SUB_PAR_CNT(GOTO, GOTO_INDIREKT, 0, 1); COMP_ADDR(loopp->end_jump); } ;}
    break;

  case 287:
#line 969 "src/cmpl/CMPL_Parser.yy"
    { loop_ctrl_str *loopp = PARSE_CONTEXT.search_loop_name_mod((yysemantic_stack_[(2) - (2)].i_s).s, (yysemantic_stack_[(2) - (2)].i_s).i, "continue", true);
												  if (loopp) { COMP_SUB_PAR_CNT(GOTO, GOTO_DIREKT, 0, 1); COMP_ADDR(loopp->iter_start); } ;}
    break;

  case 288:
#line 971 "src/cmpl/CMPL_Parser.yy"
    { loop_ctrl_str *loopp = PARSE_CONTEXT.search_loop_name_mod((yysemantic_stack_[(2) - (2)].i_s).s, (yysemantic_stack_[(2) - (2)].i_s).i, "repeat", false);
												  if (loopp) { COMP_SUB_PAR_CNT(GOTO, GOTO_DIREKT, 0, 1); COMP_ADDR(loopp->loop_start); } ;}
    break;

  case 289:
#line 975 "src/cmpl/CMPL_Parser.yy"
    { (yyval.i_s).s = GLV->last_str; (yyval.i_s).i = 0; ;}
    break;

  case 290:
#line 976 "src/cmpl/CMPL_Parser.yy"
    { (yyval.i_s).s = GLV->last_str; (yyval.i_s).i = 0; ;}
    break;

  case 291:
#line 977 "src/cmpl/CMPL_Parser.yy"
    { (yyval.i_s).s = GLV->last_str; (yyval.i_s).i = 0; ;}
    break;

  case 292:
#line 978 "src/cmpl/CMPL_Parser.yy"
    { (yyval.i_s).s = (char *)0; (yyval.i_s).i = (yysemantic_stack_[(1) - (1)].i); ;}
    break;

  case 293:
#line 979 "src/cmpl/CMPL_Parser.yy"
    { (yyval.i_s).s = (char *)0; (yyval.i_s).i = 0; ;}
    break;

  case 294:
#line 983 "src/cmpl/CMPL_Parser.yy"
    { COMP(STORE_TMP, 1);
												  COMP_INT((-(yysemantic_stack_[(3) - (1)].i)-2)); ;}
    break;

  case 295:
#line 993 "src/cmpl/CMPL_Parser.yy"
    { COMP_SUB(OUTPUT_MSG, ERROR_LVL_FATAL); ;}
    break;

  case 296:
#line 994 "src/cmpl/CMPL_Parser.yy"
    { COMP_SUB(OUTPUT_MSG, USR_MSG_NL); ;}
    break;


    /* Line 675 of lalr1.cc.  */
#line 1699 "src/cmpl/_CMPL_Parser.cc"
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
  const short int CMPL_Parser::yypact_ninf_ = -437;
  const short int
  CMPL_Parser::yypact_[] =
  {
       223,  -437,  -437,  -437,  -437,    40,   223,  -437,  -437,  -437,
    -437,    -3,     3,    41,    45,  -437,  -437,   983,   996,   871,
     927,    32,    53,   102,   102,   173,  1279,  1279,  -437,   227,
     227,   227,  -437,   373,   109,  -437,  -437,  -437,  -437,   121,
     111,   119,   120,   122,   124,   102,  -437,   650,   166,   165,
    -437,  -437,  -437,   121,   168,   171,   172,   174,   176,   186,
    -437,   486,  -437,  -437,   121,   190,   192,   193,   198,  -437,
     594,  -437,    54,   121,   199,   200,   203,  -437,   102,  1279,
    -437,  -437,  1279,  1320,  1320,  1320,  1359,   184,   184,  -437,
    -437,  -437,  -437,   202,   209,   210,   210,   213,   218,   217,
     220,   222,   225,   226,  -437,  -437,  -437,  -437,   -43,  -437,
    -437,  1279,   228,  -437,  -437,  -437,  -437,    20,   560,  -437,
    -437,  -437,  -437,  -437,  -437,  -437,  -437,  -437,  -437,   828,
    -437,  -437,  -437,  -437,  -437,  -437,  -437,  -437,  -437,  -437,
    -437,  -437,   277,    48,   796,   205,   215,    -2,   121,  -437,
    -437,  -437,  -437,  -437,  -437,  -437,    73,    62,   818,  -437,
    -437,  -437,  -437,  -437,  -437,  -437,    54,   701,  -437,  -437,
    -437,  -437,  -437,   251,   251,   233,    74,    30,  -437,  -437,
    -437,  -437,   121,   756,  -437,  -437,  -437,   373,  -437,   239,
     252,  -437,    20,   828,  1279,  -437,  -437,  -437,   300,  1279,
    -437,  -437,  1279,  1279,  -437,  -437,  -437,   123,   357,  -437,
    1279,  1279,  1140,   123,  1048,  -437,    16,    66,  1279,  1279,
    1279,  1320,  1320,  1320,  1320,  1320,  1320,  1320,  1320,  1279,
    1320,  1320,  1320,  1320,  1320,  1320,  1320,  1359,  1279,  1279,
    1279,  1279,  1279,  1279,  1279,  1279,  1279,  1279,  1279,   121,
     121,  -437,  -437,  -437,  -437,   246,  -437,   121,  -437,   306,
    1279,  -437,   650,   265,   265,  -437,   269,  -437,  -437,   246,
     486,   285,   246,   594,   121,  -437,  -437,  -437,  -437,  -437,
    -437,  -437,  -437,    30,  -437,  -437,  1383,  -437,    54,   246,
    -437,  -437,  1279,    20,   271,   274,    22,    31,  1407,  1407,
     102,   102,   102,   273,   102,  -437,  -437,   276,   278,    33,
     184,   279,   280,   282,  -437,    57,   284,   412,  -437,  -437,
     287,   355,  -437,   289,   289,   289,   289,   289,   289,   289,
     289,   677,    67,   394,   394,   341,   341,   341,   341,   341,
     300,    37,    67,    67,    67,    67,    67,    67,    67,    67,
     346,  -437,  1279,  1279,  1094,  -437,   291,   796,  1186,  -437,
    -437,  -437,  -437,    96,    20,  -437,  -437,  1320,  -437,  -437,
    -437,   291,   818,  -437,   107,   296,   291,   701,  -437,    54,
     121,    54,  -437,  -437,  -437,   353,  -437,   316,   316,   246,
     291,   756,  -437,  -437,  1279,  -437,  -437,   102,  -437,   301,
     308,   302,  -437,  -437,  -437,  -437,  -437,  -437,  -437,  1279,
    -437,  -437,  -437,  -437,  1279,  -437,  -437,  -437,  1320,  -437,
     246,  -437,   246,  -437,  1233,  -437,   310,  -437,  -437,  -437,
     246,   314,   327,  -437,  -437,  -437,  -437,  1279,  -437,   372,
     289,  -437,   246,  -437,  -437,  -437,  -437,   246,   246,    54,
    -437,  -437,    74,  1383,  -437,  -437,  -437,   291,    54,  -437,
     246,  -437,  -437,  -437,  -437,  -437,  -437,   329,  -437,   300,
     291,  1279,   291,  1279,   333,   338,   343,  -437,  -437,  -437,
    -437,  -437,  -437,  1320,  -437,  -437,   291,   246,  -437,  -437,
     246,  -437,  1407,  -437,  -437,   246,  -437,   246,  1279,  1233,
    -437,   354,  -437,   291,  -437,  -437,  -437,  -437,    44,  -437,
    -437,  -437,  -437
  };

  /* YYDEFACT[S] -- default rule to reduce with in state S when YYTABLE
     doesn't specify something else to do.  Zero means the default is an
     error.  */
  const unsigned short int
  CMPL_Parser::yydefact_[] =
  {
         2,     8,    34,    59,    77,     0,     2,     4,     5,     6,
       7,     0,     0,     0,     0,     1,     3,     0,     0,     0,
       0,     0,    54,   158,   158,     0,     0,     0,   260,   293,
     293,   293,     9,     0,    23,    16,    20,    21,    22,   277,
       0,     0,     0,     0,     0,   158,    35,     0,     0,    45,
      47,    41,    48,   277,     0,     0,     0,     0,   259,   260,
      60,     0,    67,    65,   277,     0,     0,     0,     0,    78,
       0,    84,    96,   277,     0,     0,     0,    12,   158,   162,
     222,   223,     0,     0,     0,     0,   239,   217,   217,   126,
     127,   129,   128,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    25,    26,    27,    28,     0,   237,
     238,     0,     0,   170,   122,   123,   124,   175,   166,   295,
     173,   249,   171,   248,   167,   168,   169,   172,   246,   176,
     177,   179,   296,   289,   291,   290,   278,   292,   286,   287,
     288,    11,     0,     0,     0,     0,   267,   273,   277,   256,
      17,    18,    19,    38,   145,    37,     0,     0,     0,    42,
      43,    44,    63,   254,   255,    62,    96,     0,    68,    69,
      70,    81,    80,    97,    98,     0,     0,    99,   110,   110,
      95,   104,   277,     0,    85,    86,    87,     0,    55,     0,
     160,   163,   294,     0,     0,   189,   188,   174,   240,     0,
     224,   225,     0,     0,   143,   133,   131,     0,     0,   257,
       0,     0,     0,     0,     0,   258,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   241,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   277,
     277,    24,    29,    30,    14,    33,   266,   277,   275,     0,
       0,   276,     0,    52,    52,    51,     0,    54,    46,    58,
       0,    71,    76,     0,   277,   106,    83,   250,   252,   251,
      88,   110,    92,    99,   102,    93,     0,    94,    96,   121,
      13,   159,   162,   221,     0,   219,     0,     0,     0,     0,
     158,   158,   158,     0,   158,   157,   155,     0,     0,     0,
     217,     0,     0,     0,   235,   233,     0,     0,   125,   247,
       0,   193,   192,   194,   197,   195,   201,   199,   198,   200,
     196,     0,   210,   181,   182,   183,   184,   187,   186,   185,
     242,     0,   202,   205,   203,   207,   209,   206,   208,   204,
     190,   191,     0,     0,     0,   265,     0,     0,     0,   268,
     270,   284,   275,     0,   285,   275,    39,   164,    49,    50,
      40,     0,     0,    64,     0,     0,     0,     0,    82,    96,
     277,    96,   101,   100,   111,   112,   115,   117,   117,   109,
       0,     0,   161,   218,     0,   141,   142,   158,   150,     0,
     146,     0,   152,   153,   154,   135,   156,   136,   140,     0,
     144,   139,   138,   245,     0,   236,   244,   130,     0,   211,
     213,   214,   216,   227,     0,   229,     0,   226,   262,    31,
      33,     0,     0,   272,   281,   280,   282,     0,   274,     0,
     165,    56,    58,    73,    72,    66,    74,    76,   109,    96,
      91,    89,    90,     0,   118,   114,   116,     0,    96,   119,
     121,   220,   151,   134,   149,   147,   132,     0,   234,   243,
       0,     0,     0,     0,     0,   231,     0,    15,    32,   264,
     263,   283,   279,   164,    57,    75,     0,   109,   113,   103,
     109,   120,     0,   137,   178,   213,   180,   213,     0,     0,
     230,     0,   105,     0,   108,   148,   212,   215,     0,   232,
      53,   107,   228
  };

  /* YYPGOTO[NTERM-NUM].  */
  const short int
  CMPL_Parser::yypgoto_[] =
  {
      -437,   424,  -437,  -437,  -437,     2,  -437,  -437,  -437,  -437,
    -437,   294,  -437,  -437,     7,  -437,  -437,   -37,  -437,  -437,
     298,  -437,  -437,   177,     0,  -437,  -437,     4,  -437,  -437,
     -15,  -437,  -437,  -437,  -437,  -437,     6,  -437,  -437,   -40,
    -437,  -437,    61,  -437,  -437,  -153,  -437,  -437,  -171,   161,
    -437,  -437,  -166,  -437,  -436,  -165,  -437,     8,  -437,    68,
    -437,    -1,  -279,  -437,  -437,  -437,  -437,  -437,   361,  -437,
    -282,  -258,  -437,  -437,  -437,  -190,  -437,   -21,   178,  -437,
     -23,   -66,   -25,   -26,  -437,  -437,  -437,  -437,  -437,  -437,
    -437,  -437,  -437,    -8,  -389,    -7,  -437,   -83,    70,  -437,
    -277,  -437,  -437,  -437,  -437,  -437,   117,   -34,    58,   -80,
    -437,    -5,    71,   -64,  -437,  -437,  -104,    47,  -437,    -9,
     216,  -437,  -437,  -437,  -437,  -296,   328,   152,  -437,  -437,
    -437,  -437,   125,    77,   139,   212,   229
  };

  /* YYDEFGOTO[NTERM-NUM].  */
  const short int
  CMPL_Parser::yydefgoto_[] =
  {
        -1,     5,     6,     7,    11,    32,   187,    33,   354,    34,
     143,   112,   254,    35,   356,     8,    12,    46,   262,    47,
      48,    49,   266,   368,    50,    78,    51,   371,     9,    13,
      60,   270,    61,   166,   375,    62,   376,    10,    14,    69,
     273,    70,   175,   381,   451,   176,   177,   178,   179,   282,
     283,   180,   181,   380,   457,   285,   286,   384,   385,   455,
      71,   390,   113,   114,   115,   116,   299,   298,   205,   311,
      52,   399,   492,   465,   400,   303,   307,    80,   189,   190,
     439,   117,   118,   425,   120,   249,   121,   250,   122,   123,
     124,   125,   126,   420,   470,   422,   472,   200,   294,   295,
      37,    38,   127,   128,   426,   474,   475,   476,   316,   129,
     280,    63,    39,   130,   131,    40,   429,   458,   358,   144,
     145,   146,   258,   259,   260,   360,   147,   148,   362,   437,
     482,   363,   365,    41,   138,    42,    43
  };

  /* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule which
     number is the opposite.  If zero, do what YYDEFACT says.  */
  const short int CMPL_Parser::yytable_ninf_ = -272;
  const short int
  CMPL_Parser::yytable_[] =
  {
       119,   132,   193,    81,   387,   201,   281,   386,   182,   388,
     155,   284,   486,   271,   287,    72,   192,    36,   306,   398,
     398,   219,   220,   313,   154,   219,   220,   219,   220,  -271,
     172,   217,   214,    36,   215,   141,   219,   220,   219,   220,
      15,   401,   219,   220,   158,   216,   165,   173,   174,   219,
     220,   503,   252,   191,   504,   167,   253,   188,   195,   196,
     197,   198,   219,   220,   183,    72,   433,    17,  -269,   438,
    -269,   173,   174,    18,   239,   240,   241,   242,   243,   244,
     245,   246,   277,   247,   247,   248,   248,   278,   279,    53,
      64,    73,   318,   267,    45,    54,    65,    74,   395,   434,
     435,    99,   182,    77,   436,   409,   506,   396,   507,   274,
     275,    19,   281,   418,   193,    20,   382,   284,    53,   193,
     512,   269,   193,   193,    54,   263,   264,   265,   216,   414,
    -259,   193,    64,   293,   317,   389,   296,   297,    65,   193,
     193,    73,   319,   289,    36,   309,   255,    74,   315,   332,
     443,   444,   272,   321,   322,   300,   301,   302,   193,   342,
     343,   344,   345,   346,   347,   348,   349,   350,   351,   139,
     140,   387,   341,   288,   386,    79,   388,    82,    72,   142,
     193,   137,   137,   137,   308,   136,   312,    36,   149,   290,
     150,   151,   320,   152,   364,   153,   323,   324,   325,   326,
     327,   328,   329,   330,   331,   333,   334,   335,   336,   337,
     338,   339,   340,   398,   331,   331,   331,   331,   331,   331,
     331,   331,   331,   331,   182,   366,   448,   410,   452,    53,
      55,    66,    75,   378,   505,    54,   156,   157,    64,   159,
     352,   353,   160,   161,    65,   162,   163,    56,    67,    76,
       1,     2,     3,     4,    73,   373,   164,   199,   133,    55,
      74,   168,   134,   169,   170,   379,   191,   441,    72,   171,
     184,   185,   446,    66,   186,   256,    56,   202,   193,   402,
     403,   404,    75,   406,   203,   204,   459,   257,   207,   135,
      67,   136,   364,   208,   209,   210,   487,   211,    99,    76,
     212,   213,   357,   218,   276,   490,   230,   231,   232,   233,
     234,   235,   236,   291,   193,   182,   372,   182,   231,   377,
     233,   234,   235,   236,   292,   355,   419,   421,   293,   104,
     105,   106,   107,    53,   193,   442,   391,   361,   367,    54,
     370,    64,   440,   374,    73,   393,   394,    65,   315,   405,
      74,   460,   407,   489,   408,   411,   412,    36,   413,   430,
     415,   220,   447,   417,   236,   248,   494,   445,   496,   428,
      55,   449,   453,   -10,    21,   454,   462,   463,   466,    66,
     464,   477,   502,   467,   479,   182,    72,    56,   304,   300,
     301,   302,   305,   469,   182,    75,    67,   480,   483,   511,
     -10,   -10,   -10,   -10,    22,   493,    23,    24,   498,    25,
     499,   481,    76,   232,   233,   234,   235,   236,   193,   500,
     239,   240,   241,   242,   243,   244,   245,   246,   510,   247,
      16,   248,   508,    26,    27,    28,   251,   478,    29,    30,
      31,   369,   450,    53,   383,   419,   484,   421,    64,    54,
    -261,   -10,   -10,   485,    65,   268,   456,   206,   440,   491,
     501,   488,    73,   495,   461,   509,   497,   471,    74,   473,
     392,   427,   468,   359,    55,     0,   261,   357,     0,     0,
       0,     0,    66,   432,     0,    75,   -61,    57,   416,   372,
       0,    56,     0,     0,   377,     0,     0,     0,     0,    67,
       0,     0,    76,  -253,  -253,     0,     0,   391,     0,     0,
       0,     0,     0,   -61,   -61,   -61,   -61,    58,  -253,  -253,
       0,  -253,    25,  -253,  -253,  -253,  -253,     0,  -253,  -253,
    -253,  -253,  -253,  -253,  -253,  -253,  -253,     0,  -253,  -253,
    -253,  -253,   471,     0,   471,     0,    26,    27,    59,     0,
       0,    29,    30,    31,     0,     0,     0,     0,     0,     0,
       0,  -253,     0,  -261,   -61,   -61,     0,     0,   221,   222,
     223,   224,   225,   226,   227,   228,   229,   230,   231,   232,
     233,   234,   235,   236,    55,     0,   237,     0,     0,    66,
       0,     0,     0,     0,   -79,    68,     0,     0,     0,     0,
       0,    56,     0,    75,     0,     0,    67,     0,     0,     0,
       0,  -253,  -253,     0,     0,     0,     0,     0,     0,     0,
      76,   -79,   -79,   -79,   -79,    58,  -253,  -253,     0,  -253,
      25,  -253,  -253,  -253,  -253,   238,  -253,  -253,  -253,  -253,
    -253,  -253,  -253,  -253,  -253,     0,  -253,  -253,  -253,  -253,
     -36,    44,     0,     0,    26,    27,    59,     0,     0,    29,
      30,    31,     0,     0,     0,     0,     0,     0,     0,  -253,
       0,  -261,   -79,   -79,     0,     0,     0,   -36,   -36,   -36,
     -36,    22,    45,     0,     0,     0,    25,     0,     0,     0,
       0,     0,     0,     0,   230,   231,   232,   233,   234,   235,
     236,     0,    57,   237,     0,     0,     0,     0,     0,     0,
      26,    27,    28,     0,     0,    29,    30,    31,  -253,  -253,
       0,     0,     0,     0,     0,     0,     0,  -261,   -36,   -36,
       0,     0,    58,  -253,  -253,     0,  -253,    25,  -253,  -253,
    -253,  -253,     0,  -253,  -253,  -253,  -253,  -253,  -253,  -253,
    -253,  -253,   238,  -253,  -253,  -253,  -253,    68,     0,     0,
       0,    26,    27,    59,     0,     0,    29,    30,    31,     0,
       0,     0,     0,  -253,  -253,     0,  -253,     0,  -261,   -61,
     -61,     0,     0,     0,     0,     0,     0,    58,  -253,  -253,
       0,  -253,    25,  -253,  -253,  -253,  -253,    21,  -253,  -253,
    -253,  -253,  -253,  -253,  -253,  -253,  -253,     0,  -253,  -253,
    -253,  -253,     0,     0,     0,     0,    26,    27,    59,    44,
       0,    29,    30,    31,     0,     0,     0,    22,     0,    23,
      24,  -253,    25,  -261,   -79,   -79,   239,   240,   241,   242,
     243,   244,   245,   246,     0,   247,     0,   248,     0,    22,
      45,     0,     0,     0,    25,     0,    26,    27,    28,     0,
       0,    29,    30,    31,     0,     0,     0,     0,     0,     0,
       0,   -61,    57,  -261,   -10,   -10,     0,     0,    26,    27,
      28,     0,     0,    29,    30,    31,     0,     0,  -253,  -253,
       0,     0,     0,     0,     0,  -261,   -36,   -36,   -61,   -61,
     -61,   -61,    58,  -253,  -253,     0,  -253,    25,  -253,  -253,
    -253,  -253,     0,  -253,  -253,  -253,  -253,  -253,  -253,  -253,
    -253,  -253,     0,  -253,  -253,  -253,  -253,   -79,    68,     0,
       0,    26,    27,    59,     0,     0,    29,    30,    31,     0,
       0,     0,     0,     0,  -253,  -253,  -253,     0,  -261,     0,
       0,     0,     0,     0,   -79,   -79,   -79,   -79,    58,  -253,
    -253,     0,  -253,    25,  -253,  -253,  -253,  -253,     0,  -253,
    -253,  -253,  -253,  -253,  -253,  -253,  -253,  -253,     0,  -253,
    -253,  -253,  -253,   -10,    21,     0,     0,    26,    27,    59,
       0,     0,    29,    30,    31,     0,   -36,    44,     0,     0,
       0,     0,  -253,     0,  -261,     0,     0,     0,     0,     0,
     -10,   -10,   -10,   -10,    22,     0,    23,    24,     0,    25,
       0,     0,     0,   -36,   -36,   -36,   -36,    22,    45,     0,
       0,     0,    25,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    26,    27,    28,     0,     0,    29,    30,
      31,     0,     0,     0,     0,     0,    26,    27,    28,     0,
    -261,    29,    30,    31,     0,    83,    84,     0,     0,     0,
       0,     0,    85,  -261,    86,     0,     0,     0,     0,     0,
       0,    87,    88,    89,     0,    90,    91,    92,    93,     0,
      94,    95,    96,    97,    98,    99,   100,   101,   102,   103,
     104,   105,   106,   107,   108,     0,     0,     0,     0,     0,
       0,    83,    84,     0,     0,     0,   109,   110,    85,     0,
      86,     0,     0,   111,   314,     0,     0,    87,    88,    89,
       0,    90,    91,    92,    93,   423,    94,    95,    96,    97,
      98,    99,   100,   101,   102,   103,   104,   105,   106,   107,
     108,   424,     0,     0,     0,     0,     0,    83,    84,     0,
       0,     0,   109,   110,    85,     0,    86,     0,     0,   111,
       0,     0,   310,    87,    88,    89,     0,    90,    91,    92,
      93,     0,    94,    95,    96,    97,    98,    99,   100,   101,
     102,   103,   104,   105,   106,   107,   108,     0,     0,     0,
       0,     0,     0,    83,    84,     0,     0,     0,   109,   110,
      85,     0,    86,     0,     0,   111,     0,     0,     0,    87,
      88,    89,     0,    90,    91,    92,    93,     0,    94,    95,
      96,    97,    98,    99,   100,   101,   102,   103,   104,   105,
     106,   107,   108,     0,     0,     0,     0,     0,     0,   431,
      83,    84,     0,     0,   109,   110,     0,    85,     0,    86,
       0,   111,     0,     0,     0,     0,    87,    88,    89,     0,
      90,    91,    92,    93,     0,    94,    95,    96,    97,    98,
      99,   100,   101,   102,   103,   104,   105,   106,   107,   108,
     424,     0,     0,     0,     0,     0,    83,    84,     0,     0,
       0,   109,   110,    85,     0,    86,     0,     0,   111,     0,
       0,     0,    87,    88,    89,     0,    90,    91,    92,    93,
       0,    94,    95,    96,    97,    98,    99,   100,   101,   102,
     103,   104,   105,   106,   107,   108,     0,    83,    84,     0,
       0,     0,     0,     0,    85,     0,     0,   109,   110,     0,
       0,     0,     0,    87,   111,    89,     0,    90,    91,    92,
      93,     0,    94,    95,    96,    97,    98,    99,   100,   101,
     102,     0,   104,   105,   106,   107,     0,    84,     0,     0,
       0,     0,     0,    85,     0,     0,     0,     0,     0,     0,
       0,     0,    87,     0,    89,   194,    90,    91,    92,    93,
       0,    94,    95,    96,    97,    98,    99,   100,   101,   102,
       0,   104,   105,   106,   107,    45,    23,     0,    89,     0,
      90,    91,    92,    93,     0,    94,    95,    96,    97,    98,
       0,   100,   101,   102,   194,   104,   105,   106,   107,     0,
     397,     0,    89,     0,    90,    91,    92,    93,     0,    94,
      95,    96,    97,    98,     0,   100,   101,   102,   194,   104,
     105,   106,   107,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   194
  };

  /* YYCHECK.  */
  const short int
  CMPL_Parser::yycheck_[] =
  {
        26,    27,    82,    24,   286,    88,   177,   286,    72,   286,
      47,   177,   448,   166,   179,    20,    82,    17,   208,   298,
     299,     5,     6,   213,    45,     5,     6,     5,     6,    31,
      70,   111,    75,    33,    77,    33,     5,     6,     5,     6,
       0,   299,     5,     6,    53,   111,    61,    17,    18,     5,
       6,   487,     4,    79,   490,    64,     8,    78,    83,    84,
      85,    86,     5,     6,    73,    70,   362,    70,    70,   365,
      72,    17,    18,    70,     8,     9,    10,    11,    12,    13,
      14,    15,     8,    17,    17,    19,    19,    13,    14,    18,
      19,    20,    76,    31,    32,    18,    19,    20,    76,     3,
       4,    47,   166,    71,     8,    72,   495,    76,   497,   173,
     174,    70,   283,    76,   194,    70,   281,   283,    47,   199,
      76,   158,   202,   203,    47,    52,    53,    54,   194,    72,
      77,   211,    61,   199,   214,   288,   202,   203,    61,   219,
     220,    70,    76,   183,   144,   211,   144,    70,   214,   229,
      43,    44,   167,   219,   220,    32,    33,    34,   238,   239,
     240,   241,   242,   243,   244,   245,   246,   247,   248,    30,
      31,   453,   238,   182,   453,    73,   453,     4,   183,    70,
     260,    29,    30,    31,   210,    64,   212,   187,    77,   187,
      71,    71,   218,    71,   260,    71,   221,   222,   223,   224,
     225,   226,   227,   228,   229,   230,   231,   232,   233,   234,
     235,   236,   237,   492,   239,   240,   241,   242,   243,   244,
     245,   246,   247,   248,   288,   262,   379,   310,   381,   158,
      18,    19,    20,   273,   492,   158,    70,    72,   167,    71,
     249,   250,    71,    71,   167,    71,    70,    18,    19,    20,
      27,    28,    29,    30,   183,   270,    70,    73,    31,    47,
     183,    71,    35,    71,    71,   274,   292,   371,   273,    71,
      71,    71,   376,    61,    71,    70,    47,    75,   358,   300,
     301,   302,    70,   304,    75,    75,   390,    72,    75,    62,
      61,    64,   358,    75,    77,    75,   449,    75,    47,    70,
      75,    75,   255,    75,    71,   458,    17,    18,    19,    20,
      21,    22,    23,    74,   394,   379,   269,   381,    18,   272,
      20,    21,    22,    23,    72,    79,   352,   353,   394,    52,
      53,    54,    55,   262,   414,   372,   289,    31,    73,   262,
      71,   270,   367,    58,   273,    74,    72,   270,   414,    76,
     273,   391,    76,   457,    76,    76,    76,   357,    76,   357,
      76,     6,   377,    76,    23,    19,   470,    71,   472,    78,
     158,   380,    19,     0,     1,    59,   397,    76,    76,   167,
      72,    71,   486,   409,    70,   449,   391,   158,    31,    32,
      33,    34,    35,   418,   458,   183,   167,    70,    26,   503,
      27,    28,    29,    30,    31,    76,    33,    34,    75,    36,
      72,   437,   183,    19,    20,    21,    22,    23,   498,    76,
       8,     9,    10,    11,    12,    13,    14,    15,    74,    17,
       6,    19,   498,    60,    61,    62,   142,   430,    65,    66,
      67,   264,   381,   372,   283,   471,   442,   473,   377,   372,
      77,    78,    79,   447,   377,   157,   388,    96,   483,   460,
     483,   453,   391,   471,   394,   499,   473,   420,   391,   422,
     292,   354,   414,   257,   262,    -1,   148,   430,    -1,    -1,
      -1,    -1,   270,   358,    -1,   273,     0,     1,    76,   442,
      -1,   262,    -1,    -1,   447,    -1,    -1,    -1,    -1,   270,
      -1,    -1,   273,    17,    18,    -1,    -1,   460,    -1,    -1,
      -1,    -1,    -1,    27,    28,    29,    30,    31,    32,    33,
      -1,    35,    36,    37,    38,    39,    40,    -1,    42,    43,
      44,    45,    46,    47,    48,    49,    50,    -1,    52,    53,
      54,    55,   495,    -1,   497,    -1,    60,    61,    62,    -1,
      -1,    65,    66,    67,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    75,    -1,    77,    78,    79,    -1,    -1,     8,     9,
      10,    11,    12,    13,    14,    15,    16,    17,    18,    19,
      20,    21,    22,    23,   372,    -1,    26,    -1,    -1,   377,
      -1,    -1,    -1,    -1,     0,     1,    -1,    -1,    -1,    -1,
      -1,   372,    -1,   391,    -1,    -1,   377,    -1,    -1,    -1,
      -1,    17,    18,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     391,    27,    28,    29,    30,    31,    32,    33,    -1,    35,
      36,    37,    38,    39,    40,    75,    42,    43,    44,    45,
      46,    47,    48,    49,    50,    -1,    52,    53,    54,    55,
       0,     1,    -1,    -1,    60,    61,    62,    -1,    -1,    65,
      66,    67,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    75,
      -1,    77,    78,    79,    -1,    -1,    -1,    27,    28,    29,
      30,    31,    32,    -1,    -1,    -1,    36,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    17,    18,    19,    20,    21,    22,
      23,    -1,     1,    26,    -1,    -1,    -1,    -1,    -1,    -1,
      60,    61,    62,    -1,    -1,    65,    66,    67,    17,    18,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    77,    78,    79,
      -1,    -1,    31,    32,    33,    -1,    35,    36,    37,    38,
      39,    40,    -1,    42,    43,    44,    45,    46,    47,    48,
      49,    50,    75,    52,    53,    54,    55,     1,    -1,    -1,
      -1,    60,    61,    62,    -1,    -1,    65,    66,    67,    -1,
      -1,    -1,    -1,    17,    18,    -1,    75,    -1,    77,    78,
      79,    -1,    -1,    -1,    -1,    -1,    -1,    31,    32,    33,
      -1,    35,    36,    37,    38,    39,    40,     1,    42,    43,
      44,    45,    46,    47,    48,    49,    50,    -1,    52,    53,
      54,    55,    -1,    -1,    -1,    -1,    60,    61,    62,     1,
      -1,    65,    66,    67,    -1,    -1,    -1,    31,    -1,    33,
      34,    75,    36,    77,    78,    79,     8,     9,    10,    11,
      12,    13,    14,    15,    -1,    17,    -1,    19,    -1,    31,
      32,    -1,    -1,    -1,    36,    -1,    60,    61,    62,    -1,
      -1,    65,    66,    67,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,     0,     1,    77,    78,    79,    -1,    -1,    60,    61,
      62,    -1,    -1,    65,    66,    67,    -1,    -1,    17,    18,
      -1,    -1,    -1,    -1,    -1,    77,    78,    79,    27,    28,
      29,    30,    31,    32,    33,    -1,    35,    36,    37,    38,
      39,    40,    -1,    42,    43,    44,    45,    46,    47,    48,
      49,    50,    -1,    52,    53,    54,    55,     0,     1,    -1,
      -1,    60,    61,    62,    -1,    -1,    65,    66,    67,    -1,
      -1,    -1,    -1,    -1,    17,    18,    75,    -1,    77,    -1,
      -1,    -1,    -1,    -1,    27,    28,    29,    30,    31,    32,
      33,    -1,    35,    36,    37,    38,    39,    40,    -1,    42,
      43,    44,    45,    46,    47,    48,    49,    50,    -1,    52,
      53,    54,    55,     0,     1,    -1,    -1,    60,    61,    62,
      -1,    -1,    65,    66,    67,    -1,     0,     1,    -1,    -1,
      -1,    -1,    75,    -1,    77,    -1,    -1,    -1,    -1,    -1,
      27,    28,    29,    30,    31,    -1,    33,    34,    -1,    36,
      -1,    -1,    -1,    27,    28,    29,    30,    31,    32,    -1,
      -1,    -1,    36,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    60,    61,    62,    -1,    -1,    65,    66,
      67,    -1,    -1,    -1,    -1,    -1,    60,    61,    62,    -1,
      77,    65,    66,    67,    -1,    17,    18,    -1,    -1,    -1,
      -1,    -1,    24,    77,    26,    -1,    -1,    -1,    -1,    -1,
      -1,    33,    34,    35,    -1,    37,    38,    39,    40,    -1,
      42,    43,    44,    45,    46,    47,    48,    49,    50,    51,
      52,    53,    54,    55,    56,    -1,    -1,    -1,    -1,    -1,
      -1,    17,    18,    -1,    -1,    -1,    68,    69,    24,    -1,
      26,    -1,    -1,    75,    76,    -1,    -1,    33,    34,    35,
      -1,    37,    38,    39,    40,    41,    42,    43,    44,    45,
      46,    47,    48,    49,    50,    51,    52,    53,    54,    55,
      56,    57,    -1,    -1,    -1,    -1,    -1,    17,    18,    -1,
      -1,    -1,    68,    69,    24,    -1,    26,    -1,    -1,    75,
      -1,    -1,    32,    33,    34,    35,    -1,    37,    38,    39,
      40,    -1,    42,    43,    44,    45,    46,    47,    48,    49,
      50,    51,    52,    53,    54,    55,    56,    -1,    -1,    -1,
      -1,    -1,    -1,    17,    18,    -1,    -1,    -1,    68,    69,
      24,    -1,    26,    -1,    -1,    75,    -1,    -1,    -1,    33,
      34,    35,    -1,    37,    38,    39,    40,    -1,    42,    43,
      44,    45,    46,    47,    48,    49,    50,    51,    52,    53,
      54,    55,    56,    -1,    -1,    -1,    -1,    -1,    -1,    63,
      17,    18,    -1,    -1,    68,    69,    -1,    24,    -1,    26,
      -1,    75,    -1,    -1,    -1,    -1,    33,    34,    35,    -1,
      37,    38,    39,    40,    -1,    42,    43,    44,    45,    46,
      47,    48,    49,    50,    51,    52,    53,    54,    55,    56,
      57,    -1,    -1,    -1,    -1,    -1,    17,    18,    -1,    -1,
      -1,    68,    69,    24,    -1,    26,    -1,    -1,    75,    -1,
      -1,    -1,    33,    34,    35,    -1,    37,    38,    39,    40,
      -1,    42,    43,    44,    45,    46,    47,    48,    49,    50,
      51,    52,    53,    54,    55,    56,    -1,    17,    18,    -1,
      -1,    -1,    -1,    -1,    24,    -1,    -1,    68,    69,    -1,
      -1,    -1,    -1,    33,    75,    35,    -1,    37,    38,    39,
      40,    -1,    42,    43,    44,    45,    46,    47,    48,    49,
      50,    -1,    52,    53,    54,    55,    -1,    18,    -1,    -1,
      -1,    -1,    -1,    24,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    33,    -1,    35,    75,    37,    38,    39,    40,
      -1,    42,    43,    44,    45,    46,    47,    48,    49,    50,
      -1,    52,    53,    54,    55,    32,    33,    -1,    35,    -1,
      37,    38,    39,    40,    -1,    42,    43,    44,    45,    46,
      -1,    48,    49,    50,    75,    52,    53,    54,    55,    -1,
      33,    -1,    35,    -1,    37,    38,    39,    40,    -1,    42,
      43,    44,    45,    46,    -1,    48,    49,    50,    75,    52,
      53,    54,    55,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    75
  };

  /* STOS_[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
  const unsigned char
  CMPL_Parser::yystos_[] =
  {
         0,    27,    28,    29,    30,    81,    82,    83,    95,   108,
     117,    84,    96,   109,   118,     0,    81,    70,    70,    70,
      70,     1,    31,    33,    34,    36,    60,    61,    62,    65,
      66,    67,    85,    87,    89,    93,   104,   180,   181,   192,
     195,   213,   215,   216,     1,    32,    97,    99,   100,   101,
     104,   106,   150,   192,   213,   215,   216,     1,    31,    62,
     110,   112,   115,   191,   192,   213,   215,   216,     1,   119,
     121,   140,   191,   192,   213,   215,   216,    71,   105,    73,
     157,   157,     4,    17,    18,    24,    26,    33,    34,    35,
      37,    38,    39,    40,    42,    43,    44,    45,    46,    47,
      48,    49,    50,    51,    52,    53,    54,    55,    56,    68,
      69,    75,    91,   142,   143,   144,   145,   161,   162,   163,
     164,   166,   168,   169,   170,   171,   172,   182,   183,   189,
     193,   194,   163,    31,    35,    62,    64,   207,   214,   214,
     214,    85,    70,    90,   199,   200,   201,   206,   207,    77,
      71,    71,    71,    71,   157,    97,    70,    72,   199,    71,
      71,    71,    71,    70,    70,   110,   113,   199,    71,    71,
      71,    71,   119,    17,    18,   122,   125,   126,   127,   128,
     131,   132,   193,   199,    71,    71,    71,    86,   157,   158,
     159,   163,   161,   189,    75,   162,   162,   162,   162,    73,
     177,   177,    75,    75,    75,   148,   148,    75,    75,    77,
      75,    75,    75,    75,    75,    77,   161,   189,    75,     5,
       6,     8,     9,    10,    11,    12,    13,    14,    15,    16,
      17,    18,    19,    20,    21,    22,    23,    26,    75,     8,
       9,    10,    11,    12,    13,    14,    15,    17,    19,   165,
     167,    91,     4,     8,    92,    85,    70,    72,   202,   203,
     204,   206,    98,    52,    53,    54,   102,    31,   100,    97,
     111,   125,   110,   120,   193,   193,    71,     8,    13,    14,
     190,   128,   129,   130,   132,   135,   136,   135,   199,   119,
      85,    74,    72,   161,   178,   179,   161,   161,   147,   146,
      32,    33,    34,   155,    31,    35,   155,   156,   163,   161,
      32,   149,   163,   155,    76,   161,   188,   189,    76,    76,
     163,   161,   161,   162,   162,   162,   162,   162,   162,   162,
     162,   162,   189,   162,   162,   162,   162,   162,   162,   162,
     162,   161,   189,   189,   189,   189,   189,   189,   189,   189,
     189,   189,   199,   199,    88,    79,    94,   197,   198,   200,
     205,    31,   208,   211,   161,   212,    97,    73,   103,   103,
      71,   107,   197,   110,    58,   114,   116,   197,   119,   199,
     133,   123,   135,   129,   137,   138,   142,   150,   180,   125,
     141,   197,   158,    74,    72,    76,    76,    33,   142,   151,
     154,   151,   157,   157,   157,    76,   157,    76,    76,    72,
     177,    76,    76,    76,    72,    76,    76,    76,    76,   163,
     173,   163,   175,    41,    57,   163,   184,   186,    78,   196,
      85,    63,   212,   205,     3,     4,     8,   209,   205,   160,
     162,   196,    97,    43,    44,    71,   196,   110,   125,   199,
     122,   124,   125,    19,    59,   139,   139,   134,   197,   196,
     119,   178,   157,    76,    72,   153,    76,   163,   188,   162,
     174,   197,   176,   197,   185,   186,   187,    71,    94,    70,
      70,   163,   210,    26,   107,   116,   134,   125,   137,   196,
     125,   141,   152,    76,   196,   173,   196,   175,    75,    72,
      76,   160,   196,   134,   134,   151,   174,   174,   161,   187,
      74,   196,    76
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
      58,    59,    44,    91,    93,    40,    41,   123,   125,   124
  };
#endif

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
  const unsigned char
  CMPL_Parser::yyr1_[] =
  {
         0,    80,    81,    81,    82,    82,    82,    82,    84,    83,
      85,    85,    86,    85,    88,    87,    87,    87,    87,    87,
      89,    89,    89,    90,    90,    91,    91,    91,    91,    92,
      92,    93,    94,    94,    96,    95,    97,    97,    98,    97,
      99,    99,    99,    99,    99,   100,   100,   101,   101,   102,
     102,   102,   103,   103,   105,   104,   106,   107,   107,   109,
     108,   110,   110,   111,   110,   113,   112,   112,   112,   112,
     112,   114,   114,   114,   115,   116,   116,   118,   117,   119,
     119,   120,   119,   121,   121,   121,   121,   121,   123,   122,
     124,   124,   125,   126,   126,   126,   127,   128,   128,   129,
     129,   130,   130,   131,   131,   132,   133,   132,   134,   134,
     136,   135,   137,   137,   138,   138,   138,   139,   139,   140,
     141,   141,   142,   142,   142,   142,   142,   143,   143,   143,
     144,   146,   145,   147,   145,   145,   145,   145,   145,   145,
     145,   145,   145,   148,   149,   150,   151,   152,   151,   153,
     154,   154,   155,   155,   155,   156,   156,   156,   157,   157,
     158,   158,   159,   159,   160,   160,   161,   161,   161,   161,
     162,   162,   162,   162,   162,   163,   163,   165,   164,   167,
     166,   168,   168,   168,   168,   168,   168,   168,   168,   168,
     169,   169,   170,   170,   171,   171,   171,   171,   171,   171,
     171,   171,   172,   172,   172,   172,   172,   172,   172,   172,
     172,   173,   174,   174,   175,   176,   176,   177,   177,   178,
     178,   179,   180,   181,   182,   183,   184,   185,   184,   186,
     186,   187,   187,   188,   188,   189,   189,   189,   189,   189,
     189,   189,   189,   189,   189,   189,   189,   189,   189,   189,
     190,   190,   190,   191,   191,   191,   192,   193,   194,   195,
     195,   195,   196,   197,   197,   198,   199,   200,   200,   202,
     201,   203,   201,   204,   201,   205,   206,   206,   207,   208,
     209,   209,   209,   210,   211,   212,   213,   213,   213,   214,
     214,   214,   214,   214,   215,   216,   216
  };

  /* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
  const unsigned char
  CMPL_Parser::yyr2_[] =
  {
         0,     2,     0,     2,     1,     1,     1,     1,     0,     4,
       0,     2,     0,     4,     0,     6,     1,     2,     2,     2,
       1,     1,     1,     0,     2,     1,     1,     1,     1,     1,
       1,     5,     3,     0,     0,     4,     0,     2,     0,     4,
       4,     1,     2,     2,     2,     1,     3,     1,     1,     2,
       2,     1,     0,     5,     0,     3,     5,     3,     0,     0,
       4,     0,     2,     0,     4,     0,     5,     1,     2,     2,
       2,     0,     2,     2,     5,     3,     0,     0,     4,     0,
       2,     0,     4,     3,     1,     2,     2,     2,     0,     4,
       1,     1,     2,     2,     2,     1,     0,     1,     1,     0,
       2,     2,     1,     5,     1,     6,     0,     7,     3,     0,
       0,     2,     1,     3,     2,     1,     2,     0,     1,     5,
       3,     0,     1,     1,     1,     3,     1,     1,     1,     1,
       4,     0,     5,     0,     5,     4,     4,     6,     4,     4,
       4,     4,     4,     1,     2,     2,     1,     0,     4,     1,
       1,     2,     2,     2,     2,     1,     2,     1,     0,     3,
       1,     3,     0,     1,     0,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     2,     1,     1,     0,     6,     0,
       6,     3,     3,     3,     3,     3,     3,     3,     2,     2,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     1,     3,     0,     1,     3,     0,     0,     3,     1,
       3,     1,     2,     2,     2,     2,     1,     0,     5,     1,
       3,     1,     3,     1,     3,     3,     4,     1,     1,     1,
       2,     2,     3,     5,     4,     4,     1,     3,     1,     1,
       1,     1,     1,     0,     2,     2,     2,     2,     2,     1,
       1,     0,     1,     3,     3,     1,     2,     1,     3,     0,
       3,     0,     4,     0,     4,     0,     2,     0,     1,     3,
       1,     1,     1,     1,     1,     1,     2,     2,     2,     1,
       1,     1,     1,     0,     3,     2,     2
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
  "SECT_PARAM", "SECT_VAR", "SECT_OBJ", "SECT_CSTR", "UNDEF_SYMBOL",
  "DEFS_VAR", "DEFS_VALUE", "DEFS_SET", "DEFS_TMP", "DEFS_TMP_ASSIGN",
  "INT_NUMBER", "STRING", "DOUBLE_NUMBER", "MATHFUNC", "READCSV",
  "READSTDIN", "FCT_MIN", "FCT_MAX", "DIM", "DEF", "SUM", "LEN", "FORMAT",
  "TYPE", "DEFSET", "TD_REAL", "TD_INT", "TD_BINARY", "TD_STRING",
  "TD_SET", "VEKT_BEG", "OBJ_TO", "TRANSPOSE", "ERRMSG", "USRMSG",
  "LINENAME_SYMBOL", "DEFAULT", "WHILE", "BREAK", "CONTINUE", "REPEAT",
  "SET_ALL_NUM", "SET_ALL_STR", "':'", "';'", "','", "'['", "']'", "'('",
  "')'", "'{'", "'}'", "'|'", "$accept", "prog", "prog_sec", "const_sec",
  "@1", "const_defs", "@2", "const_def", "@3", "ul_sym_vekt",
  "const_type_def", "const_type", "const_set_op", "const_loop",
  "const_alts", "var_sec", "@4", "var_defs", "@5", "var_def",
  "d_var_vekts", "d_var_vekt", "var_type", "var_defber",
  "d_undef_sym_vekt", "@6", "var_loop", "var_alts", "obj_sec", "@7",
  "obj_defs", "@8", "obj_def", "@9", "obj_def_to", "obj_loop", "obj_alts",
  "cstr_sec", "@10", "cstr_defs", "@11", "cstr_def", "cstr_line", "@12",
  "o_cstr_line", "sum_line", "f_term", "leer", "term_sgn", "o_terms",
  "o_term", "f_sum_term", "sum_term", "@13", "sum_line_alts", "term",
  "@14", "tterm", "t_expr", "transp", "cstr_loop", "cstr_alts", "all_expr",
  "literal_expr", "convert_expr", "function_expr", "@15", "@16",
  "start_list", "def_var_vekt", "d_def_var_vekt", "const_list", "@17",
  "sep_list", "const_elem", "bel_sym_vekt", "func_def_par", "d_vekt",
  "d_vekt_inds", "d_vekt_ind", "o_c_expr", "const_expr",
  "const_expr_no_log", "const_set_expr", "const_loop_sum", "@18",
  "const_loop_set", "@19", "num_op_expr", "set_op_expr", "log_op_expr",
  "cmp_op_expr", "cmp_set_op_expr", "add_const_expr",
  "add_const_expr_alts", "count_const_expr", "count_const_expr_alts",
  "vekt", "vekt_inds", "vekt_ind", "d_def_const_vekt", "d_def_set_vekt",
  "def_const_vekt", "def_set_vekt", "vekt_def", "@20", "vekt_val",
  "vekt_val_list", "set_val_list", "set_expr", "verg_op", "line_name",
  "loop_start", "loop_sum_start", "loop_set_start", "loop_name",
  "loop_stop", "loop_alt", "loop_alt_start", "loop_header", "loop_heads",
  "loop_head", "@21", "@22", "@23", "loop_comp_start", "loop_mods",
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
        81,     0,    -1,    -1,    82,    81,    -1,    83,    -1,    95,
      -1,   108,    -1,   117,    -1,    -1,    27,    84,    70,    85,
      -1,    -1,    87,    85,    -1,    -1,     1,    71,    86,    85,
      -1,    -1,    89,    90,    92,    88,   184,    71,    -1,    93,
      -1,   213,    71,    -1,   215,    71,    -1,   216,    71,    -1,
     104,    -1,   180,    -1,   181,    -1,    -1,    70,    91,    -1,
      52,    -1,    53,    -1,    54,    -1,    55,    -1,     4,    -1,
       8,    -1,   192,   199,    85,    94,   196,    -1,   197,    85,
      94,    -1,    -1,    -1,    28,    96,    70,    97,    -1,    -1,
      99,    97,    -1,    -1,     1,    71,    98,    97,    -1,   100,
      70,   102,    71,    -1,   106,    -1,   213,    71,    -1,   215,
      71,    -1,   216,    71,    -1,   101,    -1,   101,    72,   100,
      -1,   104,    -1,   150,    -1,    52,   103,    -1,    53,   103,
      -1,    54,    -1,    -1,    73,   160,    26,   160,    74,    -1,
      -1,    31,   105,   157,    -1,   192,   199,    97,   107,   196,
      -1,   197,    97,   107,    -1,    -1,    -1,    29,   109,    70,
     110,    -1,    -1,   112,   110,    -1,    -1,     1,    71,   111,
     110,    -1,    -1,   191,   113,   125,   114,    71,    -1,   115,
      -1,   213,    71,    -1,   215,    71,    -1,   216,    71,    -1,
      -1,    58,    44,    -1,    58,    43,    -1,   192,   199,   110,
     116,   196,    -1,   197,   110,   116,    -1,    -1,    -1,    30,
     118,    70,   119,    -1,    -1,   121,   119,    -1,    -1,     1,
      71,   120,   119,    -1,   191,   122,    71,    -1,   140,    -1,
     213,    71,    -1,   215,    71,    -1,   216,    71,    -1,    -1,
     125,   190,   123,   124,    -1,   125,    -1,   122,    -1,   126,
     129,    -1,   127,   135,    -1,   128,   135,    -1,   131,    -1,
      -1,    17,    -1,    18,    -1,    -1,   130,   129,    -1,   128,
     135,    -1,   132,    -1,   193,   199,   125,   134,   196,    -1,
     132,    -1,    17,   193,   199,   125,   134,   196,    -1,    -1,
      18,   193,   133,   199,   125,   134,   196,    -1,   197,   125,
     134,    -1,    -1,    -1,   136,   137,    -1,   138,    -1,   138,
      19,   137,    -1,   150,   139,    -1,   142,    -1,   180,   139,
      -1,    -1,    59,    -1,   192,   199,   119,   141,   196,    -1,
     197,   119,   141,    -1,    -1,   143,    -1,   144,    -1,   145,
      -1,    75,   161,    76,    -1,    35,    -1,    37,    -1,    39,
      -1,    38,    -1,    91,    75,   163,    76,    -1,    -1,    44,
     148,   146,   151,    76,    -1,    -1,    43,   148,   147,   151,
      76,    -1,    45,    75,   155,    76,    -1,    46,    75,   156,
      76,    -1,    49,    75,   161,    72,   163,    76,    -1,    50,
      75,   163,    76,    -1,    50,    75,   149,    76,    -1,    48,
      75,   163,    76,    -1,    40,    75,   161,    76,    -1,    42,
      75,   161,    76,    -1,    75,    -1,    32,   177,    -1,    32,
     157,    -1,   154,    -1,    -1,   154,   153,   152,   151,    -1,
      72,    -1,   142,    -1,    33,   157,    -1,    32,   157,    -1,
      33,   157,    -1,    34,   157,    -1,   155,    -1,    31,   157,
      -1,    35,    -1,    -1,    73,   158,    74,    -1,   159,    -1,
     159,    72,   158,    -1,    -1,   163,    -1,    -1,   162,    -1,
     162,    -1,   170,    -1,   171,    -1,   172,    -1,   142,    -1,
     168,    -1,   182,    -1,   164,    -1,    24,   162,    -1,   161,
      -1,   189,    -1,    -1,   193,   165,   199,   173,   174,   196,
      -1,    -1,   194,   167,   199,   175,   176,   196,    -1,   162,
      17,   162,    -1,   162,    18,   162,    -1,   162,    19,   162,
      -1,   162,    20,   162,    -1,   162,    23,   162,    -1,   162,
      22,   162,    -1,   162,    21,   162,    -1,    18,   162,    -1,
      17,   162,    -1,   189,    17,   189,    -1,   189,    19,   189,
      -1,   161,     6,   161,    -1,   161,     5,   161,    -1,   162,
       8,   162,    -1,   162,    10,   162,    -1,   162,    15,   162,
      -1,   162,     9,   162,    -1,   162,    13,   162,    -1,   162,
      12,   162,    -1,   162,    14,   162,    -1,   162,    11,   162,
      -1,   189,     8,   189,    -1,   189,    10,   189,    -1,   189,
      15,   189,    -1,   189,     9,   189,    -1,   189,    13,   189,
      -1,   189,    11,   189,    -1,   189,    14,   189,    -1,   189,
      12,   189,    -1,   162,    16,   189,    -1,   163,    -1,   197,
     173,   174,    -1,    -1,   163,    -1,   197,   175,   174,    -1,
      -1,    -1,    73,   178,    74,    -1,   179,    -1,   179,    72,
     178,    -1,   161,    -1,    33,   157,    -1,    34,   157,    -1,
      33,   177,    -1,    34,   177,    -1,   186,    -1,    -1,    41,
     185,    75,   161,    76,    -1,   163,    -1,    57,   187,    76,
      -1,   186,    -1,   186,    72,   187,    -1,   161,    -1,   161,
      72,   188,    -1,    56,    75,    76,    -1,    56,    75,   188,
      76,    -1,    68,    -1,    69,    -1,    26,    -1,    26,   162,
      -1,   162,    26,    -1,   162,    26,   162,    -1,   162,    75,
     161,    76,   162,    -1,    56,    75,   189,    76,    -1,    51,
      75,   155,    76,    -1,   183,    -1,    75,   189,    76,    -1,
     169,    -1,   166,    -1,     8,    -1,    14,    -1,    13,    -1,
      -1,    31,    70,    -1,    62,    70,    -1,   195,    77,    -1,
      47,    77,    -1,    56,    77,    -1,    31,    -1,    62,    -1,
      -1,    78,    -1,   198,   212,    70,    -1,   198,    63,    70,
      -1,    79,    -1,   200,    70,    -1,   201,    -1,   201,    72,
     200,    -1,    -1,   206,   202,   205,    -1,    -1,   206,   203,
     208,   205,    -1,    -1,   206,   204,   212,   205,    -1,    -1,
     207,   206,    -1,    -1,    64,    -1,   211,   209,   210,    -1,
       4,    -1,     3,    -1,     8,    -1,   163,    -1,    31,    -1,
     161,    -1,    65,   214,    -1,    66,   214,    -1,    67,   214,
      -1,    31,    -1,    62,    -1,    35,    -1,   207,    -1,    -1,
      36,     4,   161,    -1,    60,   163,    -1,    61,   163,    -1
  };

  /* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
     YYRHS.  */
  const unsigned short int
  CMPL_Parser::yyprhs_[] =
  {
         0,     0,     3,     4,     7,     9,    11,    13,    15,    16,
      21,    22,    25,    26,    31,    32,    39,    41,    44,    47,
      50,    52,    54,    56,    57,    60,    62,    64,    66,    68,
      70,    72,    78,    82,    83,    84,    89,    90,    93,    94,
      99,   104,   106,   109,   112,   115,   117,   121,   123,   125,
     128,   131,   133,   134,   140,   141,   145,   151,   155,   156,
     157,   162,   163,   166,   167,   172,   173,   179,   181,   184,
     187,   190,   191,   194,   197,   203,   207,   208,   209,   214,
     215,   218,   219,   224,   228,   230,   233,   236,   239,   240,
     245,   247,   249,   252,   255,   258,   260,   261,   263,   265,
     266,   269,   272,   274,   280,   282,   289,   290,   298,   302,
     303,   304,   307,   309,   313,   316,   318,   321,   322,   324,
     330,   334,   335,   337,   339,   341,   345,   347,   349,   351,
     353,   358,   359,   365,   366,   372,   377,   382,   389,   394,
     399,   404,   409,   414,   416,   419,   422,   424,   425,   430,
     432,   434,   437,   440,   443,   446,   448,   451,   453,   454,
     458,   460,   464,   465,   467,   468,   470,   472,   474,   476,
     478,   480,   482,   484,   486,   489,   491,   493,   494,   501,
     502,   509,   513,   517,   521,   525,   529,   533,   537,   540,
     543,   547,   551,   555,   559,   563,   567,   571,   575,   579,
     583,   587,   591,   595,   599,   603,   607,   611,   615,   619,
     623,   627,   629,   633,   634,   636,   640,   641,   642,   646,
     648,   652,   654,   657,   660,   663,   666,   668,   669,   675,
     677,   681,   683,   687,   689,   693,   697,   702,   704,   706,
     708,   711,   714,   718,   724,   729,   734,   736,   740,   742,
     744,   746,   748,   750,   751,   754,   757,   760,   763,   766,
     768,   770,   771,   773,   777,   781,   783,   786,   788,   792,
     793,   797,   798,   803,   804,   809,   810,   813,   814,   816,
     820,   822,   824,   826,   828,   830,   832,   835,   838,   841,
     843,   845,   847,   849,   850,   854,   857
  };

  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
  const unsigned short int
  CMPL_Parser::yyrline_[] =
  {
         0,   144,   144,   145,   150,   151,   152,   153,   160,   160,
     164,   165,   166,   166,   172,   171,   191,   192,   193,   195,
     198,   200,   202,   207,   208,   212,   213,   214,   215,   219,
     220,   224,   227,   228,   235,   235,   239,   240,   241,   241,
     246,   247,   248,   249,   251,   254,   255,   258,   260,   265,
     266,   267,   271,   272,   278,   278,   289,   292,   293,   300,
     300,   304,   305,   306,   306,   311,   311,   314,   315,   316,
     318,   322,   323,   324,   328,   331,   332,   339,   339,   343,
     344,   345,   345,   350,   351,   352,   353,   355,   359,   359,
     370,   371,   375,   378,   379,   380,   383,   386,   387,   390,
     391,   394,   395,   400,   401,   404,   405,   405,   409,   410,
     415,   415,   422,   424,   428,   431,   432,   437,   438,   442,
     445,   446,   454,   455,   456,   457,   458,   464,   466,   469,
     474,   478,   478,   482,   482,   486,   490,   491,   494,   496,
     499,   501,   503,   508,   512,   516,   525,   526,   526,   531,
     535,   536,   545,   546,   547,   551,   554,   557,   562,   564,
     568,   569,   574,   576,   589,   590,   594,   595,   596,   597,
     603,   604,   605,   608,   609,   613,   614,   618,   618,   624,
     624,   634,   636,   638,   640,   642,   644,   646,   648,   650,
     655,   657,   662,   664,   669,   671,   673,   675,   677,   679,
     681,   683,   688,   690,   692,   694,   696,   698,   700,   702,
     704,   708,   712,   714,   717,   721,   723,   727,   728,   731,
     732,   735,   739,   747,   756,   759,   763,   764,   764,   768,
     771,   777,   779,   783,   785,   789,   791,   794,   796,   798,
     800,   802,   804,   807,   810,   812,   816,   819,   820,   821,
     826,   827,   828,   832,   835,   838,   847,   851,   855,   859,
     860,   861,   864,   869,   873,   879,   899,   902,   903,   906,
     906,   910,   910,   914,   914,   920,   932,   933,   936,   941,
     951,   952,   953,   956,   959,   963,   967,   969,   971,   975,
     976,   977,   978,   979,   982,   993,   994
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
      75,    76,    19,    17,    72,    18,     2,    20,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    70,    71,
       2,     8,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    73,     2,    74,    23,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    77,    79,    78,     2,     2,     2,     2,
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
      62,    63,    64,    65,    66,    67,    68,    69
    };
    if ((unsigned int) t <= yyuser_token_number_max_)
      return translate_table[t];
    else
      return yyundef_token_;
  }

  const int CMPL_Parser::yyeof_ = 0;
  const int CMPL_Parser::yylast_ = 1482;
  const int CMPL_Parser::yynnts_ = 137;
  const int CMPL_Parser::yyempty_ = -2;
  const int CMPL_Parser::yyfinal_ = 15;
  const int CMPL_Parser::yyterror_ = 1;
  const int CMPL_Parser::yyerrcode_ = 256;
  const int CMPL_Parser::yyntokens_ = 80;

  const unsigned int CMPL_Parser::yyuser_token_number_max_ = 317;
  const CMPL_Parser::token_number_type CMPL_Parser::yyundef_token_ = 2;

} // namespace cmpl

#line 998 "src/cmpl/CMPL_Parser.yy"


void cmpl::CMPL_Parser::error(const cmpl::CMPL_Parser::location_type& l, const std::string& m)
{
	PARSE_CONTEXT.output_error_parse(ERROR_TYPE_COMP, ERROR_LVL_NORMAL, m.c_str());
}


