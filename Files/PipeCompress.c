/* Students: Esmeralda Amado, Cristian Hernandez Juan, Gustavo Trejo
   Class: CS 4440 - Operating Systems
   Description: Two programs communicating using pipe system call
*/

//importing c libraries

#include <stdio.h>  //input/output
#include <unistd.h>  //linux system calls
#include <sys/types.h>   //pid_t
#include <sys/wait.h>  //wait system call
#include <stdlib.h>  //exit system call

//main function
int main(int argc, char *argv[]){
    
    if(argc != 3){
        fprintf(stderr, "Usage: %s <source file> <destination file>\n", argv[0]);
        return 1;
    }

    //pipe file descriptor
    int pipefd[2];

    //parent
    pid_t pid;

    //error in pipe
    if(pipe(pipefd) == -1){
        perror("pipe");
        return 1;
    }

    pid = fork();

    if(pid < 0){
        perror("fork");
        return 1;
    }

    //child process
    if(pid == 0){
        close(pipefd[1]);

        FILE *output = fopen(argv[2], "w");

        if(output == NULL){
            perror("destination file");
            close(pipefd[0]);
            exit(1);
        }

        //variables
        char ch2;
        char current = '\0';
        int count = 0;

        //read one character at a time from the pipe
        while(read(pipefd[0], &ch2, 1) > 0){

            //current character is a binary digit
            if(ch2 == '0' || ch2 == '1'){

                //start a new sequence
                if(count == 0){
                    current = ch2;
                    count = 1;
                }

                //continue counting the same bit
                else if(ch2 == current){
                    count++;
                }

                //process the previous sequence
                else{

                    //compress sequence if it is 16 or more
                    if(count >= 16){
                        if(current == '0'){
                            fprintf(output, "-%d-", count);
                        }
                        else{
                            fprintf(output, "+%d+", count);
                        }
                    }

                    //else copy the sequence 
                    else{
                        for(int i = 0; i < count; i++){
                            fputc(current, output);
                        }
                    }

                    //begin counting the new sequence
                    current = ch2;
                    count = 1;
                }
            }

            //current character is a space or newline
            else{

                //write any unfinished sequence before copying separator
                if(count > 0){

                    //compress long sequences
                    if(count >= 16){
                        if(current == '0'){
                            fprintf(output, "-%d-", count);
                        }
                        else{
                            fprintf(output, "+%d+", count);
                        }
                    }

                    //copy short sequences
                    else{
                        for(int i = 0; i < count; i++){
                            fputc(current, output);
                        }
                    }

                    // reset sequence counter
                    count = 0;
                    current = '\0';
                }

                //preserve spaces and newlines in the output
                fputc(ch2, output);
            }
        }

        //write the last sequence after reaching the end of the file
        if(count > 0){

            //compress long sequences
            if(count >= 16){
                if(current == '0'){
                    fprintf(output, "-%d-", count);
                }
                else{
                    fprintf(output, "+%d+", count);
                }
            }

            //copy short sequences normally
            else{
                for(int i = 0; i < count; i++){
                    fputc(current, output);
                }
            }
        }

        //close destination file and read end of pipe
        fclose(output);
        close(pipefd[0]);

        //terminate child process
        exit(0);
    }

    //parent process
    else{
        close(pipefd[0]);

        FILE *input = fopen(argv[1], "r");

        if(input == NULL){
            perror("source file");
            close(pipefd[1]);
            wait(NULL);
            return 1;
        }

        char ch1;
        while((ch1 = fgetc(input)) != EOF){
            
            //write to pipe
            if(write(pipefd[1], &ch1, 1) == -1){
                perror("write");
                fclose(input);
                close(pipefd[1]);
                wait(NULL);
                return 1;
            }
        }

        fclose(input);
        close(pipefd[1]);

        wait(NULL);

        printf("PipeCompress Complete\n");
    }

    return 0;
}
