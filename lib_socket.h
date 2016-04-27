//
// Created by Liuwenjie on 4/25/16.
//

#ifndef LEARNCPP_LIB_SOCKET_H
#define LEARNCPP_LIB_SOCKET_H

#include <fcntl.h>
#include <strings.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <netinet/in.h>

int socket_set_nonblocking(int fd);

struct sockaddr_in str2sa(char *str);

#endif //LEARNCPP_SOCKET_H
