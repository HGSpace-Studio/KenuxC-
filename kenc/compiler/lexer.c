#include "lexer.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>

static const char* keywords[] = {
    "int", "long", "float", "double", "char", "string", "bool", "void",
    "if", "else", "for", "while", "do", "switch", "case", "default", "break", "continue",
    "return", "goto", "struct", "enum", "class", "extends", "implements", "interface",
    "public", "private", "protected", "static", "const", "var", "true", "false", "null",
    "new", "delete", "this", "super", "abstract", "virtual", "override",
    "try", "catch", "finally", "throw", "throws",
    "import", "export", "module", "namespace", "using",
    "typedef", "sizeof", "typeof", "instanceof",
    "async", "await", "yield",
    "template", "typename",
    "operator", "explicit", "implicit",
    "get", "set", "value",
    "in", "out", "ref",
    "lock", "unlock",
    "where", "select", "from", "group", "into", "orderby", "join",
    "partial", "sealed", "unsafe", "fixed", "stackalloc",
    "checked", "unchecked",
    "volatile", "readonly",
    "extern", "internal",
    "delegate", "event",
    "base",
    "as", "is",
    "__asm", "__attribute__", "__builtin", "__cdecl", "__stdcall",
    NULL
};

static const char* operators[] = {
    "+", "-", "*", "/", "%", "++", "--",
    "==", "!=", "<", ">", "<=", ">=",
    "&&", "||", "!",
    "&", "|", "^", "~", "<<", ">>",
    "=", "+=", "-=", "*=", "/=", "%=", "&=", "|=", "^=", "<<=", ">>=",
    "?", ":", "??",
    "->", ".", ".*", "->*",
    "::",
    "=>",
    NULL
};

static const char* punctuators[] = {
    "(", ")", "[", "]", "{", "}",
    ";", ",", "...",
    "@", "#",
    NULL
};

lexer_t* lexer_create(const char* source, const char* filename) {
    lexer_t* lexer = (lexer_t*)malloc(sizeof(lexer_t));
    if (!lexer) return NULL;
    
    lexer->source = strdup(source);
    lexer->position = 0;
    lexer->line = 1;
    lexer->column = 1;
    lexer->filename = strdup(filename);
    
    lexer->current_token.type = TOKEN_UNKNOWN;
    lexer->current_token.value = NULL;
    lexer->current_token.line = 0;
    lexer->current_token.column = 0;
    lexer->current_token.filename = NULL;
    
    return lexer;
}

void lexer_free(lexer_t* lexer) {
    if (lexer) {
        if (lexer->source) free(lexer->source);
        if (lexer->filename) free(lexer->filename);
        free(lexer);
    }
}

static char lexer_current_char(lexer_t* lexer) {
    if (lexer->position >= strlen(lexer->source)) {
        return '\0';
    }
    return lexer->source[lexer->position];
}

static char lexer_peek_char(lexer_t* lexer, int offset) {
    int pos = lexer->position + offset;
    if (pos >= strlen(lexer->source)) {
        return '\0';
    }
    return lexer->source[pos];
}

static void lexer_advance_char(lexer_t* lexer) {
    if (lexer->position < strlen(lexer->source)) {
        if (lexer->source[lexer->position] == '\n') {
            lexer->line++;
            lexer->column = 1;
        } else {
            lexer->column++;
        }
        lexer->position++;
    }
}

static void lexer_skip_whitespace(lexer_t* lexer) {
    while (isspace(lexer_current_char(lexer))) {
        lexer_advance_char(lexer);
    }
}

static void lexer_skip_comment(lexer_t* lexer) {
    if (lexer_current_char(lexer) == '/' && lexer_peek_char(lexer, 1) == '/') {
        while (lexer_current_char(lexer) != '\n' && lexer_current_char(lexer) != '\0') {
            lexer_advance_char(lexer);
        }
    } else if (lexer_current_char(lexer) == '/' && lexer_peek_char(lexer, 1) == '*') {
        lexer_advance_char(lexer);
        lexer_advance_char(lexer);
        while (!(lexer_current_char(lexer) == '*' && lexer_peek_char(lexer, 1) == '/') && 
               lexer_current_char(lexer) != '\0') {
            lexer_advance_char(lexer);
        }
        if (lexer_current_char(lexer) == '*') {
            lexer_advance_char(lexer);
            lexer_advance_char(lexer);
        }
    }
}

static token_t lexer_read_number(lexer_t* lexer) {
    token_t token;
    token.type = TOKEN_NUMBER;
    token.line = lexer->line;
    token.column = lexer->column;
    token.filename = strdup(lexer->filename);
    
    char buffer[256];
    int pos = 0;
    
    while (isdigit(lexer_current_char(lexer))) {
        buffer[pos++] = lexer_current_char(lexer);
        lexer_advance_char(lexer);
    }
    
    if (lexer_current_char(lexer) == '.') {
        buffer[pos++] = lexer_current_char(lexer);
        lexer_advance_char(lexer);
        
        while (isdigit(lexer_current_char(lexer))) {
            buffer[pos++] = lexer_current_char(lexer);
            lexer_advance_char(lexer);
        }
    }
    
    if (lexer_current_char(lexer) == 'e' || lexer_current_char(lexer) == 'E') {
        buffer[pos++] = lexer_current_char(lexer);
        lexer_advance_char(lexer);
        
        if (lexer_current_char(lexer) == '+' || lexer_current_char(lexer) == '-') {
            buffer[pos++] = lexer_current_char(lexer);
            lexer_advance_char(lexer);
        }
        
        while (isdigit(lexer_current_char(lexer))) {
            buffer[pos++] = lexer_current_char(lexer);
            lexer_advance_char(lexer);
        }
    }
    
    buffer[pos] = '\0';
    token.value = strdup(buffer);
    
    return token;
}

static token_t lexer_read_string(lexer_t* lexer) {
    token_t token;
    token.type = TOKEN_STRING;
    token.line = lexer->line;
    token.column = lexer->column;
    token.filename = strdup(lexer->filename);
    
    char delimiter = lexer_current_char(lexer);
    lexer_advance_char(lexer);
    
    char buffer[4096];
    int pos = 0;
    
    while (lexer_current_char(lexer) != delimiter && lexer_current_char(lexer) != '\0') {
        if (lexer_current_char(lexer) == '\\') {
            lexer_advance_char(lexer);
            switch (lexer_current_char(lexer)) {
                case 'n': buffer[pos++] = '\n'; break;
                case 't': buffer[pos++] = '\t'; break;
                case 'r': buffer[pos++] = '\r'; break;
                case 'b': buffer[pos++] = '\b'; break;
                case 'f': buffer[pos++] = '\f'; break;
                case '\\': buffer[pos++] = '\\'; break;
                case '\'': buffer[pos++] = '\''; break;
                case '\"': buffer[pos++] = '\"'; break;
                case '0': buffer[pos++] = '\0'; break;
                default: buffer[pos++] = lexer_current_char(lexer); break;
            }
        } else {
            buffer[pos++] = lexer_current_char(lexer);
        }
        lexer_advance_char(lexer);
    }
    
    if (lexer_current_char(lexer) == delimiter) {
        lexer_advance_char(lexer);
    }
    
    buffer[pos] = '\0';
    token.value = strdup(buffer);
    
    return token;
}

static token_t lexer_read_char(lexer_t* lexer) {
    token_t token;
    token.type = TOKEN_CHAR;
    token.line = lexer->line;
    token.column = lexer->column;
    token.filename = strdup(lexer->filename);
    
    lexer_advance_char(lexer);
    
    char buffer[8];
    int pos = 0;
    
    if (lexer_current_char(lexer) == '\\') {
        lexer_advance_char(lexer);
        switch (lexer_current_char(lexer)) {
            case 'n': buffer[pos++] = '\n'; break;
            case 't': buffer[pos++] = '\t'; break;
            case 'r': buffer[pos++] = '\r'; break;
            case 'b': buffer[pos++] = '\b'; break;
            case 'f': buffer[pos++] = '\f'; break;
            case '\\': buffer[pos++] = '\\'; break;
            case '\'': buffer[pos++] = '\''; break;
            case '\"': buffer[pos++] = '\"'; break;
            case '0': buffer[pos++] = '\0'; break;
            default: buffer[pos++] = lexer_current_char(lexer); break;
        }
        lexer_advance_char(lexer);
    } else {
        buffer[pos++] = lexer_current_char(lexer);
        lexer_advance_char(lexer);
    }
    
    if (lexer_current_char(lexer) == '\'') {
        lexer_advance_char(lexer);
    }
    
    buffer[pos] = '\0';
    token.value = strdup(buffer);
    
    return token;
}

static token_t lexer_read_identifier_or_keyword(lexer_t* lexer) {
    token_t token;
    token.line = lexer->line;
    token.column = lexer->column;
    token.filename = strdup(lexer->filename);
    
    char buffer[256];
    int pos = 0;
    
    while (isalnum(lexer_current_char(lexer)) || lexer_current_char(lexer) == '_') {
        buffer[pos++] = lexer_current_char(lexer);
        lexer_advance_char(lexer);
    }
    
    buffer[pos] = '\0';
    token.value = strdup(buffer);
    
    if (lexer_is_keyword(buffer)) {
        token.type = TOKEN_KEYWORD;
    } else {
        token.type = TOKEN_IDENTIFIER;
    }
    
    return token;
}

static token_t lexer_read_operator_or_punctuator(lexer_t* lexer) {
    token_t token;
    token.line = lexer->line;
    token.column = lexer->column;
    token.filename = strdup(lexer->filename);
    
    char buffer[8];
    int pos = 0;
    
    buffer[pos++] = lexer_current_char(lexer);
    lexer_advance_char(lexer);
    
    char two_char[3] = {buffer[0], lexer_current_char(lexer), '\0'};
    char three_char[4] = {buffer[0], lexer_current_char(lexer), lexer_peek_char(lexer, 1), '\0'};
    
    if (lexer_is_operator(three_char)) {
        buffer[pos++] = lexer_current_char(lexer);
        lexer_advance_char(lexer);
        buffer[pos++] = lexer_current_char(lexer);
        lexer_advance_char(lexer);
        buffer[pos] = '\0';
        token.value = strdup(buffer);
        token.type = TOKEN_OPERATOR;
    } else if (lexer_is_operator(two_char) || lexer_is_punctuator(two_char)) {
        buffer[pos++] = lexer_current_char(lexer);
        lexer_advance_char(lexer);
        buffer[pos] = '\0';
        token.value = strdup(buffer);
        if (lexer_is_operator(buffer)) {
            token.type = TOKEN_OPERATOR;
        } else {
            token.type = TOKEN_PUNCTUATOR;
        }
    } else {
        buffer[pos] = '\0';
        token.value = strdup(buffer);
        if (lexer_is_operator(buffer)) {
            token.type = TOKEN_OPERATOR;
        } else if (lexer_is_punctuator(buffer)) {
            token.type = TOKEN_PUNCTUATOR;
        } else {
            token.type = TOKEN_UNKNOWN;
        }
    }
    
    return token;
}

token_t lexer_next_token(lexer_t* lexer) {
    lexer_skip_whitespace(lexer);
    
    if (lexer_current_char(lexer) == '/' && 
        (lexer_peek_char(lexer, 1) == '/' || lexer_peek_char(lexer, 1) == '*')) {
        lexer_skip_comment(lexer);
        return lexer_next_token(lexer);
    }
    
    if (lexer_current_char(lexer) == '\0') {
        token_t token;
        token.type = TOKEN_EOF;
        token.value = strdup("");
        token.line = lexer->line;
        token.column = lexer->column;
        token.filename = strdup(lexer->filename);
        return token;
    }
    
    if (isdigit(lexer_current_char(lexer))) {
        return lexer_read_number(lexer);
    }
    
    if (lexer_current_char(lexer) == '"' || lexer_current_char(lexer) == '`') {
        return lexer_read_string(lexer);
    }
    
    if (lexer_current_char(lexer) == '\'') {
        return lexer_read_char(lexer);
    }
    
    if (isalpha(lexer_current_char(lexer)) || lexer_current_char(lexer) == '_') {
        return lexer_read_identifier_or_keyword(lexer);
    }
    
    if (lexer_current_char(lexer) == '\n') {
        token_t token;
        token.type = TOKEN_NEWLINE;
        token.value = strdup("\n");
        token.line = lexer->line;
        token.column = lexer->column;
        token.filename = strdup(lexer->filename);
        lexer_advance_char(lexer);
        return token;
    }
    
    return lexer_read_operator_or_punctuator(lexer);
}

token_t lexer_peek_token(lexer_t* lexer, int offset) {
    lexer_t temp = *lexer;
    token_t token;
    for (int i = 0; i <= offset; i++) {
        token = lexer_next_token(&temp);
    }
    return token;
}

void lexer_advance(lexer_t* lexer) {
    if (lexer->current_token.value) {
        free(lexer->current_token.value);
    }
    if (lexer->current_token.filename) {
        free(lexer->current_token.filename);
    }
    lexer->current_token = lexer_next_token(lexer);
}

int lexer_is_keyword(const char* value) {
    for (int i = 0; keywords[i] != NULL; i++) {
        if (strcmp(value, keywords[i]) == 0) {
            return 1;
        }
    }
    return 0;
}

int lexer_is_operator(const char* value) {
    for (int i = 0; operators[i] != NULL; i++) {
        if (strcmp(value, operators[i]) == 0) {
            return 1;
        }
    }
    return 0;
}

int lexer_is_punctuator(const char* value) {
    for (int i = 0; punctuators[i] != NULL; i++) {
        if (strcmp(value, punctuators[i]) == 0) {
            return 1;
        }
    }
    return 0;
}

char* token_type_to_string(token_type_t type) {
    switch (type) {
        case TOKEN_EOF: return "EOF";
        case TOKEN_IDENTIFIER: return "IDENTIFIER";
        case TOKEN_KEYWORD: return "KEYWORD";
        case TOKEN_NUMBER: return "NUMBER";
        case TOKEN_STRING: return "STRING";
        case TOKEN_CHAR: return "CHAR";
        case TOKEN_OPERATOR: return "OPERATOR";
        case TOKEN_PUNCTUATOR: return "PUNCTUATOR";
        case TOKEN_COMMENT: return "COMMENT";
        case TOKEN_WHITESPACE: return "WHITESPACE";
        case TOKEN_NEWLINE: return "NEWLINE";
        default: return "UNKNOWN";
    }
}

void token_free(token_t* token) {
    if (token) {
        if (token->value) free(token->value);
        if (token->filename) free(token->filename);
        token->value = NULL;
        token->filename = NULL;
    }
}