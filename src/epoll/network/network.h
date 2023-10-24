#pragma once

#include <stdint.h>

void add_to_epoll_fd_list(int fd, uint32_t ep_events);

void remove_from_epoll_fd_list(int);
