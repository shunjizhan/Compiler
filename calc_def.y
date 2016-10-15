
%{
	#include <stdio.h>
	int yylex(void);
	void yyerror(char *);
%}


%%

S 	:	L {printf("parsed expresion\n");}
	;

L   :   '(' E ')' Tt Ee '.' L
	|	'n' Tt Ee '.' L
	|
	;

E 	:	'(' E ')' Tt Ee
	|	'n' Tt Ee
	;

Ee 	:	'+' T Ee
	|	'-' T Ee
	|	
	;

T	:	'(' E ')' Tt
	|	'n' Tt
	;

Tt	:	'*'	F Tt
	|	'%' F Tt
	|	
	;

F 	:	'(' E ')'
	|	'n'
	;



%%

void yyerror(char *s) {
	fprintf(stderr, "%s\n", s);
	return;
}

int main(void) {
	yyparse();
	return 0;
}
