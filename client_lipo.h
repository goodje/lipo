//
// Created by Liuwenjie on 4/24/16.
//

#ifndef LEARNCPP_CLIENT_LIPO_H
#define LEARNCPP_CLIENT_LIPO_H

#include <sys/socket.h>
#include <sys/epoll.h>
#include <arpa/inet.h>
#include <errno.h>
#include "lib_socket.h"

#define LIPOD_CONN_DEFAULT 0
#define LIPOD_CONN_PROCESSING 1
#define LIPOD_CONN_CONNECTED 2

struct lipod_connection
{
    int socket_fd;
    char *lipod_ip[15]; //todo int is well
    unsigned int lipod_port;
    unsigned int local_port;
    int status;
};


void main();




#endif //LEARNCPP_CLIENT_LIPO_H
