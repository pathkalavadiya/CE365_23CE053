%{
#include <stdio.h>

int hasLower = 0;
int hasUpper = 0;
int hasDigit = 0;
int hasSymbol = 0;
int length = 0;

void checkPassword() {
    if(length >= 9 && length <= 15 &&
       hasLower && hasUpper &&
       hasDigit && hasSymbol)
        printf("Valid password\n");
    else
        printf("Invalid password\n");

    /* Reset values */
    hasLower = hasUpper = hasDigit = hasSymbol = 0;
    length = 0;
}
%}

%%
[a-z]          { hasLower = 1; length++; }
[A-Z]          { hasUpper = 1; length++; }
[0-9]          { hasDigit = 1; length++; }
[\*\;\#\$\@]   { hasSymbol = 1; length++; }

\n             { 
                  checkPassword(); 
                  printf("Enter Password: ");
               }

.              { length++; }   /* Count any other character */

<<EOF>>        { 
                  if(length > 0)
                      checkPassword();
                  return 0; 
               }
%%

int main()
{
    printf("Enter Password: ");
    yylex();
    return 0;
}

int yywrap()
{
    return 1;
}
