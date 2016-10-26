%option yylineno
%pointer

%{
    #include <stdlib.h>
    #include <string.h>
    #include "parser.hpp"

    void yyerror(const char *);
%}

/** WRITE ME:sudo apt-get install bison
 *  Put any definitions you need here. Add your own regular expressions
 *  below for ALL tokens defined in the manual. You can choose not to use
 *  definitions here.
 */

/** The example below is very simple, but it shows almost everything you will
 *  need to do for this first part. The syntax is as follows:
 *
 *  regular_expression      {  c code  }
 *
 * The lexer will try to match each regular expression. When it does it will
 * execute the corresponding C code.
*/

%%

boolean  { return BOOLEAN; }
char  { return CHAR; }  
integer  { return INTEGER; }
string  { return STRING; }
intptr  { return INTPTR; }
charptr  { return CHARPTR; }
if  { return IF; }
else  { return ELSE; }
while  { return WHILE; }
var  { return VAR; }
procedure  { return PROCEDURE; }
return  { return RETURN; }

"&&"  { return ANDAND; }
"/"  { return DIVIDE; }
"="  { return EQUAL; }
"=="  { return EQEQ; }
">"  { return BIGER; }
">="  { return  BIGERE; }
"<"  { return LESS; }
"<="  { return LESSE; }
"-"  { return MINUS; }
"!"  { return NOT; }
"!="  { return NOTEQUAL; }
"||"  { return OROR; }
"+"  { return PLUS; }
"*"  { return TIMES; }
"&"  { return ADDRESS; }
"^"  { return DEREF; }

";"  { return SEMI; }
":"  { return COLON; }
","  { return COMMA; }
"|"  { return OR; }
"{"  { return LBRACE; }
"}"  { return RBRACE; }
"("  { return LP; }
")"  { return RP; }
"["  { return LBRAKET; }
"]"  { return RBRAKET; }

true  { return TRUE; }
false  { return FALSE; }

[1-9][0-9]*  { return INT; }
[0]  { return INT; } 			   /* 0 */
[0][x|X][0-9A-F]+  { return INT; } /* hex */
[0][0-7]+  { return INT; } 	       /* octal */
[0-1]+[b]  { return INT; } 	   /* binary */

[a-zA-Z][a-zA-Z0-9_]*  { return ID; } /* identifier */

[a-zA-Z]  { return C; }		/* character */ /*???????????*/

[ \t\n]             ; /* skip whitespace */
[/%][0-9a-zA-Z]*[%/]			;	/* comments */


.                   yyerror((char *) "invalid character");



%%

/** You shall not pass!
 *  You should not  have to do or edit anything past this.
 */

int yywrap(void) {
    return 1;
}
