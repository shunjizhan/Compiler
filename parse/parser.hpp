/* A Bison parser, made by GNU Bison 2.3.  */

/* Skeleton interface for Bison's Yacc-like parsers in C

   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.

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

/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     BOOLEAN = 258,
     CHAR = 259,
     INTEGER = 260,
     STRING = 261,
     INTPTR = 262,
     CHARPTR = 263,
     IF = 264,
     ELSE = 265,
     WHILE = 266,
     VAR = 267,
     PROCEDURE = 268,
     RETURN = 269,
     ANDAND = 270,
     SLASH = 271,
     EQUAL = 272,
     EQEQ = 273,
     BIGER = 274,
     BIGERE = 275,
     LESS = 276,
     LESSE = 277,
     MINUS = 278,
     NOT = 279,
     NOTEQUAL = 280,
     OROR = 281,
     PLUS = 282,
     TIMES = 283,
     ADDRESS = 284,
     DEREF = 285,
     SEMI = 286,
     COLON = 287,
     COMMA = 288,
     OR = 289,
     LBRACE = 290,
     RBRACE = 291,
     LP = 292,
     RP = 293,
     LBRAKET = 294,
     RBRAKET = 295,
     C = 296,
     STR = 297,
     TRUE = 298,
     FALSE = 299,
     INT = 300,
     ID = 301,
     CL = 302,
     CR = 303,
     BIGGERE = 304,
     BIGGER = 305,
     DIVIDE = 306
   };
#endif
/* Tokens.  */
#define BOOLEAN 258
#define CHAR 259
#define INTEGER 260
#define STRING 261
#define INTPTR 262
#define CHARPTR 263
#define IF 264
#define ELSE 265
#define WHILE 266
#define VAR 267
#define PROCEDURE 268
#define RETURN 269
#define ANDAND 270
#define SLASH 271
#define EQUAL 272
#define EQEQ 273
#define BIGER 274
#define BIGERE 275
#define LESS 276
#define LESSE 277
#define MINUS 278
#define NOT 279
#define NOTEQUAL 280
#define OROR 281
#define PLUS 282
#define TIMES 283
#define ADDRESS 284
#define DEREF 285
#define SEMI 286
#define COLON 287
#define COMMA 288
#define OR 289
#define LBRACE 290
#define RBRACE 291
#define LP 292
#define RP 293
#define LBRAKET 294
#define RBRAKET 295
#define C 296
#define STR 297
#define TRUE 298
#define FALSE 299
#define INT 300
#define ID 301
#define CL 302
#define CR 303
#define BIGGERE 304
#define BIGGER 305
#define DIVIDE 306




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef int YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif

extern YYSTYPE yylval;

