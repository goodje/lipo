//
// Created by liuwenjie on 8/28/16.
//

#include <stdlib.h>
#include "event_manage.h"
#include "../tenon.h"


void event_manage_register_handle(struct event_manage *event_manage, int fd, int events, void *handle)
{
    struct event_handle_entity *handle_entity = malloc(sizeof(struct event_handle_entity));

    handle_entity->fd = fd;
    handle_entity->events = events;
    handle_entity->event_handle = handle;
    handle_entity->next = NULL;
    if (event_manage->handles == NULL)
    {
        event_manage->handles = handle_entity;
        return;
    }


    struct event_handle_entity *next_handle_entity = event_manage->handles;
    while(1)
    {
        if (next_handle_entity->next == NULL)
        {
            next_handle_entity->next = handle_entity;
            break;
        }

        next_handle_entity = next_handle_entity->next;
    }
}

void event_manage_unregister_handle(struct event_manage *event_manage, int fd, int events, void *handle)
{
    //todo
}

void event_manage_event_loop(struct event_manage *event_manage)
{
    if (event_manage->handles == NULL)
    {
        return;
    }


    int i, count = 0;

    //evs will be use for events loop
    static struct lipo_event *evs;

    // allocate enough memory to store all the events in the "events" structure
    if (NULL == (evs = calloc(1000, sizeof(struct lipo_event))))
    {
        perror("calloc events");
        exit(1);
    }

    while (Tenon->running)
    {
        /* wait for events on the file descriptors added into epfd
         *
         * if one of the socket that's contained into epfd is available for reading, writing,
         * is closed or have an error, this socket will be return in events[i].data.fd
         * and events[i].events will be set to the corresponding events
         *
         * count contain the number of returned events
         */
        //count = epoll_wait(epfd, events, Tenon->config->mortise_conn_count, 1000);
        count = Tenon->event_engine->wait(Tenon->event_engine, evs, 1000, 0);

        for (i = 0; i < count; i++)
        {
            struct event_handle_entity *handle_entity = event_manage->handles;
            while(handle_entity != NULL)
            {
                //printf("hh %d\n", evs[i].data.fd);
                if (handle_entity->fd == evs[i].data.fd && handle_entity->events & evs[i].events)
                {
                    handle_entity->event_handle(&(evs[i]));
                }

                handle_entity = handle_entity->next;
            }
        }
    }

    free(evs);
    evs = NULL;
}

void event_manage_free(struct event_manage *event_manage)
{

    //free handle linked list
    struct event_handle_entity *handle_entity = event_manage->handles, *next_handle_entity;
    while(handle_entity != NULL)
    {
        next_handle_entity = handle_entity->next;
        free(handle_entity);
        handle_entity = next_handle_entity;
    }
    event_manage->handles = NULL;

    free(event_manage);
    event_manage = NULL;
}

void event_manage_init(struct event_manage **event_manage)
{
    (*event_manage) = malloc(sizeof(struct event_manage));
    (*event_manage)->init = &event_manage_init;
    (*event_manage)->free = &event_manage_free;
    (*event_manage)->register_handle = &event_manage_register_handle;
    (*event_manage)->unregister_handle = &event_manage_unregister_handle;
    (*event_manage)->event_loop = &event_manage_event_loop;


    (*event_manage)->handles = NULL;
}




