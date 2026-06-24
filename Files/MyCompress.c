/*
    Students: Esmeralda Amado, Cristian Hernandez Juan, Gustavo Trejo
    Class: CS 4440-01 (Operating Systems)
    Description: Compresses an input file of 1's and 0's using system calls and outputs a compressed version 
*/

// Importing libraries 
#include <stdio.h> // Input/output 
#include <stdlib.h> // Exit system call      
#include <unistd.h> // POSIX system calls
#include <fcntl.h>  // File Control options 
#include <sys/stat.h> // For file info and permissions

// CONSTANTS: 
// Any run shorter than this length is copied as it is
#define MINIMUM_RUN_LENGTH_TO_COMPRESS 16
// Size of the chunk used to read the input file
#define READ_CHUNK_SIZE_IN_BYTES 4096
// Size for compressed marker length ("+" or "-")
#define MAX_COMPRESSED_MARKER_LENGTH 32

// Function prototypes
static char *readEntireFileIntoBuffer(int sourceFileDescriptor, long *outFileSizeInBytes);
static void compressFileDescriptor(int sourceFileDescriptor, int destinationFileDescriptor);
static void writeAllBytes(int destinationFileDescriptor, const char *dataBuffer, long numberOfBytesToWrite);
static void writeSingleCharacter(int destinationFileDescriptor, char characterToWrite);
static void flushRunToOutput(int destinationFileDescriptor, char runCharacter, long runLength);

/* Main function: 
    - Opens the input file for reading and the output file for writing 
    - Calls the compression function (compressFileDescriptor) to compress the input file 
    - Closes both files and exits if there are any errors. */

int main(int argumentCount, char *argumentValues[]){
    // Validates the Command Line Arguments, it is expected to be used in the following way 
    // There must be exactly 3 arguments: the program name, the source file, and the destination file.
    if (argumentCount != 3){
        fprintf(stderr, "Usage: %s <source_file> <destination_file>\n", argumentValues[0]);
        exit(EXIT_FAILURE);
    }

    const char *sourceFilePath = argumentValues[1];
    const char *destinationFilePath = argumentValues[2];

    // Opens the source file to be Read only, If an error occurs, prints an error message and exits. 
    int sourceFileDescriptor = open(sourceFilePath, O_RDONLY);
    if (sourceFileDescriptor == -1){
        perror("Error opening source file");
        exit(EXIT_FAILURE);
    }

    /* Opens or creates the output file in READ/WRITE mode . 
        O_CREAT, Creates the file it doesn't exist 
        O_TRUNC, shortens the file to zero length if the file already exists
        O_RDWR, READ/WRITE access

        Permissions: S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH
        The owner can read and write to the file while other users can only read it. 
    */
    int destinationFileDescriptor = open(destinationFilePath, O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    if (destinationFileDescriptor == -1){
        perror("Error opening destination file");
        close(sourceFileDescriptor); // cleans up the file descriptor that was already opened 
        exit(EXIT_FAILURE);
    }

    // Calls the compression function which takes the input and output file descriptors which does all the work. 
    compressFileDescriptor(sourceFileDescriptor, destinationFileDescriptor);
    // Closes the source file.
    if (close(sourceFileDescriptor) == -1){
        perror("Error closing source file");
        exit(EXIT_FAILURE);
    }
    // Closes the destination file.
    if (close(destinationFileDescriptor) == -1){
        perror("Error closing destination file");
        exit(EXIT_FAILURE);
    }
    // Prints message that the compression was successful and exits 
    printf("Compression complete. '%s' -> '%s'\n", sourceFilePath, destinationFilePath);
    return EXIT_SUCCESS;
}


 /*
    readEntireFileIntoBuffer function: 
    - Reads the entire contents of a file into a single dynamically allocated buffer that uses repeated read() system calls. 
    - If successful, returns a pointer to the buffer and sets *outFileSizeInBytes to the number of bytes read.
    - If the function fails, prints an error message and exits the program. 
 */

// Takes a file descriptor and a pointer (outFileSizeInBytes) to store the file size.
static char *readEntireFileIntoBuffer(int sourceFileDescriptor, long *outFileSizeInBytes){
    // Sets the initial capacity of the buffer to the size of a single read chunk.
    long bufferCapacityInBytes = READ_CHUNK_SIZE_IN_BYTES;
    long totalBytesReadSoFar = 0; // Initilizes the total number of bytes read so far to 0 

    // Allocates memory for the buffer.
    char *fileContentsBuffer = malloc(bufferCapacityInBytes);
    // If out of memory, program exits. 
    if (fileContentsBuffer == NULL){
        fprintf(stderr, "Error: unable to allocate memory to read the source file\n");
        exit(EXIT_FAILURE);
    }
    // Will break when the end of the file is reached.
    while (1){
        // Checks if adding another chunk of bytes will exceed the buffer capacity.
        if (totalBytesReadSoFar + READ_CHUNK_SIZE_IN_BYTES > bufferCapacityInBytes){
            bufferCapacityInBytes *= 2; // Double the buffer capacity if needed 
            // Reallocates the buffer to the new size.
            char *grownBuffer = realloc(fileContentsBuffer, bufferCapacityInBytes);
            // If realloc returns null, the reallocation failed and the program exits.
            if (grownBuffer == NULL){
                fprintf(stderr, "Error: unable to grow buffer while reading source file\n");
                free(fileContentsBuffer);
                exit(EXIT_FAILURE);
            }
            fileContentsBuffer = grownBuffer;
        }
        // Reads a chunk of the file into the buffer.
        ssize_t bytesReadThisCall = read(sourceFileDescriptor, fileContentsBuffer + totalBytesReadSoFar, READ_CHUNK_SIZE_IN_BYTES);        
        // If read() returns -1, an error occurred and frees the allocated memory and exits 
        if (bytesReadThisCall == -1){
            perror("Error reading source file");
            free(fileContentsBuffer);
            exit(EXIT_FAILURE);
        }
        // If read() returns 0 it has reached the end of the file and exits the loop 
        if (bytesReadThisCall == 0){
            break;
        }
        // Adds the number of bytes read to the total.
        totalBytesReadSoFar += bytesReadThisCall;
    }
    // Updates the output parameter with the total size of the file.
    *outFileSizeInBytes = totalBytesReadSoFar;
    // Returns the buffer containing the file's contents.
    return fileContentsBuffer;
}


// CompressFileDescriptor Function: Does the compression and copying. 
static void compressFileDescriptor(int sourceFileDescriptor, int destinationFileDescriptor){
    // Pulls the whole file into memory. 
    long fileSizeInBytes = 0;
    char *data = readEntireFileIntoBuffer(sourceFileDescriptor, &fileSizeInBytes);
    long index = 0; // Current position in the data buffer
    // Loops through every byte of the file from start to finish. 
    while (index < fileSizeInBytes){
        char currentCharacter = data[index];
        // Anything that isn't '0' or '1' is a just copied through.
        if (currentCharacter != '0' && currentCharacter != '1'){
            writeSingleCharacter(destinationFileDescriptor, currentCharacter);
            index++;
            continue;
        }
        // Start of a new run. Counts the length of the run.
        long runLength = 1; // length starts at 1
        long lookaheadIndex = index;
        while (lookaheadIndex + 1 < fileSizeInBytes &&
               (data[lookaheadIndex] - data[lookaheadIndex + 1]) == 0){
            runLength++;
            lookaheadIndex++;
        }
        // Emits the run to the output file.
        flushRunToOutput(destinationFileDescriptor, currentCharacter, runLength);
        index += runLength;
    }
    free(data);
}

 // FlushRunToOutput: Writes a run of characters to the output file. 
 // Writes a run as "-N-" for zeros or "+N+" for ones if it is long enough or as the raw repeated characters.
static void flushRunToOutput(int destinationFileDescriptor, char runCharacter, long runLength){
    // If the run is long enough, compress it; otherwise, write the original characters.
    if (runLength >= MINIMUM_RUN_LENGTH_TO_COMPRESS){
        char markerCharacter = (runCharacter == '0') ? '-' : '+';
        // Build a small string using snprintf and writes it out with the write() system call.
        char compressedMarkerText[MAX_COMPRESSED_MARKER_LENGTH];
        int markerTextLength = snprintf(compressedMarkerText, sizeof(compressedMarkerText),"%c%ld%c", markerCharacter, runLength, markerCharacter);
        writeAllBytes(destinationFileDescriptor, compressedMarkerText, markerTextLength);
    }
    else{
        // Run is too short to compress so it will be written as the original characters.
        for (long i = 0; i < runLength; i++){
            writeSingleCharacter(destinationFileDescriptor, runCharacter);
        }
    }
}

// WriteSingleCharacter: Writes a single character to the destination file using the write() system call that checks for failure.
static void writeSingleCharacter(int destinationFileDescriptor, char characterToWrite){
    writeAllBytes(destinationFileDescriptor, &characterToWrite, 1);
}

// writeAllBytes: Loops until all the requested bytes are written to the destination file. uses the write() system call. 
static void writeAllBytes(int destinationFileDescriptor, const char *dataBuffer, long numberOfBytesToWrite){
    long totalBytesWrittenSoFar = 0; // Bytes written so far
    while (totalBytesWrittenSoFar < numberOfBytesToWrite){
        ssize_t bytesWrittenThisCall = write(destinationFileDescriptor, dataBuffer + totalBytesWrittenSoFar,numberOfBytesToWrite - totalBytesWrittenSoFar);
        if (bytesWrittenThisCall == -1){
            perror("Error writing to destination file");
            exit(EXIT_FAILURE);
        }
        totalBytesWrittenSoFar += bytesWrittenThisCall;
    }
}
