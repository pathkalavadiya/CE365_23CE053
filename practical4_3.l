%option noyywrap

%{
#include <stdio.h>

int characters = 0;
int words = 0;
int lines = 0;
%}

%%

\n              { lines++; characters++; }

[ \t]+          { characters += yyleng; }

[^ \t\n]+       { words++; characters += yyleng; }

%%

int main()
{
   
    yylex();

    printf("\nOutput:\n");
    printf("Characters : %d\n", characters);
    printf("Words : %d\n", words);
    printf("Lines : %d\n", lines);

    return 0;
}
