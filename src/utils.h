//
// Created by liuwenjie on 11/20/16.
//

#ifndef PROJECT_UTILS_H
#define PROJECT_UTILS_H


#ifdef __linux__

#include <sys/syscall.h>

#elif __APPLE__

#include <pthread.h>

#endif


long unsigned get_thread_id(void);

int trim(char *str, char *res_str);

#endif //PROJECT_UTILS_H
