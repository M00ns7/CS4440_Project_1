/* Students: Esmeralda Amado, Cristian Hernandez Juan, Gustavo Trejo
   Class: CS 4440 - Operating Systems
   Description: Shell program that executes UNIX commands with command line arguments
*/

// importing c libraries

#include <stdio.h>      //input/output
#include <stdlib.h>     //exit()
#include <unistd.h>     //fork(), execvp()
#include <string.h>     //string functions
#include <sys/wait.h>   //wait()

#define MAX_LINE 1024
#define MAX_ARGS 64

// main function
int main() {

    // stores the command entered by the user
    char line[MAX_LINE];

    // stores the parsed command and its arguments
    char *args[MAX_ARGS];

    while (1) {

        // display shell prompt
        printf("MoreShell> ");
        fflush(stdout);

        // read command from the user
        if (fgets(line, MAX_LINE, stdin) == NULL)
            break;

        // remove the newline character
        line[strcspn(line, "\n")] = '\0';

        // exit the shell
        if (strcmp(line, "exit") == 0)
            break;

        // split the command into individual arguments
        int i = 0;
        args[i] = strtok(line, " ");

        while (args[i] != NULL) {
            i++;
            args[i] = strtok(NULL, " ");
        }

        // ignore empty commands
        if (args[0] == NULL)
            continue;

        // create child process
        pid_t pid = fork();

        // child process executes the command
        if (pid == 0) {

            // execute the command with its arguments
            execvp(args[0], args);

            // runs only if execvp fails
            perror("execvp");
            exit(EXIT_FAILURE);
        }

        // parent waits for child process to finish
        else if (pid > 0) {
            wait(NULL);
        }

        // fork failed
        else {
            perror("fork");
        }
    }

    return 0;
}
