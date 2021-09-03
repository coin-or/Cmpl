// A Bison parser, made by GNU Bison 3.0.4.

// Skeleton implementation for Bison LALR(1) parsers in C++

// Copyright (C) 2002-2015 Free Software Foundation, Inc.

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

// As a special exception, you may create a larger work that contains
// part or all of the Bison parser skeleton and distribute that work
// under terms of your choice, so long as that work isn't itself a
// parser generator using the skeleton or a modified version thereof
// as a parser skeleton.  Alternatively, if you modify or redistribute
// the parser skeleton itself, you may (at your option) remove this
// special exception, which will cause the skeleton and the resulting
// Bison output files to be licensed under the GNU General Public
// License without this special exception.

// This special exception was added by the Free Software Foundation in
// version 2.2 of Bison.

// Take the name prefix into account.
#define yylex   cmpllex

// First part of user declarations.
#line 36 "CMPL_Parser.yy" // lalr1.cc:404

namespace cmpl
{
    class CompilerContext;
	class SyntaxElement;
}
#line 53 "CMPL_Parser.yy" // lalr1.cc:404

#include "../../CommonData/SyntaxElements.hh"
#include "CompilerUtil.hh"
#include "CompilerContext.hh"

#define CMPLCOMP_FOR_SCANNER    cmplcomp._scanner

// in the skeleton the definition of yyerrok is missing, thats why it is defined here
#define yyerrok     (yyerrstatus_ = 0)

#line 56 "_CMPL_Parser.cc" // lalr1.cc:404

# ifndef YY_NULLPTR
#  if defined __cplusplus && 201103L <= __cplusplus
#   define YY_NULLPTR nullptr
#  else
#   define YY_NULLPTR 0
#  endif
# endif

#include "_CMPL_Parser.hh"

// User implementation prologue.
#line 110 "CMPL_Parser.yy" // lalr1.cc:412

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

#line 115 "_CMPL_Parser.cc" // lalr1.cc:412


#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> // FIXME: INFRINGES ON USER NAME SPACE.
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif

#define YYRHSLOC(Rhs, K) ((Rhs)[K].location)
/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

# ifndef YYLLOC_DEFAULT
#  define YYLLOC_DEFAULT(Current, Rhs, N)                               \
    do                                                                  \
      if (N)                                                            \
        {                                                               \
          (Current).begin  = YYRHSLOC (Rhs, 1).begin;                   \
          (Current).end    = YYRHSLOC (Rhs, N).end;                     \
        }                                                               \
      else                                                              \
        {                                                               \
          (Current).begin = (Current).end = YYRHSLOC (Rhs, 0).end;      \
        }                                                               \
    while (/*CONSTCOND*/ false)
# endif


// Suppress unused-variable warnings by "using" E.
#define YYUSE(E) ((void) (E))

// Enable debugging if requested.
#if YYDEBUG

// A pseudo ostream that takes yydebug_ into account.
# define YYCDEBUG if (yydebug_) (*yycdebug_)

# define YY_SYMBOL_PRINT(Title, Symbol)         \
  do {                                          \
    if (yydebug_)                               \
    {                                           \
      *yycdebug_ << Title << ' ';               \
      yy_print_ (*yycdebug_, Symbol);           \
      *yycdebug_ << std::endl;                  \
    }                                           \
  } while (false)

# define YY_REDUCE_PRINT(Rule)          \
  do {                                  \
    if (yydebug_)                       \
      yy_reduce_print_ (Rule);          \
  } while (false)

# define YY_STACK_PRINT()               \
  do {                                  \
    if (yydebug_)                       \
      yystack_print_ ();                \
  } while (false)

#else // !YYDEBUG

# define YYCDEBUG if (false) std::cerr
# define YY_SYMBOL_PRINT(Title, Symbol)  YYUSE(Symbol)
# define YY_REDUCE_PRINT(Rule)           static_cast<void>(0)
# define YY_STACK_PRINT()                static_cast<void>(0)

#endif // !YYDEBUG

#define yyerrok         (yyerrstatus_ = 0)
#define yyclearin       (yyla.clear ())

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab
#define YYRECOVERING()  (!!yyerrstatus_)


namespace cmpl {
#line 201 "_CMPL_Parser.cc" // lalr1.cc:479

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
              // Fall through.
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


  /// Build a parser object.
  CMPL_Parser::CMPL_Parser (CompilerContext& cmplcomp_yyarg)
    :
#if YYDEBUG
      yydebug_ (false),
      yycdebug_ (&std::cerr),
#endif
      cmplcomp (cmplcomp_yyarg)
  {}

  CMPL_Parser::~CMPL_Parser ()
  {}


  /*---------------.
  | Symbol types.  |
  `---------------*/

  inline
  CMPL_Parser::syntax_error::syntax_error (const location_type& l, const std::string& m)
    : std::runtime_error (m)
    , location (l)
  {}

  // basic_symbol.
  template <typename Base>
  inline
  CMPL_Parser::basic_symbol<Base>::basic_symbol ()
    : value ()
  {}

  template <typename Base>
  inline
  CMPL_Parser::basic_symbol<Base>::basic_symbol (const basic_symbol& other)
    : Base (other)
    , value ()
    , location (other.location)
  {
    value = other.value;
  }


  template <typename Base>
  inline
  CMPL_Parser::basic_symbol<Base>::basic_symbol (typename Base::kind_type t, const semantic_type& v, const location_type& l)
    : Base (t)
    , value (v)
    , location (l)
  {}


  /// Constructor for valueless symbols.
  template <typename Base>
  inline
  CMPL_Parser::basic_symbol<Base>::basic_symbol (typename Base::kind_type t, const location_type& l)
    : Base (t)
    , value ()
    , location (l)
  {}

  template <typename Base>
  inline
  CMPL_Parser::basic_symbol<Base>::~basic_symbol ()
  {
    clear ();
  }

  template <typename Base>
  inline
  void
  CMPL_Parser::basic_symbol<Base>::clear ()
  {
    Base::clear ();
  }

  template <typename Base>
  inline
  bool
  CMPL_Parser::basic_symbol<Base>::empty () const
  {
    return Base::type_get () == empty_symbol;
  }

  template <typename Base>
  inline
  void
  CMPL_Parser::basic_symbol<Base>::move (basic_symbol& s)
  {
    super_type::move(s);
    value = s.value;
    location = s.location;
  }

  // by_type.
  inline
  CMPL_Parser::by_type::by_type ()
    : type (empty_symbol)
  {}

  inline
  CMPL_Parser::by_type::by_type (const by_type& other)
    : type (other.type)
  {}

  inline
  CMPL_Parser::by_type::by_type (token_type t)
    : type (yytranslate_ (t))
  {}

  inline
  void
  CMPL_Parser::by_type::clear ()
  {
    type = empty_symbol;
  }

  inline
  void
  CMPL_Parser::by_type::move (by_type& that)
  {
    type = that.type;
    that.clear ();
  }

  inline
  int
  CMPL_Parser::by_type::type_get () const
  {
    return type;
  }


  // by_state.
  inline
  CMPL_Parser::by_state::by_state ()
    : state (empty_state)
  {}

  inline
  CMPL_Parser::by_state::by_state (const by_state& other)
    : state (other.state)
  {}

  inline
  void
  CMPL_Parser::by_state::clear ()
  {
    state = empty_state;
  }

  inline
  void
  CMPL_Parser::by_state::move (by_state& that)
  {
    state = that.state;
    that.clear ();
  }

  inline
  CMPL_Parser::by_state::by_state (state_type s)
    : state (s)
  {}

  inline
  CMPL_Parser::symbol_number_type
  CMPL_Parser::by_state::type_get () const
  {
    if (state == empty_state)
      return empty_symbol;
    else
      return yystos_[state];
  }

  inline
  CMPL_Parser::stack_symbol_type::stack_symbol_type ()
  {}


  inline
  CMPL_Parser::stack_symbol_type::stack_symbol_type (state_type s, symbol_type& that)
    : super_type (s, that.location)
  {
    value = that.value;
    // that is emptied.
    that.type = empty_symbol;
  }

  inline
  CMPL_Parser::stack_symbol_type&
  CMPL_Parser::stack_symbol_type::operator= (const stack_symbol_type& that)
  {
    state = that.state;
    value = that.value;
    location = that.location;
    return *this;
  }


  template <typename Base>
  inline
  void
  CMPL_Parser::yy_destroy_ (const char* yymsg, basic_symbol<Base>& yysym) const
  {
    if (yymsg)
      YY_SYMBOL_PRINT (yymsg, yysym);

    // User destructor.
    switch (yysym.type_get ())
    {
            case 1: // error

#line 233 "CMPL_Parser.yy" // lalr1.cc:614
        { if (!(yysym.value.s)._empty) { PARSE_CONTEXT.insertErrElem((yysym.value.s)._elem); }  /*if ($$._tp == semanticValueExpr) { delete $$._u.exp; }*/ }
#line 454 "_CMPL_Parser.cc" // lalr1.cc:614
        break;

      case 3: // "operator `->`"

#line 233 "CMPL_Parser.yy" // lalr1.cc:614
        { if (!(yysym.value.s)._empty) { PARSE_CONTEXT.insertErrElem((yysym.value.s)._elem); }  /*if ($$._tp == semanticValueExpr) { delete $$._u.exp; }*/ }
#line 461 "_CMPL_Parser.cc" // lalr1.cc:614
        break;

      case 4: // "assign operator `:=`"

#line 233 "CMPL_Parser.yy" // lalr1.cc:614
        { if (!(yysym.value.s)._empty) { PARSE_CONTEXT.insertErrElem((yysym.value.s)._elem); }  /*if ($$._tp == semanticValueExpr) { delete $$._u.exp; }*/ }
#line 468 "_CMPL_Parser.cc" // lalr1.cc:614
        break;

      case 9: // "operator `||`"

#line 233 "CMPL_Parser.yy" // lalr1.cc:614
        { if (!(yysym.value.s)._empty) { PARSE_CONTEXT.insertErrElem((yysym.value.s)._elem); }  /*if ($$._tp == semanticValueExpr) { delete $$._u.exp; }*/ }
#line 475 "_CMPL_Parser.cc" // lalr1.cc:614
        break;

      case 10: // "operator `&&`"

#line 233 "CMPL_Parser.yy" // lalr1.cc:614
        { if (!(yysym.value.s)._empty) { PARSE_CONTEXT.insertErrElem((yysym.value.s)._elem); }  /*if ($$._tp == semanticValueExpr) { delete $$._u.exp; }*/ }
#line 482 "_CMPL_Parser.cc" // lalr1.cc:614
        break;

      case 11: // "operator `=`"

#line 233 "CMPL_Parser.yy" // lalr1.cc:614
        { if (!(yysym.value.s)._empty) { PARSE_CONTEXT.insertErrElem((yysym.value.s)._elem); }  /*if ($$._tp == semanticValueExpr) { delete $$._u.exp; }*/ }
#line 489 "_CMPL_Parser.cc" // lalr1.cc:614
        break;

      case 12: // "operator `==`"

#line 233 "CMPL_Parser.yy" // lalr1.cc:614
        { if (!(yysym.value.s)._empty) { PARSE_CONTEXT.insertErrElem((yysym.value.s)._elem); }  /*if ($$._tp == semanticValueExpr) { delete $$._u.exp; }*/ }
#line 496 "_CMPL_Parser.cc" // lalr1.cc:614
        break;

      case 13: // "operator `<>`"

#line 233 "CMPL_Parser.yy" // lalr1.cc:614
        { if (!(yysym.value.s)._empty) { PARSE_CONTEXT.insertErrElem((yysym.value.s)._elem); }  /*if ($$._tp == semanticValueExpr) { delete $$._u.exp; }*/ }
#line 503 "_CMPL_Parser.cc" // lalr1.cc:614
        break;

      case 14: // "operator `!=`"

#line 233 "CMPL_Parser.yy" // lalr1.cc:614
        { if (!(yysym.value.s)._empty) { PARSE_CONTEXT.insertErrElem((yysym.value.s)._elem); }  /*if ($$._tp == semanticValueExpr) { delete $$._u.exp; }*/ }
#line 510 "_CMPL_Parser.cc" // lalr1.cc:614
        break;

      case 15: // "operator `<`"

#line 233 "CMPL_Parser.yy" // lalr1.cc:614
        { if (!(yysym.value.s)._empty) { PARSE_CONTEXT.insertErrElem((yysym.value.s)._elem); }  /*if ($$._tp == semanticValueExpr) { delete $$._u.exp; }*/ }
#line 517 "_CMPL_Parser.cc" // lalr1.cc:614
        break;

      case 16: // "operator `>`"

#line 233 "CMPL_Parser.yy" // lalr1.cc:614
        { if (!(yysym.value.s)._empty) { PARSE_CONTEXT.insertErrElem((yysym.value.s)._elem); }  /*if ($$._tp == semanticValueExpr) { delete $$._u.exp; }*/ }
#line 524 "_CMPL_Parser.cc" // lalr1.cc:614
        break;

      case 17: // "operator `<=`"

#line 233 "CMPL_Parser.yy" // lalr1.cc:614
        { if (!(yysym.value.s)._empty) { PARSE_CONTEXT.insertErrElem((yysym.value.s)._elem); }  /*if ($$._tp == semanticValueExpr) { delete $$._u.exp; }*/ }
#line 531 "_CMPL_Parser.cc" // lalr1.cc:614
        break;

      case 18: // "operator `>=`"

#line 233 "CMPL_Parser.yy" // lalr1.cc:614
        { if (!(yysym.value.s)._empty) { PARSE_CONTEXT.insertErrElem((yysym.value.s)._elem); }  /*if ($$._tp == semanticValueExpr) { delete $$._u.exp; }*/ }
#line 538 "_CMPL_Parser.cc" // lalr1.cc:614
        break;

      case 19: // "operator `in`"

#line 233 "CMPL_Parser.yy" // lalr1.cc:614
        { if (!(yysym.value.s)._empty) { PARSE_CONTEXT.insertErrElem((yysym.value.s)._elem); }  /*if ($$._tp == semanticValueExpr) { delete $$._u.exp; }*/ }
#line 545 "_CMPL_Parser.cc" // lalr1.cc:614
        break;

      case 20: // "operator `of`"

#line 233 "CMPL_Parser.yy" // lalr1.cc:614
        { if (!(yysym.value.s)._empty) { PARSE_CONTEXT.insertErrElem((yysym.value.s)._elem); }  /*if ($$._tp == semanticValueExpr) { delete $$._u.exp; }*/ }
#line 552 "_CMPL_Parser.cc" // lalr1.cc:614
        break;

      case 25: // "operator `*>`"

#line 233 "CMPL_Parser.yy" // lalr1.cc:614
        { if (!(yysym.value.s)._empty) { PARSE_CONTEXT.insertErrElem((yysym.value.s)._elem); }  /*if ($$._tp == semanticValueExpr) { delete $$._u.exp; }*/ }
#line 559 "_CMPL_Parser.cc" // lalr1.cc:614
        break;

      case 26: // "transpose operator `T`"

#line 233 "CMPL_Parser.yy" // lalr1.cc:614
        { if (!(yysym.value.s)._empty) { PARSE_CONTEXT.insertErrElem((yysym.value.s)._elem); }  /*if ($$._tp == semanticValueExpr) { delete $$._u.exp; }*/ }
#line 566 "_CMPL_Parser.cc" // lalr1.cc:614
        break;

      case 27: // "array cast operation"

#line 233 "CMPL_Parser.yy" // lalr1.cc:614
        { if (!(yysym.value.s)._empty) { PARSE_CONTEXT.insertErrElem((yysym.value.s)._elem); }  /*if ($$._tp == semanticValueExpr) { delete $$._u.exp; }*/ }
#line 573 "_CMPL_Parser.cc" // lalr1.cc:614
        break;

      case 29: // "operator `..`"

#line 233 "CMPL_Parser.yy" // lalr1.cc:614
        { if (!(yysym.value.s)._empty) { PARSE_CONTEXT.insertErrElem((yysym.value.s)._elem); }  /*if ($$._tp == semanticValueExpr) { delete $$._u.exp; }*/ }
#line 580 "_CMPL_Parser.cc" // lalr1.cc:614
        break;

      case 32: // "operator `!`"

#line 233 "CMPL_Parser.yy" // lalr1.cc:614
        { if (!(yysym.value.s)._empty) { PARSE_CONTEXT.insertErrElem((yysym.value.s)._elem); }  /*if ($$._tp == semanticValueExpr) { delete $$._u.exp; }*/ }
#line 587 "_CMPL_Parser.cc" // lalr1.cc:614
        break;

      case 35: // "`{`"

#line 233 "CMPL_Parser.yy" // lalr1.cc:614
        { if (!(yysym.value.s)._empty) { PARSE_CONTEXT.insertErrElem((yysym.value.s)._elem); }  /*if ($$._tp == semanticValueExpr) { delete $$._u.exp; }*/ }
#line 594 "_CMPL_Parser.cc" // lalr1.cc:614
        break;

      case 36: // "subsymbol operator `.`"

#line 233 "CMPL_Parser.yy" // lalr1.cc:614
        { if (!(yysym.value.s)._empty) { PARSE_CONTEXT.insertErrElem((yysym.value.s)._elem); }  /*if ($$._tp == semanticValueExpr) { delete $$._u.exp; }*/ }
#line 601 "_CMPL_Parser.cc" // lalr1.cc:614
        break;

      case 37: // "int literal"

#line 233 "CMPL_Parser.yy" // lalr1.cc:614
        { if (!(yysym.value.s)._empty) { PARSE_CONTEXT.insertErrElem((yysym.value.s)._elem); }  /*if ($$._tp == semanticValueExpr) { delete $$._u.exp; }*/ }
#line 608 "_CMPL_Parser.cc" // lalr1.cc:614
        break;

      case 38: // "double literal"

#line 233 "CMPL_Parser.yy" // lalr1.cc:614
        { if (!(yysym.value.s)._empty) { PARSE_CONTEXT.insertErrElem((yysym.value.s)._elem); }  /*if ($$._tp == semanticValueExpr) { delete $$._u.exp; }*/ }
#line 615 "_CMPL_Parser.cc" // lalr1.cc:614
        break;

      case 39: // "string literal"

#line 233 "CMPL_Parser.yy" // lalr1.cc:614
        { if (!(yysym.value.s)._empty) { PARSE_CONTEXT.insertErrElem((yysym.value.s)._elem); }  /*if ($$._tp == semanticValueExpr) { delete $$._u.exp; }*/ }
#line 622 "_CMPL_Parser.cc" // lalr1.cc:614
        break;

      case 40: // "linename prefix"

#line 233 "CMPL_Parser.yy" // lalr1.cc:614
        { if (!(yysym.value.s)._empty) { PARSE_CONTEXT.insertErrElem((yysym.value.s)._elem); }  /*if ($$._tp == semanticValueExpr) { delete $$._u.exp; }*/ }
#line 629 "_CMPL_Parser.cc" // lalr1.cc:614
        break;

      case 41: // "int literal factor"

#line 233 "CMPL_Parser.yy" // lalr1.cc:614
        { if (!(yysym.value.s)._empty) { PARSE_CONTEXT.insertErrElem((yysym.value.s)._elem); }  /*if ($$._tp == semanticValueExpr) { delete $$._u.exp; }*/ }
#line 636 "_CMPL_Parser.cc" // lalr1.cc:614
        break;

      case 42: // "double literal factor"

#line 233 "CMPL_Parser.yy" // lalr1.cc:614
        { if (!(yysym.value.s)._empty) { PARSE_CONTEXT.insertErrElem((yysym.value.s)._elem); }  /*if ($$._tp == semanticValueExpr) { delete $$._u.exp; }*/ }
#line 643 "_CMPL_Parser.cc" // lalr1.cc:614
        break;

      case 43: // "symbol"

#line 233 "CMPL_Parser.yy" // lalr1.cc:614
        { if (!(yysym.value.s)._empty) { PARSE_CONTEXT.insertErrElem((yysym.value.s)._elem); }  /*if ($$._tp == semanticValueExpr) { delete $$._u.exp; }*/ }
#line 650 "_CMPL_Parser.cc" // lalr1.cc:614
        break;

      case 44: // "subsymbol"

#line 233 "CMPL_Parser.yy" // lalr1.cc:614
        { if (!(yysym.value.s)._empty) { PARSE_CONTEXT.insertErrElem((yysym.value.s)._elem); }  /*if ($$._tp == semanticValueExpr) { delete $$._u.exp; }*/ }
#line 657 "_CMPL_Parser.cc" // lalr1.cc:614
        break;

      case 45: // "object type symbol"

#line 233 "CMPL_Parser.yy" // lalr1.cc:614
        { if (!(yysym.value.s)._empty) { PARSE_CONTEXT.insertErrElem((yysym.value.s)._elem); }  /*if ($$._tp == semanticValueExpr) { delete $$._u.exp; }*/ }
#line 664 "_CMPL_Parser.cc" // lalr1.cc:614
        break;

      case 46: // "data type symbol"

#line 233 "CMPL_Parser.yy" // lalr1.cc:614
        { if (!(yysym.value.s)._empty) { PARSE_CONTEXT.insertErrElem((yysym.value.s)._elem); }  /*if ($$._tp == semanticValueExpr) { delete $$._u.exp; }*/ }
#line 671 "_CMPL_Parser.cc" // lalr1.cc:614
        break;

      case 47: // "assign modificator"

#line 233 "CMPL_Parser.yy" // lalr1.cc:614
        { if (!(yysym.value.s)._empty) { PARSE_CONTEXT.insertErrElem((yysym.value.s)._elem); }  /*if ($$._tp == semanticValueExpr) { delete $$._u.exp; }*/ }
#line 678 "_CMPL_Parser.cc" // lalr1.cc:614
        break;

      case 48: // "order modificator"

#line 233 "CMPL_Parser.yy" // lalr1.cc:614
        { if (!(yysym.value.s)._empty) { PARSE_CONTEXT.insertErrElem((yysym.value.s)._elem); }  /*if ($$._tp == semanticValueExpr) { delete $$._u.exp; }*/ }
#line 685 "_CMPL_Parser.cc" // lalr1.cc:614
        break;

      case 49: // "assign operator `::=`"

#line 233 "CMPL_Parser.yy" // lalr1.cc:614
        { if (!(yysym.value.s)._empty) { PARSE_CONTEXT.insertErrElem((yysym.value.s)._elem); }  /*if ($$._tp == semanticValueExpr) { delete $$._u.exp; }*/ }
#line 692 "_CMPL_Parser.cc" // lalr1.cc:614
        break;

      case 50: // "assign operator `+=`"

#line 233 "CMPL_Parser.yy" // lalr1.cc:614
        { if (!(yysym.value.s)._empty) { PARSE_CONTEXT.insertErrElem((yysym.value.s)._elem); }  /*if ($$._tp == semanticValueExpr) { delete $$._u.exp; }*/ }
#line 699 "_CMPL_Parser.cc" // lalr1.cc:614
        break;

      case 51: // "assign operator `-=`"

#line 233 "CMPL_Parser.yy" // lalr1.cc:614
        { if (!(yysym.value.s)._empty) { PARSE_CONTEXT.insertErrElem((yysym.value.s)._elem); }  /*if ($$._tp == semanticValueExpr) { delete $$._u.exp; }*/ }
#line 706 "_CMPL_Parser.cc" // lalr1.cc:614
        break;

      case 52: // "assign operator `*=`"

#line 233 "CMPL_Parser.yy" // lalr1.cc:614
        { if (!(yysym.value.s)._empty) { PARSE_CONTEXT.insertErrElem((yysym.value.s)._elem); }  /*if ($$._tp == semanticValueExpr) { delete $$._u.exp; }*/ }
#line 713 "_CMPL_Parser.cc" // lalr1.cc:614
        break;

      case 53: // "assign operator `/=`"

#line 233 "CMPL_Parser.yy" // lalr1.cc:614
        { if (!(yysym.value.s)._empty) { PARSE_CONTEXT.insertErrElem((yysym.value.s)._elem); }  /*if ($$._tp == semanticValueExpr) { delete $$._u.exp; }*/ }
#line 720 "_CMPL_Parser.cc" // lalr1.cc:614
        break;

      case 54: // "increment/decrement operator"

#line 233 "CMPL_Parser.yy" // lalr1.cc:614
        { if (!(yysym.value.s)._empty) { PARSE_CONTEXT.insertErrElem((yysym.value.s)._elem); }  /*if ($$._tp == semanticValueExpr) { delete $$._u.exp; }*/ }
#line 727 "_CMPL_Parser.cc" // lalr1.cc:614
        break;

      case 55: // "`:`"

#line 233 "CMPL_Parser.yy" // lalr1.cc:614
        { if (!(yysym.value.s)._empty) { PARSE_CONTEXT.insertErrElem((yysym.value.s)._elem); }  /*if ($$._tp == semanticValueExpr) { delete $$._u.exp; }*/ }
#line 734 "_CMPL_Parser.cc" // lalr1.cc:614
        break;

      case 56: // "`]`"

#line 233 "CMPL_Parser.yy" // lalr1.cc:614
        { if (!(yysym.value.s)._empty) { PARSE_CONTEXT.insertErrElem((yysym.value.s)._elem); }  /*if ($$._tp == semanticValueExpr) { delete $$._u.exp; }*/ }
#line 741 "_CMPL_Parser.cc" // lalr1.cc:614
        break;

      case 57: // "`}`"

#line 233 "CMPL_Parser.yy" // lalr1.cc:614
        { if (!(yysym.value.s)._empty) { PARSE_CONTEXT.insertErrElem((yysym.value.s)._elem); }  /*if ($$._tp == semanticValueExpr) { delete $$._u.exp; }*/ }
#line 748 "_CMPL_Parser.cc" // lalr1.cc:614
        break;

      case 58: // "`|`"

#line 233 "CMPL_Parser.yy" // lalr1.cc:614
        { if (!(yysym.value.s)._empty) { PARSE_CONTEXT.insertErrElem((yysym.value.s)._elem); }  /*if ($$._tp == semanticValueExpr) { delete $$._u.exp; }*/ }
#line 755 "_CMPL_Parser.cc" // lalr1.cc:614
        break;

      case 59: // "`&`"

#line 233 "CMPL_Parser.yy" // lalr1.cc:614
        { if (!(yysym.value.s)._empty) { PARSE_CONTEXT.insertErrElem((yysym.value.s)._elem); }  /*if ($$._tp == semanticValueExpr) { delete $$._u.exp; }*/ }
#line 762 "_CMPL_Parser.cc" // lalr1.cc:614
        break;

      case 60: // "`@`"

#line 233 "CMPL_Parser.yy" // lalr1.cc:614
        { if (!(yysym.value.s)._empty) { PARSE_CONTEXT.insertErrElem((yysym.value.s)._elem); }  /*if ($$._tp == semanticValueExpr) { delete $$._u.exp; }*/ }
#line 769 "_CMPL_Parser.cc" // lalr1.cc:614
        break;

      case 61: // "`;`"

#line 233 "CMPL_Parser.yy" // lalr1.cc:614
        { if (!(yysym.value.s)._empty) { PARSE_CONTEXT.insertErrElem((yysym.value.s)._elem); }  /*if ($$._tp == semanticValueExpr) { delete $$._u.exp; }*/ }
#line 776 "_CMPL_Parser.cc" // lalr1.cc:614
        break;

      case 65: // cmpl

#line 232 "CMPL_Parser.yy" // lalr1.cc:614
        { }
#line 783 "_CMPL_Parser.cc" // lalr1.cc:614
        break;

      case 66: // cmplParts

#line 233 "CMPL_Parser.yy" // lalr1.cc:614
        { if (!(yysym.value.s)._empty) { PARSE_CONTEXT.insertErrElem((yysym.value.s)._elem); }  /*if ($$._tp == semanticValueExpr) { delete $$._u.exp; }*/ }
#line 790 "_CMPL_Parser.cc" // lalr1.cc:614
        break;

      case 67: // cmplPart

#line 233 "CMPL_Parser.yy" // lalr1.cc:614
        { if (!(yysym.value.s)._empty) { PARSE_CONTEXT.insertErrElem((yysym.value.s)._elem); }  /*if ($$._tp == semanticValueExpr) { delete $$._u.exp; }*/ }
#line 797 "_CMPL_Parser.cc" // lalr1.cc:614
        break;

      case 68: // directive

#line 233 "CMPL_Parser.yy" // lalr1.cc:614
        { if (!(yysym.value.s)._empty) { PARSE_CONTEXT.insertErrElem((yysym.value.s)._elem); }  /*if ($$._tp == semanticValueExpr) { delete $$._u.exp; }*/ }
#line 804 "_CMPL_Parser.cc" // lalr1.cc:614
        break;

      case 69: // directiveParts

#line 233 "CMPL_Parser.yy" // lalr1.cc:614
        { if (!(yysym.value.s)._empty) { PARSE_CONTEXT.insertErrElem((yysym.value.s)._elem); }  /*if ($$._tp == semanticValueExpr) { delete $$._u.exp; }*/ }
#line 811 "_CMPL_Parser.cc" // lalr1.cc:614
        break;

      case 70: // directivePart

#line 233 "CMPL_Parser.yy" // lalr1.cc:614
        { if (!(yysym.value.s)._empty) { PARSE_CONTEXT.insertErrElem((yysym.value.s)._elem); }  /*if ($$._tp == semanticValueExpr) { delete $$._u.exp; }*/ }
#line 818 "_CMPL_Parser.cc" // lalr1.cc:614
        break;

      case 71: // objDataType

#line 233 "CMPL_Parser.yy" // lalr1.cc:614
        { if (!(yysym.value.s)._empty) { PARSE_CONTEXT.insertErrElem((yysym.value.s)._elem); }  /*if ($$._tp == semanticValueExpr) { delete $$._u.exp; }*/ }
#line 825 "_CMPL_Parser.cc" // lalr1.cc:614
        break;

      case 72: // statement

#line 233 "CMPL_Parser.yy" // lalr1.cc:614
        { if (!(yysym.value.s)._empty) { PARSE_CONTEXT.insertErrElem((yysym.value.s)._elem); }  /*if ($$._tp == semanticValueExpr) { delete $$._u.exp; }*/ }
#line 832 "_CMPL_Parser.cc" // lalr1.cc:614
        break;

      case 73: // statementExpr

#line 233 "CMPL_Parser.yy" // lalr1.cc:614
        { if (!(yysym.value.s)._empty) { PARSE_CONTEXT.insertErrElem((yysym.value.s)._elem); }  /*if ($$._tp == semanticValueExpr) { delete $$._u.exp; }*/ }
#line 839 "_CMPL_Parser.cc" // lalr1.cc:614
        break;

      case 74: // assignLhsOp

#line 233 "CMPL_Parser.yy" // lalr1.cc:614
        { if (!(yysym.value.s)._empty) { PARSE_CONTEXT.insertErrElem((yysym.value.s)._elem); }  /*if ($$._tp == semanticValueExpr) { delete $$._u.exp; }*/ }
#line 846 "_CMPL_Parser.cc" // lalr1.cc:614
        break;

      case 75: // assignRhs

#line 233 "CMPL_Parser.yy" // lalr1.cc:614
        { if (!(yysym.value.s)._empty) { PARSE_CONTEXT.insertErrElem((yysym.value.s)._elem); }  /*if ($$._tp == semanticValueExpr) { delete $$._u.exp; }*/ }
#line 853 "_CMPL_Parser.cc" // lalr1.cc:614
        break;

      case 76: // objDataTypeRhs

#line 233 "CMPL_Parser.yy" // lalr1.cc:614
        { if (!(yysym.value.s)._empty) { PARSE_CONTEXT.insertErrElem((yysym.value.s)._elem); }  /*if ($$._tp == semanticValueExpr) { delete $$._u.exp; }*/ }
#line 860 "_CMPL_Parser.cc" // lalr1.cc:614
        break;

      case 77: // assignOp

#line 233 "CMPL_Parser.yy" // lalr1.cc:614
        { if (!(yysym.value.s)._empty) { PARSE_CONTEXT.insertErrElem((yysym.value.s)._elem); }  /*if ($$._tp == semanticValueExpr) { delete $$._u.exp; }*/ }
#line 867 "_CMPL_Parser.cc" // lalr1.cc:614
        break;

      case 78: // codeblock

#line 233 "CMPL_Parser.yy" // lalr1.cc:614
        { if (!(yysym.value.s)._empty) { PARSE_CONTEXT.insertErrElem((yysym.value.s)._elem); }  /*if ($$._tp == semanticValueExpr) { delete $$._u.exp; }*/ }
#line 874 "_CMPL_Parser.cc" // lalr1.cc:614
        break;

      case 79: // blockStart

#line 233 "CMPL_Parser.yy" // lalr1.cc:614
        { if (!(yysym.value.s)._empty) { PARSE_CONTEXT.insertErrElem((yysym.value.s)._elem); }  /*if ($$._tp == semanticValueExpr) { delete $$._u.exp; }*/ }
#line 881 "_CMPL_Parser.cc" // lalr1.cc:614
        break;

      case 80: // blockParts

#line 233 "CMPL_Parser.yy" // lalr1.cc:614
        { if (!(yysym.value.s)._empty) { PARSE_CONTEXT.insertErrElem((yysym.value.s)._elem); }  /*if ($$._tp == semanticValueExpr) { delete $$._u.exp; }*/ }
#line 888 "_CMPL_Parser.cc" // lalr1.cc:614
        break;

      case 81: // blockAltStart

#line 233 "CMPL_Parser.yy" // lalr1.cc:614
        { if (!(yysym.value.s)._empty) { PARSE_CONTEXT.insertErrElem((yysym.value.s)._elem); }  /*if ($$._tp == semanticValueExpr) { delete $$._u.exp; }*/ }
#line 895 "_CMPL_Parser.cc" // lalr1.cc:614
        break;

      case 82: // blockPart

#line 233 "CMPL_Parser.yy" // lalr1.cc:614
        { if (!(yysym.value.s)._empty) { PARSE_CONTEXT.insertErrElem((yysym.value.s)._elem); }  /*if ($$._tp == semanticValueExpr) { delete $$._u.exp; }*/ }
#line 902 "_CMPL_Parser.cc" // lalr1.cc:614
        break;

      case 83: // blockPartStart

#line 233 "CMPL_Parser.yy" // lalr1.cc:614
        { if (!(yysym.value.s)._empty) { PARSE_CONTEXT.insertErrElem((yysym.value.s)._elem); }  /*if ($$._tp == semanticValueExpr) { delete $$._u.exp; }*/ }
#line 909 "_CMPL_Parser.cc" // lalr1.cc:614
        break;

      case 84: // blockModifiers

#line 233 "CMPL_Parser.yy" // lalr1.cc:614
        { if (!(yysym.value.s)._empty) { PARSE_CONTEXT.insertErrElem((yysym.value.s)._elem); }  /*if ($$._tp == semanticValueExpr) { delete $$._u.exp; }*/ }
#line 916 "_CMPL_Parser.cc" // lalr1.cc:614
        break;

      case 85: // blockModifier

#line 233 "CMPL_Parser.yy" // lalr1.cc:614
        { if (!(yysym.value.s)._empty) { PARSE_CONTEXT.insertErrElem((yysym.value.s)._elem); }  /*if ($$._tp == semanticValueExpr) { delete $$._u.exp; }*/ }
#line 923 "_CMPL_Parser.cc" // lalr1.cc:614
        break;

      case 86: // blockHeaders

#line 233 "CMPL_Parser.yy" // lalr1.cc:614
        { if (!(yysym.value.s)._empty) { PARSE_CONTEXT.insertErrElem((yysym.value.s)._elem); }  /*if ($$._tp == semanticValueExpr) { delete $$._u.exp; }*/ }
#line 930 "_CMPL_Parser.cc" // lalr1.cc:614
        break;

      case 87: // blockHeader

#line 233 "CMPL_Parser.yy" // lalr1.cc:614
        { if (!(yysym.value.s)._empty) { PARSE_CONTEXT.insertErrElem((yysym.value.s)._elem); }  /*if ($$._tp == semanticValueExpr) { delete $$._u.exp; }*/ }
#line 937 "_CMPL_Parser.cc" // lalr1.cc:614
        break;

      case 88: // blockHeaderStart

#line 233 "CMPL_Parser.yy" // lalr1.cc:614
        { if (!(yysym.value.s)._empty) { PARSE_CONTEXT.insertErrElem((yysym.value.s)._elem); }  /*if ($$._tp == semanticValueExpr) { delete $$._u.exp; }*/ }
#line 944 "_CMPL_Parser.cc" // lalr1.cc:614
        break;

      case 89: // blockBody

#line 233 "CMPL_Parser.yy" // lalr1.cc:614
        { if (!(yysym.value.s)._empty) { PARSE_CONTEXT.insertErrElem((yysym.value.s)._elem); }  /*if ($$._tp == semanticValueExpr) { delete $$._u.exp; }*/ }
#line 951 "_CMPL_Parser.cc" // lalr1.cc:614
        break;

      case 90: // expressionWithCodeblockSymbolDef

#line 233 "CMPL_Parser.yy" // lalr1.cc:614
        { if (!(yysym.value.s)._empty) { PARSE_CONTEXT.insertErrElem((yysym.value.s)._elem); }  /*if ($$._tp == semanticValueExpr) { delete $$._u.exp; }*/ }
#line 958 "_CMPL_Parser.cc" // lalr1.cc:614
        break;

      case 91: // refcodeblock

#line 233 "CMPL_Parser.yy" // lalr1.cc:614
        { if (!(yysym.value.s)._empty) { PARSE_CONTEXT.insertErrElem((yysym.value.s)._elem); }  /*if ($$._tp == semanticValueExpr) { delete $$._u.exp; }*/ }
#line 965 "_CMPL_Parser.cc" // lalr1.cc:614
        break;

      case 92: // expression

#line 233 "CMPL_Parser.yy" // lalr1.cc:614
        { if (!(yysym.value.s)._empty) { PARSE_CONTEXT.insertErrElem((yysym.value.s)._elem); }  /*if ($$._tp == semanticValueExpr) { delete $$._u.exp; }*/ }
#line 972 "_CMPL_Parser.cc" // lalr1.cc:614
        break;

      case 93: // symbolIncDec

#line 233 "CMPL_Parser.yy" // lalr1.cc:614
        { if (!(yysym.value.s)._empty) { PARSE_CONTEXT.insertErrElem((yysym.value.s)._elem); }  /*if ($$._tp == semanticValueExpr) { delete $$._u.exp; }*/ }
#line 979 "_CMPL_Parser.cc" // lalr1.cc:614
        break;

      case 94: // symbol

#line 233 "CMPL_Parser.yy" // lalr1.cc:614
        { if (!(yysym.value.s)._empty) { PARSE_CONTEXT.insertErrElem((yysym.value.s)._elem); }  /*if ($$._tp == semanticValueExpr) { delete $$._u.exp; }*/ }
#line 986 "_CMPL_Parser.cc" // lalr1.cc:614
        break;

      case 95: // expressionList

#line 233 "CMPL_Parser.yy" // lalr1.cc:614
        { if (!(yysym.value.s)._empty) { PARSE_CONTEXT.insertErrElem((yysym.value.s)._elem); }  /*if ($$._tp == semanticValueExpr) { delete $$._u.exp; }*/ }
#line 993 "_CMPL_Parser.cc" // lalr1.cc:614
        break;

      case 96: // literal

#line 233 "CMPL_Parser.yy" // lalr1.cc:614
        { if (!(yysym.value.s)._empty) { PARSE_CONTEXT.insertErrElem((yysym.value.s)._elem); }  /*if ($$._tp == semanticValueExpr) { delete $$._u.exp; }*/ }
#line 1000 "_CMPL_Parser.cc" // lalr1.cc:614
        break;

      case 97: // literalMult

#line 233 "CMPL_Parser.yy" // lalr1.cc:614
        { if (!(yysym.value.s)._empty) { PARSE_CONTEXT.insertErrElem((yysym.value.s)._elem); }  /*if ($$._tp == semanticValueExpr) { delete $$._u.exp; }*/ }
#line 1007 "_CMPL_Parser.cc" // lalr1.cc:614
        break;

      case 98: // arrayExpr

#line 233 "CMPL_Parser.yy" // lalr1.cc:614
        { if (!(yysym.value.s)._empty) { PARSE_CONTEXT.insertErrElem((yysym.value.s)._elem); }  /*if ($$._tp == semanticValueExpr) { delete $$._u.exp; }*/ }
#line 1014 "_CMPL_Parser.cc" // lalr1.cc:614
        break;

      case 99: // arrayStart

#line 233 "CMPL_Parser.yy" // lalr1.cc:614
        { if (!(yysym.value.s)._empty) { PARSE_CONTEXT.insertErrElem((yysym.value.s)._elem); }  /*if ($$._tp == semanticValueExpr) { delete $$._u.exp; }*/ }
#line 1021 "_CMPL_Parser.cc" // lalr1.cc:614
        break;

      case 100: // tupleExpr

#line 233 "CMPL_Parser.yy" // lalr1.cc:614
        { if (!(yysym.value.s)._empty) { PARSE_CONTEXT.insertErrElem((yysym.value.s)._elem); }  /*if ($$._tp == semanticValueExpr) { delete $$._u.exp; }*/ }
#line 1028 "_CMPL_Parser.cc" // lalr1.cc:614
        break;

      case 101: // tupleStart

#line 233 "CMPL_Parser.yy" // lalr1.cc:614
        { if (!(yysym.value.s)._empty) { PARSE_CONTEXT.insertErrElem((yysym.value.s)._elem); }  /*if ($$._tp == semanticValueExpr) { delete $$._u.exp; }*/ }
#line 1035 "_CMPL_Parser.cc" // lalr1.cc:614
        break;

      case 102: // listParts

#line 233 "CMPL_Parser.yy" // lalr1.cc:614
        { if (!(yysym.value.s)._empty) { PARSE_CONTEXT.insertErrElem((yysym.value.s)._elem); }  /*if ($$._tp == semanticValueExpr) { delete $$._u.exp; }*/ }
#line 1042 "_CMPL_Parser.cc" // lalr1.cc:614
        break;

      case 103: // listPart

#line 233 "CMPL_Parser.yy" // lalr1.cc:614
        { if (!(yysym.value.s)._empty) { PARSE_CONTEXT.insertErrElem((yysym.value.s)._elem); }  /*if ($$._tp == semanticValueExpr) { delete $$._u.exp; }*/ }
#line 1049 "_CMPL_Parser.cc" // lalr1.cc:614
        break;

      case 104: // intervalExpr

#line 233 "CMPL_Parser.yy" // lalr1.cc:614
        { if (!(yysym.value.s)._empty) { PARSE_CONTEXT.insertErrElem((yysym.value.s)._elem); }  /*if ($$._tp == semanticValueExpr) { delete $$._u.exp; }*/ }
#line 1056 "_CMPL_Parser.cc" // lalr1.cc:614
        break;

      case 105: // intervalOp

#line 233 "CMPL_Parser.yy" // lalr1.cc:614
        { if (!(yysym.value.s)._empty) { PARSE_CONTEXT.insertErrElem((yysym.value.s)._elem); }  /*if ($$._tp == semanticValueExpr) { delete $$._u.exp; }*/ }
#line 1063 "_CMPL_Parser.cc" // lalr1.cc:614
        break;

      case 106: // setExpr

#line 233 "CMPL_Parser.yy" // lalr1.cc:614
        { if (!(yysym.value.s)._empty) { PARSE_CONTEXT.insertErrElem((yysym.value.s)._elem); }  /*if ($$._tp == semanticValueExpr) { delete $$._u.exp; }*/ }
#line 1070 "_CMPL_Parser.cc" // lalr1.cc:614
        break;

      case 107: // setExprEnd

#line 233 "CMPL_Parser.yy" // lalr1.cc:614
        { if (!(yysym.value.s)._empty) { PARSE_CONTEXT.insertErrElem((yysym.value.s)._elem); }  /*if ($$._tp == semanticValueExpr) { delete $$._u.exp; }*/ }
#line 1077 "_CMPL_Parser.cc" // lalr1.cc:614
        break;

      case 108: // unaryOp

#line 233 "CMPL_Parser.yy" // lalr1.cc:614
        { if (!(yysym.value.s)._empty) { PARSE_CONTEXT.insertErrElem((yysym.value.s)._elem); }  /*if ($$._tp == semanticValueExpr) { delete $$._u.exp; }*/ }
#line 1084 "_CMPL_Parser.cc" // lalr1.cc:614
        break;

      case 109: // binaryOp

#line 233 "CMPL_Parser.yy" // lalr1.cc:614
        { if (!(yysym.value.s)._empty) { PARSE_CONTEXT.insertErrElem((yysym.value.s)._elem); }  /*if ($$._tp == semanticValueExpr) { delete $$._u.exp; }*/ }
#line 1091 "_CMPL_Parser.cc" // lalr1.cc:614
        break;

      case 110: // numOpSignAdd

#line 233 "CMPL_Parser.yy" // lalr1.cc:614
        { if (!(yysym.value.s)._empty) { PARSE_CONTEXT.insertErrElem((yysym.value.s)._elem); }  /*if ($$._tp == semanticValueExpr) { delete $$._u.exp; }*/ }
#line 1098 "_CMPL_Parser.cc" // lalr1.cc:614
        break;

      case 111: // numOpSignMult

#line 233 "CMPL_Parser.yy" // lalr1.cc:614
        { if (!(yysym.value.s)._empty) { PARSE_CONTEXT.insertErrElem((yysym.value.s)._elem); }  /*if ($$._tp == semanticValueExpr) { delete $$._u.exp; }*/ }
#line 1105 "_CMPL_Parser.cc" // lalr1.cc:614
        break;

      case 112: // logOpSignNot

#line 233 "CMPL_Parser.yy" // lalr1.cc:614
        { if (!(yysym.value.s)._empty) { PARSE_CONTEXT.insertErrElem((yysym.value.s)._elem); }  /*if ($$._tp == semanticValueExpr) { delete $$._u.exp; }*/ }
#line 1112 "_CMPL_Parser.cc" // lalr1.cc:614
        break;

      case 113: // inOpPre

#line 233 "CMPL_Parser.yy" // lalr1.cc:614
        { if (!(yysym.value.s)._empty) { PARSE_CONTEXT.insertErrElem((yysym.value.s)._elem); }  /*if ($$._tp == semanticValueExpr) { delete $$._u.exp; }*/ }
#line 1119 "_CMPL_Parser.cc" // lalr1.cc:614
        break;

      case 114: // ofOpPre

#line 233 "CMPL_Parser.yy" // lalr1.cc:614
        { if (!(yysym.value.s)._empty) { PARSE_CONTEXT.insertErrElem((yysym.value.s)._elem); }  /*if ($$._tp == semanticValueExpr) { delete $$._u.exp; }*/ }
#line 1126 "_CMPL_Parser.cc" // lalr1.cc:614
        break;

      case 115: // compareOp

#line 233 "CMPL_Parser.yy" // lalr1.cc:614
        { if (!(yysym.value.s)._empty) { PARSE_CONTEXT.insertErrElem((yysym.value.s)._elem); }  /*if ($$._tp == semanticValueExpr) { delete $$._u.exp; }*/ }
#line 1133 "_CMPL_Parser.cc" // lalr1.cc:614
        break;

      case 116: // subsymbolOp

#line 233 "CMPL_Parser.yy" // lalr1.cc:614
        { if (!(yysym.value.s)._empty) { PARSE_CONTEXT.insertErrElem((yysym.value.s)._elem); }  /*if ($$._tp == semanticValueExpr) { delete $$._u.exp; }*/ }
#line 1140 "_CMPL_Parser.cc" // lalr1.cc:614
        break;

      case 117: // subsymbolIncDec

#line 233 "CMPL_Parser.yy" // lalr1.cc:614
        { if (!(yysym.value.s)._empty) { PARSE_CONTEXT.insertErrElem((yysym.value.s)._elem); }  /*if ($$._tp == semanticValueExpr) { delete $$._u.exp; }*/ }
#line 1147 "_CMPL_Parser.cc" // lalr1.cc:614
        break;

      case 118: // subsymbol

#line 233 "CMPL_Parser.yy" // lalr1.cc:614
        { if (!(yysym.value.s)._empty) { PARSE_CONTEXT.insertErrElem((yysym.value.s)._elem); }  /*if ($$._tp == semanticValueExpr) { delete $$._u.exp; }*/ }
#line 1154 "_CMPL_Parser.cc" // lalr1.cc:614
        break;

      case 119: // functionCall

#line 233 "CMPL_Parser.yy" // lalr1.cc:614
        { if (!(yysym.value.s)._empty) { PARSE_CONTEXT.insertErrElem((yysym.value.s)._elem); }  /*if ($$._tp == semanticValueExpr) { delete $$._u.exp; }*/ }
#line 1161 "_CMPL_Parser.cc" // lalr1.cc:614
        break;

      case 120: // indexation

#line 233 "CMPL_Parser.yy" // lalr1.cc:614
        { if (!(yysym.value.s)._empty) { PARSE_CONTEXT.insertErrElem((yysym.value.s)._elem); }  /*if ($$._tp == semanticValueExpr) { delete $$._u.exp; }*/ }
#line 1168 "_CMPL_Parser.cc" // lalr1.cc:614
        break;

      case 121: // arraycast

#line 233 "CMPL_Parser.yy" // lalr1.cc:614
        { if (!(yysym.value.s)._empty) { PARSE_CONTEXT.insertErrElem((yysym.value.s)._elem); }  /*if ($$._tp == semanticValueExpr) { delete $$._u.exp; }*/ }
#line 1175 "_CMPL_Parser.cc" // lalr1.cc:614
        break;

      case 122: // errorExpression

#line 233 "CMPL_Parser.yy" // lalr1.cc:614
        { if (!(yysym.value.s)._empty) { PARSE_CONTEXT.insertErrElem((yysym.value.s)._elem); }  /*if ($$._tp == semanticValueExpr) { delete $$._u.exp; }*/ }
#line 1182 "_CMPL_Parser.cc" // lalr1.cc:614
        break;


      default:
        break;
    }
  }

#if YYDEBUG
  template <typename Base>
  void
  CMPL_Parser::yy_print_ (std::ostream& yyo,
                                     const basic_symbol<Base>& yysym) const
  {
    std::ostream& yyoutput = yyo;
    YYUSE (yyoutput);
    symbol_number_type yytype = yysym.type_get ();
    // Avoid a (spurious) G++ 4.8 warning about "array subscript is
    // below array bounds".
    if (yysym.empty ())
      std::abort ();
    yyo << (yytype < yyntokens_ ? "token" : "nterm")
        << ' ' << yytname_[yytype] << " ("
        << yysym.location << ": ";
    YYUSE (yytype);
    yyo << ')';
  }
#endif

  inline
  void
  CMPL_Parser::yypush_ (const char* m, state_type s, symbol_type& sym)
  {
    stack_symbol_type t (s, sym);
    yypush_ (m, t);
  }

  inline
  void
  CMPL_Parser::yypush_ (const char* m, stack_symbol_type& s)
  {
    if (m)
      YY_SYMBOL_PRINT (m, s);
    yystack_.push (s);
  }

  inline
  void
  CMPL_Parser::yypop_ (unsigned int n)
  {
    yystack_.pop (n);
  }

#if YYDEBUG
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
#endif // YYDEBUG

  inline CMPL_Parser::state_type
  CMPL_Parser::yy_lr_goto_state_ (state_type yystate, int yysym)
  {
    int yyr = yypgoto_[yysym - yyntokens_] + yystate;
    if (0 <= yyr && yyr <= yylast_ && yycheck_[yyr] == yystate)
      return yytable_[yyr];
    else
      return yydefgoto_[yysym - yyntokens_];
  }

  inline bool
  CMPL_Parser::yy_pact_value_is_default_ (int yyvalue)
  {
    return yyvalue == yypact_ninf_;
  }

  inline bool
  CMPL_Parser::yy_table_value_is_error_ (int yyvalue)
  {
    return yyvalue == yytable_ninf_;
  }

  int
  CMPL_Parser::parse ()
  {
    // State.
    int yyn;
    /// Length of the RHS of the rule being reduced.
    int yylen = 0;

    // Error handling.
    int yynerrs_ = 0;
    int yyerrstatus_ = 0;

    /// The lookahead symbol.
    symbol_type yyla;

    /// The locations where the error started and ended.
    stack_symbol_type yyerror_range[3];

    /// The return value of parse ().
    int yyresult;

    // FIXME: This shoud be completely indented.  It is not yet to
    // avoid gratuitous conflicts when merging into the master branch.
    try
      {
    YYCDEBUG << "Starting parse" << std::endl;


    /* Initialize the stack.  The initial state will be set in
       yynewstate, since the latter expects the semantical and the
       location values to have been already stored, initialize these
       stacks with a primary value.  */
    yystack_.clear ();
    yypush_ (YY_NULLPTR, 0, yyla);

    // A new symbol was pushed on the stack.
  yynewstate:
    YYCDEBUG << "Entering state " << yystack_[0].state << std::endl;

    // Accept?
    if (yystack_[0].state == yyfinal_)
      goto yyacceptlab;

    goto yybackup;

    // Backup.
  yybackup:

    // Try to take a decision without lookahead.
    yyn = yypact_[yystack_[0].state];
    if (yy_pact_value_is_default_ (yyn))
      goto yydefault;

    // Read a lookahead token.
    if (yyla.empty ())
      {
        YYCDEBUG << "Reading a token: ";
        try
          {
            yyla.type = yytranslate_ (yylex (&yyla.value, &yyla.location, CMPLCOMP_FOR_SCANNER, cmplcomp));
          }
        catch (const syntax_error& yyexc)
          {
            error (yyexc);
            goto yyerrlab1;
          }
      }
    YY_SYMBOL_PRINT ("Next token is", yyla);

    /* If the proper action on seeing token YYLA.TYPE is to reduce or
       to detect an error, take that action.  */
    yyn += yyla.type_get ();
    if (yyn < 0 || yylast_ < yyn || yycheck_[yyn] != yyla.type_get ())
      goto yydefault;

    // Reduce or error.
    yyn = yytable_[yyn];
    if (yyn <= 0)
      {
        if (yy_table_value_is_error_ (yyn))
          goto yyerrlab;
        yyn = -yyn;
        goto yyreduce;
      }

    // Count tokens shifted since error; after three, turn off error status.
    if (yyerrstatus_)
      --yyerrstatus_;

    // Shift the lookahead token.
    yypush_ ("Shifting", yyn, yyla);
    goto yynewstate;

  /*-----------------------------------------------------------.
  | yydefault -- do the default action for the current state.  |
  `-----------------------------------------------------------*/
  yydefault:
    yyn = yydefact_[yystack_[0].state];
    if (yyn == 0)
      goto yyerrlab;
    goto yyreduce;

  /*-----------------------------.
  | yyreduce -- Do a reduction.  |
  `-----------------------------*/
  yyreduce:
    yylen = yyr2_[yyn];
    {
      stack_symbol_type yylhs;
      yylhs.state = yy_lr_goto_state_(yystack_[yylen].state, yyr1_[yyn]);
      /* If YYLEN is nonzero, implement the default value of the
         action: '$$ = $1'.  Otherwise, use the top of the stack.

         Otherwise, the following line sets YYLHS.VALUE to garbage.
         This behavior is undocumented and Bison users should not rely
         upon it.  */
      if (yylen)
        yylhs.value = yystack_[yylen - 1].value;
      else
        yylhs.value = yystack_[0].value;

      // Compute the default @$.
      {
        slice<stack_symbol_type, stack_type> slice (yystack_, yylen);
        YYLLOC_DEFAULT (yylhs.location, slice, yylen);
      }

      // Perform the reduction.
      YY_REDUCE_PRINT (yyn);
      try
        {
          switch (yyn)
            {
  case 2:
#line 241 "CMPL_Parser.yy" // lalr1.cc:859
    { (yylhs.value.s) = (yystack_[0].value.s); PROTO_PC_OUTL("Parse cmpl End"); SET_CMPLELEM((yystack_[0].value.s), yystack_[0].location); COMP_END((yystack_[0].value.s)._elem); }
#line 1421 "_CMPL_Parser.cc" // lalr1.cc:859
    break;

  case 3:
#line 244 "CMPL_Parser.yy" // lalr1.cc:859
    { (yylhs.value.s).init(); }
#line 1427 "_CMPL_Parser.cc" // lalr1.cc:859
    break;

  case 4:
#line 245 "CMPL_Parser.yy" // lalr1.cc:859
    { (yylhs.value.s).init(); CMPLELEM_CONCAT((yylhs.value.s), (yystack_[1].value.s), (yystack_[0].value.s)); }
#line 1433 "_CMPL_Parser.cc" // lalr1.cc:859
    break;

  case 5:
#line 248 "CMPL_Parser.yy" // lalr1.cc:859
    { (yylhs.value.s) = (yystack_[0].value.s); PARSE_CONTEXT.setCbExprModeStatementStart(); PARSE_CONTEXT.setDirectiveFromCurrent(&yystack_[0].location); }
#line 1439 "_CMPL_Parser.cc" // lalr1.cc:859
    break;

  case 6:
#line 249 "CMPL_Parser.yy" // lalr1.cc:859
    { (yylhs.value.s) = (yystack_[0].value.s); PARSE_CONTEXT.setCbExprModeStatementStart(); PARSE_CONTEXT.clearModificators(); }
#line 1445 "_CMPL_Parser.cc" // lalr1.cc:859
    break;

  case 7:
#line 252 "CMPL_Parser.yy" // lalr1.cc:859
    { (yylhs.value.s).init(); CMPLELEMENTRY((yylhs.value.s), SyntaxElementDirective, yystack_[1].location, yystack_[0].location); CMPLELEM_CHILD((yylhs.value.s), (yystack_[1].value.s)); NOT_USED((yystack_[0].value.s)); }
#line 1451 "_CMPL_Parser.cc" // lalr1.cc:859
    break;

  case 8:
#line 255 "CMPL_Parser.yy" // lalr1.cc:859
    { (yylhs.value.s).init(); }
#line 1457 "_CMPL_Parser.cc" // lalr1.cc:859
    break;

  case 9:
#line 256 "CMPL_Parser.yy" // lalr1.cc:859
    { (yylhs.value.s).init(); CMPLELEM_CONCAT((yylhs.value.s), (yystack_[1].value.s), (yystack_[0].value.s)); }
#line 1463 "_CMPL_Parser.cc" // lalr1.cc:859
    break;

  case 10:
#line 259 "CMPL_Parser.yy" // lalr1.cc:859
    { (yylhs.value.s).init(); CMPLELEMENTRY_TXT((yylhs.value.s), SyntaxElementModificator, yystack_[0].location, yystack_[0].location, (yystack_[0].value.s)._name); PARSE_CONTEXT.setModificator(&yystack_[0].location, (ModificatorKey) (yystack_[0].value.s)._u.i); }
#line 1469 "_CMPL_Parser.cc" // lalr1.cc:859
    break;

  case 11:
#line 260 "CMPL_Parser.yy" // lalr1.cc:859
    { (yylhs.value.s).init(); CMPLELEMENTRY_TXT((yylhs.value.s), SyntaxElementModificator, yystack_[0].location, yystack_[0].location, (yystack_[0].value.s)._name); PARSE_CONTEXT.setModificator(&yystack_[0].location, (ModificatorKey) (yystack_[0].value.s)._u.i); }
#line 1475 "_CMPL_Parser.cc" // lalr1.cc:859
    break;

  case 12:
#line 261 "CMPL_Parser.yy" // lalr1.cc:859
    { (yylhs.value.s) = (yystack_[0].value.s); CMPLELEM_SET((yylhs.value.s), (yystack_[0].value.s)); PARSE_CONTEXT.setModificator(&yystack_[0].location, (yystack_[0].value.s)._u.exp->sym(), (SPECIALSYMBOL_IS_OBJTYPE((yystack_[0].value.s)._u.exp->sym()->special())));
																				  delete (yystack_[0].value.s)._u.exp; }
#line 1482 "_CMPL_Parser.cc" // lalr1.cc:859
    break;

  case 13:
#line 263 "CMPL_Parser.yy" // lalr1.cc:859
    { (yylhs.value.s).init(); CMPLELEMENTRY((yylhs.value.s), SyntaxElementLineNamePrefix, yystack_[0].location, yystack_[0].location);
																				  PARSE_CONTEXT.setLineNamePref((yylhs.value.s)._elem, &yystack_[0].location, (yystack_[0].value.s)._u.s); }
#line 1489 "_CMPL_Parser.cc" // lalr1.cc:859
    break;

  case 14:
#line 267 "CMPL_Parser.yy" // lalr1.cc:859
    { (yylhs.value.s) = (yystack_[0].value.s); CMPLELEMENTRY_SYM((yylhs.value.s), SyntaxElementSymbol, yystack_[0].location, yystack_[0].location, (yystack_[0].value.s)._u.exp->sym()); }
#line 1495 "_CMPL_Parser.cc" // lalr1.cc:859
    break;

  case 15:
#line 268 "CMPL_Parser.yy" // lalr1.cc:859
    { (yylhs.value.s) = (yystack_[0].value.s); CMPLELEMENTRY_SYM((yylhs.value.s), SyntaxElementSymbol, yystack_[0].location, yystack_[0].location, (yystack_[0].value.s)._u.exp->sym()); }
#line 1501 "_CMPL_Parser.cc" // lalr1.cc:859
    break;

  case 16:
#line 276 "CMPL_Parser.yy" // lalr1.cc:859
    { (yylhs.value.s).init();
		   																		  if (!(yystack_[1].value.s)._empty && PARSE_CONTEXT.testCompControlCB((yystack_[1].value.s)._elem, yystack_[1].location, false)) { CMPLELEMENTRY((yylhs.value.s), SyntaxElementStatement, yystack_[1].location, yystack_[0].location); CMPLELEM_CHILD((yylhs.value.s), (yystack_[1].value.s)); }
																				  else { CMPLELEMENTRY_EMPTY((yystack_[0].value.s), yystack_[0].location); CMPLELEMENTRY((yylhs.value.s), SyntaxElementStatement, yystack_[0].location, yystack_[0].location); CMPLELEM_CHILD((yylhs.value.s), (yystack_[0].value.s)); }
																				  PARSE_CONTEXT.endOfStatement((yylhs.value.s)._elem); }
#line 1510 "_CMPL_Parser.cc" // lalr1.cc:859
    break;

  case 17:
#line 280 "CMPL_Parser.yy" // lalr1.cc:859
    { (yylhs.value.s).init();
																				  if ((yystack_[1].value.s)._u.b) PARSE_CONTEXT.testCompControlCB((yystack_[2].value.s)._elem, yystack_[2].location, true);
																				  CMPLELEMENTRY((yylhs.value.s), SyntaxElementStatement, ((yystack_[2].value.s)._empty ? (yystack_[1].location) : (yystack_[2].location)), yystack_[0].location); CMPLELEM_CHILD2((yylhs.value.s), (yystack_[2].value.s), (yystack_[1].value.s)); PARSE_CONTEXT.endOfStatement((yylhs.value.s)._elem); NOT_USED((yystack_[0].value.s)); }
#line 1518 "_CMPL_Parser.cc" // lalr1.cc:859
    break;

  case 18:
#line 283 "CMPL_Parser.yy" // lalr1.cc:859
    { (yylhs.value.s).init(); CMPLELEMENTRY_ERR((yystack_[1].value.s), yystack_[1].location); CMPLELEMENTRY((yylhs.value.s), SyntaxElementStatement, yystack_[1].location, yystack_[0].location); CMPLELEM_CHILD((yylhs.value.s), (yystack_[1].value.s)); PARSE_CONTEXT.endOfStatement((yylhs.value.s)._elem); NOT_USED((yystack_[0].value.s));
																				  yyerrok; }
#line 1525 "_CMPL_Parser.cc" // lalr1.cc:859
    break;

  case 19:
#line 287 "CMPL_Parser.yy" // lalr1.cc:859
    { (yylhs.value.s).init(); CMPLELEMENTRY((yylhs.value.s), SyntaxElementAssignRhs, yystack_[0].location, yystack_[0].location); CMPLELEM_CHILD((yylhs.value.s), (yystack_[0].value.s)); CMPLELEM_SET((yystack_[0].value.s), (yylhs.value.s));
			  																	  CMPLELEMENTRY_TXT((yylhs.value.s), SyntaxElementAssignment, yystack_[1].location, yystack_[0].location, (yystack_[1].value.s)._u.exp->assignOp()); CMPLELEM_CHILD2((yylhs.value.s), (yystack_[1].value.s), (yystack_[0].value.s));
																				  if ((yystack_[1].value.s)._u.exp->isRefAssign()) { (yystack_[0].value.s)._u.exp->compChangeFetchLvalue(&PARSE_CONTEXT); }
																				  COMP_ASSIGN((yylhs.value.s)._elem, (yystack_[1].value.s)._u.exp, (yystack_[0].value.s)._u.exp);
																				  (yystack_[0].value.s)._u.exp->checkClearLvalSyms(&PARSE_CONTEXT);
																				  (yystack_[1].value.s)._u.exp->assignSpecialToConst((yystack_[0].value.s)._u.exp->special());
																				  delete (yystack_[1].value.s)._u.exp; delete (yystack_[0].value.s)._u.exp; (yylhs.value.s)._u.b = false; }
#line 1537 "_CMPL_Parser.cc" // lalr1.cc:859
    break;

  case 20:
#line 294 "CMPL_Parser.yy" // lalr1.cc:859
    { (yylhs.value.s) = (yystack_[0].value.s);
																				  bool controlCB = false;
																				  bool amEmpty = PARSE_CONTEXT.assignModCurrentEmpty(true);
																				  if ((yystack_[0].value.s)._u.exp->setAssignIfLhs(&PARSE_CONTEXT)) {
																				  	(yystack_[0].value.s)._u.exp->checkLNPError(&PARSE_CONTEXT);
																				  	CMPLELEMENTRY((yylhs.value.s), SyntaxElementAssignLhs, yystack_[0].location, yystack_[0].location); CMPLELEM_CHILD((yylhs.value.s), (yystack_[0].value.s));
																					COMP_ASSIGN((yylhs.value.s)._elem, (yystack_[0].value.s)._u.exp, NULL); }
																				  else {
																				    controlCB = PARSE_CONTEXT.assignModControlCB(&yystack_[0].location);
																					if (!controlCB) {
																				  		if (!amEmpty) PARSE_CONTEXT.outParseError(ERROR_LVL_NORMAL, "statement cannot have assign modificators", yystack_[0].location);
																						COMP_ASSIGN((yylhs.value.s)._elem, NULL, (yystack_[0].value.s)._u.exp); } }
																				  delete (yystack_[0].value.s)._u.exp; (yylhs.value.s)._u.b = controlCB; }
#line 1555 "_CMPL_Parser.cc" // lalr1.cc:859
    break;

  case 21:
#line 309 "CMPL_Parser.yy" // lalr1.cc:859
    { (yylhs.value.s) = (yystack_[1].value.s); CMPLELEMENTRY((yylhs.value.s), SyntaxElementAssignLhs, yystack_[1].location, yystack_[1].location); CMPLELEM_CHILD((yylhs.value.s), (yystack_[1].value.s));
			 																	  (yystack_[1].value.s)._u.exp->checkLNPError(&PARSE_CONTEXT);
			 																	  (yylhs.value.s)._u.exp->setAssignLhs(&PARSE_CONTEXT, (yystack_[0].value.s)._name, &yystack_[0].location); }
#line 1563 "_CMPL_Parser.cc" // lalr1.cc:859
    break;

  case 22:
#line 314 "CMPL_Parser.yy" // lalr1.cc:859
    { (yylhs.value.s) = (yystack_[0].value.s); (yystack_[0].value.s)._u.exp->checkLNPError(&PARSE_CONTEXT); }
#line 1569 "_CMPL_Parser.cc" // lalr1.cc:859
    break;

  case 23:
#line 315 "CMPL_Parser.yy" // lalr1.cc:859
    { (yylhs.value.s) = (yystack_[0].value.s); }
#line 1575 "_CMPL_Parser.cc" // lalr1.cc:859
    break;

  case 24:
#line 316 "CMPL_Parser.yy" // lalr1.cc:859
    { (yylhs.value.s).init(); CMPLELEMENTRY((yylhs.value.s), SyntaxElementIndexation, yystack_[1].location, yystack_[0].location); CMPLELEM_CHILD2((yylhs.value.s), (yystack_[1].value.s), (yystack_[0].value.s));
																				  (yylhs.value.s).setExp((yystack_[1].value.s)._u.exp->oper(&PARSE_CONTEXT, EXPR_INFO_OP_IND, (yystack_[0].value.s)._u.exp, true)); }
#line 1582 "_CMPL_Parser.cc" // lalr1.cc:859
    break;

  case 25:
#line 320 "CMPL_Parser.yy" // lalr1.cc:859
    { COMP_FETCH_SYM((yylhs.value.s)._elem, (yystack_[0].value.s)._u.exp); (yylhs.value.s)._u.exp->checkSetCbhExpr(&PARSE_CONTEXT); }
#line 1588 "_CMPL_Parser.cc" // lalr1.cc:859
    break;

  case 26:
#line 323 "CMPL_Parser.yy" // lalr1.cc:859
    { (yylhs.value.s) = (yystack_[0].value.s); PARSE_CONTEXT.setCbExprModeReset(); }
#line 1594 "_CMPL_Parser.cc" // lalr1.cc:859
    break;

  case 27:
#line 324 "CMPL_Parser.yy" // lalr1.cc:859
    { (yylhs.value.s) = (yystack_[0].value.s); PARSE_CONTEXT.setCbExprModeReset(); }
#line 1600 "_CMPL_Parser.cc" // lalr1.cc:859
    break;

  case 28:
#line 325 "CMPL_Parser.yy" // lalr1.cc:859
    { (yylhs.value.s) = (yystack_[0].value.s); PARSE_CONTEXT.setCbExprModeReset(); }
#line 1606 "_CMPL_Parser.cc" // lalr1.cc:859
    break;

  case 29:
#line 326 "CMPL_Parser.yy" // lalr1.cc:859
    { (yylhs.value.s) = (yystack_[0].value.s); PARSE_CONTEXT.setCbExprModeReset(); }
#line 1612 "_CMPL_Parser.cc" // lalr1.cc:859
    break;

  case 30:
#line 327 "CMPL_Parser.yy" // lalr1.cc:859
    { (yylhs.value.s) = (yystack_[0].value.s); PARSE_CONTEXT.setCbExprModeReset(); }
#line 1618 "_CMPL_Parser.cc" // lalr1.cc:859
    break;

  case 31:
#line 328 "CMPL_Parser.yy" // lalr1.cc:859
    { (yylhs.value.s) = (yystack_[0].value.s); PARSE_CONTEXT.setCbExprModeReset(); }
#line 1624 "_CMPL_Parser.cc" // lalr1.cc:859
    break;

  case 32:
#line 329 "CMPL_Parser.yy" // lalr1.cc:859
    { (yylhs.value.s) = (yystack_[0].value.s); PARSE_CONTEXT.setCbExprModeReset(); }
#line 1630 "_CMPL_Parser.cc" // lalr1.cc:859
    break;

  case 33:
#line 336 "CMPL_Parser.yy" // lalr1.cc:859
    { (yylhs.value.s).init(); CMPLELEMENTRY((yylhs.value.s), SyntaxElementCodeblock, yystack_[2].location, yystack_[0].location); CMPLELEM_CHILD((yylhs.value.s), (yystack_[1].value.s)); NOT_USED((yystack_[2].value.s)); NOT_USED((yystack_[0].value.s));
		   																		  (yylhs.value.s).setExp(new ExprParseInfo(&PARSE_CONTEXT, (yylhs.value.s)._elem->loc(), specialSymbolNo, false, true));
																				  (yylhs.value.s)._u.fnc.exp = (yylhs.value.s)._u.exp; (yylhs.value.s)._u.fnc.cntLocal = PARSE_CONTEXT.funcParseScopeInfo((yylhs.value.s)._u.fnc.useSubs);
		   																		  PARSE_CONTEXT.endParseScope((yylhs.value.s)._elem, yystack_[0].location); }
#line 1639 "_CMPL_Parser.cc" // lalr1.cc:859
    break;

  case 34:
#line 340 "CMPL_Parser.yy" // lalr1.cc:859
    { (yylhs.value.s).init(); CMPLELEMENTRY_ERR((yystack_[1].value.s), yystack_[1].location); CMPLELEMENTRY((yylhs.value.s), SyntaxElementCodeblock, yystack_[2].location, yystack_[0].location); CMPLELEM_CHILD((yylhs.value.s), (yystack_[1].value.s)); NOT_USED((yystack_[2].value.s)); NOT_USED((yystack_[0].value.s));
																				  (yylhs.value.s).setExp(new ExprParseInfo(&PARSE_CONTEXT, (yylhs.value.s)._elem->loc(), specialSymbolNo, false, true));
																				  (yylhs.value.s)._u.fnc.exp = (yylhs.value.s)._u.exp; (yylhs.value.s)._u.fnc.cntLocal = PARSE_CONTEXT.funcParseScopeInfo((yylhs.value.s)._u.fnc.useSubs);
																				  PARSE_CONTEXT.endParseScope((yylhs.value.s)._elem, yystack_[0].location);
																				  yyerrok; }
#line 1649 "_CMPL_Parser.cc" // lalr1.cc:859
    break;

  case 35:
#line 347 "CMPL_Parser.yy" // lalr1.cc:859
    { (yylhs.value.s) = (yystack_[0].value.s); PARSE_CONTEXT.startParseScope((yylhs.value.s)._elem, yystack_[0].location, parseScopeTypeBlock, (yystack_[0].value.s)._u.sptt);
																				  PARSE_CONTEXT.setCbExprModeReset(); }
#line 1656 "_CMPL_Parser.cc" // lalr1.cc:859
    break;

  case 36:
#line 351 "CMPL_Parser.yy" // lalr1.cc:859
    { (yylhs.value.s) = (yystack_[0].value.s); }
#line 1662 "_CMPL_Parser.cc" // lalr1.cc:859
    break;

  case 37:
#line 352 "CMPL_Parser.yy" // lalr1.cc:859
    { (yylhs.value.s).init(); CMPLELEM_CONCAT((yylhs.value.s), (yystack_[2].value.s), (yystack_[0].value.s)); NOT_USED((yystack_[1].value.s)); }
#line 1668 "_CMPL_Parser.cc" // lalr1.cc:859
    break;

  case 38:
#line 355 "CMPL_Parser.yy" // lalr1.cc:859
    { (yylhs.value.s) = (yystack_[0].value.s); PARSE_CONTEXT.startParseScope((yylhs.value.s)._elem, yystack_[0].location, parseScopeTypePart); PARSE_CONTEXT.setCbExprModeReset(); }
#line 1674 "_CMPL_Parser.cc" // lalr1.cc:859
    break;

  case 39:
#line 358 "CMPL_Parser.yy" // lalr1.cc:859
    { (yylhs.value.s).init(); CMPLELEMENTRY((yylhs.value.s), SyntaxElementBlockPart, yystack_[1].location, yystack_[0].location); CMPLELEM_CHILD2((yylhs.value.s), (yystack_[1].value.s), (yystack_[0].value.s)); delete (yystack_[1].value.s)._u.exp; PARSE_CONTEXT.setPartParseScopeElem((yylhs.value.s)._elem); }
#line 1680 "_CMPL_Parser.cc" // lalr1.cc:859
    break;

  case 40:
#line 361 "CMPL_Parser.yy" // lalr1.cc:859
    { (yylhs.value.s) = (yystack_[1].value.s); PARSE_CONTEXT.startParseScope((yystack_[0].value.s)._elem, yystack_[0].location, parseScopeTypeBody); PARSE_CONTEXT.setCbExprModeCBBodyStart(); NOT_USED((yystack_[2].value.s)); }
#line 1686 "_CMPL_Parser.cc" // lalr1.cc:859
    break;

  case 41:
#line 364 "CMPL_Parser.yy" // lalr1.cc:859
    { (yylhs.value.s).init(); }
#line 1692 "_CMPL_Parser.cc" // lalr1.cc:859
    break;

  case 42:
#line 365 "CMPL_Parser.yy" // lalr1.cc:859
    { (yylhs.value.s).init(); CMPLELEM_CONCAT((yylhs.value.s), (yystack_[1].value.s), (yystack_[0].value.s)); }
#line 1698 "_CMPL_Parser.cc" // lalr1.cc:859
    break;

  case 43:
#line 368 "CMPL_Parser.yy" // lalr1.cc:859
    { (yylhs.value.s) = (yystack_[0].value.s); CMPLELEMENTRY_TXT((yylhs.value.s), SyntaxElementModificator, yystack_[0].location, yystack_[0].location, (yystack_[0].value.s)._name); PARSE_CONTEXT.setParseScopeParBlockPart(ICPAR_BLOCK_ORDERED); }
#line 1704 "_CMPL_Parser.cc" // lalr1.cc:859
    break;

  case 44:
#line 371 "CMPL_Parser.yy" // lalr1.cc:859
    { (yylhs.value.s).init();
			 																	  if ((yystack_[0].value.s)._empty) { CMPLELEMENTRY_EMPTY((yystack_[0].value.s), yystack_[0].location); (yystack_[0].value.s).setExp(new ExprParseInfo(&PARSE_CONTEXT, yystack_[0].location)); }
			 																	  CMPLELEMENTRY((yylhs.value.s), SyntaxElementBlockHeader, yystack_[0].location, yystack_[0].location); CMPLELEM_CHILD((yylhs.value.s), (yystack_[0].value.s));
																				  (yylhs.value.s).setExp((yystack_[0].value.s)._u.exp); }
#line 1713 "_CMPL_Parser.cc" // lalr1.cc:859
    break;

  case 45:
#line 375 "CMPL_Parser.yy" // lalr1.cc:859
    { (yylhs.value.s).init();
																				  if ((yystack_[2].value.s)._empty) { CMPLELEMENTRY_EMPTY((yystack_[2].value.s), yystack_[2].location); (yystack_[2].value.s).setExp(new ExprParseInfo(&PARSE_CONTEXT, yystack_[2].location)); }
																				  CMPLELEMENTRY((yylhs.value.s), SyntaxElementBlockHeader, yystack_[2].location, yystack_[2].location); CMPLELEM_CHILD2((yylhs.value.s), (yystack_[2].value.s), (yystack_[0].value.s));
																				  (yylhs.value.s).setExp((yystack_[2].value.s)._u.exp->oper(&PARSE_CONTEXT, EXPR_INFO_OP_CBHCAT, (yystack_[0].value.s)._u.exp, true)); }
#line 1722 "_CMPL_Parser.cc" // lalr1.cc:859
    break;

  case 46:
#line 381 "CMPL_Parser.yy" // lalr1.cc:859
    { (yylhs.value.s).init(); }
#line 1728 "_CMPL_Parser.cc" // lalr1.cc:859
    break;

  case 47:
#line 382 "CMPL_Parser.yy" // lalr1.cc:859
    { (yylhs.value.s) = (yystack_[0].value.s); (yylhs.value.s)._u.exp->setAssignCbh(&PARSE_CONTEXT); COMP_CBHEADER_END((yylhs.value.s)._elem, (yylhs.value.s)._u.exp, (yystack_[1].value.s)._u.cbr.a); (yylhs.value.s)._u.exp->resetCbhExpr(); }
#line 1734 "_CMPL_Parser.cc" // lalr1.cc:859
    break;

  case 48:
#line 385 "CMPL_Parser.yy" // lalr1.cc:859
    { (yylhs.value.s).init(); COMP_CBHEADER_START(NULL, (yylhs.value.s)._u.cbr.a); }
#line 1740 "_CMPL_Parser.cc" // lalr1.cc:859
    break;

  case 49:
#line 388 "CMPL_Parser.yy" // lalr1.cc:859
    { (yylhs.value.s).init(); CMPLELEMENTRY((yylhs.value.s), SyntaxElementBlockBody, yystack_[1].location, yystack_[0].location); CMPLELEM_CHILD2((yylhs.value.s), (yystack_[1].value.s), (yystack_[0].value.s));
		   																		  if (!(yystack_[1].value.s)._empty) {
																				  	if (!PARSE_CONTEXT.assignModCurrentEmpty()) PARSE_CONTEXT.outParseError(ERROR_LVL_NORMAL, "expression as code block body cannot have assign modificators", yystack_[1].location);
																					if (PARSE_CONTEXT.hasLineNamePref()) PARSE_CONTEXT.outParseError(ERROR_LVL_NORMAL, "expression as code block body cannot have line name prefix", yystack_[1].location);
																				  }
																				  (yystack_[0].value.s)._u.exp->checkLNPError(&PARSE_CONTEXT); (yystack_[0].value.s)._u.exp->checkClearLvalSyms(&PARSE_CONTEXT);
																				  if ((yystack_[0].value.s)._u.exp->ncSymAccess()) PARSE_CONTEXT.setParseScopeParBlockPart(ICPAR_BLOCK_NCSYM);
		   																		  delete (yystack_[0].value.s)._u.exp; }
#line 1753 "_CMPL_Parser.cc" // lalr1.cc:859
    break;

  case 50:
#line 396 "CMPL_Parser.yy" // lalr1.cc:859
    { (yylhs.value.s).init(); CMPLELEMENTRY((yylhs.value.s), SyntaxElementBlockBody, yystack_[0].location, yystack_[0].location); CMPLELEM_CHILD((yylhs.value.s), (yystack_[0].value.s)); PARSE_CONTEXT.setParseScopeParBlockPart(ICPAR_BLOCK_STMNT); }
#line 1759 "_CMPL_Parser.cc" // lalr1.cc:859
    break;

  case 51:
#line 399 "CMPL_Parser.yy" // lalr1.cc:859
    { (yylhs.value.s) = (yystack_[0].value.s); (yystack_[0].value.s)._u.exp->checkLNPError(&PARSE_CONTEXT); }
#line 1765 "_CMPL_Parser.cc" // lalr1.cc:859
    break;

  case 52:
#line 400 "CMPL_Parser.yy" // lalr1.cc:859
    { (yystack_[2].value.s)._u.exp->checkCbhAssignError(&PARSE_CONTEXT, yystack_[1].location);
																				  (yylhs.value.s).init(); CMPLELEMENTRY_ALT((yylhs.value.s), SyntaxElementCompareOp, yystack_[2].location, yystack_[0].location, (yystack_[2].value.s)._name, "=="); CMPLELEM_CHILD2((yylhs.value.s), (yystack_[2].value.s), (yystack_[0].value.s)); NOT_USED((yystack_[1].value.s));
                                                                                  (yylhs.value.s).setExp((yystack_[2].value.s)._u.exp->oper(&PARSE_CONTEXT, EXPR_INFO_OP_CMP2, (yystack_[0].value.s)._u.exp, true));
																				  COMP_OPERATION((yylhs.value.s)._elem, ICS_OPER_EQ, 2); }
#line 1774 "_CMPL_Parser.cc" // lalr1.cc:859
    break;

  case 53:
#line 406 "CMPL_Parser.yy" // lalr1.cc:859
    { (yylhs.value.s) = (yystack_[0].value.s); PARSE_CONTEXT.setCbExprModeReset(); (yylhs.value.s)._u.cbr.b = (yystack_[0].value.s)._u.b; COMP_FUNC_START((yylhs.value.s)._elem, (yylhs.value.s)._u.cbr.a); }
#line 1780 "_CMPL_Parser.cc" // lalr1.cc:859
    break;

  case 54:
#line 412 "CMPL_Parser.yy" // lalr1.cc:859
    { (yylhs.value.s) = (yystack_[0].value.s); PARSE_CONTEXT.setCbExprModeResetStorePrv(); }
#line 1786 "_CMPL_Parser.cc" // lalr1.cc:859
    break;

  case 55:
#line 413 "CMPL_Parser.yy" // lalr1.cc:859
    { (yylhs.value.s) = (yystack_[0].value.s); PARSE_CONTEXT.setCbExprModeReset(); }
#line 1792 "_CMPL_Parser.cc" // lalr1.cc:859
    break;

  case 56:
#line 414 "CMPL_Parser.yy" // lalr1.cc:859
    { (yylhs.value.s) = (yystack_[0].value.s); PARSE_CONTEXT.setCbExprModeReset(); }
#line 1798 "_CMPL_Parser.cc" // lalr1.cc:859
    break;

  case 57:
#line 415 "CMPL_Parser.yy" // lalr1.cc:859
    { (yylhs.value.s) = (yystack_[0].value.s); PARSE_CONTEXT.setCbExprModeReset(); }
#line 1804 "_CMPL_Parser.cc" // lalr1.cc:859
    break;

  case 58:
#line 416 "CMPL_Parser.yy" // lalr1.cc:859
    { (yylhs.value.s) = (yystack_[0].value.s); PARSE_CONTEXT.setCbExprModeReset(); }
#line 1810 "_CMPL_Parser.cc" // lalr1.cc:859
    break;

  case 59:
#line 417 "CMPL_Parser.yy" // lalr1.cc:859
    { (yylhs.value.s) = (yystack_[0].value.s); PARSE_CONTEXT.setCbExprModeReset(); }
#line 1816 "_CMPL_Parser.cc" // lalr1.cc:859
    break;

  case 60:
#line 418 "CMPL_Parser.yy" // lalr1.cc:859
    { (yylhs.value.s) = (yystack_[0].value.s); PARSE_CONTEXT.setCbExprModeReset(); }
#line 1822 "_CMPL_Parser.cc" // lalr1.cc:859
    break;

  case 61:
#line 419 "CMPL_Parser.yy" // lalr1.cc:859
    { (yylhs.value.s) = (yystack_[0].value.s); PARSE_CONTEXT.setCbExprModeReset(); }
#line 1828 "_CMPL_Parser.cc" // lalr1.cc:859
    break;

  case 62:
#line 420 "CMPL_Parser.yy" // lalr1.cc:859
    { (yylhs.value.s) = (yystack_[0].value.s); PARSE_CONTEXT.setCbExprModeReset(); }
#line 1834 "_CMPL_Parser.cc" // lalr1.cc:859
    break;

  case 63:
#line 421 "CMPL_Parser.yy" // lalr1.cc:859
    { (yylhs.value.s) = (yystack_[0].value.s); PARSE_CONTEXT.setCbExprModeReset(); }
#line 1840 "_CMPL_Parser.cc" // lalr1.cc:859
    break;

  case 64:
#line 422 "CMPL_Parser.yy" // lalr1.cc:859
    { (yylhs.value.s) = (yystack_[0].value.s); PARSE_CONTEXT.setCbExprModeReset(); }
#line 1846 "_CMPL_Parser.cc" // lalr1.cc:859
    break;

  case 65:
#line 423 "CMPL_Parser.yy" // lalr1.cc:859
    { (yylhs.value.s) = (yystack_[0].value.s); PARSE_CONTEXT.setCbExprModeReset(); }
#line 1852 "_CMPL_Parser.cc" // lalr1.cc:859
    break;

  case 66:
#line 424 "CMPL_Parser.yy" // lalr1.cc:859
    { (yylhs.value.s) = (yystack_[0].value.s); PARSE_CONTEXT.setCbExprModeReset(); }
#line 1858 "_CMPL_Parser.cc" // lalr1.cc:859
    break;

  case 67:
#line 425 "CMPL_Parser.yy" // lalr1.cc:859
    { (yylhs.value.s) = (yystack_[0].value.s); PARSE_CONTEXT.setCbExprModeReset(); }
#line 1864 "_CMPL_Parser.cc" // lalr1.cc:859
    break;

  case 68:
#line 426 "CMPL_Parser.yy" // lalr1.cc:859
    { (yylhs.value.s) = (yystack_[0].value.s); (yylhs.value.s)._u.exp = (yystack_[0].value.s)._u.fnc.exp; (yylhs.value.s)._u.exp->resetVolaExp();
																				  CMPLELEMENTRY_TXT((yylhs.value.s), SyntaxElementFunction, yystack_[1].location, yystack_[0].location, ((yystack_[1].value.s)._u.cbr.b ? "pure" : "nonpure")); CMPLELEM_CHILD((yylhs.value.s), (yystack_[0].value.s));
																				  COMP_FUNC_END((yylhs.value.s)._elem, (yystack_[1].value.s)._u.cbr.a, (yystack_[0].value.s)._u.fnc.cntLocal, (yystack_[0].value.s)._u.fnc.useSubs);
																				  PARSE_CONTEXT.setCbExprModeReset(); }
#line 1873 "_CMPL_Parser.cc" // lalr1.cc:859
    break;

  case 69:
#line 430 "CMPL_Parser.yy" // lalr1.cc:859
    { (yylhs.value.s) = (yystack_[0].value.s); PARSE_CONTEXT.setCbExprModeReset();
																				  PARSE_CONTEXT.outParseError(ERROR_LVL_NORMAL, "illegal expression, maybe missing operation sign", yystack_[0].location); }
#line 1880 "_CMPL_Parser.cc" // lalr1.cc:859
    break;

  case 70:
#line 434 "CMPL_Parser.yy" // lalr1.cc:859
    { (yylhs.value.s) = (yystack_[0].value.s); (yylhs.value.s)._u.exp->checkSetCbhExpr(&PARSE_CONTEXT); }
#line 1886 "_CMPL_Parser.cc" // lalr1.cc:859
    break;

  case 71:
#line 435 "CMPL_Parser.yy" // lalr1.cc:859
    { (yylhs.value.s) = (yystack_[0].value.s); CMPLELEMENTRY_TXT((yylhs.value.s), SyntaxElementIncDec, yystack_[1].location, yystack_[0].location, ((yystack_[1].value.s)._u.i > 0 ? "prefix inc" : "prefix dec")); CMPLELEM_CHILD((yylhs.value.s), (yystack_[0].value.s));
																				  (yylhs.value.s).setExp((yystack_[0].value.s)._u.exp->oper(&PARSE_CONTEXT, EXPR_INFO_OP_INC, NULL, false));
																				  (yylhs.value.s)._u.exp->checkSetCbhExpr(&PARSE_CONTEXT);
																				  COMP_INCDEC_FETCH((yylhs.value.s)._elem, true, (yystack_[1].value.s)._u.i); }
#line 1895 "_CMPL_Parser.cc" // lalr1.cc:859
    break;

  case 72:
#line 439 "CMPL_Parser.yy" // lalr1.cc:859
    { (yylhs.value.s) = (yystack_[1].value.s); CMPLELEMENTRY_TXT((yylhs.value.s), SyntaxElementIncDec, yystack_[1].location, yystack_[0].location, ((yystack_[0].value.s)._u.i > 0 ? "postfix inc" : "postfix dec")); CMPLELEM_CHILD((yylhs.value.s), (yystack_[1].value.s));
																				  (yylhs.value.s).setExp((yystack_[1].value.s)._u.exp->oper(&PARSE_CONTEXT, EXPR_INFO_OP_INC, NULL, false));
																				  (yylhs.value.s)._u.exp->checkSetCbhExpr(&PARSE_CONTEXT);
																				  COMP_INCDEC_FETCH((yylhs.value.s)._elem, false, (yystack_[0].value.s)._u.i); }
#line 1904 "_CMPL_Parser.cc" // lalr1.cc:859
    break;

  case 73:
#line 445 "CMPL_Parser.yy" // lalr1.cc:859
    { (yylhs.value.s) = (yystack_[0].value.s); CMPLELEMENTRY_SYM((yylhs.value.s), SyntaxElementSymbol, yystack_[0].location, yystack_[0].location, (yystack_[0].value.s)._u.exp->sym());
		 																		  COMP_FETCH_SYM((yylhs.value.s)._elem, (yystack_[0].value.s)._u.exp);
																				  (yylhs.value.s)._u.exp->setLvalueSyntaxElem((yylhs.value.s)._elem); }
#line 1912 "_CMPL_Parser.cc" // lalr1.cc:859
    break;

  case 74:
#line 450 "CMPL_Parser.yy" // lalr1.cc:859
    { (yylhs.value.s) = (yystack_[0].value.s); }
#line 1918 "_CMPL_Parser.cc" // lalr1.cc:859
    break;

  case 75:
#line 451 "CMPL_Parser.yy" // lalr1.cc:859
    { (yylhs.value.s).init(); CMPLELEM_CONCAT((yylhs.value.s), (yystack_[2].value.s), (yystack_[0].value.s)); (yylhs.value.s).setExp((yystack_[2].value.s)._u.exp->oper(&PARSE_CONTEXT, EXPR_INFO_OP_CAT, (yystack_[0].value.s)._u.exp, true)); }
#line 1924 "_CMPL_Parser.cc" // lalr1.cc:859
    break;

  case 76:
#line 454 "CMPL_Parser.yy" // lalr1.cc:859
    { (yylhs.value.s) = (yystack_[0].value.s); CMPLELEMENTRY((yylhs.value.s), SyntaxElementLitDouble, yystack_[0].location, yystack_[0].location);
		  																		  COMP_LIT_VALUE((yylhs.value.s)._elem, (yystack_[0].value.s)._u.s, ICS_LIT_REAL); (yylhs.value.s).setExp(new ExprParseInfo(&PARSE_CONTEXT, yystack_[0].location)); }
#line 1931 "_CMPL_Parser.cc" // lalr1.cc:859
    break;

  case 77:
#line 456 "CMPL_Parser.yy" // lalr1.cc:859
    { (yylhs.value.s) = (yystack_[0].value.s); CMPLELEMENTRY((yylhs.value.s), SyntaxElementLitInt, yystack_[0].location, yystack_[0].location);
																				  COMP_LIT_VALUE((yylhs.value.s)._elem, (yystack_[0].value.s)._u.s, ICS_LIT_INT); (yylhs.value.s).setExp(new ExprParseInfo(&PARSE_CONTEXT, yystack_[0].location)); }
#line 1938 "_CMPL_Parser.cc" // lalr1.cc:859
    break;

  case 78:
#line 458 "CMPL_Parser.yy" // lalr1.cc:859
    { (yylhs.value.s) = (yystack_[0].value.s); CMPLELEMENTRY((yylhs.value.s), SyntaxElementLitString, yystack_[0].location, yystack_[0].location);
																				  COMP_LIT_VALUE((yylhs.value.s)._elem, (yystack_[0].value.s)._u.s, ICS_LIT_STR); (yylhs.value.s).setExp(new ExprParseInfo(&PARSE_CONTEXT, yystack_[0].location)); }
#line 1945 "_CMPL_Parser.cc" // lalr1.cc:859
    break;

  case 79:
#line 462 "CMPL_Parser.yy" // lalr1.cc:859
    { (yylhs.value.s) = (yystack_[0].value.s); CMPLELEMENTRY((yylhs.value.s), SyntaxElementLitDouble, yystack_[0].location, yystack_[0].location);
			 																	  COMP_LIT_VALUE((yylhs.value.s)._elem, (yystack_[0].value.s)._u.s, ICS_LIT_REAL); (yylhs.value.s).setExp(new ExprParseInfo(&PARSE_CONTEXT, yystack_[0].location));
			 																	  PARSE_CONTEXT.setCbExprModeReset(); }
#line 1953 "_CMPL_Parser.cc" // lalr1.cc:859
    break;

  case 80:
#line 465 "CMPL_Parser.yy" // lalr1.cc:859
    { (yylhs.value.s) = (yystack_[0].value.s); CMPLELEMENTRY((yylhs.value.s), SyntaxElementLitInt, yystack_[0].location, yystack_[0].location);
																				  COMP_LIT_VALUE((yylhs.value.s)._elem, (yystack_[0].value.s)._u.s, ICS_LIT_INT); (yylhs.value.s).setExp(new ExprParseInfo(&PARSE_CONTEXT, yystack_[0].location));
																				  PARSE_CONTEXT.setCbExprModeReset(); }
#line 1961 "_CMPL_Parser.cc" // lalr1.cc:859
    break;

  case 81:
#line 471 "CMPL_Parser.yy" // lalr1.cc:859
    { (yylhs.value.s).init(); CMPLELEMENTRY((yylhs.value.s), SyntaxElementArray, yystack_[2].location, yystack_[0].location); CMPLELEM_CHILD((yylhs.value.s), (yystack_[1].value.s)); NOT_USED((yystack_[2].value.s));
		   																		  if ((yystack_[1].value.s)._u.exp->listCnt() || (yystack_[1].value.s)._u.exp->empty()) { COMP_CONSTRUCT((yylhs.value.s)._elem, ICS_CONSTRUCT_ARRAY, ((yystack_[1].value.s)._u.exp->listCnt() ?: 1)); }
		   																		  (yylhs.value.s).setExp((yystack_[1].value.s)._u.exp->oper(&PARSE_CONTEXT, EXPR_INFO_OP_ARR, NULL, false)); }
#line 1969 "_CMPL_Parser.cc" // lalr1.cc:859
    break;

  case 82:
#line 474 "CMPL_Parser.yy" // lalr1.cc:859
    { (yylhs.value.s).init(); CMPLELEMENTRY_ERR((yystack_[1].value.s), yystack_[1].location); CMPLELEMENTRY((yylhs.value.s), SyntaxElementArray, yystack_[2].location, yystack_[0].location); CMPLELEM_CHILD((yylhs.value.s), (yystack_[1].value.s)); NOT_USED((yystack_[2].value.s));
																				  (yylhs.value.s).setExp(new ExprParseInfo(&PARSE_CONTEXT, yystack_[2].location));
																				  yyerrok; }
#line 1977 "_CMPL_Parser.cc" // lalr1.cc:859
    break;

  case 83:
#line 479 "CMPL_Parser.yy" // lalr1.cc:859
    { (yylhs.value.s).init(); PARSE_CONTEXT.setCbExprModeReset(); }
#line 1983 "_CMPL_Parser.cc" // lalr1.cc:859
    break;

  case 84:
#line 482 "CMPL_Parser.yy" // lalr1.cc:859
    { (yylhs.value.s).init(); CMPLELEMENTRY((yylhs.value.s), SyntaxElementTupel, yystack_[2].location, yystack_[0].location); CMPLELEM_CHILD((yylhs.value.s), (yystack_[1].value.s)); NOT_USED((yystack_[2].value.s)); NOT_USED((yystack_[0].value.s))
                                                                                  COMP_CONSTRUCT((yylhs.value.s)._elem, ICS_CONSTRUCT_TUPLE, ((yystack_[1].value.s)._u.exp->listCnt() ?: 1));
		   																		  (yylhs.value.s).setExp((yystack_[1].value.s)._u.exp->oper(&PARSE_CONTEXT, EXPR_INFO_OP_TUP, NULL, false)); }
#line 1991 "_CMPL_Parser.cc" // lalr1.cc:859
    break;

  case 85:
#line 485 "CMPL_Parser.yy" // lalr1.cc:859
    { (yylhs.value.s).init(); CMPLELEMENTRY_ERR((yystack_[1].value.s), yystack_[1].location); CMPLELEMENTRY((yylhs.value.s), SyntaxElementTupel, yystack_[2].location, yystack_[0].location); CMPLELEM_CHILD((yylhs.value.s), (yystack_[1].value.s)); NOT_USED((yystack_[2].value.s)); NOT_USED((yystack_[0].value.s));
																				  (yylhs.value.s).setExp(new ExprParseInfo(&PARSE_CONTEXT, yystack_[2].location));
																				  yyerrok; }
#line 1999 "_CMPL_Parser.cc" // lalr1.cc:859
    break;

  case 86:
#line 490 "CMPL_Parser.yy" // lalr1.cc:859
    { (yylhs.value.s).init(); PARSE_CONTEXT.setCbExprModeReset(); }
#line 2005 "_CMPL_Parser.cc" // lalr1.cc:859
    break;

  case 87:
#line 493 "CMPL_Parser.yy" // lalr1.cc:859
    { (yylhs.value.s).init(); 
		   																		  if ((yystack_[0].value.s)._empty) { CMPLELEMENTRY_EMPTY((yylhs.value.s), yystack_[0].location); COMP_SET_SYNTAXELEM((yystack_[0].value.s)._u.u, (yylhs.value.s)._elem); (yystack_[0].value.s).setExp(new ExprParseInfo(&PARSE_CONTEXT, yystack_[0].location, specialSymbolKnownVal, true)); }
		   																		  else { CMPLELEM_SET((yylhs.value.s), (yystack_[0].value.s)); };
		   																		  (yylhs.value.s).setExp((yystack_[0].value.s)._u.exp); }
#line 2014 "_CMPL_Parser.cc" // lalr1.cc:859
    break;

  case 88:
#line 497 "CMPL_Parser.yy" // lalr1.cc:859
    { (yylhs.value.s).init(); 
																				  if ((yystack_[2].value.s)._empty) { CMPLELEMENTRY_EMPTY((yystack_[2].value.s), yystack_[2].location); COMP_SET_SYNTAXELEM((yystack_[2].value.s)._u.u, (yystack_[2].value.s)._elem); (yystack_[2].value.s).setExp(new ExprParseInfo(&PARSE_CONTEXT, yystack_[2].location, specialSymbolKnownVal, true)); }
																				  CMPLELEM_CONCAT((yylhs.value.s), (yystack_[2].value.s), (yystack_[0].value.s));
																				  (yylhs.value.s).setExp((yystack_[2].value.s)._u.exp->oper(&PARSE_CONTEXT, EXPR_INFO_OP_CAT, (yystack_[0].value.s)._u.exp, true)); }
#line 2023 "_CMPL_Parser.cc" // lalr1.cc:859
    break;

  case 89:
#line 503 "CMPL_Parser.yy" // lalr1.cc:859
    { (yylhs.value.s).init(); COMP_LIT_BLANK((yylhs.value.s)._u.u); }
#line 2029 "_CMPL_Parser.cc" // lalr1.cc:859
    break;

  case 90:
#line 504 "CMPL_Parser.yy" // lalr1.cc:859
    { (yylhs.value.s) = (yystack_[0].value.s); (yystack_[0].value.s)._u.exp->checkLNPError(&PARSE_CONTEXT); }
#line 2035 "_CMPL_Parser.cc" // lalr1.cc:859
    break;

  case 91:
#line 507 "CMPL_Parser.yy" // lalr1.cc:859
    { (yylhs.value.s).init(); CMPLELEMENTRY((yylhs.value.s), SyntaxElementInterval, yystack_[2].location, yystack_[0].location); CMPLELEM_CHILD2((yylhs.value.s), (yystack_[2].value.s), (yystack_[0].value.s)); NOT_USED((yystack_[1].value.s));
			 																	  (yylhs.value.s).setExp((yystack_[2].value.s)._u.exp->oper(&PARSE_CONTEXT, EXPR_INFO_OP_SET, (yystack_[0].value.s)._u.exp, true));
																				  COMP_CONSTRUCT((yylhs.value.s)._elem, ICS_CONSTRUCT_IV_LB, 2); }
#line 2043 "_CMPL_Parser.cc" // lalr1.cc:859
    break;

  case 92:
#line 510 "CMPL_Parser.yy" // lalr1.cc:859
    { (yylhs.value.s).init(); CMPLELEMENTRY((yylhs.value.s), SyntaxElementInterval, yystack_[1].location, yystack_[0].location); CMPLELEMENTRY_EMPTY((yystack_[0].value.s), yystack_[0].location); CMPLELEM_CHILD2((yylhs.value.s), (yystack_[1].value.s), (yystack_[0].value.s));
																				  (yylhs.value.s).setExp((yystack_[1].value.s)._u.exp->oper(&PARSE_CONTEXT, EXPR_INFO_OP_SET, NULL, false));
																				  COMP_CONSTRUCT((yylhs.value.s)._elem, ICS_CONSTRUCT_IV_LB, 1); }
#line 2051 "_CMPL_Parser.cc" // lalr1.cc:859
    break;

  case 93:
#line 513 "CMPL_Parser.yy" // lalr1.cc:859
    { (yylhs.value.s).init(); CMPLELEMENTRY_EMPTY((yystack_[1].value.s), yystack_[1].location); CMPLELEMENTRY((yylhs.value.s), SyntaxElementInterval, yystack_[1].location, yystack_[0].location); CMPLELEM_CHILD2((yylhs.value.s), (yystack_[1].value.s), (yystack_[0].value.s));
																				  (yylhs.value.s).setExp((yystack_[0].value.s)._u.exp->oper(&PARSE_CONTEXT, EXPR_INFO_OP_SET, NULL, false));
																				  COMP_CONSTRUCT((yylhs.value.s)._elem, ICS_CONSTRUCT_IV_WOLB, 1); }
#line 2059 "_CMPL_Parser.cc" // lalr1.cc:859
    break;

  case 94:
#line 516 "CMPL_Parser.yy" // lalr1.cc:859
    { (yylhs.value.s).init(); CMPLELEMENTRY((yylhs.value.s), SyntaxElementInterval, yystack_[0].location, yystack_[0].location);
																				  CMPLELEMENTRY_EMPTY((yystack_[0].value.s), yystack_[0].location); CMPLELEM_CHILD((yylhs.value.s), (yystack_[0].value.s));
																				  CMPLELEMENTRY_EMPTY((yystack_[0].value.s), yystack_[0].location); CMPLELEM_CHILD((yylhs.value.s), (yystack_[0].value.s));
																				  (yylhs.value.s).setExp(new ExprParseInfo(&PARSE_CONTEXT, yystack_[0].location));
																				  COMP_CONSTRUCT((yylhs.value.s)._elem, ICS_CONSTRUCT_IV_WOLB, 0); }
#line 2069 "_CMPL_Parser.cc" // lalr1.cc:859
    break;

  case 95:
#line 523 "CMPL_Parser.yy" // lalr1.cc:859
    { (yylhs.value.s) = (yystack_[0].value.s); PARSE_CONTEXT.setCbExprModeReset(); }
#line 2075 "_CMPL_Parser.cc" // lalr1.cc:859
    break;

  case 96:
#line 526 "CMPL_Parser.yy" // lalr1.cc:859
    { (yylhs.value.s).init(); CMPLELEMENTRY((yylhs.value.s), SyntaxElementSet, yystack_[2].location, yystack_[0].location); CMPLELEM_CHILD2((yylhs.value.s), (yystack_[2].value.s), (yystack_[1].value.s)); CMPLELEM_CHILD((yylhs.value.s), (yystack_[0].value.s));
		  																		  if (!(yystack_[1].value.s)._u.exp->oneElemArr()) PARSE_CONTEXT.outParseError(ERROR_LVL_NORMAL, "step for algorithmic set must be a single value", yystack_[1].location);
		  																		  (yylhs.value.s).setExp((yystack_[2].value.s)._u.exp->oper(&PARSE_CONTEXT, EXPR_INFO_OP_SET, (yystack_[0].value.s)._u.exp, true)); delete (yystack_[1].value.s)._u.exp;
																				  COMP_CONSTRUCT((yylhs.value.s)._elem, ICS_CONSTRUCT_SET, 3); }
#line 2084 "_CMPL_Parser.cc" // lalr1.cc:859
    break;

  case 97:
#line 532 "CMPL_Parser.yy" // lalr1.cc:859
    { (yylhs.value.s) = (yystack_[0].value.s); }
#line 2090 "_CMPL_Parser.cc" // lalr1.cc:859
    break;

  case 98:
#line 533 "CMPL_Parser.yy" // lalr1.cc:859
    { (yylhs.value.s) = (yystack_[0].value.s); }
#line 2096 "_CMPL_Parser.cc" // lalr1.cc:859
    break;

  case 99:
#line 540 "CMPL_Parser.yy" // lalr1.cc:859
    { (yylhs.value.s).init(); CMPLELEMENTRY_TXT((yylhs.value.s), SyntaxElementUnaryOp, yystack_[1].location, yystack_[0].location, ((yystack_[1].value.s)._u.b ? "+" : "-")); CMPLELEM_CHILD((yylhs.value.s), (yystack_[0].value.s)); (yylhs.value.s).setExp((yystack_[0].value.s)._u.exp->oper(&PARSE_CONTEXT, EXPR_INFO_OP_NUM, NULL, false));
		  																		  COMP_OPERATION((yylhs.value.s)._elem, ((yystack_[1].value.s)._u.b ? ICS_OPER_ADD : ICS_OPER_SUB), 1); }
#line 2103 "_CMPL_Parser.cc" // lalr1.cc:859
    break;

  case 100:
#line 542 "CMPL_Parser.yy" // lalr1.cc:859
    { (yylhs.value.s).init(); CMPLELEMENTRY_EMPTY((yystack_[0].value.s), yystack_[0].location); CMPLELEMENTRY_TXT((yylhs.value.s), SyntaxElementUnaryOp, yystack_[0].location, yystack_[0].location, ((yystack_[0].value.s)._u.b ? "*" : "/")); CMPLELEM_CHILD((yylhs.value.s), (yystack_[0].value.s)); (yylhs.value.s).setExp(new ExprParseInfo(&PARSE_CONTEXT, yystack_[0].location));
																				  COMP_OPERATION((yylhs.value.s)._elem, ((yystack_[0].value.s)._u.b ? ICS_OPER_MUL : ICS_OPER_DIV), 0); }
#line 2110 "_CMPL_Parser.cc" // lalr1.cc:859
    break;

  case 101:
#line 544 "CMPL_Parser.yy" // lalr1.cc:859
    { bool arr = (yystack_[0].value.s)._u.exp->expTypeArray(false);
                                                                                  (yylhs.value.s).init(); CMPLELEMENTRY_TXT((yylhs.value.s), SyntaxElementUnaryOp, yystack_[1].location, yystack_[0].location, ((yystack_[1].value.s)._u.b ? "*" : "/")); CMPLELEM_CHILD((yylhs.value.s), (yystack_[0].value.s)); (yylhs.value.s).setExp((yystack_[0].value.s)._u.exp->oper(&PARSE_CONTEXT, EXPR_INFO_OP_NUM, NULL, false));
                                                                                  if (arr) {
                                                                                    COMP_ARRCONSTRUCT_MODE((yylhs.value.s)._elem, (yystack_[1].value.s)._u.b);
																				  } else {
																					COMP_OPERATION((yylhs.value.s)._elem, ((yystack_[1].value.s)._u.b ? ICS_OPER_MUL : ICS_OPER_DIV), 1);
																				} }
#line 2122 "_CMPL_Parser.cc" // lalr1.cc:859
    break;

  case 102:
#line 551 "CMPL_Parser.yy" // lalr1.cc:859
    { (yylhs.value.s).init(); CMPLELEMENTRY_TXT((yylhs.value.s), SyntaxElementUnaryOp, yystack_[1].location, yystack_[0].location, (yystack_[0].value.s)._name); CMPLELEM_CHILD((yylhs.value.s), (yystack_[1].value.s)); (yylhs.value.s).setExp((yystack_[1].value.s)._u.exp->oper(&PARSE_CONTEXT, EXPR_INFO_OP_NUM, NULL, false));
																				  COMP_OPERATION((yylhs.value.s)._elem, ICS_OPER_TRP, 1); }
#line 2129 "_CMPL_Parser.cc" // lalr1.cc:859
    break;

  case 103:
#line 553 "CMPL_Parser.yy" // lalr1.cc:859
    { (yylhs.value.s).init(); CMPLELEMENTRY_TXT((yylhs.value.s), SyntaxElementUnaryOp, yystack_[2].location, yystack_[0].location, (yystack_[0].value.s)._name); CMPLELEM_CHILD((yylhs.value.s), (yystack_[2].value.s)); (yylhs.value.s).setExp((yystack_[2].value.s)._u.exp->oper(&PARSE_CONTEXT, EXPR_INFO_OP_NUM, NULL, false));
                                                                                  COMP_OPERATION((yylhs.value.s)._elem, ICS_OPER_TRP, 1); }
#line 2136 "_CMPL_Parser.cc" // lalr1.cc:859
    break;

  case 104:
#line 555 "CMPL_Parser.yy" // lalr1.cc:859
    { (yylhs.value.s).init(); CMPLELEMENTRY_TXT((yylhs.value.s), SyntaxElementUnaryOp, yystack_[1].location, yystack_[0].location, (yystack_[1].value.s)._name); CMPLELEM_CHILD((yylhs.value.s), (yystack_[0].value.s)); (yylhs.value.s).setExp((yystack_[0].value.s)._u.exp->oper(&PARSE_CONTEXT, EXPR_INFO_OP_LOG, NULL, false));
																				  COMP_OPERATION((yylhs.value.s)._elem, ICS_OPER_NOT, 1); }
#line 2143 "_CMPL_Parser.cc" // lalr1.cc:859
    break;

  case 105:
#line 559 "CMPL_Parser.yy" // lalr1.cc:859
    { (yylhs.value.s).init(); CMPLELEMENTRY_TXT((yylhs.value.s), SyntaxElementBinaryOp, yystack_[2].location, yystack_[0].location, ((yystack_[1].value.s)._u.b ? "+" : "-")); CMPLELEM_CHILD2((yylhs.value.s), (yystack_[2].value.s), (yystack_[0].value.s)); (yylhs.value.s).setExp((yystack_[2].value.s)._u.exp->oper(&PARSE_CONTEXT, EXPR_INFO_OP_NUM, (yystack_[0].value.s)._u.exp, true));
		  																		  COMP_OPERATION((yylhs.value.s)._elem, ((yystack_[1].value.s)._u.b ? ICS_OPER_ADD : ICS_OPER_SUB), 2); }
#line 2150 "_CMPL_Parser.cc" // lalr1.cc:859
    break;

  case 106:
#line 561 "CMPL_Parser.yy" // lalr1.cc:859
    { (yylhs.value.s).init(); CMPLELEMENTRY_TXT((yylhs.value.s), SyntaxElementBinaryOp, yystack_[2].location, yystack_[0].location, ((yystack_[1].value.s)._u.b ? "*" : "/")); CMPLELEM_CHILD2((yylhs.value.s), (yystack_[2].value.s), (yystack_[0].value.s)); (yylhs.value.s).setExp((yystack_[2].value.s)._u.exp->oper(&PARSE_CONTEXT, EXPR_INFO_OP_NUM, (yystack_[0].value.s)._u.exp, true));
																				  COMP_OPERATION((yylhs.value.s)._elem, ((yystack_[1].value.s)._u.b ? ICS_OPER_MUL : ICS_OPER_DIV), 2); }
#line 2157 "_CMPL_Parser.cc" // lalr1.cc:859
    break;

  case 107:
#line 563 "CMPL_Parser.yy" // lalr1.cc:859
    { (yylhs.value.s).init(); CMPLELEMENTRY_TXT((yylhs.value.s), SyntaxElementBinaryOp, yystack_[2].location, yystack_[0].location, "^"); CMPLELEM_CHILD2((yylhs.value.s), (yystack_[2].value.s), (yystack_[0].value.s)); (yylhs.value.s).setExp((yystack_[2].value.s)._u.exp->oper(&PARSE_CONTEXT, EXPR_INFO_OP_NUM, (yystack_[0].value.s)._u.exp, true));
                                                                                  COMP_OPERATION((yylhs.value.s)._elem, ICS_OPER_POW, 2); }
#line 2164 "_CMPL_Parser.cc" // lalr1.cc:859
    break;

  case 108:
#line 565 "CMPL_Parser.yy" // lalr1.cc:859
    { (yylhs.value.s).init(); CMPLELEMENTRY_TXT((yylhs.value.s), SyntaxElementBinaryOp, yystack_[2].location, yystack_[0].location, (yystack_[1].value.s)._name); CMPLELEM_CHILD2((yylhs.value.s), (yystack_[2].value.s), (yystack_[0].value.s)); (yylhs.value.s).setExp((yystack_[2].value.s)._u.exp->oper(&PARSE_CONTEXT, EXPR_INFO_OP_LOG_AND, (yystack_[0].value.s)._u.exp, true));
																				  COMP_OPERATION((yylhs.value.s)._elem, ICS_OPER_AND, 2); }
#line 2171 "_CMPL_Parser.cc" // lalr1.cc:859
    break;

  case 109:
#line 567 "CMPL_Parser.yy" // lalr1.cc:859
    { (yylhs.value.s).init(); CMPLELEMENTRY_TXT((yylhs.value.s), SyntaxElementBinaryOp, yystack_[2].location, yystack_[0].location, (yystack_[1].value.s)._name); CMPLELEM_CHILD2((yylhs.value.s), (yystack_[2].value.s), (yystack_[0].value.s)); (yylhs.value.s).setExp((yystack_[2].value.s)._u.exp->oper(&PARSE_CONTEXT, EXPR_INFO_OP_LOG, (yystack_[0].value.s)._u.exp, true));
																				  COMP_OPERATION((yylhs.value.s)._elem, ICS_OPER_OR, 2); }
#line 2178 "_CMPL_Parser.cc" // lalr1.cc:859
    break;

  case 110:
#line 569 "CMPL_Parser.yy" // lalr1.cc:859
    { (yylhs.value.s).init(); CMPLELEMENTRY_TXT((yylhs.value.s), SyntaxElementBinaryOp, yystack_[2].location, yystack_[0].location, (yystack_[1].value.s)._name); CMPLELEM_CHILD2((yylhs.value.s), (yystack_[2].value.s), (yystack_[0].value.s)); (yylhs.value.s).setExp((yystack_[2].value.s)._u.exp->oper(&PARSE_CONTEXT, EXPR_INFO_OP_NUM, (yystack_[0].value.s)._u.exp, true));
																				  COMP_OPERATION((yylhs.value.s)._elem, ICS_OPER_IRR, 2); }
#line 2185 "_CMPL_Parser.cc" // lalr1.cc:859
    break;

  case 111:
#line 571 "CMPL_Parser.yy" // lalr1.cc:859
    { (yylhs.value.s).init(); CMPLELEMENTRY_TXT((yylhs.value.s), SyntaxElementBinaryOp, yystack_[2].location, yystack_[0].location, (yystack_[1].value.s)._name); CMPLELEM_CHILD2((yylhs.value.s), (yystack_[2].value.s), (yystack_[0].value.s)); (yylhs.value.s).setExp((yystack_[2].value.s)._u.exp->oper(&PARSE_CONTEXT, EXPR_INFO_OP_OBJTO, (yystack_[0].value.s)._u.exp, true));
																				  COMP_OPERATION((yylhs.value.s)._elem, ICS_OPER_OBJTO, 2); }
#line 2192 "_CMPL_Parser.cc" // lalr1.cc:859
    break;

  case 112:
#line 573 "CMPL_Parser.yy" // lalr1.cc:859
    { (yylhs.value.s).init(); CMPLELEMENTRY_TXT((yylhs.value.s), SyntaxElementBinaryOp, yystack_[1].location, yystack_[0].location, "in"); CMPLELEM_CHILD2((yylhs.value.s), (yystack_[1].value.s), (yystack_[0].value.s)); (yylhs.value.s).setExp((yystack_[1].value.s)._u.exp->oper(&PARSE_CONTEXT, EXPR_INFO_OP_IN, (yystack_[0].value.s)._u.exp, true));
																				  COMP_OPERATION((yylhs.value.s)._elem, ICS_OPER_IN, 2); }
#line 2199 "_CMPL_Parser.cc" // lalr1.cc:859
    break;

  case 113:
#line 575 "CMPL_Parser.yy" // lalr1.cc:859
    { (yylhs.value.s).init(); CMPLELEMENTRY_TXT((yylhs.value.s), SyntaxElementBinaryOp, yystack_[1].location, yystack_[0].location, "of"); CMPLELEM_CHILD2((yylhs.value.s), (yystack_[1].value.s), (yystack_[0].value.s)); (yylhs.value.s).setExp((yystack_[1].value.s)._u.exp->oper(&PARSE_CONTEXT, EXPR_INFO_OP_IN, (yystack_[0].value.s)._u.exp, true));
																				  COMP_OPERATION((yylhs.value.s)._elem, ICS_OPER_OF, 2); }
#line 2206 "_CMPL_Parser.cc" // lalr1.cc:859
    break;

  case 114:
#line 577 "CMPL_Parser.yy" // lalr1.cc:859
    { (yylhs.value.s).init(); CMPLELEMENTRY_ALT((yylhs.value.s), SyntaxElementBinaryOp, yystack_[1].location, yystack_[0].location, "", "*"); CMPLELEM_CHILD2((yylhs.value.s), (yystack_[1].value.s), (yystack_[0].value.s)); (yylhs.value.s).setExp((yystack_[1].value.s)._u.exp->oper(&PARSE_CONTEXT, EXPR_INFO_OP_NUM, (yystack_[0].value.s)._u.exp, true));
																				  COMP_OPERATION((yylhs.value.s)._elem, ICS_OPER_MUL, 2); }
#line 2213 "_CMPL_Parser.cc" // lalr1.cc:859
    break;

  case 115:
#line 581 "CMPL_Parser.yy" // lalr1.cc:859
    { (yylhs.value.s).init(); (yylhs.value.s)._u.b = true; PARSE_CONTEXT.setCbExprModeReset(); }
#line 2219 "_CMPL_Parser.cc" // lalr1.cc:859
    break;

  case 116:
#line 582 "CMPL_Parser.yy" // lalr1.cc:859
    { (yylhs.value.s).init(); (yylhs.value.s)._u.b = false; PARSE_CONTEXT.setCbExprModeReset(); }
#line 2225 "_CMPL_Parser.cc" // lalr1.cc:859
    break;

  case 117:
#line 585 "CMPL_Parser.yy" // lalr1.cc:859
    { (yylhs.value.s).init(); (yylhs.value.s)._u.b = true; PARSE_CONTEXT.setCbExprModeReset(); }
#line 2231 "_CMPL_Parser.cc" // lalr1.cc:859
    break;

  case 118:
#line 586 "CMPL_Parser.yy" // lalr1.cc:859
    { (yylhs.value.s).init(); (yylhs.value.s)._u.b = false; PARSE_CONTEXT.setCbExprModeReset(); }
#line 2237 "_CMPL_Parser.cc" // lalr1.cc:859
    break;

  case 119:
#line 589 "CMPL_Parser.yy" // lalr1.cc:859
    { (yylhs.value.s) = (yystack_[0].value.s); PARSE_CONTEXT.setCbExprModeReset(); }
#line 2243 "_CMPL_Parser.cc" // lalr1.cc:859
    break;

  case 120:
#line 592 "CMPL_Parser.yy" // lalr1.cc:859
    { (yylhs.value.s) = (yystack_[1].value.s); NOT_USED((yystack_[0].value.s)); }
#line 2249 "_CMPL_Parser.cc" // lalr1.cc:859
    break;

  case 121:
#line 593 "CMPL_Parser.yy" // lalr1.cc:859
    { (yylhs.value.s) = (yystack_[1].value.s); NOT_USED((yystack_[0].value.s)); /* $1 doesn't mean multiplication here, but is delivered so by flex, because "in" is a keyword operator */ }
#line 2255 "_CMPL_Parser.cc" // lalr1.cc:859
    break;

  case 122:
#line 596 "CMPL_Parser.yy" // lalr1.cc:859
    { (yylhs.value.s) = (yystack_[1].value.s); NOT_USED((yystack_[0].value.s)); }
#line 2261 "_CMPL_Parser.cc" // lalr1.cc:859
    break;

  case 123:
#line 597 "CMPL_Parser.yy" // lalr1.cc:859
    { (yylhs.value.s) = (yystack_[1].value.s); NOT_USED((yystack_[0].value.s)); /* $1 doesn't mean multiplication here, but is delivered so by flex, because "of" is a keyword operator */ }
#line 2267 "_CMPL_Parser.cc" // lalr1.cc:859
    break;

  case 124:
#line 600 "CMPL_Parser.yy" // lalr1.cc:859
    { (yylhs.value.s).init(); CMPLELEMENTRY_TXT((yylhs.value.s), SyntaxElementCompareOp, yystack_[2].location, yystack_[0].location, (yystack_[1].value.s)._name); CMPLELEM_CHILD2((yylhs.value.s), (yystack_[2].value.s), (yystack_[0].value.s)); (yylhs.value.s).setExp((yystack_[2].value.s)._u.exp->oper(&PARSE_CONTEXT, EXPR_INFO_OP_CMP, (yystack_[0].value.s)._u.exp, true));
		   																		  COMP_OPERATION((yylhs.value.s)._elem, ICS_OPER_EQ, 2); }
#line 2274 "_CMPL_Parser.cc" // lalr1.cc:859
    break;

  case 125:
#line 602 "CMPL_Parser.yy" // lalr1.cc:859
    { (yylhs.value.s).init(); CMPLELEMENTRY_TXT((yylhs.value.s), SyntaxElementCompareOp, yystack_[2].location, yystack_[0].location, (yystack_[1].value.s)._name); CMPLELEM_CHILD2((yylhs.value.s), (yystack_[2].value.s), (yystack_[0].value.s)); (yylhs.value.s).setExp((yystack_[2].value.s)._u.exp->oper(&PARSE_CONTEXT, EXPR_INFO_OP_CMP2, (yystack_[0].value.s)._u.exp, true));
																				  COMP_OPERATION((yylhs.value.s)._elem, ICS_OPER_EQ2, 2); }
#line 2281 "_CMPL_Parser.cc" // lalr1.cc:859
    break;

  case 126:
#line 604 "CMPL_Parser.yy" // lalr1.cc:859
    { (yylhs.value.s).init(); CMPLELEMENTRY_TXT((yylhs.value.s), SyntaxElementCompareOp, yystack_[2].location, yystack_[0].location, (yystack_[1].value.s)._name); CMPLELEM_CHILD2((yylhs.value.s), (yystack_[2].value.s), (yystack_[0].value.s)); (yylhs.value.s).setExp((yystack_[2].value.s)._u.exp->oper(&PARSE_CONTEXT, EXPR_INFO_OP_CMP, (yystack_[0].value.s)._u.exp, true));
																				  COMP_OPERATION((yylhs.value.s)._elem, ICS_OPER_NE, 2); }
#line 2288 "_CMPL_Parser.cc" // lalr1.cc:859
    break;

  case 127:
#line 606 "CMPL_Parser.yy" // lalr1.cc:859
    { (yylhs.value.s).init(); CMPLELEMENTRY_TXT((yylhs.value.s), SyntaxElementCompareOp, yystack_[2].location, yystack_[0].location, (yystack_[1].value.s)._name); CMPLELEM_CHILD2((yylhs.value.s), (yystack_[2].value.s), (yystack_[0].value.s)); (yylhs.value.s).setExp((yystack_[2].value.s)._u.exp->oper(&PARSE_CONTEXT, EXPR_INFO_OP_CMP2, (yystack_[0].value.s)._u.exp, true));
																				  COMP_OPERATION((yylhs.value.s)._elem, ICS_OPER_NE2, 2); }
#line 2295 "_CMPL_Parser.cc" // lalr1.cc:859
    break;

  case 128:
#line 608 "CMPL_Parser.yy" // lalr1.cc:859
    { (yylhs.value.s).init(); CMPLELEMENTRY_TXT((yylhs.value.s), SyntaxElementCompareOp, yystack_[2].location, yystack_[0].location, (yystack_[1].value.s)._name); CMPLELEM_CHILD2((yylhs.value.s), (yystack_[2].value.s), (yystack_[0].value.s)); (yylhs.value.s).setExp((yystack_[2].value.s)._u.exp->oper(&PARSE_CONTEXT, EXPR_INFO_OP_CMP, (yystack_[0].value.s)._u.exp, true));
																				  COMP_OPERATION((yylhs.value.s)._elem, ICS_OPER_LT, 2); }
#line 2302 "_CMPL_Parser.cc" // lalr1.cc:859
    break;

  case 129:
#line 610 "CMPL_Parser.yy" // lalr1.cc:859
    { (yylhs.value.s).init(); CMPLELEMENTRY_TXT((yylhs.value.s), SyntaxElementCompareOp, yystack_[2].location, yystack_[0].location, (yystack_[1].value.s)._name); CMPLELEM_CHILD2((yylhs.value.s), (yystack_[2].value.s), (yystack_[0].value.s)); (yylhs.value.s).setExp((yystack_[2].value.s)._u.exp->oper(&PARSE_CONTEXT, EXPR_INFO_OP_CMP, (yystack_[0].value.s)._u.exp, true));
																				  COMP_OPERATION((yylhs.value.s)._elem, ICS_OPER_GT, 2); }
#line 2309 "_CMPL_Parser.cc" // lalr1.cc:859
    break;

  case 130:
#line 612 "CMPL_Parser.yy" // lalr1.cc:859
    { (yylhs.value.s).init(); CMPLELEMENTRY_TXT((yylhs.value.s), SyntaxElementCompareOp, yystack_[2].location, yystack_[0].location, (yystack_[1].value.s)._name); CMPLELEM_CHILD2((yylhs.value.s), (yystack_[2].value.s), (yystack_[0].value.s)); (yylhs.value.s).setExp((yystack_[2].value.s)._u.exp->oper(&PARSE_CONTEXT, EXPR_INFO_OP_CMP, (yystack_[0].value.s)._u.exp, true));
																				  COMP_OPERATION((yylhs.value.s)._elem, ICS_OPER_LE, 2); }
#line 2316 "_CMPL_Parser.cc" // lalr1.cc:859
    break;

  case 131:
#line 614 "CMPL_Parser.yy" // lalr1.cc:859
    { (yylhs.value.s).init(); CMPLELEMENTRY_TXT((yylhs.value.s), SyntaxElementCompareOp, yystack_[2].location, yystack_[0].location, (yystack_[1].value.s)._name); CMPLELEM_CHILD2((yylhs.value.s), (yystack_[2].value.s), (yystack_[0].value.s)); (yylhs.value.s).setExp((yystack_[2].value.s)._u.exp->oper(&PARSE_CONTEXT, EXPR_INFO_OP_CMP, (yystack_[0].value.s)._u.exp, true));
																				  COMP_OPERATION((yylhs.value.s)._elem, ICS_OPER_GE, 2); }
#line 2323 "_CMPL_Parser.cc" // lalr1.cc:859
    break;

  case 132:
#line 619 "CMPL_Parser.yy" // lalr1.cc:859
    { (yylhs.value.s).init(); CMPLELEMENTRY_TXT((yylhs.value.s), SyntaxElementSubSymbolOp, yystack_[2].location, yystack_[0].location, "."); CMPLELEM_CHILD2((yylhs.value.s), (yystack_[2].value.s), (yystack_[0].value.s)); NOT_USED((yystack_[1].value.s));
			 																	  (yylhs.value.s).setExp((yystack_[2].value.s)._u.exp->oper(&PARSE_CONTEXT, EXPR_INFO_OP_SUB, (yystack_[0].value.s)._u.exp, true)); }
#line 2330 "_CMPL_Parser.cc" // lalr1.cc:859
    break;

  case 133:
#line 623 "CMPL_Parser.yy" // lalr1.cc:859
    { (yylhs.value.s) = (yystack_[0].value.s); }
#line 2336 "_CMPL_Parser.cc" // lalr1.cc:859
    break;

  case 134:
#line 624 "CMPL_Parser.yy" // lalr1.cc:859
    { (yylhs.value.s) = (yystack_[0].value.s); CMPLELEMENTRY_TXT((yylhs.value.s), SyntaxElementIncDec, yystack_[1].location, yystack_[0].location, ((yystack_[1].value.s)._u.i > 0 ? "prefix inc" : "prefix dec")); CMPLELEM_CHILD((yylhs.value.s), (yystack_[0].value.s));
																				  (yylhs.value.s).setExp((yystack_[0].value.s)._u.exp->oper(&PARSE_CONTEXT, EXPR_INFO_OP_INC, NULL, false));
																				  COMP_INCDEC_FETCH((yylhs.value.s)._elem, true, (yystack_[1].value.s)._u.i); }
#line 2344 "_CMPL_Parser.cc" // lalr1.cc:859
    break;

  case 135:
#line 627 "CMPL_Parser.yy" // lalr1.cc:859
    { (yylhs.value.s) = (yystack_[1].value.s); CMPLELEMENTRY_TXT((yylhs.value.s), SyntaxElementIncDec, yystack_[1].location, yystack_[0].location, ((yystack_[0].value.s)._u.i > 0 ? "postfix inc" : "postfix dec")); CMPLELEM_CHILD((yylhs.value.s), (yystack_[1].value.s));
																				  (yylhs.value.s).setExp((yystack_[1].value.s)._u.exp->oper(&PARSE_CONTEXT, EXPR_INFO_OP_INC, NULL, false));
																				  COMP_INCDEC_FETCH((yylhs.value.s)._elem, false, (yystack_[0].value.s)._u.i); }
#line 2352 "_CMPL_Parser.cc" // lalr1.cc:859
    break;

  case 136:
#line 632 "CMPL_Parser.yy" // lalr1.cc:859
    { (yylhs.value.s) = (yystack_[0].value.s); CMPLELEMENTRY_SYM((yylhs.value.s), SyntaxElementSymbol, yystack_[0].location, yystack_[0].location, (yystack_[0].value.s)._u.exp->sym());
																				  COMP_FETCH_SYM((yylhs.value.s)._elem, (yystack_[0].value.s)._u.exp); (yylhs.value.s)._u.exp->setLvalueSyntaxElem((yylhs.value.s)._elem); }
#line 2359 "_CMPL_Parser.cc" // lalr1.cc:859
    break;

  case 137:
#line 637 "CMPL_Parser.yy" // lalr1.cc:859
    { (yylhs.value.s).init(); CMPLELEMENTRY((yylhs.value.s), SyntaxElementFuncCall, yystack_[1].location, yystack_[0].location); CMPLELEM_CHILD2((yylhs.value.s), (yystack_[1].value.s), (yystack_[0].value.s)); /* auch als Set-Konstruktor der Art 1(1)(n-1) */
			 																	  (yylhs.value.s).setExp((yystack_[1].value.s)._u.exp->oper(&PARSE_CONTEXT, EXPR_INFO_OP_CALL, (yystack_[0].value.s)._u.exp, true));
																				  COMP_OPERATION((yylhs.value.s)._elem, ICS_OPER_FUNC, 2); }
#line 2367 "_CMPL_Parser.cc" // lalr1.cc:859
    break;

  case 138:
#line 640 "CMPL_Parser.yy" // lalr1.cc:859
    { (yylhs.value.s).init();
																				  if (PARSE_CONTEXT.modpt()->compatLineNamePref() && (yystack_[1].value.s)._u.exp->sym() && (yystack_[1].value.s)._u.exp->sym()->notDefined()) {
																					// no function call, $1 is a line name prefix
																					COMP_LNP_COMPAT((yystack_[1].value.s)._elem, (yystack_[1].value.s)._u.exp);
																					delete (yystack_[1].value.s)._u.exp; (yylhs.value.s) = (yystack_[0].value.s); (yylhs.value.s)._u.exp->setHasLNP(yystack_[1].location);
																					CMPLELEMENTRY((yylhs.value.s), SyntaxElementLineNamePrefix, yystack_[1].location, yystack_[1].location); CMPLELEM_CHILD((yylhs.value.s), (yystack_[1].value.s)); CMPLELEM_NEXT((yylhs.value.s), (yystack_[0].value.s));
																				  } else {
																				  	CMPLELEMENTRY((yylhs.value.s), SyntaxElementFuncCall, yystack_[1].location, yystack_[0].location); CMPLELEM_CHILD2((yylhs.value.s), (yystack_[1].value.s), (yystack_[0].value.s));
																				  	(yylhs.value.s).setExp((yystack_[1].value.s)._u.exp->oper(&PARSE_CONTEXT, EXPR_INFO_OP_CALL, (yystack_[0].value.s)._u.exp, true));
																					COMP_OPERATION((yylhs.value.s)._elem, ICS_OPER_FUNC, 2);
																				} }
#line 2383 "_CMPL_Parser.cc" // lalr1.cc:859
    break;

  case 139:
#line 653 "CMPL_Parser.yy" // lalr1.cc:859
    { (yylhs.value.s).init(); CMPLELEMENTRY((yylhs.value.s), SyntaxElementIndexation, yystack_[1].location, yystack_[0].location); CMPLELEM_CHILD2((yylhs.value.s), (yystack_[1].value.s), (yystack_[0].value.s));
																				  (yylhs.value.s).setExp((yystack_[1].value.s)._u.exp->oper(&PARSE_CONTEXT, EXPR_INFO_OP_IND, (yystack_[0].value.s)._u.exp, true));
																				  COMP_OPERATION((yylhs.value.s)._elem, ICS_OPER_INDEX, 2); }
#line 2391 "_CMPL_Parser.cc" // lalr1.cc:859
    break;

  case 140:
#line 658 "CMPL_Parser.yy" // lalr1.cc:859
    { (yylhs.value.s).init(); NOT_USED((yystack_[1].value.s));
		   																		  if ((yystack_[2].value.s)._u.exp->expTypeTuple()) {
																					CMPLELEMENTRY((yylhs.value.s), SyntaxElementArrayCast, yystack_[2].location, yystack_[0].location); CMPLELEM_CHILD2((yylhs.value.s), (yystack_[2].value.s), (yystack_[0].value.s));
																					(yylhs.value.s).setExp((yystack_[2].value.s)._u.exp->oper(&PARSE_CONTEXT, EXPR_INFO_OP_ARRC, (yystack_[0].value.s)._u.exp, true));
																					COMP_OPERATION((yylhs.value.s)._elem, ICS_OPER_ARRCAST, 2);
																				  } else {
																				  	if ((yystack_[0].value.s)._u.exp->expTypeTuple()) {
																						// indexation
																						CMPLELEMENTRY((yylhs.value.s), SyntaxElementIndexation, yystack_[2].location, yystack_[0].location); CMPLELEM_CHILD2((yylhs.value.s), (yystack_[2].value.s), (yystack_[0].value.s));
																						(yylhs.value.s).setExp((yystack_[2].value.s)._u.exp->oper(&PARSE_CONTEXT, EXPR_INFO_OP_IND, (yystack_[0].value.s)._u.exp, true));
																						COMP_OPERATION((yylhs.value.s)._elem, ICS_OPER_INDEX, 2);
																					} else if ((yystack_[0].value.s)._u.exp->expTypeArray()) {
																						// function call
																						CMPLELEMENTRY((yylhs.value.s), SyntaxElementFuncCall, yystack_[2].location, yystack_[0].location); CMPLELEM_CHILD2((yylhs.value.s), (yystack_[2].value.s), (yystack_[0].value.s));
																						(yylhs.value.s).setExp((yystack_[2].value.s)._u.exp->oper(&PARSE_CONTEXT, EXPR_INFO_OP_CALL, (yystack_[0].value.s)._u.exp, true));
																						COMP_OPERATION((yylhs.value.s)._elem, ICS_OPER_FUNC, 2);
																					} else {
																						// error
																						CMPLELEMENTRY_TXT((yylhs.value.s), SyntaxElementError, yystack_[2].location, yystack_[0].location, "illegal expression"); CMPLELEM_CHILD2((yylhs.value.s), (yystack_[2].value.s), (yystack_[0].value.s));
																						(yylhs.value.s).setExp((yystack_[2].value.s)._u.exp->oper(&PARSE_CONTEXT, EXPR_INFO_OP_NUM, (yystack_[0].value.s)._u.exp, true));
																					}
																				} }
#line 2418 "_CMPL_Parser.cc" // lalr1.cc:859
    break;

  case 141:
#line 682 "CMPL_Parser.yy" // lalr1.cc:859
    { (yylhs.value.s).init(); CMPLELEMENTRY_TXT((yylhs.value.s), SyntaxElementError, yystack_[1].location, yystack_[0].location, "illegal expression"); CMPLELEM_CHILD2((yylhs.value.s), (yystack_[1].value.s), (yystack_[0].value.s)); (yylhs.value.s).setExp((yystack_[1].value.s)._u.exp->oper(&PARSE_CONTEXT, EXPR_INFO_OP_NUM, (yystack_[0].value.s)._u.exp, true)); }
#line 2424 "_CMPL_Parser.cc" // lalr1.cc:859
    break;

  case 142:
#line 683 "CMPL_Parser.yy" // lalr1.cc:859
    { (yylhs.value.s).init(); CMPLELEMENTRY_TXT((yylhs.value.s), SyntaxElementError, yystack_[1].location, yystack_[0].location, "illegal expression"); CMPLELEM_CHILD2((yylhs.value.s), (yystack_[1].value.s), (yystack_[0].value.s)); (yylhs.value.s).setExp((yystack_[1].value.s)._u.exp->oper(&PARSE_CONTEXT, EXPR_INFO_OP_NUM, (yystack_[0].value.s)._u.exp, true)); }
#line 2430 "_CMPL_Parser.cc" // lalr1.cc:859
    break;


#line 2434 "_CMPL_Parser.cc" // lalr1.cc:859
            default:
              break;
            }
        }
      catch (const syntax_error& yyexc)
        {
          error (yyexc);
          YYERROR;
        }
      YY_SYMBOL_PRINT ("-> $$ =", yylhs);
      yypop_ (yylen);
      yylen = 0;
      YY_STACK_PRINT ();

      // Shift the result of the reduction.
      yypush_ (YY_NULLPTR, yylhs);
    }
    goto yynewstate;

  /*--------------------------------------.
  | yyerrlab -- here on detecting error.  |
  `--------------------------------------*/
  yyerrlab:
    // If not already recovering from an error, report this error.
    if (!yyerrstatus_)
      {
        ++yynerrs_;
        error (yyla.location, yysyntax_error_ (yystack_[0].state, yyla));
      }


    yyerror_range[1].location = yyla.location;
    if (yyerrstatus_ == 3)
      {
        /* If just tried and failed to reuse lookahead token after an
           error, discard it.  */

        // Return failure if at end of input.
        if (yyla.type_get () == yyeof_)
          YYABORT;
        else if (!yyla.empty ())
          {
            yy_destroy_ ("Error: discarding", yyla);
            yyla.clear ();
          }
      }

    // Else will try to reuse lookahead token after shifting the error token.
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
    yyerror_range[1].location = yystack_[yylen - 1].location;
    /* Do not reclaim the symbols of the rule whose action triggered
       this YYERROR.  */
    yypop_ (yylen);
    yylen = 0;
    goto yyerrlab1;

  /*-------------------------------------------------------------.
  | yyerrlab1 -- common code for both syntax error and YYERROR.  |
  `-------------------------------------------------------------*/
  yyerrlab1:
    yyerrstatus_ = 3;   // Each real token shifted decrements this.
    {
      stack_symbol_type error_token;
      for (;;)
        {
          yyn = yypact_[yystack_[0].state];
          if (!yy_pact_value_is_default_ (yyn))
            {
              yyn += yyterror_;
              if (0 <= yyn && yyn <= yylast_ && yycheck_[yyn] == yyterror_)
                {
                  yyn = yytable_[yyn];
                  if (0 < yyn)
                    break;
                }
            }

          // Pop the current state because it cannot handle the error token.
          if (yystack_.size () == 1)
            YYABORT;

          yyerror_range[1].location = yystack_[0].location;
          yy_destroy_ ("Error: popping", yystack_[0]);
          yypop_ ();
          YY_STACK_PRINT ();
        }

      yyerror_range[2].location = yyla.location;
      YYLLOC_DEFAULT (error_token.location, yyerror_range, 2);

      // Shift the error token.
      error_token.state = yyn;
      yypush_ ("Shifting", error_token);
    }
    goto yynewstate;

    // Accept.
  yyacceptlab:
    yyresult = 0;
    goto yyreturn;

    // Abort.
  yyabortlab:
    yyresult = 1;
    goto yyreturn;

  yyreturn:
    if (!yyla.empty ())
      yy_destroy_ ("Cleanup: discarding lookahead", yyla);

    /* Do not reclaim the symbols of the rule whose action triggered
       this YYABORT or YYACCEPT.  */
    yypop_ (yylen);
    while (1 < yystack_.size ())
      {
        yy_destroy_ ("Cleanup: popping", yystack_[0]);
        yypop_ ();
      }

    return yyresult;
  }
    catch (...)
      {
        YYCDEBUG << "Exception caught: cleaning lookahead and stack"
                 << std::endl;
        // Do not try to display the values of the reclaimed symbols,
        // as their printer might throw an exception.
        if (!yyla.empty ())
          yy_destroy_ (YY_NULLPTR, yyla);

        while (1 < yystack_.size ())
          {
            yy_destroy_ (YY_NULLPTR, yystack_[0]);
            yypop_ ();
          }
        throw;
      }
  }

  void
  CMPL_Parser::error (const syntax_error& yyexc)
  {
    error (yyexc.location, yyexc.what());
  }

  // Generate an error message.
  std::string
  CMPL_Parser::yysyntax_error_ (state_type yystate, const symbol_type& yyla) const
  {
    // Number of reported tokens (one for the "unexpected", one per
    // "expected").
    size_t yycount = 0;
    // Its maximum.
    enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
    // Arguments of yyformat.
    char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];

    /* There are many possibilities here to consider:
       - If this state is a consistent state with a default action, then
         the only way this function was invoked is if the default action
         is an error action.  In that case, don't check for expected
         tokens because there are none.
       - The only way there can be no lookahead present (in yyla) is
         if this state is a consistent state with a default action.
         Thus, detecting the absence of a lookahead is sufficient to
         determine that there is no unexpected or expected token to
         report.  In that case, just report a simple "syntax error".
       - Don't assume there isn't a lookahead just because this state is
         a consistent state with a default action.  There might have
         been a previous inconsistent state, consistent state with a
         non-default action, or user semantic action that manipulated
         yyla.  (However, yyla is currently not documented for users.)
       - Of course, the expected token list depends on states to have
         correct lookahead information, and it depends on the parser not
         to perform extra reductions after fetching a lookahead from the
         scanner and before detecting a syntax error.  Thus, state
         merging (from LALR or IELR) and default reductions corrupt the
         expected token list.  However, the list is correct for
         canonical LR with one exception: it will still contain any
         token that will not be accepted due to an error action in a
         later state.
    */
    if (!yyla.empty ())
      {
        int yytoken = yyla.type_get ();
        yyarg[yycount++] = yytname_[yytoken];
        int yyn = yypact_[yystate];
        if (!yy_pact_value_is_default_ (yyn))
          {
            /* Start YYX at -YYN if negative to avoid negative indexes in
               YYCHECK.  In other words, skip the first -YYN actions for
               this state because they are default actions.  */
            int yyxbegin = yyn < 0 ? -yyn : 0;
            // Stay within bounds of both yycheck and yytname.
            int yychecklim = yylast_ - yyn + 1;
            int yyxend = yychecklim < yyntokens_ ? yychecklim : yyntokens_;
            for (int yyx = yyxbegin; yyx < yyxend; ++yyx)
              if (yycheck_[yyx + yyn] == yyx && yyx != yyterror_
                  && !yy_table_value_is_error_ (yytable_[yyx + yyn]))
                {
                  if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
                    {
                      yycount = 1;
                      break;
                    }
                  else
                    yyarg[yycount++] = yytname_[yyx];
                }
          }
      }

    char const* yyformat = YY_NULLPTR;
    switch (yycount)
      {
#define YYCASE_(N, S)                         \
        case N:                               \
          yyformat = S;                       \
        break
        YYCASE_(0, YY_("syntax error"));
        YYCASE_(1, YY_("syntax error, unexpected %s"));
        YYCASE_(2, YY_("syntax error, unexpected %s, expecting %s"));
        YYCASE_(3, YY_("syntax error, unexpected %s, expecting %s or %s"));
        YYCASE_(4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
        YYCASE_(5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
#undef YYCASE_
      }

    std::string yyres;
    // Argument number.
    size_t yyi = 0;
    for (char const* yyp = yyformat; *yyp; ++yyp)
      if (yyp[0] == '%' && yyp[1] == 's' && yyi < yycount)
        {
          yyres += yytnamerr_ (yyarg[yyi++]);
          ++yyp;
        }
      else
        yyres += *yyp;
    return yyres;
  }


  const signed char CMPL_Parser::yypact_ninf_ = -65;

  const signed char CMPL_Parser::yytable_ninf_ = -90;

  const short int
  CMPL_Parser::yypact_[] =
  {
     544,   -56,   -65,   -65,   -65,   -65,   -65,     6,   -65,   495,
     -65,   798,    19,   -65,   -65,   -65,   -65,   -65,   -65,   -65,
     -65,   -65,   -65,   -65,   -65,   -65,   -65,   -65,   -65,   -65,
     -65,   -65,   -65,   -36,   -65,   -65,   -65,   -51,   864,   -65,
     587,     8,   782,   -65,    99,     5,   -65,   841,   -65,   451,
     -65,   630,   -65,  1001,   -65,   -65,   -65,   916,  1001,   916,
     916,   916,   -65,   -65,   -65,   -65,   -65,   -65,   -65,   -65,
     -65,   -65,   -65,    11,   996,    -6,   -65,    -4,   -32,   673,
     -21,    13,   -65,   -36,   916,   916,   916,   916,   916,   916,
     916,   916,   916,   916,   -65,   -65,   916,   -65,   916,   893,
     -19,   916,   -65,   112,   -65,  1001,   916,   916,   -65,   -65,
     -65,   -65,   -65,   -65,   -65,   -65,   -65,   -65,   -65,   -65,
     -65,   397,    -1,   996,    21,     7,    12,    30,   132,   132,
     132,   132,   742,   742,   -65,   -65,   -65,   -65,    13,   -65,
     798,   -65,    34,    35,   916,   -65,   -65,  1051,  1077,    -5,
      -5,    -5,    -5,    -5,    -5,    -5,    -5,   397,    77,   -65,
       4,   -65,    48,   -65,    53,   -65,   -65,   -65,   -65,   132,
     366,   397,   -65,   -65,   916,   -65,   -65,   -65,   726,   -65,
     -21,   -65,   968,   -65,   -65,   -65,   -65,   916,   996
  };

  const unsigned char
  CMPL_Parser::yydefact_[] =
  {
       0,     0,    13,    14,    15,    10,    11,     0,     2,     0,
       5,     0,     8,    12,     6,    18,     1,     4,   115,   116,
     117,   118,    95,   119,    83,    86,    35,    77,    76,    78,
      80,    79,    73,     0,     7,    53,    16,     0,     0,    67,
       0,     0,    74,    54,    70,    20,    55,     0,    56,     0,
      57,     0,    58,    94,    59,    60,    61,     0,   100,     0,
       0,     0,    62,    63,    64,    65,    66,    69,     9,    71,
      17,    25,    19,    23,    22,     0,    43,     0,    36,     0,
      48,    41,    68,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   120,   122,     0,   102,     0,     0,
       0,     0,   138,   137,   139,    92,     0,     0,    72,   141,
     142,    26,    27,    28,    29,    30,    31,    32,    21,   121,
     123,   114,     0,    90,     0,    87,     0,     0,    93,    99,
     101,   104,   112,   113,    24,    34,    33,    38,    41,    50,
       0,    39,     0,    44,     0,    42,   111,   109,   108,   124,
     125,   126,   127,   128,   129,   130,   131,   110,   140,   103,
     107,   136,     0,   132,   133,    75,    98,    97,    96,    91,
     105,   106,    82,    81,    89,    85,    84,    37,    74,    40,
      48,    47,    51,   134,   135,    88,    45,     0,    52
  };

  const short int
  CMPL_Parser::yypgoto_[] =
  {
     -65,   -65,    -8,   -65,   -65,    -9,   -65,    70,   -65,   -65,
     -65,   -65,   -65,   -65,   -30,   -65,   -29,   -65,   -65,   -65,
      51,   -65,   -64,   -65,   -65,   -65,   -65,   -65,   -11,   -65,
     -31,    33,   -40,   -65,   103,   -65,    41,   -65,   -43,   -65,
     -65,   165,   -65,   -65,   -65,   -65,   227,   289,   -65,   -65,
     -65,   -65,   -65,   -65,   -27,   -65,   -65,   -65,   -65
  };

  const short int
  CMPL_Parser::yydefgoto_[] =
  {
      -1,     7,     8,     9,    10,    11,    12,    13,    14,    37,
      38,    72,    73,   118,    39,    40,    77,   138,    78,    79,
      80,    81,   142,   143,   144,   141,   181,    41,   123,    43,
      44,    45,    46,    47,    48,    49,    50,    51,   124,   125,
      52,    53,    54,   168,    55,    56,    57,    58,    59,    60,
      61,    62,    63,   163,   164,    64,    65,    66,    67
  };

  const short int
  CMPL_Parser::yytable_[] =
  {
      42,    17,    69,    68,   110,    15,    16,    32,   127,   111,
      70,    82,   102,   109,    94,    95,    18,    19,    20,    21,
      96,    97,    98,    99,    22,   161,   137,    74,    24,    25,
      26,   100,    99,    22,   -46,   162,   121,    24,    25,    26,
     100,   -46,   128,    26,   102,    25,   129,   130,   131,   132,
     133,   135,   146,   136,   112,   113,   114,   115,   116,     2,
     117,    76,   172,   167,     3,     4,     5,     6,   175,   174,
     140,   139,   166,   147,   148,   149,   150,   151,   152,   153,
     154,   155,   156,   104,   173,   157,   176,   158,   160,   179,
      42,   102,   161,   102,   169,   170,   171,   180,   102,   102,
     102,   102,   102,   102,    98,    99,    22,   184,    71,   177,
      24,    25,    26,   100,   134,   104,   186,   102,   102,   102,
     102,   102,   102,   102,   102,   102,   102,   102,   102,   178,
     102,   185,   145,   182,   165,   183,    27,    28,    29,   102,
     102,   102,    32,     0,     0,   103,     0,     0,   102,    27,
      28,    29,   102,   108,     0,    32,     0,     0,   102,     0,
       0,     0,   104,     0,   104,    24,    25,    26,   100,   104,
     104,   104,   104,   104,   104,     0,   188,   103,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   104,   104,
     104,   104,   104,   104,   104,   104,   104,   104,   104,   104,
       0,   104,     0,     0,     0,     0,     0,   105,     0,     0,
     104,   104,   104,     0,     0,     0,     0,     0,     0,   104,
       0,     0,     0,   104,   103,     0,   103,     0,     0,   104,
       0,   103,   103,   103,   103,   103,   103,     0,     0,   105,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     103,   103,   103,   103,   103,   103,   103,   103,   103,   103,
     103,   103,     0,   103,     0,     0,     0,     0,     0,   106,
       0,     0,   103,   103,   103,     0,     0,     0,     0,     0,
       0,   103,     0,     0,     0,   103,   105,     0,   105,     0,
       0,   103,     0,   105,   105,   105,   105,   105,   105,     0,
       0,   106,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   105,   105,   105,   105,   105,   105,   105,   105,
     105,   105,   105,   105,     0,   105,     0,     0,     0,     0,
       0,   107,     0,     0,   105,   105,   105,     0,     0,     0,
       0,     0,     0,   105,     0,     0,     0,   105,   106,     0,
     106,     0,     0,   105,     0,   106,   106,   106,   106,   106,
     106,     0,     0,   107,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   106,   106,   106,   106,   106,   106,
     106,   106,   106,   106,   106,   106,     0,   106,     0,    20,
      21,    96,    97,    98,    99,    22,   106,   106,   106,    24,
      25,    26,   100,     0,     0,   106,     0,     0,     0,   106,
     107,     0,   107,     0,     0,   106,     0,   107,   107,   107,
     107,   107,   107,    97,    98,    99,    22,     0,     0,     0,
      24,    25,    26,   100,     0,     0,   107,   107,   107,   107,
     107,   107,   107,   107,   107,   107,   107,   107,     0,   107,
       0,     0,   122,     0,     0,     0,     0,     0,   107,   107,
     107,     0,     0,     0,     0,     0,     0,   107,     0,     0,
       0,   107,    18,    19,    20,    21,     0,   107,     0,     0,
      22,     0,     0,    23,    24,    25,    26,     0,    27,    28,
      29,     0,    30,    31,    32,    -3,     1,     0,     0,     0,
       0,     0,     0,     0,     0,    33,     0,     0,     0,     0,
      35,     0,     0,   -89,   -89,     0,    -8,    -8,    -8,    -8,
       0,     0,     0,     0,    -8,     0,     0,    -8,    -8,    -8,
      -8,     0,    -8,    -8,    -8,     2,    -8,    -8,    -8,     0,
       3,     4,     5,     6,    -3,     1,     0,     0,     0,    -8,
      -8,     0,    -3,    -3,    -8,     0,    -8,     0,     0,     0,
       0,     0,     0,     0,     0,    -8,    -8,    -8,    -8,     0,
       0,     0,     0,    -8,     0,     0,    -8,    -8,    -8,    -8,
       0,    -8,    -8,    -8,     2,    -8,    -8,    -8,    75,     3,
       4,     5,     6,     0,     0,     0,     0,     0,    -8,    -8,
       0,     0,     0,    -8,     0,    -8,     0,     0,   -41,   -41,
     -41,   -41,     0,     0,     0,     0,   -41,     0,     0,   -41,
     -41,   -41,   -41,     0,   -41,   -41,   -41,     0,   -41,   -41,
     -41,   126,     0,     0,     0,    76,     0,     0,     0,     0,
       0,   -41,   -41,     0,     0,     0,   -41,     0,     0,   -41,
       0,    18,    19,    20,    21,     0,     0,     0,     0,    22,
       0,     0,    23,    24,    25,    26,     0,    27,    28,    29,
       0,    30,    31,    32,     1,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    33,     0,   -89,     0,     0,    35,
       0,     0,   -89,     0,    -8,    -8,    -8,    -8,     0,     0,
       0,     0,    -8,     0,     0,    -8,    -8,    -8,    -8,     0,
      -8,    -8,    -8,     2,    -8,    -8,    -8,     0,     3,     4,
       5,     6,     0,     0,     0,     0,     0,    -8,    -8,    83,
      -3,    -3,    -8,     0,    -8,    84,    85,    86,    87,    88,
      89,    90,    91,    92,    93,    94,    95,    18,    19,    20,
      21,    96,    97,    98,    99,    22,     0,     0,     0,    24,
      25,    26,   100,    18,    19,    20,    21,    96,    97,    98,
      99,    22,     0,     0,     0,    24,    25,    26,   100,     0,
       0,     0,     0,   -49,   -49,    83,     0,     0,   101,     0,
       0,    84,    85,    86,    87,    88,    89,    90,    91,    92,
      93,    94,    95,    18,    19,    20,    21,    96,    97,    98,
      99,    22,     0,     0,     0,    24,    25,    26,   100,    18,
      19,    20,    21,     0,     0,     0,     0,    22,     0,     0,
      23,    24,    25,    26,     0,    27,    28,    29,     0,    30,
      31,    32,     0,     0,   101,     0,     0,     0,     0,     0,
       0,     0,    33,    34,     0,     0,     0,    35,     0,    36,
     119,   120,    18,    19,    20,    21,     0,     0,     0,     0,
      22,     0,     0,    23,    24,    25,    26,     0,    27,    28,
      29,     0,    30,    31,    32,    18,    19,    20,    21,     0,
       0,     0,     0,    22,     0,    33,    23,    24,    25,    26,
      35,    27,    28,    29,     0,    30,    31,    32,     0,     3,
       4,     0,     0,     0,    18,    19,    20,    21,    33,   159,
       0,     0,    22,    35,     0,    23,    24,    25,    26,     0,
      27,    28,    29,     0,    30,    31,    32,    18,    19,    20,
      21,     0,     0,     0,     0,    22,     0,    33,    23,    24,
      25,    26,    35,    27,    28,    29,     0,    30,    31,    32,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      33,    83,   187,     0,     0,    35,     0,    84,    85,    86,
      87,    88,    89,    90,    91,    92,    93,    94,    95,    18,
      19,    20,    21,    96,    97,    98,    99,    22,     0,    83,
       0,    24,    25,    26,   100,    84,    85,    86,    87,    88,
      89,    90,    91,    92,    93,    94,    95,    18,    19,    20,
      21,    96,    97,    98,    99,    22,     0,     0,     0,    24,
      25,    26,   100,    23,    24,    25,    26,     0,    27,    28,
      29,     0,    30,    31,    32,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    33,     0,     0,     0,     0,
      35,    85,    86,    87,    88,    89,    90,    91,    92,    93,
      94,    95,    18,    19,    20,    21,    96,    97,    98,    99,
      22,     0,     0,     0,    24,    25,    26,   100,    86,    87,
      88,    89,    90,    91,    92,    93,    94,    95,    18,    19,
      20,    21,    96,    97,    98,    99,    22,     0,     0,     0,
      24,    25,    26,   100
  };

  const short int
  CMPL_Parser::yycheck_[] =
  {
      11,     9,    33,    12,    44,    61,     0,    43,    51,     4,
      61,    41,    42,    44,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    44,    58,    38,    33,    34,
      35,    36,    28,    29,    55,    54,    47,    33,    34,    35,
      36,    62,    53,    35,    74,    34,    57,    58,    59,    60,
      61,    57,    83,    57,    49,    50,    51,    52,    53,    40,
      55,    48,    63,   103,    45,    46,    47,    48,    56,    62,
      79,    79,   103,    84,    85,    86,    87,    88,    89,    90,
      91,    92,    93,    42,    63,    96,    56,    98,    99,    55,
     101,   121,    44,   123,   105,   106,   107,    62,   128,   129,
     130,   131,   132,   133,    27,    28,    29,    54,    38,   138,
      33,    34,    35,    36,    73,    74,   180,   147,   148,   149,
     150,   151,   152,   153,   154,   155,   156,   157,   158,   140,
     160,   174,    81,   144,   101,   162,    37,    38,    39,   169,
     170,   171,    43,    -1,    -1,    42,    -1,    -1,   178,    37,
      38,    39,   182,    54,    -1,    43,    -1,    -1,   188,    -1,
      -1,    -1,   121,    -1,   123,    33,    34,    35,    36,   128,
     129,   130,   131,   132,   133,    -1,   187,    74,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   147,   148,
     149,   150,   151,   152,   153,   154,   155,   156,   157,   158,
      -1,   160,    -1,    -1,    -1,    -1,    -1,    42,    -1,    -1,
     169,   170,   171,    -1,    -1,    -1,    -1,    -1,    -1,   178,
      -1,    -1,    -1,   182,   121,    -1,   123,    -1,    -1,   188,
      -1,   128,   129,   130,   131,   132,   133,    -1,    -1,    74,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     147,   148,   149,   150,   151,   152,   153,   154,   155,   156,
     157,   158,    -1,   160,    -1,    -1,    -1,    -1,    -1,    42,
      -1,    -1,   169,   170,   171,    -1,    -1,    -1,    -1,    -1,
      -1,   178,    -1,    -1,    -1,   182,   121,    -1,   123,    -1,
      -1,   188,    -1,   128,   129,   130,   131,   132,   133,    -1,
      -1,    74,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   147,   148,   149,   150,   151,   152,   153,   154,
     155,   156,   157,   158,    -1,   160,    -1,    -1,    -1,    -1,
      -1,    42,    -1,    -1,   169,   170,   171,    -1,    -1,    -1,
      -1,    -1,    -1,   178,    -1,    -1,    -1,   182,   121,    -1,
     123,    -1,    -1,   188,    -1,   128,   129,   130,   131,   132,
     133,    -1,    -1,    74,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   147,   148,   149,   150,   151,   152,
     153,   154,   155,   156,   157,   158,    -1,   160,    -1,    23,
      24,    25,    26,    27,    28,    29,   169,   170,   171,    33,
      34,    35,    36,    -1,    -1,   178,    -1,    -1,    -1,   182,
     121,    -1,   123,    -1,    -1,   188,    -1,   128,   129,   130,
     131,   132,   133,    26,    27,    28,    29,    -1,    -1,    -1,
      33,    34,    35,    36,    -1,    -1,   147,   148,   149,   150,
     151,   152,   153,   154,   155,   156,   157,   158,    -1,   160,
      -1,    -1,     1,    -1,    -1,    -1,    -1,    -1,   169,   170,
     171,    -1,    -1,    -1,    -1,    -1,    -1,   178,    -1,    -1,
      -1,   182,    21,    22,    23,    24,    -1,   188,    -1,    -1,
      29,    -1,    -1,    32,    33,    34,    35,    -1,    37,    38,
      39,    -1,    41,    42,    43,     0,     1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    54,    -1,    -1,    -1,    -1,
      59,    -1,    -1,    62,    63,    -1,    21,    22,    23,    24,
      -1,    -1,    -1,    -1,    29,    -1,    -1,    32,    33,    34,
      35,    -1,    37,    38,    39,    40,    41,    42,    43,    -1,
      45,    46,    47,    48,     0,     1,    -1,    -1,    -1,    54,
      55,    -1,    57,    58,    59,    -1,    61,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    21,    22,    23,    24,    -1,
      -1,    -1,    -1,    29,    -1,    -1,    32,    33,    34,    35,
      -1,    37,    38,    39,    40,    41,    42,    43,     1,    45,
      46,    47,    48,    -1,    -1,    -1,    -1,    -1,    54,    55,
      -1,    -1,    -1,    59,    -1,    61,    -1,    -1,    21,    22,
      23,    24,    -1,    -1,    -1,    -1,    29,    -1,    -1,    32,
      33,    34,    35,    -1,    37,    38,    39,    -1,    41,    42,
      43,     1,    -1,    -1,    -1,    48,    -1,    -1,    -1,    -1,
      -1,    54,    55,    -1,    -1,    -1,    59,    -1,    -1,    62,
      -1,    21,    22,    23,    24,    -1,    -1,    -1,    -1,    29,
      -1,    -1,    32,    33,    34,    35,    -1,    37,    38,    39,
      -1,    41,    42,    43,     1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    54,    -1,    56,    -1,    -1,    59,
      -1,    -1,    62,    -1,    21,    22,    23,    24,    -1,    -1,
      -1,    -1,    29,    -1,    -1,    32,    33,    34,    35,    -1,
      37,    38,    39,    40,    41,    42,    43,    -1,    45,    46,
      47,    48,    -1,    -1,    -1,    -1,    -1,    54,    55,     3,
      57,    58,    59,    -1,    61,     9,    10,    11,    12,    13,
      14,    15,    16,    17,    18,    19,    20,    21,    22,    23,
      24,    25,    26,    27,    28,    29,    -1,    -1,    -1,    33,
      34,    35,    36,    21,    22,    23,    24,    25,    26,    27,
      28,    29,    -1,    -1,    -1,    33,    34,    35,    36,    -1,
      -1,    -1,    -1,    57,    58,     3,    -1,    -1,    62,    -1,
      -1,     9,    10,    11,    12,    13,    14,    15,    16,    17,
      18,    19,    20,    21,    22,    23,    24,    25,    26,    27,
      28,    29,    -1,    -1,    -1,    33,    34,    35,    36,    21,
      22,    23,    24,    -1,    -1,    -1,    -1,    29,    -1,    -1,
      32,    33,    34,    35,    -1,    37,    38,    39,    -1,    41,
      42,    43,    -1,    -1,    62,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    54,    55,    -1,    -1,    -1,    59,    -1,    61,
      19,    20,    21,    22,    23,    24,    -1,    -1,    -1,    -1,
      29,    -1,    -1,    32,    33,    34,    35,    -1,    37,    38,
      39,    -1,    41,    42,    43,    21,    22,    23,    24,    -1,
      -1,    -1,    -1,    29,    -1,    54,    32,    33,    34,    35,
      59,    37,    38,    39,    -1,    41,    42,    43,    -1,    45,
      46,    -1,    -1,    -1,    21,    22,    23,    24,    54,    26,
      -1,    -1,    29,    59,    -1,    32,    33,    34,    35,    -1,
      37,    38,    39,    -1,    41,    42,    43,    21,    22,    23,
      24,    -1,    -1,    -1,    -1,    29,    -1,    54,    32,    33,
      34,    35,    59,    37,    38,    39,    -1,    41,    42,    43,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      54,     3,     4,    -1,    -1,    59,    -1,     9,    10,    11,
      12,    13,    14,    15,    16,    17,    18,    19,    20,    21,
      22,    23,    24,    25,    26,    27,    28,    29,    -1,     3,
      -1,    33,    34,    35,    36,     9,    10,    11,    12,    13,
      14,    15,    16,    17,    18,    19,    20,    21,    22,    23,
      24,    25,    26,    27,    28,    29,    -1,    -1,    -1,    33,
      34,    35,    36,    32,    33,    34,    35,    -1,    37,    38,
      39,    -1,    41,    42,    43,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    54,    -1,    -1,    -1,    -1,
      59,    10,    11,    12,    13,    14,    15,    16,    17,    18,
      19,    20,    21,    22,    23,    24,    25,    26,    27,    28,
      29,    -1,    -1,    -1,    33,    34,    35,    36,    11,    12,
      13,    14,    15,    16,    17,    18,    19,    20,    21,    22,
      23,    24,    25,    26,    27,    28,    29,    -1,    -1,    -1,
      33,    34,    35,    36
  };

  const unsigned char
  CMPL_Parser::yystos_[] =
  {
       0,     1,    40,    45,    46,    47,    48,    65,    66,    67,
      68,    69,    70,    71,    72,    61,     0,    66,    21,    22,
      23,    24,    29,    32,    33,    34,    35,    37,    38,    39,
      41,    42,    43,    54,    55,    59,    61,    73,    74,    78,
      79,    91,    92,    93,    94,    95,    96,    97,    98,    99,
     100,   101,   104,   105,   106,   108,   109,   110,   111,   112,
     113,   114,   115,   116,   119,   120,   121,   122,    69,    94,
      61,    71,    75,    76,    92,     1,    48,    80,    82,    83,
      84,    85,    78,     3,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    25,    26,    27,    28,
      36,    62,    78,    98,   100,   105,   110,   111,    54,    94,
      96,     4,    49,    50,    51,    52,    53,    55,    77,    19,
      20,    92,     1,    92,   102,   103,     1,   102,    92,    92,
      92,    92,    92,    92,   100,    57,    57,    58,    81,    66,
      69,    89,    86,    87,    88,    84,    94,    92,    92,    92,
      92,    92,    92,    92,    92,    92,    92,    92,    92,    26,
      92,    44,    54,   117,   118,    95,    94,    96,   107,    92,
      92,    92,    63,    63,    62,    56,    56,    80,    92,    55,
      62,    90,    92,   118,    54,   102,    86,     4,    92
  };

  const unsigned char
  CMPL_Parser::yyr1_[] =
  {
       0,    64,    65,    66,    66,    67,    67,    68,    69,    69,
      70,    70,    70,    70,    71,    71,    72,    72,    72,    73,
      73,    74,    75,    75,    75,    76,    77,    77,    77,    77,
      77,    77,    77,    78,    78,    79,    80,    80,    81,    82,
      83,    84,    84,    85,    86,    86,    87,    87,    88,    89,
      89,    90,    90,    91,    92,    92,    92,    92,    92,    92,
      92,    92,    92,    92,    92,    92,    92,    92,    92,    92,
      93,    93,    93,    94,    95,    95,    96,    96,    96,    97,
      97,    98,    98,    99,   100,   100,   101,   102,   102,   103,
     103,   104,   104,   104,   104,   105,   106,   107,   107,   108,
     108,   108,   108,   108,   108,   109,   109,   109,   109,   109,
     109,   109,   109,   109,   109,   110,   110,   111,   111,   112,
     113,   113,   114,   114,   115,   115,   115,   115,   115,   115,
     115,   115,   116,   117,   117,   117,   118,   119,   119,   120,
     121,   122,   122
  };

  const unsigned char
  CMPL_Parser::yyr2_[] =
  {
       0,     2,     1,     0,     2,     1,     1,     2,     0,     2,
       1,     1,     1,     1,     1,     1,     2,     3,     2,     2,
       1,     2,     1,     1,     2,     1,     1,     1,     1,     1,
       1,     1,     1,     3,     3,     1,     1,     3,     1,     2,
       3,     0,     2,     1,     1,     3,     0,     2,     0,     2,
       1,     1,     3,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     2,     1,
       1,     2,     2,     1,     1,     3,     1,     1,     1,     1,
       1,     3,     3,     1,     3,     3,     1,     1,     3,     0,
       1,     3,     2,     2,     1,     1,     3,     1,     1,     2,
       1,     2,     2,     3,     2,     3,     3,     3,     3,     3,
       3,     3,     2,     2,     2,     1,     1,     1,     1,     1,
       2,     2,     2,     2,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     1,     2,     2,     1,     2,     2,     2,
       3,     2,     2
  };



  // YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
  // First, the terminals, then, starting at \a yyntokens_, nonterminals.
  const char*
  const CMPL_Parser::yytname_[] =
  {
  "\"end of file\"", "error", "$undefined", "\"operator `->`\"",
  "\"assign operator `:=`\"", "ASSIGN_ADD", "ASSIGN_SUB", "ASSIGN_MUL",
  "ASSIGN_DIV", "\"operator `||`\"", "\"operator `&&`\"",
  "\"operator `=`\"", "\"operator `==`\"", "\"operator `<>`\"",
  "\"operator `!=`\"", "\"operator `<`\"", "\"operator `>`\"",
  "\"operator `<=`\"", "\"operator `>=`\"", "\"operator `in`\"",
  "\"operator `of`\"", "'+'", "'-'", "'*'", "'/'", "\"operator `*>`\"",
  "\"transpose operator `T`\"", "\"array cast operation\"", "'^'",
  "\"operator `..`\"", "NUMBER_SIGN", "INDEX_SIGN", "\"operator `!`\"",
  "'('", "'['", "\"`{`\"", "\"subsymbol operator `.`\"", "\"int literal\"",
  "\"double literal\"", "\"string literal\"", "\"linename prefix\"",
  "\"int literal factor\"", "\"double literal factor\"", "\"symbol\"",
  "\"subsymbol\"", "\"object type symbol\"", "\"data type symbol\"",
  "\"assign modificator\"", "\"order modificator\"",
  "\"assign operator `::=`\"", "\"assign operator `+=`\"",
  "\"assign operator `-=`\"", "\"assign operator `*=`\"",
  "\"assign operator `/=`\"", "\"increment/decrement operator\"",
  "\"`:`\"", "\"`]`\"", "\"`}`\"", "\"`|`\"", "\"`&`\"", "\"`@`\"",
  "\"`;`\"", "','", "')'", "$accept", "cmpl", "cmplParts", "cmplPart",
  "directive", "directiveParts", "directivePart", "objDataType",
  "statement", "statementExpr", "assignLhsOp", "assignRhs",
  "objDataTypeRhs", "assignOp", "codeblock", "blockStart", "blockParts",
  "blockAltStart", "blockPart", "blockPartStart", "blockModifiers",
  "blockModifier", "blockHeaders", "blockHeader", "blockHeaderStart",
  "blockBody", "expressionWithCodeblockSymbolDef", "refcodeblock",
  "expression", "symbolIncDec", "symbol", "expressionList", "literal",
  "literalMult", "arrayExpr", "arrayStart", "tupleExpr", "tupleStart",
  "listParts", "listPart", "intervalExpr", "intervalOp", "setExpr",
  "setExprEnd", "unaryOp", "binaryOp", "numOpSignAdd", "numOpSignMult",
  "logOpSignNot", "inOpPre", "ofOpPre", "compareOp", "subsymbolOp",
  "subsymbolIncDec", "subsymbol", "functionCall", "indexation",
  "arraycast", "errorExpression", YY_NULLPTR
  };

#if YYDEBUG
  const unsigned short int
  CMPL_Parser::yyrline_[] =
  {
       0,   241,   241,   244,   245,   248,   249,   252,   255,   256,
     259,   260,   261,   263,   267,   268,   276,   280,   283,   287,
     294,   309,   314,   315,   316,   320,   323,   324,   325,   326,
     327,   328,   329,   336,   340,   347,   351,   352,   355,   358,
     361,   364,   365,   368,   371,   375,   381,   382,   385,   388,
     396,   399,   400,   406,   412,   413,   414,   415,   416,   417,
     418,   419,   420,   421,   422,   423,   424,   425,   426,   430,
     434,   435,   439,   445,   450,   451,   454,   456,   458,   462,
     465,   471,   474,   479,   482,   485,   490,   493,   497,   503,
     504,   507,   510,   513,   516,   523,   526,   532,   533,   540,
     542,   544,   551,   553,   555,   559,   561,   563,   565,   567,
     569,   571,   573,   575,   577,   581,   582,   585,   586,   589,
     592,   593,   596,   597,   600,   602,   604,   606,   608,   610,
     612,   614,   619,   623,   624,   627,   632,   637,   640,   653,
     658,   682,   683
  };

  // Print the state stack on the debug stream.
  void
  CMPL_Parser::yystack_print_ ()
  {
    *yycdebug_ << "Stack now";
    for (stack_type::const_iterator
           i = yystack_.begin (),
           i_end = yystack_.end ();
         i != i_end; ++i)
      *yycdebug_ << ' ' << i->state;
    *yycdebug_ << std::endl;
  }

  // Report on the debug stream that the rule \a yyrule is going to be reduced.
  void
  CMPL_Parser::yy_reduce_print_ (int yyrule)
  {
    unsigned int yylno = yyrline_[yyrule];
    int yynrhs = yyr2_[yyrule];
    // Print the symbols being reduced, and their result.
    *yycdebug_ << "Reducing stack by rule " << yyrule - 1
               << " (line " << yylno << "):" << std::endl;
    // The symbols being reduced.
    for (int yyi = 0; yyi < yynrhs; yyi++)
      YY_SYMBOL_PRINT ("   $" << yyi + 1 << " =",
                       yystack_[(yynrhs) - (yyi + 1)]);
  }
#endif // YYDEBUG

  // Symbol number corresponding to token number t.
  inline
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
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
      33,    63,    23,    21,    62,    22,     2,    24,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    34,     2,     2,    28,     2,     2,     2,     2,     2,
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
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    25,    26,    27,    29,
      30,    31,    32,    35,    36,    37,    38,    39,    40,    41,
      42,    43,    44,    45,    46,    47,    48,    49,    50,    51,
      52,    53,    54,    55,    56,    57,    58,    59,    60,    61
    };
    const unsigned int user_token_number_max_ = 309;
    const token_number_type undef_token_ = 2;

    if (static_cast<int>(t) <= yyeof_)
      return yyeof_;
    else if (static_cast<unsigned int> (t) <= user_token_number_max_)
      return translate_table[t];
    else
      return undef_token_;
  }


} // cmpl
#line 3206 "_CMPL_Parser.cc" // lalr1.cc:1167
#line 686 "CMPL_Parser.yy" // lalr1.cc:1168


void cmpl::CMPL_Parser::error(const cmpl::CMPL_Parser::location_type& l, const std::string& m)
{
	PARSE_CONTEXT.outParseError(ERROR_LVL_NORMAL, m, l, true);
}


