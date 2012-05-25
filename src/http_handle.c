/*
 * http_handle.c
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

#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include "handle.h"
#include "logger.h"
#include "buffer.h"
#include "http_handle.h"

static void process_request(struct handle *handle)
{
    char url[4096];
    int rv;
    size_t len;
    struct http_arg *arg;

    arg = (struct http_arg *)handle->arg;
    len = sizeof url;
    rv = buffer_read_until(arg->recvbuf, " ", url, &len);
    if (rv == -1 && len == sizeof url) {
        LOG(WARN, "request url too long");
        handle_destroy(handle);
        return;
    } else if (rv == -1 && len == 0) {
        return;
    }
    url[len] = 0;
    LOG(INFO, url);
}

static void on_http_get(struct handle *handle)
{
    int rv;
    struct http_arg *arg;

    arg = (struct http_arg *)handle->arg;
    rv = buffer_recv(arg->recvbuf, handle->fd);
    if (rv == -1) {
        LOG(ERR, "recv() failed:%s", strerror(errno));
        handle_destroy(handle);
        return;
    }
    if (rv == 0) {
        LOG(INFO, "client closed");
        handle_destroy(handle);
        return;
    }
    process_request(handle);
}

static void on_http_post(struct handle *handle)
{
}

static void on_http_connect(struct handle *handle)
{
}

void http_arg_freer(void *_arg) {
    struct http_arg *arg;

    if (_arg == NULL) {
        return;
    }
    arg = (struct http_arg *)_arg;
    buffer_destroy(arg->recvbuf);
    buffer_destroy(arg->sendbuf);
    free(arg);
}

void on_http_read(struct handle *handle)
{
    char command[16];
    size_t len;
    int rv;
    struct http_arg *arg;

    arg = (struct http_arg *)handle->arg;
    if (arg->recvbuf == NULL) {
        arg->recvbuf = buffer_create();
    }
    rv = buffer_recv(arg->recvbuf, handle->fd);
    if (rv == -1) {
        LOG(ERR, "recv() failed:%s", strerror(errno));
        handle_destroy(handle);
        return;
    }
    if (rv == 0) {
        LOG(INFO, "client closed");
        handle_destroy(handle);
        return;
    }

    len = sizeof command;
    rv = buffer_read_until(arg->recvbuf, " ", command, &len);
    if (rv == -1 || len == sizeof command) {
        LOG(WARN, "request is not http");
        handle_destroy(handle);
        return;
    }
    command[len] = 0;
    if (strcmp(command, "GET ") == 0) {
        LOG(INFO, "GET");
        handle->readcb = on_http_get;
        process_request(handle);
    } else if (strcmp(command, "POST ") == 0) {
        LOG(INFO, "POST");
        handle->readcb = on_http_post;
        on_http_post(handle);
    } else if (strcmp(command, "CONNECT ") == 0) {
        LOG(INFO, "CONNECT");
        handle->readcb = on_http_connect;
        on_http_connect(handle);
    }
}

