//
// Created by liuwenjie on 12/18/16.
//

#ifndef PROJECT_MORTISE_TENON_H
#define PROJECT_MORTISE_TENON_H

#include "../logging/logging.h"
#include "../mortise.h"

//extern struct mortise_app Mortise;

struct mortise_tenon
{
    int sock;
    struct mortise_tenon *prev;
    struct mortise_tenon *next;
};

int mortise_tenon_init(int sock, struct sockaddr_in* tenon_addr);

int mortise_tenon_close(struct mortise_tenon *conn);

#endif //PROJECT_MORTISE_TENON_H
