/* simplest version of calculator */
%{
#include <stdio.h>
%}

/* declare tokens */
%token NUMBER
%token ADD SUB MUL DIV ABS
%token EOL
%token OP CP

/*
1 * 2 + 3 * 4 + |5
=   =   =   =   == terms
-----   -----   -- factors
:::::::::::::   :: exps
^^^^^^^^^^^^^^^^^^ calclist
*/

%%

calclist    : /* nothing */
            | calclist exp EOL { printf("= %d\n", $2); }
            ;

exp         : factor
            | exp ADD factor { $$ = $1 + $3; printf("EXP = %d\n", $$);}
            | exp SUB factor { $$ = $1 - $3; printf("EXP = %d\n", $$);}
            ;

factor      : term
            | factor MUL term { $$ = $1 * $3; printf("FACTOR = %d\n", $$);}
            | factor DIV term { $$ = $1 / $3; printf("FACTOR = %d\n", $$);}
            ;

term        : NUMBER
            | ABS term  { $$ = $2 >= 0 ? $2 : - $2; printf("TERM = %d\n", $$);}
            | OP exp CP { $$ = $2; }
            ;

%%

main(int argc, char *argv[])
{
    yyparse();
}

yyerror(char *s)
{
    fprintf(stderr, "error: %s\n", s);
}
