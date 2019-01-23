#include "Common.h"

int main()
{

    int fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (fd < 0) {
        perror("socket error");
        return 0;
    }

    const char* pathname = "/home/vagrant/foo.sock";
    unlink(pathname);

    struct sockaddr_un serverAddr;
    bzero(&serverAddr, sizeof(serverAddr));
    serverAddr.sun_family = AF_UNIX;
    strcpy(serverAddr.sun_path, pathname);
    int len = offsetof(struct sockaddr_un, sun_path) + strlen(pathname);

    if (bind(fd, (struct sockaddr*)&serverAddr, len) < 0) {
        perror("bind error");
        close(fd);
        return 0;
    }

    if (listen(fd, 10) < 0) {
        perror("listen error");
        close(fd);
        return 0;
    }

    struct sockaddr_un clientAddr;
    socklen_t clientAddrLen = sizeof(clientAddr);
    int connfd = accept(fd, (struct sockaddr*)&clientAddr, &clientAddrLen);
    if (connfd < 0) {
        perror("accept error");
        close(fd);
        return 0;
    }

    char buf[100];
    ssize_t recvN =  recv(connfd, buf, 100, 0);
    if (recvN < 0) {
        perror("recv error");
        close(connfd);
        close(fd);
        return 0;
    }

    strcat(buf, " world\n");
    ssize_t n = send(connfd, buf, 12, 0);
    if (n < 0) {
        perror("send error");
        close(connfd);
        close(fd);
        return 0;
    }

    close(connfd);
    close(fd);
    return 0;
}