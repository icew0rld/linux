#include "Common.h"

int setnonblocking(int sfd)
{
    int flags, s;
 
    // 得到文件状态标志
    flags = fcntl (sfd, F_GETFL, 0);
    if (flags == -1){
        perror ("fcntl");
        return -1;
    }
 
    // 设置文件状态标志
    flags |= O_NONBLOCK;
    s = fcntl (sfd, F_SETFL, flags);
    if (s == -1){
        perror ("fcntl");
        return -1;
    }
 
    return 0;
}


int main()
{
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0){
        perror("socket error");
        return 0;
    }

    struct sockaddr_in serverAddr;
    bzero(&serverAddr, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(8001);
    inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr);

    if (bind(fd, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0){
        perror("bind error");
        close(fd);
        return 0;
    }

    if (listen(fd, 10) < 0){
        perror("listen error");
        close(fd);
        return 0;
    }

    int epollfd = epoll_create1(0);
    if (epollfd < 0){
        perror("epoll create1 error");
        return 0;
    }

    struct epoll_event ev;
    ev.events = EPOLLIN;
    ev.data.fd = fd;
    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &ev) < 0){
        perror("epoll ctl epollfd error");
        return 0;
    }

    const int MAX_EVENTS = 10;
    struct epoll_event events[MAX_EVENTS];
    int nfds;
    while (1) {
        nfds = epoll_wait(epollfd, events, MAX_EVENTS, -1);
        if (nfds < 0) {
            perror("epoll wait error");
            return 0;
        }
        printf("epoll wait return\n");
        int i;
        for (i = 0; i < nfds; i++){
            if (events[i].data.fd == fd){
                printf("fd event\n");
                struct sockaddr_in clientAddr;
                socklen_t clientAddrLen = sizeof(clientAddr);
                int connfd = accept(fd, (struct sockaddr*)&clientAddr, &clientAddrLen);
                if (connfd < 0){
                    perror("accept error");
                }
                char remote[INET_ADDRSTRLEN];
                printf("connected with ip:%s and port:%d\n",
                    inet_ntop(AF_INET, &clientAddr.sin_addr, remote, INET_ADDRSTRLEN),
                    ntohs(clientAddr.sin_port));
                setnonblocking(connfd);
                ev.events = EPOLLIN | EPOLLET;
                ev.data.fd = connfd;
                if (epoll_ctl(epollfd, EPOLL_CTL_ADD, connfd, &ev) < 0){
                    perror("epoll_ctl: connfd");
                    close(connfd);
                }
            } else if (events[i].events & EPOLLIN){
                printf("read event\n");
                char buf[100];
                ssize_t recvN = recv(events[i].data.fd, buf, 100, 0);

                // client close the socket
                if (recvN == 0) {
                    close(events[i].data.fd);
                    continue;
                }
                if (recvN < 0) {
                    perror("recv error");
                    continue;
                }
                buf[recvN] = '\0';
                printf("recv %s\n", buf);
                strcat(buf, " world\n");
                ssize_t n = send(events[i].data.fd, buf, 12, 0);
                if (n < 0) {
                    perror("send error");
                    continue;
                }
                printf("send %s\n", buf);
            } else if (events[i].events & EPOLLOUT){
                printf("write event\n");
            } else {
                printf("other event\n");
            }
        }
    }

    close(fd);
    return 0;
}