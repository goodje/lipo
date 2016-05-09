//
// Created by Liuwenjie on 5/1/16.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include "event.h"

int event_engine_init(struct event_engine *engine, int size)
{
    engine->fd = epoll_create(size);
    engine->event_engine = EVENT_EPOLL;
    engine->init = &event_engine_init;
//    engine->free = &event_engine_free;
    engine->add_event = &event_add_event;
    engine->modify_event = &event_modify_event;
    engine->del_event = &event_del_event;
    engine->wait = &event_wait;

    return 0;
}

int event_engine_free(struct event_engine *engine)
{
    printf("hello, free!\n");
    //printf("engine type: %d\n", engine->event_engine);
    return 0;
}

int event_add_event(struct event_engine *engine, int fd, int events)
{
    struct epoll_event ev;
    int epoll_events = 0;
    ev.events = _convert_events(events);
    ev.data.fd = fd;

    if (epoll_ctl(engine->fd, EPOLL_CTL_ADD, fd, &ev) != 0)
    {
        perror("epoll ctl add error.");
        exit(-1);
    }
    return 0;
}

int event_modify_event(struct event_engine *engine, int fd, int events)
{
    struct epoll_event ev;
    int epoll_events = 0;
    ev.events = _convert_events(events);
    ev.data.fd = fd;

    if (epoll_ctl(engine->fd, EPOLL_CTL_MOD, fd, &ev) != 0)
    {
        perror("epoll ctl mod error.");
        exit(-1);
    }
    return 0;
}

int event_del_event(struct event_engine *engine, int fd)
{
    if (epoll_ctl(engine->fd, EPOLL_CTL_DEL, fd, NULL) != 0)
    {
        perror("epoll ctl del error.");
        exit(-1);
    }
    return 0;
}

int event_wait(struct event_engine *engine, struct event **event, int max_events, int timeout)
{
    struct epoll_event *epoll_events[max_events];

    int i;
    int count = 0;
    count = epoll_wait(engine->fd, *epoll_events, max_events, timeout);

    memset(event, 0x0, sizeof(event) * max_events);

    for (i = 0; i < max_events; i++)
    {
        (*event)[i].events = _convert_events(epoll_events[i]->events);
        (*event)[i].data->fd = epoll_events[i]->data.fd;
    }

    return count;
}

int _convert_events(int events)
{
    int epoll_events = 0;
    if (events & EVENT_READ)
    {
        epoll_events |= _read_event();
    }

    if (events & EVENT_WRITE)
    {
        epoll_events |= _write_event();
    }

    if (events & EVENT_ERROR)
    {
        epoll_events |= _err_event();
    }
    return epoll_events;
};

int _read_event()
{
    return EPOLLIN;
}

int _write_event()
{
    return EPOLLOUT;
}

int _err_event()
{
    return EPOLLRDHUP | EPOLLERR | EPOLLET;
}