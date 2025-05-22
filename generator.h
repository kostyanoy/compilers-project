#ifndef RISC_GENERATOR_H
#define RISC_GENERATOR_H

#include "ast.h"

// Функция для генерации кода RISC процессора
void generate_stmt(FILE* file, Node* node);
void generate_expr(FILE* file, Node* node, int res_reg);

// инициализация генератора
void init_generator();

#endif /* RISC_GENERATOR_H */ 