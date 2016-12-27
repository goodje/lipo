//
// Created by Liuwenjie on 5/1/16.
//

#ifdef __APPLE__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/event.h>
#include "lipo_event.h"
#include "../logging/logging.h"

int event_engine_init(struct event_engine **engine, int size)
{
    *engine = malloc(sizeof(struct event_engine));

    (*engine)->fd = kqueue();

    log_trace("init events engine. type[kqueue], fd[%d]", (*engine)->fd);

    (*engine)->engine_type = EVENT_KQUEUE;
    (*engine)->init = &event_engine_init;//todo needed?
//  (*  engi)ne->free = &event_engine_free;
    (*engine)->add_event = &event_add_event;
    (*engine)->mod_event = &event_mod_event;
    (*engine)->del_event = &event_del_event;
    (*engine)->wait = &event_wait;

    return 0;
}

int event_engine_free(struct event_engine *engine)
{
    //todo
    printf("hello, free!\n");
    return 0;
}

int event_add_event(struct event_engine *engine, int fd, int events)
{
    struct kevent changes[1];

    if (events & EVENT_READ)
    {
//        EV_SET(&changes[0], fd, EVFILT_READ, EV_ADD, 0, 0, NULL);
//        kevent(engine->fd, changes, 1, NULL, 0, NULL);
    }

    if (events & EVENT_WRITE)
    {
        EV_SET(&changes[0], fd, EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, NULL);
        kevent(engine->fd, changes, 1, NULL, 0, NULL);
    }

    /*
    if (events & EVENT_ERROR)
    {
        EV_SET(&changes[0], fd, , EV_ADD, 0, 0, NULL);
        kevent(engine->fd, changes, 1, NULL, 0, NULL);
    }*/

    //perror("epoll ctl add error.");
    //exit(-1);

    return 0;
}

int event_mod_event(struct event_engine *engine, int fd, int events)
{
//    engine->del_event(engine, fd);
//    engine->add_event(engine, fd, events);

    struct kevent changes[1];

//    EV_SET(&changes[0], fd, EVFILT_READ, EV_DISABLE | EV_DELETE, 0, 0, NULL);
//    kevent(engine->fd, changes, 1, NULL, 0, NULL);

    EV_SET(&changes[0], fd, EVFILT_READ, EV_DISABLE | EV_DELETE, 0, 0, NULL);


    perror("1event modify error.");

    int count;
    count = kevent(engine->fd, changes, 1, NULL, 0, NULL);

    printf("kevent count %d\n", count);
    perror("events modify error.");
    perror("events modify error.");
    //exit(-1);
    return 0;
}

int event_del_event(struct event_engine *engine, int fd)
{
    struct kevent changes[1];

    EV_SET(&changes[0], fd, EVFILT_READ, EV_DISABLE | EV_DELETE, 0, 0, NULL);
    kevent(engine->fd, changes, 1, NULL, 0, NULL);

    EV_SET(&changes[0], fd, EVFILT_WRITE, EV_DISABLE | EV_DELETE, 0, 0, NULL);
    kevent(engine->fd, changes, 1, NULL, 0, NULL);

//        perror("epoll ctl del error.");
//        exit(-1);

    return 0;
}

int event_wait(struct event_engine *engine, struct lipo_event *evs, int max_events, int timeout)
{
    //optimize no need declare evList every be called. don't laugh me by my english ==.
    struct kevent evList[max_events];

    int i;
    int count = kevent(engine->fd, NULL, 0, evList, 32, NULL);

    if (count < 1)
        perror("kevent error");

    memset(evs, 0x0, sizeof(struct lipo_event) * max_events);

    for (i = 0; i < count; i++)
    {
//        printf("events count: %d index: %d events.flag: %d\n", count, i, evList[i].flags);
        //printf("%d\n", evs[i]->events);

        // = _convert_events(epoll_events[i]->events);
        evs[i].events = 0;
        //sprintf("events: %d\n", evList[i].flags);
        if (evList[i].flags & EV_ERROR)
        {
            evs[i].events |= EVENT_ERROR;
        }

        //TODO EV_EOF
        if (evList[i].flags & EV_EOF)
        {
            evs[i].events |= EVENT_EOF;
        }

        if (evList[i].flags & EVFILT_READ)
        {
            evs[i].events |= EVENT_READ;
        }

        if (evList[i].flags & EVFILT_WRITE)
        {
            evs[i].events |= EVENT_WRITE;
        }

//        printf("evList[i].flags: %d\n", evList[i].flags);
//        printf("evs[i].events: %d\n", evs[i].events);
//        exit(-1);

        evs[i].data.fd = (int)evList[i].ident;
    }

    return count;
}

#endif