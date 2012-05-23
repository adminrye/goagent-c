#include <stdlib.h>
#include <errno.h>
#include "buffer.h"
#include "logger.h"

buffer_t *
buffer_create() {
    buffer_t *buffer;

    buffer = calloc(1, sizeof *buffer);
    if (buffer == NULL) {
        LOG(ERR, "memory alloc failed");
        abort();
        return NULL;
    }
    return buffer;
}

void
buffer_destroy(buffer_t *buffer) {
    buffer_node_t *node, *next;

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

static buffer_node_t *
buffer_node_create() {
    buffer_node_t *node;

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

int
buffer_recv(buffer_t *buffer, int fd) {
    int rv;

    if (buffer->first == NULL) {
        buffer->first = buffer_node_create();
        buffer->last = buffer->first;
    } else if (buffer->last->end == NODE_SIZE) {
        buffer->last->next = buffer_node_create();
        buffer->last = buffer->last->next;
    }
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
    return rv;
}

static inline int
buffer_ptr_begin(buffer_t *buffer, buffer_ptr_t *ptr) {
    ptr->buffer = buffer;
    ptr->node = buffer->first;
    ptr->pos = buffer->first->begin;
}

static inline char
buffer_ptr_char(buffer_ptr_t *ptr) {
    return ptr->node->buffer[ptr->pos];
}

static inline int
buffer_ptr_eof(buffer_ptr_t *ptr) {
    if (ptr->pos == ptr->node->end) {
        return 1;
    }
    return 0;
}

static inline int
buffer_ptr_next(buffer_ptr_t *ptr) {
    ++ptr->pos;
    if (ptr->pos == ptr->node->end) {
        ptr->node = ptr->node->next;
        ptr->pos = ptr->node->begin;
    }
}

int
buffer_read_until(buffer_t *buffer,
                  const char *_delim,
                  char *outbuf,
                  size_t *outlen) {
    buffer_node_t *node, *next;
    size_t pos;
    const char *delim;
    buffer_ptr_t ptr;

    if (buffer == NULL) {
        *outlen = 0;
        return -1;
    }
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
                    return 0;
                }
                if (pos == *outlen) {
                    return -1;
                }
                buffer_ptr_next(&ptr);
                if (buffer_ptr_eof(&ptr)) {
                    *outlen = 0;
                    return -1;
                }
                ch = buffer_ptr_char(&ptr);
                outbuf[pos++] = ch;
                if (*delim != ch) {
                    if (pos == *outlen) {
                        return -1;
                    }
                    delim = _delim;
                    break;
                }
            }
        }
        buffer_ptr_next(&ptr);
    }
    *outlen = 0;
    return -1;
}
