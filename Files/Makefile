# Students: Esmeralda Amado, Cristian Hernandez Juan, Gustavo Trejo
# Class: CS 4440-01 (Operating Systems)
# A single makefile that compiles all C files  in the current directory (/Files) and creates
# an executable for each C file. 


# C Compiler used 
CC = gcc
# Flags for the compiler
CFLAGS = -Wall -Wextra -g
 
# Finds every .C File in the current directory (Files) and names the executables. 
Source = $(wildcard *.c)
Executables = $(Source:.c=)
 
# Builds all executables 
all: $(Executables)

# Builds any executable from its matching .c file. 
%: %.c
	$(CC) $(CFLAGS) -o $@ $< $(LDLIBS)
 
# Remove every executable that was built 
clean:
	rm -f $(Executables)
.PHONY: all clean
