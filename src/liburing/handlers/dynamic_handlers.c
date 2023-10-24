#include <liburing.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../utils/constants.h"
#include "../../utils/fs.h"
#include "../network/network.h"
#include "../utils/request.h"
#include "dynamic_handlers.h"
#include "static_handlers.h"

static void
send_headers(const char *path, off_t len, struct iovec *iov)
{
	char send_buffer[buffer_size];
	char *str = "HTTP/1.0 200 OK\r\n";
	const char *file_ext = get_filename_ext(path);
	unsigned long slen = strlen(str);
	iov[0].iov_base = malloc(slen);
	iov[0].iov_len = slen;
	memcpy(iov[0].iov_base, str, slen);

	slen = strlen(server_string);
	iov[1].iov_base = malloc(slen);
	iov[1].iov_len = slen;
	memcpy(iov[1].iov_base, server_string, slen);

	if (strcmp("jpg", file_ext) == 0) {
		strcpy(send_buffer, "Content-Type: image/jpeg\r\n");
	}
	if (strcmp("jpeg", file_ext) == 0) {
		strcpy(send_buffer, "Content-Type: image/jpeg\r\n");
	}
	if (strcmp("png", file_ext) == 0) {
		strcpy(send_buffer, "Content-Type: image/png\r\n");
	}
	if (strcmp("gif", file_ext) == 0) {
		strcpy(send_buffer, "Content-Type: image/gif\r\n");
	}
	if (strcmp("htm", file_ext) == 0) {
		strcpy(send_buffer, "Content-Type: text/html\r\n");
	}
	if (strcmp("html", file_ext) == 0) {
		strcpy(send_buffer, "Content-Type: text/html\r\n");
	}
	if (strcmp("js", file_ext) == 0) {
		strcpy(send_buffer, "Content-Type: application/javascript\r\n");
	}
	if (strcmp("css", file_ext) == 0) {
		strcpy(send_buffer, "Content-Type: text/css\r\n");
	}
	if (strcmp("txt", file_ext) == 0) {
		strcpy(send_buffer, "Content-Type: text/plain\r\n");
	}
	slen = strlen(send_buffer);
	iov[2].iov_base = malloc(slen);
	iov[2].iov_len = slen;
	memcpy(iov[2].iov_base, send_buffer, slen);

	(void)sprintf(send_buffer, "content-length: %ld\r\n", len);
	slen = strlen(send_buffer);
	iov[3].iov_base = malloc(slen);
	iov[3].iov_len = slen;
	memcpy(iov[3].iov_base, send_buffer, slen);

	strcpy(send_buffer, "\r\n");
	slen = strlen(send_buffer);
	iov[4].iov_base = malloc(slen);
	iov[4].iov_len = slen;
	memcpy(iov[4].iov_base, send_buffer, slen);
}

void
handle_get_method_liburing(char *path, int client_socket)
{
	char final_path[buffer_size];
	struct stat path_stat;

	strcpy(final_path, ".");
	strcat(final_path, path);

	if (stat(final_path, &path_stat) == -1) {
		printf("404 Not Found: %s (%s)\n", final_path, path);
		handle_http_404_liburing(client_socket);
	} else {

		if (S_ISREG(path_stat.st_mode)) {
			request_t *req = malloc(
			    sizeof(*req) + (sizeof(struct iovec) * 6));
			req->iovec_count = 6;
			req->client_socket = client_socket;
			send_headers(final_path, path_stat.st_size, req->iov);
			copy_file_contents(final_path, path_stat.st_size,
			    &req->iov[5]);
			printf("200 %s %ld bytes\n", final_path,
			    path_stat.st_size);
			add_write_request(req);
		} else {
			handle_http_404_liburing(client_socket);
			printf("404 Not Found: %s\n", final_path);
		}
	}
}
