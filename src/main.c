#include <stdio.h> 
#include <string.h> 
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <errno.h>
#include "handle.h"
#include "listen_handle.h"

void run() {
    handle_t *handle, *temp;
    int maxfd, rv, err;
    fd_set rfds, efds;
    socklen_t len;
    
    maxfd = -1;
    FD_ZERO(&rfds);
    FD_ZERO(&efds);

    handle = handle_first();
    while (handle) {
        FD_SET(handle->fd, &rfds);
        FD_SET(handle->fd, &efds);
        if (handle->fd > maxfd) {
            maxfd = handle->fd;
        }
        handle = handle_next(handle);
    }
    if (maxfd == -1) {
        return;
    }

retry:
    rv = select(maxfd + 1, &rfds, NULL, &efds, NULL);
    if (rv == -1) {
        if (errno == EINTR) {
            goto retry;
        }
        perror("select() failed");
        return;
    }
    
    handle = handle_first();
    while (handle) {
        temp = handle_next(handle);
        if (handle->readcb && FD_ISSET(handle->fd, &rfds)) {
            handle->readcb(handle);
        }
        handle = temp;
    }

    handle = handle_first();
    while (handle) {
        if (!FD_ISSET(handle->fd, &efds)) {
            handle = handle_next(handle);
            continue;
        }
        temp = handle_next(handle);
        len = sizeof err;
        rv = getsockopt(handle->fd, SOL_SOCKET, SO_ERROR, &err, &len);
        if (rv == -1) {
            perror("getsockopt() failed");
        } else {
            fprintf(stderr, "error event:%s", strerror(err));
            handle_destroy(handle);
        }
        handle = temp;
    }
}

int main() { 
    int listenfd, rv;
    struct sockaddr_in listenaddr;
    handle_t *handle;

    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if (listenfd == -1) {
        perror("socket() failed");
        return 1;
    }
    memset(&listenaddr, 0, sizeof listenaddr);
    listenaddr.sin_family = AF_INET;
    listenaddr.sin_port = htons(9087);
    listenaddr.sin_addr.s_addr = INADDR_ANY;
    rv = bind(listenfd, (struct sockaddr *)&listenaddr, sizeof listenaddr);
    if (rv == -1) {
        perror("bind() failed");
        return 1;
    }
    rv = listen(listenfd, 5);
    if (rv == -1) {
        perror("listen() failed");
        return 1;
    }

    handle = handle_create(listenfd);
    handle->readcb = on_accept;

    run();
    return 0;
}
