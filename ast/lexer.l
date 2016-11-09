%option yylineno
%pointer

%{
    #include <stdlib.h>
    #include <string.h>
    
    #include "ast.hpp"
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
"|"  { return ABS; }
"{"  { return LBRACE; }
"}"  { return RBRACE; }
"("  { return LP; }
")"  { return RP; }
"["  { return LBRAKET; }
"]"  { return RBRAKET; }

true  { yylval.u_base_int = 1; return TRUE; }
false { yylval.u_base_int = 0; return FALSE; }

[1-9][0-9]*  		{ yylval.u_base_int = atoi(yytext); 			return INT; }
[0]  				{ yylval.u_base_int = atoi(yytext); 			return INT; }   /* 0 */
[0][x|X][0-9A-F]+ 	{ yylval.u_base_int = strtol(yytext, NULL, 16); return INT; }   /* hex */
[0][0-7]+ 			{ yylval.u_base_int = strtol(yytext, NULL, 8);  return INT; } 	/* octal */
[0-1]+[b] 			{ yylval.u_base_int = strtol(yytext, NULL, 2);  return INT; } 	/* binary */

[a-zA-Z][a-zA-Z0-9_]*  { yylval.u_base_charptr = strdup(yytext); return ID;} /* identifier */

[\"].*[\"]  { 
	char* cha = strdup(yytext);
	cha += 1;
	cha[strlen(cha)-1] = '\0';
	yylval.u_base_charptr = cha;  
	return STR; 
}		/* string */ 

"'"[a-zA-Z]"'"  { yylval.u_base_int = int(yytext[1]); return C; }		/* character */ 


[ \t\n]             ; /* skip whitespace */

"/"\%([^%]|(\%+[^%/]))*\%+"/"   ;   /* skip comments */

.                   yyerror((char *) "invalid character");



%%

/** You shall not pass!
 *  You should not  have to do or edit anything past this.
 */

int yywrap(void) {
    return 1;
}
