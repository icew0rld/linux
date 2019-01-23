#include "Common.h"

int main()
{
    int fd[2];
    if (pipe(fd) < 0) {
        printf("pipe error\n");
        return 0;
    }
    int pid = fork();
    if (pid < 0) {
        printf("fork error\n");
    } else if (pid > 0) {
        close(fd[0]);
        write(fd[1], "hello world\n", 12);
        int status;
        pid_t teminatedPid = wait(&status);
    } else {
        close(fd[1]);
        int MAXLINE = 80;
        char line[MAXLINE];
        int n = read(fd[0], line, MAXLINE);
        write(STDOUT_FILENO, line, n);
    }
    return 0;
}