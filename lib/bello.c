#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#include <time.h>
#include <sys/utsname.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

void bello(int argc, char** argv) {

    // Username
    printf("Username: %s\n", getlogin());

    // Hostname
    char hostname[1024];
    gethostname(hostname, sizeof(hostname));
    printf("Hostname: %s\n", hostname);

    // Last Executed Command
    char* history_path = argv[0];
    while (history_path[strlen(history_path) - 1] != '/') {
        history_path[strlen(history_path) - 1] = '\0';
    }
    history_path[strlen(history_path) - 1] = '\0';
    while (history_path[strlen(history_path) - 1] != '/') {
        history_path[strlen(history_path) - 1] = '\0';
    }

    FILE *history = fopen(strcat(history_path, ".history"), "r");
    char *last = NULL;
    if (history) {
        char *line = NULL;
        size_t len = 0;
        while (getline(&line, &len, history) != -1) {
            last = line;
        }
        fclose(history);

    } 
    if (last != NULL)
        printf("Last Executed Command: %s", last);
    else 
        printf("Last Executed Command: %s\n", "No commands executed yet");

    // TTY
    char *tty = ttyname(STDIN_FILENO);
    printf("TTY: %s\n", tty ? tty : "Not available");

    // Current Shell Name
    printf("Current Shell Name: %s\n", getenv("SHELL"));

    // Home Location
    printf("Home Location: %s\n", getenv("HOME"));

    // Current Time and Date
    time_t t;
    time(&t);
    printf("Current Time and Date: %s", ctime(&t));

    // Current number of processes being executed
    int processCount = 0;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    FILE *fp = fopen("/proc/stat", "r");
    while ((read = getline(&line, &len, fp)) != -1) {
        if (line[0] == 'p' && line[1] == 'r' && line[2] == 'o' && line[3] == 'c') {
            processCount++;
        }
    }
    fclose(fp);
    printf("Current number of processes being executed: %d\n", processCount);
}

int main(int argc, char** argv) {
    bello(argc, argv);
    return 0;
}
