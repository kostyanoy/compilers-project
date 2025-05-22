#include "generator.h"
#include "ast.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// init sttructures
typedef enum {
    STRING,
    INT
} VarType;

typedef struct {
    char* name;
    VarType type;
    int len;
    int address;
} VariableEntry;

typedef struct {
    VariableEntry* entries;
    int count;
    int memory_counter; // Текущий адрес для следующей переменной
    int label_counter;
} SymbolTable;

SymbolTable symbol_table = {0};

// инициализировать нужные параметры в файле
void init_generator() {
    symbol_table.count = 0;
    symbol_table.memory_counter = 0x1000; // Начальный адрес для данных
    symbol_table.entries = NULL;
    symbol_table.label_counter = 0;
}

// добавить новую переменную
int add_variable(const char* name, int size, VarType type) {
    VariableEntry entry;
    entry.name = strdup(name);
    entry.address = symbol_table.memory_counter;
    entry.type = type;
    entry.len = size;
    symbol_table.memory_counter += size;

    symbol_table.count++;
    symbol_table.entries = realloc(symbol_table.entries, sizeof(VariableEntry) * symbol_table.count);
    symbol_table.entries[symbol_table.count - 1] = entry;
    return entry.address;
}

// узнать тип переменной
VarType get_var_type(const char* name) {
    for (int i = 0; i < symbol_table.count; ++i) {
        if (strcmp(symbol_table.entries[i].name, name) == 0) {
            return symbol_table.entries[i].type;
        }
    }
}

// получить адресс переменной
int get_variable_address(const char* name) {
    for (int i = 0; i < symbol_table.count; ++i) {
        if (strcmp(symbol_table.entries[i].name, name) == 0) {
            return symbol_table.entries[i].address;
        }
    }
    return -1; // Не найдено
}

// получить переменную
VariableEntry get_variable(const char* name) {
    for (int i = 0; i < symbol_table.count; ++i) {
        if (strcmp(symbol_table.entries[i].name, name) == 0) {
            return symbol_table.entries[i];
        }
    }
}

// получить новую меку
char* generate_label() {
    char* label = malloc(32);
    sprintf(label, "L%d", symbol_table.label_counter++);
    return label;
}

// обработать выражение с одним операндом
void generate_unary_op(FILE* file, Node* node, int res_reg) {
    generate_expr(file, node->un_op.expr, res_reg);

    if (strcmp(node->un_op.op, "!") == 0) {
        fprintf(file, "seq x%d, x%d, x0\n", res_reg, res_reg);
    } else {
        fprintf(stderr, "Wrong unary operation: %s\n", node->un_op.op);
    }
}

// обработать выражение с двумя операндами
void generate_binary_op(FILE* file, Node* node, int res_reg) {   
    int left_reg = res_reg + 1;
    int right_reg = res_reg + 2;

    generate_expr(file, node->bin_op.left, left_reg); // Вычисляем левую часть
    generate_expr(file, node->bin_op.right, right_reg); // Вычисляем правую часть

    if (strcmp(node->bin_op.op, "+") == 0) {
        fprintf(file, "add x%d, x%d, x%d\n", res_reg, left_reg, right_reg);
    } else if (strcmp(node->bin_op.op, "-") == 0) {
        fprintf(file, "sub x%d, x%d, x%d\n", res_reg, left_reg, right_reg);
    } else if (strcmp(node->bin_op.op, "*") == 0) {
        fprintf(file, "mul x%d, x%d, x%d\n", res_reg, left_reg, right_reg);
    } else if (strcmp(node->bin_op.op, "/") == 0) {
        fprintf(file, "div x%d, x%d, x%d\n", res_reg, left_reg, right_reg);
    } else if (strcmp(node->bin_op.op, "==") == 0) {
        fprintf(file, "seq x%d, x%d, x%d\n", res_reg, left_reg, right_reg);
    } else if (strcmp(node->bin_op.op, "!=") == 0) {
        fprintf(file, "sne x%d, x%d, x%d\n", res_reg, left_reg, right_reg);
    } else if (strcmp(node->bin_op.op, "<") == 0) {
        fprintf(file, "slt x%d, x%d, x%d\n", res_reg, left_reg, right_reg);
    } else if (strcmp(node->bin_op.op, ">") == 0) {
        fprintf(file, "slt x%d, x%d, x%d\n", res_reg, right_reg, left_reg);
    } else if (strcmp(node->bin_op.op, "<=") == 0) {
        fprintf(file, "sge x%d, x%d, x%d\n", res_reg, right_reg, left_reg);
    } else if (strcmp(node->bin_op.op, ">=") == 0) {
        fprintf(file, "sge x%d, x%d, x%d\n", res_reg, left_reg, right_reg);
    } else if (strcmp(node->bin_op.op, "&&") == 0) {
        fprintf(file, "and x%d, x%d, x%d\n", res_reg, left_reg, right_reg);
    } else if (strcmp(node->bin_op.op, "||") == 0) {
        fprintf(file, "or x%d, x%d, x%d\n", res_reg, left_reg, right_reg);
    } else if (strcmp(node->bin_op.op, "!") == 0) {
        fprintf(file, "seq x%d, x%d, x%d\n", res_reg, left_reg, right_reg);
    } else {
        fprintf(stderr, "Wrong binary operation: %s\n", node->bin_op.op);
    }
}
 
// обработать выражение
void generate_expr(FILE* file, Node* node, int res_reg) {
    if (!node) {
        fprintf(file, "li x%d, %d\n", res_reg, 0);
        return;
    }

    if (node->type == NODE_INT) {
        // конкретное целое значение
        fprintf(file, "li x%d, %d\n", res_reg, node->int_val);
    } else if (node->type == NODE_ID) {
        // значение из переменной
        char* name = node->id_name;
        if (get_variable_address(name) == -1) {
            fprintf(stderr, "Uninitialized variable in expression: %s\n", name);
            return;
        }
        if (get_var_type(name) == STRING) {
            fprintf(stderr, "Can't use STRING variable in expression: %s\n", name);
            return;
        }
        int addr = get_variable_address(name);
        fprintf(file, "li x%d, %d\n", res_reg + 1, addr);
        fprintf(file, "lw x%d, x%d, 0\n", res_reg, res_reg+1);
    } else if (node->type == NODE_BINARY_OP) {
        // вычисляемое целое значение
        generate_binary_op(file, node, res_reg);
    } else if (node->type == NODE_UNARY_OP) {
        generate_unary_op(file, node, res_reg);
    } else {
        fprintf(stderr, "Unknown type in expr\n");
        return;
    }    
}

// присвоить знаение переменной
void generate_assign_stmt(FILE* file, Node* node) {
    char* name = node->assign_stmt.id_name;
    Node* expr = node->assign_stmt.expr;
    if (expr->type == NODE_STRING) {
        // присвоить переменную-строку
        if (get_variable_address(name) != -1) {
            fprintf(stderr, "Can't override the string\n");
            return;
        }
        int addr = add_variable(name, strlen(expr->string_val) + 1, STRING);
        // посимвольно сохраняем в память
        fprintf(file, "li x1, %d\n", addr);
        for (int i = 0; expr->string_val[i]; ++i) {
            fprintf(file, "li x2, %d\n", expr->string_val[i]);
            fprintf(file, "sw x1, %d, x2\n", i);
        }
    } else {
        // присвоить переменную-число
        if (get_variable_address(name) == -1) {
            add_variable(name, 1, INT);
        }
        int addr = get_variable_address(name);
        int type = get_var_type(name);
        if (type != INT) {
            fprintf(stderr, "Can't assign INT to non-INT variable\n");
            return;
        }
        generate_expr(file, expr, 2);
        fprintf(file, "li x1, %d\n", addr);
        fprintf(file, "sw x1, %d, x2\n", 0);     
    }
}

void generate_if_stmt(FILE* file, Node* node) {
    Node* cond = node->if_stmt.cond;
    Node* then_branch = node->if_stmt.then_branch;
    Node* else_branch = node->if_stmt.else_branch;

    char* else_label = generate_label();
    char* end_label = generate_label();

    generate_expr(file, cond, 1); // Результат условия в x1
    fprintf(file, "beq x1, x0, %s\n", else_label);
    generate_stmt(file, then_branch);
    fprintf(file, "jal x0, %s\n", end_label);
    fprintf(file, "%s:\n", else_label);
    if (else_branch) {
        generate_stmt(file, else_branch);
    }
    fprintf(file, "%s:\n", end_label);
}

void generate_while_stmt(FILE* file, Node* node) {
    Node* cond = node->while_stmt.cond;
    Node* body = node->while_stmt.body;

    char* loop_start = generate_label();
    char* loop_end = generate_label();

    fprintf(file, "%s:\n", loop_start);
    generate_expr(file, cond, 1); // Результат условия в x1
    fprintf(file, "beq x1, x0, %s\n", loop_end);
    generate_stmt(file, body);
    fprintf(file, "jal x0, %s\n", loop_start);
    fprintf(file, "%s:\n", loop_end);
}

// вывести в консоль
void generate_print_stmt(FILE* file, Node* node) {
    Node* expr = node->print_stmt.expr;

    if (expr->type == NODE_STRING) {
        // вывести посимвольно
        for (int i = 0; i < expr->string_val[i]; ++i) {
            fprintf(file, "li x1, %d\n", expr->string_val[i]);
            fprintf(file, "ewrite x1\n");
        }

    } else if (expr->type == NODE_ID && get_var_type(expr->id_name) == STRING) {
        // вывести посимвольно из памяти
        char* name = expr->id_name;
        VariableEntry var = get_variable(name);

        fprintf(file, "li x1, %d\n", var.address);
        for (int i = 0; i < var.len - 1; ++i) {
            fprintf(file, "lw x2, x1, %d\n", i);
            fprintf(file, "ewrite x2\n");
        }
    } else {
        generate_expr(file, expr, 1);
        fprintf(file, "li x3, 10\n"); // для деления
        fprintf(file, "li x4, 48\n"); // сдивг до ascii 0
        fprintf(file, "li x5, 1\n");  // делитель
        // ищем макс число меньше данного
        fprintf(file, "mul x5, x5, x3\n");  // a * 10
        fprintf(file, "bge x1, x5, -2\n");  // a * 10
        fprintf(file, "div x5, x5, x3\n");  // a * 10

        // вывод
        fprintf(file, "div x2, x1, x5\n"); // деление на x5
        fprintf(file, "rem x1, x1, x5\n"); // остаток на x5
        fprintf(file, "div x5, x5, x3\n"); // остаток на x5
        fprintf(file, "add x2, x2, x4\n"); // сдвиг
        fprintf(file, "ewrite x2\n");      // вывод
        fprintf(file, "bne x0, x5, -6\n");  // возврат если не 0   
    }
    fprintf(file, "li x1, %d\n", '\n');
    fprintf(file, "ewrite x1\n");

}

void generate_stmt(FILE* file, Node* node) {
    if (!node) return;

    switch (node->type) {
        case NODE_ASSIGN: {
            generate_assign_stmt(file, node);
            break;
        }
        case NODE_IF: {
            generate_if_stmt(file, node);
            break;
        }
        case NODE_WHILE: {
            generate_while_stmt(file, node);
            break;
        }
        case NODE_PRINT: {
            generate_print_stmt(file, node);
            break;
        }
        case NODE_BLOCK: {
            // Блок кода — список инструкций внутри { ... }
            for (int i = 0; i < node->block.count; ++i) {
                generate_stmt(file, node->block.stmts[i]);
            }
            break;
        }
        default:
            fprintf(stderr, "Incorrect node type in generate_stmt\n");
            break;
    }
}