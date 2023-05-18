#ifndef ST_REACTOR_H_
#define ST_REACTOR_H_

#include <stdbool.h>

typedef void (*handler_t)(int fd);

typedef struct st_reactor_t st_reactor_t;

st_reactor_t *create_reactor();
void start_reactor(st_reactor_t *reactor);
void stop_reactor(st_reactor_t *reactor);
void add_fd(st_reactor_t *reactor, int fd, handler_t handler);
void wait_for_reactor(st_reactor_t *reactor);

#endif /* ST_REACTOR_H_ */
