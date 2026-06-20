/*
    Students: Esmeralda Amado, Cristian Hernandez Juan, Gustavo Trejo
    Class: CS 4440-01 (Operating Systems)
    Description: Compresses a copy of a file using system calls
*/


// Importing Libraries:  
#include <stdio.h>  // Input/Output
#include <unistd.h> // Linux system calls 
#include <fcntl.h>  // File control 
#include <stdlib.h> // Standard Library 


// Num of Bytes pulled from source file per read 
#define bufferSize 4096
// Run of bits must be this long to be compressed 
#define runLimit 16 
#define tokenSize 32 


/* Write All: 
    Writes all bytes from a buffer to a file descriptor. Returns 0 on success and -1 on error.
    Uses the Write() system call to write all bytes. 
*/
static int writeAll(int destinationFile, const char *buffer, size_t totalBytes){
    size_t bytesWrittenSoFar = 0; 

    while (bytesWrittenSoFar < totalBytes) {
        ssize_t result = write (destinationFile, buffer + bytesWrittenSoFar, totalBytes - bytesWrittenSoFar);
        
        // The write call was interupted, so try to write again 
        if (result < 0) {
            if (errno == EINTR) {
                continue;
            }
            return -1;
        }
        bytesWrittenSoFar += (size_t)result; 
    }
    return 0; // All bytes were written successfully 
}



