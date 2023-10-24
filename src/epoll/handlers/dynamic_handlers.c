#include <liburing.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "../../utils/constants.h"
#include "../../utils/fs.h"
#include "../network/network.h"
#include "dynamic_handlers.h"
#include "static_handlers.h"

static void
send_headers(const char *path, off_t len, int client_socket)
{
	char small_case_path[buffer_size];
	char send_buffer[buffer_size];
	const char *file_ext = get_filename_ext(small_case_path);
	strcpy(small_case_path, path);
	// strtolower(small_case_path);

	strcpy(send_buffer, "HTTP/1.0 200 OK\r\n");
	send(client_socket, send_buffer, strlen(send_buffer), 0);
	strcpy(send_buffer, server_string);
	send(client_socket, send_buffer, strlen(send_buffer), 0);

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
	send(client_socket, send_buffer, strlen(send_buffer), 0);

	(void)sprintf(send_buffer, "content-length: %ld\r\n", len);
	send(client_socket, send_buffer, strlen(send_buffer), 0);

	strcpy(send_buffer, "\r\n");
	send(client_socket, send_buffer, strlen(send_buffer), 0);
}

void
handle_get_method_epoll(char *path, int client_socket)
{
	char final_path[buffer_size];
	struct stat path_stat;

	strcpy(final_path, ".");
	strcat(final_path, path);

	if (stat(final_path, &path_stat) == -1) {
		printf("404 Not Found: %s\n", final_path);
		handle_http_404_epoll(client_socket);
	} else {
		if (S_ISREG(path_stat.st_mode)) {
			send_headers(final_path, path_stat.st_size,
			    client_socket);
			transfer_file_contents(final_path, client_socket,
			    path_stat.st_size);
			printf("200 %s %ld bytes\n", final_path,
			    path_stat.st_size);
		} else {
			handle_http_404_epoll(client_socket);
			printf("404 Not Found: %s\n", final_path);
		}
	}
}
