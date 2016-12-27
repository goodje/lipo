//
// Created by liuwenjie on 9/16/16.
//

#ifndef PROJECT_LOGGING_H
#define PROJECT_LOGGING_H

#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <stdio.h>
#include <unistd.h>

#include "../utils.h"

//log level
#define LOG_INFO 1
#define LOG_DEBUG 2
#define LOG_TRACE 3
#define LOG_WARN 4
#define LOG_ERROR 5

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

#define LOG_STR_LENGTH 1024

#ifndef LOG_LEVEL
#define LOG_LEVEL LOG_INFO
#endif

void logging_init();

void logging_free();

void inline log_process(__uint8_t level, char *str);

char log_str[LOG_STR_LENGTH];

#define log_info(str, ...) \
snprintf(log_str, LOG_STR_LENGTH, str " %s():%d ", __VA_ARGS__, __func__, __LINE__);\
log_process(LOG_INFO, log_str);

#define log_debug(str, ...) \
snprintf(log_str, LOG_STR_LENGTH, str " %s():%d ", __VA_ARGS__, __func__, __LINE__);\
log_process(LOG_DEBUG, log_str);

#define log_trace(str, ...) \
snprintf(log_str, LOG_STR_LENGTH, str " %s():%d ", __VA_ARGS__, __func__, __LINE__);\
log_process(LOG_TRACE, log_str);

#define log_warn(str, ...) \
snprintf(log_str, LOG_STR_LENGTH, str " error[%d:%s] %s():%d", __VA_ARGS__, errno, strerror(errno), __func__, __LINE__);\
log_process(LOG_WARN, log_str);

#define log_error(str, ...) \
snprintf(log_str, LOG_STR_LENGTH, str " error[%d:%s] %s():%d", __VA_ARGS__, errno, strerror(errno), __func__, __LINE__);\
log_process(LOG_ERROR, log_str);

#endif //PROJECT_LOGGING_H
