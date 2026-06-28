# Students: Esmeralda Amado, Cristian Hernandez Juan, Gustavo Trejo
# Class: CS 4440-01 (Operating Systems)
# A single makefile that compiles all C files  in the current directory (/Files) and creates
# an executable for each C file. 



# C compiler used
CC = gcc
 
# Flags for the compiler
CFLAGS = -Wall -Wextra -g
 
# Finds every .c file in the Files Directory, and turns each file into an executable 
SOURCES = $(wildcard *.c)
EXECUTABLES = $(SOURCES:.c=)
 
# Builds every executable in the Files Folder.
all: $(EXECUTABLES)
 

%: %.c
	$(CC) $(CFLAGS) -o $@ $< $(LDLIBS)
 
# For Pthread, link the POSIX threads library.
ParThread: LDLIBS = -pthread
 
# Remove every executable that was built.
clean:
	rm -f $(EXECUTABLES)

.PHONY: all clean