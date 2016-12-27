//
// Created by liuwenjie on 11/20/16.
//


#include <string.h>
#include "utils.h"

long unsigned get_thread_id(void)
{

#ifdef __linux__
    syscall(__NR_gettid);

#elif __APPLE__

    long unsigned long pid;
    pthread_threadid_np(NULL, &pid);
    return pid;
#else
    //todo
    return 0;
#endif
}



int trim(char *str, char *res_str)
{
    size_t str_len = strlen(str), res_str_len = 0;
    size_t start, end = str_len - 1;

    memset(res_str, 0x0, strlen(res_str));

    if (str == NULL)
    {
        res_str = NULL;

        return 0;
    }

    for (start = 0; start < str_len, (int)str[start] < 32; start++); // 32 same to !, the least ascii code

    if (start == str_len)
    {
        res_str = "";
        return 0;
    }

    for (; (int)str[end] < 32; end--);


    for (start; start <= end; start++)
    {
        res_str[res_str_len++] = str[start];
    }


    return 0;
}