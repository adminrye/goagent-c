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

#define HTTP_GET "GET"
#define HTTP_POST "POST"
#define HTTP_CONNECT "CONNECT"

static void process_request(struct handle *handle) {
    char url[4096];
    enum buffer_result rv;
    size_t len;
    struct http_arg *arg;

    arg = (struct http_arg *)handle->arg;
    len = sizeof url;
    rv = buffer_read_until(arg->recvbuf, " ", url, &len);
    if (rv == BUFFER_TOOSMALL) {
        LOG(WARN, "request url too long");
        handle_destroy(handle);
        return;
    }
    if (rv == BUFFER_NOTFOUND) {
        return;
    }
    url[len] = 0;
    LOG(INFO, url);
    return;
}

static void on_http_get(struct handle *handle) {
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

static void on_http_post(struct handle *handle) {
}

static void on_http_connect(struct handle *handle) {
}

void http_arg_deleter(void *_arg) { struct http_arg *arg;

    if (_arg == NULL) {
        return;
    }
    arg = (struct http_arg *)_arg;
    buffer_destroy(arg->recvbuf);
    buffer_destroy(arg->sendbuf);
    free(arg);
}

void on_http_read(struct handle *handle) {
    char command[16];
    size_t len;
    enum buffer_result rv;
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
    if (rv == BUFFER_TOOSMALL) {
        LOG(WARN, "request is not http");
        handle_destroy(handle);
        return;
    }

    LOG(INFO, "%.*s", len, command);
    if (strncmp(command, HTTP_GET" ", sizeof HTTP_GET" " - 1) == 0) {
        handle->readcb = on_http_get;
        process_request(handle);
    } else if (strncmp(command, HTTP_POST" ", sizeof HTTP_POST" " - 1) == 0) {
        handle->readcb = on_http_post;
        on_http_post(handle);
    } else if (strncmp(command, HTTP_CONNECT" ", sizeof HTTP_CONNECT" " - 1) == 0) {
        handle->readcb = on_http_connect;
        on_http_connect(handle);
    }
}

