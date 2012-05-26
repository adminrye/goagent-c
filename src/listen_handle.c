/*
 * listen_handle.c
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
#include "http_handle.h"
#include "logger.h"

void on_accept(struct handle *handle) {
    int fd;
    struct handle *client;

retry:
    fd = accept(handle->fd, NULL, 0);
    if (fd == -1) {
        if (errno == EINTR) {
            goto retry;
        }
        LOG(WARN, "accept() failed:%s", strerror(errno));
        handle_destroy(handle);
        return;
    }

    LOG(INFO, "client connected");
    client = handle_create(fd);
    client->readcb = on_http_read;
    client->arg = calloc(1, sizeof(struct http_arg));
    client->deleter = http_arg_deleter;
}
