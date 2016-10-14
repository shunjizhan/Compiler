
%{
	#include <stdio.h>
	int yylex(void);
	void yyerror(char *);
%}


%%


S   :   E '.'    {printf("parsed expresion\n");}
	|	E '.' S  {printf("parsed expresion\n");}
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
