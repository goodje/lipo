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
#include <fcntl.h>
#include <signal.h>
#include <sys/time.h>
#include <pthread.h>
#include "event/lipo_event.h"
#include "event/event_manage.h"
#include "network/lib_socket.h"

#include "tenon/tenon_mortise.h"
#include "logging/logging.h"


struct _tenon_config
{
    int mortise_conn_count;
    uint32_t mortise_ip;
    in_port_t mortise_port;
};

struct tenon
{
    int running;
    struct _tenon_config config;
    int mortise_conn_count;
    struct tenon_mortise_conn *mortise_conns;
    struct event_engine *event_engine;
    struct event_manage *event_manage;
    struct logging *log;
};

struct tenon *Tenon;

int main(int argc, char *argv[]);


#endif //LEARNCPP_CLIENT_LIPO_H
