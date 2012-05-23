#ifndef _HANDLE_H_
#define _HANDLE_H_

#include "uthash.h"
#include "buffer.h"

struct tag_handle_t;
typedef void (*handle_cb)(struct tag_handle_t *);

typedef struct tag_handle_t {
    int fd;
    handle_cb readcb;
    buffer_t *readbuf;
    UT_hash_handle hh;
} handle_t;

handle_t * handle_create(int fd);
int handle_destroy(handle_t *handle);
handle_t * handle_get(int fd);

handle_t * handle_first();
handle_t * handle_next(handle_t *handle);


#endif

