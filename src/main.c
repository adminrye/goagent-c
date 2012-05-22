#include <stdio.h> 
#include <string.h> 
#include <sys/socket.h> 
#include <netinet/in.h> 
#include "handle.h"

void run() {
    handle_t *handle;

    handle = handle_first();
    while (handle) {
        handle = handle_next(handle);
    }
}

int main() { 
    int listenfd, rv;
    struct sockaddr_in listenaddr;

    
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

    handle_create(listenfd);

    run();
    return 0;
}
