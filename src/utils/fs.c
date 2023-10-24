#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "fs.h"

void
copy_file_contents(char *file_path, off_t file_size, struct iovec *iov)
{
	int fd = 0;

	char *buf = malloc(file_size);
	fd = open(file_path, O_RDONLY);
	if (fd < 0) {
		exit(1);
	}

	read(fd, buf, file_size);

	close(fd);

	iov->iov_base = buf;
	iov->iov_len = file_size;
}

void
transfer_file_contents(char *file_path, int client_socket, off_t file_size)
{
	int fd = 0;

	fd = open(file_path, O_RDONLY);
	sendfile(client_socket, fd, NULL, file_size);
	close(fd);
}

const char *
get_filename_ext(const char *filename)
{
	const char *dot = strrchr(filename, '.');
	if (!dot || dot == filename) {
		return "";
	}
	return dot + 1;
}
