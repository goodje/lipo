//
// Created by liuwenjie on 8/28/16.
//

#ifndef PROJECT_TENON_MORTISE_H
#define PROJECT_TENON_MORTISE_H

#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>
#include "../network/lib_socket.h"
#include "../event/lipo_event.h"
#include "../tenon.h"

#define TENON_MORTISE_CONN_DEFAULT 0
#define TENON_MORTISE_CONN_PROCESSING 1
#define TENON_MORTISE_CONN_CONNECTED 2

//events type define
#define EVENT_TYPE_TENON_MORTISE_CONN 1


struct tenon_mortise_conn
{
    int socket_fd;
    uint32_t mortise_ip;
    in_port_t mortise_port;
    in_port_t local_port;
    int status;
};

extern struct tenon *Tenon;

void tenon_mortise_init();
void tenon_mortise_free();
void tenon_mortise_connect();


#endif //PROJECT_TENON_MORTISE_H
