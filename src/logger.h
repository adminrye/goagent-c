#ifndef _LOGGER_H_
#define _LOGGER_H_

typedef enum {
    INFO,
    WARN,
    ERR
} LoggerLevel;

#define LOG(e, fmt...) LogFunc(e, __FILE__, __LINE__, fmt)

void LogFunc(LoggerLevel level, const char *filename, const int line, const char *fmt, ...);

#endif

