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
#include <stdio.h>
#include <assert.h>
#include "handle.h"
#include "logger.h"
#include "buffer.h"
#include "http_handle.h"
#include "z.h"

static void on_read_headkey1(struct handle *handle);
static void on_read_headkey2(struct handle *handle);

void http_arg_deleter(void *_arg) {
    struct http_arg *arg;

    if (_arg == NULL) {
        return;
    }
    arg = (struct http_arg *)_arg;
    buffer_destroy(arg->recvbuf);
    buffer_destroy(arg->sendbuf);
    if (arg->zstrm) {
        deflateEnd(arg->zstrm);
        free(arg->zstrm);
    }
    free(arg);
}

#define ADD_REQ_STRING(buf, str) add_req_string(buf, str, sizeof str - 1)
static void add_req_string(struct buffer *buffer, const char *str, size_t len) {
    buffer_append(buffer, str, len);
}

#define ADD_REQ_HEXSTRING(buf, str) add_req_string(buf, str, sizeof str - 1)
static void add_req_hexstring(struct buffer *buffer,
                              const char *str,
                              size_t len) {
    int i, l;
    char temp[4];

    for (i = 0; i < len; ++i) {
        l = snprintf(temp, sizeof temp, "%02x", str[i]);
        buffer_append(buffer, temp, l);
    }
}

static void on_read_payload(struct handle *handle) {
    char payload[4096];
    enum buffer_result rv;
    size_t len;
    struct http_arg *arg;

    arg = (struct http_arg *)handle->arg;
    if (arg->content_length == 0) {
        return;
    }
    if (arg->recvbuf->len == 0) {
    }
}

static void send_request(struct handle *handle) {
}

static void on_read_headval(struct handle *handle) {
    char val[4096];
    enum buffer_result rv;
    size_t len;
    struct http_arg *arg;

    arg = (struct http_arg *)handle->arg;
    len = sizeof val;
    rv = buffer_read_until(arg->recvbuf, "\r\n", val, &len);
    if (rv == BUFFER_TOOSMALL) {
        LOG(WARN, "request val too long");
        handle_destroy(handle);
        return;
    }
    if (rv == BUFFER_NOTFOUND) {
        handle->readcb = on_read_headkey2;
        return;
    }
    if (len == 0) {
        LOG(WARN, "request not http request");
        handle_destroy(handle);
        return;
    }
    if (len == 2) {
        if (arg->type == HTTP_POST) {
            on_read_payload(handle);
        } else {
            send_request(handle);
        }
        return;
    }

    if (val[0] == ' ') {
        add_req_hexstring(arg->sendbuf, val + 1, len - 3);
    } else {
        add_req_hexstring(arg->sendbuf, val, len - 2);
    }
    if (arg->content_length == -1) {
        if (val[0] == ' ') {
            arg->content_length = atoi(val + 1);
        } else {
            arg->content_length = atoi(val);
        }
    }
    on_read_headkey2(handle);
    return;
}

static void on_read_headkey(struct handle *handle, int first) {
    char key[1024];
    enum buffer_result rv;
    size_t len;
    struct http_arg *arg;

    arg = (struct http_arg *)handle->arg;
    len = sizeof key;
    rv = buffer_read_until(arg->recvbuf, ":", key, &len);
    if (rv == BUFFER_TOOSMALL) {
        LOG(WARN, "request key too long");
        handle_destroy(handle);
        return;
    }
    if (rv == BUFFER_NOTFOUND) {
        if (first) {
            handle->readcb = on_read_headkey1;
        } else {
            handle->readcb = on_read_headkey2;
        }
        return;
    }
    if (len == 0) {
        LOG(WARN, "request not http request");
        handle_destroy(handle);
        return;
    }

    if (first) {
        ADD_REQ_STRING(arg->sendbuf, "&headers=");
    } else {
        ADD_REQ_HEXSTRING(arg->sendbuf, "\r\n");
    }
    add_req_hexstring(arg->sendbuf, key, len - 1);
    on_read_headval(handle);
    if (!strcasecmp(key, "Content-Length:")) {
        arg->content_length = -1;
    }
    return;
}

static void on_read_headkey1(struct handle *handle) {
    on_read_headkey(handle, 1);
}

static void on_read_headkey2(struct handle *handle) {
    on_read_headkey(handle, 0);
}

static void on_read_version(struct handle *handle) {
    char version[16];
    enum buffer_result rv;
    size_t len;
    struct http_arg *arg;

    arg = (struct http_arg *)handle->arg;
    len = sizeof version;
    rv = buffer_read_until(arg->recvbuf, "\n", version, &len);
    if (rv == BUFFER_TOOSMALL) {
        LOG(WARN, "request not http request");
        handle_destroy(handle);
        return;
    }
    if (rv == BUFFER_NOTFOUND) {
        handle->readcb = on_read_version;
        return;
    }
    if (len == 0) {
        LOG(WARN, "request not http request");
        handle_destroy(handle);
        return;
    }

    on_read_headkey1(handle);
    return;
}

static void on_read_url(struct handle *handle) {
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
        handle->readcb = on_read_url;
        return;
    }
    if (len == 0) {
        LOG(WARN, "request not http request");
        handle_destroy(handle);
        return;
    }

    ADD_REQ_STRING(arg->sendbuf, "&url=");
    add_req_hexstring(arg->sendbuf, url, len - 1);

    on_read_version(handle);
    return;
}

void on_http_read(struct handle *handle) {
    char temp[64];
    char command[16];
    size_t len;
    int strlen;
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
    } else if (rv == BUFFER_NOTFOUND) {
        return;
    }

    LOG(INFO, "%.*s", len, command);
    if (strncmp(command, "GET ", sizeof "GET " - 1) == 0) {
        arg->type = HTTP_GET;
        arg->sendbuf = buffer_create();
        ADD_REQ_STRING(arg->sendbuf, "method=");
        ADD_REQ_HEXSTRING(arg->sendbuf, "GET");
    } else if (strncmp(command, "POST ", sizeof "POST " - 1) == 0) {
        arg->type = HTTP_POST;
        arg->sendbuf = buffer_create();
        ADD_REQ_STRING(arg->sendbuf, "method=");
        ADD_REQ_HEXSTRING(arg->sendbuf, "POST");
    } else if (strncmp(command, "CONNECT ", sizeof "CONNECT " - 1) == 0) {
        arg->type = HTTP_CONNECT;
    }

    if (arg->type == HTTP_GET || arg->type == HTTP_POST) {
        on_read_url(handle);
    }
}

