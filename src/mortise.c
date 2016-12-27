//
// Created by Liuwenjie on 4/24/16.
//


#include "mortise.h"

#define MAXLINE 256
#define LISTENQ 20
#define MAX_EVENTS 256

//How does a socket event get propagated/converted to epoll?
//http://stackoverflow.com/questions/18103093/how-does-a-socket-event-get-propagated-converted-to-epoll

void signal_handler(int signal_id)
{
    if (Mortise.sock > 0)
    {
        close(Mortise.sock);
        log_debug("caught signal. signal_id[%d]", signal_id);
    }
    Mortise.running = 0;
}

int signal_init()
{
    // catch SIGINT to exit in a clean way
    struct sigaction sa;
    memset(&sa, 0x0, sizeof(struct sigaction));
    sa.sa_handler = signal_handler;
    sa.sa_flags = 0;
    sigemptyset (&(sa.sa_mask));
    if (sigaction(SIGINT, &sa, NULL) != 0)// catch ctrl+c
    {
        perror("sigaction failed");
        exit(1);
    }
    return 0;
}

void mortise_init()
{
    Mortise.sock = 0;
    Mortise.running = 1;
    Mortise.connect_timeout = 1;
    Mortise.mortise_tenon.conn_count = 0;
    Mortise.mortise_tenon.conns = NULL;
}

void mortise_free()
{
    struct mortise_tenon *conn;
    struct mortise_tenon *next = Mortise.mortise_tenon.conns;
    Mortise.mortise_tenon.conns = NULL;

    while(next)
    {
        conn = next;
        next = conn->next != NULL ? conn->next : NULL;
        mortise_tenon_close(conn);
    }
}

void main()
{
    int i, j, listenfd, client_fd, sockfd, epfd, nfds;

    log_trace("%s", "starting...");


    //global events engine
    //event_engine_init(&(Tenon->event_engine), Tenon->config.mortise_conn_count);
    //event_manage_init(&(Tenon->event_manage));

    ssize_t len;
    ssize_t send_len;
    char *error_str[128];
    char buffer[MAXLINE];

    int optval;
    int optlen = sizeof(optval);

    struct sockaddr_in serveraddr;
    struct sockaddr_in client_addr;
    socklen_t clilen = sizeof(struct sockaddr);

    //init
    logging_init();
    mortise_init();
    signal_init();

    struct epoll_event ev;
    struct epoll_event events[MAX_EVENTS];
    epfd = epoll_create(MAX_EVENTS);



    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    Mortise.sock = listenfd;
    socket_set_nonblocking(listenfd);
    ev.data.fd = listenfd;
//    ev.events = EPOLLIN | EPOLLPRI | EPOLLOUT | EPOLLRDNORM | EPOLLRDBAND | EPOLLWRNORM | EPOLLWRBAND | EPOLLMSG | EPOLLERR
//                | EPOLLHUP | EPOLLRDHUP;
    ev.events = EPOLLIN | EPOLLOUT | EPOLLERR;
    // |EPOLLWAKEUP | EPOLLONESHOT
    epoll_ctl(epfd, EPOLL_CTL_ADD, listenfd, &ev);

    char *local_addr = "::1";//todo
    int port = 6424;
    bzero(&serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    inet_aton(local_addr, &(serveraddr.sin_addr));
    serveraddr.sin_port = htons(port);

    //reuse socket
    int on = 1;
    if((setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on))) < 0)
    {
        log_error("%s", "setsockopt failed");
        exit(-1);
    }

    if (bind(listenfd, (struct sockaddr*)&serveraddr, sizeof(serveraddr)) < 0)
    {
        log_error("%s", "bind error");
        exit(-1);
    }

    if (listen(listenfd, LISTENQ) < 0)
    {
        log_error("%s", "listen port error");
        exit(-1);
    }

    while(Mortise.running)
    {
        //event loop
        nfds = epoll_wait(epfd, events, MAX_EVENTS, 1000);

        for (i = 0; i < nfds; ++i)
        {
            log_trace("event loop count:%d i:%d events:%d fd:%d", nfds, i, events[i].events, events[i].data.fd);

            //client connect
            if (events[i].data.fd == listenfd)
            {
                memset(&client_addr, 0x0, clilen);
                client_fd = accept(listenfd, (struct sockaddr*)&client_addr, &clilen);

                if (mortise_tenon_init(client_fd, &client_addr) != 0)
                {
                    continue;
                }

                ev.data.fd = client_fd;
                ev.events = EPOLLIN;// | EPOLLET;
                epoll_ctl(epfd, EPOLL_CTL_ADD, client_fd, &ev);
                continue;
            }

            //data received
            if (events[i].events & EPOLLIN)
            {
                //EPOLLIN and EPOLLRDNORM are identical
                // (epoll returns EPOLLIN | EPOLLRDNORM when data is available for reading from the file descriptor).

                log_trace("EPOLLIN fd:%d", events[i].data.fd);

                if ((sockfd = events[i].data.fd) < 0)
                    continue;//todo
                memset(buffer, 0x0, sizeof(buffer));

                if ((len = recv(sockfd, buffer, MAXLINE, 0)) < 0)
                {
                    if (errno == ECONNRESET)
                    {
                        close(sockfd);
                        log_error("recv error. fd[%d]", events[i].data.fd);
                        events[i].data.fd = -1;
                    }
                    else
                    {
                        log_error("recv error. fd[%d]", events[i].data.fd);
                        //todo read err
                    }
                }
                else if (len == 0) //handle FIN from client
                {
                    //getsockopt(events[i].data.fd, SOL_SOCKET, SO_ERROR, &optval, &optlen);

                    close(sockfd);
//                    events[i].data.fd = -1;
                    log_debug("client closed connection. client_fd[%d]", events[i].data.fd);
                    //todo
                }
                else
                {
                    //printf("recv data. fd[%d] size[%d], str_len[%d] data[%s]\n", sockfd, (int)len, (int)strlen(buffer), buffer);

                    char *req_trimmed;
                    strcmp(buffer, req_trimmed);
                    //trim(buffer, req_trimmed); todo

                    log_debug("recv data. fd[%d] size[%d], data_trimmed_len[%d] data_trimmed[%s]", sockfd, (int)len, (int)strlen(req_trimmed), req_trimmed);

                    for (j = 1; j < strlen(req_trimmed); j++);// { printf("ascii: %d\n", req_trimmed[j]); } // why must show. todo
                    if (strcmp(req_trimmed, "1") == 0)
                    {
                        printf("http\n");

                        struct http_client *client = malloc(sizeof(struct http_client));

                        http_init(client);

                        http_request("http://goodje.com/");
                    }
                    else
                    {
                        send_len = write(sockfd, buffer, len);

                        //printf("send data. fd[%d] data[%s] len[%d] sent_len[%d]\n", sockfd, buffer, len, send_len);
                        log_debug("send data. fd[%d] data[%s] len[%d] sent_len[%d]", sockfd, "[the same to received]", len, send_len);
                    }
                }

                //ev.data.fd = sockfd;
                //ev.events = EPOLLOUT | EPOLLET;
                //epoll_ctl(epfd, EPOLL_CTL_MOD, sockfd,&ev);
            }

            //有连接建立？
            if (events[i].events & EPOLLOUT)
            {
                //EPOLLOUT and EPOLLWRNORM are identical
                // (epoll returns EPOLLOUT | EPOLLWRNORM when buffer space is available for writing).
                printf("epollout fd:%d.\n", events[i].data.fd);
                sockfd = events[i].data.fd;
                write(sockfd, buffer, len);

                ev.data.fd = sockfd;
                //ev.events = EPOLLIN | EPOLLET;

                epoll_ctl(epfd, EPOLL_CTL_MOD, sockfd, &ev);
            }

            if (events[i].events & EPOLLERR)
            {
                log_error("%s", "EPOLLERR");
            }

            if (events[i].events & EPOLLHUP)
            {
                log_error("%s", "EPOLLHUP");
            }

            if (events[i].events & EPOLLRDHUP)
            {
                log_error("%s", "EPOLLRDHUP");
            }

        }
    }

    //end
    mortise_free();

    log_trace("%s", "mortise end.");
    sleep(10);
}
