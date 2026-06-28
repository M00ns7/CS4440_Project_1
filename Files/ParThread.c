/* Students: Esmeralda Amado, Cristian Hernandez Juan, Gustavo Trejo
   Class: CS 4440 - Operating Systems
   Description: Concurrent file compression using POSIX threads
*/

// importing c libraries

#include <stdio.h>      //input/output
#include <stdlib.h>     //malloc(), free(), atoi(), exit()
#include <pthread.h>    //pthread library
#include <string.h>     //string functions

#define MAX_THREADS 8

// structure used to pass information to each thread
typedef struct {
    char *input;
    int start;
    int end;
    char *output;
} ThreadData;

// compresses one section of the input file
void compressSegment(char *input, int start, int end, char *output) {

    // current position in the input
    int i = start;

    // stores compressed output
    char buffer[100];

    output[0] = '\0';

    // process characters until the end of the assigned chunk
    while (i < end) {

        // current character being processed
        char current = input[i];
        int count = 0;

        // count how many times the character repeats
        while (i < end && input[i] == current) {
            count++;
            i++;
        }

        // compress runs of 0s or 1s that are 16 or more
        if ((current == '0' || current == '1') && count >= 16) {

            if (current == '0')
                sprintf(buffer, "-%d-", count);
            else
                sprintf(buffer, "+%d+", count);

            strcat(output, buffer);
        }

        // copy short runs normally
        else {

            for (int j = 0; j < count; j++) {

                int len = strlen(output);
                output[len] = current;
                output[len + 1] = '\0';
            }
        }
    }
}

// function executed by each thread
void *threadFunction(void *arg) {

    ThreadData *data = (ThreadData *)arg;

    // compress the assigned chunk
    compressSegment(data->input,
                    data->start,
                    data->end,
                    data->output);

    // terminate thread
    pthread_exit(NULL);
}

// main function
int main(int argc, char *argv[]) {

    // check command line arguments
    if (argc != 4) {
        printf("Usage: %s sourcefile destinationfile num_threads\n", argv[0]);
        return 1;
    }

    // open source file
    FILE *src = fopen(argv[1], "r");

    if (src == NULL) {
        perror("Error opening source file");
        return 1;
    }

    // determine file size
    fseek(src, 0, SEEK_END);
    long fileSize = ftell(src);
    rewind(src);

    // allocate memory for the input file
    char *input = malloc(fileSize + 1);

    if (input == NULL) {
        perror("malloc");
        fclose(src);
        return 1;
    }

    // read the entire file into memory
    fread(input, 1, fileSize, src);
    input[fileSize] = '\0';
    fclose(src);

    // number of threads requested
    int numThreads = atoi(argv[3]);

    // validate number of threads
    if (numThreads <= 0 || numThreads > MAX_THREADS) {
        printf("Number of threads must be between 1 and %d\n", MAX_THREADS);
        free(input);
        return 1;
    }

    // thread ids
    pthread_t threads[MAX_THREADS];

    // information passed to each thread
    ThreadData data[MAX_THREADS];

    // divide file into equal chunks
    int chunkSize = fileSize / numThreads;

    // create threads
    for (int i = 0; i < numThreads; i++) {

        data[i].input = input;
        data[i].start = i * chunkSize;

        // last thread processes remaining bytes
        if (i == numThreads - 1)
            data[i].end = fileSize;
        else
            data[i].end = (i + 1) * chunkSize;

        // allocate memory for compressed output
        data[i].output = malloc((data[i].end - data[i].start) * 2 + 100);

        if (data[i].output == NULL) {
            perror("malloc");
            free(input);
            return 1;
        }

        // create thread
        if (pthread_create(&threads[i], NULL, threadFunction, &data[i]) != 0) {
            perror("pthread_create");
            free(input);
            return 1;
        }
    }

    // open destination file
    FILE *dest = fopen(argv[2], "w");

    if (dest == NULL) {
        perror("Error opening destination file");
        free(input);
        return 1;
    }

    // wait for each thread and write results to output file
    for (int i = 0; i < numThreads; i++) {

        pthread_join(threads[i], NULL);

        fprintf(dest, "%s", data[i].output);

        free(data[i].output);
    }

    // close destination file
    fclose(dest);

    // free allocated memory
    free(input);

    printf("Compression completed using %d threads.\n", numThreads);

    return 0;
}
