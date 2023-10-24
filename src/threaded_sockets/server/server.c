#include <sys/types.h>
#include <sys/mman.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/wait.h>

#include <netinet/in.h>

#include <arpa/inet.h>
#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <setjmp.h>
#include <signal.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>

#include "../helpers/io_helpers.h"
#include "server.h"

#define CONNECTION_REQUESTS 1024

static int
build_error(int file_descriptor, char *cause, char *error_number,
    char *short_message, char *long_message)
{
	char buf[MAXBUF], body[MAXBUF];

	(void)sprintf(body,
	    ""
	    "<!doctype html>\r\n"
	    "<head>\r\n"
	    "  <title>Error</title>\r\n"
	    "</head>\r\n"
	    "<body>\r\n"
	    "  <h2>%s: %s</h2>\r\n"
	    "  <p>%s: %s</p>\r\n"
	    "</body>\r\n"
	    "</html>\r\n",
	    error_number, short_message, long_message, cause);

	(void)sprintf(buf, "HTTP/1.0 %s %s\r\n", error_number, short_message);

	if (write(file_descriptor, buf, strlen(buf)) == -1) {
		return -1;
	}

	(void)sprintf(buf, "Content-Type: text/html\r\n");

	if (write(file_descriptor, buf, strlen(buf)) == -1) {
		return -1;
	}

	(void)sprintf(buf, "Content-Length: %zu\r\n\r\n", strlen(body));

	if (write(file_descriptor, buf, strlen(buf)) == -1) {
		return -1;
	}

	if (write(file_descriptor, body, strlen(body)) == -1) {
		return -1;
	}

	return 0;
}

static int
read_headers(int fd)
{
	char buf[MAXBUF];

	do {
		if (read_line(fd, buf, MAXBUF) == -1) {
			return -1;
		}
	} while (strcmp(buf, "\r\n") != 0);
	return 0;
}

static int
parse_uri(char *uri, char *file_name)
{
	if (!strstr(uri, "script")) {

		(void)sprintf(file_name, "%s", uri);

		return 1;
	} else {

		(void)sprintf(file_name, ".%s", uri);
		return 0;
	}
}

static int
server_dynamic_request(buffer_t *buffer)
{
	char buf[MAXBUF], *argv[] = { NULL };
	extern char **environ;

	(void)sprintf(buf,
	    ""
	    "HTTP/1.0 200 OK\r\n"
	    "Server: Xyderos\r\n");

	if (write(buffer->fd, buf, strlen(buf)) == -1) {
		return -1;
	}

	if (fork() == 0) {
		dup2(buffer->fd, STDOUT_FILENO);

		execve(buffer->path_name, argv, environ);
	} else {
		wait(NULL);
	}

	return 0;
}

static int
server_static_request(buffer_t *buf)
{
	int file;
	char *source_pointer, buffer[MAXBUF];

	file = open(buf->path_name, O_RDONLY, 0);

	if (file == -1)
		return -1;

	// hack it and instead of reading that uses a buffer by memory map it
	source_pointer = mmap(0, buf->size, PROT_READ, MAP_PRIVATE, file, 0);

	if (close(file) == -1) {
		return -1;
	}

	(void)sprintf(buffer,
	    ""
	    "HTTP/1.0 200 OK\r\n"
	    "Server: Xyderos \r\n"
	    "Content-Length: %zu\r\n"
	    "Content-Type: %s\r\n\r\n",
	    buf->size, "text/plain");

	if (write(buf->fd, buffer, strlen(buffer)) == -1)
		return -1;

	if (write(buf->fd, source_pointer, buf->size) == -1)
		return -1;
	if (munmap(source_pointer, buf->size) == -1)
		return -1;

	return 0;
}
void
server_handle_request(buffer_t *req)
{
	if (req->is_static) {
		server_static_request(req);
	} else {
		server_dynamic_request(req);
	}
}

int
server_build_request(int file_descriptor, buffer_t *request_buffer)
{
	int is_static;
	struct stat sbuf;
	char buf[MAXBUF], method[MAXBUF], uri[MAXBUF], version[MAXBUF];
	char file_name[MAXBUF];

	if (read_line(file_descriptor, buf, MAXBUF) == -1) {
		return Error;
	}

	(void)sscanf(buf, "%s %s %s", method, uri, version);

	if (strstr(uri, "..")) {
		build_error(file_descriptor, uri, "403", "Forbidden",
		    "Could not read file");
		return Forbidden;
	}

	if (read_headers(file_descriptor) == -1)
		return Error;

	is_static = parse_uri(uri, file_name);
	if (stat(file_name, &sbuf) < 0) {
		if (build_error(file_descriptor, file_name, "404", "Not found",
			"Could not find file") == -1) {
			return Error;
		}

		return NotFound;
	}

	if (is_static) {
		if (!(S_ISREG(sbuf.st_mode)) || !(S_IRUSR & sbuf.st_mode)) {
			if (build_error(file_descriptor, file_name, "403",
				"Forbidden", "Could not read file") == -1) {
				return Error;
			}
			return Forbidden;
		}
	} else {
		if (!(S_ISREG(sbuf.st_mode)) || !(S_IXUSR & sbuf.st_mode)) {
			if (build_error(file_descriptor, file_name, "403",
				"Forbidden", "Could not run CGI") == -1) {
				return Error;
			}
			return Forbidden;
		}
	}

	request_buffer->fd = file_descriptor;
	request_buffer->is_static = is_static;
	request_buffer->size = (size_t)sbuf.st_size;
	strcpy(request_buffer->path_name, file_name);

	return OK;
}

int
server_open_file_descriptor(int port)
{
	int listen_fd = 0, optval = 1;
	struct sockaddr_in server_addr;

	listen_fd = socket(AF_INET, SOCK_STREAM, 0);

	if (listen_fd < 0) {
		(void)fprintf(stderr, "SOCKET\n");
		return -1;
	}

	if (setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR,
		(const void *)&optval, sizeof(int)) < 0) {
		(void)fprintf(stderr, "SETSOCKOPT\n");
		return -1;
	}

	memset(&server_addr, 0, sizeof(server_addr));

	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	server_addr.sin_port = htons((unsigned short)port);

	if (bind(listen_fd, (sockaddr_t *)&server_addr, sizeof(server_addr)) <
	    0) {
		(void)fprintf(stderr, "BIND\n");
		return -1;
	}

	if (listen(listen_fd, CONNECTION_REQUESTS) < 0) {
		(void)fprintf(stderr, "LISTEN\n");
		return -1;
	}
	return listen_fd;
}
