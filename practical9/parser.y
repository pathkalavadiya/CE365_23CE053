%{
#include <iostream>
using namespace std;

int yylex();
void yyerror(const char *s);
%}

%token I T E_TOKEN A B

%%

S  : I E T S S1
   | A
   ;

S1 : E_TOKEN S
   | /* epsilon */
   ;

E  : B
   ;

%%

void yyerror(const char *s) {
    cout << "Invalid string" << endl;
}

int main() {
    cout << "Enter string: ";
    if (yyparse() == 0)
        cout << "Valid string" << endl;
    return 0;
}