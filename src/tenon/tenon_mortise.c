//
// Created by liuwenjie on 8/28/16.
//

#include "tenon_mortise.h"

/* create a TCP socket with non blocking options and connect it to the target
* if succeed, add the socket in the epoll list and exit with 0
*/
int _create_connection(struct tenon_mortise_conn *conn)
{
    int yes = 1;
    int sock;

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("socket");
        exit(1);
    }

    //set socket to non blocking and allow port reuse
    if ((setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) || (socket_set_nonblocking(sock) == -1))
    {
        perror("setsockopt || fcntl");
        exit(1);
    }

    conn->socket_fd = sock;
    conn->status = TENON_MORTISE_CONN_DEFAULT;

    return 0;
}

void _connect_mortise(struct sockaddr_in target, struct tenon_mortise_conn *conn)
{

    if (connect(conn->socket_fd, (struct sockaddr *) &target, sizeof(struct sockaddr)) == -1
        && errno != EINPROGRESS)
    {
        // connect doesn't work, are we running out of available ports ? if yes, destruct the socket
        if (errno)
        {
            close(conn->socket_fd);

            log_error("%s", "connect is EAGAIN");
            exit(1);
        }
    }
    else
    {
        conn->status = TENON_MORTISE_CONN_PROCESSING;
        conn->mortise_port = target.sin_port;
        conn->mortise_ip = target.sin_addr.s_addr;

        Tenon->event_engine->add_event(Tenon->event_engine, conn->socket_fd, EVENT_READ | EVENT_WRITE);
    }
}


/* reading waiting errors on the socket
 * return 0 if there's no, 1 otherwise
 */
int socket_check(int fd, int *code)
{
    *code = 0;
    socklen_t len = sizeof(int);

    if (getsockopt(fd, SOL_SOCKET, SO_ERROR, code, &len) != 0)
    {
        log_error("getsockopt failed. fd[%d]", fd);
        return -1;
    }

    return 0;
}

int get_conn(int fd, struct tenon_mortise_conn **conn)
{
    int i;
    for (i = 0; i < Tenon->config.mortise_conn_count; i++)
    {
        if (Tenon->mortise_conns[i].socket_fd == fd)
        {
            *conn = &(Tenon->mortise_conns[i]);
            return 0;
        }
    }

    return -1;
}

void tenon_mortise_conn_handle(struct lipo_event *events)
{
    ssize_t process_len;
    char msg[128];
    size_t msg_len;
    char buffer[1500];
    size_t buffer_len = sizeof(buffer);
    int code;


    struct tenon_mortise_conn *conn;
    if (get_conn(events->data.fd, &conn) == -1)
    {
        printf("connection not exists. fd[%d]", events->data.fd);
        exit(-1);
    }

    if (events->events & EVENT_WRITE)
    {
        // verify the socket is connected and doesn't return an error
        if (socket_check(events->data.fd, &code) != 0)
        {
            if (errno != EINPROGRESS)
            {
                log_error("get failed by socket_check.fd[%d]", events->data.fd);
                exit(-1);
            }
        }
        else if (code != 0)
        {
            //if "61:Connection refused", maybe can have a reconnect
            log_error("socket error detected when can write. fd[%d]", events->data.fd);
            exit(-1);
        }
        else
        {

            if (conn->status != TENON_MORTISE_CONN_CONNECTED)
            {
                conn->status = TENON_MORTISE_CONN_CONNECTED;
                log_trace("mortise connected. sock[%d]", events->data.fd);

                sprintf(msg, "hello. svr_fd:%d", conn->socket_fd);
                msg_len = strlen(msg);

                if ((process_len = send(events->data.fd, msg, msg_len, 0)) < 0)
                {
                    perror("send failed");
                    exit(-1);
                }
                log_trace("sent mortise a msg. sock[%d] msg[%s] msg_len[%d] sent_len[%d]", events->data.fd, msg, msg_len, process_len);


            }
        }
    }


    //socket is ready for reading
    if (events->events & EVENT_READ)
    {
        // verify the socket is connected and doesn't return an error
        if (socket_check(events->data.fd, &code) != 0)
        {
            if (errno != EINPROGRESS)
            {
                log_error("get failed by socket_check. fd[%d]", events->data.fd);
                exit(-1);
            }
        }
        else if (code != 0)
        {
            //if "61:Connection refused", maybe can have a reconnect
            log_error("socket error detected when can read. fd[%d]", events->data.fd);
            exit(-1);
        }
        else
        {
            memset(buffer, 0x0, sizeof(buffer));

            if ((process_len = recv(events->data.fd, buffer, buffer_len, 0)) < 0)
            {
                if (errno == EAGAIN)
                {
                    log_debug("recv not ready, try again. fd[%d] msg[%d:%s]", events->data.fd, errno, strerror(errno));
                }
                else
                {
                    log_error("recv failed. fd[%d]", events->data.fd);
                }
            }
            else if (process_len >= 0)
            {
                log_trace("recv data. fd[%d] length[%zd] data[%s]", events->data.fd, process_len, buffer);
            }
        }
    }

}

void tenon_mortise_init()
{
    int i;

    struct tenon_mortise_conn *mortise_conns = Tenon->mortise_conns = malloc(sizeof(struct tenon_mortise_conn) * Tenon->config.mortise_conn_count);
    //Tenon->mortise_conns;

    // create sock
    for (i = 0; i < Tenon->config.mortise_conn_count; i++)
    {
        if (_create_connection(&mortise_conns[i]) != 0)
        {
            perror("create and connect");
            exit(1);
        }
        //else
        //    stats.nbsock++;
        Tenon->mortise_conn_count++;

        Tenon->event_manage->register_handle(
                Tenon->event_manage,
                mortise_conns[i].socket_fd,
                EVENT_EOF | EVENT_WRITE | EVENT_READ,
                &tenon_mortise_conn_handle);

        log_trace("created socket for mortise connection. fd[%d]", mortise_conns[i].socket_fd);
    }
}

void tenon_mortise_free()
{
    free(Tenon->mortise_conns);
    Tenon->mortise_conns = NULL;
    Tenon->mortise_conn_count = 0;
}

void tenon_mortise_connect()
{
    int i;
    struct sockaddr_in target; // convert target information

    struct sockaddr_in sin;
    socklen_t sinlen;

    bzero(&target,sizeof(target));
    target.sin_family = AF_INET;

    target.sin_port = Tenon->config.mortise_port;
    target.sin_addr.s_addr = Tenon->config.mortise_ip;


    for (i = 0; i < Tenon->config.mortise_conn_count; i++)
    {
        //printf("%d\n", (&Tenon->mortise_conns[i])->socket_fd);
        _connect_mortise(target, &Tenon->mortise_conns[i]);

        if (getsockname(Tenon->mortise_conns[i].socket_fd, (struct sockaddr*)&sin, &sinlen) == 0)
        {
            Tenon->mortise_conns[i].local_port = sin.sin_port;
        }

        log_trace("connecting to mortise. fd[%d] server[%s:%d] local_port[%d]", Tenon->mortise_conns[i].socket_fd,
                inet_ntoa(target.sin_addr), ntohs(target.sin_port), ntohs(Tenon->mortise_conns[i].local_port));
    }
}