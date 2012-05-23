#ifndef _BUFFER_H_
#define _BUFFER_H_

#define NODE_SIZE 4 * 1024 - sizeof(void *) - sizeof(size_t) * 2

typedef struct tag_buffer_node_t {
    struct tag_buffer_node_t *next;
    size_t begin;
    size_t end;
    char buffer[NODE_SIZE];
} buffer_node_t;

typedef struct {
    buffer_node_t *first;
    buffer_node_t *last;
    size_t len;
} buffer_t;

buffer_t * buffer_create();
void buffer_destroy(buffer_t *buffer);
int buffer_recv(buffer_t *buffer, int fd);

#endif

