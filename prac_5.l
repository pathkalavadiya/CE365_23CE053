%option noyywrap

%{
#include <stdio.h>
#include <string.h>

#define MAX_SYMBOLS 100

int line = 1;

char symbolTable[MAX_SYMBOLS][50];
int symbolCount = 0;

/* keyword check */
int isKeyword(char *word)
{
    char *keywords[] = {
        "int","char","float","double","void","return",
        "if","else","for","while","do","break","continue",
        "struct","long","short","unsigned","signed"
    };

    int n = sizeof(keywords)/sizeof(keywords[0]);
    int i;

    for(i=0;i<n;i++)
    {
        if(strcmp(word,keywords[i])==0)
            return 1;
    }
    return 0;
}

/* add symbol */
void addSymbol(char *word)
{
    int i;

    for(i=0;i<symbolCount;i++)
    {
        if(strcmp(symbolTable[i],word)==0)
            return;
    }

    strcpy(symbolTable[symbolCount],word);
    symbolCount++;
}
%}

/* patterns */

LETTER   [a-zA-Z]
DIGIT    [0-9]
ID       {LETTER}({LETTER}|{DIGIT})*
NUMBER   {DIGIT}+
CHAR     \'[^\']\'
STRING   \"[^\"]*\"

%%

"//".*             { }

[ \t]+             { }

\n                 { line++; }

{ID}               {
                     if(isKeyword(yytext))
                         printf("Keyword: %s\n", yytext);
                     else
                     {
                         printf("Identifier: %s\n", yytext);
                         addSymbol(yytext);
                     }
                   }

{NUMBER}           {
                     printf("Constant: %s\n", yytext);
                   }

{CHAR}             {
                     printf("Constant: %s\n", yytext);
                   }

{STRING}           {
                     printf("String: %s\n", yytext);
                   }

"+"                { printf("Operator: %s\n", yytext); }
"-"                { printf("Operator: %s\n", yytext); }
"*"                { printf("Operator: %s\n", yytext); }
"/"                { printf("Operator: %s\n", yytext); }
"="                { printf("Operator: %s\n", yytext); }
"<"                { printf("Operator: %s\n", yytext); }
">"                { printf("Operator: %s\n", yytext); }

";"                { printf("Punctuation: %s\n", yytext); }
","                { printf("Punctuation: %s\n", yytext); }
"("                { printf("Punctuation: %s\n", yytext); }
")"                { printf("Punctuation: %s\n", yytext); }
"{"                { printf("Punctuation: %s\n", yytext); }
"}"                { printf("Punctuation: %s\n", yytext); }

{DIGIT}+{LETTER}+  {
                     printf("LEXICAL ERROR at line %d: %s invalid lexeme\n", line, yytext);
                   }

.                  {
                     printf("Unknown token at line %d: %s\n", line, yytext);
                   }

%%

int main()
{
    FILE *fp = fopen("input.c","r");

    if(fp==NULL)
    {
        printf("Cannot open input.c\n");
        return 1;
    }

    yyin = fp;

    printf("TOKENS:\n\n");

    yylex();

    printf("\nSYMBOL TABLE:\n");

    int i;
    for(i=0;i<symbolCount;i++)
    {
        printf("%d) %s\n", i+1, symbolTable[i]);
    }

    fclose(fp);
    return 0;
}
