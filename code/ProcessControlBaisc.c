#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
int main()
{

    pid_t pid = fork();
    int status;
    if (pid == 0){
        if (execl("./Hello", "dumpArg", (char*)0) < 0){
            printf("execl error\n");
            return 1;
        }
    } 

    pid_t teminatedPid = wait(&status);
    printf("child proccess teminated, pid: %d, status: %d\n", teminatedPid, status);
    exit(0);
}