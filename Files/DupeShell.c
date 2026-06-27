// DupShell.c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

#define MAX_LINE 1024
#define MAX_ARGS 64

int main() {

    char line[MAX_LINE];
    char *left[MAX_ARGS];
    char *right[MAX_ARGS];

    while (1) {

        printf("DupShell> ");
        fflush(stdout);

        if (fgets(line, MAX_LINE, stdin) == NULL)
            break;

        line[strcspn(line, "\n")] = '\0';

        if (strcmp(line, "exit") == 0)
            break;

        char *pipePos = strchr(line, '|');

        if (pipePos == NULL) {
            printf("Please enter command with a pipe.\n");
            continue;
        }

        *pipePos = '\0';

        char *cmd1 = line;
        char *cmd2 = pipePos + 1;

        int i = 0;
        left[i] = strtok(cmd1, " ");
        while (left[i] != NULL) {
            i++;
            left[i] = strtok(NULL, " ");
        }

        i = 0;
        right[i] = strtok(cmd2, " ");
        while (right[i] != NULL) {
            i++;
            right[i] = strtok(NULL, " ");
        }

        int fd[2];

        if (pipe(fd) == -1) {
            perror("pipe");
            exit(EXIT_FAILURE);
        }

        pid_t pid1 = fork();

        if (pid1 == 0) {

            dup2(fd[1], STDOUT_FILENO);

            close(fd[0]);
            close(fd[1]);

            execvp(left[0], left);

            perror("execvp");
            exit(EXIT_FAILURE);
        }

        pid_t pid2 = fork();

        if (pid2 == 0) {

            dup2(fd[0], STDIN_FILENO);

            close(fd[1]);
            close(fd[0]);

            execvp(right[0], right);

            perror("execvp");
            exit(EXIT_FAILURE);
        }

        close(fd[0]);
        close(fd[1]);

        wait(NULL);
        wait(NULL);
    }

    return 0;
}