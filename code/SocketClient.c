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


    if (connect(fd, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
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