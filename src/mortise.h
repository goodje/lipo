//
// Created by Liuwenjie on 4/24/16.
//

#ifndef LEARNCPP_MORTISE_H
#define LEARNCPP_MORTISE_H

#include <printf.h>
#include <strings.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <errno.h>
#include <signal.h>
#include <sys/epoll.h>

#include "mortise/mortise_tenon.h"
#include "network/lib_socket.h"
#include "network/http.h"
#include "logging/logging.h"
#include "utils.h"

//max tenon
#define MAX_TENON 10000

struct mortise_tenon_conns
{
    int conn_count;
    struct mortise_tenon *conns;
    struct mortise_tenon* conns_index[MAX_TENON]; //todo {MAX_TENON} not eq max tenon
};

struct mortise_app
{
    int sock;
    int running;
    u_short connect_timeout;
    struct mortise_tenon_conns mortise_tenon;
} Mortise;



void main();



#endif //LEARNCPP_MORTISE_H
