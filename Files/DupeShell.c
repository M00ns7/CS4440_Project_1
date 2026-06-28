/* Students: Esmeralda Amado, Cristian Hernandez Juan, Gustavo Trejo
   Class: CS 4440 - Operating Systems
   Description: Shell program that executes commands connected by a pipe
*/

// importing c libraries

#include <stdio.h>      //input/output
#include <stdlib.h>     //exit()
#include <unistd.h>     //fork(), pipe(), dup2(), execvp()
#include <string.h>     //string functions
#include <sys/wait.h>   //wait()

#define MAX_LINE 1024
#define MAX_ARGS 64

// main function
int main() {

    // stores the command entered by the user
    char line[MAX_LINE];

    // stores the command before the pipe
    char *left[MAX_ARGS];

    // stores the command after the pipe
    char *right[MAX_ARGS];

    while (1) {

        // display shell prompt
        printf("DupShell> ");
        fflush(stdout);

        // read user input
        if (fgets(line, MAX_LINE, stdin) == NULL)
            break;

        // remove newline character
        line[strcspn(line, "\n")] = '\0';

        // exit shell
        if (strcmp(line, "exit") == 0)
            break;

        // locate the pipe symbol
        char *pipePos = strchr(line, '|');

        // make sure the command contains a pipe
        if (pipePos == NULL) {
            printf("Please enter command with a pipe.\n");
            continue;
        }

        // split command into left and right side
        *pipePos = '\0';

        char *cmd1 = line;
        char *cmd2 = pipePos + 1;

        // parse first command
        int i = 0;
        left[i] = strtok(cmd1, " ");
        while (left[i] != NULL) {
            i++;
            left[i] = strtok(NULL, " ");
        }

        // parse second command
        i = 0;
        right[i] = strtok(cmd2, " ");
        while (right[i] != NULL) {
            i++;
            right[i] = strtok(NULL, " ");
        }

        // pipe file descriptors
        int fd[2];

        // create communication pipe
        if (pipe(fd) == -1) {
            perror("pipe");
            exit(EXIT_FAILURE);
        }

        // create first child process
        pid_t pid1 = fork();

        // first child executes first command
        if (pid1 == 0) {

            // redirect standard output to the pipe
            dup2(fd[1], STDOUT_FILENO);

            // child no longer needs pipe descriptors
            close(fd[0]);
            close(fd[1]);

            // execute first command
            execvp(left[0], left);

            // runs only if execvp fails
            perror("execvp");
            exit(EXIT_FAILURE);
        }

        // create second child process
        pid_t pid2 = fork();

        // second child executes second command
        if (pid2 == 0) {

            // redirect standard input from the pipe
            dup2(fd[0], STDIN_FILENO);

            // child no longer needs pipe descriptors
            close(fd[1]);
            close(fd[0]);

            // execute second command
            execvp(right[0], right);

            // runs only if execvp fails
            perror("execvp");
            exit(EXIT_FAILURE);
        }

        // parent closes both ends of the pipe
        close(fd[0]);
        close(fd[1]);

        // wait for both child processes
        wait(NULL);
        wait(NULL);
    }

    return 0;
}
