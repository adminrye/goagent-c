/*
 * main.c
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

#include <stdio.h> 
#include <string.h> 
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <errno.h>
#include "handle.h"
#include "listen_handle.h"
#include "logger.h"
#include "buffer.h"

static void
run() {
    struct handle *handle, *temp;
    int maxfd, rv, err;
    fd_set rfds, efds;
    socklen_t len;
    
    for (;;) {
        maxfd = -1;
        FD_ZERO(&rfds);
        FD_ZERO(&efds);

        handle = handle_first();
        while (handle) {
            FD_SET(handle->fd, &rfds);
            FD_SET(handle->fd, &efds);
            if (handle->fd > maxfd) {
                maxfd = handle->fd;
            }
            handle = handle_next(handle);
        }
        if (maxfd == -1) {
            LOG(INFO, "all handle closed");
            return;
        }

retry:
        rv = select(maxfd + 1, &rfds, NULL, &efds, NULL);
        if (rv == -1) {
            if (errno == EINTR) {
                goto retry;
            }
            LOG(ERR, "select() failed:%s", strerror(errno));
            return;
        }
        
        handle = handle_first();
        while (handle) {
            temp = handle_next(handle);
            if (handle->readcb && FD_ISSET(handle->fd, &rfds)) {
                handle->readcb(handle);
            }
            handle = temp;
        }

        handle = handle_first();
        while (handle) {
            if (!FD_ISSET(handle->fd, &efds)) {
                handle = handle_next(handle);
                continue;
            }
            temp = handle_next(handle);
            len = sizeof err;
            rv = getsockopt(handle->fd, SOL_SOCKET, SO_ERROR, &err, &len);
            if (rv == -1) {
                LOG(ERR, "getsockopt() failed:%s", strerror(errno));
            } else {
                LOG(ERR, "socket error:%s", strerror(err));
                handle_destroy(handle);
            }
            handle = temp;
        }
    }
}

static void
set_reuseaddr(int fd) {
    int flag, len, rv;

    flag = 1;
    len = sizeof flag;
    rv = setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &flag, len);
    if (rv == -1) {
        LOG(ERR, "getsockopt() failed:%s", strerror(errno));
    }
}

int main() { 
    int fd, rv;
    struct sockaddr_in listenaddr;
    struct handle *handle;

    fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd == -1) {
        LOG(ERR, "socket() failed:%s", strerror(errno));
        return 1;
    }
    set_reuseaddr(fd);
    memset(&listenaddr, 0, sizeof listenaddr);
    listenaddr.sin_family = AF_INET;
    listenaddr.sin_port = htons(9087);
    listenaddr.sin_addr.s_addr = INADDR_ANY;
    rv = bind(fd, (struct sockaddr *)&listenaddr, sizeof listenaddr);
    if (rv == -1) {
        LOG(ERR, "bind() failed:%s", strerror(errno));
        return 1;
    }
    rv = listen(fd, 5);
    if (rv == -1) {
        LOG(ERR, "listen() failed:%s", strerror(errno));
        return 1;
    }

    handle = handle_create(fd);
    handle->readcb = on_accept;

    LOG(INFO, "listen succeed");
    run();

    return 0;
}
