#include <sys/socket.h>

#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "../../utils/constants.h"
#include "../network/network.h"
#include "client_handler.h"
#include "dynamic_handlers.h"
#include "static_handlers.h"

static int
get_line(int sock, char *buf, int size)
{
	int i = 0;
	char c = '\0';
	ssize_t n = 0;

	while ((i < size - 1) && (c != '\n')) {
		n = recv(sock, &c, 1, 0);
		if (n > 0) {
			if (c == '\r') {
				n = recv(sock, &c, 1, MSG_PEEK);
				if ((n > 0) && (c == '\n')) {
					recv(sock, &c, 1, 0);
				}
			} else {
				buf[i] = c;
				i++;
			}
		} else {
			return 0;
		}
	}
	buf[i] = '\0';

	return (i);
}

static void
handle_http_method(char *method_buffer, int client_sock)
{
	char *method = NULL, *path = NULL, *saveptr = NULL;

	method = strtok_r(method_buffer, " ", &saveptr);
	path = strtok_r(NULL, " ", &saveptr);

	if (strcmp(method, "GET") == 0) {
		handle_get_method_epoll(path, client_sock);
	} else {
		handle_unimplemented_method_epoll(client_sock);
	}
}

void
handle_client(int client_sock)
{
	char line_buffer[buffer_size];
	char method_buffer[buffer_size];
	int method_line = 0;
	unsigned long len = 0;

	while (1) {
		get_line(client_sock, line_buffer, (int)sizeof(line_buffer));
		method_line++;

		len = strlen(line_buffer);

		if (method_line == 1) {
			if (len == 0) {
				return;
			}

			strcpy(method_buffer, line_buffer);
		} else {
			if (len == 0) {
				break;
			}
		}
	}

	handle_http_method(method_buffer, client_sock);
}
