#ifndef _BUFFER_H_
#define _BUFFER_H_

#define NODE_SIZE 4 * 1024 - sizeof(void *) - sizeof(size_t) * 2

typedef struct buffer_node {
    struct buffer_node *next;
    size_t begin;
    size_t end;
    char buffer[NODE_SIZE];
} buffer_node_t;

typedef struct {
    buffer_node_t *first;
    buffer_node_t *last;
    size_t len;
} buffer_t;

typedef struct {
    buffer_t *buffer;
    buffer_node_t *node;
    size_t pos;
} buffer_ptr_t;

buffer_t * buffer_create();
void buffer_destroy(buffer_t *buffer);
int buffer_recv(buffer_t *buffer, int fd);
int buffer_read_until(buffer_t *buffer,
                      const char *_delim,
                      char *outbuf,
                      size_t *outlen); 

#endif

