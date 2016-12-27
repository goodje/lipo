//
// Created by liuwenjie on 12/18/16.
//

#include "mortise_tenon.h"

int mortise_tenon_init(int sock, struct sockaddr_in* tenon_addr)
{
    if (sock < 0)
    {
        log_error("%s", "accept client error.");
        return -1;
    }
//    write(sock, "HTTP/1.1 OK 200\r\n\r\n", 19);
//    while (close(sock) != 0) {}
//    return -1;

    Mortise.mortise_tenon.conn_count++;

    if (Mortise.mortise_tenon.conn_count >= MAX_TENON)
    {
        while (close(sock) != 0) {}
        log_warn("tenon count max then allowed. fd[%d] conn_count[%d] limit[%d]", sock, Mortise.mortise_tenon.conn_count, MAX_TENON);
        return -1;
    }

    if (sock >= MAX_TENON)//todo 这种限制也许不合理
    {
        while (close(sock) != 0) {}
        log_warn("tenon fd max then allow numer. fd[%d] limit[%d]", sock, MAX_TENON);
        exit(-1);
    }

    socket_set_nonblocking(sock);

    char *client_ip = inet_ntoa(tenon_addr->sin_addr);
    log_trace("accept tenon. tenon_fd[%d] tenon_ip[%s]", sock, client_ip);

    struct mortise_tenon *conn = malloc(sizeof(struct mortise_tenon));

    if (Mortise.mortise_tenon.conns)
    {
        Mortise.mortise_tenon.conns->prev = conn;
    }

    conn->sock = sock;
    conn->next = Mortise.mortise_tenon.conns;
    conn->prev = NULL;

    Mortise.mortise_tenon.conns = conn;

    Mortise.mortise_tenon.conns_index[conn->sock] = conn;

    return 0;
}

int mortise_tenon_close(struct mortise_tenon *conn)
{
    if (conn->sock < 0)
    {
        return -1;
    }

    while (close(conn->sock) != 0) {}
    Mortise.mortise_tenon.conn_count--;

    if (conn->prev)
    {
        conn->prev->next = conn->next;
        conn->next->prev = conn->prev;
    }
    else
    {
        Mortise.mortise_tenon.conns = conn->next;
        if (conn->next)
            conn->next->prev = NULL;
    }

    Mortise.mortise_tenon.conns_index[conn->sock] = NULL;
    free(conn);

    return 0;
}