#include "Common.h"

int main()
{
    int fd;
    const char* pathname = "foo.lock";

    if (fd = open(pathname, O_CREAT | O_EXCL, S_IWUSR) > 0) {
        printf("create new file\n");
        close(fd);
    }


    fd = open(pathname, O_WRONLY);
    if (fd < 0) {
        perror("open error");
        return 0;
    }

    struct flock lock;
    lock.l_type = F_WRLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start = 0;
    lock.l_len = 0;
    int res = fcntl(fd, F_SETLKW, &lock);
    if (res < 0) {
        perror("setlkw wr error");
        close(fd);
        return 0;
    }
    printf("do something with lock\n");
    sleep(5);

    lock.l_type = F_UNLCK;
    res = fcntl(fd, F_SETLKW, &lock);
    if (res < 0) {
        perror("setlkw un error");
        close(fd);
        return 0;
    }

    close(fd);
    return 0;
}