# Files & Descriptions:

- MyCompress.c
  - **(Q1):** Compresses a copy of a file (Made of 1's and 0's) using System calls. Needs an input (.TXT) in order to compress and outputs a .TXT file
- MyDecompress.
  - **(Q2):** Reads a compressed file produced by MyCompress.c and outputs a decompressed output file
- ForkCompress.c
  - **(Q3):** Uses the FORK() system call to create a new process and uses EXECL() to run and execute MyCompress.c then uses WAIT() for it to finish.
- PipeCompress.c
  - **(Q4):** Forks two processes connected by a pipe, one for reading from a source file and the other writing the compressed output
- ParFork.c
  - **(Q5):** Splits a large input file into n chunks and forks n processes to compress each chunk at the same time and finnaly assembles the results.
- MinShell.c
  - **(Q6):** A simple shell program that prompts the user and reads argument-less commands and executes it
- MoreShell.c
  - **(Q7):** Builds upon Minshell.c to allow support for commands with arguments.
- DupShell.c
  - **(Q8):** Extends upon MoreShell.C to execute commands connected by pipes **'|'**
- ParThread.c
  - **(Q9):** Uses the POSIX threads library to compress a large file by using the thread model for concurrency

# Other Files:

- Makefile
  - Compiles all .C files into executables
- Input.txt
  - Large 50.4 MB File of '0's and '1's
- Typescript
  - Shows the working of all the programs for correct input as well as gracefull exits on error input

# Problem 10 (Time Information)

- Uses the 50.4 mb file as input to test run time

| Run Time Information            |                                                                                                                                                                                                          |
| ------------------------------- | -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| **(i) Sequential (MyCompress)** | <ul><li>Runtime 1: 0.21s user 2.10s system 99% cpu 2.336 total</li><li>Runtime 2: 0.22s user 2.09s system 99% cpu 2.331 total </li><li>Runtime 3: 0.22s user 2.53s system 99% cpu 2.774 total </li></ul> |
| **(ii) Forked (ParFork)**       | <ul><li>INSERT</li><li>INSERT</li><li>INSERT </li></ul>                                                                                                                                                  |
| **(iii) PThread (ParThread)**   | <ul><li>INSERT</li><li>INSERT</li><li>INSERT</li></ul>                                                                                                                                                   |
