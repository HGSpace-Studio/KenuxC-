#ifndef KENC_PARSER_H
#define KENC_PARSER_H

#include "lexer.h"

typedef enum {
    AST_PROGRAM,
    AST_FUNCTION_DECL,
    AST_FUNCTION_CALL,
    AST_VARIABLE_DECL,
    AST_ASSIGNMENT,
    AST_BINARY_OP,
    AST_UNARY_OP,
    AST_LITERAL,
    AST_IDENTIFIER,
    AST_IF_STATEMENT,
    AST_WHILE_STATEMENT,
    AST_FOR_STATEMENT,
    AST_RETURN_STATEMENT,
    AST_BLOCK,
    AST_CLASS_DECL,
    AST_METHOD_DECL,
    AST_PROPERTY_DECL,
    AST_NEW_EXPR,
    AST_MEMBER_ACCESS,
    AST_ARRAY_ACCESS,
    AST_ARRAY_DECL,
    AST_STRUCT_DECL,
    AST_ENUM_DECL,
    AST_IMPORT_STMT,
    AST_NAMESPACE_DECL,
    AST_TRY_CATCH,
    AST_THROW_STMT,
    AST_CAST_EXPR,
    AST_TERNARY_EXPR,
    AST_UNKNOWN
} ast_type_t;

typedef struct ast_node {
    ast_type_t type;
    char* value;
    struct ast_node** children;
    int child_count;
    int line;
    int column;
    char* filename;
} ast_node_t;

typedef struct {
    lexer_t* lexer;
    token_t current_token;
    ast_node_t* root;
} parser_t;

parser_t* parser_create(lexer_t* lexer);
void parser_free(parser_t* parser);

ast_node_t* parser_parse(parser_t* parser);
ast_node_t* parser_parse_program(parser_t* parser);
ast_node_t* parser_parse_function_decl(parser_t* parser);
ast_node_t* parser_parse_function_call(parser_t* parser);
ast_node_t* parser_parse_variable_decl(parser_t* parser);
ast_node_t* parser_parse_assignment(parser_t* parser);
ast_node_t* parser_parse_expression(parser_t* parser);
ast_node_t* parser_parse_binary_op(parser_t* parser, int precedence);
ast_node_t* parser_parse_unary_op(parser_t* parser);
ast_node_t* parser_parse_literal(parser_t* parser);
ast_node_t* parser_parse_identifier(parser_t* parser);
ast_node_t* parser_parse_if_statement(parser_t* parser);
ast_node_t* parser_parse_while_statement(parser_t* parser);
ast_node_t* parser_parse_for_statement(parser_t* parser);
ast_node_t* parser_parse_return_statement(parser_t* parser);
ast_node_t* parser_parse_block(parser_t* parser);
ast_node_t* parser_parse_class_decl(parser_t* parser);
ast_node_t* parser_parse_method_decl(parser_t* parser);
ast_node_t* parser_parse_property_decl(parser_t* parser);
ast_node_t* parser_parse_new_expr(parser_t* parser);
ast_node_t* parser_parse_member_access(parser_t* parser);
ast_node_t* parser_parse_array_access(parser_t* parser);
ast_node_t* parser_parse_array_decl(parser_t* parser);
ast_node_t* parser_parse_struct_decl(parser_t* parser);
ast_node_t* parser_parse_enum_decl(parser_t* parser);
ast_node_t* parser_parse_import_stmt(parser_t* parser);
ast_node_t* parser_parse_namespace_decl(parser_t* parser);
ast_node_t* parser_parse_try_catch(parser_t* parser);
ast_node_t* parser_parse_throw_stmt(parser_t* parser);
ast_node_t* parser_parse_cast_expr(parser_t* parser);
ast_node_t* parser_parse_ternary_expr(parser_t* parser);
ast_node_t* parser_parse_postfix(parser_t* parser);
ast_node_t* parser_parse_primary(parser_t* parser);

void parser_advance(parser_t* parser);
int parser_match(parser_t* parser, token_type_t type, const char* value);
int parser_match_type(parser_t* parser, token_type_t type);
int parser_match_value(parser_t* parser, const char* value);

ast_node_t* ast_node_create(ast_type_t type, const char* value, int line, int column, const char* filename);
void ast_node_add_child(ast_node_t* parent, ast_node_t* child);
void ast_node_free(ast_node_t* node);
char* ast_type_to_string(ast_type_t type);

void ast_print(ast_node_t* node, int indent);

#endif