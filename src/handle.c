/*
 * handle.c
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

#include "handle.h"
#include "logger.h"

static struct handle *g_handles = NULL;

struct handle * handle_create(int fd) {
    struct handle *handle;
    
    handle = (struct handle *)calloc(1, sizeof *handle);
    if (handle == NULL) {
        LOG(ERR, "memory alloc failed");
        abort();
        return NULL;
    }
    handle->fd = fd;
    HASH_ADD_INT(g_handles, fd, handle);
    return handle;
}

int handle_destroy(struct handle *handle) {
    HASH_DEL(g_handles, handle);
    close(handle->fd);
    if (handle->freer) {
        handle->freer(handle->arg);
    }
    free(handle);
    return 0;
}

struct handle *handle_get(int fd) {
    struct handle *h;

    HASH_FIND_INT(g_handles, &fd, h);  
    return h;
}

struct handle * handle_first() {
    return g_handles;
}

struct handle * handle_next(struct handle *handle) {
    return handle->hh.next;
}
