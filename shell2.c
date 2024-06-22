#include "apue.h"
#include <sys/wait.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

static void sig_int(int); /* our signal-catching function */

int main() {
    char buf[MAXLINE];    /* from apue.h */
    char* args[MAXLINE];  /* string of arguments */
    pid_t pid;
    int status;

    if (signal(SIGINT, sig_int) == SIG_ERR)
        err_sys("signal error");

    printf("%% "); /* print prompt (printf requires %% to print %) */
    while (fgets(buf, MAXLINE, stdin) != NULL) {
        if (buf[strlen(buf) - 1] == '\n')
            buf[strlen(buf) - 1] = 0; /* replace newline with null */

        char* arg = strtok(buf, "\t\n "); /* points to arguments, delimited by spaces */
        int i = 0;
        int fd, stdin_saved = -1, stdout_saved = -1, stderr_saved = -1;

        while (arg != NULL) { /* loops and stores arguments together */
            /* Handles > case*/
            if (strcmp(arg, ">") == 0) {
                arg = strtok(NULL, " ");
                if ((fd = open(arg, O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR)) < 0) {
                    err_sys("open error");
                }
                stdout_saved = dup(1);
                if (dup2(fd, 1) < 0) {
                    err_sys("dup2 error");
                }
                close(fd);
            }
            /* Handles < case*/
            else if (strcmp(arg, "<") == 0) {
                arg = strtok(NULL, " ");
                if ((fd = open(arg, O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR)) < 0) {
                    err_sys("open error");
                }
                stdin_saved = dup(0);
                if (dup2(fd, 0) < 0) {
                    err_sys("dup2 error");
                }
                close(fd);

                /* Revert the redirected file descriptors to the saved ones(for stdin) */
                if (stdin_saved > -1) {
                    close(0);
                    dup2(stdin_saved, 0);
                    close(stdin_saved);
                }
            }
            /* Handles >& case*/
            else if (strcmp(arg, ">&") == 0) {
                arg = strtok(NULL, " ");
                if (strcmp(arg, "1") == 0) { /* Handles "2>&1" case */
                    stderr_saved = dup(2);
                    if (dup2(1, 2) < 0) {
                        err_sys("dup2 error");
                    }
                }
                else {
                    if ((fd = open(arg, O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR)) < 0) {
                        err_sys("open error");
                    }
                    stdout_saved = dup(1);
                    if (dup2(fd, 1) < 0 || dup2(fd, 2) < 0) {
                        err_sys("dup2 error");
                    }
                    close(fd);
                }
            }
            else {
                args[i++] = arg;
                arg = strtok(NULL, " ");
            }
        }

        args[i] = NULL;

        if ((pid = fork()) < 0) {
            err_sys("fork error");
        }
        else if (pid == 0) { /* child */
            execvp(args[0], args); /* input the arguments */
            err_ret("couldn't execute: %s", buf);
            exit(127);
        }

        /* parent */
        if ((pid = waitpid(pid, &status, 0)) < 0)
            err_sys("waitpid error");

        /* Revert the redirected file descriptors to the saved ones */
        if (stdout_saved > -1) {
            close(1);
            dup2(stdout_saved, 1);
            close(stdout_saved);
            stdout_saved = -1;
        }

        if (stderr_saved > -1) {
            close(2);
            dup2(stderr_saved, 2);
            close(stderr_saved);
            stderr_saved = -1;
        }

        printf("%% ");
    }
    exit(0);
}

void sig_int(int signo) {
    printf("interrupt\n%% ");
}
