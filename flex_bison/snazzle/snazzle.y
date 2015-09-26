%{
#include <stdio.h>
#include <stdlib.h>

extern int yylex();
extern int yyparse();
extern FILE *yyin;
extern int line_num;

void yyerror(const char *s);

%}

%union {
    int ival;
    float fval;
    char *sval;
}

%token SNAZZLE TYPE
%token END ENDL
%token <ival> INT
%token <fval> FLOAT
%token <sval> STRING

%%
snazzle     :   header template body_section footer { printf("done with a snazzle file!\n"); }
            ;

header      :   SNAZZLE FLOAT ENDLS { printf("reading a snazzle file version: %f\n", $2); }
            ;

template    :   typelines
            ;

typelines   :   typelines typeline
            |   typeline
            ;

typeline    :   TYPE STRING ENDLS { printf("new defined snazzle type: %s\n", $2); }
            ;

body_section:   body_lines
            ;

body_lines  :   body_lines body_line
            |   body_line
            ;

body_line   :   INT INT INT INT STRING ENDLS { printf("new snazzle: %d %d %d %d %s\n", $1, $2, $3, $4, $5); }
            ;

footer      :   END ENDLS
            ;

ENDLS       :   ENDLS ENDL
            |   ENDL
            ;

%%

main()
{
    //do {
        yyparse();
    //} while (!feof(yyin));
}

void yyerror(const char *s)
{
    printf("EEK, parse error on line %d ! Message: %s\n", line_num, s);
    exit(-1);
}
