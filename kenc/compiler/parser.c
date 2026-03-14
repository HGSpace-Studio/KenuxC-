#include "parser.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

static int get_operator_precedence(const char* op) {
    if (strcmp(op, "=") == 0 || strcmp(op, "+=") == 0 || strcmp(op, "-=") == 0 ||
        strcmp(op, "*=") == 0 || strcmp(op, "/=") == 0 || strcmp(op, "%=") == 0 ||
        strcmp(op, "&=") == 0 || strcmp(op, "|=") == 0 || strcmp(op, "^=") == 0 ||
        strcmp(op, "<<=") == 0 || strcmp(op, ">>=") == 0) {
        return 1;
    }
    if (strcmp(op, "||") == 0) return 2;
    if (strcmp(op, "&&") == 0) return 3;
    if (strcmp(op, "|") == 0) return 4;
    if (strcmp(op, "^") == 0) return 5;
    if (strcmp(op, "&") == 0) return 6;
    if (strcmp(op, "==") == 0 || strcmp(op, "!=") == 0) return 7;
    if (strcmp(op, "<") == 0 || strcmp(op, ">") == 0 || 
        strcmp(op, "<=") == 0 || strcmp(op, ">=") == 0) return 8;
    if (strcmp(op, "<<") == 0 || strcmp(op, ">>") == 0) return 9;
    if (strcmp(op, "+") == 0 || strcmp(op, "-") == 0) return 10;
    if (strcmp(op, "*") == 0 || strcmp(op, "/") == 0 || strcmp(op, "%") == 0) return 11;
    return 0;
}

parser_t* parser_create(lexer_t* lexer) {
    parser_t* parser = (parser_t*)malloc(sizeof(parser_t));
    if (!parser) return NULL;
    
    parser->lexer = lexer;
    parser->current_token = lexer_next_token(lexer);
    parser->root = NULL;
    
    return parser;
}

void parser_free(parser_t* parser) {
    if (parser) {
        if (parser->root) ast_node_free(parser->root);
        free(parser);
    }
}

void parser_advance(parser_t* parser) {
    if (parser->current_token.value) free(parser->current_token.value);
    if (parser->current_token.filename) free(parser->current_token.filename);
    parser->current_token = lexer_next_token(parser->lexer);
}

int parser_match(parser_t* parser, token_type_t type, const char* value) {
    if (parser->current_token.type != type) return 0;
    if (value && strcmp(parser->current_token.value, value) != 0) return 0;
    return 1;
}

int parser_match_type(parser_t* parser, token_type_t type) {
    return parser->current_token.type == type;
}

int parser_match_value(parser_t* parser, const char* value) {
    return strcmp(parser->current_token.value, value) == 0;
}

ast_node_t* ast_node_create(ast_type_t type, const char* value, int line, int column, const char* filename) {
    ast_node_t* node = (ast_node_t*)malloc(sizeof(ast_node_t));
    if (!node) return NULL;
    
    node->type = type;
    node->value = value ? strdup(value) : NULL;
    node->children = NULL;
    node->child_count = 0;
    node->line = line;
    node->column = column;
    node->filename = strdup(filename);
    
    return node;
}

void ast_node_add_child(ast_node_t* parent, ast_node_t* child) {
    if (!parent || !child) return;
    
    parent->child_count++;
    parent->children = (ast_node_t**)realloc(parent->children, 
                                              sizeof(ast_node_t*) * parent->child_count);
    parent->children[parent->child_count - 1] = child;
}

void ast_node_free(ast_node_t* node) {
    if (!node) return;
    
    if (node->value) free(node->value);
    if (node->filename) free(node->filename);
    
    for (int i = 0; i < node->child_count; i++) {
        ast_node_free(node->children[i]);
    }
    
    if (node->children) free(node->children);
    free(node);
}

char* ast_type_to_string(ast_type_t type) {
    switch (type) {
        case AST_PROGRAM: return "PROGRAM";
        case AST_FUNCTION_DECL: return "FUNCTION_DECL";
        case AST_FUNCTION_CALL: return "FUNCTION_CALL";
        case AST_VARIABLE_DECL: return "VARIABLE_DECL";
        case AST_ASSIGNMENT: return "ASSIGNMENT";
        case AST_BINARY_OP: return "BINARY_OP";
        case AST_UNARY_OP: return "UNARY_OP";
        case AST_LITERAL: return "LITERAL";
        case AST_IDENTIFIER: return "IDENTIFIER";
        case AST_IF_STATEMENT: return "IF_STATEMENT";
        case AST_WHILE_STATEMENT: return "WHILE_STATEMENT";
        case AST_FOR_STATEMENT: return "FOR_STATEMENT";
        case AST_RETURN_STATEMENT: return "RETURN_STATEMENT";
        case AST_BLOCK: return "BLOCK";
        case AST_CLASS_DECL: return "CLASS_DECL";
        case AST_METHOD_DECL: return "METHOD_DECL";
        case AST_PROPERTY_DECL: return "PROPERTY_DECL";
        case AST_NEW_EXPR: return "NEW_EXPR";
        case AST_MEMBER_ACCESS: return "MEMBER_ACCESS";
        case AST_ARRAY_ACCESS: return "ARRAY_ACCESS";
        case AST_ARRAY_DECL: return "ARRAY_DECL";
        case AST_STRUCT_DECL: return "STRUCT_DECL";
        case AST_ENUM_DECL: return "ENUM_DECL";
        case AST_IMPORT_STMT: return "IMPORT_STMT";
        case AST_NAMESPACE_DECL: return "NAMESPACE_DECL";
        case AST_TRY_CATCH: return "TRY_CATCH";
        case AST_THROW_STMT: return "THROW_STMT";
        case AST_CAST_EXPR: return "CAST_EXPR";
        case AST_TERNARY_EXPR: return "TERNARY_EXPR";
        default: return "UNKNOWN";
    }
}

ast_node_t* parser_parse(parser_t* parser) {
    return parser_parse_program(parser);
}

ast_node_t* parser_parse_program(parser_t* parser) {
    ast_node_t* program = ast_node_create(AST_PROGRAM, "program", 
                                           parser->current_token.line, 
                                           parser->current_token.column,
                                           parser->lexer->filename);
    
    while (parser->current_token.type != TOKEN_EOF) {
        if (parser_match(parser, TOKEN_KEYWORD, "import")) {
            ast_node_add_child(program, parser_parse_import_stmt(parser));
        } else if (parser_match(parser, TOKEN_KEYWORD, "namespace")) {
            ast_node_add_child(program, parser_parse_namespace_decl(parser));
        } else if (parser_match(parser, TOKEN_KEYWORD, "class")) {
            ast_node_add_child(program, parser_parse_class_decl(parser));
        } else if (parser_match(parser, TOKEN_KEYWORD, "struct")) {
            ast_node_add_child(program, parser_parse_struct_decl(parser));
        } else if (parser_match(parser, TOKEN_KEYWORD, "enum")) {
            ast_node_add_child(program, parser_parse_enum_decl(parser));
        } else if (parser_match(parser, TOKEN_KEYWORD, "void") ||
                   parser_match(parser, TOKEN_KEYWORD, "int") ||
                   parser_match(parser, TOKEN_KEYWORD, "long") ||
                   parser_match(parser, TOKEN_KEYWORD, "float") ||
                   parser_match(parser, TOKEN_KEYWORD, "double") ||
                   parser_match(parser, TOKEN_KEYWORD, "char") ||
                   parser_match(parser, TOKEN_KEYWORD, "string") ||
                   parser_match(parser, TOKEN_KEYWORD, "bool")) {
            ast_node_add_child(program, parser_parse_function_decl(parser));
        } else {
            parser_advance(parser);
        }
    }
    
    return program;
}

ast_node_t* parser_parse_import_stmt(parser_t* parser) {
    ast_node_t* import = ast_node_create(AST_IMPORT_STMT, "import",
                                          parser->current_token.line,
                                          parser->current_token.column,
                                          parser->lexer->filename);
    parser_advance(parser);
    
    if (parser_match_type(parser, TOKEN_STRING)) {
        ast_node_add_child(import, ast_node_create(AST_LITERAL, parser->current_token.value,
                                                   parser->current_token.line,
                                                   parser->current_token.column,
                                                   parser->lexer->filename));
        parser_advance(parser);
    } else if (parser_match_type(parser, TOKEN_IDENTIFIER)) {
        ast_node_add_child(import, parser_parse_identifier(parser));
    }
    
    if (parser_match(parser, TOKEN_PUNCTUATOR, ";")) {
        parser_advance(parser);
    }
    
    return import;
}

ast_node_t* parser_parse_namespace_decl(parser_t* parser) {
    ast_node_t* namespace_node = ast_node_create(AST_NAMESPACE_DECL, "namespace",
                                                  parser->current_token.line,
                                                  parser->current_token.column,
                                                  parser->lexer->filename);
    parser_advance(parser);
    
    ast_node_add_child(namespace_node, parser_parse_identifier(parser));
    
    if (parser_match(parser, TOKEN_PUNCTUATOR, "{")) {
        parser_advance(parser);
        
        while (!parser_match(parser, TOKEN_PUNCTUATOR, "}")) {
            if (parser_match(parser, TOKEN_KEYWORD, "class")) {
                ast_node_add_child(namespace_node, parser_parse_class_decl(parser));
            } else if (parser_match(parser, TOKEN_KEYWORD, "struct")) {
                ast_node_add_child(namespace_node, parser_parse_struct_decl(parser));
            } else if (parser_match(parser, TOKEN_KEYWORD, "enum")) {
                ast_node_add_child(namespace_node, parser_parse_enum_decl(parser));
            } else {
                parser_advance(parser);
            }
        }
        
        parser_advance(parser);
    }
    
    return namespace_node;
}

ast_node_t* parser_parse_function_decl(parser_t* parser) {
    ast_node_t* func = ast_node_create(AST_FUNCTION_DECL, "function",
                                       parser->current_token.line,
                                       parser->current_token.column,
                                       parser->lexer->filename);
    
    ast_node_add_child(func, ast_node_create(AST_LITERAL, parser->current_token.value,
                                              parser->current_token.line,
                                              parser->current_token.column,
                                              parser->lexer->filename));
    parser_advance(parser);
    
    ast_node_add_child(func, parser_parse_identifier(parser));
    
    if (parser_match(parser, TOKEN_PUNCTUATOR, "(")) {
        parser_advance(parser);
        
        while (!parser_match(parser, TOKEN_PUNCTUATOR, ")")) {
            ast_node_add_child(func, parser_parse_variable_decl(parser));
            if (parser_match(parser, TOKEN_PUNCTUATOR, ",")) {
                parser_advance(parser);
            }
        }
        
        parser_advance(parser);
    }
    
    if (parser_match(parser, TOKEN_PUNCTUATOR, "{")) {
        ast_node_add_child(func, parser_parse_block(parser));
    } else if (parser_match(parser, TOKEN_PUNCTUATOR, ";")) {
        parser_advance(parser);
    }
    
    return func;
}

ast_node_t* parser_parse_variable_decl(parser_t* parser) {
    ast_node_t* var = ast_node_create(AST_VARIABLE_DECL, "variable",
                                       parser->current_token.line,
                                       parser->current_token.column,
                                       parser->lexer->filename);
    
    if (parser_match_type(parser, TOKEN_KEYWORD) || parser_match_type(parser, TOKEN_IDENTIFIER)) {
        ast_node_add_child(var, ast_node_create(AST_LITERAL, parser->current_token.value,
                                                 parser->current_token.line,
                                                 parser->current_token.column,
                                                 parser->lexer->filename));
        parser_advance(parser);
    }
    
    if (parser_match_type(parser, TOKEN_IDENTIFIER)) {
        ast_node_add_child(var, parser_parse_identifier(parser));
    }
    
    if (parser_match(parser, TOKEN_OPERATOR, "=")) {
        parser_advance(parser);
        ast_node_add_child(var, parser_parse_expression(parser));
    }
    
    return var;
}

ast_node_t* parser_parse_expression(parser_t* parser) {
    return parser_parse_binary_op(parser, 0);
}

ast_node_t* parser_parse_binary_op(parser_t* parser, int precedence) {
    ast_node_t* left = parser_parse_unary_op(parser);
    
    while (parser_match_type(parser, TOKEN_OPERATOR)) {
        const char* op = parser->current_token.value;
        int op_precedence = get_operator_precedence(op);
        
        if (op_precedence <= precedence) {
            break;
        }
        
        parser_advance(parser);
        
        ast_node_t* right = parser_parse_binary_op(parser, op_precedence);
        
        ast_node_t* binary = ast_node_create(AST_BINARY_OP, op,
                                              parser->current_token.line,
                                              parser->current_token.column,
                                              parser->lexer->filename);
        ast_node_add_child(binary, left);
        ast_node_add_child(binary, right);
        left = binary;
    }
    
    if (parser_match(parser, TOKEN_OPERATOR, "?")) {
        parser_advance(parser);
        ast_node_t* then_expr = parser_parse_expression(parser);
        
        if (parser_match(parser, TOKEN_OPERATOR, ":")) {
            parser_advance(parser);
            ast_node_t* else_expr = parser_parse_expression(parser);
            
            ast_node_t* ternary = ast_node_create(AST_TERNARY_EXPR, "?",
                                                    parser->current_token.line,
                                                    parser->current_token.column,
                                                    parser->lexer->filename);
            ast_node_add_child(ternary, left);
            ast_node_add_child(ternary, then_expr);
            ast_node_add_child(ternary, else_expr);
            return ternary;
        }
    }
    
    return left;
}

ast_node_t* parser_parse_unary_op(parser_t* parser) {
    if (parser_match(parser, TOKEN_OPERATOR, "!") || 
        parser_match(parser, TOKEN_OPERATOR, "-") ||
        parser_match(parser, TOKEN_OPERATOR, "~") ||
        parser_match(parser, TOKEN_OPERATOR, "++") ||
        parser_match(parser, TOKEN_OPERATOR, "--")) {
        const char* op = parser->current_token.value;
        parser_advance(parser);
        
        ast_node_t* unary = ast_node_create(AST_UNARY_OP, op,
                                            parser->current_token.line,
                                            parser->current_token.column,
                                            parser->lexer->filename);
        ast_node_add_child(unary, parser_parse_unary_op(parser));
        return unary;
    }
    
    return parser_parse_postfix(parser);
}

ast_node_t* parser_parse_postfix(parser_t* parser) {
    ast_node_t* primary = parser_parse_primary(parser);
    
    while (1) {
        if (parser_match(parser, TOKEN_PUNCTUATOR, "(")) {
            parser_advance(parser);
            ast_node_t* call = ast_node_create(AST_FUNCTION_CALL, "call",
                                                parser->current_token.line,
                                                parser->current_token.column,
                                                parser->lexer->filename);
            ast_node_add_child(call, primary);
            
            while (!parser_match(parser, TOKEN_PUNCTUATOR, ")")) {
                ast_node_add_child(call, parser_parse_expression(parser));
                if (parser_match(parser, TOKEN_PUNCTUATOR, ",")) {
                    parser_advance(parser);
                }
            }
            
            parser_advance(parser);
            primary = call;
        } else if (parser_match(parser, TOKEN_PUNCTUATOR, "[")) {
            parser_advance(parser);
            ast_node_t* access = ast_node_create(AST_ARRAY_ACCESS, "[]",
                                                   parser->current_token.line,
                                                   parser->current_token.column,
                                                   parser->lexer->filename);
            ast_node_add_child(access, primary);
            ast_node_add_child(access, parser_parse_expression(parser));
            
            if (parser_match(parser, TOKEN_PUNCTUATOR, "]")) {
                parser_advance(parser);
            }
            
            primary = access;
        } else if (parser_match(parser, TOKEN_OPERATOR, ".")) {
            parser_advance(parser);
            ast_node_t* member = ast_node_create(AST_MEMBER_ACCESS, ".",
                                                  parser->current_token.line,
                                                  parser->current_token.column,
                                                  parser->lexer->filename);
            ast_node_add_child(member, primary);
            ast_node_add_child(member, parser_parse_identifier(parser));
            primary = member;
        } else if (parser_match(parser, TOKEN_OPERATOR, "->")) {
            parser_advance(parser);
            ast_node_t* member = ast_node_create(AST_MEMBER_ACCESS, "->",
                                                  parser->current_token.line,
                                                  parser->current_token.column,
                                                  parser->lexer->filename);
            ast_node_add_child(member, primary);
            ast_node_add_child(member, parser_parse_identifier(parser));
            primary = member;
        } else {
            break;
        }
    }
    
    return primary;
}

ast_node_t* parser_parse_primary(parser_t* parser) {
    if (parser_match_type(parser, TOKEN_NUMBER)) {
        ast_node_t* node = ast_node_create(AST_LITERAL, parser->current_token.value,
                                             parser->current_token.line,
                                             parser->current_token.column,
                                             parser->lexer->filename);
        parser_advance(parser);
        return node;
    }
    
    if (parser_match_type(parser, TOKEN_STRING)) {
        ast_node_t* node = ast_node_create(AST_LITERAL, parser->current_token.value,
                                             parser->current_token.line,
                                             parser->current_token.column,
                                             parser->lexer->filename);
        parser_advance(parser);
        return node;
    }
    
    if (parser_match_type(parser, TOKEN_CHAR)) {
        ast_node_t* node = ast_node_create(AST_LITERAL, parser->current_token.value,
                                             parser->current_token.line,
                                             parser->current_token.column,
                                             parser->lexer->filename);
        parser_advance(parser);
        return node;
    }
    
    if (parser_match(parser, TOKEN_KEYWORD, "true") || 
        parser_match(parser, TOKEN_KEYWORD, "false")) {
        ast_node_t* node = ast_node_create(AST_LITERAL, parser->current_token.value,
                                             parser->current_token.line,
                                             parser->current_token.column,
                                             parser->lexer->filename);
        parser_advance(parser);
        return node;
    }
    
    if (parser_match(parser, TOKEN_KEYWORD, "null")) {
        ast_node_t* node = ast_node_create(AST_LITERAL, "null",
                                             parser->current_token.line,
                                             parser->current_token.column,
                                             parser->lexer->filename);
        parser_advance(parser);
        return node;
    }
    
    if (parser_match(parser, TOKEN_KEYWORD, "new")) {
        return parser_parse_new_expr(parser);
    }
    
    if (parser_match(parser, TOKEN_PUNCTUATOR, "(")) {
        parser_advance(parser);
        ast_node_t* expr = parser_parse_expression(parser);
        if (parser_match(parser, TOKEN_PUNCTUATOR, ")")) {
            parser_advance(parser);
        }
        return expr;
    }
    
    if (parser_match_type(parser, TOKEN_IDENTIFIER)) {
        return parser_parse_identifier(parser);
    }
    
    return NULL;
}

ast_node_t* parser_parse_identifier(parser_t* parser) {
    ast_node_t* node = ast_node_create(AST_IDENTIFIER, parser->current_token.value,
                                       parser->current_token.line,
                                       parser->current_token.column,
                                       parser->lexer->filename);
    parser_advance(parser);
    return node;
}

ast_node_t* parser_parse_new_expr(parser_t* parser) {
    ast_node_t* new_expr = ast_node_create(AST_NEW_EXPR, "new",
                                            parser->current_token.line,
                                            parser->current_token.column,
                                            parser->lexer->filename);
    parser_advance(parser);
    
    ast_node_add_child(new_expr, parser_parse_identifier(parser));
    
    if (parser_match(parser, TOKEN_PUNCTUATOR, "(")) {
        parser_advance(parser);
        
        while (!parser_match(parser, TOKEN_PUNCTUATOR, ")")) {
            ast_node_add_child(new_expr, parser_parse_expression(parser));
            if (parser_match(parser, TOKEN_PUNCTUATOR, ",")) {
                parser_advance(parser);
            }
        }
        
        parser_advance(parser);
    }
    
    return new_expr;
}

ast_node_t* parser_parse_block(parser_t* parser) {
    ast_node_t* block = ast_node_create(AST_BLOCK, "block",
                                        parser->current_token.line,
                                        parser->current_token.column,
                                        parser->lexer->filename);
    parser_advance(parser);
    
    while (!parser_match(parser, TOKEN_PUNCTUATOR, "}")) {
        if (parser_match(parser, TOKEN_KEYWORD, "if")) {
            ast_node_add_child(block, parser_parse_if_statement(parser));
        } else if (parser_match(parser, TOKEN_KEYWORD, "while")) {
            ast_node_add_child(block, parser_parse_while_statement(parser));
        } else if (parser_match(parser, TOKEN_KEYWORD, "for")) {
            ast_node_add_child(block, parser_parse_for_statement(parser));
        } else if (parser_match(parser, TOKEN_KEYWORD, "return")) {
            ast_node_add_child(block, parser_parse_return_statement(parser));
        } else if (parser_match(parser, TOKEN_KEYWORD, "try")) {
            ast_node_add_child(block, parser_parse_try_catch(parser));
        } else if (parser_match(parser, TOKEN_KEYWORD, "throw")) {
            ast_node_add_child(block, parser_parse_throw_stmt(parser));
        } else if (parser_match(parser, TOKEN_KEYWORD, "var") ||
                   parser_match(parser, TOKEN_KEYWORD, "const") ||
                   parser_match(parser, TOKEN_KEYWORD, "int") ||
                   parser_match(parser, TOKEN_KEYWORD, "long") ||
                   parser_match(parser, TOKEN_KEYWORD, "float") ||
                   parser_match(parser, TOKEN_KEYWORD, "double") ||
                   parser_match(parser, TOKEN_KEYWORD, "char") ||
                   parser_match(parser, TOKEN_KEYWORD, "string") ||
                   parser_match(parser, TOKEN_KEYWORD, "bool")) {
            ast_node_add_child(block, parser_parse_variable_decl(parser));
        } else {
            ast_node_t* expr = parser_parse_expression(parser);
            if (expr) {
                ast_node_add_child(block, expr);
            }
            if (parser_match(parser, TOKEN_PUNCTUATOR, ";")) {
                parser_advance(parser);
            }
        }
    }
    
    parser_advance(parser);
    return block;
}

ast_node_t* parser_parse_if_statement(parser_t* parser) {
    ast_node_t* if_stmt = ast_node_create(AST_IF_STATEMENT, "if",
                                           parser->current_token.line,
                                           parser->current_token.column,
                                           parser->lexer->filename);
    parser_advance(parser);
    
    if (parser_match(parser, TOKEN_PUNCTUATOR, "(")) {
        parser_advance(parser);
        ast_node_add_child(if_stmt, parser_parse_expression(parser));
        if (parser_match(parser, TOKEN_PUNCTUATOR, ")")) {
            parser_advance(parser);
        }
    }
    
    if (parser_match(parser, TOKEN_PUNCTUATOR, "{")) {
        ast_node_add_child(if_stmt, parser_parse_block(parser));
    } else {
        ast_node_t* stmt = parser_parse_expression(parser);
        if (stmt) {
            ast_node_add_child(if_stmt, stmt);
        }
        if (parser_match(parser, TOKEN_PUNCTUATOR, ";")) {
            parser_advance(parser);
        }
    }
    
    if (parser_match(parser, TOKEN_KEYWORD, "else")) {
        parser_advance(parser);
        if (parser_match(parser, TOKEN_PUNCTUATOR, "{")) {
            ast_node_add_child(if_stmt, parser_parse_block(parser));
        } else {
            ast_node_t* stmt = parser_parse_expression(parser);
            if (stmt) {
                ast_node_add_child(if_stmt, stmt);
            }
        }
    }
    
    return if_stmt;
}

ast_node_t* parser_parse_while_statement(parser_t* parser) {
    ast_node_t* while_stmt = ast_node_create(AST_WHILE_STATEMENT, "while",
                                              parser->current_token.line,
                                              parser->current_token.column,
                                              parser->lexer->filename);
    parser_advance(parser);
    
    if (parser_match(parser, TOKEN_PUNCTUATOR, "(")) {
        parser_advance(parser);
        ast_node_add_child(while_stmt, parser_parse_expression(parser));
        if (parser_match(parser, TOKEN_PUNCTUATOR, ")")) {
            parser_advance(parser);
        }
    }
    
    if (parser_match(parser, TOKEN_PUNCTUATOR, "{")) {
        ast_node_add_child(while_stmt, parser_parse_block(parser));
    }
    
    return while_stmt;
}

ast_node_t* parser_parse_for_statement(parser_t* parser) {
    ast_node_t* for_stmt = ast_node_create(AST_FOR_STATEMENT, "for",
                                            parser->current_token.line,
                                            parser->current_token.column,
                                            parser->lexer->filename);
    parser_advance(parser);
    
    if (parser_match(parser, TOKEN_PUNCTUATOR, "(")) {
        parser_advance(parser);
        
        if (!parser_match(parser, TOKEN_PUNCTUATOR, ";")) {
            ast_node_add_child(for_stmt, parser_parse_variable_decl(parser));
        }
        
        if (parser_match(parser, TOKEN_PUNCTUATOR, ";")) {
            parser_advance(parser);
        }
        
        if (!parser_match(parser, TOKEN_PUNCTUATOR, ";")) {
            ast_node_add_child(for_stmt, parser_parse_expression(parser));
        }
        
        if (parser_match(parser, TOKEN_PUNCTUATOR, ";")) {
            parser_advance(parser);
        }
        
        if (!parser_match(parser, TOKEN_PUNCTUATOR, ")")) {
            ast_node_add_child(for_stmt, parser_parse_expression(parser));
        }
        
        if (parser_match(parser, TOKEN_PUNCTUATOR, ")")) {
            parser_advance(parser);
        }
    }
    
    if (parser_match(parser, TOKEN_PUNCTUATOR, "{")) {
        ast_node_add_child(for_stmt, parser_parse_block(parser));
    }
    
    return for_stmt;
}

ast_node_t* parser_parse_return_statement(parser_t* parser) {
    ast_node_t* return_stmt = ast_node_create(AST_RETURN_STATEMENT, "return",
                                               parser->current_token.line,
                                               parser->current_token.column,
                                               parser->lexer->filename);
    parser_advance(parser);
    
    if (!parser_match(parser, TOKEN_PUNCTUATOR, ";")) {
        ast_node_add_child(return_stmt, parser_parse_expression(parser));
    }
    
    if (parser_match(parser, TOKEN_PUNCTUATOR, ";")) {
        parser_advance(parser);
    }
    
    return return_stmt;
}

ast_node_t* parser_parse_try_catch(parser_t* parser) {
    ast_node_t* try_catch = ast_node_create(AST_TRY_CATCH, "try-catch",
                                             parser->current_token.line,
                                             parser->current_token.column,
                                             parser->lexer->filename);
    parser_advance(parser);
    
    if (parser_match(parser, TOKEN_PUNCTUATOR, "{")) {
        ast_node_add_child(try_catch, parser_parse_block(parser));
    }
    
    if (parser_match(parser, TOKEN_KEYWORD, "catch")) {
        parser_advance(parser);
        
        if (parser_match(parser, TOKEN_PUNCTUATOR, "(")) {
            parser_advance(parser);
            ast_node_add_child(try_catch, parser_parse_variable_decl(parser));
            if (parser_match(parser, TOKEN_PUNCTUATOR, ")")) {
                parser_advance(parser);
            }
        }
        
        if (parser_match(parser, TOKEN_PUNCTUATOR, "{")) {
            ast_node_add_child(try_catch, parser_parse_block(parser));
        }
    }
    
    if (parser_match(parser, TOKEN_KEYWORD, "finally")) {
        parser_advance(parser);
        if (parser_match(parser, TOKEN_PUNCTUATOR, "{")) {
            ast_node_add_child(try_catch, parser_parse_block(parser));
        }
    }
    
    return try_catch;
}

ast_node_t* parser_parse_throw_stmt(parser_t* parser) {
    ast_node_t* throw_stmt = ast_node_create(AST_THROW_STMT, "throw",
                                              parser->current_token.line,
                                              parser->current_token.column,
                                              parser->lexer->filename);
    parser_advance(parser);
    
    ast_node_add_child(throw_stmt, parser_parse_expression(parser));
    
    if (parser_match(parser, TOKEN_PUNCTUATOR, ";")) {
        parser_advance(parser);
    }
    
    return throw_stmt;
}

ast_node_t* parser_parse_class_decl(parser_t* parser) {
    ast_node_t* class_node = ast_node_create(AST_CLASS_DECL, "class",
                                             parser->current_token.line,
                                             parser->current_token.column,
                                             parser->lexer->filename);
    parser_advance(parser);
    
    ast_node_add_child(class_node, parser_parse_identifier(parser));
    
    if (parser_match(parser, TOKEN_KEYWORD, "extends")) {
        parser_advance(parser);
        ast_node_add_child(class_node, parser_parse_identifier(parser));
    }
    
    if (parser_match(parser, TOKEN_KEYWORD, "implements")) {
        parser_advance(parser);
        ast_node_add_child(class_node, parser_parse_identifier(parser));
    }
    
    if (parser_match(parser, TOKEN_PUNCTUATOR, "{")) {
        parser_advance(parser);
        
        while (!parser_match(parser, TOKEN_PUNCTUATOR, "}")) {
            if (parser_match(parser, TOKEN_KEYWORD, "public") ||
                parser_match(parser, TOKEN_KEYWORD, "private") ||
                parser_match(parser, TOKEN_KEYWORD, "protected")) {
                parser_advance(parser);
            }
            
            if (parser_match(parser, TOKEN_KEYWORD, "void") ||
                parser_match(parser, TOKEN_KEYWORD, "int") ||
                parser_match(parser, TOKEN_KEYWORD, "long") ||
                parser_match(parser, TOKEN_KEYWORD, "float") ||
                parser_match(parser, TOKEN_KEYWORD, "double") ||
                parser_match(parser, TOKEN_KEYWORD, "char") ||
                parser_match(parser, TOKEN_KEYWORD, "string") ||
                parser_match(parser, TOKEN_KEYWORD, "bool")) {
                ast_node_add_child(class_node, parser_parse_method_decl(parser));
            } else if (parser_match_type(parser, TOKEN_IDENTIFIER)) {
                ast_node_add_child(class_node, parser_parse_property_decl(parser));
            } else {
                parser_advance(parser);
            }
        }
        
        parser_advance(parser);
    }
    
    return class_node;
}

ast_node_t* parser_parse_method_decl(parser_t* parser) {
    ast_node_t* method = ast_node_create(AST_METHOD_DECL, "method",
                                         parser->current_token.line,
                                         parser->current_token.column,
                                         parser->lexer->filename);
    
    ast_node_add_child(method, ast_node_create(AST_LITERAL, parser->current_token.value,
                                               parser->current_token.line,
                                               parser->current_token.column,
                                               parser->lexer->filename));
    parser_advance(parser);
    
    ast_node_add_child(method, parser_parse_identifier(parser));
    
    if (parser_match(parser, TOKEN_PUNCTUATOR, "(")) {
        parser_advance(parser);
        
        while (!parser_match(parser, TOKEN_PUNCTUATOR, ")")) {
            ast_node_add_child(method, parser_parse_variable_decl(parser));
            if (parser_match(parser, TOKEN_PUNCTUATOR, ",")) {
                parser_advance(parser);
            }
        }
        
        parser_advance(parser);
    }
    
    if (parser_match(parser, TOKEN_PUNCTUATOR, "{")) {
        ast_node_add_child(method, parser_parse_block(parser));
    } else if (parser_match(parser, TOKEN_PUNCTUATOR, ";")) {
        parser_advance(parser);
    }
    
    return method;
}

ast_node_t* parser_parse_property_decl(parser_t* parser) {
    ast_node_t* prop = ast_node_create(AST_PROPERTY_DECL, "property",
                                        parser->current_token.line,
                                        parser->current_token.column,
                                        parser->lexer->filename);
    
    ast_node_add_child(prop, parser_parse_identifier(parser));
    
    if (parser_match(parser, TOKEN_OPERATOR, "=")) {
        parser_advance(parser);
        ast_node_add_child(prop, parser_parse_expression(parser));
    }
    
    if (parser_match(parser, TOKEN_PUNCTUATOR, ";")) {
        parser_advance(parser);
    }
    
    return prop;
}

ast_node_t* parser_parse_struct_decl(parser_t* parser) {
    ast_node_t* struct_node = ast_node_create(AST_STRUCT_DECL, "struct",
                                               parser->current_token.line,
                                               parser->current_token.column,
                                               parser->lexer->filename);
    parser_advance(parser);
    
    ast_node_add_child(struct_node, parser_parse_identifier(parser));
    
    if (parser_match(parser, TOKEN_PUNCTUATOR, "{")) {
        parser_advance(parser);
        
        while (!parser_match(parser, TOKEN_PUNCTUATOR, "}")) {
            ast_node_add_child(struct_node, parser_parse_variable_decl(parser));
        }
        
        parser_advance(parser);
    }
    
    return struct_node;
}

ast_node_t* parser_parse_enum_decl(parser_t* parser) {
    ast_node_t* enum_node = ast_node_create(AST_ENUM_DECL, "enum",
                                            parser->current_token.line,
                                            parser->current_token.column,
                                            parser->lexer->filename);
    parser_advance(parser);
    
    ast_node_add_child(enum_node, parser_parse_identifier(parser));
    
    if (parser_match(parser, TOKEN_PUNCTUATOR, "{")) {
        parser_advance(parser);
        
        while (!parser_match(parser, TOKEN_PUNCTUATOR, "}")) {
            ast_node_add_child(enum_node, parser_parse_identifier(parser));
            if (parser_match(parser, TOKEN_PUNCTUATOR, ",")) {
                parser_advance(parser);
            }
        }
        
        parser_advance(parser);
    }
    
    return enum_node;
}

ast_node_t* parser_parse_array_decl(parser_t* parser) {
    ast_node_t* array = ast_node_create(AST_ARRAY_DECL, "array",
                                        parser->current_token.line,
                                        parser->current_token.column,
                                        parser->lexer->filename);
    
    if (parser_match(parser, TOKEN_KEYWORD, "array")) {
        parser_advance(parser);
    }
    
    if (parser_match(parser, TOKEN_PUNCTUATOR, "<")) {
        parser_advance(parser);
        ast_node_add_child(array, parser_parse_identifier(parser));
        if (parser_match(parser, TOKEN_PUNCTUATOR, ">")) {
            parser_advance(parser);
        }
    }
    
    return array;
}

ast_node_t* parser_parse_cast_expr(parser_t* parser) {
    ast_node_t* cast = ast_node_create(AST_CAST_EXPR, "cast",
                                        parser->current_token.line,
                                        parser->current_token.column,
                                        parser->lexer->filename);
    
    if (parser_match(parser, TOKEN_PUNCTUATOR, "(")) {
        parser_advance(parser);
        ast_node_add_child(cast, parser_parse_identifier(parser));
        if (parser_match(parser, TOKEN_PUNCTUATOR, ")")) {
            parser_advance(parser);
        }
    }
    
    ast_node_add_child(cast, parser_parse_expression(parser));
    
    return cast;
}

ast_node_t* parser_parse_ternary_expr(parser_t* parser) {
    return parser_parse_expression(parser);
}

ast_node_t* parser_parse_function_call(parser_t* parser) {
    ast_node_t* call = ast_node_create(AST_FUNCTION_CALL, "call",
                                       parser->current_token.line,
                                       parser->current_token.column,
                                       parser->lexer->filename);
    
    ast_node_add_child(call, parser_parse_identifier(parser));
    
    if (parser_match(parser, TOKEN_PUNCTUATOR, "(")) {
        parser_advance(parser);
        
        while (!parser_match(parser, TOKEN_PUNCTUATOR, ")")) {
            ast_node_add_child(call, parser_parse_expression(parser));
            if (parser_match(parser, TOKEN_PUNCTUATOR, ",")) {
                parser_advance(parser);
            }
        }
        
        parser_advance(parser);
    }
    
    return call;
}

ast_node_t* parser_parse_assignment(parser_t* parser) {
    ast_node_t* assign = ast_node_create(AST_ASSIGNMENT, "=",
                                          parser->current_token.line,
                                          parser->current_token.column,
                                          parser->lexer->filename);
    
    ast_node_add_child(assign, parser_parse_expression(parser));
    
    if (parser_match(parser, TOKEN_OPERATOR, "=")) {
        parser_advance(parser);
        ast_node_add_child(assign, parser_parse_expression(parser));
    }
    
    return assign;
}

ast_node_t* parser_parse_array_access(parser_t* parser) {
    ast_node_t* access = ast_node_create(AST_ARRAY_ACCESS, "[]",
                                         parser->current_token.line,
                                         parser->current_token.column,
                                         parser->lexer->filename);
    
    ast_node_add_child(access, parser_parse_expression(parser));
    
    if (parser_match(parser, TOKEN_PUNCTUATOR, "[")) {
        parser_advance(parser);
        ast_node_add_child(access, parser_parse_expression(parser));
        if (parser_match(parser, TOKEN_PUNCTUATOR, "]")) {
            parser_advance(parser);
        }
    }
    
    return access;
}

ast_node_t* parser_parse_member_access(parser_t* parser) {
    ast_node_t* access = ast_node_create(AST_MEMBER_ACCESS, ".",
                                          parser->current_token.line,
                                          parser->current_token.column,
                                          parser->lexer->filename);
    
    ast_node_add_child(access, parser_parse_expression(parser));
    
    if (parser_match(parser, TOKEN_OPERATOR, ".")) {
        parser_advance(parser);
        ast_node_add_child(access, parser_parse_identifier(parser));
    }
    
    return access;
}

ast_node_t* parser_parse_literal(parser_t* parser) {
    return parser_parse_primary(parser);
}

void ast_print(ast_node_t* node, int indent) {
    if (!node) return;
    
    for (int i = 0; i < indent; i++) {
        printf("  ");
    }
    
    printf("%s", ast_type_to_string(node->type));
    if (node->value) {
        printf(": %s", node->value);
    }
    printf(" (%d:%d)\n", node->line, node->column);
    
    for (int i = 0; i < node->child_count; i++) {
        ast_print(node->children[i], indent + 1);
    }
}