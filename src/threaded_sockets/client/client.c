#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "../helpers/io_helpers.h"
#include "client.h"

int
client_open_file_descriptor(char *host_name, int port)
{
	int client_fd = socket(AF_INET, SOCK_STREAM, 0);
	struct hostent *hp = gethostbyname(host_name);
	struct sockaddr_in adr;

	client_fd = socket(AF_INET, SOCK_STREAM, 0);

	if (client_fd < 0 || !hp) {
		return -1;
	}

	memset(&adr, 0, sizeof(adr));

	adr.sin_family = AF_INET;

	memcpy(&adr.sin_addr.s_addr, hp->h_addr, (unsigned long)hp->h_length);

	adr.sin_port = htons((unsigned short)port);

	if (connect(client_fd, (sockaddr_t *)&adr, sizeof(adr)) < 0) {
		return -1;
	}
	return client_fd;
}

void
client_write_to_file_descriptor(int file_descriptor, char *file_name)
{
	char buffer[MAXBUF], host_name[MAXBUF / 10];

	gethostname(host_name, MAXBUF);

	(void)sprintf(buffer, "GET %s HTTP/1.1\nhost: %s\n\r\n", file_name,
	    host_name);
	write(file_descriptor, buffer, strlen(buffer));
}

void
client_read_from_file_descriptor(int file_descriptor)
{
	char buf[MAXBUF];
	ssize_t n;

	n = read_line(file_descriptor, buf, MAXBUF);

	while (strcmp(buf, "\r\n") != 0 && (n > 0)) {
		printf("Header: %s", buf);
		n = read_line(file_descriptor, buf, MAXBUF);
	}

	n = read_line(file_descriptor, buf, MAXBUF);
	while (n > 0) {
		printf("%s", buf);
		n = read_line(file_descriptor, buf, MAXBUF);
	}
}
