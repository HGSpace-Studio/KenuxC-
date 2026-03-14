#ifndef KENC_CODEGEN_H
#define KENC_CODEGEN_H

#include "parser.h"

typedef struct {
    char* code;
    int position;
    int capacity;
    int indent_level;
} codegen_t;

codegen_t* codegen_create(void);
void codegen_free(codegen_t* gen);

char* codegen_generate(ast_node_t* ast);
void codegen_emit(codegen_t* gen, const char* format, ...);
void codegen_emit_indent(codegen_t* gen);
void codegen_emit_newline(codegen_t* gen);
void codegen_emit_string(codegen_t* gen, const char* str);

void codegen_generate_program(codegen_t* gen, ast_node_t* node);
void codegen_generate_function_decl(codegen_t* gen, ast_node_t* node);
void codegen_generate_variable_decl(codegen_t* gen, ast_node_t* node);
void codegen_generate_expression(codegen_t* gen, ast_node_t* node);
void codegen_generate_binary_op(codegen_t* gen, ast_node_t* node);
void codegen_generate_unary_op(codegen_t* gen, ast_node_t* node);
void codegen_generate_literal(codegen_t* gen, ast_node_t* node);
void codegen_generate_identifier(codegen_t* gen, ast_node_t* node);
void codegen_generate_if_statement(codegen_t* gen, ast_node_t* node);
void codegen_generate_while_statement(codegen_t* gen, ast_node_t* node);
void codegen_generate_for_statement(codegen_t* gen, ast_node_t* node);
void codegen_generate_return_statement(codegen_t* gen, ast_node_t* node);
void codegen_generate_block(codegen_t* gen, ast_node_t* node);
void codegen_generate_function_call(codegen_t* gen, ast_node_t* node);
void codegen_generate_class_decl(codegen_t* gen, ast_node_t* node);
void codegen_generate_method_decl(codegen_t* gen, ast_node_t* node, const char* class_name);
void codegen_generate_property_decl(codegen_t* gen, ast_node_t* node);
void codegen_generate_new_expr(codegen_t* gen, ast_node_t* node);
void codegen_generate_member_access(codegen_t* gen, ast_node_t* node);
void codegen_generate_array_access(codegen_t* gen, ast_node_t* node);
void codegen_generate_import_stmt(codegen_t* gen, ast_node_t* node);
void codegen_generate_try_catch(codegen_t* gen, ast_node_t* node);
void codegen_generate_throw_stmt(codegen_t* gen, ast_node_t* node);
void codegen_generate_struct_decl(codegen_t* gen, ast_node_t* node);
void codegen_generate_enum_decl(codegen_t* gen, ast_node_t* node);
void codegen_generate_ternary_expr(codegen_t* gen, ast_node_t* node);

#endif