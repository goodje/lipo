//
// Created by liuwenjie on 8/28/16.
//

#ifndef PROJECT_TENON_EVENT_H
#define PROJECT_TENON_EVENT_H

struct event_handle_entity
{
    int fd;
    int events;
    void (*event_handle)(struct lipo_event *events);
    struct event_handle_entity *next;
};


struct event_manage
{
    struct event_handle_entity *handles;

    void (*init)(struct event_manage **event_manage);
    void (*free)(struct event_manage *event_manage);
    void (*register_handle)(struct event_manage *event_manage, int fd, int events, void *handle);
    void (*unregister_handle)(struct event_manage *event_manage, int fd, int events, void *handle);
    void (*event_loop)(struct event_manage *event_manage);
};

void event_manage_init(struct event_manage **event_manage);

extern struct tenon *Tenon;

#endif //PROJECT_TENON_EVENT_H
