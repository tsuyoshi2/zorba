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

namespace zorba
{
  class position;
  class location;
}

/* First part of user declarations.  */
#line 38 "/home/muresan/Projects/zorba/src/compiler/parser/xquery_parser.y"


#include "common/common.h"

#ifdef _WIN32
#pragma warning(disable: 4786)
#endif

#include <string>

#include "compiler/parsetree/parsenodes.h"
#include "compiler/parser/parse_constants.h"
#include "compiler/api/compilercb.h"
#include "store/api/update_consts.h"

#define SYMTAB( n ) driver.symtab.get ((off_t) n)
#define LOC( p ) driver.createQueryLoc(p)

namespace zorba 
{

namespace parser 
{
  extern const char *the_tumbling, *the_sliding, *the_start, *the_end, *the_only_end, *the_ofor, *the_declare, *the_create;
}

class xquery_driver;


class FunctionSig {
public:
  rchandle<ParamList> param;
  rchandle<SequenceType> ret;

  FunctionSig (ParamList *param_, SequenceType *ret_ = NULL)
    : param (param_), ret (ret_)
  {}
};

class VarNameAndType {
public:
  std::string name;
  rchandle<TypeDeclaration> type;


  VarNameAndType (std::string name_, rchandle<TypeDeclaration> type_)
    : name (name_), type (type_)
  {}
};

}

#define YYDEBUG 1



/* Line 35 of lalr1.cc.  */
#line 110 "/home/muresan/Projects/zorba/test/zorbatest/build/src/compiler/parser/xquery_parser.hpp"

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

namespace zorba
{

  /// A Bison parser.
  class xquery_parser
  {
  public:
    /// Symbol semantic values.
#ifndef YYSTYPE
    union semantic_type
#line 138 "/home/muresan/Projects/zorba/src/compiler/parser/xquery_parser.y"
{
  zorba::parsenode * node;
  zorba::exprnode * expr;
  zorba::FunctionSig *fnsig;
  zorba::VarNameAndType *varnametype;
  off_t sval;
  const char *strval;
	xqp_integer* ival;
	xqp_double* dval;
	xqp_decimal* decval;
  XQUERY_ERROR err;
}
/* Line 35 of lalr1.cc.  */
#line 175 "/home/muresan/Projects/zorba/test/zorbatest/build/src/compiler/parser/xquery_parser.hpp"
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
     _EOF = 0,
     UNRECOGNIZED = 258,
     APOS_ATTR_CONTENT = 259,
     CHAR_LITERAL = 260,
     CHAR_LITERAL_AND_CDATA_END = 261,
     CHAR_LITERAL_AND_PI_END = 262,
     CHAR_REF_LITERAL = 263,
     ELEMENT_CONTENT = 264,
     ELEM_WILDCARD = 265,
     ENTITY_REF = 266,
     EXPR_COMMENT_LITERAL = 267,
     PI_NCNAME_LBRACE = 268,
     PI_TARGET_LITERAL = 269,
     PRAGMA_LITERAL_AND_END_PRAGMA = 270,
     QNAME_SVAL_AND_END_PRAGMA = 271,
     PREFIX_WILDCARD = 272,
     QNAME_SVAL = 273,
     QUOTE_ATTR_CONTENT = 274,
     STRING_LITERAL = 275,
     XML_COMMENT_LITERAL = 276,
     BLANK = 280,
     BLOCK = 281,
     EXIT = 282,
     BREAK = 283,
     LOOP = 284,
     CONTINUE = 285,
     WHILE = 286,
     VALIDATE = 287,
     TYPESWITCH = 288,
     ELEMENT = 289,
     DOCUMENT = 290,
     TEXT = 291,
     COMMENT = 292,
     DECLARE = 293,
     FUNCTION = 294,
     UPDATING = 295,
     SEQUENTIAL = 296,
     DETERMINISTIC = 297,
     NONDETERMINISTIC = 298,
     SIMPLE = 299,
     IF = 300,
     PROCESSING_INSTRUCTION = 301,
     MOST = 302,
     IMPORT = 303,
     SOME = 304,
     STABLE = 305,
     MODULE = 306,
     OPTION = 307,
     WORD = 308,
     SCHEMA = 309,
     SPACE = 310,
     SET = 311,
     LET = 312,
     CONSTRUCTION = 313,
     EVAL = 314,
     FOR = 315,
     OUTER = 316,
     SLIDING = 317,
     TUMBLING = 318,
     PREVIOUS = 319,
     NEXT = 320,
     ONLY = 321,
     WHEN = 322,
     COUNT = 323,
     ORDERING = 324,
     CONT = 325,
     BASE_URI = 326,
     SCHEMA_ELEMENT = 327,
     DOCUMENT_NODE = 328,
     COPY_NAMESPACES = 329,
     INSTANCE = 330,
     SCHEMA_ATTRIBUTE = 331,
     BOUNDARY_SPACE = 332,
     ANCESTOR_AXIS = 333,
     ANCESTOR_OR_SELF_AXIS = 334,
     AND = 335,
     APOS = 336,
     AS = 337,
     ASCENDING = 338,
     AT = 339,
     ATTRIBUTE = 340,
     ATTRIBUTE_AXIS = 341,
     AT_SIGN = 342,
     CASE = 343,
     CASTABLE = 344,
     CAST = 345,
     CDATA_BEGIN = 346,
     CDATA_END = 347,
     CHILD_AXIS = 348,
     COLLATION = 349,
     COMMA = 350,
     COMMENT_BEGIN = 351,
     COMMENT_END = 352,
     DECIMAL_LITERAL = 353,
     CONTEXT = 354,
     VARIABLE = 355,
     DEFAULT = 356,
     DESCENDANT_AXIS = 357,
     DESCENDANT_OR_SELF_AXIS = 358,
     DESCENDING = 359,
     DIV = 360,
     DOLLAR = 361,
     DOT = 362,
     DOT_DOT = 363,
     DOUBLE_LBRACE = 364,
     DOUBLE_LITERAL = 365,
     DOUBLE_RBRACE = 366,
     ELSE = 367,
     _EMPTY = 368,
     GREATEST = 369,
     LEAST = 370,
     EMPTY_TAG_END = 371,
     ENCODING = 372,
     EQUALS = 373,
     ESCAPE_APOS = 374,
     ESCAPE_QUOTE = 375,
     EVERY = 376,
     EXCEPT = 377,
     EXTERNAL = 378,
     FOLLOWING_AXIS = 379,
     FOLLOWING_SIBLING_AXIS = 380,
     FOLLOWS = 381,
     GE = 382,
     GETS = 383,
     GT = 384,
     HOOK = 385,
     IDIV = 386,
     _IN = 387,
     INHERIT = 388,
     INTEGER_LITERAL = 389,
     INTERSECT = 390,
     IS = 391,
     ITEM = 392,
     LBRACE = 393,
     LBRACK = 394,
     LE = 395,
     LPAR = 396,
     LT_OR_START_TAG = 397,
     MINUS = 398,
     MOD = 399,
     NAMESPACE = 400,
     _NAN = 401,
     NE = 402,
     NODECOMP = 403,
     NOT_OPERATOR_KEYWORD = 404,
     NO_INHERIT = 405,
     NO_PRESERVE = 406,
     OR = 407,
     ORDERED = 408,
     ORDER = 409,
     BY = 410,
     GROUP = 411,
     PARENT_AXIS = 412,
     PI_BEGIN = 413,
     PI_END = 414,
     PLUS = 415,
     PRAGMA_BEGIN = 416,
     PRAGMA_END = 417,
     PRECEDES = 418,
     PRECEDING_AXIS = 419,
     PRECEDING_SIBLING_AXIS = 420,
     PRESERVE = 421,
     QUOTE = 422,
     RBRACE = 423,
     RBRACK = 424,
     RETURN = 425,
     RPAR = 426,
     SATISFIES = 427,
     SCHEMA_ATTRIBUTE_LPAR = 428,
     SELF_AXIS = 429,
     SEMI = 430,
     SLASH = 431,
     SLASH_SLASH = 432,
     STAR = 433,
     START_TAG_END = 434,
     STRIP = 435,
     TAG_END = 436,
     THEN = 437,
     TO = 438,
     TREAT = 439,
     UNION = 440,
     UNORDERED = 441,
     VAL_EQ = 442,
     VAL_GE = 443,
     VAL_GT = 444,
     VAL_LE = 445,
     VAL_LT = 446,
     VAL_NE = 447,
     VALUECOMP = 448,
     VBAR = 449,
     EMPTY_SEQUENCE = 450,
     WHERE = 451,
     XML_COMMENT_BEGIN = 452,
     XML_COMMENT_END = 453,
     XQUERY = 454,
     VERSION = 455,
     END_PRAGMA = 456,
     START = 457,
     AFTER = 458,
     BEFORE = 459,
     REVALIDATION = 460,
     _STRICT = 461,
     LAX = 462,
     SKIP = 463,
     _DELETE = 464,
     NODE = 465,
     INSERT = 466,
     NODES = 467,
     RENAME = 468,
     REPLACE = 469,
     VALUE = 470,
     OF = 471,
     FIRST = 472,
     INTO = 473,
     LAST = 474,
     MODIFY = 475,
     COPY = 476,
     WITH = 477,
     TRY = 478,
     CATCH = 479,
     USING = 480,
     ALL = 481,
     ANY = 482,
     SENSITIVE = 483,
     FT_OPTION = 484,
     DIACRITICS = 485,
     INSENSITIVE = 486,
     DIFFERENT = 487,
     DISTANCE = 488,
     ENTIRE = 489,
     END = 490,
     CONTENT = 491,
     EXACTLY = 492,
     FROM = 493,
     FTAND = 494,
     FTCONTAINS = 495,
     FTNOT = 496,
     FTOR = 497,
     LANGUAGE = 498,
     LEVELS = 499,
     LOWERCASE = 500,
     OCCURS = 501,
     PARAGRAPH = 502,
     PHRASE = 503,
     RELATIONSHIP = 504,
     SAME = 505,
     SCORE = 506,
     SENTENCE = 507,
     SENTENCES = 508,
     TIMES = 509,
     UPPERCASE = 510,
     WEIGHT = 511,
     WINDOW = 512,
     WITHOUT = 513,
     STEMMING = 514,
     STOP = 515,
     WORDS = 516,
     THESAURUS = 517,
     WILDCARDS = 518,
     CREATE = 519,
     DROP = 520,
     UNIQUE = 521,
     INDEX = 522,
     HASH = 523,
     BTREE = 524,
     ON = 525,
     BYTE_ORDER_MARK_UTF8 = 526,
     FTCONTAINS_REDUCE = 527,
     RANGE_REDUCE = 528,
     ADDITIVE_REDUCE = 529,
     MULTIPLICATIVE_REDUCE = 530,
     UNION_REDUCE = 531,
     INTERSECT_EXCEPT_REDUCE = 532,
     QVARINDECLLIST_REDUCE = 533,
     COMMA_DOLLAR = 534,
     UNARY_PREC = 535,
     SEQUENCE_TYPE_REDUCE = 536,
     OCCURS_STAR = 537,
     OCCURS_PLUS = 538,
     OCCURS_HOOK = 539,
     STEP_REDUCE = 540
   };

    };
    /// Token type.
    typedef token::yytokentype token_type;

    /// Build a parser object.
    xquery_parser (xquery_driver& driver_yyarg);
    virtual ~xquery_parser ();

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
    typedef unsigned short int token_number_type;
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
    static const unsigned short int yytable_[];
    static const signed char yytable_ninf_;

    static const short int yycheck_[];

    /// For a state, its accessing symbol.
    static const unsigned short int yystos_[];

    /// For a rule, its LHS.
    static const unsigned short int yyr1_[];
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
    xquery_driver& driver;
  };
}


#endif /* ! defined PARSER_HEADER_H */
