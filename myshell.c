#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pwd.h>
#include <sys/wait.h>

#include "util.h"
#include "token.h"
#include "command.h"

char* get_shell_prompt() {
    char* uid = getlogin();
    char* cwd = getcwd(NULL, 0);
    char* host = calloc(256, sizeof(char));
    gethostname(host, 256);

    char* prompt = calloc(strlen(uid) + strlen(host) + strlen(cwd) + 8, sizeof(char));
    sprintf(prompt, "%s@%s %s --- ", uid, host, cwd);

    return prompt;
}

char** get_path_values() {
    char* path = getenv("PATH");
    char** path_values = calloc(256, sizeof(char*));
    int path_values_count = 0;

    char* path_value = strtok(path, ":");
    while (path_value != NULL) {
        path_values[path_values_count++] = path_value;
        path_value = strtok(NULL, ":");
    }

    return path_values;
}

char* get_command_from_path(char* command_name, char** path_values) {
    char* command = NULL;

    for (int i = 0; i < sizeof(path_values); i++) {
        
        char* command_path = calloc(strlen(path_values[i]) + strlen(command_name) + 2, sizeof(char));
        
        sprintf(command_path, "%s/%s", path_values[i], command_name);
        
        if (access(command_path, X_OK) == 0) {
            command = command_path;
            break;
        }

        free(command_path);
    }

    return command;
}

int main(int argc, char** argv) {

    while(argv[0][strlen(argv[0]) - 1] != '/') {
        argv[0][strlen(argv[0]) - 1] = '\0';
    }

    // source_directory is the directory where the shell is located
    source_directory = copy(argv[0], strlen(argv[0])); 
    history_path = strcat(copy(source_directory, strlen(source_directory)), ".history");
    alias_path = strcat(copy(source_directory, strlen(source_directory)), ".alias");
    char* bello = strcat(copy(source_directory, strlen(source_directory)), "bin/bello");
    char* alias = strcat(copy(source_directory, strlen(source_directory)), "bin/alias");
    
    char* line = NULL;
    size_t size = 256;


    FILE* history_file = fopen(history_path, "a");
    
    if (history_file == NULL) {
        printf("%s\n", "Error opening history file");
    }

    int is_exit = 0;

    // .reverse is a temporary file to store the reverse output of the command
    // reverse_id is used to create a new unique file for each command to avoid collision
    int reverse_id = 0;
    char* reverse_file = calloc(256, sizeof(char));
    sprintf(reverse_file, ".reverse%d", reverse_id); 

    char** path_values = get_path_values();
    
    while (!is_exit) {
        
        printf("%s", get_shell_prompt());

        if (getline(&line, &size, stdin) == -1) {
            break;
        }

        int* depth = calloc(1, sizeof(int));
        *depth = 0;

        Token* token = tokenize(line, depth, 0);
        Command* command = parse(token);
        Command* command_ptr = command;

        while (command != NULL && command->name != NULL) {

            if (strcmp(command->name, "\0") == 0) {
                break;
            }
            else if (strcmp(command->name, "exit") == 0) {
                is_exit = 1;
                break;
            }
            else if (strcmp(command->name, "bello") == 0) {
                command->name = bello;
                command->args[0] = bello;
            }
            else if (strcmp(command->name, "alias") == 0) {
                command->name = alias;
                command->args[0] = alias;
            }
            else {
                command->name = get_command_from_path(command->name, path_values);
            }

            validate_command(command);

            if (command->error == -1) {
                printf("%s\n", "Command not found");
            }
            else if (command->error == 1) {
                printf("%s\n", "Invalid argument");
            } 
            else {
                
                pid_t pid = fork();
                
                if (pid == 0) {
                    
                    pid_t pid2 = fork();
                    
                    if (pid2 == 0) {
                        if (command->redirect) {
                            dup2(fileno(command->redirect), STDOUT_FILENO);
                        }
                        else if (command->append) {
                            dup2(fileno(command->append), STDOUT_FILENO);
                        }
                        else if (command->reverse_append) {
                            freopen(reverse_file, "w", stdout);
                        }
                        execv(command->name, command->args);
                    }
                    else {
                        // second forked process is for handling the reverse output
                        waitpid(pid2, NULL, 0);
                        print_reversed(command->reverse_append, reverse_file);
                        exit(0);
                    }

                } 
                else { 
                    // first forked process is for handling the background process
                    // if there were no second fork, reverse outaput handling in a background process would not be done easily
                    if (command->background == NULL) {
                        waitpid(pid, NULL, 0);
                    } 
                }
            } 
            command = command->background;                      // switch to the next command in the same line
            sprintf(reverse_file, ".reverse%d", ++reverse_id);  // a new unique .reverse file name for each command
        }

        fprintf(history_file, "%s", line);
        fflush(history_file);

        free_token(token);
        free_command(command_ptr);
        free(depth);

    }

    free(line);
    free(path_values);
    fclose(history_file);

    free(history_path);
    free(alias_path);
    free(source_directory);

    free(bello);
    free(alias);

    return 0;
}
