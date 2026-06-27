// MoreShell.c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

#define MAX_LINE 1024
#define MAX_ARGS 64

int main() {

    char line[MAX_LINE];
    char *args[MAX_ARGS];

    while (1) {

        printf("MoreShell> ");
        fflush(stdout);

        if (fgets(line, MAX_LINE, stdin) == NULL)
            break;

        line[strcspn(line, "\n")] = '\0';

        if (strcmp(line, "exit") == 0)
            break;

        int i = 0;
        args[i] = strtok(line, " ");

        while (args[i] != NULL) {
            i++;
            args[i] = strtok(NULL, " ");
        }

        pid_t pid = fork();

        if (pid == 0) {
            execvp(args[0], args);
            perror("execvp");
            exit(EXIT_FAILURE);
        }
        else if (pid > 0) {
            wait(NULL);
        }
        else {
            perror("fork");
        }
    }

    return 0;
}