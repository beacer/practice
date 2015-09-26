/* English -> American */
%%
"colour" {printf("color");}
"flavour" {printf("flavor");}
. {printf("%s", yytext);} 
%%
