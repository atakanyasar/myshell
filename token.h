#ifndef TOKEN_H
#define TOKEN_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stddef.h>


typedef enum TokenType {
    TOKEN_EOF,
    TOKEN_ARG,
    TOKEN_COMMAND,
    TOKEN_ASSIGN,
    TOKEN_REDIRECT,
    TOKEN_APPEND,
    TOKEN_REVERSE_APPEND,
    TOKEN_BACKGROUND,
    TOKEN_ERROR
} TokenType;

typedef struct Token {
    TokenType type;
    char* value;
    struct Token* next_token;
} Token;

void free_token(Token* token) {
    if (token == NULL) {
        return;
    }
    if (token->next_token != NULL) {
        free_token(token->next_token);
    }
    free(token->value);
    free(token);
}


char* get_next_str(char* str) {
    int len = 0;
    while (str[len] != ' ' && str[len] != '\0' && str[len] != '>' && str[len] != '&' && str[len] != '=' && str[len] != '"' && str[len] != '\n') {
        len++;
    }
    return copy(str, len);
}

/*
 * @param depth parameter in tokenization to detect commands. 
 * The depth of a command starts at zero and is incremented by one in each tokenization step. 
 * Additionally, after encountering the '&' symbol, the depth is reset to zero. 
 * @param is_alias Each command is checked for an alias. 
 * If an alias is found, a recursive call of the tokenization function tokenizes the alias and replaces the command with alias tokens. 
 * After alias tokenization, the next tokens are connected to the tail of the linked list. 
 * Aliases of aliases are prevented by the is_alias parameter. 
*/
Token* tokenize(char* str, int* depth, int is_alias) {
    if (str == NULL) {
        return NULL;
    }

    Token* token = calloc(1, sizeof(Token));

    while (str[0] == ' ' || str[0] == '\t') {
        if (str[0] == '\0' || str[0] == '\n') {
            break;
        }
        str++;
    }

    if (*str == '\0' || *str == '\n') {
        token->type = TOKEN_EOF;
        token->value = copy(str, 1);
        token->next_token = NULL;
        return token;
    }

    if (*depth == 0) {
        char* next_str = get_next_str(str);
        char* alias = get_alias(next_str);
        if (!is_alias && alias != NULL) {
            Token* token_alias = tokenize(alias, depth, 1);
            Token* tail_alias = token_alias;
            while (tail_alias->next_token->type != TOKEN_EOF) {
                tail_alias = tail_alias->next_token;
            }

            free(tail_alias->next_token);
            tail_alias->next_token = tokenize(str + strlen(next_str), depth, 0);
            free_token(token);
            return token_alias;
        }
        else {
            token->value = next_str;
            token->type = TOKEN_COMMAND;
        }
    }
    else if (strncmp(str, "=", 1) == 0) {
        token->type = TOKEN_ASSIGN;
        token->value = copy(str, 1);
    }
    else if (strncmp(str, ">>>>", 4) == 0) {
        token->type = TOKEN_ERROR;
        token->value = copy(str, 4);
    }
    else if (strncmp(str, ">>>", 3) == 0) {
        token->type = TOKEN_REVERSE_APPEND;
        token->value = copy(str, 3);
    }
    else if (strncmp(str, ">>", 2) == 0) {
        token->type = TOKEN_APPEND;
        token->value = copy(str, 2);
    }
    else if (strncmp(str, ">", 1) == 0) {
        token->type = TOKEN_REDIRECT;
        token->value = copy(str, 1);
    }
    else if (strncmp(str, "&", 1) == 0) {
        token->type = TOKEN_BACKGROUND;
        token->value = copy(str, 1);
        *depth = -1;
    }
    else if (strncmp(str, "\"", 1) == 0) {
        token->type = TOKEN_ARG;
        str++;
        int len = 0;
        while (str[len] != '\"' && str[len] != '\0' && str[len] != '\n') {
            len++;
        }
        if (str[len] == '\0' || str[len] == '\n') {
            token->type = TOKEN_ERROR;
        }
        token->value = copy(str, len);
        str++;
    }
    else {
        token->type = TOKEN_ARG;
        token->value = get_next_str(str);
    }
    
    ++*depth;
    token->next_token = tokenize(str + strlen(token->value), depth, is_alias);
    return token;
}


#endif