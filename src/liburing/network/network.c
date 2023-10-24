#include <liburing.h>
#include <stdlib.h>
#include <string.h>

#include "../../utils/constants.h"
#include "../../utils/globals.h"
#include "network.h"

int
add_accept_request(int server_socket, struct sockaddr_in *client_addr,
    socklen_t *client_addr_len)
{
	request_t *req = NULL;
	struct io_uring_sqe *sqe = io_uring_get_sqe(&ring);
	io_uring_prep_accept(sqe, server_socket, (struct sockaddr *)client_addr,
	    client_addr_len, 0);
	req = malloc(sizeof(*req));
	req->event_type = EVENT_TYPE_ACCEPT;
	io_uring_sqe_set_data(sqe, req);
	io_uring_submit(&ring);

	return 0;
}

int
add_read_request(int client_socket)
{
	struct io_uring_sqe *sqe = io_uring_get_sqe(&ring);
	struct request *req = malloc(sizeof(*req) + sizeof(struct iovec));
	req->iov[0].iov_base = malloc(read_size);
	req->iov[0].iov_len = read_size;
	req->event_type = EVENT_TYPE_READ;
	req->client_socket = client_socket;
	memset(req->iov[0].iov_base, 0, read_size);
	io_uring_prep_readv(sqe, client_socket, &req->iov[0], 1, 0);
	io_uring_sqe_set_data(sqe, req);
	io_uring_submit(&ring);
	return 0;
}

int
add_write_request(request_t *req)
{
	struct io_uring_sqe *sqe = io_uring_get_sqe(&ring);
	req->event_type = EVENT_TYPE_WRITE;
	io_uring_prep_writev(sqe, req->client_socket, req->iov,
	    req->iovec_count, 0);
	io_uring_sqe_set_data(sqe, req);
	io_uring_submit(&ring);
	return 0;
}
