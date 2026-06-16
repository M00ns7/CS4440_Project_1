/* Students: Esmeralda Amado, Cristian Hernandez Juan, Gustavo Trejo
   Class: CS 4440 - Operating Systems
   Description: Compressing a file using fork system call
*/

// importing c libraries

#include <stdio.h>  //input/output
#include <unistd.h>  //linux system calls
#include <sys/types.h>   //pid_t
#include <sys/wait.h>  //wait system call


// main function
int main(int args, char *argv[]) {

	// check command line arguments
	if(args != 3){
		fprintf(stderr, "Usage: %s <source file> <destination file>/n", argv[0]);
		return 1;
	}

	// parent id values
	pid_t pid;

	// fork a child process
	pid = fork();
	
	// if an error occurs
	if(pid < 0){
		fprintf(stderr, "Fork Failed");
		return 1;
	}

	// if it works then child process gets created
	else if(pid == 0){
		execl("./MyCompress", "MyCompress", argv[1], argv[2], NULL);
	}

	// parent process
	else{

	// parent process prevents zombies
	wait (NULL);
	printf("Child Complete");
	}

	return 0;
}