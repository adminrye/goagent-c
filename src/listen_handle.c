#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include "handle.h"
#include "http_handle.h"

void on_accept(struct tag_handle_t *handle) {
    int fd;
    handle_t *client;

retry:
    fd = accept(handle->fd, NULL, 0);
    if (fd == -1) {
        if (errno == EINTR) {
            goto retry;
        }
        perror("accept() failed");
        handle_destroy(handle);
        return;
    }

    client = handle_create(fd);
    client->readcb = on_http_read;
}
