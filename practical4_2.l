%{
#include <stdio.h>
%}

%%
[cC][hH][aA][rR][uU][sS][aA][tT]   { printf("university"); }
\n                               { printf("\n"); return 0; }
.                                { printf("%s", yytext); }
%%

int main()
{
    yylex();
    return 0;
}

int yywrap()
{
    return 1;
}