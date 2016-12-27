//
// Created by Liuwenjie on 5/1/16.
//

#include "http.h"


int http_init(struct http_client *client)
{
    client->http_connect = &http_connect;
    client->http_request = &http_request;
    return 0;
}

int http_connect(char *url)
{

    return 0;
}

int http_request(char *url)
{



















    return 0;
}