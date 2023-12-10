#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int alias(int argc, char** argv) {
    
    if (argc != 4) {
        printf("Invalid argument\n");
        return 1;
    }
    if (strcmp(argv[2], "=") != 0 || strlen(argv[1]) == 0 || strlen(argv[3]) == 0) {
        printf("Invalid argument\n");
        return 1;
    }

    char* alias_path = argv[0];
    while (alias_path[strlen(alias_path) - 1] != '/') {
        alias_path[strlen(alias_path) - 1] = '\0';
    }
    alias_path[strlen(alias_path) - 1] = '\0';
    while (alias_path[strlen(alias_path) - 1] != '/') {
        alias_path[strlen(alias_path) - 1] = '\0';
    }
    
    FILE* alias = fopen(strcat(argv[0], ".alias"), "a");

    if (alias == NULL) {
        printf("Error opening file\n");
        return 1;
    }

    fprintf(alias, "%s=%s\n", argv[1], argv[3]);

}

int main(int argc, char** argv) {
    return alias(argc, argv);
}