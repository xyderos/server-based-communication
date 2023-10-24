#include <sys/epoll.h>

#include <netinet/in.h>

#include <liburing.h>
#include <stdlib.h>
#include <string.h>

#include "../../utils/constants.h"
#include "../../utils/globals.h"
#include "network.h"

void
add_to_epoll_fd_list(int fd, uint32_t ep_events)
{
	struct epoll_event event;
	event.data.fd = fd;
	event.events = ep_events;
	if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &event)) {
		exit(1);
	}
}

void
remove_from_epoll_fd_list(int fd)
{
	if (epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, NULL)) {
		exit(1);
	}
}
