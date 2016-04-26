//
// Created by Liuwenjie on 4/24/16.
//

#include <printf.h>
#include <stdlib.h>
#include <string.h>
#include <err.h>
#include "client_lipo.h"


#define MAXLINE 5
#define LIPOD_CON_COUNT 10 //count of connect with lipo server
//#define LIPOD_IP "103.251.90.122"
#define LIPOD_IP "127.0.0.1"
#define LIPOD_PORT 6424

void main()
{
    printf("starting...\n");

    int epfd, i, j, client_fd, optval, event_count;
    struct lipod_connection *lipod_conns[LIPOD_CON_COUNT];
    int lipod_conn_count = 0;
    socklen_t optlen;
    char *sock_val[128];
    char *error_str[128];

    char line[MAXLINE];
    struct epoll_event ev;
    epfd = epoll_create(256);

    //struct lipod_connection conn;
    for (i = 0; i < LIPOD_CON_COUNT; i++)
    {
        struct lipod_connection* conn = &(lipod_conns[i]);

        conn->socket_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

        strcpy(conn->lipod_ip, LIPOD_IP);
        conn->lipod_port = LIPOD_PORT;
        conn->status = LIPOD_CONN_DEFAULT;

        struct sockaddr_in sockaddr;
        sockaddr.sin_family = AF_INET;
        sockaddr.sin_port = htons(conn->lipod_port);
        struct sockaddr_in *psa = &sockaddr;
        inet_pton(AF_INET, (const char*)conn->lipod_ip, &psa->sin_addr.s_addr);

        ev.data.fd = conn->socket_fd;
        ev.events = EPOLLIN | EPOLLOUT | EPOLLET | EPOLLRDHUP | EPOLLHUP | EPOLLERR;
        epoll_ctl(epfd, EPOLL_CTL_ADD, conn->socket_fd, &ev);

        socket_set_nonblocking(conn->socket_fd);
        if (connect(conn->socket_fd, (const struct sockaddr*)&sockaddr, sizeof(sockaddr)) < 0)
        {
            if (errno != EINPROGRESS)
            {
                perror("connect fail.");
                continue;
            }

            printf("connect processing. socket_fd: %d\n", conn->socket_fd);
            conn->status = LIPOD_CONN_PROCESSING;
        }
        else
        {
            lipod_conn_count++;
            conn->status = LIPOD_CONN_CONNECTED;
            printf("connected.\n");
        }
    }

    struct epoll_event events[1024];
    while(1)
    {
        //event loop
        event_count = epoll_wait(epfd, events, 1024, 1024);

        for (i = 0; i < event_count; ++i)
        {
            printf("%d\n", event_count);

            printf("event detected1.\n");

            if (events[i].events & EPOLLERR)
            {
                printf("event detected EPOLLERR.\n");
            }

            printf("event detected1.\n");

            if (events[i].events & EPOLLOUT)
            {
                printf("event detected EPOLLOUT.\n");
                printf("fd: %d\n", events[i].data.fd);


                if (lipod_conn_count < LIPOD_CON_COUNT)
                {
                    for (j = 0; j < LIPOD_CON_COUNT; j++)
                    {
                        if (lipod_conns[j]->socket_fd != events[i].data.fd)
                            continue;

                        if (lipod_conns[j]->status == LIPOD_CONN_CONNECTED)
                            continue;

                        if (getsockopt(lipod_conns[j]->socket_fd, SOL_SOCKET, SO_ERROR, &optval,
                                       &optlen) == -1)
                            err(1, "getsockopt");

                        if (optval == 0)
                        {
                            printf("%s connected OK on "
                                           "port %d socket %d\n",
                                   lipod_conns[j]->lipod_ip, lipod_conns[j]->lipod_port,
                                   lipod_conns[j]->socket_fd);
                        }
                        else
                        {
                            printf("%s connect failed on "
                                           "port %d socket %d (%s)\n",
                                   lipod_conns[j]->lipod_ip, lipod_conns[j]->lipod_port,
                                   lipod_conns[j]->socket_fd,
                                   strerror(optval));
                        }
                        lipod_conn_count++;
                    }
                }


//                if (getsockopt(events[i].data.fd, SOL_SOCKET, SO_ERROR, sock_val, &sock_len) < 0)
//                {
//                    sprintf("getsockopt(%d)", (int)errno);
//                    perror("");
//                    // ERROR, fail somehow, close socket
//                }
//
//                printf("%d \n", sock_len);
            }

            if (events[i].events & EPOLLIN)
            {
                printf("event detected EPOLLIN.\n");
            }

        }
    }
}