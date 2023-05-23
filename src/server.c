#include <sys/socket.h>
#include <event2/event.h>
#include <event2/event_struct.h>
#include <event2/util.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

#include "server.h"
#include "macro.h"

#define MAX_BACKLOG 1024

static void accept_cb(evutil_socket_t fd, short events, void *arg) {
    UNUSED(fd);
    UNUSED(events);
    UNUSED(arg);

    int client_fd;
    struct sockaddr_storage client_addr;
    socklen_t client_len = sizeof(client_addr);

    client_fd = accept(fd, (struct sockaddr *)&client_addr, &client_len);
    if(client_fd < 0) {
        perror("accept");
        return;
    }
    char buf[] = "hi erryone!";
    send(client_fd, buf, sizeof(buf), 0);
    close(client_fd);

}

static int set_nonblocking(int fd) {
    int flags;

    if ((flags = fcntl(fd, F_GETFL)) < 0) {
        perror("fcntl");
        return flags;
    }
    flags |= O_NONBLOCK;
    if (fcntl(fd, F_SETFL, flags) < 0) {
        perror("fcntl");
        return -1;
    }

    return 0;
}

static int get_listener(char *port) {
    if(port == NULL) {
        return -1;
    }
    struct addrinfo hints, *res;
    int gai_status;
    int ret_listener;

    memset(&hints, 0x0, sizeof(struct addrinfo));
    hints.ai_flags = AI_PASSIVE;
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if((gai_status = getaddrinfo(NULL, port, &hints, &res)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(gai_status));
        return -1;
    }

    ret_listener = -1;
    struct addrinfo *p;
    for(p = res; p; p = p->ai_next) {
        int yes = 1;
        if((ret_listener = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
                perror("socket");
                freeaddrinfo(res);
                return -1;
        }

        if(setsockopt(ret_listener, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &yes, sizeof(int)) == -1) {
            perror("setsockopt");
            close(ret_listener);
            freeaddrinfo(res);
            return -1;
        }

        if(bind(ret_listener, p->ai_addr, p->ai_addrlen) == -1) {
            perror("bind");
            close(ret_listener);
            freeaddrinfo(res);
            return -1;
        }

        if(listen(ret_listener, MAX_BACKLOG) == -1) {
            perror("listen");
            close(ret_listener);
            freeaddrinfo(res);
            return -1;
        }
        break;
    }
    freeaddrinfo(res);
    return ret_listener;
}

int server_init() {
    int listenfd = get_listener("8080");
    struct event ev_accept;
    struct event_base *evb;

    evb = event_base_new();

    if(listenfd < 0) {
        return -1;
    }
    if(set_nonblocking(listenfd) < 0) {
        close(listenfd);
        return -1;
    }

    if(event_assign(&ev_accept, evb, listenfd, EV_READ | EV_PERSIST, accept_cb, NULL) < 0) {
        close(listenfd);
        return -1;
    }

    if(event_add(&ev_accept, NULL) < 0) {
        close(listenfd);
        return -1;
    }
    fprintf(stdout, "Starting server on port 8080\n");
    event_base_dispatch(evb);
    event_base_free(evb);
    close(listenfd);

    return 0;
}
