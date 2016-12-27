//
// Created by Liuwenjie on 4/24/16.
//

#include "tenon.h"

void interrupt(int signal_id)
{
    Tenon->running = 0;
}

// init struct for statistics
struct statistics
{
    int reqsent;
    int bytessent;
    int reprecv;
    int bytesrecv;
    int error;
    int nbsock;
};

void printstats(struct statistics *stats)
{
    struct statistics previous;
    previous.reqsent = 0;
    previous.reprecv = 0;
    int banner = 0;

    printf("\nreprecv\tbytes\t^hit\treqsent\tbytes\t^req\tErrors\tActive\n");

    for (; ;)
    {
        sleep(1);
        if (banner == 10)
        {
            printf("\nreprecv\tbytes\t^hit\treqsent\tbytes\t^req\tErrors\tActive\n");
            banner = 0;
        }
        else
            banner++;


        printf("%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\n", stats->reprecv, stats->bytesrecv, stats->reprecv - previous.reprecv,
               stats->reqsent, stats->bytessent, stats->reqsent - previous.reqsent, stats->error, stats->nbsock);

        previous.reqsent = stats->reqsent;
        previous.reprecv = stats->reprecv;
    }
}


int signal_init()
{
    // catch SIGINT to exit in a clean way
    struct sigaction sa;
    memset(&sa, 0, sizeof(struct sigaction));
    sa.sa_handler = interrupt;
    sa.sa_flags = 0;
    sigemptyset (&(sa.sa_mask));
    if (sigaction(SIGINT, &sa, NULL) != 0)
    {
        perror("sigaction failed");
        exit(1);
    }
    return 0;
}



int main(int argc, char *argv[])
{

    if (argc != 4)
    {
        printf("tenon <ip> <port> <num socket>\n");
        exit(1);
    }

    signal_init();

    Tenon = malloc(sizeof(struct tenon));

    logging_init();
    log_trace("%s", "starting...");

    Tenon->config.mortise_ip = inet_addr(argv[1]);
    Tenon->config.mortise_port = htons(atoi(argv[2]));
    Tenon->config.mortise_conn_count = atoi(argv[3]);

    Tenon->mortise_conn_count = 0;

    //global events engine
    event_engine_init(&(Tenon->event_engine), Tenon->config.mortise_conn_count);
    event_manage_init(&(Tenon->event_manage));

    //client
    tenon_mortise_init();

    Tenon->running = 1;

    tenon_mortise_connect();

    Tenon->event_manage->event_loop(Tenon->event_manage);


    //free Tenon
    event_engine_free(Tenon->event_engine);
    //todo event_manage_free
    tenon_mortise_free();
    //todo free clients

    logging_free();

    printf("end\n");


//    while (!stop)
//    {
//
//        for (i = 0; i < count; i++)
//        {
//            if (evs[i].events & EVENT_EOF)
//            {
//                perror("eof");
//                printf("eof sock[%d]\n", evs[i].data.fd);
//                Tenon->event_engine->del_event(Tenon->event_engine, evs[i].data.fd);
//                close(evs[i].data.fd);
//                continue;
//            }
//
//            if (evs[i].events & EVENT_ERROR)
//            {
//                printf("error\n");
//                Tenon->event_engine->del_event(Tenon->event_engine, evs[i].data.fd);
//                close(evs[i].data.fd);
//                continue;
//            }
//
//
//
//
//
//            /*
//            if (evs[i].events & (EPOLLRDHUP | EPOLLHUP)) //socket closed, delete and create a new one
//            {
//                // socket is closed, remove the socket from epoll and create a new one
//                epoll_ctl(epfd, EPOLL_CTL_DEL, events[i].data.fd, NULL);
//
//                if (close(events[i].data.fd) != 0)
//                {
//                    perror("close");
//                    continue;
//                }
//                else
//                    stats.nbsock--;
//
//                struct tenon_mortise_conn *conn;
//                get_conn(events[i].data.fd, conn);
//
//                if (_create_connection(target, conn) != 0)
//                {
//                    perror("create and connect");
//                    continue;
//                }
//                else
//                    stats.nbsock++;
//            }
//            if (events[i].events & EPOLLERR)
//            {
//                perror("epoll");
//                continue;
//            }*/
//        }
//    }




//    struct statistics stats;
//    memset(&stats, 0x0, sizeof(struct statistics));
//    pthread_t Statsthread;
//
//    // time
//    struct timeval start;
//    struct timeval current;
//    float elapsedtime;
//    // start the thread that prints the statistics
//    if (0 != pthread_create(&Statsthread, NULL, (void *) printstats, &stats))
//    {
//        perror("stats thread");
//        exit(1);
//    }
//
//    gettimeofday(&start, NULL);

//    gettimeofday(&current, NULL);
//
//    elapsedtime = (current.tv_sec * 1000000 + current.tv_usec) - (start.tv_sec * 1000000 + start.tv_usec);
//
//    printf("\n\nTime: %4.6f\nRequests sent: %d\nBytes sent: %d\nReponses received: %d\nBytes received: %d\nRates out: %4.6freq/s, %4.6fbytes/s\nRates in : %4.6frep/s, %4.6fbytes/s\nErrors: %d\n",
//           elapsedtime / 1000000, stats.reqsent, stats.bytessent, stats.reprecv, stats.bytesrecv,
//           stats.reqsent / (elapsedtime / 1000000), stats.bytessent / (elapsedtime / 1000000),
//           stats.reprecv / (elapsedtime / 1000000), stats.bytesrecv / (elapsedtime / 1000000), stats.error);

    return 0;
}