#include <stdio.h>
#include <string.h>

#include "../../utils/constants.h"
#include "dynamic_handlers.h"
#include "request_handler.h"
#include "static_handlers.h"

static int
get_line(const char *src, char *dest, int dest_sz)
{
	for (int i = 0; i < dest_sz; i++) {
		dest[i] = src[i];
		if (src[i] == '\r' && src[i + 1] == '\n') {
			dest[i] = '\0';
			return 0;
		}
	}
	return 1;
}

static void
handle_http_method(char *method_buffer, int client_socket)
{
	char *method = NULL, *path = NULL, *saveptr = NULL;

	method = strtok_r(method_buffer, " ", &saveptr);
	path = strtok_r(NULL, " ", &saveptr);

	if (!strcmp(method, "GET")) {
		handle_get_method_liburing(path, client_socket);
	} else {
		handle_unimplemented_method_liburing(client_socket);
	}
}

int
handle_client_request(struct request *req)
{
	char http_request[buffer_size];
	if (get_line(req->iov[0].iov_base, http_request,
		sizeof(http_request))) {
		(void)fprintf(stderr, "Malformed request\n");
		exit(1);
	}
	handle_http_method(http_request, req->client_socket);
	return 0;
}
