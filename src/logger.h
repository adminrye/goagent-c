/*
 * logger.h
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

#ifndef _LOGGER_H_
#define _LOGGER_H_

enum logger_level {
    INFO,
    WARN,
    ERR
};

#define LOG(e, fmt...) log_func(e, __FILE__, __LINE__, fmt)

void log_func(enum logger_level level,
              const char *filename,
              const int line,
              const char *fmt, ...);

#endif

