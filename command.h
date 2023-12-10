#ifndef COMMAND_H
#define COMMAND_H

#include <stdio.h>
#include <stdlib.h>
#include "token.h"


typedef struct Command {
    char* name;
    int args_count;
    char** args;
    FILE* redirect;
    FILE* append;
    FILE* reverse_append;
    struct Command* background;
    int error;
} Command;

void free_command(Command* command) {
    if (command == NULL) {
        return;
    }

    free(command->args);
    if (command->redirect != NULL) {
        fclose(command->redirect);
    }
    if (command->append != NULL) {
        fclose(command->append);
    }
    if (command->reverse_append != NULL) {
        fclose(command->reverse_append);
    }
    if (command->background != NULL) {
        free_command(command->background);
    }
}

void validate_command(Command* command) {
    if (command->name == NULL) {
        command->error = -1;
    }
    if (command->redirect != NULL && command->append != NULL) {
        command->error = 1;
    }
    if (command->redirect != NULL && command->reverse_append != NULL) {
        command->error = 1;
    }
    if (command->append != NULL && command->reverse_append != NULL) {
        command->error = 1;
    }
}

Command* parse(Token* token) {
    
    if (token == NULL) {
        return NULL;
    }

    Command* command = calloc(1, sizeof(Command));
    command->name = NULL;
    command->args_count = 0;
    command->args = calloc(256, sizeof(char*));
    command->redirect = NULL;
    command->append = NULL;
    command->reverse_append = NULL;
    command->background = NULL;

    while (token->type != TOKEN_EOF) {
        if (token->type == TOKEN_COMMAND) {
            command->name = token->value;
            command->args[command->args_count++] = token->value;
            token = token->next_token;
        }
        else if (token->type == TOKEN_REDIRECT && token->next_token->type == TOKEN_ARG) {
            command->redirect = fopen(token->next_token->value, "w");
            token = token->next_token->next_token;
        }
        else if (token->type == TOKEN_APPEND && token->next_token->type == TOKEN_ARG) {
            command->append = fopen(token->next_token->value, "a");
            token = token->next_token->next_token;
        }
        else if (token->type == TOKEN_REVERSE_APPEND && token->next_token->type == TOKEN_ARG) {
            command->reverse_append = fopen(token->next_token->value, "a");
            token = token->next_token->next_token;
        }
        else if (token->type == TOKEN_BACKGROUND) {
            command->background = parse(token->next_token);
            break;
        }
        else if (token->type == TOKEN_ARG || token->type == TOKEN_ASSIGN) {
            command->args[command->args_count++] = token->value;
            token = token->next_token;
        } 
        else {
            command->args[command->args_count++] = token->value;
            command->error = 1;
            token = token->next_token;
        }
    }

    return command;
    
}



#endif