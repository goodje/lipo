//
// Created by Liuwenjie on 5/1/16.
//

#ifndef LEARNCPP_EVENT_H
#define LEARNCPP_EVENT_H

//event engine type
#define EVENT_EPOLL 1
#define EVENT_KQUEUE 2
#define EVENT_IOCP 3

//event types
#define EVENT_READ 4
#define EVENT_WRITE 2
#define EVENT_ERROR 1

//event operate method
#define EVENT_ADD 1
#define EVENT_MODIFY 2
#define EVENT_DEL 3

struct _event_data
{
    int fd;
};

struct event
{
    int events;
    struct _event_data *data;
};

struct event_engine
{
    int engine_type;
    int fd; //todo maybe not need
    int (*init)(struct event_engine *engine, int size);
    int (*free)(struct event_engine *engine);
    int (*add_event)(struct event_engine *engine, int fd, int events);
    int (*modify_event)(struct event_engine *engine, int fd, int events);
    int (*del_event)(struct event_engine *engine, int fd);
    int (*wait)(struct event_engine *engine, struct event *event[], int max_events, int timeout);
};

int event_engine_init(struct event_engine *engine, int size);
int event_engine_free(struct event_engine *engine);
int event_add_event(struct event_engine *engine, int fd, int events);
int event_modify_event(struct event_engine *engine, int fd, int events);
int event_del_event(struct event_engine *engine, int fd);
int event_wait(struct event_engine *engine, struct event **event, int max_events, int timeout);

#endif //LEARNCPP_EVENT_H
