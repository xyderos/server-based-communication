#pragma once

#include <sys/types.h>

#include "../helpers/io_helpers.h"

enum result { OK, NotFound, Forbidden, Error };

typedef struct buffer {
	int fd, is_static;
	unsigned long size;
	char path_name[MAXBUF];
} buffer_t;

int server_build_request(int, buffer_t *);
void server_handle_request(buffer_t *);
int server_open_file_descriptor(int);
