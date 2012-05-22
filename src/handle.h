#ifndef _HANDLE_H_
#define _HANDLE_H_

#include "uthash.h"

typedef struct {
    int fd;
    UT_hash_handle hh;
} handle_t;

handle_t * handle_create(int fd);
int handle_destroy(handle_t *handle);

handle_t * handle_first();
handle_t * handle_next(handle_t *handle);


#endif

