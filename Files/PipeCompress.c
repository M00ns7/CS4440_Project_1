/* Students: Esmeralda Amado, Cristian Hernandez Juan, Gustavo Trejo
   Class: CS 4440 - Operating Systems
   Description: Two programs communicating using pipe system call
*/

// importing c libraries

#include <stdio.h>  //input/output
#include <unistd.h>  //linux system calls
#include <sys/types.h>   //pid_t
#include <sys/wait.h>  //wait system call
#include <stdlib.h>  //exit system call

// main function
int main(){
	
	// pipe file descriptor
	int pipefd[2];

	// parent id type
	pid_t pid;
	pid = fork();

	// error in pipe
	if(pipe(pipefd) == -1){
		perror("pipe");
		return 1:
	}

	// child process
	if(pid == 0){
		close(pipefd[1]);

	FILE *output = fopen("destination.txt", "w");

	char ch2;

	fclose(output);
	close(pipefd[0]);
	}

	// parent process
	else{
		close(pipefd[0]);

		FILE *input = fopen("source.txt", "r");

		char ch1;
		while((ch1 = fgetc(input)) != EOF){
			
			//write to pipe
			write(pipefd[1], &ch1, 1);
		}

		fclose(input);
		close(pipefd[1]);

		wait(NULL);

	}
}