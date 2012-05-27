/*
 * buffer.h
 *
 * Copyright xubin, 2012
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

#ifndef _BUFFER_H_
#define _BUFFER_H_

#define NODE_SIZE 4 * 1024 - sizeof(void *) - sizeof(size_t) * 2

struct buffer_node {
    struct buffer_node *next;
    size_t begin;
    size_t end;
    char buffer[NODE_SIZE];
};

struct buffer {
    struct buffer_node *first;
    struct buffer_node *last;
    size_t len;
};

struct buffer_ptr {
    struct buffer *buffer;
    struct buffer_node *node;
    size_t pos;
};

enum buffer_result {
    BUFFER_FOUND,
    BUFFER_NOTFOUND,
    BUFFER_TOOSMALL,
    BUFFER_INVAL // invalid argument passed
};

struct buffer * buffer_create();
void buffer_destroy(struct buffer *buffer);
int buffer_recv(struct buffer *buffer, int fd);
enum buffer_result buffer_read_until(struct buffer *buffer,
                                     const char *_delim,
                                     char *outbuf,
                                     size_t *outlen); 
int buffer_drain(struct buffer *buffer, size_t len);
void buffer_prepare_space(struct buffer *buffer);
void buffer_append(struct buffer *buffer, const char *str, size_t len);

#endif

