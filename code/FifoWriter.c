#include "Common.h"

int main()
{
    const char* name = "/home/vagrant/fifo";
    if (access(name, F_OK) < 0) {
        mode_t RWXUSR = S_IRUSR | S_IWUSR | S_IXUSR;
        if (mkfifo(name, RWXUSR) < 0) {
            perror("mkfifo error");
            return 0;
        } else {
            printf("fifo created\n");
        }
    } else {
        printf("fifo created\n");
    }

    int fd = open(name, O_WRONLY);
    if (fd < 0) {
        perror("open error");
    }
    write(fd, "hello world\n", 12);

    return 0;
}