/**
* gcc with -lpthread
*/

#include "Common.h"

struct foo {
    int arg1;
    int arg2;
};

struct foo returnStatus;

void* threadFunction(void* arg)
{
    struct foo* argFoo = (struct foo*)arg;
    printf("%d %d\n", argFoo->arg1, argFoo->arg2);
    returnStatus.arg1 = 3;
    returnStatus.arg2 = 4;
    return (void*)&returnStatus;
}

int main()
{
    pthread_t tid;
    struct foo arg = {1, 2};
    int res = pthread_create(&tid, NULL, threadFunction, (void*)&arg);
    if (res != 0){
        fprintf(stderr, "pthread create error: %d\n", res);
        return 0;
    }

    void* status;
    int joinRes = pthread_join(tid, &status);
    if (joinRes != 0){
        fprintf(stderr, "pthread join error: %d\n", joinRes);
        return 0;
    }
    struct foo* returnStatus = (struct foo*)status;
    printf("%d %d\n", returnStatus->arg1, returnStatus->arg2);

    return 0;
}