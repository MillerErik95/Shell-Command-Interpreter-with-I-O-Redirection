Simple Shell Command Interpreter with I/O Redirection

This project is a C-based shell-like command interpreter designed to run in a Linux Bash shell environment. The program reads user input, processes command-line arguments, and supports basic input/output redirection. It handles common redirection operators such as > for output redirection, < for input redirection, and >& for combined stdout and stderr redirection.

The program uses fork() to create a child process for command execution and execvp() to execute the commands. It also includes a signal handler to catch SIGINT (Ctrl+C) and properly handle interruptions.

Key features include:

Reading and tokenizing user input.
Handling redirection operators to allow users to redirect input and output to files.
Using fork() to create child processes for executing commands.
Restoring original file descriptors after redirection.
Signal handling for graceful termination upon user interruption (Ctrl+C).
This simple command interpreter provides a foundation for understanding process control, file I/O, and signal handling in a Unix-like operating system. It is an educational tool for learning about system calls and shell scripting in C.
