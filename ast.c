#include "ast.h"
#include <stdio.h>

Node* create_int_node(int value) {
    Node* n = calloc(1, sizeof(Node));
    n->type = NODE_INT;
    n->int_val = value;
    return n;
}

Node* create_string_node(char* value) {
    Node* n = calloc(1, sizeof(Node));
    n->type = NODE_STRING;
    n->string_val = strdup(value);
    return n;
}

Node* create_id_node(char* name) {
    Node* n = calloc(1, sizeof(Node));
    n->type = NODE_ID;
    n->id_name = strdup(name);
    return n;
}

Node* create_assign_node(char* name, Node* expr) {
    Node* n = calloc(1, sizeof(Node));
    n->type = NODE_ASSIGN;
    n->assign_stmt.id_name = strdup(name);
    n->assign_stmt.expr = expr;
    return n;
}

Node* create_binary_op_node(char* op, Node* left, Node* right) {
    Node* n = calloc(1, sizeof(Node));
    n->type = NODE_BINARY_OP;
    n->bin_op.op = strdup(op);
    n->bin_op.left = left;
    n->bin_op.right = right;
    return n;
}

Node* create_unary_op_node(char* op, Node* expr) {
    Node* n = calloc(1, sizeof(Node));
    n->type = NODE_UNARY_OP;
    n->un_op.op = strdup(op);
    n->un_op.expr = expr;
    return n;
}

Node* create_if_node(Node* cond, Node* then_branch, Node* else_branch) {
    Node* n = calloc(1, sizeof(Node));
    n->type = NODE_IF;
    n->if_stmt.cond = cond;
    n->if_stmt.then_branch = then_branch;
    n->if_stmt.else_branch = else_branch;
    return n;
}

Node* create_while_node(Node* cond, Node* body) {
    Node* n = calloc(1, sizeof(Node));
    n->type = NODE_WHILE;
    n->while_stmt.cond = cond;
    n->while_stmt.body = body;
    return n;
}

Node* create_print_node(Node* expr) {
    Node* n = calloc(1, sizeof(Node));
    n->type = NODE_PRINT;
    n->print_stmt.expr = expr;
    return n;
}

Node* create_block_node(Node** stmts, int count) {
    Node* n = calloc(1, sizeof(Node));
    n->type = NODE_BLOCK;
    n->block.stmts = stmts;
    n->block.count = count;
    return n;
}

void print_indent(FILE* file, int depth) {
    for (int i = 0; i < depth; ++i) fprintf(file, "  ");
}

void print_ast(FILE* file, Node* node, int depth) {
    if (!node) return;

    switch (node->type) {
        case NODE_INT:
            print_indent(file, depth);
            fprintf(file, "INT: %d\n", node->int_val);
            break;
        case NODE_STRING:
            print_indent(file, depth);
            fprintf(file, "STRING: \"%s\"\n", node->string_val);
            break;
        case NODE_ID:
            print_indent(file, depth);
            fprintf(file, "ID: %s\n", node->id_name);
            break;
        case NODE_ASSIGN:
            print_indent(file, depth);
            fprintf(file, "ASSIGN: %s\n", node->assign_stmt.id_name);
            print_ast(file, node->assign_stmt.expr, depth + 1);
            break;
        case NODE_BINARY_OP:
            print_indent(file, depth);
            fprintf(file, "BINOP: %s\n", node->bin_op.op);
            print_ast(file, node->bin_op.left, depth + 1);
            print_ast(file, node->bin_op.right, depth + 1);
            break;
        case NODE_UNARY_OP:
            print_indent(file, depth);
            fprintf(file, "UNOP: %s\n", node->un_op.op);
            print_ast(file, node->un_op.expr, depth + 1);
            break;
        case NODE_IF:
            print_indent(file, depth);
            fprintf(file, "IF\n");
            print_ast(file, node->if_stmt.cond, depth + 1);
            print_ast(file, node->if_stmt.then_branch, depth + 1);
            if (node->if_stmt.else_branch)
                print_ast(file, node->if_stmt.else_branch, depth + 1);
            break;
        case NODE_WHILE:
            print_indent(file, depth);
            fprintf(file, "WHILE\n");
            print_ast(file, node->while_stmt.cond, depth + 1);
            print_ast(file, node->while_stmt.body, depth + 1);
            break;
        case NODE_PRINT:
            print_indent(file, depth);
            fprintf(file, "PRINT\n");
            print_ast(file, node->print_stmt.expr, depth + 1);
            break;
        case NODE_BLOCK:
            print_indent(file, depth);
            fprintf(file, "BLOCK\n");
            for (int i = 0; i < node->block.count; ++i) {
                print_ast(file, node->block.stmts[i], depth + 1);
            }
            break;
    }
}