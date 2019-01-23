#include "Common.h"

int main()
{
    const char* name = "/home/vagrant/fifo";
    
    int fd = open(name, O_RDONLY);
    if (fd < 0) {
        perror("open error");
    }
    int MAXLINE = 80;
    char line[MAXLINE];
    int n = read(fd, line, MAXLINE);
    write(STDOUT_FILENO, line, n);
    unlink(name);

    return 0;
}