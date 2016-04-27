//
// Created by Liuwenjie on 4/24/16.
//

#ifndef LEARNCPP_CLIENT_LIPO_H
#define LEARNCPP_CLIENT_LIPO_H

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <errno.h>
#include <string.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/time.h>
#include <pthread.h>
#include "lib_socket.h"

#define MORTISE_CONN_DEFAULT 0
#define MORTISE_CONN_PROCESSING 1
#define MORTISE_CONN_CONNECTED 2

//event type define
#define EVENT_TYPE_MAO_CONN 1

struct mortise_conn
{
    int socket_fd;
    char *mortise_ip[15]; //todo int is well
    unsigned int mortise_port;
    unsigned int local_port;
    int status;
};

extern struct mortise_conn *G_mortise_cons[];

struct tenon_config
{
    int mortise_conn_count;
    char *mortise_ip[15]; //todo int is well
    unsigned int mortise_port;
};

struct tenon
{
    struct mortise_conn *mortise_conns[];
    struct tenon_config *config;
    int mortise_conn_count;
};

extern struct tenon G_tenon;

int main(int argc, char *argv[]);


#endif //LEARNCPP_CLIENT_LIPO_H
