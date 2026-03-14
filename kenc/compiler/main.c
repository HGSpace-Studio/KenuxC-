#define _POSIX_C_SOURCE 200809L

#include "lexer.h"
#include "parser.h"
#include "codegen.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#define KENC_VERSION "1.0.0"

typedef struct {
    char* input_file;
    char* output_file;
    int verbose;
    int debug;
    int optimize;
    int emit_ast;
    int emit_tokens;
} compiler_options_t;

void print_version(void) {
    printf("KenC# Compiler v%s\n", KENC_VERSION);
    printf("KenuxOS Programming Language Compiler\n");
    printf("Copyright (c) 2026 KenuxOS Project\n");
}

void print_help(void) {
    printf("Usage: kenc [options] <input-file>\n");
    printf("\nOptions:\n");
    printf("  -o <file>      Specify output file (default: input.ken)\n");
    printf("  -v, --verbose  Enable verbose output\n");
    printf("  -d, --debug    Enable debug mode\n");
    printf("  -O<level>      Set optimization level (0-3)\n");
    printf("  --emit-ast     Emit AST to stdout\n");
    printf("  --emit-tokens  Emit tokens to stdout\n");
    printf("  -h, --help     Show this help message\n");
    printf("  --version      Show version information\n");
    printf("\nExamples:\n");
    printf("  kenc hello.kenp\n");
    printf("  kenc -o output.ken hello.kenp\n");
    printf("  kenc -v -O2 program.kenp\n");
}

char* read_file(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, "Error: Cannot open file '%s'\n", filename);
        return NULL;
    }
    
    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    fseek(file, 0, SEEK_SET);
    
    char* content = (char*)malloc(size + 1);
    if (!content) {
        fclose(file);
        return NULL;
    }
    
    size_t read_size = fread(content, 1, size, file);
    content[read_size] = '\0';
    
    fclose(file);
    return content;
}

int write_file(const char* filename, const char* content) {
    FILE* file = fopen(filename, "w");
    if (!file) {
        fprintf(stderr, "Error: Cannot create file '%s'\n", filename);
        return -1;
    }
    
    fprintf(file, "%s", content);
    fclose(file);
    
    return 0;
}

char* get_output_filename(const char* input_file) {
    const char* dot = strrchr(input_file, '.');
    if (!dot) {
        return strdup(input_file);
    }
    
    size_t len = dot - input_file;
    char* output = (char*)malloc(len + 5);
    strncpy(output, input_file, len);
    strcpy(output + len, ".ken");
    
    return output;
}

void print_tokens(lexer_t* lexer) {
    printf("Tokens:\n");
    printf("--------\n");
    
    token_t token;
    do {
        token = lexer_next_token(lexer);
        printf("%-15s %-20s %4d:%-4d\n", 
               token_type_to_string(token.type),
               token.value ? token.value : "",
               token.line,
               token.column);
    } while (token.type != TOKEN_EOF);
}

int compile(compiler_options_t* options) {
    if (options->verbose) {
        printf("Compiling: %s\n", options->input_file);
    }
    
    char* source = read_file(options->input_file);
    if (!source) {
        return 1;
    }
    
    if (options->verbose) {
        printf("Source file loaded: %zu bytes\n", strlen(source));
    }
    
    lexer_t* lexer = lexer_create(source, options->input_file);
    if (!lexer) {
        free(source);
        return 1;
    }
    
    if (options->emit_tokens) {
        print_tokens(lexer);
        lexer_free(lexer);
        free(source);
        return 0;
    }
    
    parser_t* parser = parser_create(lexer);
    if (!parser) {
        lexer_free(lexer);
        free(source);
        return 1;
    }
    
    ast_node_t* ast = parser_parse(parser);
    if (!ast) {
        fprintf(stderr, "Error: Failed to parse source file\n");
        parser_free(parser);
        lexer_free(lexer);
        free(source);
        return 1;
    }
    
    if (options->emit_ast) {
        printf("AST:\n");
        printf("----\n");
        ast_print(ast, 0);
        parser_free(parser);
        lexer_free(lexer);
        free(source);
        return 0;
    }
    
    if (options->verbose) {
        printf("AST generated successfully\n");
    }
    
    char* code = codegen_generate(ast);
    if (!code) {
        fprintf(stderr, "Error: Failed to generate code\n");
        ast_node_free(ast);
        parser_free(parser);
        lexer_free(lexer);
        free(source);
        return 1;
    }
    
    if (options->verbose) {
        printf("Code generated successfully\n");
    }
    
    if (write_file(options->output_file, code) != 0) {
        free(code);
        ast_node_free(ast);
        parser_free(parser);
        lexer_free(lexer);
        free(source);
        return 1;
    }
    
    if (options->verbose) {
        printf("Output written to: %s\n", options->output_file);
        printf("Compilation successful!\n");
    }
    
    free(code);
    ast_node_free(ast);
    parser_free(parser);
    lexer_free(lexer);
    free(source);
    
    return 0;
}

int main(int argc, char** argv) {
    if (argc < 2) {
        print_help();
        return 1;
    }
    
    compiler_options_t options = {
        .input_file = NULL,
        .output_file = NULL,
        .verbose = 0,
        .debug = 0,
        .optimize = 0,
        .emit_ast = 0,
        .emit_tokens = 0
    };
    
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            print_help();
            return 0;
        }
        
        if (strcmp(argv[i], "--version") == 0) {
            print_version();
            return 0;
        }
        
        if (strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--verbose") == 0) {
            options.verbose = 1;
            continue;
        }
        
        if (strcmp(argv[i], "-d") == 0 || strcmp(argv[i], "--debug") == 0) {
            options.debug = 1;
            options.verbose = 1;
            continue;
        }
        
        if (strcmp(argv[i], "--emit-ast") == 0) {
            options.emit_ast = 1;
            continue;
        }
        
        if (strcmp(argv[i], "--emit-tokens") == 0) {
            options.emit_tokens = 1;
            continue;
        }
        
        if (strncmp(argv[i], "-O", 2) == 0) {
            options.optimize = argv[i][2] - '0';
            if (options.optimize < 0 || options.optimize > 3) {
                options.optimize = 0;
            }
            continue;
        }
        
        if (strcmp(argv[i], "-o") == 0) {
            if (i + 1 < argc) {
                options.output_file = argv[++i];
            }
            continue;
        }
        
        if (argv[i][0] != '-') {
            options.input_file = argv[i];
        }
    }
    
    if (!options.input_file) {
        fprintf(stderr, "Error: No input file specified\n");
        print_help();
        return 1;
    }
    
    if (!options.output_file) {
        options.output_file = get_output_filename(options.input_file);
    }
    
    return compile(&options);
}