#include "st_reactor.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/poll.h>

#define MAX_FDS 1024

typedef struct fd_handler_t {
    int fd;
    handler_t handler;
} fd_handler_t;

typedef struct st_reactor_t {
    fd_handler_t fd_handlers[MAX_FDS];
    bool running;
    struct pollfd poll_fds[MAX_FDS];
    int num_fds;
} st_reactor_t;

st_reactor_t *create_reactor() {
    st_reactor_t *reactor = malloc(sizeof(st_reactor_t));
    if (reactor == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    reactor->running = false;
    reactor->num_fds = 0;
    return reactor;
}

void start_reactor(st_reactor_t *reactor) {
    reactor->running = true;
    while (reactor->running) {
        int num_fds = poll(reactor->poll_fds, reactor->num_fds, -1);
        if (num_fds < 0) {
            if (errno != EINTR) {
                perror("poll");
                exit(EXIT_FAILURE);
            }
        } else {
            for (int i = 0; i < reactor->num_fds; i++) {
                if (reactor->poll_fds[i].revents & POLLIN) {
                    reactor->fd_handlers[i].handler(reactor->poll_fds[i].fd);
                }
            }
        }
    }
}

void stop_reactor(st_reactor_t *reactor) {
    reactor->running = false;
}

void add_fd(st_reactor_t *reactor, int fd, handler_t handler) {
    if (reactor->num_fds >= MAX_FDS) {
        fprintf(stderr, "Maximum number of file descriptors exceeded\n");
        exit(EXIT_FAILURE);
    }
    fd_handler_t fd_handler = { .fd = fd, .handler = handler };
    reactor->fd_handlers[reactor->num_fds] = fd_handler;
    struct pollfd poll_fd = { .fd = fd, .events = POLLIN };
    reactor->poll_fds[reactor->num_fds] = poll_fd;
    reactor->num_fds++;
}

void wait_for_reactor(st_reactor_t *reactor) {
    while (reactor->running) {
        sleep(1);
    }
}
