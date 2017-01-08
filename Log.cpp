#include <cstdio>
#include <cstdarg>
#include <cstdlib>

#include <time.h>
#include <sys/time.h>
#include <pthread.h>

#include "Log.h" 

namespace Adoter {

static LogLevel s_log_level = LOG_LEVEL_ERROR;

LogLevel GetLevel()
{
    return s_log_level;
}

void SetLevel(LogLevel level)
{
    s_log_level = level;
}

void DefaultLogHandler(LogLevel level, const char* filename, int line, const char *fmt, va_list ap)
{
    static const char* level_names[] = { "FATAL", "ERROR", "WARNNING", "INFO", "TRACE", "DEBUG" };
    char buf[1024];
    vsnprintf(buf, 1024, fmt, ap);
    struct timeval now_tv;
    gettimeofday(&now_tv, NULL);
    const time_t seconds = now_tv.tv_sec;
    struct tm t;
    localtime_r(&seconds, &t);
    fprintf(stderr, "[%s %04d/%02d/%02d-%02d:%02d:%02d.%06d %llx %s:%d] %s\n",
            level_names[level],
            t.tm_year + 1900,
            t.tm_mon + 1,
            t.tm_mday,
            t.tm_hour,
            t.tm_min,
            t.tm_sec,
            static_cast<int>(now_tv.tv_usec),
            static_cast<long long unsigned int>(pthread_self()),
            filename, line, buf);
    fflush(stderr);

    if (level == LOG_LEVEL_FATAL)
    {
        abort();
    }
}

void null_log_handler(LogLevel, const char*, int, const char *, va_list)
{
    // Nothing
}

static LogHandler* s_log_handler = DefaultLogHandler;

void log_handler(LogLevel level, const char* filename, int line, const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    s_log_handler(level, filename, line, fmt, ap);
    va_end(ap);
}

LogHandler* SetLogHandler(LogHandler* new_func) {
    LogHandler* old = s_log_handler;
    if (old == &null_log_handler) {
        old = NULL;
    }
    if (new_func == NULL) {
        s_log_handler = &null_log_handler;
    } else {
        s_log_handler = new_func;
    }
    return old;
}

} 
