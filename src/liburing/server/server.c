#include <netinet/in.h>

#include <liburing.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "../../utils/constants.h"
#include "../handlers/request_handler.h"
#include "../network/network.h"
#include "../utils/request.h"
#include "server.h"

struct io_uring ring;

static void
server_loop(int server_socket)
{
	struct io_uring_cqe *cqe = NULL;
	struct sockaddr_in client_addr;
	socklen_t client_addr_len = sizeof(client_addr);

	add_accept_request(server_socket, &client_addr, &client_addr_len);

	while (1) {
		int ret = io_uring_wait_cqe(&ring, &cqe);
		request_t *req = (request_t *)cqe->user_data;
		if (ret < 0) {
			exit(1);
		}
		if (cqe->res < 0) {
			(void)fprintf(stderr,
			    "Async request failed: %s for event: %d\n",
			    strerror(-cqe->res), req->event_type);
			exit(1);
		}

		switch (req->event_type) {
		case EVENT_TYPE_ACCEPT:
			add_accept_request(server_socket, &client_addr,
			    &client_addr_len);
			add_read_request(cqe->res);
			free(req);
			break;
		case EVENT_TYPE_READ:
			if (!cqe->res) {
				(void)fprintf(stderr, "Empty request!\n");
				break;
			}
			handle_client_request(req);
			free(req->iov[0].iov_base);
			free(req);
			break;
		case EVENT_TYPE_WRITE:
			for (int i = 0; i < req->iovec_count; i++) {
				free(req->iov[i].iov_base);
			}
			close(req->client_socket);
			free(req);
			break;
		}
		io_uring_cqe_seen(&ring, cqe);
	}
}

static void
sigint_handler(int signo)
{
	io_uring_queue_exit(&ring);
	exit(0);
}
void
serve(void)
{
	int server_socket = setup_listening_socket(server_port);

	(void)signal(SIGINT, sigint_handler);
	io_uring_queue_init(queue_depth, &ring, 0);
	server_loop(server_socket);
}
