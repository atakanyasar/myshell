#ifndef UTIL_H
#define UTIL_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char* source_directory = NULL;
char* history_path = NULL;
char* alias_path = NULL;

char* copy(char* str, int len) {
    char* ret = calloc(len + 1, sizeof(char));
    strncpy(ret, str, len);
    ret[len] = '\0';
    return ret;
}

char* reverse(char* str) {
    int len = strlen(str);
    for (int i = 0; i < len / 2; i++) {
        char tmp = str[i];
        str[i] = str[len - i - 1];
        str[len - i - 1] = tmp;
    }
    return str;
}

/*
 * Iterates through the .alias file
 * If the alias name matches the command name, return the alias command
*/
char* get_alias(char* command_name) {
    FILE* alias_file = fopen(alias_path, "r");
    char* alias = NULL;

    if (alias_file == NULL) {
        return NULL;
    }
    
    char* line = NULL;
    size_t size = 256;
    while (getline(&line, &size, alias_file) != -1) {
        char* alias_name = line;
        while (line[0] != '=') {
            line++;
        }
        line[0] = '\0';
        line++;
        char* alias_command = line;
        if (strcmp(alias_name, command_name) == 0) {
            if (alias != NULL) {
                free(alias);
            }
            alias = copy(alias_command, strlen(alias_command) - 1);
        }
    }
    fclose(alias_file);
    return alias;

}

void print_reversed(FILE* file, const char* reverse_file_name) {
    if (file == NULL) {
        return;
    }
    
    FILE* reverse_file = fopen(reverse_file_name, "r");
    char* line = NULL;
    size_t size = 256;
    while (getline(&line, &size, reverse_file) != -1) {
        fprintf(file, "%s", reverse(line));
    }
    fclose(reverse_file);
    remove(reverse_file_name);
}

#endif