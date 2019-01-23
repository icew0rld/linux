#include "Common.h"

int main()
{
    int fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (fd < 0) {
        perror("socket error");
        return 0;
    }

    const char* pathname = "/home/vagrant/foo.sock";
    struct sockaddr_un serverAddr;
    bzero(&serverAddr, sizeof(serverAddr));
    serverAddr.sun_family = AF_UNIX;
    strcpy(serverAddr.sun_path, pathname);
    int len = offsetof(struct sockaddr_un, sun_path) + strlen(pathname);


    if (connect(fd, (struct sockaddr*)&serverAddr, len) < 0) {
        perror("connect error");
        close(fd);
        return 0;
    }

    ssize_t n = send(fd, "hello", 5, 0);
    if (n < 0) {
        perror("send error");
        close(fd);
        return 0;
    }

    char buf[100];
    ssize_t recvN = recv(fd, buf, 100, 0);
    if (recvN < 0) {
        perror("recv error");
        close(fd);
        return 0;
    }

    printf("%s", buf);

    close(fd);

    return 0;
}