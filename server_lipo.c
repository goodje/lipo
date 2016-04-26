//
// Created by Liuwenjie on 4/24/16.
//

#include "server_lipo.h"

#define MAXLINE 5
#define LISTENQ 20

void main()
{
    printf("starting...\n");
    int i, listenfd, conn_fd, sockfd,epfd,nfds;
    ssize_t len;
    char *error_str[128];

    char buffer[MAXLINE];

    socklen_t clilen;
    struct sockaddr_in client_addr;
    struct sockaddr_in serveraddr;
    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    socket_set_nonblocking(listenfd);

    struct epoll_event ev;
    struct epoll_event events[20];
    epfd = epoll_create(256);
    ev.data.fd = listenfd;
    ev.events = EPOLLIN | EPOLLET;

    epoll_ctl(epfd, EPOLL_CTL_ADD, listenfd, &ev);

    char *local_addr = "::1";//todo
    int port = 6424;
    bzero(&serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    inet_aton(local_addr, &(serveraddr.sin_addr));
    serveraddr.sin_port = htons(port);

    if (bind(listenfd, (struct sockaddr*)&serveraddr, sizeof(serveraddr)) < 0)
    {
        sprintf(error_str, "bind error(%d)", errno);
        perror((const char*)error_str);
        exit(-1);
    }

    if (listen(listenfd, LISTENQ) < 0)
    {
        sprintf(error_str, "listen error(%d)", errno);
        perror((const char*)error_str);
        exit(-1);
    }

    while(1)
    {
        //event loop
        nfds = epoll_wait(epfd, events, 20, 500);

        for (i = 0; i < nfds; ++i)
        {
            if (events[i].data.fd == listenfd) //client connect
            {
                conn_fd = accept(listenfd, (struct sockaddr*)&client_addr, &clilen);
                if (conn_fd < 0)
                {
                    perror("conn_fd < 0");
                    exit(-1);
                }
                socket_set_nonblocking(conn_fd);

                char *client_ip = inet_ntoa(client_addr.sin_addr);
                printf("accept connect from %s.\n", client_ip);

                ev.data.fd = conn_fd;
                ev.events = EPOLLIN | EPOLLET;
                epoll_ctl(epfd, EPOLL_CTL_ADD, conn_fd, &ev);
            }
            if (events[i].events & EPOLLIN) //data received
            {
                if ((sockfd = events[i].data.fd) < 0)
                    continue;

                if ((len = read(sockfd, buffer, MAXLINE)) < 0)
                {
                    if (errno == ECONNRESET)
                    {
                        close(sockfd);
                        events[i].data.fd = -1;
                    }
                    else
                    {
                        //read err
                    }
                }
                else if (len == 0)
                {
                    close(sockfd);
                    events[i].data.fd = -1;
                    //todo
                }
                else
                {
                    printf("size: %d, data: %s\n", (int)len, buffer);

                    //write(sockfd, buffer, len);
                }

                //ev.data.fd = sockfd;
                //ev.events = EPOLLOUT | EPOLLET;
                //epoll_ctl(epfd, EPOLL_CTL_MOD, sockfd,&ev);
            }
           if (events[i].events & EPOLLOUT) //data sent
           {
               printf("epollout.\n");
               sockfd = events[i].data.fd;
               write(sockfd, buffer, len);

               ev.data.fd = sockfd;
               ev.events = EPOLLIN | EPOLLET;

               epoll_ctl(epfd, EPOLL_CTL_MOD, sockfd, &ev);
           }
        }
    }
}
