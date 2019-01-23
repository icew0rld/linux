#include <stdio.h>
int main()
{
    extern char** environ;
    int i;
    for (i = 0; environ[i] != NULL; i++){
        printf("environ[%d]:%s\n", i, environ[i]);
    }
    return 0;
}