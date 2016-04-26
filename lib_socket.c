//
// Created by Liuwenjie on 4/25/16.
//

#include <fcntl.h>
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
