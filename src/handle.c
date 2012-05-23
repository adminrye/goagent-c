#include "handle.h"
#include "logger.h"

static handle_t *g_handles = NULL;

handle_t * handle_create(int fd) {
    handle_t *handle;
    
    handle = (handle_t *)calloc(1, sizeof *handle);
    if (handle == NULL) {
        LOG(ERR, "memory alloc failed");
        abort();
        return NULL;
    }
    handle->fd = fd;
    HASH_ADD_INT(g_handles, fd, handle);
    return handle;
}

int handle_destroy(handle_t *handle) {
    HASH_DEL(g_handles, handle);
    close(handle->fd);
    buffer_destroy(handle->readbuf);
    free(handle);
    return 0;
}

handle_t *handle_get(int fd) {
   handle_t *h;

   HASH_FIND_INT(g_handles, &fd, h);  
   return h;
}

handle_t * handle_first() {
    return g_handles;
}

handle_t * handle_next(handle_t *handle) {
    return handle->hh.next;
}
