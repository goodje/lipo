//
// Created by Liuwenjie on 5/1/16.
//

#ifndef LEARNCPP_HTTP_H
#define LEARNCPP_HTTP_H


struct http_client
{
    char *host[256];
    char fd;
    char ssl;
    int (*http_connect)(char *url);
    int (*http_request)(char *url);
    int (*http_close)();
};

int http_init(struct http_client *client);
int http_connect(char *url);
int http_request(char *url);
int http_close();

#endif //LEARNCPP_HTTP_H
