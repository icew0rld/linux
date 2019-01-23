#include "Common.h"

int main()
{
    const key_t KEY = 422;
    const size_t SIZE = 100;
    const int MODE = S_IRUSR | S_IWUSR;

    int shmid = shmget(KEY, SIZE, MODE | IPC_CREAT);
    if (shmid < 0) {
        perror("shmget error");
        return 0;
    }

    char* addr = shmat(shmid, 0, 0);
    if (addr < 0) {
        perror("shmat error");
        return 0;
    }

    strcpy(addr, "hello");

    pid_t pid = fork();
    if (pid > 0) {
        int status;
        pid_t teminatedPid = wait(&status);
        printf("%s", addr);
    } else {
        int shmidInChild = shmget(KEY, SIZE, MODE);
        if (shmidInChild < 0) {
            perror("shmget error");
            return 0;
        }
        char* addrInChild = shmat(shmidInChild, 0, 0);
        if (addrInChild < 0) {
            perror("shmat error");
            return 0;
        }
        strcat(addrInChild, " world\n");
    }

    return 0;
}