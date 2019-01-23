/*
 * note: gcc with option -lpthread
*/
#include "Common.h"

int main()
{
    const char* semName = "/semaphore";
    sem_t* sem = sem_open(semName, O_CREAT, S_IWUSR | S_IRUSR, 1);
    if (sem == SEM_FAILED){
        perror("sem open error");
        return 0;
    }

    if (sem_wait(sem) < 0){
        perror("sem wait error");
        sem_close(sem);
        return 0;
    }

    printf("do something\n");
    sleep(5);

    if (sem_post(sem) < 0){
        perror("sem post error");
        sem_close(sem);
        return 0;
    }

    sem_close(sem);
    sem_unlink(semName);

    return 0;
}