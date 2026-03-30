%{
#include "parser.tab.h"
%}

%%
"i"     { return I; }
"t"     { return T; }
"e"     { return E_TOKEN; }
"a"     { return A; }
"b"     { return B; }

[ \t\n]    ;   // ignore whitespace

.       { return yytext[0]; }  // invalid char
%%

int yywrap() {
    return 1;
}