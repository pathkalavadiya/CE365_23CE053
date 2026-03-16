%{
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int i = 0;
char str[100];   

int S();
int L();
int Lprime();
%}

%%
[a(),]    { str[i++] = yytext[0]; }
\n        { str[i] = '\0'; return 0; }
.         { printf("Invalid character\n"); exit(0); }
%%

int S() {
    if (str[i] == 'a') {
        i++;
        return 1;
    }
    else if (str[i] == '(') {
        i++;
        if (L()) {
            if (str[i] == ')') {
                i++;
                return 1;
            }
        }
    }
    return 0;
}

int L() {
    if (S()) {
        if (Lprime()) {
            return 1;
        }
    }
    return 0;
}

int Lprime() {
    if (str[i] == ',') {
        i++;
        if (S()) {
            if (Lprime()) {
                return 1;
            }
        }
        return 0;
    }
    return 1;  // epsilon case
}

int main() {
    printf("Enter string: ");
    yylex();

    i = 0;
    if (S() && str[i] == '\0')
        printf("Valid string\n");
    else
        printf("Invalid string\n");

    return 0;
}

int yywrap() {
    return 1;
}