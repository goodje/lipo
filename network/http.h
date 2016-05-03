//
// Created by Liuwenjie on 5/1/16.
//

#ifndef LEARNCPP_HTTP_H
#define LEARNCPP_HTTP_H


struct http
{
    char *host[256];
    char fd;
    char ssl;
    int (*http_request)(char *url[256]);
};

int http_init(struct http *client);
int http_request(char *url[256]);

#endif //LEARNCPP_HTTP_H
