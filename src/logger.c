/*
 * logger.c
 *
 * Copyright xubin
 *
 * Author : xubin <nybux.tsui@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2.1 of the GNU Lesser General Public License
 * as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it would be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 */

#include "logger.h"
#include <stdio.h>
#include <stdarg.h>

void log_func(enum logger_level level,
              const char *filename,
              const int line,
              const char *fmt, ...)
{
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

