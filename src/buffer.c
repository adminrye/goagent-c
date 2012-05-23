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
