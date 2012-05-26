/*
 * buffer.c
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

#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include "buffer.h"
#include "logger.h"

struct buffer * buffer_create() {
    struct buffer *buffer;

    buffer = calloc(1, sizeof *buffer);
    if (buffer == NULL) {
        LOG(ERR, "memory alloc failed");
        abort();
        return NULL;
    }
    return buffer;
}

void buffer_destroy(struct buffer *buffer) {
    struct buffer_node *node, *next;

    if (buffer == NULL) {
        return;
    }
    node = buffer->first;
    while (node) {
        next = node->next;
        free(node);
        node = next;
    }
    free(buffer);
}

static struct buffer_node * buffer_node_create() {
    struct buffer_node *node;

    node = malloc(sizeof *node);
    if (node == NULL) {
        LOG(ERR, "memory alloc failed");
        abort();
        return NULL;
    }
    node->next = NULL;
    node->begin = node->end = 0;
    return node;
}

int buffer_recv(struct buffer *buffer, int fd) {
    int rv;

    buffer_prepare_space(buffer);
retry:
    rv = recv(fd,
              buffer->last->buffer + buffer->last->end,
              NODE_SIZE - buffer->last->end,
              0);
    if (rv == -1 && errno == EINTR) {
        goto retry;
    }
    if (rv > 0) {
        buffer->last->end = buffer->last->end + rv;
    }
    buffer->len += rv;
    return rv;
}

void buffer_append_string(struct buffer *buffer, const char *str, size_t _len) {
    const char *src;
    size_t len, node_len, cpy_len;

    src = str;
    len = _len;
    while (len > 0) {
        buffer_prepare_space(buffer);
        node_len = NODE_SIZE - buffer->last->end;
        cpy_len = node_len > len ? len : node_len;
        memcpy(buffer->last->buffer + buffer->last->end,
               str,
               cpy_len);
        str += cpy_len;
        len -= cpy_len;
    }
}

static inline int buffer_ptr_begin(struct buffer *buffer,
                                   struct buffer_ptr *ptr) {
    ptr->buffer = buffer;
    ptr->node = buffer->first;
    ptr->pos = buffer->first->begin;
}

static inline char buffer_ptr_char(struct buffer_ptr *ptr) {
    return ptr->node->buffer[ptr->pos];
}

static inline int buffer_ptr_eof(struct buffer_ptr *ptr) {
    return (ptr->pos == ptr->node->end) ? 1 : 0;
}

static inline void buffer_ptr_next(struct buffer_ptr *ptr) {
    ++ptr->pos;
    if (ptr->pos == ptr->node->end) {
        if (ptr->node->next == NULL) {
            return;
        }
        ptr->node = ptr->node->next;
        ptr->pos = ptr->node->begin;
    }
    return;
}

enum buffer_result buffer_read_until(struct buffer *buffer,
                                     const char *_delim,
                                     char *outbuf,
                                     size_t *outlen) {
    struct buffer_node *node, *next;
    size_t pos;
    const char *delim;
    struct buffer_ptr ptr;

    if (buffer == NULL
            || _delim == NULL
            || outbuf == NULL
            || outlen == NULL
            || _delim[0] == 0) {
        LOG(ERR, "buffer_read_until invalid arugment passed");
        abort();
        return BUFFER_INVAL;
    }

    if (*outlen == 0)
        return BUFFER_TOOSMALL;

    if (buffer == NULL || buffer->first == NULL) 
        return BUFFER_NOTFOUND;

    pos = 0;
    buffer_ptr_begin(buffer, &ptr);
    delim = _delim;
    while (!buffer_ptr_eof(&ptr)) {
        char ch = buffer_ptr_char(&ptr);
        outbuf[pos++] = ch;
        if (ch == *delim) {
            for (;;) {
                ++delim;
                if (*delim == 0) {
                    *outlen = pos;
                    buffer_drain(buffer, pos);
                    return BUFFER_FOUND; 
                }
                if (pos >= *outlen) 
                    return BUFFER_TOOSMALL;

                buffer_ptr_next(&ptr);
                if (buffer_ptr_eof(&ptr)) 
                    return BUFFER_NOTFOUND;

                ch = buffer_ptr_char(&ptr);
                outbuf[pos++] = ch;
                if (*delim != ch) {
                    delim = _delim;
                    break;
                }
            }
        }
        if (pos >= *outlen) 
            return BUFFER_TOOSMALL;
        buffer_ptr_next(&ptr);
    }
    *outlen = 0;
    return BUFFER_NOTFOUND;
}

int buffer_drain(struct buffer *buffer, size_t _len) {
    struct buffer_node *node, *next;
    size_t node_size;
    size_t len;

    len = _len;
    node = buffer->first;
    while (node) {
        next = node->next;
        node_size = node->end - node->begin;
        if (node_size > len) {
            node->begin += len;
            buffer->len -= len;
            return 0;
        } else if (node_size == len) {
            buffer->first = next;
            if (buffer->first == NULL) {
                buffer->last = NULL;
            }
            free(node);
            buffer->len -= len;
            return 0;
        } else {
            buffer->first = next;
            free(node);
            len -= node_size;
        }
        node = next;
    }
    return -1;
}

void buffer_prepare_space(struct buffer *buffer) {
    if (buffer->first == NULL) {
        buffer->first = buffer_node_create();
        buffer->last = buffer->first;
    } else if (buffer->last->end == NODE_SIZE) {
        buffer->last->next = buffer_node_create();
        buffer->last = buffer->last->next;
    }
}
