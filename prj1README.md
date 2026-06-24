# Files & Descriptions:

- MyCompress.c
  - **(Q1):** Compresses a copy of a file (Made of 1's and 0's) using System calls. Needs an input (.TXT) in order to compress and outputs a .TXT file
- MyDecompress.c
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
  - **(Q9):**
- Makefile
  - Compiles all .C files into executables

# Problem 10 (Time Information)

| Sequential | Forked (Process) | Pthread |
| ---------- | ---------------- | ------- |
| Blank      | Blank            | Blank   |
| Blank      | Blank            | Blank   |
