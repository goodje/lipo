//
// Created by liuwenjie on 9/16/16.
//

#include "logging.h"

/// get current time
/// \param datetime output datetime
/// \return
const char* current_datetime()
{
    struct timeval tv;
    struct tm* tm_info;
    static char datetime[27];
    memset(datetime, 0x0, 27);

    if (gettimeofday(&tv, NULL) == -1)
    {
        perror("get time error.");
        return NULL;
    }

    tm_info = localtime(&tv.tv_sec);
    strftime(datetime, 20, "%Y-%m-%d %H:%M:%S", tm_info);//len for 19 second not show.
    sprintf(datetime, "%s.%d", datetime, tv.tv_usec);
    return datetime;
}

//process log
void inline log_process(__uint8_t level, char *str)
{
    char *level_str;

    switch (level)
    {
        case LOG_DEBUG:
            level_str = "DEBUG";
            break;
        case LOG_TRACE:
            level_str = "TRACE";
            break;
        case LOG_WARN:
            level_str = "WARN";
            printf(ANSI_COLOR_YELLOW);
            break;
        case LOG_ERROR:
            level_str = "ERROR";
            printf(ANSI_COLOR_RED);
            break;
        default:
            level_str = "INFO";
            break;
    }
    // __FILE__, __LINE__, __func__

    if (level >= LOG_LEVEL)
        printf("%s %s %s %d#%lu\n" ANSI_COLOR_RESET, current_datetime(), level_str, str, getpid(), get_thread_id());
}

void logging_init()
{
}

void logging_free()
{
}