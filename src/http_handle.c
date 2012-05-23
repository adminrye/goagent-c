#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include "handle.h"
#include "logger.h"
#include "buffer.h"

void on_http_read(struct tag_handle_t *handle) {
    char buf[4096];
    ssize_t rv;
    
    if (handle->readbuf == NULL) {
        handle->readbuf = buffer_create();
    }
    rv = buffer_recv(handle->readbuf, handle->fd);
    if (rv == -1) {
        LOG(ERR, "recv() failed:%s", strerror(errno));
        handle_destroy(handle);
        return;
    }
    if (rv == 0) {
        LOG(INFO, "client closed");
        handle_destroy(handle);
    }
}
