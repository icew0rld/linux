#include "Common.h"

int main()
{
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) {
        perror("socket error");
        return 0;
    }

    struct sockaddr_in serverAddr;
    bzero(&serverAddr, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(8001);
    inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr);

    if (bind(fd, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        perror("bind error");
        close(fd);
        return 0;
    }

    if (listen(fd, 10) < 0) {
        perror("listen error");
        close(fd);
        return 0;
    }

    struct sockaddr_in clientAddr;
    socklen_t clientAddrLen = sizeof(clientAddr);
    int connfd = accept(fd, (struct sockaddr*)&clientAddr, &clientAddrLen);
    if (connfd < 0) {
        perror("accept error");
        close(fd);
        return 0;
    }

    char remote[INET_ADDRSTRLEN];
    printf("connected with ip:%s and port:%d\n",
        inet_ntop(AF_INET, &clientAddr.sin_addr, remote, INET_ADDRSTRLEN),
        ntohs(clientAddr.sin_port));

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