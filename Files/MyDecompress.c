/*
    Students: Esmeralda Amado, Cristian Hernandez Juan, Gustavo Trejo
    Class: CS 4440-01 (Operating Systems)
    Description: Decompresses a file that was compressed by the MyCompress.C program, using systems calls and outputs a decompressed file. 
*/

// Importing Libraries 
#include <stdio.h>  // For input and output 
#include <stdlib.h> // Standard library functions
#include <unistd.h> // POSIX System calls   
#include <fcntl.h>  // For File Control options 
#include <sys/stat.h> // For File info and permissions 


// number of bytes pulled from the file per read() call 
#define READ_CHUNK_SIZE_IN_BYTES 4096
// Max num of digits a run() length can have 
#define MAX_RUN_LENGTH_DIGITS 32

// Function declarations
static void decompressFileDescriptor(int sourceFileDescriptor, int destinationFileDescriptor);
static void writeAllBytes(int destinationFileDescriptor, const char *dataBuffer, long numberOfBytesToWrite);
static void writeSingleCharacter(int destinationFileDescriptor, char characterToWrite);
static void writeRepeatedCharacter(int destinationFileDescriptor, char characterToRepeat, long repeatCount);


// Main function
// Opens the compressed input file input and output file, runs the decompression and then closes both files safely
int main(int argumentCount, char *argumentValues[]){
    // Expects the arguments from the user: the input file and the output file 
    if (argumentCount != 3){
        fprintf(stderr, "Usage: %s <compressed_source_file> <decompressed_destination_file>\n", argumentValues[0]);
        exit(EXIT_FAILURE);
    }


    const char *sourceFilePath = argumentValues[1]; // Compressed input file path
    const char *destinationFilePath = argumentValues[2]; // Decompressed output file path
    
    // Opens the compressed input file for reading only 
    int sourceFileDescriptor = open(sourceFilePath, O_RDONLY);

    // Runs if there is an error opening the source file
    if (sourceFileDescriptor == -1){
        perror("Error opening source file");
        exit(EXIT_FAILURE);
    }

    // Opens/Creates the output file: trunucates if it exists, owner can read and write while the others can only read it 
    int destinationFileDescriptor = open(destinationFilePath, O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    
    // Runs if there is an error opening the destination file
    if (destinationFileDescriptor == -1){
        perror("Error opening destination file");
        close(sourceFileDescriptor); 
        exit(EXIT_FAILURE);
    }

    // Starts the decompression process
    decompressFileDescriptor(sourceFileDescriptor, destinationFileDescriptor);

    // Safely close the input/source file 
    if (close(sourceFileDescriptor) == -1){
        perror("Error closing source file");
        exit(EXIT_FAILURE);
    }

    // Safely close the destination file
    if (close(destinationFileDescriptor) == -1){
        perror("Error closing destination file");
        exit(EXIT_FAILURE);
    }
    // Prints a success message 
    printf("Decompression complete. '%s' -> '%s'\n", sourceFilePath, destinationFilePath);

    // Exits 
    return EXIT_SUCCESS;
}

// Reads the compressed file in chunks and expands the run-length markers back to their original form of '1' and '0'
static void decompressFileDescriptor(int sourceFileDescriptor, int destinationFileDescriptor){
    char readBuffer[READ_CHUNK_SIZE_IN_BYTES]; // Holds one chunk of the compressed/input file 
    int insideToken = 0; // Indicates if it is currently reading a compressed token 
    char tokenDelimiter = 0; // The marker: "+" or "-"
    char digitBuffer[MAX_RUN_LENGTH_DIGITS + 1]; // Buffer to hold the number string representing the run length 
    int digitCount = 0;   // How many digits have been collected so far in the digitBuffer


    // Loops until the entire compressed file is processed
    while (1){
        // Reads a chunk of the compressed file 
        ssize_t bytesReadThisCall = read(sourceFileDescriptor, readBuffer, READ_CHUNK_SIZE_IN_BYTES);
        
        // Checks for errors while reading the file
        if (bytesReadThisCall == -1){
            perror("Error reading source file");
            exit(EXIT_FAILURE);
        }
        // If no more bytes are available it means the file has been fully read
        if (bytesReadThisCall == 0){
            break;
        }

        // Processes the bytes read from the file
        for (ssize_t i = 0; i < bytesReadThisCall; i++){
            // Processes each character in the current chunk
            char currentCharacter = readBuffer[i];

            // Case 1: Found the start of a new compression token ("+" or "-") 
            if (!insideToken && (currentCharacter == '+' || currentCharacter == '-')){
                // Initialize the token
                insideToken = 1;
                // Set the token delimiter
                tokenDelimiter = currentCharacter;
                // Reset the digit count for the new token
                digitCount = 0;
            }

            // Case 2: Found the end of the current compression token 
            else if (insideToken && currentCharacter == tokenDelimiter){
                // Matching delimiter closes the marker: turn the digits into a number
                digitBuffer[digitCount] = '\0';
                long runLength = atol(digitBuffer);

                // "+" expands to '1', "-" expands to '0'
                char characterToExpand = (tokenDelimiter == '+') ? '1' : '0';
                writeRepeatedCharacter(destinationFileDescriptor, characterToExpand, runLength);

                // Reset the token state 
                insideToken = 0;
                digitCount = 0;
            }
            // Currently inside a compression token and reading a digit
            else if (insideToken){
                // If the current character is a digit, add it to the digit buffer
                if (digitCount < MAX_RUN_LENGTH_DIGITS){
                    digitBuffer[digitCount] = currentCharacter;
                    digitCount++;
                }
            }
            // Plain char that wasn't compressed is copied straight to the destination
            else{
                writeSingleCharacter(destinationFileDescriptor, currentCharacter);
            }
        }
    }
}

// Writes the given character a specified number of times to the destination file
static void writeRepeatedCharacter(int destinationFileDescriptor, char characterToRepeat, long repeatCount){
    for (long i = 0; i < repeatCount; i++){
        writeSingleCharacter(destinationFileDescriptor, characterToRepeat);
    }
}

// Writes a single character to the destination file
static void writeSingleCharacter(int destinationFileDescriptor, char characterToWrite){
    writeAllBytes(destinationFileDescriptor, &characterToWrite, 1);
}

// Loops on write() since it may not write all the bytes in a single call
static void writeAllBytes(int destinationFileDescriptor, const char *dataBuffer, long numberOfBytesToWrite){
    long totalBytesWrittenSoFar = 0; // Num of bytes written so far

    while (totalBytesWrittenSoFar < numberOfBytesToWrite){
        ssize_t bytesWrittenThisCall = write(destinationFileDescriptor, dataBuffer + totalBytesWrittenSoFar,numberOfBytesToWrite - totalBytesWrittenSoFar);
        // Handles error case
        if (bytesWrittenThisCall == -1){
            perror("Error writing to destination file");
            exit(EXIT_FAILURE);
        }
        // Update the total number of bytes written so far
        totalBytesWrittenSoFar += bytesWrittenThisCall;
    }
}
