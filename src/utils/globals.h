#pragma once

#include <sys/epoll.h>

#include <liburing.h>

#include "constants.h"

extern const struct io_uring ring;

extern int epoll_fd;
extern struct epoll_event events[MAX_EVENTS];
