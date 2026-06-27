// ParThread.c
// Concurrent compression using POSIX threads.

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>

#define MAX_THREADS 8

typedef struct {
    char *input;
    int start;
    int end;
    char *output;
} ThreadData;

void compressSegment(char *input, int start, int end, char *output) {
    int i = start;
    char buffer[100];

    output[0] = '\0';

    while (i < end) {
        char current = input[i];
        int count = 0;

        while (i < end && input[i] == current) {
            count++;
            i++;
        }

        if ((current == '0' || current == '1') && count >= 16) {
            if (current == '0')
                sprintf(buffer, "-%d-", count);
            else
                sprintf(buffer, "+%d+", count);

            strcat(output, buffer);
        } else {
            for (int j = 0; j < count; j++) {
                int len = strlen(output);
                output[len] = current;
                output[len + 1] = '\0';
            }
        }
    }
}

void *threadFunction(void *arg) {
    ThreadData *data = (ThreadData *)arg;
    compressSegment(data->input, data->start, data->end, data->output);
    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        printf("Usage: %s sourcefile destinationfile num_threads\n", argv[0]);
        return 1;
    }

    FILE *src = fopen(argv[1], "r");
    if (src == NULL) {
        perror("Error opening source file");
        return 1;
    }

    fseek(src, 0, SEEK_END);
    long fileSize = ftell(src);
    rewind(src);

    char *input = malloc(fileSize + 1);
    if (input == NULL) {
        perror("malloc");
        fclose(src);
        return 1;
    }

    fread(input, 1, fileSize, src);
    input[fileSize] = '\0';
    fclose(src);

    int numThreads = atoi(argv[3]);

    if (numThreads <= 0 || numThreads > MAX_THREADS) {
        printf("Number of threads must be between 1 and %d\n", MAX_THREADS);
        free(input);
        return 1;
    }

    pthread_t threads[MAX_THREADS];
    ThreadData data[MAX_THREADS];

    int chunkSize = fileSize / numThreads;

    for (int i = 0; i < numThreads; i++) {
        data[i].input = input;
        data[i].start = i * chunkSize;

        if (i == numThreads - 1)
            data[i].end = fileSize;
        else
            data[i].end = (i + 1) * chunkSize;

        data[i].output = malloc((data[i].end - data[i].start) * 2 + 100);

        if (data[i].output == NULL) {
            perror("malloc");
            free(input);
            return 1;
        }

        if (pthread_create(&threads[i], NULL, threadFunction, &data[i]) != 0) {
            perror("pthread_create");
            free(input);
            return 1;
        }
    }

    FILE *dest = fopen(argv[2], "w");
    if (dest == NULL) {
        perror("Error opening destination file");
        free(input);
        return 1;
    }

    for (int i = 0; i < numThreads; i++) {
        pthread_join(threads[i], NULL);
        fprintf(dest, "%s", data[i].output);
        free(data[i].output);
    }

    fclose(dest);
    free(input);

    printf("Compression completed using %d threads.\n", numThreads);

    return 0;
}