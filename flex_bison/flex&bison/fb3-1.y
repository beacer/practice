/* calculator with AST */
%{
#include <stdio.h>
#include <stdlib.h>
#include "fb3-1.h"
%}

/* the symbol 'value' return as $$ , default is integers.
   now the value returned could be either 'struct ast point' or 'double' */
%union {
    struct ast *a;
    double d;
}

/* declare tokens */
%token <d> NUMBER    /* %token <TYPE> NAME,
                        has the value <d> to hold the value of number
                        */
%token EOL

%type <a> exp factor term /* assign value <a> to exp, factor and term */

%%

calclist: /* nothing */
    | calclist exp EOL {
            printf("= %4.4g\n", eval($2));
            treefree($2);
            printf("> ");
        }
    | calclist EOL { printf("> "); }
    ;

exp: factor
    | exp '+' factor { $$ = newast('+', $1, $3); }
    | exp '-' factor { $$ = newast('+', $1, $3); }
    ;
