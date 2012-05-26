/*
 * handle.h
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

#ifndef _HANDLE_H_
#define _HANDLE_H_

#include "uthash.h"
#include "buffer.h"

struct handle;
typedef void (*handle_cb)(struct handle *);
typedef void (*arg_deleter)(void *arg);

struct handle
{
    int fd;
    handle_cb readcb;
    arg_deleter deleter;
    void *arg;
    UT_hash_handle hh;
};

struct handle * handle_create(int fd);
int handle_destroy(struct handle *handle);
struct handle * handle_get(int fd);

struct handle * handle_first();
struct handle * handle_next(struct handle *handle);


#endif

