/* Students: Esmeralda Amado, Cristian Hernandez Juan, Gustavo Trejo
   Class: CS 4440 - Operating Systems
   Description: Parallel file compressor using fork and pipe
*/

//importing C libraries

#include <stdio.h>      //input/output
#include <unistd.h>     //linux system calls
#include <stdlib.h>     //malloc, free, atoi, exit
#include <sys/wait.h>   //wait system call

//compress a chunk of memory and write result to a pipe
//basically the same logic from PipeCompress but now it works on a memory slice
void compress_chunk(char *chunk, long start, long end, int write_fd) {

    //tracking the current bit run
    char current = '\0';
    int count = 0;

    //loop through the chunk
    for (long i = start; i < end; i++) {
        char ch = chunk[i];

        //only compress 0s and 1s
        if (ch == '0' || ch == '1') {

            //starting a new run
            if (count == 0) {
                current = ch;
                count = 1;
            }

            //same run
            else if (ch == current) {
                count++;
            }

            //run ended, time to dump it
            else {

                //compress long runs (16+)
                if (count >= 16) {
                    if (current == '0')
                        dprintf(write_fd, "-%d-", count);
                    else
                        dprintf(write_fd, "+%d+", count);
                }

                //short runs get copied normally
                else {
                    for (int k = 0; k < count; k++)
                        write(write_fd, &current, 1);
                }

                //start new run
                current = ch;
                count = 1;
            }

        }
        else {

            //if we were in the middle of a run, dump it first
            if (count > 0) {

                if (count >= 16) {
                    if (current == '0')
                        dprintf(write_fd, "-%d-", count);
                    else
                        dprintf(write_fd, "+%d+", count);
                }

                else {
                    for (int k = 0; k < count; k++)
                        write(write_fd, &current, 1);
                }

                count = 0;
                current = '\0';
            }

            //no bit characters get copied
            write(write_fd, &ch, 1);
        }
    }

    //finish last run
    if (count > 0) {
        if (count >= 16) {
            if (current == '0')
                dprintf(write_fd, "-%d-", count);
            else
                dprintf(write_fd, "+%d+", count);
        }

        else {
            for (int k = 0; k < count; k++)
                write(write_fd, &current, 1);
        }
    }
}


int main(int argc, char *argv[]) {

    if (argc != 4) {
        fprintf(stderr, "Usage: %s <input> <output> <num_processes>\n", argv[0]);
        return 1;
    }

    int n = atoi(argv[3]);  // number of processes to fork
    if (n <= 0) {
        fprintf(stderr, "Invalid number of processes\n");
        return 1;
    }

    //open input file
    FILE *input = fopen(argv[1], "r");
    if (!input) {
        perror("input file");
        return 1;
    }

    //get file size (so we know how to split it)
    fseek(input, 0, SEEK_END);
    long size = ftell(input);
    fseek(input, 0, SEEK_SET);

    //read entire file into memory
    char *buffer = malloc(size);
    fread(buffer, 1, size, input);
    fclose(input);

    //each child gets an equal chunk
    long chunk = size / n;

    //pipes for each child
    int pipes[n][2];

    for (int i = 0; i < n; i++) {

        pipe(pipes[i]);  // one pipe per child

        pid_t pid = fork();


	//child process
        if (pid == 0) {

            close(pipes[i][0]); // child doesn't read from pipe

            //figure out chunk boundaries
            long start = i * chunk;
            long end = (i == n - 1) ? size : start + chunk;

            //compress chunk
            compress_chunk(buffer, start, end, pipes[i][1]);

            close(pipes[i][1]);
            exit(0);
        }

        else if (pid < 0) {
            perror("fork");
            return 1;
        }

        //parent closes write end
        close(pipes[i][1]);
    }

    //parent collects all results in order
    FILE *output = fopen(argv[2], "w");

    for (int i = 0; i < n; i++) {
        wait(NULL); // wait for each child to finish

        char temp[4096];
        int r;

        //read compressed chunk from child
        while ((r = read(pipes[i][0], temp, sizeof(temp))) > 0) {
            fwrite(temp, 1, r, output);
        }

        close(pipes[i][0]);
    }

    fclose(output);
    free(buffer);

    printf("ParFork Complete\n");
    return 0;
}
