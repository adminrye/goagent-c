/*
 * http_handle.h
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

#ifndef _HTTP_HANDLE_H_
#define _HTTP_HANDLE_H_

#include "zlib.h"

enum http_method {
    HTTP_GET,
    HTTP_POST
};

struct http_arg {
    struct buffer *recvbuf;
    struct buffer *sendbuf;
    enum http_method method;
    int content_length;
};

void http_arg_deleter(void *arg);

void on_http_read(struct handle *handle);

#endif

