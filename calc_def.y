
%{
	#include <stdio.h>
	int yylex(void);
	void yyerror(char *);
%}


%%

S 	:	L {printf("parsed expresion\n");}
	;

L   :   E '.' K 
	;

K	:	S
	|
	;

E 	:	T	Ee
	;

Ee 	:	'+' T Ee
	|	'-' T Ee
	|	
	;

T	:	F Tt
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
