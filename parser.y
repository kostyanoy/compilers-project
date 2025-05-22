%{
#include "ast.h"
#include "generator.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern int yylex();
extern FILE *yyin;
void yyerror(const char *s);

Node* root;  // корень AST
%}

/* Объявляем YYSTYPE для хранения значений */
%union {
    int int_val;
    char* string_val;
    Node* node;
}

/* Терминалы */
%token <int_val> INTEGER_LITERAL
%token <string_val> IDENTIFIER STRING_LITERAL

%token KEYWORD_INT
%token KEYWORD_STRING
%token KEYWORD_IF
%token KEYWORD_ELSE
%token KEYWORD_WHILE
%token KEYWORD_PRINT

%token SEMICOLON
%token ASSIGN
%token LPAREN RPAREN LBRACE RBRACE

%token OP_PLUS OP_MINUS OP_MUL OP_DIV
%token OP_EQ OP_NE OP_LT OP_GT OP_LE OP_GE
%token OP_AND OP_OR OP_NOT

/* Приоритет операций */
%left OP_OR
%left OP_AND
%left OP_EQ OP_NE
%left OP_LT OP_GT OP_LE OP_GE
%left OP_PLUS OP_MINUS
%left OP_MUL OP_DIV
%left OP_NOT

/* Нетерминалы */
%type <node> program statement_list statement if_statement while_statement print_statement assignment variable_declaration expression

%nonassoc LOWER_THAN_ELSE
%nonassoc KEYWORD_ELSE

/* Начальный символ */
%start program

%%

program:
    statement_list {
        FILE* ast_file = fopen("ast_output.txt", "w");
        FILE* code_file = fopen("code_output.txt", "w");

        init_generator();

        root = create_block_node($1->block.stmts, $1->block.count);

        print_ast(ast_file, root, 0);
        generate_stmt(code_file, root);
    }
    ;

statement_list:
    /* пустой список */ {
        $$ = create_block_node(NULL, 0);
    }
    | statement_list statement {
        Node** new_stmts = realloc($1->block.stmts, sizeof(Node*) * ($1->block.count + 1));
        new_stmts[$1->block.count] = $2;
        $$ = create_block_node(new_stmts, $1->block.count + 1);
    }
    ;

statement:
    variable_declaration SEMICOLON {
        $$ = $1;
    }
    | assignment SEMICOLON {
        $$ = $1;
    }
    | print_statement SEMICOLON {
        $$ = $1;
    }
    | if_statement {
        $$ = $1;
    }
    | while_statement {
        $$ = $1;
    }
    | LBRACE statement_list RBRACE {
        $$ = $2;
    }
    ;

variable_declaration:
    KEYWORD_INT IDENTIFIER ASSIGN expression {
        $$ = create_assign_node($2, $4);
    }
    | KEYWORD_STRING IDENTIFIER ASSIGN expression {
        // expression должен быть STRING_LITERAL
        $$ = create_assign_node($2, $4);
    }
    ;
assignment:
    IDENTIFIER ASSIGN expression {
        $$ = create_assign_node($1, $3);
    }
    ;

print_statement:
    KEYWORD_PRINT LPAREN expression RPAREN {
        $$ = create_print_node($3);
    }
    ;

if_statement:
    KEYWORD_IF LPAREN expression RPAREN statement %prec LOWER_THAN_ELSE {
        $$ = create_if_node($3, $5, NULL);
    }
    | KEYWORD_IF LPAREN expression RPAREN statement KEYWORD_ELSE statement {
        $$ = create_if_node($3, $5, $7);
    }
    ;

while_statement:
    KEYWORD_WHILE LPAREN expression RPAREN statement {
        $$ = create_while_node($3, $5);
    }
    ;

expression:
    INTEGER_LITERAL {
        $$ = create_int_node($1);
    }
    | STRING_LITERAL {
        $$ = create_string_node(strdup($1 + 1));  // убираем кавычки
        $$->string_val[strlen($$->string_val) - 1] = '\0';
    }
    | IDENTIFIER {
        $$ = create_id_node($1);
    }
    | expression OP_PLUS expression {
        $$ = create_binary_op_node("+", $1, $3);
    }
    | expression OP_MINUS expression {
        $$ = create_binary_op_node("-", $1, $3);
    }
    | expression OP_MUL expression {
        $$ = create_binary_op_node("*", $1, $3);
    }
    | expression OP_DIV expression {
        $$ = create_binary_op_node("/", $1, $3);
    }
    | expression OP_EQ expression {
        $$ = create_binary_op_node("==", $1, $3);
    }
    | expression OP_NE expression {
        $$ = create_binary_op_node("!=", $1, $3);
    }
    | expression OP_LT expression {
        $$ = create_binary_op_node("<", $1, $3);
    }
    | expression OP_GT expression {
        $$ = create_binary_op_node(">", $1, $3);
    }
    | expression OP_LE expression {
        $$ = create_binary_op_node("<=", $1, $3);
    }
    | expression OP_GE expression {
        $$ = create_binary_op_node(">=", $1, $3);
    }
    | expression OP_AND expression {
        $$ = create_binary_op_node("&&", $1, $3);
    }
    | expression OP_OR expression {
        $$ = create_binary_op_node("||", $1, $3);
    }
    | OP_NOT expression {
        $$ = create_unary_op_node("!", $2);  // только один операнд
    }
    | LPAREN expression RPAREN {
        $$ = $2;
    }

%%

void yyerror(const char *s) {
    fprintf(stderr, "Parse error: %s\n", s);
}