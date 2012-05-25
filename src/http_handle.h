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

enum http_stage
{
    HTTP_COMMAND,
    HTTP_URL,
    HTTP_REQUEST,
    HTTP_HEAD_KEY,
    HTTP_HEAD_VAL,
    HTTP_DATA
};

struct http_arg
{
    struct buffer *recvbuf;
    struct buffer *sendbuf;
    enum http_stage stage;
};

void http_arg_freer(void *arg);

void on_http_read(struct handle *handle);

#endif

