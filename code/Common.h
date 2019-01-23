#include <unistd.h>
#include <stdio.h>
#include <string.h>

#include <stdlib.h>
#include <fcntl.h>
#include <error.h>
#include <errno.h>
#include <stddef.h>

#include <netinet/in.h>
#include <arpa/inet.h>
#include <assert.h>

#include <sys/un.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <semaphore.h>
#include <pthread.h>