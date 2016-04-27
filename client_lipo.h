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

#define LIPOD_CONN_DEFAULT 0
#define LIPOD_CONN_PROCESSING 1
#define LIPOD_CONN_CONNECTED 2

struct lipod_conn
{
    int socket_fd;
    char *lipod_ip[15]; //todo int is well
    unsigned int lipod_port;
    unsigned int local_port;
    int status;
};

extern struct lipod_conn *G_lipod_cons[];


int main(int argc, char *argv[]);




#endif //LEARNCPP_CLIENT_LIPO_H
