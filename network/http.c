//
// Created by Liuwenjie on 5/1/16.
//

#include "http.h"


int http_init(struct http *client)
{
    client->http_request = &http_request;
    return 0;
}

int http_request(char *url[256])
{

    return 0;
}