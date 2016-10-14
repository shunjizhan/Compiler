
%{
	#include <stdio.h>
	int yylex(void);
	void yyerror(char *);
%}


%%


S    :    F R {printf("parsed expresion\n");}
       | 'E'
       ;

R    :    '+' F R 
        | '-' F R
        | Z
        ;

Z    :    '*' F R
        | '/' F R
        | ',' S
        ;

F    :  'n' 
        | '(' F A ')'
        ;

A   :   '+' F A
        | '-' F A
        | B

B   :   '*' F A
        | '/' F A
|

%%

void yyerror(char *s) {
	fprintf(stderr, "%s\n", s);
	return;
}

int main(void) {
	yyparse();
	return 0;
}
