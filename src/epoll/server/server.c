#include <sys/epoll.h>

#include <netinet/in.h>

#include <liburing.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "../../utils/constants.h"
#include "../../utils/globals.h"
#include "../handlers/client_handler.h"
#include "../network/network.h"
#include "server.h"

int epoll_fd;
struct epoll_event events[MAX_EVENTS];

_Noreturn static void
server_loop(int server_socket)
{
	int epoll_ret = 0;
	int client_socket = 0;
	struct sockaddr_in client_addr;
	socklen_t client_addr_len = sizeof(client_addr);

	while (1) {
		epoll_ret = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
		if (epoll_ret < 0) {
			exit(1);
		}

		for (int i = 0; i < epoll_ret; i++) {

			if (events[i].events == 0) {
				continue;
			}

			client_socket = accept(server_socket,
			    (struct sockaddr *)&client_addr, &client_addr_len);

			if (client_socket > 0) {
				handle_client(client_socket);
			}
		}
	}
}

_Noreturn void
serve_epoll(void)
{
	int server_socket = setup_listening_socket((int)server_port);

	epoll_fd = epoll_create1(0);
	if (epoll_fd < 0) {
		exit(1);
	}

	add_to_epoll_fd_list(server_socket, EPOLLIN);
	server_loop(server_socket);
}
