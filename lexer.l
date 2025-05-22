%{
#include "ast.h"
#include "generator.c"
#include "parser.tab.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
extern YYSTYPE yylval;
%}

DIGIT       [0-9]
LETTER      [a-zA-Z_]
ID          {LETTER}({LETTER}|{DIGIT})*
INTEGER     [-]?{DIGIT}+
STRING      \"[^\"\n]*\" 

%%

"int"               { return KEYWORD_INT; }
"string"            { return KEYWORD_STRING; }
"if"                { return KEYWORD_IF; }
"else"              { return KEYWORD_ELSE; }
"while"             { return KEYWORD_WHILE; }
"print"             { return KEYWORD_PRINT; }

{ID}                {
                        yylval.string_val = strdup(yytext);
                        return IDENTIFIER;
                    }

{INTEGER}           {
                        yylval.int_val = atoi(yytext);
                        return INTEGER_LITERAL;
                    }

{STRING}            {
                        yylval.string_val = strdup(yytext);
                        return STRING_LITERAL;
                    }

"=="                { return OP_EQ; }
"!="                { return OP_NE; }
"<="                { return OP_LE; }
">="                { return OP_GE; }
"<"                 { return OP_LT; }
">"                 { return OP_GT; }

"&&"                { return OP_AND; }
"\|\|"              { return OP_OR; }
"!"                 { return OP_NOT; }

"+"                 { return OP_PLUS; }
"-"                 { return OP_MINUS; }
"*"                 { return OP_MUL; }
"/"                 { return OP_DIV; }

"="                 { return ASSIGN; }
";"                 { return SEMICOLON; }

"{"                 { return LBRACE; }
"}"                 { return RBRACE; }
"("                 { return LPAREN; }
")"                 { return RPAREN; }

"//".*                  ;   // игнорируем
"/*"([^*]|\*[^/])*"*/"  ;   // игнорируем
[ \t\n]                 ;   // игнорируем

.                   { printf("UNKNOWN: %s\n", yytext); }

%%

// Нужно для завершения сканирования
int yywrap(void) {
    return 1;
}