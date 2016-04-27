//
// Created by Liuwenjie on 4/25/16.
//

#include "lib_socket.h"


int socket_set_nonblocking(int fd)
{
    int flags = fcntl(fd, F_GETFL, 0);

    if (flags == -1)
    {
        return -1;
    }

    flags |= O_NONBLOCK;

    if (fcntl(fd, F_SETFL, flags) == -1)
    {
        return -1;
    }
    return 0;
}


/* WT (inject29) function to convert a ip:port chain into a sockaddr struct */
struct sockaddr_in str2sa(char *str)
{
    static struct sockaddr_in sa;
    char *c;
    int port;

    bzero(&sa, sizeof(sa));
    str = strdup(str);
    if ((c = strrchr(str, ':')) != NULL)
    {
        *c++ = 0;
        port = atol(c);
    }
    else
        port = 0;

    if (!inet_aton(str, &sa.sin_addr))
    {
        struct hostent *he;

        if ((he = gethostbyname(str)) == NULL)
            fprintf(stderr, "[NetTools] Invalid server name: %s\n", str);
        else
            sa.sin_addr = *(struct in_addr *) *(he->h_addr_list);
    }
    sa.sin_port = htons(port);
    sa.sin_family = AF_INET;

    free(str);
    return sa;
}