#pragma once

#include <stdarg.h>

namespace Adoter {

enum LogLevel {
    LOG_LEVEL_FATAL   = 0,
    LOG_LEVEL_ERROR   = 1,
    LOG_LEVEL_WARNING = 2,
    LOG_LEVEL_NOTICE  = 3,
    LOG_LEVEL_INFO    = 3,
    LOG_LEVEL_TRACE   = 4,
    LOG_LEVEL_DEBUG   = 5,
};

typedef void LogHandler(LogLevel level, const char* filename, int line, const char *fmt, va_list ap);

LogHandler* SetLogHandler(LogHandler* new_func);

LogLevel GetLevel();
void SetLevel(LogLevel level);
void log_handler(LogLevel level, const char* filename, int line, const char *fmt, ...);

#define SET_LOG_LEVEL(level) \
    SetLevel(LOG_LEVEL_##level)

#define LOG(level, fmt, arg...) \
    (LOG_LEVEL_##level > GetLevel()) ? \
            (void)0 : log_handler( \
                    LOG_LEVEL_##level, __FILE__, __LINE__, fmt, ##arg) \

#define LOG_IF(condition, level, fmt, arg...) \
    !(condition) ? (void)0 : log_handler( \
            LOG_LEVEL_##level, __FILE__, __LINE__, fmt, ##arg)
} 
