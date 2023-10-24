#include <liburing.h>
#include <stdlib.h>
#include <string.h>

#include "../../utils/constants.h"
#include "../network/network.h"
#include "../utils/request.h"
#include "static_handlers.h"

static void
static_content(const char *str, int client_socket)
{
	request_t *req = malloc(sizeof(*req) + sizeof(struct iovec) + 1);
	unsigned long slen = strlen(str);
	req->iovec_count = 1;
	req->client_socket = client_socket;
	req->iov[0].iov_base = malloc(slen);
	req->iov[0].iov_len = slen;
	strcpy((char *)req->iov[0].iov_base, str);
	add_write_request(req);
}

void
handle_unimplemented_method_liburing(int client_socket)
{
	static_content(unimplemented_content, client_socket);
}

void
handle_http_404_liburing(int client_socket)
{
	static_content(http_404_content, client_socket);
}
