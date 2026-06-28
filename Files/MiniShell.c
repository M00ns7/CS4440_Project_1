/* Students: Esmeralda Amado, Cristian Hernandez Juan, Gustavo Trejo
   Class: CS 4440 - Operating Systems
   Description: Simple shell that executes UNIX commands
*/

// importing c libraries

#include <stdio.h>      //input/output
#include <stdlib.h>     //exit()
#include <unistd.h>     //fork(), execlp()
#include <sys/wait.h>   //wait()
#include <string.h>     //string functions

// main function
int main() {

    // stores the command entered by the user
    char command[100];

    while (1) {

        // display shell prompt
        printf("MiniShell> ");
        fflush(stdout);

        // read command from the user
        if (fgets(command, sizeof(command), stdin) == NULL)
            break;

        // remove the newline character
        command[strcspn(command, "\n")] = '\0';

        // exit the shell
        if (strcmp(command, "exit") == 0)
            break;

        // create child process
        pid_t pid = fork();

        // check if fork failed
        if (pid < 0) {
            perror("fork");
            exit(EXIT_FAILURE);
        }

        // child process executes the command
        else if (pid == 0) {

            // execute the command entered by the user
            execlp(command, command, NULL);

            // runs only if execlp fails
            perror("execlp");
            exit(EXIT_FAILURE);
        }

        // parent waits for child process to finish
        else {
            wait(NULL);
        }
    }

    return 0;
}
