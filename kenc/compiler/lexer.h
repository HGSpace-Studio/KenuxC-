#ifndef KENC_LEXER_H
#define KENC_LEXER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum {
    TOKEN_EOF,
    TOKEN_IDENTIFIER,
    TOKEN_KEYWORD,
    TOKEN_NUMBER,
    TOKEN_STRING,
    TOKEN_CHAR,
    TOKEN_OPERATOR,
    TOKEN_PUNCTUATOR,
    TOKEN_COMMENT,
    TOKEN_WHITESPACE,
    TOKEN_NEWLINE,
    TOKEN_UNKNOWN
} token_type_t;

typedef struct {
    token_type_t type;
    char* value;
    int line;
    int column;
    char* filename;
} token_t;

typedef struct {
    char* source;
    int position;
    int line;
    int column;
    char* filename;
    token_t current_token;
} lexer_t;

lexer_t* lexer_create(const char* source, const char* filename);
void lexer_free(lexer_t* lexer);

token_t lexer_next_token(lexer_t* lexer);
token_t lexer_peek_token(lexer_t* lexer, int offset);
void lexer_advance(lexer_t* lexer);

int lexer_is_keyword(const char* value);
int lexer_is_operator(const char* value);
int lexer_is_punctuator(const char* value);

char* token_type_to_string(token_type_t type);
void token_free(token_t* token);

#endif