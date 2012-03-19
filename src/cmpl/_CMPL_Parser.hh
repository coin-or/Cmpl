/* A Bison parser, made by GNU Bison 2.3.  */

/* Skeleton interface for Bison LALR(1) parsers in C++

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

/* C++ LALR(1) parser skeleton written by Akim Demaille.  */

#ifndef PARSER_HEADER_H
# define PARSER_HEADER_H

#include <string>
#include <iostream>
#include "stack.hh"

namespace cmpl
{
  class position;
  class location;
}

/* First part of user declarations.  */
#line 37 "src/cmpl/CMPL_Parser.yy"

namespace cmpl
{
	class Compiler;
}
#line 53 "src/cmpl/CMPL_Parser.yy"

// include of CmplCore.hh must stand before %union, because the definition of enum etp is needed
#include "CmplCore.hh"

#define CMPLCOMP_FOR_SCANNER	cmplcomp.scanner

// in the skeleton the definition of yyerrok is missing, thats why it is defined here
#define yyerrok     (yyerrstatus_ = 0)


/* Line 303 of lalr1.cc.  */
#line 70 "src/cmpl/_CMPL_Parser.hh"

#include "location.hh"

/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 1
#endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 1
#endif

/* Enabling the token table.  */
#ifndef YYTOKEN_TABLE
# define YYTOKEN_TABLE 0
#endif

/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)		\
do {							\
  if (N)						\
    {							\
      (Current).begin = (Rhs)[1].begin;			\
      (Current).end   = (Rhs)[N].end;			\
    }							\
  else							\
    {							\
      (Current).begin = (Current).end = (Rhs)[0].end;	\
    }							\
} while (false)
#endif

namespace cmpl
{

  /// A Bison parser.
  class CMPL_Parser
  {
  public:
    /// Symbol semantic values.
#ifndef YYSTYPE
    union semantic_type
#line 64 "src/cmpl/CMPL_Parser.yy"
{
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
/* Line 303 of lalr1.cc.  */
#line 176 "src/cmpl/_CMPL_Parser.hh"
	;
#else
    typedef YYSTYPE semantic_type;
#endif
    /// Symbol locations.
    typedef location location_type;
    /// Tokens.
    struct token
    {
      /* Tokens.  */
   enum yytokentype {
     END = 0,
     ASSIGN_ITER = 258,
     ASSIGN = 259,
     LOG_OR = 260,
     LOG_AND = 261,
     VERG_SET = 262,
     VERG_NE2 = 263,
     VERG_EQ2 = 264,
     VERG_LT = 265,
     VERG_GT = 266,
     VERG_LE = 267,
     VERG_GE = 268,
     VERG_NE = 269,
     IN_SET = 270,
     MOD = 271,
     DIV = 272,
     VORZEICHEN = 273,
     POINTS = 274,
     SUBSYM_OP = 275,
     SECT_PARAM = 276,
     SECT_VAR = 277,
     SECT_OBJ = 278,
     SECT_CSTR = 279,
     SYMBOL_UNDEF = 280,
     SYMBOL_VAR = 281,
     SYMBOL_VALUE = 282,
     SYMBOL_LINENAME = 283,
     DEFS_TMP = 284,
     DEFS_TMP_ASSIGN = 285,
     SUBSYMBOL_UNDEF = 286,
     SUBSYMBOL_VALUE = 287,
     INT_NUMBER = 288,
     STRING = 289,
     DOUBLE_NUMBER = 290,
     MATHFUNC = 291,
     PROBLEM_FUNC = 292,
     READCSV = 293,
     READSTDIN = 294,
     FCT_MIN = 295,
     FCT_MAX = 296,
     DIM = 297,
     DEF = 298,
     SUM = 299,
     LEN = 300,
     FORMAT = 301,
     TYPE = 302,
     DEFSET = 303,
     PROBLEM_SOLVE = 304,
     TD_REAL = 305,
     TD_INT = 306,
     TD_BINARY = 307,
     TD_STRING = 308,
     TD_SET = 309,
     TD_PROBLEM = 310,
     OBJ_TO = 311,
     TRANSPOSE = 312,
     ERRMSG = 313,
     USRMSG = 314,
     DEFAULT = 315,
     WHILE = 316,
     REF = 317,
     BREAK = 318,
     CONTINUE = 319,
     REPEAT = 320,
     SET_ALL_NUM = 321,
     SET_ALL_STR = 322
   };

    };
    /// Token type.
    typedef token::yytokentype token_type;

    /// Build a parser object.
    CMPL_Parser (Compiler& cmplcomp_yyarg);
    virtual ~CMPL_Parser ();

    /// Parse.
    /// \returns  0 iff parsing succeeded.
    virtual int parse ();

    /// The current debugging stream.
    std::ostream& debug_stream () const;
    /// Set the current debugging stream.
    void set_debug_stream (std::ostream &);

    /// Type for debugging levels.
    typedef int debug_level_type;
    /// The current debugging level.
    debug_level_type debug_level () const;
    /// Set the current debugging level.
    void set_debug_level (debug_level_type l);

  private:
    /// Report a syntax error.
    /// \param loc    where the syntax error is found.
    /// \param msg    a description of the syntax error.
    virtual void error (const location_type& loc, const std::string& msg);

    /// Generate an error message.
    /// \param state   the state where the error occurred.
    /// \param tok     the look-ahead token.
    virtual std::string yysyntax_error_ (int yystate, int tok);

#if YYDEBUG
    /// \brief Report a symbol value on the debug stream.
    /// \param yytype       The token type.
    /// \param yyvaluep     Its semantic value.
    /// \param yylocationp  Its location.
    virtual void yy_symbol_value_print_ (int yytype,
					 const semantic_type* yyvaluep,
					 const location_type* yylocationp);
    /// \brief Report a symbol on the debug stream.
    /// \param yytype       The token type.
    /// \param yyvaluep     Its semantic value.
    /// \param yylocationp  Its location.
    virtual void yy_symbol_print_ (int yytype,
				   const semantic_type* yyvaluep,
				   const location_type* yylocationp);
#endif /* ! YYDEBUG */


    /// State numbers.
    typedef int state_type;
    /// State stack type.
    typedef stack<state_type>    state_stack_type;
    /// Semantic value stack type.
    typedef stack<semantic_type> semantic_stack_type;
    /// location stack type.
    typedef stack<location_type> location_stack_type;

    /// The state stack.
    state_stack_type yystate_stack_;
    /// The semantic value stack.
    semantic_stack_type yysemantic_stack_;
    /// The location stack.
    location_stack_type yylocation_stack_;

    /// Internal symbol numbers.
    typedef unsigned char token_number_type;
    /* Tables.  */
    /// For a state, the index in \a yytable_ of its portion.
    static const short int yypact_[];
    static const short int yypact_ninf_;

    /// For a state, default rule to reduce.
    /// Unless\a  yytable_ specifies something else to do.
    /// Zero means the default is an error.
    static const unsigned short int yydefact_[];

    static const short int yypgoto_[];
    static const short int yydefgoto_[];

    /// What to do in a state.
    /// \a yytable_[yypact_[s]]: what to do in state \a s.
    /// - if positive, shift that token.
    /// - if negative, reduce the rule which number is the opposite.
    /// - if zero, do what YYDEFACT says.
    static const short int yytable_[];
    static const short int yytable_ninf_;

    static const short int yycheck_[];

    /// For a state, its accessing symbol.
    static const unsigned char yystos_[];

    /// For a rule, its LHS.
    static const unsigned char yyr1_[];
    /// For a rule, its RHS length.
    static const unsigned char yyr2_[];

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
    /// For a symbol, its name in clear.
    static const char* const yytname_[];
#endif

#if YYERROR_VERBOSE
    /// Convert the symbol name \a n to a form suitable for a diagnostic.
    virtual std::string yytnamerr_ (const char *n);
#endif

#if YYDEBUG
    /// A type to store symbol numbers and -1.
    typedef short int rhs_number_type;
    /// A `-1'-separated list of the rules' RHS.
    static const rhs_number_type yyrhs_[];
    /// For each rule, the index of the first RHS symbol in \a yyrhs_.
    static const unsigned short int yyprhs_[];
    /// For each rule, its source line number.
    static const unsigned short int yyrline_[];
    /// For each scanner token number, its symbol number.
    static const unsigned short int yytoken_number_[];
    /// Report on the debug stream that the rule \a r is going to be reduced.
    virtual void yy_reduce_print_ (int r);
    /// Print the state stack on the debug stream.
    virtual void yystack_print_ ();
#endif

    /// Convert a scanner token number \a t to a symbol number.
    token_number_type yytranslate_ (int t);

    /// \brief Reclaim the memory associated to a symbol.
    /// \param yymsg        Why this token is reclaimed.
    /// \param yytype       The symbol type.
    /// \param yyvaluep     Its semantic value.
    /// \param yylocationp  Its location.
    inline void yydestruct_ (const char* yymsg,
			     int yytype,
			     semantic_type* yyvaluep,
			     location_type* yylocationp);

    /// Pop \a n symbols the three stacks.
    inline void yypop_ (unsigned int n = 1);

    /* Constants.  */
    static const int yyeof_;
    /* LAST_ -- Last index in TABLE_.  */
    static const int yylast_;
    static const int yynnts_;
    static const int yyempty_;
    static const int yyfinal_;
    static const int yyterror_;
    static const int yyerrcode_;
    static const int yyntokens_;
    static const unsigned int yyuser_token_number_max_;
    static const token_number_type yyundef_token_;

    /* Debugging.  */
    int yydebug_;
    std::ostream* yycdebug_;


    /* User arguments.  */
    Compiler& cmplcomp;
  };
}


#endif /* ! defined PARSER_HEADER_H */
