#include "logger.h"
#include <stdio.h>
#include <stdarg.h>

void LogFunc(LoggerLevel level, const char *filename, const int line, const char *fmt, ...) {
    switch (level) {
        case ERR:
            fprintf(stderr, "[ERR] ");
            break;
        case WARN:
            fprintf(stderr, "[WARN]");
            break;
        case INFO:
            fprintf(stderr, "[INFO]");
            break;
    }
    fprintf(stderr, " %s:%d\n       ", filename, line);
    va_list va;
    va_start(va, fmt);
    vfprintf(stderr, fmt, va);
    va_end(va);
    fprintf(stderr, "\n");
}

