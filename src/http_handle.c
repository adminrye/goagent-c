#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include "handle.h"
#include "logger.h"
#include "buffer.h"

static void
on_http_get(handle_t *handle) {
}

static void
on_http_post(handle_t *handle) {
}

static void
on_http_connect(handle_t *handle) {
}

void
on_http_read(handle_t *handle) {
    char command[16];
    size_t len;
    int rv;
    
    if (handle->recvbuf == NULL) {
        handle->recvbuf = buffer_create();
    }
    rv = buffer_recv(handle->recvbuf, handle->fd);
    if (rv == -1) {
        LOG(ERR, "recv() failed:%s", strerror(errno));
        handle_destroy(handle);
        return;
    }
    if (rv == 0) {
        LOG(INFO, "client closed");
        handle_destroy(handle);
        return;
    }

    len = sizeof command;
    rv = buffer_read_until(handle->recvbuf, " ", command, &len);
    if (rv == -1 || len == sizeof command) {
        LOG(WARN, "request is not http");
        handle_destroy(handle);
        return;
    }
    command[len] = 0;
    if (strcmp(command, "GET ") == 0) {
        LOG(INFO, "GET");
    } else if (strcmp(command, "POST ") == 0) {
        LOG(INFO, "POST");
    } else if (strcmp(command, "CONNECT ") == 0) {
        LOG(INFO, "CONNECT");
    }
}
