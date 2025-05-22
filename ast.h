#ifndef AST_H
#define AST_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Перечисление типов узлов AST — описывает, какой вид данных хранится в данном узле
typedef enum {
    NODE_INT,         // Целое число, например: 42
    NODE_STRING,      // Строка, например: "Hello"
    NODE_ID,          // Имя переменной, например: x
    NODE_ASSIGN,      // Присваивание, например: x = 5
    NODE_BINARY_OP,   // Бинарная операция, например: a + b, x > y
    NODE_UNARY_OP,    // Унарная операция: !a
    NODE_IF,          // Условный оператор if
    NODE_WHILE,       // Цикл while
    NODE_PRINT,       // Оператор вывода print
    NODE_BLOCK        // Блок кода, состоящий из нескольких операторов
} NodeType;

// Структура Node — представляет собой узел AST
// Каждый узел имеет тип (NodeType) и данные, зависящие от этого типа
typedef struct Node {
    NodeType type;
    union {
        int int_val;            // Значение целого числа
        char* string_val;       // Указатель на строку
        char* id_name;          // Имя переменной

        // Структура для приснаивания
        struct {
            char* id_name;      // Имя переменной
            struct Node* expr;  // Значение
        } assign_stmt;

        // Структура для бинарных операций: например, a + b
        struct {
            char* op;            // Оператор: "+", "==", "&&" и т.д.
            struct Node* left;   // Левый операнд
            struct Node* right;  // Правый операнд
        } bin_op;

        struct {
            char* op;
            struct Node* expr;
        } un_op;

        // Условный оператор if
        struct {
            struct Node* cond;        // Условие
            struct Node* then_branch; // Ветка then
            struct Node* else_branch; // Ветка else (может быть NULL)
        } if_stmt;

        // Цикл while
        struct {
            struct Node* cond;        // Условие цикла
            struct Node* body;        // Тело цикла
        } while_stmt;

        // Оператор print
        struct {
            struct Node* expr;        // Что печатать
        } print_stmt;

        // Блок — список инструкций внутри { ... }
        struct {
            struct Node** stmts;      // Массив указателей на узлы (инструкции)
            int count;                // Количество инструкций в блоке
        } block;
    };
} Node;

// Функции создания узлов AST
Node* create_int_node(int value);
Node* create_string_node(char* value);
Node* create_id_node(char* name);
Node* create_assign_node(char* name, Node* expr);
Node* create_binary_op_node(char* op, Node* left, Node* right);
Node* create_unary_op_node(char* op, Node* expr);
Node* create_if_node(Node* cond, Node* then_branch, Node* else_branch);
Node* create_while_node(Node* cond, Node* body);
Node* create_print_node(Node* expr);
Node* create_block_node(Node** stmts, int count);

// Функция для вывода AST в консоль в виде дерева с отступами
void print_ast(FILE* fiel, Node* node, int depth);

#endif // AST_H